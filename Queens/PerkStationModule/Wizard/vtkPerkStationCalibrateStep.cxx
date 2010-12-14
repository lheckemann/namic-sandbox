
#include "vtkPerkStationCalibrateStep.h"

#include <fstream>
#include <sstream>
#include <vector>

#include "vtkPerkStationModuleGUI.h"
#include "vtkMRMLPerkStationModuleNode.h"
#include "vtkPerkStationSecondaryMonitor.h"

#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkMRMLLayoutNode.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSlicerModuleCollapsibleFrame.h"

#include "vtkKWApplication.h"
#include "vtkKWCheckButton.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWEntrySet.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWPushButton.h"
#include "vtkKWText.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

#include "vtkMath.h"
#include "vtkMatrixToHomogeneousTransform.h"
#include "vtkTransform.h"
#include "vtkMRMLLinearTransformNode.h"


#include "PerkStationCommon.h"


// --------------------------------------------------------------

#define FORGET( obj ) \
  if ( obj ) \
    { \
    this->Script( "pack forget %s", obj->GetWidgetName() ); \
    };



//----------------------------------------------------------------------------

vtkStandardNewMacro( vtkPerkStationCalibrateStep );

vtkCxxRevisionMacro( vtkPerkStationCalibrateStep, "$Revision: 1.0 $" );


//----------------------------------------------------------------------------
vtkPerkStationCalibrateStep
::vtkPerkStationCalibrateStep()
{
  this->SetName( "1/4. Calibrate" );
  this->SetDescription( "Do image overlay system calibration" );
  
  PERKLOG_ERROR( "calibration constructor" );
  
  this->WizardGUICallbackCommand->SetCallback(
    vtkPerkStationCalibrateStep::WizardGUICallback );
  
  
    // Load, save, reset controls.
  
  this->CalibFileName = "";
  this->CalibFilePath = "";

  
    // Table calibration.
  
  this->TableFrame = NULL;
  this->TableOverlayLabel = NULL;
  this->TableOverlayEntry = NULL;
  this->TableUpdateButton = NULL;
  
  
    // Hardware selection.
  
  this->HardwareFrame = NULL;
  this->HardwareMenu = NULL;
  
  
    // Calibration list.
  
  this->ListButtonsFrame = NULL;
  this->CalibrationListFrame = NULL;
  this->CalibrationList = NULL;
  this->AddButton = NULL;
  this->DeleteButton = NULL;
  this->CalibrationUID = 0;
  
    // Hardware calibration.
  
  this->HardwareCalibrationFrame = NULL;
  this->FlipFrame = NULL;
  this->VerticalFlipCheckButton = NULL;
  this->HorizontalFlipCheckButton = NULL;
  this->MonPhySizeFrame = NULL;
  this->MonPhySizeLabel = NULL;
  this->MonPhySize = NULL;
  this->MonPixResFrame = NULL;
  this->MonPixResLabel = NULL;
  this->MonPixRes = NULL;
  this->HardwareUpdateButton = NULL;
  
  
    // GUI state.
  
  this->ObserverCount = 0;
  this->ProcessingCallback = false;
  this->ClickNumber = 0;
  this->SecondMonitor = 0;
}



vtkPerkStationCalibrateStep
::~vtkPerkStationCalibrateStep()
{
  
    // Table calibration.
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TableFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TableOverlayLabel );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TableOverlayEntry );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->TableUpdateButton );
  
  
    // Hardware selection.
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->HardwareFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->HardwareMenu );
  
  
    // Calibration list.
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->ListButtonsFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->CalibrationListFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->CalibrationList );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->AddButton );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->DeleteButton );
  
  
    // Hardware calibration.
  
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->HardwareCalibrationFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->FlipFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->VerticalFlipCheckButton );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->HorizontalFlipCheckButton );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->MonPhySizeFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->MonPhySizeLabel );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->MonPhySize );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->MonPixResFrame );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->MonPixResLabel );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->MonPixRes );
  DELETE_IF_NULL_WITH_SETPARENT_NULL( this->HardwareUpdateButton );
}



bool
vtkPerkStationCalibrateStep
::DoubleEqual( double val1, double val2 )
{
  bool result = false;
    
  if(fabs(val2-val1) < 0.0001)
      result = true;

  return result;
}



void
vtkPerkStationCalibrateStep
::EnableDisableFlipComponents( bool enable )
{
  this->VerticalFlipCheckButton->GetWidget()->SetEnabled( enable );
  this->HorizontalFlipCheckButton->GetWidget()->SetEnabled( enable );
}



void
vtkPerkStationCalibrateStep
::EnableDisableScaleComponents( bool enable )
{
  this->MonPhySize->GetWidget( 0 )->SetEnabled( enable );
  this->MonPhySize->GetWidget( 1 )->SetEnabled( enable);
  this->MonPixRes->GetWidget( 0 )->SetEnabled( enable );
  this->MonPixRes->GetWidget( 1 )->SetEnabled( enable );
  this->HardwareUpdateButton->SetEnabled( enable );
}



void
vtkPerkStationCalibrateStep
::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();
  
  this->SetName("1/4. Calibrate");
  this->GetGUI()->GetWizardWidget()->SetErrorText( "" );
  this->GetGUI()->GetWizardWidget()->Update();
  
  this->SetDescription( "Do image overlay system calibration" );

  
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
    wizard_widget->GetCancelButton()->SetEnabled( 0 );
  
  
  this->ShowHardwareCalibration();
  this->ShowCalibrationList();
  
  this->UpdateGUI();
  
  this->InstallCallbacks();
  
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->GetSliceViewer()->Focus();
}



void
vtkPerkStationCalibrateStep
::ShowCalibrationList()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  
    // Create frame for the buttons.
  
  if ( ! this->ListButtonsFrame )
    {
    this->ListButtonsFrame = vtkKWFrame::New();
    }
  if ( ! this->ListButtonsFrame->IsCreated() )
    {
    this->ListButtonsFrame->SetParent( parent );
    this->ListButtonsFrame->Create();
    }
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->ListButtonsFrame->GetWidgetName() );
  
  
  if ( ! this->AddButton )
    {
    this->AddButton = vtkKWPushButton::New();
    }
  if ( ! this->AddButton->IsCreated() )
    {
    this->AddButton->SetParent( this->ListButtonsFrame );
    this->AddButton->Create();
    this->AddButton->SetText( "Add Calibration" );
    this->AddButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    }
  this->Script( "pack %s -side left -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->AddButton->GetWidgetName() );
  
  if ( ! this->DeleteButton )
    {
    this->DeleteButton = vtkKWPushButton::New();
    }
  if ( ! this->DeleteButton->IsCreated() )
    {
    this->DeleteButton->SetParent( this->ListButtonsFrame );
    this->DeleteButton->Create();
    this->DeleteButton->SetText( "Delete Selected" );
    this->DeleteButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    }
  this->Script( "pack %s -side left -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->DeleteButton->GetWidgetName() );
  
  
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
  
  
    // Create a frame for the list.
  
  vtkKWFrame* calibrationListFrame = vtkKWFrame::New();
    calibrationListFrame->SetParent( parent );
    calibrationListFrame->Create();
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                calibrationListFrame->GetWidgetName() );
  
    // Create the list.
  
  if ( ! this->CalibrationList )
    {
    this->CalibrationList = vtkKWMultiColumnListWithScrollbars::New();
    }
  if ( ! this->CalibrationList->IsCreated() )
    {
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
    this->CalibrationList->GetWidget()->ColumnEditableOn( 0 );  // Name column.
    this->CalibrationList->GetWidget()->SetColumnEditWindowToEntry( 0 );
    }
  
  this->Script( "pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 2",
                this->CalibrationList->GetWidgetName());
  
  
  calibrationListFrame->Delete();
}



void
vtkPerkStationCalibrateStep
::ShowHardwareCalibration()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  
    // Table calibration.
  
  if ( ! this->TableFrame )
    {
    this->TableFrame = vtkKWFrameWithLabel::New();
    this->TableOverlayLabel = vtkKWLabel::New();
    this->TableOverlayEntry = vtkKWEntry::New();
    this->TableUpdateButton = vtkKWPushButton::New();
    }
  if ( ! this->TableFrame->IsCreated() )
    {
    this->TableFrame->SetParent( parent );
    this->TableFrame->SetLabelText( "Table calibration" );
    this->TableFrame->Create();
    
    this->TableOverlayLabel->SetParent( this->TableFrame->GetFrame() );
    this->TableOverlayLabel->Create();
    this->TableOverlayLabel->SetText( "Patient at overlay laser (mm):" );
    
    this->TableOverlayEntry->SetParent( this->TableFrame->GetFrame() );
    this->TableOverlayEntry->Create();
    this->TableOverlayEntry->SetWidth( 6 );
    
    this->TableUpdateButton->SetParent( this->TableFrame->GetFrame() );
    this->TableUpdateButton->Create();
    this->TableUpdateButton->SetText( "Update" );
    this->TableUpdateButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    }
  
  
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->TableFrame->GetWidgetName() );
  
  this->Script( "grid %s -column 0 -row 1 -sticky w -padx 6 -pady 2", 
                this->TableOverlayLabel->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 1 -sticky w -padx 1 -pady 2",
                this->TableOverlayEntry->GetWidgetName() );
  
  this->Script( "grid %s -column 2 -row 1 -sticky e -padx 6 -pady 2",
                this->TableUpdateButton->GetWidgetName() );
  
  this->Script( "grid columnconfigure %s 0 -weight 1",
                this->TableFrame->GetFrame()->GetWidgetName() );
  this->Script( "grid columnconfigure %s 1 -weight 3",
                this->TableFrame->GetFrame()->GetWidgetName() );
  this->Script( "grid columnconfigure %s 2 -weight 1",
                this->TableFrame->GetFrame()->GetWidgetName() );
  
  
  
    // Hardware selection.
  
  if ( ! this->HardwareFrame )
    {
    this->HardwareFrame = vtkKWFrameWithLabel::New();
    this->HardwareMenu = vtkKWMenuButtonWithLabel::New();
    }
  if ( ! this->HardwareFrame->IsCreated() )
    {
    this->HardwareFrame->SetParent( parent );
    this->HardwareFrame->Create();
    this->HardwareFrame->SetLabelText( "Overlay hardware" );
    
    this->HardwareMenu->SetParent( this->HardwareFrame->GetFrame() );
    this->HardwareMenu->Create();
    this->HardwareMenu->GetWidget()->SetWidth( 22 ); // What is the unit?
    }
  
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->HardwareFrame->GetWidgetName());
  
  this->Script("pack %s -side left -anchor nw -padx 3 -pady 2", 
                  this->HardwareMenu->GetWidgetName());
  

    // Hardware calibration.
  
  if ( ! this->HardwareCalibrationFrame )
    {
    this->HardwareCalibrationFrame = vtkKWFrameWithLabel::New();
    this->FlipFrame = vtkKWFrame::New();
    this->VerticalFlipCheckButton = vtkKWCheckButtonWithLabel::New();
    this->HorizontalFlipCheckButton = vtkKWCheckButtonWithLabel::New();
    this->MonPhySizeFrame = vtkKWFrame::New();
    this->MonPhySizeLabel =  vtkKWLabel::New();
    this->MonPhySize = vtkKWEntrySet::New();
    this->MonPixResFrame = vtkKWFrame::New();
    this->MonPixResLabel = vtkKWLabel::New();
    this->MonPixRes = vtkKWEntrySet::New();
    this->HardwareUpdateButton = vtkKWPushButton::New();
    }
  if ( ! this->HardwareCalibrationFrame->IsCreated() )
    {
    this->HardwareCalibrationFrame->SetParent( parent );
    this->HardwareCalibrationFrame->Create();
    this->HardwareCalibrationFrame->SetLabelText( "Hardware calibration" );
    this->HardwareCalibrationFrame->GetFrame();
    this->HardwareCalibrationFrame->CollapseFrame();
    
    this->FlipFrame->SetParent( this->HardwareCalibrationFrame->GetFrame() );
    this->FlipFrame->Create();
    
    this->VerticalFlipCheckButton->SetParent( this->FlipFrame );
    this->VerticalFlipCheckButton->Create();
    this->VerticalFlipCheckButton->SetLabelText( "Vertical Flip:" );
    
    this->HorizontalFlipCheckButton->SetParent( this->FlipFrame );
    this->HorizontalFlipCheckButton->Create();
    this->HorizontalFlipCheckButton->SetLabelText( "Horizontal Flip:" );
    
    
    this->MonPhySizeFrame->SetParent( this->HardwareCalibrationFrame->GetFrame() );
    this->MonPhySizeFrame->Create();
    
    this->MonPhySizeLabel->SetParent( this->MonPhySizeFrame );
    this->MonPhySizeLabel->Create();
    this->MonPhySizeLabel->SetText( "Monitor physical size (mm):" );
    
    this->MonPhySize->SetParent( this->MonPhySizeFrame );
    this->MonPhySize->Create();
    this->MonPhySize->SetBorderWidth( 2 );
    this->MonPhySize->SetReliefToGroove();
    this->MonPhySize->SetPackHorizontally( 1 );
    this->MonPhySize->SetWidgetsInternalPadX( 2 );  
    this->MonPhySize->SetMaximumNumberOfWidgetsInPackingDirection( 2 );
    
    for ( int id = 0; id < 2; id++ )  // two entries of monitor size (x, y)
      {
      vtkKWEntry *entry = this->MonPhySize->AddWidget( id );  
      entry->SetWidth( 7 );
      entry->SetDisabledBackgroundColor( 0.9, 0.9, 0.9 );
      }
    
    
    this->MonPixResFrame->SetParent( this->HardwareCalibrationFrame->GetFrame() );
    this->MonPixResFrame->Create();
    
    this->MonPixResLabel->SetParent( this->MonPixResFrame );
    this->MonPixResLabel->Create();
    this->MonPixResLabel->SetText( "Monitor pixel resolution:   " );
    
    this->MonPixRes->SetParent(this->MonPixResFrame);
    this->MonPixRes->Create();
    this->MonPixRes->SetBorderWidth( 2 );
    this->MonPixRes->SetReliefToGroove();
    this->MonPixRes->SetPackHorizontally( 1 );
    this->MonPixRes->SetWidgetsInternalPadX( 2 );  
    this->MonPixRes->SetMaximumNumberOfWidgetsInPackingDirection( 2 );
    this->MonPixRes->SetEnabled( false );
    
    for ( int id = 0; id < 2; id++ ) // two entries of monitor size (x, y)
      {
      vtkKWEntry *entry = this->MonPixRes->AddWidget( id );  
      entry->SetWidth( 7 );
      entry->SetDisabledBackgroundColor( 0.9, 0.9, 0.9 );
      }
    
    this->HardwareUpdateButton->SetParent( this->MonPixResFrame );
    this->HardwareUpdateButton->Create();
    this->HardwareUpdateButton->SetText( "Update" );
    this->HardwareUpdateButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    }
  
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->HardwareCalibrationFrame->GetWidgetName() );
  this->Script( "pack %s -side top -anchor nw -padx 0 -pady 2", 
                this->FlipFrame->GetWidgetName() );
  this->Script( "pack %s -side left -anchor nw -padx 6 -pady 2", 
                this->VerticalFlipCheckButton->GetWidgetName() );
  this->Script( "pack %s -side top -anchor nw -padx 6 -pady 2", 
                this->HorizontalFlipCheckButton->GetWidgetName() );
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->MonPhySizeFrame->GetWidgetName() );
  this->Script( "pack %s -side left -anchor nw -padx 6 -pady 2", 
                this->MonPhySizeLabel->GetWidgetName() );
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->MonPhySize->GetWidgetName() );
  
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->MonPixResFrame->GetWidgetName() );
  this->Script( "pack %s -side left -anchor nw -padx 6 -pady 2", 
                this->MonPixResLabel->GetWidgetName() );
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->MonPixRes->GetWidgetName() );
  
  this->Script( "pack %s -side top -anchor ne -padx 2 -pady 4", 
                this->HardwareUpdateButton->GetWidgetName() );
  
  
  this->PopulateControls();
}

//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::InstallCallbacks()
{
  this->AddGUIObservers();
}


/**
 * Update UI fields with data from the MRML node.
 * Also resets the timer.
 */
void
vtkPerkStationCalibrateStep
::PopulateControls()
{
  if (    ( ! this->GetGUI()->GetMRMLNode() )
       || ( ! this->GetGUI()->GetSecondaryMonitor() ) )
    {
    PERKLOG_ERROR( "PerkStation MRML Node or SecondaryMonitor not created." );
    return;
    }
  
  
  vtkMRMLPerkStationModuleNode* node = this->GetGUI()->GetMRMLNode();
  int ni = node->GetHardwareIndex();
  OverlayHardware hardware = node->GetHardwareList()[ node->GetHardwareIndex() ];
  
    // Update UI fields.
  
  this->TableOverlayEntry->SetValueAsDouble( node->GetTableAtOverlay() );
  
  this->HardwareMenu->GetWidget()->SetValue( hardware.Name.c_str() );
  
  
  // double monPhySize[ 2 ];
  // this->GetGUI()->GetSecondaryMonitor()->GetPhysicalSize(
  //   monPhySize[ 0 ], monPhySize[ 1 ] );
  
  this->MonPhySize->GetWidget( 0 )->SetValueAsDouble( hardware.SizeX );
  this->MonPhySize->GetWidget( 1 )->SetValueAsDouble( hardware.SizeY );
  
  double monPixRes[ 2 ];
  this->GetGUI()->GetSecondaryMonitor()->GetPixelResolution(
    monPixRes[ 0 ], monPixRes[ 1 ] );
  this->MonPixRes->GetWidget( 0 )->SetValueAsDouble( monPixRes[ 0 ] );
  this->MonPixRes->GetWidget( 1 )->SetValueAsDouble( monPixRes[ 1 ] );
  
  this->GetGUI()->GetWizardWidget()->SetErrorText( "" );
  this->GetGUI()->GetWizardWidget()->Update();
  
  
  // Q: do we want to reset timer on reset of calibration?
  this->LogTimer->StartTimer();
}


//----------------------------------------------------------------------------
void
vtkPerkStationCalibrateStep
::PopulateControlsOnLoadCalibration()
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if ( ! mrmlNode )
    {
    PERKLOG_ERROR( "PerkStation MRML Node not created." );
    return;
    }
  
  this->PopulateControls();
}


//----------------------------------------------------------------------------
void
vtkPerkStationCalibrateStep
::HorizontalFlipCallback( bool value )
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if ( mrmlNode && this->GetGUI()->GetSecondaryMonitor() )
    {
    this->GetGUI()->GetMRMLNode()->SetSecondMonitorHorizontalFlip( (bool)value );
    this->UpdateGUI();
    this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
    }
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep
::VerticalFlipCallback( bool value )
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  
  if ( mrmlNode && this->GetGUI()->GetSecondaryMonitor() )
    {
    this->GetGUI()->GetMRMLNode()->SetSecondMonitorVerticalFlip( (bool)value );
    this->UpdateGUI();
    this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
    }
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::UpdateAutoScaleCallback()
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if ( ! mrmlNode ) return;
  
  vtkMRMLScalarVolumeNode *inVolume = mrmlNode->GetPlanningVolumeNode();
  if ( ! inVolume )
    {
    vtkErrorMacro( "Cannot do calibration without a planning volume." );
    return;
    }
  
  
  
  double mmX = this->MonPhySize->GetWidget( 0 )->GetValueAsDouble();
  double mmY = this->MonPhySize->GetWidget( 1 )->GetValueAsDouble();
  mrmlNode->UpdateHardwareCalibration( mmX, mmY );
  
  double pixX = this->MonPixRes->GetWidget( 0 )->GetValueAsDouble();
  double pixY = this->MonPixRes->GetWidget( 1 )->GetValueAsDouble();
  // TODO: Use these values, instead of the ones read from system.
  
    // set the values in secondary monitor
  this->GetGUI()->GetSecondaryMonitor()->SetPixelResolution( pixX, pixY );
  
  
}


//----------------------------------------------------------------------------
void
vtkPerkStationCalibrateStep
::ProcessKeyboardEvents( vtkObject *caller, unsigned long event, void *callData )
{
  if (    ! this->GetGUI()->GetMRMLNode()
       || ! this->GetGUI()->GetMRMLNode()->GetPlanningVolumeNode()
       || strcmp( this->GetGUI()->GetMRMLNode()->GetVolumeInUse(),
                  "Planning" ) != 0 )
    {
    return;
    }
  
  
    // has to be when it is in 
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  if (    ! wizard_widget
       || wizard_widget->GetWizardWorkflow()->GetCurrentStep() != this )
    {
    return;
    }
  
  
  if ( this->ProcessingCallback ) return;
  this->ProcessingCallback = true;
  
  
  vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast( caller );
  
  
    // Image calibration on second monitor with focus on Red Slice.
  
  double stepSize = 0.8;
  
  double translation[ 2 ];
  double rotation = this->GetGUI()->GetMRMLNode()->GetSecondMonitorRotation();
  this->GetGUI()->GetMRMLNode()->GetSecondMonitorTranslation( translation );
  
  /*
  double hFlip = 1.0;
  if ( this->GetGUI()->GetMRMLNode()->GetFinalHorizontalFlip() )
    {
    hFlip = - 1.0;
    }
  */
  
  if ( event == vtkCommand::KeyPressEvent )
    {
    char  *key = style->GetKeySym();
    switch ( *key )
      {
      case 'a' : translation[ 1 ] -= stepSize; break;
      case 'z' : translation[ 1 ] += stepSize; break;
      case 'q' : translation[ 0 ] += stepSize; break;
      case 'w' : translation[ 0 ] -= stepSize; break;
      case 'g' : rotation += ( stepSize / 3.0 ); break;
      case 'h' : rotation -= ( stepSize / 3.0 ); break;
      }
    }
  
  this->GetGUI()->GetMRMLNode()->SetSecondMonitorTranslation( translation );
  this->GetGUI()->GetMRMLNode()->SetSecondMonitorRotation( rotation );
  
  this->UpdateGUI();
  
  this->ProcessingCallback = false;
}



void vtkPerkStationCalibrateStep::SuggestFileName()
{
}



void vtkPerkStationCalibrateStep::Reset()
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if ( ! mrmlNode )
    {
    return;
    }
  
  double tx[ 2 ] = { 0.0, 0.0 };
  this->GetGUI()->GetMRMLNode()->SetTableAtOverlay( 0.0 );
  this->GetGUI()->GetMRMLNode()->SetSecondMonitorTranslation( tx );
  this->GetGUI()->GetMRMLNode()->SetSecondMonitorRotation( 0.0 );
  this->PopulateControls();
  
  vtkMRMLScalarVolumeNode *inVolume = mrmlNode->GetPlanningVolumeNode();
  if ( ! inVolume )
    {
    return;
    }
  
  this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
  
  
  this->ClickNumber = 0;
}



void vtkPerkStationCalibrateStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf( os,indent );
}



void vtkPerkStationCalibrateStep::AddGUIObservers()
{
  this->RemoveGUIObservers();
  
  
    // Table calibration.
  
  if ( this->TableOverlayEntry )
    {
    this->TableOverlayEntry->AddObserver(
      vtkKWEntry::EntryValueChangedEvent, ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  
  ADD_BUTTON_INVOKED_EVENT_WIZARD( this->TableUpdateButton );
  
  
    // Hardware selection.
  
  if ( this->HardwareMenu )
    {
    this->HardwareMenu->GetWidget()->GetMenu()->AddObserver(
      vtkKWMenu::MenuItemInvokedEvent, ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  
  
    // Calibration list.
  
  if ( this->CalibrationList )
    {
    this->CalibrationList->GetWidget()->SetCellUpdatedCommand( this, "OnMultiColumnListUpdate" );
    this->CalibrationList->GetWidget()->SetSelectionChangedCommand( this, "OnMultiColumnListSelectionChanged" );
    }
  
  ADD_BUTTON_INVOKED_EVENT_WIZARD( this->AddButton );
  ADD_BUTTON_INVOKED_EVENT_WIZARD( this->DeleteButton );
  
  
    // flip controls
  
  if (this->VerticalFlipCheckButton)
    {
    this->VerticalFlipCheckButton->GetWidget()->AddObserver(
      vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)( this->WizardGUICallbackCommand ) );
    }
  if (this->HorizontalFlipCheckButton)
    {
    this->HorizontalFlipCheckButton->GetWidget()->AddObserver(
      vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand*)( this->WizardGUICallbackCommand ) );
    }
  
  
  ADD_BUTTON_INVOKED_EVENT_WIZARD( this->HardwareUpdateButton );
  
  
  this->ObserverCount++;
}


void
vtkPerkStationCalibrateStep
::RemoveGUIObservers()
{
  
    // Table calibration.
  
  if ( this->TableOverlayEntry )
    {
    this->TableOverlayEntry->RemoveObservers( vtkKWEntry::EntryValueChangedEvent,
                                              ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  
  REMOVE_BUTTON_INVOKED_EVENT_WIZARD( this->TableUpdateButton );
  
  
    // Hardware selection.
  
  if ( this->HardwareMenu )
    {
    this->HardwareMenu->GetWidget()->GetMenu()->RemoveObservers(
      vtkKWMenu::MenuItemInvokedEvent, ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  
  
    // Calibration list.
  
  if ( this->CalibrationList )
    {
    this->CalibrationList->GetWidget()->SetCellUpdatedCommand( this, "" );
    this->CalibrationList->GetWidget()->SetSelectionChangedCommand( this, "" );
    }
  
  REMOVE_BUTTON_INVOKED_EVENT_WIZARD( this->AddButton );
  REMOVE_BUTTON_INVOKED_EVENT_WIZARD( this->DeleteButton );
  
  
    // flip controls
  
  if ( this->VerticalFlipCheckButton )
    {
    this->VerticalFlipCheckButton->GetWidget()->RemoveObservers(
      vtkKWCheckButton::SelectedStateChangedEvent,
      ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  if ( this->HorizontalFlipCheckButton )
    {
    this->HorizontalFlipCheckButton->GetWidget()->RemoveObservers(
      vtkKWCheckButton::SelectedStateChangedEvent,
      ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  
  
   // change in monitor physical size/ res, therefore update button
  
  REMOVE_BUTTON_INVOKED_EVENT_WIZARD( this->HardwareUpdateButton );
  
  
  this->ObserverCount--;
}



void
vtkPerkStationCalibrateStep
::WizardGUICallback( vtkObject *caller, unsigned long event, void *clientData, void *callData )
{
  vtkPerkStationCalibrateStep *self = reinterpret_cast< vtkPerkStationCalibrateStep *>( clientData );
  if ( self )
    {
    self->ProcessGUIEvents( caller, event, callData );
    }
}



void
vtkPerkStationCalibrateStep
::HideUserInterface()
{
  Superclass::HideUserInterface();
}



void
vtkPerkStationCalibrateStep
::ProcessGUIEvents( vtkObject *caller, unsigned long event, void *callData )
{
  if ( this->ProcessingCallback ) return;
  this->ProcessingCallback = true;
  
  vtkMRMLPerkStationModuleNode* node = this->GetGUI()->GetMRMLNode();
  if ( node == NULL ) return;
  
  
    // Table position entries changed.
  
  if (    this->TableOverlayEntry
       && this->TableOverlayEntry == vtkKWEntry::SafeDownCast( caller )
       && event == vtkKWEntry::EntryValueChangedEvent )
    {
    if (    this->TableOverlayEntry->GetValueAsDouble()
         != node->GetTableAtOverlay() )
      {
      this->TableUpdateButton->SetBackgroundColor( 1.0, 0.8, 0.8 );
      }
    }
  
  
    // Table update button pressed.
  
  if (    this->TableUpdateButton
       && this->TableUpdateButton == vtkKWPushButton::SafeDownCast( caller ) )
    {
    node->SetTableAtOverlay( this->TableOverlayEntry->GetValueAsDouble() );
    this->TableUpdateButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    }
  
  
    // Hardware selection menu.
  
  if (    this->HardwareMenu
       && this->HardwareMenu->GetWidget()->GetMenu() == vtkKWMenu::SafeDownCast( caller )
       && event == vtkKWMenu::MenuItemInvokedEvent )
    {
    this->HardwareSelected( this->HardwareMenu->GetWidget()->GetMenu()->GetIndexOfSelectedItem() );
    
    this->HardwareMenu->SetWidth( 200 );
    this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
    }
  
  
    // Hardware calibration.
  
  if (    this->HardwareUpdateButton
       && this->HardwareUpdateButton == vtkKWPushButton::SafeDownCast(caller)
       && ( event == vtkKWPushButton::InvokedEvent ) )
    {
    this->UpdateAutoScaleCallback();
    this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
    }
  
  
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  
  if( ! mrmlNode )
    {
    this->ProcessingCallback = false;
    return;
    }
  
    
    // Add and delete calibrations.
  
  if (    ( this->AddButton == vtkKWPushButton::SafeDownCast( caller ) )
       && ( event == vtkKWPushButton::InvokedEvent ) )
    {
    
    std::stringstream ss;
      ss << "Calibr" << this->CalibrationUID;
    
    this->CalibrationUID ++;
    OverlayCalibration* newCal = new OverlayCalibration; // Deleted by mrmlNode destructor.
      newCal->Name = ss.str();
    int index = mrmlNode->AddCalibration( newCal );
    mrmlNode->SetCurrentCalibrationIndex( index );
    this->UpdateGUI();
    }
  
  if (    this->DeleteButton == vtkKWPushButton::SafeDownCast( caller )
       && ( event == vtkKWPushButton::InvokedEvent ) )
    {
    OverlayCalibration* cal = mrmlNode->GetCalibrationAtIndex( mrmlNode->GetCurrentCalibration() );
    if ( cal != NULL )
      {
      mrmlNode->RemoveCalibrationAtIndex( mrmlNode->GetCurrentCalibration() );
      if ( mrmlNode->GetNumberOfCalibrations() > 0 )
        {
        mrmlNode->SetCurrentCalibrationIndex( 0 );
        }
      else 
        {
        mrmlNode->SetCurrentCalibrationIndex( -1 );
        }
      this->UpdateGUI();
      }
    }
  
  
    // Continue only with panning volume loaded.
  
  if( ! mrmlNode->GetPlanningVolumeNode()
      || strcmp( mrmlNode->GetVolumeInUse(), "Planning" ) != 0 )
    {
    this->ProcessingCallback = false;
    return;
    }
  
  
    // check buttons
  
  if ( this->VerticalFlipCheckButton
       && this->VerticalFlipCheckButton->GetWidget() == vtkKWCheckButton::SafeDownCast( caller )
       && (event == vtkKWCheckButton::SelectedStateChangedEvent ) )
    {
    this->VerticalFlipCallback( bool( this->VerticalFlipCheckButton->GetWidget()->GetSelectedState() ) );
    }
  
  
  if ( this->HorizontalFlipCheckButton
       && this->HorizontalFlipCheckButton->GetWidget() == vtkKWCheckButton::SafeDownCast( caller )
       && ( event == vtkKWCheckButton::SelectedStateChangedEvent ) )
    {
    this->HorizontalFlipCallback( bool( this->HorizontalFlipCheckButton->GetWidget()->GetSelectedState() ) );
    }

  
  this->ProcessingCallback = false;
}



void
vtkPerkStationCalibrateStep
::Validate()
{
  this->Superclass::Validate();
}




void
vtkPerkStationCalibrateStep
::OnMultiColumnListUpdate(int row, int col, char * str)
{
    // Make sure the row and col exists.
  if (    ( row < 0 ) || ( row >= this->CalibrationList->GetWidget()->GetNumberOfRows() )
       || ( row < 0 ) || ( row >= this->CalibrationList->GetWidget()->GetNumberOfRows() ) )
    {
    return;
    }
  
  bool updated = false;
  
  if ( col == CALIBRATION_COL_NAME )
    {
    OverlayCalibration* cal = this->GetGUI()->GetMRMLNode()->GetCalibrationAtIndex( row );
    cal->Name = std::string( str );
    updated = true;
    }
  
  if ( updated )
    {
    this->UpdateGUI();
    }
}


void
vtkPerkStationCalibrateStep
::OnMultiColumnListSelectionChanged()
{
  int numRows = this->CalibrationList->GetWidget()->GetNumberOfSelectedRows();
  
  if ( numRows != 1 ) return;
  
  vtkMRMLPerkStationModuleNode* moduleNode = this->GetGUI()->GetMRMLNode();
  
  int rowIndex = this->CalibrationList->GetWidget()->GetIndexOfFirstSelectedRow();
  OverlayCalibration* cal = moduleNode->GetCalibrationAtIndex( rowIndex );
  
  moduleNode->SetCurrentCalibrationIndex( rowIndex );
  this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
}


/**
 * Currently, hardware parameters are hardcoded into this function.
 * TODO: Read hardware parameters from an xml file.
 */
void
vtkPerkStationCalibrateStep
::HardwareSelected( int index )
{
  this->SecondMonitor = index;
  this->GetGUI()->GetMRMLNode()->SetHardwareIndex( index );
  this->PopulateControls();
}


/**
 * Updates data displayed on the GUI, reading the actual MRML node.
 */
void
vtkPerkStationCalibrateStep
::UpdateGUI()
{
  vtkMRMLPerkStationModuleNode* node = this->GetGUI()->GetMRMLNode();
  
  if ( node == NULL ) return; // No MRML node, no GUI update.
  
  
  
  this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
  
  
    // Hardware calibration.
  
  this->HorizontalFlipCheckButton->GetWidget()->SetSelectedState( node->GetSecondMonitorHorizontalFlip() );
  this->VerticalFlipCheckButton->GetWidget()->SetSelectedState( node->GetSecondMonitorVerticalFlip() );
  
  
  
    // Update hardware list, only if it changed.
  
  this->HardwareMenu->GetWidget()->GetMenu()->DeleteAllItems();
  std::vector< OverlayHardware > list = node->GetHardwareList();
  bool listChanged = false;
  if ( list.size() != this->HardwareMenu->GetWidget()->GetMenu()->
                      GetNumberOfItems() )
    {
    listChanged = true;
    }
  else
    {
    for ( unsigned int i = 0; i < list.size(); ++ i )
      {
      if ( strcmp( this->HardwareMenu->GetWidget()->GetMenu()->GetItemLabel( i ),
                   list[ i ].Name.c_str() ) != 0 )
        {
        listChanged = true;
        }
      }
    }
  
  if ( listChanged )
    {
    for ( unsigned int i = 0; i < list.size(); ++ i )
      {
      this->HardwareMenu->GetWidget()->GetMenu()->AddRadioButton( list[ i ].Name.c_str() );
      }
    }
  
    // Update calibration list.
  
  bool updateCalibrationList = true;
  if ( this->CalibrationList == NULL || this->CalibrationList->GetWidget() == NULL )
    {
    updateCalibrationList = false;
    }
  
  if ( updateCalibrationList )
    {
    int numCals = node->GetNumberOfCalibrations();
    
    bool deleteFlag = true;
    if ( numCals != this->CalibrationList->GetWidget()->GetNumberOfRows() )
      {
      this->CalibrationList->GetWidget()->DeleteAllRows();
      }
    else
      {
      deleteFlag = false;
      }
    
    const int PRECISION_DIGITS = 1;
    
    for ( int row = 0; row < numCals; ++ row )
      {
      OverlayCalibration* cal = node->GetCalibrationAtIndex( row );
      
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
    
    this->CalibrationList->GetWidget()->SelectRow( node->GetCurrentCalibration() );
    }
  
  this->PopulateControls();
}

