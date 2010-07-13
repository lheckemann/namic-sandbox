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



vtkStandardNewMacro( vtkPerkStationValidateStep );
vtkCxxRevisionMacro( vtkPerkStationValidateStep, "$Revision: 1.0 $" );



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



void
vtkPerkStationValidateStep
::InstallCallbacks()
{
    // Configure the OK button to start
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  
  this->AddGUIObservers();
}



vtkPerkStationValidateStep
::vtkPerkStationValidateStep()
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
  
  
  this->ClickNumber = 0;
  this->ProcessingCallback = false;

}



vtkPerkStationValidateStep
::~vtkPerkStationValidateStep()
{
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->PlanListFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->PlanListLabel );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->PlanList );
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->ResetFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->ResetValidationButton );
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->InsertionDepth );
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->ValidationErrorsFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->EntryPointError );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TargetPointError );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->InsertionAngleError );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->InsertionDepthError );
  
  DELETE_IF_NOT_NULL( this->ValidationNeedleActor );
  DELETE_IF_NOT_NULL( this->PlanActor );
}



void
vtkPerkStationValidateStep
::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled( 0 );
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
  
  if ( ! this->ResetFrame )
    {
    this->ResetFrame = vtkKWFrame::New();
    }
  if ( ! this->ResetFrame->IsCreated() )
    {
    this->ResetFrame->SetParent( parent );
    this->ResetFrame->Create();     
    }
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", this->ResetFrame->GetWidgetName() );
  
  
  if ( ! this->ResetValidationButton )
    {
    this->ResetValidationButton = vtkKWPushButton::New();
    }
  if( ! this->ResetValidationButton->IsCreated() )
    {
    this->ResetValidationButton->SetParent( this->ResetFrame );
    this->ResetValidationButton->SetText( "Reset validation" );
    this->ResetValidationButton->SetBorderWidth( 2 );
    this->ResetValidationButton->SetReliefToRaised();      
    this->ResetValidationButton->SetHighlightThickness( 2 );
    this->ResetValidationButton->SetImageToPredefinedIcon( vtkKWIcon::IconTrashcan );
    this->ResetValidationButton->Create();
    }
  this->Script( "pack %s -side top -padx 2 -pady 4", this->ResetValidationButton->GetWidgetName() );
     
  
  this->SetDescription( "Mark actual entry point and target hit" );  

  
  // Create the individual components


   // insertion depth  
  if ( ! this->InsertionDepth )
    {
    this->InsertionDepth =  vtkKWEntryWithLabel::New(); 
    }
  if ( ! this->InsertionDepth->IsCreated() )
    {
    this->InsertionDepth->SetParent( parent );
    this->InsertionDepth->Create();
    this->InsertionDepth->GetWidget()->SetRestrictValueToDouble();
    this->InsertionDepth->SetLabelText( "Planned insertion depth (mm):" );
    }
  this->Script( "pack %s -side top -anchor nw -padx 2 -pady 2", this->InsertionDepth->GetWidgetName() );
  
  
  if ( ! this->ValidationErrorsFrame )
    {
    this->ValidationErrorsFrame = vtkKWFrameWithLabel::New();
    }
  if ( ! this->ValidationErrorsFrame->IsCreated() )
    {
    this->ValidationErrorsFrame->SetParent( parent );
    this->ValidationErrorsFrame->Create();
    this->ValidationErrorsFrame->SetLabelText( "Evaluation" );
    this->ValidationErrorsFrame->SetBalloonHelpString( "" );
    }
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->ValidationErrorsFrame->GetWidgetName() );



  // entry point error
  if ( ! this->EntryPointError )
    {
    this->EntryPointError =  vtkKWEntryWithLabel::New();  
    }
  if ( ! this->EntryPointError->IsCreated() )
    {
    this->EntryPointError->SetParent( this->ValidationErrorsFrame->GetFrame() );
    this->EntryPointError->Create();
    this->EntryPointError->GetWidget()->SetRestrictValueToDouble();
    this->EntryPointError->SetLabelText( "Error in entry point (mm):" );
    this->EntryPointError->SetWidth(7);
    this->EntryPointError->GetWidget()->ReadOnlyOn();
    }

  this->Script( "pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->EntryPointError->GetWidgetName() );


    // target point error
  
  if ( ! this->TargetPointError )
    {
    this->TargetPointError =  vtkKWEntryWithLabel::New();  
    }
  if ( ! this->TargetPointError->IsCreated() )
    {
    this->TargetPointError->SetParent( this->ValidationErrorsFrame->GetFrame() );
    this->TargetPointError->Create();
    this->TargetPointError->GetWidget()->SetRestrictValueToDouble();
    this->TargetPointError->SetLabelText( "Error in target point (mm):" );
    this->TargetPointError->SetWidth( 7 );
    this->TargetPointError->GetWidget()->ReadOnlyOn();
    }
  this->Script( "pack %s -side top -anchor nw -padx 2 -pady 2", this->TargetPointError->GetWidgetName() );

 

    // entry point error
  
  if ( ! this->InsertionDepthError )
    {
    this->InsertionDepthError =  vtkKWEntryWithLabel::New();  
    }
  if ( ! this->InsertionDepthError->IsCreated() )
    {
    this->InsertionDepthError->SetParent( this->ValidationErrorsFrame->GetFrame() );
    this->InsertionDepthError->Create();
    this->InsertionDepthError->GetWidget()->SetRestrictValueToDouble();
    this->InsertionDepthError->SetLabelText( "Error in insertion depth (mm):" );
    this->InsertionDepthError->SetWidth( 7 );
    this->InsertionDepthError->GetWidget()->ReadOnlyOn();
    }
  this->Script( "pack %s -side top -anchor nw -padx 2 -pady 2", this->InsertionDepthError->GetWidgetName() );


    // target point error
  
  if ( ! this->InsertionAngleError )
    {
    this->InsertionAngleError =  vtkKWEntryWithLabel::New();  
    }
  if ( ! this->InsertionAngleError->IsCreated() )
    {
    this->InsertionAngleError->SetParent(this->ValidationErrorsFrame->GetFrame());
    this->InsertionAngleError->Create();
    this->InsertionAngleError->GetWidget()->SetRestrictValueToDouble();
    this->InsertionAngleError->SetLabelText( "Error in-plane angle (degrees):" );
    this->InsertionAngleError->SetWidth( 7 );
    this->InsertionAngleError->GetWidget()->ReadOnlyOn();
    }
  this->Script( "pack %s -side top -anchor nw -padx 2 -pady 2", this->InsertionAngleError->GetWidgetName() );
  
  
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
                                  "needle entry point and real needle tip on the image." );
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



void
vtkPerkStationValidateStep
::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os,indent );
}



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



void
vtkPerkStationValidateStep
::RemoveValidationNeedleAxis()
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
  
  
  vtkPerkStationPlan* plan = moduleNode->GetPlanAtIndex( rowIndex );
  double point[ 3 ] = { 0, 0, 0 };
  plan->GetTargetPointRAS( point );
  moduleNode->SetCurrentSliceOffset( point[ 2 ] );
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetLogic()->SetSliceOffset(
    moduleNode->GetCurrentSliceOffset() );
  
  this->ClickNumber = 0;
  
  this->UpdateGUI();
}



void
vtkPerkStationValidateStep
::OnSliceOffsetChanged( double offset )
{
  vtkMRMLPerkStationModuleNode* node = this->GetGUI()->GetMRMLNode();
  if ( ! node  ||  node->GetCurrentPlanIndex() < 0 ) return;
  
  
  vtkPerkStationPlan* plan = node->GetPlanAtIndex( node->GetCurrentPlanIndex() );
  double* target = plan->GetTargetPointRAS();
  
  if ( target[ 2 ] >= ( offset - 0.5 )  &&  target[ 2 ] < ( offset + 0.5 ) )
    {
    this->PlanActor->SetVisibility( 1 );
    }
  else
    {
    this->PlanActor->SetVisibility( 0 );
    }
}



void
vtkPerkStationValidateStep
::Reset()
{
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
  
  
    // Plan depth.
 
  if ( mrmlNode->GetCurrentPlanIndex() >= 0 )
    {
    this->InsertionDepth->GetWidget()->SetValueAsDouble( mrmlNode->GetActualPlanInsertionDepth() );
    }
  else
    {
    this->InsertionDepth->GetWidget()->SetValue( "" );
    }

  
    // Update error metric values. --------------------------------------------
  
  if ( mrmlNode->GetValidated() )
    {
    this->EntryPointError->GetWidget()->SetValueAsDouble( mrmlNode->GetEntryPointError() );
    this->TargetPointError->GetWidget()->SetValueAsDouble( mrmlNode->GetTargetPointError() );
    this->InsertionDepthError->GetWidget()->SetValueAsDouble( mrmlNode->GetDepthError() );
    this->InsertionAngleError->GetWidget()->SetValueAsDouble( mrmlNode->GetAngleError() );
    }
  else
    {
    this->EntryPointError->GetWidget()->SetValue( "" );
    this->TargetPointError->GetWidget()->SetValue( "" );
    this->InsertionDepthError->GetWidget()->SetValue( "" );
    this->InsertionAngleError->GetWidget()->SetValue( "" );
    }
  
  
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
  vtkSmartPointer< vtkMatrix4x4 > rasToXY = vtkSmartPointer< vtkMatrix4x4 >::New();
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
