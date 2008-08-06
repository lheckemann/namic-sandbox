#include "vtkPerkStationSecondaryMonitor.h"
#include "vtkPerkStationModuleGUI.h"
#include "vtkMRMLPerkStationModuleNode.h"

#include "vtkRenderer.h"
#include "vtkWin32OpenGLRenderWindow.h"
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
  this->MonitorSpacing[0] = 1;
  this->MonitorSpacing[1] = 1;
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
 
 
 // interator
 this->Interator = vtkRenderWindowInteractor::New();
 this->Interator->SetRenderWindow(this->RenderWindow);

 vtkSlicerInteractorStyle *iStyle = vtkSlicerInteractorStyle::New();
 this->Interator->SetInteractorStyle (iStyle);

 // mapper
 this->ImageMapper = vtkImageMapper::New();
 this->ImageMapper->SetColorWindow(255);
 this->ImageMapper->SetColorLevel(127.5);

 // actor
 this->ImageActor = vtkActor2D::New();
 this->ImageActor->SetMapper(this->ImageMapper);

}


//----------------------------------------------------------------------------
vtkPerkStationSecondaryMonitor::~vtkPerkStationSecondaryMonitor()
{
  this->SetGUI(NULL);
}


//----------------------------------------------------------------------------
/*void vtkPerkStationSecondaryMonitor::UpdateTransforms()
{
  vtkMatrix4x4 *xyToIJK = vtkMatrix4x4::New();
  xyToIJK->Identity();

  //vtkMatrix4x4::Multiply4x4(this->XYToRAS, xyToIJK, xyToIJK);
  
  vtkMatrix4x4 *rasToIJK = vtkMatrix4x4::New();
  this->VolumeNode->GetRASToIJKMatrix(rasToIJK);
  
  vtkMatrix4x4::Multiply4x4(rasToIJK, xyToIJK, xyToIJK); 
  rasToIJK->Delete();

  vtkMatrix4x4 *ijkToRAS = vtkMatrix4x4::New();
  this->VolumeNode->GetIJKToRASMatrix(ijkToRAS);

  vtkMatrix4x4 *tmpMatrix = vtkMatrix4x4::New();
  tmpMatrix->SetElement(1,1, -1.0);
  tmpMatrix->SetElement(1,3, 255);
  vtkMatrix4x4::Multiply4x4(tmpMatrix, this->XYToSlice, this->XYToSlice); 
  vtkMatrix4x4::Multiply4x4(ijkToRAS, this->XYToSlice, this->XYToRAS); 

  
  xyToIJK->SetElement(1,1,-1.0);
  xyToIJK->SetElement(1,3,255);
  this->XYToIJKTransform->SetMatrix( xyToIJK );
  xyToIJK->Delete();
  
  this->UpdateImageDisplay();

  
}*/

//----------------------------------------------------------------------------
 void vtkPerkStationSecondaryMonitor::SetupImageData()
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }

  this->VolumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->GetGUI()->GetMRMLScene()->GetNodeByID(mrmlNode->GetInputVolumeRef()));
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
  // mapper
  this->ImageMapper->SetInput(this->Reslice->GetOutput());
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

  // to have consistent display i.e. same display as in SLICER's slice viewer, and own render window
 // figure out whether a horizontal flip required or a vertical flip or both
  vtkMatrix4x4 *directionMatrix = vtkMatrix4x4::New();
  this->VolumeNode->GetIJKToRASDirectionMatrix(directionMatrix);

  vtkMatrix4x4 *flipMatrix = vtkMatrix4x4::New();

  bool verticalFlip = false;
  bool horizontalFlip = false;

  flipMatrix = this->GetFlipMatrixFromDirectionCosines(directionMatrix,verticalFlip,horizontalFlip);
  this->CurrentTransformMatrix->DeepCopy(flipMatrix);

  this->UpdateMatrices(); 
  
  this->Scale(6.1047,6.1217,1);

  this->Translate(0, 26, 0);
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

  this->UpdateImageDisplay();

}
//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::UpdateImageDisplay()
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
        int width_mm = GetDeviceCaps(hdc, HORZSIZE);
        int height_mm = GetDeviceCaps(hdc, VERTSIZE);
         
        int width_pix = GetDeviceCaps(hdc, HORZRES);
        int height_pix = GetDeviceCaps(hdc, VERTRES);
        double xSpacing = double(width_mm)/double(width_pix);
        double ySpacing = double(height_mm)/double(height_pix);

        if (!(lpDisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
          {
          this->MonitorSpacing[0] = xSpacing;
          this->MonitorSpacing[1] = ySpacing;
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
  
  // scale about center
  transform->Translate(-this->ImageSize[0]/2., -this->ImageSize[1]/2., 0);
  transform->RotateZ(angle);
  transform->Translate(this->ImageSize[0]/2., this->ImageSize[1]/2., 0);

  vtkMatrix4x4 *rotateMatrix = vtkMatrix4x4::New();
  rotateMatrix->DeepCopy(transform->GetMatrix());

  this->CurrentTransformMatrix->DeepCopy(rotateMatrix);

  this->UpdateMatrices();
}
//----------------------------------------------------------------------------
void vtkPerkStationSecondaryMonitor::Translate(double tx, double ty, double tz)
{
  // translation components come in here as mm
  // convert into pixels with original spacing

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
