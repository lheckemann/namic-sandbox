/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkMTIPlanGUI.h"
#include "vtkMTIPlanLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"
#include "vtkSmartPointer.h"
#include "vtkLineSource.h"
#include "vtkSphereSource.h"
#include "vtkTubeFilter.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLModelDisplayNode.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"

#include "vtkKWPushButton.h"

#include "vtkCornerAnnotation.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkMTIPlanGUI );
vtkCxxRevisionMacro ( vtkMTIPlanGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkMTIPlanGUI::vtkMTIPlanGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkMTIPlanGUI::DataCallback);

  //----------------------------------------------------------------
  // GUI widgets

  this->SelectXMLFileButton = NULL;


  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkMTIPlanGUI::~vtkMTIPlanGUI ( )
{

  //----------------------------------------------------------------
  // Remove Callbacks

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  //----------------------------------------------------------------
  // Remove Observers

  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // Remove GUI widgets


  if(this->SelectXMLFileButton)
    {
    this->SelectXMLFileButton->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkMTIPlanGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkMTIPlanGUI::Enter()
{
  if (this->TimerFlag == 0)
    {
    this->TimerFlag     = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }

}


//---------------------------------------------------------------------------
void vtkMTIPlanGUI::Exit ( )
{
}


//---------------------------------------------------------------------------
void vtkMTIPlanGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "MTIPlanGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkMTIPlanGUI::RemoveGUIObservers ( )
{
  this->RemoveLogicObservers();

  if(this->SelectXMLFileButton)
    {
    this->SelectXMLFileButton->GetWidget()->GetLoadSaveDialog()->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

}


//---------------------------------------------------------------------------
void vtkMTIPlanGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // MRML

  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);

  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }

  //----------------------------------------------------------------
  // GUI Observers

  if(this->SelectXMLFileButton)
    {
    this->SelectXMLFileButton->GetWidget()->GetLoadSaveDialog()->AddObserver(vtkKWLoadSaveDialog::FileNameChangedEvent, (vtkCommand*)this->GUICallbackCommand);
    }


  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkMTIPlanGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkMTIPlanGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkMTIPlanLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkMTIPlanGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkMTIPlanGUI::ProcessGUIEvents(vtkObject     *caller,
                                     unsigned long  event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  if(this->SelectXMLFileButton->GetWidget()->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) &&
     event == vtkKWLoadSaveDialog::FileNameChangedEvent)
    {
    this->GetLogic()->ParseMTIPlanFile(this->SelectXMLFileButton->GetWidget()->GetFileName());
    if(this->GetLogic()->GetReadyToDraw())
      {
      this->DrawTrajectories(this->GetLogic()->vTrajectories);
      }
    }
}

//---------------------------------------------------------------------------

void vtkMTIPlanGUI::DataCallback(vtkObject     *caller,
                                 unsigned long  eid, void *clientData, void *callData)
{
  vtkMTIPlanGUI *self = reinterpret_cast<vtkMTIPlanGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkMTIPlanGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkMTIPlanGUI::ProcessLogicEvents ( vtkObject     *caller,
                                         unsigned long  event, void *callData )
{

  if (this->GetLogic() == vtkMTIPlanLogic::SafeDownCast(caller))
    {
    if (event == vtkMTIPlanLogic::StatusUpdateEvent)
      {
      }
    }
}


//---------------------------------------------------------------------------
void vtkMTIPlanGUI::ProcessMRMLEvents ( vtkObject     *caller,
                                        unsigned long  event, void *callData )
{
  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkMTIPlanGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(),
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");
    }
}


//---------------------------------------------------------------------------
void vtkMTIPlanGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME
  // create a page
  this->UIPanel->AddPage ( "MTIPlan", "MTIPlan", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForLoadingTrajectory();
}

//---------------------------------------------------------------------------
void vtkMTIPlanGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help  =
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:MTIPlan</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "MTIPlan" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkMTIPlanGUI::BuildGUIForLoadingTrajectory()
{

  vtkSlicerApplication *app  = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget          *page = this->UIPanel->GetPageWidget ("MTIPlan");

  vtkSmartPointer<vtkSlicerModuleCollapsibleFrame> conBrowsFrame = vtkSmartPointer<vtkSlicerModuleCollapsibleFrame>::New();
  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Load Multi-Trajectory Interventional Plan");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  this->SelectXMLFileButton = vtkKWLoadSaveButtonWithLabel::New();
  this->SelectXMLFileButton->SetParent(conBrowsFrame->GetFrame());
  this->SelectXMLFileButton->Create();
  this->SelectXMLFileButton->GetWidget()->SetText ("Multi-Trajectory Interventional Plan file");
  this->SelectXMLFileButton->GetWidget()->GetLoadSaveDialog()->SetFileTypes("{ {Multi-Trajectory Interventional Plan} {*.xml} }");
  this->SelectXMLFileButton->GetWidget()->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
  this->SelectXMLFileButton->SetEnabled(1);

  this->Script("pack %s -side top -anchor w -expand y -fill x -padx 2 -pady 2",
               this->SelectXMLFileButton->GetWidgetName());

}

//----------------------------------------------------------------------------
void vtkMTIPlanGUI::UpdateAll()
{
}

//----------------------------------------------------------------------------
void vtkMTIPlanGUI::DrawTrajectories(std::vector<vtkMTIPlanLogic::Trajectory> vTraj)
{
  for(unsigned int i = 0; i < vTraj.size(); i++)
    {
    vtkSmartPointer<vtkLineSource> line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoint1(vTraj[i].EntryPoint);
    line->SetPoint2(vTraj[i].Target);
    line->SetResolution(10);

    vtkSmartPointer<vtkTubeFilter> tube = vtkSmartPointer<vtkTubeFilter>::New();
    tube->SetInput(line->GetOutput());
    tube->SetVaryRadius(0);
    tube->SetRadius(1);
    tube->SetNumberOfSides(50);
    tube->CappingOn();

    vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
    sphere->SetCenter(vTraj[i].Target);
    sphere->SetRadius(15);
    sphere->SetThetaResolution(20);
    sphere->SetPhiResolution(20);
    
    vtkSmartPointer<vtkMRMLModelNode> TubeModel = vtkSmartPointer<vtkMRMLModelNode>::New();
    TubeModel->SetScene(this->GetMRMLScene());
    TubeModel->SetAndObservePolyData(tube->GetOutput());
    TubeModel->SetModifiedSinceRead(1);
    this->GetMRMLScene()->AddNode(TubeModel);

    vtkSmartPointer<vtkMRMLModelDisplayNode> TubeDisplayNode = vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
    TubeDisplayNode->SetPolyData(TubeModel->GetPolyData());
    TubeDisplayNode->SetColor(1,1,0);
    TubeDisplayNode->SetVisibility(1);
    TubeDisplayNode->SetSliceIntersectionVisibility(1);
    this->GetMRMLScene()->AddNode(TubeDisplayNode);

    TubeModel->SetAndObserveDisplayNodeID(TubeDisplayNode->GetID());

    vtkSmartPointer<vtkMRMLModelNode> SphereModel = vtkSmartPointer<vtkMRMLModelNode>::New();
    SphereModel->SetScene(this->GetMRMLScene());
    SphereModel->SetAndObservePolyData(sphere->GetOutput());
    SphereModel->SetModifiedSinceRead(1);
    this->GetMRMLScene()->AddNode(SphereModel);

    vtkSmartPointer<vtkMRMLModelDisplayNode> SphereDisplayNode = vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
    SphereDisplayNode->SetPolyData(SphereModel->GetPolyData());
    SphereDisplayNode->SetColor(0,1,0);
    SphereDisplayNode->SetOpacity(0.5);
    SphereDisplayNode->SetVisibility(1);
    SphereDisplayNode->SetSliceIntersectionVisibility(1);
    this->GetMRMLScene()->AddNode(SphereDisplayNode);

    SphereModel->SetAndObserveDisplayNodeID(SphereDisplayNode->GetID());

    }
}

