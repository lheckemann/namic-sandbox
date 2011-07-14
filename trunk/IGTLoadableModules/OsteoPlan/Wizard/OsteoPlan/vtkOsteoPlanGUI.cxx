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

#include "vtkOsteoPlanGUI.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"
#include "vtkCornerAnnotation.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkMRMLOsteoPlanNode.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"
#include "vtkKWPushButton.h"
#include "vtkKWPushButtonSet.h"

#include "vtkOsteoPlanStep.h"
#include "vtkOsteoPlanCuttingModelStep.h"
#include "vtkOsteoPlanSelectingPartsStep.h"
#include "vtkOsteoPlanMovingPartsStep.h"
#include "vtkOsteoPlanPlacingFiducialsStep.h"
#include "vtkOsteoPlanReturningOriginalPositionStep.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkOsteoPlanGUI );
vtkCxxRevisionMacro ( vtkOsteoPlanGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkOsteoPlanGUI::vtkOsteoPlanGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkOsteoPlanGUI::DataCallback);
  
  this->OsteoPlanNode = vtkMRMLOsteoPlanNode::New();

  //----------------------------------------------------------------
  // GUI widgets

  this->WorkflowButtonSet = NULL;

  //----------------------------------------------------------------
  // GUI widgets

  this->WizardFrame = vtkSlicerModuleCollapsibleFrame::New();
  this->WizardWidget = NULL;

  //----------------------------------------------------------------
  // Wizard Steps

  this->CuttingStep = NULL;
  this->SelectingStep = NULL;  
  this->MovingStep = NULL;
  this->PlacingStep = NULL;
  this->ReturningStep = NULL;

  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkOsteoPlanGUI::~vtkOsteoPlanGUI ( )
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


  if(this->WorkflowButtonSet)
    {
    this->WorkflowButtonSet->SetParent(NULL);
    this->WorkflowButtonSet->Delete();
    this->WorkflowButtonSet = NULL;
    }


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

  if(this->OsteoPlanNode)
    {
      this->OsteoPlanNode->Delete();
    }

  //----------------------------------------------------------------
  // Wizard Step

  if(this->CuttingStep)
    {
    this->CuttingStep->Delete(); 
    this->CuttingStep = NULL;
    }

  if(this->SelectingStep)
    {
    this->SelectingStep->Delete(); 
    this->SelectingStep = NULL;
    }

  if(this->MovingStep)
    {
    this->MovingStep->Delete(); 
    this->MovingStep = NULL;
    }

  if(this->PlacingStep)
    {
    this->PlacingStep->Delete(); 
    this->PlacingStep = NULL;
    }

  if(this->ReturningStep)
    {
    this->ReturningStep->Delete(); 
    this->ReturningStep = NULL;
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::Enter()
{
  // Fill in
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }

}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "OsteoPlanGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();


  if(this->WorkflowButtonSet)
    {
    for(int i = 0; i < this->WorkflowButtonSet->GetNumberOfWidgets(); i++)
      {
      this->WorkflowButtonSet->GetWidget(i)->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
      }
    }

  if (this->WizardWidget)
    {
    this->WizardWidget->GetWizardWorkflow()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }


  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  //events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  //events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  
  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

  if (this->WorkflowButtonSet!=NULL)
    {
    for (int i = 0; i < this->WorkflowButtonSet->GetNumberOfWidgets(); i++)
      {
      this->WorkflowButtonSet->GetWidget(i)
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


  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }

}




//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkOsteoPlanLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::ProcessGUIEvents(vtkObject *caller,
                                        unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  vtkKWPushButton* pushButtonCaller = vtkKWPushButton::SafeDownCast(caller);
  vtkKWPushButtonSet* pushButtonCallerParent = NULL;
  if(pushButtonCaller != NULL)
    {
    pushButtonCallerParent = vtkKWPushButtonSet::SafeDownCast(pushButtonCaller->GetParent());
    }

  if(this->WorkflowButtonSet != NULL && pushButtonCallerParent == this->WorkflowButtonSet
     && event == vtkKWPushButton::InvokedEvent)
    {
      for(int i = 0; i < this->WorkflowButtonSet->GetNumberOfWidgets(); i++)
  {
    if(this->WorkflowButtonSet->GetWidget(i) == pushButtonCaller)
      {
        ChangeWorkphaseGUI(i);
      }
  }

    }  
} 


void vtkOsteoPlanGUI::DataCallback(vtkObject *caller, 
                                    unsigned long eid, void *clientData, void *callData)
{
  vtkOsteoPlanGUI *self = reinterpret_cast<vtkOsteoPlanGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkOsteoPlanGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::ProcessLogicEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkOsteoPlanLogic::SafeDownCast(caller))
    {
    if (event == vtkOsteoPlanLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::ProcessMRMLEvents ( vtkObject *caller,
                                           unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::ProcessTimerEvents()
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
void vtkOsteoPlanGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "OsteoPlan", "OsteoPlan", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForWorkflowFrame();
  BuildGUIForWizardFrame();
}


void vtkOsteoPlanGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:OsteoPlan</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "OsteoPlan" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::TearDownGUI()
{
  this->RemoveGUIObservers();

  //  this->GetLogic()->SetGUI(NULL);
  if(this->WizardWidget!=NULL)
    {

      for(int i = 0; i < this->WizardWidget->GetWizardWorkflow()->GetNumberOfSteps(); i++)
  {
    vtkOsteoPlanStep* step = vtkOsteoPlanStep::SafeDownCast(this->WizardWidget->GetWizardWorkflow()->GetNthStep(i));
    if(step!=NULL)
      {
        step->TearDownGUI();
        step->SetGUI(NULL);
        step->SetLogic(NULL);
      }
    else
      {
        vtkErrorMacro("Invalid step page: "<<i);
      }
  }
    }
}

//---------------------------------------------------------------------------
void vtkOsteoPlanGUI::BuildGUIForWorkflowFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("OsteoPlan");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("OsteoPlan Workflow");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Test push button

  this->WorkflowButtonSet = vtkKWPushButtonSet::New();
  this->WorkflowButtonSet->SetParent(conBrowsFrame->GetFrame());
  this->WorkflowButtonSet->Create();
  this->WorkflowButtonSet->PackHorizontallyOn();
  this->WorkflowButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(3);
  this->WorkflowButtonSet->SetWidgetsPadX(1);
  this->WorkflowButtonSet->SetWidgetsPadY(1);
  this->WorkflowButtonSet->UniformColumnsOn();
  this->WorkflowButtonSet->UniformRowsOn();


  this->Script("pack %s -side left -padx 2 -pady 2", 
               this->WorkflowButtonSet->GetWidgetName());

  conBrowsFrame->Delete();

}



void vtkOsteoPlanGUI::BuildGUIForWizardFrame()
{
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "OsteoPlan" );
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

  if (!this->WizardFrame->IsCreated())
    {
    this->WizardFrame->SetParent(page);
    this->WizardFrame->Create();
    this->WizardFrame->SetLabelText("Wizard");
    this->WizardFrame->ExpandFrame();

    app->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 0 -in %s",
                this->WizardFrame->GetWidgetName(), 
                page->GetWidgetName());
    }



  this->WizardWidget=vtkKWWizardWidget::New();   
  this->WizardWidget->SetParent(this->WizardFrame->GetFrame());
  this->WizardWidget->Create();
  this->WizardWidget->GetSubTitleLabel()->SetHeight(1);
  this->WizardWidget->SetClientAreaMinimumHeight(200);
  this->WizardWidget->NextButtonVisibilityOn();
  this->WizardWidget->BackButtonVisibilityOn();
  this->WizardWidget->OKButtonVisibilityOff();
  this->WizardWidget->CancelButtonVisibilityOff();
  this->WizardWidget->FinishButtonVisibilityOff();
  this->WizardWidget->HelpButtonVisibilityOff();

 
  this->Script("pack %s -side top -anchor nw -fill both -expand y",
               this->WizardWidget->GetWidgetName());
 


  //==============================================
  // Create Wizard
  // And Setup Steps

  vtkKWWizardWorkflow* wizard_workflow = this->WizardWidget->GetWizardWorkflow();

  // First Step: Cutting Model
  if(!this->CuttingStep)
    {
      this->CuttingStep = vtkOsteoPlanCuttingModelStep::New();
      PrepareMyStep(this->CuttingStep); 
      wizard_workflow->AddStep(this->CuttingStep);
      UpdateWorkflowStepNames();
    }

  // Second Step: Select Parts of the model
  if(!this->SelectingStep)
    {
      this->SelectingStep = vtkOsteoPlanSelectingPartsStep::New();
      PrepareMyStep(this->SelectingStep);
      wizard_workflow->AddNextStep(this->SelectingStep);
      UpdateWorkflowStepNames();
    }

  // Third Step: Move parts of the model
  if(!this->MovingStep)
    {
      this->MovingStep = vtkOsteoPlanMovingPartsStep::New();
      PrepareMyStep(this->MovingStep);
      wizard_workflow->AddNextStep(this->MovingStep);
      UpdateWorkflowStepNames();
    }

  if(!this->PlacingStep)
    {
      this->PlacingStep = vtkOsteoPlanPlacingFiducialsStep::New();
      PrepareMyStep(this->PlacingStep);
      wizard_workflow->AddNextStep(this->PlacingStep);
      UpdateWorkflowStepNames();
    }

  if(!this->ReturningStep)
    {
      this->ReturningStep = vtkOsteoPlanReturningOriginalPositionStep::New();
      PrepareMyStep(this->ReturningStep);
      wizard_workflow->AddNextStep(this->ReturningStep);
      UpdateWorkflowStepNames();
    }


  // Start State Machine
  wizard_workflow->SetFinishStep(this->ReturningStep);  
  wizard_workflow->SetInitialStep(this->CuttingStep);

  //==============================================

  this->GetWizardWidget()->Update();


}


//----------------------------------------------------------------------------
void vtkOsteoPlanGUI::UpdateAll()
{
}

//----------------------------------------------------------------------------
void vtkOsteoPlanGUI::PrepareMyStep(vtkOsteoPlanStep* wStep)
{

  // Add step to the workflow
  vtkKWWizardWorkflow* wizard_workflow = this->WizardWidget->GetWizardWorkflow();

  if(wizard_workflow != NULL)
    {
    int numSteps = wizard_workflow->GetNumberOfSteps();
    if(wStep != NULL)
      {
      wStep->SetGUI(this);
      wStep->SetLogic(this->Logic);
      wStep->SetApplication(this->GetApplication());
      wStep->SetTotalSteps(numSteps+1);
      wStep->SetStepNumber(numSteps+1);
      }
  
    // Update GUI
    int insertStep = this->WorkflowButtonSet->GetNumberOfWidgets();
    if( insertStep == numSteps)
      {
      // Synchronize buttons with steps
      double r,g,b;
      wStep->GetTitleBackgroundColor(&r, &g, &b);

      this->WorkflowButtonSet->AddWidget(insertStep);
      this->WorkflowButtonSet->GetWidget(insertStep)->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
      this->WorkflowButtonSet->GetWidget(insertStep)->SetWidth(16);
      this->WorkflowButtonSet->GetWidget(insertStep)->SetText(wStep->GetTitle());
      this->WorkflowButtonSet->GetWidget(insertStep)->SetBackgroundColor(r,g,b);
      this->WorkflowButtonSet->GetWidget(insertStep)->SetActiveBackgroundColor(r,g,b);
      }

    }

}

void vtkOsteoPlanGUI::UpdateWorkflowStepNames()
{
  int numSteps = this->WizardWidget->GetWizardWorkflow()->GetNumberOfSteps();

  for(int i = 0; i < numSteps; i++)
    {
    vtkOsteoPlanStep* renameStep = vtkOsteoPlanStep::SafeDownCast(this->WizardWidget->GetWizardWorkflow()->GetNthStep(i));
    renameStep->SetTotalSteps(numSteps);
    renameStep->UpdateName();
    }

}


void vtkOsteoPlanGUI::ChangeWorkphaseGUI(int StepNumberToGo)
{

  if(this->WizardWidget)
    {
    vtkKWWizardWorkflow* wizard = this->WizardWidget->GetWizardWorkflow();
    if(wizard)
      {
      vtkKWWizardStep* currentStep = this->WizardWidget->GetWizardWorkflow()->GetCurrentStep();
      int stepNumber = GetStepNumber(currentStep);    
      if(stepNumber >= 0)
        {
        int difference = StepNumberToGo - stepNumber;
        if(difference > 0)
          {
          for(int i = 0; i < difference; i++)
            {
            wizard->AttemptToGoToNextStep();
            }
          }
        else
          {
          difference = -difference;
          for(int i = 0; i < difference; i++)
            {
            wizard->AttemptToGoToPreviousStep();
            }
          }
        }
      }
    }

}

int vtkOsteoPlanGUI::GetStepNumber(vtkKWWizardStep* step)
{
  if(this->WizardWidget)
    {
      vtkKWWizardWorkflow* wizard = this->WizardWidget->GetWizardWorkflow();
      if(wizard)
  {
    for(int i = 0; i < wizard->GetNumberOfSteps(); i++)
      {
        if(wizard->GetNthStep(i) == step)
    {
      return i;
    }
      }
  }
    }
  return -1;
}
