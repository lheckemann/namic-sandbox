#include "vtkPerkStationInsertStep.h"

#include "vtkPerkStationModuleGUI.h"
#include "vtkPerkStationModuleLogic.h"

#include "vtkKWCheckButton.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWEntrySet.h"
#include "vtkKWLabel.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationInsertStep);
vtkCxxRevisionMacro(vtkPerkStationInsertStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkPerkStationInsertStep::vtkPerkStationInsertStep()
{
  this->SetName("3/5. Insert");
  this->SetDescription("Do the needle insertion");

  this->ConnectTrackerCheckButton = NULL;
  this->NeedleTipPositionFrame = NULL;
  this->NeedleTipPositionLabel = NULL;
  this->NeedleTipPosition = NULL;
  

}

//----------------------------------------------------------------------------
vtkPerkStationInsertStep::~vtkPerkStationInsertStep()
{
  if(this->ConnectTrackerCheckButton)
    {
    this->ConnectTrackerCheckButton->Delete();
    this->ConnectTrackerCheckButton = NULL;
    }
  if(this->NeedleTipPositionFrame)
    {
    this->NeedleTipPositionFrame->Delete();
    this->NeedleTipPositionFrame = NULL;
    }
  if(this->NeedleTipPositionLabel)
    {
    this->NeedleTipPositionLabel->Delete();
    this->NeedleTipPositionLabel = NULL;
    }
  
  if(this->NeedleTipPosition)
    {
    this->NeedleTipPosition->DeleteAllWidgets();
    this->NeedleTipPosition = NULL;
    }
  
}

//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);

  vtkKWWidget *parent = wizard_widget->GetClientArea();

  // Connect to tracker check button : this will try to establish connection in callback, and receive data in another callback

  if (!this->ConnectTrackerCheckButton)
    {
    this->ConnectTrackerCheckButton = vtkKWCheckButtonWithLabel::New();
    }
  if (!this->ConnectTrackerCheckButton->IsCreated())
    {
    this->ConnectTrackerCheckButton->SetParent(parent);
    this->ConnectTrackerCheckButton->Create();
    this->ConnectTrackerCheckButton->GetLabel()->SetBackgroundColor(0.7,0.7,0.7);
    this->ConnectTrackerCheckButton->SetLabelText("Connect to tracker:");
    this->ConnectTrackerCheckButton->SetHeight(4);
    }
 
  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->ConnectTrackerCheckButton->GetWidgetName());
  
  /*this->Script("grid %s -column 0 -row 0 -sticky nw -padx 2 -pady 2", 
               this->ConnectTrackerCheckButton->GetWidgetName());*/
  
  
  if (!this->NeedleTipPositionFrame)
    {
    this->NeedleTipPositionFrame = vtkKWFrame::New();
    }
  if (!this->NeedleTipPositionFrame->IsCreated())
    {
    this->NeedleTipPositionFrame->SetParent(parent);
    this->NeedleTipPositionFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
               this->NeedleTipPositionFrame->GetWidgetName());


  // label

  if (!this->NeedleTipPositionLabel)
    { 
    this->NeedleTipPositionLabel = vtkKWLabel::New();
    }
  if (!this->NeedleTipPositionLabel->IsCreated())
    {
    this->NeedleTipPositionLabel->SetParent(this->NeedleTipPositionFrame);
    this->NeedleTipPositionLabel->Create();
    this->NeedleTipPositionLabel->SetText("Needle tip position: ");
    this->NeedleTipPositionLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }

  
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->NeedleTipPositionLabel->GetWidgetName());
  
  // Needle tip position: will get populated/updated in a callback, which receives data from tracker
 
  if (!this->NeedleTipPosition)
    {
    this->NeedleTipPosition =  vtkKWEntrySet::New();    
    }
  if (!this->NeedleTipPosition->IsCreated())
    {
    this->NeedleTipPosition->SetParent(this->NeedleTipPositionFrame);
    this->NeedleTipPosition->Create();
    this->NeedleTipPosition->SetBorderWidth(2);
    this->NeedleTipPosition->SetReliefToGroove();
    this->NeedleTipPosition->SetPackHorizontally(1);
    this->NeedleTipPosition->SetMaximumNumberOfWidgetsInPackingDirection(3);
    // two entries of image spacing (x, y)
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->NeedleTipPosition->AddWidget(id);     
      entry->SetWidth(7);
      //entry->ReadOnlyOn();      
      }
    }

  this->Script("pack %s -side top  -anchor nw -padx 2 -pady 2", 
                this->NeedleTipPosition->GetWidgetName());

 
   // TO DO: install callbacks
  this->InstallCallbacks();

  // TO DO: populate controls wherever needed
  this->PopulateControls();

}

//----------------------------------------------------------------------------
/*void vtkPerkStationInsertStep::
  PopulateNormalizationTargetVolumeSelector()
{  
  vtkIdType target_vol_id;
  char buffer[256];

  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
  int nb_of_target_volumes = mrmlManager->GetTargetNumberOfSelectedVolumes();
  
  vtkKWMenu* menu = this->NormalizationTargetVolumeMenuButton->
    GetWidget()->GetMenu();
  menu->DeleteAllItems();

  // Update the target volume list in the menu button

  for(int i = 0; i < nb_of_target_volumes; i++)
    {
    target_vol_id = mrmlManager->GetTargetSelectedVolumeNthID(i);
    sprintf(buffer, "%s %d", 
            "NormalizationTargetSelectionChangedCallback", 
            static_cast<int>(target_vol_id));
    const char *name = mrmlManager->GetVolumeName(target_vol_id);
    if (name)
      {
      menu->AddRadioButton(name, this, buffer);
      }
    }
}
*/
//----------------------------------------------------------------------------
/*void vtkPerkStationInsertStep::
  NormalizationTargetSelectionChangedCallback(vtkIdType target_vol_id)
{
  // The target volumes have changed because of user interaction

  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
  
  char buffer[256];
  int parentEnabled = 
    this->NormalizationParametersFrame->GetEnabled();

  // Update Normalization-Enable check button

  this->NormalizationEnableCheckButton->SetEnabled(parentEnabled);  
  vtkKWCheckButton *cbEnable = 
    this->NormalizationEnableCheckButton->GetWidget();
  sprintf(buffer, "NormalizationEnableCallback %d", 
          static_cast<int>(target_vol_id));
  cbEnable->SetCommand(this, buffer);
  cbEnable->SetSelectedState(mrmlManager->
    GetTargetVolumeIntensityNormalizationEnabled(target_vol_id));
  this->NormalizationEnableCallback(
    target_vol_id, cbEnable->GetSelectedState());
}
*/
//----------------------------------------------------------------------------
/*void vtkPerkStationInsertStep::
  NormalizationEnableCallback(vtkIdType target_vol_id, int checked)
{
  // The target volume enabled checkbutton has changed 
  // because of user interaction

  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
  mrmlManager->SetTargetVolumeIntensityNormalizationEnabled(target_vol_id, 
    checked);

  int iEnabled = 
    this->NormalizationParametersFrame->GetEnabled();
  if(!checked)
    {
    iEnabled = 0;
    }
  this->NormalizationDefaultsMenuButton->SetEnabled(iEnabled);
  this->NormalizationPrintCheckButton->SetEnabled(iEnabled);
  this->NormalizationNormValueEntry->SetEnabled(iEnabled);
  this->NormalizationSmoothingWidthEntry->SetEnabled(iEnabled);
  this->NormalizationMaxSmoothingWidthEntry->SetEnabled(iEnabled);
  this->NormalizationRelativeMaxVoxelScale->SetEnabled(iEnabled);

  if(!iEnabled)
    {
    return;
    }

  // Update Normalization default settings drop down
  char buffer[256];

  this->NormalizationDefaultsMenuButton->GetMenu()->DeleteAllItems();
  sprintf(buffer, "NormalizationNormTypeCallback %d %d", 
          static_cast<int>(target_vol_id), NormalizationDefaultT1SPGR);
  this->NormalizationDefaultsMenuButton->
    GetMenu()->AddRadioButton("MR T1 SPGR", this, buffer);
  sprintf(buffer, "NormalizationNormTypeCallback %d %d", 
          static_cast<int>(target_vol_id), NormalizationDefaultT2);
  this->NormalizationDefaultsMenuButton->
    GetMenu()->AddRadioButton("MR T2", this, buffer);

  // Update Print-Info check button

  sprintf(buffer, "NormalizationPrintInfoCallback %d", 
          static_cast<int>(target_vol_id));
  this->NormalizationPrintCheckButton->GetWidget()->SetCommand(
    this, buffer);

  // Update Norm-value entry

  vtkKWEntry *entry = this->NormalizationNormValueEntry->GetWidget();
  sprintf(buffer, "NormalizationNormValueCallback %d", 
          static_cast<int>(target_vol_id));
  entry->SetCommand(this, buffer);

  // Update Histogram-smoothing-width

  entry = this->NormalizationSmoothingWidthEntry->GetWidget();
  sprintf(buffer, "NormalizationSmoothingWidthCallback %d", 
          static_cast<int>(target_vol_id));
  entry->SetCommand(this, buffer);

  // Update Max Histogram-smoothing-width

  entry = this->NormalizationMaxSmoothingWidthEntry->GetWidget();
  sprintf(buffer, "NormalizationMaxSmoothingWidthCallback %d", 
          static_cast<int>(target_vol_id));
  entry->SetCommand(this, buffer);

  // Update the Relative-Max-Voxel Num scale entry.

  sprintf(
    buffer, "NormalizationRelativeMaxVoxelNumCallback %d", 
    static_cast<int>(target_vol_id));
  this->NormalizationRelativeMaxVoxelScale->SetEndCommand(this, buffer);
  this->NormalizationRelativeMaxVoxelScale->SetEntryCommand(this, buffer);

  this->ResetDefaultParameters(target_vol_id);

  int parentEnabled = 
    this->NormalizationParametersFrame->GetEnabled();
  vtkKWCheckButton *cbEnable = 
    this->NormalizationEnableCheckButton->GetWidget();

  this->NormalizationDefaultsMenuButton->SetEnabled(
    cbEnable->GetSelectedState() ? parentEnabled : 0);
}
*/
//----------------------------------------------------------------------------
/*void vtkPerkStationInsertStep::
  NormalizationNormTypeCallback(vtkIdType target_vol_id,
  int enumDefaultsetting)
{
  // The target volume default settings dropdown has changed 
  // because of user interaction

  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
  
  if (enumDefaultsetting == NormalizationDefaultT1SPGR)
    {
    mrmlManager->SetTargetVolumeIntensityNormalizationToDefaultT1SPGR(
      target_vol_id);
    }
  else if (enumDefaultsetting == NormalizationDefaultT2)
    {
    mrmlManager->SetTargetVolumeIntensityNormalizationToDefaultT2(
      target_vol_id);
    }
  this->ResetDefaultParameters(target_vol_id);
}
*/
//----------------------------------------------------------------------------
/*void vtkPerkStationInsertStep::
  NormalizationPrintInfoCallback(
  vtkIdType target_vol_id, int checked)
{
  // The print-info checkbutton has changed 
  // because of user interaction

  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
 
  mrmlManager->SetTargetVolumeIntensityNormalizationPrintInfo(
    target_vol_id, checked);

}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationInsertStep::
  NormalizationNormValueCallback(vtkIdType target_vol_id, double dValue)
{
  // The Norm-value entry has changed 
  // because of user interaction
  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
 
  mrmlManager->
    SetTargetVolumeIntensityNormalizationNormValue(target_vol_id, dValue);

}*/


//----------------------------------------------------------------------------
/*void vtkPerkStationInsertStep::
  NormalizationSmoothingWidthCallback(
  vtkIdType target_vol_id, int iValue)
{
  // The Norm-value entry has changed 
  // because of user interaction

  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
 
  mrmlManager->
    SetTargetVolumeIntensityNormalizationInitialHistogramSmoothingWidth(
    target_vol_id, iValue);

}
*/
//----------------------------------------------------------------------------
/*void vtkPerkStationInsertStep::
  NormalizationMaxSmoothingWidthCallback(
  vtkIdType target_vol_id, int iValue)
{
  // The Norm-value entry has changed 
  // because of user interaction

  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
 
  mrmlManager->SetTargetVolumeIntensityNormalizationMaxHistogramSmoothingWidth(
    target_vol_id, iValue);

}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationInsertStep::
  NormalizationRelativeMaxVoxelNumCallback(
  vtkIdType target_vol_id, double dValue)
{
  // The Norm-value entry has changed 
  // because of user interaction

  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
 
  mrmlManager->SetTargetVolumeIntensityNormalizationRelativeMaxVoxelNum(
    target_vol_id, dValue);
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationInsertStep::ResetDefaultParameters(
  vtkIdType target_vol_id)
{
  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
  this->NormalizationDefaultsMenuButton->SetConfigurationOption(
    "-text", "Reset Defaults");

  this->NormalizationPrintCheckButton->GetWidget()->SetSelectedState(
    mrmlManager->GetTargetVolumeIntensityNormalizationPrintInfo(
    target_vol_id));
  this->NormalizationNormValueEntry->GetWidget()->SetValueAsDouble(
    mrmlManager->GetTargetVolumeIntensityNormalizationNormValue(
    target_vol_id));
  this->NormalizationSmoothingWidthEntry->GetWidget()->SetValueAsInt(
    mrmlManager->
    GetTargetVolumeIntensityNormalizationInitialHistogramSmoothingWidth(
    target_vol_id));
  this->NormalizationMaxSmoothingWidthEntry->GetWidget()->SetValueAsInt(
    mrmlManager->
    GetTargetVolumeIntensityNormalizationMaxHistogramSmoothingWidth(
    target_vol_id));
  this->NormalizationRelativeMaxVoxelScale->
    SetValue(mrmlManager->
    GetTargetVolumeIntensityNormalizationRelativeMaxVoxelNum(
    target_vol_id));
}
*/
//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::InstallCallbacks()
{
}
//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::PopulateControls()
{

}

//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::ResetControls()
{
}
//----------------------------------------------------------------------------
void vtkPerkStationInsertStep::Reset()
{
  this->ResetControls();
}
