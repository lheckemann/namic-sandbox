#include "vtkPerkStationSecondaryMonitor.h"
#include "vtkPerkStationModuleGUI.h"
#include "vtkMRMLPerkStationModuleNode.h"

#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkWin32OpenGLRenderWindow.h"
#include "vtkCylinderSource.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkMatrixToHomogeneousTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkProperty2D.h"
#include "vtkActorCollection.h"
#include "vtkActor2DCollection.h"

#include "vtkTextProperty.h"
#include "vtkTextActor.h"
#include "vtkTextSource.h"
#include "vtkTextMapper.h"
#include "vtkVectorText.h"

#include "vtkLineSource.h"
#include "vtkGlyphSource2D.h"
#include "vtkProperty.h"
#include "vtkPropCollection.h"

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
//----------------------------------------------------------------------------
vtkPerkStationSecondaryMonitor::vtkPerkStationSecondaryMonitor()
{
  this->GUI = NULL;

  // monitor info related
  this->DeviceActive = false; 
  this->DisplayInitialized = false;  
  this->VirtualScreenCoord[0] = 0;
  this->VirtualScreenCoord[1] = 0;
  this->ScreenSize[0] = 800;
  this->ScreenSize[1] = 600;
  this->ScreenSize[2] = 1;
  this->CurrentRotation = 0.0;
  this->CurrentTranslation[0] = 0.0;
  this->CurrentTranslation[1] = 0.0;
  this->CalibrationFromFileLoaded = false;

  this->DepthLinesInitialized = false;
  this->NumOfDepthPerceptionLines = 0;

  // these values should be read in from a config file
  // the values below have been measure for 
  // Viewsonic monitor VA503b
  this->MonitorPhysicalSizeMM[0] = 304.8;
  this->MonitorPhysicalSizeMM[1] = 228.6;

  this->MonitorPixelResolution[0] = 800;
  this->MonitorPixelResolution[1] = 600;

  this->MeasureNeedleLengthInWorldCoordinates = 0;

  // display/view related
  this->ImageData = NULL;
  this->VolumeNode = NULL;
 
  // matrices
  this->XYToIJK = vtkMatrix4x4::New();
  this->XYToRAS = vtkMatrix4x4::New();
  this->CurrentTransformMatrix = vtkMatrix4x4::New();
  this->ResliceTransform = vtkTransform::New();

  this->SystemStateResliceMatrix = vtkMatrix4x4::New();
  this->SystemStateXYToIJK = vtkMatrix4x4::New();
  
  this->HorizontalFlipped = false;
  this->VerticalFlipped = false;
  
  this->Reslice = vtkImageReslice::New();
  this->Reslice->SetBackgroundColor(0, 0, 0, 0);  
  this->Reslice->SetOutputDimensionality(2);  
  this->Reslice->SetInterpolationModeToLinear();
  
  // set up render window, renderer, actor, mapper
 
 this->Renderer = vtkRenderer::New();
 this->RenderWindow = vtkWin32OpenGLRenderWindow::New();
 this->RenderWindow->AddRenderer(this->Renderer);
 this->RenderWindow->SetBorders(0);
 
 vtkCamera *camera = vtkCamera::New();
 camera->SetParallelProjection(1);

 this->Renderer->SetActiveCamera(camera);
 
 // Interactor
 this->Interactor = vtkRenderWindowInteractor::New();
 this->Interactor->SetRenderWindow(this->RenderWindow);

 vtkSlicerInteractorStyle *iStyle = vtkSlicerInteractorStyle::New();
 this->Interactor->SetInteractorStyle (iStyle);

 // window level mapping
 this->MapToWindowLevelColors = vtkImageMapToWindowLevelColors::New();
 this->MapToWindowLevelColors->SetOutputFormatToLuminance();

 // mapper
 this->ImageMapper = vtkImageMapper::New();
 this->ImageMapper->SetColorWindow(255);
 this->ImageMapper->SetColorLevel(127.5);

 // actor
 this->ImageActor = vtkActor2D::New();
 this->ImageActor->SetMapper(this->ImageMapper);
 this->ImageActor->GetProperty()->SetDisplayLocationToBackground();

 this->NeedleGuideActor = vtkActor::New();  
 this->NeedleTipActor = vtkActor::New();
 this->RealTimeNeedleLineActor = vtkActor::New();
 this->NeedleTipZLocationText = vtkTextActor::New();
 this->DepthPerceptionLines = vtkActorCollection::New(); 
 this->TextActorsCollection = vtkActor2DCollection::New();

}


//----------------------------------------------------------------------------
vtkPerkStationSecondaryMonitor::~vtkPerkStationSecondaryMonitor()
{
  this->SetGUI(NULL);

  if (this->RenderWindow)
    {
    this->RenderWindow->Delete();
    this->RenderWindow = NULL;
    }
  if (this->Renderer)
    {
    this->Renderer->RemoveAllViewProps();
    this->Renderer->Delete();
    this->Renderer = NULL;
    }

  if (this->Interactor)
    {
    this->Interactor->Delete();
    this->Interactor = NULL;
    }

  if (this->ImageMapper)
    {
    this->ImageMapper->Delete();
    this->ImageMapper = NULL;
    }
  if (this->ImageActor)
    {
    this->ImageActor->Delete();
    this->ImageActor = NULL;
    }
  if (this->NeedleGuideActor)
    {
    this->NeedleGuideActor->Delete();
    this->NeedleGuideActor = NULL;
    }
  if (this->NeedleTipActor)
    {
    this->NeedleTipActor->Delete();
    this->NeedleTipActor = NULL;
    }
  if (this->RealTimeNeedleLineActor)
    {
    this->RealTimeNeedleLineActor->Delete();
    this->RealTimeNeedleLineActor = NULL;
    }
  if (this->NeedleTipZLocationText)
    {
    this->NeedleTipZLocationText->Delete();
    this->NeedleTipZLocationText = NULL;
    }
  if (this->MapToWindowLevelColors)
    {
    this->MapToWindowLevelColors->Delete();
    this->MapToWindowLevelColors = NULL;
    }
  if (this->Reslice)
    {
    this->Reslice->Delete();
    this->Reslice = NULL;
    }
  if (this->XYToIJK)
    {
    this->XYToIJK->Delete();
    this->XYToIJK = NULL;
    }
  if (this->XYToRAS)
    {
    this->XYToRAS->Delete();
    this->XYToRAS = NULL;
    }
  if (this->CurrentTransformMatrix)
    {
    this->CurrentTransformMatrix->Delete();
    this->CurrentTransformMatrix = NULL;
    }
  if (this->ResliceTransform)
    {
    this->ResliceTransform->Delete();
    this->ResliceTransform = NULL;
    }
  if (this->VolumeNode)
    {
    this->VolumeNode->Delete();
    this->VolumeNode = NULL;
    }
  if (this->ImageData)
    {
    this->ImageData->Delete();
    this->ImageData = NULL;
    }  
  if (this->DepthPerceptionLines)
    {
    this->DepthPerceptionLines->RemoveAllItems();
    this->DepthPerceptionLines->Delete();
    this->DepthPerceptionLines = NULL;
    }
  if (this->TextActorsCollection)
    {
    this->TextActorsCollection->RemoveAllItems();
    this->TextActorsCollection->Delete();
    this->TextActorsCollection = NULL;
    }

}
//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::ResetCalibration()
{
  if (!this->DisplayInitialized)
      return;

  // should reset the matrices
  // matrices
  this->XYToIJK->Identity();
  this->XYToRAS->Identity();
  this->CurrentTransformMatrix->Identity();
  this->ResliceTransform->Identity();
  
  this->HorizontalFlipped = false;
  this->VerticalFlipped = false;

  this->CurrentRotation = 0.0;
  this->CurrentTranslation[0] = 0.0;
  this->CurrentTranslation[1] = 0.0;

  // calculate initial xytoijk matrix
  vtkMatrix4x4 *xyToIJK = vtkMatrix4x4::New();
  xyToIJK->Identity();

  for (int i = 0; i < 3; i++)
   {
   xyToIJK->SetElement(i, i, 1.0);
   xyToIJK->SetElement(i, 3, -(this->ScreenSize[i]-this->ImageSize[i])/ 2.); //translation assuming both image & display have origins at bottom left respectively
   }
 
  xyToIJK->SetElement(2,3,0.);
  this->XYToIJK->DeepCopy(xyToIJK);

  // to have consistent display i.e. same display as in SLICER's slice viewer, and own render window in secondary monitor
  // figure out whether a horizontal flip required or a vertical flip or both
  // Note: this does not counter the flip that will be required due to secondary monitors orientation/mounting
  // It only makes sure that two displays have same view if they are physically in same orientation
  vtkMatrix4x4 *directionMatrix = vtkMatrix4x4::New();
  this->VolumeNode->GetIJKToRASDirectionMatrix(directionMatrix);

  vtkMatrix4x4 *flipMatrix = vtkMatrix4x4::New();
  bool verticalFlip = false;
  bool horizontalFlip = false;

  flipMatrix = this->GetFlipMatrixFromDirectionCosines(directionMatrix,verticalFlip,horizontalFlip);
  this->CurrentTransformMatrix->DeepCopy(flipMatrix);

  this->UpdateMatrices();    
  
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
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }

  this->VolumeNode = mrmlNode->GetPlanningVolumeNode();
  if (!this->VolumeNode)
    {
    // TO DO: what to do on failure
    return;
    }

  this->ImageData = this->VolumeNode->GetImageData();

  int imageExtent[6];
  this->ImageData->GetExtent(imageExtent);

  this->ImageSize[0] = imageExtent[1] - imageExtent[0] + 1;
  this->ImageSize[1] = imageExtent[3] - imageExtent[2] + 1;
  this->ImageSize[2] = 1;

  // positioning window
  this->RenderWindow->SetPosition(this->VirtualScreenCoord[0], this->VirtualScreenCoord[1]);
  // window size
  this->RenderWindow->SetSize(this->ScreenSize[0], this->ScreenSize[1]);
  //window/level
  this->MapToWindowLevelColors->SetWindow(this->VolumeNode->GetScalarVolumeDisplayNode()->GetWindow());
  this->MapToWindowLevelColors->SetLevel(this->VolumeNode->GetScalarVolumeDisplayNode()->GetLevel());
  this->MapToWindowLevelColors->SetInput( this->Reslice->GetOutput() );
  // mapper
  this->ImageMapper->SetInput(this->MapToWindowLevelColors->GetOutput());
  // actor  
  // remove previous active actors
  this->Renderer->RemoveAllViewProps();
  // add new image actor
  this->Renderer->AddActor(this->ImageActor);

  // matrix stuff  
  // matrices
  this->XYToIJK->Identity();
  this->XYToRAS->Identity();
  this->CurrentTransformMatrix->Identity();
  this->ResliceTransform->Identity();

  // calculate initial xytoijk matrix
  vtkMatrix4x4 *xyToIJK = vtkMatrix4x4::New();
  xyToIJK->Identity();

  for (int i = 0; i < 3; i++)
   {
   xyToIJK->SetElement(i, i, 1.0);
   xyToIJK->SetElement(i, 3, -(this->ScreenSize[i]-this->ImageSize[i])/ 2.); //translation assuming both image & display have origins at bottom left respectively
   }
 
  xyToIJK->SetElement(2,3,0.0);
  this->XYToIJK->DeepCopy(xyToIJK);

    
  // to have consistent display i.e. same display as in SLICER's slice viewer, and own render window in secondary monitor
  // figure out whether a horizontal flip required or a vertical flip or both
  // Note: this does not counter the flip that will be required due to secondary monitors orientation/mounting
  // It only makes sure that two displays have same view if they are physically in same orientation
  vtkMatrix4x4 *directionMatrix = vtkMatrix4x4::New();
  this->VolumeNode->GetIJKToRASDirectionMatrix(directionMatrix);

  vtkMatrix4x4 *flipMatrix = vtkMatrix4x4::New();
  bool verticalFlip = false;
  bool horizontalFlip = false;

  flipMatrix = this->GetFlipMatrixFromDirectionCosines(directionMatrix,verticalFlip,horizontalFlip);
  this->CurrentTransformMatrix->DeepCopy(flipMatrix);


  this->DisplayInitialized = true;

  this->UpdateMatrices();  
 
  
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

  if (this->GetGUI()->GetMode()!= vtkPerkStationModuleGUI::ModeId::Clinical)
    {
    return;
    }

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
  mrmlNode->SetActualScaling(double(imgSpacing[0]/monSpacing[0]), double(imgSpacing[1]/monSpacing[1]));
  double scale[2];
  mrmlNode->GetActualScaling(scale);
  
  // actually scale the image
  this->Scale(scale[0], scale[1], 1.0);

  // 2) Vertical/Horizonal flip
  bool verticalFlip = false;
  bool horizontalFlip = false;

  verticalFlip = this->GetGUI()->GetMRMLNode()->GetVerticalFlip();
  horizontalFlip = this->GetGUI()->GetMRMLNode()->GetHorizontalFlip();

  if (verticalFlip)
    this->GetGUI()->GetSecondaryMonitor()->FlipVertical();

  if (horizontalFlip)
    this->GetGUI()->GetSecondaryMonitor()->FlipHorizontal(); 

  // 3) Translation
  double trans[2];
  mrmlNode->GetClinicalModeTranslation(trans);
  this->Translate(trans[0], trans[1], 0);

  // 4) Rotation
  // note that center of rotation will be automatically read inside the rotate function from the mrml node
  double rot = 0;
  rot = mrmlNode->GetClinicalModeRotation();  
  this->Rotate(rot);

  this->CalibrationFromFileLoaded = true;
}
//----------------------------------------------------------------------------
vtkMatrix4x4 *vtkPerkStationSecondaryMonitor::GetFlipMatrixFromDirectionCosines (vtkMatrix4x4 *directionMatrix, bool & verticalFlip, bool & horizontalFlip)
{
  vtkMatrix4x4 *flipMatrix = vtkMatrix4x4::New();
  flipMatrix->Identity();

  vtkMatrix4x4 *targetMatrix = vtkMatrix4x4::New();
  targetMatrix->Identity();
  targetMatrix->SetElement(0,0, -1.);

  // target 3x3 matrix  -1 0 0 
  //                     0 1 0
  //                     0 0 1

  // source: directionMatrix

  // target = flipMatrix x source
  // flipMatrix = source.invert() x target

  directionMatrix->Invert();
  vtkMatrix4x4::Multiply4x4(directionMatrix, targetMatrix, flipMatrix);

  if (flipMatrix->GetElement(0,0) < 0 ) // i.e. -1
    {
    horizontalFlip = true;
    // flip on y axis and bring back to match center of screen
    flipMatrix->SetElement(0,3, this->ImageSize[0]);
    }

  if (flipMatrix->GetElement(1,1) < 0 ) // i.e. -1
    {
    verticalFlip = true;
    // flip on x-axis and bring back to match center of screen
    flipMatrix->SetElement(1,3, this->ImageSize[1]);
    }

  return flipMatrix;
}
//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::UpdateMatrices()
{
  vtkMatrix4x4 *resliceMatrix = this->ResliceTransform->GetMatrix();
  vtkMatrix4x4 *xyToIJK = this->XYToIJK;

  //update reslice matrix
  vtkMatrix4x4::Multiply4x4(this->CurrentTransformMatrix, resliceMatrix,resliceMatrix);
  this->ResliceTransform->GetMatrix()->DeepCopy(resliceMatrix);

  //update xyToIJK
  vtkMatrix4x4::Multiply4x4(this->CurrentTransformMatrix,xyToIJK, xyToIJK);
  this->XYToIJK->DeepCopy(xyToIJK);

  
  //update xyToRAS
  vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
  this->VolumeNode->GetIJKToRASMatrix(ijkToRAS);
  vtkMatrix4x4::Multiply4x4(ijkToRAS, this->XYToIJK, this->XYToRAS);

  vtkSlicerSliceLogic *sliceLogic = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Red")->GetLogic();
  this->UpdateImageDataOnSliceOffset(sliceLogic->GetSliceOffset());

  this->UpdateImageDisplay();

}
//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::UpdateImageDisplay()
{
  if(this->DisplayInitialized)
    {
    this->MapToWindowLevelColors->SetWindow(this->VolumeNode->GetScalarVolumeDisplayNode()->GetWindow());
    this->MapToWindowLevelColors->SetLevel(this->VolumeNode->GetScalarVolumeDisplayNode()->GetLevel());
    this->Reslice->SetOutputExtent(0,this->ScreenSize[0]-1,0,this->ScreenSize[1]-1,0,0);
    this->Reslice->SetInput( this->ImageData);
    this->Reslice->SetResliceTransform( this->ResliceTransform );
    this->Reslice->Update();
    
    if (this->DeviceActive)
        {
        this->RenderWindow->Render();  
        }
    }

  /*vtkSlicerSliceLogic *sliceLogic = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI0()->GetLogic();
  vtkMRMLSliceNode *sliceNode = sliceLogic->GetSliceNode();

  vtkMatrix4x4 *xyslice = sliceNode->GetXYToSlice();
  vtkMatrix4x4 *sliceRas = sliceNode->GetSliceToRAS();
  vtkMatrix4x4 *xyRas = sliceNode->GetXYToRAS();
  */
  
    
}
//---------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::UpdateImageDataOnSliceOffset(double rasOffset)
{
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

  if (strcmpi(mrmlNode->GetVolumeInUse(), "Planning"))
      return;

  // first convert 's' offset into 'k' offset in ijk space
  // this can be done using rastoijk matrix

  // we need to update reslice transform and the xytoijk transform
  vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
  this->VolumeNode->GetIJKToRASMatrix(ijkToRAS);

  vtkMatrix4x4 *rasToIJK = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert(ijkToRAS, rasToIJK);

  double rasPt[4] = { 0,0, rasOffset, 1};
  double ijkPt[4];
  rasToIJK->MultiplyPoint(rasPt, ijkPt);

  // now we have the ijk offset
  double kOffset = ijkPt[2];

  vtkMatrix4x4 *xyToRAS = vtkMatrix4x4::New();
  xyToRAS->DeepCopy( this->XYToRAS );

  for (int i = 0; i < 3; i++)
    {
    xyToRAS->SetElement( i, 3, 0.0 );  // Zero out the tranlation portion
    }
  xyToRAS->Invert();
  double v1[4], v2[4];
  for (int i = 0; i < 4; i++)
    { // get the translation back as a vector
    v1[i] = this->XYToRAS->GetElement( i, 3 );
    }
  // bring the translation into slice space
  // and overwrite the z part
  xyToRAS->MultiplyPoint(v1, v2);

  v2[2] = rasOffset;

  // Now bring the new translation vector back into RAS space
  xyToRAS->Invert();
  xyToRAS->MultiplyPoint(v2, v1);
  for (int i = 0; i < 4; i++)
    {
    xyToRAS->SetElement( i, 3, v1[i] );
    }
 
  this->XYToRAS->DeepCopy( xyToRAS );
  // calculate the xyToRAS matrix
  vtkMatrix4x4::Multiply4x4(rasToIJK, this->XYToRAS, this->XYToIJK);
  
  xyToRAS->Delete();


  


  /*
  //
  // Set the Offset
  // - get the current translation in ijk space and convert it to xy space
  //   by transforming it by the invers of the upper 3x3 of SliceToRAS
  // - replace the k value of the translation with the new value given by the kOffset
  // - this preserves whatever translation was already in place
  //


  vtkMatrix4x4 *xyToIJK = vtkMatrix4x4::New();
  xyToIJK->DeepCopy( this->XYToIJK );

  for (int i = 0; i < 3; i++)
    {
    xyToIJK->SetElement( i, 3, 0.0 );  // Zero out the tranlation portion
    }
  xyToIJK->Invert();
  double v1[4], v2[4];
  for (int i = 0; i < 4; i++)
    { // get the translation back as a vector
    v1[i] = this->XYToIJK->GetElement( i, 3 );
    }
  // bring the translation into slice space
  // and overwrite the z part
  xyToIJK->MultiplyPoint(v1, v2);

  v2[2] = kOffset;

  // Now bring the new translation vector back into RAS space
  xyToIJK->Invert();
  xyToIJK->MultiplyPoint(v2, v1);
  for (int i = 0; i < 4; i++)
    {
    xyToIJK->SetElement( i, 3, v1[i] );
    }
 
  // if the translation has changed, update the rest of the matrices
    if ( xyToIJK->GetElement( 0, 3 ) != this->XYToIJK->GetElement( 0, 3 ) ||
       xyToIJK->GetElement( 1, 3 ) != this->XYToIJK->GetElement( 1, 3 ) ||
       xyToIJK->GetElement( 2, 3 ) != this->XYToIJK->GetElement( 2, 3 ) )
    {
    this->XYToIJK->DeepCopy( xyToIJK );
    // calculate the xyToRAS matrix
    vtkMatrix4x4::Multiply4x4(ijkToRAS, this->XYToIJK, this->XYToRAS);
    }
  xyToIJK->Delete();
*/

  // same exercise for reslicetransform matrix

  vtkMatrix4x4 *resliceMatrix = vtkMatrix4x4::New();
  resliceMatrix->DeepCopy(this->ResliceTransform->GetMatrix());

  for (int i = 0; i < 3; i++)
    {
    resliceMatrix->SetElement( i, 3, 0.0 );  // Zero out the tranlation portion
    }
  resliceMatrix->Invert();

  for (int i = 0; i < 4; i++)
    { // get the translation back as a vector
    v1[i] = this->ResliceTransform->GetMatrix()->GetElement( i, 3 );
    }
  // bring the translation into slice space
  // and overwrite the z part
  resliceMatrix->MultiplyPoint(v1, v2);

  v2[2] = kOffset;

  // Now bring the new translation vector back into RAS space
  resliceMatrix->Invert();
  resliceMatrix->MultiplyPoint(v2, v1);
  for (int i = 0; i < 4; i++)
    {
    resliceMatrix->SetElement( i, 3, v1[i] );
    }
 
 this->ResliceTransform->GetMatrix()->DeepCopy( resliceMatrix );
 resliceMatrix->Delete();


  this->UpdateImageDisplay();







  /*
  vtkSlicerSliceLogic *sliceLogic = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Red")->GetLogic();
  vtkMRMLSliceNode *sliceNode = sliceLogic->GetSliceNode();

  vtkMatrix4x4 *sliceRas = sliceNode->GetSliceToRAS();

  vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
  this->VolumeNode->GetIJKToRASMatrix(ijkToRAS);

  vtkMatrix4x4 *rasToIJK = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert(ijkToRAS, rasToIJK);

  double inPt[4] = {0, 0, sliceRas->GetElement(2,3), 1};
  double outPt[4];
  rasToIJK->MultiplyPoint(inPt, outPt);

  
  this->ResliceTransform->GetMatrix()->SetElement(2,3, outPt[2]);
  this->XYToIJK->SetElement(2,3, outPt[2]);
  this->XYToRAS->SetElement(2,3, sliceRas->GetElement(2,3));

  this->UpdateImageDisplay();

  */

}
//----------------------------------------------------------------------------
BOOL CALLBACK MyInfoEnumProc(HMONITOR hMonitor, HDC hdc, LPRECT prc, LPARAM dwData) 
{
  vtkPerkStationSecondaryMonitor *self = (vtkPerkStationSecondaryMonitor *)dwData;
  if (self)
    {
    MONITORINFO mi;
    mi.cbSize = sizeof(MONITORINFO);
    if (GetMonitorInfo(hMonitor, &mi))
      {
      // check if the monitor is primary or not
      if (!(mi.dwFlags & MONITORINFOF_PRIMARY))
        {
        // monitor not primary, store the rect information
        self->SetVirtualScreenCoord(mi.rcMonitor.left, mi.rcMonitor.top);       
        }
      }
    
    }
  return TRUE;
} 
//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::Initialize()
{

  // first make sure secondary monitor is connected
  int nMonitors = GetSystemMetrics(SM_CMONITORS);

  if (nMonitors == 2)
    {
    // get info about the monitors
    EnumDisplayMonitors(NULL, NULL, MyInfoEnumProc, (LPARAM) this);

    // get the resolution/dimensions/spacing
    DISPLAY_DEVICE lpDisplayDevice;
    lpDisplayDevice.cb = sizeof(lpDisplayDevice);
    lpDisplayDevice.StateFlags = DISPLAY_DEVICE_ATTACHED_TO_DESKTOP;
    int iDevNum = 0;
    int iPhyDevNum = 0;
    DWORD dwFlags = 0;

    while(EnumDisplayDevices(NULL, iDevNum, &lpDisplayDevice, dwFlags))
      {
      iDevNum++;
      if( (lpDisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == 1)
        {
        iPhyDevNum++;
        
        // get the device context for the monitor
        HDC hdc = CreateDC("DISPLAY", lpDisplayDevice.DeviceName, NULL, NULL);    
          
        // now the device context can be used in many functions to retrieve information about the monitor
        double width_mm = GetDeviceCaps(hdc, HORZSIZE);
        double height_mm = GetDeviceCaps(hdc, VERTSIZE);
         
        double width_pix = GetDeviceCaps(hdc, HORZRES);
        double height_pix = GetDeviceCaps(hdc, VERTRES);
        
        if (!(lpDisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
          {
          this->MonitorPhysicalSizeMM[0] = width_mm;
          this->MonitorPhysicalSizeMM[1] = height_mm;
          this->MonitorPixelResolution[0] = width_pix;
          this->MonitorPixelResolution[1] = height_pix;
          this->DeviceActive = true;
          this->ScreenSize[0] = width_pix;
          this->ScreenSize[1] = height_pix;
          }

        DeleteDC(hdc);

        }
      }

    }

}


//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::FlipVertical()
{
    vtkMatrix4x4 *flipVerticalMatrix = vtkMatrix4x4::New();
    flipVerticalMatrix->Identity();
    flipVerticalMatrix->SetElement(1,1,-1);
    flipVerticalMatrix->SetElement(1,3,this->ImageSize[1]);

    this->CurrentTransformMatrix->DeepCopy(flipVerticalMatrix);

    this->VerticalFlipped = true;
    this->UpdateMatrices();
}
//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::FlipHorizontal()
{
    vtkMatrix4x4 *flipHorizontalMatrix = vtkMatrix4x4::New();
    flipHorizontalMatrix->Identity();
    flipHorizontalMatrix->SetElement(0,0,-1);
    flipHorizontalMatrix->SetElement(0,3,this->ImageSize[0]);

    this->CurrentTransformMatrix->DeepCopy(flipHorizontalMatrix);

    this->HorizontalFlipped = true;
    this->UpdateMatrices();
}
//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::Scale(double sx, double sy, double sz)
{
  vtkTransform *transform = vtkTransform::New();
  transform->PostMultiply();
  
  // scale about center
  transform->Translate(-this->ImageSize[0]/2., -this->ImageSize[1]/2., 0);
  transform->Scale(1./sx, 1./sy, 1./sz);
  transform->Translate(this->ImageSize[0]/2., this->ImageSize[1]/2., 0);

  vtkMatrix4x4 *scaleMatrix = vtkMatrix4x4::New();
  scaleMatrix->DeepCopy(transform->GetMatrix());

  this->CurrentTransformMatrix->DeepCopy(scaleMatrix);

  this->UpdateMatrices();

}
//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::Rotate(double angle)
{
  
  vtkTransform *transform = vtkTransform::New();
  transform->PostMultiply();
  
  vtkMatrix4x4 *rasToXY = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert(this->XYToRAS, rasToXY);

  this->CurrentRotation += angle;

  // center of rotation
  double rasCOR[3];
  this->GetGUI()->GetMRMLNode()->GetCenterOfRotation(rasCOR);
  double rasPoint[4] = {rasCOR[0], rasCOR[1], 0, 1};
  double xyPoint[4];  // point in xy space
  double ijkPoint[4]; // point in image ijk space
  rasToXY->MultiplyPoint(rasPoint, xyPoint);  
  this->XYToIJK->MultiplyPoint(xyPoint,ijkPoint);

  // rotate about center of rotation
  transform->Translate(-ijkPoint[0], -ijkPoint[1], 0);
  transform->RotateZ(angle);
  transform->Translate(ijkPoint[0], ijkPoint[1], 0);

  vtkMatrix4x4 *rotateMatrix = vtkMatrix4x4::New();
  rotateMatrix->DeepCopy(transform->GetMatrix());

  this->CurrentTransformMatrix->DeepCopy(rotateMatrix);

  this->UpdateMatrices();
}
//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::TiltOutOfPlane(double tiltAngle, double rasCor[3])
{
  // save the state for reset?
  // state determined by three matrices: reslice, xytoijk, and current transform
  // we won't meddle with current transform matrix, so need to save reslice and xytoijk
  this->SystemStateResliceMatrix->DeepCopy(this->ResliceTransform->GetMatrix());
  this->SystemStateXYToIJK->DeepCopy(this->XYToIJK);



  // apply the tilt
  vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
  this->VolumeNode->GetIJKToRASMatrix(ijkToRAS);

  vtkMatrix4x4 *rasToIJK = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert(ijkToRAS, rasToIJK);
  

  /*
  double inPt[4] = {rasCor[0], rasCor[1], rasCor[2], 1};
  double outPt[4];

  rasToIJK->MultiplyPoint(inPt, outPt);

  double ijkPt[3] = {outPt[0], outPt[1], outPt[2]};

  */


  // rotate the reslice transform by tilt angle about x-axis
  vtkTransform *transform = vtkTransform::New();
  transform->RotateX(tiltAngle);
  vtkMatrix4x4 *rotMatrix = vtkMatrix4x4::New();
  rotMatrix->DeepCopy(transform->GetMatrix());

  //this->CurrentTransformMatrix->DeepCopy(rotMatrix);

  vtkMatrix4x4 *xyToRAS = vtkMatrix4x4::New();
  xyToRAS->DeepCopy(this->XYToRAS);

  vtkMatrix4x4::Multiply4x4(rotMatrix, xyToRAS, xyToRAS);
  xyToRAS->SetElement(2,3, rasCor[2]);
  this->XYToRAS->DeepCopy(xyToRAS);

  // calculate the translation vector between xyToIJK & reslice transform
  double t[3];
  t[0] = this->XYToIJK->GetElement(0,3) - this->ResliceTransform->GetMatrix()->GetElement(0,3);
  t[1] = this->XYToIJK->GetElement(1,3) - this->ResliceTransform->GetMatrix()->GetElement(1,3);
  t[2] = this->XYToIJK->GetElement(2,3) - this->ResliceTransform->GetMatrix()->GetElement(2,3);


  vtkMatrix4x4 *xyToIJK = vtkMatrix4x4::New();
  vtkMatrix4x4::Multiply4x4(rasToIJK, xyToRAS, xyToIJK);
  this->XYToIJK->DeepCopy(xyToIJK);
  
  
  vtkMatrix4x4 *resliceMatrix = vtkMatrix4x4::New();
  resliceMatrix->DeepCopy(xyToIJK);
  resliceMatrix->SetElement(0,3,xyToIJK->GetElement(0,3)-t[0]);
  resliceMatrix->SetElement(1,3,xyToIJK->GetElement(1,3)-t[1]);
  resliceMatrix->SetElement(2,3,xyToIJK->GetElement(2,3)-t[2]);
  this->ResliceTransform->GetMatrix()->DeepCopy(resliceMatrix);


  /*vtkMatrix4x4 *resliceMatrix = vtkMatrix4x4::New();
  resliceMatrix->DeepCopy(this->ResliceTransform->GetMatrix());

  vtkMatrix4x4::Multiply4x4(this->CurrentTransformMatrix, resliceMatrix,resliceMatrix);
  this->ResliceTransform->GetMatrix()->DeepCopy(resliceMatrix);
  
 // this->ResliceTransform->RotateX(tiltAngle);

  vtkMatrix4x4 *newReslice = vtkMatrix4x4::New();
  newReslice->DeepCopy(this->ResliceTransform->GetMatrix());
*/
  this->UpdateImageDisplay();

  

}
//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::Translate(double tx, double ty, double tz)
{
  // translation components come in here as mm
  // convert into pixels with original spacing

  this->CurrentTranslation[0] += tx;
  this->CurrentTranslation[1] += ty;

  double spacing[3];
  spacing[0] = this->VolumeNode->GetSpacing()[0];
  spacing[1] = this->VolumeNode->GetSpacing()[1];
  spacing[2] = this->VolumeNode->GetSpacing()[2];

  vtkTransform *transform = vtkTransform::New();
  transform->Translate(tx/spacing[0], ty/spacing[1], tz/spacing[2]);

  vtkMatrix4x4 *translateMatrix = vtkMatrix4x4::New();
  translateMatrix->DeepCopy(transform->GetMatrix());

  this->CurrentTransformMatrix->DeepCopy(translateMatrix);

  this->UpdateMatrices();
}

//-----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::OverlayRealTimeNeedleTip(double tipRAS[3], vtkMatrix4x4 *toolTransformMatrix)
{
  vtkMatrix4x4 *rasToXY = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert(this->XYToRAS, rasToXY);

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
  this->XYToRAS->MultiplyPoint(inPt, outPt);
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

  /*
    // TO DO: transfrom needle mapper using vtkTransformPolyData
  vtkMatrix4x4 *transformMatrix = vtkMatrix4x4::New();
  transformMatrix->Identity();
 

  transformMatrix->SetElement(0,0, cos(angleRad));
  transformMatrix->SetElement(0,1, sin(angleRad));
  transformMatrix->SetElement(0,2, 0);
  transformMatrix->SetElement(0,3, 0);
  

  transformMatrix->SetElement(1,0, -sin(angleRad));
  transformMatrix->SetElement(1,1, cos(angleRad));
  transformMatrix->SetElement(1,2, 0);
  transformMatrix->SetElement(1,3, 0);
  

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
  filter->SetInputConnection(needleTip->GetOutputPort()); 
  filter->SetTransform(transform);  
*/


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
  if (this->HorizontalFlipped)
    { 
    this->NeedleTipZLocationText->GetTextProperty()->SetJustificationToCentered();
    this->NeedleTipZLocationText->SetOrientation(180);
    //char *revText = vtkPerkStationModuleLogic::strrev(text, strlen(text));
    //this->NeedleTipZLocationText->SetInput(revText);
    }
  else if (this->VerticalFlipped)
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
  vtkMatrix4x4::Invert(this->XYToRAS, rasToXY);

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
  // get the cylinder source, create the cylinder, whose height is equal to calculated insertion depth
  // apply transform on the cylinder to world coordinates, using the information of entry and target point
  // i.e. using the insertion angle
  // add it to slice viewer's renderer

  vtkCylinderSource *needleGuide = vtkCylinderSource::New();
  // TO DO: how to relate this to actual depth???
  needleGuide->SetHeight(2*halfNeedleGuideLength );
  //needleGuide->SetHeight(0.75);
  needleGuide->SetRadius( 0.015 );  
  needleGuide->SetResolution( 20 );

  //int *windowSize = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindow()->GetSize();
  // because cylinder is positioned at the window center
  double needleCenter[3];  
  needleCenter[0] = wcEntryPoint[0];// - windowSize[0]/2;
  needleCenter[1] = wcEntryPoint[1];// - windowSize[1]/2;
  //needleCenter[2] = wcEntryPoint[2];
  //needleGuide->SetCenter(needleCenter[0], needleCenter[1], needleCenter[2]);

  // angle as calculated from xy coordinates
  double insAngle = -double(180/vtkMath::Pi()) * atan(double((xyEntry[1] - xyTarget[1])/(xyEntry[0] - xyTarget[0])));

/*  
  double angleNeedle = this->GetGUI()->GetMRMLNode()->GetActualPlanInsertionAngle();
  if (this->HorizontalFlipped || this->VerticalFlipped)
      angleNeedle = -angleNeedle;

  // account for image rotation
  double rotationRAS = this->GetGUI()->GetMRMLNode()->GetUserRotation();
  angleNeedle = angleNeedle + rotationRAS;

  double insAngleRad = vtkMath::Pi()/2 - double(vtkMath::Pi()/180)*angleNeedle;
*/

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
  //needleMapper->SetInputConnection(needleGuide->GetOutputPort());

  // after transfrom, set up actor 
  this->NeedleGuideActor->SetMapper(needleMapper );
  //this->NeedleActor->GetProperty()->SetOpacity(0.3);
  
  //needleActor->SetPosition(needleCenter[0], needleCenter[1],0);  
  //needleActor->RotateZ(90-angle);
  
 
  // add to renderer of the Axial slice viewer
  this->Renderer->AddActor(this->NeedleGuideActor);  
  this->RenderWindow->Render(); 
 
  
}


//------------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::SetDepthPerceptionLines()
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if (!mrmlNode)
      return;

  if (this->DepthLinesInitialized)
      return;

  this->DepthPerceptionLines->RemoveAllItems();
  this->TextActorsCollection->RemoveAllItems();

  // Get insertion depth
  double insertionDepth = mrmlNode->GetActualPlanInsertionDepth();

  if (insertionDepth > 0)
    {
    // first calculate how many, in increments of 10 mm, less than equal to 5 lines, that less than max number of lines
    this->NumOfDepthPerceptionLines = insertionDepth/10;
      
    double lengthIncrement = 10.0; // in mm
      
    vtkMatrix4x4 *rasToXY = vtkMatrix4x4::New();
    vtkMatrix4x4::Invert(this->XYToRAS, rasToXY);

    double pointXY[2];
    double worldCoordinate[4];
    double wcStartPoint[4];
    double wcEndPoint[4];

    // entry point
    double rasEntry[3];
    mrmlNode->GetPlanEntryPoint(rasEntry);
    // target point
    double rasTarget[3];
    mrmlNode->GetPlanTargetPoint(rasTarget);

    double rasTemp[3];
    double inPt[4];
    double outPt[4];  
        

    double denom = rasTarget[0]-rasEntry[0];
    double numer = rasTarget[1]-rasEntry[1];

    double insertionAngle = atan(double(rasTarget[1]-rasEntry[1])/double(rasTarget[0]-rasEntry[0]));
      

    // create the end point coordinates of each depth perception line, starting from entry point, moving towards target point
    for (unsigned int i = 0; i < this->NumOfDepthPerceptionLines; i++)
        {
        if (denom>=0)
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
       
        vtkTextActor *textActor = vtkTextActor::New();
        textActor->SetInput(text);

        //textActor->SetPosition(wcStartPoint[0],wcStartPoint[1],wcStartPoint[2]);
        //textActor->GetProperty()->SetColor(0,0,1);
        textActor->GetTextProperty()->SetColor(0,1,0);
        textActor->SetTextScaleModeToNone();
        textActor->GetTextProperty()->SetFontSize(30);
        
        if (denom>=0)
          {
          textActor->SetDisplayPosition(pointXY[0]+100, pointXY[1]+5);
          // convert to world coordinate
          //this->Renderer->SetDisplayPoint(pointXY[0]+100,pointXY[1]+5, 0);
          //this->Renderer->DisplayToWorld();
          //this->Renderer->GetWorldPoint(worldCoordinate);
          //textActor->SetPosition(worldCoordinate[0],worldCoordinate[1],worldCoordinate[2]);       
          }
        else
          {
          textActor->SetDisplayPosition(pointXY[0]-100, pointXY[1]+5);
          // convert to world coordinate
          //this->Renderer->SetDisplayPoint(pointXY[0]-100,pointXY[1]+5, 0);
          //this->Renderer->DisplayToWorld();
          //this->Renderer->GetWorldPoint(worldCoordinate);
          //textActor->SetPosition(worldCoordinate[0],worldCoordinate[1],worldCoordinate[2]);
          }

        
        // flip vertically or horizontally the text actor
        if (this->HorizontalFlipped)
          { 
          //textActor->GetTextProperty()->SetJustificationToLeft();
          //textActor->SetOrientation(180);
          //textActor->GetTextProperty()->SetVerticalJustificationToBottom();
          //textActor->SetOrientation(180);
          char *revText = vtkPerkStationModuleLogic::strrev(text, strlen(text));
          textActor->SetInput(revText);
          //textActor->RotateY(180);
          }
        else if (this->VerticalFlipped)
          {
          textActor->GetTextProperty()->SetVerticalJustificationToTop();
          textActor->SetOrientation(180);
          //textActor->RotateX(180);
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

      

    inPt[0] = rasTarget[0];
    inPt[1] = rasTarget[1];
    inPt[2] = rasTarget[2];
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


    // add to actor collection
    this->DepthPerceptionLines->AddItem(lineActor);
    this->Renderer->AddActor(lineActor);


    // set up the measurement line/cylinder
    // find the point where to place the measurement cylinder, either in the right or left, but in the middle in y-axis
    double xyMeasuringLineDock[2];

    if (denom>=0)
    {
    xyMeasuringLineDock[0] = this->MonitorPixelResolution[0]-100;   
    }
    else
    {    
    xyMeasuringLineDock[0] = 100;
    } 

    xyMeasuringLineDock[1] = this->MonitorPixelResolution[1]/2;

    vtkCylinderSource *needleMeasure = vtkCylinderSource::New();    
    needleMeasure->SetHeight(this->MeasureNeedleLengthInWorldCoordinates ); 
    needleMeasure->SetRadius( 0.005 );  
    needleMeasure->SetResolution( 20 );

    // map
    vtkPolyDataMapper *measureMapper = vtkPolyDataMapper::New();  
    measureMapper->SetInputConnection(needleMeasure->GetOutputPort());

    // actor
    vtkActor *measureLineActor = vtkActor::New();
    measureLineActor->SetMapper(measureMapper );  
    // set the correct display position

    // convert to world coordinate
    this->Renderer->SetDisplayPoint(xyMeasuringLineDock[0],xyMeasuringLineDock[1], 0);
    this->Renderer->DisplayToWorld();
    this->Renderer->GetWorldPoint(worldCoordinate);

    measureLineActor->SetPosition(worldCoordinate[0], worldCoordinate[1], worldCoordinate[2]);
 
     // add to actor collection
    this->DepthPerceptionLines->AddItem(measureLineActor);
    this->Renderer->AddActor(measureLineActor);


    lineMapper->Delete();
    lineActor->Delete();

    int actorCount = this->Renderer->VisibleActorCount();
    this->DepthLinesInitialized = true;

    if (this->DeviceActive && this->DisplayInitialized)
      {   
      this->RenderWindow->Render();
      }

  }

}
//------------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::RemoveDepthPerceptionLines()
{
  if(!this->DepthLinesInitialized)
      return;

  int actorCount = this->Renderer->VisibleActorCount();

  vtkActorCollection *collection = this->Renderer->GetActors();
  
  for(int i = 0; i < this->DepthPerceptionLines->GetNumberOfItems(); i++)
    {
    vtkActor *lineActor = vtkActor::SafeDownCast(this->DepthPerceptionLines->GetItemAsObject(i));
    if (collection->IsItemPresent(lineActor))
      {
      this->Renderer->RemoveActor(lineActor);
      }   
    }

  this->DepthPerceptionLines->RemoveAllItems();
  this->DepthLinesInitialized = false;

  this->RemoveTextActors();


 
  if (this->DeviceActive && this->DisplayInitialized)
    this->RenderWindow->Render();
}
//--------------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::RemoveTextActors()
{
  int actorCount = this->Renderer->VisibleActorCount();

  //vtkActorCollection *collection = this->Renderer->GetActors();
  
  // remove text actors
  for(int i = 0; i < this->TextActorsCollection->GetNumberOfItems(); i++)
    {
    vtkTextActor *textActor = vtkTextActor::SafeDownCast(this->TextActorsCollection->GetItemAsObject(i));
    //if (collection->IsItemPresent(textActor))
      {
      this->Renderer->RemoveActor(textActor);
      }   
    }

  this->TextActorsCollection->RemoveAllItems();
}

//-------------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::ResetTilt()
{
  this->ResliceTransform->GetMatrix()->DeepCopy(this->SystemStateResliceMatrix);
  this->XYToIJK->DeepCopy(this->SystemStateXYToIJK);
  
  //update xyToRAS
  vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
  this->VolumeNode->GetIJKToRASMatrix(ijkToRAS);
  vtkMatrix4x4::Multiply4x4(ijkToRAS, this->XYToIJK, this->XYToRAS);

  vtkSlicerSliceLogic *sliceLogic = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Red")->GetLogic();
  this->UpdateImageDataOnSliceOffset(sliceLogic->GetSliceOffset());

  this->UpdateImageDisplay();
}
//-------------------------------------------------------------------------------
/*void vtkPerkStationSecondaryMonitor::JumpSliceByOffsetting(double r, double a, double s)
{
  vtkMatrix4x4 *sliceToRAS = this->GetSliceToRAS();
  double sr = sliceToRAS->GetElement(0, 3);
  double sa = sliceToRAS->GetElement(1, 3);
  double ss = sliceToRAS->GetElement(2, 3);

  // deduce the slice spacing
  vtkMatrix4x4 *xyzToRAS = this->GetXYToRAS();

  double p1xyz[4] = {0.0,0.0,0.0,1.0};
  double p2xyz[4] = {0.0,0.0,1.0,1.0};

  double p1ras[4], p2ras[4];

  xyzToRAS->MultiplyPoint(p1xyz, p1ras);
  xyzToRAS->MultiplyPoint(p2xyz, p2ras);

  double sliceSpacing = sqrt(vtkMath::Distance2BetweenPoints(p2ras, p1ras));
  
  double d;
  d = (r-sr)*sliceToRAS->GetElement(0,2)
      + (a-sa)*sliceToRAS->GetElement(1,2)
      + (s-ss)*sliceToRAS->GetElement(2,2);
  sr += (d - this->ActiveSlice*sliceSpacing)*sliceToRAS->GetElement(0,2);
  sa += (d - this->ActiveSlice*sliceSpacing)*sliceToRAS->GetElement(1,2);
  ss += (d - this->ActiveSlice*sliceSpacing)*sliceToRAS->GetElement(2,2);
  
  sliceToRAS->SetElement( 0, 3, sr );
  sliceToRAS->SetElement( 1, 3, sa );
  sliceToRAS->SetElement( 2, 3, ss );
  this->UpdateMatrices();
}
*/
