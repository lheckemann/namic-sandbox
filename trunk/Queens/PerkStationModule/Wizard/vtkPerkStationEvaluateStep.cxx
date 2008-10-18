#include "vtkPerkStationEvaluateStep.h"

#include "vtkPerkStationModuleGUI.h"
#include "vtkPerkStationModuleLogic.h"
#include "vtkMRMLPerkStationModuleNode.h"


#include "vtkKWWizardStep.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWEntrySet.h"
#include "vtkKWEntryWithLabel.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationEvaluateStep);
vtkCxxRevisionMacro(vtkPerkStationEvaluateStep, "$Revision: 1.0 $");

//----------------------------------------------------------------------------
vtkPerkStationEvaluateStep::vtkPerkStationEvaluateStep()
{
  this->SetName("5/5. Evaluation");
  this->SetDescription("Performance evaluation");

  // claibrate errors components
  this->CalibrateErrorsFrame = NULL;

  this->CalibrateScaleErrorFrame = NULL;
  this->CalibrateScaleErrorLabel = NULL;
  this->CalibrateScaleError = NULL;

  this->CalibrateTranslationErrorFrame = NULL;
  this->CalibrateTranslationErrorLabel = NULL;
  this->CalibrateTranslationError = NULL;

  this->CalibrateRotationError = NULL;

  // plan errors components
  this->PlanErrorsFrame = NULL;
  this->PlanInsertionAngleError = NULL;
  this->PlanInsertionDepthError = NULL;

  // insertion/validation errors
  this->InsertionValidationErrorsFrame = NULL;
  this->EntryPointError = NULL;
  this->TargetPointError = NULL;
 
}

//----------------------------------------------------------------------------
vtkPerkStationEvaluateStep::~vtkPerkStationEvaluateStep()
{
  if (this->CalibrateErrorsFrame)
    {
    this->CalibrateErrorsFrame->Delete();
    this->CalibrateErrorsFrame = NULL;
    }
  
  if (this->CalibrateScaleErrorFrame)
    {
    this->CalibrateScaleErrorFrame->Delete();
    this->CalibrateScaleErrorFrame = NULL;
    }
  if (this->CalibrateScaleErrorLabel)
    {
    this->CalibrateScaleErrorLabel->Delete();
    this->CalibrateScaleErrorLabel = NULL;
    }
  if (this->CalibrateScaleError)
    {
    this->CalibrateScaleError->Delete();
    this->CalibrateScaleError = NULL;
    }

  if (this->CalibrateTranslationErrorFrame)
    {
    this->CalibrateTranslationErrorFrame->Delete();
    this->CalibrateTranslationErrorFrame = NULL;
    }
  if (this->CalibrateTranslationErrorLabel)
    {
    this->CalibrateTranslationErrorLabel->Delete();
    this->CalibrateTranslationErrorLabel = NULL;
    }
  if (this->CalibrateTranslationError)
    {
    this->CalibrateTranslationError->Delete();
    this->CalibrateTranslationError = NULL;
    }

  if (this->CalibrateRotationError)
    {
    this->CalibrateRotationError->Delete();
    this->CalibrateRotationError = NULL;
    }

  if (this->PlanErrorsFrame)
    {
    this->PlanErrorsFrame->Delete();
    this->PlanErrorsFrame = NULL;
    }

  if (this->PlanInsertionAngleError)
    {
    this->PlanInsertionAngleError->Delete();
    this->PlanInsertionAngleError = NULL;
    }

  if (this->PlanInsertionDepthError)
    {
    this->PlanInsertionDepthError->Delete();
    this->PlanInsertionDepthError = NULL;
    }

  if (this->InsertionValidationErrorsFrame)
    {
    this->InsertionValidationErrorsFrame->Delete();
    this->InsertionValidationErrorsFrame = NULL;
    }

  if (this->EntryPointError)
    {
    this->EntryPointError->Delete();
    this->EntryPointError = NULL;
    }

  if (this->TargetPointError)
    {
    this->TargetPointError->Delete();
    this->TargetPointError = NULL;
    }

}

//----------------------------------------------------------------------------
void vtkPerkStationEvaluateStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();
  
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);
  vtkKWWidget *parent = wizard_widget->GetClientArea();
  int enabled = parent->GetEnabled();

  // Create the individual components

  // calibrate errors

  //frame
  if (!this->CalibrateErrorsFrame)
    {
    this->CalibrateErrorsFrame = vtkKWFrameWithLabel::New();
    }
  if (!this->CalibrateErrorsFrame->IsCreated())
    {
    this->CalibrateErrorsFrame->SetParent(parent);
    this->CalibrateErrorsFrame->Create();
    this->CalibrateErrorsFrame->SetLabelText("Calibrate errors");
    this->CalibrateErrorsFrame->SetBalloonHelpString("");
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->CalibrateErrorsFrame->GetWidgetName());


    
  // frame
  if (!this->CalibrateScaleErrorFrame)
    {
    this->CalibrateScaleErrorFrame = vtkKWFrame::New();
    }
  if (!this->CalibrateScaleErrorFrame->IsCreated())
    {
    this->CalibrateScaleErrorFrame->SetParent(this->CalibrateErrorsFrame->GetFrame());
    this->CalibrateScaleErrorFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->CalibrateScaleErrorFrame->GetWidgetName());

  // label
  if (!this->CalibrateScaleErrorLabel)
    { 
    this->CalibrateScaleErrorLabel = vtkKWLabel::New();
    }
  if (!this->CalibrateScaleErrorLabel->IsCreated())
    {
    this->CalibrateScaleErrorLabel->SetParent(this->CalibrateScaleErrorFrame);
    this->CalibrateScaleErrorLabel->Create();
    this->CalibrateScaleErrorLabel->SetText("Errors in calculating scale in percentage (x,y):   ");
    this->CalibrateScaleErrorLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }
  
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->CalibrateScaleErrorLabel->GetWidgetName());
  

  // actual text boxes
  if (!this->CalibrateScaleError)
    {
    this->CalibrateScaleError =  vtkKWEntrySet::New();   
    }
  if (!this->CalibrateScaleError->IsCreated())
    {
    this->CalibrateScaleError->SetParent(this->CalibrateScaleErrorFrame);
    this->CalibrateScaleError->Create();
    this->CalibrateScaleError->SetBorderWidth(2);
    this->CalibrateScaleError->SetReliefToGroove();
    this->CalibrateScaleError->SetPackHorizontally(1);
    this->CalibrateScaleError->SetWidgetsInternalPadX(2);  
    this->CalibrateScaleError->SetMaximumNumberOfWidgetsInPackingDirection(2);
    // two entries of image spacing (x, y)
    for (int id = 0; id < 2; id++)
      {
      vtkKWEntry *entry = this->CalibrateScaleError->AddWidget(id);  
      entry->SetWidth(7);
      entry->ReadOnlyOn();
      entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->CalibrateScaleError->GetWidgetName());


  
    
  // frame
  if (!this->CalibrateTranslationErrorFrame)
    {
    this->CalibrateTranslationErrorFrame = vtkKWFrame::New();
    }
  if (!this->CalibrateTranslationErrorFrame->IsCreated())
    {
    this->CalibrateTranslationErrorFrame->SetParent(this->CalibrateErrorsFrame->GetFrame());
    this->CalibrateTranslationErrorFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->CalibrateTranslationErrorFrame->GetWidgetName());

  // label
  if (!this->CalibrateTranslationErrorLabel)
    { 
    this->CalibrateTranslationErrorLabel = vtkKWLabel::New();
    }
  if (!this->CalibrateTranslationErrorLabel->IsCreated())
    {
    this->CalibrateTranslationErrorLabel->SetParent(this->CalibrateTranslationErrorFrame);
    this->CalibrateTranslationErrorLabel->Create();
    this->CalibrateTranslationErrorLabel->SetText("Error in calculating translation in mm (x,y):   ");
    this->CalibrateTranslationErrorLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }
  
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->CalibrateTranslationErrorLabel->GetWidgetName());
  

  // actual text boxes
  if (!this->CalibrateTranslationError)
    {
    this->CalibrateTranslationError =  vtkKWEntrySet::New();   
    }
  if (!this->CalibrateTranslationError->IsCreated())
    {
    this->CalibrateTranslationError->SetParent(this->CalibrateTranslationErrorFrame);
    this->CalibrateTranslationError->Create();
    this->CalibrateTranslationError->SetBorderWidth(2);
    this->CalibrateTranslationError->SetReliefToGroove();
    this->CalibrateTranslationError->SetPackHorizontally(1);
    this->CalibrateTranslationError->SetWidgetsInternalPadX(2);  
    this->CalibrateTranslationError->SetMaximumNumberOfWidgetsInPackingDirection(2);
    // two entries of image spacing (x, y)
    for (int id = 0; id < 2; id++)
      {
      vtkKWEntry *entry = this->CalibrateTranslationError->AddWidget(id);  
      entry->SetWidth(7);
      entry->ReadOnlyOn();
      entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->CalibrateTranslationError->GetWidgetName());



  // rotation error

  if (!this->CalibrateRotationError)
    {
    this->CalibrateRotationError =  vtkKWEntryWithLabel::New();  
    }
  if (!this->CalibrateRotationError->IsCreated())
    {
    this->CalibrateRotationError->SetParent(this->CalibrateErrorsFrame->GetFrame());
    this->CalibrateRotationError->Create();
    this->CalibrateRotationError->GetWidget()->SetRestrictValueToDouble();
    this->CalibrateRotationError->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->CalibrateRotationError->SetLabelText("Error in calculating rotation (degrees):");
    this->CalibrateRotationError->SetWidth(7);
    this->CalibrateRotationError->GetWidget()->SetDisabledBackgroundColor(0.9,0.9,0.9);
    this->CalibrateRotationError->GetWidget()->ReadOnlyOn();
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->CalibrateRotationError->GetWidgetName());
  

  // plan errors

  // frame
  if (!this->PlanErrorsFrame)
    {
    this->PlanErrorsFrame = vtkKWFrameWithLabel::New();
    }
  if (!this->PlanErrorsFrame->IsCreated())
    {
    this->PlanErrorsFrame->SetParent(parent);
    this->PlanErrorsFrame->Create();
    this->PlanErrorsFrame->SetLabelText("Plan errors");
    this->PlanErrorsFrame->SetBalloonHelpString("");
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->PlanErrorsFrame->GetWidgetName());

  // insertion angle calculation error
  if (!this->PlanInsertionAngleError)
    {
    this->PlanInsertionAngleError =  vtkKWEntryWithLabel::New();  
    }
  if (!this->PlanInsertionAngleError->IsCreated())
    {
    this->PlanInsertionAngleError->SetParent(this->PlanErrorsFrame->GetFrame());
    this->PlanInsertionAngleError->Create();
    this->PlanInsertionAngleError->GetWidget()->SetRestrictValueToDouble();
    this->PlanInsertionAngleError->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->PlanInsertionAngleError->SetLabelText("Error in calculating insertion angle (degrees):");
    this->PlanInsertionAngleError->SetWidth(7);
    this->PlanInsertionAngleError->GetWidget()->SetDisabledBackgroundColor(0.9,0.9,0.9);
    this->PlanInsertionAngleError->GetWidget()->ReadOnlyOn();
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->PlanInsertionAngleError->GetWidgetName());


  // insertion depth calculation error
  if (!this->PlanInsertionDepthError)
    {
    this->PlanInsertionDepthError =  vtkKWEntryWithLabel::New();  
    }
  if (!this->PlanInsertionDepthError->IsCreated())
    {
    this->PlanInsertionDepthError->SetParent(this->PlanErrorsFrame->GetFrame());
    this->PlanInsertionDepthError->Create();
    this->PlanInsertionDepthError->GetWidget()->SetRestrictValueToDouble();
    this->PlanInsertionDepthError->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->PlanInsertionDepthError->SetLabelText("Error in calculating insertion depth (mm):");
    this->PlanInsertionDepthError->SetWidth(7);
    this->PlanInsertionDepthError->GetWidget()->SetDisabledBackgroundColor(0.9,0.9,0.9);
    this->PlanInsertionDepthError->GetWidget()->ReadOnlyOn();
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->PlanInsertionDepthError->GetWidgetName());



  // insertion errors

  // frame
  if (!this->InsertionValidationErrorsFrame)
    {
    this->InsertionValidationErrorsFrame = vtkKWFrameWithLabel::New();
    }
  if (!this->InsertionValidationErrorsFrame->IsCreated())
    {
    this->InsertionValidationErrorsFrame->SetParent(parent);
    this->InsertionValidationErrorsFrame->Create();
    this->InsertionValidationErrorsFrame->SetLabelText("Plan errors");
    this->InsertionValidationErrorsFrame->SetBalloonHelpString("");
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->InsertionValidationErrorsFrame->GetWidgetName());


  // entry point error
  if (!this->EntryPointError)
    {
    this->EntryPointError =  vtkKWEntryWithLabel::New();  
    }
  if (!this->EntryPointError->IsCreated())
    {
    this->EntryPointError->SetParent(this->InsertionValidationErrorsFrame->GetFrame());
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
    this->TargetPointError->SetParent(this->InsertionValidationErrorsFrame->GetFrame());
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


  // TO DO: install callbacks
  this->InstallCallbacks();

  // TO DO: populate controls wherever needed
  this->PopulateControls();

  
}

//----------------------------------------------------------------------------
void vtkPerkStationEvaluateStep::HideUserInterface()
{
  this->Superclass::HideUserInterface();
 // this->RemoveRunRegistrationOutputGUIObservers();
}



//---------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::AddRunRegistrationOutputGUIObservers() 
{
  this->RunSegmentationOutVolumeSelector->AddObserver(
    vtkSlicerNodeSelectorWidget::NodeSelectedEvent, 
    this->GetGUI()->GetGUICallbackCommand()); 
}
*/
//---------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::RemoveRunRegistrationOutputGUIObservers()
{
  this->RunSegmentationOutVolumeSelector->RemoveObservers(
    vtkSlicerNodeSelectorWidget::NodeSelectedEvent, 
    this->GetGUI()->GetGUICallbackCommand()); 
}*/

//---------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::ProcessRunRegistrationOutputGUIEvents(
  vtkObject *caller,
  unsigned long event,
  void *callData) 
{
  if (caller == this->RunSegmentationOutVolumeSelector && 
      event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  &&
      this->RunSegmentationOutVolumeSelector->GetSelected() != NULL) 
    { 
    vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
    if (mrmlManager)
      {
      mrmlManager->SetOutputVolumeMRMLID(
        this->RunSegmentationOutVolumeSelector->GetSelected()->GetID());
      }
    }

}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::SelectTemplateFileCallback()
{
  // The template file has changed because of user interaction

  if (this->RunSegmentationSaveTemplateButton && 
      this->RunSegmentationSaveTemplateButton->IsCreated())
    {
    if (this->RunSegmentationSaveTemplateButton->GetLoadSaveDialog()->
        GetStatus() == vtkKWDialog::StatusOK)
      {
      this->RunSegmentationSaveTemplateButton->GetLoadSaveDialog()->
        SaveLastPathToRegistry("OpenPath");
      vtksys_stl::string filename = 
        this->RunSegmentationSaveTemplateButton->GetFileName();
      vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
      vtkPerkStationModuleLogic *logic = this->GetGUI()->GetLogic();
      if (mrmlManager)
        {
        mrmlManager->SetSaveTemplateFilename(filename.c_str());
        }
      if (logic)
        {
        logic->SaveTemplateNow();
        }
      }
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::SelectDirectoryCallback()
{
  // The template file has changed because of user interaction

  if (this->RunSegmentationDirectoryButton && 
      this->RunSegmentationDirectoryButton->IsCreated())
    {
    if (this->RunSegmentationDirectoryButton->GetLoadSaveDialog()->
        GetStatus() == vtkKWDialog::StatusOK)
      {
      this->RunSegmentationDirectoryButton->GetLoadSaveDialog()->
        SaveLastPathToRegistry("OpenPath");
      vtksys_stl::string filename = 
        this->RunSegmentationDirectoryButton->GetFileName();
      if(!vtksys::SystemTools::FileExists(filename.c_str()) ||
        !vtksys::SystemTools::FileIsDirectory(filename.c_str()))
        {
        if(!vtksys::SystemTools::MakeDirectory(filename.c_str()))
          {
          vtkErrorMacro("Can not create directory: " << filename.c_str());
          return;
          }
        }

      vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
      if (mrmlManager)
        {
        mrmlManager->SetSaveWorkingDirectory(filename.c_str());
        }
      }
    }
}
*/
//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::SaveAfterSegmentationCallback(
  int state)
{
  // The save template checkbutton has changed because of user interaction

  vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (mrmlManager)
    {
    mrmlManager->SetSaveTemplateAfterSegmentation(state);
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::SaveIntermediateCallback(int state)
{
  // The save intermediate checkbutton has changed because of user interaction

  vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (mrmlManager)
    {
    mrmlManager->SetSaveIntermediateResults(state);
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::GenerateSurfaceModelsCallback(
  int state)
{
  // The save surface checkbutton has changed because of user interaction

  vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (mrmlManager)
    {
    mrmlManager->SetSaveSurfaceModels(state);
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::RunSegmentationROIMaxChangedCallback(
    int row, int col, const char *value)
{
  int ijk[3] = {0, 0, 0};
  vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
  mrmlManager->GetSegmentationBoundaryMax(ijk);
  ijk[col] = atoi(value);
  if (mrmlManager->HasGlobalParametersNode())
    {
    mrmlManager->SetSegmentationBoundaryMax(ijk);
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::RunSegmentationROIMinChangedCallback(
    int row, int col, const char *value)
{
  int ijk[3] = {0, 0, 0};
  vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
  mrmlManager->GetSegmentationBoundaryMin(ijk);
  ijk[col] = atoi(value);
  if (mrmlManager->HasGlobalParametersNode())
    {
    mrmlManager->SetSegmentationBoundaryMin(ijk);
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::MultiThreadingCallback(int state)
{
  // The multithreading checkbutton has changed because of user interaction

  vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (mrmlManager)
    {
    mrmlManager->SetEnableMultithreading(state);
    }
}*/

//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::StartSegmentationCallback()
{
  vtkPerkStationModuleLogic *logic = this->GetGUI()->GetLogic();
  vtkMRMLPerkStationModuleNode* mrmlManager = this->GetGUI()->GetMRMLManager();

  if (!mrmlManager || !logic)
    {
    return;
    }
  
  // make sure that data types are the same
  if (!mrmlManager->DoTargetAndAtlasDataTypesMatch())
    {
    // popup an error message
    std::string errorMessage = 
      "Scalar type mismatch for input images; all image scalar types must be "
      "the same (including input channels and atlas images).";

    vtkKWMessageDialog::PopupMessage(this->GetApplication(),
                                     NULL,
                                     "Input Image Error",
                                     errorMessage.c_str(),
                                     vtkKWMessageDialog::ErrorIcon | 
                                     vtkKWMessageDialog::InvokeAtPointer);    
    return;
    }

  // start the segmentation
  logic->StartSegmentation();
}
*/
//----------------------------------------------------------------------------
/*void vtkPerkStationEvaluateStep::CancelSegmentationCallback()
{
  //vtkMRMLPerkStationModuleNode *mrmlManager = this->GetGUI()->GetMRMLManager();
  //if (mrmlManager)
  //  {
  //    mrmlManager->CancelSegmentation();
  //  }
}
*/
//----------------------------------------------------------------------------
void vtkPerkStationEvaluateStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkPerkStationEvaluateStep::InstallCallbacks()
{
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  // Configure the OK button to start

  if (wizard_widget->GetOKButton())
    {
    wizard_widget->GetOKButton()->SetText("Start over");
    wizard_widget->GetOKButton()->SetCommand(
      this, "StartOverNewExperiment");
    wizard_widget->GetOKButton()->SetBalloonHelpString(
      "Do another experiment");
    }

 
}
//----------------------------------------------------------------------------
void vtkPerkStationEvaluateStep::StartOverNewExperiment()
{
  this->GetGUI()->SaveExperiment();
  this->GetGUI()->ResetAndStartNewExperiment();
}
//----------------------------------------------------------------------------
void vtkPerkStationEvaluateStep::PopulateControls()
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

  vtkMRMLScalarVolumeNode *inVolume = vtkMRMLScalarVolumeNode::SafeDownCast(this->GetGUI()->GetMRMLScene()->GetNodeByID(mrmlNode->GetInputVolumeRef()));
  if (!inVolume)
    {
    // TO DO: what to do on failure
    return;
    }

  double scaleError[2];
  mrmlNode->GetCalibrateScaleError(scaleError);
  this->CalibrateScaleError->GetWidget(0)->SetValueAsDouble(scaleError[0]);
  this->CalibrateScaleError->GetWidget(1)->SetValueAsDouble(scaleError[1]);

  double translationError[2];
  mrmlNode->GetCalibrateTranslationError(translationError);
  this->CalibrateTranslationError->GetWidget(0)->SetValueAsDouble(translationError[0]);
  this->CalibrateTranslationError->GetWidget(1)->SetValueAsDouble(translationError[1]);

  this->CalibrateRotationError->GetWidget()->SetValueAsDouble(mrmlNode->GetCalibrateRotationError());

  this->PlanInsertionAngleError->GetWidget()->SetValueAsDouble(mrmlNode->GetPlanInsertionAngleError());
  this->PlanInsertionDepthError->GetWidget()->SetValueAsDouble(mrmlNode->GetPlanInsertionDepthError());

  this->EntryPointError->GetWidget()->SetValueAsDouble(mrmlNode->GetEntryPointError());
  this->TargetPointError->GetWidget()->SetValueAsDouble(mrmlNode->GetTargetPointError());


}

//----------------------------------------------------------------------------
void vtkPerkStationEvaluateStep::ResetControls()
{
  
}
//----------------------------------------------------------------------------
void vtkPerkStationEvaluateStep::Reset()
{
  this->ResetControls();
}
