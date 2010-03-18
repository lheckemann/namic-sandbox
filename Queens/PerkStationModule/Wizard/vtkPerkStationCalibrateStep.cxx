
#include "vtkPerkStationCalibrateStep.h"

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
  this->SecondMonitor = MONITOR_SIEMENS;
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
  switch (this->GetGUI()->GetMode())
    {
    case vtkPerkStationModuleGUI::Training:
      break;

    case vtkPerkStationModuleGUI::Clinical:
      this->LoadCalibrationFileButton->SetEnabled(enable);
      this->ResetCalibrationButton->SetEnabled(enable);
      break;
    }
}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::EnableDisableSaveControls(bool enable)
{
  switch (this->GetGUI()->GetMode())
    {
    case vtkPerkStationModuleGUI::Training:
      break;

    case vtkPerkStationModuleGUI::Clinical:
      this->SaveCalibrationFileButton->SetEnabled(enable);
      break;
    }
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
  
  this->PopulateControls();
    
  this->InstallCallbacks();
  
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
    GetSliceViewer()->Focus();
}

//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ShowLoadResetControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  
  // Create the frame
  if (!this->LoadResetFrame->IsCreated())
    {
    this->LoadResetFrame->SetParent( parent );
    // this->LoadResetFrame->GetLabel()->SetText( "File" );
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
    this->TableScannerLabel->SetText( "Scanner laser (mm):");
    
    this->TableScannerEntry->SetParent( this->TableFrame->GetFrame() );
    this->TableScannerEntry->Create();
    this->TableScannerEntry->SetWidth( 5 );
    
    this->TableOverlayLabel->SetParent( this->TableFrame->GetFrame() );
    this->TableOverlayLabel->Create();
    this->TableOverlayLabel->SetText( "Overlay laser (mm):");
    
    this->TableOverlayEntry->SetParent( this->TableFrame->GetFrame() );
    this->TableOverlayEntry->Create();
    this->TableOverlayEntry->SetWidth( 5 );
    
    this->TableUpdateButton->SetParent( this->TableFrame->GetFrame() );
    this->TableUpdateButton->Create();
    this->TableUpdateButton->SetText( "Update" );
    this->TableUpdateButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    }

  this->Script( "pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->TableFrame->GetWidgetName() );
  /*
  this->Script( "pack %s -side left -anchor nw -padx 6 -pady 2", 
                this->TableScannerLabel->GetWidgetName() );
  this->Script( "pack %s -side left -anchor nw -padx 1 -pady 2",
                this->TableScannerEntry->GetWidgetName() );
  */
  this->Script( "pack %s -side left -anchor nw -padx 6 -pady 2", 
                this->TableOverlayLabel->GetWidgetName() );
  this->Script( "pack %s -side left -anchor nw -padx 1 -pady 2",
                this->TableOverlayEntry->GetWidgetName() );
  this->Script( "pack %s -side right -anchor nw -padx 4 -pady 2",
                this->TableUpdateButton->GetWidgetName() );
  
  
    // Hardware selection.
  
  if ( ! this->HardwareFrame->IsCreated() )
    {
    this->HardwareFrame->SetParent( parent );
    this->HardwareFrame->Create();
    this->HardwareFrame->SetLabelText( "Overlay hardware" );
    
    this->HardwareMenu->SetParent( this->HardwareFrame->GetFrame() );
    this->HardwareMenu->Create();
    this->HardwareMenu->GetWidget()->SetWidth( 22 ); // What is the unit?
      // Make sure this is the same order as in enum OverlayMonitor.
    this->HardwareMenu->GetWidget()->GetMenu()->
      AddRadioButton( "Siemens MR compatible" );
    this->HardwareMenu->GetWidget()->GetMenu()->
      AddRadioButton( "PerkStation ViewSonic" );
    this->HardwareMenu->GetWidget()->GetMenu()->
      AddRadioButton( "Acer X203w" );
    this->HardwareMenu->GetWidget()->SetValue( "Siemens MR compatible" );
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
}

//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::InstallCallbacks()
{
  this->AddGUIObservers();
}


//----------------------------------------------------------------------------
void
vtkPerkStationCalibrateStep
::PopulateControls()
{
  // get all the input information that is needed to populate the controls

  // that information is in mrml node, which has input volume reference,
  // and other parameters which will be get/set
  
  
  if (    ( ! this->GetGUI()->GetLogic() )
       || ( ! this->GetGUI()->GetLogic()->GetPerkStationModuleNode() )
       || ( ! this->GetGUI()->GetSecondaryMonitor() ) )
    {
    return;
    }
  
  
    // Update UI fields.
  
  this->HorizontalFlipCheckButton->GetWidget()->SetSelectedState(
    this->GetGUI()->GetLogic()->GetPerkStationModuleNode()->GetHorizontalFlip());
  this->VerticalFlipCheckButton->GetWidget()->SetSelectedState(
    this->GetGUI()->GetLogic()->GetPerkStationModuleNode()->GetVerticalFlip() );
  
  double monPhySize[ 2 ];
  this->GetGUI()->GetSecondaryMonitor()->GetPhysicalSize(
    monPhySize[ 0 ], monPhySize[ 1 ] );
  this->MonPhySize->GetWidget( 0 )->SetValueAsDouble( monPhySize[ 0 ] );
  this->MonPhySize->GetWidget( 1 )->SetValueAsDouble( monPhySize[ 1 ] );
  
  double monPixRes[ 2 ];
  this->GetGUI()->GetSecondaryMonitor()->GetPixelResolution(
    monPixRes[ 0 ], monPixRes[ 1 ] );
  this->MonPixRes->GetWidget( 0 )->SetValueAsDouble( monPixRes[ 0 ] );
  this->MonPixRes->GetWidget( 1 )->SetValueAsDouble( monPixRes[ 1 ] );
  
  
    // Input specific settings.
    // TODO: Remove it from here.
  
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if ( ! mrmlNode )
    {
    // TO DO: what to do on failure
    return;
    }
  
  vtkMRMLScalarVolumeNode *inVolume = mrmlNode->GetPlanningVolumeNode();
  if (!inVolume)
    {
    // TO DO: what to do on failure
    return;
    }
 
  
  double monSpacing[ 2 ];
  this->GetGUI()->GetSecondaryMonitor()->GetMonitorSpacing(
    monSpacing[ 0 ], monSpacing[ 1 ] );
  
  double imgSpacing[ 3 ];
  inVolume->GetSpacing( imgSpacing );

  
  // set the actual scaling (image/mon) in mrml node
  mrmlNode->SetActualScaling( double( imgSpacing[ 0 ] / monSpacing[ 0 ] ),
                              double( imgSpacing[ 1 ] / monSpacing[ 1 ] ) );
  
  
  // start the log timer??
  this->LogTimer->StartTimer();

  // Q: do we want to reset timer on reset of calibration?
}

//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::PopulateControlsOnLoadCalibration()
{
  // get all the input information that is needed to populate the controls

  // that information is in mrml node, which has input volume reference, and other parameters which will be get/set

  // first get the input volume/slice
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }

  vtkMRMLScalarVolumeNode *inVolume = mrmlNode->GetPlanningVolumeNode();
  if (!inVolume)
    {
    // TO DO: what to do on failure
    return;
    }
 
 
  // populate flip frame components
  this->VerticalFlipCheckButton->GetWidget()->SetSelectedState(
    mrmlNode->GetVerticalFlip() );
  this->HorizontalFlipCheckButton->GetWidget()->SetSelectedState(
    mrmlNode->GetHorizontalFlip() );

  // scale components
  double monPhySize[2];
  this->GetGUI()->GetSecondaryMonitor()->GetPhysicalSize(monPhySize[0], monPhySize[1]);
  this->MonPhySize->GetWidget(0)->SetValueAsDouble(monPhySize[0]);
  this->MonPhySize->GetWidget(1)->SetValueAsDouble(monPhySize[1]);

  double monPixRes[2];
  this->GetGUI()->GetSecondaryMonitor()->GetPixelResolution(monPixRes[0], monPixRes[1]);
  this->MonPixRes->GetWidget(0)->SetValueAsDouble(monPixRes[0]);
  this->MonPixRes->GetWidget(1)->SetValueAsDouble(monPixRes[1]);

  // actual scaling already calculated and set inside LoadCalibrationFromFile function of SecondaryMonitor

  this->GetGUI()->GetWizardWidget()->SetErrorText( "");
  this->GetGUI()->GetWizardWidget()->Update();
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::HorizontalFlipCallback( bool value )
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if ( mrmlNode && this->GetGUI()->GetSecondaryMonitor() )
    {
    this->GetGUI()->GetMRMLNode()->SetHorizontalFlip( ( bool ) value );
    this->GetGUI()->GetSecondaryMonitor()->SetHorizontalFlip( value );
    }
}



//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::VerticalFlipCallback( bool value )
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  
  if ( mrmlNode && this->GetGUI()->GetSecondaryMonitor() )
    {
    this->GetGUI()->GetMRMLNode()->SetVerticalFlip((bool)value);
    this->GetGUI()->GetSecondaryMonitor()->SetVerticalFlip( value );
    }
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::UpdateAutoScaleCallback()
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  
  vtkMRMLScalarVolumeNode *inVolume = mrmlNode->GetPlanningVolumeNode();
  if (!inVolume)
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
      
    // set the actual scaling (image/mon) in mrml node
  mrmlNode->SetActualScaling( double( imgSpacing[ 0 ] / monSpacing[ 0 ] ),
                              double( imgSpacing[ 1 ] / monSpacing[ 1 ] ) );

  
  // reset current & rescale to new scaling
  // reset flip/scale/rotate/translate on secondary display
  this->GetGUI()->GetSecondaryMonitor()->ResetCalibration();
}


//----------------------------------------------------------------------------
void
vtkPerkStationCalibrateStep
::ProcessKeyboardEvents( vtkObject *caller,
                         unsigned long event,
                         void *callData )
{
  if(!this->GetGUI()->GetMRMLNode())
      return;
  
  if( ! this->GetGUI()->GetMRMLNode()->GetPlanningVolumeNode()
      || strcmp(this->GetGUI()->GetMRMLNode()->GetVolumeInUse(), "Planning")!=0)
      return;

  // has to be in clinical mode
  if (this->GetGUI()->GetMode()!= vtkPerkStationModuleGUI::Clinical)
    return;

  
  // has to be when it is in 
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  
  if ( ! wizard_widget
       || wizard_widget->GetWizardWorkflow()->GetCurrentStep() != this )
    {
    return;
    }
  
  
  if (this->ProcessingCallback)
    {
    return;
    }
  
  this->ProcessingCallback = true;

  vtkSlicerInteractorStyle *style =
    vtkSlicerInteractorStyle::SafeDownCast( caller );
  vtkSlicerInteractorStyle *istyle0 =
    vtkSlicerInteractorStyle::SafeDownCast(
      this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI( "Red" )->
      GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->
      GetInteractorStyle() );
  
  
    // Image calibration on second monitor with focus on Red Slice.
  
  // todo: handle keys "Up" "Down" etc.
  
  double stepSize = 0.8;
  
  double translation[ 2 ];
  double rotation;
  this->GetGUI()->GetSecondaryMonitor()->GetTranslation(
    translation[ 0 ], translation[ 1 ] );
  this->GetGUI()->GetSecondaryMonitor()->GetRotation( rotation );
  
  if ( event == vtkCommand::KeyPressEvent )
    {
    char  *key = style->GetKeySym();
    if ( ! strcmp( key, "a" ) ) translation[ 1 ] += stepSize;
    if ( ! strcmp( key, "z" ) ) translation[ 1 ] -= stepSize;
    if ( ! strcmp( key, "q" ) ) translation[ 0 ] -= stepSize;
    if ( ! strcmp( key, "w" ) ) translation[ 0 ] += stepSize;
    if ( ! strcmp( key, "g" ) ) rotation += ( stepSize / 3.0 );
    if ( ! strcmp( key, "h" ) ) rotation -= ( stepSize / 3.0 );
    }
  
  this->GetGUI()->GetSecondaryMonitor()->SetTranslation(
        translation[ 0 ], translation[ 1 ] );
  this->GetGUI()->GetSecondaryMonitor()->SetRotation( rotation );
  
  this->ProcessingCallback = false;
}


//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::LoadCalibrationButtonCallback()
{
  std::string fileNameWithPath = this->CalibFileName;
  ifstream file(fileNameWithPath.c_str());
  
  this->LoadCalibration(file);
  file.close();
}


//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::SaveCalibrationButtonCallback()
{
  std::string fileNameWithPath = this->CalibFileName;//+"/"+this->CalibFileName;
  ofstream file(fileNameWithPath.c_str());

  this->SaveCalibration(file);
  
  file.close();
}


//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::LoadCalibration(istream &file)
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }  


  switch (this->GetGUI()->GetMode())      
    {

    case vtkPerkStationModuleGUI::Training:
     
      break;

    case vtkPerkStationModuleGUI::Clinical:
       
      // in clinical mode
      char currentLine[256];  
      char* attName = "";
      char* attValue = "";
      char* pdest;
      int nCharCount = 0;
      unsigned int indexEndOfAttribute = 0;
      unsigned int indexStartOfValue = 0;
      unsigned int indexEndOfValue = 0;

      int paramSetCount = 0;
      while(!file.eof())
        {
        // first get each line,
        // then parse each line on basis of attName, and attValue
        // this can be done as delimiters '='[]' is used to separate out name from value
        file.getline(&currentLine[0], 256, '\n');   
        nCharCount = strlen(currentLine);
        indexEndOfAttribute = strcspn(currentLine,"=");
        if(indexEndOfAttribute >0)
          {
          attName = new char[indexEndOfAttribute+1];
          strncpy(attName, currentLine,indexEndOfAttribute);
          attName[indexEndOfAttribute] = '\0';
          pdest = strchr(currentLine, '"');   
          indexStartOfValue = (int)(pdest - currentLine + 1);
          pdest = strrchr(currentLine, '"');
          indexEndOfValue = (int)(pdest - currentLine + 1);
          attValue = new char[indexEndOfValue-indexStartOfValue+1];
          strncpy(attValue, &currentLine[indexStartOfValue], indexEndOfValue-indexStartOfValue-1);
          attValue[indexEndOfValue-indexStartOfValue-1] = '\0';

          // at this point, we have line separated into, attributeName, and attributeValue
          // now we need to do string matching on attributeName, and further parse attributeValue as it may have more than one value
          if (!strcmp(attName, " VerticalFlip"))
              {
              std::stringstream ss;
              ss << attValue;
              bool val;
              ss >> val;
              mrmlNode->SetVerticalFlip(val);
              paramSetCount++;
              }
          else if (!strcmp(attName, " HorizontalFlip"))
              {
              std::stringstream ss;
              ss << attValue;
              bool val;
              ss >> val;
              mrmlNode->SetHorizontalFlip(val);
              paramSetCount++;
              }
          else if (!strcmp(attName, " CenterOfRotation"))
              {
              // read data into a temporary vector
              std::stringstream ss;
              ss << attValue;
              double d;
              std::vector<double> tmpVec;
              while (ss >> d)
                {
                tmpVec.push_back(d);
                }
              if (tmpVec.size()==3)
                {
                double cor[3];
                for (unsigned int i = 0; i < tmpVec.size(); i++)
                    cor[i] = tmpVec[i];
                mrmlNode->SetCenterOfRotation(cor);
                paramSetCount++;
                }
              else
                {
                // error in file?
                }   
              }
          else if (!strcmp(attName, " Rotation"))
              {
              std::stringstream ss;
              ss << attValue;
              double val;
              ss >> val;
              mrmlNode->SetClinicalModeRotation(val);       
              paramSetCount++;
              }
          else if (!strcmp(attName, " PhysicalSizeMMs"))
              {
              // read data into a temporary vector
              std::stringstream ss;
              ss << attValue;
              double d;
              std::vector<double> tmpVec;
              while (ss >> d)
                {
                tmpVec.push_back(d);
                }
              if (tmpVec.size()==2)
                {
                double mm[2];
                for (unsigned int i = 0; i < tmpVec.size(); i++)
                    mm[i] = tmpVec[i];
                this->GetGUI()->GetSecondaryMonitor()->SetPhysicalSize(mm[0], mm[1]);
                paramSetCount++;
                }
              else
                {
                // error in file?
                }       
              }
          else if (!strcmp(attName, " PixelResolution"))
            {
            // read data into a temporary vector
            std::stringstream ss;
            ss << attValue;
            double d;
            std::vector<double> tmpVec;
            while (ss >> d)
              {
              tmpVec.push_back(d);
              }
            if (tmpVec.size()==2)
              {
              double pix[2];
              for (unsigned int i = 0; i < tmpVec.size(); i++)
                pix[i] = tmpVec[i];
              this->GetGUI()->GetSecondaryMonitor()->SetPixelResolution(pix[0], pix[1]);
              paramSetCount++;
              }
            else
              {
              // error in file?
              }     
            }
          else if (!strcmp(attName, " Translation"))
            {
            // read data into a temporary vector
            std::stringstream ss;
            ss << attValue;
            double d;
            std::vector<double> tmpVec;
            while (ss >> d)
              {
              tmpVec.push_back(d);
              }
            if (tmpVec.size()==2)
              {
              double trans[2];
              for (unsigned int i = 0; i < tmpVec.size(); i++)
                trans[i] = tmpVec[i];
              mrmlNode->SetClinicalModeTranslation(trans);
              paramSetCount++;
              }
            else
              {
              // error in file?
              }     
            }
          }// end if testing for it is a valid attName

        } // end while going through the file
      if (paramSetCount == 7)
        {
        // all params correctly read from file
        this->GetGUI()->GetSecondaryMonitor()->LoadCalibration();
        // reflect the values of params in GUI controls
        this->PopulateControlsOnLoadCalibration();
        // set any state variables required to be set
        }
      else
        {
        // error reading file, not all values set
        int error = -1;
        }
   
      break;
    }
  
}

//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::SaveCalibration( ostream& of )
{
  // reset parameters at MRML node
   vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }  
  
  // flip parameters
  of << " VerticalFlip=\"" << mrmlNode->GetVerticalFlip()
     << "\" \n";
      
  of << " HorizontalFlip=\"" << mrmlNode->GetHorizontalFlip()
     << "\" \n";

  double monPhySize[ 2 ];
  this->GetGUI()->GetSecondaryMonitor()->GetPhysicalSize(
    monPhySize[ 0 ], monPhySize[ 1 ] );
  of << " PhysicalSizeMMs=\"";
  for( int i = 0; i < 2; i++ )
    of << monPhySize[ i ] << " ";
  of << "\" \n";
  
  double monPixResolution[ 2 ];
  this->GetGUI()->GetSecondaryMonitor()->GetPixelResolution(
    monPixResolution[ 0 ], monPixResolution[ 1 ] );
  of << " PixelResolution=\"";
  for( int i = 0; i < 2; i++ )
    of << monPixResolution[ i ] << " ";
  of << "\" \n";

  of << " CenterOfRotation=\"";
  for( int i = 0; i < 3; i++ )
    of << mrmlNode->GetCenterOfRotation()[ i ] << " ";
  of << "\" \n";

  of << " Rotation=\"";
  double rotation;
  this->GetGUI()->GetSecondaryMonitor()->GetRotation( rotation );
  of << rotation << " ";
  of << "\" \n";

  of << " Translation=\"";
  double tX, tY;
  this->GetGUI()->GetSecondaryMonitor()->GetTranslation( tX, tY );
  // mrmlNode->SetClinicalModeTranslation( this->Translation );
  of << tX << " " << tY << " ";
  of << "\" \n";
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::SuggestFileName()
{
}


//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::Reset()
{
  // reset flip/scale/rotate/translate on secondary display
  this->GetGUI()->GetSecondaryMonitor()->ResetCalibration();

  // reset parameters at MRML node
   vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }
  
  
  this->TableOverlayEntry->SetValueAsDouble( 0.0 );
  this->TableScannerEntry->SetValueAsDouble( 0.0 );
  this->HardwareMenu->GetWidget()->SetValue( "Siemens MR compatible" );
  this->HardwareSelected( MONITOR_SIEMENS );
  
  
  vtkMRMLScalarVolumeNode *inVolume = mrmlNode->GetPlanningVolumeNode();
  if (!inVolume)
    {
    // TO DO: what to do on failure
    return;
    }
  
  this->GetGUI()->GetSecondaryMonitor()->SetTranslation( 0.0, 0.0 );
  this->GetGUI()->GetSecondaryMonitor()->SetRotation( 0.0 );
  this->GetGUI()->GetSecondaryMonitor()->UpdateImageDisplay();
  
  
  /*  
  // flip
  mrmlNode->SetHorizontalFlip(false);
  mrmlNode->SetVerticalFlip(false);
  // scale
  mrmlNode->SetUserScaling(1.0,1.0);
  mrmlNode->SetActualScaling(1.0,1.0);
  mrmlNode->CalculateCalibrateScaleError();
  // translate
  mrmlNode->SetClinicalModeTranslation(0.0,0.0);
  mrmlNode->SetUserTranslation(0.0,0.0,0.0);
  mrmlNode->SetActualTranslation(0.0,0.0,0.0);
  mrmlNode->CalculateCalibrateTranslationError();
  // rotate
  mrmlNode->SetClinicalModeRotation(0.0);
  mrmlNode->SetCenterOfRotation(0.0,0.0,0.0);
  mrmlNode->SetUserRotation(0.0);
  mrmlNode->SetActualRotation(0.0);
  mrmlNode->CalculateCalibrateRotationError();
  */
  
    // reset member variables to defaults
  this->ClickNumber = 0;
  this->PopulateControls();
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
  
  
  // save controls
  if (this->SaveCalibrationFileButton)
    {
    this->SaveCalibrationFileButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
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
  
  
    // Table calibration.
  
  if (    this->TableOverlayEntry == vtkKWEntry::SafeDownCast( caller )
       || this->TableScannerEntry == vtkKWEntry::SafeDownCast( caller ) )
    {
    // TODO: This coloring to remind press update would be a good idea.
    // this->TableUpdateButton->SetBackgroundColor( 1.0, 0.9, 0.9 );
    }
  
  if ( this->TableUpdateButton == vtkKWPushButton::SafeDownCast( caller ) )
    {
    this->GetGUI()->GetMRMLNode()->SetTableAtOverlay(
      this->TableOverlayEntry->GetValueAsDouble() );
    this->GetGUI()->GetMRMLNode()->SetTableAtScanner(
      this->TableScannerEntry->GetValueAsDouble() );
    
    // this->TableUpdateButton->SetBackgroundColor( 0.85, 0.85, 0.85 );
    }
  
  
    // Hardware selection menu.
  
  if ( this->HardwareMenu->GetWidget()->GetMenu()
         == vtkKWMenu::SafeDownCast( caller )
       && event == vtkKWMenu::MenuItemInvokedEvent )
    {
      // Make sure the HardwareMenu is set up according to OverlayMonitor.
    this->HardwareSelected( ( OverlayMonitor )(
      this->HardwareMenu->GetWidget()->GetMenu()->GetIndexOfSelectedItem() ) );
    
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
  
  
    // Continue only with panning volume loaded.
  
  if( ! mrmlNode->GetPlanningVolumeNode()
      || strcmp( mrmlNode->GetVolumeInUse(), "Planning" ) != 0 )
    {
    this->ProcessingCallback = false;
    return;
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
      //this->CalibFilePath = std::string(this->LoadCalibrationFileButton->GetLoadSaveDialog()->GetLastPath());
      // indicates ok has been pressed with a file name
      this->CalibFileName = std::string(fileName);
      // call the callback function
      this->LoadCalibrationButtonCallback();
      }
    // reset the file browse button text
    this->LoadCalibrationFileButton->SetText ("Load calibration");
    }
  
  
    // save calib dialog button
  
  if ( this->SaveCalibrationFileButton
       && this->SaveCalibrationFileButton->GetLoadSaveDialog()
          == vtkKWLoadSaveDialog::SafeDownCast( caller )
       && ( event == vtkKWTopLevel::WithdrawEvent ) )
    {
    const char *fileName = this->SaveCalibrationFileButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      this->CalibFileName = std::string(fileName) + ".xml";
      // get the file name and file path
      this->SaveCalibrationFileButton->GetLoadSaveDialog()->SaveLastPathToRegistry("OpenPath");
        
      // call the callback function
      this->SaveCalibrationButtonCallback();
      }
    // reset the file browse button text
    this->SaveCalibrationFileButton->SetText( "Save calibration" );
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


OverlayMonitor
vtkPerkStationCalibrateStep
::GetOverlayMonitor()
{
  return this->SecondMonitor;
}


/**
 * Currently, hardware parameters are hardcoded into this function.
 * TODO: Read hardware parameters from an xml file.
 */
void
vtkPerkStationCalibrateStep
::HardwareSelected( OverlayMonitor monitor )
{
  this->SecondMonitor = monitor;
  
  switch ( monitor )
    {
    case MONITOR_SIEMENS :
      this->GetGUI()->GetSecondaryMonitor()->SetPhysicalSize( 360, 290 );
      this->GetGUI()->GetLogic()->GetPerkStationModuleNode()->
            SetHorizontalFlip( true );
      this->GetGUI()->GetLogic()->GetPerkStationModuleNode()->
            SetVerticalFlip( false );
      break;
    
    case MONITOR_VIEWSONIC :
      this->GetGUI()->GetSecondaryMonitor()->SetPhysicalSize( 305, 228 );
      this->GetGUI()->GetLogic()->GetPerkStationModuleNode()->
            SetHorizontalFlip( true );
      this->GetGUI()->GetLogic()->GetPerkStationModuleNode()->
            SetVerticalFlip( false );
      break;
    
    case MONITOR_ACER :
      this->GetGUI()->GetSecondaryMonitor()->SetPhysicalSize( 433.4, 270.9 );
      this->GetGUI()->GetLogic()->GetPerkStationModuleNode()->
        SetHorizontalFlip( false );
      this->GetGUI()->GetLogic()->GetPerkStationModuleNode()->
        SetVerticalFlip( false );
      break;
    }
  
  this->PopulateControls();
  
    // Update calibration data from new MRMLPerkStationModuleNode data.
  this->GetGUI()->GetSecondaryMonitor()->UpdateCalibration();
}
