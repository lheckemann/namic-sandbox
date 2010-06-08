#include "vtkPerkStationPlanStep.h"

#include "vtkPerkStationModuleGUI.h"
#include "vtkPerkStationModuleLogic.h"
#include "vtkPerkStationSecondaryMonitor.h"
#include "vtkMRMLFiducialListNode.h"

#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWEntrySet.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

#include "vtkCylinderSource.h"
#include "vtkIdentityTransform.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkMatrixToHomogeneousTransform.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkTransformPolyDataFilter.h"

// debug
#include "vtkSlicerApplication.h"



#define DELETE_IF_NULL_WITH_SETPARENT_NULL(obj) \
  if (obj) \
    { \
    obj->SetParent(NULL); \
    obj->Delete(); \
    obj = NULL; \
    };



//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationPlanStep);
vtkCxxRevisionMacro(vtkPerkStationPlanStep, "$Revision: 1.1 $");


//----------------------------------------------------------------------------
vtkPerkStationPlanStep::vtkPerkStationPlanStep()
{
  this->SetName("2/5. Plan");
  this->SetDescription("Plan the needle insertion");

  this->WizardGUICallbackCommand->SetCallback(vtkPerkStationPlanStep::WizardGUICallback);

  this->ResetFrame = NULL;
  this->ResetPlanButton = NULL;
  
  this->TargetFirstFrame = NULL;
  this->TargetFirstCheck = NULL;
  this->EntryPointFrame = NULL;
  this->EntryPointLabel = NULL;
  this->EntryPoint = NULL;
  this->TargetPointFrame = NULL;
  this->TargetPointLabel = NULL;
  this->TargetPoint = NULL;
  this->InsertionAngle = NULL;
  this->InsertionDepth = NULL;
  this->TiltInformationFrame = NULL;
  this->TiltMsg = NULL;
  this->SystemTiltAngle = NULL;
  
  this->PlanningLineActor = vtkSmartPointer< vtkActor >::New();
    this->PlanningLineActor->SetVisibility( 0 );
    this->PlanningLineActor->GetProperty()->SetColor( 1.0, 0.0, 1.0 );
  
    // PlanList frame
  
  this->PlanListFrame = NULL;
  this->PlanList = NULL;
  this->DeleteButton = NULL;
  
  
  this->WCEntryPoint[0] = 0.0;
  this->WCEntryPoint[1] = 0.0;
  this->WCEntryPoint[2] = 0.0;
  this->WCTargetPoint[0] = 0.0;
  this->WCTargetPoint[1] = 0.0;
  this->WCTargetPoint[2] = 0.0;

  this->EntryTargetAcquired = false;
  this->ClickNumber = 0;
  this->ProcessingCallback = false;
  this->NeedleActor = NULL;
  
  this->SelectTargetFirst = true;
}

//----------------------------------------------------------------------------
vtkPerkStationPlanStep::~vtkPerkStationPlanStep()
{
  this->RemoveGUIObservers();
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->ResetFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->ResetPlanButton );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TargetFirstFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TargetFirstCheck );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->EntryPointFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->EntryPointLabel );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->EntryPoint );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TargetPointFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TargetPointLabel );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TargetPoint );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->InsertionAngle );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->InsertionDepth );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TiltInformationFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TiltMsg );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->SystemTiltAngle );
  
    // PlanList frame
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->PlanListFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->PlanList );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->DeleteButton );
}

//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();
    
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);
  vtkKWWidget *parent = wizard_widget->GetClientArea();
  int enabled = parent->GetEnabled();

  // clear controls
  if (this->ResetFrame)
    {
    this->Script("pack forget %s", 
                    this->ResetFrame->GetWidgetName());
    }

  if (this->ResetPlanButton)
    {
    this->Script("pack forget %s", 
                    this->ResetPlanButton->GetWidgetName());
    }
  
  
  if ( this->TargetFirstCheck )
    {
    this->Script( "pack forget %s", this->TargetFirstCheck->GetWidgetName() );
    }
  

  this->SetName("2/4. Plan");
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
  
  

  if (!this->ResetPlanButton)
    {
    this->ResetPlanButton = vtkKWPushButton::New();
    }
  if(!this->ResetPlanButton->IsCreated())
    {
    this->ResetPlanButton->SetParent(this->ResetFrame);
    this->ResetPlanButton->SetText("Reset plan");
    this->ResetPlanButton->SetBorderWidth(2);
    this->ResetPlanButton->SetReliefToRaised();      
    this->ResetPlanButton->SetHighlightThickness(2);
    this->ResetPlanButton->SetBackgroundColor(0.85,0.85,0.85);
    this->ResetPlanButton->SetActiveBackgroundColor(1,1,1);      
    this->ResetPlanButton->SetImageToPredefinedIcon(vtkKWIcon::IconTrashcan);
    this->ResetPlanButton->Create();
    }
  
  this->Script("pack %s -side top -padx 2 -pady 4", 
                    this->ResetPlanButton->GetWidgetName());
 
 
  this->SetDescription("Plan the needle insertion");

  /*vtkPerkStationtMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  
  if (!mrmlManager || !wizard_widget)
    {
    return;
    }*/

 

  // Create the individual components
  
  if ( ! this->TargetFirstFrame )
    {
    this->TargetFirstFrame = vtkKWFrame::New();
    }
  if ( ! this->TargetFirstFrame->IsCreated() )
    {
    this->TargetFirstFrame->SetParent( parent );
    this->TargetFirstFrame->Create();
    }
  
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->TargetFirstFrame->GetWidgetName() );
  
  if ( ! this->TargetFirstCheck )
    {
    this->TargetFirstCheck = vtkKWCheckButtonWithLabel::New();
    }
  if ( ! this->TargetFirstCheck->IsCreated() )
    {
    this->TargetFirstCheck->SetParent( this->TargetFirstFrame );
    this->TargetFirstCheck->SetLabelText( "Select target point first" );
    this->TargetFirstCheck->Create();
    this->TargetFirstCheck->GetWidget()->SetSelectedState(
       ( this->SelectTargetFirst ? 1 : 0 ) );
    }
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 4", 
                this->TargetFirstCheck->GetWidgetName() );
  
  
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
                this->EntryPointFrame->GetWidgetName() );

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


  // insertion angle  
  if (!this->InsertionAngle)
    {
    this->InsertionAngle =  vtkKWEntryWithLabel::New(); 
    }
  if (!this->InsertionAngle->IsCreated())
    {
    this->InsertionAngle->SetParent(parent);
    this->InsertionAngle->Create();
    this->InsertionAngle->GetWidget()->SetRestrictValueToDouble();
    this->InsertionAngle->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->InsertionAngle->SetLabelText("Insertion angle (in degrees):");
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->InsertionAngle->GetWidgetName());


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
    this->InsertionDepth->SetLabelText("Insertion depth (in mm):   ");
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->InsertionDepth->GetWidgetName());


   //frame
  if (!this->TiltInformationFrame)
    {
    this->TiltInformationFrame = vtkKWFrame::New();
    }
  if (!this->TiltInformationFrame->IsCreated())
    {
    this->TiltInformationFrame->SetParent(parent);
    this->TiltInformationFrame->Create();
    }

  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->TiltInformationFrame->GetWidgetName() );


  // insertion depth  
  if ( ! this->SystemTiltAngle )
    {
    this->SystemTiltAngle =  vtkKWEntryWithLabel::New(); 
    }
  if (!this->SystemTiltAngle->IsCreated())
    {
    this->SystemTiltAngle->SetParent( this->TiltInformationFrame );
    this->SystemTiltAngle->Create();
    this->SystemTiltAngle->GetWidget()->SetRestrictValueToDouble();
    this->SystemTiltAngle->GetLabel()->SetBackgroundColor( 0.7, 0.7, 0.7 );
    this->SystemTiltAngle->SetLabelText( "System tilt angle (in degrees):   " );
    }

  this->Script( "pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->SystemTiltAngle->GetWidgetName() );

  
  // msg label
  if( ! this->TiltMsg )
    {
    this->TiltMsg = vtkKWLabel::New();
    }
  if( ! this->TiltMsg->IsCreated() )
    {
    this->TiltMsg->SetParent( this->TiltInformationFrame );
    this->TiltMsg->Create();
    this->TiltMsg->SetImageToPredefinedIcon( vtkKWIcon::IconInfoMini );
    this->TiltMsg->SetCompoundModeToLeft();
    this->TiltMsg->SetPadX(2);
    this->TiltMsg->SetText("");
    }
   this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                 this->TiltMsg->GetWidgetName() ); 
  
  
  this->ShowPlanListFrame();
  
  
  // TO DO: install callbacks
  this->InstallCallbacks();
  
  this->PopulateControlsOnLoadPlanning();
  
  wizard_widget->SetErrorText(
    "Please note that the order of the clicks on image is important." );
}


void
vtkPerkStationPlanStep
::ShowPlanListFrame()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
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
    
    
    }
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->PlanList->GetWidgetName());
  
  
  if ( ! this->DeleteButton )
    {
    this->DeleteButton = vtkKWPushButton::New();
    }
  if ( ! this->DeleteButton->IsCreated() )
    {
    this->DeleteButton->SetParent( this->PlanListFrame );
    this->DeleteButton->Create();
    this->DeleteButton->SetText( "Delete plan" );
    }
  this->Script( "pack %s -side top -anchor ne -padx 2 -pady 4", 
                this->DeleteButton->GetWidgetName() );
}


//----------------------------------------------------------------------------
void
vtkPerkStationPlanStep
::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void
vtkPerkStationPlanStep
::AddGUIObservers()
{
  this->RemoveGUIObservers();

  if (this->ResetPlanButton)
    {
    this->ResetPlanButton->AddObserver( vtkKWPushButton::InvokedEvent,
      (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if(this->InsertionDepth)
    {
    this->InsertionDepth->GetWidget()->SetRestrictValueToDouble();
    this->InsertionDepth->GetWidget()->AddObserver(
      vtkKWEntry::EntryValueChangedEvent,
      (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if(this->InsertionAngle)
    {
    this->InsertionAngle->GetWidget()->SetRestrictValueToDouble();
    this->InsertionAngle->GetWidget()->AddObserver(
      vtkKWEntry::EntryValueChangedEvent,
      (vtkCommand *)this->WizardGUICallbackCommand );
    }
  
  
  vtkSlicerApplicationGUI::SafeDownCast( this->GetGUI()->GetApplicationGUI() )->
    GetMainSliceGUI( "Red" )->GetSliceViewer()->GetRenderWidget()->
    GetOverlayRenderer()->AddActor( this->PlanningLineActor ); 
}


//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::RemoveGUIObservers()
{
  if (this->ResetPlanButton)
    {
    this->ResetPlanButton->RemoveObservers( vtkKWPushButton::InvokedEvent,
      (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if(this->InsertionDepth)
    {
    this->InsertionDepth->RemoveObservers( vtkKWEntry::EntryValueChangedEvent,
      (vtkCommand *)this->WizardGUICallbackCommand );
    }

  if(this->InsertionAngle)
    {
    this->InsertionAngle->RemoveObservers( vtkKWEntry::EntryValueChangedEvent,
      (vtkCommand *)this->WizardGUICallbackCommand );
    }

}


//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::InstallCallbacks()
{
  this->AddGUIObservers();
}


//---------------------------------------------------------------------------
void vtkPerkStationPlanStep::PopulateControlsOnLoadPlanning()
{
    if ( ! this->GetGUI()->GetMRMLNode() ) return;
    
    double rasEntry[ 3 ];
    this->GetGUI()->GetMRMLNode()->GetPlanEntryPoint( rasEntry );
    
    // entry point
    this->EntryPoint->GetWidget( 0 )->SetValueAsDouble( rasEntry[ 0 ] );
    this->EntryPoint->GetWidget( 1 )->SetValueAsDouble( rasEntry[ 1 ] );
    this->EntryPoint->GetWidget( 2 )->SetValueAsDouble( rasEntry[ 2 ] );
    
    double rasTarget[ 3 ];
    this->GetGUI()->GetMRMLNode()->GetPlanTargetPoint( rasTarget );
    
    // target point
    this->TargetPoint->GetWidget( 0 )->SetValueAsDouble( rasTarget[ 0 ] );
    this->TargetPoint->GetWidget( 1 )->SetValueAsDouble( rasTarget[ 1 ] );
    this->TargetPoint->GetWidget( 2 )->SetValueAsDouble( rasTarget[ 2 ] );
    
    // insertion angle  
    double insAngle = this->GetGUI()->GetMRMLNode()->
                      GetActualPlanInsertionAngle();
    this->InsertionAngle->GetWidget()->SetValueAsDouble( insAngle );
    
    // insertion depth
    double insDepth = this->GetGUI()->GetMRMLNode()->
                      GetActualPlanInsertionDepth();
    this->InsertionDepth->GetWidget()->SetValueAsDouble( insDepth );
}


void
vtkPerkStationPlanStep
::ProcessImageClickEvents( vtkObject *caller,
                           unsigned long event,
                           void *callData )
{
  
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  
  
  if (    ! wizard_widget
       || wizard_widget->GetWizardWorkflow()->GetCurrentStep() != this
       || ! this->GetGUI()->GetMRMLNode()
       || ! this->GetGUI()->GetMRMLNode()->GetPlanningVolumeNode()
       || strcmp( this->GetGUI()->GetMRMLNode()->GetVolumeInUse(),
                  "Planning" )!= 0 )
    {
    return;
    }
  
  
    // see if the entry and target have already been acquired
  
  if( this->EntryTargetAcquired )
    {
    return;
    }
  
  
  vtkSlicerInteractorStyle *s =
    vtkSlicerInteractorStyle::SafeDownCast( caller );
  
  vtkSlicerInteractorStyle *istyle0 =
    vtkSlicerInteractorStyle::SafeDownCast(
      this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->
      GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->
      GetInteractorStyle() );
  
  
    // planning has to happen on slicer laptop, cannot be done from secondary
    // monitor, so don't listen to clicks in secondary monitor
  
  if (    ( s != istyle0 )
       || ( event != vtkCommand::LeftButtonPressEvent ) )
    {
    return;
    }
  
  
    // mouse click happened in the axial "Red" slice view
  
  vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(
    this->GetGUI()->GetApplicationGUI() )->GetMainSliceGUI( "Red" );
  
  vtkRenderWindowInteractor *rwi;
  rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->
        GetRenderWindowInteractor();    
  
  vtkMatrix4x4 *matrix;
    matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
  
  
  int point[ 2 ];
  rwi->GetLastEventPosition( point );
  double inPt[ 4 ] = { point[ 0 ], point[ 1 ], 0.0, 1.0 };
  double outPt[ 4 ];    
  matrix->MultiplyPoint( inPt, outPt ); 
  double ras[ 3 ] = { outPt[ 0 ], outPt[ 1 ], outPt[ 2 ] };
  
  
    // depending on click number, it is either Entry point or target point
  
  ++ this->ClickNumber;
  
  int entryClick = 1;
  int targetClick = 2;
  
  // this->TargetFirstCheck->GlobalWarningDisplayOn();
  
  if ( this->TargetFirstCheck->GetWidget()->GetSelectedState() != 0 )
    {
    targetClick = 1;
    entryClick = 2;
    }
  
  
  if ( this->ClickNumber == entryClick )
    {
      // entry point specification by user
    this->EntryPoint->GetWidget( 0 )->SetValueAsDouble( ras[ 0 ] );
    this->EntryPoint->GetWidget( 1 )->SetValueAsDouble( ras[ 1 ] );
    this->EntryPoint->GetWidget( 2 )->SetValueAsDouble( ras[ 2 ] );

      // record value in mrml node
    this->GetGUI()->GetMRMLNode()->SetPlanEntryPoint( ras );

      // record value in mrml fiducial list node          
    int index = this->GetGUI()->GetMRMLNode()->
      GetPlanMRMLFiducialListNode()->AddFiducialWithXYZ(
        ras[ 0 ], ras[ 1 ], ras[ 2 ], false );
    
    this->GetGUI()->GetMRMLNode()->GetPlanMRMLFiducialListNode()->
      SetNthFiducialLabelText( index, "Entry" );
    }
  else if ( this->ClickNumber == targetClick )
    {
    this->TargetPoint->GetWidget( 0 )->SetValueAsDouble( ras[ 0 ] );
    this->TargetPoint->GetWidget( 1 )->SetValueAsDouble( ras[ 1 ] );
    this->TargetPoint->GetWidget( 2 )->SetValueAsDouble( ras[ 2 ] );
    
      // record value in the MRML node
    this->GetGUI()->GetMRMLNode()->SetPlanTargetPoint( ras );
  
      // record value in mrml fiducial list node      
    int index = this->GetGUI()->GetMRMLNode()->GetPlanMRMLFiducialListNode()->
      AddFiducialWithXYZ( ras[0], ras[1], ras[2], false );
    
    this->GetGUI()->GetMRMLNode()->GetPlanMRMLFiducialListNode()->
      SetNthFiducialLabelText( index, "Target" );
    }
  
  
  if ( this->ClickNumber == 1 )  // On first click.
    {   
    this->LogTimer->StartTimer();  // Start the log timer.
    this->TargetFirstCheck->GetWidget()->SetEnabled( 0 );
    }
  
  
  if ( this->ClickNumber == 2 ) // Needle guide ready.
    {
    this->PopulateControlsOnLoadPlanning();
    this->OverlayNeedleGuide();
    this->GetGUI()->GetSecondaryMonitor()->OverlayNeedleGuide();  
    
    this->ClickNumber = 0;
    
    this->EntryTargetAcquired = true;
    this->TargetFirstCheck->GetWidget()->SetEnabled( 1 );
    }
  
  if ( this->ClickNumber != 1 )
    {
    this->PlanningLineActor->SetVisibility( 0 );
    }
}


void
vtkPerkStationPlanStep
::ProcessMouseMoveEvent( vtkObject *caller,
                         unsigned long event,
                         void *callData )
{
  if ( this->ClickNumber != 1 )
    {
    this->PlanningLineActor->SetVisibility( 0 );
    return;
    }
  
  this->PlanningLineActor->SetVisibility( 1 );
  
  
  vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(
    this->GetGUI()->GetApplicationGUI() )->GetMainSliceGUI( "Red" );
  
  vtkRenderWindowInteractor *rwi;
  rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->
        GetRenderWindowInteractor();    
  
  vtkMatrix4x4 *matrix;
  matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
  
  int point[ 2 ];
  rwi->GetLastEventPosition( point );
  double inPt[ 4 ] = { point[ 0 ], point[ 1 ], 0.0, 1.0 };
  double outPt[ 4 ];    
  matrix->MultiplyPoint( inPt, outPt ); 
  double ras[ 3 ] = { outPt[ 0 ], outPt[ 1 ], outPt[ 2 ] };
  
  
    // get the world coordinates
  vtkMatrix4x4 *xyToRAS = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
  vtkMatrix4x4 *rasToXY = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert(xyToRAS, rasToXY);
  
    // start point
  double rasEntry[ 3 ];
  if ( this->TargetFirstCheck->GetWidget()->GetSelectedState() != 0 )
    {
    this->GetGUI()->GetMRMLNode()->GetPlanTargetPoint( rasEntry );
    }
  else
    {
    this->GetGUI()->GetMRMLNode()->GetPlanEntryPoint( rasEntry );
    }
  // inPt[ 4 ] = { rasEntry[ 0 ], rasEntry[ 1 ], rasEntry[ 2 ], 1 };
  for ( int i = 0; i < 3; ++ i ) inPt[ i ] = rasEntry[ i ];
  inPt[ 3 ] = 1.0;
  rasToXY->MultiplyPoint( inPt, outPt );
  
  double entryPointXY[ 2 ] = { outPt[ 0 ], outPt[ 1 ] };
  
  vtkRenderer *renderer = sliceGUI->GetSliceViewer()->GetRenderWidget()->
    GetOverlayRenderer();
  
  double wc[ 4 ];
  renderer->SetDisplayPoint( point[ 0 ], point[ 1 ], 0 );
  renderer->DisplayToWorld();
  renderer->GetWorldPoint( wc );
  double endPointWC[ 3 ] = { wc[ 0 ], wc[ 1 ], wc[ 2 ] };
  
  renderer->SetDisplayPoint( entryPointXY[ 0 ], entryPointXY[ 1 ], 0.0 );
  renderer->DisplayToWorld();
  renderer->GetWorldPoint( wc );
  double startPointWC[ 3 ] = { wc[ 0 ], wc[ 1 ], wc[ 2 ] };
  
  vtkSmartPointer< vtkLineSource > lineSource =
      vtkSmartPointer< vtkLineSource >::New();
    lineSource->SetPoint1( startPointWC );
    lineSource->SetPoint2( endPointWC );
  
  vtkSmartPointer< vtkPolyDataMapper > lineMapper =
      vtkSmartPointer< vtkPolyDataMapper >::New();
    lineMapper->SetInputConnection( lineSource->GetOutputPort() );
  
  this->PlanningLineActor->SetMapper( lineMapper );
  
  sliceGUI->GetSliceViewer()->RequestRender(); 
}


//----------------------------------------------------------------------------
bool
vtkPerkStationPlanStep
::DoubleEqual( double val1, double val2 )
{
  bool result = false;
  if ( fabs(val2-val1) < 0.0001 ) result = true;
  return result;
}


//------------------------------------------------------------------------------
void
vtkPerkStationPlanStep
::OverlayNeedleGuide()
{
  vtkRenderer *renderer = this->GetGUI()->GetApplicationGUI()->
    GetMainSliceGUI( "Red" )->GetSliceViewer()->GetRenderWidget()->
    GetOverlayRenderer();

  
    // get the world coordinates of Entry and Target points.
  
  vtkSlicerSliceGUI *sliceGUI =
    vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->
      GetApplicationGUI() )->GetMainSliceGUI( "Red" );
  vtkMatrix4x4 *xyToRAS = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
  vtkMatrix4x4 *rasToXY = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert( xyToRAS, rasToXY );
  
  int entryPointXY[ 2 ];
  int targetPointXY[ 2 ];
  double worldCoordinate[ 4 ];
  
    // entry point
  
  double rasEntry[ 3 ];
  this->GetGUI()->GetMRMLNode()->GetPlanEntryPoint( rasEntry );
  double inPt[ 4 ] = { rasEntry[ 0 ], rasEntry[ 1 ], rasEntry[ 2 ], 1.0 };
  double outPt[ 4 ];  
  rasToXY->MultiplyPoint( inPt, outPt );
  entryPointXY[ 0 ] = outPt[ 0 ];
  entryPointXY[ 1 ] = outPt[ 1 ];
  
  renderer->SetDisplayPoint( entryPointXY[ 0 ], entryPointXY[ 1 ], 0 );
  renderer->DisplayToWorld();
  renderer->GetWorldPoint( worldCoordinate );
  this->WCEntryPoint[ 0 ] = worldCoordinate[ 0 ];
  this->WCEntryPoint[ 1 ] = worldCoordinate[ 1 ];
  this->WCEntryPoint[ 2 ] = worldCoordinate[ 2 ];
  
  double rasTarget[ 3 ];
  this->GetGUI()->GetMRMLNode()->GetPlanTargetPoint( rasTarget );
  inPt[ 0 ] = rasTarget[ 0 ];
  inPt[ 1 ] = rasTarget[ 1 ];
  inPt[ 2 ] = rasTarget[ 2 ];
  rasToXY->MultiplyPoint( inPt, outPt );
  targetPointXY[ 0 ] = outPt[ 0 ];
  targetPointXY[ 1 ] = outPt[ 1 ];
  renderer->SetDisplayPoint( targetPointXY[ 0 ], targetPointXY[ 1 ], 0 );
  renderer->DisplayToWorld();
  renderer->GetWorldPoint( worldCoordinate );
  this->WCTargetPoint[ 0 ] = worldCoordinate[ 0 ];
  this->WCTargetPoint[ 1 ] = worldCoordinate[ 1 ];
  this->WCTargetPoint[ 2 ] = worldCoordinate[ 2 ];
  
  
    // steps
    // get the cylinder source, create the cylinder, whose height is equal to
    // calculated insertion depth apply transform on the cylinder to world 
    // coordinates, using the information of entry and target point
    // i.e. using the insertion angle
    // add it to slice viewer's renderer
  
  vtkSmartPointer< vtkCylinderSource > needleGuide =
    vtkSmartPointer< vtkCylinderSource >::New();
  
  // TO DO: how to relate this to actual depth???
  
  double halfNeedleLength =
    sqrt( ( this->WCTargetPoint[ 0 ] - this->WCEntryPoint[ 0 ] ) *
          ( this->WCTargetPoint[ 0 ] - this->WCEntryPoint[ 0 ] ) +
          ( this->WCTargetPoint[ 1 ] - this->WCEntryPoint[ 1 ] ) *
          ( this->WCTargetPoint[ 1 ] - this->WCEntryPoint[ 1 ] ) );
  needleGuide->SetHeight( 2 * halfNeedleLength );
  needleGuide->SetRadius( 0.009 );
  needleGuide->SetResolution( 10 );
  
    // because cylinder is positioned at the window center
  double needleCenter[ 3 ];
  needleCenter[ 0 ] = this->WCEntryPoint[ 0 ];
  needleCenter[ 1 ] = this->WCEntryPoint[ 1 ];
  
  
    // insertion angle is the angle with x-axis of the line which has entry
    // and target as its end points;
  
  double tang =   ( this->WCEntryPoint[ 0 ] - this->WCTargetPoint[ 0 ] )
                / ( this->WCEntryPoint[ 1 ] - this->WCTargetPoint[ 1 ] );
  double angle = - ( 180.0 / vtkMath::Pi() ) * atan( tang );
  
  
  vtkSmartPointer< vtkTransform > transform =
      vtkSmartPointer< vtkTransform >::New();
    transform->Translate( needleCenter[ 0 ], needleCenter[ 1 ], 0.0 );
    transform->RotateZ( angle );
  
  vtkSmartPointer< vtkTransformPolyDataFilter > filter =
      vtkSmartPointer< vtkTransformPolyDataFilter >::New();
    filter->SetInputConnection( needleGuide->GetOutputPort() );
    filter->SetTransform( transform );
  
  vtkSmartPointer< vtkPolyDataMapper > needleMapper =
      vtkSmartPointer< vtkPolyDataMapper >::New();
    needleMapper->SetInputConnection( filter->GetOutputPort() );
  
  this->NeedleActor = vtkActor::New(); 
  this->NeedleActor->SetMapper( needleMapper );  
  this->NeedleActor->GetProperty()->SetOpacity( 0.3 );
  
  // add to renderer of the Axial slice viewer
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
    GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->
      AddActor( this->NeedleActor );
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
    GetSliceViewer()->RequestRender(); 
}


void
vtkPerkStationPlanStep
::Reset()
{
  // reset the overlay needle guide both in sliceviewer and in secondary monitor
  this->GetGUI()->GetSecondaryMonitor()->RemoveOverlayNeedleGuide();

  this->GetGUI()->GetSecondaryMonitor()->RemoveDepthPerceptionLines();
  
  
  this->RemoveOverlayNeedleGuide();

  // reset parameters of mrml node
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }
  
  double ras[3] = {0.0,0.0,0.0};
  mrmlNode->SetPlanEntryPoint(ras);
  mrmlNode->SetPlanTargetPoint(ras);
  
  
  double tiltAngle = mrmlNode->GetTiltAngle();

  if (!vtkPerkStationModuleLogic::DoubleEqual(tiltAngle,0))
    {
     // also if the tilt angle was not zero,
    // then reset the slice to ras matrix to the original
    vtkSlicerSliceLogic *sliceLogic = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetLogic();
    vtkMRMLSliceNode *sliceNode = sliceLogic->GetSliceNode();
    sliceNode->GetSliceToRAS()->DeepCopy( mrmlNode->GetSliceToRAS() );
    sliceNode->UpdateMatrices();
    // this should reset the display in slicer viewer's

    // the display and hence matrices also need to be reset in secondary monitor
    // this->GetGUI()->GetSecondaryMonitor()->ResetTilt();

     // repack the slicer viewer's layout to one slice only   
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetGUI()->GetApplicationGUI ( ));
    p->RepackMainViewer ( vtkMRMLLayoutNode::SlicerLayoutOneUpRedSliceView, "Red");
    
    }
  // set tilt angle back to zero
  mrmlNode->SetTiltAngle(0);
  // the fiducial list node
  mrmlNode->GetPlanMRMLFiducialListNode()->RemoveAllFiducials();


  // reset local member variables to defaults
  this->WCEntryPoint[0] = 0.0;
  this->WCEntryPoint[1] = 0.0;
  this->WCEntryPoint[2] = 0.0;
  this->WCTargetPoint[0] = 0.0;
  this->WCTargetPoint[1] = 0.0;
  this->WCTargetPoint[2] = 0.0;
  this->EntryTargetAcquired = false;
  this->ClickNumber = 0;
  this->ProcessingCallback = false;
  // reset gui controls
  this->ResetControls();
  
}
//------------------------------------------------------------------------------
void vtkPerkStationPlanStep::ResetControls()
{
  if(this->EntryPoint)
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

  if (this->InsertionAngle)
    {
    this->InsertionAngle->GetWidget()->SetValue("");
    }  
  if (this->InsertionDepth)
    {
    this->InsertionDepth->GetWidget()->SetValue("");
    }
  if (this->SystemTiltAngle)
    {
    this->SystemTiltAngle->GetWidget()->SetValue("");
    }
  if (this->TiltMsg)
    {
    this->TiltMsg->SetText("");
    }
}


// ----------------------------------------------------------------------------
void vtkPerkStationPlanStep::RemoveOverlayNeedleGuide()
{
    // should remove the overlay needle guide
  vtkActorCollection *collection = this->GetGUI()->GetApplicationGUI()->
      GetMainSliceGUI( "Red" )->GetSliceViewer()->GetRenderWidget()->
      GetOverlayRenderer()->GetActors();
  
  if ( collection->IsItemPresent( this->NeedleActor ) )
    {
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
          GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->
          RemoveActor( this->NeedleActor );
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
          GetSliceViewer()->RequestRender();
    }
}


// ----------------------------------------------------------------------------
void vtkPerkStationPlanStep::WizardGUICallback( vtkObject* caller,
                                                unsigned   long event,
                                                void*      clientData,
                                                void*      callData )
{
    vtkPerkStationPlanStep *self =
      reinterpret_cast< vtkPerkStationPlanStep* >( clientData );
    if ( self ) self->ProcessGUIEvents( caller, event, callData );
}


//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::ProcessGUIEvents( vtkObject* caller,
                                           unsigned long event, void* callData )
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(    ! mrmlNode
      || ! mrmlNode->GetPlanningVolumeNode()
      || strcmp( mrmlNode->GetVolumeInUse(), "Planning" ) != 0 )
    {
    return;
    }

  
    // Avoid parallel instances.
  
  if ( this->ProcessingCallback ) return;
  this->ProcessingCallback = true;

  
    // reset plan button
  
  if (    this->ResetPlanButton
       && this->ResetPlanButton == vtkKWPushButton::SafeDownCast( caller )
       && ( event == vtkKWPushButton::InvokedEvent ) )
    {
    this->Reset();
    }
  
  this->ProcessingCallback = false;
}


//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::Validate()
{
  this->Superclass::Validate();
  
  // depth perception lines render
  this->GetGUI()->GetSecondaryMonitor()->SetDepthPerceptionLines();

  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }
}

