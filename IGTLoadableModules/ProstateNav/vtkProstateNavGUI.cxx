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

#include "vtkMRMLBrpRobotCommandNode.h"
#include "vtkMRMLRobotDisplayNode.h"

#include "vtkProstateNavGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkProstateNavStep.h"
#include "vtkProstateNavStepSetUp.h"
#include "vtkProstateNavStepVerification.h"
#include "vtkProstateNavCalibrationStep.h"
#include "vtkProstateNavTargetingStep.h"
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

// for Realtime Image
#include "vtkImageChangeInformation.h"
#include "vtkSlicerColorLogic.h"
//#include "vtkSlicerVolumesGUI.h"

#include "vtkCylinderSource.h"
#include "vtkMRMLLinearTransformNode.h"

#include "vtkOpenIGTLinkIFGUI.h"
#include "vtkOpenIGTLinkIFLogic.h"
#include "vtkIGTLToMRMLCoordinate.h"
#include "vtkIGTLToMRMLBrpRobotCommand.h"

#include "vtkMRMLProstateNavManagerNode.h"

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
  
  
  //----------------------------------------------------------------
  // Workphase Frame
  
  this->WorkPhaseButtonSet = NULL;

  // these need to be set to null no matter what
  this->ScannerStatusLabelDisp  = NULL;
  this->RobotStatusLabelDisp = NULL;

  //----------------------------------------------------------------  
  // Wizard Frame
  
  this->WizardWidget = vtkKWWizardWidget::New();

  //----------------------------------------------------------------
  // Target Fiducials List (MRML)

  this->FiducialListNodeID = NULL;
  this->FiducialListNode   = NULL;

  this->Entered = 0;
  this->CoordinateConverter = NULL;
  this->CommandConverter = NULL;
 
  // -----------------------------------------
  // Set up manager (the node type will be registered in Init())
  this->ProstateNavManager =  vtkMRMLProstateNavManagerNode::New();

  this->TimerFlag = 0;  
  this->ScannerConnectedFlag = 0;
  this->RobotConnectedFlag   = 0;
}



//---------------------------------------------------------------------------
vtkProstateNavGUI::~vtkProstateNavGUI ( )
{
  this->TimerFlag = 0;

  this->RemoveGUIObservers();

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }


  //----------------------------------------------------------------
  // Workphase Frame

  if (this->WorkPhaseButtonSet)
    {
    this->WorkPhaseButtonSet->SetParent(NULL);
    this->WorkPhaseButtonSet->Delete();
    }
  this->SetModuleLogic ( NULL );


  //----------------------------------------------------------------
  // Wizard Frame

  if (this->WizardWidget)
    {
    this->WizardWidget->SetParent(NULL);
    this->WizardWidget->Delete(); 
    this->WizardWidget = NULL;
    }

  // -----------------------------------------
  // Work Phase Display Frame

  if (this->ScannerStatusLabelDisp)
    {
    this->ScannerStatusLabelDisp->SetParent(NULL);
    this->ScannerStatusLabelDisp->Delete(); 
    this->ScannerStatusLabelDisp = NULL;
    }
  if (this->RobotStatusLabelDisp)
    {
    this->RobotStatusLabelDisp->SetParent(NULL);
    this->RobotStatusLabelDisp->Delete(); 
    this->RobotStatusLabelDisp = NULL;
    }
  if (this->CoordinateConverter)
    { 
    this->CoordinateConverter->Delete();
    this->CoordinateConverter = NULL;
    }
  if (this->CommandConverter)
    {
    this->CommandConverter->Delete();
    this->CommandConverter = NULL;
    }

}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->vtkObject::PrintSelf ( os, indent );
    



    os << indent << "ProstateNavGUI: " << this->GetClassName ( ) << "\n";
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
  // Workphase Frame

  if (this->WorkPhaseButtonSet)
    {
    for (int i = 0; i < this->WorkPhaseButtonSet->GetNumberOfWidgets(); i ++)
      {
      this->WorkPhaseButtonSet->GetWidget(i)->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
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

  // make a user interactor style to process our events
  // look at the InteractorStyle to get our events
  
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  //
  //appGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()
  //  ->GetRenderWindowInteractor()->GetInteractorStyle()
  //  ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  //appGUI->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()
  //  ->GetRenderWindowInteractor()->GetInteractorStyle()
  //  ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  //appGUI->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()
  //  ->GetRenderWindowInteractor()->GetInteractorStyle()
  //  ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  
  //----------------------------------------------------------------
  // Workphase Frame

  for (int i = 0; i < this->WorkPhaseButtonSet->GetNumberOfWidgets(); i ++)
    {
    this->WorkPhaseButtonSet->GetWidget(i)
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
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

//---------------------------------------------------------------------------
void vtkProstateNavGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{

  /*
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  vtkSlicerInteractorStyle *istyle0 
    = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI("Red")->GetSliceViewer()
                                             ->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyle1 
    = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI("Yellow")->GetSliceViewer()
                                             ->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyle2 
    = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI("Green")->GetSliceViewer()
                                             ->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());

  vtkCornerAnnotation *anno = NULL;
  if (style == istyle0)
    {
    anno = appGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }
  else if (style == istyle1)
    {
    anno = appGUI->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }
  else if (style == istyle2)
    {
    anno = appGUI->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }
  if (anno)
    {
    const char *rasText = anno->GetText(1);
    if ( rasText != NULL )
      {
      std::string ras = std::string(rasText);
        
      // remove "R:," "A:," and "S:" from the string
      int loc = ras.find("R:", 0);
      if ( loc != std::string::npos ) 
        {
        ras = ras.replace(loc, 2, "");
        }
      loc = ras.find("A:", 0);
      if ( loc != std::string::npos ) 
        {
        ras = ras.replace(loc, 2, "");
        }
      loc = ras.find("S:", 0);
      if ( loc != std::string::npos ) 
        {
        ras = ras.replace(loc, 2, "");
        }
      
      // remove "\n" from the string
      int found = ras.find("\n", 0);
      while ( found != std::string::npos )
        {
        ras = ras.replace(found, 1, " ");
        found = ras.find("\n", 0);
        }
      
      }
    }
  */
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  //----------------------------------------------------------------
  // Check Work Phase Transition Buttons

  if ( event == vtkKWPushButton::InvokedEvent)
    {
    int phase;
    for (phase = 0; phase < this->WorkPhaseButtonSet->GetNumberOfWidgets(); phase ++)
      {
      if (this->WorkPhaseButtonSet->GetWidget(phase) == vtkKWPushButton::SafeDownCast(caller))
        {
        break;
        }
      }
    if (phase < this->ProstateNavManager->GetNumberOfSteps()) // if pressed one of them
      {
      ChangeWorkPhase(phase, 1);
      }
    }


  //----------------------------------------------------------------
  // Wizard Frame

  else if (this->WizardWidget!=NULL && this->WizardWidget->GetWizardWorkflow() == vtkKWWizardWorkflow::SafeDownCast(caller) &&
      event == vtkKWWizardWorkflow::CurrentStateChangedEvent)
    {
          
    int phase = vtkProstateNavLogic::Emergency;
    vtkKWWizardStep* step =  this->WizardWidget->GetWizardWorkflow()->GetCurrentStep();

    int numSteps = this->ProstateNavManager->GetNumberOfSteps();
    for (int i = 0; i < numSteps; i ++)
      {
        if (step == GetStepPage(i))
        {
        phase = i;
        }
      }
    
    ChangeWorkPhase(phase);
    }


  //----------------------------------------------------------------
  // Etc Frame

  // Process Wizard GUI (Active step only)
  else
    {
    int stepId = this->ProstateNavManager->GetCurrentStep();
    
    vtkProstateNavStep *step=GetStepPage(stepId);
    if (step!=NULL)
      {
      step->ProcessGUIEvents(caller, event, callData);
      }
    }

} 


//---------------------------------------------------------------------------
void vtkProstateNavGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    // Scanner connector status
    if (this->ScannerConnectedFlag == 1)
      {
      this->ScannerStatusLabelDisp->SetReadOnlyBackgroundColor(15.0/255.0, 102.0/255.0, 192.0/255.0);
      this->ScannerStatusLabelDisp->SetForegroundColor(0.1, 0.1, 0.1);
      this->ScannerStatusLabelDisp->SetValue (" SCANNER: ON ");
      }
    else if (this->ScannerConnectedFlag == 2)
      {
      this->ScannerStatusLabelDisp->SetReadOnlyBackgroundColor(0.9, 0.9, 0.9);
      this->ScannerStatusLabelDisp->SetForegroundColor(0.4, 0.4, 0.4);
      this->ScannerStatusLabelDisp->SetValue (" SCANNER: OFF ");
      }
    this->ScannerConnectedFlag   = 0;

    // Robot connector status
    if (this->RobotConnectedFlag == 1)
      {
      this->RobotStatusLabelDisp->SetReadOnlyBackgroundColor(15.0/255.0, 102.0/255.0, 192.0/255.0);
      this->RobotStatusLabelDisp->SetForegroundColor(0.1, 0.1, 0.1);
      this->RobotStatusLabelDisp->SetValue (" ROBOT: ON ");
      }
    else if (this->RobotConnectedFlag == 2)
      {
      this->RobotStatusLabelDisp->SetReadOnlyBackgroundColor(0.9, 0.9, 0.9);
      this->RobotStatusLabelDisp->SetForegroundColor(0.4, 0.4, 0.4);
      this->RobotStatusLabelDisp->SetValue (" ROBOT: OFF ");
      }
    this->RobotConnectedFlag   = 0;

    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::Init()
{
  
  // -----------------------------------------
  // Register all new MRML node classes
  {
    // SmartPointer is used to create an instance of the class, and desstroy immediately after registration is complete
    this->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLBrpRobotCommandNode >::New() );
    this->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLProstateNavManagerNode >::New() );
    this->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLRobotDisplayNode >::New() );
    //:TODO: make sure that all MRML classes are registered (needed for creating/updating the node from XML)
  }

  // Add the manager node to the scene and start observing it.
  this->GetMRMLScene()->AddNode(this->ProstateNavManager);
  vtkIntArray  *events = vtkIntArray::New();
  events->InsertNextValue ( vtkCommand::ModifiedEvent );
  events->InsertNextValue ( vtkMRMLIGTLConnectorNode::ConnectedEvent );
  events->InsertNextValue ( vtkMRMLIGTLConnectorNode::DisconnectedEvent );

  vtkMRMLProstateNavManagerNode* node = this->ProstateNavManager;
  vtkSetAndObserveMRMLNodeEventsMacro ( this->ProstateNavManager, node, events );
  
  int numSteps = this->ProstateNavManager->GetNumberOfSteps();
  for (int i = 0; i < numSteps; i ++)
    {
    vtkProstateNavStep *step=GetStepPage(i);    
    if (step!=NULL)
      {
      step->SetAndObserveMRMLScene(this->GetMRMLScene());
      }
    else
      {
      vtkErrorMacro("Invalid step page: "<<i);
      }    
    }


  //if (linxnode->IsA("vtkMRMLBrpRobotCommandNode"))
  //  {
  //  std::cerr << "OK" << std::endl;
  //  }
  //else
  //  {
  //  std::cerr << "NOT OK" << std::endl;
  //  }
  //
  //std::vector<vtkMRMLNode*> nodes;
  //this->GetMRMLScene()->GetNodesByClass("vtkMRMLBrpRobotCommandNode", nodes);



}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::DataCallback(vtkObject *caller, 
        unsigned long eid, void *clientData, void *callData)
{
  vtkProstateNavGUI *self = reinterpret_cast<vtkProstateNavGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkProstateNavGUI DataCallback");
  
  self->UpdateAll();
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

  if (this->ProstateNavManager && this->ProstateNavManager == vtkMRMLProstateNavManagerNode::SafeDownCast(caller))
    {
    if (event == vtkMRMLIGTLConnectorNode::ConnectedEvent || event == vtkMRMLIGTLConnectorNode::DisconnectedEvent)
      {
      const char* connector = (char*) callData;
      if (strcmp(connector, "Robot") == 0)
        {
        if (event == vtkMRMLIGTLConnectorNode::ConnectedEvent)
          {
          this->RobotConnectedFlag = 1;
          }
        else
          {
          this->RobotConnectedFlag = 2;
          }
        }
      else // if (strcmp(connector, "Scanner") == 0))
        if (event == vtkMRMLIGTLConnectorNode::ConnectedEvent)
          {
          this->ScannerConnectedFlag = 1;
          }
        else
          {
          this->ScannerConnectedFlag = 2;
          }
      }
    }
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::Enter()
{

  // Fill in
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (this->Entered == 0)
    {
    this->GetLogic()->SetGUI(this);

    ChangeWorkPhase(vtkProstateNavLogic::StartUp, 1);
  
    //----------------------------------------------------------------
    // Following code should be in the logic class, but GetApplication()
    // is not available there.
    
    vtkOpenIGTLinkIFGUI* igtlGUI = 
      vtkOpenIGTLinkIFGUI::SafeDownCast(vtkSlicerApplication::SafeDownCast(this->GetApplication())
                                        ->GetModuleGUIByName("OpenIGTLink IF"));
    if (igtlGUI)
      {
      igtlGUI->Enter();
      this->CoordinateConverter = vtkIGTLToMRMLCoordinate::New();
      this->CommandConverter = vtkIGTLToMRMLBrpRobotCommand::New();
      //igtlGUI->GetLogic()->RegisterMessageConverter(this->CoordinateConverter);
      igtlGUI->GetLogic()->RegisterMessageConverter(this->CommandConverter);
      }
    
    //this->GetLogic()->Enter();

    std::cerr << "Adding robot command" << std::endl;
    
    // Set up robot command node.
    vtkMRMLBrpRobotCommandNode* node = vtkMRMLBrpRobotCommandNode::New();
    node->SetName("BRPRobotCommand");
    node->SetScene(this->GetMRMLScene());
    this->GetMRMLScene()->SaveStateForUndo();
    this->GetMRMLScene()->AddNode(node);
    node->Modified();
    this->GetMRMLScene()->Modified();
    //this->RobotCommandNodeID = node->GetID();
    this->ProstateNavManager->SetAndObserveRobotCommand(node);

    node->Delete();
    
    // Target node
    vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::New();
    tnode->SetName("BRPRobotTarget");
    
    vtkMatrix4x4* transform = vtkMatrix4x4::New();
    transform->Identity();
    //transformNode->SetAndObserveImageData(transform);
    tnode->ApplyTransform(transform);
    tnode->SetScene(this->MRMLScene);
    transform->Delete();
    
    this->GetMRMLScene()->AddNode(tnode);
    //this->RobotTargetNodeID = tnode->GetID();
    this->ProstateNavManager->SetAndObserveRobotTarget(tnode);
    tnode->Delete();
    
    
    // Z-Frame node
    vtkMRMLLinearTransformNode* ztnode = vtkMRMLLinearTransformNode::New();
    ztnode->SetName("ZFrameTransform");
    vtkMatrix4x4* ztransform = vtkMatrix4x4::New();
    ztransform->Identity();
    //transformNode->SetAndObserveImageData(transform);
    ztnode->ApplyTransform(ztransform);
    ztnode->SetScene(this->MRMLScene);
    ztransform->Delete();
    this->GetMRMLScene()->AddNode(ztnode);
    //this->ZFrameTransformNodeID = ztnode->GetID();
    this->ProstateNavManager->SetAndObserveZFrameTransform(ztnode);
    
    // ZFrame
    // This part should be moved to Robot Display Node.
    const char* nodeID = AddZFrameModel("ZFrame");
    vtkMRMLModelNode*  modelNode =
      vtkMRMLModelNode::SafeDownCast(this->MRMLScene->GetNodeByID(nodeID));
    
    if (modelNode)
      {
        vtkMRMLDisplayNode* displayNode = modelNode->GetDisplayNode();
        displayNode->SetVisibility(0);
        modelNode->Modified();
        this->MRMLScene->Modified();
        modelNode->SetAndObserveTransformNodeID(ztnode->GetID());
        this->ProstateNavManager->SetZFrameModel(modelNode);
      }

    ztnode->Delete();
    
    this->Entered = 1;
    }  

  this->TimerFlag = 1;
  this->TimerInterval = 100;  // 100 ms
  ProcessTimerEvents();
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::Exit ( )
{
  // stop timer on leaving the module
  this->TimerFlag = 0;  
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::BuildGUI ( )
{

    // ---
    // MODULE GUI FRAME 
    // create a page
    this->UIPanel->AddPage ( "ProstateNav", "ProstateNav", NULL );

    BuildGUIForHelpFrame();
    BuildGUIForWorkPhaseFrame ();
    BuildGUIForWizardFrame();

}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::TearDownGUI ( )
{


  // disconnect circular references so destructor can be called
  
  this->GetLogic()->SetGUI(NULL);
  int numSteps = this->ProstateNavManager->GetNumberOfSteps();
  for (int i = 0; i < numSteps; i ++)
    {
    vtkProstateNavStep *step=GetStepPage(i);    
    if (step!=NULL)
      {
      step->SetGUI(NULL);
      step->SetLogic(NULL);
      }
    else
      {
      vtkErrorMacro("Invalid step page: "<<i);
      }    
    }
}


//---------------------------------------------------------------------------
void vtkProstateNavGUI::BuildGUIForWizardFrame()
{
    // the widget shall be dynamically created/destroyed when the
    // wizard GUI is built, because there is no API to remove any steps
    // from the wizard (only to add steps)
    if (this->WizardWidget)
      {
      this->WizardWidget->SetParent(NULL);
      this->WizardWidget->Delete(); 
      this->WizardWidget = NULL;
      }
    this->WizardWidget=vtkKWWizardWidget::New();

    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "ProstateNav" );
    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    

    // ----------------------------------------------------------------
    // WIZARD FRAME         
    // ----------------------------------------------------------------

    vtkSlicerModuleCollapsibleFrame *wizardFrame = 
      vtkSlicerModuleCollapsibleFrame::New();
    wizardFrame->SetParent(page);
    wizardFrame->Create();
    wizardFrame->SetLabelText("Wizard");
    wizardFrame->ExpandFrame();

    app->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                wizardFrame->GetWidgetName(), 
                page->GetWidgetName());
   
    this->WizardWidget->SetParent(wizardFrame->GetFrame());
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

    app->Script("pack %s -side top -anchor nw -fill both -expand y",
                this->WizardWidget->GetWidgetName());
    wizardFrame->Delete();

    // -----------------------------------------------------------------
    // Add the steps to the workflow

    vtkKWWizardWorkflow *wizard_workflow = 
      this->WizardWidget->GetWizardWorkflow();


    // -----------------------------------------------------------------
    // Set GUI/Logic to each step and add to workflow
  
    int numSteps = this->ProstateNavManager->GetNumberOfSteps();

    for (int i = 0; i < numSteps; i ++)
      {

        vtkStdString stepName=this->ProstateNavManager->GetStepName(i);

        vtkProstateNavStep* newStep=NULL;

        if (!stepName.compare("SetUp"))
          {
          vtkProstateNavStepSetUp* setupStep = vtkProstateNavStepSetUp::New();
          setupStep->SetTitleBackgroundColor(205.0/255.0, 200.0/255.0, 177.0/255.0);
          //this->ProstateNavManager->AddNewStep("Set Up", setupStep, "START_UP");
          newStep=setupStep;
          }
        else if (!stepName.compare("ZFrameCalibration"))
          {
          vtkProstateNavCalibrationStep* calibrationStep = vtkProstateNavCalibrationStep::New();
          calibrationStep->SetTitleBackgroundColor(193.0/255.0, 115.0/255.0, 80.0/255.0);
          newStep=calibrationStep;
          }
        else if (!stepName.compare("PointTargeting"))
          {
          vtkProstateNavTargetingStep* targetingStep = vtkProstateNavTargetingStep::New();
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
          newStep->SetProstateNavManager(this->ProstateNavManager);
          newStep->SetTotalSteps(numSteps);
          newStep->SetStepNumber(i+1);
          newStep->UpdateName();
          
          wizard_workflow->AddNextStep(newStep);          

          this->WorkPhaseButtonSet->AddWidget(i);
          this->WorkPhaseButtonSet->GetWidget(i)->SetWidth(16);
                    
          this->WorkPhaseButtonSet->GetWidget(i)->SetText(newStep->GetTitle());

          double r;
          double g;
          double b;
          newStep->GetTitleBackgroundColor(&r, &g, &b);
          this->WorkPhaseButtonSet->GetWidget(i)->SetBackgroundColor(r, g, b);
          this->WorkPhaseButtonSet->GetWidget(i)->SetActiveBackgroundColor(r, g, b);

        }
      
      }


    // -----------------------------------------------------------------
    // Initial and finish step

    wizard_workflow->SetFinishStep(GetStepPage(numSteps-1));
    wizard_workflow->CreateGoToTransitionsToFinishStep();
    wizard_workflow->SetInitialStep(GetStepPage(0));

    // -----------------------------------------------------------------
    // Show the user interface

    if (wizard_workflow->GetCurrentStep())
      {
      wizard_workflow->GetCurrentStep()->ShowUserInterface();
      }

}


void vtkProstateNavGUI::BuildGUIForHelpFrame ()
{

    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "ProstateNav" );

    // Define your help text here.
    const char *help = 
      "The **ProstateNav Module** helps you to do prostate Biopsy and Treatment by:"
      " getting Realtime Images from MR-Scanner into Slicer3, control Scanner with Slicer 3,"
      " determin fiducial detection and control the Robot."
      " Module and Logic mainly coded by Junichi Tokuda, David Gobbi and Philip Mewes"; 

    // ----------------------------------------------------------------
    // HELP FRAME         
    // ----------------------------------------------------------------
    vtkSlicerModuleCollapsibleFrame *ProstateNavHelpFrame = vtkSlicerModuleCollapsibleFrame::New ( );
    ProstateNavHelpFrame->SetParent ( page );
    ProstateNavHelpFrame->Create ( );
    ProstateNavHelpFrame->CollapseFrame ( );
    ProstateNavHelpFrame->SetLabelText ("Help");
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
        ProstateNavHelpFrame->GetWidgetName(), page->GetWidgetName());

    // configure the parent classes help text widget
    this->HelpText->SetParent ( ProstateNavHelpFrame->GetFrame() );
    this->HelpText->Create ( );
    this->HelpText->SetHorizontalScrollbarVisibility ( 0 );
    this->HelpText->SetVerticalScrollbarVisibility ( 1 );
    this->HelpText->GetWidget()->SetText ( help );
    this->HelpText->GetWidget()->SetReliefToFlat ( );
    this->HelpText->GetWidget()->SetWrapToWord ( );
    this->HelpText->GetWidget()->ReadOnlyOn ( );
    this->HelpText->GetWidget()->QuickFormattingOn ( );
    this->HelpText->GetWidget()->SetBalloonHelpString ( "" );
    app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 4",
        this->HelpText->GetWidgetName ( ) );

    ProstateNavHelpFrame->Delete();

}

//---------------------------------------------------------------------------
void vtkProstateNavGUI::BuildGUIForWorkPhaseFrame ()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "ProstateNav" );
  
  vtkSlicerModuleCollapsibleFrame *workphaseFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  workphaseFrame->SetParent(page);
  workphaseFrame->Create();
  workphaseFrame->SetLabelText("Workphase Frame");
  workphaseFrame->ExpandFrame();
  app->Script("pack %s -side top -anchor center -fill x -padx 2 -pady 2 -in %s",
              workphaseFrame->GetWidgetName(), page->GetWidgetName());
  

  // -----------------------------------------
  // Frames

  vtkKWFrame *workphaseStatusFrame = vtkKWFrame::New ( );
  workphaseStatusFrame->SetParent ( workphaseFrame->GetFrame() );
  workphaseStatusFrame->Create ( );
  
  vtkKWFrame *buttonFrame = vtkKWFrame::New();
  buttonFrame->SetParent( workphaseFrame->GetFrame());
  buttonFrame->Create();
  
  app->Script ( "pack %s %s -side top -fill x -expand y -padx 1 -pady 1",
                workphaseStatusFrame->GetWidgetName(),
                buttonFrame->GetWidgetName());
  
  // -----------------------------------------
  // Work Phase Display Frame

  this->ScannerStatusLabelDisp = vtkKWEntry::New();
  this->ScannerStatusLabelDisp->SetParent(workphaseStatusFrame);
  this->ScannerStatusLabelDisp->Create();
  this->ScannerStatusLabelDisp->ReadOnlyOn();
  this->ScannerStatusLabelDisp->SetBorderWidth(1);
  this->ScannerStatusLabelDisp->SetReadOnlyBackgroundColor(0.9, 0.9, 0.9);
  this->ScannerStatusLabelDisp->SetForegroundColor(0.4, 0.4, 0.4);
  this->ScannerStatusLabelDisp->SetValue (" SCANNER: OFF ");
  
  this->RobotStatusLabelDisp = vtkKWEntry::New();
  this->RobotStatusLabelDisp->SetParent(workphaseStatusFrame);
  this->RobotStatusLabelDisp->Create();
  this->RobotStatusLabelDisp->ReadOnlyOn();
  this->RobotStatusLabelDisp->SetBorderWidth(1);
  this->RobotStatusLabelDisp->SetReadOnlyBackgroundColor(0.9, 0.9, 0.9);
  this->RobotStatusLabelDisp->SetForegroundColor(0.4, 0.4, 0.4);
  this->RobotStatusLabelDisp->SetValue (" ROBOT: OFF ");
  
  this->Script("pack %s %s -side left -fill x -expand y -padx 1 -pady 1", 
               RobotStatusLabelDisp->GetWidgetName(),
               ScannerStatusLabelDisp->GetWidgetName()
               );
  
  // -----------------------------------------
  // Work Phase Transition Buttons Frame

  this->WorkPhaseButtonSet = vtkKWPushButtonSet::New();
  this->WorkPhaseButtonSet->SetParent(buttonFrame);
  this->WorkPhaseButtonSet->Create();
  this->WorkPhaseButtonSet->PackHorizontallyOn();
  this->WorkPhaseButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(3);
  this->WorkPhaseButtonSet->SetWidgetsPadX(1);
  this->WorkPhaseButtonSet->SetWidgetsPadY(1);
  this->WorkPhaseButtonSet->UniformColumnsOn();
  this->WorkPhaseButtonSet->UniformRowsOn();
  
  this->Script("pack %s -side left -anchor w -fill x -padx 1 -pady 1", 
               this->WorkPhaseButtonSet->GetWidgetName());
  
  workphaseFrame->Delete ();
  buttonFrame->Delete ();
  workphaseStatusFrame->Delete ();
  
}

//----------------------------------------------------------------------------
int vtkProstateNavGUI::ChangeWorkPhase(int phase, int fChangeWizard)
{


  if (!this->ProstateNavManager->SwitchStep(phase)) // Set next phase
    {
    cerr << "ChangeWorkPhase: Cannot transition!" << endl;
    return 0;
    }
  
  int numSteps = this->ProstateNavManager->GetNumberOfSteps();
  
  for (int i = 0; i < numSteps; i ++)
    {
    vtkKWPushButton *pb = this->WorkPhaseButtonSet->GetWidget(i);
    bool transitionable=true; // :TODO: get this information from the workflow widget state machine
    if (i == this->ProstateNavManager->GetCurrentStep())
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
    step_to = this->ProstateNavManager->GetCurrentStep();
    //step_from = this->Logic->GetPrevPhase();
    step_from = this->ProstateNavManager->GetPreviousStep();
    
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
    wizard->GetCurrentStep()->ShowUserInterface();
    }
  
  return 1;
}


//----------------------------------------------------------------------------
void vtkProstateNavGUI::UpdateAll()
{

}


//----------------------------------------------------------------------------
const char* vtkProstateNavGUI::AddZFrameModel(const char* nodeName)
{
  std::string rstr;

  vtkMRMLModelNode           *zframeModel;
  vtkMRMLModelDisplayNode    *zframeDisp;

  zframeModel = vtkMRMLModelNode::New();
  zframeDisp = vtkMRMLModelDisplayNode::New();

  this->MRMLScene->SaveStateForUndo();
  this->MRMLScene->AddNode(zframeDisp);
  this->MRMLScene->AddNode(zframeModel);  

  zframeDisp->SetScene(this->MRMLScene);
  zframeModel->SetName(nodeName);
  zframeModel->SetScene(this->MRMLScene);
  zframeModel->SetAndObserveDisplayNodeID(zframeDisp->GetID());
  zframeModel->SetHideFromEditors(0);

  // construct Z-frame model
  const double length = 60; // mm

  //----- cylinder 1 (R-A) -----
  vtkCylinderSource *cylinder1 = vtkCylinderSource::New();
  cylinder1->SetRadius(1.5);
  cylinder1->SetHeight(length);
  cylinder1->SetCenter(0, 0, 0);
  cylinder1->Update();
  
  vtkTransformPolyDataFilter *tfilter1 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans1 =   vtkTransform::New();
  trans1->Translate(length/2.0, length/2.0, 0.0);
  trans1->RotateX(90.0);
  trans1->Update();
  tfilter1->SetInput(cylinder1->GetOutput());
  tfilter1->SetTransform(trans1);
  tfilter1->Update();


  //----- cylinder 2 (R-center) -----
  vtkCylinderSource *cylinder2 = vtkCylinderSource::New();
  cylinder2->SetRadius(1.5);
  cylinder2->SetHeight(length*1.4142135);
  cylinder2->SetCenter(0, 0, 0);
  cylinder2->Update();

  vtkTransformPolyDataFilter *tfilter2 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans2 =   vtkTransform::New();
  trans2->Translate(length/2.0, 0.0, 0.0);
  trans2->RotateX(90.0);
  trans2->RotateX(-45.0);
  trans2->Update();
  tfilter2->SetInput(cylinder2->GetOutput());
  tfilter2->SetTransform(trans2);
  tfilter2->Update();


  //----- cylinder 3 (R-P) -----
  vtkCylinderSource *cylinder3 = vtkCylinderSource::New();
  cylinder3->SetRadius(1.5);
  cylinder3->SetHeight(length);
  cylinder3->SetCenter(0, 0, 0);
  cylinder3->Update();

  vtkTransformPolyDataFilter *tfilter3 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans3 =   vtkTransform::New();
  trans3->Translate(length/2.0, -length/2.0, 0.0);
  trans3->RotateX(90.0);
  trans3->Update();
  tfilter3->SetInput(cylinder3->GetOutput());
  tfilter3->SetTransform(trans3);
  tfilter3->Update();


  //----- cylinder 4 (center-P) -----  
  vtkCylinderSource *cylinder4 = vtkCylinderSource::New();
  cylinder4->SetRadius(1.5);
  cylinder4->SetHeight(length*1.4142135);
  cylinder4->SetCenter(0, 0, 0);
  cylinder4->Update();

  vtkTransformPolyDataFilter *tfilter4 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans4 =   vtkTransform::New();
  trans4->Translate(0.0, -length/2.0, 0.0);
  trans4->RotateX(90.0);
  trans4->RotateZ(-45.0);
  trans4->Update();
  tfilter4->SetInput(cylinder4->GetOutput());
  tfilter4->SetTransform(trans4);
  tfilter4->Update();


  //----- cylinder 5 (L-P) -----  
  vtkCylinderSource *cylinder5 = vtkCylinderSource::New();
  cylinder5->SetRadius(1.5);
  cylinder5->SetHeight(length);
  cylinder5->SetCenter(0, 0, 0);
  cylinder5->Update();

  vtkTransformPolyDataFilter *tfilter5 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans5 =   vtkTransform::New();
  trans5->Translate(-length/2.0, -length/2.0, 0.0);
  trans5->RotateX(90.0);
  trans5->Update();
  tfilter5->SetInput(cylinder5->GetOutput());
  tfilter5->SetTransform(trans5);
  tfilter5->Update();


  //----- cylinder 6 (L-center) -----  
  vtkCylinderSource *cylinder6 = vtkCylinderSource::New();
  cylinder6->SetRadius(1.5);
  cylinder6->SetHeight(length*1.4142135);
  cylinder6->SetCenter(0, 0, 0);
  cylinder6->Update();

  vtkTransformPolyDataFilter *tfilter6 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans6 =   vtkTransform::New();
  trans6->Translate(-length/2.0, 0.0, 0.0);
  trans6->RotateX(90.0);
  trans6->RotateX(45.0);
  trans6->Update();
  tfilter6->SetInput(cylinder6->GetOutput());
  tfilter6->SetTransform(trans6);
  tfilter6->Update();


  //----- cylinder 7 (L-A) -----  
  vtkCylinderSource *cylinder7 = vtkCylinderSource::New();
  cylinder7->SetRadius(1.5);
  cylinder7->SetHeight(length);
  cylinder7->SetCenter(0, 0, 0);
  cylinder7->Update();

  vtkTransformPolyDataFilter *tfilter7 = vtkTransformPolyDataFilter::New();
  vtkTransform* trans7 =   vtkTransform::New();
  trans7->Translate(-length/2.0, length/2.0, 0.0);
  trans7->RotateX(90.0);
  trans7->Update();
  tfilter7->SetInput(cylinder7->GetOutput());
  tfilter7->SetTransform(trans7);
  tfilter7->Update();

  vtkAppendPolyData *apd = vtkAppendPolyData::New();
  apd->AddInput(tfilter1->GetOutput());
  apd->AddInput(tfilter2->GetOutput());
  apd->AddInput(tfilter3->GetOutput());
  apd->AddInput(tfilter4->GetOutput());
  apd->AddInput(tfilter5->GetOutput());
  apd->AddInput(tfilter6->GetOutput());
  apd->AddInput(tfilter7->GetOutput());
  apd->Update();
  
  zframeModel->SetAndObservePolyData(apd->GetOutput());

  double color[3];
  color[0] = 1.0;
  color[1] = 1.0;
  color[2] = 0.0;
  zframeDisp->SetPolyData(zframeModel->GetPolyData());
  zframeDisp->SetColor(color);

  rstr = zframeModel->GetID();
  
  trans1->Delete();
  trans2->Delete();
  trans3->Delete();
  trans4->Delete();
  trans5->Delete();
  trans6->Delete();
  trans7->Delete();
  tfilter1->Delete();
  tfilter2->Delete();
  tfilter3->Delete();
  tfilter4->Delete();
  tfilter5->Delete();
  tfilter6->Delete();
  tfilter7->Delete();
  cylinder1->Delete();
  cylinder2->Delete();
  cylinder3->Delete();
  cylinder4->Delete();
  cylinder5->Delete();
  cylinder6->Delete();
  cylinder7->Delete();

  apd->Delete();

  zframeDisp->Delete();
  zframeModel->Delete();

  return rstr.c_str();

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
