#include "vtkPerkStationCalibrateStep.h"


#include "vtkPerkStationModuleGUI.h"
#include "vtkMRMLPerkStationModuleNode.h"
#include "vtkPerkStationSecondaryMonitor.h"

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
  this->SetName("1/5. Calibrate");
  this->SetDescription("Do image overlay system calibration");

  this->WizardGUICallbackCommand->SetCallback(vtkPerkStationCalibrateStep::WizardGUICallback);

  // load/reset controls
  this->LoadResetFrame = NULL;
  this->LoadCalibrationFileButton = NULL;
  this->ResetCalibrationButton = NULL;
  this->CalibFileName = "";
  this->CalibFilePath = "";

  // save controls
  this->SaveFrame = NULL;
  this->SaveCalibrationFileButton = NULL;
  
  
    // Hardware selection.
  this->HardwareFrame = NULL;
  this->HardwareMenu = NULL;
  
  
  // flip
  this->FlipFrame = NULL;
  this->VerticalFlipCheckButton = NULL;
  this->HorizontalFlipCheckButton = NULL;
  this->ImageFlipDone = false;

  // scale step
  this->ScaleFrame = NULL;
  this->MonPhySizeFrame = NULL;
  this->MonPhySizeLabel = NULL;
  this->MonPhySize = NULL;
  this->MonPixResFrame = NULL;
  this->MonPixResLabel = NULL;
  this->MonPixRes = NULL;
  this->UpdateAutoScale = NULL;
  this->ImgPixSizeFrame = NULL;
  this->ImgPixSizeLabel = NULL;
  this->ImgSpacing = NULL;
  this->MonPixSizeFrame = NULL;
  this->MonPixSizeLabel = NULL;  
  this->MonSpacing = NULL;
  this->ImgScaleFrame = NULL;
  this->ImgScaleLabel = NULL;  
  this->ImgScaling = NULL; 
  this->ImageScalingDone = false;

  // translation step
  this->TranslateFrame = NULL;
  this->TransMessage = NULL;
  this->TransImgFidFrame = NULL;
  this->TransImgFidLabel = NULL;
  this->TransImgFid = NULL;
  this->TransPhyFidFrame = NULL;
  this->TransPhyFidLabel = NULL;
  this->TransPhyFid = NULL;
  this->TransEntryFrame = NULL;
  this->TransEntryLabel = NULL;
  // this->Translation = NULL;
  this->ImageTranslationDone = false;
  
  // rotation step
  this->RotateFrame = NULL;
  this->RotMessage = NULL;
  this->CORFrame = NULL;
  this->CORLabel = NULL;
  this->COR = NULL;
  this->CORSpecified = false;
  this->RotImgFidFrame = NULL;
  this->RotImgFidLabel = NULL;
  this->RotImgFid = NULL;
  this->RotPhyFidFrame = NULL;
  this->RotPhyFidLabel = NULL;
  this->RotPhyFid = NULL;
  this->RotationAngle = NULL;
  this->ImageRotationDone = false;

  this->TrainingModeControlsPopulated = false;
  this->ClinicalModeControlsPopulated = false;

  this->CurrentSubState = -1; // indicates invalid state, state will be valid only once volume is loaded

  this->ObserverCount = 0;
  this->ProcessingCallback = false;
  this->ClickNumber = 0;
  
  
    // Calibration data.
  
  this->Translation[ 0 ] = 0.0;
  this->Translation[ 1 ] = 0.0;
  this->Rotation = 0.0;
  this->RotationCenter[ 0 ] = 0.0;
  this->RotationCenter[ 1 ] = 0.0;
}

//----------------------------------------------------------------------------
vtkPerkStationCalibrateStep::~vtkPerkStationCalibrateStep()
{
  // load/reset controls
  if (this->LoadResetFrame)
    {
    this->LoadResetFrame->SetParent(NULL);
    this->LoadResetFrame->Delete();
    this->LoadResetFrame = NULL;
    }
  if (this->LoadCalibrationFileButton)
    {
    this->LoadCalibrationFileButton->SetParent(NULL);
    this->LoadCalibrationFileButton->Delete();
    this->LoadCalibrationFileButton = NULL;
    }

  if (this->ResetCalibrationButton)
    {
    this->ResetCalibrationButton->SetParent(NULL);
    this->ResetCalibrationButton->Delete();
    this->ResetCalibrationButton = NULL;
    }

  // save controls
  if (this->SaveFrame)
    {
    this->SaveFrame->SetParent(NULL);
    this->SaveFrame->Delete();
    this->SaveFrame = NULL;
    }
  if (this->SaveCalibrationFileButton)
    {
    this->SaveCalibrationFileButton->SetParent(NULL);
    this->SaveCalibrationFileButton->Delete();
    this->SaveCalibrationFileButton = NULL;
    }
    
  if ( this->HardwareMenu )
    {
    this->HardwareMenu->SetParent( NULL );
    this->HardwareMenu->Delete();
    this->HardwareMenu = NULL;
    }
  
  // flip step
  if (this->FlipFrame)
    {
    this->FlipFrame->SetParent(NULL);
    this->FlipFrame->Delete();
    this->FlipFrame = NULL;
    }
  if (this->VerticalFlipCheckButton)
    {
    this->VerticalFlipCheckButton->SetParent(NULL);
    this->VerticalFlipCheckButton->Delete();
    this->VerticalFlipCheckButton = NULL;
    }
  if (this->HorizontalFlipCheckButton)
    {
    this->HorizontalFlipCheckButton->SetParent(NULL);
    this->HorizontalFlipCheckButton->Delete();
    this->HorizontalFlipCheckButton = NULL;
    }
  // scale step
  if (this->ScaleFrame)
    {
    this->ScaleFrame->SetParent(NULL);
    this->ScaleFrame->Delete();
    this->ScaleFrame = NULL;
    }
  if (this->MonPhySizeFrame)
    {
    this->MonPhySizeFrame->SetParent(NULL);
    this->MonPhySizeFrame->Delete();
    this->MonPhySizeFrame = NULL;
    }
  if (this->MonPhySizeLabel)
    {
    this->MonPhySizeLabel->SetParent(NULL);
    this->MonPhySizeLabel->Delete();
    this->MonPhySizeLabel = NULL;
    }
  if (this->MonPhySize)
    {
    this->ReleaseReferencesForKWEntrySet(this->MonPhySize);
    this->MonPhySize->DeleteAllWidgets();
    this->MonPhySize->SetParent(NULL);
    this->MonPhySize->Delete();
    this->MonPhySize = NULL;
    }
  if (this->UpdateAutoScale)
    {
    this->UpdateAutoScale->SetParent(NULL);
    this->UpdateAutoScale->Delete();
    this->UpdateAutoScale = NULL;
    }
  if (this->MonPixResFrame)
    {
    this->MonPixResFrame->SetParent(NULL);
    this->MonPixResFrame->Delete();
    this->MonPixResFrame = NULL;
    }
  if (this->MonPixResLabel)
    {
    this->MonPixResLabel->SetParent(NULL);
    this->MonPixResLabel->Delete();
    this->MonPixResLabel = NULL;
    }
  if (this->MonPixRes)
    {
    this->ReleaseReferencesForKWEntrySet(this->MonPixRes);
    this->MonPixRes->DeleteAllWidgets();
    this->MonPixRes->SetParent(NULL);
    this->MonPixRes->Delete();
    this->MonPixRes = NULL;
    }

  if (this->ImgPixSizeFrame)
    {
    this->ImgPixSizeFrame->SetParent(NULL);
    this->ImgPixSizeFrame->Delete();
    this->ImgPixSizeFrame = NULL;
    }
  if (this->ImgPixSizeLabel)
    {
    this->ImgPixSizeLabel->SetParent(NULL);
    this->ImgPixSizeLabel->Delete();
    this->ImgPixSizeLabel = NULL;
    }

  if (this->ImgSpacing)
    {
    this->ReleaseReferencesForKWEntrySet(this->ImgSpacing);
    this->ImgSpacing->DeleteAllWidgets();
    this->ImgSpacing->SetParent(NULL);
    this->ImgSpacing->Delete();
    this->ImgSpacing = NULL;
    }

  if (this->ImgScaleFrame)
    {
    this->ImgScaleFrame->SetParent(NULL);
    this->ImgScaleFrame->Delete();
    this->ImgScaleFrame = NULL;
    }
  if (this->ImgScaleLabel)
    {
    this->ImgScaleLabel->SetParent(NULL);
    this->ImgScaleLabel->Delete();
    this->ImgScaleLabel = NULL;
    }
  if (this->ImgScaling)
    {
    this->ReleaseReferencesForKWEntrySet(this->ImgScaling);
    this->ImgScaling->DeleteAllWidgets();
    this->ImgScaling->SetParent(NULL);
    this->ImgScaling->Delete();
    this->ImgScaling = NULL;
    }
  if (this->MonPixSizeFrame)
    {
    this->MonPixSizeFrame->SetParent(NULL);
    this->MonPixSizeFrame->Delete();
    this->MonPixSizeFrame = NULL;
    }
  if (this->MonPixSizeLabel)
    {
    this->MonPixSizeLabel->SetParent(NULL);
    this->MonPixSizeLabel->Delete();
    this->MonPixSizeLabel = NULL;
    }
  if (this->MonSpacing)
    {
    this->ReleaseReferencesForKWEntrySet(this->MonSpacing);
    this->MonSpacing->DeleteAllWidgets();
    this->MonSpacing->SetParent(NULL);
    this->MonSpacing->Delete();
    this->MonSpacing = NULL;
    }
  
  this->ImageScalingDone = false;

  // translate step
  if (this->TranslateFrame)
    {
    this->TranslateFrame->SetParent(NULL);
    this->TranslateFrame->Delete();
    this->TranslateFrame = NULL;
    }  
  if (this->TransMessage)
    {
    this->TransMessage->SetParent(NULL);
    this->TransMessage->Delete();
    this->TransMessage = NULL;
    }  
  if (this->TransImgFidFrame)
    {
    this->TransImgFidFrame->SetParent(NULL);
    this->TransImgFidFrame->Delete();
    this->TransImgFidFrame = NULL;
    }
  if (this->TransImgFidLabel)
    {
    this->TransImgFidLabel->SetParent(NULL);
    this->TransImgFidLabel->Delete();
    this->TransImgFidLabel = NULL;
    }
  if (this->TransImgFid)
    {
    this->ReleaseReferencesForKWEntrySet(this->TransImgFid);
    this->TransImgFid->DeleteAllWidgets();
    this->TransImgFid->SetParent(NULL);
    this->TransImgFid->Delete();
    this->TransImgFid = NULL;
    }
  if (this->TransPhyFidFrame)
    {
    this->TransPhyFidFrame->SetParent(NULL);
    this->TransPhyFidFrame->Delete();
    this->TransPhyFidFrame = NULL;
    }
  if (this->TransPhyFidLabel)
    {
    this->TransPhyFidLabel->SetParent(NULL);
    this->TransPhyFidLabel->Delete();
    this->TransPhyFidLabel = NULL;
    }  
  if (this->TransPhyFid)
    {
    this->ReleaseReferencesForKWEntrySet(this->TransPhyFid);
    this->TransPhyFid->DeleteAllWidgets();
    this->TransPhyFid->SetParent(NULL);
    this->TransPhyFid->Delete();
    this->TransPhyFid = NULL;
    }
  if (this->TransEntryFrame)
    {
    this->TransEntryFrame->SetParent(NULL);
    this->TransEntryFrame->Delete();
    this->TransEntryFrame = NULL;
    }
  if (this->TransEntryLabel)
    {
    this->TransEntryLabel->SetParent(NULL);
    this->TransEntryLabel->Delete();
    this->TransEntryLabel = NULL;
    }  
  /*
  if (this->Translation)
    {
    this->ReleaseReferencesForKWEntrySet(this->Translation);
    this->Translation->DeleteAllWidgets();
    this->Translation->SetParent(NULL);
    this->Translation->Delete();
    this->Translation = NULL;
    }
  */
  
  this->ImageTranslationDone = false;


  // rotate step
  if (this->RotateFrame)
    {
    this->RotateFrame->SetParent(NULL);
    this->RotateFrame->Delete();
    this->RotateFrame = NULL;
    }
  if (this->RotMessage)
    {
    this->RotMessage->SetParent(NULL);
    this->RotMessage->Delete();
    this->RotMessage = NULL;
    }
  if (this->CORFrame)
    {
    this->CORFrame->SetParent(NULL);
    this->CORFrame->Delete();
    this->CORFrame = NULL;
    }
  if (this->CORLabel)
    {
    this->CORLabel->SetParent(NULL);
    this->CORLabel->Delete();
    this->CORLabel = NULL;
    }    
  if (this->COR)
    {
    this->ReleaseReferencesForKWEntrySet(this->COR);
    this->COR->DeleteAllWidgets();
    this->COR->SetParent(NULL);
    this->COR->Delete();
    this->COR = NULL;
    }
  if (this->RotImgFidFrame)
    {
    this->RotImgFidFrame->SetParent(NULL);
    this->RotImgFidFrame->Delete();
    this->RotImgFidFrame = NULL;
    }
  if (this->RotImgFidLabel)
    {
    this->RotImgFidLabel->SetParent(NULL);
    this->RotImgFidLabel->Delete();
    this->RotImgFidLabel = NULL;
    }    
  
  if (this->RotImgFid)
    {
    this->ReleaseReferencesForKWEntrySet(this->RotImgFid);
    this->RotImgFid->DeleteAllWidgets();
    this->RotImgFid->SetParent(NULL);
    this->RotImgFid->Delete();
    this->RotImgFid = NULL;
    }
  if (this->RotPhyFidFrame)
    {
    this->RotPhyFidFrame->SetParent(NULL);
    this->RotPhyFidFrame->Delete();
    this->RotPhyFidFrame = NULL;
    }
  if (this->RotPhyFidLabel)
    {
    this->RotPhyFidLabel->SetParent(NULL);
    this->RotPhyFidLabel->Delete();
    this->RotPhyFidLabel = NULL;
    }
  if (this->RotPhyFid)
    {
    this->ReleaseReferencesForKWEntrySet(this->RotPhyFid);
    this->RotPhyFid->DeleteAllWidgets();
    this->RotPhyFid->SetParent(NULL);
    this->RotPhyFid->Delete();
    this->RotPhyFid = NULL;
    }
  if (this->RotImgFidFrame)
    {
    this->RotImgFidFrame->Delete();
    this->RotImgFidFrame->SetParent(NULL);
    this->RotImgFidFrame = NULL;
    }
  if (this->RotImgFidLabel)
    {
    this->RotImgFidLabel->SetParent(NULL);
    this->RotImgFidLabel->Delete();
    this->RotImgFidLabel = NULL;
    }      
  if (this->RotationAngle)
    {
    this->RotationAngle->SetParent(NULL);
    this->RotationAngle->Delete();
    this->RotationAngle = NULL;
    }

  this->ImageRotationDone = false;
  
  this->CurrentSubState = -1;
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
void vtkPerkStationCalibrateStep::EnableDisableFlipComponents(bool enable)
{
  this->FlipFrame->GetFrame()->SetEnabled(enable);
  this->VerticalFlipCheckButton->GetWidget()->SetEnabled(enable);
  this->HorizontalFlipCheckButton->GetWidget()->SetEnabled(enable);
}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::EnableDisableScaleComponents(bool enable)
{
  this->ScaleFrame->GetFrame()->SetEnabled(enable);
  switch (this->GetGUI()->GetMode())
    {
    case vtkPerkStationModuleGUI::Training:
      this->ImgSpacing->GetWidget(0)->SetEnabled(enable);
      this->ImgSpacing->GetWidget(1)->SetEnabled(enable);
      this->MonSpacing->GetWidget(0)->SetEnabled(enable);
      this->MonSpacing->GetWidget(1)->SetEnabled(enable);
      this->ImgScaling->GetWidget(0)->SetEnabled(enable);
      this->ImgScaling->GetWidget(1)->SetEnabled(enable);
      break;

    case vtkPerkStationModuleGUI::Clinical:
      this->MonPhySize->GetWidget(0)->SetEnabled(enable);
      this->MonPhySize->GetWidget(1)->SetEnabled(enable);
      this->MonPixRes->GetWidget(0)->SetEnabled(enable);
      this->MonPixRes->GetWidget(1)->SetEnabled(enable);
      this->UpdateAutoScale->SetEnabled(enable);
      break;
    }
  

}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::EnableDisableTranslateComponents(bool enable)
{
  this->TranslateFrame->GetFrame()->SetEnabled(enable);
  switch (this->GetGUI()->GetMode())
    {
    case vtkPerkStationModuleGUI::Clinical:
      break;
    }

  

}
//---------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::EnableDisableRotateComponents(bool enable)
{
  this->RotateFrame->GetFrame()->SetEnabled(enable);
  switch (this->GetGUI()->GetMode())
    {
    case vtkPerkStationModuleGUI::Training:
      this->COR->GetWidget(0)->SetEnabled(enable);
      this->COR->GetWidget(1)->SetEnabled(enable);    
      this->RotImgFid->GetWidget(0)->SetEnabled(enable);
      this->RotImgFid->GetWidget(1)->SetEnabled(enable);
      this->RotPhyFid->GetWidget(0)->SetEnabled(enable);
      this->RotPhyFid->GetWidget(1)->SetEnabled(enable);
      this->RotationAngle->GetWidget()->SetEnabled(enable);
      break;

    case vtkPerkStationModuleGUI::Clinical:
      this->COR->GetWidget(0)->SetEnabled(enable);
      this->COR->GetWidget(1)->SetEnabled(enable);
      break;
    }

  


}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::EnableDisableControls()
{
  if (this->GetGUI()->GetMode() == vtkPerkStationModuleGUI::Training)
  {
  switch (this->CurrentSubState)
    {
    case 0: // indicates flip state
        EnableDisableFlipComponents(true);
        EnableDisableScaleComponents(false);
        EnableDisableTranslateComponents(false);
        EnableDisableRotateComponents(false);
        break;
    case 1: //indicates Scale state; enable only scale, disable translate and rotate components
        EnableDisableFlipComponents(false);
        EnableDisableScaleComponents(true);
        EnableDisableTranslateComponents(false);
        EnableDisableRotateComponents(false);
        break;
    case 2: //indicates Translate state; enable only translate state, disable other two
        EnableDisableFlipComponents(false);
        EnableDisableScaleComponents(false);
        EnableDisableTranslateComponents(true);
        EnableDisableRotateComponents(false);
        break;
    case 3: //indicates Rotate state; enable only Rotate state, disable other two
        EnableDisableFlipComponents(false);
        EnableDisableScaleComponents(false);
        EnableDisableTranslateComponents(false);
        EnableDisableRotateComponents(true);
        break;

    default:
        EnableDisableFlipComponents(false);
        EnableDisableScaleComponents(false);
        EnableDisableTranslateComponents(false);
        EnableDisableRotateComponents(false);
        
        break;
    }
  }
  else
  {
    switch (this->CurrentSubState)
    {
        case -1:
        EnableDisableLoadResetControls(false);
        EnableDisableFlipComponents(false);
        EnableDisableScaleComponents(false);
        EnableDisableTranslateComponents(false);
        EnableDisableRotateComponents(false);
        EnableDisableSaveControls(false);
        break;

        default:
        EnableDisableLoadResetControls(true);
        EnableDisableFlipComponents(true);
        EnableDisableScaleComponents(true);
        EnableDisableTranslateComponents(true);
        EnableDisableRotateComponents(true);
        EnableDisableSaveControls(true);
        break;
    }
        
  }

}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();
  
  switch (this->GetGUI()->GetMode())      
    {

    case vtkPerkStationModuleGUI::Training:

      this->SetName("1/5. Calibrate");
      this->GetGUI()->GetWizardWidget()->SetErrorText( "Please note that the order of the click (first image fiducial, then physical fiducial) is important.");
      this->GetGUI()->GetWizardWidget()->Update();  
      break;

    case vtkPerkStationModuleGUI::Clinical:
       
      // in clinical mode
      this->SetName("1/4. Calibrate");
      this->GetGUI()->GetWizardWidget()->SetErrorText( "");
      this->GetGUI()->GetWizardWidget()->Update();
      break;
    }

  
  this->SetDescription("Do image overlay system calibration");

  // gui building/showing

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);

  
  // load/reset controls, if needed
  this->ShowLoadResetControls();
  
  this->ShowHardwareSelector();
  
  //flip components
  this->ShowFlipComponents();

  // scale step components
  this->ShowScaleComponents();
  
  // translate components
  this->ShowTranslateComponents();
  
  // rotate components
  this->ShowRotateComponents();
  
  // save calibration controls if needed
  this->ShowSaveControls();
  
  // TO DO: populate controls wherever needed
  this->PopulateControls();

   // enable disable controls  
  this->EnableDisableControls();
    
  // TO DO: install callbacks
  this->InstallCallbacks();
  
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->
    GetSliceViewer()->Focus();
  
}

//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ShowLoadResetControls()
{
 vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // first clear the components if they created before for other mode
  this->ClearLoadResetControls();

  
  switch (this->GetGUI()->GetMode())      
    {

    case vtkPerkStationModuleGUI::Training:
      // no controls to show at the moment

      break;

    case vtkPerkStationModuleGUI::Clinical:
       
      // in clinical mode
      {
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
      if (!this->LoadCalibrationFileButton)
        {
        this->LoadCalibrationFileButton = vtkKWLoadSaveButton::New();
        }
      if (!this->LoadCalibrationFileButton->IsCreated())
        {
        this->LoadCalibrationFileButton->SetParent(this->LoadResetFrame);
        this->LoadCalibrationFileButton->Create();
        this->LoadCalibrationFileButton->SetBorderWidth(2);
        this->LoadCalibrationFileButton->SetReliefToRaised();       
        this->LoadCalibrationFileButton->SetHighlightThickness(2);
        this->LoadCalibrationFileButton->SetBackgroundColor(0.85,0.85,0.85);
        this->LoadCalibrationFileButton->SetActiveBackgroundColor(1,1,1);
        this->LoadCalibrationFileButton->SetText("Load calibration");
        this->LoadCalibrationFileButton->SetImageToPredefinedIcon(vtkKWIcon::IconPresetLoad);
        this->LoadCalibrationFileButton->SetBalloonHelpString("Click to load a previous calibration file");
        this->LoadCalibrationFileButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
        this->LoadCalibrationFileButton->TrimPathFromFileNameOff();
        this->LoadCalibrationFileButton->SetMaximumFileNameLength(256);
        this->LoadCalibrationFileButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode
        this->LoadCalibrationFileButton->GetLoadSaveDialog()->SetFileTypes("{{XML File} {.xml}} {{All Files} {*.*}}");      
        }
      this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                            this->LoadCalibrationFileButton->GetWidgetName());

      
      // create the load file dialog button
      if (!this->SaveCalibrationFileButton)
        {
        this->SaveCalibrationFileButton = vtkKWLoadSaveButton::New();
        }
      if (!this->SaveCalibrationFileButton->IsCreated())
        {
        this->SaveCalibrationFileButton->SetParent( this->LoadResetFrame );
        this->SaveCalibrationFileButton->Create();
        this->SaveCalibrationFileButton->SetText("Save calibration");
        this->SaveCalibrationFileButton->SetBorderWidth(2);
        this->SaveCalibrationFileButton->SetReliefToRaised();       
        this->SaveCalibrationFileButton->SetHighlightThickness(2);
        this->SaveCalibrationFileButton->SetBackgroundColor(0.85,0.85,0.85);
        this->SaveCalibrationFileButton->SetActiveBackgroundColor(1,1,1);               
        this->SaveCalibrationFileButton->SetImageToPredefinedIcon(vtkKWIcon::IconFloppy);
        this->SaveCalibrationFileButton->SetBalloonHelpString("Click to save calibration in a file");
        this->SaveCalibrationFileButton->GetLoadSaveDialog()->SaveDialogOn(); // save mode
        this->SaveCalibrationFileButton->TrimPathFromFileNameOff();
        this->SaveCalibrationFileButton->SetMaximumFileNameLength(256);
        this->SaveCalibrationFileButton->GetLoadSaveDialog()->SetFileTypes("{{XML File} {.xml}} {{All Files} {*.*}}");      
        this->SaveCalibrationFileButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
        }
      this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                            this->SaveCalibrationFileButton->GetWidgetName());
      
      
      // create the reset calib button
       if (!this->ResetCalibrationButton)
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
        this->ResetCalibrationButton->Create();
        }
      
      this->Script("pack %s -side top -anchor ne -padx 2 -pady 4", 
                        this->ResetCalibrationButton->GetWidgetName());
      }
      break;
    }

}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ShowSaveControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // first clear the components if they created before for other mode
  this->ClearSaveControls();

  
    // Create the frame
  if (!this->SaveFrame)
    {
    this->SaveFrame = vtkKWFrame::New();
    }
  if (!this->SaveFrame->IsCreated())
    {
    this->SaveFrame->SetParent(parent);
    this->SaveFrame->Create();
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->SaveFrame->GetWidgetName());
}


void
vtkPerkStationCalibrateStep
::ShowHardwareSelector()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  if ( ! this->HardwareFrame )
    {
    this->HardwareFrame = vtkKWFrameWithLabel::New();
    }
  
  if ( ! this->HardwareFrame->IsCreated() )
    {
    this->HardwareFrame->SetParent( parent );
    this->HardwareFrame->Create();
    this->HardwareFrame->SetLabelText( "Overlay hardware" );
    }
  
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->HardwareFrame->GetWidgetName());
  
    // Contents of the hardware frame.
    
  if ( ! this->HardwareMenu )
    {
    this->HardwareMenu = vtkKWMenuButtonWithLabel::New();
    }
  
  if ( ! this->HardwareMenu->IsCreated() )
    {
    this->HardwareMenu->SetParent( this->HardwareFrame->GetFrame() );
    this->HardwareMenu->Create();
    this->HardwareMenu->GetWidget()->GetMenu()->
      AddRadioButton( "Siemens MR compatible" );
    this->HardwareMenu->GetWidget()->GetMenu()->
      AddRadioButton( "PerkStation ViewSonic" );
    this->HardwareMenu->GetWidget()->SetValue( "Siemens MR compatible" );
    }
  
  this->Script("pack %s -side left -anchor nw -padx 8 -pady 2", 
                  this->HardwareMenu->GetWidgetName());

}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ShowFlipComponents()
{
  
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  this->ClearFlipComponents();
  
  
  // Create the frame
  if (!this->FlipFrame)
    {
    this->FlipFrame = vtkKWFrameWithLabel::New();
    }
  if (!this->FlipFrame->IsCreated())
    {
    this->FlipFrame->SetParent(parent);
    this->FlipFrame->Create();
    this->FlipFrame->SetLabelText("Flip");
    this->FlipFrame->SetBalloonHelpString("Check whether vertical flip or horizontal flip required");   
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                  this->FlipFrame->GetWidgetName());

  // individual components
  if (!this->VerticalFlipCheckButton)
    {
    this->VerticalFlipCheckButton = vtkKWCheckButtonWithLabel::New();
    }
  if (!this->VerticalFlipCheckButton->IsCreated())
    {
    this->VerticalFlipCheckButton->SetParent(this->FlipFrame->GetFrame());
    this->VerticalFlipCheckButton->Create();
    this->VerticalFlipCheckButton->GetLabel()->SetBackgroundColor(0.7,0.7,0.7);
    this->VerticalFlipCheckButton->SetLabelText("Vertical Flip:");
    this->VerticalFlipCheckButton->SetHeight(4);
    this->VerticalFlipCheckButton->GetWidget()->SetIndicatorVisibility(0);
    }
   
  this->Script("pack %s -side left -anchor nw -padx 8 -pady 2", 
                  this->VerticalFlipCheckButton->GetWidgetName());

  // individual components
  if (!this->HorizontalFlipCheckButton)
    {
    this->HorizontalFlipCheckButton = vtkKWCheckButtonWithLabel::New();
    }
  if (!this->HorizontalFlipCheckButton->IsCreated())
    {
    this->HorizontalFlipCheckButton->SetParent(this->FlipFrame->GetFrame());
    this->HorizontalFlipCheckButton->Create();
    this->HorizontalFlipCheckButton->GetLabel()->SetBackgroundColor(0.7,0.7,0.7);
    this->HorizontalFlipCheckButton->SetLabelText("Horizontal Flip:");
    this->HorizontalFlipCheckButton->SetHeight(4);
    }
   
  this->Script("pack %s -side top -anchor nw -padx 8 -pady 2", 
                  this->HorizontalFlipCheckButton->GetWidgetName());
}

//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ShowScaleComponents()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // first clear the components if they created before for other mode
  this->ClearScaleComponents();

  
  switch (this->GetGUI()->GetMode())      
    {

    case vtkPerkStationModuleGUI::Training:
      {
      // Create the frame
        if (!this->ScaleFrame)
            {
            this->ScaleFrame = vtkKWFrameWithLabel::New();
            }
        if (!this->ScaleFrame->IsCreated())
            {
            this->ScaleFrame->SetParent(parent);
            this->ScaleFrame->Create();
            this->ScaleFrame->SetLabelText("Scale");
            this->ScaleFrame->SetBalloonHelpString("Enter the image scaling require to map image from dicom space to monitor's physical space");
            }
        this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                        this->ScaleFrame->GetWidgetName());

      // create the invidual components within the scale frame
      
      // frame
      if (!this->ImgPixSizeFrame)
        {
        this->ImgPixSizeFrame = vtkKWFrame::New();
        }
      if (!this->ImgPixSizeFrame->IsCreated())
        {
        this->ImgPixSizeFrame->SetParent(this->ScaleFrame->GetFrame());
        this->ImgPixSizeFrame->Create();
        }

      this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->ImgPixSizeFrame->GetWidgetName());

      // label
      if (!this->ImgPixSizeLabel)
        { 
        this->ImgPixSizeLabel = vtkKWLabel::New();
        }
      if (!this->ImgPixSizeLabel->IsCreated())
        {
        this->ImgPixSizeLabel->SetParent(this->ImgPixSizeFrame);
        this->ImgPixSizeLabel->Create();
        this->ImgPixSizeLabel->SetText("Image pixel size:   ");
        this->ImgPixSizeLabel->SetBackgroundColor(0.7, 0.7, 0.7);
        }
      
      this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                    this->ImgPixSizeLabel->GetWidgetName());
      

      // entries of image spacing; actual data will be populated in another function, which will be called in the end
      // after all gui elements have been created

      if (!this->ImgSpacing)
        {
        this->ImgSpacing =  vtkKWEntrySet::New();   
        }
      if (!this->ImgSpacing->IsCreated())
        {
        this->ImgSpacing->SetParent(this->ImgPixSizeFrame);
        this->ImgSpacing->Create();
        this->ImgSpacing->SetBorderWidth(2);
        this->ImgSpacing->SetReliefToGroove();
        this->ImgSpacing->SetPackHorizontally(1);
        this->ImgSpacing->SetWidgetsInternalPadX(2);  
        this->ImgSpacing->SetMaximumNumberOfWidgetsInPackingDirection(2);
        // two entries of image spacing (x, y)
        for (int id = 0; id < 2; id++)
          {
          vtkKWEntry *entry = this->ImgSpacing->AddWidget(id);  
          entry->SetWidth(7);
          entry->ReadOnlyOn();
          entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
          }
        }

      this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                    this->ImgSpacing->GetWidgetName());


      // monitor pixel spacing
      if (!this->MonPixSizeFrame)
        {
        this->MonPixSizeFrame = vtkKWFrame::New();
        }
      if (!this->MonPixSizeFrame->IsCreated())
        {
        this->MonPixSizeFrame->SetParent(this->ScaleFrame->GetFrame());
        this->MonPixSizeFrame->Create();
        }

      this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->MonPixSizeFrame->GetWidgetName());

      // label
      if (!this->MonPixSizeLabel)
        { 
        this->MonPixSizeLabel = vtkKWLabel::New();
        }
      if (!this->MonPixSizeLabel->IsCreated())
        {
        this->MonPixSizeLabel->SetParent(this->MonPixSizeFrame);
        this->MonPixSizeLabel->Create();
        this->MonPixSizeLabel->SetText("Monitor pixel size: ");
        this->MonPixSizeLabel->SetBackgroundColor(0.7, 0.7, 0.7);
        }
      
      this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                    this->MonPixSizeLabel->GetWidgetName());
     
      if (!this->MonSpacing)
        {
        this->MonSpacing =  vtkKWEntrySet::New();   
        }
      if (!this->MonSpacing->IsCreated())
        {
        this->MonSpacing->SetParent(this->MonPixSizeFrame);
        this->MonSpacing->Create();
        this->MonSpacing->SetBorderWidth(2);
        this->MonSpacing->SetReliefToGroove();
        this->MonSpacing->SetPackHorizontally(1);
        this->MonSpacing->SetWidgetsInternalPadX(2);  
        this->MonSpacing->SetMaximumNumberOfWidgetsInPackingDirection(2);
        // two entries of monitor spacing (x, y)
        for (int id = 0; id < 2; id++)
          {
          vtkKWEntry *entry = this->MonSpacing->AddWidget(id);    
          entry->ReadOnlyOn();
          entry->SetWidth(7);
          entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
          }
        }

      this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                    this->MonSpacing->GetWidgetName());
     
      // Scaling
      if (!this->ImgScaleFrame)
        {
        this->ImgScaleFrame = vtkKWFrame::New();
        }
      if (!this->ImgScaleFrame->IsCreated())
        {
        this->ImgScaleFrame->SetParent(this->ScaleFrame->GetFrame());
        this->ImgScaleFrame->Create();
        }
      
      this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->ImgScaleFrame->GetWidgetName());

     
      // label
      if (!this->ImgScaleLabel)
        { 
        this->ImgScaleLabel = vtkKWLabel::New();
        }
      if (!this->ImgScaleLabel->IsCreated())
        {
        this->ImgScaleLabel->SetParent(this->ImgScaleFrame);
        this->ImgScaleLabel->Create();
        this->ImgScaleLabel->SetText("Image scaling:       ");
        this->ImgScaleLabel->SetBackgroundColor(0.7, 0.7, 0.7);
        }
      
      this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                    this->ImgScaleLabel->GetWidgetName());
      
      if (!this->ImgScaling)
        {
        this->ImgScaling =  vtkKWEntrySet::New();   
        }
      if (!this->ImgScaling->IsCreated())
        {
        this->ImgScaling->SetParent(this->ImgScaleFrame);
        this->ImgScaling->Create();
        this->ImgScaling->SetBorderWidth(2);
        this->ImgScaling->SetReliefToGroove();
        this->ImgScaling->SetPackHorizontally(1);
        this->ImgScaling->SetWidgetsInternalPadX(2);  
        this->ImgScaling->SetMaximumNumberOfWidgetsInPackingDirection(2);
        // two entries of monitor spacing (x, y)
        for (int id = 0; id < 2; id++)
          {
          vtkKWEntry *entry = this->ImgScaling->AddWidget(id);
          entry->SetWidth(7);
          entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
          //entry->ReadOnlyOn(); 
          }
        }

      this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                    this->ImgScaling->GetWidgetName());
      }
      break;

    case vtkPerkStationModuleGUI::Clinical:
       
      // in clinical mode

      {
          // Create the frame
        if (!this->ScaleFrame)
            {
            this->ScaleFrame = vtkKWFrameWithLabel::New();
            }
        if (!this->ScaleFrame->IsCreated())
            {
            this->ScaleFrame->SetParent(parent);
            this->ScaleFrame->Create();
            this->ScaleFrame->SetLabelText("Scale");
            this->ScaleFrame->SetBalloonHelpString("Verify, correct if needed, physical dimensions of secondary monitor, and pixel resolution");
            }
        this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                        this->ScaleFrame->GetWidgetName());

      // monitor physical size

      if (!this->MonPhySizeFrame)
        {
        this->MonPhySizeFrame = vtkKWFrame::New();
        }
      if (!this->MonPhySizeFrame->IsCreated())
        {
        this->MonPhySizeFrame->SetParent(this->ScaleFrame->GetFrame());
        this->MonPhySizeFrame->Create();
        }

      this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                        this->MonPhySizeFrame->GetWidgetName());

      // label
      if (!this->MonPhySizeLabel)
        { 
        this->MonPhySizeLabel = vtkKWLabel::New();
        }
      if (!this->MonPhySizeLabel->IsCreated())
        {
        this->MonPhySizeLabel->SetParent(this->MonPhySizeFrame);
        this->MonPhySizeLabel->Create();
        this->MonPhySizeLabel->SetText("Monitor physical size (mm):   ");
        this->MonPhySizeLabel->SetBackgroundColor(0.7, 0.7, 0.7);
        }
          
      this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                        this->MonPhySizeLabel->GetWidgetName());
          

      // entries of monitor physical size; actual data will be populated in another function, which will be called in the end
      // after all gui elements have been created

      if (!this->MonPhySize)
        {
        this->MonPhySize =  vtkKWEntrySet::New();   
        }
      if (!this->MonPhySize->IsCreated())
        {
        this->MonPhySize->SetParent(this->MonPhySizeFrame);
        this->MonPhySize->Create();
        this->MonPhySize->SetBorderWidth(2);
        this->MonPhySize->SetReliefToGroove();
        this->MonPhySize->SetPackHorizontally(1);
        this->MonPhySize->SetWidgetsInternalPadX(2);  
        this->MonPhySize->SetMaximumNumberOfWidgetsInPackingDirection(2);
        
        // two entries of monitor size (x, y)
        for (int id = 0; id < 2; id++)
          {
          vtkKWEntry *entry = this->MonPhySize->AddWidget(id);  
          entry->SetWidth(7);
          //entry->ReadOnlyOn();
          entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
          }
        }

      this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                        this->MonPhySize->GetWidgetName());

      // monitor pizel resolution

      if (!this->MonPixResFrame)
        {
        this->MonPixResFrame = vtkKWFrame::New();
        }
      if (!this->MonPixResFrame->IsCreated())
        {
        this->MonPixResFrame->SetParent(this->ScaleFrame->GetFrame());
        this->MonPixResFrame->Create();
        }

      this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                        this->MonPixResFrame->GetWidgetName());

      // label
      if (!this->MonPixResLabel)
        { 
        this->MonPixResLabel = vtkKWLabel::New();
        }
      if (!this->MonPixResLabel->IsCreated())
        {
        this->MonPixResLabel->SetParent(this->MonPixResFrame);
        this->MonPixResLabel->Create();
        this->MonPixResLabel->SetText("Monitor pixel resolution:   ");
        this->MonPixResLabel->SetBackgroundColor(0.7, 0.7, 0.7);
        }
          
      this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                        this->MonPixResLabel->GetWidgetName());
          

      // entries of monitor physical size; actual data will be populated in another function, which will be called in the end
      // after all gui elements have been created

      if (!this->MonPixRes)
        {
        this->MonPixRes =  vtkKWEntrySet::New();   
        }
      if (!this->MonPixRes->IsCreated())
        {
        this->MonPixRes->SetParent(this->MonPixResFrame);
        this->MonPixRes->Create();
        this->MonPixRes->SetBorderWidth(2);
        this->MonPixRes->SetReliefToGroove();
        this->MonPixRes->SetPackHorizontally(1);
        this->MonPixRes->SetWidgetsInternalPadX(2);  
        this->MonPixRes->SetMaximumNumberOfWidgetsInPackingDirection(2);
        
        // two entries of monitor size (x, y)
        for (int id = 0; id < 2; id++)
          {
          vtkKWEntry *entry = this->MonPixRes->AddWidget(id);  
          entry->SetWidth(7);
          //entry->ReadOnlyOn();
          entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
          }
        }

      this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                        this->MonPixRes->GetWidgetName());


      if (!this->UpdateAutoScale)
        {
        this->UpdateAutoScale = vtkKWPushButton::New();
        }
      if(!this->UpdateAutoScale->IsCreated())
        {
        this->UpdateAutoScale->SetParent(this->MonPixResFrame);
        this->UpdateAutoScale->SetText("Update");
        this->UpdateAutoScale->Create();
        }
      
      this->Script("pack %s -side top -anchor ne -padx 2 -pady 4", 
                        this->UpdateAutoScale->GetWidgetName());
      }
      break;
    }

}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ShowTranslateComponents()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
   
  // first clear the translate components if they were created by previous mode
  this->ClearTranslateComponents();

  

  switch (this->GetGUI()->GetMode())
    {

    case vtkPerkStationModuleGUI::Clinical:
      {
      // Create the frame
      if (!this->TranslateFrame)
        {
        this->TranslateFrame = vtkKWFrameWithLabel::New();
        }
      if (!this->TranslateFrame->IsCreated())
        {
        this->TranslateFrame->SetParent(parent);
        this->TranslateFrame->Create();
        this->TranslateFrame->SetLabelText("Translate");
        this->TranslateFrame->SetBalloonHelpString(
          "Use arrow keys to translate and align image with phatom/patient");
        }
      // this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
      //               this->TranslateFrame->GetWidgetName());
      if(!this->TransMessage)
        {
        this->TransMessage = vtkKWText::New();
        }
      if(!this->TransMessage->IsCreated())
        {
        this->TransMessage->SetParent(this->TranslateFrame->GetFrame());
        this->TransMessage->Create();
        this->TransMessage->SetText(
          "First make sure that the secondary monitor image window has 'focus'."
          " You can click in that window to get focus. Use arrow keys on the "
          "keyboard to move/translate image. Use corresponding numeric keypad "
          "with NumLock 'On' for finer movement" );      
        this->TransMessage->SetBackgroundColor(0.7, 0.7, 0.95);
        this->TransMessage->SetHeight(6);
        this->TransMessage->SetWrapToWord();
        this->TransMessage->ReadOnlyOn();
        this->TransMessage->SetBorderWidth(2);
        this->TransMessage->SetReliefToGroove();
        this->TransMessage->SetBalloonHelpString(
            "Allows you align image fiducials and physical fiducials "
            "as seen through mirror ");
        }
        // this->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 6", 
        //            this->TransMessage->GetWidgetName());
      }
      break;
    } 
}


//----------------------------------------------------------------------------
void
vtkPerkStationCalibrateStep
::ShowRotateComponents()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  this->ClearRotateComponents();
  
  switch (this->GetGUI()->GetMode())
    {
    case vtkPerkStationModuleGUI::Clinical :
      {
      // Create the frame
      if (!this->RotateFrame)
        {
        this->RotateFrame = vtkKWFrameWithLabel::New();
        }
      if (!this->RotateFrame->IsCreated())
        {
        this->RotateFrame->SetParent(parent);
        this->RotateFrame->Create();
        this->RotateFrame->SetLabelText("Rotate");
        this->RotateFrame->SetBalloonHelpString(
          "Perform fine rotational alignment of image and phantom/patient");
        }
      // this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
      //               this->RotateFrame->GetWidgetName());

      // center of rotation   
      if (!this->CORFrame)
        {
        this->CORFrame = vtkKWFrame::New();
        }
      if (!this->CORFrame->IsCreated())
        {
        this->CORFrame->SetParent(this->RotateFrame->GetFrame());
        this->CORFrame->Create();
        }

      // this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
      //                   this->CORFrame->GetWidgetName());


      // label
      if (!this->CORLabel)
        { 
        this->CORLabel = vtkKWLabel::New();
        }
      if (!this->CORLabel->IsCreated())
        {
        this->CORLabel->SetParent(this->CORFrame);
        this->CORLabel->Create();
        this->CORLabel->SetText("Center of rotation: ");
        this->CORLabel->SetBackgroundColor(0.7, 0.7, 0.7);
        }
            
      // this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
      //                   this->CORLabel->GetWidgetName());
            

      if (!this->COR)
        {
        this->COR =  vtkKWEntrySet::New();  
        }
      if (!this->COR->IsCreated())
        {
        this->COR->SetParent(this->CORFrame);
        this->COR->Create();
        this->COR->SetBorderWidth(2);
        this->COR->SetReliefToGroove();
        this->COR->SetPackHorizontally(1);
        this->COR->SetMaximumNumberOfWidgetsInPackingDirection(2);
        // two entries of monitor spacing (x, y)
        for (int id = 0; id < 2; id++)
            {
            vtkKWEntry *entry = this->COR->AddWidget(id);
            entry->SetWidth(7);
            entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
            //entry->ReadOnlyOn(); 
            }
        }
        
      //this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
      //                  this->COR->GetWidgetName());
      if(!this->RotMessage)
        {
        this->RotMessage = vtkKWText::New();
        }
      if(!this->RotMessage->IsCreated())
        {
        this->RotMessage->SetParent(this->RotateFrame->GetFrame());
        this->RotMessage->Create();
        this->RotMessage->SetText("First make sure that the secondary monitor image window has 'focus'. You can click in that window to get focus. Use PageUp key for clockwise rotation, Home key for anti-clockwise. Use corresponding numeric keys 9 and 7 for finer rotation. Note that Center of Rotation must be specified prior to performing rotation.");      
        this->RotMessage->SetBackgroundColor(0.7, 0.7, 0.95);
        this->RotMessage->SetBorderWidth(2);
        this->RotMessage->SetHeight(6);
        this->RotMessage->SetWrapToWord();
        this->RotMessage->ReadOnlyOn();
        this->RotMessage->SetReliefToGroove();
        this->RotMessage->SetBalloonHelpString(
            "Allows you align image fiducials and physical fiducials "
            "as seen through mirror ");
        }
      //this->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 6", 
      //              this->RotMessage->GetWidgetName());
      }
      break;
    } 

  

  
}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::InstallCallbacks()
{
  this->AddGUIObservers();
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::PopulateControls()
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
 
  switch (this->GetGUI()->GetMode())
    {
    case vtkPerkStationModuleGUI::Training:
    if(!this->TrainingModeControlsPopulated)
      {
      // populate flip frame components
      if (this->VerticalFlipCheckButton)
        {
        this->VerticalFlipCheckButton->GetWidget()->SetSelectedState(0);
        }
      if(this->HorizontalFlipCheckButton)
        {
        this->HorizontalFlipCheckButton->GetWidget()->SetSelectedState(0);
        }


      // populate scale frame controls
      // 1. image spacing
      double imgSpacing[3];
      inVolume->GetSpacing(imgSpacing);

      if (this->ImgSpacing)
        {
        this->ImgSpacing->GetWidget(0)->SetValueAsDouble(imgSpacing[0]);
        this->ImgSpacing->GetWidget(1)->SetValueAsDouble(imgSpacing[1]);
        }

      // 2. monitor spacing
      double monSpacing[2];
      this->GetGUI()->GetSecondaryMonitor()->GetMonitorSpacing(monSpacing[0], monSpacing[1]);

      if (this->MonSpacing)
        {
        this->MonSpacing->GetWidget(0)->SetValueAsDouble(monSpacing[0]);
        this->MonSpacing->GetWidget(1)->SetValueAsDouble(monSpacing[1]);
        }

          
      // set the actual scaling (image/mon) in mrml node
      mrmlNode->SetActualScaling(double(imgSpacing[0]/monSpacing[0]), double(imgSpacing[1]/monSpacing[1]));

      // populate translate frame controls
      // the information will come in a callback that handles the mouse click

      // populate rotation frame controls
      // center of rotation
      /*vtkSlicerSliceLogic *sliceLogic = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Red")->GetLogic();
      double rasDimensions[3], rasCenter[3], sliceCenter[3];
      sliceLogic->GetVolumeRASBox (inVolume, rasDimensions, rasCenter);
      this->COR->GetWidget(0)->SetValueAsDouble(rasCenter[0]);
      this->COR->GetWidget(1)->SetValueAsDouble(rasCenter[1]);
      sliceLogic->GetVolumeSliceDimensions(inVolume, rasDimensions, sliceCenter);
      rasCenter[2] = sliceCenter[2];
      mrmlNode->SetCenterOfRotation(rasCenter);*/
      this->TrainingModeControlsPopulated = true;
      }
      break;

    case vtkPerkStationModuleGUI::Clinical:
    if(!this->ClinicalModeControlsPopulated)
      {
      // populate flip frame components
      this->VerticalFlipCheckButton->GetWidget()->SetSelectedState(0);
      this->HorizontalFlipCheckButton->GetWidget()->SetSelectedState(0);

      // scale components
      double monPhySize[2];
      this->GetGUI()->GetSecondaryMonitor()->GetPhysicalSize(monPhySize[0], monPhySize[1]);
      this->MonPhySize->GetWidget(0)->SetValueAsDouble(monPhySize[0]);
      this->MonPhySize->GetWidget(1)->SetValueAsDouble(monPhySize[1]);

      double monPixRes[2];
      this->GetGUI()->GetSecondaryMonitor()->GetPixelResolution(monPixRes[0], monPixRes[1]);
      this->MonPixRes->GetWidget(0)->SetValueAsDouble(monPixRes[0]);
      this->MonPixRes->GetWidget(1)->SetValueAsDouble(monPixRes[1]);

      double monSpacing[2];
      this->GetGUI()->GetSecondaryMonitor()->GetMonitorSpacing(monSpacing[0], monSpacing[1]);
      
      double imgSpacing[3];
      inVolume->GetSpacing(imgSpacing);

           
      // set the actual scaling (image/mon) in mrml node
      mrmlNode->SetActualScaling(double(imgSpacing[0]/monSpacing[0]), double(imgSpacing[1]/monSpacing[1]));

      this->ImageScalingDone = false;

      this->ImageScalingDone = true;

      this->ClinicalModeControlsPopulated = true;
      }
      break;
    } 

  

  this->CurrentSubState = 0;

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
 
  switch (this->GetGUI()->GetMode())
    {
    case vtkPerkStationModuleGUI::Training:
      // error
      break;

    case vtkPerkStationModuleGUI::Clinical:
   
      {
      // populate flip frame components
      this->VerticalFlipCheckButton->GetWidget()->SetSelectedState(mrmlNode->GetVerticalFlip());
      this->HorizontalFlipCheckButton->GetWidget()->SetSelectedState(mrmlNode->GetHorizontalFlip());

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


      // rotate components    
      double mrmlCOR[3];
      mrmlNode->GetCenterOfRotation(mrmlCOR[0], mrmlCOR[1], mrmlCOR[2]);  
      this->CORSpecified = true;
    
      this->COR->GetWidget(0)->SetValueAsDouble(mrmlCOR[0]);
      this->COR->GetWidget(1)->SetValueAsDouble(mrmlCOR[1]);

      this->GetGUI()->GetWizardWidget()->SetErrorText( "");
      this->GetGUI()->GetWizardWidget()->Update();      
      }
      
      break;
    } 
  
  this->CurrentSubState = 0;
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::HorizontalFlipCallback( bool value )
{
  if ( this->CurrentSubState != 0 ) return;
  
  // set user scaling in mrml node
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if (mrmlNode)
    this->GetGUI()->GetMRMLNode()->SetHorizontalFlip( ( bool ) value );
}


void
vtkPerkStationCalibrateStep
::HardwareMenuCallback()
{
  
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::VerticalFlipCallback( bool value )
{
  if ( this->CurrentSubState != 0 ) return;
 
  // set user scaling in mrml node
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  
  if (mrmlNode)
    this->GetGUI()->GetMRMLNode()->SetVerticalFlip((bool)value);
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

 
  
  double mmX = this->MonPhySize->GetWidget(0)->GetValueAsDouble();
  double mmY = this->MonPhySize->GetWidget(1)->GetValueAsDouble();

  double pixX = this->MonPixRes->GetWidget(0)->GetValueAsDouble();
  double pixY = this->MonPixRes->GetWidget(1)->GetValueAsDouble();

  // set the values in secondary monitor
  this->GetGUI()->GetSecondaryMonitor()->SetPhysicalSize(mmX,mmY);
  this->GetGUI()->GetSecondaryMonitor()->SetPixelResolution(pixX, pixY);

  // calculate new scaling
  // note updating monitor physical size in previous lines of code, will automatically update correct monitor spacing
  double monSpacing[2];
  this->GetGUI()->GetSecondaryMonitor()->GetMonitorSpacing(monSpacing[0], monSpacing[1]);
    
  double imgSpacing[3];
  inVolume->GetSpacing(imgSpacing);
      
  // set the actual scaling (image/mon) in mrml node
  mrmlNode->SetActualScaling(double(imgSpacing[0]/monSpacing[0]), double(imgSpacing[1]/monSpacing[1]));

    
  // reset current & rescale to new scaling
  // reset flip/scale/rotate/translate on secondary display
  this->GetGUI()->GetSecondaryMonitor()->ResetCalibration();
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::COREntryCallback(int widgetIndex)
{
  if (this->CurrentSubState != 3 && this->GetGUI()->GetMode() != vtkPerkStationModuleGUI::Clinical)
      return;
  
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  double mrmlCOR[3];
  mrmlNode->GetCenterOfRotation(mrmlCOR[0], mrmlCOR[1], mrmlCOR[2]);

  static bool xValChanged = false;
  static bool yValChanged = false;

  if (widgetIndex == 0)
    {
    // x scaling entered
    if (strcmpi(this->COR->GetWidget(0)->GetValue(), "")!=0)
      {
      if(!this->DoubleEqual(mrmlCOR[0],this->COR->GetWidget(0)->GetValueAsDouble()))
        {
        xValChanged = true;
        }
      // counter the case when the user enters the same value in entry, which is equal to the initial value set in MRML node
      // in this case above code, won't detect change, but actually user has entered a change
      // for COR, initial value is 0
      if( (mrmlCOR[0] == 0) && (this->COR->GetWidget(0)->GetValueAsDouble() == 0))
        {
        xValChanged = true;
        }
      }
    
    }
  else if(widgetIndex == 1)
    { 
    // y scaling entered    
    if (strcmpi(this->COR->GetWidget(1)->GetValue(), "")!=0)
      {
      if(!this->DoubleEqual(mrmlCOR[1],this->COR->GetWidget(1)->GetValueAsDouble()))
        {
        yValChanged = true;
        }
      // counter the case when the user enters the same value in entry, which is equal to the initial value set in MRML node
      // in this case above code, won't detect change, but actually user has entered a change
      // for COR, initial value is 0
      if( (mrmlCOR[1] == 0) && (this->COR->GetWidget(1)->GetValueAsDouble() == 0))
        {
        yValChanged = true;
        }
      }
    }
  
  if (xValChanged && yValChanged)
    {
    double xCOR = this->COR->GetWidget(0)->GetValueAsDouble();
    double yCOR = this->COR->GetWidget(1)->GetValueAsDouble();
    // set user scaling in mrml node
    mrmlNode->SetCenterOfRotation(xCOR, yCOR, mrmlCOR[2]);    
    this->CORSpecified = true;
    xValChanged = false;
    yValChanged = false;
    if (this->GetGUI()->GetMode() == vtkPerkStationModuleGUI::Clinical)
      {
      this->GetGUI()->GetWizardWidget()->SetErrorText( "");
      this->GetGUI()->GetWizardWidget()->Update();      
      }
    }
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ProcessKeyboardEvents(vtkObject *caller, unsigned long event, void *callData)
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
  
  if ( event == vtkCommand::KeyPressEvent )
    {
    char  *key = style->GetKeySym();
    if ( ! strcmp( key, "a" ) ) this->Translation[ 1 ] += stepSize;
    if ( ! strcmp( key, "z" ) ) this->Translation[ 1 ] -= stepSize;
    if ( ! strcmp( key, "q" ) ) this->Translation[ 0 ] -= stepSize;
    if ( ! strcmp( key, "w" ) ) this->Translation[ 0 ] += stepSize;
    if ( ! strcmp( key, "g" ) ) this->Rotation += ( stepSize / 3.0 );
    if ( ! strcmp( key, "h" ) ) this->Rotation -= ( stepSize / 3.0 );
    }
  
  this->GetGUI()->GetSecondaryMonitor()->SetTranslation(
        this->Translation[ 0 ], this->Translation[ 1 ] );
  this->GetGUI()->GetSecondaryMonitor()->SetRotation( this->Rotation );
  
  this->ProcessingCallback = false;
}


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::RecordClick(int xyPoint[2], double rasPoint[3])
{
  switch (this->CurrentSubState)
    {
    case 2: // click happened while in Translate state, depending on click number, it is either image fiducial click or physical fiducial click
        if (this->ClickNumber ==1)
          {
          this->TransImgFid->GetWidget(0)->SetValueAsDouble(rasPoint[0]);
          this->TransImgFid->GetWidget(1)->SetValueAsDouble(rasPoint[1]);
          }
        else if (this->ClickNumber == 2)
          {
          this->TransPhyFid->GetWidget(0)->SetValueAsDouble(rasPoint[0]);
          this->TransPhyFid->GetWidget(1)->SetValueAsDouble(rasPoint[1]);
          // record value in the MRML node
          double translation[3];
          translation[0] = (this->TransPhyFid->GetWidget(0)->GetValueAsDouble() - this->TransImgFid->GetWidget(0)->GetValueAsDouble());
          translation[1] = (this->TransPhyFid->GetWidget(1)->GetValueAsDouble() - this->TransImgFid->GetWidget(1)->GetValueAsDouble());
          translation[2] = 0;
          this->GetGUI()->GetMRMLNode()->SetActualTranslation(translation);
          this->ClickNumber = 0;
          }

        break;
    case 3:
        // click happened while in Rotate state, depending on click number, it is either image fiducial click or physical fiducial click
        if (this->CORSpecified)
        {
        if (this->ClickNumber ==1)
          {
          this->RotImgFid->GetWidget(0)->SetValueAsDouble(rasPoint[0]);
          this->RotImgFid->GetWidget(1)->SetValueAsDouble(rasPoint[1]);
          }
        else if (this->ClickNumber == 2)
          {
          this->RotPhyFid->GetWidget(0)->SetValueAsDouble(rasPoint[0]);
          this->RotPhyFid->GetWidget(1)->SetValueAsDouble(rasPoint[1]);       
          
          // calculate rotation
          double actualRotation = 0;
          this->CalculateImageRotation(actualRotation);
          // and set the value in the MRML node
          this->GetGUI()->GetMRMLNode()->SetActualRotation(actualRotation);
          this->ClickNumber = 0;
          }
        }
        else
        {
        // in this case, take the first click as the click for COR
        if (this->ClickNumber ==1)
          {
          this->COR->GetWidget(0)->SetValueAsDouble(rasPoint[0]);
          this->COR->GetWidget(1)->SetValueAsDouble(rasPoint[1]);
          // set cneter of rotation in mrml node
          this->GetGUI()->GetMRMLNode()->SetCenterOfRotation(rasPoint[0], rasPoint[1], rasPoint[2]);    
          this->CORSpecified = true;
          }
        // reset the click number to start from 0 again
        this->ClickNumber = 0;
        }
        break;
    default:
        if (this->GetGUI()->GetMode() == vtkPerkStationModuleGUI::Clinical && !this->CORSpecified)
          {
          this->COR->GetWidget(0)->SetValueAsDouble(rasPoint[0]);
          this->COR->GetWidget(1)->SetValueAsDouble(rasPoint[1]);
          // set cneter of rotation in mrml node
          this->GetGUI()->GetMRMLNode()->SetCenterOfRotation(rasPoint[0], rasPoint[1], rasPoint[2]);    
          this->CORSpecified = true;
          }
        this->ClickNumber = 0;
        break;

    }
}
//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::CalculateImageRotation(double & rotationAngle)
{
  // calculate rotation between vector formed by image click-COR and vector of fiducial click-COR
  
  double imgFidVecAngle = 0;
  double phyFidVecAngle = 0;
  

  double imgFidPoint[2];
  double phyFidPoint[2];

  double cor[3]; // center of rotation
  this->GetGUI()->GetMRMLNode()->GetCenterOfRotation(cor[0], cor[1], cor[2]);
  imgFidPoint[0] = this->RotImgFid->GetWidget(0)->GetValueAsDouble();
  imgFidPoint[1] = this->RotImgFid->GetWidget(1)->GetValueAsDouble();

  phyFidPoint[0] = this->RotPhyFid->GetWidget(0)->GetValueAsDouble();
  phyFidPoint[1] = this->RotPhyFid->GetWidget(1)->GetValueAsDouble();

  // first calculate the angle which the image fiducial vector makes
  imgFidVecAngle = (180/vtkMath::Pi()) * atan(double((imgFidPoint[1] - cor[1])/(imgFidPoint[0] - cor[0])));
  // now calculate the angle which the physical fiducial vector makes
  phyFidVecAngle = (180/vtkMath::Pi()) * atan(double((phyFidPoint[1] - cor[1])/(phyFidPoint[0] - cor[0])));
  
  rotationAngle = imgFidVecAngle - phyFidVecAngle; // thats the additional rotation needed 

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
void vtkPerkStationCalibrateStep::SaveCalibration(ostream& of)
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

      double monPhySize[2];
      this->GetGUI()->GetSecondaryMonitor()->GetPhysicalSize(monPhySize[0],monPhySize[1]);
      // monitor physical size
      of << " PhysicalSizeMMs=\"";
      for(int i = 0; i < 2; i++)
        of << monPhySize[i] << " ";
      of << "\" \n";

      double monPixResolution[2];
      this->GetGUI()->GetSecondaryMonitor()->GetPixelResolution(monPixResolution[0],monPixResolution[1]);
      // monitor pixel resolution
      of << " PixelResolution=\"";
      for(int i = 0; i < 2; i++)
        of << monPixResolution[i] << " ";
      of << "\" \n";

      of << " CenterOfRotation=\"";
      for(int i = 0; i < 3; i++)
        of << mrmlNode->GetCenterOfRotation()[i] << " ";
      of << "\" \n";

      of << " Rotation=\"";
      mrmlNode->SetClinicalModeRotation( this->Rotation );
      of << this->Rotation << " ";
      of << "\" \n";

      of << " Translation=\"";
      mrmlNode->SetClinicalModeTranslation( this->Translation );
      for(int i = 0; i < 2; i++)
        {
        of << this->Translation[ i ] << " ";
        }    
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

  vtkMRMLScalarVolumeNode *inVolume = mrmlNode->GetPlanningVolumeNode();
  if (!inVolume)
    {
    // TO DO: what to do on failure
    return;
    }

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

  // reset member variables to defaults
  this->ImageFlipDone = false;
  this->ImageScalingDone = false;
  this->ImageTranslationDone = false;
  this->ImageRotationDone = false;
  this->CORSpecified = false;
  this->ClickNumber = 0;
  // reset individual gui components to default values
  this->ResetControls();

  // reset gui state; enable disable
  this->CurrentSubState = 0;
  this->EnableDisableControls();
}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ResetControls()
{
  this->TrainingModeControlsPopulated = false;
  this->ClinicalModeControlsPopulated = false;

  // do what PopulateControls() does, in addition, reset the values that have been input by user
  this->PopulateControls();

   switch (this->GetGUI()->GetMode())
    {
    case vtkPerkStationModuleGUI::Training:
        if (this->ImgScaling)
          {
          // image scaling
          this->ImgScaling->GetWidget(0)->SetValue("");
          this->ImgScaling->GetWidget(1)->SetValue("");
          }

        if(this->TransImgFid)
          {
          // image translation, clear the previos entries
          this->TransImgFid->GetWidget(0)->SetValue("");
          this->TransImgFid->GetWidget(1)->SetValue("");
          }

        if (this->TransPhyFid)
          {
          this->TransPhyFid->GetWidget(0)->SetValue("");
          this->TransPhyFid->GetWidget(1)->SetValue("");
          }


        if (this->RotImgFid)
          {
          // image rotation
          this->RotImgFid->GetWidget(0)->SetValue("");
          this->RotImgFid->GetWidget(1)->SetValue("");
          }

        if (this->RotPhyFid)
          {
          this->RotPhyFid->GetWidget(0)->SetValue("");
          this->RotPhyFid->GetWidget(1)->SetValue("");
          }

        if (this->RotationAngle)
          {
          this->RotationAngle->GetWidget()->SetValue("");
          }
        if (this->COR)
          {
          this->COR->GetWidget(0)->SetValue("");
          this->COR->GetWidget(1)->SetValue("");
          }
        break;

    case vtkPerkStationModuleGUI::Clinical:
        if (this->COR)
          {
          this->COR->GetWidget(0)->SetValue("");
          this->COR->GetWidget(1)->SetValue("");
          }
      
        break;
    }
  
  

}

//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ClearLoadResetControls()
{
   
    if (this->LoadResetFrame)
    {
    this->Script("pack forget %s", 
                    this->LoadResetFrame->GetWidgetName());
    }
    if (this->LoadCalibrationFileButton)
    {
    this->Script("pack forget %s", 
                    this->LoadCalibrationFileButton->GetWidgetName());
    }
    if (this->SaveCalibrationFileButton)
    {
    this->Script("pack forget %s", 
                    this->SaveCalibrationFileButton->GetWidgetName());
    }
    
    if (this->ResetCalibrationButton)
    {
    this->Script("pack forget %s", 
                    this->ResetCalibrationButton->GetWidgetName());
    }
    
}
//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ClearSaveControls()
{
   
    if (this->SaveFrame)
    {
    this->Script("pack forget %s", 
                    this->SaveFrame->GetWidgetName());
    }
    /*
    if (this->SaveCalibrationFileButton)
    {
    this->Script("pack forget %s", 
                    this->SaveCalibrationFileButton->GetWidgetName());
    }
    */

   
}
//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ClearFlipComponents()
{
  
    if (this->FlipFrame)
    {
    this->Script("pack forget %s", 
                    this->FlipFrame->GetWidgetName());
    }
    if (this->VerticalFlipCheckButton)
    {
    this->Script("pack forget %s", 
                    this->VerticalFlipCheckButton->GetWidgetName());
    }
    if (this->HorizontalFlipCheckButton)
    {
    this->Script("pack forget %s", 
                    this->HorizontalFlipCheckButton->GetWidgetName());
    }

}
//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ClearScaleComponents()
{
    if (this->ScaleFrame)
    {
    this->Script("pack forget %s", 
                    this->ScaleFrame->GetWidgetName());
    }
    if (this->MonPhySizeFrame)
    {
    this->Script("pack forget %s", 
                    this->MonPhySizeFrame->GetWidgetName());
    }
    if (this->MonPhySizeLabel)
    {
    this->Script("pack forget %s", 
                    this->MonPhySizeLabel->GetWidgetName());
    }
    if (this->MonPhySize) 
    {
    this->Script("pack forget %s", 
                    this->MonPhySize->GetWidgetName());
    }
    if (this->MonPixResFrame)
    {
    this->Script("pack forget %s", 
                    this->MonPixResFrame->GetWidgetName());
    }
    if (this->MonPixResLabel)
    {
    this->Script("pack forget %s", 
                    this->MonPixResLabel->GetWidgetName());
    }
    if (this->MonPixRes)
    {
    this->Script("pack forget %s", 
                    this->MonPixRes->GetWidgetName());
    }
    if (this->UpdateAutoScale)
    {
    this->Script("pack forget %s", 
                    this->UpdateAutoScale->GetWidgetName());
    }
    if (this->ImgPixSizeFrame)
    {
    this->Script("pack forget %s", 
                this->ImgPixSizeFrame->GetWidgetName());
    }

    if (this->ImgPixSizeLabel)
    {
    this->Script("pack forget %s", 
                this->ImgPixSizeLabel->GetWidgetName());
    }

    if (this->ImgSpacing)
    {
    this->Script("pack forget %s", 
                this->ImgSpacing->GetWidgetName());
    }

    if (this->MonPixSizeFrame)
    {
    this->Script("pack forget %s", 
                this->MonPixSizeFrame->GetWidgetName());
    } 
    if (this->MonPixSizeLabel)
    {
    this->Script("pack forget %s", 
                this->MonPixSizeLabel->GetWidgetName());
    }

    if (this->MonSpacing)
    {
    this->Script("pack forget %s", 
                this->MonSpacing->GetWidgetName());
    }
    if (this->ImgScaleFrame)
    {
    this->Script("pack forget %s", 
                this->ImgScaleFrame->GetWidgetName());
    }

    if (this->ImgScaleLabel)
    {
    this->Script("pack forget %s", 
                this->ImgScaleLabel->GetWidgetName());
    }

    if (this->ImgScaling)
    {
    this->Script("pack forget %s", 
                this->ImgScaling->GetWidgetName());
    }

}
//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ClearTranslateComponents()
{
    if (this->TranslateFrame)
    {
    this->Script("pack forget %s", 
                this->TranslateFrame->GetWidgetName());
    }
    if (this->TransMessage)
    {
    this->Script("pack forget %s", 
                this->TransMessage->GetWidgetName());
    }
    if (this->TransImgFidFrame)
    {
    this->Script("pack forget %s", 
                this->TransImgFidFrame->GetWidgetName());
    }
    if (this->TransImgFidLabel)
    {
    this->Script( "pack forget %s", 
                this->TransImgFidLabel->GetWidgetName());
    }
    if (this->TransImgFid)
    {
    this->Script("pack forget %s", 
                this->TransImgFid->GetWidgetName());
    }
    if (this->TransPhyFidFrame)
    {
    this->Script("pack forget %s", 
                this->TransPhyFidFrame->GetWidgetName());
    }
    if (this->TransPhyFidLabel)
    {
    this->Script( "pack forget %s", 
                this->TransPhyFidLabel->GetWidgetName());
    }
    if (this->TransPhyFid)
    {
    this->Script("pack forget %s", 
                this->TransPhyFid->GetWidgetName());
    }
    if (this->TransEntryFrame)
    {
    this->Script("pack forget %s", 
                this->TransEntryFrame->GetWidgetName());
    }
    if (this->TransEntryLabel)
    {
    this->Script( "pack forget %s", 
                this->TransEntryLabel->GetWidgetName());
    }
    
}
//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ClearRotateComponents()
{
  
    if (this->RotateFrame)
    {
    this->Script("pack forget %s", 
                this->RotateFrame->GetWidgetName());
    }
    if (this->CORFrame)
    {
    this->Script("pack forget %s", 
                this->CORFrame->GetWidgetName());
    }
    if (this->CORLabel)
    {
    this->Script("pack forget %s", 
                this->CORLabel->GetWidgetName());
    }
    if (this->COR)
    {
    this->Script("pack forget %s", 
                this->COR->GetWidgetName());
    }
    if (this->RotMessage)
    {
    this->Script("pack forget %s", 
                this->RotMessage->GetWidgetName());
    }
    if (this->RotImgFidFrame)
    {
    this->Script("pack forget %s", 
                    this->RotImgFidFrame->GetWidgetName()); 
    }
    if (this->RotImgFidLabel)
    {
    this->Script( "pack forget %s", 
                    this->RotImgFidLabel->GetWidgetName());
    }
    if (this->RotImgFid)
    {
    this->Script("pack forget %s", 
                    this->RotImgFid->GetWidgetName());
    }
    if (this->RotPhyFidFrame)
    {
    this->Script("pack forget %s", 
                    this->RotPhyFidFrame->GetWidgetName());
    }
    if (this->RotPhyFidLabel)
    {
    this->Script( "pack forget %s", 
                    this->RotPhyFidLabel->GetWidgetName());
    }
    if (this->RotPhyFid)
    {
    this->Script("pack forget %s", 
                    this->RotPhyFid->GetWidgetName());
    }
    if (this->RotationAngle)
    {
    this->Script("pack forget %s", 
                    this->RotationAngle->GetWidgetName());
    }

}

//----------------------------------------------------------------------------
/*void vtkPerkStationCalibrateStep::DisplaySelectedNodeSpatialPriorsCallback()
{
  // Update the UI with the proper value, if there is a selection

  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  vtkEMSegmentAnatomicalStructureStep *anat_step = 
    this->GetGUI()->GetAnatomicalStructureStep();
  vtkKWTree *tree = anat_step->GetAnatomicalStructureTree()->GetWidget();
  vtksys_stl::string sel_node;
  vtkIdType sel_vol_id = 0;
  int has_valid_selection = tree->HasSelection();
  if (has_valid_selection)
    {
    sel_node = tree->GetSelection();
    sel_vol_id = tree->GetNodeUserDataAsInt(sel_node.c_str());
    has_valid_selection = mrmlManager->GetTreeNodeIsLeaf(sel_vol_id);
    }
  char buffer[256];

  // Update the spatial prior volume selector

  if (this->SpatialPriorsVolumeMenuButton)
    {
    vtkKWMenu *menu = 
      this->SpatialPriorsVolumeMenuButton->GetWidget()->GetMenu();
    menu->DeleteAllItems();
    if (has_valid_selection)
      {
      this->SpatialPriorsVolumeMenuButton->SetEnabled(tree->GetEnabled());
      sprintf(buffer, "SpatialPriorsVolumeCallback %d", 
              static_cast<int>(sel_vol_id));
      this->PopulateMenuWithLoadedVolumes(menu, this, buffer);
      vtkIdType vol_id = mrmlManager->GetTreeNodeSpatialPriorVolumeID(sel_vol_id);
      if(!this->SetMenuButtonSelectedItem(menu, vol_id))
        {
        this->SpatialPriorsVolumeMenuButton->GetWidget()->SetValue("");
        }
      }
    else
      {
      this->SpatialPriorsVolumeMenuButton->GetWidget()->SetValue("");
      this->SpatialPriorsVolumeMenuButton->SetEnabled(0);
      }
    }
}
*/
//----------------------------------------------------------------------------
/*void vtkPerkStationCalibrateStep::SpatialPriorsVolumeCallback(
  vtkIdType sel_vol_id, vtkIdType vol_id)
{
  // The spatial prior volume has changed because of user interaction

  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  mrmlManager->SetTreeNodeSpatialPriorVolumeID(sel_vol_id, vol_id);
}
*/
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::AddGUIObservers()
{
  this->RemoveGUIObservers();
  // add event handling callbacks for each control
  
  if ( this->HardwareMenu )
    {
    this->HardwareMenu->GetWidget()->GetMenu()->AddObserver(
      vtkKWMenu::MenuItemInvokedEvent, ( vtkCommand* )( this->WizardGUICallbackCommand ) );
    
    }
  
   // load reset components
  if (this->LoadCalibrationFileButton)
    {
    this->LoadCalibrationFileButton->GetLoadSaveDialog()->AddObserver(
      vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  if (this->ResetCalibrationButton)
    {
    this->ResetCalibrationButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  
  // flip controls
  if (this->VerticalFlipCheckButton)
    {
    this->VerticalFlipCheckButton->GetWidget()->AddObserver(
      vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }
  if (this->HorizontalFlipCheckButton)
    {
    this->HorizontalFlipCheckButton->GetWidget()->AddObserver(
      vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }
  
  // change in monitor physical size/ res, therefore update button
  if (this->UpdateAutoScale)
    {
    this->UpdateAutoScale->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand);        
    }

  // rotation controls
  if (this->COR)
    {
    this->COR->GetWidget(0)->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    this->COR->GetWidget(1)->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
    }
  
  // save controls
  if (this->SaveCalibrationFileButton)
    {
    this->SaveCalibrationFileButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }


  this->ObserverCount++;
}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::RemoveGUIObservers()
{
  // add event handling callbacks for each control
  switch (this->GetGUI()->GetMode())
    {
    case vtkPerkStationModuleGUI::Training:
      {
      
      // flip controls
      if (this->VerticalFlipCheckButton)
        {
        this->VerticalFlipCheckButton->GetWidget()->RemoveObservers(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
        }
      if (this->HorizontalFlipCheckButton)
        {
        this->HorizontalFlipCheckButton->GetWidget()->RemoveObservers(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
        }

      // scale controls     
      if(this->ImgScaling)
        {
        this->ImgScaling->GetWidget(0)->RemoveObservers(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
        this->ImgScaling->GetWidget(1)->RemoveObservers(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
        }

      // rotation controls
      if (this->COR)
        {
        this->COR->GetWidget(0)->RemoveObservers(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
        this->COR->GetWidget(1)->RemoveObservers(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
        }
   
      // rotation angle
      if(this->RotationAngle)
        {
        this->RotationAngle->RemoveObservers(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
        }

      }

      break;

    case vtkPerkStationModuleGUI::Clinical:
      {
      // load reset components
      if (this->LoadCalibrationFileButton)
        {
        this->LoadCalibrationFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
        }
      if (this->ResetCalibrationButton)
        {
        this->ResetCalibrationButton->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
        }
      // flip controls
      if (this->VerticalFlipCheckButton)
        {
        this->VerticalFlipCheckButton->GetWidget()->RemoveObservers(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
        }
      if (this->HorizontalFlipCheckButton)
        {
        this->HorizontalFlipCheckButton->GetWidget()->RemoveObservers(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
        }

       // change in monitor physical size/ res, therefore update button
      if (this->UpdateAutoScale)
        {
        this->UpdateAutoScale->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand);        
        }

       // rotation controls
      if (this->COR)
        {
        this->COR->GetWidget(0)->RemoveObservers(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
        this->COR->GetWidget(1)->RemoveObservers(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
        }

      // save controls    
      if (this->SaveCalibrationFileButton)
        {
        this->SaveCalibrationFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
        }
      
      }
      
      break;


    }

  this->ObserverCount--;
}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData )
{
    vtkPerkStationCalibrateStep *self = reinterpret_cast<vtkPerkStationCalibrateStep *>(clientData);
    if (self) { self->ProcessGUIEvents(caller, event, callData); }
}

//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData)
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  
  
  int hardware = -1;
  
  if ( this->HardwareMenu->GetWidget()->GetMenu()
         == vtkKWMenu::SafeDownCast( caller )
       && event == vtkKWMenu::MenuItemInvokedEvent )
    {
    if ( this->HardwareMenu->GetWidget()->GetMenu()->
         GetIndexOfSelectedItem() == 0 )
      {
      hardware = 0;
      }
    else if ( this->HardwareMenu->GetWidget()->GetMenu()->
              GetIndexOfSelectedItem() == 1 )
      {
      hardware = 1;
      }
    }
  
  
  if(!mrmlNode)
      return;

  if(!mrmlNode->GetPlanningVolumeNode() || strcmp(mrmlNode->GetVolumeInUse(), "Planning")!=0)
      return;

  
  if (this->ProcessingCallback)
    {
    return;
    }

  this->ProcessingCallback = true;
  
  
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
    this->SaveCalibrationFileButton->SetText ("Save calibration");
    }


    // reset calib button
  
  if ( this->ResetCalibrationButton
       && this->ResetCalibrationButton == vtkKWPushButton::SafeDownCast(caller)
       && ( event == vtkKWPushButton::InvokedEvent ) )
    {
    this->Reset();
    }
  
  
    // update auto scale button
  
  if ( this->UpdateAutoScale
       && this->UpdateAutoScale == vtkKWPushButton::SafeDownCast(caller)
       && ( event == vtkKWPushButton::InvokedEvent ) )
    {
    this->UpdateAutoScaleCallback();
    }
  
  
    // check buttons
  
  if ( this->VerticalFlipCheckButton
       && this->VerticalFlipCheckButton->GetWidget()
          == vtkKWCheckButton::SafeDownCast( caller )
       && (event == vtkKWCheckButton::SelectedStateChangedEvent ) )
    {
      // vertical flip button selected state changed
    this->VerticalFlipCallback(
      bool( this->VerticalFlipCheckButton->GetWidget()->GetSelectedState() ) );
    }
  
  
  if ( this->HorizontalFlipCheckButton
       && this->HorizontalFlipCheckButton->GetWidget()
          == vtkKWCheckButton::SafeDownCast( caller )
       && ( event == vtkKWCheckButton::SelectedStateChangedEvent ) )
    {
      // horizontal flip button selected state changed
    this->HorizontalFlipCallback(
      bool( this->HorizontalFlipCheckButton->GetWidget()->GetSelectedState() ));
    }

  
  // COR x
  if (this->COR && this->COR->GetWidget(0)== vtkKWEntry::SafeDownCast(caller) && (event == vtkKWEntry::EntryValueChangedEvent))
    {
    // x-entry of COR
    this->COREntryCallback(0);
    }
  // COR y
  if (this->COR && this->COR->GetWidget(1)== vtkKWEntry::SafeDownCast(caller) && (event == vtkKWEntry::EntryValueChangedEvent))
    {
    // y-entry of COR
    this->COREntryCallback(1);
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


