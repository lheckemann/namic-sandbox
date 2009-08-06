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

#include "vtk4DUsEndoNavGUI.h"
#include "BRPTPRInterface.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtk4DUsEndoNavStepBase.h"
#include "vtk4DUsEndoNavFirstStep.h"
#include "vtk4DUsEndoNavSecondStep.h"
#include "vtk4DUsEndoNavThirdStep.h"

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

#include "vtkIGTDataStream.h"
#include "vtkCylinderSource.h"
#include "vtkMRMLLinearTransformNode.h"

#include "vtkOpenIGTLinkIFGUI.h"
#include "vtkOpenIGTLinkIFLogic.h"

#include <vector>

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtk4DUsEndoNavGUI );
vtkCxxRevisionMacro ( vtk4DUsEndoNavGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Button Colors and Labels for Work Phase Control
const double vtk4DUsEndoNavGUI::WorkPhaseColor[vtk4DUsEndoNavLogic::NumPhases][NUM_PHASES] =
  {
  /* St */ { 1.0, 0.6, 1.0 },
  /* Pl */ { 0.6, 1.0, 0.6 },
  /* Cl */ { 1.0, 1.0, 0.6 },
//  /* Tg */ { 0.6, 0.6, 1.0 },
//  /* Mn */ { 0.6, 1.0, 1.0 },
//  /* Em */ { 1.0, 0.0, 0.0 },
  };

const double vtk4DUsEndoNavGUI::WorkPhaseColorActive[vtk4DUsEndoNavLogic::NumPhases][NUM_PHASES] =
  {
  /* St */ { 1.0, 0.4, 1.0 },
  /* Pl */ { 0.4, 1.0, 0.4 },
  /* Cl */ { 1.0, 1.0, 0.4 },
//  /* Tg */ { 0.4, 0.4, 1.0 },
//  /* Mn */ { 0.4, 1.0, 1.0 },
//  /* Em */ { 1.0, 0.0, 0.0 },
  };

const double vtk4DUsEndoNavGUI::WorkPhaseColorDisabled[vtk4DUsEndoNavLogic::NumPhases][NUM_PHASES] =
  {
  /* St */ { 1.0, 0.95, 1.0 },
  /* Pl */ { 0.95, 1.0, 0.95 },
  /* Cl */ { 1.0, 1.0, 0.95 },
//  /* Tg */ { 0.95, 0.95, 1.0 },
//  /* Mn */ { 0.95, 1.0, 1.0 },
//  /* Em */ { 1.0, 0.95, 0.95 },
  };

const char *vtk4DUsEndoNavGUI::WorkPhaseStr[vtk4DUsEndoNavLogic::NumPhases] =
  {
  /* Su */ "Second Window",
  /* Pl */ "Second",
  /* Cl */ "Third",
  };

//---------------------------------------------------------------------------
vtk4DUsEndoNavGUI::vtk4DUsEndoNavGUI ( )
{

  std::cerr << "vtk4DUsEndoNavGUI::vtk4DUsEndoNavGUI ( )" << std::endl;

  //----------------------------------------------------------------
  // Logic values
  
  this->Logic = NULL;
  this->DataManager = vtkIGTDataManager::New();
  this->Pat2ImgReg = vtkIGTPat2ImgRegistration::New();
  
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtk4DUsEndoNavGUI::DataCallback);
  
  //----------------------------------------------------------------
  // Workphase Frame
  
  this->WorkPhaseButtonSet = NULL;

  // these need to be set to null no matter what

  //----------------------------------------------------------------  
  // Wizard Frame
  
  this->WizardWidget = vtkKWWizardWidget::New();
  this->WizardSteps = new vtk4DUsEndoNavStepBase*[vtk4DUsEndoNavLogic::NumPhases];
  for (int i = 0; i < vtk4DUsEndoNavLogic::NumPhases; i ++)
    {
    this->WizardSteps[i] = NULL;
    }
  

  this->Entered = 0;

  std::cerr << "vtk4DUsEndoNavGUI::vtk4DUsEndoNavGUI ( ) end " << std::endl;

}

//---------------------------------------------------------------------------
vtk4DUsEndoNavGUI::~vtk4DUsEndoNavGUI ( )
{
  this->RemoveGUIObservers();

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

  if ( this->WizardSteps )
    {
    for (int i = 0; i < vtk4DUsEndoNavLogic::NumPhases; i ++)
      {
      if ( this->WizardSteps[i] != NULL )
        {
        this->WizardSteps[i]->Delete();
        this->WizardSteps[i] = NULL;
        }
      }
    delete [] this->WizardSteps;
    this->WizardSteps = NULL;
    }
  
}



//---------------------------------------------------------------------------
void vtk4DUsEndoNavGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->vtkObject::PrintSelf ( os, indent );
    
    os << indent << "4DUsEndoNavGUI: " << this->GetClassName ( ) << "\n";
    os << indent << "Logic: " << this->GetLogic ( ) << "\n";
   
    // print widgets?
}


//---------------------------------------------------------------------------
void vtk4DUsEndoNavGUI::RemoveGUIObservers ( )
{
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

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
void vtk4DUsEndoNavGUI::RemoveLogicObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtk4DUsEndoNavGUI::AddGUIObservers ( )
{
  std::cerr << "vtk4DUsEndoNavGUI::AddGUIObservers ( )" << std::endl;

  this->RemoveGUIObservers();

  // make a user interactor style to process our events
  // look at the InteractorStyle to get our events
  
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  

  std::cerr << "vtk4DUsEndoNavGUI::AddGUIObservers ( ) --2" << std::endl;

  //----------------------------------------------------------------
  // Workphase Frame

  for (int i = 0; i < this->WorkPhaseButtonSet->GetNumberOfWidgets(); i ++)
    {
    this->WorkPhaseButtonSet->GetWidget(i)
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  std::cerr << "vtk4DUsEndoNavGUI::AddGUIObservers ( ) --3" << std::endl;
  
  //----------------------------------------------------------------
  // Wizard Frame

  this->WizardWidget->GetWizardWorkflow()->AddObserver(vtkKWWizardWorkflow::CurrentStateChangedEvent,
                                                       (vtkCommand *)this->GUICallbackCommand);


  std::cerr << "vtk4DUsEndoNavGUI::AddGUIObservers ( ) --4" << std::endl;

  //----------------------------------------------------------------
  // Etc Frame

  // observer load volume button

  this->AddLogicObservers();

  std::cerr << "vtk4DUsEndoNavGUI::AddGUIObservers ( ) end" << std::endl;
  
}


//---------------------------------------------------------------------------
void vtk4DUsEndoNavGUI::AddLogicObservers ( )
{

  std::cerr << "vtk4DUsEndoNavGUI::AddLogicObservers ( )" << std::endl;

  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    /*
    this->GetLogic()->AddObserver(vtk4DUsEndoNavLogic::LocatorUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    */
    /*
    this->GetLogic()->AddObserver(vtk4DUsEndoNavLogic::SliceUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    */
      /*
    this->GetLogic()->AddObserver(vtk4DUsEndoNavLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
      */

    }
  std::cerr << "vtk4DUsEndoNavGUI::AddLogicObservers ( ) end " << std::endl;
}


//---------------------------------------------------------------------------
void vtk4DUsEndoNavGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
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
void vtk4DUsEndoNavGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  std::cerr << "vtk4DUsEndoNavGUI::ProcessGUIEvents() " << std::endl;

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

//    if(phase == vtk4DUsEndoNavLogic::StartUp)
//      {// Start Second window
//
//      std::cerr << "showCaptureData button is pressed." << std::endl;
//
//      // initialize mutex
//
//      int resGlobal;
//      resGlobal = pthread_mutex_init(&work_mutex, NULL);
//
//      // for display capture data
//      //this->GetLogic()->SetVisibilityOfCaptureData("capture", 1);
//
//      }

    if (phase < vtk4DUsEndoNavLogic::NumPhases) // if pressed one of them
      {
      ChangeWorkPhase(phase, 1);
      }
    }


  //----------------------------------------------------------------
  // Wizard Frame

  if (this->WizardWidget->GetWizardWorkflow() == vtkKWWizardWorkflow::SafeDownCast(caller) &&
      event == vtkKWWizardWorkflow::CurrentStateChangedEvent)
    {
          
    ////////int phase = vtk4DUsEndoNavLogic::Emergency;
    int phase = 0;
    vtkKWWizardStep* step =  this->WizardWidget->GetWizardWorkflow()->GetCurrentStep();

    for (int i = 0; i < vtk4DUsEndoNavLogic::NumPhases-1; i ++)
      {
      if (step == vtkKWWizardStep::SafeDownCast(this->WizardSteps[i]))
        {
        phase = i;
        }
      }
    
    ChangeWorkPhase(phase);
    }


  // Process Wizard GUI (Active step only)
  else
    {
    int phase = this->Logic->GetCurrentPhase();
    this->WizardSteps[phase]->ProcessGUIEvents(caller, event, callData);
    }

  std::cerr << "vtk4DUsEndoNavGUI::ProcessGUIEvents() end " << std::endl;

} 


void vtk4DUsEndoNavGUI::Init()
{
    this->DataManager->SetMRMLScene(this->GetMRMLScene());
    //   this->LocatorModelID = std::string(this->DataManager->RegisterStream(0));
    //this->LocatorModelID_new = std::string(this->DataManager->RegisterStream_new(0));
    
}



void vtk4DUsEndoNavGUI::DataCallback(vtkObject *caller,
        unsigned long eid, void *clientData, void *callData)
{
    vtk4DUsEndoNavGUI *self = reinterpret_cast<vtk4DUsEndoNavGUI *>(clientData);
    vtkDebugWithObjectMacro(self, "In vtk4DUsEndoNavGUI DataCallback");

    self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtk4DUsEndoNavGUI::ProcessLogicEvents ( vtkObject *caller,
    unsigned long event, void *callData )
{

  if (this->GetLogic() == vtk4DUsEndoNavLogic::SafeDownCast(caller))
    {
    if (event == vtk4DUsEndoNavLogic::StatusUpdateEvent)
      {
      }
    }
}


//---------------------------------------------------------------------------
void vtk4DUsEndoNavGUI::ProcessMRMLEvents ( vtkObject *caller,
    unsigned long event, void *callData )
{
    // Fill in
}


//---------------------------------------------------------------------------
void vtk4DUsEndoNavGUI::Enter()
{
  // Fill in
  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  std::cerr << "vtk4DUsEndoNavGUI::Enter() " << std::endl;

  if (this->Entered == 0)
    {
    this->GetLogic()->SetGUI(this);

    ChangeWorkPhase(vtk4DUsEndoNavLogic::StartUp, 1);
  
    //----------------------------------------------------------------
    // Following code should be in the logic class, but GetApplication()
    // is not available there.
    
    vtkOpenIGTLinkIFGUI* igtlGUI = 
      vtkOpenIGTLinkIFGUI::SafeDownCast(vtkSlicerApplication::SafeDownCast(this->GetApplication())
                                        ->GetModuleGUIByName("OpenIGTLink IF"));

    this->GetLogic()->Enter();

    this->Entered = 1;
    }

  std::cerr << "vtk4DUsEndoNavGUI::Enter() end " << std::endl;
  
}


//---------------------------------------------------------------------------
void vtk4DUsEndoNavGUI::Exit ( )
{
    // Fill in
}


//---------------------------------------------------------------------------
void vtk4DUsEndoNavGUI::BuildGUI ( )
{
  std::cerr << "vtk4DUsEndoNavGUI::BuildGUI ( )" << std::endl;
  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "4DUsEndoNav", "4DUsEndoNav", NULL );
  
  BuildGUIForHelpFrame();
  BuildGUIForWorkPhaseFrame ();
  BuildGUIForWizardFrame();
  std::cerr << "vtk4DUsEndoNavGUI::BuildGUI ( ) end" << std::endl;
}

//---------------------------------------------------------------------------
void vtk4DUsEndoNavGUI::TearDownGUI ( )
{
  // disconnect circular references so destructor can be called
  
  this->GetLogic()->SetGUI(NULL);
  for (int i = 0; i < vtk4DUsEndoNavLogic::NumPhases; i ++)
    {
    if (this->WizardSteps[i])
      {
      this->WizardSteps[i]->SetGUI(NULL);
      this->WizardSteps[i]->SetLogic(NULL);
      }
    }
}

//---------------------------------------------------------------------------
void vtk4DUsEndoNavGUI::BuildGUIForWizardFrame()
{
    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "4DUsEndoNav" );
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
    // First step

    if (!this->WizardSteps[vtk4DUsEndoNavLogic::StartUp])
      {
      this->WizardSteps[vtk4DUsEndoNavLogic::StartUp] = vtk4DUsEndoNavFirstStep::New();
      }

    // -----------------------------------------------------------------
    // Second step

    if (!this->WizardSteps[vtk4DUsEndoNavLogic::Planning])
      {
      this->WizardSteps[vtk4DUsEndoNavLogic::Planning] = vtk4DUsEndoNavSecondStep::New();
      }

    // -----------------------------------------------------------------
    // Third step

    if (!this->WizardSteps[vtk4DUsEndoNavLogic::Calibration])
      {
      this->WizardSteps[vtk4DUsEndoNavLogic::Calibration] = vtk4DUsEndoNavThirdStep::New();
      }

    // -----------------------------------------------------------------
    // Set GUI/Logic to each step and add to workflow

    for (int i = 0; i < vtk4DUsEndoNavLogic::NumPhases-1; i ++)
      {
      this->WizardSteps[i]->SetGUI(this);
      this->WizardSteps[i]->SetLogic(this->Logic);

      // Set color for the wizard title:

      this->WizardSteps[i]->SetTitleBackgroundColor(0.8, 0.8, 1.0);
      //this->WizardSteps[i]->SetTitleBackgroundColor(WorkPhaseColor[i][0],
      //                                              WorkPhaseColor[i][1],
      //                                              WorkPhaseColor[i][2]);
      wizard_workflow->AddNextStep(this->WizardSteps[i]);
      }


    // -----------------------------------------------------------------
    // Initial and finish step

    wizard_workflow->SetFinishStep(this->WizardSteps[vtk4DUsEndoNavLogic::Calibration]);
    wizard_workflow->CreateGoToTransitionsToFinishStep();
    wizard_workflow->SetInitialStep(this->WizardSteps[vtk4DUsEndoNavLogic::StartUp]);

    // -----------------------------------------------------------------
    // Show the user interface

//    this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->ShowUserInterface();
    if (wizard_workflow->GetCurrentStep())
      {
      wizard_workflow->GetCurrentStep()->ShowUserInterface();
      }
}


void vtk4DUsEndoNavGUI::BuildGUIForHelpFrame ()
{

    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "4DUsEndoNav" );

    // Define your help text here.
    const char *help = 
      "The **4DUsEndoNav Module** helps you to do prostate Biopsy and Treatment by:"
      " getting Realtime Images from MR-Scanner into Slicer3, control Scanner with Slicer 3,"
      " determin fiducial detection and control the Robot."
      " Module and Logic mainly coded by Junichi Tokuda, David Gobbi and Philip Mewes"; 

    // ----------------------------------------------------------------
    // HELP FRAME         
    // ----------------------------------------------------------------
    vtkSlicerModuleCollapsibleFrame *4DUsEndoNavHelpFrame = vtkSlicerModuleCollapsibleFrame::New ( );
    4DUsEndoNavHelpFrame->SetParent ( page );
    4DUsEndoNavHelpFrame->Create ( );
    4DUsEndoNavHelpFrame->CollapseFrame ( );
    4DUsEndoNavHelpFrame->SetLabelText ("Help");
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
        4DUsEndoNavHelpFrame->GetWidgetName(), page->GetWidgetName());

    // configure the parent classes help text widget
    this->HelpText->SetParent ( 4DUsEndoNavHelpFrame->GetFrame() );
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

    4DUsEndoNavHelpFrame->Delete();

}

//---------------------------------------------------------------------------
void vtk4DUsEndoNavGUI::BuildGUIForWorkPhaseFrame ()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "4DUsEndoNav" );
  
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
  workphaseStatusFrame->SetReliefToRaised();
  workphaseStatusFrame->SetBackgroundColor(0.9, 0.9, 0.9);
  
  vtkKWFrame *buttonFrame = vtkKWFrame::New();
  buttonFrame->SetParent( workphaseFrame->GetFrame());
  buttonFrame->Create();

  app->Script ( "pack %s %s -side top -anchor center -fill x -padx 2 -pady 1",
                workphaseStatusFrame->GetWidgetName(),
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
  
  for (int i = 0; i < vtk4DUsEndoNavLogic::NumPhases; i ++)
    {
    this->WorkPhaseButtonSet->AddWidget(i);
    this->WorkPhaseButtonSet->GetWidget(i)->SetWidth(16);
    this->WorkPhaseButtonSet->GetWidget(i)->SetText(WorkPhaseStr[i]);
    this->WorkPhaseButtonSet->GetWidget(i)
      ->SetBackgroundColor(WorkPhaseColor[i][0], WorkPhaseColor[i][1], WorkPhaseColor[i][2]);
    this->WorkPhaseButtonSet->GetWidget(i)
      ->SetActiveBackgroundColor(WorkPhaseColor[i][0], WorkPhaseColor[i][1], WorkPhaseColor[i][2]);
    /*
    this->WorkPhaseButtonSet->GetWidget(i)
      ->SetDisabledBackgroundColor(WorkPhaseColor[i][0], WorkPhaseColor[i][1], WorkPhaseColor[i][2]);
    */
    }
  
  this->Script("pack %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->WorkPhaseButtonSet->GetWidgetName());
  
  workphaseFrame->Delete ();
  buttonFrame->Delete ();
  workphaseStatusFrame->Delete ();
  
}


//----------------------------------------------------------------------------
int vtk4DUsEndoNavGUI::ChangeWorkPhase(int phase, int fChangeWizard)
{

   //  cerr << "ChangeWorkPhase: started" << endl;
  if (!this->Logic->SwitchWorkPhase(phase)) // Set next phase
    {
    cerr << "ChangeWorkPhase: Cannot transition!" << endl;
    return 0;
    }
  
  for (int i = 0; i < vtk4DUsEndoNavLogic::NumPhases; i ++)
    {
    vtkKWPushButton *pb = this->WorkPhaseButtonSet->GetWidget(i);
    if (i == this->Logic->GetCurrentPhase())
      {
      pb->SetReliefToSunken();
      }
    else if (this->Logic->IsPhaseTransitionable(i))
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
  // 11/09/2007 Junichi Tokuda -- This part looks ugly. Will be fixed later.
  if (fChangeWizard)
    {
    vtkKWWizardWorkflow *wizard = 
      this->WizardWidget->GetWizardWorkflow();
    
    int step_from;
    int step_to;
    
    step_to = this->Logic->GetCurrentPhase();
    step_from = this->Logic->GetPrevPhase();

    //if (step_to == vtk4DUsEndoNavLogic::Emergency)
    //  {
    //  step_to = vtk4DUsEndoNavLogic::Manual;
    //  }
    //if (step_from == vtk4DUsEndoNavLogic::Emergency)
    //  {
    //  step_from = vtk4DUsEndoNavLogic::Manual;
    //  }
    
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
void vtk4DUsEndoNavGUI::UpdateAll()
{

}


