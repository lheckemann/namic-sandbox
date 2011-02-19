#include "vtkPerkStationPlanStep.h"

#include "PerkStationCommon.h"
#include "vtkPerkStationModuleGUI.h"
#include "vtkPerkStationModuleLogic.h"
#include "vtkPerkStationSecondaryMonitor.h"

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

#include "vtkMRMLFiducialListNode.h"

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


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationPlanStep);
vtkCxxRevisionMacro(vtkPerkStationPlanStep, "$Revision: 1.1 $");


//----------------------------------------------------------------------------
vtkPerkStationPlanStep::vtkPerkStationPlanStep()
{
  this->SetName("2/5. Plan");
  this->SetDescription("Plan the needle insertion");
  
  
  this->WizardGUICallbackCommand->SetCallback( vtkPerkStationPlanStep::WizardGUICallback );
  
  this->GUICallbackCommand = vtkCallbackCommand::New();
  this->GUICallbackCommand->SetClientData( reinterpret_cast<void *>(this) );
  this->GUICallbackCommand->SetCallback( &vtkPerkStationPlanStep::GUICallback );
  
  this->TargetFirstFrame = NULL;
  this->TargetFirstCheck = NULL;
  
  this->TiltInformationFrame = NULL;
  this->SystemTiltAngle = NULL;
  
  this->PlanningLineActor = vtkSmartPointer< vtkActor >::New();
    this->PlanningLineActor->SetVisibility( 0 );
    this->PlanningLineActor->GetProperty()->SetColor( 1.0, 0.0, 1.0 );
  
  
    // PlanList frame
  
  this->PlanListFrame = NULL;
  this->PlanList = NULL;
  this->DeleteButton = NULL;
  
  
  this->WCEntryPoint[ 0 ] = 0.0;
  this->WCEntryPoint[ 1 ] = 0.0;
  this->WCEntryPoint[ 2 ] = 0.0;
  this->WCTargetPoint[ 0 ] = 0.0;
  this->WCTargetPoint[ 1 ] = 0.0;
  this->WCTargetPoint[ 2 ] = 0.0;

  this->ProcessingCallback = false;
  
  this->NeedleActor = vtkActor::New(); 
  
  
    // Initail state.
  
  this->NumPointsSelected = 0;
  this->SelectTargetFirst = true;
}



//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::GUICallback( vtkObject *caller,
                           unsigned long eid, void *clientData, void *callData )
{

  vtkPerkStationPlanStep *self = reinterpret_cast< vtkPerkStationPlanStep* >( clientData );
  
  
  if ( self->GetInGUICallbackFlag() )
    {
    }
  
  
  vtkDebugWithObjectMacro(self, "In vtkProstateNavStep GUICallback");
  
  self->SetInGUICallbackFlag( 1 );
  self->ProcessGUIEvents( caller, eid, callData );
  self->SetInGUICallbackFlag( 0 );
  
}


//----------------------------------------------------------------------------
vtkPerkStationPlanStep::~vtkPerkStationPlanStep()
{
  this->RemoveGUIObservers();
  
  DELETE_IF_NOT_NULL( this->GUICallbackCommand );
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TargetFirstFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TargetFirstCheck );
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TiltInformationFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->SystemTiltAngle );
  
    // PlanList frame
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->PlanListFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->PlanList );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->DeleteButton );
  
  if ( this->NeedleActor )
    {
    this->NeedleActor->Delete();
    this->NeedleActor = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();
  
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);
  vtkKWWidget *parent = wizard_widget->GetClientArea();
  int enabled = parent->GetEnabled();
  
  this->SetDescription( "Plan the needle insertion" );
  
  
  vtkSlicerApplicationGUI::SafeDownCast( this->GetGUI()->GetApplicationGUI() )->
    GetMainSliceGUI( "Red" )->GetSliceViewer()->GetRenderWidget()->
    GetOverlayRenderer()->AddActor( this->PlanningLineActor );
  
  
  // clear controls
  FORGET( this->TargetFirstCheck );
  
  this->SetName("2/4. Plan");
  this->GetGUI()->GetWizardWidget()->Update();

  
  this->ShowTargetFirstFrame();
  
  
   //frame
  if ( ! this->TiltInformationFrame )
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

  
  this->ShowPlanListFrame();
  
  
  this->InstallCallbacks();
  this->AddGUIObservers();
  
  this->UpdateGUI();
}


void
vtkPerkStationPlanStep
::ShowTargetFirstFrame()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
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
}


void
vtkPerkStationPlanStep
::ShowPlanListFrame()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  
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
    this->PlanList->GetWidget()->ColumnEditableOn( 0 );  // Name column.
    this->PlanList->GetWidget()->SetColumnEditWindowToEntry( 0 );
    }
  
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->PlanList->GetWidgetName() );
  
  
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


void
vtkPerkStationPlanStep
::HideUserInterface()
{
  Superclass::HideUserInterface();
  
  vtkSlicerApplicationGUI* app = vtkSlicerApplicationGUI::SafeDownCast( this->GetGUI()->GetApplicationGUI() );
  vtkRenderer* renderer = app->GetMainSliceGUI( "Red" )->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer();
  
  if ( renderer->GetActors()->IsItemPresent( this->PlanningLineActor ) )
    {
    renderer->RemoveActor( this->PlanningLineActor );
    }
  
  this->RemoveGUIObservers();
}


void
vtkPerkStationPlanStep
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
    if (    deleteFlag
         || plan->GetName().compare( this->PlanList->GetWidget()->GetCellText( row, PLAN_COL_NAME ) ) != 0 )
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
        }
      }
    } // for ( int row = 0; row < numPlans; ++ row )
    
  this->PlanList->GetWidget()->SelectRow( mrmlNode->GetCurrentPlanIndex() );
}



void
vtkPerkStationPlanStep
::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf(os,indent);
}



void
vtkPerkStationPlanStep
::AddGUIObservers()
{
  this->RemoveGUIObservers();
  
  if ( this->PlanList )
    {
    this->PlanList->GetWidget()->SetCellUpdatedCommand( this, "OnMultiColumnListUpdate" );
    this->PlanList->GetWidget()->SetSelectionChangedCommand( this, "OnMultiColumnListSelectionChanged" );
    }
  
  if ( this->DeleteButton )
    {
    this->DeleteButton->AddObserver( vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand );
    }
}



void
vtkPerkStationPlanStep
::RemoveGUIObservers()
{
  if ( this->PlanList )
    {
    this->PlanList->GetWidget()->SetCellUpdatedCommand( this, "" );
    this->PlanList->GetWidget()->SetSelectionChangedCommand( this, "" );
    }
  
  if ( this->DeleteButton )
    {
    this->DeleteButton->RemoveObservers( vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand );
    }
}


//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::InstallCallbacks()
{
  
}


void
vtkPerkStationPlanStep
::ProcessImageClickEvents( vtkObject *caller, unsigned long event, void *callData )
{
  
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  
  
    // Return, if current step is not Planning.
  
  if (    ! wizard_widget
       || wizard_widget->GetWizardWorkflow()->GetCurrentStep() != this
       || ! this->GetGUI()->GetMRMLNode()
       || strcmp( this->GetGUI()->GetMRMLNode()->GetVolumeInUse(),
                  "Planning" )!= 0 )
    {
    return;
    }
  
  
    // Planning has to happen on slicer laptop, not secondary monitor
  
  vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast( caller );
  vtkSlicerSliceGUI* sliceGUI = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red");
  
  vtkSlicerInteractorStyle *istyle0 =
    vtkSlicerInteractorStyle::SafeDownCast( sliceGUI->GetSliceViewer()->
      GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle() );
  
  if (    ( style != istyle0 )
       || ( event != vtkCommand::LeftButtonPressEvent ) )
    {
    return;
    }
  
  
    // Get the RAS position of the image click.
  
  vtkRenderWindowInteractor* rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->
                                   GetRenderWindowInteractor();    
  
  vtkMatrix4x4* matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
  
  int point[ 2 ];
  rwi->GetLastEventPosition( point );
  double inPt[ 4 ] = { point[ 0 ], point[ 1 ], 0.0, 1.0 };
  double outPt[ 4 ];    
  matrix->MultiplyPoint( inPt, outPt ); 
  double ras[ 3 ] = { outPt[ 0 ], outPt[ 1 ], outPt[ 2 ] };
  
  
    // depending on click number, it is either Entry point or Target point
  
  ++ this->NumPointsSelected;
  
  if ( this->NumPointsSelected == 3 ) // Starting a new plan.
    {
    this->NumPointsSelected = 1;
    this->GetGUI()->TwoFiducials->SetAllFiducialsVisibility( 0 );
    this->GetGUI()->EntryActor->SetVisibility( 0 );
    this->GetGUI()->TargetActor->SetVisibility( 1 );
    this->RemoveOverlayNeedleGuide();
    }
  
  
  if ( this->NumPointsSelected == 1 )  // On first click.
    {   
    this->GetGUI()->GetMRMLNode()->AddNewPlan();
    this->TargetFirstCheck->GetWidget()->SetEnabled( 0 );
    }
  
  
    // Determine which point is to be selected first.
  
  int entryClick = 1;
  int targetClick = 2;
  
  if ( this->TargetFirstCheck->GetWidget()->GetSelectedState() != 0 )
    {
    targetClick = 1;
    entryClick = 2;
    }
  
  
    // Process the new selected point.
  
  if ( this->NumPointsSelected == entryClick )
    {
      // record value in mrml node
    this->GetGUI()->GetMRMLNode()->SetPlanEntryPoint( ras );
    this->GetGUI()->TwoFiducials->SetNthFiducialXYZ( 0, ras[ 0 ], ras[ 1 ], ras[ 2 ] );
    this->GetGUI()->TwoFiducials->SetNthFiducialVisibility( 0, 1 );
    }
  else if ( this->NumPointsSelected == targetClick )
    {
      // record value in the MRML node
    this->GetGUI()->GetMRMLNode()->SetPlanTargetPoint( ras );
    this->GetGUI()->TwoFiducials->SetNthFiducialXYZ( 1, ras[ 0 ], ras[ 1 ], ras[ 2 ] );
    this->GetGUI()->TwoFiducials->SetNthFiducialVisibility( 1, 1 );
    }
  
  
  if ( this->NumPointsSelected == 2 ) // Needle guide ready.
    {
    this->OverlayNeedleGuide();
    this->GetGUI()->GetSecondaryMonitor()->OverlayNeedleGuide();  
    
    this->TargetFirstCheck->GetWidget()->SetEnabled( 1 );
    
    this->UpdateGUI();
    }
  
  if ( this->NumPointsSelected != 1 )
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
  if ( this->NumPointsSelected != 1 )
    {
    this->PlanningLineActor->SetVisibility( 0 );
    return;
    }
  
  this->PlanningLineActor->SetVisibility( 1 );
  
  
  vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(
    this->GetGUI()->GetApplicationGUI() )->GetMainSliceGUI( "Red" );
  
  vtkRenderWindowInteractor *rwi;
  rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();    
  
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
  vtkSmartPointer< vtkMatrix4x4 > rasToXY = vtkSmartPointer< vtkMatrix4x4 >::New();
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
  
  vtkRenderer *renderer = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer();
  
  double wc[ 4 ];
  renderer->SetDisplayPoint( point[ 0 ], point[ 1 ], 0 );
  renderer->DisplayToWorld();
  renderer->GetWorldPoint( wc );
  double endPointWC[ 3 ] = { wc[ 0 ], wc[ 1 ], wc[ 2 ] };
  
  renderer->SetDisplayPoint( entryPointXY[ 0 ], entryPointXY[ 1 ], 0.0 );
  renderer->DisplayToWorld();
  renderer->GetWorldPoint( wc );
  double startPointWC[ 3 ] = { wc[ 0 ], wc[ 1 ], wc[ 2 ] };
  
  vtkSmartPointer< vtkLineSource > lineSource = vtkSmartPointer< vtkLineSource >::New();
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



void
vtkPerkStationPlanStep
::OnMultiColumnListUpdate(int row, int col, char * str)
{
    // Make sure the row and col exists.
  if (    ( row < 0 ) || ( row >= this->PlanList->GetWidget()->GetNumberOfRows() )
       || ( row < 0 ) || ( row >= this->PlanList->GetWidget()->GetNumberOfRows() ) )
    {
    return;
    }
  
  bool updated = false;
  
  if ( col == PLAN_COL_NAME )
    {
    vtkPerkStationPlan* plan = this->GetGUI()->GetMRMLNode()->GetPlanAtIndex( row );
    plan->SetName( std::string( str ) );
    updated = true;
    }
  
  if ( updated )
    {
    this->UpdateGUI();
    }
}


void
vtkPerkStationPlanStep
::OnMultiColumnListSelectionChanged()
{
  int numRows = this->PlanList->GetWidget()->GetNumberOfSelectedRows();
  
  if ( numRows != 1 ) return;
  
  vtkMRMLPerkStationModuleNode* moduleNode = this->GetGUI()->GetMRMLNode();
  
  int rowIndex = this->PlanList->GetWidget()->GetIndexOfFirstSelectedRow();
  vtkPerkStationPlan* plan = moduleNode->GetPlanAtIndex( rowIndex );
  
  moduleNode->SetCurrentPlanIndex( rowIndex );
  
  
  this->NumPointsSelected = 2;
  this->RemoveOverlayNeedleGuide();
  
  double point[ 3 ];
  
  plan->GetEntryPointRAS( point );
  this->GetGUI()->TwoFiducials->SetNthFiducialXYZ( 0, point[ 0 ], point[ 1 ], point[ 2 ] );
  moduleNode->SetPlanEntryPoint( point );
  // this->GetGUI()->SetEntryPosition( point[ 0 ], point[ 1 ] );
  // this->GetGUI()->EntryActor->SetVisibility( 1 );
  
  plan->GetTargetPointRAS( point );
  this->GetGUI()->TwoFiducials->SetNthFiducialXYZ( 1, point[ 0 ], point[ 1 ], point[ 2 ] );
  moduleNode->SetPlanTargetPoint( point );
  // this->GetGUI()->SetTargetPosition( point[ 0 ], point[ 1 ] );
  // this->GetGUI()->TargetActor->SetVisibility( 1 );
  
  this->GetGUI()->TwoFiducials->SetAllFiducialsVisibility( 1 );
  
  
  moduleNode->SetCurrentSliceOffset( point[ 2 ] );
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetLogic()->SetSliceOffset( moduleNode->GetCurrentSliceOffset() );
  this->OverlayNeedleGuide();
}



void
vtkPerkStationPlanStep
::OnSliceOffsetChanged( double offset )
{
  vtkMRMLPerkStationModuleNode* node = this->GetGUI()->GetMRMLNode();
  if ( node == NULL ) return;
  
  vtkPerkStationPlan* plan = node->GetPlanAtIndex( node->GetCurrentPlanIndex() );
  if ( plan == NULL ) return;
  
  
  double* target = plan->GetTargetPointRAS();
  
  if ( target[ 2 ] >= ( offset - 0.5 )  &&  target[ 2 ] < ( offset + 0.5 ) )
    {
    this->OverlayNeedleGuide();
    }
  else
    {
    this->RemoveOverlayNeedleGuide();
    }
}



void
vtkPerkStationPlanStep
::OverlayNeedleGuide()
{
  vtkRenderer *renderer = this->GetGUI()->GetApplicationGUI()->
    GetMainSliceGUI( "Red" )->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer();

  
    // get the world coordinates of Entry and Target points.
  
  vtkSlicerSliceGUI *sliceGUI =
    vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI() )->GetMainSliceGUI( "Red" );
  vtkMatrix4x4 *xyToRAS = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
  vtkSmartPointer< vtkMatrix4x4 > rasToXY = vtkSmartPointer< vtkMatrix4x4 >::New();
  vtkMatrix4x4::Invert( xyToRAS, rasToXY );
  
  int entryPointXY[ 2 ];
  int targetPointXY[ 2 ];
  double worldCoordinate[ 4 ];
  
  
  vtkMRMLPerkStationModuleNode* moduleNode = this->GetGUI()->GetMRMLNode();
  
  
    // entry point
  
  double rasEntry[ 3 ];
  moduleNode->GetPlanEntryPoint( rasEntry );
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
  moduleNode->GetPlanTargetPoint( rasTarget );
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
  
  vtkSmartPointer< vtkCylinderSource > needleGuide = vtkSmartPointer< vtkCylinderSource >::New();
  
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
  
  
  vtkSmartPointer< vtkTransform > transform = vtkSmartPointer< vtkTransform >::New();
    transform->Translate( needleCenter[ 0 ], needleCenter[ 1 ], 0.0 );
    transform->RotateZ( angle );
  
  vtkSmartPointer< vtkTransformPolyDataFilter > filter =
      vtkSmartPointer< vtkTransformPolyDataFilter >::New();
    filter->SetInputConnection( needleGuide->GetOutputPort() );
    filter->SetTransform( transform );
  
  vtkSmartPointer< vtkPolyDataMapper > needleMapper = vtkSmartPointer< vtkPolyDataMapper >::New();
    needleMapper->SetInputConnection( filter->GetOutputPort() );
  
  this->NeedleActor->SetMapper( needleMapper );  
  this->NeedleActor->GetProperty()->SetOpacity( 0.3 );
  
  // add to renderer of the Axial slice viewer
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
    GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->AddActor( this->NeedleActor );
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetSliceViewer()->RequestRender(); 
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
  if ( ! mrmlNode ) return;
  
  
  double ras[ 3 ] = { 0.0, 0.0, 0.0 };
  mrmlNode->SetPlanEntryPoint( ras );
  mrmlNode->SetPlanTargetPoint( ras );
  
  
  double tiltAngle = mrmlNode->GetTiltAngle();

  if ( ! vtkPerkStationModuleLogic::DoubleEqual( tiltAngle, 0 ) )
    {
    // TODO: Reset slice angle.
    }
  
  
    // set tilt angle back to zero
  mrmlNode->SetTiltAngle( 0 );
  
  this->GetGUI()->TwoFiducials->SetAllFiducialsVisibility( 0 );
  
  this->GetGUI()->EntryActor->SetVisibility( 0 );
  this->GetGUI()->TargetActor->SetVisibility( 0 );
  
  // reset local member variables to defaults
  this->WCEntryPoint[0] = 0.0;
  this->WCEntryPoint[1] = 0.0;
  this->WCEntryPoint[2] = 0.0;
  this->WCTargetPoint[0] = 0.0;
  this->WCTargetPoint[1] = 0.0;
  this->WCTargetPoint[2] = 0.0;
  this->NumPointsSelected = 0;
  this->ProcessingCallback = false;
}



void
vtkPerkStationPlanStep
::RemoveOverlayNeedleGuide()
{
    // should remove the overlay needle guide
  vtkActorCollection *collection = this->GetGUI()->GetApplicationGUI()->
      GetMainSliceGUI( "Red" )->GetSliceViewer()->GetRenderWidget()->
      GetOverlayRenderer()->GetActors();
  
  if ( collection->IsItemPresent( this->NeedleActor ) )
    {
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
          GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->RemoveActor( this->NeedleActor );
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetSliceViewer()->RequestRender();
    }
}



void
vtkPerkStationPlanStep
::WizardGUICallback( vtkObject* caller, unsigned long event, void* clientData, void* callData )
{
    vtkPerkStationPlanStep *self = reinterpret_cast< vtkPerkStationPlanStep* >( clientData );
    if ( self ) self->ProcessGUIEvents( caller, event, callData );
}



void
vtkPerkStationPlanStep
::ProcessGUIEvents( vtkObject* caller, unsigned long event, void* callData )
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

  
    // Delete selected plan.
  
  if ( this->DeleteButton == vtkKWPushButton::SafeDownCast( caller )
       && event == vtkKWPushButton::InvokedEvent )
    {
    vtkPerkStationPlan* plan = mrmlNode->GetPlanAtIndex( mrmlNode->GetCurrentPlanIndex() );
    if ( plan != NULL )
      {
      mrmlNode->RemovePlanAtIndex( mrmlNode->GetCurrentPlanIndex() );
      this->GetGUI()->TwoFiducials->SetAllFiducialsVisibility( 0 );
      
      this->GetGUI()->EntryActor->SetVisibility( 0 );
      this->GetGUI()->TargetActor->SetVisibility( 0 );
      
      this->RemoveOverlayNeedleGuide();
      mrmlNode->SetCurrentPlanIndex( - 1 );
      this->UpdateGUI();
      }
    }
  
  this->ProcessingCallback = false;
}


//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::Validate()
{
  this->Superclass::Validate();
  
  // depth perception lines render
  // this->GetGUI()->GetSecondaryMonitor()->SetDepthPerceptionLines();
}

