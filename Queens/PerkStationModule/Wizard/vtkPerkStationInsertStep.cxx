#include "vtkPerkStationInsertStep.h"

#include "vtkPerkStationModuleGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkPerkStationModuleLogic.h"
#include "vtkPerkStationSecondaryMonitor.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWApplication.h"
#include "vtkKWCheckButton.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWEntrySet.h"
#include "vtkKWLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWLoadSaveButton.h"

#include <stdio.h>

#include "PerkStationCommon.h"



vtkStandardNewMacro( vtkPerkStationInsertStep );
vtkCxxRevisionMacro( vtkPerkStationInsertStep, "$Revision: 1.1 $" );



vtkPerkStationInsertStep
::vtkPerkStationInsertStep()
{
  this->SetName("3/5. Insert");
  this->SetDescription("Do the needle insertion");
  
  this->WizardGUICallbackCommand->SetCallback(vtkPerkStationInsertStep::WizardGUICallback);
  
  this->AngleInPlaneLabel = NULL;
  
  this->PlanList = NULL;
  this->PlanListFrame = NULL;
  
  this->CalibrationList = NULL;
  this->CalibrationListFrame = NULL;
  
  
  this->ProcessingCallback = false;
}



vtkPerkStationInsertStep
::~vtkPerkStationInsertStep()
{
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->AngleInPlaneLabel );
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->PlanList );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->PlanListFrame );
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->CalibrationList );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->CalibrationListFrame );
}



void vtkPerkStationInsertStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  // in clinical mode
  this->SetName("3/4. Insert");
  this->GetGUI()->GetWizardWidget()->Update();
  
  this->SetDescription("Select plan and insert needle.");
  
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);
  vtkKWWidget *parent = wizard_widget->GetClientArea();
  
  
    // Angle in plane.
  
  
  if ( ! this->AngleInPlaneLabel )
    {
    this->AngleInPlaneLabel = vtkKWLabel::New();
    }
  if ( ! this->AngleInPlaneLabel->IsCreated() )
    {
    this->AngleInPlaneLabel->SetParent( parent );
    this->AngleInPlaneLabel->Create();
    this->AngleInPlaneLabel->SetText( "In plane angle: " );
    }
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->AngleInPlaneLabel->GetWidgetName() );
  
  
    // Plan list.
  
  
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
                this->PlanList->GetWidgetName());
  
  
    // Calibration list.
  
  if ( ! this->CalibrationListFrame )
    {
    this->CalibrationListFrame = vtkKWFrame::New();
    }
  if ( ! this->CalibrationListFrame->IsCreated() )
    {
    this->CalibrationListFrame->SetParent( parent );
    this->CalibrationListFrame->Create();
    }
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->CalibrationListFrame->GetWidgetName() );
  
  
  if ( ! this->CalibrationList )
    {
    this->CalibrationList = vtkKWMultiColumnListWithScrollbars::New();
    this->CalibrationList->SetParent( this->CalibrationListFrame );
    this->CalibrationList->Create();
    this->CalibrationList->SetHeight( 1 );
    this->CalibrationList->GetWidget()->SetSelectionTypeToRow();
    this->CalibrationList->GetWidget()->SetSelectionBackgroundColor( 1, 0, 0 );
    this->CalibrationList->GetWidget()->MovableRowsOff();
    this->CalibrationList->GetWidget()->MovableColumnsOff();
    
      // Create the columns.
    
    for ( int col = 0; col < CALIBRATION_COL_COUNT; ++ col )
      {
      this->CalibrationList->GetWidget()->AddColumn( CALIBRATION_COL_LABELS[ col ] );
      this->CalibrationList->GetWidget()->SetColumnWidth( col, CALIBRATION_COL_WIDTHS[ col ] );
      this->CalibrationList->GetWidget()->SetColumnAlignmentToLeft( col );
      }
    }
  
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->CalibrationList->GetWidgetName());
  
  
    // TO DO: install callbacks
  this->InstallCallbacks();

  this->AddGUIObservers();

  this->UpdateGUI();
}



void
vtkPerkStationInsertStep
::HideUserInterface()
{
  Superclass::HideUserInterface();
}



void
vtkPerkStationInsertStep
::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf(os,indent);
}



void
vtkPerkStationInsertStep
::OnMultiColumnListSelectionChanged()
{
  int numRows = this->PlanList->GetWidget()->GetNumberOfSelectedRows();
  
  if ( numRows != 1 ) return;
  
  vtkMRMLPerkStationModuleNode* moduleNode = this->GetGUI()->GetMRMLNode();
  
  int rowIndex = this->PlanList->GetWidget()->GetIndexOfFirstSelectedRow();
  vtkPerkStationPlan* plan = moduleNode->GetPlanAtIndex( rowIndex );
  
  moduleNode->SetCurrentPlanIndex( rowIndex );
  
  
  moduleNode->GetPlanMRMLFiducialListNode()->RemoveAllFiducials();
  
  double point[ 3 ];
  
  plan->GetEntryPointRAS( point );
  int ind = moduleNode->GetPlanMRMLFiducialListNode()->AddFiducialWithXYZ( point[ 0 ], point[ 1 ], point[ 2 ], 0 );
  moduleNode->GetPlanMRMLFiducialListNode()->SetNthFiducialLabelText( ind, "Entry" );
  moduleNode->SetPlanEntryPoint( point );
  
  plan->GetTargetPointRAS( point );
  ind = moduleNode->GetPlanMRMLFiducialListNode()->AddFiducialWithXYZ( point[ 0 ], point[ 1 ], point[ 2 ], 0 );
  moduleNode->GetPlanMRMLFiducialListNode()->SetNthFiducialLabelText( ind, "Target" );
  moduleNode->SetPlanTargetPoint( point );
  
  moduleNode->SetCurrentSliceOffset( point[ 2 ] );
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetLogic()->SetSliceOffset(
    moduleNode->GetCurrentSliceOffset() );
  
  this->UpdateGUI();
}



void
vtkPerkStationInsertStep
::OnCalibrationSelectionChanged()
{
  int numRows = this->CalibrationList->GetWidget()->GetNumberOfSelectedRows();
  
  if ( numRows != 1 ) return;
  
  vtkMRMLPerkStationModuleNode* moduleNode = this->GetGUI()->GetMRMLNode();
  
  int rowIndex = this->CalibrationList->GetWidget()->GetIndexOfFirstSelectedRow();
  OverlayCalibration* cal = moduleNode->GetCalibrationAtIndex( rowIndex );
  
  moduleNode->SetCurrentCalibrationIndex( rowIndex );
  this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
}



void
vtkPerkStationInsertStep
::InstallCallbacks()
{
}



void
vtkPerkStationInsertStep
::WizardGUICallback( vtkObject *caller, unsigned long event, void *clientData, void *callData )
{
  vtkPerkStationInsertStep *self = reinterpret_cast<vtkPerkStationInsertStep *>( clientData );
  if (self)
    {
    self->ProcessGUIEvents(caller, event, callData);
    }
}



void
vtkPerkStationInsertStep
::ProcessGUIEvents( vtkObject *caller, unsigned long event, void *callData )
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  if(!mrmlNode->GetPlanningVolumeNode() || strcmp(mrmlNode->GetVolumeInUse(), "Planning")!=0)
      return;

  
  if (this->ProcessingCallback)
    {
    return;
    }

  this->ProcessingCallback = true;

  this->ProcessingCallback = false;
}



void
vtkPerkStationInsertStep
::UpdateGUI()
{
  vtkMRMLPerkStationModuleNode* mrmlNode = this->GetGUI()->GetMRMLNode();
  
  if ( ! mrmlNode ) return;
  
  
    // In plane angle.
  
  double angle = mrmlNode->GetActualPlanInsertionAngle();
  std::stringstream ss;
  ss << "In plane insertion angle: " << angle;
  this->AngleInPlaneLabel->SetText( ss.str().c_str() );
  
  
  
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
  
  
    // Update calibration list.
  
  bool updateCalibrationList = true;
  if ( this->CalibrationList == NULL || this->CalibrationList->GetWidget() == NULL )
    {
    updateCalibrationList = false;
    }
  
  if ( updateCalibrationList )
    {
    int numCals = mrmlNode->GetNumberOfCalibrations();
    
    bool deleteFlag = true;
    if ( numCals != this->CalibrationList->GetWidget()->GetNumberOfRows() )
      {
      this->CalibrationList->GetWidget()->DeleteAllRows();
      }
    else
      {
      deleteFlag = false;
      }
    
    for ( int row = 0; row < numCals; ++ row )
      {
      OverlayCalibration* cal = mrmlNode->GetCalibrationAtIndex( row );
      
      if ( cal == NULL )
        {
        vtkErrorMacro( "ERROR: Calibration expected is NULL." );
        return;
        }
      
      if ( deleteFlag )
        {
        this->CalibrationList->GetWidget()->AddRow();
        }
      
      vtkKWMultiColumnList* colList = this->CalibrationList->GetWidget();
      if ( deleteFlag || cal->Name.compare(
             this->CalibrationList->GetWidget()->GetCellText( row, CALIBRATION_COL_NAME ) ) != 0 )
        {
        }
      
      colList->SetCellText( row, CALIBRATION_COL_NAME, cal->Name.c_str() );
      colList->SetCellText( row, CALIBRATION_COL_TX, DoubleToString( cal->SecondMonitorTranslation[ 0 ] ).c_str() );
      colList->SetCellText( row, CALIBRATION_COL_TY, DoubleToString( cal->SecondMonitorTranslation[ 1 ] ).c_str() );
      colList->SetCellText( row, CALIBRATION_COL_RO, DoubleToString( cal->SecondMonitorRotation ).c_str() );
      colList->SetCellText( row, CALIBRATION_COL_FV, BoolToString( cal->SecondMonitorVerticalFlip ).c_str() );
      colList->SetCellText( row, CALIBRATION_COL_FH, BoolToString( cal->SecondMonitorHorizontalFlip ).c_str() );
      } // for ( int row = 0; row < numPlans; ++ row )
    
    this->CalibrationList->GetWidget()->SelectRow( mrmlNode->GetCurrentCalibration() );
    }
  
  
    // Second monitor.
  this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
}
  


void
vtkPerkStationInsertStep
::AddGUIObservers()
{
  this->RemoveGUIObservers();
  
  if ( this->PlanList )
    {
    this->PlanList->GetWidget()->SetSelectionChangedCommand(
      this, "OnMultiColumnListSelectionChanged" );
    }
  
  if ( this->CalibrationList )
    {
    this->CalibrationList->GetWidget()->SetSelectionChangedCommand(
      this, "OnCalibrationSelectionChanged" );
    }
}



void
vtkPerkStationInsertStep
::RemoveGUIObservers()
{
  if ( this->PlanList )
    {
    this->PlanList->GetWidget()->SetSelectionChangedCommand( this, "" );
    }
}



void
vtkPerkStationInsertStep
::Validate()
{
  this->Superclass::Validate();
}
