#include "vtkTRProstateBiopsySecondaryMonitor.h"
#include "vtkTRProstateBiopsyGUI.h"


#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkWin32OpenGLRenderWindow.h"
#include "vtkCylinderSource.h"
#include "vtkMath.h"
#include "vtkMatrixToHomogeneousTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkProperty2D.h"
#include "vtkActorCollection.h"
//----------------------------------------------------------------------------
vtkTRProstateBiopsySecondaryMonitor* vtkTRProstateBiopsySecondaryMonitor::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTRProstateBiopsySecondaryMonitor");
  if(ret)
    {
      return (vtkTRProstateBiopsySecondaryMonitor*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkTRProstateBiopsySecondaryMonitor;
}
//----------------------------------------------------------------------------
vtkTRProstateBiopsySecondaryMonitor::vtkTRProstateBiopsySecondaryMonitor()
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
  

  // display/view related
  this->ImageData = NULL;
  this->VolumeNode = NULL;
 
  // matrices
  this->XYToIJK = vtkMatrix4x4::New();
  this->XYToRAS = vtkMatrix4x4::New();
  this->CurrentTransformMatrix = vtkMatrix4x4::New();
  this->ResliceTransform = vtkTransform::New();
  
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
 
 // interator
 this->Interator = vtkRenderWindowInteractor::New();
 this->Interator->SetRenderWindow(this->RenderWindow);

 vtkSlicerInteractorStyle *iStyle = vtkSlicerInteractorStyle::New();
 this->Interator->SetInteractorStyle (iStyle);

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

 this->NeedleActor = vtkActor::New();  
}


//----------------------------------------------------------------------------
vtkTRProstateBiopsySecondaryMonitor::~vtkTRProstateBiopsySecondaryMonitor()
{
  this->SetGUI(NULL);
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsySecondaryMonitor::ResetCalibration()
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
void vtkTRProstateBiopsySecondaryMonitor::RemoveOverlayNeedleGuide()
{
  if (!this->DisplayInitialized)
      return;

  // should remove the overlay needle guide
  vtkActorCollection *collection = this->Renderer->GetActors();
  if (collection->IsItemPresent(this->NeedleActor))
    {
    this->Renderer->RemoveActor(this->NeedleActor);
    if (this->DeviceActive)
        this->RenderWindow->Render();
    }

}
//----------------------------------------------------------------------------
 void vtkTRProstateBiopsySecondaryMonitor::SetupImageData()
{
   
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
  this->Renderer->AddActor(this->ImageActor);

  // matrix stuff  

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

  // scale up to original size
  // add code here


  this->UpdateMatrices();  
 
  this->DisplayInitialized = true;
}
//----------------------------------------------------------------------------
vtkMatrix4x4 *vtkTRProstateBiopsySecondaryMonitor::GetFlipMatrixFromDirectionCosines (vtkMatrix4x4 *directionMatrix, bool & verticalFlip, bool & horizontalFlip)
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
void vtkTRProstateBiopsySecondaryMonitor::UpdateMatrices()
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

  this->UpdateImageDisplay();

}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsySecondaryMonitor::UpdateImageDisplay()
{
  this->Reslice->SetOutputExtent(0,this->ScreenSize[0]-1,0,this->ScreenSize[1]-1,0,0);
  this->Reslice->SetInput( this->ImageData);
  this->Reslice->SetResliceTransform( this->ResliceTransform );
  this->Reslice->Update();
  if (this->DeviceActive)
    {
    this->RenderWindow->Render();  
    }
  

  /*vtkSlicerSliceLogic *sliceLogic = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI0()->GetLogic();
  vtkMRMLSliceNode *sliceNode = sliceLogic->GetSliceNode();

  vtkMatrix4x4 *xyslice = sliceNode->GetXYToSlice();
  vtkMatrix4x4 *sliceRas = sliceNode->GetSliceToRAS();
  vtkMatrix4x4 *xyRas = sliceNode->GetXYToRAS();
  */
  
    
}
//----------------------------------------------------------------------------
BOOL CALLBACK MyInfoEnumProc(HMONITOR hMonitor, HDC hdc, LPRECT prc, LPARAM dwData) 
{
  vtkTRProstateBiopsySecondaryMonitor *self = (vtkTRProstateBiopsySecondaryMonitor *)dwData;
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
void vtkTRProstateBiopsySecondaryMonitor::Initialize()
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
         /* this->MonitorPhysicalSizeMM[0] = width_mm;
          this->MonitorPhysicalSizeMM[1] = height_mm;
          this->MonitorPixelResolution[0] = width_pix;
          this->MonitorPixelResolution[1] = height_pix;*/
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
void vtkTRProstateBiopsySecondaryMonitor::Scale(double sx, double sy, double sz)
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
