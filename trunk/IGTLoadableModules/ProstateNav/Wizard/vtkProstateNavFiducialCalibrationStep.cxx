#include "vtkProstateNavFiducialCalibrationStep.h"

#include "vtkProstateNavGUI.h"
#include "vtkProstatenavLogic.h"
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
#include "vtkAppendPolyData.h"
#include "vtkActor.h"
#include "vtkSphereSource.h"
#include "vtkGlyph3D.h"
#include "vtkPoints.h"

#include "vtkMRMLTransRectalProstateRobotNode.h"

#include <vtksys/ios/sstream>

static const char MARKER_LABEL_NAMES[CALIB_MARKER_COUNT][9]={ "Marker 1", "Marker 2", "Marker 3", "Marker 4"};

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkProstateNavFiducialCalibrationStep);
vtkCxxRevisionMacro(vtkProstateNavFiducialCalibrationStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkProstateNavFiducialCalibrationStep::vtkProstateNavFiducialCalibrationStep()
{
  this->SetTitle("Calibration");
  this->SetDescription("Select a calibration volume and click on the calibration markers in the image.");

  this->CalibrationPointListNode=NULL;

  this->LoadVolumeDialogFrame=vtkSmartPointer<vtkKWFrame>::New();
  this->LoadCalibrationVolumeButton=vtkSmartPointer<vtkKWLoadSaveButton>::New();
  this->ResetCalibrationButton=vtkSmartPointer<vtkKWPushButton>::New();
  this->ResegmentButton=vtkSmartPointer<vtkKWPushButton>::New();
  this->EditMarkerPositionButton=vtkSmartPointer<vtkKWCheckButton>::New();  
  this->LoadResetFrame=vtkSmartPointer<vtkKWFrame>::New();
  this->ExportImportFrame=vtkSmartPointer<vtkKWFrame>::New();
  this->FiducialPropertiesFrame=vtkSmartPointer<vtkKWFrameWithLabel>::New();
  this->FiducialSizeLabel=vtkSmartPointer<vtkKWLabel>::New();
  this->FiducialWidthSpinBox=vtkSmartPointer<vtkKWSpinBoxWithLabel>::New();
  this->FiducialHeightSpinBox=vtkSmartPointer<vtkKWSpinBoxWithLabel>::New();
  this->FiducialDepthSpinBox=vtkSmartPointer<vtkKWSpinBoxWithLabel>::New();  
  for (int i = 0; i < CALIB_MARKER_COUNT; i++)
    {
    this->FiducialThresholdScale[i]=vtkSmartPointer<vtkKWScaleWithEntry>::New();
    this->JumpToFiducialButton[i]=vtkSmartPointer<vtkKWPushButton>::New();
    }
  this->RadiusSpinBox=vtkSmartPointer<vtkKWSpinBoxWithLabel>::New();
  this->RadiusCheckButton=vtkSmartPointer<vtkKWCheckButton>::New();
  this->InitialAngleSpinBox=vtkSmartPointer<vtkKWSpinBoxWithLabel>::New();
  this->SegmentationResultsFrame=vtkSmartPointer<vtkKWFrameWithLabel>::New();
  this->CalibrationResultsBox=vtkSmartPointer<vtkKWTextWithScrollbars>::New();
  this->Axes1Actor=vtkSmartPointer<vtkActor>::New();
  this->Axes2Actor=vtkSmartPointer<vtkActor>::New();

  this->AxesCenterPointsActor=vtkSmartPointer<vtkActor>::New();

  this->ProcessingCallback = false;

  CalibPointPreProcRendererList.resize(CALIB_MARKER_COUNT);

  this->ObservedRobot=NULL;
}

//----------------------------------------------------------------------------
vtkProstateNavFiducialCalibrationStep::~vtkProstateNavFiducialCalibrationStep()
{
}

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::ShowLoadResetControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
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
void vtkProstateNavFiducialCalibrationStep::ShowLoadVolumeControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
   
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
void vtkProstateNavFiducialCalibrationStep::ShowFiducialSegmentParamsControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  if (!this->FiducialPropertiesFrame->IsCreated())
    {
    this->FiducialPropertiesFrame->SetParent(parent);
    this->FiducialPropertiesFrame->Create();
    this->FiducialPropertiesFrame->SetLabelText("Fiducial properties");
    this->FiducialPropertiesFrame->CollapseFrame();    
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", this->FiducialPropertiesFrame->GetWidgetName());

  if (!this->FiducialSizeLabel->IsCreated())
    {
    this->FiducialSizeLabel->SetParent(this->FiducialPropertiesFrame->GetFrame());
    this->FiducialSizeLabel->Create();
    this->FiducialSizeLabel->SetText("Fiducial size:");
    }
  this->Script("grid %s -row 0 -column 0 -sticky ew -padx 2 -pady 2", this->FiducialSizeLabel->GetWidgetName());

  if (!this->FiducialWidthSpinBox->IsCreated())
    {
    this->FiducialWidthSpinBox->SetParent(this->FiducialPropertiesFrame->GetFrame());
    this->FiducialWidthSpinBox->Create();
    this->FiducialWidthSpinBox->GetWidget()->SetWidth(11);
    this->FiducialWidthSpinBox->GetWidget()->SetRange(0,100);
    this->FiducialWidthSpinBox->GetWidget()->SetIncrement(1);
    this->FiducialWidthSpinBox->GetWidget()->SetValue(8);
    this->FiducialWidthSpinBox->SetLabelText("width");
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
    this->FiducialHeightSpinBox->SetLabelText("height");
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
    this->FiducialDepthSpinBox->SetLabelText("depth");
    //this->FiducialDepthSpinBox->GetWidget()->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, this->WizardGUICallbackCommand);
    }
  /*this->Script("pack %s %s %s -side top -anchor w -padx 2 -pady 2", 
               this->FiducialWidthSpinBox->GetWidgetName(),
               this->FiducialHeightSpinBox->GetWidgetName(),
               this->FiducialDepthSpinBox->GetWidgetName());
               */

  this->Script("grid %s -row 0 -column 1 -sticky ew -padx 2 -pady 2", this->FiducialWidthSpinBox->GetWidgetName());
  this->Script("grid %s -row 0 -column 2 -sticky ew -padx 2 -pady 2", this->FiducialHeightSpinBox->GetWidgetName());
  this->Script("grid %s -row 0 -column 3 -sticky ew -padx 2 -pady 2", this->FiducialDepthSpinBox->GetWidgetName());

  if (!this->RadiusSpinBox->IsCreated())
    {
    this->RadiusSpinBox->SetParent(this->FiducialPropertiesFrame->GetFrame());
    this->RadiusSpinBox->Create();
    this->RadiusSpinBox->GetWidget()->SetWidth(11);
    this->RadiusSpinBox->GetWidget()->SetRange(1,10);
    this->RadiusSpinBox->GetWidget()->SetIncrement(0.1);
    this->RadiusSpinBox->GetWidget()->SetValue(3.5);
    this->RadiusSpinBox->SetLabelText("Fiducial radius (mm)");
    //this->RadiusSpinBox->GetWidget()->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, this->WizardGUICallbackCommand);
    }
  //this->Script("pack %s -side top -anchor w -padx 2 -pady 2", this->RadiusSpinBox->GetWidgetName());
  this->Script("grid %s -row 1 -column 0 -columnspan 2 -padx 2 -pady 2", this->RadiusSpinBox->GetWidgetName());

  for (int i = 0; i < CALIB_MARKER_COUNT; i++)
    {
      {
        std::ostrstream ospack;
        ospack << "grid";
        if (!this->JumpToFiducialButton[i]->IsCreated())
        {  
          this->JumpToFiducialButton[i]->SetParent(this->FiducialPropertiesFrame->GetFrame());
          this->JumpToFiducialButton[i]->Create();
          std::ostrstream os;
          os << "Marker "<< i+1 << std::ends;    
          this->JumpToFiducialButton[i]->SetText(os.str());
          os.rdbuf()->freeze();
        }
        ospack << " " << this->JumpToFiducialButton[i]->GetWidgetName();
        ospack << " -row "<<2+i<<" -column 0 -padx 2 -pady 2" << std::ends;
        this->Script(ospack.str());
        ospack.rdbuf()->freeze();
        ospack.clear();
      }

      {
        std::ostrstream ospack;
        ospack << "grid";
        if (!this->FiducialThresholdScale[i]->IsCreated())
        {  
          this->FiducialThresholdScale[i]->SetParent(this->FiducialPropertiesFrame->GetFrame());
          this->FiducialThresholdScale[i]->Create();
          std::ostrstream os;
          os << "Threshold "<< i+1 << std::ends;    
          this->FiducialThresholdScale[i]->SetLabelText(os.str());
          os.rdbuf()->freeze();
          this->FiducialThresholdScale[i]->SetWidth(3);
          this->FiducialThresholdScale[i]->SetLength(200);
          this->FiducialThresholdScale[i]->SetRange(0,100);
          this->FiducialThresholdScale[i]->SetResolution(0.5);
          this->FiducialThresholdScale[i]->SetValue(9);
          //this->FiducialThresholdScale[i]->AddObserver(vtkKWScale::ScaleValueChangingEvent, this->WizardGUICallbackCommand);            
        }
        ospack << " " << this->FiducialThresholdScale[i]->GetWidgetName();
        ospack << " -row "<<2+i<<" -column 1 -padx 2 -pady 2" << std::ends;
        this->Script(ospack.str());
        ospack.rdbuf()->freeze();
        ospack.clear();
      }
    }
}

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::ShowFiducialSegmentationResultsControls()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
   
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
    }
  this->Script("pack %s -side top -anchor nw -expand n -fill x -padx 2 -pady 6", this->CalibrationResultsBox->GetWidgetName());  

}

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::ShowExportImportControls()
{
   vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  
  // Create the frame
  if (!this->ExportImportFrame->IsCreated())
    {
    this->ExportImportFrame->SetParent(parent);
    this->ExportImportFrame->Create();
    }
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", this->ExportImportFrame->GetWidgetName());

  // create the Calibrate button
  if (!this->EditMarkerPositionButton->IsCreated())
    {
    this->EditMarkerPositionButton->SetParent(this->ExportImportFrame);
    this->EditMarkerPositionButton->Create();
    this->EditMarkerPositionButton->SetText("Edit marker positions");
    this->EditMarkerPositionButton->SetBalloonHelpString("Click to set calibration marker positions on the images");
    this->EditMarkerPositionButton->SetEnabled(1);
    }    
   this->Script("grid %s -row 0 -column 0 -sticky ew -padx 2 -pady 4", this->EditMarkerPositionButton->GetWidgetName());

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
  this->Script("grid %s -row 0 -column 1 -sticky ew -padx 2 -pady 4", this->ResegmentButton->GetWidgetName());

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
  this->Script("grid %s -row 0 -column 2 -sticky ew -padx 2 -pady 4", this->ResetCalibrationButton->GetWidgetName());

}

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  this->ShowLoadVolumeControls();
  this->ShowLoadResetControls();  
  this->ShowFiducialSegmentParamsControls();
  this->ShowFiducialSegmentationResultsControls();
  this->ShowExportImportControls();

  this->ShowAxesIn3DView(true);
  this->ShowMarkerVolumesIn3DView(true);

  vtkMRMLFiducialListNode* fidNode=vtkMRMLFiducialListNode::New();
  fidNode->SetName("CalibrationMarkers");
  SetCalibrationPointListNode(fidNode);

  UpdateMRMLObserver();

  this->AddGUIObservers();
}
//-------------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::HideUserInterface()
{
  this->Superclass::HideUserInterface();

  RemoveGUIObservers();

  ClearMarkerVolumesIn3DView();
  ClearAxesIn3DView();

  this->GetLogic()->GetApplicationLogic()->GetMRMLScene()->RemoveNode(this->CalibrationPointListNode);
  if (this->CalibrationPointListNode)
  {
    this->CalibrationPointListNode->Delete();
    this->CalibrationPointListNode=NULL;
  }
  SetCalibrationPointListNode(NULL);
  UpdateMRMLObserver(); // remove observer

}

//----------------------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::PopulateCalibrationResults()
{
  std::ostrstream os;  
  
  vtkMRMLTransRectalProstateRobotNode* robot=GetRobot();
  TRProstateBiopsyCalibrationData calibData;
  if (robot!=NULL)
  {    
    calibData=robot->GetCalibrationData();
  }

  if (robot!=NULL && calibData.CalibrationValid)
  {
    os << "Calibration results:"<<std::endl;

    os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(1);
    os << "  Axes angle: "<<calibData.AxesAngleDegrees<<" deg"<<std::endl;  
    os << "  Axes distance: "<<calibData.AxesDistance<<" mm"<<std::endl;  
    os << "  Initial rotation angle: "<<calibData.RobotRegistrationAngleDegrees<<" deg"<<std::endl;  
    os << "Segmentation results:";
    os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(2);

    int modifyOld=this->CalibrationPointListNode->StartModify();  
    for (int i=0; i<CALIB_MARKER_COUNT; i++)
    {
      double r, a, s;
      bool valid;
      robot->GetCalibrationMarker(i, r, a, s, valid);
      if (valid)
      {
        os << std::endl << "  Marker "<<i+1<<": R="<<r<<" A="<<a<<" S="<<s;
      }

      if (this->CalibrationPointListNode->GetNumberOfFiducials()<i+1)
      {    
        this->CalibrationPointListNode->AddFiducialWithLabelXYZSelectedVisibility(MARKER_LABEL_NAMES[i],r,a,s,true,true);
      }
      else
      {
        float* fidXYZ=this->CalibrationPointListNode->GetNthFiducialXYZ(i);
        if (fidXYZ[0]!=r || fidXYZ[1]!=a || fidXYZ[2]!=s)
        {
          this->CalibrationPointListNode->SetNthFiducialXYZ(i, r, a, s);
        }
      }
    }
    //this->CalibrationPointListNode->SetLocked(true);
    this->CalibrationPointListNode->EndModify(modifyOld); 
    // StartModify/EndModify discarded vtkMRMLFiducialListNode::FiducialModifiedEvent-s, so we have to resubmit them now
    this->CalibrationPointListNode->InvokeEvent(vtkMRMLFiducialListNode::FiducialModifiedEvent, NULL);
  }
  else
  {
    os << "Not calibrated.";
    //this->CalibrationPointListNode->SetAllFiducialsVisibility(false);
  }

  os << std::ends;
  this->CalibrationResultsBox->GetWidget()->SetText(os.str());
  os.rdbuf()->freeze();
}
//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::AddGUIObservers()
{
  this->RemoveGUIObservers();
  this->ResetCalibrationButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->EditMarkerPositionButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->ResegmentButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
  for (int i=0; i<CALIB_MARKER_COUNT; i++)
  {
    this->JumpToFiducialButton[i]->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  }
}
//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::RemoveGUIObservers()
{
  this->ResetCalibrationButton->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);  
  this->EditMarkerPositionButton->RemoveObservers(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->ResegmentButton->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->LoadCalibrationVolumeButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
}
//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::Enter()
{


}

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::Exit()
{

}

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::WizardGUICallback( vtkObject *caller,
                         unsigned long eid, void *clientData, void *callData )
{

  vtkProstateNavFiducialCalibrationStep *self = reinterpret_cast<vtkProstateNavFiducialCalibrationStep *>(clientData);
  
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
void vtkProstateNavFiducialCalibrationStep::UpdateMRML() 
{

}

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::UpdateMRMLObserver()
{
  if (this->GetProstateNavManager())
  {
    vtkMRMLTransRectalProstateRobotNode* robot = GetRobot();
    if ( this->ObservedRobot != robot )
    {
      vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
      events->InsertNextValue(vtkCommand::ModifiedEvent);
      this->MRMLObserverManager->SetAndObserveObjectEvents(vtkObjectPointer(&(this->ObservedRobot)),robot,events);
    }
  }

}


//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::UpdateGUI() 
{
  PopulateCalibrationResults();
} 

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::ProcessGUIEvents(vtkObject *caller,
        unsigned long event, void *callData)
{
  //slicerCerr("vtkProstateNavFiducialCalibrationStep::ProcessGUIEvents");

  vtkMRMLProstateNavManagerNode *mrmlNode = this->GetGUI()->GetProstateNavManager();

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

  // Calibrate
  if (this->EditMarkerPositionButton && this->EditMarkerPositionButton == vtkKWCheckButton::SafeDownCast(caller) && (event == vtkKWCheckButton::SelectedStateChangedEvent))
    {
      EnableMarkerPositionEdit(this->EditMarkerPositionButton->GetSelectedState() == 1);
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

  for (int i=0; i<CALIB_MARKER_COUNT; i++)
  {
    if (this->JumpToFiducialButton[i] && this->JumpToFiducialButton[i] == vtkKWPushButton::SafeDownCast(caller) && (event == vtkKWPushButton::InvokedEvent))
    {
      this->JumpToFiducial(i);
    }  
  }

}

//-----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::LoadCalibrationVolumeButtonCallback(const char *fileName)
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

  vtkMRMLScalarVolumeNode *volumeNode = this->GetGUI()->GetLogic()->AddVolumeToScene(fileString.c_str(), VOL_CALIBRATION);
        
  if (volumeNode)
    {
    this->GetGUI()->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID( volumeNode->GetID() );
    this->GetGUI()->GetApplicationLogic()->PropagateVolumeSelection();
    this->EditMarkerPositionButton->SetSelectedState(1);
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
void vtkProstateNavFiducialCalibrationStep::Reset()
{
  vtkMRMLTransRectalProstateRobotNode* robot=GetRobot();
  if (robot==NULL)
  {
    return;
  }
  robot->RemoveAllCalibrationMarkers();

  this->CalibrationPointListNode->RemoveAllFiducials();

  ClearAxesIn3DView();
  ClearMarkerVolumesIn3DView();

  const TRProstateBiopsyCalibrationData calibData=robot->GetCalibrationData();
  TRProstateBiopsyCalibrationData calibdataInvalidated=calibData;
  calibdataInvalidated.CalibrationValid=false;
  robot->SetCalibrationData(calibdataInvalidated);

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->SetErrorText("");
}
//-----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::Resegment()
{
  vtkMRMLProstateNavManagerNode *mrmlNode = this->GetGUI()->GetProstateNavManager();

  if(!mrmlNode)
  {
    vtkErrorMacro("Resegment: node is invalid");
    return;
  }

  vtkMRMLTransRectalProstateRobotNode* robot=GetRobot();
  if (robot==NULL)
  {    
    vtkErrorMacro("Resegment: robot is invalid");
    return;
  }

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  std::ostrstream os;

  if (this->CalibrationPointListNode->GetNumberOfFiducials()<CALIB_MARKER_COUNT)
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
  double thresh[CALIB_MARKER_COUNT];
  int i=0;
  for (i=0 ; i<CALIB_MARKER_COUNT; i++)
  {
    thresh[i] = this->FiducialThresholdScale[i]->GetValue();
  }
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
  bool found[CALIB_MARKER_COUNT]={false, false, false, false};
  std::string calibResultDetails;

  const char* calVolNodeID = mrmlNode->GetCalibrationVolumeNodeID();
  vtkMRMLScalarVolumeNode *calVolNode=vtkMRMLScalarVolumeNode::SafeDownCast(this->GetLogic()->GetApplicationLogic()->GetMRMLScene()->GetNodeByID(calVolNodeID));

  bool calibResult=robot->SegmentRegisterMarkers(calVolNode, thresh, fidDims, radius, bUseRadius, initialAngle, calibResultDetails);
  
  wizard_widget->SetErrorText(calibResultDetails.c_str());
  wizard_widget->Update();
 
  i=0;
  for (std::vector<CalibPointRenderer>::iterator it=CalibPointPreProcRendererList.begin(); it!=CalibPointPreProcRendererList.end(); ++it)
  {
      it->Update(this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer(), calVolNode, robot->GetCalibMarkerPreProcOutput(i));
      ++i;  
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

  //this->CalibrationPointListNode->SetAllFiducialsVisibility(false);

  UpdateGUI(); // TODO: this should be called automatically, as the robot node is changed
}

//--------------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::UpdateAxesIn3DView()
{
  vtkMRMLTransRectalProstateRobotNode* robot=GetRobot();
  if (robot==NULL)
  {
    return;
  }

  const TRProstateBiopsyCalibrationData calibData=robot->GetCalibrationData();

  // form the axis 1 line
  // set up the line actors
  vtkSmartPointer<vtkLineSource> axis1Line = vtkSmartPointer<vtkLineSource>::New();  
  axis1Line->SetResolution(100); 
  
  double needle1[3];
  double needle2[3];
  for (int i=0; i<3; i++)
  {
    needle1[i]=calibData.I2[i]-100*calibData.v2[i];
    needle2[i]=calibData.I2[i]+200*calibData.v2[i];
  }

  axis1Line->SetPoint1(needle1);
  axis1Line->SetPoint2(needle2);

  axis1Line->Update();
      
  vtkSmartPointer<vtkPolyDataMapper> axis1Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();  
  axis1Mapper->SetInputConnection(axis1Line->GetOutputPort());
  this->Axes1Actor->SetMapper(axis1Mapper);  
  this->Axes1Actor->SetVisibility(true);
  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->AddViewProp(this->Axes1Actor);

  // 2nd axis line
  /*
  // start point as marker 4 coordinate
  double marker4RAS[3];
  robot->GetCalibrationMarker(3, marker4RAS[0], marker4RAS[1], marker4RAS[2]);

  // end point as overshot marker 3 coordinate
  double marker3RAS[3];
  robot->GetCalibrationMarker(2, marker3RAS[0], marker3RAS[1], marker3RAS[2]);

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
  */
  // form the axis 2 line
  // set up the line actors
  vtkSmartPointer<vtkLineSource> axis2Line = vtkSmartPointer<vtkLineSource>::New();  
  axis2Line->SetResolution(100); 

  double robotaxis1[3];
  double robotaxis2[3];
  for (int i=0; i<3; i++)
  {
    robotaxis1[i]=calibData.I1[i]-100*calibData.v1[i];
    robotaxis2[i]=calibData.I1[i]+200*calibData.v1[i];
  }

  axis2Line->SetPoint1(robotaxis1);
  axis2Line->SetPoint2(robotaxis2);

  axis2Line->Update();
      
  vtkSmartPointer<vtkPolyDataMapper> axis2Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();  
  axis2Mapper->SetInputConnection(axis2Line->GetOutputPort());
  this->Axes2Actor->SetMapper(axis2Mapper);  
  this->Axes2Actor->SetVisibility(true);
  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->AddViewProp(this->Axes2Actor);


  vtkPoints *points = vtkPoints::New();
  robot->GetCalibrationAxisCenterpoints(points,0);
  robot->GetCalibrationAxisCenterpoints(points,1);

  vtkPolyData *pointspoly = vtkPolyData::New();
  pointspoly->SetPoints(points);
  
  vtkSphereSource *glyph = vtkSphereSource::New();
  vtkGlyph3D *glypher = vtkGlyph3D::New();
  glypher->SetInput(pointspoly);
  glypher->SetSourceConnection(glyph->GetOutputPort());
  glypher->SetScaleFactor(0.25);

  //TODO: check why it is not displayed???

  vtkPolyDataMapper *pointsMapper = vtkPolyDataMapper::New();
  pointsMapper->SetInputConnection(glypher->GetOutputPort());
  
  this->AxesCenterPointsActor->SetMapper(pointsMapper);
  
  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->AddViewProp(this->AxesCenterPointsActor);


  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->Render();
}

//--------------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::ShowAxesIn3DView(bool show)
{
  this->Axes1Actor->SetVisibility(show);
  this->Axes2Actor->SetVisibility(show);
  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->Render();
}
//--------------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::ClearAxesIn3DView()
{
  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->RemoveViewProp(this->Axes1Actor);
  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->RemoveViewProp(this->Axes2Actor);
  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->GetMainViewer()->RemoveViewProp(this->AxesCenterPointsActor);
}
//-------------------------------------------------------------------------------
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
  Reset();
}

void CalibPointRenderer::Reset()
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

void vtkProstateNavFiducialCalibrationStep::ShowMarkerVolumesIn3DView(bool show)
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

void vtkProstateNavFiducialCalibrationStep::ClearMarkerVolumesIn3DView()
{
  for (std::vector<CalibPointRenderer>::iterator it=CalibPointPreProcRendererList.begin(); it!=CalibPointPreProcRendererList.end(); ++it)
  {
    it->Reset();
  }
  this->GetGUI()->GetApplicationGUI()->GetViewerWidget()->RequestRender();
}

vtkMRMLTransRectalProstateRobotNode* vtkProstateNavFiducialCalibrationStep::GetRobot()
{
  if (this->GetGUI()==NULL)
  {
    return NULL;
  }
  if (this->GetGUI()->GetProstateNavManager()==NULL)
  {
    return NULL;
  }
  return vtkMRMLTransRectalProstateRobotNode::SafeDownCast(this->GetGUI()->GetProstateNavManager()->GetRobotNode());
}

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::SetCalibrationPointListNode(vtkMRMLFiducialListNode *node)
{
  vtkMRMLScene* scene=NULL;
  if (this->GetLogic())
  {
    if (this->GetLogic()->GetApplicationLogic())
    {
      if (this->GetLogic()->GetApplicationLogic()->GetMRMLScene())
      {
        scene=this->GetLogic()->GetApplicationLogic()->GetMRMLScene();
      }
    }
  }

  if (this->CalibrationPointListNode!=NULL && scene!=NULL)
  {
    scene->RemoveNode(this->CalibrationPointListNode);
  }
  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLFiducialListNode::DisplayModifiedEvent);
  events->InsertNextValue(vtkMRMLFiducialListNode::FiducialModifiedEvent);
  if (this->MRMLObserverManager)
  {
    this->MRMLObserverManager->SetAndObserveObjectEvents(vtkObjectPointer(&this->CalibrationPointListNode),node,events);
  }
  if (this->CalibrationPointListNode!=NULL && scene!=NULL)
  {
    scene->AddNode(this->CalibrationPointListNode);
  }  
}

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::ProcessMRMLEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{
  if (caller == this->CalibrationPointListNode)
  {
    switch (event)
    {
    case vtkCommand::ModifiedEvent: // Modified Event
    case vtkMRMLScene::NodeAddedEvent: // when a fiducial is added to the list
    case vtkMRMLFiducialListNode::FiducialModifiedEvent:
    case vtkMRMLFiducialListNode::DisplayModifiedEvent:
      UpdateCalibration();
      break;
    }
  }

  vtkMRMLTransRectalProstateRobotNode *robot = vtkMRMLTransRectalProstateRobotNode::SafeDownCast(caller);
  if (robot!=NULL && robot == GetRobot() && event == vtkCommand::ModifiedEvent)
    {
    UpdateGUI();
    }

}

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::AddMRMLObservers()
{
  
}

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::RemoveMRMLObservers()
{
  if (CalibrationPointListNode!=NULL)
  {
    vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
    events->InsertNextValue(vtkCommand::ModifiedEvent);
    events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
    events->InsertNextValue(vtkMRMLFiducialListNode::DisplayModifiedEvent);
    events->InsertNextValue(vtkMRMLFiducialListNode::FiducialModifiedEvent);
    this->MRMLObserverManager->SetAndObserveObjectEvents(vtkObjectPointer(&CalibrationPointListNode),CalibrationPointListNode,events);
  }
}

//----------------------------------------------------------------------------
void vtkProstateNavFiducialCalibrationStep::EnableMarkerPositionEdit(bool enable)
{  
  // Activate target fiducials in the Fiducial GUI
  if (this->GetLogic()==NULL)
  {
    vtkErrorMacro("Logic is invalid");
    return;
  }    

  if (enable)
  {
    // Set fiducial placement mode
    GetLogic()->SetCurrentFiducialList(this->CalibrationPointListNode);
    if (this->CalibrationPointListNode->GetNumberOfFiducials()<CALIB_MARKER_COUNT)
    {
      GetLogic()->SetMouseInteractionMode(vtkMRMLInteractionNode::Place); 
    }
    else
    {
      GetLogic()->SetMouseInteractionMode(vtkMRMLInteractionNode::ViewTransform); 
    }
    this->CalibrationPointListNode->SetLocked(false);
  }
  else
  {
    GetLogic()->SetMouseInteractionMode(vtkMRMLInteractionNode::ViewTransform);
    this->CalibrationPointListNode->SetLocked(true);
  }

}


void vtkProstateNavFiducialCalibrationStep::UpdateCalibration()
{  

  vtkMRMLTransRectalProstateRobotNode* robot= GetRobot();
  if(robot==NULL)
  {
    vtkErrorMacro("UpdateCalibration: node is invalid");
    return;
  }

  // remove extra markers
  for (int i=CALIB_MARKER_COUNT; i<this->CalibrationPointListNode->GetNumberOfFiducials(); i++)
  {
    // this is an extra marker => delete it
    this->CalibrationPointListNode->RemoveFiducial(i);
  }

  // make sure that markers have proper names and their values are set in the robot node
  int fidCount=this->CalibrationPointListNode->GetNumberOfFiducials();
  for (int i=0; i<fidCount; i++)
  {
    float* rasPoint=this->CalibrationPointListNode->GetNthFiducialXYZ(i);
    double rasPointDouble[3]={rasPoint[0], rasPoint[1], rasPoint[2]};
    robot->SetCalibrationMarker(i, rasPointDouble);
    if (strcmp(this->CalibrationPointListNode->GetNthFiducialLabelText(i),MARKER_LABEL_NAMES[i])!=0)
    {
      this->CalibrationPointListNode->SetNthFiducialLabelText(i,MARKER_LABEL_NAMES[i]);      
    }
  }

  // Update marker positioning mode (as new points are added the mode should be changed from "place new points" to "edit points"
  EnableMarkerPositionEdit(this->EditMarkerPositionButton->GetSelectedState() == 1);

  if (this->CalibrationPointListNode->GetNumberOfFiducials()<CALIB_MARKER_COUNT)
  {
    // not enough fiducials to compute result
    return;
  }

  this->Resegment();
}

void vtkProstateNavFiducialCalibrationStep::JumpToFiducial(unsigned int fid1Index)
{
  if (fid1Index>=CALIB_MARKER_COUNT)
  {
    vtkErrorMacro("Invalid fiducial id "<<fid1Index);
    return;
  }

  if (fid1Index>=this->CalibrationPointListNode->GetNumberOfFiducials())
  {
    vtkErrorMacro("Fiducial is not defined yet "<<fid1Index);
    return;
  }

  vtkMRMLTransRectalProstateRobotNode* robot=GetRobot();
  if(robot==NULL)
  {
    vtkErrorMacro("UpdateCalibration: node is invalid");
    return;
  }

  // get the index of the fiducial pair (on the same line)
  int fid2Index=-1;
  if (fid1Index<2)
  {
    // probe fiducial
    if (fid1Index==0)
    {
      fid2Index=1;
    }
    else
    {
      fid2Index=0;
    }
  }
  else
  {
    // needle fiducial
    if (fid1Index==2)
    {
      fid2Index=3;
    }
    else
    {
      fid2Index=2;
    }
  }

  float* fid1PosFloat=this->CalibrationPointListNode->GetNthFiducialXYZ(fid1Index);
  
  double fid1Pos[4]={fid1PosFloat[0], fid1PosFloat[1], fid1PosFloat[2]};

  if (fid2Index<this->CalibrationPointListNode->GetNumberOfFiducials())
  {
    double n[4]={1,0,0,0}; // plane normal
    double t[4]={0,1,0,0}; // a transverse plane

    float* fid2PosFloat=this->CalibrationPointListNode->GetNthFiducialXYZ(fid2Index);
    double fid2Pos[3]={fid2PosFloat[0], fid2PosFloat[1], fid2PosFloat[2]};
    n[0]=fid2Pos[0]-fid1Pos[0];
    n[1]=fid2Pos[1]-fid1Pos[1];
    n[2]=fid2Pos[2]-fid1Pos[2];

    // transverse plane will be an arbitrary plane that is perpendicular to n
    double point1[4]={1,0,0,1}; // Any vector
    if (fabs(n[0])>fabs(n[1]))
    {
      // to make sure that it is not parallel to the planeNormal_RAS vector
      point1[0]=0;
      point1[1]=1;
      point1[2]=0;
    }
    // planeX_RAS is perpendicular to planeNormal_RAS
    vtkMath::Cross(n, point1, t);
    vtkMath::Normalize(t);

    this->GetGUI()->BringMarkerToViewIn2DViews(fid1Pos,n,t);
  }
  else
  {
    this->GetGUI()->BringMarkerToViewIn2DViews(fid1Pos);
  }
 
}
