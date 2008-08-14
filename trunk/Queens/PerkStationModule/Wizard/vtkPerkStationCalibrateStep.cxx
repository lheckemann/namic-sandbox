#include "vtkPerkStationCalibrateStep.h"

#include "vtkPerkStationModuleGUI.h"
#include "vtkMRMLPerkStationModuleNode.h"
#include "vtkPerkStationSecondaryMonitor.h"

#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWCheckButton.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWEntrySet.h"
#include "vtkKWEntryWithLabel.h"
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

  // flip
  this->FlipFrame = NULL;
  this->VerticalFlipCheckButton = NULL;
  this->HorizontalFlipCheckButton = NULL;
  this->ImageFlipDone = false;

  // scale step
  this->ScaleFrame = NULL;
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
  this->TransImgFidFrame = NULL;
  this->TransImgFidLabel = NULL;
  this->TransImgFid = NULL;
  this->TransPhyFidFrame = NULL;
  this->TransPhyFidLabel = NULL;
  this->TransPhyFid = NULL;
  this->TransEntryFrame = NULL;
  this->TransEntryLabel = NULL;
  this->Translation = NULL;
  this->ImageTranslationDone = false;
  
  // rotation step
  this->RotateFrame = NULL;
  this->CORFrame = NULL;
  this->CORLabel = NULL;
  this->COR = NULL;
  this->RotImgFidFrame = NULL;
  this->RotImgFidLabel = NULL;
  this->RotImgFid = NULL;
  this->RotPhyFidFrame = NULL;
  this->RotPhyFidLabel = NULL;
  this->RotPhyFid = NULL;
  this->RotationAngle = NULL;
  this->ImageRotationDone = false;

  this->CurrentSubState = -1; // indicates invalid state, state will be valid only once volume is loaded

}

//----------------------------------------------------------------------------
vtkPerkStationCalibrateStep::~vtkPerkStationCalibrateStep()
{
  // flip step
  if (this->FlipFrame)
    {
    this->FlipFrame->Delete();
    this->FlipFrame = NULL;
    }
  if (this->VerticalFlipCheckButton)
    {
    this->VerticalFlipCheckButton->Delete();
    this->VerticalFlipCheckButton = NULL;
    }
  if (this->HorizontalFlipCheckButton)
    {
    this->HorizontalFlipCheckButton->Delete();
    this->HorizontalFlipCheckButton = NULL;
    }
  // scale step
  if (this->ScaleFrame)
    {
    this->ScaleFrame->Delete();
    this->ScaleFrame = NULL;
    }
  if (this->ImgPixSizeFrame)
    {
    this->ImgPixSizeFrame->Delete();
    this->ImgPixSizeFrame = NULL;
    }
  if (this->ImgPixSizeLabel)
    {
    this->ImgPixSizeLabel->Delete();
    this->ImgPixSizeLabel = NULL;
    }

  if (this->ImgSpacing)
    {
    this->ImgSpacing->DeleteAllWidgets();
    this->ImgSpacing = NULL;
    }

  if (this->ImgScaleFrame)
    {
    this->ImgScaleFrame->Delete();
    this->ImgScaleFrame = NULL;
    }
  if (this->ImgScaleLabel)
    {
    this->ImgScaleLabel->Delete();
    this->ImgScaleLabel = NULL;
    }
  if (this->ImgScaling)
    {
    this->ImgScaling->DeleteAllWidgets();
    this->ImgScaling = NULL;
    }
  if (this->MonPixSizeFrame)
    {
    this->MonPixSizeFrame->Delete();
    this->MonPixSizeFrame = NULL;
    }
  if (this->MonPixSizeLabel)
    {
    this->MonPixSizeLabel->Delete();
    this->MonPixSizeLabel = NULL;
    }
  if (this->MonSpacing)
    {
    this->MonSpacing->DeleteAllWidgets();
    this->MonSpacing = NULL;
    }
  
  this->ImageScalingDone = false;

  // translate step
  if (this->TranslateFrame)
    {
    this->TranslateFrame->Delete();
    this->TranslateFrame = NULL;
    }  
  if (this->TransImgFidFrame)
    {
    this->TransImgFidFrame->Delete();
    this->TransImgFidFrame = NULL;
    }
  if (this->TransImgFidLabel)
    {
    this->TransImgFidLabel->Delete();
    this->TransImgFidLabel = NULL;
    }
  if (this->TransImgFid)
    {
    this->TransImgFid->DeleteAllWidgets();
    this->TransImgFid = NULL;
    }
  if (this->TransPhyFidFrame)
    {
    this->TransPhyFidFrame->Delete();
    this->TransPhyFidFrame = NULL;
    }
  if (this->TransPhyFidLabel)
    {
    this->TransPhyFidLabel->Delete();
    this->TransPhyFidLabel = NULL;
    }  
  if (this->TransPhyFid)
    {
    this->TransPhyFid->DeleteAllWidgets();
    this->TransPhyFid = NULL;
    }
  if (this->TransEntryFrame)
    {
    this->TransEntryFrame->Delete();
    this->TransEntryFrame = NULL;
    }
  if (this->TransEntryLabel)
    {
    this->TransEntryLabel->Delete();
    this->TransEntryLabel = NULL;
    }  
  if (this->Translation)
    {
    this->Translation->DeleteAllWidgets();
    this->Translation = NULL;
    }

  this->ImageTranslationDone = false;


  // rotate step
  if (this->RotateFrame)
    {
    this->RotateFrame->Delete();
    this->RotateFrame = NULL;
    }
  if (this->CORFrame)
    {
    this->CORFrame->Delete();
    this->CORFrame = NULL;
    }
  if (this->CORLabel)
    {
    this->CORLabel->Delete();
    this->CORLabel = NULL;
    }    
  if (this->COR)
    {
    this->COR->DeleteAllWidgets();
    this->COR = NULL;
    }
  if (this->RotImgFidFrame)
    {
    this->RotImgFidFrame->Delete();
    this->RotImgFidFrame = NULL;
    }
  if (this->RotImgFidLabel)
    {
    this->RotImgFidLabel->Delete();
    this->RotImgFidLabel = NULL;
    }    
  
  if (this->RotImgFid)
    {
    this->RotImgFid->DeleteAllWidgets();
    this->RotImgFid = NULL;
    }
  if (this->RotPhyFidFrame)
    {
    this->RotPhyFidFrame->Delete();
    this->RotPhyFidFrame = NULL;
    }
  if (this->RotPhyFidLabel)
    {
    this->RotPhyFidLabel->Delete();
    this->RotPhyFidLabel = NULL;
    }
  if (this->RotPhyFid)
    {
    this->RotPhyFid->DeleteAllWidgets();
    this->RotPhyFid = NULL;
    }
  if (this->RotImgFidFrame)
    {
    this->RotImgFidFrame->Delete();
    this->RotImgFidFrame = NULL;
    }
  if (this->RotImgFidLabel)
    {
    this->RotImgFidLabel->Delete();
    this->RotImgFidLabel = NULL;
    }      
  if (this->RotationAngle)
    {
    this->RotationAngle->Delete();
    this->RotationAngle = NULL;
    }

  this->ImageRotationDone = false;
  
  this->CurrentSubState = -1;
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
  this->ImgSpacing->GetWidget(0)->SetEnabled(enable);
  this->ImgSpacing->GetWidget(1)->SetEnabled(enable);
  this->MonSpacing->GetWidget(0)->SetEnabled(enable);
  this->MonSpacing->GetWidget(1)->SetEnabled(enable);
  this->ImgScaling->GetWidget(0)->SetEnabled(enable);
  this->ImgScaling->GetWidget(1)->SetEnabled(enable);

}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::EnableDisableTranslateComponents(bool enable)
{
  this->TranslateFrame->GetFrame()->SetEnabled(enable);
  this->TransImgFid->GetWidget(0)->SetEnabled(enable);
  this->TransImgFid->GetWidget(1)->SetEnabled(enable);
  this->TransPhyFid->GetWidget(0)->SetEnabled(enable);
  this->TransPhyFid->GetWidget(1)->SetEnabled(enable);
  this->Translation->GetWidget(0)->SetEnabled(enable);
  this->Translation->GetWidget(1)->SetEnabled(enable);

}
//---------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::EnableDisableRotateComponents(bool enable)
{
  this->RotateFrame->GetFrame()->SetEnabled(enable);
  this->COR->GetWidget(0)->SetEnabled(enable);
  this->COR->GetWidget(1)->SetEnabled(enable);
  this->RotImgFid->GetWidget(0)->SetEnabled(enable);
  this->RotImgFid->GetWidget(1)->SetEnabled(enable);
  this->RotPhyFid->GetWidget(0)->SetEnabled(enable);
  this->RotPhyFid->GetWidget(1)->SetEnabled(enable);
  this->RotationAngle->GetWidget()->SetEnabled(enable);


}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::EnableDisableControls()
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
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  

/*  vtkEMSegmentAnatomicalStructureStep *anat_step = 
    this->GetGUI()->GetAnatomicalStructureStep();
  anat_step->ShowAnatomicalStructureTree();

  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();

  vtkIdType vol_id = mrmlManager->GetTreeRootNodeID();
  const char *root_node = 
    anat_step->GetAnatomicalStructureTree()->GetWidget()->FindNodeWithUserDataAsInt(
      NULL, vol_id);
  if (root_node && *root_node)
    {
    anat_step->SetAnatomicalTreeParentNodeSelectableState(root_node, 0);
    }

  // Override the tree callbacks for that specific step

  anat_step->GetAnatomicalStructureTree()->GetWidget()->SetSelectionChangedCommand(
      this, "DisplaySelectedNodeSpatialPriorsCallback");

  
  
  */

  // guid building/showing

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);

  vtkKWWidget *parent = wizard_widget->GetClientArea();

  //flip components
  this->ShowFlipComponents();

  // scale step components
  this->ShowScaleComponents();
  
  // translate components
  this->ShowTranslateComponents();

  // rotate components
  this->ShowRotateComponents();
  
  // TO DO: install callbacks
  this->InstallCallbacks();

  // TO DO: populate controls wherever needed
  this->PopulateControls();

  // enable disable controls
  this->EnableDisableControls();

  // Update the UI with the proper value, if there is a selection
  //this->DisplaySelectedNodeSpatialPriorsCallback();
  
  wizard_widget->SetErrorText(
    "Please note that the order of the click (first image fiducial, then physical fiducial) is important.");

  

}

//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ShowFlipComponents()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
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


//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ShowTranslateComponents()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
   
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
    this->TranslateFrame->SetBalloonHelpString("Click on the image fiducial first, followed by click on the corresponding physical fiducial you see through mirror, then enter the required translation here");
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->TranslateFrame->GetWidgetName());


  // translation individual components

  // image fiducial click
  if (!this->TransImgFidFrame)
    {
    this->TransImgFidFrame = vtkKWFrame::New();
    }
  if (!this->TransImgFidFrame->IsCreated())
    {
    this->TransImgFidFrame->SetParent(this->TranslateFrame->GetFrame());
    this->TransImgFidFrame->Create();
    }
  
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->TransImgFidFrame->GetWidgetName());

  // label
  if (!this->TransImgFidLabel)
    { 
    this->TransImgFidLabel = vtkKWLabel::New();
    }
  if (!this->TransImgFidLabel->IsCreated())
    {
    this->TransImgFidLabel->SetParent(this->TransImgFidFrame);
    this->TransImgFidLabel->Create();
    this->TransImgFidLabel->SetText("Image fiducial:     ");
    this->TransImgFidLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }  
    
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->TransImgFidLabel->GetWidgetName());
  
  if (!this->TransImgFid)
    {
    this->TransImgFid =  vtkKWEntrySet::New();  
    }
  if (!this->TransImgFid->IsCreated())
    {
    this->TransImgFid->SetParent(this->TransImgFidFrame);
    this->TransImgFid->Create();
    this->TransImgFid->SetBorderWidth(2);
    this->TransImgFid->SetReliefToGroove();
    this->TransImgFid->SetPackHorizontally(1);   
    this->TransImgFid->SetMaximumNumberOfWidgetsInPackingDirection(2);
    // two entries of monitor spacing (x, y)
    for (int id = 0; id < 2; id++)
      {
      vtkKWEntry *entry = this->TransImgFid->AddWidget(id);
      entry->SetWidth(7);
      entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
      //entry->ReadOnlyOn(); 
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->TransImgFid->GetWidgetName());

 

  // physical fiducial click
  if (!this->TransPhyFidFrame)
    {
    this->TransPhyFidFrame = vtkKWFrame::New();
    }
  if (!this->TransPhyFidFrame->IsCreated())
    {
    this->TransPhyFidFrame->SetParent(this->TranslateFrame->GetFrame());
    this->TransPhyFidFrame->Create();
    }
 
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->TransPhyFidFrame->GetWidgetName());

  // label
  if (!this->TransPhyFidLabel)
    { 
    this->TransPhyFidLabel = vtkKWLabel::New();
    }
  if (!this->TransPhyFidLabel->IsCreated())
    {
    this->TransPhyFidLabel->SetParent(this->TransPhyFidFrame);
    this->TransPhyFidLabel->Create();
    this->TransPhyFidLabel->SetText("Physical fiducial: ");
    this->TransPhyFidLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    } 
 
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->TransPhyFidLabel->GetWidgetName());
  

  if (!this->TransPhyFid)
    {
    this->TransPhyFid =  vtkKWEntrySet::New();  
    }
  if (!this->TransPhyFid->IsCreated())
    {
    this->TransPhyFid->SetParent(this->TransPhyFidFrame);
    this->TransPhyFid->Create();
    this->TransPhyFid->SetBorderWidth(2);
    this->TransPhyFid->SetReliefToGroove();
    this->TransPhyFid->SetPackHorizontally(1);   
    this->TransPhyFid->SetMaximumNumberOfWidgetsInPackingDirection(2);
    // two entries of monitor spacing (x, y)
    for (int id = 0; id < 2; id++)
      {
      vtkKWEntry *entry = this->TransPhyFid->AddWidget(id);
      entry->SetWidth(7);
      entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
      //entry->ReadOnlyOn(); 
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->TransPhyFid->GetWidgetName());
  

  // translation
  if (!this->TransEntryFrame)
    {
    this->TransEntryFrame = vtkKWFrame::New();
    }
  if (!this->TransEntryFrame->IsCreated())
    {
    this->TransEntryFrame->SetParent(this->TranslateFrame->GetFrame());
    this->TransEntryFrame->Create();
    }

  
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->TransEntryFrame->GetWidgetName());

  // label
  if (!this->TransEntryLabel)
    { 
    this->TransEntryLabel = vtkKWLabel::New();
    }
  if (!this->TransEntryLabel->IsCreated())
    {
    this->TransEntryLabel->SetParent(this->TransEntryFrame);
    this->TransEntryLabel->Create();
    this->TransEntryLabel->SetText("Translation:         ");
    this->TransEntryLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }
  
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->TransEntryLabel->GetWidgetName());
  

  if (!this->Translation)
    {
    this->Translation =  vtkKWEntrySet::New();  
    }
  if (!this->Translation->IsCreated())
    {
    this->Translation->SetParent(this->TransEntryFrame);
    this->Translation->Create();
    this->Translation->SetBorderWidth(2);
    this->Translation->SetReliefToGroove();
    this->Translation->SetPackHorizontally(1);    
    this->Translation->SetMaximumNumberOfWidgetsInPackingDirection(2);
    // two entries of monitor spacing (x, y)
    for (int id = 0; id < 2; id++)
      {
      vtkKWEntry *entry = this->Translation->AddWidget(id);
      entry->SetWidth(7);
      entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
      //entry->ReadOnlyOn(); 
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->Translation->GetWidgetName());

 
}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ShowRotateComponents()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
   
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
    this->RotateFrame->SetBalloonHelpString("Click on the image fiducial first, followed by click on the corresponding physical fiducial you see through mirror, then enter the required rotation angle here");
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->RotateFrame->GetWidgetName());


  // rotation individual components

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

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->CORFrame->GetWidgetName());


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
 
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->CORLabel->GetWidgetName());
  

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

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->COR->GetWidgetName());

  

  // image fiducial click
  if (!this->RotImgFidFrame)
    {
    this->RotImgFidFrame = vtkKWFrame::New();
    }
  if (!this->RotImgFidFrame->IsCreated())
    {
    this->RotImgFidFrame->SetParent(this->RotateFrame->GetFrame());
    this->RotImgFidFrame->Create();
    }

  
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->RotImgFidFrame->GetWidgetName());


  // label
  if (!this->RotImgFidLabel)
    { 
    this->RotImgFidLabel = vtkKWLabel::New();
    }
  if (!this->RotImgFidLabel->IsCreated())
    {
    this->RotImgFidLabel->SetParent(this->RotImgFidFrame);
    this->RotImgFidLabel->Create();
    this->RotImgFidLabel->SetText("Image fiducial:       ");
    this->RotImgFidLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }
  
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->RotImgFidLabel->GetWidgetName());
  

  if (!this->RotImgFid)
    {
    this->RotImgFid =  vtkKWEntrySet::New();    
    }
  if (!this->RotImgFid->IsCreated())
    {
    this->RotImgFid->SetParent(this->RotImgFidFrame);
    this->RotImgFid->Create();
    this->RotImgFid->SetBorderWidth(2);
    this->RotImgFid->SetReliefToGroove();
    this->RotImgFid->SetPackHorizontally(1);
    this->RotImgFid->SetMaximumNumberOfWidgetsInPackingDirection(2);
    // two entries of monitor spacing (x, y)
    for (int id = 0; id < 2; id++)
      {
      vtkKWEntry *entry = this->RotImgFid->AddWidget(id);
      entry->SetWidth(7);
      entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
      //entry->ReadOnlyOn(); 
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->RotImgFid->GetWidgetName());



  // physical fiducial click
  if (!this->RotPhyFidFrame)
    {
    this->RotPhyFidFrame = vtkKWFrame::New();
    }
  if (!this->RotPhyFidFrame->IsCreated())
    {
    this->RotPhyFidFrame->SetParent(this->RotateFrame->GetFrame());
    this->RotPhyFidFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->RotPhyFidFrame->GetWidgetName());


  // label
  if (!this->RotPhyFidLabel)
    { 
    this->RotPhyFidLabel = vtkKWLabel::New();
    }
  if (!this->RotPhyFidLabel->IsCreated())
    {
    this->RotPhyFidLabel->SetParent(this->RotPhyFidFrame);
    this->RotPhyFidLabel->Create();
    this->RotPhyFidLabel->SetText("Physical fiducial:   ");
    this->RotPhyFidLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }
 
  this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->RotPhyFidLabel->GetWidgetName());
  
  if (!this->RotPhyFid)
    {
    this->RotPhyFid =  vtkKWEntrySet::New();    
    }
  if (!this->RotPhyFid->IsCreated())
    {
    this->RotPhyFid->SetParent(this->RotPhyFidFrame);
    this->RotPhyFid->Create();
    this->RotPhyFid->SetBorderWidth(2);
    this->RotPhyFid->SetReliefToGroove();
    this->RotPhyFid->SetPackHorizontally(1);
    this->RotPhyFid->SetMaximumNumberOfWidgetsInPackingDirection(2);
    // two entries of monitor spacing (x, y)
    for (int id = 0; id < 2; id++)
      {
      vtkKWEntry *entry = this->RotPhyFid->AddWidget(id);
      entry->SetWidth(7);
      entry->SetDisabledBackgroundColor(0.9,0.9,0.9);
      //entry->ReadOnlyOn(); 
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->RotPhyFid->GetWidgetName());

  
  // rotation

  if (!this->RotationAngle)
    {
    this->RotationAngle =  vtkKWEntryWithLabel::New();  
    }
  if (!this->RotationAngle->IsCreated())
    {
    this->RotationAngle->SetParent(this->RotateFrame->GetFrame());
    this->RotationAngle->Create();
    this->RotationAngle->GetWidget()->SetRestrictValueToDouble();
    this->RotationAngle->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->RotationAngle->SetLabelText("Rotation (in degrees):");
    this->RotationAngle->SetWidth(7);
    this->RotationAngle->GetWidget()->SetDisabledBackgroundColor(0.9,0.9,0.9);
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->RotationAngle->GetWidgetName());

  
}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::InstallCallbacks()
{
  // flip components
  this->VerticalFlipCheckButton->GetWidget()->SetCommand(this, "VerticalFlipCallback");
  this->AddCallbackCommandObserver(this->VerticalFlipCheckButton->GetWidget(), vtkKWCheckButton::SelectedStateChangedEvent);
  this->HorizontalFlipCheckButton->GetWidget()->SetCommand(this, "HorizontalFlipCallback");
  this->AddCallbackCommandObserver(this->HorizontalFlipCheckButton->GetWidget(), vtkKWCheckButton::SelectedStateChangedEvent);

  char buffer[256]= "";
  // callback on image scaling entry
  for (int id = 0; id < 2; id++)
    {
    this->ImgScaling->GetWidget(id)->SetRestrictValueToDouble();
    sprintf(buffer, "ImageScalingEntryCallback %d", id);
    this->ImgScaling->GetWidget(id)->SetCommand(this, buffer);
    this->AddCallbackCommandObserver(this->ImgScaling->GetWidget(id), vtkKWEntry::EntryValueChangedEvent);
    }

  // callback on image translation entry
  for (int id = 0; id < 2; id++)
    {
    this->Translation->GetWidget(id)->SetRestrictValueToDouble();
    sprintf(buffer, "ImageTranslationEntryCallback %d", id);
    this->Translation->GetWidget(id)->SetCommand(this, buffer);
    this->AddCallbackCommandObserver(this->Translation->GetWidget(id), vtkKWEntry::EntryValueChangedEvent);
    }

  // callback on image rotation
  this->RotationAngle->GetWidget()->SetRestrictValueToDouble();
  this->RotationAngle->GetWidget()->SetCommand(this, "ImageRotationEntryCallback");
  this->AddCallbackCommandObserver(this->RotationAngle->GetWidget(), vtkKWEntry::EntryValueChangedEvent);
  
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

  vtkMRMLScalarVolumeNode *inVolume = vtkMRMLScalarVolumeNode::SafeDownCast(this->GetGUI()->GetMRMLScene()->GetNodeByID(mrmlNode->GetInputVolumeRef()));
  if (!inVolume)
    {
    // TO DO: what to do on failure
    return;
    }
 
  // populate flip frame components
  this->VerticalFlipCheckButton->GetWidget()->SetSelectedState(0);
  this->HorizontalFlipCheckButton->GetWidget()->SetSelectedState(0);


  // populate scale frame controls
  // 1. image spacing
  double imgSpacing[3];
  inVolume->GetSpacing(imgSpacing);
  this->ImgSpacing->GetWidget(0)->SetValueAsDouble(imgSpacing[0]);
  this->ImgSpacing->GetWidget(1)->SetValueAsDouble(imgSpacing[1]);

  // 2. monitor spacing
  double monSpacing[2];
  this->GetGUI()->GetSecondaryMonitor()->GetMonitorSpacing(monSpacing[0], monSpacing[1]);
  this->MonSpacing->GetWidget(0)->SetValueAsDouble(monSpacing[0]);
  this->MonSpacing->GetWidget(1)->SetValueAsDouble(monSpacing[1]);

  // set the actual scaling (image/mon) in mrml node
  mrmlNode->SetActualScaling(double(imgSpacing[0]/monSpacing[0]), double(imgSpacing[1]/monSpacing[1]), 1);

  // populate translate frame controls
  // the information will come in a callback that handles the mouse click

  // populate rotation frame controls
  // center of rotation
  vtkSlicerSliceLogic *sliceLogic = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI0()->GetLogic();
  double rasDimensions[3], rasCenter[3], sliceCenter[3];
  sliceLogic->GetVolumeRASBox (inVolume, rasDimensions, rasCenter);
  this->COR->GetWidget(0)->SetValueAsDouble(rasCenter[0]);
  this->COR->GetWidget(1)->SetValueAsDouble(rasCenter[1]);
  sliceLogic->GetVolumeSliceDimensions(inVolume, rasDimensions, sliceCenter);
  rasCenter[2] = sliceCenter[2];
  mrmlNode->SetCenterOfRotation(rasCenter);

  this->CurrentSubState = 0;

}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::HorizontalFlipCallback(int value)
{
  this->GetGUI()->GetMRMLNode()->SetHorizontalFlip(value);
  this->FlipImage();
  this->ImageFlipDone = true;
  this->CurrentSubState = 1;
  this->EnableDisableControls();

}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::VerticalFlipCallback(int value)
{
  // set user scaling in mrml node
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (mrmlNode)
    this->GetGUI()->GetMRMLNode()->SetVerticalFlip(value);
  this->FlipImage();
  this->ImageFlipDone = true;
  this->CurrentSubState = 1;
  this->EnableDisableControls();

}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ImageScalingEntryCallback(int widgetIndex, double value)
{
  static double xScaling = 0;
  static double yScaling = 0;
  if (widgetIndex == 0)
    {
    // x scaling entered
    xScaling = value;
    }
  else if(widgetIndex == 1)
    { 
    // y scaling entered
    yScaling = value;
    }
  
  if (xScaling > 0 && yScaling > 0)
    {
    // set user scaling in mrml node
    vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
    if (mrmlNode)
      {
      mrmlNode->SetUserScaling(xScaling, yScaling,1);
      mrmlNode->CalculateCalibrateScaleError();
      }

    // actually scale the image
    // TO DO:
    this->ScaleImage();

    this->ImageScalingDone = true;

    // go to translation state
    this->CurrentSubState = 2;
    this->EnableDisableControls();
    }
    
}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ImageTranslationEntryCallback(int widgetIndex, double value)
{
  static double xTranslation;
  static double yTranslation;
  static bool xTransChanged = false;
  static bool yTransChanged = false;
  if (widgetIndex == 0)
    {
    // x translation entered
    xTranslation = value;
    xTransChanged = true;
    }
  else if(widgetIndex == 1)
    { 
    // y translation entered
    yTranslation = value;
    yTransChanged = true;
    }
  
  if (xTransChanged && yTransChanged)
    {
    // record user input in mrml node
    this->GetGUI()->GetMRMLNode()->SetUserTranslation(xTranslation, yTranslation, 0);
    this->GetGUI()->GetMRMLNode()->CalculateCalibrateTranslationError();
    // actually translate the image
    // TO DO:
    this->TranslateImage();

    this->ImageTranslationDone = true;
    // go to rotation state
    this->CurrentSubState = 3;
    this->EnableDisableControls();
    }
    
}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ImageRotationEntryCallback(double value)
{
    // TO DO: check if its a valid input i.e. between 0 to 360
    this->GetGUI()->GetMRMLNode()->SetUserRotation(value);
    this->GetGUI()->GetMRMLNode()->CalculateCalibrateRotationError();
    // actually rotate the image
    // TO DO:
    this->RotateImage();
    this->ImageRotationDone = true;
    // diable all
    this->CurrentSubState = -1;
    this->EnableDisableControls(); 
}

//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData)
{
  static unsigned int clickNum = 0;
  // first identify if the step is pertinent, i.e. current step of wizard workflow is actually calibration step

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  if (!wizard_widget ||
      wizard_widget->GetWizardWorkflow()->GetCurrentStep() != 
      this)
    {
    return;
    }

  vtkSlicerInteractorStyle *s = vtkSlicerInteractorStyle::SafeDownCast(caller);
  //vtkSlicerInteractorStyle *istyle0 = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyleSecondary = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetSecondaryMonitor()->GetRenderWindowInteractor()->GetInteractorStyle());
  
  // listen to click only when they come from secondary monitor's window
  if ((s == istyleSecondary) && (event == vtkCommand::LeftButtonPressEvent) )
    {
    // hear clicks only if the current sub state is Translate or Rotate
    if (! ((this->CurrentSubState == 2) || (this->CurrentSubState == 3)))
        return;

    // mouse click happened in the axial slice view
    ++clickNum;

    // capture the point

    vtkRenderWindowInteractor *rwi;
    vtkMatrix4x4 *matrix;    
    
    /*if (s == istyle0)
      {
      // coming from main gui viewer of SLICER
      vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI0();
      rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();
      matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();    
      }
    else if ( s == istyleSecondary)*/
      {
      // coming from click on secondary monitor
      rwi = this->GetGUI()->GetSecondaryMonitor()->GetRenderWindowInteractor();
      matrix = this->GetGUI()->GetSecondaryMonitor()->GetXYToRAS();  
      //matrix1 = this->GetGUI()->GetSecondaryMonitor()->GetXYToIJK();
      }

    int point[2];
    rwi->GetLastEventPosition(point);
    double inPt[4] = {point[0], point[1], 0, 1};
    double outPt[4];    
    matrix->MultiplyPoint(inPt, outPt); 
    double ras[3] = {outPt[0], outPt[1], outPt[2]};
    this->RecordClick(point, ras, clickNum);
    }
}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::RecordClick(int xyPoint[2], double rasPoint[3], unsigned int & clickNum)
{
  switch (this->CurrentSubState)
    {
    case 2: // click happened while in Translate state, depending on click number, it is either image fiducial click or physical fiducial click
        if (clickNum ==1)
          {
          this->TransImgFid->GetWidget(0)->SetValueAsDouble(rasPoint[0]);
          this->TransImgFid->GetWidget(1)->SetValueAsDouble(rasPoint[1]);
          }
        else if (clickNum == 2)
          {
          this->TransPhyFid->GetWidget(0)->SetValueAsDouble(rasPoint[0]);
          this->TransPhyFid->GetWidget(1)->SetValueAsDouble(rasPoint[1]);
          // record value in the MRML node
          double translation[3];
          translation[0] = (this->TransPhyFid->GetWidget(0)->GetValueAsDouble() - this->TransImgFid->GetWidget(0)->GetValueAsDouble());
          translation[1] = (this->TransPhyFid->GetWidget(1)->GetValueAsDouble() - this->TransImgFid->GetWidget(1)->GetValueAsDouble());
          translation[2] = 0;
          this->GetGUI()->GetMRMLNode()->SetActualTranslation(translation);
          clickNum = 0;
          }

        break;
    case 3:
        // click happened while in Rotate state, depending on click number, it is either image fiducial click or physical fiducial click
        if (clickNum ==1)
          {
          this->RotImgFid->GetWidget(0)->SetValueAsDouble(rasPoint[0]);
          this->RotImgFid->GetWidget(1)->SetValueAsDouble(rasPoint[1]);
          }
        else if (clickNum == 2)
          {
          this->RotPhyFid->GetWidget(0)->SetValueAsDouble(rasPoint[0]);
          this->RotPhyFid->GetWidget(1)->SetValueAsDouble(rasPoint[1]);       
          
          // calculate rotation
          double actualRotation = 0;
          this->CalculateImageRotation(actualRotation);
          // and set the value in the MRML node
          this->GetGUI()->GetMRMLNode()->SetActualRotation(actualRotation);
          clickNum = 0;
          }
        break;
    default:
        clickNum = 0;
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

//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::FlipImage()
{
  if(this->ImageFlipDone)
      return;

  bool verticalFlip = false;
  bool horizontalFlip = false;

  verticalFlip = this->GetGUI()->GetMRMLNode()->GetVerticalFlip();
  horizontalFlip = this->GetGUI()->GetMRMLNode()->GetHorizontalFlip();

  if (verticalFlip)
    this->GetGUI()->GetSecondaryMonitor()->FlipVertical();

  if (horizontalFlip)
    this->GetGUI()->GetSecondaryMonitor()->FlipHorizontal(); 

}
//----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::ScaleImage()
{
  if (this->ImageScalingDone)
      return;

  double scale[3];
  scale[0] = 1;
  scale[1] = 1;
  scale[2] = 1;
  this->GetGUI()->GetMRMLNode()->GetUserScaling(scale);

  // scaling only required in secondary monitor
  this->GetGUI()->GetSecondaryMonitor()->Scale(scale[0], scale[1], scale[2]);

  /*
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


  //
  // Scale by TransformNode method
  //
  double scale[3];
  scale[0] = 1;
  scale[1] = 1;
  scale[2] = 1;
  this->GetGUI()->GetMRMLNode()->GetUserScaling(scale);

  double cor[3];  
  this->GetGUI()->GetMRMLNode()->GetCenterOfRotation(cor);
  
  vtkTransform* transform = vtkTransform::New();
  transform->Identity();
  transform->PostMultiply();
  transform->Translate (-cor[0],-cor[1], -cor[2]);
  transform->Scale(scale[0], scale[1], 1);
  transform->Translate (cor[0],cor[1], cor[2]); 
  inVolume->ApplyTransform(transform);

  inVolume->Modified();

  
  // reflect those changes in the MRML transform node

  vtkMRMLLinearTransformNode *node = vtkMRMLLinearTransformNode::SafeDownCast(this->GetGUI()->GetMRMLNode()->GetCalibrationMRMLTransformNode());
 
  if (node != NULL)
    {
    node->GetMatrixTransformToParent()->SetElement(0,0, transform->GetMatrix()->GetElement(0,0)) ;  
    node->GetMatrixTransformToParent()->SetElement(1,1, transform->GetMatrix()->GetElement(1,1)) ;  
    node->GetMatrixTransformToParent()->SetElement(2,2, transform->GetMatrix()->GetElement(2,2)) ;
    node->Modified();
    }
*/

}
//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::TranslateImage()
{
  if (this->ImageTranslationDone)
      return;

  double translationRAS[3];  
  this->GetGUI()->GetMRMLNode()->GetUserTranslation(translationRAS[0], translationRAS[1], translationRAS[2]);

  this->GetGUI()->GetSecondaryMonitor()->Translate(translationRAS[0], translationRAS[1], translationRAS[2]);

  /*
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


  //
  // translation by TransformNode method
  //
  double translationRAS[4];
  translationRAS[3] = 0;
  this->GetGUI()->GetMRMLNode()->GetUserTranslation(translationRAS[0], translationRAS[1], translationRAS[2]);


  vtkTransform* transform = vtkTransform::New();

  transform->Identity();

  transform->Translate(translationRAS[0], translationRAS[1], translationRAS[2]);
    
  inVolume->ApplyTransform(transform);

  inVolume->Modified();

  // reflect those changes in the MRML transform node

  vtkMRMLLinearTransformNode *node = vtkMRMLLinearTransformNode::SafeDownCast(this->GetGUI()->GetMRMLNode()->GetCalibrationMRMLTransformNode());

  if (node != NULL)
    {
    node->GetMatrixTransformToParent()->SetElement(0,3, transform->GetMatrix()->GetElement(0,3)) ;  
    node->GetMatrixTransformToParent()->SetElement(1,3, transform->GetMatrix()->GetElement(1,3)) ;  
    node->GetMatrixTransformToParent()->SetElement(2,3, transform->GetMatrix()->GetElement(2,3)) ;
    node->Modified();
    }


    // update the center of rotation, since there has been a translation
  // center of rotation
  
  vtkSlicerSliceLogic *sliceLogic = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI0()->GetLogic();
  double rasDimensions[3], rasCenter[3], sliceCenter[3];
  sliceLogic->GetVolumeRASBox (inVolume, rasDimensions, rasCenter);
  this->COR->GetWidget(0)->SetValueAsDouble(rasCenter[0]);
  this->COR->GetWidget(1)->SetValueAsDouble(rasCenter[1]);
  sliceLogic->GetVolumeSliceDimensions(inVolume, rasDimensions, sliceCenter);
  rasCenter[2] = sliceCenter[2];
  this->GetGUI()->GetMRMLNode()->SetCenterOfRotation(rasCenter);*/
  
  /*
  // translate by SliceLogic and SliceNode way

  // get the slice node and slice logic
//  vtkSlicerSliceLogic *sliceLogic = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI0()->GetLogic();
  vtkMRMLSliceNode *sliceNode = sliceLogic->GetSliceNode();
  


  //
  // translation 
  //
  //double translationRAS[4];
  translationRAS[3] = 0;
  double translationSlice[4];
  this->GetGUI()->GetMRMLNode()->GetUserTranslation(translationRAS[0], translationRAS[1], translationRAS[2]);

  // get the translation from ras to slice space
  vtkMatrix4x4 *rasToSlice = vtkMatrix4x4::New();
  rasToSlice->DeepCopy(sliceNode->GetSliceToRAS());
  rasToSlice->MultiplyPoint( translationRAS, translationSlice );
  rasToSlice->Delete();

  vtkMatrix4x4 *sliceToRAS = vtkMatrix4x4::New();
  sliceToRAS->DeepCopy(sliceNode->GetSliceToRAS());  
  double sr = sliceToRAS->GetElement(0, 3);
  double sa = sliceToRAS->GetElement(1, 3);
  double ss = sliceToRAS->GetElement(2, 3);  
  sliceToRAS->SetElement(0, 3, sr + translationSlice[0]);
  sliceToRAS->SetElement(1, 3, sa + translationSlice[1]);
  sliceToRAS->SetElement(2, 3, ss + translationSlice[2]);
  sliceNode->GetSliceToRAS()->DeepCopy(sliceToRAS);
  vtkMatrix4x4 *xyslice = sliceNode->GetXYToSlice();
  vtkMatrix4x4 *sliceRas = sliceNode->GetSliceToRAS();
  vtkMatrix4x4 *xyRas = sliceNode->GetXYToRAS();
  sliceNode->UpdateMatrices( );
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI0()->GetSliceViewer()->RequestRender();
  sliceToRAS->Delete(); 
*/

}
//-----------------------------------------------------------------------------
void vtkPerkStationCalibrateStep::RotateImage()
{
  if (this->ImageRotationDone)
     return;

  double rotationRAS = this->GetGUI()->GetMRMLNode()->GetUserRotation();

  this->GetGUI()->GetSecondaryMonitor()->Rotate(rotationRAS);

   /*
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
  
  double rotationRAS = this->GetGUI()->GetMRMLNode()->GetUserRotation();
  
  double cor[3];  
  this->GetGUI()->GetMRMLNode()->GetCenterOfRotation(cor);
  
  
  vtkTransform* transform = vtkTransform::New();
  transform->Identity();
  transform->PostMultiply();
  transform->Translate (-cor[0],-cor[1], -cor[2]);
  transform->RotateZ(-rotationRAS);
  transform->Translate (cor[0],cor[1], cor[2]);
  inVolume->ApplyTransform(transform);
  inVolume->Modified();

  vtkMRMLLinearTransformNode *node = vtkMRMLLinearTransformNode::SafeDownCast(this->GetGUI()->GetMRMLNode()->GetCalibrationMRMLTransformNode());
  
  double sx = node->GetMatrixTransformToParent()->GetElement(0,0);
  double sy = node->GetMatrixTransformToParent()->GetElement(1,1);
  double sz = node->GetMatrixTransformToParent()->GetElement(2,2);
  if (node != NULL)
    {
    node->GetMatrixTransformToParent()->SetElement(0,0, sx * transform->GetMatrix()->GetElement(0,0)) ; 
    node->GetMatrixTransformToParent()->SetElement(1,0, transform->GetMatrix()->GetElement(1,0)) ;  
    node->GetMatrixTransformToParent()->SetElement(2,0, transform->GetMatrix()->GetElement(2,0)) ;
    node->GetMatrixTransformToParent()->SetElement(0,1, transform->GetMatrix()->GetElement(0,1)) ;  
    node->GetMatrixTransformToParent()->SetElement(1,1, sy * transform->GetMatrix()->GetElement(1,1)) ; 
    node->GetMatrixTransformToParent()->SetElement(2,1, transform->GetMatrix()->GetElement(2,1)) ;
    node->GetMatrixTransformToParent()->SetElement(0,2, transform->GetMatrix()->GetElement(0,2)) ;  
    node->GetMatrixTransformToParent()->SetElement(1,2, transform->GetMatrix()->GetElement(1,2)) ;  
    node->GetMatrixTransformToParent()->SetElement(2,2, sz * transform->GetMatrix()->GetElement(2,2)) ; 
    node->Modified();
    }
*/

   /*
 // translate by SliceLogic and SliceNode way

  // get the slice node and slice logic
  vtkSlicerSliceLogic *sliceLogic = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI0()->GetLogic();
  vtkMRMLSliceNode *sliceNode = sliceLogic->GetSliceNode();
  


  //
  // rotation 
  //
  //double translationRAS[4];

  vtkMatrix4x4 *xyslice = sliceNode->GetXYToSlice();
  vtkMatrix4x4 *sliceRas = sliceNode->GetSliceToRAS();
  vtkMatrix4x4 *xyRas = sliceNode->GetXYToRAS();
  
  vtkMatrix4x4 *sliceToRAS = vtkMatrix4x4::New();
  sliceToRAS->DeepCopy(sliceNode->GetSliceToRAS());    
  
  double insAngleRad = -double(vtkMath::Pi()/180)*rotationRAS;
  sliceToRAS->SetElement(0,0, cos(insAngleRad));
  sliceToRAS->SetElement(0,1, -sin(insAngleRad));  
  sliceToRAS->SetElement(1,0, sin(insAngleRad));
  sliceToRAS->SetElement(1,1, cos(insAngleRad));  

  sliceNode->GetSliceToRAS()->DeepCopy(sliceToRAS);
  sliceNode->UpdateMatrices( );
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI0()->GetSliceViewer()->RequestRender();
  sliceToRAS->Delete(); 
*/
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

//----------------------------------------------------------------------------
/*void vtkPerkStationCalibrateStep::Validate()
{
}*/
