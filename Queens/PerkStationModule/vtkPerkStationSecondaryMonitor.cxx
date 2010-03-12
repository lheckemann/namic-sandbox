
#include "vtkPerkStationSecondaryMonitor.h"

#include <sstream>

#include "vtkActorCollection.h"
#include "vtkActor2DCollection.h"
#include "vtkCamera.h"
#include "vtkCylinderSource.h"
#include "vtkGlyphSource2D.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkMatrixToHomogeneousTransform.h"
#include "vtkPropCollection.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkSlicerApplication.h"
#include "vtkTextProperty.h"
#include "vtkTextActor.h"
#include "vtkTextActorFlippable.h"
#include "vtkTextSource.h"
#include "vtkTextMapper.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkVectorText.h"
#include "vtkWin32OpenGLRenderWindow.h"

#include "vtkPerkStationModuleConfigure.h"
#include "vtkPerkStationModuleGUI.h"
#include "vtkMRMLPerkStationModuleNode.h"



//----------------------------------------------------------------------------
vtkPerkStationSecondaryMonitor* vtkPerkStationSecondaryMonitor::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkPerkStationSecondaryMonitor");
  if(ret)
    {
      return (vtkPerkStationSecondaryMonitor*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkPerkStationSecondaryMonitor;
}


/**
 * Constructor.
 */
vtkPerkStationSecondaryMonitor
::vtkPerkStationSecondaryMonitor()
{
  this->GUI = NULL;
  
  this->NumberOfMonitors = 0;
  
  // monitor info related
  this->DeviceActive = false; 
  this->DisplayInitialized = false;  
  this->VirtualScreenCoord[ 0 ] = 0;
  this->VirtualScreenCoord[ 1 ] = 0;
  this->ScreenSize[ 0 ] = 800;
  this->ScreenSize[ 1 ] = 600;
  this->ScreenSize[ 2 ] = 1;
  this->CurrentRotation = 0.0;
  this->CurrentTranslation[ 0 ] = 0.0;
  this->CurrentTranslation[ 1 ] = 0.0;
  this->CalibrationFromFileLoaded = false;

  this->DepthLinesInitialized = false;
  this->NumOfDepthPerceptionLines = 0;
  
    // these values should be read in from a config file
    // the values below have been measure for 
    // Viewsonic monitor VA503b
  this->MonitorPhysicalSizeMM[ 0 ] = 304.8;
  this->MonitorPhysicalSizeMM[ 1 ] = 228.6;

  this->MonitorPixelResolution[ 0 ] = 800;
  this->MonitorPixelResolution[ 1 ] = 600;

  this->MeasureNeedleLengthInWorldCoordinates = 0;
  
    // display/view related
  this->ImageData = NULL;
  this->VolumeNode = NULL;
  
  
    // matrices
  this->SystemStateResliceMatrix = vtkSmartPointer< vtkMatrix4x4 >::New();
  this->SystemStateXYToIJK = vtkSmartPointer< vtkMatrix4x4 >::New();
  
  
  
  this->ResetCalibration(); // Give initial values to calibration parameters.
  
 
    // set up render window, renderer, actor, mapper
 
  this->Renderer = vtkSmartPointer< vtkRenderer >::New();

  this->RenderWindow = vtkSmartPointer< vtkWin32OpenGLRenderWindow >::New();
  
  this->RenderWindow->AddRenderer( this->Renderer );
  this->RenderWindow->SetBorders( 0 );
  
  vtkCamera *camera = vtkCamera::New();
  camera->SetParallelProjection( 1 );

  this->Renderer->SetActiveCamera(camera);
  camera->Delete();

  this->Interactor = vtkSmartPointer< vtkRenderWindowInteractor >::New();
   this->Interactor->SetRenderWindow( this->RenderWindow );

  vtkSlicerInteractorStyle *iStyle = vtkSlicerInteractorStyle::New();
  this->Interactor->SetInteractorStyle( iStyle );
  iStyle->Delete();

  this->MapToWindowLevelColors =
     vtkSmartPointer< vtkImageMapToWindowLevelColors >::New();
   this->MapToWindowLevelColors->SetOutputFormatToLuminance();

  this->ImageMapper = vtkSmartPointer< vtkImageMapper >::New();
   this->ImageMapper->SetColorWindow( 255 );
   this->ImageMapper->SetColorLevel( 127.5 );


  this->ImageActor = vtkSmartPointer< vtkActor2D >::New();
   this->ImageActor->SetMapper( this->ImageMapper );
   this->ImageActor->GetProperty()->SetDisplayLocationToBackground();

  this->NeedleGuideActor = vtkSmartPointer< vtkActor >::New();  
  this->NeedleTipActor = vtkSmartPointer< vtkActor >::New();
  this->RealTimeNeedleLineActor = vtkSmartPointer< vtkActor >::New();
  this->NeedleTipZLocationText = vtkSmartPointer< vtkTextActor >::New();

  this->DepthPerceptionLines = vtkSmartPointer< vtkActorCollection >::New(); 
  this->TextActorsCollection = vtkActor2DCollection::New();

  this->LeftSideActor = vtkSmartPointer< vtkTextActorFlippable >::New();
    this->LeftSideActor->SetInput( "L" );
    this->LeftSideActor->GetTextProperty()->SetColor( 1, 0.5, 0 );
    this->LeftSideActor->SetTextScaleModeToNone();
    this->LeftSideActor->GetTextProperty()->SetFontSize( 30 );
    this->LeftSideActor->GetTextProperty()->BoldOn();
    this->LeftSideActor->FlipAroundX( true );
    this->LeftSideActor->SetOrientation( 180 );
  
  this->RightSideActor = vtkSmartPointer< vtkTextActorFlippable >::New();
    this->RightSideActor->SetInput( "R" );
    this->RightSideActor->GetTextProperty()->SetColor( 1, 0.5, 0 );
    this->RightSideActor->SetTextScaleModeToNone();
    this->RightSideActor->GetTextProperty()->SetFontSize( 30 );
    this->RightSideActor->GetTextProperty()->BoldOn();
    this->RightSideActor->FlipAroundX( true );
    this->RightSideActor->SetOrientation( 180 );
  
  
   // Calibration controls.
  this->CalibrationControlsActor =
   vtkSmartPointer< vtkTextActorFlippable >::New();

    // Image geometry.
  
  this->SliceOffsetRAS = 0.0;
  
  this->SecMonFlipTransform = vtkSmartPointer< vtkTransform >::New();
  this->SecMonRotateTransform = vtkSmartPointer< vtkTransform >::New();
  this->SecMonTranslateTransform = vtkSmartPointer< vtkTransform >::New();
  
  this->ResliceFilter = vtkSmartPointer< vtkImageReslice >::New();
    this->ResliceFilter->SetBackgroundColor( 0, 0, 0, 0 );
    this->ResliceFilter->SetOutputDimensionality( 2 );
    this->ResliceFilter->SetInterpolationModeToLinear();
  
  this->ResliceTransform = vtkSmartPointer< vtkTransform >::New();
  
  this->RASToIJK = vtkSmartPointer< vtkTransform >::New();
}


//----------------------------------------------------------------------------
vtkPerkStationSecondaryMonitor::~vtkPerkStationSecondaryMonitor()
{
  this->SetGUI( NULL );

    // Release references of vtk rendering object to other object
  if ( this->RenderWindow )
    {
    this->RenderWindow->RemoveRenderer( this->Renderer );
    }
  if ( this->Renderer )
    {
    this->Renderer->RemoveAllViewProps();
    }
  if ( this->Interactor )
    {
    this->Interactor->SetRenderWindow( NULL );
    }
  if (this->ImageActor)
    {
    this->ImageActor->SetMapper( NULL );
    }

  
  /*
  if ( this->VolumeNode )
    {
    this->VolumeNode->Delete();
    this->VolumeNode = NULL;
    }
  */
  
  if ( this->DepthPerceptionLines )
    {
    this->DepthPerceptionLines->RemoveAllItems();
    }
  
  if ( this->TextActorsCollection )
    {
    this->TextActorsCollection->RemoveAllItems();
    }

}
//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::ResetCalibration()
{
  this->HorizontalFlip = false;
  this->VerticalFlip = false;
  
  this->Rotation = 0.0;
  
  this->RotationCenter[ 0 ] = 0.0;
  this->RotationCenter[ 1 ] = 0.0;
  
  this->Translation[ 0 ] = 0.0;
  this->Translation[ 1 ] = 0.0;
  
  this->Scale[ 0 ] = 1.0;
  this->Scale[ 1 ] = 1.0;
}


//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::RemoveOverlayNeedleGuide()
{
  if (!this->DisplayInitialized)
      return;

  // should remove the overlay needle guide
  vtkActorCollection *collection = this->Renderer->GetActors();
  if (collection->IsItemPresent(this->NeedleGuideActor))
    {
    this->Renderer->RemoveActor(this->NeedleGuideActor);
    if (this->DeviceActive)
        this->RenderWindow->Render();
    }

}
//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::RemoveOverlayRealTimeNeedleTip()
{
  if (!this->DisplayInitialized)
      return;

 
  vtkActorCollection *collection = this->Renderer->GetActors();
  if (collection->IsItemPresent(this->NeedleTipActor))
    {
    this->Renderer->RemoveActor(this->NeedleTipActor);
    if (this->DeviceActive)
        this->RenderWindow->Render();
    }
  if (collection->IsItemPresent(this->RealTimeNeedleLineActor))
    {
    this->Renderer->RemoveActor(this->RealTimeNeedleLineActor);
    if (this->DeviceActive)
        this->RenderWindow->Render();
    }
  if (collection->IsItemPresent(this->NeedleTipZLocationText))
    {
    this->Renderer->RemoveActor(this->NeedleTipZLocationText);
    if (this->DeviceActive)
        this->RenderWindow->Render();
    }
}


//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::SetupImageData()
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if ( ! mrmlNode )
    {
    // TODO: what to do on failure
    return;
    }

  this->VolumeNode = mrmlNode->GetPlanningVolumeNode();
  if ( ! this->VolumeNode )
    {
    // TODO: what to do on failure
    return;
    }

  this->Position = mrmlNode->GetPatientPosition();
  
  this->ImageData = this->VolumeNode->GetImageData();
  
  int imageExtent[ 6 ];
  this->ImageData->GetExtent( imageExtent );

  this->ImageSize[ 0 ] = imageExtent[ 1 ] - imageExtent[ 0 ] + 1;
  this->ImageSize[ 1 ] = imageExtent[ 3 ] - imageExtent[ 2 ] + 1;
  this->ImageSize[ 2 ] = 1;
  
  
    // Position, size, intensity window of second monitor window.
  
  if ( this->NumberOfMonitors == 2 )
    {
    this->RenderWindow->SetPosition(
      this->VirtualScreenCoord[ 0 ], this->VirtualScreenCoord[ 1 ] );
    }
  else
    {
    this->RenderWindow->SetPosition( 0, 0 );
    }
  
  this->RenderWindow->SetSize( this->ScreenSize[ 0 ], this->ScreenSize[ 1 ] );
  
  this->MapToWindowLevelColors->SetWindow(
      this->VolumeNode->GetScalarVolumeDisplayNode()->GetWindow() );
  this->MapToWindowLevelColors->SetLevel(
      this->VolumeNode->GetScalarVolumeDisplayNode()->GetLevel() );
  this->MapToWindowLevelColors->SetInput( this->ResliceFilter->GetOutput() );
  
  this->ImageMapper->SetInput( this->MapToWindowLevelColors->GetOutput() );
  
  this->Renderer->RemoveAllViewProps(); // remove previous active actors
  this->Renderer->AddActor( this->ImageActor ); // add new image actor
  
  
  this->LeftSideActor->SetDisplayPosition( this->ScreenSize[ 0 ] - 50,
                                           this->ScreenSize[ 1 ] - 50 );
  this->Renderer->AddActor( this->LeftSideActor );
  
  this->RightSideActor->SetDisplayPosition( 50, this->ScreenSize[ 1 ] - 50 );
  this->Renderer->AddActor( this->RightSideActor );
  
  
    // Calibration controls.
  
  this->CalibrationControlsActor->SetInput(
    "Up:[A]  Down:[Z]  Left:[Q]  Right:[W]   Rotatations:[G][H]" );
  this->CalibrationControlsActor->GetTextProperty()->SetColor( 1, 1, 0 );
  this->CalibrationControlsActor->SetTextScaleModeToNone();
  this->CalibrationControlsActor->GetTextProperty()->SetFontSize( 28 );
  this->CalibrationControlsActor->GetTextProperty()->BoldOn();
  this->CalibrationControlsActor->SetDisplayPosition(
    this->ScreenSize[ 0 ] / 2 + 370, this->ScreenSize[ 1 ] - 40 );
  this->CalibrationControlsActor->FlipAroundX( true );
  this->CalibrationControlsActor->SetOrientation( 180 );
  // this->CalibrationControlsActor->SetVisibility( 0 );
  
  this->Renderer->AddActor( this->CalibrationControlsActor );
  
  
  // to have consistent display i.e. same display as in SLICER's slice viewer, and own render window in secondary monitor
  // figure out whether a horizontal flip required or a vertical flip or both
  // Note: this does not counter the flip that will be required due to secondary monitors orientation/mounting
  // It only makes sure that two displays have same view if they are physically in same orientation
  vtkMatrix4x4 *directionMatrix = vtkMatrix4x4::New();
  this->VolumeNode->GetIJKToRASDirectionMatrix( directionMatrix );

  vtkMatrix4x4 *flipMatrix = vtkMatrix4x4::New();
  bool verticalFlip = false;
  bool horizontalFlip = false;

  
    // Set RASToIJK matrix, which only be updated at image load.
  
  vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
  this->VolumeNode->GetIJKToRASMatrix( ijkToRAS );
  vtkMatrix4x4::Invert( ijkToRAS, this->RASToIJK->GetMatrix() );
    
    
  this->DisplayInitialized = true;
  
  
  double spacing[ 3 ];
  this->VolumeNode->GetSpacing( spacing ); // mm between pixels
    // pixel / mm.
  double s0 = this->MonitorPhysicalSizeMM[ 0 ] / this->ScreenSize[ 0 ];
  double s1 = this->MonitorPhysicalSizeMM[ 1 ] / this->ScreenSize[ 1 ];
    // These values will be placed in the reslice transform matrix.
  this->Scale[ 0 ] = s0;
  this->Scale[ 1 ] = s1;
}


/**
 * @param x Set as horizontal displacement of the virtual image.
 * @param y Set as vertical displacement of the virtual image.
 */
void
vtkPerkStationSecondaryMonitor
::SetTranslation( double x, double y )
{
  this->Translation[ 0 ] = x;
  this->Translation[ 1 ] = y;
  this->UpdateImageDisplay();
}


/**
 * Updates calibration data from MRMLPerkStationModuleNode.
 */
void
vtkPerkStationSecondaryMonitor
::UpdateCalibration()
{
  this->VerticalFlip =
    this->GetGUI()->GetLogic()->GetPerkStationModuleNode()->GetVerticalFlip();
  this->HorizontalFlip =
    this->GetGUI()->GetLogic()->GetPerkStationModuleNode()->GetHorizontalFlip();
  
  
}


/**
 * @param angle Set as rotation of the virtual image.
 */
void
vtkPerkStationSecondaryMonitor
::SetRotation( double angle )
{
  this->Rotation = angle;
  this->UpdateImageDisplay();
}


/**
 * @returns XY to RAS transform.
 */
vtkSmartPointer< vtkTransform >
vtkPerkStationSecondaryMonitor
::XYToRAS()
{
  vtkSmartPointer< vtkTransform > ret = vtkSmartPointer< vtkTransform >::New();
  
  vtkSmartPointer< vtkMatrix4x4 > ijkToRASMatrix =
    vtkSmartPointer< vtkMatrix4x4 >::New();
  this->VolumeNode->GetIJKToRASMatrix( ijkToRASMatrix );
  
  vtkSmartPointer< vtkMatrix4x4 > xyToRASMatrix =
    vtkSmartPointer< vtkMatrix4x4 >::New();
  vtkMatrix4x4::Multiply4x4( ijkToRASMatrix, this->XYToIJK()->GetMatrix(),
    xyToRASMatrix );
  
  ret->GetMatrix()->DeepCopy( xyToRASMatrix );
  
  return ret;
}


/**
 * Vritual display = mirrored image of the second monitor screen.
 *
 * XY is the overlayed image coordinate system:
 * * Origin: lower left corner of the virtual display.
 * * X axis go to the right on the virtual display.
 * * Y axis go up on the virutal display.
 * * Units are pixels of the second monitor.
 *
 * IJK is the coordinate system of the image volume as stored in the DICOM file
 * and the vtkImageData. I and J are rows and columns of the scanned image
 * slices, K is the slice offset.
 * * Origin:
 * * Units are voxels.
 *
 * The transformation computed here moves the virtual display to the IJK image
 * volume. This is mainly used to find the reslice plane to for the
 * vtkImageReslice transform, that extracts the image slice for the second
 * monitor.
 *
 * @returns XYToIJK transformation.
 */
vtkSmartPointer< vtkTransform >
vtkPerkStationSecondaryMonitor::XYToIJK()
{
  vtkSmartPointer< vtkTransform > ret = vtkSmartPointer< vtkTransform >::New();
  
  // To make the implementation the shortest, we use Slicer's slice coordinate
  // system, which already contains the requested image slice. However, slice
  // coordinate system has some differences compared to XY:
  // * Origin is the center of the image slice.
  // * Coordinates are set for direct display, not mirrored.
  // * Units are mm.
  
    // Compute slice to IJK matrix.
  
  vtkMRMLSliceNode* sliceNode = this->GetGUI()->GetLogic()->
    GetApplicationLogic()->GetSliceLogic( "Red" )->GetSliceNode();
  
  vtkSmartPointer< vtkMatrix4x4 > sliceToRAS = sliceNode->GetSliceToRAS();
  
  vtkSmartPointer< vtkMatrix4x4 > rasToIJK =
    vtkSmartPointer< vtkMatrix4x4 >::New();
  this->VolumeNode->GetRASToIJKMatrix( rasToIJK );
  
  vtkSmartPointer< vtkMatrix4x4 > sliceToIJK =
    vtkSmartPointer< vtkMatrix4x4 >::New();
  vtkMatrix4x4::Multiply4x4( rasToIJK, sliceToRAS, sliceToIJK );
  
    // Set this as our starting transform.
  
  ret->SetMatrix( sliceToIJK );
  
  
    // Apply the following transforms, that implement the XYToSlice transform.
    // * Move the origin to the center of the virtual display.
    // * Transform units from pixels to mm, and flip image at the same time.
    // * Rotate around origin, according to manual calibraion.
    // * Move the origin, accoring to manual calibration.
  
  double hFlipFactor = 1.0;
  double vFlipFactor = 1.0;
  if ( this->HorizontalFlip ) hFlipFactor = - 1.0;
  if ( this->VerticalFlip ) vFlipFactor = - 1.0;
  
    // We are in pre-multiply mode, so write transforms in reverse order.
  
  ret->Translate( this->Translation[ 0 ], this->Translation[ 1 ], 0.0 );
  ret->RotateZ( this->Rotation );
  ret->Scale( this->Scale[ 0 ] * hFlipFactor,
              this->Scale[ 1 ] * vFlipFactor,
              1.0 );
  ret->Translate( - this->ScreenSize[ 0 ] / 2.0,
                  - this->ScreenSize[ 1 ] / 2.0,
                  0.0 );
  
  return ret;
}


//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::LoadCalibration()
{
  // note that this is under the assumption that
  // 1) this happens in Clinical mode only
  // 2) xml file has been read
  // 3) Vertical/horizontal flip has to be read in from mrml node, which was set while reading from the file
  // 4) translation, rotation has to be used from the variables CurrentRotation, CurrentTranslation
  // 5) Center of rotation is to be read from mrml node
  // 6) The monitor's physical size, and pixel resolution is also set to this class member variables while reading from the file, this defines
  // the monitor pixel size, and hence the scaling that is required
  // It is to be done in following order:
  // first reset the current calibration
  // 1) First perform the scaling, based on image pixel size and monitor pixel size
  // 2) Vertical/Horizonal flip
  // 3) Translation
  // 4) Rotation

  
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }

 
  if (!this->VolumeNode)
    {
    // TO DO: what to do on failure
    // volume node not initialized
    return;
    }


  // It is to be done in following order:
  // first reset the current calibration
  this->ResetCalibration();

  // 1) First perform the scaling, based on image pixel size and monitor pixel size
  double monSpacing[2];
  this->GetMonitorSpacing(monSpacing[0], monSpacing[1]);
      
  double imgSpacing[3];
  this->VolumeNode->GetSpacing(imgSpacing);
      
    // set the actual scaling (image/mon) in mrml node
  mrmlNode->SetActualScaling( double( imgSpacing[0] / monSpacing[0] ),
                              double( imgSpacing[1] / monSpacing[1] ) );
  double scale[2];
  mrmlNode->GetActualScaling( scale );
  
    // actually scale the image
  this->Scale[ 0 ] = scale[ 0 ];
  this->Scale[ 1 ] = scale[ 1 ];
  
  
  // 2) Flips
  this->VerticalFlip = this->GetGUI()->GetMRMLNode()->GetVerticalFlip();
  this->HorizontalFlip = this->GetGUI()->GetMRMLNode()->GetHorizontalFlip();

  
  // 3) Translation
  double trans[2];
  mrmlNode->GetClinicalModeTranslation(trans);
  this->Translation[ 0 ] = trans[0];
  this->Translation[ 1 ] = trans[1];

  // 4) Rotation
  // note that center of rotation will be automatically read inside the rotate function from the mrml node
  double rot = 0;
  this->Rotation = mrmlNode->GetClinicalModeRotation();  
  
  this->CalibrationFromFileLoaded = true;
}


//----------------------------------------------------------------------------
vtkMatrix4x4*
vtkPerkStationSecondaryMonitor
::GetFlipMatrixFromDirectionCosines ( vtkMatrix4x4 *directionMatrix,
                                      bool & verticalFlip,
                                      bool & horizontalFlip )
{
  vtkMatrix4x4 *flipMatrix = vtkMatrix4x4::New();
  flipMatrix->Identity();

  vtkMatrix4x4 *targetMatrix = vtkMatrix4x4::New();
    targetMatrix->Identity();
    targetMatrix->SetElement( 0, 0, -1.0 );

  // target 3x3 matrix  -1 0 0 
  //                     0 1 0
  //                     0 0 1

  // source: directionMatrix

  // target = flipMatrix x source
  // flipMatrix = source.invert() x target

  directionMatrix->Invert();
  vtkMatrix4x4::Multiply4x4( directionMatrix, targetMatrix, flipMatrix );

  if ( flipMatrix->GetElement( 0, 0 ) < 0 ) // i.e. -1
    {
    horizontalFlip = true;
    // flip on y axis and bring back to match center of screen
    flipMatrix->SetElement( 0, 3, this->ImageSize[0]);
    }

  if ( flipMatrix->GetElement( 1, 1 ) < 0 ) // i.e. -1
    {
    verticalFlip = true;
    // flip on x-axis and bring back to match center of screen
    flipMatrix->SetElement(1,3, this->ImageSize[1]);
    }

  return flipMatrix;
}


//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::UpdateImageDisplay()
{
  if ( ! this->GetGUI()->GetMRMLNode() ) return;
  
     // Switch visibility of needle guide.
     // Show needle guide only in planning plane +/- 0.5 mm.
   
  double entry[ 3 ];
  double target[ 3 ];
  this->GetGUI()->GetMRMLNode()->GetPlanEntryPoint( entry );
  this->GetGUI()->GetMRMLNode()->GetPlanTargetPoint( target );
  double minOffset = target[ 2 ] - 0.5;
  double maxOffset = entry[ 2 ] + 0.5;
  if ( entry[ 2 ] < target[ 2 ] )
   {
   minOffset = entry[ 2 ] - 0.5;
   maxOffset = target[ 2 ] + 0.5;
   }

  if (
      ( this->SliceOffsetRAS <= maxOffset )
      && ( this->SliceOffsetRAS >= minOffset )
      && ( this->GetGUI()->GetMRMLNode()->GetCurrentStep() == 2 )
    )
   {
   this->ShowNeedleGuide( true );
   this->ShowDepthPerceptionLines( true );
   }
  else
   {
   this->ShowNeedleGuide( false );
   this->ShowDepthPerceptionLines( false );
   }
 
  
  if ( ! this->DisplayInitialized )
    {
    return;
    }
  
  
    // Extract slice from the image volume.
  
    // It is not possible to apply vtkTransformFilter on an image data.
    // So all transformations has to be applied in ResliceFilter.
    // Currently, transformations of the overlayed image is done only in
    // XYToIJK function.
  
  this->ResliceTransform->SetMatrix( this->XYToIJK()->GetMatrix() );
  this->ResliceTransform->Update();
  
  this->ResliceFilter->SetOutputExtent( 0, this->ScreenSize[ 0 ] - 1,
                                        0, this->ScreenSize[ 1 ] - 1,
                                        0, 0 );
  this->ResliceFilter->SetOutputSpacing( 1.0, 1.0, 1.0 );
  this->ResliceFilter->SetOutputOrigin( 0.0, 0.0, 0.0 );
  this->ResliceFilter->SetInput( this->ImageData );
  this->ResliceFilter->SetResliceTransform( this->ResliceTransform );
  
    // Adjust color window / level.
  
  this->MapToWindowLevelColors->SetWindow(
      this->VolumeNode->GetScalarVolumeDisplayNode()->GetWindow() );
  this->MapToWindowLevelColors->SetLevel( 
      this->VolumeNode->GetScalarVolumeDisplayNode()->GetLevel() );
  this->MapToWindowLevelColors->SetInput(
      this->ResliceFilter->GetOutput() );
  this->MapToWindowLevelColors->Update();
  
  
  if ( this->DeviceActive )
    {
    this->RenderWindow->Render();  
    }
}


//---------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::UpdateImageDataOnSliceOffset(
  double rasOffset )
{
  this->SliceOffsetRAS = rasOffset;
  
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }
  if (!mrmlNode->GetVolumeInUse())
    {
    // TO DO: what to do on failure
    return;
    }

  // if ( strcmpi( mrmlNode->GetVolumeInUse(), "Planning" ) ) return;

  
    // Compute the K offset from RAS Z offset (S value).
  
  double rasPt[ 4 ] = { 0, 0, rasOffset, 1 };
  double ijkPt[ 4 ];
  this->RASToIJK->MultiplyPoint( rasPt, ijkPt );
  this->SliceOffsetIJK = ijkPt[ 2 ];
  
  this->UpdateImageDisplay();
}


//----------------------------------------------------------------------------
BOOL
CALLBACK
MyInfoEnumProc( HMONITOR hMonitor, HDC hdc, LPRECT prc, LPARAM dwData ) 
{
  vtkPerkStationSecondaryMonitor *self = (vtkPerkStationSecondaryMonitor *)dwData;
  if (self)
    {
    MONITORINFO mi;
    mi.cbSize = sizeof( MONITORINFO );
    if ( GetMonitorInfo( hMonitor, &mi ) )
      {
      // check if the monitor is primary or not
      if ( ! ( mi.dwFlags & MONITORINFOF_PRIMARY ) )
        {
        // monitor not primary, store the rect information
        self->SetVirtualScreenCoord( mi.rcMonitor.left, mi.rcMonitor.top );       
        }
      }
    
    }
  return TRUE;
}


//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::Initialize()
{
    // first make sure secondary monitor is connected
  this->NumberOfMonitors = GetSystemMetrics( SM_CMONITORS );
  
  
    // For testing, in case of one physical monitor, create the second monitor
    // in a separate window on the first monitor.
  
  if ( this->NumberOfMonitors != 2 )
    {
    // this->MonitorPhysicalSizeMM[ 0 ] = 160;
    // this->MonitorPhysicalSizeMM[ 1 ] = 120;
    this->MonitorPixelResolution[ 0 ] = 1024;
    this->MonitorPixelResolution[ 1 ] = 768;
    this->DeviceActive = true;
    this->ScreenSize[ 0 ] = 1024;
    this->ScreenSize[ 1 ] = 768;
    }
  else
    {
    // get info about the monitors
    EnumDisplayMonitors( NULL, NULL, MyInfoEnumProc, ( LPARAM ) this );
    
    // get the resolution/dimensions/spacing
    DISPLAY_DEVICE lpDisplayDevice;
    lpDisplayDevice.cb = sizeof( lpDisplayDevice );
    lpDisplayDevice.StateFlags = DISPLAY_DEVICE_ATTACHED_TO_DESKTOP;
    int iDevNum = 0;
    int iPhyDevNum = 0;
    DWORD dwFlags = 0;

    while( EnumDisplayDevices( NULL, iDevNum, &lpDisplayDevice, dwFlags ) )
      {
      iDevNum++;
      if( ( lpDisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP )
           == 1 )
        {
        iPhyDevNum++;
        
          // get the device context for the monitor
        HDC hdc = CreateDC( "DISPLAY", lpDisplayDevice.DeviceName, NULL, NULL );    
        
          // now the device context can be used in many functions to retrieve
          // information about the monitor
        double width_mm = GetDeviceCaps( hdc, HORZSIZE );
        double height_mm = GetDeviceCaps( hdc, VERTSIZE );
        double width_pix = GetDeviceCaps( hdc, HORZRES );
        double height_pix = GetDeviceCaps( hdc, VERTRES );
        
        if ( ! ( lpDisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) )
          {
          this->MonitorPhysicalSizeMM[ 0 ] = width_mm;
          this->MonitorPhysicalSizeMM[ 1 ] = height_mm;
          this->MonitorPixelResolution[ 0 ] = width_pix;
          this->MonitorPixelResolution[ 1 ] = height_pix;
          this->DeviceActive = true;
          this->ScreenSize[ 0 ] = width_pix;
          this->ScreenSize[ 1 ] = height_pix;
          }
        
        DeleteDC( hdc );
        }
      }
    }
  
  // The physical parameters of the second monitor are theoretically known
  // at this point, but are they often not accurate. So we don't use
  // these values, but ask the user to select the second monitor from a
  // drop-down list. Use values from an xml file, corresponding to the
  // selected monitor type.
}



//-----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::OverlayRealTimeNeedleTip(double tipRAS[3], vtkMatrix4x4 *toolTransformMatrix)
{
  vtkMatrix4x4 *rasToXY = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert( this->XYToRAS()->GetMatrix(), rasToXY );
  
  
  double xyPoint[2];
  double worldCoordinate[4];
  double wcPoint[3];
   
  double inPt[4] = {tipRAS[0], tipRAS[1], tipRAS[2], 1};
  double outPt[4];  
  rasToXY->MultiplyPoint(inPt, outPt);
  xyPoint[0] = outPt[0];
  xyPoint[1] = outPt[1];

  this->Renderer->SetDisplayPoint(xyPoint[0],xyPoint[1], 0);
  this->Renderer->DisplayToWorld();
  this->Renderer->GetWorldPoint(worldCoordinate);
  wcPoint[0] = worldCoordinate[0];
  wcPoint[1] = worldCoordinate[1];
  wcPoint[2] = worldCoordinate[2];

  // model it as a line, with glyph at the end
  // one point on the line is needle tip itself
  // we know the slope of the line, hence we can get the second point on the line
  // y = mx + c
    
  vtkTransform *toolTransform = vtkTransform::New();
  toolTransform->SetMatrix(toolTransformMatrix);
  float xyz[3];
  toolTransform->GetOrientation(xyz);
   
  double angleRad = double(vtkMath::Pi()/180)*xyz[2];

  // make needle tip point lie on the line
  // c = y1 - mx1
  float c = tipRAS[1] - tan(angleRad)*tipRAS[0];

  // for the end point lets say we know y = 0, we need to know x
  double xyEndPoint[2];
  xyEndPoint[1] = this->ScreenSize[1];

  // first to RAS
  // to get correct A coordinate
  double rasEndPoint[3];
  inPt[0] = 0;
  inPt[1] = xyEndPoint[1];
  inPt[2] = 0;
  this->XYToRAS()->GetMatrix()->MultiplyPoint(inPt, outPt);
  rasEndPoint[1] = outPt[1];

  // get the R coordinate
  // x = (y - c)/m
  rasEndPoint[0] = (rasEndPoint[1]-c)/tan(angleRad);
  rasEndPoint[2] = 0;

  // back to xy coordinate system
  inPt[0] = rasEndPoint[0];
  inPt[1] = rasEndPoint[1];
  inPt[2] = rasEndPoint[2];
  rasToXY->MultiplyPoint(inPt, outPt);
  xyEndPoint[0] = outPt[0];
  xyEndPoint[1] = outPt[1];


  // now from xy to world coordinates
  double wcEndPoint[3];
  this->Renderer->SetDisplayPoint(xyEndPoint[0],xyEndPoint[1], 0);
  this->Renderer->DisplayToWorld();
  this->Renderer->GetWorldPoint(worldCoordinate);
  wcEndPoint[0] = worldCoordinate[0];
  wcEndPoint[1] = worldCoordinate[1];
  wcEndPoint[2] = worldCoordinate[2];

  vtkLineSource *needleLine = vtkLineSource::New();
  needleLine->SetPoint1(wcPoint);
  needleLine->SetPoint2(wcEndPoint);
  

  vtkPolyDataMapper *needleLineMapper = vtkPolyDataMapper::New();  
  needleLineMapper->SetInputConnection(needleLine->GetOutputPort());

  this->RealTimeNeedleLineActor->SetMapper(needleLineMapper);
  this->RealTimeNeedleLineActor->GetProperty()->SetColor(1,0,0);
 
  // add to renderer of the Axial slice viewer
  this->Renderer->AddActor(this->RealTimeNeedleLineActor);  


  // set up the needle tip actor
  vtkGlyphSource2D *needleTip = vtkGlyphSource2D::New();
  needleTip->SetGlyphTypeToThickCross(); 
  needleTip->SetFilled (0);
  needleTip->SetScale(0.05);
  needleTip->SetColor(1,0,0);  
  needleTip->Update();
  
  
  vtkPolyDataMapper *needleMapper = vtkPolyDataMapper::New();
  //needleMapper->SetInputConnection(filter->GetOutputPort());
  needleMapper->SetInputConnection(needleTip->GetOutputPort());

  this->NeedleTipActor->SetMapper(needleMapper);
  this->NeedleTipActor->SetPosition(wcPoint[0], wcPoint[1],wcPoint[2]);
  //this->NeedleTipActor->RotateZ(xyz[2]);
  
 
  // add to renderer of the Axial slice viewer
  this->Renderer->AddActor(this->NeedleTipActor);

  // text actors for needle tip S, and target location    
  double rasTarget[3];
  this->GetGUI()->GetMRMLNode()->GetPlanEntryPoint(rasTarget);
  char text[50];
  sprintf(text,"Needle Tip Z:  %.2f\nTarget Z:     %.2f",tipRAS[2], rasTarget[2]);      
  this->NeedleTipZLocationText->SetInput(text);
  this->NeedleTipZLocationText->SetTextScaleModeToNone();
  this->NeedleTipZLocationText->GetTextProperty()->SetFontSize(25);
  this->NeedleTipZLocationText->SetDisplayPosition(this->MonitorPixelResolution[0]-250, 100);
  if ( this->HorizontalFlip )
    { 
    this->NeedleTipZLocationText->GetTextProperty()->SetJustificationToCentered();
    this->NeedleTipZLocationText->SetOrientation(180);
    //char *revText = vtkPerkStationModuleLogic::strrev(text, strlen(text));
    //this->NeedleTipZLocationText->SetInput(revText);
    }
  else if ( this->VerticalFlip )
    {
    this->NeedleTipZLocationText->GetTextProperty()->SetVerticalJustificationToTop();
    this->NeedleTipZLocationText->SetOrientation(180);
    }

  this->Renderer->AddActor(this->NeedleTipZLocationText);

  if (this->DeviceActive && this->DisplayInitialized)
    this->RenderWindow->Render(); 
  
}
//------------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::OverlayNeedleGuide()
{
  
  if (!this->DeviceActive)
    return;

  // get the world coordinates
  int point[2];
  double xyEntry[2];
  double xyTarget[2];

  double worldCoordinate[4];
  double wcEntryPoint[3];
  double wcTargetPoint[3];
  
  vtkMatrix4x4 *rasToXY = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert(this->XYToRAS()->GetMatrix(), rasToXY);

  // entry point
  double rasEntry[3];
  this->GetGUI()->GetMRMLNode()->GetPlanEntryPoint(rasEntry);
  double inPt[4] = {rasEntry[0], rasEntry[1], rasEntry[2], 1};
  double outPt[4];  
  rasToXY->MultiplyPoint(inPt, outPt);
  point[0] = outPt[0];
  point[1] = outPt[1];
  xyEntry[0] = point[0];
  xyEntry[1] = point[1];

  this->Renderer->SetDisplayPoint(point[0],point[1], 0);
  this->Renderer->DisplayToWorld();
  this->Renderer->GetWorldPoint(worldCoordinate);
  wcEntryPoint[0] = worldCoordinate[0];
  wcEntryPoint[1] = worldCoordinate[1];
  wcEntryPoint[2] = worldCoordinate[2];

  double rasTarget[3];
  this->GetGUI()->GetMRMLNode()->GetPlanTargetPoint(rasTarget);
  inPt[0] = rasTarget[0];
  inPt[1] = rasTarget[1];
  inPt[2] = rasTarget[2];
  rasToXY->MultiplyPoint(inPt, outPt);
  point[0] = outPt[0];
  point[1] = outPt[1];
  xyTarget[0] = point[0];
  xyTarget[1] = point[1];

  this->Renderer->SetDisplayPoint(point[0],point[1], 0);
  this->Renderer->DisplayToWorld();
  this->Renderer->GetWorldPoint(worldCoordinate);
  wcTargetPoint[0] = worldCoordinate[0];
  wcTargetPoint[1] = worldCoordinate[1];
  wcTargetPoint[2] = worldCoordinate[2];


  double halfNeedleGuideLength = sqrt( (wcTargetPoint[0]-wcEntryPoint[0])*(wcTargetPoint[0]-wcEntryPoint[0]) + (wcTargetPoint[1]-wcEntryPoint[1])*(wcTargetPoint[1]-wcEntryPoint[1]));

  this->MeasureNeedleLengthInWorldCoordinates = halfNeedleGuideLength;

  // steps
  // get the cylinder source, create the cylinder, whose height is equal to
  // calculated insertion depth apply transform on the cylinder to world
  // coordinates, using the information of entry and target point
  // i.e. using the insertion angle
  // add it to slice viewer's renderer

  vtkCylinderSource *needleGuide = vtkCylinderSource::New();
  // TO DO: how to relate this to actual depth???
  needleGuide->SetHeight(2*halfNeedleGuideLength );
  //needleGuide->SetHeight(0.75);
  needleGuide->SetRadius( 0.009 );  
  needleGuide->SetResolution( 20 );

  // because cylinder is positioned at the window center
  double needleCenter[3];  
  needleCenter[0] = wcEntryPoint[0];// - windowSize[0]/2;
  needleCenter[1] = wcEntryPoint[1];// - windowSize[1]/2;
  
  // angle as calculated from xy coordinates
  double insAngle = -double(180/vtkMath::Pi()) * atan(double((xyEntry[1] - xyTarget[1])/(xyEntry[0] - xyTarget[0])));
  
  
  // TO DO: transfrom needle mapper using vtkTransformPolyData
  vtkMatrix4x4 *transformMatrix = vtkMatrix4x4::New();
  transformMatrix->Identity();


  
  double insAngleRad = vtkMath::Pi()/2 - double(vtkMath::Pi()/180)*insAngle;

  transformMatrix->SetElement(0,0, cos(insAngleRad));
  transformMatrix->SetElement(0,1, -sin(insAngleRad));
  transformMatrix->SetElement(0,2, 0);
  transformMatrix->SetElement(0,3, 0);
  transformMatrix->SetElement(0,3, needleCenter[0]);

  transformMatrix->SetElement(1,0, sin(insAngleRad));
  transformMatrix->SetElement(1,1, cos(insAngleRad));
  transformMatrix->SetElement(1,2, 0);
  transformMatrix->SetElement(1,3, 0);
  transformMatrix->SetElement(1,3, needleCenter[1]);

  transformMatrix->SetElement(2,0, 0);
  transformMatrix->SetElement(2,1, 0);
  transformMatrix->SetElement(2,2, 1);
  transformMatrix->SetElement(2,3, 0);

  transformMatrix->SetElement(3,0, 0);
  transformMatrix->SetElement(3,1, 0);
  transformMatrix->SetElement(3,2, 0);
  transformMatrix->SetElement(3,3, 1);

  vtkMatrixToHomogeneousTransform *transform = vtkMatrixToHomogeneousTransform::New();
  transform->SetInput(transformMatrix);
  vtkTransformPolyDataFilter *filter = vtkTransformPolyDataFilter::New();
  filter->SetInputConnection(needleGuide->GetOutputPort()); 
  filter->SetTransform(transform);

  // map
  vtkPolyDataMapper *needleMapper = vtkPolyDataMapper::New();
  needleMapper->SetInputConnection( filter->GetOutputPort() );
  
  // after transfrom, set up actor 
  this->NeedleGuideActor->SetMapper(needleMapper );
  
  // add to renderer of the Axial slice viewer
  this->Renderer->AddActor(this->NeedleGuideActor);  
  this->RenderWindow->Render(); 
}


//------------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::SetDepthPerceptionLines()
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if ( ! mrmlNode ) return;

  if ( this->DepthLinesInitialized ) return;

  this->DepthPerceptionLines->RemoveAllItems();
  this->TextActorsCollection->RemoveAllItems();

    // Get insertion depth
  double insertionDepth = mrmlNode->GetActualPlanInsertionDepth();

  if ( insertionDepth > 0 )
    {
    // first calculate how many, in increments of 10 mm, less than equal to 5 lines, that less than max number of lines
    this->NumOfDepthPerceptionLines = insertionDepth / 10;
      
    double lengthIncrement = 10.0; // in mm
    
    
    vtkMatrix4x4 *rasToXY = vtkMatrix4x4::New();
    vtkMatrix4x4::Invert( this->XYToRAS()->GetMatrix(), rasToXY );

    double pointXY[ 2 ];
    double worldCoordinate[ 4 ];
    double wcStartPoint[ 4 ];
    double wcEndPoint[ 4 ];

    // entry point
    double rasEntry[ 3 ];
    mrmlNode->GetPlanEntryPoint( rasEntry );
    // target point
    double rasTarget[ 3 ];
    mrmlNode->GetPlanTargetPoint( rasTarget );

    double rasTemp[ 3 ];
    double inPt[ 4 ];
    double outPt[ 4 ];  
        

    double denom = rasTarget[ 0 ] - rasEntry[ 0 ];
    double numer = rasTarget[ 1 ] - rasEntry[ 1 ];

    double insertionAngle = atan( double( rasTarget[1] - rasEntry[1] ) /
                                  double( rasTarget[0] - rasEntry[0] ) );
      

    // create the end point coordinates of each depth perception line,
    // starting from entry point, moving towards target point
    for ( unsigned int i = 0; i < this->NumOfDepthPerceptionLines; i++ )
      {
      if ( denom >= 0 )
        {
        rasTemp[0] = rasEntry[0] + lengthIncrement*(i+1)*cos(insertionAngle);
        rasTemp[1] = rasEntry[1] + lengthIncrement*(i+1)*sin(insertionAngle);
        }
      else
        {
        rasTemp[0] = rasEntry[0] - lengthIncrement*(i+1)*cos(insertionAngle);
        rasTemp[1] = rasEntry[1] - lengthIncrement*(i+1)*sin(insertionAngle);
        }
      rasTemp[2] = rasEntry[2];

      inPt[0] = rasTemp[0];
      inPt[1] = rasTemp[1];
      inPt[2] = rasTemp[2];
      inPt[3] = 1;

      // convert to xy
      rasToXY->MultiplyPoint(inPt, outPt);
      pointXY[0] = outPt[0];
      pointXY[1] = outPt[1];

      // convert to world coordinate
      this->Renderer->SetDisplayPoint(pointXY[0],pointXY[1], 0);
      this->Renderer->DisplayToWorld();
      this->Renderer->GetWorldPoint(worldCoordinate);
      
      // get start and end points
      wcStartPoint[0] = worldCoordinate[0];
      wcStartPoint[1] = worldCoordinate[1];
      wcStartPoint[2] = worldCoordinate[2];

        // end point
      if (denom>=0)
        {
        pointXY[0] = 0;   
        }
      else
        {
        pointXY[0] = this->MonitorPixelResolution[0];
        }


      // convert to world coordinate
      this->Renderer->SetDisplayPoint(pointXY[0],pointXY[1], 0);
      this->Renderer->DisplayToWorld();
      this->Renderer->GetWorldPoint(worldCoordinate);

      wcEndPoint[0] = worldCoordinate[0];
      wcEndPoint[1] = worldCoordinate[1];
      wcEndPoint[2] = worldCoordinate[2];

      // set up the line
      vtkLineSource *line = vtkLineSource::New();
      line->SetPoint1(wcStartPoint);
      line->SetPoint2(wcEndPoint);

      // set up the mapper,
      vtkPolyDataMapper *lineMapper = vtkPolyDataMapper::New();
      lineMapper->SetInputConnection( line->GetOutputPort() );
    

      // actor
      vtkActor *lineActor = vtkActor::New();
      lineActor->SetMapper(lineMapper);
      lineActor->GetProperty()->SetColor(0,255,0);


      char *text = new char[10];
      sprintf(text,"%d mm",(i+1)*10);     
     
      // vtkTextActor *textActor = vtkTextActor::New();
      vtkTextActorFlippable *textActor = vtkTextActorFlippable::New();
      textActor->SetInput(text);

      //textActor->SetPosition(wcStartPoint[0],wcStartPoint[1],wcStartPoint[2]);
      //textActor->GetProperty()->SetColor(0,0,1);
      textActor->GetTextProperty()->SetColor(0,1,0);
      textActor->SetTextScaleModeToNone();
      textActor->GetTextProperty()->SetFontSize(30);
      textActor->FlipAroundX( true );
      textActor->SetOrientation( 180 );
      
      if (denom>=0)
        {
        // textActor->SetDisplayPosition(pointXY[0]+100, pointXY[1]+5);
        textActor->SetDisplayPosition(pointXY[0]+120, pointXY[1]+25);
        // convert to world coordinate
        //this->Renderer->SetDisplayPoint(pointXY[0]+100,pointXY[1]+5, 0);
        //this->Renderer->DisplayToWorld();
        //this->Renderer->GetWorldPoint(worldCoordinate);
        //textActor->SetPosition(worldCoordinate[0],worldCoordinate[1],worldCoordinate[2]);       
        }
      else
        {
        textActor->SetDisplayPosition(pointXY[0]-120, pointXY[1]+25);
        // convert to world coordinate
        //this->Renderer->SetDisplayPoint(pointXY[0]-100,pointXY[1]+5, 0);
        //this->Renderer->DisplayToWorld();
        //this->Renderer->GetWorldPoint(worldCoordinate);
        //textActor->SetPosition(worldCoordinate[0],worldCoordinate[1],worldCoordinate[2]);
        }

      
      // flip vertically the text actor
      if ( this->VerticalFlip )
        {
        textActor->GetTextProperty()->SetVerticalJustificationToTop();
        }

      // add text actor to text actor collection
      this->TextActorsCollection->AddItem(textActor);
     
      // add text actor to the renderer
      this->Renderer->AddActor(textActor);


       
      // add to actor collection
      this->DepthPerceptionLines->AddItem(lineActor);
      this->Renderer->AddActor(lineActor);



      lineMapper->Delete();
      lineActor->Delete();
      textActor->Delete();
      }
      
    // add the last line for final target depth
    this->NumOfDepthPerceptionLines++;

    
    inPt[ 0 ] = rasTarget[ 0 ];
    inPt[ 1 ] = rasTarget[ 1 ];
    inPt[ 2 ] = rasTarget[ 2 ];
    inPt[ 3 ] = 1;

    // convert to xy
    rasToXY->MultiplyPoint( inPt, outPt );
    pointXY[ 0 ] = outPt[ 0 ];
    pointXY[ 1 ] = outPt[ 1 ];

    // convert to world coordinate
    this->Renderer->SetDisplayPoint( pointXY[0], pointXY[1], 0 );
    this->Renderer->DisplayToWorld();
    this->Renderer->GetWorldPoint( worldCoordinate );
        
    // get start and end points
    wcStartPoint[ 0 ] = worldCoordinate[ 0 ];
    wcStartPoint[ 1 ] = worldCoordinate[ 1 ];
    wcStartPoint[ 2 ] = worldCoordinate[ 2 ];

    // end point
    if (denom>=0)
      {
      pointXY[0] = 0;   
      }
    else
      {
      pointXY[0] = this->MonitorPixelResolution[0];
      } 

    // convert to world coordinate
    this->Renderer->SetDisplayPoint( pointXY[ 0 ], pointXY[ 1 ], 0 );
    this->Renderer->DisplayToWorld();
    this->Renderer->GetWorldPoint( worldCoordinate );

    wcEndPoint[ 0 ] = worldCoordinate[ 0 ];
    wcEndPoint[ 1 ] = worldCoordinate[ 1 ];
    wcEndPoint[ 2 ] = worldCoordinate[ 2 ];


    // Set up the line and its actor.
    vtkSmartPointer< vtkLineSource > line =
        vtkSmartPointer< vtkLineSource >::New();
      line->SetPoint1( wcStartPoint );
      line->SetPoint2( wcEndPoint );

    vtkSmartPointer< vtkPolyDataMapper > lineMapper =
        vtkSmartPointer< vtkPolyDataMapper >::New();
      lineMapper->SetInputConnection( line->GetOutputPort() );
    
    vtkSmartPointer< vtkActor > lineActor = vtkSmartPointer< vtkActor >::New();
      lineActor->SetMapper( lineMapper );
      lineActor->GetProperty()->SetColor( 0, 255, 0 );

    // add to actor collection
    this->DepthPerceptionLines->AddItem( lineActor );
    this->Renderer->AddActor( lineActor );
    
    
    // set up the measurement line/cylinder
    // find the point where to place the measurement cylinder, either in the right or left, but in the middle in y-axis
    double xyMeasuringLineDock[2];

    if ( denom >= 0 )
      {
      xyMeasuringLineDock[ 0 ] = this->MonitorPixelResolution[ 0 ] - 100;   
      }
    else
      {    
      xyMeasuringLineDock[ 0 ] = 100;
      } 

    xyMeasuringLineDock[ 1 ] = this->MonitorPixelResolution[ 1 ] / 2;
    
    
      // Set up the needle measure.
    
    vtkSmartPointer< vtkCylinderSource > needleMeasure =
        vtkSmartPointer< vtkCylinderSource >::New();    
      needleMeasure->SetHeight( this->MeasureNeedleLengthInWorldCoordinates ); 
      needleMeasure->SetRadius( 0.005 );  
      needleMeasure->SetResolution( 20 );

    vtkSmartPointer< vtkPolyDataMapper > measureMapper =
        vtkSmartPointer< vtkPolyDataMapper >::New();  
      measureMapper->SetInputConnection( needleMeasure->GetOutputPort() );

    vtkSmartPointer< vtkActor > measureLineActor =
        vtkSmartPointer< vtkActor >::New();
      measureLineActor->SetMapper( measureMapper );  
    
    
      // convert to world coordinate
    this->Renderer->SetDisplayPoint( xyMeasuringLineDock[ 0 ],
      xyMeasuringLineDock[ 1 ], 0 );
    this->Renderer->DisplayToWorld();
    this->Renderer->GetWorldPoint( worldCoordinate );

    measureLineActor->SetPosition( worldCoordinate[ 0 ],
      worldCoordinate[ 1 ], worldCoordinate[ 2 ] );
 
     // add to actor collection
    this->DepthPerceptionLines->AddItem( measureLineActor );
    this->Renderer->AddActor( measureLineActor );
    
    int actorCount = this->Renderer->VisibleActorCount();
    this->DepthLinesInitialized = true;

    if ( this->DeviceActive && this->DisplayInitialized )
      {   
      this->RenderWindow->Render();
      }
  }

}
//------------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::RemoveDepthPerceptionLines()
{
  if( ! this->DepthLinesInitialized ) return;

  int actorCount = this->Renderer->VisibleActorCount();

  vtkActorCollection *collection = this->Renderer->GetActors();
  
  for( int i = 0; i < this->DepthPerceptionLines->GetNumberOfItems(); i++ )
    {
    vtkActor *lineActor = vtkActor::SafeDownCast(
      this->DepthPerceptionLines->GetItemAsObject( i ) );
    if ( collection->IsItemPresent( lineActor ) )
      {
      this->Renderer->RemoveActor( lineActor );
      }   
    }

  this->DepthPerceptionLines->RemoveAllItems();
  this->DepthLinesInitialized = false;

  this->RemoveTextActors();
  
  if ( this->DeviceActive && this->DisplayInitialized )
    {
    this->RenderWindow->Render();
    }
}


//--------------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::RemoveTextActors()
{
  int actorCount = this->Renderer->VisibleActorCount();

  // remove text actors
  for(int i = 0; i < this->TextActorsCollection->GetNumberOfItems(); i++)
    {
    vtkTextActor *textActor = vtkTextActor::SafeDownCast(
      this->TextActorsCollection->GetItemAsObject( i ) );
    this->Renderer->RemoveActor( textActor );
    }

  this->TextActorsCollection->RemoveAllItems();
}


// ----------------------------------------------------------------------------
void
vtkPerkStationSecondaryMonitor
::SetRealTimeNeedleLineActorVisibility( bool v )
{
  this->RealTimeNeedleLineActor->SetVisibility(v);
  this->NeedleTipActor->SetVisibility(v);
  this->RenderWindow->Render();
}



  // New image geometry handling.

void
vtkPerkStationSecondaryMonitor
::SetHorizontalFlip( bool flip )
{
  this->HorizontalFlip = flip;
  this->UpdateImageDisplay();
}


void
vtkPerkStationSecondaryMonitor
::SetVerticalFlip( bool flip )
{
  this->VerticalFlip = flip;
  this->UpdateImageDisplay();
}



void
vtkPerkStationSecondaryMonitor
::SetRotationCenter( double center[ 3 ] )
{
  for ( int i = 0; i < 3; ++ i )
    {
    this->RotationCenter[ i ] = center[ i ];
    }
}


/**
 * @param show True makes object visible, false hides it.
 */
void
vtkPerkStationSecondaryMonitor
::ShowCalibrationControls( bool show )
{
  if ( show )
    {
    this->CalibrationControlsActor->SetVisibility( 1 );
    }
  else
    {
    this->CalibrationControlsActor->SetVisibility( 0 );
    }
}


/**
 * @param show True makes object visible, false hides it.
 */
void
vtkPerkStationSecondaryMonitor
::ShowNeedleGuide( bool show )
{
  if ( show )
    {
    this->NeedleGuideActor->SetVisibility( 1 );
    }
  else
    {
    this->NeedleGuideActor->SetVisibility( 0 );
    }
}



/**
 * @param show True makes object visible, false hides it.
 */
void
vtkPerkStationSecondaryMonitor
::ShowDepthPerceptionLines( bool show )
{
  if ( show )
    {
    for( int i = 0; i < this->DepthPerceptionLines->GetNumberOfItems(); ++ i )
      {
      vtkActor* actor = vtkActor::SafeDownCast(
        this->DepthPerceptionLines->GetItemAsObject( i ) );
      actor->SetVisibility( 1 );
      }
    for( int i = 0; i < this->TextActorsCollection->GetNumberOfItems(); ++ i )
      {
      vtkTextActorFlippable* actor = vtkTextActorFlippable::SafeDownCast(
        this->TextActorsCollection->GetItemAsObject( i ) );
      actor->SetVisibility( 1 );
      }
    }
  else
    {
    for( int i = 0; i < this->DepthPerceptionLines->GetNumberOfItems(); ++ i )
      {
      vtkActor* actor = vtkActor::SafeDownCast(
        this->DepthPerceptionLines->GetItemAsObject( i ) );
      actor->SetVisibility( 0 );
      }
    for( int i = 0; i < this->TextActorsCollection->GetNumberOfItems(); ++ i )
      {
      vtkTextActorFlippable* actor = vtkTextActorFlippable::SafeDownCast(
        this->TextActorsCollection->GetItemAsObject( i ) );
      actor->SetVisibility( 0 );
      }
    }
}
