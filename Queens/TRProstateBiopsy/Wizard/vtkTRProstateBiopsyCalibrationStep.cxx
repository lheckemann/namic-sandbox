#include "vtkTRProstateBiopsyCalibrationStep.h"

#include "vtkTRProstateBiopsyGUI.h"
#include "vtkTRProstateBiopsyLogic.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkMRMLFiducialListNode.h"

#include "vtkMath.h"
#include "vtkLineSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"

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
#include "itkMetaDataObject.h"
#include "vtkImageChangeInformation.h"


#include "vtkVolumeTextureMapper3D.h"
#include "vtkPiecewiseFunction.h"
#include "vtkFixedPointVolumeRayCastMapper.h"
#include "vtkVolumeProperty.h"
#include "vtkColorTransferFunction.h"
#include "vtkPointData.h"

#include "vtkTRProstateBiopsyCalibrationAlgo.h"

#include <vtksys/ios/sstream>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkTRProstateBiopsyCalibrationStep);
vtkCxxRevisionMacro(vtkTRProstateBiopsyCalibrationStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkTRProstateBiopsyCalibrationStep::vtkTRProstateBiopsyCalibrationStep()
{
  this->SetName("1/4. Calibration");
  this->SetDescription("Select a calibration volume, then click calibration markers.");

  this->WizardGUICallbackCommand->SetCallback(&vtkTRProstateBiopsyCalibrationStep::WizardGUICallback);

  //this->MRMLCallbackCommand->SetCallback(&vtkTRProstateBiopsyCalibrationStep::MRMLCallback);

  this->LoadVolumeDialogFrame=vtkSmartPointer<vtkKWFrame>::New();
  this->LoadCalibrationVolumeButton=vtkSmartPointer<vtkKWLoadSaveButton>::New();
  this->LoadCalibrationSettingsFileButton=vtkSmartPointer<vtkKWLoadSaveButton>::New();
  this->SaveCalibrationSettingsFileButton=vtkSmartPointer<vtkKWLoadSaveButton>::New();
  this->ResetCalibrationButton=vtkSmartPointer<vtkKWPushButton>::New();
  this->ResegmentButton=vtkSmartPointer<vtkKWPushButton>::New();
  this->LoadResetFrame=vtkSmartPointer<vtkKWFrame>::New();
  this->ExportImportFrame=vtkSmartPointer<vtkKWFrame>::New();
  this->FiducialPropertiesFrame=vtkSmartPointer<vtkKWFrameWithLabel>::New();
  this->FiducialWidthSpinBox=vtkSmartPointer<vtkKWSpinBoxWithLabel>::New();
  this->FiducialHeightSpinBox=vtkSmartPointer<vtkKWSpinBoxWithLabel>::New();
  this->FiducialDepthSpinBox=vtkSmartPointer<vtkKWSpinBoxWithLabel>::New();
  for (int i = 0; i < CALIB_MARKER_COUNT; i++)
    {
    this->FiducialThresholdScale[i]=vtkSmartPointer<vtkKWScaleWithEntry>::New();
    }
  this->RadiusSpinBox=vtkSmartPointer<vtkKWSpinBoxWithLabel>::New();
  this->RadiusCheckButton=vtkSmartPointer<vtkKWCheckButton>::New();
  this->InitialAngleSpinBox=vtkSmartPointer<vtkKWSpinBoxWithLabel>::New();
  this->SegmentationResultsFrame=vtkSmartPointer<vtkKWFrameWithLabel>::New();
  this->CalibrationResultsBox=vtkSmartPointer<vtkKWTextWithScrollbars>::New();
  this->Axes1Actor=vtkSmartPointer<vtkActor>::New();
  this->Axes2Actor=vtkSmartPointer<vtkActor>::New();

  this->ProcessingCallback = false;
  this->ClickNumber = 0;
  this->AllMarkersAcquired = false;

  CalibPointPreProcRendererList.resize(CALIB_MARKER_COUNT);
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
void vtkTRProstateBiopsyCalibrationStep::ShowLoadResetControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // first clear the components
  //this->ClearLoadResetControls();
  
  // show load dialog box, and reset push button
    
  // Create the frame
  if (!this->LoadResetFrame->IsCreated())
    {
    this->LoadResetFrame->SetParent(parent);
    this->LoadResetFrame->Create();     
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", this->LoadResetFrame->GetWidgetName());

  if (!this->InitialAngleSpinBox->IsCreated())
    {
    this->InitialAngleSpinBox->SetParent(this->LoadResetFrame);
    this->InitialAngleSpinBox->Create();
    this->InitialAngleSpinBox->GetWidget()->SetWidth(11);
    this->InitialAngleSpinBox->GetWidget()->SetRange(-180,180);
    this->InitialAngleSpinBox->GetWidget()->SetIncrement(1);
    this->InitialAngleSpinBox->GetWidget()->SetValue(0);
    this->InitialAngleSpinBox->SetLabelText("Initial rotation angle");
    //this->InitialAngleSpinBox->GetWidget()->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, this->WizardGUICallbackCommand);
    }
  this->Script("pack %s -side top -anchor w -padx 2 -pady 2", this->InitialAngleSpinBox->GetWidgetName() );
  
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ClearLoadResetControls()
{
  this->Script("pack forget %s", this->LoadResetFrame->GetWidgetName());
  this->Script("pack forget %s", this->InitialAngleSpinBox->GetWidgetName());
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ShowLoadVolumeControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // first clear the components
  //this->ClearLoadVolumeControls();
  
  if (!this->LoadVolumeDialogFrame->IsCreated())
    {
    this->LoadVolumeDialogFrame->SetParent(parent);
    this->LoadVolumeDialogFrame->Create();     
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", this->LoadVolumeDialogFrame->GetWidgetName());
  
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
    this->LoadCalibrationVolumeButton->TrimPathFromFileNameOff();    
    this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode    
    //this->LoadCalibrationVolumeButton->GetWidget()->AddObserver(vtkKWPushButton::InvokedEvent, this->WizardGUICallbackCommand);
    }
  this->Script("pack %s -side top -fill x -anchor nw -padx 2 -pady 2", this->LoadCalibrationVolumeButton->GetWidgetName());
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ClearLoadVolumeControls()
{
  this->Script("pack forget %s", this->LoadVolumeDialogFrame->GetWidgetName());
  this->Script("pack forget %s", this->LoadCalibrationVolumeButton->GetWidgetName());
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ShowFiducialSegmentParamsControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // first clear the components
  //this->ClearFiducialSegmentParamsControls();

  if (!this->FiducialPropertiesFrame->IsCreated())
    {
    this->FiducialPropertiesFrame->SetParent(parent);
    this->FiducialPropertiesFrame->Create();
    this->FiducialPropertiesFrame->SetLabelText("Fiducial properties");
    this->FiducialPropertiesFrame->CollapseFrame();    
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", this->FiducialPropertiesFrame->GetWidgetName());

  if (!this->FiducialWidthSpinBox->IsCreated())
    {
    this->FiducialWidthSpinBox->SetParent(this->FiducialPropertiesFrame->GetFrame());
    this->FiducialWidthSpinBox->Create();
    this->FiducialWidthSpinBox->GetWidget()->SetWidth(11);
    this->FiducialWidthSpinBox->GetWidget()->SetRange(0,100);
    this->FiducialWidthSpinBox->GetWidget()->SetIncrement(1);
    this->FiducialWidthSpinBox->GetWidget()->SetValue(8);
    this->FiducialWidthSpinBox->SetLabelText("Fiducial width");
    //this->FiducialWidthSpinBox->GetWidget()->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, this->WizardGUICallbackCommand);
    }
  if (!this->FiducialHeightSpinBox->IsCreated())
    {
    this->FiducialHeightSpinBox->SetParent(this->FiducialPropertiesFrame->GetFrame());
    this->FiducialHeightSpinBox->Create();
    this->FiducialHeightSpinBox->GetWidget()->SetWidth(11);
    this->FiducialHeightSpinBox->GetWidget()->SetRange(0,100);
    this->FiducialHeightSpinBox->GetWidget()->SetIncrement(1);
    this->FiducialHeightSpinBox->GetWidget()->SetValue(5);
    this->FiducialHeightSpinBox->SetLabelText("Fiducial height");
    //this->FiducialHeightSpinBox->GetWidget()->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, this->WizardGUICallbackCommand);
    }
  if (!this->FiducialDepthSpinBox->IsCreated())
    {
    this->FiducialDepthSpinBox->SetParent(this->FiducialPropertiesFrame->GetFrame());
    this->FiducialDepthSpinBox->Create();
    this->FiducialDepthSpinBox->GetWidget()->SetWidth(11);
    this->FiducialDepthSpinBox->GetWidget()->SetRange(0,100);
    this->FiducialDepthSpinBox->GetWidget()->SetIncrement(1);
    this->FiducialDepthSpinBox->GetWidget()->SetValue(5);
    this->FiducialDepthSpinBox->SetLabelText("Fiducial depth");
    //this->FiducialDepthSpinBox->GetWidget()->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, this->WizardGUICallbackCommand);
    }
  this->Script("pack %s %s %s -side top -anchor w -padx 2 -pady 2", 
               this->FiducialWidthSpinBox->GetWidgetName(),
               this->FiducialHeightSpinBox->GetWidgetName(),
               this->FiducialDepthSpinBox->GetWidgetName());

  if (!this->RadiusSpinBox->IsCreated())
    {
    this->RadiusSpinBox->SetParent(this->FiducialPropertiesFrame->GetFrame());
    this->RadiusSpinBox->Create();
    this->RadiusSpinBox->GetWidget()->SetWidth(11);
    this->RadiusSpinBox->GetWidget()->SetRange(1,100);
    this->RadiusSpinBox->GetWidget()->SetIncrement(1);
    this->RadiusSpinBox->GetWidget()->SetValue(35);
    this->RadiusSpinBox->SetLabelText("Fiducial radius");
    //this->RadiusSpinBox->GetWidget()->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, this->WizardGUICallbackCommand);
    }
  //this->Script("pack %s -side left -anchor w -padx 2 -pady 2", this->RadiusSpinBox->GetWidgetName());
  this->Script("pack %s -side top -anchor w -padx 2 -pady 2", this->RadiusSpinBox->GetWidgetName());

  std::ostrstream ospack;
  ospack << "pack";
  for (int i = 0; i < CALIB_MARKER_COUNT; i++)
    {
    if (!this->FiducialThresholdScale[i]->IsCreated())
      {  
      this->FiducialThresholdScale[i]->SetParent(this->FiducialPropertiesFrame->GetFrame());
      this->FiducialThresholdScale[i]->Create();
      std::ostrstream os;
      os << "Threshold "<< i << std::ends;    
      this->FiducialThresholdScale[i]->SetLabelText(os.str());
      os.rdbuf()->freeze();
      this->FiducialThresholdScale[i]->SetWidth(3);
      this->FiducialThresholdScale[i]->SetLength(200);
      this->FiducialThresholdScale[i]->SetRange(0,100);
      this->FiducialThresholdScale[i]->SetResolution(1);
      this->FiducialThresholdScale[i]->SetValue(9);
      //this->FiducialThresholdScale[i]->AddObserver(vtkKWScale::ScaleValueChangingEvent, this->WizardGUICallbackCommand);            
      }
    ospack << " " << this->FiducialThresholdScale[i]->GetWidgetName();
    }
  ospack << " -side top -anchor w -padx 2 -pady 2" << std::ends;
  this->Script(ospack.str());
  ospack.rdbuf()->freeze();
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ClearFiducialSegmentParamsControls()
{
  this->Script("pack forget %s", this->FiducialPropertiesFrame->GetWidgetName());
  this->Script("pack forget %s", this->FiducialWidthSpinBox->GetWidgetName());
  this->Script("pack forget %s", this->FiducialHeightSpinBox->GetWidgetName());
  this->Script("pack forget %s", this->FiducialDepthSpinBox->GetWidgetName());  
  for (int i = 0; i<CALIB_MARKER_COUNT; i++)
    {
    this->Script("pack forget %s", this->FiducialThresholdScale[i]->GetWidgetName());
    }
  this->Script("pack forget %s", this->RadiusSpinBox->GetWidgetName());
  this->Script("pack forget %s", this->RadiusCheckButton->GetWidgetName());
  this->Script("pack forget %s", this->ResegmentButton->GetWidgetName());
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ShowFiducialSegmentationResultsControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
   
  // first clear the translate components if they were created by previous mode
  //this->ClearFiducialSegmentationResultsControls();

  // Create the frame
  if (!this->SegmentationResultsFrame->IsCreated())
    {
    this->SegmentationResultsFrame->SetParent(parent);
    this->SegmentationResultsFrame->Create();
    this->SegmentationResultsFrame->SetLabelText("Segmentation/registration results");
    this->SegmentationResultsFrame->SetBalloonHelpString("Results of segmentation and robot registration");
    this->SegmentationResultsFrame->AllowFrameToCollapseOff();
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    this->SegmentationResultsFrame->GetWidgetName());

  if(!this->CalibrationResultsBox->IsCreated())
    {
    this->CalibrationResultsBox->SetParent(this->SegmentationResultsFrame->GetFrame());
    this->CalibrationResultsBox->Create();
    this->CalibrationResultsBox->SetHorizontalScrollbarVisibility(0);
    this->CalibrationResultsBox->SetVerticalScrollbarVisibility( 1) ;
    this->CalibrationResultsBox->GetWidget()->SetText("");      
    this->CalibrationResultsBox->GetWidget()->SetBackgroundColor(0.7, 0.7, 0.95);
    this->CalibrationResultsBox->SetHeight(6);
    this->CalibrationResultsBox->GetWidget()->SetWrapToWord();
    this->CalibrationResultsBox->GetWidget()->ReadOnlyOn();
    this->CalibrationResultsBox->SetBorderWidth(2);
    this->CalibrationResultsBox->SetReliefToGroove();
    this->CalibrationResultsBox->GetWidget()->SetFont("times 12 bold");
    //this->Message->SetForegroundColor(0.0, 1.0, 0.0);
    }
  this->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 6", this->CalibrationResultsBox->GetWidgetName());  

}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ClearFiducialSegmentationResultsControls()
{
  this->Script("pack forget %s", this->SegmentationResultsFrame->GetWidgetName());
  this->Script("pack forget %s", this->CalibrationResultsBox->GetWidgetName());
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ShowExportImportControls()
{
   vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // first clear the components if they created before for other mode
  //this->ClearExportImportControls();

  // Create the frame
  if (!this->ExportImportFrame->IsCreated())
    {
    this->ExportImportFrame->SetParent(parent);
    this->ExportImportFrame->Create();
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", this->ExportImportFrame->GetWidgetName());

   // create the resegment button
  if (!this->ResegmentButton->IsCreated())
    {
    this->ResegmentButton->SetParent(this->ExportImportFrame);
    this->ResegmentButton->Create();
    this->ResegmentButton->SetText("Re-segment");
    this->ResegmentButton->SetBorderWidth(2);
    this->ResegmentButton->SetReliefToRaised();      
    this->ResegmentButton->SetHighlightThickness(2);
    this->ResegmentButton->SetBackgroundColor(0.85,0.85,0.85);
    this->ResegmentButton->SetActiveBackgroundColor(1,1,1);      
    this->ResegmentButton->SetBalloonHelpString("Click to re-segment the fiducials, with new parameters, but old fiducial location guesses. If you wish to give fiducial guesses again, press 'Reset calibration' button on top of the panel");
    //this->ResegmentButton->SetImageToPredefinedIcon(vtkKWIcon::IconTrashcan);
    this->ResegmentButton->SetEnabled(1);
    }    
  //this->Script("pack %s -side left -anchor nw -padx 2 -pady 4", this->ResegmentButton->GetWidgetName());
  this->Script("grid %s -row 0 -column 0 -sticky ew -padx 2 -pady 4", this->ResegmentButton->GetWidgetName());

  // create the reset calib button
  if(!this->ResetCalibrationButton->IsCreated())
    {
    this->ResetCalibrationButton->SetParent(this->ExportImportFrame);
    this->ResetCalibrationButton->Create();
    this->ResetCalibrationButton->SetText("Reset calibration");
    this->ResetCalibrationButton->SetBorderWidth(2);
    this->ResetCalibrationButton->SetReliefToRaised();      
    this->ResetCalibrationButton->SetHighlightThickness(2);
    this->ResetCalibrationButton->SetBackgroundColor(0.85,0.85,0.85);
    this->ResetCalibrationButton->SetActiveBackgroundColor(1,1,1);      
    //this->ResetCalibrationButton->SetImageToPredefinedIcon(vtkKWIcon::IconTrashcan);
    this->ResetCalibrationButton->SetBalloonHelpString("Click to re-start the calibration, with new fiducial location guesses");    
    }
    
  //this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", this->ResetCalibrationButton->GetWidgetName());
  this->Script("grid %s -row 0 -column 1 -sticky ew -padx 2 -pady 4", this->ResetCalibrationButton->GetWidgetName());


  // create the save file dialog button
  if (!this->SaveCalibrationSettingsFileButton->IsCreated())
    {
    this->SaveCalibrationSettingsFileButton->SetParent(this->ExportImportFrame);
    this->SaveCalibrationSettingsFileButton->Create();
    this->SaveCalibrationSettingsFileButton->SetText("Export calibration");
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
    }
  //this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", this->SaveCalibrationSettingsFileButton->GetWidgetName());
  this->Script("grid %s -row 1 -column 0 -sticky ew -padx 2 -pady 4", this->SaveCalibrationSettingsFileButton->GetWidgetName());

  // create the load file dialog button
  if (!this->LoadCalibrationSettingsFileButton->IsCreated())
    {
    this->LoadCalibrationSettingsFileButton->SetParent(this->ExportImportFrame);
    this->LoadCalibrationSettingsFileButton->Create();
    this->LoadCalibrationSettingsFileButton->SetBorderWidth(2);
    this->LoadCalibrationSettingsFileButton->SetReliefToRaised();       
    this->LoadCalibrationSettingsFileButton->SetHighlightThickness(2);
    this->LoadCalibrationSettingsFileButton->SetBackgroundColor(0.85,0.85,0.85);
    this->LoadCalibrationSettingsFileButton->SetActiveBackgroundColor(1,1,1);
    this->LoadCalibrationSettingsFileButton->SetText("Import calibration");
    this->LoadCalibrationSettingsFileButton->SetImageToPredefinedIcon(vtkKWIcon::IconPresetLoad);
    this->LoadCalibrationSettingsFileButton->SetBalloonHelpString("Click to load a previous calibration file");
    this->LoadCalibrationSettingsFileButton->TrimPathFromFileNameOff();
    this->LoadCalibrationSettingsFileButton->SetMaximumFileNameLength(256);
    this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode
    this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog()->SetFileTypes("{{XML File} {.xml}} {{All Files} {*.*}}");      
    }
  //this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", this->LoadCalibrationSettingsFileButton->GetWidgetName());
  this->Script("grid %s -row 1 -column 1 -sticky ew -padx 2 -pady 4", this->LoadCalibrationSettingsFileButton->GetWidgetName());

}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ClearExportImportControls()
{
  this->Script("pack forget %s", this->ExportImportFrame->GetWidgetName());
  this->Script("pack forget %s", this->SaveCalibrationSettingsFileButton->GetWidgetName());
  this->Script("pack forget %s", this->LoadCalibrationSettingsFileButton->GetWidgetName());
  this->Script("pack forget %s", this->ResetCalibrationButton->GetWidgetName());
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

  this->ShowLoadVolumeControls();
  this->ShowLoadResetControls();  
  this->ShowFiducialSegmentParamsControls();
  this->ShowFiducialSegmentationResultsControls();
  this->ShowExportImportControls();

  this->ShowAxesIn3DView(true);
  this->ShowMarkerVolumesIn3DView(true);

  this->AddGUIObservers();

  // Activate additional details for entering this step
  //this->Enter();
}
//-------------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::HideUserInterface()
{
  // this method is never called
  this->Superclass::HideUserInterface();

  //this->Exit();
}
//----------------------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::PopulateCalibrationResults()
{
  std::ostrstream os;  
  
  const TRProstateBiopsyCalibrationData calibData=this->GetGUI()->GetMRMLNode()->GetCalibrationData();

  if (calibData.CalibrationValid)
    {
    os << "Calibration results:"<<std::endl;
    
    os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(1);
    os << "  Axes angle: "<<calibData.AxesAngleDegrees<<" deg"<<std::endl;  
    os << "  Axes distance: "<<calibData.AxesDistance<<" mm"<<std::endl;  
    os << "  Initial rotation angle: "<<calibData.RobotRegistrationAngleDegrees<<" deg"<<std::endl;  
    os << "Segmentation results:";
    os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(2);
    for (int i=0; i<CALIB_MARKER_COUNT; i++)
      {
      double r, a, s;
      this->GetGUI()->GetMRMLNode()->GetCalibrationMarker(i, r, a, s);
      os << std::endl << "  Marker "<<i+1<<": R="<<r<<" A="<<a<<" S="<<s;
      }
    }
  else
    {
    os << "Not calibrated.";
    }

  os << std::ends;
  this->CalibrationResultsBox->GetWidget()->SetText(os.str());
  os.rdbuf()->freeze();
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::AddGUIObservers()
{
  this->RemoveGUIObservers();
  this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
  this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
  this->ResetCalibrationButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
  this->ResegmentButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
  this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::RemoveGUIObservers()
{
  this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
  this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
  this->ResetCalibrationButton->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
  this->ResegmentButton->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand);
  this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->WizardGUICallbackCommand );
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
void vtkTRProstateBiopsyCalibrationStep::UpdateMRML() 
{
  /*vtkMRMLChangeTrackerNode* node = this->GetGUI()->GetNode();
  if (!node) 
    return; 

  if (this->VolumeMenuButton && this->VolumeMenuButton->GetSelected() ) 
  {
    node->SetScan1_Ref(this->VolumeMenuButton->GetSelected()->GetID());
    vtkMRMLVolumeNode *VolNode = vtkMRMLVolumeNode::SafeDownCast(this->VolumeMenuButton->GetSelected());

    if (!VolNode && !VolNode->GetStorageNode() && !VolNode->GetStorageNode()->GetFileName()) 
      return;

    if(!node->GetWorkingDir())
      {
      vtkSlicerApplication *application   = vtkSlicerApplication::SafeDownCast(this->GetGUI()->GetApplication());
      std::string FilePath = application->GetTemporaryDirectory();
      node->SetWorkingDir(FilePath.c_str());
      }
  }

  if (this->SecondVolumeMenuButton && this->SecondVolumeMenuButton->GetSelected() ) {
    node->SetScan2_Ref(this->SecondVolumeMenuButton->GetSelected()->GetID());
  } 
*/
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::UpdateGUI() 
{
  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if (!mrmlNode)
  {
    return;
  }

  PopulateCalibrationResults();

  /*
  vtkMRMLChangeTrackerNode* n = this->GetGUI()->GetNode();
  if (!n) {
    this->GetGUI()->UpdateNode();
    n = this->GetGUI()->GetNode();
  }

  if (n != NULL &&  this->VolumeMenuButton)
  {
    
    vtkSlicerApplicationGUI *applicationGUI = this->GetGUI()->GetApplicationGUI();
    this->VolumeMenuButton->SetSelected(applicationGUI->GetMRMLScene()->GetNodeByID(n->GetScan1_Ref()));
  }
  if (n != NULL &&  this->SecondVolumeMenuButton)
  {
    vtkSlicerApplicationGUI *applicationGUI = this->GetGUI()->GetApplicationGUI();
    this->SecondVolumeMenuButton->SetSelected(applicationGUI->GetMRMLScene()->GetNodeByID(n->GetScan2_Ref()));
  }
  */
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
    this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("TRProstateOpenPathCalibVol");
    const char *fileName = this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->GetFileName();    
    if ( fileName ) 
      {
      this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->SaveLastPathToRegistry("TRProstateOpenPathCalibVol");
      // call the callback function
      this->LoadCalibrationVolumeButtonCallback(fileName);    
      } 
    }

  // load calib dialog button
  if (this->LoadCalibrationSettingsFileButton && this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("TRProstateOpenPathCalibSettins");
    const char *fileName = this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      this->LoadCalibrationSettingsFileButton->GetLoadSaveDialog()->SaveLastPathToRegistry("TRProstateOpenPathCalibSettins");
      // call the callback function
      this->LoadCalibrationSettingsFileButtonCallback(fileName);    
      } 
    // reset the file browse button text
    this->LoadCalibrationSettingsFileButton->SetText ("Load calibration");
    }

  // save calib dialog button
  if (this->SaveCalibrationSettingsFileButton && this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("TRProstateOpenPathCalibSettins");
    const char *fileName = this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {      
      this->SaveCalibrationSettingsFileButton->GetLoadSaveDialog()->SaveLastPathToRegistry("TRProstateOpenPathCalibSettins");     
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
    this->Resegment();
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

  vtkMRMLScalarVolumeNode *volumeNode = this->GetGUI()->GetLogic()->AddVolumeToScene(app,fileString.c_str(), VOL_CALIBRATION);
        
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
  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  mrmlNode->RemoveAllCalibrationMarkers();

  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->RemoveViewProp(this->Axes1Actor);
  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->RemoveViewProp(this->Axes2Actor);
  
  const TRProstateBiopsyCalibrationData calibData=this->GetGUI()->GetMRMLNode()->GetCalibrationData();
  TRProstateBiopsyCalibrationData calibdataInvalidated=calibData;
  calibdataInvalidated.CalibrationValid=false;
  this->GetGUI()->GetMRMLNode()->SetCalibrationData(calibdataInvalidated);

  //this->PopulateCalibrationResults();
  this->AllMarkersAcquired = false;
  this->ClickNumber = 0;

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->SetErrorText("");
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
  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
  {
    vtkErrorMacro("RecordClick: node is invalid");
    return;
  }

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  std::ostrstream os;
  os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(1);
  os << "Clicked marker "<<this->ClickNumber<<" at "<<rasPoint[0]<<" "<<rasPoint[1]<<" "<<rasPoint[2]<< std::ends;
  wizard_widget->SetErrorText(os.str());
  os.rdbuf()->freeze();
  wizard_widget->Update();

  if (this->ClickNumber>0 && this->ClickNumber<=CALIB_MARKER_COUNT)    {
    mrmlNode->SetCalibrationMarker(this->ClickNumber-1, rasPoint);
    }
  if (this->ClickNumber == CALIB_MARKER_COUNT)
    {
    this->ClickNumber = 0;
    this->AllMarkersAcquired = true;
    this->Resegment();    
    }


}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::Resegment()
{
  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
  {
    vtkErrorMacro("Resegment: node is invalid");
    return;
  }

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  std::ostrstream os;

  if (!this->AllMarkersAcquired)
  {
    os << "Please define all calibration markers." << std::ends;
    wizard_widget->SetErrorText(os.str());
    os.rdbuf()->freeze();
    wizard_widget->Update();
    return;
  }

  os << "Starting segmentation..." << std::ends;
  wizard_widget->SetErrorText(os.str());
  os.rdbuf()->freeze();
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
  bool found[4]={false, false, false, false};
  std::string calibResultDetails;
  bool calibResult=this->GetGUI()->GetLogic()->SegmentRegisterMarkers(thresh, fidDims, radius, bUseRadius, initialAngle, calibResultDetails);
  wizard_widget->SetErrorText(calibResultDetails.c_str());
  wizard_widget->Update();

  vtkMRMLVolumeNode* volumeNode = mrmlNode->GetCalibrationVolumeNode();
  int i=0;
  for (std::vector<CalibPointRenderer>::iterator it=CalibPointPreProcRendererList.begin(); it!=CalibPointPreProcRendererList.end(); ++it, ++i)
  {
      it->Update(this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer(), volumeNode, this->GetGUI()->GetLogic()->GetCalibMarkerPreProcOutput(i));
  }

  ShowMarkerVolumesIn3DView(true);

  if (calibResult==true)
  {
    // calibration successful  
    this->UpdateAxesIn3DView();
    this->ShowAxesIn3DView(true);
  }
  else
  {
    // calibration failed
    this->ShowAxesIn3DView(false);
  }

  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->RequestRender();

  mrmlNode->GetCalibrationFiducialsListNode()->SetAllFiducialsVisibility(true);

  // populate controls   
  //this->PopulateCalibrationResults();
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::SaveToExperimentFile(ostream &of)
{
  // reset parameters at MRML node
   vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }  


  // parameters to be saved for calibration
  // 1) FoR str
  // 2) marker centroids (all 4)
  // 3) I1, I2, v1, v2
  // 4) axes angle degrees
  // 5) axes distance mm
  // 6) robot registration angle
  
  // FoR str (this information is in mrml scalar volume node as metadata)
  // frame of reference uid
  vtkMRMLScalarVolumeNode *volNode = mrmlNode->GetCalibrationVolumeNode();
  if (!volNode)
    {
    // error
    return;
    }
  const itk::MetaDataDictionary &volDictionary = volNode->GetMetaDataDictionary();
  std::string tagValue;
  tagValue.clear();
  itk::ExposeMetaData<std::string>( volDictionary, "0020|0052", tagValue );
  of << " CalibrationVolumeFoRString=\"" << tagValue << "\" \n";

  // marker centroids
  for (int i=0; i<CALIB_MARKER_COUNT; i++)
  {
    double markerRAS[3];
    mrmlNode->GetCalibrationMarker(i, markerRAS[0], markerRAS[1], markerRAS[2]);
    of << " Marker_"<<i<<"=\""<<markerRAS[0]<<" "<<markerRAS[1]<<" "<<markerRAS[2]<<"\" \n";
  }

  const TRProstateBiopsyCalibrationData calibData=this->GetGUI()->GetMRMLNode()->GetCalibrationData();

  // I1, I2, v1, v2
  of << " I1=\""<<calibData.I1[0]<<" "<<calibData.I1[1]<<" "<<calibData.I1[2]<<"\" \n";
  of << " I2=\""<<calibData.I2[0]<<" "<<calibData.I2[1]<<" "<<calibData.I2[2]<<"\" \n";
  of << " v1=\""<<calibData.v1[0]<<" "<<calibData.v1[1]<<" "<<calibData.v1[2]<<"\" \n";
  of << " v2=\""<<calibData.v2[0]<<" "<<calibData.v2[1]<<" "<<calibData.v2[2]<<"\" \n";

  of << " AxesAngle (degrees)=\"" << calibData.AxesAngleDegrees << "\" \n";
  of << " AxesDistance (mm)=\"" << calibData.AxesDistance <<"\" \n";
  of << " RobotRegistrationAngle (degrees)=\"" << calibData.RobotRegistrationAngleDegrees<<"\" \n";
  
}
//----------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::LoadFromExperimentFile(istream &file)
{
}
//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::UpdateAxesIn3DView()
{
  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;
  
  // get RAS points of start and end point of axis 1, and axis 2
  // for the 3D viewer, the RAS coodinates are the world coordinates!!
  // this makes things simpler
  // render the needle as a thin pipe

  // first axes line  
  // start point is marker 1 centroid
  double marker1RAS[3];
  mrmlNode->GetCalibrationMarker(0, marker1RAS[0], marker1RAS[1], marker1RAS[2]);

  // end point is marker 2 centroid
  double marker2RAS[3];
  mrmlNode->GetCalibrationMarker(1, marker2RAS[0], marker2RAS[1], marker2RAS[2]);

  // form the axis 1 line
  // set up the line actors
  vtkSmartPointer<vtkLineSource> axis1Line = vtkSmartPointer<vtkLineSource>::New();  
  axis1Line->SetResolution(100); 
  axis1Line->SetPoint1(marker1RAS);
  axis1Line->SetPoint2(marker2RAS);
  axis1Line->Update();
      
  vtkSmartPointer<vtkPolyDataMapper> axis1Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();  
  axis1Mapper->SetInputConnection(axis1Line->GetOutputPort());
  this->Axes1Actor->SetMapper(axis1Mapper);  
  this->Axes1Actor->SetVisibility(true);
  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->AddViewProp(this->Axes1Actor);


  // 2nd axis line
  // start point as marker 4 coordinate
  double marker4RAS[3];
  mrmlNode->GetCalibrationMarker(3, marker4RAS[0], marker4RAS[1], marker4RAS[2]);

  // end point as overshot marker 3 coordinate
  double marker3RAS[3];
  mrmlNode->GetCalibrationMarker(2, marker3RAS[0], marker3RAS[1], marker3RAS[2]);

  double axis2Vector[3];
  axis2Vector[0] = marker3RAS[0] - marker4RAS[0];
  axis2Vector[1] = marker3RAS[1] - marker4RAS[1];
  axis2Vector[2] = marker3RAS[2] - marker4RAS[2];
  vtkMath::Normalize(axis2Vector);

  double overshoot = 100;

  double axis2EndRAS[3];
  axis2EndRAS[0] = marker3RAS[0] + overshoot*axis2Vector[0];
  axis2EndRAS[1] = marker3RAS[1] + overshoot*axis2Vector[1];
  axis2EndRAS[2] = marker3RAS[2] + overshoot*axis2Vector[2];
  
  // form the axis 2 line
  // set up the line actors
  vtkSmartPointer<vtkLineSource> axis2Line = vtkSmartPointer<vtkLineSource>::New();  
  axis2Line->SetResolution(100); 
  axis2Line->SetPoint1(axis2EndRAS);
  axis2Line->SetPoint2(marker4RAS);
  axis2Line->Update();
      
  vtkSmartPointer<vtkPolyDataMapper> axis2Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();  
  axis2Mapper->SetInputConnection(axis2Line->GetOutputPort());
  this->Axes2Actor->SetMapper(axis2Mapper);  
  this->Axes2Actor->SetVisibility(true);
  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->AddViewProp(this->Axes2Actor);

  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->Render();
}

//--------------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::ShowAxesIn3DView(bool show)
{
  this->Axes1Actor->SetVisibility(show);
  this->Axes2Actor->SetVisibility(show);
  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->Render();
}
//-------------------------------------------------------------------------------
void vtkTRProstateBiopsyCalibrationStep::Validate()
{
  this->Superclass::Validate();

  this->ShowAxesIn3DView(false);
  this->ShowMarkerVolumesIn3DView(false);
}

 
CalibPointRenderer::CalibPointRenderer()
{
  this->Renderer = NULL;
  this->Render_Image = NULL;
  this->Render_Mapper = NULL;
  this->Render_VolumeProperty = NULL;
  this->Render_Volume = NULL;
}

CalibPointRenderer::~CalibPointRenderer()
{
  if (this->Renderer!=NULL && this->Render_Volume!=NULL)
  {
    if (this->Render_Volume->GetReferenceCount()>1)
    {
      // the object is still in the renderer, remove it now
      this->Renderer->RemoveViewProp(this->Render_Volume);
    }
    this->Renderer=NULL;
  }
  if (this->Render_Volume) 
  {
    this->Render_Volume->Delete();
    this->Render_Volume = NULL; 
  }
  if (this->Render_Mapper) 
  {
    this->Render_Mapper->Delete();
    this->Render_Mapper = NULL;
  }
  if (this->Render_VolumeProperty) 
  {
    this->Render_VolumeProperty->Delete();
    this->Render_VolumeProperty = NULL;
  }
  this->Render_Image = NULL;
}

vtkVolume* CalibPointRenderer::GetVolume() 
{
  return this->Render_Volume;
}

void CalibPointRenderer::Update(vtkKWRenderWidget *renderer, vtkMRMLVolumeNode *volumeNode, vtkImageData *imagedata)
{ 
  if (this->Render_Volume==NULL)
  {
    this->Render_Volume = vtkVolume::New();

    //this->Render_Mapper = vtkFixedPointVolumeRayCastMapper::New(); // it may be needed if the software is used on an ancient graphics card
    this->Render_Mapper = vtkVolumeTextureMapper3D::New();

    this->Render_Volume->SetMapper(this->Render_Mapper);

    this->Render_VolumeProperty = vtkVolumeProperty::New();
    this->Render_VolumeProperty->SetShade(1);
    this->Render_VolumeProperty->SetAmbient(0.3);
    this->Render_VolumeProperty->SetDiffuse(0.6);
    this->Render_VolumeProperty->SetSpecular(0.5);
    this->Render_VolumeProperty->SetSpecularPower(40.0);
    this->Render_VolumeProperty->SetInterpolationTypeToLinear();
    this->Render_VolumeProperty->ShadeOn();
    
    this->Render_Volume->SetProperty(this->Render_VolumeProperty);
  }

  if (renderer!=this->Renderer)
  {
    if (this->Renderer!=0)
    {
       this->Renderer->RemoveViewProp(this->Render_Volume);
       this->Renderer=0;
    }
    if (renderer!=0)
    {
      // assigne to a different renderer
      int rfbef=this->Render_Volume->GetReferenceCount();
      renderer->AddViewProp(this->Render_Volume);    
      int rfaft=this->Render_Volume->GetReferenceCount();
      this->Renderer=renderer;
    }
  }

  if (volumeNode!=0 && imagedata!=0)
  {
    double* imgIntensityRange = imagedata->GetPointData()->GetScalars()->GetRange();
    double imgIntensityUnit = (imgIntensityRange[1]-imgIntensityRange[0])* 0.01; // 1 unit is 1 percent of the intensity range    

    double opacity=0.2;
    vtkSmartPointer<vtkPiecewiseFunction> scalarOpacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();  
    scalarOpacityTransferFunction->AddPoint(imgIntensityRange[0], 0.0);
    scalarOpacityTransferFunction->AddPoint(imgIntensityRange[0]+1*imgIntensityUnit, 0.0);
    scalarOpacityTransferFunction->AddPoint(imgIntensityRange[0]+5*imgIntensityUnit, opacity);
    scalarOpacityTransferFunction->AddPoint(imgIntensityRange[1], opacity);

    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    colorTransferFunction->AddRGBPoint(imgIntensityRange[0], 0.8, 0.8, 0);
    colorTransferFunction->AddRGBPoint(imgIntensityRange[1], 0.8, 0.8, 0);

    this->Render_VolumeProperty->SetScalarOpacity(scalarOpacityTransferFunction);
    this->Render_VolumeProperty->SetColor(colorTransferFunction);
   
    vtkSmartPointer<vtkMatrix4x4> orientationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();  
    volumeNode->GetIJKToRASMatrix(orientationMatrix);
    this->Render_Volume->PokeMatrix(orientationMatrix);

    this->Render_Image = imagedata;
    this->Render_Mapper->SetInput(this->Render_Image);
  }
}

void vtkTRProstateBiopsyCalibrationStep::ShowMarkerVolumesIn3DView(bool show)
{
  for (std::vector<CalibPointRenderer>::iterator it=CalibPointPreProcRendererList.begin(); it!=CalibPointPreProcRendererList.end(); ++it)
  {
    vtkVolume *vol=it->GetVolume();
    if (vol!=0)
    {
      vol->SetVisibility(show);
    }
  }
  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->RequestRender();
}
