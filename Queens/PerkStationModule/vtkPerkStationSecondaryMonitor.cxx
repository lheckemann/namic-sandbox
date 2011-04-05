
#include "vtkPerkStationSecondaryMonitor.h"

#include <sstream>
#include <vector>

#include "vtkActorCollection.h"
#include "vtkActor2DCollection.h"
#include "vtkCamera.h"
#include "vtkCollection.h"
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
#include "vtkSlicerApplicationGUI.h"
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




vtkPerkStationSecondaryMonitor*
vtkPerkStationSecondaryMonitor
::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance( "vtkPerkStationSecondaryMonitor" );
  if( ret )
    {
      return ( vtkPerkStationSecondaryMonitor* )ret;
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
  this->DeviceActive     = false; 
  this->Initialized      = false;
  this->ImageLoaded      = false;
  this->NumberOfMonitors = 0;
  
  this->VirtualScreenCoord[ 0 ] = 0;
  this->VirtualScreenCoord[ 1 ] = 0;
  this->MonitorSizePixel[ 0 ]   = 0;
  this->MonitorSizePixel[ 1 ]   = 0;
  this->MonitorSizeMm[ 0 ]      = 0;
  this->MonitorSizeMm[ 1 ]      = 0;
  
  this->NumOfDepthPerceptionLines = 0;
  this->DepthPerceptionLinesIncrement = 10.0; // mm.
  
    // References to other objects.
  
  this->PerkStationModuleGUI = NULL;
  this->PerkStationModuleNodeID = NULL;
  
  
    // set up render window, renderer, actor, mapper
 
  this->Renderer = vtkSmartPointer< vtkRenderer >::New();
  
  vtkCamera *camera = vtkCamera::New();
  camera->SetParallelProjection( 1 );
  this->Renderer->SetActiveCamera(camera);
  camera->Delete();
  
  this->RenderWindow = vtkSmartPointer< vtkWin32OpenGLRenderWindow >::New();
  this->Interactor = vtkSmartPointer< vtkRenderWindowInteractor >::New();
  
  vtkSlicerInteractorStyle *iStyle = vtkSlicerInteractorStyle::New();
  this->Interactor->SetInteractorStyle( iStyle );
  iStyle->Delete();

  this->MapToWindowLevelColors = vtkSmartPointer< vtkImageMapToWindowLevelColors >::New();
    this->MapToWindowLevelColors->SetOutputFormatToLuminance();

  this->ImageMapper = vtkSmartPointer< vtkImageMapper >::New();
    this->ImageMapper->SetColorWindow( 255 );
    this->ImageMapper->SetColorLevel( 127.5 );
  
  this->ImageActor = vtkActor2D::New();
  this->EmptyImage = vtkSmartPointer< vtkImageData >::New();
  
  this->NeedleGuideActor = vtkSmartPointer< vtkActor >::New();
  
  this->NeedleTipActor = vtkSmartPointer< vtkActor >::New();
  this->RealTimeNeedleLineActor = vtkSmartPointer< vtkActor >::New();
  this->NeedleTipZLocationText = vtkSmartPointer< vtkTextActor >::New();

  this->DepthMeasureLineActor = vtkSmartPointer< vtkActor >::New();
    this->DepthMeasureLineActor->GetProperty()->SetColor( 0.9, 0.9, 1.0 );
  this->DepthMeasureTextActor = vtkSmartPointer< vtkTextActorFlippable >::New();
    this->DepthMeasureTextActor->GetTextProperty()->SetColor( 0.9, 0.9, 1.0 );
    this->DepthMeasureTextActor->GetTextProperty()->SetFontSize( 24 );
    this->DepthMeasureTextActor->SetTextScaleModeToNone();
    this->DepthMeasureTextActor->GetTextProperty()->BoldOn();
  
  this->DepthPerceptionLines = vtkSmartPointer< vtkActorCollection >::New();
  this->DepthPerceptionTexts = vtkSmartPointer< vtkCollection >::New();
  
  this->LeftSideActor = vtkSmartPointer< vtkTextActorFlippable >::New();
    this->LeftSideActor->SetInput( "L" );
    this->LeftSideActor->GetTextProperty()->SetColor( 1, 0.5, 0 );
    this->LeftSideActor->SetTextScaleModeToNone();
    this->LeftSideActor->GetTextProperty()->SetFontSize( 30 );
    this->LeftSideActor->GetTextProperty()->BoldOn();
    
  this->RightSideActor = vtkSmartPointer< vtkTextActorFlippable >::New();
    this->RightSideActor->SetInput( "R" );
    this->RightSideActor->GetTextProperty()->SetColor( 1, 0.5, 0 );
    this->RightSideActor->SetTextScaleModeToNone();
    this->RightSideActor->GetTextProperty()->SetFontSize( 30 );
    this->RightSideActor->GetTextProperty()->BoldOn();
  
  this->WorkflowActor = vtkSmartPointer< vtkTextActorFlippable >::New();
    this->WorkflowActor->SetInput( "Insertion planning in progress" );
    this->WorkflowActor->GetTextProperty()->SetColor( 1, 1, 1 );
    this->WorkflowActor->SetTextScaleModeToNone();
    this->WorkflowActor->GetTextProperty()->SetFontSize( 40 );
    this->WorkflowActor->GetTextProperty()->BoldOn();
  
  this->CalibrationControlsActor = vtkSmartPointer< vtkTextActorFlippable >::New();
  
  this->TablePositionActor = vtkSmartPointer< vtkTextActorFlippable >::New();
    this->TablePositionActor->GetTextProperty()->SetFontSize( 28 );
    this->TablePositionActor->GetTextProperty()->BoldOn();
    this->TablePositionActor->GetTextProperty()->SetColor( 1.0, 1.0, 0.2 );
    this->TablePositionActor->SetTextScaleModeToNone();
  
  this->CalibrationNameActor = vtkSmartPointer< vtkTextActorFlippable >::New();
    this->CalibrationNameActor->GetTextProperty()->SetFontSize( 24 );
    this->CalibrationNameActor->GetTextProperty()->BoldOn();
    this->CalibrationNameActor->GetTextProperty()->SetColor( 1.0, 0.9, 0.2 );
    this->CalibrationNameActor->SetTextScaleModeToNone();
  
  this->PlanNameActor = vtkSmartPointer< vtkTextActorFlippable >::New();
    this->PlanNameActor->GetTextProperty()->SetFontSize( 24 );
    this->PlanNameActor->GetTextProperty()->BoldOn();
    this->PlanNameActor->GetTextProperty()->SetColor( 1.0, 0.9, 0.2 );
    this->PlanNameActor->SetTextScaleModeToNone();
  
  
    // Image geometry.
  
  this->ImageData = NULL;
  this->SliceOffsetRAS = 0.0;
  
  this->SecMonFlipTransform = vtkSmartPointer< vtkTransform >::New();
  this->SecMonRotateTransform = vtkSmartPointer< vtkTransform >::New();
  this->SecMonTranslateTransform = vtkSmartPointer< vtkTransform >::New();
  
  this->ResliceFilter = vtkSmartPointer< vtkImageReslice >::New();
    // this->ResliceFilter->SetBackgroundColor( 0, 0, 0, 1 );
    this->ResliceFilter->SetOutputDimensionality( 2 );
    this->ResliceFilter->SetInterpolationModeToLinear();
  
  this->ResliceTransform = vtkSmartPointer< vtkTransform >::New();
  
  this->RASToIJK = vtkSmartPointer< vtkTransform >::New();
}



vtkPerkStationSecondaryMonitor
::~vtkPerkStationSecondaryMonitor()
{
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
    this->ImageActor->Delete();
    this->ImageActor = NULL;
    }

  
  if ( this->ImageData )
    {
    this->ImageData = NULL;  // Deleted when MRML scene is deleted.
    }
  
  
  if ( this->DepthPerceptionLines )
    {
    this->DepthPerceptionLines->RemoveAllItems();
    }
  
  if ( this->DepthPerceptionTexts )
    {
    this->DepthPerceptionTexts->RemoveAllItems();
    }
}



void
vtkPerkStationSecondaryMonitor
::RemoveOverlayRealTimeNeedleTip()
{
  if ( ! this->Initialized ) return;
  
  vtkActorCollection *collection = this->Renderer->GetActors();
  
  if ( collection->IsItemPresent( this->NeedleTipActor ) )
    {
    this->Renderer->RemoveActor( this->NeedleTipActor );
    }
  
  if ( collection->IsItemPresent( this->RealTimeNeedleLineActor ) )
    {
    this->Renderer->RemoveActor( this->RealTimeNeedleLineActor );
    }
  
  if ( collection->IsItemPresent( this->NeedleTipZLocationText ) )
    {
    this->Renderer->RemoveActor( this->NeedleTipZLocationText );
    }
}



void
vtkPerkStationSecondaryMonitor
::SetupImageData()
{
  this->ImageData = NULL;
  
  vtkMRMLPerkStationModuleNode* moduleNode = this->GetPerkStationModuleNode();
  if ( moduleNode == NULL )
    {
    vtkErrorMacro( "Tried to set up working volume without a module node." );
    return;
    }
  
  vtkMRMLScalarVolumeNode* volumeNode = moduleNode->GetActiveVolumeNode();
  if ( volumeNode == NULL )
    {
    vtkErrorMacro( "No active volume set in the module node." );
    return;
    }
  
  this->ImageData = volumeNode->GetImageData();
  
  int imageExtent[ 6 ];
  this->ImageData->GetExtent( imageExtent );

  this->ImageSizeVoxel[ 0 ] = imageExtent[ 1 ] - imageExtent[ 0 ] + 1;
  this->ImageSizeVoxel[ 1 ] = imageExtent[ 3 ] - imageExtent[ 2 ] + 1;
  this->ImageSizeVoxel[ 2 ] = 1;
  
  
    // Create an empty image of the same size, for when second monitor is not used.
  
  this->EmptyImage->ReleaseData();
  this->EmptyImage->SetExtent( this->ImageData->GetExtent() );
  this->EmptyImage->SetOrigin( this->ImageData->GetOrigin() );
  this->EmptyImage->SetSpacing( this->ImageData->GetSpacing() );
  this->EmptyImage->AllocateScalars();
  
  
    // Position, size, intensity window of second monitor window.
  
  if ( this->NumberOfMonitors == 2 )
    {
    this->RenderWindow->SetPosition( this->VirtualScreenCoord[ 0 ], this->VirtualScreenCoord[ 1 ] );
    }
  else
    {
    this->RenderWindow->SetPosition( 0, 0 );
    }
  
  this->RenderWindow->SetSize( this->MonitorSizePixel[ 0 ], this->MonitorSizePixel[ 1 ] );
  
  this->MapToWindowLevelColors->SetWindow( volumeNode->GetScalarVolumeDisplayNode()->GetWindow() );
  this->MapToWindowLevelColors->SetLevel( volumeNode->GetScalarVolumeDisplayNode()->GetLevel() );
  this->MapToWindowLevelColors->SetInput( this->ResliceFilter->GetOutput() );
    // Making the background black.
  this->ResliceFilter->SetBackgroundLevel( volumeNode->GetScalarVolumeDisplayNode()->GetLevel() -
                                           volumeNode->GetScalarVolumeDisplayNode()->GetWindow() / 2 );
  
  this->ImageMapper->SetInput( this->MapToWindowLevelColors->GetOutput() );
  
  // this->Renderer->RemoveAllViewProps(); // remove previous active actors
  // this->Renderer->AddActor( this->ImageActor ); // add new image actor
  
  
  this->LeftSideActor->SetDisplayPosition( this->MonitorSizePixel[ 0 ] - 50, this->MonitorSizePixel[ 1 ] - 50 );
  this->RightSideActor->SetDisplayPosition( 50, this->MonitorSizePixel[ 1 ] - 50 );
  
  
    // Calibration controls.
  
  this->CalibrationControlsActor->SetInput( "Up:[A]  Down:[Z]  Left:[Q]  Right:[W]   Rotatations:[J][H]  Fast: +SHIFT" );
  this->CalibrationControlsActor->GetTextProperty()->SetColor( 1, 1, 0 );
  this->CalibrationControlsActor->SetTextScaleModeToNone();
  this->CalibrationControlsActor->GetTextProperty()->SetFontSize( 28 );
  this->CalibrationControlsActor->GetTextProperty()->BoldOn();
  this->CalibrationControlsActor->SetDisplayPosition( this->MonitorSizePixel[ 0 ] / 2 - 400, this->MonitorSizePixel[ 1 ] - 50 );
  if ( moduleNode->GetFinalHorizontalFlip() )
    {
    this->CalibrationControlsActor->FlipAroundY( true );
    }
  if ( moduleNode->GetFinalVerticalFlip() )
    {
    this->CalibrationControlsActor->FlipAroundX( true );
    }
  
  // this->Renderer->AddActor( this->CalibrationControlsActor );
  
  
    // Set RASToIJK matrix, which only be updated at image load.
  
  vtkMatrix4x4* ijkToRAS = vtkMatrix4x4::New();
  volumeNode->GetIJKToRASMatrix( ijkToRAS );
  vtkMatrix4x4::Invert( ijkToRAS, this->RASToIJK->GetMatrix() );
  ijkToRAS->Delete();
  
  
  double spacing[ 3 ];
  volumeNode->GetSpacing( spacing ); // mm between pixels
  
  double MonitorPhysicalSizeMM[ 2 ];
  moduleNode->GetSecondMonitorPhysicalSize( MonitorPhysicalSizeMM );
  
    // pixel / mm.
  // double s0 = MonitorPhysicalSizeMM[ 0 ] / this->MonitorSizePixel[ 0 ];
  // double s1 = MonitorPhysicalSizeMM[ 1 ] / this->MonitorSizePixel[ 1 ];
  
  
  this->ImageLoaded = true;
  this->RenderWindow->Render(); // TODO: Do we need this here?
}



void
vtkPerkStationSecondaryMonitor
::GetMonitorSpacing( double& resolutionX, double& resolutionY )
{
  resolutionX = this->MonitorSizePixel[ 0 ] / this->MonitorSizeMm[ 0 ];
  resolutionY = this->MonitorSizePixel[ 1 ] / this->MonitorSizeMm[ 1 ];
}



vtkSmartPointer< vtkTransform >
vtkPerkStationSecondaryMonitor
::OverlayToRAS()
{
  vtkSmartPointer< vtkTransform > ret = vtkSmartPointer< vtkTransform >::New();
  
  vtkMRMLPerkStationModuleNode* moduleNode = this->GetPerkStationModuleNode();
  if ( moduleNode == NULL )
    {
    vtkErrorMacro( "Module node not set!" );
    return ret;
    }
  
  vtkMRMLScalarVolumeNode* volumeNode = moduleNode->GetActiveVolumeNode();
  if ( volumeNode == NULL )
    {
    vtkErrorMacro( "Active volume not found." );
    return ret;
    }
  
  vtkSmartPointer< vtkMatrix4x4 > ijkToRASMatrix = vtkSmartPointer< vtkMatrix4x4 >::New();
  volumeNode->GetIJKToRASMatrix( ijkToRASMatrix );
  
  vtkSmartPointer< vtkMatrix4x4 > overlayToRASMatrix = vtkSmartPointer< vtkMatrix4x4 >::New();
  vtkMatrix4x4::Multiply4x4( ijkToRASMatrix, this->OverlayToIJK()->GetMatrix(), overlayToRASMatrix );
  
  ret->GetMatrix()->DeepCopy( overlayToRASMatrix );
  
  return ret;
}


/**
 * Vritual display = mirrored image of the second monitor screen.
 *
 * The transformation computed here moves the virtual display to the IJK image
 * volume. This is mainly used to find the reslice plane to for the
 * vtkImageReslice transform, that extracts the image slice for the second
 * monitor.
 */
vtkSmartPointer< vtkTransform >
vtkPerkStationSecondaryMonitor::OverlayToIJK()
{
  vtkSmartPointer< vtkTransform > ret = vtkSmartPointer< vtkTransform >::New();
  
  // To make the implementation the shortest, we use Slicer's slice coordinate
  // system, which already contains the requested image slice. However, slice
  // coordinate system has some differences compared to Overlay:
  // * Origin is the center of the image slice.
  // * Coordinates are set for direct display, not mirrored.
  // * Units are mm.
  
    // Compute slice to IJK matrix.
  
  if ( this->PerkStationModuleGUI == NULL )
    {
    return ret;
    }
  
  vtkMRMLSliceNode* sliceNode = this->PerkStationModuleGUI->GetLogic()->
    GetApplicationLogic()->GetSliceLogic( "Red" )->GetSliceNode();
  
  vtkSmartPointer< vtkMatrix4x4 > sliceToRAS = sliceNode->GetSliceToRAS();
  
  
  /*
  // debug.
  std::stringstream sss;
  sss << sliceToRAS->GetElement( 0, 3 ) << " "
     << sliceToRAS->GetElement( 1, 3 ) << " "
     << sliceToRAS->GetElement( 2, 3 ) << std::endl;
  LOG_TO_FILE( sss.str() );
  */
  
  
  vtkSmartPointer< vtkMatrix4x4 > rasToIJK = vtkSmartPointer< vtkMatrix4x4 >::New();
  vtkMRMLScalarVolumeNode* volumeNode = this->GetActiveVolumeNode();
  if ( volumeNode != NULL )
    {
    volumeNode->GetRASToIJKMatrix( rasToIJK );
    }
  else
    {
    return ret;
    }
  
  vtkSmartPointer< vtkMatrix4x4 > sliceToIJK = vtkSmartPointer< vtkMatrix4x4 >::New();
  vtkMatrix4x4::Multiply4x4( rasToIJK, sliceToRAS, sliceToIJK );
  
  
    // Set this as our starting transform.
  
  
    // Make X and Y translation independent from the Slicer slice widget position.
  int* imageSizePixels = this->ImageData->GetDimensions();
  ret->SetMatrix( sliceToIJK );
  ret->GetMatrix()->SetElement( 0, 3, imageSizePixels[ 0 ] / 2.0 );
  ret->GetMatrix()->SetElement( 1, 3, imageSizePixels[ 1 ] / 2.0 );
  
  
  /*
    // debug.
  double _sliceToIJK_s = sliceToIJK->GetElement( 2, 3 ); // debug.
  int* dim = this->ImageData->GetDimensions();
  double* ori = this->ImageData->GetOrigin();
  double* spa = this->ImageData->GetSpacing();
  std::stringstream ss;
  ss << "slice RAS  = " << this->SliceOffsetRAS << std::endl;
  ss << "slice IJK  = " << this->SliceOffsetIJK << std::endl;
  ss << "Slice2IJK  = " << _sliceToIJK_s << std::endl;
  ss << "image dims = " << dim[ 0 ] << " " << dim[ 1 ] << " " << dim[ 2 ] << std::endl;
  ss << "image orig = " << ori[ 0 ] << " " << ori[ 1 ] << " " << ori[ 2 ] << std::endl;
  ss << "image spac = " << spa[ 0 ] << " " << spa[ 1 ] << " " << spa[ 2 ] << std::endl;
  LOG_TO_FILE( ss.str() );
  */
  
  
    // Apply the following transforms, that implement the XYToSlice transform.
    // * Move the origin to the center of the virtual display.
    // * Transform units from pixels to mm, and flip image at the same time.
    // * Rotate around origin, according to manual calibraion.
    // * Move the origin, accoring to manual calibration.
  
  double hFlipFactor = 1.0;
  double vFlipFactor = 1.0;
  if ( this->GetPerkStationModuleNode()->GetFinalHorizontalFlip() ) hFlipFactor = - 1.0;
  if ( this->GetPerkStationModuleNode()->GetFinalVerticalFlip() ) vFlipFactor = - 1.0;
  
  
    // Compute scaling factors.
  
  double s0 = this->GetPerkStationModuleNode()->
    GetHardwareList()[ this->GetPerkStationModuleNode()->GetHardwareIndex() ].SizeX / this->MonitorSizePixel[ 0 ];
  double s1 = this->GetPerkStationModuleNode()->
    GetHardwareList()[ this->GetPerkStationModuleNode()->GetHardwareIndex() ].SizeY / this->MonitorSizePixel[ 1 ];
  
    
    // Compute translation.
  
  double translation[ 2 ];
  this->GetPerkStationModuleNode()->GetSecondMonitorTranslation( translation );
  
  
    // We are in pre-multiply mode, so write transforms in reverse order.
  
  ret->Translate( translation[ 0 ], translation[ 1 ], 0.0 );
  ret->RotateZ( this->GetPerkStationModuleNode()->GetSecondMonitorRotation() );
  ret->Scale( s0 * hFlipFactor, s1 * vFlipFactor, 1.0 );
  ret->Translate( - this->MonitorSizePixel[ 0 ] / 2.0,  - this->MonitorSizePixel[ 1 ] / 2.0,  0.0 );
  
  return ret;
}



void
vtkPerkStationSecondaryMonitor
::SetPerkStationModuleGUI( vtkPerkStationModuleGUI* gui )
{
  this->PerkStationModuleGUI = gui;
}



vtkMRMLPerkStationModuleNode*
vtkPerkStationSecondaryMonitor
::GetPerkStationModuleNode()
{
  vtkMRMLPerkStationModuleNode* node = NULL;
  if ( this->PerkStationModuleGUI->GetMRMLScene() && this->PerkStationModuleNodeID != NULL )
    {
    vtkMRMLNode* snode = this->PerkStationModuleGUI->GetMRMLScene()->GetNodeByID( this->PerkStationModuleNodeID );
    node = vtkMRMLPerkStationModuleNode::SafeDownCast( snode );
    }
  return node;
}



vtkMRMLScalarVolumeNode*
vtkPerkStationSecondaryMonitor
::GetActiveVolumeNode()
{
  vtkMRMLPerkStationModuleNode* moduleNode = this->GetPerkStationModuleNode();
  if ( moduleNode == NULL )
    {
    return NULL;
    }
  vtkMRMLScalarVolumeNode* volumeNode = moduleNode->GetActiveVolumeNode();
  return volumeNode;
}



/**
 * This function controls what appears on the sencond monitor screen.
 */
void
vtkPerkStationSecondaryMonitor
::UpdateImageDisplay()
{
  bool displayEmpty = false;
  this->UpdateVisibilities( displayEmpty );  // Updates the visibility of most actors.
  
  if ( ! this->PerkStationModuleGUI->GetPerkStationModuleNode() ) return;
  if ( ! this->Initialized ) return;
  if ( ! this->ImageLoaded ) return;
  
  
  if ( displayEmpty )  // Nothing else to be displayed.
    {
    if ( this->DeviceActive ) this->RenderWindow->Render();  // Render before return.
    return;
    }
  
  
    // Update the current slice offset value.
  
  this->SliceOffsetRAS = this->PerkStationModuleGUI->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetLogic()->GetSliceOffset();
  
  
    // Switch visibility of needle guide and depth perception lines.
    // Show them only in planning plane +/- 0.5 mm.
    
  double entry[ 3 ] = { 0, 0, -100000 };
  double target[ 3 ] = { 0, 0, -100000 };
  
  this->PerkStationModuleGUI->GetPerkStationModuleNode()->GetPlanEntryPoint( entry );
  this->PerkStationModuleGUI->GetPerkStationModuleNode()->GetPlanTargetPoint( target );
  
  double minOffset = -100000;
  double maxOffset = -100000;
  
  if ( entry != NULL && target != NULL )
    {
    minOffset = target[ 2 ] - 0.5;
    maxOffset = entry[ 2 ] + 0.5;
    if ( entry[ 2 ] < target[ 2 ] )
      {
      minOffset = entry[ 2 ] - 0.5;
      maxOffset = target[ 2 ] + 0.5;
      }
    }
  
  
  if (
          ( this->SliceOffsetRAS < maxOffset )
       && ( this->SliceOffsetRAS >= minOffset )
       && ( this->PerkStationModuleGUI->GetPerkStationModuleNode()->GetCurrentStep() == 2 )
       )
    {
    this->OverlayNeedleGuide();  // TODO: Need for recomputation should be checked!
    this->CreateDepthPerceptionLines();
    this->ShowDepthPerceptionLines( true );
    }
  else
    {
    this->NeedleGuideActor->SetVisibility( 0 );
    this->RemoveDepthPerceptionLines();
    this->ShowDepthPerceptionLines( false );
    }
 
 
    // Update value of table position, calibration name and plan name text actors.
   
  this->TablePositionActor->SetDisplayPosition( this->MonitorSizePixel[ 0 ] / 3.0 - 150, this->MonitorSizePixel[ 1 ] - 50 );
  std::stringstream ss;
  ss.setf( std::ios::fixed );
  ss << "Table position: " << std::setprecision( 1 )
     << this->PerkStationModuleGUI->GetPerkStationModuleNode()->GetCurrentTablePosition() << " mm";
  this->TablePositionActor->SetInput( ss.str().c_str() );
  
  this->CalibrationNameActor->SetDisplayPosition( this->MonitorSizePixel[ 0 ] / 3.0 - 150, this->MonitorSizePixel[ 1 ] - 80 );
  ss.str( "" );
  ss << "Calibration: ";
  int currentCalibration = this->PerkStationModuleGUI->GetPerkStationModuleNode()->GetCurrentCalibration();
  if ( currentCalibration >= 0 )
    {
    ss << this->PerkStationModuleGUI->GetPerkStationModuleNode()->GetCalibrationAtIndex( currentCalibration )->Name;
    }
  this->CalibrationNameActor->SetInput( ss.str().c_str() );
  
  int currentPlanIndex = this->PerkStationModuleGUI->GetPerkStationModuleNode()->GetCurrentPlanIndex();
  if ( currentPlanIndex >= 0 )
    {
    this->PlanNameActor->SetDisplayPosition( this->MonitorSizePixel[ 0 ] / 3.0 - 150, this->MonitorSizePixel[ 1 ] - 110 );
    ss.str( "" );
    ss << "Plan: ";
    ss << this->PerkStationModuleGUI->GetPerkStationModuleNode()->GetPlanAtIndex( currentPlanIndex )->GetName();
    this->PlanNameActor->SetInput( ss.str().c_str() );
    }
  
 
   // Position L/R side indicator and calibration guide texts. ----------------
 
  bool Lup = false;
  bool Lleft = false;
  bool Rup = false;
  bool Rleft = false;
 
   // Take patient position into account.
 
 switch ( this->PerkStationModuleGUI->GetPerkStationModuleNode()->GetPatientPosition() )
   {
   case HFP :  Lleft = true;  Lup = true;  Rleft = false; Rup = true;   break;
   case HFS :  Lleft = false; Lup = true;  Rleft = true;  Rup = true;   break;
   case HFDR : Lleft = true;  Lup = true;  Rleft = true;  Rup = false;  break;
   case HFDL : Lleft = true;  Lup = false; Rleft = true;  Rup = true;   break;
   case FFDR : Lleft = true;  Lup = true;  Rleft = true;  Rup = false;  break;
   case FFDL : Lleft = true;  Lup = false; Rleft = true;  Rup = true;   break;
   case FFP :  Lleft = false; Lup = true;  Rleft = true;  Rup = true;   break;
   case FFS :  Lleft = true;  Lup = true;  Rleft = false; Rup = true;   break;
   }
 
 
    // Take hardware into account.
  
  if ( this->GetPerkStationModuleNode()->GetFinalHorizontalFlip() )
    {
    Lleft = ! Lleft;
    Rleft = ! Rleft;
    this->LeftSideActor->FlipAroundY( true );
    this->RightSideActor->FlipAroundY( true );
    this->WorkflowActor->FlipAroundY( true );
    this->CalibrationControlsActor->FlipAroundY( true );
    this->TablePositionActor->FlipAroundY( true );
    this->CalibrationNameActor->FlipAroundY( true );
    this->PlanNameActor->FlipAroundY( true );
    this->DepthMeasureTextActor->FlipAroundY( true );
    }
  else
    {
    this->LeftSideActor->FlipAroundY( false );
    this->RightSideActor->FlipAroundY( false );
    this->WorkflowActor->FlipAroundY( false );
    this->CalibrationControlsActor->FlipAroundY( false );
    this->TablePositionActor->FlipAroundY( false );
    this->CalibrationNameActor->FlipAroundY( false );
    this->PlanNameActor->FlipAroundY( false );
    this->DepthMeasureTextActor->FlipAroundY( false );
    }

  if (    this->GetPerkStationModuleNode()->GetFinalVerticalFlip() )
    {
    this->LeftSideActor->FlipAroundX( true );
    this->RightSideActor->FlipAroundX( true );
    this->WorkflowActor->FlipAroundX( true );
    this->CalibrationControlsActor->FlipAroundX( true );
    this->TablePositionActor->FlipAroundX( true );
    this->CalibrationNameActor->FlipAroundX( true );
    this->PlanNameActor->FlipAroundX( true );
    this->DepthMeasureTextActor->FlipAroundX( true );
    }
  else
    {
    this->LeftSideActor->FlipAroundX( false );
    this->RightSideActor->FlipAroundX( false );
    this->WorkflowActor->FlipAroundX( false );
    this->CalibrationControlsActor->FlipAroundX( false );
    this->TablePositionActor->FlipAroundX( false );
    this->CalibrationNameActor->FlipAroundX( false );
    this->PlanNameActor->FlipAroundX( false );
    this->DepthMeasureTextActor->FlipAroundX( false );
    }
  
  
    // Position the L-R actors.
  
  this->LeftSideActor->SetDisplayPosition( this->UpperLeftCorner[ 0 ], this->UpperLeftCorner[ 1 ] );
  if ( ! Lleft && Lup ) this->LeftSideActor->SetDisplayPosition( this->UpperRightCorner[ 0 ], this->UpperRightCorner[ 1 ] );
  if ( ! Lleft && ! Lup ) this->LeftSideActor->SetDisplayPosition( this->LowerRightCorner[ 0 ], this->LowerRightCorner[ 1 ] );
  if ( Lleft && ! Lup ) this->LeftSideActor->SetDisplayPosition( this->LowerLeftCorner[ 0 ], this->LowerLeftCorner[ 1 ] );
  
  
  this->RightSideActor->SetDisplayPosition( this->UpperLeftCorner[ 0 ], this->UpperLeftCorner[ 1 ] );
  if ( ! Rleft && Rup ) this->RightSideActor->SetDisplayPosition( this->UpperRightCorner[ 0 ], this->UpperRightCorner[ 1 ] );
  if ( ! Rleft && ! Rup ) this->RightSideActor->SetDisplayPosition( this->LowerRightCorner[ 0 ], this->LowerRightCorner[ 1 ] );
  if ( Rleft && ! Rup ) this->RightSideActor->SetDisplayPosition( this->LowerLeftCorner[ 0 ], this->LowerLeftCorner[ 1 ] );
  
  
    // Extract slice from the image volume. -------------------------------
  
    // It is not possible to apply vtkTransformFilter on an image data.
    // So all transformations has to be applied in ResliceFilter.
    // Currently, transformations of the overlayed image is done only in
    // XYToIJK function.
  
  this->ResliceTransform->SetMatrix( this->OverlayToIJK()->GetMatrix() );
  this->ResliceTransform->Update();
  
  this->ResliceFilter->SetOutputExtent( 0, this->MonitorSizePixel[ 0 ] - 1, 0, this->MonitorSizePixel[ 1 ] - 1,0, 0 );
  this->ResliceFilter->SetOutputSpacing( 1.0, 1.0, 1.0 );
  this->ResliceFilter->SetOutputOrigin( 0.0, 0.0, 0.0 );
  this->ResliceFilter->SetInput( this->ImageData );
  this->ResliceFilter->SetResliceTransform( this->ResliceTransform );
  
  
    // Adjust color window / level.
  
  if ( this->GetActiveVolumeNode() != NULL )
    {
    this->MapToWindowLevelColors->SetWindow( this->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->GetWindow() );
    this->MapToWindowLevelColors->SetLevel( this->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->GetLevel() );
    this->MapToWindowLevelColors->SetInput( this->ResliceFilter->GetOutput() );
    this->MapToWindowLevelColors->Update();
    }
  
  
  this->ImageMapper->SetInput( this->MapToWindowLevelColors->GetOutput() );
  
  
    // Finally, render the window.
  
  if ( this->DeviceActive )
    {
    this->RenderWindow->Render();  
    }
}



void
vtkPerkStationSecondaryMonitor
::UpdateImageDataOnSliceOffset( double rasOffset )
{
  this->SliceOffsetRAS = rasOffset;
  
  vtkMRMLPerkStationModuleNode *mrmlNode = this->PerkStationModuleGUI->GetPerkStationModuleNode();
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
  vtkPerkStationSecondaryMonitor *self =
    (vtkPerkStationSecondaryMonitor *)dwData;
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
          // This is where we have to draw the second monitor content.
        int virtualScreenCoord[ 2 ] = { mi.rcMonitor.left, mi.rcMonitor.top };
        self->SetVirtualScreenCoord( virtualScreenCoord );
        }
      }
    
    }
  return 1;
}



void
vtkPerkStationSecondaryMonitor
::Initialize()
{
    // first make sure secondary monitor is connected
  this->NumberOfMonitors = GetSystemMetrics( SM_CMONITORS );
  
  
    // For testing, in case of one physical monitor, create the second monitor
    // in a separate window on the first monitor.
  
  if ( this->NumberOfMonitors != 2 )
    {
    this->MonitorSizePixel[ 0 ] = 1024;
    this->MonitorSizePixel[ 1 ] = 768;
    this->DeviceActive = true;
    }
  else
    {
      // get info about the monitors
    
    EnumDisplayMonitors( NULL, NULL, MyInfoEnumProc, ( LPARAM ) this );
    
    DISPLAY_DEVICE lpDisplayDevice;
    lpDisplayDevice.cb = sizeof( lpDisplayDevice );
    lpDisplayDevice.StateFlags = DISPLAY_DEVICE_ATTACHED_TO_DESKTOP;
    int iDevNum = 0;
    int iPhyDevNum = 0;
    DWORD dwFlags = 0;

    while( EnumDisplayDevices( NULL, iDevNum, &lpDisplayDevice, dwFlags ) )
      {
      iDevNum++;
      if( ( lpDisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) != 1 )
        {
        continue;
        }
      
      iPhyDevNum++;
      
        // get the device context for the monitor
      HDC hdc = CreateDC( "DISPLAY", lpDisplayDevice.DeviceName, NULL, NULL );    
      
        // now the device context can be used in many functions to retrieve information about the monitor
      
        // TODO: Compare physical size with calibration, maybe give warning on difference.
      
      double width_mm = GetDeviceCaps( hdc, HORZSIZE );
      double height_mm = GetDeviceCaps( hdc, VERTSIZE );
      
      double width_pix = GetDeviceCaps( hdc, HORZRES );
      double height_pix = GetDeviceCaps( hdc, VERTRES );
      
      if ( ! ( lpDisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) )
        {
        this->MonitorSizePixel[ 0 ] = width_pix;
        this->MonitorSizePixel[ 1 ] = height_pix;
        this->DeviceActive = true;
        }
      
      DeleteDC( hdc );
      }
    }
  
  this->UpdateCornerPositions();
  
  // The physical parameters of the second monitor are theoretically known
  // at this point, but are they often not accurate. So we don't use
  // physical size values, but ask the user to select the second monitor from a
  // drop-down list.
  
  
    // Initialize the visualization pipeline for the second monitor.
  
  this->RenderWindow->AddRenderer( this->Renderer );
  this->RenderWindow->SetBorders( 0 );
  
  this->Interactor->SetRenderWindow( this->RenderWindow );
  
  this->ImageActor->SetMapper( this->ImageMapper );
  this->ImageActor->GetProperty()->SetDisplayLocationToBackground();
  
  this->UpdateCornerPositions();
  
  this->WorkflowActor->SetDisplayPosition( this->MonitorSizePixel[ 0 ] / 4, this->MonitorSizePixel[ 1 ] / 2 );
  
    // Add actors in alphabetical order.
  
  this->Renderer->AddActor( this->CalibrationControlsActor );
  this->Renderer->AddActor( this->CalibrationNameActor );
  this->Renderer->AddActor( this->DepthMeasureLineActor );
  this->Renderer->AddActor( this->DepthMeasureTextActor );
  this->Renderer->AddActor( this->ImageActor );
  this->Renderer->AddActor( this->LeftSideActor );
  this->Renderer->AddActor( this->NeedleGuideActor );
  this->Renderer->AddActor( this->NeedleTipActor );
  this->Renderer->AddActor( this->NeedleTipZLocationText );
  this->Renderer->AddActor( this->PlanNameActor );
  this->Renderer->AddActor( this->RealTimeNeedleLineActor);
  this->Renderer->AddActor( this->RightSideActor );
  this->Renderer->AddActor( this->TablePositionActor );
  this->Renderer->AddActor( this->WorkflowActor );
  
  
  this->Initialized = true;
}



void
vtkPerkStationSecondaryMonitor
::OverlayRealTimeNeedleTip( double tipRAS[ 3 ], vtkMatrix4x4* toolTransformMatrix )
{
  vtkSmartPointer< vtkMatrix4x4 > rasToOverlay = vtkSmartPointer< vtkMatrix4x4 >::New();
  vtkMatrix4x4::Invert( this->OverlayToRAS()->GetMatrix(), rasToOverlay );
  
  
  double tipPointRAS[ 4 ] = { tipRAS[ 0 ], tipRAS[ 1 ], tipRAS[ 2 ], 1 };
  double tipPointOverlay[ 4 ] = { 0, 0, 0, 1 };  
  rasToOverlay->MultiplyPoint( tipPointRAS, tipPointOverlay );
  
  double tipPointWorld[ 4 ] = { 0, 0, 0, 1 };
  this->PointOverlayToRenderer( tipPointOverlay, tipPointWorld );
  
  
  // This code has been deleted.
  // Real time needle display has been disabled in a previous version.
  // TODO: Implement real time needle display.
}



void
vtkPerkStationSecondaryMonitor
::OverlayNeedleGuide()
{ 
  if ( ! this->DeviceActive ) return;
  
  
    // Create transform matrix for RAS to Overlay transform.
  
  vtkSmartPointer< vtkMatrix4x4 > rasToOverlay = vtkSmartPointer< vtkMatrix4x4 >::New();
  vtkMatrix4x4::Invert( this->OverlayToRAS()->GetMatrix(), rasToOverlay );
  
  double entryRAS[ 4 ] = { 0, 0, 0, 1 };
  double targetRAS[ 4 ] = { 0, 0, 0, 1 };
  this->PerkStationModuleGUI->GetPerkStationModuleNode()->GetPlanEntryPoint( entryRAS );
  this->PerkStationModuleGUI->GetPerkStationModuleNode()->GetPlanTargetPoint( targetRAS );
  
  double entryOverlay[ 4 ] = { 0, 0, 0, 1 };
  double targetOverlay[ 4 ] = { 0, 0, 0, 1 };
  rasToOverlay->MultiplyPoint( entryRAS, entryOverlay );
  rasToOverlay->MultiplyPoint( targetRAS, targetOverlay );
  
  double entryRenderer[ 4 ] = { 0, 0, 0, 1 };
  double targetRenderer[ 4 ] = { 0, 0, 0, 1 };
  this->PointOverlayToRenderer( entryOverlay, entryRenderer );
  this->PointOverlayToRenderer( targetOverlay, targetRenderer );
  
  
  double halfNeedleGuideLength =
    sqrt(   ( targetRenderer[ 0 ] - entryRenderer[ 0 ] ) * ( targetRenderer[ 0 ] - entryRenderer[ 0 ] )
          + ( targetRenderer[ 1 ] - entryRenderer[ 1 ] ) * ( targetRenderer[ 1 ] - entryRenderer[ 1 ] ) );
  
  
  vtkSmartPointer< vtkCylinderSource > needleSource = vtkSmartPointer< vtkCylinderSource >::New();
    needleSource->SetHeight( 2 * halfNeedleGuideLength ); // TO DO: how to relate this to actual depth?
    needleSource->SetRadius( 0.009 );  
    needleSource->SetResolution( 10 );


    // Insertion angle as calculated from Overlay coordinates
  
  double insAngle = - double( 180 / vtkMath::Pi() ) *
    atan( double( ( entryOverlay[ 1 ] - targetOverlay[ 1 ] ) / ( entryOverlay[ 0 ] - targetOverlay[ 0 ] ) ) );
  
  vtkSmartPointer< vtkTransform > needleTransform = vtkSmartPointer< vtkTransform >::New();
    // needleTransform->Translate( entryRenderer[ 0 ], entryRenderer[ 1 ], entryRenderer[ 2 ] );  // Center at enry point.
    needleTransform->RotateZ( 90.0 - insAngle );
  
  vtkSmartPointer< vtkTransformPolyDataFilter > filter = vtkSmartPointer< vtkTransformPolyDataFilter >::New();
    filter->SetInput( needleSource->GetOutput() ); 
    filter->SetTransform( needleTransform );

  vtkSmartPointer< vtkPolyDataMapper > needleMapper = vtkSmartPointer< vtkPolyDataMapper >::New();
    // needleMapper->SetInput( filter->GetOutput() );
    needleMapper->SetInput( needleSource->GetOutput() );
    needleMapper->Update();
  
  this->NeedleGuideActor->SetPosition( entryRenderer );
  this->NeedleGuideActor->SetOrientation( 0, 0, 90.0 - insAngle );
  this->NeedleGuideActor->SetMapper( needleMapper );
  this->NeedleGuideActor->SetVisibility( 1 );
  
}



void
vtkPerkStationSecondaryMonitor
::CreateDepthPerceptionLines()
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->PerkStationModuleGUI->GetPerkStationModuleNode();
  if ( ! mrmlNode ) return;
  
  this->RemoveDepthPerceptionLines();
  
  
    // Depth is defined as distance between entry point and target point.
  
  double insertionDepth = mrmlNode->GetActualPlanInsertionDepth();
  if ( insertionDepth <= 0 ) return;
  
  
  this->NumOfDepthPerceptionLines = floor( insertionDepth / this->DepthPerceptionLinesIncrement );
  this->NumOfDepthPerceptionLines ++;  // First line at zero.
  
  
    // Store at what ratios of depth should there be a depth perception line.
  
  std::vector< double > lineDivisionRatios;
  for ( unsigned int i = 0; i < this->NumOfDepthPerceptionLines; ++ i )
    {
    double ratio = double( i * this->DepthPerceptionLinesIncrement ) / insertionDepth;
    lineDivisionRatios.push_back( ratio );
    }
  lineDivisionRatios.push_back( 1.0 );  // Last line is at the target level.
  
  
    // Create a RAS to Overlay matrix.
  
  vtkSmartPointer< vtkMatrix4x4 > mRASToOverlay = vtkSmartPointer< vtkMatrix4x4 >::New();
  vtkMatrix4x4::Invert( this->OverlayToRAS()->GetMatrix(), mRASToOverlay );
  
  
  double entryRAS[ 4 ] = { 0, 0, 0, 1 };
  double targetRAS[ 4 ] = { 0, 0, 0, 1 };
  
  double entryPlan[ 3 ];
  double targetPlan[ 3 ];
  mrmlNode->GetPlanTargetPoint( targetPlan );
  mrmlNode->GetPlanEntryPoint( entryPlan );
  for ( int i = 0; i < 3; ++ i )
    {
    entryRAS[ i ] = entryPlan[ i ];
    targetRAS[ i ] = targetPlan[ i ];
    }
  
  
  double entryOverlay[ 4 ] = { 0, 0, 0, 1 };
  double targetOverlay[ 4 ] = { 0, 0, 0, 1 };
  
  mRASToOverlay->MultiplyPoint( entryRAS, entryOverlay );
  mRASToOverlay->MultiplyPoint( targetRAS, targetOverlay );
  
  
  double entryRenderer[ 4 ] = { 0, 0, 0, 1 };
  double targetRenderer[ 4 ] = { 0, 0, 0, 1 };
  
  this->PointOverlayToRenderer( entryOverlay, entryRenderer );
  this->PointOverlayToRenderer( targetOverlay, targetRenderer );
  
  
    // Determine if lines go to the right or to the left.
    // Line point on the edge of the screen.
  
  double edgePointOverlay[ 4 ] = { 0, 0, 0, 1 };
  if ( targetOverlay[ 0 ] > this->MonitorSizePixel[ 0 ] / 2.0 )
    {
    edgePointOverlay[ 0 ] = this->MonitorSizePixel[ 0 ] - 1;
    }
  
  double edgePointRenderer[ 4 ] = { 0, 0, 0, 1 };
  this->PointOverlayToRenderer( edgePointOverlay, edgePointRenderer );
  
  
    // These differences will be added to the entry point with different weights
    // to get positions of the depth perception line series.
  
  double diffOverlay[ 3 ] = { 0, 0, 0 };
  double diffRenderer[ 3 ] = { 0, 0, 0 };
  
  for ( int i = 0; i < 3; ++ i )
    {
    diffOverlay[ i ] = targetOverlay[ i ] - entryOverlay[ i ];
    diffRenderer[ i ] = targetRenderer[ i ] - entryRenderer[ i ];
    }
  
  
    // Create depth perception lines and text one in each loop.
  
  for ( unsigned int lineIndex = 0; lineIndex < lineDivisionRatios.size(); ++ lineIndex )
    {
    
    double point1Renderer[ 3 ] = { entryRenderer[ 0 ] + diffRenderer[ 0 ] * lineDivisionRatios[ lineIndex ],
                                   entryRenderer[ 1 ] + diffRenderer[ 1 ] * lineDivisionRatios[ lineIndex ],
                                   entryRenderer[ 2 ] };
    
    double point2Renderer[ 3 ] = { edgePointRenderer[ 0 ], point1Renderer[ 1 ], targetRenderer[ 2 ] };
    
    
      // Create line, and add to the line collection.
    
    vtkSmartPointer< vtkLineSource > line = vtkSmartPointer< vtkLineSource >::New();
      line->SetPoint1( point1Renderer );
      line->SetPoint2( point2Renderer );
    
    vtkSmartPointer< vtkPolyDataMapper > lineMapper = vtkSmartPointer< vtkPolyDataMapper >::New();
      lineMapper->SetInputConnection( line->GetOutputPort() );
    
    vtkSmartPointer< vtkActor > lineActor = vtkSmartPointer< vtkActor >::New();
      lineActor->SetMapper( lineMapper );
      lineActor->GetProperty()->SetColor( 0.2, 1.0, 0.2 );
    
    this->DepthPerceptionLines->AddItem( lineActor );
    this->Renderer->AddActor( lineActor );
    
    
      // Create text and add to the text collection.
    
    std::stringstream ss;
    ss << int( lineIndex * this->DepthPerceptionLinesIncrement ) << " mm";
  
    vtkSmartPointer< vtkTextActorFlippable > textActor = vtkSmartPointer< vtkTextActorFlippable >::New();
      textActor->SetInput( ss.str().c_str() );
      textActor->GetTextProperty()->SetColor( 0.2, 1.0, 0.2 );
      textActor->SetTextScaleModeToNone();
      textActor->GetTextProperty()->SetFontSize( 26 );
      textActor->GetTextProperty()->BoldOn();
    if ( this->GetPerkStationModuleNode()->GetFinalHorizontalFlip() ) textActor->FlipAroundY( true );
    if ( this->GetPerkStationModuleNode()->GetFinalVerticalFlip() ) textActor->FlipAroundX( true );
      
    
      // Determine and set text position.
    
    double point1Overlay[ 3 ] = { entryOverlay[ 0 ] + diffOverlay[ 0 ] * lineDivisionRatios[ lineIndex ],
                                  entryOverlay[ 1 ] + diffOverlay[ 1 ] * lineDivisionRatios[ lineIndex ],
                                  entryOverlay[ 2 ] };
    
    double point2Overlay[ 3 ] = { edgePointOverlay[ 0 ], point1Overlay[ 1 ], targetOverlay[ 2 ] };
    
    if ( edgePointOverlay[ 0 ] == 0.0 )
      {
      textActor->SetDisplayPosition( point2Overlay[ 0 ] + 30, point2Overlay[ 1 ] + 2 );
      }
    else
      {
      textActor->SetDisplayPosition( point2Overlay[ 0 ] - 120, point2Overlay[ 1 ] + 2 );
      }
    
    
      // Take reflection flips into account.
    
    if ( this->GetPerkStationModuleNode()->GetSecondMonitorVerticalFlip() )
      {
      textActor->GetTextProperty()->SetVerticalJustificationToTop();
      }
    
    
      // Add text to collection and renderer.
    
    if ( lineIndex < lineDivisionRatios.size() - 1 )  // Don't write on the last line.
      {
      this->DepthPerceptionTexts->AddItem( textActor );
      this->Renderer->AddActor( textActor );
      }
    }
  
  
    // Set up the needle depth measure line.
  
  double depthMeasureOverlay[ 4 ] = { 130, this->MonitorSizePixel[ 1 ] * 0.75, 0.0, 1.0 };
  if ( edgePointOverlay[ 0 ] == 0.0 )
    {
    depthMeasureOverlay[ 0 ] = this->MonitorSizePixel[ 0 ] - 130;
    }
  double depthMeasureRenderer[ 4 ] = { 0, 0, 0, 1 };
  this->PointOverlayToRenderer( depthMeasureOverlay, depthMeasureRenderer );
  
  
    // Compute needle depth in Renderer coordinate system.
  
  double sumsq = 0;
  for ( int i = 0; i < 3; ++ i ) sumsq += ( ( targetRenderer[ i ] - entryRenderer[ i ] ) * ( targetRenderer[ i ] - entryRenderer[ i ] ) );
  double depthRenderer = sqrt( sumsq );
  
    // Set up the needle measure.
  
  vtkSmartPointer< vtkCylinderSource > needleMeasure = vtkSmartPointer< vtkCylinderSource >::New();    
    needleMeasure->SetHeight( depthRenderer ); 
    needleMeasure->SetRadius( 0.008 );  
    needleMeasure->SetResolution( 10 );

  vtkSmartPointer< vtkPolyDataMapper > measureMapper = vtkSmartPointer< vtkPolyDataMapper >::New();  
    measureMapper->SetInputConnection( needleMeasure->GetOutputPort() );

  this->DepthMeasureLineActor->SetMapper( measureMapper );
  this->DepthMeasureLineActor->SetPosition( depthMeasureRenderer[ 0 ], depthMeasureRenderer[ 1 ], depthMeasureRenderer[ 2 ] );
  
    // Measure with numbers.
  
  this->DepthMeasureTextActor->SetPosition( depthMeasureOverlay[ 0 ] + 10, depthMeasureOverlay[ 1 ] + 50 );
  std::stringstream ss1;
  ss1.setf( std::ios::fixed );
  ss1 << std::setprecision( 1 ) << insertionDepth << " mm";
  this->DepthMeasureTextActor->SetInput( ss1.str().c_str() );
}



void
vtkPerkStationSecondaryMonitor
::RemoveDepthPerceptionLines()
{
  vtkActorCollection *collection = this->Renderer->GetActors();
  
  for( int i = 0; i < this->DepthPerceptionLines->GetNumberOfItems(); i++ )
    {
    vtkActor *lineActor = vtkActor::SafeDownCast( this->DepthPerceptionLines->GetItemAsObject( i ) );
    if ( collection->IsItemPresent( lineActor ) )
      {
      this->Renderer->RemoveActor( lineActor );
      }   
    }
  
  for ( int i = 0; i < this->DepthPerceptionTexts->GetNumberOfItems(); ++ i )
    {
    vtkTextActorFlippable* textActor = vtkTextActorFlippable::SafeDownCast( this->DepthPerceptionTexts->GetItemAsObject( i ) );
    this->Renderer->RemoveActor( textActor );
    if ( collection->IsItemPresent( textActor ) )
      {
      this->Renderer->RemoveActor( textActor );
      }
    }
  
  this->DepthPerceptionTexts->RemoveAllItems();
  this->DepthPerceptionLines->RemoveAllItems();
}



void
vtkPerkStationSecondaryMonitor
::SetRealTimeNeedleLineActorVisibility( bool v )
{
  this->RealTimeNeedleLineActor->SetVisibility(v);
  this->NeedleTipActor->SetVisibility(v);
  this->RenderWindow->Render();
}



void
vtkPerkStationSecondaryMonitor
::UpdateCornerPositions()
{
  this->UpperLeftCorner[ 0 ] = 40;
  this->UpperLeftCorner[ 1 ] = this->MonitorSizePixel[ 1 ] - 50;
  
  this->UpperRightCorner[ 0 ] = this->MonitorSizePixel[ 0 ] - 50;
  this->UpperRightCorner[ 1 ] = this->MonitorSizePixel[ 1 ] - 50;
  
  this->LowerLeftCorner[ 0 ] = 40;
  this->LowerLeftCorner[ 1 ] = 50;
  
  this->LowerRightCorner[ 0 ] = this->MonitorSizePixel[ 0 ] - 50;
  this->LowerRightCorner[ 1 ] = 50;
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
 * @param show True makes depth perception lines and texts visible, false hides them.
 */
void
vtkPerkStationSecondaryMonitor
::ShowDepthPerceptionLines( bool show )
{
  int visibility = 0;
  if ( show ) visibility = 1;
  
  for( int i = 0; i < this->DepthPerceptionLines->GetNumberOfItems(); ++ i )
    {
    vtkActor* actor = vtkActor::SafeDownCast( this->DepthPerceptionLines->GetItemAsObject( i ) );
    actor->SetVisibility( visibility );
    }
  for( int i = 0; i < this->DepthPerceptionTexts->GetNumberOfItems(); ++ i )
    {
    vtkTextActorFlippable* actor = vtkTextActorFlippable::SafeDownCast( this->DepthPerceptionTexts->GetItemAsObject( i ) );
    actor->SetVisibility( visibility );
    }
}



void
vtkPerkStationSecondaryMonitor
::UpdateVisibilities( bool& displayEmpty )
{
    // Controlling visibility of visual aids that only depend on workphase.
  
  if ( this->GetPerkStationModuleNode() == NULL )  // Initial state.
    {
    this->ShowDepthPerceptionLines( false );
    displayEmpty = true;
    this->CalibrationControlsActor->SetVisibility( 0 );
    this->CalibrationNameActor->SetVisibility( 0 );
    this->DepthMeasureLineActor->SetVisibility( 0 );
    this->DepthMeasureTextActor->SetVisibility( 0 );
    this->LeftSideActor->SetVisibility( 0 );
    this->NeedleGuideActor->SetVisibility( 0 );
    this->PlanNameActor->SetVisibility( 0 );
    this->RightSideActor->SetVisibility( 0 );
    this->TablePositionActor->SetVisibility( 0 );
    this->WorkflowActor->SetInput( "" );
    this->WorkflowActor->SetVisibility( 0 );
    }
  
  if ( this->GetPerkStationModuleNode()->GetCurrentStep() == 0 ) // Calibration.
    {
    this->ShowDepthPerceptionLines( false );
    displayEmpty = false;
    this->CalibrationControlsActor->SetVisibility( 1 );
    this->CalibrationNameActor->SetVisibility( 0 );
    this->DepthMeasureLineActor->SetVisibility( 0 );
    this->DepthMeasureTextActor->SetVisibility( 0 );
    this->LeftSideActor->SetVisibility( 1 );
    this->NeedleGuideActor->SetVisibility( 0 );
    this->PlanNameActor->SetVisibility( 0 );
    this->RightSideActor->SetVisibility( 1 );
    this->TablePositionActor->SetVisibility( 0 );
    this->WorkflowActor->SetInput( "" );
    this->WorkflowActor->SetVisibility( 0 );
    }
  
  else if ( this->GetPerkStationModuleNode()->GetCurrentStep() == 1 ) // Planning.
    {
    this->ShowDepthPerceptionLines( false );
    displayEmpty = true;
    this->CalibrationControlsActor->SetVisibility( 0 );
    this->CalibrationNameActor->SetVisibility( 0 );
    this->DepthMeasureLineActor->SetVisibility( 0 );
    this->DepthMeasureTextActor->SetVisibility( 0 );
    this->LeftSideActor->SetVisibility( 0 );
    this->NeedleGuideActor->SetVisibility( 0 );
    this->PlanNameActor->SetVisibility( 0 );
    this->RightSideActor->SetVisibility( 0 );
    this->TablePositionActor->SetVisibility( 0 );
    this->WorkflowActor->SetInput( "Insertion planning in progress" );
    this->WorkflowActor->SetVisibility( 1 );
    }
  
  else if ( this->GetPerkStationModuleNode()->GetCurrentStep() == 2 ) // Insertion.
    {
    this->ShowDepthPerceptionLines( true );
    displayEmpty = false;
    this->CalibrationControlsActor->SetVisibility( 0 );
    this->CalibrationNameActor->SetVisibility( 1 );
    this->DepthMeasureLineActor->SetVisibility( 1 );
    this->DepthMeasureTextActor->SetVisibility( 1 );
    this->LeftSideActor->SetVisibility( 1 );
    this->NeedleGuideActor->SetVisibility( 1 );
    this->PlanNameActor->SetVisibility( 1 );
    this->RightSideActor->SetVisibility( 1 );
    this->TablePositionActor->SetVisibility( 1 );
    this->WorkflowActor->SetInput( "" );
    this->WorkflowActor->SetVisibility( 0 );
  
    }
  
  else if ( this->GetPerkStationModuleNode()->GetCurrentStep() == 3 ) // Validation.
    {
    this->ShowDepthPerceptionLines( false );
    displayEmpty = true;
    this->CalibrationControlsActor->SetVisibility( 0 );
    this->CalibrationNameActor->SetVisibility( 0 );
    this->DepthMeasureLineActor->SetVisibility( 0 );
    this->DepthMeasureTextActor->SetVisibility( 0 );
    this->LeftSideActor->SetVisibility( 0 );
    this->NeedleGuideActor->SetVisibility( 0 );
    this->PlanNameActor->SetVisibility( 0 );
    this->RightSideActor->SetVisibility( 0 );
    this->TablePositionActor->SetVisibility( 0 );
    this->WorkflowActor->SetInput( "Insertion validation in progress" );
    this->WorkflowActor->SetVisibility( 1 );
    }
  
  else  // No workphase set!
    {
    vtkErrorMacro( "No workphase set in Perk Station Module!" );
    }
  
  
    // Controlling visibility of the image slice.
  
  if ( displayEmpty )
    {
    this->ImageMapper->SetInput( this->EmptyImage );
    }
  else
    {
    this->ImageMapper->SetInput( this->MapToWindowLevelColors->GetOutput() );
    }
  
}



void
vtkPerkStationSecondaryMonitor
::PointOverlayToRenderer( double* pointDisplay, double* pointWorld )
{
  this->Renderer->SetDisplayPoint( pointDisplay[ 0 ], pointDisplay[ 1 ], 0 );
  this->Renderer->DisplayToWorld();
  this->Renderer->GetWorldPoint( pointWorld );
}

