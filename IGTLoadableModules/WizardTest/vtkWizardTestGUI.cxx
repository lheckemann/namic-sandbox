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

#include "vtkWizardTestGUI.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"

#include "vtkKWPushButton.h"
#include "vtkKWPushButtonSet.h"

#include "vtkCornerAnnotation.h"


#include "vtkWizardTestStep.h"
#include "vtkWizardTestStepOne.h"
#include "vtkWizardTestStepTwo.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkWizardTestGUI );
vtkCxxRevisionMacro ( vtkWizardTestGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkWizardTestGUI::vtkWizardTestGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkWizardTestGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets

  this->WorkflowButtonSet = NULL;

  //----------------------------------------------------------------
  // GUI widgets

  this->WizardFrame = vtkSlicerModuleCollapsibleFrame::New();
  this->WizardWidget = NULL;
  
  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkWizardTestGUI::~vtkWizardTestGUI ( )
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

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkWizardTestGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkWizardTestGUI::Enter()
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
void vtkWizardTestGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkWizardTestGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "WizardTestGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkWizardTestGUI::RemoveGUIObservers ( )
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
void vtkWizardTestGUI::AddGUIObservers ( )
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
void vtkWizardTestGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }

}




//---------------------------------------------------------------------------
void vtkWizardTestGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkWizardTestLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkWizardTestGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkWizardTestGUI::ProcessGUIEvents(vtkObject *caller,
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


void vtkWizardTestGUI::DataCallback(vtkObject *caller, 
                                    unsigned long eid, void *clientData, void *callData)
{
  vtkWizardTestGUI *self = reinterpret_cast<vtkWizardTestGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkWizardTestGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkWizardTestGUI::ProcessLogicEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkWizardTestLogic::SafeDownCast(caller))
    {
    if (event == vtkWizardTestLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkWizardTestGUI::ProcessMRMLEvents ( vtkObject *caller,
                                           unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkWizardTestGUI::ProcessTimerEvents()
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
void vtkWizardTestGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "WizardTest", "WizardTest", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForTestFrame1();
  BuildGUIForWizardFrame();
}


void vtkWizardTestGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:WizardTest</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "WizardTest" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkWizardTestGUI::TearDownGUI()
{
  this->RemoveGUIObservers();

  //  this->GetLogic()->SetGUI(NULL);
  if(this->WizardWidget!=NULL)
    {

      for(int i = 0; i < this->WizardWidget->GetWizardWorkflow()->GetNumberOfSteps(); i++)
  {
    vtkWizardTestStep* step = vtkWizardTestStep::SafeDownCast(this->WizardWidget->GetWizardWorkflow()->GetNthStep(i));
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
void vtkWizardTestGUI::BuildGUIForTestFrame1()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("WizardTest");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Test Frame 1");
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


//  AddButtons();

  conBrowsFrame->Delete();

}



void vtkWizardTestGUI::BuildGUIForWizardFrame()
{
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "WizardTest" );
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


  vtkKWWizardWorkflow* wizard_workflow = this->WizardWidget->GetWizardWorkflow();

  vtkWizardTestStepOne* firstStep = vtkWizardTestStepOne::New();
  AddMyStep(firstStep); 
  wizard_workflow->AddNextStep(firstStep);
  UpdateWorkflowStepNames();

  vtkWizardTestStepTwo* secondStep = vtkWizardTestStepTwo::New();
  AddMyStep(secondStep);
  wizard_workflow->AddNextStep(secondStep);
  UpdateWorkflowStepNames();

  // Start State Machine
  wizard_workflow->SetFinishStep(secondStep);  
  wizard_workflow->SetInitialStep(firstStep);

  firstStep->Delete(); 
  secondStep->Delete();

}


//----------------------------------------------------------------------------
void vtkWizardTestGUI::UpdateAll()
{
}

//----------------------------------------------------------------------------
void vtkWizardTestGUI::AddMyStep(vtkWizardTestStep* wStep)
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
      wStep->SetTotalSteps(numSteps+1);
      wStep->SetStepNumber(numSteps+1);
      wStep->ShowUserInterface();
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

void vtkWizardTestGUI::UpdateWorkflowStepNames()
{
  int numSteps = this->WizardWidget->GetWizardWorkflow()->GetNumberOfSteps();

  for(int i = 0; i < numSteps; i++)
    {
    vtkWizardTestStep* renameStep = vtkWizardTestStep::SafeDownCast(this->WizardWidget->GetWizardWorkflow()->GetNthStep(i));
    renameStep->SetTotalSteps(numSteps);
    renameStep->UpdateName();
    }

}


void vtkWizardTestGUI::ChangeWorkphaseGUI(int StepNumberToGo)
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

int vtkWizardTestGUI::GetStepNumber(vtkKWWizardStep* step)
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
