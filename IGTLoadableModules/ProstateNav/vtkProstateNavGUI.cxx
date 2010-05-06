/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

#include "vtkMRMLRobotNode.h"
#include "vtkMRMLIGTLConnectorNode.h"

#include "vtkMRMLBrpRobotCommandNode.h"
#include "vtkMRMLRobotDisplayNode.h"

#include "vtkProstateNavGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkProstateNavStep.h"
#include "vtkProstateNavStepSetUp.h"
#include "vtkProstateNavStepSetUpTemplate.h"
#include "vtkProstateNavStepVerification.h"
#include "vtkProstateNavCalibrationStep.h"
#include "vtkProstateNavFiducialCalibrationStep.h"
#include "vtkProstateNavTargetingStep.h"
#include "vtkProstateNavStepTargetingTemplate.h"
#include "vtkProstateNavManualControlStep.h"

#include "vtkSlicerFiducialsGUI.h"
#include "vtkSlicerFiducialsLogic.h"

#include "vtkKWRenderWidget.h"
#include "vtkKWWidget.h"
#include "vtkKWMenuButton.h"
#include "vtkKWCheckButton.h"
#include "vtkKWPushButton.h"
#include "vtkKWPushButtonSet.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWLoadSaveDialog.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWMenu.h"
#include "vtkKWLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWEvent.h"
#include "vtkKWOptions.h"

#include "vtkKWTkUtilities.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkCylinderSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkCornerAnnotation.h"
#include "vtkMath.h"

// for Realtime Image
#include "vtkImageChangeInformation.h"
#include "vtkSlicerColorLogic.h"
//#include "vtkSlicerVolumesGUI.h"

#include "vtkCylinderSource.h"
#include "vtkMRMLLinearTransformNode.h"

#include "vtkMRMLProstateNavManagerNode.h"
#include "vtkMRMLTransRectalProstateRobotNode.h"
#include "vtkMRMLTransPerinealProstateRobotNode.h"
#include "vtkMRMLTransPerinealProstateTemplateNode.h"
#include "vtkSlicerSecondaryViewerWindow.h"
#include "vtkSlicerViewerWidget.h"
#include "vtkMRMLViewNode.h"

#include <vector>



//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkProstateNavGUI );
vtkCxxRevisionMacro ( vtkProstateNavGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkProstateNavGUI::vtkProstateNavGUI ( )
{
  
  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkProstateNavGUI::DataCallback);
  
  this->ProstateNavManagerNodeID =  NULL;
  this->ProstateNavManagerNode =  NULL;

  this->TargetPlanListNodeID = NULL;
  this->TargetPlanListNode = NULL;

  this->RobotNodeID = NULL;
  this->RobotNode = NULL;

  //----------------------------------------------------------------
  // Configuration Frame

  this->ProstateNavManagerSelectorWidget = NULL;
  this->RobotSelectorWidget = NULL;

  //----------------------------------------------------------------
  // Workphase Frame
  
  this->StatusButtonFrame = vtkKWFrame::New();
  this->StatusButtonSet = NULL;

  this->WorkphaseButtonFrame = vtkKWFrame::New();
  this->WorkphaseButtonSet = NULL;

  //----------------------------------------------------------------  
  // Wizard Frame
  
  this->WizardFrame = vtkSlicerModuleCollapsibleFrame::New();
  this->WizardWidget = NULL;

  this->DisplayedWorkflowSteps=vtkStringArray::New();

  this->SecondaryWindow=NULL;

  this->Entered = 0;
}



//---------------------------------------------------------------------------
vtkProstateNavGUI::~vtkProstateNavGUI ( )
{
  this->RemoveMRMLObservers();
  this->RemoveGUIObservers();

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    this->DataCallbackCommand=NULL;
    }  

  if (this->SecondaryWindow!=NULL)
  {
    this->SecondaryWindow->SetApplication(NULL);
    this->SecondaryWindow->Delete();
    this->SecondaryWindow=NULL;
  }

  //----------------------------------------------------------------
  // Configuration Frame

  if (this->ProstateNavManagerSelectorWidget)
    {
    this->ProstateNavManagerSelectorWidget->SetParent(NULL );
    this->ProstateNavManagerSelectorWidget->Delete ( );
    this->ProstateNavManagerSelectorWidget=NULL;
    }

  if (this->RobotSelectorWidget)
    {
    this->RobotSelectorWidget->SetParent(NULL );
    this->RobotSelectorWidget->Delete ( );
    this->RobotSelectorWidget=NULL;
    }

  //----------------------------------------------------------------
  // Workphase Frame

  if (this->StatusButtonFrame)
    {
    this->StatusButtonFrame->SetParent(NULL);
    this->StatusButtonFrame->Delete(); 
    this->StatusButtonFrame = NULL;
    }

  if (this->StatusButtonSet)
    {
    this->StatusButtonSet->SetParent(NULL);
    this->StatusButtonSet->Delete();
    this->StatusButtonSet=NULL;
    }

  if (this->WorkphaseButtonFrame)
    {
    this->WorkphaseButtonFrame->SetParent(NULL);
    this->WorkphaseButtonFrame->Delete(); 
    this->WorkphaseButtonFrame = NULL;
    }

  if (this->WorkphaseButtonSet)
    {
    this->WorkphaseButtonSet->SetParent(NULL);
    this->WorkphaseButtonSet->Delete();
    this->WorkphaseButtonSet=NULL;
    }
  this->SetModuleLogic ( NULL );


  //----------------------------------------------------------------
  // Wizard Frame

  if (this->WizardFrame)
    {
    this->WizardFrame->SetParent(NULL);
    this->WizardFrame->Delete(); 
    this->WizardFrame = NULL;
    }

  if (this->WizardWidget)
    {
    this->WizardWidget->SetParent(NULL);
    this->WizardWidget->Delete(); 
    this->WizardWidget = NULL;
    }

  if (this->DisplayedWorkflowSteps)
    {
    this->DisplayedWorkflowSteps->Delete(); 
    this->DisplayedWorkflowSteps = NULL;
    }  

  this->SetAndObserveRobotNodeID( NULL );
  this->SetAndObserveProstateNavManagerNodeID( NULL );
  this->SetAndObserveTargetPlanListNodeID( NULL );
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->vtkObject::PrintSelf ( os, indent );
    
    os << indent << "ProstateNavGUI: " << this->GetClassName ( ) << "\n";
    os << indent << "ProstateNavManager: ";
    if (this->ProstateNavManagerNodeID)
    {
      os << this->ProstateNavManagerNodeID << "\n";
    }
    else
    {
       os << "NULL\n";
    }
    os << indent << "RobotNode: ";
    if (this->RobotNodeID)
    {
      os << this->RobotNodeID << "\n";
    }
    else
    {
       os << "NULL\n";
    }
    os << indent << "TargetPlanListNode: ";
    if (this->TargetPlanListNodeID)
    {
      os << this->TargetPlanListNodeID << "\n";
    }
    else
    {
       os << "NULL\n";
    }
    os << indent << "Logic: " << this->GetLogic ( ) << "\n";    
   
    // print widgets?
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::RemoveGUIObservers ( )
{
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  if (appGUI) 
    {
    appGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()
      ->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    appGUI->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()
      ->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    appGUI->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()
      ->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  
  //----------------------------------------------------------------
  // Configuration Frame

  if (this->ProstateNavManagerSelectorWidget)
    {
    this->ProstateNavManagerSelectorWidget->RemoveObservers ( vtkSlicerNodeSelectorWidget::NodeSelectedEvent,  (vtkCommand *)this->GUICallbackCommand );
    }

  if (this->RobotSelectorWidget)
    {
    this->RobotSelectorWidget->RemoveObservers ( vtkSlicerNodeSelectorWidget::NodeSelectedEvent,  (vtkCommand *)this->GUICallbackCommand );
    }

  
  //----------------------------------------------------------------
  // Workphase Frame

  if (this->WorkphaseButtonSet)
    {
    for (int i = 0; i < this->WorkphaseButtonSet->GetNumberOfWidgets(); i ++)
      {
      this->WorkphaseButtonSet->GetWidget(i)->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
      }
    }
    

  //----------------------------------------------------------------
  // Wizard Frame

  if (this->WizardWidget)
    {
    this->WizardWidget->GetWizardWorkflow()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::RemoveLogicObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // Configuration Frame
  
  this->ProstateNavManagerSelectorWidget->AddObserver ( vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->RobotSelectorWidget->AddObserver ( vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );  

  //----------------------------------------------------------------
  // Workphase Frame

  if (this->WorkphaseButtonSet!=NULL)
    {
    for (int i = 0; i < this->WorkphaseButtonSet->GetNumberOfWidgets(); i ++)
      {
      this->WorkphaseButtonSet->GetWidget(i)
        ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
      }
    }
  
  
  //----------------------------------------------------------------
  // Wizard Frame

  if (this->WizardWidget)
    {
    this->WizardWidget->GetWizardWorkflow()->AddObserver(vtkKWWizardWorkflow::CurrentStateChangedEvent,
      (vtkCommand *)this->GUICallbackCommand);
    }


  //----------------------------------------------------------------
  // Etc Frame

  // observer load volume button

  this->AddLogicObservers();
  
  
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkProstateNavLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

void vtkProstateNavGUI::AddMRMLObservers(void)
{
  // observe the scene for node deleted events
  if (this->MRMLScene!=NULL)
  {
    if (this->MRMLScene->HasObserver(vtkMRMLScene::NodeRemovedEvent, (vtkCommand *)this->MRMLCallbackCommand) < 1)
    {
      this->MRMLScene->AddObserver(vtkMRMLScene::NodeRemovedEvent, (vtkCommand *)this->MRMLCallbackCommand);
    }
    if (this->MRMLScene->HasObserver(vtkMRMLScene::NodeAboutToBeRemovedEvent, (vtkCommand *)this->MRMLCallbackCommand) < 1)
    {
      this->MRMLScene->AddObserver(vtkMRMLScene::NodeAboutToBeRemovedEvent, (vtkCommand *)this->MRMLCallbackCommand);
    }
    if (this->MRMLScene->HasObserver(vtkMRMLScene::NodeAddedEvent, (vtkCommand *)this->MRMLCallbackCommand) < 1)
    {
      this->MRMLScene->AddObserver(vtkMRMLScene::NodeAddedEvent, (vtkCommand *)this->MRMLCallbackCommand);
    }
    if (this->MRMLScene->HasObserver(vtkMRMLScene::SceneCloseEvent, (vtkCommand *)this->MRMLCallbackCommand) < 1)
    {
      this->MRMLScene->AddObserver(vtkMRMLScene::SceneCloseEvent, (vtkCommand *)this->MRMLCallbackCommand);
    }
  }
}

void vtkProstateNavGUI::RemoveMRMLObservers(void)
{
  if (this->MRMLScene!=NULL)
  {
    this->MRMLScene->RemoveObservers(vtkMRMLScene::NodeRemovedEvent, (vtkCommand *)this->MRMLCallbackCommand);
    this->MRMLScene->RemoveObservers(vtkMRMLScene::NodeAboutToBeRemovedEvent, (vtkCommand *)this->MRMLCallbackCommand);
    this->MRMLScene->RemoveObservers(vtkMRMLScene::NodeAddedEvent, (vtkCommand *)this->MRMLCallbackCommand);
    this->MRMLScene->RemoveObservers(vtkMRMLScene::SceneCloseEvent, (vtkCommand *)this->MRMLCallbackCommand);
  }
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{
  vtkMRMLProstateNavManagerNode *manager=this->GetProstateNavManagerNode();

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  //----------------------------------------------------------------
  // Check Work Phase Transition Buttons

  if ( event == vtkKWPushButton::InvokedEvent && this->WorkphaseButtonSet!=NULL)
  {
    int phase;
    for (phase = 0; phase < this->WorkphaseButtonSet->GetNumberOfWidgets(); phase ++)
    {
      if (this->WorkphaseButtonSet->GetWidget(phase) == vtkKWPushButton::SafeDownCast(caller))
      {
        break;
      }
    }
    if (manager!=NULL)
    {
      if (phase < manager->GetNumberOfSteps()) // if pressed one of them
      {
        ChangeWorkphase(phase, 1);
      }
    }
  }

  //----------------------------------------------------------------
  // Configuration Frame

  else if (this->ProstateNavManagerSelectorWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) &&
           (event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent || event == vtkSlicerNodeSelectorWidget::NewNodeEvent )) 
    {
    vtkMRMLProstateNavManagerNode *managerNode = vtkMRMLProstateNavManagerNode::SafeDownCast(this->ProstateNavManagerSelectorWidget->GetSelected());
    char *managerID=NULL;
    if (managerNode!=NULL)
      {
      managerID=managerNode->GetID();
      }
    this->SetAndObserveProstateNavManagerNodeID(managerID);    
    return;
    }

  else if (this->RobotSelectorWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) &&
           (event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent || event == vtkSlicerNodeSelectorWidget::NewNodeEvent )) 
    {    
    char *robotID=NULL;
    vtkMRMLRobotNode *refNode = vtkMRMLRobotNode::SafeDownCast(this->RobotSelectorWidget->GetSelected());
    if (refNode!=NULL)
      {
      robotID=refNode->GetID();
      }
    if (manager!=NULL)
      {
      manager->SetAndObserveRobotNodeID(robotID);
      }        
    SetAndObserveRobotNodeID(robotID);

    return;
    }


  //----------------------------------------------------------------
  // Wizard Frame

  else if (this->WizardWidget!=NULL && this->WizardWidget->GetWizardWorkflow() == vtkKWWizardWorkflow::SafeDownCast(caller) &&
      event == vtkKWWizardWorkflow::CurrentStateChangedEvent)
  {

    int phase = 0;
    vtkKWWizardStep* step =  this->WizardWidget->GetWizardWorkflow()->GetCurrentStep();

    if (manager)
    {
      int numSteps = manager->GetNumberOfSteps();
      for (int i = 0; i < numSteps; i ++)
      {
        if (step == GetStepPage(i))
        {
          phase = i;
        }
      }

      ChangeWorkphase(phase);
    }
  }


  //----------------------------------------------------------------
  // Etc Frame

  // Process Wizard GUI (Active step only)
  else
    {
    if (manager)
      {
      int stepId = manager->GetCurrentStep();
      vtkProstateNavStep *step=GetStepPage(stepId);
      if (step!=NULL)
        {
        step->ProcessGUIEvents(caller, event, callData);
        }
      }
    }

} 

//---------------------------------------------------------------------------
void vtkProstateNavGUI::Init()
{
  
  // -----------------------------------------
  // Register all new MRML node classes
  {
    // Make sure that all MRML classes are registered (needed for creating/updating the node from XML)
    // SmartPointer is used to create an instance of the class, and destroy immediately after registration is complete
    this->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLBrpRobotCommandNode >::New() );
    this->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLProstateNavManagerNode >::New() );
    this->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLRobotDisplayNode >::New() );
    this->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLTransRectalProstateRobotNode >::New() );
    this->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLTransPerinealProstateRobotNode >::New() );    
    this->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLTransPerinealProstateTemplateNode >::New() );    
  }

}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::DataCallback(vtkObject *caller, 
        unsigned long eid, void *clientData, void *callData)
{
  vtkProstateNavGUI *self = reinterpret_cast<vtkProstateNavGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkProstateNavGUI DataCallback");
  
  self->UpdateGUI();
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::ProcessLogicEvents ( vtkObject *caller,
    unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkProstateNavLogic::SafeDownCast(caller))
    {
    if (event == vtkProstateNavLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::ProcessMRMLEvents ( vtkObject *caller,
    unsigned long event, void *callData )
{

  // manager node chaged
  vtkMRMLProstateNavManagerNode *manager=this->GetProstateNavManagerNode();
  if (manager!=NULL && manager == vtkMRMLProstateNavManagerNode::SafeDownCast(caller))
    {
    switch (event)
      {
      case vtkCommand::ModifiedEvent:
        UpdateGUI();
        break;
      case vtkMRMLProstateNavManagerNode::CurrentTargetChangedEvent:
        // BringTargetToViewIn2DViews(); this is now called in wizard steps
        break;
      }
    }
  // :TODO: update GUI (and observers) if robotnode or targetplanlistnode within manager node is changed

  // robot status changed
  vtkMRMLRobotNode *robotNode=GetRobotNode();
  if (robotNode!=NULL && robotNode == vtkMRMLRobotNode::SafeDownCast(caller))
    {
    if (event == vtkMRMLRobotNode::ChangeStatusEvent)
      {
      UpdateStatusButtons();
      }
    }

  // current target changed
  vtkMRMLFiducialListNode* targetPlanList=NULL;
  targetPlanList=this->GetTargetPlanListNode();
  if (targetPlanList!=NULL && targetPlanList == vtkMRMLFiducialListNode::SafeDownCast(caller))
    {
    UpdateCurrentTargetDisplay();
    }

  if ( this->MRMLScene!=NULL && vtkMRMLScene::SafeDownCast(caller) == this->MRMLScene && (event == vtkMRMLScene::NodeAboutToBeRemovedEvent) )
    {
      if (GetRobotNode() != NULL && callData == GetRobotNode())
        {
        // robot node will be deleted => remove referenced nodes from the scene
        GetRobotNode()->RemoveChildNodes();
        }
    }

  // a node has been deleted
  if ( this->MRMLScene!=NULL && vtkMRMLScene::SafeDownCast(caller) == this->MRMLScene && (event == vtkMRMLScene::NodeRemovedEvent ) )
    {
    if (this->ProstateNavManagerNodeID != NULL && this->MRMLScene->GetNodeByID(this->ProstateNavManagerNodeID) == NULL)
      {
      // manager node has been deleted
      this->SetAndObserveProstateNavManagerNodeID(NULL);
      }
    if (this->RobotNodeID != NULL && this->MRMLScene->GetNodeByID(this->RobotNodeID) == NULL)
      {
      // robot node has been deleted
      if (manager!=NULL)
        {
        manager->SetAndObserveRobotNodeID(NULL);
        }
      this->SetAndObserveRobotNodeID(NULL);
      }
    if (this->TargetPlanListNodeID != NULL && this->MRMLScene->GetNodeByID(this->TargetPlanListNodeID) == NULL)
      {
      // target plan list node has been deleted
      this->SetAndObserveTargetPlanListNodeID(NULL);
      }
    }
  
  // scene is closing
  if (event == vtkMRMLScene::SceneCloseEvent )
    {
    this->SetAndObserveProstateNavManagerNodeID(NULL);
    this->SetAndObserveRobotNodeID(NULL);
    this->SetAndObserveTargetPlanListNodeID(NULL);
    }
}

//---------------------------------------------------------------------------
void vtkProstateNavGUI::Enter()
{
  if (this->Entered == 0)
    {
    this->GetLogic()->SetGUI(this);
    this->GetLogic()->Enter();    
    this->Entered = 1;
    }

  // The user interface is hidden on Exit, show it now
  if (this->WizardWidget!=NULL)
    {
    vtkKWWizardWorkflow *wizard_workflow = this->WizardWidget->GetWizardWorkflow();
    if (wizard_workflow!=NULL)
      {
      vtkProstateNavStep* step=vtkProstateNavStep::SafeDownCast(wizard_workflow->GetCurrentStep());
      if (step)
        {
        step->ShowUserInterface();
        }
      }
    }

  if (this->SecondaryWindow==NULL)
  {
    this->SecondaryWindow=vtkSlicerSecondaryViewerWindow::New();
  }
  if (!this->SecondaryWindow->IsCreated())
  {
    this->SecondaryWindow->SetApplication(this->GetApplication());
    this->SecondaryWindow->Create();
    vtkSlicerViewerWidget* viewerWidget=this->SecondaryWindow->GetViewerWidget();
    if (viewerWidget!=NULL && viewerWidget->GetViewNode()!=NULL)
    {
      viewerWidget->GetViewNode()->SetRenderMode(vtkMRMLViewNode::Orthographic);
    }
  }
  this->SecondaryWindow->DisplayOnSecondaryMonitor();

  AddMRMLObservers();

  // Anything could have been done while using an other module, so update the GUI now
  UpdateGUI();
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::Enter(vtkMRMLNode *node)
{
  Enter();

  vtkMRMLProstateNavManagerNode *managerNode = vtkMRMLProstateNavManagerNode::SafeDownCast(node);
  if ( managerNode )
    {
    this->ProstateNavManagerSelectorWidget->UpdateMenu();
    this->ProstateNavManagerSelectorWidget->SetSelected( managerNode ); // :TODO: check if observers are updated after this
    }

  this->UpdateGUI();
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::Exit ( )
{
  // Show the user interface, because there could be observers that must be deactivated
  RemoveMRMLObservers();

  if (this->WizardWidget!=NULL)
    {
    vtkKWWizardWorkflow *wizard_workflow = this->WizardWidget->GetWizardWorkflow();
    if (wizard_workflow!=NULL)
      {
      vtkProstateNavStep* step=vtkProstateNavStep::SafeDownCast(wizard_workflow->GetCurrentStep());
      if (step)
        {
        step->HideUserInterface();
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::BuildGUI ( )
{
    this->UIPanel->AddPage ( "ProstateNav", "ProstateNav", NULL );
    BuildGUIForHelpFrame();
    BuildGUIForConfigurationFrame();
    BuildGUIForWorkphaseFrame();
    BuildGUIForWizardFrame();
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::TearDownGUI ( )
{
  // REMOVE OBSERVERS and references to MRML and Logic
  // disconnect circular references so destructor can be called

  this->RemoveMRMLObservers();
  this->RemoveGUIObservers();

  if (this->SecondaryWindow)
  {  
    this->SecondaryWindow->Destroy();
  }

  this->GetLogic()->SetGUI(NULL);

  if (this->WizardWidget!=NULL)
  {
    for (int i=0; i<this->WizardWidget->GetWizardWorkflow()->GetNumberOfSteps(); i++)
    {
      vtkProstateNavStep *step=vtkProstateNavStep::SafeDownCast(this->WizardWidget->GetWizardWorkflow()->GetNthStep(i));
      if (step!=NULL)
        {
        step->TearDownGUI();
        step->SetGUI(NULL);
        step->SetLogic(NULL);
        step->SetAndObserveMRMLScene(NULL);
        step->SetProstateNavManager(NULL);
        }
      else
        {
        vtkErrorMacro("Invalid step page: "<<i);
        }
    }
  }
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::BuildGUIForWizardFrame()
{
    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "ProstateNav" );
    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

    if (!this->WizardFrame->IsCreated())
    {
      this->WizardFrame->SetParent(page);
      this->WizardFrame->Create();
      this->WizardFrame->SetLabelText("Wizard");
      this->WizardFrame->ExpandFrame();

      app->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                  this->WizardFrame->GetWidgetName(), 
                  page->GetWidgetName());
    }
}


void vtkProstateNavGUI::BuildGUIForHelpFrame ()
{

  // ----------------------------------------------------------------
  // HELP FRAME         
  // ----------------------------------------------------------------

  // Define your help text here.
  std::stringstream helpss;
  helpss << "Module Revision: " << ProstateNav_REVISION << std::endl;
  helpss << "The **ProstateNav Module** helps you to plan and navigate MRI-guided prostate biopsy ";
  helpss << "and brachytherapy using transrectal and transperineal needle placement devices. \n";
  helpss << "See <a>http://www.slicer.org/slicerWiki/index.php/Modules:ProstateNav-Documentation-3.6</a> for details.";
  
  std::stringstream aboutss;
  aboutss << "The module is developed by Junichi Tokuda (Brigham and Women's Hospital), Andras Lasso,";
  aboutss << "David Gobbi (Queen's University) and Philip Mewes.";
  aboutss << "This work is supported by NCIGT, NA-MIC and BRP \"Enabling Technologies for MRI-Guided Prostate Intervention\" funded by NIH.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "ProstateNav" );
  this->BuildHelpAndAboutFrame (page, helpss.str().c_str(), aboutss.str().c_str());

}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::BuildGUIForConfigurationFrame ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "ProstateNav" );
  
  vtkSlicerModuleCollapsibleFrame *configurationFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  configurationFrame->SetParent(page);
  configurationFrame->Create();
  configurationFrame->SetLabelText("Configuration");
  configurationFrame->ExpandFrame();
  app->Script("pack %s -side top -anchor center -fill x -padx 2 -pady 2 -in %s",
              configurationFrame->GetWidgetName(), page->GetWidgetName());
  
  //  Manager node selector widget
  this->ProstateNavManagerSelectorWidget = vtkSlicerNodeSelectorWidget::New() ;
  this->ProstateNavManagerSelectorWidget->SetParent(configurationFrame->GetFrame());
  this->ProstateNavManagerSelectorWidget->Create();
  this->ProstateNavManagerSelectorWidget->SetNodeClass("vtkMRMLProstateNavManagerNode", NULL, NULL, NULL);
  this->ProstateNavManagerSelectorWidget->SetMRMLScene(this->GetMRMLScene());
  this->ProstateNavManagerSelectorWidget->SetBorderWidth(2);
  this->ProstateNavManagerSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->ProstateNavManagerSelectorWidget->GetWidget()->GetWidget()->SetWidth(24);
  this->ProstateNavManagerSelectorWidget->SetLabelText( "Active configuration: ");
  this->ProstateNavManagerSelectorWidget->NewNodeEnabledOn();
  this->ProstateNavManagerSelectorWidget->SetBalloonHelpString("Select the active ProstateNav configuration from the current scene.");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->ProstateNavManagerSelectorWidget->GetWidgetName());

  this->RobotSelectorWidget = vtkSlicerNodeSelectorWidget::New() ;
  this->RobotSelectorWidget->SetParent(configurationFrame->GetFrame());
  this->RobotSelectorWidget->Create(); 
  this->RobotSelectorWidget->AddNodeClass("vtkMRMLTransPerinealProstateRobotNode", NULL, NULL, NULL);
  this->RobotSelectorWidget->AddNodeClass("vtkMRMLTransPerinealProstateTemplateNode", NULL, NULL, NULL);
  this->RobotSelectorWidget->AddNodeClass("vtkMRMLTransRectalProstateRobotNode", NULL, NULL, NULL);
  this->RobotSelectorWidget->SetMRMLScene(this->GetMRMLScene());
  this->RobotSelectorWidget->SetBorderWidth(2);
  this->RobotSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->RobotSelectorWidget->GetWidget()->GetWidget()->SetWidth(24);
  this->RobotSelectorWidget->SetLabelText( "Robot: ");
  this->RobotSelectorWidget->NewNodeEnabledOn();
  this->RobotSelectorWidget->SetBalloonHelpString("Select the robot.");
  this->RobotSelectorWidget->SetEnabled(0);
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->RobotSelectorWidget->GetWidgetName());  

  configurationFrame->Delete();
}
    
//---------------------------------------------------------------------------
void vtkProstateNavGUI::BuildGUIForWorkphaseFrame ()
{
  // the buttonset shall be dynamically created/destroyed when the
  // wizard GUI is built, because there is no API to remove any buttons
  // so just create the frame now

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "ProstateNav" );
  
  if (!this->WorkphaseButtonFrame->IsCreated())
  {
    vtkSmartPointer<vtkSlicerModuleCollapsibleFrame> workphaseFrame=vtkSmartPointer<vtkSlicerModuleCollapsibleFrame>::New();
    workphaseFrame->SetParent(page);
    workphaseFrame->Create();
    workphaseFrame->SetLabelText("Workphase");
    workphaseFrame->ExpandFrame();
    app->Script("pack %s -side top -anchor center -fill x -padx 2 -pady 2 -in %s",
                workphaseFrame->GetWidgetName(), page->GetWidgetName());

    // -----------------------------------------
    // Frames

    this->StatusButtonFrame->SetParent ( workphaseFrame->GetFrame() );
    this->StatusButtonFrame->Create ( );
    
    this->WorkphaseButtonFrame->SetParent( workphaseFrame->GetFrame());
    this->WorkphaseButtonFrame->Create();
    
    app->Script ( "pack %s %s -side top -fill x -expand y -padx 1 -pady 1",
                  this->StatusButtonFrame->GetWidgetName(),
                  this->WorkphaseButtonFrame->GetWidgetName());
  }  
    
}

//----------------------------------------------------------------------------
int vtkProstateNavGUI::ChangeWorkphase(int phase, int fChangeWizard)
{
  vtkMRMLProstateNavManagerNode *manager=this->GetProstateNavManagerNode();
  if (manager==NULL)
  {
    return 0;
  }

  if (this->WorkphaseButtonSet==NULL)
  {
    // no GUI (e.g., exiting application)
    return 0;
  }

  if (!manager->SwitchStep(phase)) // Set next phase
    {
    cerr << "ChangeWorkphase: Cannot transition!" << endl;
    return 0;
    }
  
  int numSteps = manager->GetNumberOfSteps();
  
  for (int i = 0; i < numSteps; i ++)
    {
    vtkKWPushButton *pb = this->WorkphaseButtonSet->GetWidget(i);
    bool transitionable=true; // :TODO: get this information from the workflow widget state machine
    if (i == manager->GetCurrentStep())
      {
      pb->SetReliefToSunken();
      }
    else if (transitionable)
      {
      double r;
      double g;
      double b;
      GetStepPage(i)->GetTitleBackgroundColor(&r, &g, &b);
      
      pb->SetReliefToGroove();
      pb->SetStateToNormal();
      pb->SetBackgroundColor(r, g, b);
      }
    else
      {
      double r;
      double g;
      double b;
      GetStepPage(i)->GetTitleBackgroundColor(&r, &g, &b);
      r = r * 1.5; r = (r > 1.0) ? 1.0 : r;
      g = g * 1.5; g = (r > 1.0) ? 1.0 : g;
      b = b * 1.5; b = (r > 1.0) ? 1.0 : b;
      
      pb->SetReliefToGroove();
      pb->SetStateToDisabled();
      pb->SetBackgroundColor(r, g, b);
      }
    }
  
  // Switch Wizard Frame
  if (fChangeWizard)
    {
    vtkKWWizardWorkflow *wizard = 
      this->WizardWidget->GetWizardWorkflow();
    
    int step_from;
    int step_to;
    
    //step_to = this->Logic->GetCurrentPhase();
    step_to = manager->GetCurrentStep();
    //step_from = this->Logic->GetPrevPhase();
    step_from = manager->GetPreviousStep();
    
    int steps =  step_to - step_from;
    if (steps > 0)
      {
      for (int i = 0; i < steps; i ++) 
        {
        wizard->AttemptToGoToNextStep();
        }
      }
    else
      {
      steps = -steps;
      for (int i = 0; i < steps; i ++)
        {
        wizard->AttemptToGoToPreviousStep();
        }
      }

    vtkProstateNavStep* step=vtkProstateNavStep::SafeDownCast(wizard->GetCurrentStep());
    if (step)
      {
      //step->ShowUserInterface(); ShowUserInterface is triggered by state Enter event
      step->UpdateGUI();
      }
   
    }
  
  return 1;
}


//----------------------------------------------------------------------------
void vtkProstateNavGUI::UpdateGUI() // from MRML
{ 
  // Update the workphase and wizard frame
  UpdateStatusButtons();
  UpdateWorkflowSteps();

  vtkMRMLProstateNavManagerNode *manager=this->GetProstateNavManagerNode();
  
  // Enable robot selection only if a manager is selected
  if (this->RobotSelectorWidget!=NULL)
  {
    this->RobotSelectorWidget->SetEnabled(manager!=NULL);
  }

  if (this->WizardWidget!=NULL && manager!=NULL)
  {
    int stepId = manager->GetCurrentStep();    
    vtkProstateNavStep *step=GetStepPage(stepId);
    if (step!=NULL)
      {
      step->UpdateGUI();
      }
  }
  
  UpdateCurrentTargetDisplay(); // if a new node is added then it is selected by default => keep only the current target as selected
}


//----------------------------------------------------------------------------
vtkProstateNavStep* vtkProstateNavGUI::GetStepPage(int i)
{
  if (this->WizardWidget==NULL)
    {
    vtkErrorMacro("Invalid WizardWidget");
    return NULL;
    }
  vtkProstateNavStep *step=vtkProstateNavStep::SafeDownCast(this->WizardWidget->GetWizardWorkflow()->GetNthStep(i));
  if (step==NULL)
    {
    vtkErrorMacro("Invalid step page: "<<i);
    }
  return step;
}

//----------------------------------------------------------------------------
void vtkProstateNavGUI::UpdateStatusButtons()
{
  if (this->StatusButtonFrame==NULL)
  {
    return;
  }
  if (!this->StatusButtonFrame->IsCreated())
  {
    return;
  }

  vtkMRMLProstateNavManagerNode *manager=this->GetProstateNavManagerNode();  
  vtkMRMLRobotNode* robot=NULL;
  if (manager!=NULL)
  {
    manager->GetRobotNode();
  }
  
  // -----------------------------------------
  // there is no way to remove a button from the button set, so need to delete it, if less buttons are needed than we actually have
  int robotStatusDescriptorCount=0;
  if (robot!=NULL)
  {
    robotStatusDescriptorCount=robot->GetStatusDescriptorCount();
  }
  if (this->StatusButtonSet)
    {
    if (this->StatusButtonSet->GetNumberOfWidgets()>robotStatusDescriptorCount)
      {
      this->StatusButtonSet->SetParent(NULL);
      this->StatusButtonSet->Delete(); 
      this->StatusButtonSet = NULL;
      }
    }

  if (robot==NULL)
  {
    return;
  }

  if (this->StatusButtonSet==NULL)
    {
    this->StatusButtonSet = vtkKWPushButtonSet::New();
    this->StatusButtonSet->SetParent(this->StatusButtonFrame);
    this->StatusButtonSet->Create();
    this->StatusButtonSet->PackHorizontallyOn();
    this->StatusButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(4);
    this->StatusButtonSet->SetWidgetsPadX(1);
    this->StatusButtonSet->SetWidgetsPadY(1);
    this->StatusButtonSet->UniformColumnsOn();
    this->StatusButtonSet->UniformRowsOn();
    this->Script("pack %s -side left -anchor w -fill x -padx 1 -pady 1", 
               this->StatusButtonSet->GetWidgetName());
    }
  
  int numStatDesc=robot->GetStatusDescriptorCount();
  for (int i = 0; i < numStatDesc; i ++)
  {
    std::string text;
    vtkMRMLRobotNode::STATUS_ID statusId=vtkMRMLRobotNode::StatusOff;
    robot->GetStatusDescriptor(i, text, statusId);    
    if (i>=this->StatusButtonSet->GetNumberOfWidgets())
    {
      this->StatusButtonSet->AddWidget(i);
    }
    vtkKWPushButton* button=this->StatusButtonSet->GetWidget(i);
    if (button==NULL)
    {
      vtkErrorMacro("Invalid button");
      continue;
    }
    button->SetText(text.c_str());
    button->SetBorderWidth(1);
    button->SetEnabled(false);
    switch (statusId)
    {
    case vtkMRMLRobotNode::StatusOff:
      button->SetDisabledForegroundColor(0.4, 0.4, 0.4);
      button->SetBackgroundColor(0.9, 0.9, 0.9);
      break;
    case vtkMRMLRobotNode::StatusOk:
      button->SetBackgroundColor(128.0/255.0,255.0/255.0,128.0/255.0);
      button->SetDisabledForegroundColor(0.1, 0.1, 0.1);
      break;
    case vtkMRMLRobotNode::StatusWarning:
      button->SetBackgroundColor(255.0/255.0,128.0/255.0,0.0/255.0);
      button->SetDisabledForegroundColor(0.0, 0.0, 0.0);  
      break;
    case vtkMRMLRobotNode::StatusError:
      button->SetBackgroundColor(1.0, 0, 0);
      button->SetDisabledForegroundColor(0.0, 0.0, 0.0);  
      break;
    default:
      button->SetBackgroundColor(1.0, 1.0, 0);
      button->SetDisabledForegroundColor(0.0, 0.0, 0.0);  
      break;
    }
  }

}

//----------------------------------------------------------------------------
void vtkProstateNavGUI::UpdateWorkflowSteps()
{
  bool changed=true;
  vtkMRMLProstateNavManagerNode *manager=this->GetProstateNavManagerNode();
  if (manager!=NULL && this->DisplayedWorkflowSteps!=NULL)
  {
    int newSteps = manager->GetNumberOfSteps();
    int currentSteps = this->DisplayedWorkflowSteps->GetNumberOfValues();
    if (newSteps==currentSteps)
    {
      changed=false;
      for (int i = 0; i < newSteps; i ++)
      {
        vtkStdString stepName=manager->GetStepName(i);
        if (stepName.compare(this->DisplayedWorkflowSteps->GetValue(i))!=0)
        {
          changed=true;
          break;
        }
      }
    }
  }

  if (!changed)
  {
    return;
  }

  // the widget shall be dynamically created/destroyed when the
  // wizard GUI is built, because there is no API to remove any steps
  // from the wizard (only to add steps)

  if (this->DisplayedWorkflowSteps!=NULL)
  {
    this->DisplayedWorkflowSteps->Reset();
  }

  // Delete wizard widget
  if (this->WizardWidget)
  {

    vtkKWWizardWorkflow *wizard_workflow = this->WizardWidget->GetWizardWorkflow();
    vtkProstateNavStep* step=vtkProstateNavStep::SafeDownCast(wizard_workflow->GetCurrentStep());
    if (step)
    {
      step->HideUserInterface();
    }

    this->WizardWidget->SetParent(NULL);
    this->WizardWidget->Delete(); 
    this->WizardWidget = NULL;
  }

  // Delete workphase button set
  if (this->WorkphaseButtonSet)
  {
    for (int i = 0; i < this->WorkphaseButtonSet->GetNumberOfWidgets(); i ++)
    {
      this->WorkphaseButtonSet->GetWidget(i)->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
    this->WorkphaseButtonSet->SetParent(NULL);
    this->WorkphaseButtonSet->Delete(); 
    this->WorkphaseButtonSet = NULL;
  }

  if (manager==NULL)
  {
    // there is no active manager node, the wizard frame shall be empty
    return;
  }

  if (this->WizardFrame==NULL)
  {
    // there is no GUI
    return;
  }
  if (!this->WizardFrame->IsCreated())
  {
    // there is no GUI
    return;
  }

  int numSteps = manager->GetNumberOfSteps();

  if (numSteps<1)
  {
    // no steps, the wizard frame shall be empty
    this->WizardFrame->CollapseFrame();
    return;
  }

  this->WizardFrame->ExpandFrame();

  // Recreate workphase button set
  this->WorkphaseButtonSet = vtkKWPushButtonSet::New();
  this->WorkphaseButtonSet->SetParent(this->WorkphaseButtonFrame);
  this->WorkphaseButtonSet->Create();
  this->WorkphaseButtonSet->PackHorizontallyOn();
  this->WorkphaseButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(3);
  this->WorkphaseButtonSet->SetWidgetsPadX(1);
  this->WorkphaseButtonSet->SetWidgetsPadY(1);
  this->WorkphaseButtonSet->UniformColumnsOn();
  this->WorkphaseButtonSet->UniformRowsOn();  
  this->Script("pack %s -side left -anchor w -fill x -padx 1 -pady 1", 
    this->WorkphaseButtonSet->GetWidgetName());    

  // Recreate workphase wizard
  this->WizardWidget=vtkKWWizardWidget::New();   
  this->WizardWidget->SetParent(this->WizardFrame->GetFrame());
  this->WizardWidget->Create();
  this->WizardWidget->GetSubTitleLabel()->SetHeight(1);
  this->WizardWidget->SetClientAreaMinimumHeight(200);
  //this->WizardWidget->SetButtonsPositionToTop();
  this->WizardWidget->NextButtonVisibilityOn();
  this->WizardWidget->BackButtonVisibilityOn();
  this->WizardWidget->OKButtonVisibilityOff();
  this->WizardWidget->CancelButtonVisibilityOff();
  this->WizardWidget->FinishButtonVisibilityOff();
  this->WizardWidget->HelpButtonVisibilityOn();
  this->Script("pack %s -side top -anchor nw -fill both -expand y",
    this->WizardWidget->GetWidgetName());

  // -----------------------------------------------------------------
  // Add the steps to the workflow

  vtkKWWizardWorkflow *wizard_workflow = this->WizardWidget->GetWizardWorkflow();

  // -----------------------------------------------------------------
  // Set GUI/Logic to each step and add to workflow  

  for (int i = 0; i < numSteps; i ++)
  {

    vtkStdString stepName=manager->GetStepName(i);

    this->DisplayedWorkflowSteps->InsertNextValue(stepName);

    vtkProstateNavStep* newStep=NULL;

    if (!stepName.compare("SetUp"))
    {
      vtkProstateNavStepSetUp* setupStep = vtkProstateNavStepSetUp::New();
      setupStep->SetTitleBackgroundColor(205.0/255.0, 200.0/255.0, 177.0/255.0);
      newStep=setupStep;
    } 
    else if (!stepName.compare("SetUpTemplate"))
      {
      vtkProstateNavStepSetUpTemplate* setupStep = vtkProstateNavStepSetUpTemplate::New();
      setupStep->SetTitleBackgroundColor(205.0/255.0, 200.0/255.0, 177.0/255.0);
      newStep=setupStep;
      }
    else if (!stepName.compare("ZFrameCalibration"))
    {
      vtkProstateNavCalibrationStep* calibrationStep = vtkProstateNavCalibrationStep::New();
      calibrationStep->SetTitleBackgroundColor(193.0/255.0, 115.0/255.0, 80.0/255.0);
      newStep=calibrationStep;
    }
    else if (!stepName.compare("FiducialCalibration"))
    {
      vtkProstateNavFiducialCalibrationStep* calibrationStep = vtkProstateNavFiducialCalibrationStep::New();
      calibrationStep->SetTitleBackgroundColor(193.0/255.0, 115.0/255.0, 80.0/255.0);
      newStep=calibrationStep;
    }
    else if (!stepName.compare("PointTargetingWithoutOrientation"))
    {
      vtkProstateNavTargetingStep* targetingStep = vtkProstateNavTargetingStep::New();
      targetingStep->SetShowTargetOrientation(false);
      targetingStep->SetTitleBackgroundColor(138.0/255.0, 165.0/255.0, 111.0/255.0);
      newStep=targetingStep;
    }
    else if (!stepName.compare("PointTargeting"))
    {
      vtkProstateNavTargetingStep* targetingStep = vtkProstateNavTargetingStep::New();
      targetingStep->SetShowTargetOrientation(true);
      targetingStep->SetTitleBackgroundColor(138.0/255.0, 165.0/255.0, 111.0/255.0);
      newStep=targetingStep;
    }
    else if (!stepName.compare("TemplateTargeting"))
      {
      vtkProstateNavStepTargetingTemplate* targetingStep = vtkProstateNavStepTargetingTemplate::New();
      targetingStep->SetTitleBackgroundColor(138.0/255.0, 165.0/255.0, 111.0/255.0);
      newStep=targetingStep;
      }
    else if (!stepName.compare("TransperinealProstateRobotManualControl"))
    {
      vtkProstateNavManualControlStep* manualStep = vtkProstateNavManualControlStep::New();
      manualStep->SetTitleBackgroundColor(179.0/255.0, 179.0/255.0, 230.0/255.0);
      newStep=manualStep;
    }
    else if (!stepName.compare("PointVerification"))
    {
      vtkProstateNavStepVerification* verificationStep = vtkProstateNavStepVerification::New();
      verificationStep->SetTitleBackgroundColor(179.0/255.0, 145.0/255.0, 105.0/255.0);
      newStep=verificationStep;
    }
    else
    {
      vtkErrorMacro("Invalid step name: "<<stepName.c_str());
    }

    if (newStep!=NULL)
    {
      newStep->SetGUI(this);
      newStep->SetLogic(this->Logic);
      newStep->SetAndObserveMRMLScene(this->GetMRMLScene());
      newStep->SetProstateNavManager(manager);
      newStep->SetTotalSteps(numSteps);
      newStep->SetStepNumber(i+1);
      newStep->UpdateName();

      wizard_workflow->AddNextStep(newStep);          

      this->WorkphaseButtonSet->AddWidget(i);  

      this->WorkphaseButtonSet->GetWidget(i)->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
      this->WorkphaseButtonSet->GetWidget(i)->SetWidth(16);                  
      this->WorkphaseButtonSet->GetWidget(i)->SetText(newStep->GetTitle());

      double r;
      double g;
      double b;
      newStep->GetTitleBackgroundColor(&r, &g, &b);
      this->WorkphaseButtonSet->GetWidget(i)->SetBackgroundColor(r, g, b);
      this->WorkphaseButtonSet->GetWidget(i)->SetActiveBackgroundColor(r, g, b);

      newStep->Delete();
    }      
  }
  
  // -----------------------------------------------------------------
  // Initial and finish step
  wizard_workflow->SetFinishStep(GetStepPage(numSteps-1));
  wizard_workflow->CreateGoToTransitionsToFinishStep();
  wizard_workflow->SetInitialStep(GetStepPage(0));

  // -----------------------------------------------------------------
  // Show the user interface
  vtkProstateNavStep* step=vtkProstateNavStep::SafeDownCast(wizard_workflow->GetCurrentStep());
  if (step)
    {
    step->ShowUserInterface();
    step->UpdateGUI();
    }
}

//--------------------------------------------------------------------------------
void vtkProstateNavGUI::BringTargetToViewIn2DViews(int mode)
{
  vtkMRMLProstateNavManagerNode *manager=GetProstateNavManagerNode();
  if(manager==NULL)
  {
    return;
  }

  int currentTargetInd=manager->GetCurrentTargetIndex();
  if (currentTargetInd<0)
  {
    return;
  }
  
  vtkProstateNavTargetDescriptor* targetDesc=manager->GetTargetDescriptorAtIndex(currentTargetInd);
  if (targetDesc==NULL)
  {
    vtkErrorMacro("No target descriptor available for the current target");
    return;
  }  

  // get the point ras location of the target fiducial (P) that lies on the image plane
  double targetRAS[3];
  targetDesc->GetRASLocation(targetRAS);

  if (mode==BRING_MARKERS_TO_VIEW_KEEP_CURRENT_ORIENTATION)
  {
    // the slices may not be really orthogonal, they could be oblique
    // we could directly call slice node -> JumpAllSlices (r, a, s), this brings target in view
    // in all slices, but with target fiducial at the center of the view, moving (disturbing) the image altogether
    // for this function ->JumpSliceByOffsetting does the job
    BringMarkerToViewIn2DViews(targetRAS);
  }
  else
  {
    // Align the slices with the needle
    // Slice 1: orthogonal to the needle
    // Slice 2: aligned with the needle line and robot main axis
    // Slice 3: orthogonal to Slice 1 and 2

    double targetHingeRAS[3];
    targetDesc->GetHingePosition(targetHingeRAS);

    double needleVector[4]={0,0,0, 0};
    needleVector[0] = targetRAS[0] - targetHingeRAS[0];
    needleVector[1] = targetRAS[1] - targetHingeRAS[1];
    needleVector[2] = targetRAS[2] - targetHingeRAS[2];
    vtkMath::Normalize(needleVector);

    double transverseVector[4]={1,0,0, 0};

    vtkMRMLRobotNode* robot=manager->GetRobotNode();
    if (robot!=NULL)
    {
      // aligned transverse vector with robot base
      vtkSmartPointer<vtkMatrix4x4> transform=vtkSmartPointer<vtkMatrix4x4>::New();
      if (robot->GetRobotBaseTransform(transform))
      {
        double unalignedTransverseVector[4]={transverseVector[0],transverseVector[1],transverseVector[2], 0};
        transform->MultiplyPoint(unalignedTransverseVector, transverseVector);
      }
    }    

    BringMarkerToViewIn2DViews(targetRAS, needleVector, transverseVector);
  }
  
}

//--------------------------------------------------------------------------------
void vtkProstateNavGUI::BringMarkerToViewIn2DViews(double* P, double* N/*=NULL*/, double* T/*=NULL*/)
{
  vtkSlicerSliceLogic *redSlice = vtkSlicerApplicationGUI::SafeDownCast(GetApplicationGUI())->GetApplicationLogic()->GetSliceLogic("Red");    
  vtkSlicerSliceLogic *yellowSlice = vtkSlicerApplicationGUI::SafeDownCast(GetApplicationGUI())->GetApplicationLogic()->GetSliceLogic("Yellow");    
  vtkSlicerSliceLogic *greenSlice = vtkSlicerApplicationGUI::SafeDownCast(GetApplicationGUI())->GetApplicationLogic()->GetSliceLogic("Green");    

  int redOldModify=redSlice->GetSliceNode()->StartModify();
  int yellowOldModify=yellowSlice->GetSliceNode()->StartModify();
  int greenOldModify=greenSlice->GetSliceNode()->StartModify();

  if (N!=NULL && T!=NULL) // slice orientation is specified
  {
    redSlice->GetSliceNode()->SetSliceToRASByNTP(N[0], N[1], N[2], T[0], T[1], T[2], P[0], P[1], P[2], 0);
    yellowSlice->GetSliceNode()->SetSliceToRASByNTP(N[0], N[1], N[2], T[0], T[1], T[2], P[0], P[1], P[2], 1);
    greenSlice->GetSliceNode()->SetSliceToRASByNTP(N[0], N[1], N[2], T[0], T[1], T[2], P[0], P[1], P[2], 2);
  }
  else
  {
    redSlice->GetSliceNode()->JumpSliceByOffsetting(P[0], P[1], P[2]);
    yellowSlice->GetSliceNode()->JumpSliceByOffsetting(P[0], P[1], P[2]);
    greenSlice->GetSliceNode()->JumpSliceByOffsetting(P[0], P[1], P[2]);
  }

  redSlice->GetSliceNode()->EndModify(redOldModify);
  yellowSlice->GetSliceNode()->EndModify(yellowOldModify);
  greenSlice->GetSliceNode()->EndModify(greenOldModify);
}


//--------------------------------------------------------------------------------
void vtkProstateNavGUI::UpdateCurrentTargetDisplay()
{
  vtkMRMLProstateNavManagerNode *manager=GetProstateNavManagerNode();
  if(manager==NULL)
  {
    return;
  }

  vtkMRMLFiducialListNode* fidList = manager->GetTargetPlanListNode();
  if(fidList==NULL)
  {
    return;
  }

  int currentTargetInd=manager->GetCurrentTargetIndex();
  std::string selectedFidID="INVALID";  
  if (currentTargetInd>=0)
  {
    vtkProstateNavTargetDescriptor* targetDesc=manager->GetTargetDescriptorAtIndex(currentTargetInd);
    if (targetDesc!=NULL)
    {
      selectedFidID=targetDesc->GetFiducialID();
    }
  }

  // Changing the fiducial selection state takes quite a long time. Check if we need modification, and request a modification
  // just if it is really necessary to avoid unnecessary lengthy updates.
  bool modificationStarted=false;
  int oldModify=0;
  for (int i = 0; i < fidList->GetNumberOfFiducials(); i ++)
  {              
    // select only the active target
    int selectedAlready=fidList->GetNthFiducialSelected(i);
    int selectionNeeded=selectedFidID.compare(fidList->GetNthFiducialID(i))==0;
    if (selectedAlready!=selectionNeeded)
    {
      if (!modificationStarted)
      {
        oldModify=fidList->StartModify();
        modificationStarted=true;
      }
      fidList->SetNthFiducialSelected(i, selectionNeeded);
    }
  }
  if (modificationStarted)
  {
    fidList->EndModify(oldModify);
    // StartModify/EndModify discarded vtkMRMLFiducialListNode::FiducialModifiedEvent-s, so we have to resubmit them now
    fidList->InvokeEvent(vtkMRMLFiducialListNode::FiducialModifiedEvent, NULL);
  }  

  UpdateCurrentTargetDisplayInSecondaryWindow();
}

void vtkProstateNavGUI::UpdateCurrentTargetDisplayInSecondaryWindow()
{  
  vtkMRMLProstateNavManagerNode *manager=GetProstateNavManagerNode();
  if(manager==NULL)
  {
    return;
  }

  if (this->SecondaryWindow==NULL || 
    this->SecondaryWindow->GetViewerWidget()==NULL ||
    this->SecondaryWindow->GetViewerWidget()->GetMainViewer()==NULL)
  {
    vtkErrorMacro("Secondary window is not available");
    return;
  }
  
  vtkCornerAnnotation *anno = this->SecondaryWindow->GetViewerWidget()->GetMainViewer()->GetCornerAnnotation();
  if (anno==NULL)
  {
    vtkErrorMacro("Corner annotation is not available");
    return;
  }

  static double lsf=10.0;
  static double nlsf=0.35;
  anno->SetLinearFontScaleFactor(lsf);
  anno->SetNonlinearFontScaleFactor(nlsf);

  const int TARGET_CORNER_ID=3;
  vtkMRMLRobotNode* robot=manager->GetRobotNode();
  vtkProstateNavTargetDescriptor *targetDesc = manager->GetTargetDescriptorAtIndex(manager->GetCurrentTargetIndex()); 
  if (robot!=NULL && targetDesc!=NULL)
  {
    std::string info=robot->GetTargetInfoText(targetDesc);
    anno->SetText(TARGET_CORNER_ID,info.c_str());
  }
  else
  {
    // no target info available for the current robot with the current target    
    anno->SetText(TARGET_CORNER_ID, "No target");
  }

  this->SecondaryWindow->GetViewerWidget()->GetMainViewer()->CornerAnnotationVisibilityOn();
  // update the annotations
  this->SecondaryWindow->GetViewerWidget()->GetMainViewer()->Render();
}

//----------------------------------------------------------------------------
void vtkProstateNavGUI::SetAndObserveRobotNodeID(const char *nodeID)
{
  bool modified=false;
  if (nodeID!=NULL && this->RobotNodeID!=NULL)
  {
    if (strcmp(nodeID, this->RobotNodeID)!=0)
    {
      modified=true;
    }
  }
  else if (nodeID!=this->RobotNodeID)
  {
    modified=true;
  }

  vtkSetAndObserveMRMLObjectMacro(this->RobotNode, NULL);
  this->SetRobotNodeID(nodeID);
  vtkMRMLRobotNode *tnode = this->GetRobotNode();

  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  events->InsertNextValue(vtkMRMLRobotNode::ChangeStatusEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->RobotNode, tnode, events);

  if (!modified)
  {
    return;
  }

  if (this->RobotNode!=NULL)
  {
    //this will fire a manager update message, which will trigger a GUI update
    this->RobotNode->Init(vtkSlicerApplication::SafeDownCast(this->GetApplication())); // :TODO: init is called every time a robot is selected, however it would need to be called when the robot is created
  }

  UpdateGUI();
  /*
  ChangeWorkphase(0,true); //always start with the the first step

  UpdateStatusButtons();
  */
}

//----------------------------------------------------------------------------
vtkMRMLRobotNode* vtkProstateNavGUI::GetRobotNode()
{
  vtkMRMLRobotNode* node = NULL;
  if (this->MRMLScene && this->RobotNodeID != NULL )
    {
    vtkMRMLNode* snode = this->MRMLScene->GetNodeByID(this->RobotNodeID);
    node = vtkMRMLRobotNode::SafeDownCast(snode);
    }
  return node;
}

//----------------------------------------------------------------------------
void vtkProstateNavGUI::SetAndObserveProstateNavManagerNodeID(const char *nodeID)
{
  bool modified=false;
  if (nodeID!=NULL && this->ProstateNavManagerNodeID!=NULL)
  {
    if (strcmp(nodeID, this->ProstateNavManagerNodeID)!=0)
    {
      modified=true;
    }
  }
  else if (nodeID!=this->ProstateNavManagerNodeID)
  {
    modified=true;
  }

  vtkSetAndObserveMRMLObjectMacro(this->ProstateNavManagerNode, NULL);
  this->SetProstateNavManagerNodeID(nodeID);
  vtkMRMLProstateNavManagerNode *tnode = this->GetProstateNavManagerNode();

  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  events->InsertNextValue(vtkMRMLProstateNavManagerNode::CurrentTargetChangedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->ProstateNavManagerNode, tnode, events);

  if (!modified)
  {
    return;
  }

  // Add default needle list info
  if (this->ProstateNavManagerNode != NULL)
    {
    const char regSectionName[]="ProstateNav";
    const char regDefaultNeedleListKeyName[]="ProstateNav";      

    if (this->GetApplication()->HasRegistryValue (2, regSectionName, regDefaultNeedleListKeyName))
      {
      char *defNeedleDesc=NULL;
      this->GetApplication()->GetRegistryValue(2, regSectionName, regDefaultNeedleListKeyName, defNeedleDesc);
      this->ProstateNavManagerNode->Init(defNeedleDesc);
      delete[] defNeedleDesc;
      defNeedleDesc=NULL;
      }
    else
      {
      // Biopsy needle: Overshoot = -13; // needle has to be inserted 13 mm short of the target
      // Seed insertion needle: Overshoot >=0, as the seed is inside the needle
      const char defNeedleDesc[]=
        "<NeedleList DefaultNeedle=\"B\"> \
          <Needle Name=\"T\" Description=\"Generic\" Length=\"200\" Overshoot=\"0\" Extension=\"0\" TargetSize=\"0\" LastTargetId=\"0\" /> \
          <Needle Name=\"B\" Description=\"Biopsy\" Length=\"200\" Overshoot=\"-9.5\" Extension=\"-27\" TargetSize=\"16\" LastTargetId=\"0\" /> \
          <Needle Name=\"S\" Description=\"Seed\" Length=\"200\" Overshoot=\"0\" Extension=\"0\" TargetSize=\"3\" LastTargetId=\"0\" /> \
        </NeedleList>";
      this->GetApplication()->SetRegistryValue(2,"ProstateNav","DefaultNeedleList","%s",defNeedleDesc);
      this->ProstateNavManagerNode->Init(defNeedleDesc);          
      }

    }

  if (this->ProstateNavManagerNode!=NULL)
  {
    SetAndObserveRobotNodeID(this->ProstateNavManagerNode->GetRobotNodeID());
    SetAndObserveTargetPlanListNodeID(this->ProstateNavManagerNode->GetTargetPlanListNodeID());
  }
  else
  {
    SetAndObserveRobotNodeID(NULL);
    SetAndObserveTargetPlanListNodeID(NULL);
  }  
  
  UpdateGUI();
}

//----------------------------------------------------------------------------
vtkMRMLProstateNavManagerNode* vtkProstateNavGUI::GetProstateNavManagerNode()
{
  vtkMRMLProstateNavManagerNode* node = NULL;
  if (this->MRMLScene && this->ProstateNavManagerNodeID != NULL )
    {
    vtkMRMLNode* snode = this->MRMLScene->GetNodeByID(this->ProstateNavManagerNodeID);
    node = vtkMRMLProstateNavManagerNode::SafeDownCast(snode);
    }
  return node;
}

//----------------------------------------------------------------------------
void vtkProstateNavGUI::SetAndObserveTargetPlanListNodeID(const char *nodeID)
{
  bool modified=false;
  if (nodeID!=NULL && this->TargetPlanListNodeID!=NULL)
  {
    if (strcmp(nodeID, this->TargetPlanListNodeID)!=0)
    {
      modified=true;
    }
  }
  else if (nodeID!=this->TargetPlanListNodeID)
  {
    modified=true;
  }

  vtkSetAndObserveMRMLObjectMacro(this->TargetPlanListNode, NULL);
  this->SetTargetPlanListNodeID(nodeID);
  vtkMRMLFiducialListNode *tnode = this->GetTargetPlanListNode();

  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLFiducialListNode::DisplayModifiedEvent);
  events->InsertNextValue(vtkMRMLFiducialListNode::FiducialModifiedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->TargetPlanListNode, tnode, events);

  if (!modified)
  {
    return;
  }
  
  UpdateCurrentTargetDisplay();
}

//----------------------------------------------------------------------------
vtkMRMLFiducialListNode* vtkProstateNavGUI::GetTargetPlanListNode()
{
  vtkMRMLFiducialListNode* node = NULL;
  if (this->MRMLScene && this->TargetPlanListNodeID != NULL )
    {
    vtkMRMLNode* snode = this->MRMLScene->GetNodeByID(this->TargetPlanListNodeID);
    node = vtkMRMLFiducialListNode::SafeDownCast(snode);
    }
  return node;
}

//----------------------------------------------------------------------------
void vtkProstateNavGUI::RequestRenderInViewerWidgets()
{
  if (GetApplicationGUI()==NULL)
  {
    vtkWarningMacro("Application GUI is null");
    return;
  }

  int numberOfWidgets=GetApplicationGUI()->GetNumberOfViewerWidgets();
  for (int i=0; i<numberOfWidgets; i++)
  {
    vtkSlicerViewerWidget* widget=GetApplicationGUI()->GetNthViewerWidget(0); // main viewer is the first viewer
    if (widget!=NULL)
    {
      widget->RequestRender();
    }
  }
}
