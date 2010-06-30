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
  
  this->AddGUIObservers();
}



//----------------------------------------------------------------------------
vtkPerkStationValidateStep::vtkPerkStationValidateStep()
{
  this->SetName("4/5. Validate");
  this->SetDescription("Mark actual entry point and target hit");  
  this->WizardGUICallbackCommand->SetCallback(vtkPerkStationValidateStep::WizardGUICallback);

  
  this->PlanListFrame = NULL;
  this->PlanListLabel = NULL;
  this->PlanList = NULL;
  
  
  this->ResetFrame = NULL;
  this->ResetValidationButton = NULL;
  
  
  this->InsertionDepth = NULL;

  this->ValidationErrorsFrame = NULL;
  this->EntryPointError = NULL;
  this->TargetPointError = NULL; 
  this->InsertionAngleError = NULL;
  this->InsertionDepthError = NULL;
  
  
  this->ValidationNeedleActor = vtkActor::New();
  this->PlanActor = vtkActor::New();
  
  
  this->TimePerformanceFrame = NULL;
  this->CalibrationTime = NULL;
  this->PlanTime = NULL;
  this->InsertionTime = NULL;
  
  this->ClickNumber = 0;
  this->ProcessingCallback = false;

}


//----------------------------------------------------------------------------
vtkPerkStationValidateStep
::~vtkPerkStationValidateStep()
{
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->ResetFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->ResetValidationButton );
  
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->InsertionDepth );
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->ValidationErrorsFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->EntryPointError );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TargetPointError );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->InsertionAngleError );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->InsertionDepthError );
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TimePerformanceFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->CalibrationTime );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->PlanTime );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->InsertionTime );
  
  DELETE_IF_NOT_NULL( this->ValidationNeedleActor );
  DELETE_IF_NOT_NULL( this->PlanActor );
}


//----------------------------------------------------------------------------
void
vtkPerkStationValidateStep
::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);
  vtkKWWidget *parent = wizard_widget->GetClientArea();
  int enabled = parent->GetEnabled();
  
  
  this->ShowPlanListFrame();
  
  
    // clear controls
  
  FORGET( this->ResetFrame )
  FORGET( this->ResetValidationButton )
  
  
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
  
  this->Script( "pack %s -side top -padx 2 -pady 4", 
                this->ResetValidationButton->GetWidgetName() );
     
  
  this->SetDescription( "Mark actual entry point and target hit" );  

  
  // Create the individual components


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
    this->PlanListLabel = vtkKWLabel::New();
    }
  if ( ! this->PlanListFrame->IsCreated() )
    {
    this->PlanListFrame->SetParent( parent );
    this->PlanListFrame->Create();
    
    this->PlanListLabel->SetParent( this->PlanListFrame );
    this->PlanListLabel->Create();
    this->PlanListLabel->SetText( "Select plan to be validated. Then click on real \n"
                                  "needle tip and real needle entry point on the image." );
    }
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->PlanListFrame->GetWidgetName() );
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->PlanListLabel->GetWidgetName() );
  
  
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
  vtkMRMLPerkStationModuleNode* moduleNode = this->GetGUI()->GetMRMLNode();
  
    
  if ( this->ClickNumber > 1 ) return;  // Don't do anything after two clicks.
  
  
  vtkSlicerInteractorStyle *s = vtkSlicerInteractorStyle::SafeDownCast( caller );
  
  vtkSlicerInteractorStyle *istyle0 = vtkSlicerInteractorStyle::SafeDownCast(
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
    GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->
    GetInteractorStyle() );
  
  vtkRenderWindowInteractor *rwi;
  vtkRenderer *renderer = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetSliceViewer()->
                                GetRenderWidget()->GetOverlayRenderer();

  if (    ( s == istyle0 )
       && ( event == vtkCommand::LeftButtonPressEvent ) )
    {
    ++ this->ClickNumber;
    
      // mouse click happened in the axial slice view      
    vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(
         this->GetGUI()->GetApplicationGUI() )->GetMainSliceGUI( "Red" );
    rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();    
    
    vtkMatrix4x4 *xyToRas;
      xyToRas = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
    
    // capture the point
    int point[ 2 ];
    rwi->GetLastEventPosition( point );
    double inPt[ 4 ] = { point[ 0 ], point[ 1 ], 0, 1 };
    double outPt[ 4 ];
    
    xyToRas->MultiplyPoint( inPt, outPt ); 
    double ras[ 3 ] = { outPt[ 0 ], outPt[ 1 ], outPt[ 2 ] };
    
    
      // depending on click number, it is either Entry point or target point
    
    if ( this->ClickNumber == 1 )
      {
      moduleNode->SetValidationEntryPoint( ras );
      }
    else if ( this->ClickNumber == 2 )
      {
      moduleNode->SetValidationTargetPoint( ras );      
      moduleNode->SetValidated( true );
      
      this->OverlayValidationNeedleAxis();
      
      this->InsertionDepth->GetWidget()->SetValueAsDouble( moduleNode->GetValidationDepth() );
      }
    
    this->UpdateGUI();
    }
}



void
vtkPerkStationValidateStep
::OverlayValidationNeedleAxis()
{
  vtkSlicerSliceGUI* sliceGUI = vtkSlicerApplicationGUI::SafeDownCast( 
                                  this->GetGUI()->GetApplicationGUI() )->GetMainSliceGUI( "Red" );
  
  vtkRenderer* renderer = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer();
  
    // get the world coordinates
  
  double rasEntry[ 3 ];
  this->GetGUI()->GetMRMLNode()->GetValidationEntryPoint( rasEntry );
  
  double rasTarget[ 3 ];
  this->GetGUI()->GetMRMLNode()->GetValidationTargetPoint( rasTarget );
  
  
  double wcEntry[ 3 ];
  this->RasToWorld( rasEntry, wcEntry );
  
  double wcTarget[ 3 ];
  this->RasToWorld( rasTarget, wcTarget );
  
  
    // set up the line
  vtkSmartPointer< vtkLineSource > line = vtkSmartPointer< vtkLineSource >::New();
    line->SetPoint1( wcEntry );
    line->SetPoint2( wcTarget );
  
    // set up the mapper
  vtkSmartPointer< vtkPolyDataMapper > lineMapper = vtkSmartPointer< vtkPolyDataMapper >::New();
    lineMapper->SetInputConnection( line->GetOutputPort() );
  
  
  this->ValidationNeedleActor->SetMapper( lineMapper );
  this->ValidationNeedleActor->GetProperty()->SetColor( 255, 0, 255 );


    // add to actor collection
  sliceGUI->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->AddActor( this->ValidationNeedleActor );
  
  sliceGUI->GetSliceViewer()->RequestRender(); 
}



void
vtkPerkStationValidateStep
::OverlayPlan( bool visible )
{
  if ( ! visible )
    {
    this->PlanActor->SetVisibility( false );
    return;
    }
  
  
  vtkSlicerSliceGUI* sliceGUI = vtkSlicerApplicationGUI::SafeDownCast( 
                                  this->GetGUI()->GetApplicationGUI() )->GetMainSliceGUI( "Red" );
  
  vtkRenderer* renderer = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer();
  
  
    // Remove actor if already added.
  
  vtkActorCollection *collection = this->GetGUI()->GetApplicationGUI()->
    GetMainSliceGUI( "Red" )->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->GetActors();
  
  if ( collection->IsItemPresent( this->PlanActor ) )
    {
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
          GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->RemoveActor( this->PlanActor );
    }
  
  
    // Compute world coordinates.
  
  double rasEntry[ 3 ];
  this->GetGUI()->GetMRMLNode()->GetPlanEntryPoint( rasEntry );
  
  double rasTarget[ 3 ];
  this->GetGUI()->GetMRMLNode()->GetPlanTargetPoint( rasTarget );
  
  double wcEntry[ 3 ];
  this->RasToWorld( rasEntry, wcEntry );
  
  double wcTarget[ 3 ];
  this->RasToWorld( rasTarget, wcTarget );
  
  
    // set up the line
  vtkSmartPointer< vtkLineSource > line = vtkSmartPointer< vtkLineSource >::New();
    line->SetPoint1( wcEntry );
    line->SetPoint2( wcTarget );
    line->SetResolution( 100 );
  
    // set up the mapper
  vtkSmartPointer< vtkPolyDataMapper > lineMapper = vtkSmartPointer< vtkPolyDataMapper >::New();
    lineMapper->SetInputConnection( line->GetOutputPort() );
  
  
  this->PlanActor->SetMapper( lineMapper );
  this->PlanActor->GetProperty()->SetLineStipplePattern( 0xffff );
  this->PlanActor->GetProperty()->SetColor( 255, 50, 0 );
  this->PlanActor->SetVisibility( true );
  
  
    // add to actor collection and render
  sliceGUI->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->AddActor( this->PlanActor );
  sliceGUI->GetSliceViewer()->RequestRender();
}



//------------------------------------------------------------------------------
void vtkPerkStationValidateStep::RemoveValidationNeedleAxis()
{
    // should remove the overlay needle guide
  
  vtkActorCollection *collection = this->GetGUI()->GetApplicationGUI()->
    GetMainSliceGUI( "Red" )->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->GetActors();
  
  
  if ( collection->IsItemPresent( this->ValidationNeedleActor ) )
    {
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
          GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->RemoveActor( this->ValidationNeedleActor );
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetSliceViewer()->RequestRender();
    }
}



void
vtkPerkStationValidateStep
::OnMultiColumnListSelectionChanged()
{
  int numRows = this->PlanList->GetWidget()->GetNumberOfSelectedRows();
  if ( numRows != 1 ) return;
  
  vtkMRMLPerkStationModuleNode* moduleNode = this->GetGUI()->GetMRMLNode();
  
  int rowIndex = this->PlanList->GetWidget()->GetIndexOfFirstSelectedRow();
  moduleNode->SetCurrentPlanIndex( rowIndex );
  
  this->ClickNumber = 0;
  
  this->UpdateGUI();
}



//------------------------------------------------------------------------------
void
vtkPerkStationValidateStep
::Reset()
{
    // reset parameters of mrml node
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if ( ! mrmlNode ) return;
  
  mrmlNode->SetValidated( false );
  
    // remove the overlaid needle axis
  this->RemoveValidationNeedleAxis();

    // reset local member variables to defaults
  this->ClickNumber = 0;
  this->ProcessingCallback = false;
}



void
vtkPerkStationValidateStep
::UpdateGUI()
{
  vtkMRMLPerkStationModuleNode* mrmlNode = this->GetGUI()->GetMRMLNode();
  if ( ! mrmlNode ) return;
  
  
    // Update plan list. ------------------------------------------------------
  
  const int PRECISION_DIGITS = 1;
  
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
    
    if ( plan->GetValidated() )
      {
      for ( int i = 0; i < 3; ++ i )
        {
        std::ostrstream os;
        os << std::setiosflags( ios::fixed | ios::showpoint ) << std::setprecision( PRECISION_DIGITS );
        os << validationEntry[ i ] << std::ends;
        colList->SetCellText( row, PLAN_COL_ER + 6 + i, os.str() );
        os.rdbuf()->freeze();
        }
      for ( int i = 0; i < 3; ++ i )
        {
        std::ostrstream os;
        os << std::setiosflags( ios::fixed | ios::showpoint ) << std::setprecision( PRECISION_DIGITS );
        os << validationTarget[ i ] << std::ends;
        colList->SetCellText( row, PLAN_COL_ER + 9 + i, os.str() );
        os.rdbuf()->freeze();
        }
      } // if ( plan->GetValidated() )
    } // for ( int row = 0; row < numPlans; ++ row )
  
  this->PlanList->GetWidget()->SelectRow( mrmlNode->GetCurrentPlanIndex() );
  
  
    // Update error metric values. --------------------------------------------
  
  if ( mrmlNode->GetValidated() )
    {
    this->EntryPointError->GetWidget()->SetValueAsDouble( mrmlNode->GetEntryPointError() );
    this->TargetPointError->GetWidget()->SetValueAsDouble( mrmlNode->GetTargetPointError() );
    this->InsertionDepthError->GetWidget()->SetValueAsDouble( mrmlNode->GetDepthError() );
    }
  else
    {
    this->EntryPointError->GetWidget()->SetValue( "" );
    this->TargetPointError->GetWidget()->SetValue( "" );
    this->InsertionDepthError->GetWidget()->SetValue( "" );
    }
  
  
    // Time performance. ------------------------------------------------------
  
  char timeStr[ 20 ];
  long time = 0;
  
  time = mrmlNode->GetTimeOnCalibrateStep();
  sprintf( timeStr, "%02d:%02d", time/60, time%60 );
  this->CalibrationTime->GetWidget()->SetValue( timeStr );

  time = mrmlNode->GetTimeOnPlanStep();
  sprintf( timeStr, "%02d:%02d", time/60, time%60 );
  this->PlanTime->GetWidget()->SetValue( timeStr );

  time = mrmlNode->GetTimeOnInsertStep();
  sprintf( timeStr, "%02d:%02d", time/60, time%60 );
  this->InsertionTime->GetWidget()->SetValue( timeStr );
  
  
    // Second monitor. --------------------------------------------------------
  
  if ( mrmlNode->GetCurrentPlanIndex() >= 0 )
    {
    this->OverlayPlan( true );
    }
  else
    {
    this->OverlayPlan( false );
    }
  
  
    // Fiducials on first monitor. --------------------------------------------
  
  mrmlNode->GetPlanMRMLFiducialListNode()->RemoveAllFiducials();
  this->RemoveValidationNeedleAxis();
  
  if ( this->ClickNumber > 0  ||  mrmlNode->GetValidated() )
    {
    double point[ 3 ];
    mrmlNode->GetValidationEntryPoint( point );
    int index = mrmlNode->GetPlanMRMLFiducialListNode()->AddFiducialWithXYZ( point[ 0 ], point[ 1 ], point[ 2 ], 0 );
    mrmlNode->GetPlanMRMLFiducialListNode()->SetNthFiducialLabelText( index, "Entry" );
    }
  
  if ( this->ClickNumber > 1  ||  mrmlNode->GetValidated() )
    {
    double point[ 3 ];
    mrmlNode->GetValidationTargetPoint( point );
    int index = mrmlNode->GetPlanMRMLFiducialListNode()->AddFiducialWithXYZ( point[ 0 ], point[ 1 ], point[ 2 ], 0 );
    mrmlNode->GetPlanMRMLFiducialListNode()->SetNthFiducialLabelText( index, "Target" );
    this->OverlayValidationNeedleAxis();
    }
  
  mrmlNode->GetPlanMRMLFiducialListNode()->SetAllFiducialsVisibility( 1 );
}



void
vtkPerkStationValidateStep
::HideOverlays()
{
  this->RemoveValidationNeedleAxis();
  this->OverlayPlan( false );
}



void
vtkPerkStationValidateStep
::WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData )
{
    vtkPerkStationValidateStep *self = reinterpret_cast<vtkPerkStationValidateStep *>(clientData);
    if (self) { self->ProcessGUIEvents(caller, event, callData); }
}



void
vtkPerkStationValidateStep
::ProcessGUIEvents( vtkObject *caller, unsigned long event, void *callData )
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  
  if(    ! mrmlNode
      || ! mrmlNode->GetPlanningVolumeNode()
      || strcmp( mrmlNode->GetVolumeInUse(), "Validation" ) != 0 )
    {
    return;
    }

  
  if ( this->ProcessingCallback ) return;
  this->ProcessingCallback = true;
  
  
    // reset plan button
  
  if (    this->ResetValidationButton
       && this->ResetValidationButton == vtkKWPushButton::SafeDownCast( caller )
       && ( event == vtkKWPushButton::InvokedEvent ) )
    {
    this->Reset();
    }
  
  
  this->UpdateGUI();
  
  this->ProcessingCallback = false;
}



void
vtkPerkStationValidateStep
::RasToWorld( const double ras[ 3 ], double wc[ 3 ] )
{
  vtkSlicerSliceGUI* sliceGUI =
    vtkSlicerApplicationGUI::SafeDownCast( this->GetGUI()->GetApplicationGUI() )->GetMainSliceGUI( "Red" );
  vtkRenderer* renderer = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer();
  
  vtkMatrix4x4 *xyToRAS = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
  vtkMatrix4x4 *rasToXY = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert( xyToRAS, rasToXY );
  
  double inPt[ 4 ] = { ras[ 0 ], ras[ 1 ], ras[ 2 ], 1 };
  double outPt[ 4 ];  
  rasToXY->MultiplyPoint( inPt, outPt );
  
  double wc4[ 4 ];
  renderer->SetDisplayPoint( outPt[ 0 ], outPt[ 1 ], 0 );
  renderer->DisplayToWorld();
  renderer->GetWorldPoint( wc4 );
  
  for ( int i = 0; i < 3; ++ i ) wc[ i ] = wc4[ i ];
}
