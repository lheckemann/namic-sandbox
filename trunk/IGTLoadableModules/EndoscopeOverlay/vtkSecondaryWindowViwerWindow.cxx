#include "vtkObjectFactory.h"

#include "vtkSecondaryWindowViwerWindow.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerTheme.h"

#include "vtkRenderer.h"

#include "vtkCornerAnnotation.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSecondaryWindowViwerWindow);
vtkCxxRevisionMacro(vtkSecondaryWindowViwerWindow, "$Revision: 1.0 $");
//----------------------------------------------------------------------------
vtkSecondaryWindowViwerWindow::vtkSecondaryWindowViwerWindow()
{
  this->MainFrame = vtkKWFrame::New();
  this->ViewerWidget = vtkSlicerViewerWidget::New();
  this->MultipleMonitorsAvailable = false; 
  this->SecondaryMonitorPosition[0]=0;
  this->SecondaryMonitorPosition[1]=0;
  this->SecondaryMonitorSize[0]=0;
  this->SecondaryMonitorSize[1]=0;

  //this->CurrentTarget=NULL;
}


//----------------------------------------------------------------------------
vtkSecondaryWindowViwerWindow::~vtkSecondaryWindowViwerWindow()
{
  //if (this->CurrentTarget)
  //{
  //  this->CurrentTarget->UnRegister(this);
  //  this->CurrentTarget=NULL;
  //}
  this->ViewerWidget->RemoveAllObservers();
  this->ViewerWidget->SetApplicationLogic (NULL);
  this->ViewerWidget->Unpack();  
  this->ViewerWidget->SetParent(NULL);
  this->ViewerWidget->Delete();   
  this->MainFrame->Delete();
  this->SetApplication(NULL);
}

//----------------------------------------------------------------------------
void vtkSecondaryWindowViwerWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

#ifdef _WIN32
//----------------------------------------------------------------------------
BOOL CALLBACK StoreNonPrimaryMonitorRectEnumProc(HMONITOR hMonitor, HDC hdc, LPRECT prc, LPARAM dwData) 
{
  MONITORINFO mi;
  mi.cbSize = sizeof(MONITORINFO);
  if (GetMonitorInfo(hMonitor, &mi))
  {
    if (!(mi.dwFlags & MONITORINFOF_PRIMARY))
    {
      RECT *rect = (RECT*)dwData;
      if (rect==NULL)
      {
        return false;
      }
      // store the rect of the non-primary monitor only
      rect->left=mi.rcMonitor.left;
      rect->right=mi.rcMonitor.right;
      rect->top=mi.rcMonitor.top;
      rect->bottom=mi.rcMonitor.bottom;     
    }
  }
  return true;
} 

//----------------------------------------------------------------------------
void vtkSecondaryWindowViwerWindow::UpdateSecondaryMonitorPoisition()
{
  // Window rect
  RECT rect;    

  // Fill rect by default with primary monitor info
  rect.left=0;
  rect.right=GetSystemMetrics(SM_CXFULLSCREEN);
  rect.top=0;
  rect.bottom=GetSystemMetrics(SM_CYFULLSCREEN);    

  // if a secondary monitor is available then get its virtual screen coordinates
  int nMonitors = GetSystemMetrics(SM_CMONITORS);
  if (nMonitors>1)
  {
    this->MultipleMonitorsAvailable = true; 
    if (!EnumDisplayMonitors(NULL, NULL, StoreNonPrimaryMonitorRectEnumProc, (LPARAM) &rect))
    {
      vtkErrorMacro("DetectMonitors: EnumDisplayMonitors failed");
    }
  }
  else
  {
    this->MultipleMonitorsAvailable = false; 
  }

  this->SecondaryMonitorPosition[0]=rect.left;
  this->SecondaryMonitorPosition[1]=rect.top;
  this->SecondaryMonitorSize[0]=rect.right-rect.left;
  this->SecondaryMonitorSize[1]=rect.bottom-rect.top;
}

#else // _WIN32

void vtkSecondaryWindowViwerWindow::UpdateSecondaryMonitorPoisition()
{
  // TODO: implement monitor detection for linux
  this->MultipleMonitorsAvailable = false; 
  this->SecondaryMonitorPosition[0]=0;
  this->SecondaryMonitorPosition[1]=0;
  this->SecondaryMonitorSize[0]=1024;
  this->SecondaryMonitorSize[1]=768;
}

#endif // _WIN32

//----------------------------------------------------------------------------
void vtkSecondaryWindowViwerWindow::CreateWidget()
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

  //this->SetMasterWindow (this->GetServerMenuButton() );
  this->SetApplication ( app );
  //this->Create();
  this->SetBorderWidth ( 1 );
  this->SetReliefToFlat();

  //this->SetParent (this->GetApplicationGUI()->GetMainSlicerWindow());

  this->SetTitle ("3D Slicer -- Secondary Window");
  this->SetSize (450, 100);
  this->Withdraw();
  //this->SetDeleteWindowProtocolCommand ( this, "DestroySecondaryMonitorWindow");

  this->MainFrame->SetParent ( this );
  this->MainFrame->Create();
  this->MainFrame->SetBorderWidth ( 1 );
  this->Script ( "pack %s -side top -anchor nw -fill both -expand 1 -padx 0 -pady 1", this->MainFrame->GetWidgetName() ); 
  

  //
  // Make 3D Viewer
  //
  this->ViewerWidget->SetApplication( app );
  this->ViewerWidget->SetParent(this->MainFrame);

  // add events (for ViewerWidget and FiducialListWidget)
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  events->InsertNextValue(vtkMRMLScene::NewSceneEvent);
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkCommand::ModifiedEvent);


  // Create view node
  vtkSmartPointer<vtkMRMLViewNode> viewNode = vtkSmartPointer<vtkMRMLViewNode>::New();
  viewNode->SetName(app->GetApplicationGUI()->GetMRMLScene()->GetUniqueNameByString("SecondaryWindow"));  
  
  // Don't show the viewer widget in the main viewer (it is important because having multiple widgets for the same view node
  // causes hang in camera clipping range computation). We will create the viewer widget for showing it in our secondary viewer window.
  viewNode->SetVisibility(false);

  // Don't show the view node for the user
  viewNode->SetHideFromEditors(true);
  app->GetApplicationGUI()->GetMRMLScene()->AddNode(viewNode);  


  this->ViewerWidget->Create();
  this->ViewerWidget->GetMainViewer()->SetRendererBackgroundColor (app->GetSlicerTheme()->GetSlicerColors()->ViewerBlue );
  this->ViewerWidget->SetAndObserveMRMLSceneEvents (app->GetApplicationGUI()->GetMRMLScene(), events );
  this->ViewerWidget->UpdateFromMRML();
  this->ViewerWidget->SetApplicationLogic ( app->GetApplicationGUI()->GetApplicationLogic() );
  this->ViewerWidget->SetAndObserveViewNode (viewNode);

  this->ViewerWidget->PackWidget(this->MainFrame);
}

//----------------------------------------------------------------------------
void vtkSecondaryWindowViwerWindow::DisplayOnSecondaryMonitor()
{
  //-- display
  this->DeIconify();
  this->Raise();

  this->UpdateSecondaryMonitorPoisition();

  this->SetPosition(this->SecondaryMonitorPosition[0], this->SecondaryMonitorPosition[1]);
  this->SetSize(this->SecondaryMonitorSize[0], this->SecondaryMonitorSize[1]);

  if (this->MultipleMonitorsAvailable)
  {
    this->HideDecorationOn();
  }

}

//----------------------------------------------------------------------------
void vtkSecondaryWindowViwerWindow::SetCameraNode(vtkMRMLCameraNode* node)
{
  if (node)
    {
    vtkMRMLViewNode* vnode = this->ViewerWidget->GetViewNode();
    if (vnode)
      {
      node->SetActiveTag(vnode->GetID());
      }
    this->ViewerWidget->SetAndObserveCameraNode (node);
    this->ViewerWidget->RequestRender();
    }
}
