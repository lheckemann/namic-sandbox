#include "vtkTRProstateBiopsyCalibrationStep.h"

#include "vtkTRProstateBiopsyGUI.h"
#include "vtkTRProstateBiopsyLogic.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"

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
  this->SetName("1/3. Calibration");
  this->SetDescription("Perform robot calibration.");

  this->GUICallbackCommand->SetCallback(
    &vtkTRProstateBiopsyCalibrationStep::GUICallback);

  this->MRMLCallbackCommand->SetCallback(
    &vtkTRProstateBiopsyCalibrationStep::MRMLCallback);

  this->ButtonFrame  = NULL;
  this->LoadCalibrationImageButton = NULL;
  this->ResetCalibrationButton = NULL; 

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
}

//----------------------------------------------------------------------------
vtkTRProstateBiopsyCalibrationStep::~vtkTRProstateBiopsyCalibrationStep()
{
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ShowUserInterface()
{
  slicerCerr("vtkTRProstateBiopsyCalibrationStep::ShowUserInterface");

  this->Superclass::ShowUserInterface();

  vtkKWWizardWidget *wizardWidget = this->GetGUI()->GetWizardWidget();
  vtkKWWidget *parent = wizardWidget->GetClientArea();

  if (!this->ButtonFrame)
    {
    this->ButtonFrame = vtkKWFrame::New();
    this->ButtonFrame->SetParent(parent);
    this->ButtonFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->ButtonFrame->GetWidgetName());

  if (!this->LoadCalibrationImageButton)
    {
    this->LoadCalibrationImageButton = vtkKWLoadSaveButtonWithLabel::New();
    this->LoadCalibrationImageButton->SetParent(this->ButtonFrame);
    this->LoadCalibrationImageButton->Create();
    //this->LoadCalibrationImageButton->GetWidget()->SetWidth(240);
    this->LoadCalibrationImageButton->GetWidget()->SetText(
      "Browse For DICOM Series");
    this->LoadCalibrationImageButton->GetWidget()->GetLoadSaveDialog()
      ->SetFileTypes("{ {DICOM Files} {*} }");
    this->LoadCalibrationImageButton->GetWidget()->GetLoadSaveDialog()
      ->RetrieveLastPathFromRegistry("OpenPath");
    this->LoadCalibrationImageButton->GetWidget()->AddObserver(
      vtkKWPushButton::InvokedEvent, this->GUICallbackCommand);
    }
  this->Script("pack %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->LoadCalibrationImageButton->GetWidgetName());

  if (!this->ResetCalibrationButton)
    {
    this->ResetCalibrationButton = vtkKWPushButton::New();
    this->ResetCalibrationButton->SetParent(this->ButtonFrame);
    this->ResetCalibrationButton->Create();
    this->ResetCalibrationButton->SetText("Reset Calibration");
    this->ResetCalibrationButton->SetBalloonHelpString("Reset the calibration");
    this->ResetCalibrationButton->AddObserver(
      vtkKWPushButton::InvokedEvent, this->GUICallbackCommand);
    }

  this->Script("pack %s -side top -anchor w -padx 2 -pady 2", 
               this->ResetCalibrationButton->GetWidgetName());

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
    this->FiducialWidthSpinBox->GetWidget()->AddObserver(
      vtkKWSpinBox::SpinBoxValueChangedEvent, this->GUICallbackCommand);
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
    this->FiducialHeightSpinBox->GetWidget()->AddObserver(
      vtkKWSpinBox::SpinBoxValueChangedEvent, this->GUICallbackCommand);
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
    this->FiducialDepthSpinBox->GetWidget()->AddObserver(
      vtkKWSpinBox::SpinBoxValueChangedEvent, this->GUICallbackCommand);
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
      this->FiducialThresholdScale[i]->AddObserver(
        vtkKWScale::ScaleValueChangingEvent, this->GUICallbackCommand);
      this->FiducialThresholdScale[i]->AddObserver(
        vtkKWScale::ScaleValueChangedEvent, this->GUICallbackCommand);
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
    this->RadiusSpinBox->GetWidget()->AddObserver(
      vtkKWSpinBox::SpinBoxValueChangedEvent, this->GUICallbackCommand);
    }

  if (!this->RadiusCheckButton)
    {
    this->RadiusCheckButton = vtkKWCheckButton::New();
    this->RadiusCheckButton->SetParent(
                    this->RadiusInitialAngleFrame->GetFrame());
    this->RadiusCheckButton->Create();
    this->RadiusCheckButton->AddObserver(
      vtkKWCheckButton::SelectedStateChangedEvent, this->GUICallbackCommand);
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
    this->InitialAngleSpinBox->GetWidget()->AddObserver(
      vtkKWSpinBox::SpinBoxValueChangedEvent, this->GUICallbackCommand);
    }

  this->Script("pack %s %s %s -side left -anchor w -padx 2 -pady 2", 
               this->RadiusSpinBox->GetWidgetName(),
               this->RadiusCheckButton->GetWidgetName(),
               this->InitialAngleSpinBox->GetWidgetName() );

  // Activate additional details for entering this step
  this->Enter();
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::HideUserInterface()
{
  this->Exit();

  this->Superclass::HideUserInterface();
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::Enter()
{
  slicerCerr("vtkTRProstateBiopsyCalibrationStep::Enter");

  // change the view orientation to the calibration image orientation
  vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>(
    this->GetGUI()->GetApplication());
  vtkTRProstateBiopsyLogic *logic = this->GetGUI()->GetLogic();
  logic->SetSliceViewFromVolume(app, logic->GetCalibrationVolumeNode());

/*
  if (this->GetGUI()->GetLogic()->GetCalibrationSliceNodeXML())
    {
    static const char *panes[3] = { "Red", "Yellow", "Green" };

    vtkSlicerApplicationLogic *appLogic =
      this->GetGUI()->GetApplicationGUI()->GetApplicationLogic();

    const char *xml = this->GetGUI()->GetLogic()->GetCalibrationSliceNodeXML();

    slicerCerr(xml << "\n");

    for (int i = 0; i < 3; i++)
      {
      slicerCerr(i);

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
  slicerCerr("vtkTRProstateBiopsyCalibrationStep::Exit");

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
void vtkTRProstateBiopsyCalibrationStep::GUICallback( vtkObject *caller,
                         unsigned long eid, void *clientData, void *callData )
{

  vtkTRProstateBiopsyCalibrationStep *self =
    reinterpret_cast<vtkTRProstateBiopsyCalibrationStep *>(clientData);
  
  if (self->GetInGUICallbackFlag())
    {
    // check for infinite recursion here
    }

  vtkDebugWithObjectMacro(self,
                          "In vtkTRProstateCalibrationBiopsyStep GUICallback");
  
  self->SetInGUICallbackFlag(1);
  self->ProcessGUIEvents(caller, eid, callData);
  self->SetInGUICallbackFlag(0);
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ProcessGUIEvents(vtkObject *caller,
        unsigned long event, void *callData)
{
  slicerCerr("vtkTRProstateBiopsyCalibrationStep::ProcessGUIEvents");
  if (caller)
    {
    slicerCerr(caller->GetClassName() << " " << vtkCommand::GetStringFromEventId(event));
    }

  // Most of these events have the same numerical value, but
  // all are listed to be sure that all are caught
  if (event != vtkKWFileBrowserDialog::FileNameChangedEvent &&
      event != vtkKWPushButton::InvokedEvent &&
      event != vtkKWCheckButton::SelectedStateChangedEvent &&
      event != vtkKWSpinBox::SpinBoxValueChangedEvent &&
      event != vtkKWScale::ScaleValueChangingEvent &&
      event != vtkKWScale::ScaleValueChangedEvent &&
      event != vtkKWScale::ScaleValueStartChangingEvent)
    {
    return;
    }

  if (event == vtkKWFileBrowserDialog::FileNameChangedEvent)
    {
    // File browser

    if (vtkKWLoadSaveDialog::SafeDownCast(caller) ==
        this->LoadCalibrationImageButton->GetWidget()->GetLoadSaveDialog())
      {
      slicerCerr("vtkKWLoadSaveDialog");

      const char *fileName =
        this->LoadCalibrationImageButton->GetWidget()->GetFileName();      
  
      if ( fileName )
        {
        slicerCerr(fileName);
        
        // don't center volume, or treat as label map
        //int centered = 0;
        //int labelMap = 0;

        std::string fileString(fileName);
        for (unsigned int i = 0; i < fileString.length(); i++)
          {
          if (fileString[i] == '\\')
            {
            fileString[i] = '/';
            }
          }
        this->LoadCalibrationImageButton->GetWidget()->GetLoadSaveDialog()
          ->SaveLastPathToRegistry("TRProstateOpenPath");

        vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>(
          this->GetGUI()->GetApplication());

        vtkMRMLScalarVolumeNode *volumeNode =
          this->GetGUI()->GetLogic()->AddCalibrationVolume(app,
                                                           fileString.c_str());
        
        if (volumeNode)
          {
          this->GetGUI()->GetApplicationLogic()->GetSelectionNode()
            ->SetActiveVolumeID( volumeNode->GetID() );
          this->GetGUI()->GetApplicationLogic()->PropagateVolumeSelection();
          }
        else 
          {
          vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
          dialog->SetParent ( this->ButtonFrame );
          dialog->SetStyleToMessage();
          std::string msg = std::string("Unable to read volume file ")
            + std::string(fileName);
          dialog->SetText(msg.c_str());
          dialog->Create ( );
          dialog->Invoke();
          dialog->Delete();
          }

        }
      }
    }

  if (event == vtkKWPushButton::InvokedEvent)
    {
    // FileName & Reset

    if (vtkKWLoadSaveButton::SafeDownCast(caller) ==
        this->LoadCalibrationImageButton->GetWidget())
      {
      slicerCerr("vtkKWLoadSaveButtonWithLabel");

      vtkKWLoadSaveDialog *dialog = 
        this->LoadCalibrationImageButton->GetWidget()->GetLoadSaveDialog();
      
      // the dialog is created when the button is pressed, so here
      // is where we add the observer to the dialog
      if (dialog && 
          !dialog->HasObserver(vtkKWFileBrowserDialog::FileNameChangedEvent,
                               this->GUICallbackCommand))
        {
        dialog->AddObserver(vtkKWFileBrowserDialog::FileNameChangedEvent,
                            this->GUICallbackCommand);
        }
      }

    else if (vtkKWPushButton::SafeDownCast(caller) ==
             this->ResetCalibrationButton)
      {
      slicerCerr("ResetCalibrationButton");
      }
    }
  
  // Width, Height, Depth
  if (event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    if (vtkKWSpinBox::SafeDownCast(caller) ==
        this->FiducialWidthSpinBox->GetWidget())
      {
      slicerCerr("FiducialWidthSpinBox");
      }
    else if (vtkKWSpinBox::SafeDownCast(caller) ==
        this->FiducialHeightSpinBox->GetWidget())
      {
      slicerCerr("FiducialHeightSpinBox");
      }
    else if (vtkKWSpinBox::SafeDownCast(caller) ==
        this->FiducialDepthSpinBox->GetWidget())
      {
      slicerCerr("FiducialDepthSpinBox");
      }
    }

  // Thresholds
  if (event == vtkKWScale::ScaleValueChangingEvent)
    {
    for (int i = 0; i < 4; i++)
      {
      if (vtkKWScaleWithEntry::SafeDownCast(caller) ==
          this->FiducialThresholdScale[i])
        {
        slicerCerr("FiducialThresholdScale");
        }
      }
    }

  // Radius and Initial Angle
  if (event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    if (vtkKWCheckButton::SafeDownCast(caller) ==
        this->RadiusCheckButton)
      {
      slicerCerr("RadiusCheckButton");
      }
    }

  if (event == vtkKWSpinBox::SpinBoxValueChangedEvent)
    {
    if (vtkKWSpinBox::SafeDownCast(caller) ==
        this->RadiusSpinBox->GetWidget())
      {
      slicerCerr("RadiusSpinBox");
      }
    else if (vtkKWSpinBox::SafeDownCast(caller) ==
        this->InitialAngleSpinBox->GetWidget())
      {
      slicerCerr("InitialAngleSpinBox");
      }
    }

}


