
#include "vtkMISTVolumeRayCastMapper.h"

#include "vtkCamera.h"
#include "vtkEncodedGradientEstimator.h"
#include "vtkEncodedGradientShader.h"
#include "vtkFiniteDifferenceGradientEstimator.h"
#include "vtkGarbageCollector.h"
#include "vtkGraphicsFactory.h"
#include "vtkImageData.h"
#include "vtkMath.h"
#include "vtkMultiThreader.h"
#include "vtkObjectFactory.h"
#include "vtkPlaneCollection.h"
#include "vtkPointData.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTimerLog.h"
#include "vtkTransform.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastFunction.h"
#include "vtkRayCastImageDisplayHelper.h"

#include "vtkPiecewiseFunction.h"
#include "vtkVolumeProperty.h" 
#include "vtkColorTransferFunction.h"

#include <math.h>

#include "mist/mist.h"
#include "mist/volumerender.h"
#include "mist/draw.h"



//vtkCxxRevisionMacro(vtkMISTVolumeRayCastMapper, "$Revision: 0.0.0.1 $");
vtkStandardNewMacro(vtkMISTVolumeRayCastMapper);

#define vtkVRCMultiplyPointMacro( A, B, M ) \
  B[0] = A[0]*M[0]  + A[1]*M[1]  + A[2]*M[2]  + M[3]; \
  B[1] = A[0]*M[4]  + A[1]*M[5]  + A[2]*M[6]  + M[7]; \
  B[2] = A[0]*M[8]  + A[1]*M[9]  + A[2]*M[10] + M[11]; \
  B[3] = A[0]*M[12] + A[1]*M[13] + A[2]*M[14] + M[15]; \
  if ( B[3] != 1.0 ) { B[0] /= B[3]; B[1] /= B[3]; B[2] /= B[3]; }

#define vtkVRCMultiplyViewPointMacro( A, B, M ) \
  B[0] = A[0]*M[0]  + A[1]*M[1]  + A[2]*M[2]  + M[3]; \
  B[1] = A[0]*M[4]  + A[1]*M[5]  + A[2]*M[6]  + M[7]; \
  B[3] = A[0]*M[12] + A[1]*M[13] + A[2]*M[14] + M[15]; \
  if ( B[3] != 1.0 ) { B[0] /= B[3]; B[1] /= B[3]; }

#define vtkVRCMultiplyNormalMacro( A, B, M ) \
  B[0] = A[0]*M[0]  + A[1]*M[4]  + A[2]*M[8]; \
  B[1] = A[0]*M[1]  + A[1]*M[5]  + A[2]*M[9]; \
  B[2] = A[0]*M[2]  + A[1]*M[6]  + A[2]*M[10]

typedef mist::vector2< double > point2;
typedef mist::vector3< double > point3;
typedef point3 vector_type;

typedef mist::volumerender::attribute_table< mist::rgb< double > > volr_table_type;
typedef mist::volumerender::parameter volr_parameter_type;

struct volr_camera
{
    point3 pos;
    point3 dir;
    point3 up;
};

// Construct a new vtkVolumeRayCastMapper with default values
vtkMISTVolumeRayCastMapper::vtkMISTVolumeRayCastMapper()
{
  this->SampleDistance             =  1.0;
  this->ImageSampleDistance        =  1.0;
  this->MinimumImageSampleDistance =  1.0;
  this->MaximumImageSampleDistance = 10.0;
  
  this->PerspectiveMatrix      = vtkMatrix4x4::New();
  this->ViewToWorldMatrix      = vtkMatrix4x4::New();
  this->ViewToVoxelsMatrix     = vtkMatrix4x4::New();
  this->VoxelsToViewMatrix     = vtkMatrix4x4::New();
  this->WorldToVoxelsMatrix    = vtkMatrix4x4::New();
  this->VoxelsToWorldMatrix    = vtkMatrix4x4::New();

  this->VolumeMatrix           = vtkMatrix4x4::New();
  
  this->PerspectiveTransform   = vtkTransform::New();
  this->VoxelsTransform        = vtkTransform::New();
  this->VoxelsToViewTransform  = vtkTransform::New();
  
  
  this->ImageMemorySize[0]     = 0;
  this->ImageMemorySize[1]     = 0;

  this->Threader               = vtkMultiThreader::New();

  this->Image                  = NULL;
  this->RowBounds              = NULL;
  this->OldRowBounds           = NULL;

  this->RenderTimeTable        = NULL;
  this->RenderVolumeTable      = NULL;
  this->RenderRendererTable    = NULL;
  this->RenderTableSize        = 0;  
  this->RenderTableEntries     = 0;

  this->ZBuffer                = NULL;
  this->ZBufferSize[0]         = 0;
  this->ZBufferSize[1]         = 0;
  this->ZBufferOrigin[0]       = 0;
  this->ZBufferOrigin[1]       = 0;
  
  this->ImageDisplayHelper     = vtkRayCastImageDisplayHelper::New();
  
  this->IntermixIntersectingGeometry = 1;

    //this->mistArray.clear();
}

// Destruct a vtkVolumeRayCastMapper - clean up any memory used
vtkMISTVolumeRayCastMapper::~vtkMISTVolumeRayCastMapper()
{

  this->PerspectiveMatrix->Delete();
  this->ViewToWorldMatrix->Delete();
  this->ViewToVoxelsMatrix->Delete();
  this->VoxelsToViewMatrix->Delete();
  this->WorldToVoxelsMatrix->Delete();
  this->VoxelsToWorldMatrix->Delete();

  this->VolumeMatrix->Delete();
  
  this->VoxelsTransform->Delete();
  this->VoxelsToViewTransform->Delete();
  this->PerspectiveTransform->Delete();
  
  this->ImageDisplayHelper->Delete();

  this->Threader->Delete();
  
  if ( this->Image )
    {
    delete [] this->Image;
    }
  
  if ( this->RenderTableSize )
    {
    delete [] this->RenderTimeTable;
    delete [] this->RenderVolumeTable;
    delete [] this->RenderRendererTable;
    }
  
  if ( this->RowBounds )
    {
    delete [] this->RowBounds;
    delete [] this->OldRowBounds;
    }

    //this->mistArray.clear();
}


void vtkMISTVolumeRayCastMapper::Render( vtkRenderer *ren, vtkVolume *vol )
{
    // make sure that we have scalar input and update the scalar input
    if ( this->GetInput() == NULL ) 
    {
        vtkErrorMacro(<< "No Input!");
        return;
    }
    else
    {
        this->GetInput()->UpdateInformation();
        this->GetInput()->SetUpdateExtentToWholeExtent();
        this->GetInput()->Update();
    } 

    //this->GetInput()->SetScalarTypeToUnsignedShort();
    int scalarType = this->GetInput()->GetPointData()->GetScalars()->GetDataType();
    
    if ( scalarType != VTK_UNSIGNED_SHORT && scalarType != VTK_SHORT )
    {
        vtkErrorMacro ("Cannot volume render data of type " 
                                        << vtkImageScalarTypeNameMacro(scalarType) 
                                        << ", only unsigned short.");
        return;
    }

    //this->UpdateShadingTables( ren, vol );

    vtkImageData *input;
    int dim[3];
    double spe[3];
    double ori[3];

    // This is the input of this mapper
    input = this->GetInput();

    // Convert to mist array
    input->GetDimensions( dim );
    input->GetSpacing( spe );
    input->GetOrigin( ori );

    //if( mistArray.empty() )
    //{
        //printf("%d %d %d\n", dim[0], dim[1], dim[2]);
        //printf("%f %f %f\n", spe[0], spe[1], spe[2]);
        //printf("%f %f %f\n", ori[0], ori[1], ori[2]);
        
        //vtkDebugMacro( dim[0] << dim[1]<< dim[2] << std::endl );

        // Convert VTKImage to MIST array
        mist::array3< unsigned short > mistArray( dim[0], dim[1], dim[2], spe[0], spe[1], spe[2], (unsigned short *) input->GetScalarPointer(), dim[0]*dim[1]*dim[2]*2 );
        //mistArray = mist::array3< unsigned short >( dim[0], dim[1], dim[2], spe[0], spe[1], spe[2], (unsigned short *) input->GetScalarPointer(), dim[0]*dim[1]*dim[2]*2 );
    //}

    // Get the camera from the renderer
    vtkCamera *cam = ren->GetActiveCamera();

    // Convert to mist camera
    //printf("Convert Mist camera\n");
    double pos[3], dir[3], up[3], foc[3];
    
    cam->GetPosition( pos );
    cam->GetDirectionOfProjection( dir );
    cam->GetViewUp( up );
    cam->GetFocalPoint( foc );

    volr_camera mistCamera;

    mistCamera.pos.x = pos[0] - ori[0];
    mistCamera.pos.y = pos[1] - ori[1];
    mistCamera.pos.z = pos[2] - ori[2];
    mistCamera.dir.x = dir[0];
    mistCamera.dir.y = dir[1];
    mistCamera.dir.z = dir[2];
    mistCamera.up.x = up[0];
    mistCamera.up.y = up[1];
    mistCamera.up.z = up[2];

    //printf("%f %f %f %f %f %f %f %f %f\n", mistCamera.pos.x, mistCamera.pos.y, mistCamera.pos.z, mistCamera.dir.x, mistCamera.dir.y, mistCamera.dir.z, mistCamera.up.x, mistCamera.up.y, mistCamera.up.z);

    // Set color and opacity function
    volr_table_type volrTable;
    double max=4000, min=-2000;

    vtkColorTransferFunction *colorTable = vol->GetProperty()->GetRGBTransferFunction();
    vtkPiecewiseFunction *opacityTable = vol->GetProperty()->GetScalarOpacity();
    
    //colorTable->GetRange( min, max );
    //printf("%f %f\n",min,max);


    volrTable.create( static_cast< int >( min ), static_cast< int >( max ) );
    for( int i = static_cast< int >( min ) ; i <= static_cast< int >( max ) ; i ++ )
    {
        //printf("%d %f %f %f %f\n", i, colorTable->GetRedValue( i ), colorTable->GetGreenValue( i ), colorTable->GetBlueValue( i ), opacityTable->GetValue( i ));
        volrTable.at( i ).pixel = mist::rgb< double >( colorTable->GetRedValue( i ) * 255.0, colorTable->GetGreenValue( i ) * 255.0, colorTable->GetBlueValue( i ) * 255.0);
        volrTable.at( i ).alpha = opacityTable->GetValue( i );
        volrTable.at( i ).has_alpha = volrTable.at( i ).alpha > 0.0;
    }

    // Create depth map
    //printf("Create Depth Map\n");
    mist::array3< double > depthMap;
    //mist::generate_depth_map( mistArray, depthMap, volrTable );

    // Set some parameter for mist VolR
    volr_parameter_type volrParameter;
    bool mirrorView = false;
    
    double ax = mistArray.reso1( );
    double ay = mistArray.reso2( );
    double az = mistArray.reso3( );

    int sx = 0;
    int ex = mistArray.width( ) - 1;
    int sy = 0;
    int ey = mistArray.height( ) - 1;
    int sz = 0;
    int ez = mistArray.depth( ) - 1;

    volrParameter.offset.x = ( sx + ex ) / 2.0 * ax;
    volrParameter.offset.y = ( sy + ey ) / 2.0 * ay;
    volrParameter.offset.z = ( sz + ez ) / 2.0 * az;

    typedef mist::volumerender::boundingbox boundingbox;
    volrParameter.box[ 0 ] = boundingbox(  0,  0,  1, volrParameter.offset.z - az * 2.0 );
    volrParameter.box[ 1 ] = boundingbox(  1,  0,  0, volrParameter.offset.x - ax * 2.0 );
    volrParameter.box[ 2 ] = boundingbox(  0,  0, -1, volrParameter.offset.z - az * 2.0 );
    volrParameter.box[ 3 ] = boundingbox( -1,  0,  0, volrParameter.offset.x - ax * 2.0 );
    volrParameter.box[ 4 ] = boundingbox(  0, -1,  0, volrParameter.offset.y - ay * 2.0 );
    volrParameter.box[ 5 ] = boundingbox(  0,  1,  0, volrParameter.offset.y - ay * 2.0 );

    if( cam->GetParallelProjection() )
    {
        volrParameter.perspective_view = false;
        //printf("parallel\n");
    }
    else
    {
        volrParameter.perspective_view = true;
        //printf("perspective\n");
    }

    mistCamera.pos.x += volrParameter.offset.x;
    mistCamera.pos.y += volrParameter.offset.y;
    mistCamera.pos.z += volrParameter.offset.z;

    volrParameter.mirror_view           = mirrorView;
    volrParameter.value_interpolation   = true;
    //volrParameter.fovy                    = 80.0;
    volrParameter.fovy                  = cam->GetViewAngle()*2.0;
    //printf("view angle %f\n", cam->GetViewAngle() );
    
    volrParameter.ambient_ratio     = vol->GetProperty()->GetAmbient(); //0.3;
    volrParameter.diffuse_ratio     = vol->GetProperty()->GetDiffuse(); //0.7;
    volrParameter.light_attenuation = 0.0;
    volrParameter.sampling_step     = this->SampleDistance;
    volrParameter.termination           = 0.05;

    volrParameter.pos = mistCamera.pos;
    volrParameter.dir = mistCamera.dir;
    volrParameter.up = mistCamera.up;

    if( depth_map.empty() )
    {
        mist::generate_depth_map( mistArray, depth_map, volrTable );
    }
  // Get the aspect ratio from the renderer. This is needed for the
  // computation of the perspective matrix
  ren->ComputeAspect();
  double *aspect = ren->GetAspect();

  // Keep track of the projection matrix - we'll need it in a couple of
  // places Get the projection matrix. The method is called perspective, but
  // the matrix is valid for perspective and parallel viewing transforms.
  // Don't replace this with the GetCompositePerspectiveTransformMatrix
  // because that turns off stereo rendering!!!
  this->PerspectiveTransform->Identity();
  this->PerspectiveTransform->Concatenate(
    cam->GetPerspectiveTransformMatrix(aspect[0]/aspect[1],0.0, 1.0 ));
  this->PerspectiveTransform->Concatenate(cam->GetViewTransformMatrix());
  this->PerspectiveMatrix->DeepCopy(this->PerspectiveTransform->GetMatrix());

  // Compute some matrices from voxels to view and vice versa based 
  // on the whole input
  this->ComputeMatrices( input, vol );

  // How big is the viewport in pixels?
  double *viewport   =  ren->GetViewport();
  int *renWinSize   =  ren->GetRenderWindow()->GetSize();
    
    //this->ImageSampleDistance = 4.0;
  // The full image fills the viewport. First, compute the actual viewport
  // size, then divide by the ImageSampleDistance to find the full image
  // size in pixels
  int width, height;
  ren->GetTiledSize(&width, &height);
  this->ImageViewportSize[0] =
    static_cast<int>(width/this->ImageSampleDistance);
  this->ImageViewportSize[1] =
    static_cast<int>(height/this->ImageSampleDistance);
  
  // Compute row bounds. This will also compute the size of the image to
  // render, allocate the space if necessary, and clear the image where
  // required
  if ( this->ComputeRowBounds( vol, ren ) )
    {

    // Rendering image
    //printf("Rendering image\n");

    //printf("ImageMemorySize %d %d \n",  this->ImageMemorySize[0],  this->ImageMemorySize[1] );
    //printf("ImageViewportSize %d %d \n",  this->ImageViewportSize[0],  this->ImageViewportSize[1] );
    //printf("ImageInUseSize %d %d \n",  this->ImageInUseSize[0],  this->ImageInUseSize[1] );
    //printf("ImageOrigin %d %d \n",  this->ImageOrigin[0],  this->ImageOrigin[1] );

    mist::array2< mist::rgb< unsigned char > > image;
    //image.resize(ImageViewportSize[0],ImageViewportSize[1]);
    image.resize(ImageMemorySize[0],ImageMemorySize[1] );
    //image.resize(128,128 );
    //printf("%d %d\n",ImageViewportSize[0],ImageViewportSize[1]);
    image.reso(aspect[0]/aspect[1], 1.0 );
    //image.reso( ImageViewportSize[0]/ImageViewportSize[1], 1.0);
    //mist::volumerendering( mistArray, image, mist::volumerender::no_depth_map( ), volrParameter, volrTable );
    mist::volumerendering( mistArray, image,  mist::volumerender::depth_map< mist::array3< double > >( depth_map ), volrParameter, volrTable );

    if ( !ren->GetRenderWindow()->GetAbortRender() )
    {
            //mist::draw_image( image, ImageViewportSize[0], ImageViewportSize[1] );
        //mist::draw_image( image, ImageViewportSize[0], ImageViewportSize[1], 2.0, ImageOrigin[0], ImageOrigin[1] );
        mist::draw_image( image, ImageViewportSize[0] * this->ImageSampleDistance, ImageViewportSize[1] * this->ImageSampleDistance, this->ImageSampleDistance, 0, 0, 1.0, 1.0, 1.0, true, true );
    }

    //if ( !ren->GetRenderWindow()->GetAbortRender() )
    //  {
    //      float depth = -1;
    //      
    //      printf("ImageMemorySize %d %d \n",  this->ImageMemorySize[0],  this->ImageMemorySize[1] );
    //      printf("ImageViewportSize %d %d \n",  this->ImageViewportSize[0],  this->ImageViewportSize[1] );
    //      printf("ImageInUseSize %d %d \n",  this->ImageInUseSize[0],  this->ImageInUseSize[1] );
    //      printf("ImageOrigin %d %d \n",  this->ImageOrigin[0],  this->ImageOrigin[1] );
    //      this->ImageDisplayHelper->
    //          RenderTexture( vol, ren,
    //                                       this->ImageMemorySize,
    //                                       this->ImageViewportSize,
    //                                       this->ImageInUseSize,
    //                                       this->ImageOrigin,
    //                                       depth,
    //                                       this->Image );
    //   

    //  }
    }

    

}

int vtkMISTVolumeRayCastMapper::ComputeRowBounds(vtkVolume   *vol,
                                             vtkRenderer *ren)
{
  float voxelPoint[3];
  float viewPoint[8][4];
  int i, j, k;
  unsigned char *ucptr;
  float minX, minY, maxX, maxY, minZ, maxZ;

  minX =  1.0;
  minY =  1.0;
  maxX = -1.0;
  maxY = -1.0;
  minZ =  1.0;
  maxZ =  0.0;
  
  float bounds[6];
  int dim[3];
  
  this->GetInput()->GetDimensions(dim);
  bounds[0] = bounds[2] = bounds[4] = 0.0;
  // The rounding tie-breaker is used to protect against a very small rounding error
  // introduced in the QuickFloor function, which is used by the vtkFloorFuncMacro macro.
  bounds[1] = static_cast<float>(dim[0]-1) - vtkFastNumericConversion::RoundingTieBreaker();
  bounds[3] = static_cast<float>(dim[1]-1) - vtkFastNumericConversion::RoundingTieBreaker();
  bounds[5] = static_cast<float>(dim[2]-1) - vtkFastNumericConversion::RoundingTieBreaker();
  
  double camPos[3];
  double worldBounds[6];
  vol->GetBounds( worldBounds );
  int insideFlag = 0;
  ren->GetActiveCamera()->GetPosition( camPos );
  if ( camPos[0] >= worldBounds[0] &&
       camPos[0] <= worldBounds[1] &&
       camPos[1] >= worldBounds[2] &&
       camPos[1] <= worldBounds[3] &&
       camPos[2] >= worldBounds[4] &&
       camPos[2] <= worldBounds[5] )
    {
    insideFlag = 1;
    }
  
  
  // If we have a simple crop box then we can tighten the bounds
  // See prior explanation of RoundingTieBreaker
  if ( this->Cropping && this->CroppingRegionFlags == 0x2000 )
    {
    bounds[0] = this->VoxelCroppingRegionPlanes[0];
    bounds[1] = this->VoxelCroppingRegionPlanes[1] - vtkFastNumericConversion::RoundingTieBreaker();
    bounds[2] = this->VoxelCroppingRegionPlanes[2];
    bounds[3] = this->VoxelCroppingRegionPlanes[3] - vtkFastNumericConversion::RoundingTieBreaker();
    bounds[4] = this->VoxelCroppingRegionPlanes[4];
    bounds[5] = this->VoxelCroppingRegionPlanes[5] - vtkFastNumericConversion::RoundingTieBreaker();
    }
  
  
  // Copy the voxelsToView matrix to 16 floats
  float voxelsToViewMatrix[16];
  for ( j = 0; j < 4; j++ )
    {
    for ( i = 0; i < 4; i++ )
      {
      voxelsToViewMatrix[j*4+i] = 
        static_cast<float>(this->VoxelsToViewMatrix->GetElement(j,i));
      }
    }
  
  // Convert the voxel bounds to view coordinates to find out the
  // size and location of the image we need to generate. 
  int idx = 0;
  if ( insideFlag )
    {
    minX = -1.0;
    maxX =  1.0;
    minY = -1.0;
    maxY =  1.0;
    minZ =  0.001;
    maxZ =  0.001;
    }
  else
    {
    for ( k = 0; k < 2; k++ )
      {
      voxelPoint[2] = bounds[4+k];
      for ( j = 0; j < 2; j++ )
        {
        voxelPoint[1] = bounds[2+j];
        for ( i = 0; i < 2; i++ )
          {
          voxelPoint[0] = bounds[i];
          vtkVRCMultiplyPointMacro( voxelPoint, viewPoint[idx],
                                   voxelsToViewMatrix );
          
          minX = (viewPoint[idx][0]<minX)?(viewPoint[idx][0]):(minX);
          minY = (viewPoint[idx][1]<minY)?(viewPoint[idx][1]):(minY);        
          maxX = (viewPoint[idx][0]>maxX)?(viewPoint[idx][0]):(maxX);
          maxY = (viewPoint[idx][1]>maxY)?(viewPoint[idx][1]):(maxY);
          minZ = (viewPoint[idx][2]<minZ)?(viewPoint[idx][2]):(minZ);
          maxZ = (viewPoint[idx][2]>maxZ)?(viewPoint[idx][2]):(maxZ);
          idx++;
          }
        }
      }
    }
  
  if ( minZ < 0.001 || maxZ > 0.9999 )
    {
    minX = -1.0;
    maxX =  1.0;
    minY = -1.0;
    maxY =  1.0;
    insideFlag = 1;
    }
  
  this->MinimumViewDistance = 
    (minZ<0.001)?(0.001):((minZ>0.999)?(0.999):(minZ));
  
  // We have min/max values from -1.0 to 1.0 now - we want to convert 
  // these to pixel locations. Give a couple of pixels of breathing room
  // on each side if possible
  minX = ( minX + 1.0 ) * 0.5 * this->ImageViewportSize[0] - 2; 
  minY = ( minY + 1.0 ) * 0.5 * this->ImageViewportSize[1] - 2; 
  maxX = ( maxX + 1.0 ) * 0.5 * this->ImageViewportSize[0] + 2; 
  maxY = ( maxY + 1.0 ) * 0.5 * this->ImageViewportSize[1] + 2;

  // If we are outside the view frustum return 0 - there is no need
  // to render anything
  if ( ( minX < 0 && maxX < 0 ) ||
       ( minY < 0 && maxY < 0 ) ||
       ( minX > this->ImageViewportSize[0]-1 &&
         maxX > this->ImageViewportSize[0]-1 ) ||
       ( minY > this->ImageViewportSize[1]-1 &&
         maxY > this->ImageViewportSize[1]-1 ) )
    {
    return 0;
    }

  int oldImageMemorySize[2];
  oldImageMemorySize[0] = this->ImageMemorySize[0];
  oldImageMemorySize[1] = this->ImageMemorySize[1];
  
  // Swap the row bounds
  int *tmpptr;
  tmpptr = this->RowBounds;
  this->RowBounds = this->OldRowBounds;
  this->OldRowBounds = tmpptr;
  

  // Check the bounds - the volume might project outside of the 
  // viewing box / frustum so clip it if necessary
  minX = (minX<0)?(0):(minX);
  minY = (minY<0)?(0):(minY);
  maxX = (maxX>this->ImageViewportSize[0]-1)?
    (this->ImageViewportSize[0]-1):(maxX);
  maxY = (maxY>this->ImageViewportSize[1]-1)?
    (this->ImageViewportSize[1]-1):(maxY);

  // Create the new image, and set its size and position
  this->ImageInUseSize[0] = static_cast<int>(maxX - minX + 1.0);
  this->ImageInUseSize[1] = static_cast<int>(maxY - minY + 1.0);

  // What is a power of 2 size big enough to fit this image?
  this->ImageMemorySize[0] = 32;
  this->ImageMemorySize[1] = 32;
  while ( this->ImageMemorySize[0] < this->ImageInUseSize[0] )
    {
    this->ImageMemorySize[0] *= 2;
    }
  while ( this->ImageMemorySize[1] < this->ImageInUseSize[1] )
    {
    this->ImageMemorySize[1] *= 2;
    }
  
  this->ImageOrigin[0] = static_cast<int>(minX);
  this->ImageOrigin[1] = static_cast<int>(minY);

  // If the old image size is much too big (more than twice in
  // either direction) then set the old width to 0 which will
  // cause the image to be recreated
  if ( oldImageMemorySize[0] > 2*this->ImageMemorySize[0] ||
       oldImageMemorySize[1] > 2*this->ImageMemorySize[1] )
    {
    oldImageMemorySize[0] = 0;
    }
  
  // If the old image is big enough (but not too big - we handled
  // that above) then we'll bump up our required size to the
  // previous one. This will keep us from thrashing.
  if ( oldImageMemorySize[0] >= this->ImageMemorySize[0] &&
       oldImageMemorySize[1] >= this->ImageMemorySize[1] )
    {
    this->ImageMemorySize[0] = oldImageMemorySize[0];
    this->ImageMemorySize[1] = oldImageMemorySize[1];
    }
  
  // Do we already have a texture big enough? If not, create a new one and
  // clear it.
  if ( !this->Image ||
       this->ImageMemorySize[0] > oldImageMemorySize[0] ||
       this->ImageMemorySize[1] > oldImageMemorySize[1] )
    {
    // If there is an image there must be row bounds
    if ( this->Image )
      {
      delete [] this->Image;
      delete [] this->RowBounds;
      delete [] this->OldRowBounds;
      }
    
    this->Image = new unsigned char[(this->ImageMemorySize[0] *
                                     this->ImageMemorySize[1] * 4)];
    
    // Create the row bounds array. This will store the start / stop pixel
    // for each row. This helps eleminate work in areas outside the bounding
    // hexahedron since a bounding box is not very tight. We keep the old ones
    // too to help with only clearing where required
    this->RowBounds = new int [2*this->ImageMemorySize[1]];
    this->OldRowBounds = new int [2*this->ImageMemorySize[1]];

    for ( i = 0; i < this->ImageMemorySize[1]; i++ )
      {
      this->RowBounds[i*2]      = this->ImageMemorySize[0];
      this->RowBounds[i*2+1]    = -1;
      this->OldRowBounds[i*2]   = this->ImageMemorySize[0];
      this->OldRowBounds[i*2+1] = -1;
      }
    
    ucptr = this->Image;
    
    for ( i = 0; i < this->ImageMemorySize[0]*this->ImageMemorySize[1]; i++ )
      {
      *(ucptr++) = 0;
      *(ucptr++) = 0;
      *(ucptr++) = 0;
      *(ucptr++) = 0;      
      }
    }

  // If we are inside the volume our row bounds indicate every ray must be
  // cast - we don't need to intersect with the 12 lines
  if ( insideFlag )
    {
    for ( j = 0; j < this->ImageInUseSize[1]; j++ )
      {
      this->RowBounds[j*2] = 0;
      this->RowBounds[j*2+1] = this->ImageInUseSize[0] - 1;
      }
    }
  else
    {
    // create an array of lines where the y value of the first vertex is less
    // than or equal to the y value of the second vertex. There are 12 lines,
    // each containing x1, y1, x2, y2 values.
    float lines[12][4];
    float x1, y1, x2, y2;
    int xlow, xhigh;
    int lineIndex[12][2] = {{0,1}, {2,3}, {4,5}, {6,7}, 
                            {0,2}, {1,3} ,{4,6}, {5,7},
                            {0,4}, {1,5}, {2,6}, {3,7}};
  
    for ( i = 0; i < 12; i++ )
      {
      x1 = (viewPoint[lineIndex[i][0]][0]+1.0) * 
        0.5*this->ImageViewportSize[0] - this->ImageOrigin[0];
      
      y1 = (viewPoint[lineIndex[i][0]][1]+1.0) * 
        0.5*this->ImageViewportSize[1] - this->ImageOrigin[1];
      
      x2 = (viewPoint[lineIndex[i][1]][0]+1.0) * 
        0.5*this->ImageViewportSize[0] - this->ImageOrigin[0];
      
      y2 = (viewPoint[lineIndex[i][1]][1]+1.0) * 
        0.5*this->ImageViewportSize[1] - this->ImageOrigin[1];
      
      if ( y1 < y2 )
        {
        lines[i][0] = x1;
        lines[i][1] = y1;
        lines[i][2] = x2;
        lines[i][3] = y2;
        }
      else
        {
        lines[i][0] = x2;
        lines[i][1] = y2;
        lines[i][2] = x1;
        lines[i][3] = y1;
        }
      }

    // Now for each row in the image, find out the start / stop pixel
    // If min > max, then no intersection occurred
    for ( j = 0; j < this->ImageInUseSize[1]; j++ )
      {
      this->RowBounds[j*2] = this->ImageMemorySize[0];
      this->RowBounds[j*2+1] = -1;
      for ( i = 0; i < 12; i++ )
        {
        if ( j >= lines[i][1] && j <= lines[i][3] &&
             ( lines[i][1] != lines[i][3] ) )
          {
          x1 = lines[i][0] +
            (static_cast<float>(j) - lines[i][1])/(lines[i][3] - lines[i][1]) *
            (lines[i][2] - lines[i][0] );

          xlow  = static_cast<int>(x1 + 1.5);
          xhigh = static_cast<int>(x1 - 1.0);
          
          xlow = (xlow<0)?(0):(xlow);
          xlow = (xlow>this->ImageInUseSize[0]-1)?
            (this->ImageInUseSize[0]-1):(xlow);

          xhigh = (xhigh<0)?(0):(xhigh);
          xhigh = (xhigh>this->ImageInUseSize[0]-1)?(
            this->ImageInUseSize[0]-1):(xhigh);

          if ( xlow < this->RowBounds[j*2] )
            {
            this->RowBounds[j*2] = xlow;
            }
          if ( xhigh > this->RowBounds[j*2+1] )
            {
            this->RowBounds[j*2+1] = xhigh;
            }
          }
        }
      // If they are the same this is either a point on the cube or
      // all lines were out of bounds (all on one side or the other)
      // It is safe to ignore the point (since the ray isn't likely
      // to travel through it enough to actually take a sample) and it
      // must be ignored in the case where all lines are out of range
      if ( this->RowBounds[j*2] == this->RowBounds[j*2+1] )
        {
        this->RowBounds[j*2] = this->ImageMemorySize[0];
        this->RowBounds[j*2+1] = -1;
        }
      }
    }
  
  for ( j = this->ImageInUseSize[1]; j < this->ImageMemorySize[1]; j++ )
    {
    this->RowBounds[j*2] = this->ImageMemorySize[0];
    this->RowBounds[j*2+1] = -1;
    }

  for ( j = 0; j < this->ImageMemorySize[1]; j++ )
    {
    // New bounds are not overlapping with old bounds - clear between
    // old bounds only
    if ( this->RowBounds[j*2+1] < this->OldRowBounds[j*2] ||
         this->RowBounds[j*2]   > this->OldRowBounds[j*2+1] )
      {
      ucptr = this->Image + 4*( j*this->ImageMemorySize[0] + 
                                this->OldRowBounds[j*2] );
      for ( i = 0; 
            i <= (this->OldRowBounds[j*2+1] - this->OldRowBounds[j*2]);
            i++ )
        {
        *(ucptr++) = 0;
        *(ucptr++) = 0;
        *(ucptr++) = 0;
        *(ucptr++) = 0;
        }
      }
    // New bounds do overlap with old bounds
    else
      {
      // Clear from old min to new min
      ucptr = this->Image + 4*( j*this->ImageMemorySize[0] + 
                                this->OldRowBounds[j*2] );
      for ( i = 0; 
            i < (this->RowBounds[j*2] - this->OldRowBounds[j*2]);
            i++ )
        {
        *(ucptr++) = 0;
        *(ucptr++) = 0;
        *(ucptr++) = 0;
        *(ucptr++) = 0;
        }
      
      // Clear from new max to old max
      ucptr = this->Image + 4*( j*this->ImageMemorySize[0] + 
                                this->RowBounds[j*2+1]+1 );
      for ( i = 0; 
            i < (this->OldRowBounds[j*2+1] - this->RowBounds[j*2+1]);
            i++ )
        {
        *(ucptr++) = 0;        
        *(ucptr++) = 0;        
        *(ucptr++) = 0;        
        *(ucptr++) = 0;        
        }

      }
    }
  
  return 1;
}


void vtkMISTVolumeRayCastMapper::ComputeMatrices( vtkImageData *data, 
                                                 vtkVolume *vol )
{
  // Get the data spacing. This scaling is not accounted for in
  // the volume's matrix, so we must add it in.
  double volumeSpacing[3];
  data->GetSpacing( volumeSpacing );
  
  // Get the origin of the data.  This translation is not accounted for in
  // the volume's matrix, so we must add it in.
  float volumeOrigin[3];
  double *bds = data->GetBounds();
  volumeOrigin[0] = bds[0];
  volumeOrigin[1] = bds[2];
  volumeOrigin[2] = bds[4];

  // Get the dimensions of the data.
  int volumeDimensions[3];
  data->GetDimensions( volumeDimensions );

  vtkTransform *voxelsTransform = this->VoxelsTransform;
  vtkTransform *voxelsToViewTransform = this->VoxelsToViewTransform;
  
  // Get the volume matrix. This is a volume to world matrix right now. 
  // We'll need to invert it, translate by the origin and scale by the 
  // spacing to change it to a world to voxels matrix.
  this->VolumeMatrix->DeepCopy( vol->GetMatrix() );
  voxelsToViewTransform->SetMatrix( VolumeMatrix );

  // Create a transform that will account for the scaling and translation of
  // the scalar data. The is the volume to voxels matrix.
  voxelsTransform->Identity();
  voxelsTransform->Translate(volumeOrigin[0], 
                             volumeOrigin[1], 
                             volumeOrigin[2] );
  
  voxelsTransform->Scale( volumeSpacing[0],
                          volumeSpacing[1],
                          volumeSpacing[2] );
  
  // Now concatenate the volume's matrix with this scalar data matrix
  voxelsToViewTransform->PreMultiply();
  voxelsToViewTransform->Concatenate( voxelsTransform->GetMatrix() );

  // Now we actually have the world to voxels matrix - copy it out
  this->WorldToVoxelsMatrix->DeepCopy( voxelsToViewTransform->GetMatrix() );
  this->WorldToVoxelsMatrix->Invert();
  
  // We also want to invert this to get voxels to world
  this->VoxelsToWorldMatrix->DeepCopy( voxelsToViewTransform->GetMatrix() );
  
  // Compute the voxels to view transform by concatenating the
  // voxels to world matrix with the projection matrix (world to view)
  voxelsToViewTransform->PostMultiply();
  voxelsToViewTransform->Concatenate( this->PerspectiveMatrix );
  
  this->VoxelsToViewMatrix->DeepCopy( voxelsToViewTransform->GetMatrix() );
  
  this->ViewToVoxelsMatrix->DeepCopy( this->VoxelsToViewMatrix );
  this->ViewToVoxelsMatrix->Invert();  
}



// Print method for vtkVolumeRayCastMapper
void vtkMISTVolumeRayCastMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Sample Distance: " << this->SampleDistance << "\n";
  os << indent << "Image Sample Distance: " 
     << this->ImageSampleDistance << "\n";

}

//----------------------------------------------------------------------------
//void vtkMISTVolumeRayCastMapper::ReportReferences(vtkGarbageCollector* collector)
//{
//  this->Superclass::ReportReferences(collector);
//
//}
