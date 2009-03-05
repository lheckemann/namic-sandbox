#include "vtkTRProstateBiopsyCalibrationStep.h"

#include "vtkTRProstateBiopsyGUI.h"
#include "vtkTRProstateBiopsyLogic.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkMRMLFiducialListNode.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

#include "vtkKWLabel.h"
#include "vtkKWPushButton.h"
#include "vtkKWCheckButton.h"
#include "vtkKWSpinBox.h"
#include "vtkKWSpinBoxWithLabel.h"
#include "vtkKWScale.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWEntry.h"
#include "vtkKWEntrySet.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWMessageDialog.h"

#include "vtkImageChangeInformation.h"

#include <vtksys/ios/sstream>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkTRProstateBiopsyCalibrationStep);
vtkCxxRevisionMacro(vtkTRProstateBiopsyCalibrationStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkTRProstateBiopsyCalibrationStep::vtkTRProstateBiopsyCalibrationStep()
{
  this->SetName("1/4. Calibration");
  this->SetDescription("Perform robot calibration.");

  this->WizardGUICallbackCommand->SetCallback(&vtkTRProstateBiopsyCalibrationStep::WizardGUICallback);

  //this->MRMLCallbackCommand->SetCallback(
    //&vtkTRProstateBiopsyCalibrationStep::MRMLCallback);

  // load/reset controls
  this->LoadResetFrame = NULL;
  this->LoadCalibrationSettingsFileButton = NULL;
  this->ResetCalibrationButton = NULL;

  // load calibration volume dicom series
  this->LoadVolumeDialogFrame  = NULL;
  this->LoadCalibrationVolumeButton = NULL;

  // save controls
  this->SaveResegmentFrame = NULL;
  this->SaveCalibrationSettingsFileButton = NULL;
  this->ResegmentButton = NULL;


  this->FiducialDimensionsFrame = NULL;
  this->FiducialWidthSpinBox = NULL;
  this->FiducialHeightSpinBox = NULL;
  this->FiducialDepthSpinBox = NULL;

  this->FiducialThresholdFrame = NULL;
  for (int i = 0; i < 4; i++)
    {
    this->FiducialThresholdScale[i] = NULL;
    }

  this->RadiusInitialAngleFrame = NULL;
  this->RadiusSpinBox = NULL;
  this->RadiusCheckButton = NULL;
  this->InitialAngleSpinBox = NULL;

  this->SegmentationResultsFrame = NULL;
  this->Marker_1_CentroidFrame = NULL;
  this->Marker_1_CentroidLabel = NULL;
  this->Marker_1_Centroid = NULL;

  this->Marker_2_CentroidFrame = NULL;
  this->Marker_2_CentroidLabel = NULL;
  this->Marker_2_Centroid = NULL;

  this->Marker_3_CentroidFrame = NULL;
  this->Marker_3_CentroidLabel = NULL;
  this->Marker_3_Centroid = NULL;

  this->Marker_4_CentroidFrame = NULL;
  this->Marker_4_CentroidLabel = NULL;
  this->Marker_4_Centroid = NULL;

  this->AxesAngle = NULL;
  this->AxesDistance = NULL;

  this->ProcessingCallback = false;
  this->ObserverCount = 0;
  this->ClickNumber = 0;
  this->AllMarkersAcquired = false;

}

//----------------------------------------------------------------------------
vtkTRProstateBiopsyCalibrationStep::~vtkTRProstateBiopsyCalibrationStep()
{
  // load/reset controls
  if (this->LoadResetFrame)
        {
    this->LoadResetFrame->Delete();
    this->LoadResetFrame = NULL;
    }
  if (this->LoadCalibrationSettingsFileButton)
    {
    this->LoadCalibrationSettingsFileButton->Delete();
    this->LoadCalibrationSettingsFileButton = NULL;
    }

  if (this->ResetCalibrationButton)
    {
    this->ResetCalibrationButton->Delete();
    this->ResetCalibrationButton = NULL;
    }
  
  if (this->LoadVolumeDialogFrame)
    {
    this->LoadVolumeDialogFrame->Delete();
    this->LoadVolumeDialogFrame = NULL;
    }
  if (this->LoadCalibrationVolumeButton)
    {
    this->LoadCalibrationVolumeButton->Delete();
    this->LoadCalibrationVolumeButton = NULL;
    }

  

  // save controls
  if (this->SaveResegmentFrame)
    {
    this->SaveResegmentFrame->Delete();
    this->SaveResegmentFrame = NULL;
    }
  if (this->SaveCalibrationSettingsFileButton)
    {
    this->SaveCalibrationSettingsFileButton->Delete();
    this->SaveCalibrationSettingsFileButton = NULL;
    }

  if (this->ResegmentButton)
    {
    this->ResegmentButton->Delete();
    this->ResegmentButton = NULL;
    }

   // pre-segment controls
  if (this->FiducialDimensionsFrame)
    {
    this->FiducialDimensionsFrame->Delete();
    this->FiducialDimensionsFrame = NULL;
    }

  if (this->FiducialWidthSpinBox)
    {
    this->FiducialWidthSpinBox->Delete();
    this->FiducialWidthSpinBox = NULL;
    }

  if (this->FiducialHeightSpinBox)
    {
    this->FiducialHeightSpinBox->Delete();
    this->FiducialHeightSpinBox = NULL;
    }

  if (this->FiducialDepthSpinBox)
    {
    this->FiducialDepthSpinBox->Delete();
    this->FiducialDepthSpinBox = NULL;
    }

  if (this->FiducialThresholdFrame)
    {
    this->FiducialThresholdFrame->Delete();
    this->FiducialThresholdFrame = NULL;
    }

  for (int i = 0; i < 4; i++)
    {
    if (this->FiducialThresholdScale[i])
      {
      this->FiducialThresholdScale[i]->Delete();
      this->FiducialThresholdScale[i] = NULL;
      }

    }

  if (this->RadiusInitialAngleFrame)
    {
    this->RadiusInitialAngleFrame->Delete();
    this->RadiusInitialAngleFrame = NULL;
    }
  if (this->RadiusSpinBox)
    {
    this->RadiusSpinBox->Delete();
    this->RadiusSpinBox = NULL;
    }
  if (this->RadiusCheckButton)
    {
    this->RadiusCheckButton->Delete();
    this->RadiusCheckButton = NULL;
    }
  if (this->InitialAngleSpinBox)
    {
    this->InitialAngleSpinBox->Delete();
    this->InitialAngleSpinBox = NULL;
    }

  // segmentation results controls
  if (this->SegmentationResultsFrame)
    {
    this->SegmentationResultsFrame->Delete();
    this->SegmentationResultsFrame = NULL;
    }
  if (this->Marker_1_CentroidFrame)
    {
    this->Marker_1_CentroidFrame->Delete();
    this->Marker_1_CentroidFrame = NULL;
    }
  if (this->Marker_1_CentroidLabel)
    {
    this->Marker_1_CentroidLabel->Delete();
    this->Marker_1_CentroidLabel = NULL;
    }
  if (this->Marker_1_Centroid)
    {
    this->Marker_1_Centroid->DeleteAllWidgets();
    this->Marker_1_Centroid = NULL;
    }
  
  if (this->Marker_2_CentroidFrame)
    {
    this->Marker_2_CentroidFrame->Delete();
    this->Marker_2_CentroidFrame = NULL;
    }
  if (this->Marker_2_CentroidLabel)
    {
    this->Marker_2_CentroidLabel->Delete();
    this->Marker_2_CentroidLabel = NULL;
    }
  if (this->Marker_2_Centroid)
    {
    this->Marker_2_Centroid->DeleteAllWidgets();
    this->Marker_2_Centroid = NULL;
    }


  if (this->Marker_3_CentroidFrame)
    {
    this->Marker_3_CentroidFrame->Delete();
    this->Marker_3_CentroidFrame = NULL;
    }
  if (this->Marker_3_CentroidLabel)
    {
    this->Marker_3_CentroidLabel->Delete();
    this->Marker_3_CentroidLabel = NULL;
    }
  if (this->Marker_3_Centroid)
    {
    this->Marker_3_Centroid->DeleteAllWidgets();
    this->Marker_3_Centroid = NULL;
    }


  if (this->Marker_4_CentroidFrame)
    {
    this->Marker_4_CentroidFrame->Delete();
    this->Marker_4_CentroidFrame = NULL;
    }
  if (this->Marker_4_CentroidLabel)
    {
    this->Marker_4_CentroidLabel->Delete();
    this->Marker_4_CentroidLabel = NULL;
    }
  if (this->Marker_4_Centroid)
    {
    this->Marker_4_Centroid->DeleteAllWidgets();
    this->Marker_4_Centroid = NULL;
    }

  if (this->AxesAngle)
    {
    this->AxesAngle->Delete();
    this->AxesAngle = NULL;
    }
  
  if (this->AxesDistance)
    {
    this->AxesDistance->Delete();
    this->AxesDistance = NULL;
    }

  
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ShowLoadResetControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // first clear the components
  this->ClearLoadResetControls();

  
  // show load dialog box, and reset push button
    
  // Create the frame
  if (!this->LoadResetFrame)
    {
    this->LoadResetFrame = vtkKWFrame::New();
    }
  if (!this->LoadResetFrame->IsCreated())
    {
    this->LoadResetFrame->SetParent(parent);
    this->LoadResetFrame->Create();     
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->LoadResetFrame->GetWidgetName());

  // create the load file dialog button
  if (!this->LoadCalibrationSettingsFileButton)
    {
    this->LoadCalibrationSettingsFileButton = vtkKWLoadSaveButton::New();
    }
  if (!this->LoadCalibrationSettingsFileButton->IsCreated())
    {
    this->LoadCalibrationSettingsFileButton->SetParent(this->LoadResetFrame);
    this->LoadCalibrationSettingsFileButton->Create();
    this->LoadCalibrationSettingsFileButton->SetBorderWidth(2);
    this->LoadCalibrationSettingsFileButton->SetReliefToRaised();       
    this->LoadCalibrationSettingsFileButton->SetHighlightThickness(2);
    this->LoadCalibrationSettingsFileButton->SetBackgroundColor(0.85,0.85,0.85);
    this->LoadCalibrationSettingsFileButton->SetActiveBackgroundColor(1,1,1);
    this->LoadCalibrationSettingsFileButton->SetText("Load calibration settings");
    this->LoadCalibrationSettingsFileButton->SetImageToPredefinedIcon(vtkKWIcon::IconPresetLoad);
    this->LoadCalibrationSettingsFileButton->SetBalloonHelpString("Click to load a previous calibration file");
    this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    this->LoadCalibrationSettingsFileButton->TrimPathFromFileNameOff();
    this->LoadCalibrationSettingsFileButton->SetMaximumFileNameLength(256);
    this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode
    this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog()->SetFileTypes("{{XML File} {.xml}} {{All Files} {*.*}}");      
    }
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                        this->LoadCalibrationSettingsFileButton->GetWidgetName());

  // create the reset calib button
  if(!this->ResetCalibrationButton)
    {
    this->ResetCalibrationButton = vtkKWPushButton::New();
    }
  if(!this->ResetCalibrationButton->IsCreated())
    {
    this->ResetCalibrationButton->SetParent(this->LoadResetFrame);
    this->ResetCalibrationButton->SetText("Reset calibration");
    this->ResetCalibrationButton->SetBorderWidth(2);
    this->ResetCalibrationButton->SetReliefToRaised();      
    this->ResetCalibrationButton->SetHighlightThickness(2);
    this->ResetCalibrationButton->SetBackgroundColor(0.85,0.85,0.85);
    this->ResetCalibrationButton->SetActiveBackgroundColor(1,1,1);      
    this->ResetCalibrationButton->SetImageToPredefinedIcon(vtkKWIcon::IconTrashcan);
    this->ResetCalibrationButton->SetBalloonHelpString("Click to re-start the calibration, with new fiducial location guesses");
    this->ResetCalibrationButton->Create();
    }
    
  this->Script("pack %s -side top -anchor ne -padx 2 -pady 4", 
                    this->ResetCalibrationButton->GetWidgetName());
  
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ClearLoadResetControls()
{
  if (this->LoadResetFrame)
    {
    this->Script("pack forget %s", 
                    this->LoadResetFrame->GetWidgetName());
    }
  if (this->LoadCalibrationSettingsFileButton)
    {
    this->Script("pack forget %s", 
                    this->LoadCalibrationSettingsFileButton->GetWidgetName());
    }
  if (this->ResetCalibrationButton)
    {
    this->Script("pack forget %s", 
                    this->ResetCalibrationButton->GetWidgetName());
    }
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ShowLoadVolumeControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // first clear the components
  this->ClearLoadVolumeControls();
  
  if (!this->LoadVolumeDialogFrame)
    {
    this->LoadVolumeDialogFrame = vtkKWFrame::New();
    }
  if (!this->LoadVolumeDialogFrame->IsCreated())
    {
    this->LoadVolumeDialogFrame->SetParent(parent);
    this->LoadVolumeDialogFrame->Create();     
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->LoadVolumeDialogFrame->GetWidgetName());
  
  if (!this->LoadCalibrationVolumeButton)
    {
     this->LoadCalibrationVolumeButton = vtkKWLoadSaveButton::New();
    }
  if (!this->LoadCalibrationVolumeButton->IsCreated())
    {
    this->LoadCalibrationVolumeButton->SetParent(this->LoadVolumeDialogFrame);
    this->LoadCalibrationVolumeButton->Create();
    this->LoadCalibrationVolumeButton->SetBorderWidth(2);
    this->LoadCalibrationVolumeButton->SetReliefToRaised();       
    this->LoadCalibrationVolumeButton->SetHighlightThickness(2);
    this->LoadCalibrationVolumeButton->SetBackgroundColor(0.85,0.85,0.85);
    this->LoadCalibrationVolumeButton->SetActiveBackgroundColor(1,1,1);        
    this->LoadCalibrationVolumeButton->SetText( "Browse for Calibration Volume DICOM Series");
    this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->SetFileTypes("{ {DICOM Files} {*} }");
    this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    this->LoadCalibrationVolumeButton->TrimPathFromFileNameOff();    
    this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode    
    //this->LoadCalibrationVolumeButton->GetWidget()->AddObserver(vtkKWPushButton::InvokedEvent, this->WizardGUICallbackCommand);
    }
  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
               this->LoadCalibrationVolumeButton->GetWidgetName());
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ClearLoadVolumeControls()
{
   if (this->LoadVolumeDialogFrame)
    {
    this->Script("pack forget %s", 
                    this->LoadVolumeDialogFrame->GetWidgetName());
    }
  if (this->LoadCalibrationVolumeButton)
    {
    this->Script("pack forget %s", 
                    this->LoadCalibrationVolumeButton->GetWidgetName());
    }
  
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ShowFiducialSegmentParamsControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // first clear the components
  this->ClearFiducialSegmentParamsControls();

  if (!this->FiducialDimensionsFrame)
    {
    this->FiducialDimensionsFrame = vtkKWFrameWithLabel::New();
    this->FiducialDimensionsFrame->SetParent(parent);
    this->FiducialDimensionsFrame->Create();
    this->FiducialDimensionsFrame->SetLabelText(
                    "Approximate fiducial dimensions");
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->FiducialDimensionsFrame->GetWidgetName());

  if (!this->FiducialWidthSpinBox)
    {
    this->FiducialWidthSpinBox = vtkKWSpinBoxWithLabel::New();
    this->FiducialWidthSpinBox->SetParent(
                    this->FiducialDimensionsFrame->GetFrame());
    this->FiducialWidthSpinBox->Create();
    this->FiducialWidthSpinBox->GetWidget()->SetWidth(11);
    this->FiducialWidthSpinBox->GetWidget()->SetRange(0,100);
    this->FiducialWidthSpinBox->GetWidget()->SetIncrement(1);
    this->FiducialWidthSpinBox->GetWidget()->SetValue(8);
    this->FiducialWidthSpinBox->SetLabelText("W");
    //this->FiducialWidthSpinBox->GetWidget()->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, this->WizardGUICallbackCommand);
    }

  if (!this->FiducialHeightSpinBox)
    {
    this->FiducialHeightSpinBox = vtkKWSpinBoxWithLabel::New();
    this->FiducialHeightSpinBox->SetParent(
                    this->FiducialDimensionsFrame->GetFrame());
    this->FiducialHeightSpinBox->Create();
    this->FiducialHeightSpinBox->GetWidget()->SetWidth(11);
    this->FiducialHeightSpinBox->GetWidget()->SetRange(0,100);
    this->FiducialHeightSpinBox->GetWidget()->SetIncrement(1);
    this->FiducialHeightSpinBox->GetWidget()->SetValue(5);
    this->FiducialHeightSpinBox->SetLabelText("H");
    //this->FiducialHeightSpinBox->GetWidget()->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, this->WizardGUICallbackCommand);
    }

  if (!this->FiducialDepthSpinBox)
    {
    this->FiducialDepthSpinBox = vtkKWSpinBoxWithLabel::New();
    this->FiducialDepthSpinBox->SetParent(
                    this->FiducialDimensionsFrame->GetFrame());
    this->FiducialDepthSpinBox->Create();
    this->FiducialDepthSpinBox->GetWidget()->SetWidth(11);
    this->FiducialDepthSpinBox->GetWidget()->SetRange(0,100);
    this->FiducialDepthSpinBox->GetWidget()->SetIncrement(1);
    this->FiducialDepthSpinBox->GetWidget()->SetValue(5);
    this->FiducialDepthSpinBox->SetLabelText("D");
    //this->FiducialDepthSpinBox->GetWidget()->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, this->WizardGUICallbackCommand);
    }

  this->Script("pack %s %s %s -side left -anchor w -padx 2 -pady 2", 
               this->FiducialWidthSpinBox->GetWidgetName(),
               this->FiducialHeightSpinBox->GetWidgetName(),
               this->FiducialDepthSpinBox->GetWidgetName());

  if (!this->FiducialThresholdFrame)
    {
    this->FiducialThresholdFrame = vtkKWFrameWithLabel::New();
    this->FiducialThresholdFrame->SetParent(parent);
    this->FiducialThresholdFrame->Create();
    this->FiducialThresholdFrame->SetLabelText("Fiducial thresholds");
    //this->FiducialThresholdFrame->CollapseFrame();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->FiducialThresholdFrame->GetWidgetName());

  for (int i = 0; i < 4; i++)
    {
    if (!this->FiducialThresholdScale[i])
      {
      static const char *label[4] = {
        "1", "2", "3", "4" };

      this->FiducialThresholdScale[i] = vtkKWScaleWithEntry::New();
      this->FiducialThresholdScale[i]->SetParent(
        this->FiducialThresholdFrame->GetFrame());
      this->FiducialThresholdScale[i]->Create();
      this->FiducialThresholdScale[i]->SetLabelText(label[i]);
      this->FiducialThresholdScale[i]->SetWidth(3);
      this->FiducialThresholdScale[i]->SetLength(200);
      this->FiducialThresholdScale[i]->SetRange(0,100);
      this->FiducialThresholdScale[i]->SetResolution(1);
      this->FiducialThresholdScale[i]->SetValue(9);
      //this->FiducialThresholdScale[i]->AddObserver(vtkKWScale::ScaleValueChangingEvent, this->WizardGUICallbackCommand);      
      }
    }

  this->Script("pack %s %s %s %s -side top -anchor w -padx 2 -pady 2", 
               this->FiducialThresholdScale[0]->GetWidgetName(),
               this->FiducialThresholdScale[1]->GetWidgetName(),
               this->FiducialThresholdScale[2]->GetWidgetName(),
               this->FiducialThresholdScale[3]->GetWidgetName() );

  if (!this->RadiusInitialAngleFrame)
    {
    this->RadiusInitialAngleFrame = vtkKWFrameWithLabel::New();
    this->RadiusInitialAngleFrame->SetParent(parent);
    this->RadiusInitialAngleFrame->Create();
    this->RadiusInitialAngleFrame->SetLabelText("Radius and Angle");
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->RadiusInitialAngleFrame->GetWidgetName());

  if (!this->RadiusSpinBox)
    {
    this->RadiusSpinBox = vtkKWSpinBoxWithLabel::New();
    this->RadiusSpinBox->SetParent(
                    this->RadiusInitialAngleFrame->GetFrame());
    this->RadiusSpinBox->Create();
    this->RadiusSpinBox->GetWidget()->SetWidth(11);
    this->RadiusSpinBox->GetWidget()->SetRange(0,100);
    this->RadiusSpinBox->GetWidget()->SetIncrement(1);
    this->RadiusSpinBox->GetWidget()->SetValue(35);
    this->RadiusSpinBox->SetLabelText("Radius");
    //this->RadiusSpinBox->GetWidget()->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, this->WizardGUICallbackCommand);
    }

  if (!this->RadiusCheckButton)
    {
    this->RadiusCheckButton = vtkKWCheckButton::New();
    this->RadiusCheckButton->SetParent(
                    this->RadiusInitialAngleFrame->GetFrame());
    this->RadiusCheckButton->Create();
    //this->RadiusCheckButton->AddObserver( vtkKWCheckButton::SelectedStateChangedEvent, this->WizardGUICallbackCommand);
    }

  if (!this->InitialAngleSpinBox)
    {
    this->InitialAngleSpinBox = vtkKWSpinBoxWithLabel::New();
    this->InitialAngleSpinBox->SetParent(
                    this->RadiusInitialAngleFrame->GetFrame());
    this->InitialAngleSpinBox->Create();
    this->InitialAngleSpinBox->GetWidget()->SetWidth(11);
    this->InitialAngleSpinBox->GetWidget()->SetRange(0,100);
    this->InitialAngleSpinBox->GetWidget()->SetIncrement(1);
    this->InitialAngleSpinBox->GetWidget()->SetValue(0);
    this->InitialAngleSpinBox->SetLabelText("    Initial angle");
    //this->InitialAngleSpinBox->GetWidget()->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, this->WizardGUICallbackCommand);
    }

  this->Script("pack %s %s %s -side left -anchor w -padx 2 -pady 2", 
               this->RadiusSpinBox->GetWidgetName(),
               this->RadiusCheckButton->GetWidgetName(),
               this->InitialAngleSpinBox->GetWidgetName() );
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ClearFiducialSegmentParamsControls()
{
  if (this->FiducialDimensionsFrame)
    {
    this->Script("pack forget %s", 
                    this->FiducialDimensionsFrame->GetWidgetName());
    }
  if (this->FiducialWidthSpinBox)
    {
    this->Script("pack forget %s", 
                    this->FiducialWidthSpinBox->GetWidgetName());
    }
  if (this->FiducialHeightSpinBox)
    {
    this->Script("pack forget %s", 
                    this->FiducialHeightSpinBox->GetWidgetName());
    }
  if (this->FiducialDepthSpinBox)
    {
    this->Script("pack forget %s", 
                    this->FiducialDepthSpinBox->GetWidgetName());
    }
  if (this->FiducialThresholdFrame)
    {
    this->Script("pack forget %s", 
                    this->FiducialThresholdFrame->GetWidgetName());
    }
  for (int i = 0; i<4; i++)
    {
    if (this->FiducialThresholdScale[i])
      {
      this->Script("pack forget %s", 
                    this->FiducialThresholdScale[i]->GetWidgetName());
      }
    }
  if (this->RadiusInitialAngleFrame)
    {
    this->Script("pack forget %s", 
                    this->RadiusInitialAngleFrame->GetWidgetName());
    }
  if (this->RadiusSpinBox)
    {
    this->Script("pack forget %s", 
                    this->RadiusSpinBox->GetWidgetName());
    }
  if (this->RadiusCheckButton)
    {
    this->Script("pack forget %s", 
                    this->RadiusCheckButton->GetWidgetName());
    }
  if (this->InitialAngleSpinBox)
    {
    this->Script("pack forget %s", 
                    this->InitialAngleSpinBox->GetWidgetName());
    }

}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ShowFiducialSegmentationResultsControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
   
  // first clear the translate components if they were created by previous mode
  this->ClearFiducialSegmentationResultsControls();

  // Create the frame
  if (!this->SegmentationResultsFrame)
    {
    this->SegmentationResultsFrame = vtkKWFrameWithLabel::New();
    }
  if (!this->SegmentationResultsFrame->IsCreated())
    {
    this->SegmentationResultsFrame->SetParent(parent);
    this->SegmentationResultsFrame->Create();
    this->SegmentationResultsFrame->SetLabelText("Segmentation/registration results");
    this->SegmentationResultsFrame->SetBalloonHelpString("Results of segmentation and robot registration");
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->SegmentationResultsFrame->GetWidgetName());

  // marker centroid controls

  if (!this->Marker_1_CentroidFrame)
    {
    this->Marker_1_CentroidFrame = vtkKWFrame::New();
    }
  if (!this->Marker_1_CentroidFrame->IsCreated())
    {
    this->Marker_1_CentroidFrame->SetParent(this->SegmentationResultsFrame->GetFrame());
    this->Marker_1_CentroidFrame->Create();
    }
    
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->Marker_1_CentroidFrame->GetWidgetName());

  // label
  if (!this->Marker_1_CentroidLabel)
    { 
    this->Marker_1_CentroidLabel = vtkKWLabel::New();
    }
  if (!this->Marker_1_CentroidLabel->IsCreated())
    {
    this->Marker_1_CentroidLabel->SetParent(this->Marker_1_CentroidFrame);
    this->Marker_1_CentroidLabel->Create();
    this->Marker_1_CentroidLabel->SetText("Marker 1 (centroid):     ");
    this->Marker_1_CentroidLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }  
    
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->Marker_1_CentroidLabel->GetWidgetName());
    
  if (!this->Marker_1_Centroid)
    {
    this->Marker_1_Centroid =  vtkKWEntrySet::New();  
    }
  if (!this->Marker_1_Centroid->IsCreated())
    {
    this->Marker_1_Centroid->SetParent(this->Marker_1_CentroidFrame);
    this->Marker_1_Centroid->Create();
    this->Marker_1_Centroid->SetBorderWidth(2);
    this->Marker_1_Centroid->SetReliefToGroove();
    this->Marker_1_Centroid->SetPackHorizontally(1);   
    this->Marker_1_Centroid->SetMaximumNumberOfWidgetsInPackingDirection(3);
    
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->Marker_1_Centroid->AddWidget(id);
      entry->SetWidth(7);
      entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
      entry->ReadOnlyOn(); 
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->Marker_1_Centroid->GetWidgetName());

  if (!this->Marker_2_CentroidFrame)
    {
    this->Marker_2_CentroidFrame = vtkKWFrame::New();
    }
  if (!this->Marker_2_CentroidFrame->IsCreated())
    {
    this->Marker_2_CentroidFrame->SetParent(this->SegmentationResultsFrame->GetFrame());
    this->Marker_2_CentroidFrame->Create();
    }
    
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->Marker_2_CentroidFrame->GetWidgetName());

  // label
  if (!this->Marker_2_CentroidLabel)
    { 
    this->Marker_2_CentroidLabel = vtkKWLabel::New();
    }
  if (!this->Marker_2_CentroidLabel->IsCreated())
    {
    this->Marker_2_CentroidLabel->SetParent(this->Marker_2_CentroidFrame);
    this->Marker_2_CentroidLabel->Create();
    this->Marker_2_CentroidLabel->SetText("Marker 2 (centroid):     ");
    this->Marker_2_CentroidLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }  
    
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->Marker_2_CentroidLabel->GetWidgetName());
    
  if (!this->Marker_2_Centroid)
    {
    this->Marker_2_Centroid =  vtkKWEntrySet::New();  
    }
  if (!this->Marker_2_Centroid->IsCreated())
    {
    this->Marker_2_Centroid->SetParent(this->Marker_2_CentroidFrame);
    this->Marker_2_Centroid->Create();
    this->Marker_2_Centroid->SetBorderWidth(2);
    this->Marker_2_Centroid->SetReliefToGroove();
    this->Marker_2_Centroid->SetPackHorizontally(1);   
    this->Marker_2_Centroid->SetMaximumNumberOfWidgetsInPackingDirection(3);
    
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->Marker_2_Centroid->AddWidget(id);
      entry->SetWidth(7);
      entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
      entry->ReadOnlyOn(); 
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->Marker_2_Centroid->GetWidgetName());

  if (!this->Marker_3_CentroidFrame)
    {
    this->Marker_3_CentroidFrame = vtkKWFrame::New();
    }
  if (!this->Marker_3_CentroidFrame->IsCreated())
    {
    this->Marker_3_CentroidFrame->SetParent(this->SegmentationResultsFrame->GetFrame());
    this->Marker_3_CentroidFrame->Create();
    }
    
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->Marker_3_CentroidFrame->GetWidgetName());

  // label
  if (!this->Marker_3_CentroidLabel)
    { 
    this->Marker_3_CentroidLabel = vtkKWLabel::New();
    }
  if (!this->Marker_3_CentroidLabel->IsCreated())
    {
    this->Marker_3_CentroidLabel->SetParent(this->Marker_3_CentroidFrame);
    this->Marker_3_CentroidLabel->Create();
    this->Marker_3_CentroidLabel->SetText("Marker 3 (centroid):     ");
    this->Marker_3_CentroidLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }  
    
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->Marker_3_CentroidLabel->GetWidgetName());
    
  if (!this->Marker_3_Centroid)
    {
    this->Marker_3_Centroid =  vtkKWEntrySet::New();  
    }
  if (!this->Marker_3_Centroid->IsCreated())
    {
    this->Marker_3_Centroid->SetParent(this->Marker_3_CentroidFrame);
    this->Marker_3_Centroid->Create();
    this->Marker_3_Centroid->SetBorderWidth(2);
    this->Marker_3_Centroid->SetReliefToGroove();
    this->Marker_3_Centroid->SetPackHorizontally(1);   
    this->Marker_3_Centroid->SetMaximumNumberOfWidgetsInPackingDirection(3);
    
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->Marker_3_Centroid->AddWidget(id);
      entry->SetWidth(7);
      entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
      entry->ReadOnlyOn(); 
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->Marker_3_Centroid->GetWidgetName());

  if (!this->Marker_4_CentroidFrame)
    {
    this->Marker_4_CentroidFrame = vtkKWFrame::New();
    }
  if (!this->Marker_4_CentroidFrame->IsCreated())
    {
    this->Marker_4_CentroidFrame->SetParent(this->SegmentationResultsFrame->GetFrame());
    this->Marker_4_CentroidFrame->Create();
    }
    
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->Marker_4_CentroidFrame->GetWidgetName());

  // label
  if (!this->Marker_4_CentroidLabel)
    { 
    this->Marker_4_CentroidLabel = vtkKWLabel::New();
    }
  if (!this->Marker_4_CentroidLabel->IsCreated())
    {
    this->Marker_4_CentroidLabel->SetParent(this->Marker_4_CentroidFrame);
    this->Marker_4_CentroidLabel->Create();
    this->Marker_4_CentroidLabel->SetText("Marker 4 (centroid):     ");
    this->Marker_4_CentroidLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }  
    
    this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->Marker_4_CentroidLabel->GetWidgetName());
    
  if (!this->Marker_4_Centroid)
    {
    this->Marker_4_Centroid =  vtkKWEntrySet::New();  
    }
  if (!this->Marker_4_Centroid->IsCreated())
    {
    this->Marker_4_Centroid->SetParent(this->Marker_4_CentroidFrame);
    this->Marker_4_Centroid->Create();
    this->Marker_4_Centroid->SetBorderWidth(2);
    this->Marker_4_Centroid->SetReliefToGroove();
    this->Marker_4_Centroid->SetPackHorizontally(1);   
    this->Marker_4_Centroid->SetMaximumNumberOfWidgetsInPackingDirection(3);
    
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->Marker_4_Centroid->AddWidget(id);
      entry->SetWidth(7);
      entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
      entry->ReadOnlyOn(); 
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->Marker_4_Centroid->GetWidgetName());

  // axes angle
  if (!this->AxesAngle)
    {
    this->AxesAngle =  vtkKWEntryWithLabel::New();  
    }
  if (!this->AxesAngle->IsCreated())
    {
    this->AxesAngle->SetParent(this->SegmentationResultsFrame->GetFrame());
    this->AxesAngle->Create();
    this->AxesAngle->GetWidget()->SetRestrictValueToDouble();
    this->AxesAngle->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->AxesAngle->SetLabelText("Axes angle(in degrees, should be 37):");
    this->AxesAngle->GetWidget()->SetWidth(7);
    this->AxesAngle->GetWidget()->SetDisabledBackgroundColor(0.9,0.9,0.9);
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                    this->AxesAngle->GetWidgetName());
  
  // axes distance
  if (!this->AxesDistance)
    {
    this->AxesDistance =  vtkKWEntryWithLabel::New();  
    }
  if (!this->AxesDistance->IsCreated())
    {
    this->AxesDistance->SetParent(this->SegmentationResultsFrame->GetFrame());
    this->AxesDistance->Create();
    this->AxesDistance->GetWidget()->SetRestrictValueToDouble();
    this->AxesDistance->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->AxesDistance->SetLabelText("Axes distance(in mm, should be 0):");
    this->AxesDistance->GetWidget()->SetWidth(7);
    this->AxesDistance->GetWidget()->SetDisabledBackgroundColor(0.9,0.9,0.9);
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                    this->AxesDistance->GetWidgetName());

}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ClearFiducialSegmentationResultsControls()
{
  if (this->SegmentationResultsFrame)
    {
    this->Script("pack forget %s", 
                    this->SegmentationResultsFrame->GetWidgetName());
    }
  if (this->Marker_1_CentroidFrame)
    {
    this->Script("pack forget %s", 
                    this->Marker_1_CentroidFrame->GetWidgetName());
    }
  if (this->Marker_1_CentroidLabel)
    {
    this->Script("pack forget %s", 
                    this->Marker_1_CentroidLabel->GetWidgetName());
    }
  if (this->Marker_1_Centroid)
    {
    this->Script("pack forget %s", 
                    this->Marker_1_Centroid->GetWidgetName());
    }
  if (this->Marker_2_CentroidFrame)
    {
    this->Script("pack forget %s", 
                    this->Marker_2_CentroidFrame->GetWidgetName());
    }
  if (this->Marker_2_CentroidLabel)
    {
    this->Script("pack forget %s", 
                    this->Marker_2_CentroidLabel->GetWidgetName());
    }
  if (this->Marker_2_Centroid)
    {
    this->Script("pack forget %s", 
                    this->Marker_2_Centroid->GetWidgetName());
    }
  if (this->Marker_3_CentroidFrame)
    {
    this->Script("pack forget %s", 
                    this->Marker_3_CentroidFrame->GetWidgetName());
    }
  if (this->Marker_3_CentroidLabel)
    {
    this->Script("pack forget %s", 
                    this->Marker_3_CentroidLabel->GetWidgetName());
    }
  if (this->Marker_3_Centroid)
    {
    this->Script("pack forget %s", 
                    this->Marker_3_Centroid->GetWidgetName());
    }
  if (this->Marker_4_CentroidFrame)
    {
    this->Script("pack forget %s", 
                    this->Marker_4_CentroidFrame->GetWidgetName());
    }
  if (this->Marker_4_CentroidLabel)
    {
    this->Script("pack forget %s", 
                    this->Marker_4_CentroidLabel->GetWidgetName());
    }
  if (this->Marker_4_Centroid)
    {
    this->Script("pack forget %s", 
                    this->Marker_4_Centroid->GetWidgetName());
    }
  if (this->AxesAngle)
    {
    this->Script("pack forget %s", 
                    this->AxesAngle->GetWidgetName());
    }
  if (this->AxesDistance)
    {
    this->Script("pack forget %s", 
                    this->AxesDistance->GetWidgetName());
    }
  
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ShowSaveResegmentControls()
{
   vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // first clear the components if they created before for other mode
  this->ClearSaveResegmentControls();

  // Create the frame
  if (!this->SaveResegmentFrame)
    {
    this->SaveResegmentFrame = vtkKWFrame::New();
    }
  if (!this->SaveResegmentFrame->IsCreated())
    {
    this->SaveResegmentFrame->SetParent(parent);
    this->SaveResegmentFrame->Create();
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->SaveResegmentFrame->GetWidgetName());

  // create the save file dialog button
  if (!this->SaveCalibrationSettingsFileButton)
    {
    this->SaveCalibrationSettingsFileButton = vtkKWLoadSaveButton::New();
    }
  if (!this->SaveCalibrationSettingsFileButton->IsCreated())
    {
    this->SaveCalibrationSettingsFileButton->SetParent(this->SaveResegmentFrame);
    this->SaveCalibrationSettingsFileButton->Create();
    this->SaveCalibrationSettingsFileButton->SetText("Save calibration");
    this->SaveCalibrationSettingsFileButton->SetBorderWidth(2);
    this->SaveCalibrationSettingsFileButton->SetReliefToRaised();       
    this->SaveCalibrationSettingsFileButton->SetHighlightThickness(2);
    this->SaveCalibrationSettingsFileButton->SetBackgroundColor(0.85,0.85,0.85);
    this->SaveCalibrationSettingsFileButton->SetActiveBackgroundColor(1,1,1);               
    this->SaveCalibrationSettingsFileButton->SetImageToPredefinedIcon(vtkKWIcon::IconFloppy);
    this->SaveCalibrationSettingsFileButton->SetBalloonHelpString("Click to save calibration in a file");
    this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog()->SaveDialogOn(); // save mode
    this->SaveCalibrationSettingsFileButton->TrimPathFromFileNameOff();
    this->SaveCalibrationSettingsFileButton->SetMaximumFileNameLength(256);
    this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog()->SetFileTypes("{{XML File} {.xml}} {{All Files} {*.*}}");      
    this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    }
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                        this->SaveCalibrationSettingsFileButton->GetWidgetName());

   // create the resegment button
  if (!this->ResegmentButton)
    {
    this->ResegmentButton = vtkKWPushButton::New();
    }
  if (!this->ResegmentButton->IsCreated())
    {
    this->ResegmentButton->SetParent(this->SaveResegmentFrame);
    this->ResegmentButton->SetText("Re-segment");
    this->ResegmentButton->SetBorderWidth(2);
    this->ResegmentButton->SetReliefToRaised();      
    this->ResegmentButton->SetHighlightThickness(2);
    this->ResegmentButton->SetBackgroundColor(0.85,0.85,0.85);
    this->ResegmentButton->SetActiveBackgroundColor(1,1,1);      
    this->ResegmentButton->SetBalloonHelpString("Click to re-segment the fiducials, with new parameters, but old fiducial location guesses. If you wish to give fiducial guesses again, press 'Reset calibration' button on top of the panel");
    //this->ResegmentButton->SetImageToPredefinedIcon(vtkKWIcon::IconTrashcan);
    this->ResegmentButton->Create();
    this->ResegmentButton->SetEnabled(0);
    }
    
  this->Script("pack %s -side top -anchor ne -padx 2 -pady 4", 
                    this->ResegmentButton->GetWidgetName());

}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ClearSaveResegmentControls()
{
  if (this->SaveResegmentFrame)
    {
    this->Script("pack forget %s", 
                    this->SaveResegmentFrame->GetWidgetName());
    }
  if (this->SaveCalibrationSettingsFileButton)
    {
    this->Script("pack forget %s", 
                    this->SaveCalibrationSettingsFileButton->GetWidgetName());
    }
  if (this->ResegmentButton)
    {
    this->Script("pack forget %s", 
                    this->ResegmentButton->GetWidgetName());
    }
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ShowUserInterface()
{
  //slicerCerr("vtkTRProstateBiopsyCalibrationStep::ShowUserInterface");

  // clear page, as HideUserInterface of previous step is not getting called, a bug
  // until that bug is resolved, we will manually call ClearPage
  if (this->GetGUI())
    {
    this->GetGUI()->GetWizardWidget()->ClearPage();
    }

  this->Superclass::ShowUserInterface();

  // show load/reset calibration controls
  this->ShowLoadResetControls();

  // show load volume controls
  this->ShowLoadVolumeControls();

  // show fiducial segmentation params controls
  this->ShowFiducialSegmentParamsControls();

  // show fiducial segmentation results controls
  this->ShowFiducialSegmentationResultsControls();

  // show save resegment controls
  this->ShowSaveResegmentControls();

  this->AddGUIObservers();

  // Activate additional details for entering this step
  //this->Enter();
}
//----------------------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::PopulateSegmentationResults()
{
  double marker_1_RAS[3], marker_2_RAS[3], marker_3_RAS[3], marker_4_RAS[3] ;

  // marker 1
  this->GetGUI()->GetMRMLNode()->GetCalibrationMarker(1, marker_1_RAS[0], marker_1_RAS[1], marker_1_RAS[2]); 

  for (int i = 0; i < 3; i++)
    this->Marker_1_Centroid->GetWidget(i)->SetValueAsDouble(marker_1_RAS[i]);

  // marker 2
  this->GetGUI()->GetMRMLNode()->GetCalibrationMarker(2, marker_2_RAS[0], marker_2_RAS[1], marker_2_RAS[2]); 
  
  for (int i = 0; i < 3; i++)
    this->Marker_2_Centroid->GetWidget(i)->SetValueAsDouble(marker_2_RAS[i]);

  // marker 3
  this->GetGUI()->GetMRMLNode()->GetCalibrationMarker(3, marker_3_RAS[0], marker_3_RAS[1], marker_3_RAS[2]); 
  
  for (int i = 0; i < 3; i++)
    this->Marker_3_Centroid->GetWidget(i)->SetValueAsDouble(marker_3_RAS[i]);

  // marker 4
  this->GetGUI()->GetMRMLNode()->GetCalibrationMarker(4, marker_4_RAS[0], marker_4_RAS[1], marker_4_RAS[2]); 

  for (int i = 0; i < 3; i++)
    this->Marker_4_Centroid->GetWidget(i)->SetValueAsDouble(marker_4_RAS[i]);


}
//----------------------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::PopulateRegistrationResults()
{
  double angle, distance ;

  // angle
  angle = this->GetGUI()->GetMRMLNode()->GetAxesAngleDegrees(); 
  this->AxesAngle->GetWidget()->SetValueAsDouble(angle);

  // distance
  distance = this->GetGUI()->GetMRMLNode()->GetAxesDistance(); 
  this->AxesDistance->GetWidget()->SetValueAsDouble(distance);

}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::AddGUIObservers()
{
  this->RemoveGUIObservers();

  // add observers 
  // 1) click on load calib settings button
  if (this->LoadCalibrationSettingsFileButton)
    {
    this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }

  // 2) click on save calib settings button
  if (this->SaveCalibrationSettingsFileButton)
    {
    this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }

  // 3) click on reset calib button
  if (this->ResetCalibrationButton)
    {
    this->ResetCalibrationButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }

  // 4) click on resegment button
  if (this->ResegmentButton)
    {
    this->ResegmentButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }

  // 5) click on load calibration volume dialog
  if (this->LoadCalibrationVolumeButton)
    {
    this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }

  this->ObserverCount++;

}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::RemoveGUIObservers()
{
  // 1) click on load calib settings button
  if (this->LoadCalibrationSettingsFileButton)
    {
    this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }

  // 2) click on save calib settings button
  if (this->SaveCalibrationSettingsFileButton)
    {
    this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }

  // 3) click on reset calib button
  if (this->ResetCalibrationButton)
    {
    this->ResetCalibrationButton->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }

  // 4) click on resegment button
  if (this->ResegmentButton)
    {
    this->ResegmentButton->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }

  // 5) click on load calibration volume dialog
  if (this->LoadCalibrationVolumeButton)
    {
    this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }

  this->ObserverCount--;
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::Enter()
{
  //slicerCerr("vtkTRProstateBiopsyCalibrationStep::Enter");
/*
  // change the view orientation to the calibration image orientation
  vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>(
    this->GetGUI()->GetApplication());
  vtkTRProstateBiopsyLogic *logic = this->GetGUI()->GetLogic();
//  logic->SetSliceViewFromVolume(app, logic->GetCalibrationVolumeNode());


  if (this->GetGUI()->GetLogic()->GetCalibrationSliceNodeXML())
    {
    static const char *panes[3] = { "Red", "Yellow", "Green" };

    vtkSlicerApplicationLogic *appLogic =
      this->GetGUI()->GetApplicationGUI()->GetApplicationLogic();

    const char *xml = this->GetGUI()->GetLogic()->GetCalibrationSliceNodeXML();

    //slicerCerr(xml << "\n");

    for (int i = 0; i < 3; i++)
      {
      //slicerCerr(i);

      vtkSlicerSliceLogic *slice = appLogic->GetSliceLogic(
        const_cast<char *>(panes[i]));

      vtkMRMLSliceNode *sliceNode = slice->GetSliceNode();

      sliceNode->ReadXMLAttributes(&xml);
      }
    }
*/

  // change the fiducials to the calibration fiducials

}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::Exit()
{
  //slicerCerr("vtkTRProstateBiopsyCalibrationStep::Exit");

/*
  vtksys_ios::ostringstream xml;

  static const char *panes[3] = { "Red", "Yellow", "Green" };

  vtkSlicerApplicationLogic *appLogic =
    this->GetGUI()->GetApplicationGUI()->GetApplicationLogic();

  for (int i = 0; i < 3; i++)
    {
    vtkSlicerSliceLogic *slice = appLogic->GetSliceLogic(
      const_cast<char *>(panes[i]));

    vtkMRMLSliceNode *sliceNode = slice->GetSliceNode();

    sliceNode->WriteXML(xml, 0);
    }

  this->GetGUI()->GetLogic()->SetCalibrationSliceNodeXML(xml.str().c_str());
*/
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::WizardGUICallback( vtkObject *caller,
                         unsigned long eid, void *clientData, void *callData )
{

  vtkTRProstateBiopsyCalibrationStep *self = reinterpret_cast<vtkTRProstateBiopsyCalibrationStep *>(clientData);
  
  //if (self->GetInGUICallbackFlag())
    {
    // check for infinite recursion here
    }

  vtkDebugWithObjectMacro(self, "In vtkTRProstateCalibrationBiopsyStep GUICallback");
  
  //self->SetInGUICallbackFlag(1);
  self->ProcessGUIEvents(caller, eid, callData);
  //self->SetInGUICallbackFlag(0);
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ProcessGUIEvents(vtkObject *caller,
        unsigned long event, void *callData)
{
  //slicerCerr("vtkTRProstateBiopsyCalibrationStep::ProcessGUIEvents");

  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;
  
  // load calib volume dialog button
  if (this->LoadCalibrationVolumeButton && this->LoadCalibrationVolumeButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    const char *fileName = this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      // call the callback function
      this->LoadCalibrationVolumeButtonCallback(fileName);    
      } 
   
    }
  // load calib dialog button
  if (this->LoadCalibrationSettingsFileButton && this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    const char *fileName = this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      // call the callback function
      this->LoadCalibrationSettingsFileButtonCallback(fileName);    
      }
    
    // reset the file browse button text
    this->LoadCalibrationSettingsFileButton->SetText ("Load calibration");
   
    }
  // save calib dialog button
  if (this->SaveCalibrationSettingsFileButton && this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    const char *fileName = this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      
      // save the path to registry
      this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog()->SaveLastPathToRegistry("SavePath");
        
      // call the callback function
      this->SaveCalibrationSettingsFileButtonCallback(fileName);

    
      }
    
    // reset the file browse button text
    this->SaveCalibrationSettingsFileButton->SetText ("Save calibration");
   
    }
  // reset calib button
  if (this->ResetCalibrationButton && this->ResetCalibrationButton == vtkKWPushButton::SafeDownCast(caller) && (event == vtkKWPushButton::InvokedEvent))
    {
    this->Reset();
    }
  // resegment 
  if (this->ResegmentButton && this->ResegmentButton == vtkKWPushButton::SafeDownCast(caller) && (event == vtkKWPushButton::InvokedEvent))
    {
    //this->Resegment();
    }
  
  
  

}



//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::LoadCalibrationSettingsFileButtonCallback(const char *fileName)
{
    ifstream file(fileName);
    
    this->LoadCalibrationSettings(file);
    file.close();
}

//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::SaveCalibrationSettingsFileButtonCallback(const char *fileName)
{
    ofstream file(fileName);

    this->SaveCalibrationSettings(file);
    
    file.close();
}

//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::LoadCalibrationSettings(istream &file)
{
  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }  
  
}

//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::SaveCalibrationSettings(ostream& of)
{
  // reset parameters at MRML node
   vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }  

}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::LoadCalibrationVolumeButtonCallback(const char *fileName)
{
  std::string fileString(fileName);
  for (unsigned int i = 0; i < fileString.length(); i++)
    {
    if (fileString[i] == '\\')
      {
      fileString[i] = '/';
      }
    }
  
  this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->SaveLastPathToRegistry("TRProstateOpenPath");

  vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>(this->GetGUI()->GetApplication());

  vtkMRMLScalarVolumeNode *volumeNode = this->GetGUI()->GetLogic()->AddCalibrationVolume(app,fileString.c_str());
        
  if (volumeNode)
    {
    this->GetGUI()->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID( volumeNode->GetID() );
    this->GetGUI()->GetApplicationLogic()->PropagateVolumeSelection();
    }
  else 
    {
    vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
    dialog->SetParent ( this->LoadVolumeDialogFrame );
    dialog->SetStyleToMessage();
    std::string msg = std::string("Unable to read volume file ") + std::string(fileName);
    dialog->SetText(msg.c_str());
    dialog->Create ( );
    dialog->Invoke();
    dialog->Delete();
    }
     
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::Reset()
{
    this->AllMarkersAcquired = false;
    this->ClickNumber = 0;
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData)
{
  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  //  if(!mrmlNode->GetPlanningVolumeNode() || strcmp(mrmlNode->GetVolumeInUse(), "Planning")!=0)
  //    return;

  
  // first identify if the step is pertinent, i.e. current step of wizard workflow is actually calibration step

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  if (!wizard_widget || wizard_widget->GetWizardWorkflow()->GetCurrentStep() != this)
    {
    return;
    }

  vtkSlicerInteractorStyle *s = vtkSlicerInteractorStyle::SafeDownCast(caller);
  vtkSlicerInteractorStyle *istyle0 = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyle1 = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyle2 = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  
  
  // listen to click only when they come from secondary monitor's window
  if (( (s == istyle0)|| (s == istyle1) || (s == istyle2) )&& (event == vtkCommand::LeftButtonPressEvent) && !this->AllMarkersAcquired)
    {  
    if (this->ProcessingCallback)
    {
    return;
    }

    this->ProcessingCallback = true;

    ++this->ClickNumber;

    vtkRenderWindowInteractor *rwi;
    vtkMatrix4x4 *matrix;        
    
    if (s == istyle0)
      {
      vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Red");
      rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();
      matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();     
      }
    else if (s == istyle1)
      {
      vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Yellow");
      rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();
      matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();     
      }
    else if (s == istyle2)
      {
      vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Green");
      rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();
      matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();     
      }
    

    // capture the point
    int point[2];
    rwi->GetLastEventPosition(point);
    double inPt[4] = {point[0], point[1], 0, 1};
    double outPt[4];    
    matrix->MultiplyPoint(inPt, outPt); 
    double ras[3] = {outPt[0], outPt[1], outPt[2]};
    this->RecordClick(ras);

    this->ProcessingCallback = false;
    }
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::RecordClick(double rasPoint[])
{
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  char *msg = new char[40];

  if (this->ClickNumber ==1)
    {
    sprintf(msg, "Clicked 1st marker at %3.1f %3.1f %3.1f", rasPoint[0], rasPoint[1], rasPoint[2]);
    mrmlNode->SetCalibrationMarker(1, rasPoint);
    }
  else if (this->ClickNumber == 2)
    {
    sprintf(msg, "Clicked 2nd marker at %3.1f %3.1f %3.1f", rasPoint[0], rasPoint[1], rasPoint[2]);
    mrmlNode->SetCalibrationMarker(2, rasPoint);
    }
  else if (this->ClickNumber == 3)
    {
    sprintf(msg, "Clicked 3rd marker at %3.1f %3.1f %3.1f", rasPoint[0], rasPoint[1], rasPoint[2]);
    mrmlNode->SetCalibrationMarker(3, rasPoint);
    }
  else if (this->ClickNumber == 4)
    {
    this->ClickNumber = 0;
    this->AllMarkersAcquired = true;
    sprintf(msg, "Clicked 4th marker at %3.1f %3.1f %3.1f", rasPoint[0], rasPoint[1], rasPoint[2]);
    wizard_widget->SetErrorText(msg);
    wizard_widget->Update();
    mrmlNode->SetCalibrationMarker(4, rasPoint);
    sprintf(msg, "Starting segmentation....");
    wizard_widget->SetErrorText(msg);
    wizard_widget->Update();

    // gather information about thresholds
    double thresh[4];
    for (int i=0 ; i<4; i++)
        thresh[i] = this->FiducialThresholdScale[i]->GetValue();
    
    // gather info about the fiducial dimenstions
    double fidDims[3];
    fidDims[0] = this->FiducialWidthSpinBox->GetWidget()->GetValue();
    fidDims[1] = this->FiducialHeightSpinBox->GetWidget()->GetValue();
    fidDims[2] = this->FiducialDepthSpinBox->GetWidget()->GetValue();

    // gather info about initial radius, and initial angle
    double radius = 0;
    bool bUseRadius = false;
    double initialAngle = 0;

    radius = this->RadiusSpinBox->GetWidget()->GetValue();
    bUseRadius = this->RadiusCheckButton->GetSelectedState();
    initialAngle = this->InitialAngleSpinBox->GetWidget()->GetValue();

    // start the segmentation/registration
    this->GetGUI()->GetLogic()->SegmentRegisterMarkers(thresh, fidDims, radius, bUseRadius, initialAngle);

    // populate controls
    mrmlNode->GetCalibrationFiducialsList()->SetAllFiducialsVisibility(true);
    this->PopulateSegmentationResults();
    this->PopulateRegistrationResults();

    sprintf(msg, "Segmentation complete!"); 
    }

  wizard_widget->SetErrorText(msg);
  wizard_widget->Update();
}
