#ifndef __vtkMISTVolumeRayCastMapper_h
#define __vtkMISTVolumeRayCastMapper_h

#include "vtkVolumeMapper.h"
#include "vtkFastNumericConversion.h" // for fast rounding and floor

#include "mist/mist.h"

class vtkEncodedGradientEstimator;
class vtkEncodedGradientShader;
class vtkMatrix4x4;
class vtkMultiThreader;
class vtkPlaneCollection;
class vtkRenderer;
class vtkTimerLog;
class vtkVolume;
class vtkVolumeRayCastFunction;
class vtkVolumeTransform;
class vtkTransform;
class vtkRayCastImageDisplayHelper;

//BTX
// Macro for floor of x

//inline int vtkFloorFuncMacro(double x)
//{
//  return vtkFastNumericConversion::QuickFloor(x);
//}
//
//// Macro for rounding x (for x >= 0)
//inline int vtkRoundFuncMacro(double x)
//{
//  return vtkFastNumericConversion::Round(x);
//}
//ETX

//class VTK_VOLUMERENDERING_EXPORT vtkMISTVolumeRayCastMapper : public vtkVolumeMapper
class vtkMISTVolumeRayCastMapper : public vtkVolumeMapper
{
public:
  static vtkMISTVolumeRayCastMapper *New();
    //vtkMISTVolumeRayCastMapper *New();
  //vtkTypeRevisionMacro(vtkMISTVolumeRayCastMapper,vtkVolumeMapper);
  void PrintSelf( ostream& os, vtkIndent indent );

  // Description:
  // Set/Get the distance between samples.  This variable is only
  // used for sampling ray casting methods.  Methods that compute
  // a ray value by stepping cell-by-cell are not affected by this
  // value. 
  vtkSetMacro( SampleDistance, double );
  vtkGetMacro( SampleDistance, double );

  // Description:
  // Sampling distance in the XY image dimensions. Default value of 1 meaning
  // 1 ray cast per pixel. If set to 0.5, 4 rays will be cast per pixel. If
  // set to 2.0, 1 ray will be cast for every 4 (2 by 2) pixels.
  vtkSetClampMacro( ImageSampleDistance, double, 0.1, 100.0 );
  vtkGetMacro( ImageSampleDistance, double );

  // Description:
  // This is the minimum image sample distance allow when the image
  // sample distance is being automatically adjusted
  //vtkSetClampMacro( MinimumImageSampleDistance, double, 0.1, 100.0 );
  //vtkGetMacro( MinimumImageSampleDistance, double );

  // Description:
  // This is the maximum image sample distance allow when the image
  // sample distance is being automatically adjusted
  //vtkSetClampMacro( MaximumImageSampleDistance, double, 0.1, 100.0 );
  //vtkGetMacro( MaximumImageSampleDistance, double );
  
  // Description:
  // Set/Get the number of threads to use. This by default is equal to
  // the number of available processors detected.
  //void SetNumberOfThreads( int num );
  //int GetNumberOfThreads();
  
//BTX
  // Description:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // Initialize rendering for this volume.
  void Render( vtkRenderer *, vtkVolume * );

  // Description:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // Release any graphics resources that are being consumed by this mapper.
  // The parameter window could be used to determine which graphic
  // resources to release.
  //void ReleaseGraphicsResources(vtkWindow *);

  // Description:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // Return the scalar value below which all opacities are zero
  float GetZeroOpacityThreshold( vtkVolume *vol );
  
//ETX

protected:
  vtkMISTVolumeRayCastMapper();
  ~vtkMISTVolumeRayCastMapper();

  vtkRayCastImageDisplayHelper *ImageDisplayHelper;
  
  //virtual void ReportReferences(vtkGarbageCollector*);

  // The distance between sample points along the ray
  double                       SampleDistance;
  double                       ImageSampleDistance;
  double                       MinimumImageSampleDistance;
  double                       MaximumImageSampleDistance;
  
  double                       WorldSampleDistance;
  int                          ScalarDataType;
  void                         *ScalarDataPointer;

  void                         UpdateShadingTables( vtkRenderer *ren, 
                                                    vtkVolume *vol );

  void ComputeMatrices( vtkImageData *data, vtkVolume *vol );
  int ComputeRowBounds( vtkVolume *vol, vtkRenderer *ren );

  vtkMultiThreader  *Threader;

  vtkMatrix4x4 *PerspectiveMatrix;
  vtkMatrix4x4 *ViewToWorldMatrix;
  vtkMatrix4x4 *ViewToVoxelsMatrix;
  vtkMatrix4x4 *VoxelsToViewMatrix;
  vtkMatrix4x4 *WorldToVoxelsMatrix;
  vtkMatrix4x4 *VoxelsToWorldMatrix;

  vtkMatrix4x4 *VolumeMatrix;
  
  vtkTransform *PerspectiveTransform;
  vtkTransform *VoxelsTransform;
  vtkTransform *VoxelsToViewTransform;
  
  // This is how big the image would be if it covered the entire viewport
  int            ImageViewportSize[2];
  
  // This is how big the allocated memory for image is. This may be bigger
  // or smaller than ImageFullSize - it will be bigger if necessary to 
  // ensure a power of 2, it will be smaller if the volume only covers a
  // small region of the viewport
  int            ImageMemorySize[2];
  
  // This is the size of subregion in ImageSize image that we are using for
  // the current image. Since ImageSize is a power of 2, there is likely
  // wasted space in it. This number will be used for things such as clearing
  // the image if necessary.
  int            ImageInUseSize[2];
  
  // This is the location in ImageFullSize image where our ImageSize image
  // is located.
  int            ImageOrigin[2];
  
  // This is the allocated image
  unsigned char *Image;
  
  int  *RowBounds;
  int  *OldRowBounds;

  float        *RenderTimeTable;
  vtkVolume   **RenderVolumeTable;
  vtkRenderer **RenderRendererTable;
  int           RenderTableSize;
  int           RenderTableEntries;

  int           IntermixIntersectingGeometry;

  float        *ZBuffer;
  int           ZBufferSize[2];
  int           ZBufferOrigin[2];

  float         MinimumViewDistance;
  
  
  // Get the ZBuffer value corresponding to location (x,y) where (x,y)
  // are indexing into the ImageInUse image. This must be converted to
  // the zbuffer image coordinates. Nearest neighbor value is returned.
  double         GetZBufferValue( int x, int y );

//BTX
    //mist
    //mist::array3< unsigned short > mistArray;
    mist::array3< double > depth_map;
//ETX

private:
  vtkMISTVolumeRayCastMapper(const vtkMISTVolumeRayCastMapper&);  // Not implemented.
  void operator=(const vtkMISTVolumeRayCastMapper&);  // Not implemented.
};

#endif

