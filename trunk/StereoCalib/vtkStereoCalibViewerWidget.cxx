

#include "vtkStereoCalibViewerWidget.h"
#include "vtkRenderer.h"
#include "vtkSlicerViewerWidget.h"
#include "vtkSlicerApplication.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkStereoCalibViewerWidget);
vtkCxxRevisionMacro(vtkStereoCalibViewerWidget, "$Revision: 1.0 $");

vtkStereoCalibViewerWidget::vtkStereoCalibViewerWidget()
{
  this->MainFrame = vtkKWFrame::New();
  // this->ViewerWidget = vtkKWRenderWidget::New(); //
  this->lw = vtkKWRenderWidget::New();              // 100603-komura
  this->rw = vtkKWRenderWidget::New();
  this->SecondaryMonitorSize[0]=0;
  this->SecondaryMonitorSize[1]=0;
}
vtkStereoCalibViewerWidget::~vtkStereoCalibViewerWidget()
{
  this->MainFrame->Delete();
  // this->ViewerWidget->Delete();    // 
  this->lw->Delete();            // 100603-komura
  this->rw->Delete();
  this->MainFrame->Delete();
  this->SetApplication(NULL);
}


//----------------------------------------------------------------------------
void vtkStereoCalibViewerWidget::CreateWidget()
{
  vtkSlicerApplication *app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  if (app==NULL)
  {
    vtkErrorMacro("CreateWindow: application is invalid");
    return;
  }

  if (this->IsCreated())
  {
    return;
  }

  vtkKWTopLevel::CreateWidget();

  this->SetApplication ( app );
  this->SetBorderWidth ( 1 );
  this->SetReliefToFlat();
        this->SetTitle ("3D Slicer -- StereoCalib -- ");
        this->SetSize (640, 480);
  this->Withdraw();

  this->MainFrame->SetParent ( this );
  this->MainFrame->Create();
  this->MainFrame->SetBorderWidth ( 1 );

  // this->ViewerWidget->SetApplication( app );    //
  this->lw->SetParent(this);            // 
  this->lw->Create();                // 
  this->lw->RemoveInteractionBindings();    // 100603-komura
  this->Script("place %s -relx 0 -rely 0 -anchor nw -relwidth 0.5 -relheight 1", 
             //this->MainFrame->GetWidgetName() );
           this->lw->GetWidgetName());

  this->rw->SetParent(this);
  this->rw->Create();
  this->rw->RemoveInteractionBindings();
  this->Script("place %s -relx 0.5 -rely 0.0 -anchor nw -relwidth 0.5 -relheight 1", 
             this->rw->GetWidgetName());

}

//----------------------------------------------------------------------------
void vtkStereoCalibViewerWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkStereoCalibViewerWidget::UpdateSecondaryMonitorPoisition()
{
  this->SecondaryMonitorSize[0]=1280;    // 5/7/2010 ayamada
  this->SecondaryMonitorSize[1]=480;    // 5/7/2010 ayamada

}
//----------------------------------------------------------------------------
void vtkStereoCalibViewerWidget::DisplayOnSecondaryMonitor()
{
  //-- display
  this->DeIconify();
  this->Raise();

  this->UpdateSecondaryMonitorPoisition();

  this->SetSize(this->SecondaryMonitorSize[0], this->SecondaryMonitorSize[1]);

}
