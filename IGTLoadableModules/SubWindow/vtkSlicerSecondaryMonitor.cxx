#include "vtkObjectFactory.h"

#include "vtkSlicerSecondaryMonitor.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerTheme.h"

#include "vtkCornerAnnotation.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerSecondaryMonitor);
vtkCxxRevisionMacro(vtkSlicerSecondaryMonitor, "$Revision: 1.0 $");
//----------------------------------------------------------------------------
vtkSlicerSecondaryMonitor::vtkSlicerSecondaryMonitor()
{
  this->MainFrame = vtkKWFrame::New();
  //this->ViewerWidget=vtkTRProstateBiopsyViewerWidget::New();
  this->FiducialListWidget=vtkSlicerFiducialListWidget::New();
  //this->ROIViewerWidget=vtkSlicerROIViewerWidget::New();
  //this->RobotViewerWidget=vtkTRProstateBiopsyRobotWidget::New();
  this->MultipleMonitorsAvailable = false; 
  this->SecondaryMonitorPosition[0]=0;
  this->SecondaryMonitorPosition[1]=0;
  this->SecondaryMonitorSize[0]=0;
  this->SecondaryMonitorSize[1]=0;

  this->CurrentTarget=NULL;
}


//----------------------------------------------------------------------------
vtkSlicerSecondaryMonitor::~vtkSlicerSecondaryMonitor()
{
  if (this->CurrentTarget)
  {
    this->CurrentTarget->UnRegister(this);
    this->CurrentTarget=NULL;
  }
  //this->ViewerWidget->RemoveAllObservers();
  this->FiducialListWidget->RemoveAllObservers();
  //this->ViewerWidget->SetApplicationLogic (NULL);
  //this->ViewerWidget->Unpack();  
  this->FiducialListWidget->SetParent(NULL);
  this->FiducialListWidget->Delete();
  //this->ROIViewerWidget->SetParent(NULL);
  //this->ROIViewerWidget->Delete();
  //this->RobotViewerWidget->SetParent(NULL);
  //this->RobotViewerWidget->Delete();
  this->ViewerWidget->SetParent(NULL);
  this->ViewerWidget->SetApplicationLogic(NULL);
  this->ViewerWidget->Delete();   
  this->MainFrame->Delete();
  this->SetApplication(NULL);
}

//----------------------------------------------------------------------------
void vtkSlicerSecondaryMonitor::PrintSelf(ostream& os, vtkIndent indent)
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
void vtkSlicerSecondaryMonitor::UpdateSecondaryMonitorPoisition()
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

void vtkSlicerSecondaryMonitor::UpdateSecondaryMonitorPoisition()
{
  // TODO: implement monitor detection for linux
  this->MultipleMonitorsAvailable = false; 
  this->VirtualScreenPosition[0]=0;
  this->VirtualScreenPosition[1]=0;
  this->VirtualScreenSize[0]=1024;
  this->VirtualScreenSize[1]=768;
}

#endif // _WIN32

//----------------------------------------------------------------------------
void vtkSlicerSecondaryMonitor::CreateWidget()
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

  this->SetTitle ("TRProstateBiopsy planning");
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

  // use the events for the fiducial list widget as well

  this->ViewerWidget->Create();
  this->ViewerWidget->GetMainViewer()->SetRendererBackgroundColor (app->GetSlicerTheme()->GetSlicerColors()->ViewerBlue );
  this->ViewerWidget->SetAndObserveMRMLSceneEvents (app->GetApplicationGUI()->GetMRMLScene(), events );
  this->ViewerWidget->UpdateFromMRML();
  this->ViewerWidget->SetApplicationLogic ( app->GetApplicationGUI()->GetApplicationLogic() );
  //this->ViewerWidget->GetMainViewer()->SetRendererBackgroundColor ( app->GetSlicerTheme()->GetSlicerColors()->Black );
  //this->ViewerWidget->ColorAxisLabelActors (1.0, 1.0, 1.0);
  
  // add the fiducial list widget  
  this->FiducialListWidget->SetApplication( app );
  this->FiducialListWidget->SetViewerWidget(this->ViewerWidget);
  this->FiducialListWidget->SetInteractorStyle(vtkSlicerViewerInteractorStyle::SafeDownCast(this->ViewerWidget->GetMainViewer()->GetRenderWindowInteractor()->GetInteractorStyle()));
  this->FiducialListWidget->Create();
  this->FiducialListWidget->SetAndObserveMRMLSceneEvents (app->GetApplicationGUI()->GetMRMLScene(), events );
  this->FiducialListWidget->UpdateFromMRML();

  // add the roi widget
  /*this->ROIViewerWidget->SetApplication( app );
  this->ROIViewerWidget->SetMainViewerWidget(this->ViewerWidget);
  this->ROIViewerWidget->SetMRMLScene(app->GetApplicationGUI()->GetMRMLScene());
  this->ROIViewerWidget->Create();
  this->ROIViewerWidget->UpdateFromMRML();*/

  // add the robot viewer widget
  this->RobotViewerWidget->SetApplication( app );
  this->RobotViewerWidget->SetMainViewerWidget(this->ViewerWidget);
  this->RobotViewerWidget->SetMRMLScene(app->GetApplicationGUI()->GetMRMLScene());
  this->RobotViewerWidget->Create();
  this->RobotViewerWidget->UpdateFromMRML();

  this->ViewerWidget->PackWidget(this->MainFrame);
}

//----------------------------------------------------------------------------
void vtkSlicerSecondaryMonitor::DisplayOnSecondaryMonitor()
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


void vtkSlicerSecondaryMonitor::SetCurrentTarget(vtkTRProstateBiopsyTargetDescriptor* target)
{
  if (this->CurrentTarget!=target)
  {
    if (this->CurrentTarget)
    {
      this->CurrentTarget->UnRegister(this);
    }
    this->CurrentTarget=target;
    if (this->CurrentTarget)
    {
      this->CurrentTarget->Register(this);
    }
  }
  
  vtkCornerAnnotation *anno = this->ViewerWidget->GetMainViewer()->GetCornerAnnotation();
  if (anno)
    {
    static double lsf=10.0;
    static double nlsf=0.35;
    anno->SetLinearFontScaleFactor(lsf);
    anno->SetNonlinearFontScaleFactor(nlsf);
    anno->SetText(1, "Encoders not connected");

    if (this->CurrentTarget!=NULL)
    {
      std::ostrstream os;        
      os << this->CurrentTarget->GetFiducialIndex()+1 << ". " << this->CurrentTarget->GetNeedleTypeString() << "   "<<this->CurrentTarget->GetRASLocationString().c_str()<<std::endl;
      os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(1);      
      os << "Rotation: "<<this->CurrentTarget->GetAxisRotation()<<" deg"<<std::endl;
      os << "Reachable: "<<this->CurrentTarget->GetReachableString().c_str()<<std::endl;
      os << "Needle angle: "<<this->CurrentTarget->GetNeedleAngle()<<" deg"<<std::endl;
      os << "Depth: "<<this->CurrentTarget->GetDepthCM()<<" cm"<<std::endl;            
      os << std::ends;
      anno->SetText(3, os.str());
      os.rdbuf()->freeze();
    }
    else
    {
      anno->SetText(3, "No target");
    }

    this->ViewerWidget->GetMainViewer()->CornerAnnotationVisibilityOn();
    // update the annotations
    this->ViewerWidget->GetMainViewer()->Render();
    } 
}
