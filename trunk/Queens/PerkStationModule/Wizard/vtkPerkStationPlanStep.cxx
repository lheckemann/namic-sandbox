#include "vtkPerkStationPlanStep.h"

#include "vtkPerkStationModuleGUI.h"
#include "vtkPerkStationModuleLogic.h"
#include "vtkPerkStationSecondaryMonitor.h"
#include "vtkMRMLFiducialListNode.h"

#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWEntrySet.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"

#include "vtkCylinderSource.h"
#include "vtkRenderer.h"
#include "vtkIdentityTransform.h"
#include "vtkMatrixToHomogeneousTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkMath.h"
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationPlanStep);
vtkCxxRevisionMacro(vtkPerkStationPlanStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkPerkStationPlanStep::vtkPerkStationPlanStep()
{
  this->SetName("2/5. Plan");
  this->SetDescription("Plan the needle insertion");

  this->EntryPointFrame = NULL;
  this->EntryPointLabel = NULL;
  this->EntryPoint = NULL;
  this->TargetPointFrame = NULL;
  this->TargetPointLabel = NULL;
  this->TargetPoint = NULL;
  this->InsertionAngle = NULL;
  this->InsertionDepth = NULL;
  this->WCEntryPoint[0] = 0.0;
  this->WCEntryPoint[1] = 0.0;
  this->WCEntryPoint[2] = 0.0;
  this->WCTargetPoint[0] = 0.0;
  this->WCTargetPoint[1] = 0.0;
  this->WCTargetPoint[2] = 0.0;
  this->NeedleActor = NULL;
}

//----------------------------------------------------------------------------
vtkPerkStationPlanStep::~vtkPerkStationPlanStep()
{
  if (this->EntryPointFrame)
    {
    this->EntryPointFrame->Delete();
    this->EntryPointFrame = NULL;
    }
  if (this->EntryPointLabel)
    {
    this->EntryPointLabel->Delete();
    this->EntryPointLabel = NULL;
    }

  if (this->EntryPoint)
    {
    this->EntryPoint->DeleteAllWidgets();
    this->EntryPoint = NULL;
    }

  if (this->TargetPointFrame)
    {
    this->TargetPointFrame->Delete();
    this->TargetPointFrame = NULL;
    }
  if (this->TargetPointLabel)
    {
    this->TargetPointLabel->Delete();
    this->TargetPointLabel = NULL;
    }
  if (this->TargetPoint)
    {
    this->TargetPoint->DeleteAllWidgets();
    this->TargetPoint = NULL;
    }

  if (this->InsertionAngle)
    {
    this->InsertionAngle->Delete();
    this->InsertionAngle = NULL;
    }

  if (this->InsertionDepth)
    {
    this->InsertionDepth->Delete();
    this->InsertionDepth = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();
  switch (this->GetGUI()->GetMode())      
    {

    case vtkPerkStationModuleGUI::ModeId::Training:

      this->SetName("2/5. Plan");
      break;

    case vtkPerkStationModuleGUI::ModeId::Clinical:
       
      // in clinical mode
      this->SetName("2/4. Plan");
      break;
    }
  
  this->SetDescription("Plan the needle insertion");

  /*vtkPerkStationtMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  
  if (!mrmlManager || !wizard_widget)
    {
    return;
    }*/

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);
  vtkKWWidget *parent = wizard_widget->GetClientArea();
  int enabled = parent->GetEnabled();

  // Create the individual components

  //frame
  if (!this->EntryPointFrame)
    {
    this->EntryPointFrame = vtkKWFrame::New();
    }
  if (!this->EntryPointFrame->IsCreated())
    {
    this->EntryPointFrame->SetParent(parent);
    this->EntryPointFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->EntryPointFrame->GetWidgetName());

  // label.
  if (!this->EntryPointLabel)
    { 
    this->EntryPointLabel = vtkKWLabel::New();
    }
  if (!this->EntryPointLabel->IsCreated())
    {
    this->EntryPointLabel->SetParent(this->EntryPointFrame);
    this->EntryPointLabel->Create();
    this->EntryPointLabel->SetText("Entry point:   ");
    this->EntryPointLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    } 
  
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->EntryPointLabel->GetWidgetName());

  // Entry point: will get populated in a callback, which listens for mouse clicks on image
 
  if (!this->EntryPoint)
    {
    this->EntryPoint =  vtkKWEntrySet::New();   
    }
  if (!this->EntryPoint->IsCreated())
    {
    this->EntryPoint->SetParent(this->EntryPointFrame);
    this->EntryPoint->Create();
    this->EntryPoint->SetBorderWidth(2);
    this->EntryPoint->SetReliefToGroove();
    this->EntryPoint->SetPackHorizontally(1);
    this->EntryPoint->SetMaximumNumberOfWidgetsInPackingDirection(3);
    // two entries of image spacing (x, y)
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->EntryPoint->AddWidget(id);
      entry->SetWidth(7);
      //entry->ReadOnlyOn();      
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->EntryPoint->GetWidgetName());

  //frame
  if (!this->TargetPointFrame)
    {
    this->TargetPointFrame = vtkKWFrame::New();
    }
  if (!this->TargetPointFrame->IsCreated())
    {
    this->TargetPointFrame->SetParent(parent);
    this->TargetPointFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                this->TargetPointFrame->GetWidgetName());

  // label
  if (!this->TargetPointLabel)
    { 
    this->TargetPointLabel = vtkKWLabel::New();
    }
  if (!this->TargetPointLabel->IsCreated())
    {
    this->TargetPointLabel->SetParent(this->TargetPointFrame);
    this->TargetPointLabel->Create();
    this->TargetPointLabel->SetText("Target point: ");
    this->TargetPointLabel->SetBackgroundColor(0.7, 0.7, 0.7);
    }
  
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->TargetPointLabel->GetWidgetName());
 
  // Target point: will get populated in a callback, which listens for mouse clicks on image
 
  if (!this->TargetPoint)
    {
    this->TargetPoint =  vtkKWEntrySet::New();  
    }
  if (!this->TargetPoint->IsCreated())
    {
    this->TargetPoint->SetParent(this->TargetPointFrame);
    this->TargetPoint->Create();
    this->TargetPoint->SetBorderWidth(2);
    this->TargetPoint->SetReliefToGroove();
    this->TargetPoint->SetPackHorizontally(1);
    this->TargetPoint->SetMaximumNumberOfWidgetsInPackingDirection(3);
    // two entries of image spacing (x, y)
    for (int id = 0; id < 3; id++)
      {
      vtkKWEntry *entry = this->TargetPoint->AddWidget(id);
      entry->SetWidth(7);
      //entry->ReadOnlyOn();      
      }
    }

  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                this->TargetPoint->GetWidgetName());


  // insertion angle  
  if (!this->InsertionAngle)
    {
    this->InsertionAngle =  vtkKWEntryWithLabel::New(); 
    }
  if (!this->InsertionAngle->IsCreated())
    {
    this->InsertionAngle->SetParent(parent);
    this->InsertionAngle->Create();
    this->InsertionAngle->GetWidget()->SetRestrictValueToDouble();
    this->InsertionAngle->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->InsertionAngle->SetLabelText("Insertion angle (in degrees):");
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->InsertionAngle->GetWidgetName());


  // insertion depth  
  if (!this->InsertionDepth)
    {
    this->InsertionDepth =  vtkKWEntryWithLabel::New(); 
    }
  if (!this->InsertionDepth->IsCreated())
    {
    this->InsertionDepth->SetParent(parent);
    this->InsertionDepth->Create();
    this->InsertionDepth->GetWidget()->SetRestrictValueToDouble();
    this->InsertionDepth->GetLabel()->SetBackgroundColor(0.7, 0.7, 0.7);
    this->InsertionDepth->SetLabelText("Insertion depth (in mm):   ");
    }

  this->Script("pack %s -side top -anchor nw -padx 2 -pady 2", 
                this->InsertionDepth->GetWidgetName());

  // TO DO: install callbacks
  this->InstallCallbacks();

  // TO DO: populate controls wherever needed
  this->PopulateControls();


  wizard_widget->SetErrorText(
    "Please note that the order of the clicks on image is important.");
}

//----------------------------------------------------------------------------
/*void vtkPerkStationPlanStep::PopulateIntensityImagesTargetVolumeSelector()
{  
  vtkIdType vol_id, target_vol_id;
  char buffer[256];
  bool found = false;

  vtkPerkStationtMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (!mrmlManager)
    {
    return;
    }
  int nb_of_volumes = mrmlManager->GetVolumeNumberOfChoices();
  int nb_of_target_volumes = mrmlManager->GetTargetNumberOfSelectedVolumes();
  
  // clear the lists 
  this->IntensityImagesTargetVolumeSelector->RemoveItemsFromSourceList();
  this->IntensityImagesTargetVolumeSelector->RemoveItemsFromFinalList();

  // Update the source volume list 
  for (int index = 0; index < nb_of_volumes; index++)
    {
    vol_id = mrmlManager->GetVolumeNthID(index);
    found = false;
    for(int i = 0; i < nb_of_target_volumes; i++)
      {
      target_vol_id = mrmlManager->GetTargetSelectedVolumeNthID(i);
      if (vol_id == target_vol_id)
        {
        found = true;
        break;
        }
      }
    if (!found)
      {
      const char *name = mrmlManager->GetVolumeName(vol_id);
      if (name)
        {
        sprintf(buffer, "%s (%d)", name, static_cast<int>(vol_id));
        this->IntensityImagesTargetVolumeSelector->AddSourceElement(buffer);
        }
      }
    }

  // Update the target volume list
  for(int i = 0; i < nb_of_target_volumes; i++)
    {
    target_vol_id = mrmlManager->GetTargetSelectedVolumeNthID(i);
    const char *name = mrmlManager->GetVolumeName(target_vol_id);
    if (name)
      {
      sprintf(buffer, "%s (%d)", name, static_cast<int>(target_vol_id));
      this->IntensityImagesTargetVolumeSelector->AddFinalElement(buffer);
      }
    }
}
*/
//----------------------------------------------------------------------------
/*void vtkPerkStationPlanStep::IntensityImagesTargetSelectionChangedCallback()
{
  // nothing for now; changes are made on transitions
  // (see svn revisions)
}
*/
//----------------------------------------------------------------------------
/*void vtkPerkStationPlanStep::AlignTargetImagesCallback(int state)
{
  // The align target images checkbutton has changed because of user
  // interaction
  
  vtkPerkStationtMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  if (mrmlManager)
    {
    mrmlManager->SetEnableTargetToTargetRegistration(state);
    }
}
*/
//----------------------------------------------------------------------------
/*void vtkPerkStationPlanStep::Validate()
{/*
  vtkKWWizardWorkflow *wizard_workflow = 
    this->GetGUI()->GetWizardWidget()->GetWizardWorkflow();
  vtkPerkStationtMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  
  if (mrmlManager && mrmlManager->GetTargetInputNode() != NULL)
    {
    // decide if the number of target volumes changed
    unsigned int nb_of_parameter_target_volumes = 
      mrmlManager->GetTargetNumberOfSelectedVolumes();
    unsigned int nb_of_currently_selected_target_volumes = 
      this->IntensityImagesTargetVolumeSelector->
      GetNumberOfElementsOnFinalList();
    bool number_of_target_images_changed = 
      nb_of_parameter_target_volumes != nb_of_currently_selected_target_volumes;
    
    if (number_of_target_images_changed &&
        !vtkKWMessageDialog::PopupYesNo
        (this->GetApplication(), 
         NULL, 
         "Change the number of target images?",
         "Are you sure you want to change the number of target images?",
         vtkKWMessageDialog::WarningIcon | vtkKWMessageDialog::InvokeAtPointer))
      {
      // don't change number of volumes; stay on this step
      wizard_workflow->PushInput(vtkKWWizardStep::GetValidationFailedInput());
      wizard_workflow->ProcessInputs();
      }
    else
      {
      // record indices of currently selected volumes
      std::vector<vtkIdType> selectedIDs;
      for(unsigned int i = 0; i < nb_of_currently_selected_target_volumes; ++i) 
        {
        std::string targettext = 
          this->IntensityImagesTargetVolumeSelector->GetElementFromFinalList(i);
        std::string::size_type pos1 = targettext.rfind("(");
        std::string::size_type pos2 = targettext.rfind(")");
        if (pos1 != vtksys_stl::string::npos && pos2 != vtksys_stl::string::npos)
          {
          vtkIdType vol_id = atoi(targettext.substr(pos1+1, pos2-pos1-1).c_str());
          selectedIDs.push_back(vol_id);
          }
        }
      mrmlManager->ResetTargetSelectedVolumes(selectedIDs);
      }
    }
    
  this->Superclass::Validate();
}

*/
//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::InstallCallbacks()
{
  // callback on insertion depth
  this->InsertionDepth->GetWidget()->SetRestrictValueToDouble();
  this->InsertionDepth->GetWidget()->SetCommand(this, "InsertionDepthEntryCallback");
  this->AddCallbackCommandObserver(this->InsertionDepth->GetWidget(), vtkKWEntry::EntryValueChangedEvent);

  // callback on insertion angle
  this->InsertionAngle->GetWidget()->SetRestrictValueToDouble();
  this->InsertionAngle->GetWidget()->SetCommand(this, "InsertionAngleEntryCallback");
  this->AddCallbackCommandObserver(this->InsertionAngle->GetWidget(), vtkKWEntry::EntryValueChangedEvent);

}
//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::PopulateControls()
{

}
//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData)
{
  static unsigned int clickNum = 0;
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  
  if (!this->GetGUI()->GetMRMLNode())
    {
    // TO DO: what to do on failure
    return;
    }

  // first identify if the step is pertinent, i.e. current step of wizard workflow is actually calibration step
  if (!wizard_widget ||   wizard_widget->GetWizardWorkflow()->GetCurrentStep() != this)
    {
    return;
    }

  vtkSlicerInteractorStyle *s = vtkSlicerInteractorStyle::SafeDownCast(caller);
  vtkSlicerInteractorStyle *istyle0 = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyleSecondary = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetSecondaryMonitor()->GetRenderWindowInteractor()->GetInteractorStyle());
  
  vtkRenderWindowInteractor *rwi;
  vtkMatrix4x4 *matrix;

  if (((s == istyle0)||(s == istyleSecondary)) && (event == vtkCommand::LeftButtonPressEvent))
    {
    ++clickNum;
    // mouse click happened in the axial slice view

    if (s == istyle0)
      {
          
      vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Red");
      rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();    
      matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
      }
    else if (s == istyleSecondary)
      {   
      rwi = this->GetGUI()->GetSecondaryMonitor()->GetRenderWindowInteractor();
      matrix = this->GetGUI()->GetSecondaryMonitor()->GetXYToRAS(); 
      }

    
    int point[2];
    rwi->GetLastEventPosition(point);
    double inPt[4] = {point[0], point[1], 0, 1};
    double outPt[4];    
    matrix->MultiplyPoint(inPt, outPt); 
    double ras[3] = {outPt[0], outPt[1], outPt[2]};

    // depending on click number, it is either Entry point or target point
    if (clickNum ==1)
      {
      // entry point specification by user
      this->EntryPoint->GetWidget(0)->SetValueAsDouble(ras[0]);
      this->EntryPoint->GetWidget(1)->SetValueAsDouble(ras[1]);
      this->EntryPoint->GetWidget(2)->SetValueAsDouble(ras[2]);

      // record value in mrml node
      this->GetGUI()->GetMRMLNode()->SetPlanEntryPoint(ras);

      // record value in mrml fiducial list node      
      int index = this->GetGUI()->GetMRMLNode()->GetPlanMRMLFiducialListNode()->AddFiducialWithXYZ(ras[0], ras[1], ras[2], false);
      this->GetGUI()->GetMRMLNode()->GetPlanMRMLFiducialListNode()->SetNthFiducialLabelText(index, "Entry");
      

      }
    else if (clickNum == 2)
      {
      this->TargetPoint->GetWidget(0)->SetValueAsDouble(ras[0]);
      this->TargetPoint->GetWidget(1)->SetValueAsDouble(ras[1]);
      this->TargetPoint->GetWidget(2)->SetValueAsDouble(ras[2]);
      
      // record value in the MRML node
      this->GetGUI()->GetMRMLNode()->SetPlanTargetPoint(ras);
    
      // record value in mrml fiducial list node      
      int index = this->GetGUI()->GetMRMLNode()->GetPlanMRMLFiducialListNode()->AddFiducialWithXYZ(ras[0], ras[1], ras[2], false);
      this->GetGUI()->GetMRMLNode()->GetPlanMRMLFiducialListNode()->SetNthFiducialLabelText(index, "Target");
      
      // calculate insertion angle and insertion depth
      this->CalculatePlanInsertionAngleAndDepth();
      // record those values too in the mrml node
    
      // do an image overlay of a cylinder!!
      this->OverlayNeedleGuide();
      this->GetGUI()->GetSecondaryMonitor()->OverlayNeedleGuide();
      clickNum = 0;
      }


    }
}

//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::InsertionDepthEntryCallback(double value)
{
    // TO DO: check if its a valid input i.e. non-negative  
    
    // set the value in the mrml node
    this->GetGUI()->GetMRMLNode()->SetUserPlanInsertionDepth(value); 
    this->GetGUI()->GetMRMLNode()->CalculatePlanInsertionDepthError();
}


//----------------------------------------------------------------------------
void vtkPerkStationPlanStep::InsertionAngleEntryCallback(double value)
{
    // TO DO: check if its a valid input i.e. 0-360 
    
    // set the value in the mrml node
    this->GetGUI()->GetMRMLNode()->SetUserPlanInsertionAngle(value);  
    this->GetGUI()->GetMRMLNode()->CalculatePlanInsertionAngleError();
    
}

//------------------------------------------------------------------------------
void vtkPerkStationPlanStep::OverlayNeedleGuide()
{
  vtkRenderer *renderer = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer();

  // get the world coordinates
  int point[2];
  double worldCoordinate[4];
  vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Red");
  vtkMatrix4x4 *xyToRAS = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
  vtkMatrix4x4 *rasToXY = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert(xyToRAS, rasToXY);

  // entry point
  double rasEntry[3];
  this->GetGUI()->GetMRMLNode()->GetPlanEntryPoint(rasEntry);
  double inPt[4] = {rasEntry[0], rasEntry[1], rasEntry[2], 1};
  double outPt[4];  
  rasToXY->MultiplyPoint(inPt, outPt);
  point[0] = outPt[0];
  point[1] = outPt[1];

  renderer->SetDisplayPoint(point[0],point[1], 0);
  renderer->DisplayToWorld();
  renderer->GetWorldPoint(worldCoordinate);
  this->WCEntryPoint[0] = worldCoordinate[0];
  this->WCEntryPoint[1] = worldCoordinate[1];
  this->WCEntryPoint[2] = worldCoordinate[2];

  double rasTarget[3];
  this->GetGUI()->GetMRMLNode()->GetPlanTargetPoint(rasTarget);
  inPt[0] = rasTarget[0];
  inPt[1] = rasTarget[1];
  inPt[2] = rasTarget[2];
  rasToXY->MultiplyPoint(inPt, outPt);
  point[0] = outPt[0];
  point[1] = outPt[1];
  renderer->SetDisplayPoint(point[0],point[1], 0);
  renderer->DisplayToWorld();
  renderer->GetWorldPoint(worldCoordinate);
  this->WCTargetPoint[0] = worldCoordinate[0];
  this->WCTargetPoint[1] = worldCoordinate[1];
  this->WCTargetPoint[2] = worldCoordinate[2];


  // steps
  // get the cylinder source, create the cylinder, whose height is equal to calculated insertion depth
  // apply transform on the cylinder to world coordinates, using the information of entry and target point
  // i.e. using the insertion angle
  // add it to slice viewer's renderer

  vtkCylinderSource *needleGuide = vtkCylinderSource::New();
  // TO DO: how to relate this to actual depth???
  double halfNeedleLength = sqrt( (this->WCTargetPoint[0]-this->WCEntryPoint[0])*(this->WCTargetPoint[0]-this->WCEntryPoint[0]) + (this->WCTargetPoint[1]-this->WCEntryPoint[1])*(this->WCTargetPoint[1]-this->WCEntryPoint[1]));
  needleGuide->SetHeight(2*halfNeedleLength);
  //needleGuide->SetHeight(0.75 );
  needleGuide->SetRadius( 0.02 );
  //needleGuide->SetCenter((this->XYTargetPoint[0]+this->XYPlanPoint[0])/2, (this->XYTargetPoint[1]+this->XYPlanPoint[1])/2, 0);
  needleGuide->SetResolution( 10 );

  //int *windowSize = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindow()->GetSize();
  // because cylinder is positioned at the window center
  double needleCenter[3];  
  //needleCenter[0] = (this->WCTargetPoint[0]+this->WCEntryPoint[0])/2;// - windowSize[0]/2;
  //needleCenter[1] = (this->WCTargetPoint[1]+this->WCEntryPoint[1])/2;// - windowSize[1]/2;
  needleCenter[0] = this->WCEntryPoint[0];// - windowSize[0]/2;
  needleCenter[1] = this->WCEntryPoint[1];// - windowSize[1]/2;
  //needleGuide->SetCenter(needleCenter[0], needleCenter[1], needleCenter[2]);
  
  // TO DO: transfrom needle mapper using vtkTransformPolyData
  vtkMatrix4x4 *transformMatrix = vtkMatrix4x4::New();
  transformMatrix->Identity();
  double angle = this->GetGUI()->GetMRMLNode()->GetActualPlanInsertionAngle();
  double insAngleRad = vtkMath::Pi()/2 - double(vtkMath::Pi()/180)*angle;
  transformMatrix->SetElement(0,0, cos(insAngleRad));
  transformMatrix->SetElement(0,1, -sin(insAngleRad));
  transformMatrix->SetElement(0,2, 0);
  transformMatrix->SetElement(0,3, 0);
  transformMatrix->SetElement(0,3, needleCenter[0]);

  transformMatrix->SetElement(1,0, sin(insAngleRad));
  transformMatrix->SetElement(1,1, cos(insAngleRad));
  transformMatrix->SetElement(1,2, 0);
  transformMatrix->SetElement(1,3, 0);
  transformMatrix->SetElement(1,3, needleCenter[1]);

  transformMatrix->SetElement(2,0, 0);
  transformMatrix->SetElement(2,1, 0);
  transformMatrix->SetElement(2,2, 1);
  transformMatrix->SetElement(2,3, 0);

  transformMatrix->SetElement(3,0, 0);
  transformMatrix->SetElement(3,1, 0);
  transformMatrix->SetElement(3,2, 0);
  transformMatrix->SetElement(3,3, 1);

  vtkMatrixToHomogeneousTransform *transform = vtkMatrixToHomogeneousTransform::New();
  transform->SetInput(transformMatrix);
  vtkTransformPolyDataFilter *filter = vtkTransformPolyDataFilter::New();
  filter->SetInputConnection(needleGuide->GetOutputPort()); 
  filter->SetTransform(transform);

  // map
  vtkPolyDataMapper *needleMapper = vtkPolyDataMapper::New();
  needleMapper->SetInputConnection( filter->GetOutputPort() );
  //needleMapper->SetInputConnection(needleGuide->GetOutputPort());

  // after transfrom, set up actor
  this->NeedleActor = vtkActor::New(); 
  this->NeedleActor->SetMapper(needleMapper );  
  //needleActor->SetPosition(needleCenter[0], needleCenter[1],0);  
  //needleActor->RotateZ(90-angle);
  
 
  // add to renderer of the Axial slice viewer
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->AddActor(this->NeedleActor);
  this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->RequestRender(); 
 
  
}
//------------------------------------------------------------------------------
void vtkPerkStationPlanStep::CalculatePlanInsertionAngleAndDepth()
{
  // insertion depth is the distance between two points (entry and target) in mm
  double rasEntry[3];
  double rasTarget[3];
  this->GetGUI()->GetMRMLNode()->GetPlanEntryPoint(rasEntry);
  this->GetGUI()->GetMRMLNode()->GetPlanTargetPoint(rasTarget);
  double insDepth = sqrt( (rasTarget[0] - rasEntry[0])*(rasTarget[0] - rasEntry[0]) + (rasTarget[1] - rasEntry[1])*(rasTarget[1] - rasEntry[1]) + (rasTarget[2] - rasEntry[2])*(rasTarget[2] - rasEntry[2]) );
  this->GetGUI()->GetMRMLNode()->SetActualPlanInsertionDepth(insDepth);
  
  
  //insertion angle is the angle with x-axis of the line which has entry and target as its end points;
  double insAngle = double(180/vtkMath::Pi()) * atan(double((rasEntry[1] - rasTarget[1])/(rasEntry[0] - rasTarget[0])));
  this->GetGUI()->GetMRMLNode()->SetActualPlanInsertionAngle(insAngle);
  
  if (this->GetGUI()->GetMode() == vtkPerkStationModuleGUI::ModeId::Clinical)
    {
    this->InsertionDepth->GetWidget()->SetValueAsDouble(insDepth);
    this->InsertionAngle->GetWidget()->SetValueAsDouble(insAngle);
    }
}

//------------------------------------------------------------------------------
void vtkPerkStationPlanStep::Reset()
{
  // reset the overlay needle guide both in sliceviewer and in secondary monitor
  this->GetGUI()->GetSecondaryMonitor()->RemoveOverlayNeedleGuide();
  this->RemoveOverlayNeedleGuide();

  // reset parameters of mrml node
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }

  double ras[3] = {0.0,0.0,0.0};
  mrmlNode->SetPlanEntryPoint(ras);
  mrmlNode->SetPlanTargetPoint(ras);
  mrmlNode->SetUserPlanInsertionAngle(0.0);
  mrmlNode->SetActualPlanInsertionAngle(0.0);
  mrmlNode->SetUserPlanInsertionDepth(0.0);
  mrmlNode->SetActualPlanInsertionDepth(0.0);
  mrmlNode->CalculatePlanInsertionAngleError();
  mrmlNode->CalculatePlanInsertionDepthError();

  // reset local member variables to defaults
  this->WCEntryPoint[0] = 0.0;
  this->WCEntryPoint[1] = 0.0;
  this->WCEntryPoint[2] = 0.0;
  this->WCTargetPoint[0] = 0.0;
  this->WCTargetPoint[1] = 0.0;
  this->WCTargetPoint[2] = 0.0;

  // reset gui controls
  this->ResetControls();
  
}
//------------------------------------------------------------------------------
void vtkPerkStationPlanStep::ResetControls()
{
  this->EntryPoint->GetWidget(0)->SetValue("");
  this->EntryPoint->GetWidget(1)->SetValue("");
  this->EntryPoint->GetWidget(2)->SetValue("");
  this->TargetPoint->GetWidget(0)->SetValue("");
  this->TargetPoint->GetWidget(1)->SetValue("");
  this->TargetPoint->GetWidget(2)->SetValue("");
  this->InsertionAngle->GetWidget()->SetValue("");
  this->InsertionDepth->GetWidget()->SetValue("");
}
//------------------------------------------------------------------------------
void vtkPerkStationPlanStep::RemoveOverlayNeedleGuide()
{
  // should remove the overlay needle guide
  vtkActorCollection *collection = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->GetActors();
  if (collection->IsItemPresent(this->NeedleActor))
    {
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer()->RemoveActor(this->NeedleActor);
    this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->RequestRender();     
    }
}
