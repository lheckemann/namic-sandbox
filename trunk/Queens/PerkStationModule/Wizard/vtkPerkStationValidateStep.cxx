#include "vtkPerkStationValidateStep.h"

#include "PerkStationCommon.h"
#include "vtkPerkStationModuleGUI.h"
#include "vtkMRMLPerkStationModuleNode.h"
#include "vtkPerkStationSecondaryMonitor.h"

#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWEntrySet.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

#include "vtkLineSource.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationValidateStep);
vtkCxxRevisionMacro(vtkPerkStationValidateStep, "$Revision: 1.0 $");


//----------------------------------------------------------------------------
void
vtkPerkStationValidateStep
::AddGUIObservers()
{
  this->RemoveGUIObservers();
  
  
  if ( this->PlanList )
    {
    this->PlanList->GetWidget()->SetSelectionChangedCommand(
      this, "OnMultiColumnListSelectionChanged" );
    }
  
  
  if (this->ResetValidationButton)
    {
    this->ResetValidationButton->AddObserver( vtkKWPushButton::InvokedEvent,
                                              (vtkCommand *)this->WizardGUICallbackCommand );
    }
}


//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::InstallCallbacks()
{
  // Configure the OK button to start
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  if (wizard_widget->GetOKButton())
    {
    wizard_widget->GetOKButton()->SetText("Start over");
    wizard_widget->GetOKButton()->SetCommand( this, "StartOverNewExperiment");
    wizard_widget->GetOKButton()->SetBalloonHelpString( "Do another experiment");
    }
  
  this->AddGUIObservers();
}


//----------------------------------------------------------------------------
vtkPerkStationValidateStep::vtkPerkStationValidateStep()
{
  this->SetName("4/5. Validate");
  this->SetDescription("Mark actual entry point and target hit");  
  this->WizardGUICallbackCommand->SetCallback(vtkPerkStationValidateStep::WizardGUICallback);

  
  this->PlanListFrame = NULL;
  this->PlanList = NULL;
  
  
  this->ResetFrame = NULL;
  this->ResetValidationButton = NULL;

  this->EntryPointFrame = NULL;
  this->EntryPointLabel = NULL;
  this->EntryPoint = NULL;
  this->TargetPointFrame = NULL;
  this->TargetPointLabel = NULL;
  this->TargetPoint = NULL;
  this->InsertionDepth = NULL;

  this->ValidationErrorsFrame = NULL;
  this->EntryPointError = NULL;
  this->TargetPointError = NULL; 
  this->InsertionAngleError = NULL;
  this->InsertionDepthError = NULL;
  
  this->ValidationNeedleActor = vtkActor::New();

  this->TimePerformanceFrame = NULL;
  this->CalibrationTime = NULL;
  this->PlanTime = NULL;
  this->InsertionTime = NULL;
  
  this->EntryTargetAcquired = false;
  this->ClickNumber = 0;
  this->ProcessingCallback = false;

}

//----------------------------------------------------------------------------
vtkPerkStationValidateStep::~vtkPerkStationValidateStep()
{
  if (this->ResetFrame)
    {
    this->ResetFrame->Delete();
    this->ResetFrame = NULL;
    }
  if (this->ResetValidationButton)
    {
    this->ResetValidationButton->Delete();
    this->ResetValidationButton = NULL;
    }


  if (this->EntryPointFrame)
    {
    this->EntryPointFrame->Delete();
    this->EntryPointFrame = NULL;
    }
  if (this->EntryPointLabel)
    {
    this->EntryPointLabel->Delete();
    this->EntryPointLabel = NULL;
    }

  if (this->EntryPoint)
    {
    this->EntryPoint->DeleteAllWidgets();
    this->EntryPoint = NULL;
    }

  if (this->TargetPointFrame)
    {
    this->TargetPointFrame->Delete();
    this->TargetPointFrame = NULL;
    }
  if (this->TargetPointLabel)
    {
    this->TargetPointLabel->Delete();
    this->TargetPointLabel = NULL;
    }
  if (this->TargetPoint)
    {
    this->TargetPoint->DeleteAllWidgets();
    this->TargetPoint = NULL;
    }

  if (this->InsertionDepth)
    {
    this->InsertionDepth->Delete();
    this->InsertionDepth = NULL;
    }
  if (this->ValidationErrorsFrame)
    {
    this->ValidationErrorsFrame->SetParent(NULL);
    this->ValidationErrorsFrame->Delete();
    this->ValidationErrorsFrame = NULL;
    }
  if (this->EntryPointError)
    {
    this->EntryPointError->SetParent(NULL);
    this->EntryPointError->Delete();
    this->EntryPointError = NULL;
    }
  if (this->TargetPointError)
    {
    this->TargetPointError->SetParent(NULL);
    this->TargetPointError->Delete();
    this->TargetPointError = NULL;
    }
  if (this->InsertionAngleError)
    {
    this->InsertionAngleError->SetParent(NULL);
    this->InsertionAngleError->Delete();
    this->InsertionAngleError = NULL;
    }
  if (this->InsertionDepthError)
    {
    this->InsertionDepthError->SetParent(NULL);
    this->InsertionDepthError->Delete();
    this->InsertionDepthError = NULL;
    }
  if (this->TimePerformanceFrame)
    {
    this->TimePerformanceFrame->SetParent(NULL);
    this->TimePerformanceFrame->Delete();
    this->TimePerformanceFrame = NULL;
    }
  if (this->CalibrationTime)
    {
    this->CalibrationTime->SetParent(NULL);
    this->CalibrationTime->Delete();
    this->CalibrationTime = NULL;
    }
  if (this->PlanTime)
    {
    this->PlanTime->SetParent(NULL);
    this->PlanTime->Delete();
    this->PlanTime = NULL;
    }
  if (this->InsertionTime)
    {
    this->InsertionTime->SetParent(NULL);
    this->InsertionTime->Delete();
    this->InsertionTime = NULL;
    }  
  if (this->ValidationNeedleActor)
    {
    this->ValidationNeedleActor->Delete();
    this->ValidationNeedleActor = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);
  vtkKWWidget *parent = wizard_widget->GetClientArea();
  int enabled = parent->GetEnabled();
  
  
  this->ShowPlanListFrame();
  
  
    // clear controls
  
  if (this->ResetFrame)
    {
    this->Script("pack forget %s", 
                    this->ResetFrame->GetWidgetName());
    }

  if (this->ResetValidationButton)
    {
    this->Script("pack forget %s", 
                    this->ResetValidationButton->GetWidgetName());
    }
  
  
    // in clinical mode
  
  this->SetName("4/4. Validate");
  this->GetGUI()->GetWizardWidget()->Update();

   // additional reset button

  // frame for reset button
  if (!this->ResetFrame)
    {
    this->ResetFrame = vtkKWFrame::New();
    }
  if (!this->ResetFrame->IsCreated())
    {
    this->ResetFrame->SetParent(parent);
    this->ResetFrame->Create();     
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->ResetFrame->GetWidgetName());
  
  if (!this->ResetValidationButton)
    {
    this->ResetValidationButton = vtkKWPushButton::New();
    }
  if(!this->ResetValidationButton->IsCreated())
    {
    this->ResetValidationButton->SetParent(this->ResetFrame);
    this->ResetValidationButton->SetText("Reset validation");
    this->ResetValidationButton->SetBorderWidth(2);
    this->ResetValidationButton->SetReliefToRaised();      
    this->ResetValidationButton->SetHighlightThickness(2);
    this->ResetValidationButton->SetBackgroundColor(0.85,0.85,0.85);
    this->ResetValidationButton->SetActiveBackgroundColor(1,1,1);      
    this->ResetValidationButton->SetImageToPredefinedIcon(vtkKWIcon::IconTrashcan);
    this->ResetValidationButton->Create();
    }
  
  this->Script("pack %s -side top -padx 2 -pady 4", 
                    this->ResetValidationButton->GetWidgetName());
     
  
  this->SetDescription("Mark actual entry point and target hit");  

  
  // Create the individual components

  //frame
  if (!this->EntryPointFrame)
    {
    this->EntryPointFrame = vtkKWFrame::New();
    }
  if (!this->EntryPointFrame->IsCreated())
    {
    this->EntryPointFrame->SetParent(parent);
    this->EntryPointFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->EntryPointFrame->GetWidgetName());

  // label.
  if (!this->EntryPointLabel)
    { 
    this->EntryPointLabel = vtkKWLabel::New();
    }
  if (!this->EntryPointLabel->IsCreated())
    {
    this->EntryPointLabel->SetParent(this->EntryPointFrame);
    this->EntryPointLabel->Create();
    this->EntryPointLabel->SetText("Entry point:   ");
    this->EntryPointLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    } 
  
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->EntryPointLabel->GetWidgetName());

  // Entry point: will get populated in a callback, which listens for mouse clicks on image
 
  if (!this->EntryPoint)
    {
    this->EntryPoint =  vtkKWEntrySet::New();   
    }
  if (!this->EntryPoint->IsCreated())
    {
    this->EntryPoint->SetParent(this->EntryPointFrame);
    this->EntryPoint->Create();
    this->EntryPoint->SetBorderWidth(2);
    this->EntryPoint->SetReliefToGroove();
    this->EntryPoint->SetPackHorizontally(1);
    this->EntryPoint->SetMaximumNumberOfWidgetsInPackingDirection(3);
    // two entries of image spacing (x, y)
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->EntryPoint->AddWidget(id);
      entry->SetWidth(7);
      //entry->ReadOnlyOn();      
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->EntryPoint->GetWidgetName());

  //frame
  if (!this->TargetPointFrame)
    {
    this->TargetPointFrame = vtkKWFrame::New();
    }
  if (!this->TargetPointFrame->IsCreated())
    {
    this->TargetPointFrame->SetParent(parent);
    this->TargetPointFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->TargetPointFrame->GetWidgetName());

  // label
  if (!this->TargetPointLabel)
    { 
    this->TargetPointLabel = vtkKWLabel::New();
    }
  if (!this->TargetPointLabel->IsCreated())
    {
    this->TargetPointLabel->SetParent(this->TargetPointFrame);
    this->TargetPointLabel->Create();
    this->TargetPointLabel->SetText("Target point: ");
    this->TargetPointLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }
  
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->TargetPointLabel->GetWidgetName());
 
  // Target point: will get populated in a callback, which listens for mouse clicks on image
 
  if (!this->TargetPoint)
    {
    this->TargetPoint =  vtkKWEntrySet::New();  
    }
  if (!this->TargetPoint->IsCreated())
    {
    this->TargetPoint->SetParent(this->TargetPointFrame);
    this->TargetPoint->Create();
    this->TargetPoint->SetBorderWidth(2);
    this->TargetPoint->SetReliefToGroove();
    this->TargetPoint->SetPackHorizontally(1);
    this->TargetPoint->SetMaximumNumberOfWidgetsInPackingDirection(3);
    // two entries of image spacing (x, y)
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->TargetPoint->AddWidget(id);
      entry->SetWidth(7);
      //entry->ReadOnlyOn();      
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->TargetPoint->GetWidgetName());


   // insertion depth  
  if (!this->InsertionDepth)
    {
    this->InsertionDepth =  vtkKWEntryWithLabel::New(); 
    }
  if (!this->InsertionDepth->IsCreated())
    {
    this->InsertionDepth->SetParent(parent);
    this->InsertionDepth->Create();
    this->InsertionDepth->GetWidget()->SetRestrictValueToDouble();
    this->InsertionDepth->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->InsertionDepth->SetLabelText("Insertion depth (in mm):");
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->InsertionDepth->GetWidgetName());

  if (!this->ValidationErrorsFrame)
    {
    this->ValidationErrorsFrame = vtkKWFrameWithLabel::New();
    }
  if (!this->ValidationErrorsFrame->IsCreated())
    {
    this->ValidationErrorsFrame->SetParent(parent);
    this->ValidationErrorsFrame->Create();
    this->ValidationErrorsFrame->SetLabelText("Evaluation");
    this->ValidationErrorsFrame->SetBalloonHelpString("");
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->ValidationErrorsFrame->GetWidgetName());



  // entry point error
  if (!this->EntryPointError)
    {
    this->EntryPointError =  vtkKWEntryWithLabel::New();  
    }
  if (!this->EntryPointError->IsCreated())
    {
    this->EntryPointError->SetParent(this->ValidationErrorsFrame->GetFrame());
    this->EntryPointError->Create();
    this->EntryPointError->GetWidget()->SetRestrictValueToDouble();
    this->EntryPointError->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->EntryPointError->SetLabelText("Error in entry point (mm):");
    this->EntryPointError->SetWidth(7);
    this->EntryPointError->GetWidget()->SetDisabledBackgroundColor(0.9,0.9,0.9);
    this->EntryPointError->GetWidget()->ReadOnlyOn();
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->EntryPointError->GetWidgetName());


  // target point error
  if (!this->TargetPointError)
    {
    this->TargetPointError =  vtkKWEntryWithLabel::New();  
    }
  if (!this->TargetPointError->IsCreated())
    {
    this->TargetPointError->SetParent(this->ValidationErrorsFrame->GetFrame());
    this->TargetPointError->Create();
    this->TargetPointError->GetWidget()->SetRestrictValueToDouble();
    this->TargetPointError->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->TargetPointError->SetLabelText("Error in target point (mm):");
    this->TargetPointError->SetWidth(7);
    this->TargetPointError->GetWidget()->SetDisabledBackgroundColor(0.9,0.9,0.9);
    this->TargetPointError->GetWidget()->ReadOnlyOn();
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->TargetPointError->GetWidgetName());

 

  // entry point error
  if (!this->InsertionDepthError)
    {
    this->InsertionDepthError =  vtkKWEntryWithLabel::New();  
    }
  if (!this->InsertionDepthError->IsCreated())
    {
    this->InsertionDepthError->SetParent(this->ValidationErrorsFrame->GetFrame());
    this->InsertionDepthError->Create();
    this->InsertionDepthError->GetWidget()->SetRestrictValueToDouble();
    this->InsertionDepthError->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->InsertionDepthError->SetLabelText("Error in insertion depth (mm):");
    this->InsertionDepthError->SetWidth(7);
    this->InsertionDepthError->GetWidget()->SetDisabledBackgroundColor(0.9,0.9,0.9);
    this->InsertionDepthError->GetWidget()->ReadOnlyOn();
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->InsertionDepthError->GetWidgetName());


  // target point error
  if (!this->InsertionAngleError)
    {
    this->InsertionAngleError =  vtkKWEntryWithLabel::New();  
    }
  if (!this->InsertionAngleError->IsCreated())
    {
    this->InsertionAngleError->SetParent(this->ValidationErrorsFrame->GetFrame());
    this->InsertionAngleError->Create();
    this->InsertionAngleError->GetWidget()->SetRestrictValueToDouble();
    this->InsertionAngleError->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->InsertionAngleError->SetLabelText("Error in-plane angle(degrees):");
    this->InsertionAngleError->SetWidth(7);
    this->InsertionAngleError->GetWidget()->SetDisabledBackgroundColor(0.9,0.9,0.9);
    this->InsertionAngleError->GetWidget()->ReadOnlyOn();
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->InsertionAngleError->GetWidgetName());


  if (!this->TimePerformanceFrame)
    {
    this->TimePerformanceFrame = vtkKWFrameWithLabel::New();
    }
  if (!this->TimePerformanceFrame->IsCreated())
    {
    this->TimePerformanceFrame->SetParent(parent);
    this->TimePerformanceFrame->Create();
    this->TimePerformanceFrame->SetLabelText("Time performance");
    this->TimePerformanceFrame->SetBalloonHelpString("");
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->TimePerformanceFrame->GetWidgetName());



  // entry point error
  if (!this->CalibrationTime)
    {
    this->CalibrationTime =  vtkKWEntryWithLabel::New();  
    }
  if (!this->CalibrationTime->IsCreated())
    {
    this->CalibrationTime->SetParent(this->TimePerformanceFrame->GetFrame());
    this->CalibrationTime->Create();
    //this->CalibrationTime->GetWidget()->SetRestrictValueToDouble();
    this->CalibrationTime->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->CalibrationTime->SetLabelText("Time spent on calibration (minutes, seconds):");
    this->CalibrationTime->SetWidth(7);
    this->CalibrationTime->GetWidget()->SetDisabledBackgroundColor(0.9,0.9,0.9);
    this->CalibrationTime->GetWidget()->ReadOnlyOn();
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->CalibrationTime->GetWidgetName());


  // target point error
  if (!this->PlanTime)
    {
    this->PlanTime =  vtkKWEntryWithLabel::New();  
    }
  if (!this->PlanTime->IsCreated())
    {
    this->PlanTime->SetParent(this->TimePerformanceFrame->GetFrame());
    this->PlanTime->Create();
    //this->PlanTime->GetWidget()->SetRestrictValueToDouble();
    this->PlanTime->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->PlanTime->SetLabelText("Time spent on planning (minutes, seconds):");
    this->PlanTime->SetWidth(7);
    this->PlanTime->GetWidget()->SetDisabledBackgroundColor(0.9,0.9,0.9);
    this->PlanTime->GetWidget()->ReadOnlyOn();
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->PlanTime->GetWidgetName());

 

  // entry point error
  if (!this->InsertionTime)
    {
    this->InsertionTime =  vtkKWEntryWithLabel::New();  
    }
  if ( ! this->InsertionTime->IsCreated() )
    {
    this->InsertionTime->SetParent( this->TimePerformanceFrame->GetFrame() );
    this->InsertionTime->Create();
    //this->InsertionTime->GetWidget()->SetRestrictValueToDouble();
    this->InsertionTime->GetLabel()->SetBackgroundColor( 0.7, 0.7, 0.7 );
    this->InsertionTime->SetLabelText( "Time spent on insertion (minutes, seconds):" );
    this->InsertionTime->SetWidth( 7 );
    this->InsertionTime->GetWidget()->SetDisabledBackgroundColor( 0.9, 0.9, 0.9 );
    this->InsertionTime->GetWidget()->ReadOnlyOn();
    }

  this->Script( "pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->InsertionTime->GetWidgetName() );
  
  
  // TO DO: install callbacks
  this->InstallCallbacks();
  
  this->PopulateControls();
  this->UpdateGUI();
}


void
vtkPerkStationValidateStep
::ShowPlanListFrame()
{
  vtkKWWizardWidget* wizard_widget = this->GetGUI()->GetWizardWidget();
  vtkKWWidget* parent = wizard_widget->GetClientArea();
  
  
    // Create a frame for the plan list.
  
  if ( ! this->PlanListFrame )
    {
    this->PlanListFrame = vtkKWFrame::New();
    }
  if ( ! this->PlanListFrame->IsCreated() )
    {
    this->PlanListFrame->SetParent( parent );
    this->PlanListFrame->Create();
    }
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->PlanListFrame->GetWidgetName() );
  
  
    // Create the plan list.
  
  if ( ! this->PlanList )
    {
    this->PlanList = vtkKWMultiColumnListWithScrollbars::New();
    this->PlanList->SetParent( this->PlanListFrame );
    this->PlanList->Create();
    this->PlanList->SetHeight( 1 );
    this->PlanList->GetWidget()->SetSelectionTypeToRow();
    this->PlanList->GetWidget()->SetSelectionBackgroundColor( 1, 0, 0 );
    this->PlanList->GetWidget()->MovableRowsOff();
    this->PlanList->GetWidget()->MovableColumnsOff();
    
      // Create the columns.
    
    for ( int col = 0; col < PLAN_COL_COUNT; ++ col )
      {
      this->PlanList->GetWidget()->AddColumn( PLAN_COL_LABELS[ col ] );
      this->PlanList->GetWidget()->SetColumnWidth( col, PLAN_COL_WIDTHS[ col ] );
      this->PlanList->GetWidget()->SetColumnAlignmentToLeft( col );
      }
    }
  
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->PlanList->GetWidgetName() );
}


//----------------------------------------------------------------------------
void
vtkPerkStationValidateStep
::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os,indent );
}


//----------------------------------------------------------------------------
void
vtkPerkStationValidateStep
::RemoveGUIObservers()
{
  if ( this->PlanList )
    {
    this->PlanList->GetWidget()->SetSelectionChangedCommand( this, "" );
    }
  
  if (this->ResetValidationButton)
    {
    this->ResetValidationButton->RemoveObservers( vtkKWPushButton::InvokedEvent,
                                                  (vtkCommand *)this->WizardGUICallbackCommand );
    }
}



//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::StartOverNewExperiment()
{
 this->GetGUI()->ResetAndStartNewExperiment();
}


//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::PopulateControls()
{
  double ras[ 3 ];
  this->GetGUI()->GetMRMLNode()->GetValidateEntryPoint( ras );
      
  this->EntryPoint->GetWidget( 0 )->SetValueAsDouble( ras[ 0 ] );
  this->EntryPoint->GetWidget( 1 )->SetValueAsDouble( ras[ 1 ] );
  this->EntryPoint->GetWidget( 2 )->SetValueAsDouble( ras[ 2 ] );
  
  this->GetGUI()->GetMRMLNode()->GetValidateTargetPoint( ras );
  
  this->TargetPoint->GetWidget( 0 )->SetValueAsDouble( ras[ 0 ] );
  this->TargetPoint->GetWidget( 1 )->SetValueAsDouble( ras[ 1 ] );
  this->TargetPoint->GetWidget( 2 )->SetValueAsDouble( ras[ 2 ] );    
}


//-----------------------------------------------------------------------------
void vtkPerkStationValidateStep::ProcessImageClickEvents(
  vtkObject *caller, unsigned long event, void *callData )
{
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  
  if (    ! wizard_widget
       || wizard_widget->GetWizardWorkflow()->GetCurrentStep() !=  this
       || ! this->GetGUI()->GetMRMLNode()
       || ! this->GetGUI()->GetMRMLNode()->GetValidationVolumeNode()
       || strcmp( this->GetGUI()->GetMRMLNode()->GetVolumeInUse(),
                  "Validation" ) != 0 )
    {
    return;
    }
  
  if( this->EntryTargetAcquired )
    {
    return;
    }
  
  vtkSlicerInteractorStyle *s =
    vtkSlicerInteractorStyle::SafeDownCast( caller );
  
  vtkSlicerInteractorStyle *istyle0 = vtkSlicerInteractorStyle::SafeDownCast(
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
    GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->
    GetInteractorStyle() );
  
  vtkRenderWindowInteractor *rwi;
  vtkMatrix4x4 *matrix;
  vtkRenderer *renderer = this->GetGUI()->GetApplicationGUI()->
                                GetMainSliceGUI( "Red" )->GetSliceViewer()->
                                GetRenderWidget()->GetOverlayRenderer();

  if (    ( s == istyle0 )
       && ( event == vtkCommand::LeftButtonPressEvent ) )
    {
    ++ this->ClickNumber;
    
      // mouse click happened in the axial slice view      
    vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(
         this->GetGUI()->GetApplicationGUI() )->GetMainSliceGUI( "Red" );
    rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->
          GetRenderWindowInteractor();    
    matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
    
    // capture the point
    int point[ 2 ];
    rwi->GetLastEventPosition( point );
    double inPt[ 4 ] = { point[ 0 ], point[ 1 ], 0, 1 };
    double outPt[ 4 ];
    
    matrix->MultiplyPoint( inPt, outPt ); 
    double ras[ 3 ] = { outPt[ 0 ], outPt[ 1 ], outPt[ 2 ] };

      // depending on click number, it is either Entry point or target point
    
    if ( this->ClickNumber == 1 )
      {
      this->GetGUI()->GetMRMLNode()->SetValidateEntryPoint( ras );
      }
    else if ( this->ClickNumber == 2 )
      {
      this->GetGUI()->GetMRMLNode()->SetValidateTargetPoint( ras );      
      
      this->ClickNumber = 0;

      this->OverlayValidationNeedleAxis();

      double rasEntry[ 3 ];
      double rasTarget[ 3 ];
      this->GetGUI()->GetMRMLNode()->GetValidateEntryPoint( rasEntry );
      this->GetGUI()->GetMRMLNode()->GetValidateTargetPoint( rasTarget );
      
      this->InsertionDepth->GetWidget()->SetValueAsDouble(
        this->GetGUI()->GetMRMLNode()->GetValidationDepth() );
      
      this->LogTimer->StopTimer();
      this->GetGUI()->GetMRMLNode()->SetTimeOnValidateStep(
        this->LogTimer->GetElapsedTime() );

      this->PresentValidationErrors();

      this->EntryTargetAcquired = true;
      }
    
    this->PopulateControls();

    }

}
//------------------------------------------------------------------------------
void vtkPerkStationValidateStep::PresentValidationErrors()
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  // things to do
  // overlay the needle line
  // display entry point error, target point error, insertion angle error, insertion depth error
  // display time performance

    // display entry point error, target point error, insertion angle error,
    // insertion depth error
  
  this->EntryPointError->GetWidget()->SetValueAsDouble(
    mrmlNode->GetEntryPointError() );
  this->TargetPointError->GetWidget()->SetValueAsDouble(
    mrmlNode->GetTargetPointError() );
  
  // todo: fix this
  // insertion depth error
  // double depthError = mrmlNode->GetActualPlanInsertionDepth() - this->InsertionDepth->GetWidget()->GetValueAsDouble();
  // this->InsertionDepthError->GetWidget()->SetValueAsDouble(depthError);

  // time performance
  char timeStr[20];
  long time = 0;
  time = mrmlNode->GetTimeOnCalibrateStep();
  sprintf(timeStr, "%02d:%02d", time/60, time%60);
  this->CalibrationTime->GetWidget()->SetValue(timeStr);

  time = mrmlNode->GetTimeOnPlanStep();
  sprintf(timeStr, "%02d:%02d", time/60, time%60);
  this->PlanTime->GetWidget()->SetValue(timeStr);

  time = mrmlNode->GetTimeOnInsertStep();
  sprintf(timeStr, "%02d:%02d", time/60, time%60);
  this->InsertionTime->GetWidget()->SetValue(timeStr);





}
//------------------------------------------------------------------------------
void vtkPerkStationValidateStep::OverlayValidationNeedleAxis()
{
  vtkSlicerSliceGUI* sliceGUI = vtkSlicerApplicationGUI::SafeDownCast( 
                                  this->GetGUI()->GetApplicationGUI() )->
                                  GetMainSliceGUI( "Red" );
  
  vtkRenderer* renderer = sliceGUI->GetSliceViewer()->
                          GetRenderWidget()->GetOverlayRenderer();
  
  
    // get the world coordinates
  
  int point[ 2 ];
  double worldCoordinate[ 4 ];
  vtkMatrix4x4 *xyToRAS = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
  vtkMatrix4x4 *rasToXY = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert( xyToRAS, rasToXY );
  
  
  double rasEntry[ 3 ];
  this->GetGUI()->GetMRMLNode()->GetValidateEntryPoint( rasEntry );
  double inPt[ 4 ] = { rasEntry[ 0 ], rasEntry[ 1 ], rasEntry[ 2 ], 1 };
  double outPt[ 4 ];  
  rasToXY->MultiplyPoint( inPt, outPt );
  point[ 0 ] = outPt[ 0 ];
  point[ 1 ] = outPt[ 1 ];

  renderer->SetDisplayPoint( point[ 0 ], point[ 1 ], 0 );
  renderer->DisplayToWorld();
  renderer->GetWorldPoint( worldCoordinate );
  double wcEntry[ 3 ] = { worldCoordinate[ 0 ], worldCoordinate[ 1 ],
                          worldCoordinate[ 2 ] };

  double rasTarget[ 3 ];
  this->GetGUI()->GetMRMLNode()->GetValidateTargetPoint( rasTarget );
  inPt[ 0 ] = rasTarget[ 0 ];
  inPt[ 1 ] = rasTarget[ 1 ];
  inPt[ 2 ] = rasTarget[ 2 ];
  rasToXY->MultiplyPoint( inPt, outPt );
  point[ 0 ] = outPt[ 0 ];
  point[ 1 ] = outPt[ 1 ];
  renderer->SetDisplayPoint( point[ 0 ], point[ 1 ], 0 );
  renderer->DisplayToWorld();
  renderer->GetWorldPoint( worldCoordinate );
  double wcTarget[ 3 ] = { worldCoordinate[ 0 ], worldCoordinate[ 1 ],
                           worldCoordinate[ 2 ] };

    // add a dotted line using two end points
    // set up the line
  vtkSmartPointer< vtkLineSource > line =
      vtkSmartPointer< vtkLineSource >::New();
    line->SetPoint1( wcEntry );
    line->SetPoint2( wcTarget );
    line->SetResolution( 100 );

    // set up the mapper
  vtkSmartPointer< vtkPolyDataMapper > lineMapper =
      vtkSmartPointer< vtkPolyDataMapper >::New();
    lineMapper->SetInputConnection( line->GetOutputPort() );
  
  this->ValidationNeedleActor->SetMapper( lineMapper );
  this->ValidationNeedleActor->GetProperty()->SetLineStipplePattern( 0xffff );
  this->ValidationNeedleActor->GetProperty()->SetColor( 255, 0, 255 );


    // add to actor collection
  sliceGUI->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->
            AddActor( this->ValidationNeedleActor );
  
  sliceGUI->GetSliceViewer()->RequestRender(); 
}


//------------------------------------------------------------------------------
void vtkPerkStationValidateStep::RemoveValidationNeedleAxis()
{
  // should remove the overlay needle guide
  vtkActorCollection *collection = this->GetGUI()->GetApplicationGUI()->
    GetMainSliceGUI( "Red" )->GetSliceViewer()->GetRenderWidget()->
    GetOverlayRenderer()->GetActors();
  
  if ( collection->IsItemPresent( this->ValidationNeedleActor ) )
    {
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
          GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->
          RemoveActor( this->ValidationNeedleActor );
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
          GetSliceViewer()->RequestRender();
    }
}


//------------------------------------------------------------------------------
void
vtkPerkStationValidateStep
::Reset()
{
 
    // reset parameters of mrml node
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if ( ! mrmlNode )
    {
      // TO DO: what to do on failure
    return;
    }

  double ras[3] = { 0.0, 0.0, 0.0 };
  mrmlNode->SetValidateEntryPoint( ras );
  mrmlNode->SetValidateTargetPoint( ras );
  
    // remove the overlaid needle axis
  this->RemoveValidationNeedleAxis();

    // reset local member variables to defaults
  this->EntryTargetAcquired = false;
  this->ClickNumber = 0;
  this->ProcessingCallback = false;
    // reset gui controls
  this->ResetControls();
}



void
vtkPerkStationValidateStep
::UpdateGUI()
{
  vtkMRMLPerkStationModuleNode* mrmlNode = this->GetGUI()->GetMRMLNode();
  
  if ( ! mrmlNode ) return;
  
  
    // Update plan list.
  
  if ( this->PlanList == NULL || this->PlanList->GetWidget() == NULL ) return;
  
  int numPlans = mrmlNode->GetNumberOfPlans();
  
  bool deleteFlag = true;
  if ( numPlans != this->PlanList->GetWidget()->GetNumberOfRows() )
    {
    this->PlanList->GetWidget()->DeleteAllRows();
    }
  else
    {
    deleteFlag = false;
    }
  
  
  const int PRECISION_DIGITS = 1;
  
  double planEntry[ 3 ];
  double planTarget[ 3 ];
  double validationEntry[ 3 ];
  double validationTarget[ 3 ];
  
  for ( int row = 0; row < numPlans; ++ row )
    {
    vtkPerkStationPlan* plan = mrmlNode->GetPlanAtIndex( row );
    
    if ( deleteFlag )
      {
      this->PlanList->GetWidget()->AddRow();
      }
    
    plan->GetEntryPointRAS( planEntry );
    plan->GetTargetPointRAS( planTarget );
    plan->GetValidationEntryPointRAS( validationEntry );
    plan->GetValidationTargetPointRAS( validationTarget );
    
    if ( planEntry == NULL || planTarget == NULL )
      {
      vtkErrorMacro( "ERROR: No plan points in plan" );
      }
    
    vtkKWMultiColumnList* colList = this->PlanList->GetWidget();
    if ( deleteFlag || plan->GetName().compare( this->PlanList->GetWidget()->GetCellText( row, PLAN_COL_NAME ) ) != 0 )
      {
      this->PlanList->GetWidget()->SetCellText( row, PLAN_COL_NAME, plan->GetName().c_str() );
      for ( int i = 0; i < 3; ++ i )
        {
        std::ostrstream os;
        os << std::setiosflags( ios::fixed | ios::showpoint ) << std::setprecision( PRECISION_DIGITS );
        os << planEntry[ i ] << std::ends;
        colList->SetCellText( row, PLAN_COL_ER + i, os.str() );
        os.rdbuf()->freeze();
        }
      for ( int i = 0; i < 3; ++ i )
        {
        std::ostrstream os;
        os << std::setiosflags( ios::fixed | ios::showpoint ) << std::setprecision( PRECISION_DIGITS );
        os << planTarget[ i ] << std::ends;
        colList->SetCellText( row, PLAN_COL_ER + 3 + i, os.str() );
        os.rdbuf()->freeze();
        }
      }
    }
  
  this->PlanList->GetWidget()->SelectRow( mrmlNode->GetCurrentPlanIndex() );
}


//------------------------------------------------------------------------------
void vtkPerkStationValidateStep::ResetControls()
{
  if (this->EntryPoint)
    {
    this->EntryPoint->GetWidget(0)->SetValue("");
    this->EntryPoint->GetWidget(1)->SetValue("");
    this->EntryPoint->GetWidget(2)->SetValue("");
    }

  if (this->TargetPoint)
    {  
    this->TargetPoint->GetWidget(0)->SetValue("");
    this->TargetPoint->GetWidget(1)->SetValue("");
    this->TargetPoint->GetWidget(2)->SetValue(""); 
    }

  if (this->InsertionDepth)
    {  
    this->InsertionDepth->GetWidget()->SetValue("");
    }
}


//----------------------------------------------------------------------------
void
vtkPerkStationValidateStep
::WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData )
{
    vtkPerkStationValidateStep *self = reinterpret_cast<vtkPerkStationValidateStep *>(clientData);
    if (self) { self->ProcessGUIEvents(caller, event, callData); }
}

//----------------------------------------------------------------------------
void
vtkPerkStationValidateStep
::ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData)
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  if(!mrmlNode->GetPlanningVolumeNode() || strcmp(mrmlNode->GetVolumeInUse(), "Validation")!=0)
      return;

  
  if (this->ProcessingCallback)
    {
    return;
    }

  this->ProcessingCallback = true;
  
  // reset plan button
  if (this->ResetValidationButton && this->ResetValidationButton == vtkKWPushButton::SafeDownCast(caller) && (event == vtkKWPushButton::InvokedEvent))
    {
    this->Reset();
    }
  this->ProcessingCallback = false;
}
