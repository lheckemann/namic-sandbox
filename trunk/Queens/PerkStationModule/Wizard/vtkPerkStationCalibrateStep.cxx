
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
#include "vtkKWPushButton.h"
#include "vtkKWText.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

#include "vtkMath.h"
#include "vtkMatrixToHomogeneousTransform.h"
#include "vtkTransform.h"
#include "vtkMRMLLinearTransformNode.h"



/**
 * @param cstr C string to be converted.
 * @returns String content converted to bool.
 */
bool CharToBool( char* cstr )
{
  std::stringstream ss;
  ss << cstr;
  bool ret;
  ss >> ret;
  return ret;
}


/**
 * @param cstr C string to be converted.
 * @returns String content converted to double.
 */
double CharToDouble( char* cstr )
{
  std::stringstream ss;
  ss << cstr;
  double ret;
  ss >> ret;
  return ret;
}


/**
 * @param cstr C string containing double numbers.
 * @returns Vector of doubles converted from argument.
 */
std::vector< double > CharToDoubleVector( char* cstr )
{
  std::stringstream ss( cstr );
  double d;
  std::vector< double > ret;
  while ( ss >> d )
    {
    ret.push_back( d );
    }
  return ret;
}



//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationCalibrateStep);
vtkCxxRevisionMacro(vtkPerkStationCalibrateStep, "$Revision: 1.0 $");


//----------------------------------------------------------------------------
vtkPerkStationCalibrateStep::vtkPerkStationCalibrateStep()
{
  this->SetName("1/4. Calibrate");
  this->SetDescription("Do image overlay system calibration");
  
  
  this->WizardGUICallbackCommand->SetCallback(
    vtkPerkStationCalibrateStep::WizardGUICallback );
  
  
    // Load, save, reset controls.
  
  this->LoadResetFrame = vtkSmartPointer< vtkKWFrame >::New();
  this->LoadCalibrationFileButton = vtkSmartPointer< vtkKWLoadSaveButton >::New();
  this->SaveCalibrationFileButton = vtkSmartPointer< vtkKWLoadSaveButton >::New();
  this->ResetCalibrationButton = vtkSmartPointer< vtkKWPushButton >::New();
  this->CalibFileName = "";
  this->CalibFilePath = "";

  
    // Table calibration.
  
  this->TableFrame = vtkSmartPointer< vtkKWFrameWithLabel >::New();
  this->TableScannerLabel = vtkSmartPointer< vtkKWLabel >::New();
  this->TableScannerEntry = vtkSmartPointer< vtkKWEntry >::New();
  this->TableOverlayLabel = vtkSmartPointer< vtkKWLabel >::New();
  this->TableOverlayEntry = vtkSmartPointer< vtkKWEntry >::New();
  this->TableUpdateButton = vtkSmartPointer< vtkKWPushButton >::New();
  
  this->PatientFrame = vtkSmartPointer< vtkKWFrameWithLabel >::New();
  this->PatientLabel = vtkSmartPointer< vtkKWLabel >::New();
  this->PatientEntry = vtkSmartPointer< vtkKWEntry >::New();
  this->PatientUpdateButton = vtkSmartPointer< vtkKWPushButton >::New();
  
  
    // Hardware selection.
  
  this->HardwareFrame = vtkSmartPointer< vtkKWFrameWithLabel >::New();
  this->HardwareMenu = vtkSmartPointer< vtkKWMenuButtonWithLabel >::New();
  
  
    // Hardware calibration.
  
  this->HardwareCalibrationFrame = vtkSmartPointer< vtkKWFrameWithLabel >::New();
  
  this->FlipFrame = vtkSmartPointer< vtkKWFrame >::New();
  this->VerticalFlipCheckButton =
    vtkSmartPointer< vtkKWCheckButtonWithLabel >::New();
  this->HorizontalFlipCheckButton =
    vtkSmartPointer< vtkKWCheckButtonWithLabel >::New();
  
  this->MonPhySizeFrame = vtkSmartPointer< vtkKWFrame >::New();
  this->MonPhySizeLabel = vtkSmartPointer< vtkKWLabel >::New();
  this->MonPhySize = vtkSmartPointer< vtkKWEntrySet >::New();
  
  this->MonPixResFrame = vtkSmartPointer< vtkKWFrame >::New();
  this->MonPixResLabel = vtkSmartPointer< vtkKWLabel >::New();
  this->MonPixRes = vtkSmartPointer< vtkKWEntrySet >::New();
  
  this->HardwareUpdateButton = vtkSmartPointer< vtkKWPushButton >::New();
  
  
    // GUI state.
  
  this->ObserverCount = 0;
  this->ProcessingCallback = false;
  this->ClickNumber = 0;
  this->SecondMonitor = 0;
}



vtkPerkStationCalibrateStep
::~vtkPerkStationCalibrateStep()
{
   
}


//----------------------------------------------------------------------------
bool vtkPerkStationCalibrateStep::DoubleEqual(double val1, double val2)
{
  bool result = false;
    
  if(fabs(val2-val1) < 0.0001)
      result = true;

  return result;
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::EnableDisableLoadResetControls(bool enable)
{
  this->LoadCalibrationFileButton->SetEnabled(enable);
  this->ResetCalibrationButton->SetEnabled(enable);
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::EnableDisableSaveControls( bool enable )
{
  this->SaveCalibrationFileButton->SetEnabled( enable );
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::EnableDisableFlipComponents( bool enable )
{
  this->VerticalFlipCheckButton->GetWidget()->SetEnabled( enable );
  this->HorizontalFlipCheckButton->GetWidget()->SetEnabled( enable );
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::EnableDisableScaleComponents( bool enable )
{
  this->MonPhySize->GetWidget( 0 )->SetEnabled( enable );
  this->MonPhySize->GetWidget( 1 )->SetEnabled( enable);
  this->MonPixRes->GetWidget( 0 )->SetEnabled( enable );
  this->MonPixRes->GetWidget( 1 )->SetEnabled( enable );
  this->HardwareUpdateButton->SetEnabled( enable );
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();
  
  this->SetName("1/4. Calibrate");
  this->GetGUI()->GetWizardWidget()->SetErrorText( "");
  this->GetGUI()->GetWizardWidget()->Update();
  
  
  this->SetDescription("Do image overlay system calibration");

  // gui building/showing

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);

  this->ShowHardwareCalibration();
  this->ShowLoadResetControls();
  
  this->UpdateGUI();
    
  this->InstallCallbacks();
  
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
    GetSliceViewer()->Focus();
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ShowLoadResetControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  
  // Create the frame
  if ( ! this->LoadResetFrame->IsCreated() )
    {
    this->LoadResetFrame->SetParent( parent );
    this->LoadResetFrame->Create();
    
    this->LoadCalibrationFileButton->SetParent( this->LoadResetFrame );
    this->LoadCalibrationFileButton->Create();
    this->LoadCalibrationFileButton->SetReliefToRaised();
    this->LoadCalibrationFileButton->SetBorderWidth( 2 );
    this->LoadCalibrationFileButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    this->LoadCalibrationFileButton->SetActiveBackgroundColor( 1, 1, 1 );
    this->LoadCalibrationFileButton->SetHighlightThickness( 2 );
    this->LoadCalibrationFileButton->SetText( "Load calibration" );
    this->LoadCalibrationFileButton->SetImageToPredefinedIcon(
      vtkKWIcon::IconPresetLoad );
    this->LoadCalibrationFileButton->GetLoadSaveDialog()->
      RetrieveLastPathFromRegistry( "OpenPath" );
    this->LoadCalibrationFileButton->TrimPathFromFileNameOff();
    this->LoadCalibrationFileButton->SetMaximumFileNameLength( 256 );
    this->LoadCalibrationFileButton->GetLoadSaveDialog()->SaveDialogOff();
    this->LoadCalibrationFileButton->GetLoadSaveDialog()->SetFileTypes(
      "{{XML File} {.xml}} {{All Files} {*.*}}" );
    
    this->SaveCalibrationFileButton->SetParent( this->LoadResetFrame );
    this->SaveCalibrationFileButton->Create();
    this->SaveCalibrationFileButton->SetText( "Save calibration" );
    this->SaveCalibrationFileButton->SetReliefToRaised();
    this->SaveCalibrationFileButton->SetBorderWidth( 2 );
    this->SaveCalibrationFileButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    this->SaveCalibrationFileButton->SetActiveBackgroundColor( 1, 1, 1 );
    this->SaveCalibrationFileButton->SetHighlightThickness( 2 );
    this->SaveCalibrationFileButton->SetImageToPredefinedIcon(
      vtkKWIcon::IconFloppy  );
    this->SaveCalibrationFileButton->GetLoadSaveDialog()->SaveDialogOn();
    this->SaveCalibrationFileButton->TrimPathFromFileNameOff();
    this->SaveCalibrationFileButton->SetMaximumFileNameLength( 256 );
    this->SaveCalibrationFileButton->GetLoadSaveDialog()->SetFileTypes(
      "{{XML File} {.xml}} {{All Files} {*.*}}");      
    this->SaveCalibrationFileButton->GetLoadSaveDialog()->
      RetrieveLastPathFromRegistry( "OpenPath" );
    
    this->ResetCalibrationButton->SetParent( this->LoadResetFrame );
    this->ResetCalibrationButton->Create();
    this->ResetCalibrationButton->SetText( "Reset calibration" );
    // this->ResetCalibrationButton->SetWidth( 100 );
    this->ResetCalibrationButton->SetReliefToRaised();
    this->ResetCalibrationButton->SetBorderWidth( 2 );
    this->ResetCalibrationButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    this->ResetCalibrationButton->SetActiveBackgroundColor( 1, 1, 1 );
    this->ResetCalibrationButton->SetHighlightThickness( 2 );
    // this->ResetCalibrationButton->SetImageToPredefinedIcon(
    //     vtkKWIcon::IconTrashcan );
    }
  
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->LoadResetFrame->GetWidgetName() );
  
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->LoadCalibrationFileButton->GetWidgetName() );

  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->SaveCalibrationFileButton->GetWidgetName() );
  
  this->Script( "pack %s -side right -anchor ne -padx 2 -pady 2", 
                this->ResetCalibrationButton->GetWidgetName() );
}


void
vtkPerkStationCalibrateStep
::ShowHardwareCalibration()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  
    // Table calibration.
  
  if ( ! this->TableFrame->IsCreated() )
    {
    this->TableFrame->SetParent( parent );
    this->TableFrame->SetLabelText( "Table calibration" );
    this->TableFrame->Create();
    
    this->TableScannerLabel->SetParent( this->TableFrame->GetFrame() );
    this->TableScannerLabel->Create();
    this->TableScannerLabel->SetText( "Calibration at scanner laser (mm):" );
    
    this->TableScannerEntry->SetParent( this->TableFrame->GetFrame() );
    this->TableScannerEntry->Create();
    this->TableScannerEntry->SetWidth( 6 );
    this->TableScannerEntry->SetBalloonHelpString(
      "Value displayed on the scanner, when calibration object is under "
      "the scanner laser." );
    
    this->TableOverlayLabel->SetParent( this->TableFrame->GetFrame() );
    this->TableOverlayLabel->Create();
    this->TableOverlayLabel->SetText( "Calibration at overlay laser (mm):" );
    
    this->TableOverlayEntry->SetParent( this->TableFrame->GetFrame() );
    this->TableOverlayEntry->Create();
    this->TableOverlayEntry->SetWidth( 6 );
    this->TableOverlayEntry->SetBalloonHelpString(
      "Value displayed on the scanner, when calibration object is under "
      "the overlay laser." );
    
    this->TableUpdateButton->SetParent( this->TableFrame->GetFrame() );
    this->TableUpdateButton->Create();
    this->TableUpdateButton->SetText( "Update" );
    this->TableUpdateButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    
    
    this->PatientFrame->SetParent( parent );
    this->PatientFrame->SetLabelText( "Patient calibration" );
    this->PatientFrame->Create();
    
    this->PatientLabel->SetParent( this->PatientFrame->GetFrame() );
    this->PatientLabel->Create();
    this->PatientLabel->SetText( "Patient at scanner laser (mm):" );
    
    this->PatientEntry->SetParent( this->PatientFrame->GetFrame() );
    this->PatientEntry->Create();
    this->PatientEntry->SetWidth( 6 );
    this->PatientEntry->SetBalloonHelpString(
      "Value displayed on the scanner, when patient point set as zero "
      "image position, is under the scanner laser." );
    
    this->PatientUpdateButton->SetParent( this->PatientFrame->GetFrame() );
    this->PatientUpdateButton->Create();
    this->PatientUpdateButton->SetText( "Update" );
    this->PatientUpdateButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    }
  
  
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->TableFrame->GetWidgetName() );
  
  this->Script( "grid %s -column 0 -row 0 -sticky w -padx 6 -pady 2", 
                this->TableScannerLabel->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 0 -sticky w -padx 1 -pady 2",
                this->TableScannerEntry->GetWidgetName() );
  
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
  
  
  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->PatientFrame->GetWidgetName() );
  
  this->Script( "grid %s -column 0 -row 0 -sticky w -padx 6 -pady 2", 
                this->PatientLabel->GetWidgetName() );
  this->Script( "grid %s -column 1 -row 0 -sticky w -padx 1 -pady 2",
                this->PatientEntry->GetWidgetName() );
  
  this->Script( "grid %s -column 2 -row 0 -sticky e -padx 6 -pady 2",
                this->PatientUpdateButton->GetWidgetName() );
  
  this->Script( "grid columnconfigure %s 0 -weight 1",
                this->PatientFrame->GetFrame()->GetWidgetName() );
  this->Script( "grid columnconfigure %s 1 -weight 3",
                this->PatientFrame->GetFrame()->GetWidgetName() );
  this->Script( "grid columnconfigure %s 2 -weight 1",
                this->PatientFrame->GetFrame()->GetWidgetName() );
  
  
  
    // Hardware selection.
  
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
  
  if ( ! this->HardwareCalibrationFrame->IsCreated() )
    {
    this->HardwareCalibrationFrame->SetParent( parent );
    this->HardwareCalibrationFrame->Create();
    this->HardwareCalibrationFrame->SetLabelText( "Hardware calibration" );
    
    
    this->FlipFrame->SetParent( this->HardwareCalibrationFrame->GetFrame() );
    this->FlipFrame->Create();
    
    this->VerticalFlipCheckButton->SetParent( this->FlipFrame );
    this->VerticalFlipCheckButton->Create();
    this->VerticalFlipCheckButton->SetLabelText( "Vertical Flip:" );
    
    this->HorizontalFlipCheckButton->SetParent( this->FlipFrame );
    this->HorizontalFlipCheckButton->Create();
    this->HorizontalFlipCheckButton->SetLabelText( "Horizontal Flip:" );
    
    
    this->MonPhySizeFrame->SetParent(
      this->HardwareCalibrationFrame->GetFrame() );
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
  OverlayHardware hardware =
    node->GetHardwareList()[ node->GetHardwareIndex() ];
  
    // Update UI fields.
  
  this->TableScannerEntry->SetValueAsDouble( node->GetTableAtScanner() );
  this->TableOverlayEntry->SetValueAsDouble( node->GetTableAtOverlay() );
  this->PatientEntry->SetValueAsDouble( node->GetPatientAtScanner() );
  
  this->HardwareMenu->GetWidget()->SetValue( hardware.Name.c_str() );
  
  this->HorizontalFlipCheckButton->GetWidget()->SetSelectedState(
          node->GetSecondMonitorHorizontalFlip() );
  this->VerticalFlipCheckButton->GetWidget()->SetSelectedState(
          node->GetSecondMonitorVerticalFlip() );
  
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
    this->GetGUI()->GetMRMLNode()->
                              SetSecondMonitorHorizontalFlip( ( bool ) value );
    
    this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
    }
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::VerticalFlipCallback( bool value )
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  
  if ( mrmlNode && this->GetGUI()->GetSecondaryMonitor() )
    {
    this->GetGUI()->GetMRMLNode()->
                                 SetSecondMonitorVerticalFlip( ( bool )value );
    
    this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
    }
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::UpdateAutoScaleCallback()
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  
  vtkMRMLScalarVolumeNode *inVolume = mrmlNode->GetPlanningVolumeNode();
  if ( ! inVolume )
    {
    // TO DO: what to do on failure
    return;
    }
   
  
  double mmX = this->MonPhySize->GetWidget( 0 )->GetValueAsDouble();
  double mmY = this->MonPhySize->GetWidget( 1 )->GetValueAsDouble();

  double pixX = this->MonPixRes->GetWidget( 0 )->GetValueAsDouble();
  double pixY = this->MonPixRes->GetWidget( 1 )->GetValueAsDouble();

    // set the values in secondary monitor
  this->GetGUI()->GetSecondaryMonitor()->SetPhysicalSize( mmX, mmY );
  this->GetGUI()->GetSecondaryMonitor()->SetPixelResolution( pixX, pixY );
  
  
  // calculate new scaling
  // note updating monitor physical size in previous lines of code,
  // will automatically update correct monitor spacing
  
  double monSpacing[ 2 ];
  this->GetGUI()->GetSecondaryMonitor()->GetMonitorSpacing(
    monSpacing[ 0 ], monSpacing[ 1 ] );
    
  double imgSpacing[ 3 ];
  inVolume->GetSpacing( imgSpacing );
}


//----------------------------------------------------------------------------
void
vtkPerkStationCalibrateStep
::ProcessKeyboardEvents( vtkObject *caller,
                         unsigned long event,
                         void *callData )
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
  
  if ( ! wizard_widget
       || wizard_widget->GetWizardWorkflow()->GetCurrentStep() != this )
    {
    return;
    }
  
  
  if ( this->ProcessingCallback )
    {
    return;
    }
  
  this->ProcessingCallback = true;
  
  
  vtkSlicerInteractorStyle *style =
    vtkSlicerInteractorStyle::SafeDownCast( caller );
  /*
  vtkSlicerInteractorStyle *istyle0 =
    vtkSlicerInteractorStyle::SafeDownCast(
      this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
      GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->
      GetInteractorStyle() );
  */
  
  
    // Image calibration on second monitor with focus on Red Slice.
  
  // todo: handle keys "Up" "Down" etc.
  
  double stepSize = 0.8;
  
  double translation[ 2 ];
  double rotation = this->GetGUI()->GetMRMLNode()->GetSecondMonitorRotation();
  this->GetGUI()->GetMRMLNode()->GetSecondMonitorTranslation(
    translation[ 0 ], translation[ 1 ] );
  
  if ( event == vtkCommand::KeyPressEvent )
    {
    char  *key = style->GetKeySym();
    if ( ! strcmp( key, "a" ) ) translation[ 1 ] -= stepSize;
    if ( ! strcmp( key, "z" ) ) translation[ 1 ] += stepSize;
    if ( ! strcmp( key, "q" ) ) translation[ 0 ] -= stepSize;
    if ( ! strcmp( key, "w" ) ) translation[ 0 ] += stepSize;
    if ( ! strcmp( key, "g" ) ) rotation += ( stepSize / 3.0 );
    if ( ! strcmp( key, "h" ) ) rotation -= ( stepSize / 3.0 );
    }
  
  this->GetGUI()->GetMRMLNode()->SetSecondMonitorTranslation(
        translation[ 0 ], translation[ 1 ] );
  this->GetGUI()->GetMRMLNode()->SetSecondMonitorRotation( rotation );
  
  this->ProcessingCallback = false;
}


//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::LoadCalibrationButtonCallback()
{
  std::string fileNameWithPath = this->CalibFileName;
  ifstream file( fileNameWithPath.c_str() );
  if ( file.is_open() )
    {
    this->LoadCalibration( file );
    file.close();
    }
  
  this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
}


//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::SaveCalibrationButtonCallback()
{
  std::string fileNameWithPath = this->CalibFileName;//+"/"+this->CalibFileName;
  ofstream file( fileNameWithPath.c_str() );
  if ( file.is_open() )
    {
    this->SaveCalibration( file );
    file.close();
    }
}


//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::LoadCalibration( istream& file )
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if ( ! mrmlNode )
    {
    return;
    }  

  bool success = mrmlNode->LoadCalibration( file );
  
  if ( success )
    {
    this->GetGUI()->GetSecondaryMonitor()->LoadCalibration();
    this->PopulateControlsOnLoadCalibration();
    }
  else
    {
    PERKLOG_ERROR( "Could not read calibration from file." );
    return;
    }
}


//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::SaveCalibration( ostream& of )
{
  // reset parameters at MRML node
   vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if ( ! mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }  
  
  mrmlNode->SaveClibration( of );
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::SuggestFileName()
{
}


//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::Reset()
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if ( ! mrmlNode )
    {
    return;
    }
  
  this->GetGUI()->GetMRMLNode()->SetTableAtOverlay( 0.0 );
  this->GetGUI()->GetMRMLNode()->SetTableAtScanner( 0.0 );
  this->GetGUI()->GetMRMLNode()->SetPatientAtScanner( 0.0 );
  this->GetGUI()->GetMRMLNode()->SetSecondMonitorTranslation( 0.0, 0.0 );
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



//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf( os,indent );
}


//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::AddGUIObservers()
{
  this->RemoveGUIObservers();
  
  
    // Table calibration.
  
  if ( this->TableOverlayEntry && this->TableScannerEntry )
    {
    this->TableOverlayEntry->AddObserver(
      vtkKWEntry::EntryValueChangedEvent,
      ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    
    this->TableScannerEntry->AddObserver(
      vtkKWEntry::EntryValueChangedEvent,
      ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  
  if ( this->TableUpdateButton )
    {
    this->TableUpdateButton->AddObserver(
      vtkKWPushButton::InvokedEvent,
      ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  
  if ( this->PatientEntry )
    {
    this->PatientEntry->AddObserver(
      vtkKWEntry::EntryValueChangedEvent,
      ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  
  if ( this->PatientUpdateButton )
    {
    this->PatientUpdateButton->AddObserver(
      vtkKWPushButton::InvokedEvent,
      ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  
  
    // Hardware selection.
  
  if ( this->HardwareMenu )
    {
    this->HardwareMenu->GetWidget()->GetMenu()->AddObserver(
      vtkKWMenu::MenuItemInvokedEvent,
      ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  
  
   // load reset components
  if (this->LoadCalibrationFileButton)
    {
    this->LoadCalibrationFileButton->GetLoadSaveDialog()->AddObserver(
      vtkKWTopLevel::WithdrawEvent,
      ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  if (this->ResetCalibrationButton)
    {
    this->ResetCalibrationButton->AddObserver( vtkKWPushButton::InvokedEvent,
    ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  
  
    // flip controls
  
  if (this->VerticalFlipCheckButton)
    {
    this->VerticalFlipCheckButton->GetWidget()->AddObserver(
      vtkKWCheckButton::SelectedStateChangedEvent,
      (vtkCommand*)( this->WizardGUICallbackCommand ) );
    }
  if (this->HorizontalFlipCheckButton)
    {
    this->HorizontalFlipCheckButton->GetWidget()->AddObserver(
      vtkKWCheckButton::SelectedStateChangedEvent,
      (vtkCommand*)( this->WizardGUICallbackCommand ) );
    }
  
  
  if ( this->HardwareUpdateButton )
    {
    this->HardwareUpdateButton->AddObserver(
      vtkKWPushButton::InvokedEvent,
      (vtkCommand*)( this->WizardGUICallbackCommand ) );  
    }
  
  
  if ( this->SaveCalibrationFileButton )
    {
    this->SaveCalibrationFileButton->GetLoadSaveDialog()->AddObserver(
      vtkKWTopLevel::WithdrawEvent,
      (vtkCommand *)this->WizardGUICallbackCommand );
    }
  
  this->ObserverCount++;
}


void
vtkPerkStationCalibrateStep
::RemoveGUIObservers()
{
    // load reset components
  
  if ( this->LoadCalibrationFileButton )
    {
    this->LoadCalibrationFileButton->GetLoadSaveDialog()->RemoveObservers(
      vtkKWTopLevel::WithdrawEvent,
      ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  if ( this->ResetCalibrationButton )
    {
    this->ResetCalibrationButton->RemoveObservers(
      vtkKWPushButton::InvokedEvent,
      ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  
  
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
  
  if ( this->HardwareUpdateButton )
    {
    this->HardwareUpdateButton->RemoveObservers(
      vtkKWPushButton::InvokedEvent,
      ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    }
  

    // save controls    
  
  if (this->SaveCalibrationFileButton)
    {
    this->SaveCalibrationFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
      
  
  this->ObserverCount--;
}


//----------------------------------------------------------------------------
void
vtkPerkStationCalibrateStep
::WizardGUICallback( vtkObject *caller,
                     unsigned long event,
                     void *clientData,
                     void *callData )
{
  vtkPerkStationCalibrateStep *self =
    reinterpret_cast< vtkPerkStationCalibrateStep *>( clientData );
  if (self)
    {
    self->ProcessGUIEvents( caller, event, callData );
    }
}


//----------------------------------------------------------------------------
void
vtkPerkStationCalibrateStep
::ProcessGUIEvents( vtkObject *caller, unsigned long event, void *callData )
{
  if ( this->ProcessingCallback ) return;
  this->ProcessingCallback = true;
  
  vtkMRMLPerkStationModuleNode* node = this->GetGUI()->GetMRMLNode();
  
  
    // Table entries changed.
  
  if (    this->TableOverlayEntry == vtkKWEntry::SafeDownCast( caller )
       || this->TableScannerEntry == vtkKWEntry::SafeDownCast( caller ) )
    {
    if (    this->TableOverlayEntry->GetValueAsDouble()
         != node->GetTableAtOverlay()
         || this->TableScannerEntry->GetValueAsDouble()
         != node->GetTableAtScanner() )
      {
      this->TableUpdateButton->SetBackgroundColor( 1.0, 0.8, 0.8 );
      }
    }
  
    // Table update button pressed.
  
  if ( this->TableUpdateButton == vtkKWPushButton::SafeDownCast( caller ) )
    {
    node->SetTableAtOverlay( this->TableOverlayEntry->GetValueAsDouble() );
    node->SetTableAtScanner( this->TableScannerEntry->GetValueAsDouble() );
    this->TableUpdateButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    }
  
    // Patient entry changed.
  
  if ( this->PatientEntry == vtkKWEntry::SafeDownCast( caller ) )
    {
    if (    this->PatientEntry->GetValueAsDouble()
         != node->GetPatientAtScanner() )
      {
      this->PatientUpdateButton->SetBackgroundColor( 1.0, 0.8, 0.8 );
      }
    }
  
    // Patient update button pressed.
  
  if ( this->PatientUpdateButton == vtkKWPushButton::SafeDownCast( caller ) )
    {
    node->SetPatientAtScanner( this->PatientEntry->GetValueAsDouble() );
    this->PatientUpdateButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    }
  
  
    // Hardware selection menu.
  
  if ( this->HardwareMenu->GetWidget()->GetMenu()
         == vtkKWMenu::SafeDownCast( caller )
       && event == vtkKWMenu::MenuItemInvokedEvent )
    {
    this->HardwareSelected( 
       this->HardwareMenu->GetWidget()->GetMenu()->GetIndexOfSelectedItem() );
    
    this->HardwareMenu->SetWidth( 200 );
    }
  
  
    // Hardware calibration.
  
  if ( this->HardwareUpdateButton
       && this->HardwareUpdateButton == vtkKWPushButton::SafeDownCast(caller)
       && ( event == vtkKWPushButton::InvokedEvent ) )
    {
    this->UpdateAutoScaleCallback();
    }
  
  
    // Continue only with valid MRMLPerkStation node.
  
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  
  if( ! mrmlNode )
    {
    this->ProcessingCallback = false;
    return;
    }
  
  
    // reset calib button
  
  if (    this->ResetCalibrationButton
       && this->ResetCalibrationButton == vtkKWPushButton::SafeDownCast(caller)
       && ( event == vtkKWPushButton::InvokedEvent ) )
    {
    this->Reset();
    }
  
  
    // load calib dialog button
  
  if ( this->LoadCalibrationFileButton
       && this->LoadCalibrationFileButton->GetLoadSaveDialog()
          == vtkKWLoadSaveDialog::SafeDownCast( caller )
       && ( event == vtkKWTopLevel::WithdrawEvent ) )
    {
    const char *fileName = this->LoadCalibrationFileButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      this->CalibFileName = std::string( fileName );
      this->LoadCalibrationButtonCallback();
      }
    // reset the file browse button text
    this->LoadCalibrationFileButton->SetText ( "Load calibration" );
    }
  
  
    // save calib dialog button
  
  if ( this->SaveCalibrationFileButton
       && this->SaveCalibrationFileButton->GetLoadSaveDialog()
          == vtkKWLoadSaveDialog::SafeDownCast( caller )
       && ( event == vtkKWTopLevel::WithdrawEvent ) )
    {
    const char *fileName = this->SaveCalibrationFileButton->
                           GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      this->CalibFileName = std::string(fileName) + ".xml";
      
        // get the file name and file path
      this->SaveCalibrationFileButton->GetLoadSaveDialog()->
            SaveLastPathToRegistry( "OpenPath" );
        
      // call the callback function
      this->SaveCalibrationButtonCallback();
      }
    // reset the file browse button text
    this->SaveCalibrationFileButton->SetText( "Save calibration" );
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
       && this->VerticalFlipCheckButton->GetWidget()
          == vtkKWCheckButton::SafeDownCast( caller )
       && (event == vtkKWCheckButton::SelectedStateChangedEvent ) )
    {
    this->VerticalFlipCallback(
      bool( this->VerticalFlipCheckButton->GetWidget()->GetSelectedState() ) );
    }
  
  
  if ( this->HorizontalFlipCheckButton
       && this->HorizontalFlipCheckButton->GetWidget()
          == vtkKWCheckButton::SafeDownCast( caller )
       && ( event == vtkKWCheckButton::SelectedStateChangedEvent ) )
    {
    this->HorizontalFlipCallback(
      bool( this->HorizontalFlipCheckButton->GetWidget()->GetSelectedState() ));
    }

  
  this->ProcessingCallback = false;
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::Validate()
{
  this->Superclass::Validate();

  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }

  // start the log timer??
  this->LogTimer->StopTimer();

  // log the time in mrml node
  mrmlNode->SetTimeSpentOnCalibrateStep(this->LogTimer->GetElapsedTime());

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
  
  this->GetGUI()->GetMRMLNode()->SetSecondMonitorHorizontalFlip(
    this->GetGUI()->GetMRMLNode()->GetHardwareList()[ index ].FlipHorizontal );
  
  this->GetGUI()->GetMRMLNode()->SetSecondMonitorVerticalFlip(
    this->GetGUI()->GetMRMLNode()->GetHardwareList()[ index ].FlipVertical );
  
  
  this->GetGUI()->GetSecondaryMonitor()->SetPhysicalSize(
          this->GetGUI()->GetMRMLNode()->GetHardwareList()[ index ].SizeX,
          this->GetGUI()->GetMRMLNode()->GetHardwareList()[ index ].SizeY );
  
  
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
  
  if ( node == NULL )
    {
    return;
    }
  
  this->HardwareMenu->GetWidget()->GetMenu()->DeleteAllItems();
  
  std::vector< OverlayHardware > list = node->GetHardwareList();
  
    // Update hardware list only if it changed.
  
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
      this->HardwareMenu->GetWidget()->GetMenu()->
              AddRadioButton( list[ i ].Name.c_str() );
      }
    }
  
  // this->HardwareMenu->GetWidget()->SetValue( list[ 0 ].Name.c_str() );
  // this->HardwareSelected( 0 );
  
  this->PopulateControls();
}
