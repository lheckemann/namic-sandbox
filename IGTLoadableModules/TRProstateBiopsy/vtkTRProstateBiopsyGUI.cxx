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

#include "vtkTRProstateBiopsyGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkTRProstateBiopsyStep.h"
#include "vtkTRProstateBiopsyCalibrationStep.h"
#include "vtkTRProstateBiopsyTargetingStep.h"
#include "vtkTRProstateBiopsyVerificationStep.h"

#include "vtkSlicerFiducialsGUI.h"
#include "vtkSlicerFiducialsLogic.h"
#include "vtkSlicerInteractorStyle.h"
#include "vtkIGTDataManager.h"
#include "vtkIGTPat2ImgRegistration.h"

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

// for images
#include "vtkSlicerColorLogic.h"

// for 3D object display
#include "vtkCylinderSource.h"
#include "vtkMRMLLinearTransformNode.h"


#include <vector>

//---------------------------------------------------------------------------
vtkStandardNewMacro ( vtkTRProstateBiopsyGUI );
vtkCxxRevisionMacro ( vtkTRProstateBiopsyGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Button Colors and Labels for Work Phase Control
const double vtkTRProstateBiopsyGUI::WorkPhaseColor[vtkTRProstateBiopsyLogic::NumPhases][3] =
  {
  /* Cl */ { 1.0, 0.6, 1.0 },
  /* Tg */ { 1.0, 1.0, 0.6 },
  /* Ve */ { 0.6, 0.6, 1.0 },
  };

const double vtkTRProstateBiopsyGUI::WorkPhaseColorActive[vtkTRProstateBiopsyLogic::NumPhases][3] =
  {
  /* Cl */ { 1.0, 0.4, 1.0 },
  /* Tg */ { 1.0, 1.0, 0.4 },
  /* Ve */ { 0.4, 0.4, 1.0 },
  };

const double vtkTRProstateBiopsyGUI::WorkPhaseColorDisabled[vtkTRProstateBiopsyLogic::NumPhases][3] =
  {
  /* Cl */ { 1.0, 0.95, 1.0 },
  /* Tg */ { 1.0, 1.0, 0.95 },
  /* Ve */ { 0.95, 0.95, 1.0 },
  };

const char *vtkTRProstateBiopsyGUI::WorkPhaseStr[vtkTRProstateBiopsyLogic::NumPhases] =
  {
  /* Cl */ "Calibration",
  /* Tg */ "Targeting",
  /* Ve */ "Verification",
  };

#ifdef _WIN32
#  define slicerCerr(x) \
  do { \
    cout << x; \
    vtkstd::ostringstream str; \
    str << x; \
    MessageBox(NULL, str.str().c_str(), "Slicer Error", MB_OK);\
  } while (0)
#else
#  define slicerCerr(x) \
  cout << x
#endif

//---------------------------------------------------------------------------
vtkTRProstateBiopsyGUI::vtkTRProstateBiopsyGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  
  this->Logic = NULL;
  this->DataManager = vtkIGTDataManager::New();
  this->Pat2ImgReg = vtkIGTPat2ImgRegistration::New();
  
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkTRProstateBiopsyGUI::DataCallback);
  
  this->Logic0 = NULL; 
  this->Logic1 = NULL; 
  this->Logic2 = NULL; 
  this->SliceNode0 = NULL; 
  this->SliceNode1 = NULL; 
  this->SliceNode2 = NULL; 
  this->Control0 = NULL; 
  this->Control1 = NULL; 
  this->Control2 = NULL; 
  
  //----------------------------------------------------------------
  // Workphase Frame
  
  this->WorkPhaseButtonSet = NULL;

  //----------------------------------------------------------------  
  // Wizard Frame
  
  this->WizardWidget = vtkKWWizardWidget::New();
  this->WizardSteps = new vtkTRProstateBiopsyStep*[vtkTRProstateBiopsyLogic::NumPhases];
  for (int i = 0; i < vtkTRProstateBiopsyLogic::NumPhases; i ++)
    {
    this->WizardSteps[i] = NULL;
    }
  
  //----------------------------------------------------------------
  // Target Fiducials List (MRML)

  this->FiducialListNodeID = NULL;
  this->FiducialListNode   = NULL;
  
  
}

//---------------------------------------------------------------------------
vtkTRProstateBiopsyGUI::~vtkTRProstateBiopsyGUI ( )
{

  if (this->DataManager)
    {
    // If we don't set the scence to NULL for DataManager,
    // Slicer will report a lot leak when it is closed.
    this->DataManager->SetMRMLScene(NULL);
    this->DataManager->Delete();
    }
  if (this->Pat2ImgReg)
    {
    this->Pat2ImgReg->Delete();
    }
  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  this->RemoveGUIObservers();


  //----------------------------------------------------------------
  // Workphase Frame

  if (this->WorkPhaseButtonSet)
    {
    this->WorkPhaseButtonSet->SetParent(NULL);
    this->WorkPhaseButtonSet->Delete();
    }


  //----------------------------------------------------------------
  // Wizard Frame

  if (this->WizardWidget)
    {
    this->WizardWidget->Delete();
    this->WizardWidget = NULL;
    }

  this->SetModuleLogic ( NULL );
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::SetModuleLogic(vtkSlicerLogic* logic)
{
  this->SetLogic(dynamic_cast<vtkTRProstateBiopsyLogic*>(logic));
//  this->GetLogic()->GetMRMLManager()->SetMRMLScene(this->GetMRMLScene()); 
//  this->SetMRMLManager(this->GetLogic()->GetMRMLManager());
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->vtkObject::PrintSelf ( os, indent );
    



    os << indent << "TRProstateBiopsyGUI: " << this->GetClassName ( ) << "\n";
    os << indent << "Logic: " << this->GetLogic ( ) << "\n";
   
    // print widgets?
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::RemoveGUIObservers ( )
{
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  appGUI->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI1()->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI2()->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
  
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

  this->WizardWidget->GetWizardWorkflow()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);

  this->RemoveLogicObservers();
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::RemoveLogicObservers ( )
{
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  // make a user interactor style to process our events
  // look at the InteractorStyle to get our events
  
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  appGUI->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI1()->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI2()->GetSliceViewer()->GetRenderWidget()
    ->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  
  //----------------------------------------------------------------
  // Workphase Frame

  for (int i = 0; i < this->WorkPhaseButtonSet->GetNumberOfWidgets(); i ++)
    {
    this->WorkPhaseButtonSet->GetWidget(i)
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  
  
  //----------------------------------------------------------------
  // Wizard Frame

  this->WizardWidget->GetWizardWorkflow()->AddObserver(
                  vtkKWWizardWorkflow::CurrentStateChangedEvent,
                  (vtkCommand *)this->GUICallbackCommand);


  this->AddLogicObservers();
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkTRProstateBiopsyLogic::LocatorUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    this->GetLogic()->AddObserver(vtkTRProstateBiopsyLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    this->GetLogic()->AddObserver(vtkTRProstateBiopsyLogic::SliceUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{

  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  vtkSlicerInteractorStyle *istyle0 
    = vtkSlicerInteractorStyle::SafeDownCast(
                    appGUI->GetMainSliceGUI0()->GetSliceViewer()
                    ->GetRenderWidget()->GetRenderWindowInteractor()
                    ->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyle1 
    = vtkSlicerInteractorStyle::SafeDownCast(
                    appGUI->GetMainSliceGUI1()->GetSliceViewer()
                    ->GetRenderWidget()->GetRenderWindowInteractor()
                    ->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyle2 
    = vtkSlicerInteractorStyle::SafeDownCast(
                    appGUI->GetMainSliceGUI2()->GetSliceViewer()
                    ->GetRenderWidget()->GetRenderWindowInteractor()
                    ->GetInteractorStyle());
  
  vtkCornerAnnotation *anno = NULL;
  if (style == istyle0)
    {
    anno = appGUI->GetMainSliceGUI0()->GetSliceViewer()
            ->GetRenderWidget()->GetCornerAnnotation();
    }
  else if (style == istyle1)
    {
    anno = appGUI->GetMainSliceGUI1()->GetSliceViewer()
            ->GetRenderWidget()->GetCornerAnnotation();
    }
  else if (style == istyle2)
    {
    anno = appGUI->GetMainSliceGUI2()->GetSliceViewer()
            ->GetRenderWidget()->GetCornerAnnotation();
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
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style =
            vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);

    return;
    }

  //----------------------------------------------------------------
  // Check Work Phase Transition Buttons

  if ( event == vtkKWPushButton::InvokedEvent)
    {
    int phase;
    for (phase = 0; phase < this->WorkPhaseButtonSet->GetNumberOfWidgets(); phase++)
      {
      if (this->WorkPhaseButtonSet->GetWidget(phase) == vtkKWPushButton::SafeDownCast(caller))
        {
        break;
        }
      }
    if (phase < vtkTRProstateBiopsyLogic::NumPhases) // if pressed one of them
      {
      this->ChangeWorkPhase(phase, 1);
      }
    }


  //----------------------------------------------------------------
  // Wizard Frame

  if (this->WizardWidget->GetWizardWorkflow() ==
      vtkKWWizardWorkflow::SafeDownCast(caller) &&
      event == vtkKWWizardWorkflow::CurrentStateChangedEvent)
    {
    int phase = vtkTRProstateBiopsyLogic::Targeting;
    vtkKWWizardStep* step = 
            this->WizardWidget->GetWizardWorkflow()->GetCurrentStep();

    for (int i = 0; i < vtkTRProstateBiopsyLogic::NumPhases; i ++)
      {
      if (step == vtkKWWizardStep::SafeDownCast(this->WizardSteps[i]))
        {
        phase = i;
        }
      }
    
    this->ChangeWorkPhase(phase);
    }

  // Process Wizard GUI (Active step only)
  else
    {
    int phase = this->Logic->GetCurrentPhase();
    this->WizardSteps[phase]->ProcessGUIEvents(caller, event, callData);
    }

} 


void vtkTRProstateBiopsyGUI::Init()
{
    this->DataManager->SetMRMLScene(this->GetMRMLScene());
}



void vtkTRProstateBiopsyGUI::DataCallback(vtkObject *caller, 
        unsigned long eid, void *clientData, void *callData)
{
    vtkTRProstateBiopsyGUI *self =
            reinterpret_cast<vtkTRProstateBiopsyGUI *>(clientData);
    vtkDebugWithObjectMacro(self, "In vtkTRProstateBiopsyGUI DataCallback");

    self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::ProcessLogicEvents ( vtkObject *caller,
    unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkTRProstateBiopsyLogic::SafeDownCast(caller))
    {
    // Fill in
    }
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::ProcessMRMLEvents ( vtkObject *caller,
    unsigned long event, void *callData )
{
    // Fill in
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::UpdateAll()
{
        // Fill in
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::Enter()
{
  // Fill in
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  this->Logic0 = appGUI->GetMainSliceGUI0()->GetLogic();
  this->Logic1 = appGUI->GetMainSliceGUI1()->GetLogic();
  this->Logic2 = appGUI->GetMainSliceGUI2()->GetLogic();
  this->SliceNode0 = appGUI->GetMainSliceGUI0()->GetLogic()->GetSliceNode();
  
  this->SliceNode1 = appGUI->GetMainSliceGUI1()->GetLogic()->GetSliceNode();
  this->SliceNode2 = appGUI->GetMainSliceGUI2()->GetLogic()->GetSliceNode();
  this->Control0 = appGUI->GetMainSliceGUI0()->GetSliceController();
  this->Control1 = appGUI->GetMainSliceGUI1()->GetSliceController();
  this->Control2 = appGUI->GetMainSliceGUI2()->GetSliceController();
  
  this->ChangeWorkPhase(vtkTRProstateBiopsyLogic::Calibration, 1);
  
  //----------------------------------------------------------------
  // Target Fiducials
  
  if (!this->FiducialListNodeID)
    {
    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

    // Get a pointer to the Fiducials module
    vtkSlicerFiducialsGUI *fidGUI
      = (vtkSlicerFiducialsGUI*)app->GetModuleGUIByName("Fiducials");
    fidGUI->Enter();

    // Create New Fiducial list for Prostate Module
    vtkSlicerFiducialsLogic *fidLogic =
            (vtkSlicerFiducialsLogic*)(fidGUI->GetLogic());
    vtkMRMLFiducialListNode *newList =
            fidLogic->AddFiducialList();

    if (newList != NULL)
      {
      // Change the name of the list
      newList->SetName(this->GetMRMLScene()->GetUniqueNameByString("PM"));

      fidGUI->SetFiducialListNodeID(newList->GetID());
      newList->Delete();
      }
    else
      {
        vtkErrorMacro("Unable to add a new fid list via the logic\n");
      }
    // now get the newly active node 
    this->FiducialListNodeID = fidGUI->GetFiducialListNodeID();
    this->FiducialListNode = (vtkMRMLFiducialListNode *)this->GetMRMLScene()
                 ->GetNodeByID(this->FiducialListNodeID);

    if (this->FiducialListNode == NULL)
      {
      vtkErrorMacro ("ERROR adding a new fiducial list for the point...\n");
      return;
      }
    }
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::Exit ( )
{
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::BuildGUI ( )
{
  // ---
  // MODULE GUI FRAME 
  // create a page

  const char *help = "**Trans-Rectal Prostate Biopsy Module:** "
                     "**Under Construction** "
                     "This module provides an interface for using MRI "
                     "to localize prostate biopsy targets and to perform "
                     "the biopsies with an intra-rectal robot.";
  const char *about = "This module was developed at Queen's University, Canada "
                      "and is supported by NA-MIC and the Slicer Community. "
                      "                 ";

  this->UIPanel->AddPage ( "TRProstateBiopsy", "TRProstateBiopsy", NULL );

  vtkKWWidget *page = this->UIPanel->GetPageWidget( "TRProstateBiopsy" );

  this->BuildHelpAndAboutFrame( page, help, about );
  this->BuildGUIForWorkPhaseFrame();
  this->BuildGUIForWizardFrame();
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::BuildGUIForWizardFrame()
{
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "TRProstateBiopsy" );
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
  this->WizardWidget->SetClientAreaMinimumHeight(80);
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
  // Calibration step

  if (!this->WizardSteps[vtkTRProstateBiopsyLogic::Calibration])
    {
    this->WizardSteps[vtkTRProstateBiopsyLogic::Calibration] =
            vtkTRProstateBiopsyCalibrationStep::New();
    }

  // -----------------------------------------------------------------
  // Targeting step

  if (!this->WizardSteps[vtkTRProstateBiopsyLogic::Targeting])
    {
    this->WizardSteps[vtkTRProstateBiopsyLogic::Targeting] = vtkTRProstateBiopsyTargetingStep::New();
    }

  // -----------------------------------------------------------------
  // Config File step

  if (!this->WizardSteps[vtkTRProstateBiopsyLogic::Verification])
    {
    this->WizardSteps[vtkTRProstateBiopsyLogic::Verification] = vtkTRProstateBiopsyVerificationStep::New();
    }

  // -----------------------------------------------------------------
  // Set GUI/Logic to each step and add to workflow

  for (int i = 0; i < vtkTRProstateBiopsyLogic::NumPhases; i++)
    {
    this->WizardSteps[i]->SetGUI(this);
    //this->WizardSteps[i]->SetLogic(this->Logic);

    // Set color for the wizard title:

    this->WizardSteps[i]->SetTitleBackgroundColor(0.8, 0.8, 1.0);
    //this->WizardSteps[i]->SetTitleBackgroundColor(WorkPhaseColor[i][0],
    //                                              WorkPhaseColor[i][1],
    //                                              WorkPhaseColor[i][2]);
    wizard_workflow->AddNextStep(this->WizardSteps[i]);
    }


  // -----------------------------------------------------------------
  // Initial and finish step

  wizard_workflow->SetFinishStep(this->WizardSteps[vtkTRProstateBiopsyLogic::Verification]);
  wizard_workflow->CreateGoToTransitionsToFinishStep();
  wizard_workflow->SetInitialStep(this->WizardSteps[vtkTRProstateBiopsyLogic::Calibration]);

  // -----------------------------------------------------------------
  // Show the user interface
  this->WizardWidget->GetWizardWorkflow()->
      GetCurrentStep()->ShowUserInterface();
}


void vtkTRProstateBiopsyGUI::BuildGUIForHelpFrame ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "TRProstateBiopsy" );

  // Define your help text here.
  const char *help = 
      "The **TRProstateBiopsy Module** is for MR-Guided Trans-Rectal Prostate Biopsies:"
      " It interfaces with the MRI compatible robot developed by Axel Krieger at JHU."
      " Module and Logic mainly coded by David Gobbi and Csaba Csoma"; 

  // ----------------------------------------------------------------
  // HELP FRAME         
  // ----------------------------------------------------------------
  vtkSlicerModuleCollapsibleFrame *TRProstateBiopsyHelpFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  TRProstateBiopsyHelpFrame->SetParent ( page );
  TRProstateBiopsyHelpFrame->Create ( );
  TRProstateBiopsyHelpFrame->CollapseFrame ( );
  TRProstateBiopsyHelpFrame->SetLabelText ("Help");
  app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
      TRProstateBiopsyHelpFrame->GetWidgetName(), page->GetWidgetName());

  // configure the parent classes help text widget
  this->HelpText->SetParent ( TRProstateBiopsyHelpFrame->GetFrame() );
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

  TRProstateBiopsyHelpFrame->Delete();
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::BuildGUIForWorkPhaseFrame ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "TRProstateBiopsy" );
  
  vtkSlicerModuleCollapsibleFrame *workphaseFrame =
          vtkSlicerModuleCollapsibleFrame::New ( );
  workphaseFrame->SetParent(page);
  workphaseFrame->Create();
  workphaseFrame->SetLabelText("Workphase Frame");
  workphaseFrame->ExpandFrame();
  app->Script("pack %s -side top -anchor center -fill x -padx 2 -pady 2 -in %s",
              workphaseFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Frames

  vtkKWFrame *buttonFrame = vtkKWFrame::New();
  buttonFrame->SetParent( workphaseFrame->GetFrame());
  buttonFrame->Create();

  app->Script ( "pack %s -side top -anchor center -fill x -padx 2 -pady 1",
                buttonFrame->GetWidgetName());

  // -----------------------------------------
  // Work Phase Transition Buttons Frame

  this->WorkPhaseButtonSet = vtkKWPushButtonSet::New();
  this->WorkPhaseButtonSet->SetParent(buttonFrame);
  this->WorkPhaseButtonSet->Create();
  this->WorkPhaseButtonSet->PackHorizontallyOn();
  this->WorkPhaseButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(3);
  this->WorkPhaseButtonSet->SetWidgetsPadX(2);
  this->WorkPhaseButtonSet->SetWidgetsPadY(2);
  this->WorkPhaseButtonSet->UniformColumnsOn();
  this->WorkPhaseButtonSet->UniformRowsOn();
  
  for (int i = 0; i < vtkTRProstateBiopsyLogic::NumPhases; i ++)
    {
    this->WorkPhaseButtonSet->AddWidget(i);
    this->WorkPhaseButtonSet->GetWidget(i)->SetWidth(16);
    this->WorkPhaseButtonSet->GetWidget(i)->SetText(WorkPhaseStr[i]);
    this->WorkPhaseButtonSet->GetWidget(i)
      ->SetBackgroundColor(WorkPhaseColor[i][0],
                           WorkPhaseColor[i][1],
                           WorkPhaseColor[i][2]);
    this->WorkPhaseButtonSet->GetWidget(i)
      ->SetActiveBackgroundColor(WorkPhaseColor[i][0],
                                 WorkPhaseColor[i][1],
                                 WorkPhaseColor[i][2]);
    /*
    this->WorkPhaseButtonSet->GetWidget(i)
      ->SetDisabledBackgroundColor(WorkPhaseColor[i][0], WorkPhaseColor[i][1], WorkPhaseColor[i][2]);
    */
    }
  
  this->Script("pack %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->WorkPhaseButtonSet->GetWidgetName());
  
  workphaseFrame->Delete ();
  buttonFrame->Delete ();
}

//----------------------------------------------------------------------------
int vtkTRProstateBiopsyGUI::ChangeWorkPhase(int phase, int fChangeWizard)
{
  if (!this->Logic->SwitchWorkPhase(phase)) // Set next phase
    {
    cerr << "ChangeWorkPhase: Cannot make transition!" << endl;
    return 0;
    }
  
  for (int i = 0; i < vtkTRProstateBiopsyLogic::NumPhases; i ++)
    {
    vtkKWPushButton *pb = this->WorkPhaseButtonSet->GetWidget(i);
    if (i == this->Logic->GetCurrentPhase())
      {
      pb->SetReliefToSunken();
      }
    else if (this->Logic->IsPhaseTransitable(i))
      {
      pb->SetReliefToGroove();
      pb->SetStateToNormal();
      pb->SetBackgroundColor(WorkPhaseColor[i][0],
                             WorkPhaseColor[i][1],
                             WorkPhaseColor[i][2]);
      }
    else
      {
      pb->SetReliefToGroove();
      pb->SetStateToDisabled();
      pb->SetBackgroundColor(WorkPhaseColorDisabled[i][0],
                             WorkPhaseColorDisabled[i][1],
                             WorkPhaseColorDisabled[i][2]);
      }
    }

  // Switch Wizard Frame
  if (fChangeWizard)
    {
    vtkKWWizardWorkflow *wizard = 
          this->WizardWidget->GetWizardWorkflow();
        
    int step_from;
    int step_to;
        
    step_to = this->Logic->GetCurrentPhase();
    step_from = this->Logic->GetPrevPhase();
        
    int steps =  step_to - step_from;
    if (steps > 0)
      {
      for (int i = 0; i < steps; i++) 
        {
        wizard->AttemptToGoToNextStep();
        }
      }
    else
      {
      steps = -steps;
      for (int i = 0; i < steps; i++)
        {
        wizard->AttemptToGoToPreviousStep();
        }
      }
      wizard->GetCurrentStep()->ShowUserInterface();
    }

  return 1;
}


