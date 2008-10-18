#include "vtkPerkStationValidateStep.h"

#include "vtkPerkStationModuleGUI.h"
#include "vtkMRMLPerkStationModuleNode.h"
#include "vtkPerkStationSecondaryMonitor.h"

#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWEntrySet.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPerkStationValidateStep);
vtkCxxRevisionMacro(vtkPerkStationValidateStep, "$Revision: 1.0 $");

//----------------------------------------------------------------------------
vtkPerkStationValidateStep::vtkPerkStationValidateStep()
{
  this->SetName("4/5. Validate");
  this->SetDescription("Mark actual entry point and target hit");  

  this->EntryPointFrame = NULL;
  this->EntryPointLabel = NULL;
  this->EntryPoint = NULL;
  this->TargetPointFrame = NULL;
  this->TargetPointLabel = NULL;
  this->TargetPoint = NULL;
  this->InsertionDepth = NULL;

}

//----------------------------------------------------------------------------
vtkPerkStationValidateStep::~vtkPerkStationValidateStep()
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

  if (this->InsertionDepth)
    {
    this->InsertionDepth->Delete();
    this->InsertionDepth = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  switch (this->GetGUI()->GetMode())      
    {

    case vtkPerkStationModuleGUI::ModeId::Training:

      this->SetName("4/5. Validate");
      this->GetGUI()->GetWizardWidget()->Update();
      break;

    case vtkPerkStationModuleGUI::ModeId::Clinical:
       
      // in clinical mode
      this->SetName("4/4. Validate");
      this->GetGUI()->GetWizardWidget()->Update();
      break;
    }
  
  this->SetDescription("Mark actual entry point and target hit");  

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
    this->InsertionDepth->SetLabelText("Insertion depth (in mm):");
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
/*void vtkPerkStationValidateStep::PopulateLoadedParameterSets(
  vtkObject *obj, const char *method)
{
  if(!this->ParameterSetMenuButton ||
     !this->ParameterSetMenuButton->IsCreated())
    {
    return;
    }

  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();

  vtkKWMenu *menu = 
    this->ParameterSetMenuButton->GetWidget()->GetMenu();
  menu->DeleteAllItems();
  char buffer[256];
  
  sprintf(buffer, "%s %d", method, -1);
  menu->AddRadioButton("Create New Parameters", obj, buffer);
  
  if(!mrmlManager)
    {
    vtkWarningMacro("PopulateLoadedParameterSets: returning, no mrml manager");
    return;
    }

  int nb_of_sets = mrmlManager->GetNumberOfParameterSets();
  for(int index = 0; index < nb_of_sets; index++)
    {
    const char *name = mrmlManager->GetNthParameterSetName(index);
    if (name)
      {
      sprintf(buffer, "%s %d", method, index);
      menu->AddRadioButton(name, this, buffer);
      }
    }
}

//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::UpdateLoadedParameterSets()
{
  if(!this->ParameterSetMenuButton ||
     !this->ParameterSetMenuButton->IsCreated())
    {
    return;
    }

  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();
  if(!mrmlManager)
    {
    return;
    }

  vtkKWMenuButton *menuButton = this->ParameterSetMenuButton->GetWidget();
  vtksys_stl::string sel_value = "";
  if(menuButton->GetValue())
    {
    sel_value = menuButton->GetValue();
    }

  this->PopulateLoadedParameterSets(
    this, "SelectedParameterSetChangedCallback");

  if (strcmp(sel_value.c_str(), "") != 0)
    {
    // Select the original
    int nb_of_sets = menuButton->GetMenu()->GetNumberOfItems();
    for (int index = 0; index < nb_of_sets; index++)
      {
      const char *name = menuButton->GetMenu()->GetItemLabel(index);
      if (name && strcmp(sel_value.c_str(), name) == 0)
        {
        menuButton->GetMenu()->SelectItem(index);
        return;
        }
      }
    }

  // if there is no previous selection, select the first loaded set,
  // or if there is no loaded set, leave it blank

  int nb_of_sets = mrmlManager->GetNumberOfParameterSets();
  if(nb_of_sets > 0 &&
     menuButton->GetMenu()->GetNumberOfItems() > 1)
    {
    this->ParameterSetMenuButton->GetWidget()->GetMenu()->SelectItem(1);
    this->SelectedParameterSetChangedCallback(0);
    }
}

//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::SelectedParameterSetChangedCallback(
  int index)
{
  vtkEMSegmentMRMLManager *mrmlManager = this->GetGUI()->GetMRMLManager();

  // New Parameters

  if (index < 0)
    {
    mrmlManager->CreateAndObserveNewParameterSet();
    //Assuming the mrml manager adds the node to the end.
    int nb_of_sets = mrmlManager->GetNumberOfParameterSets();
    if (nb_of_sets > 0)
      {
      this->UpdateLoadedParameterSets();
      const char *name = mrmlManager->GetNthParameterSetName(nb_of_sets-1);
      if (name)
        {
        // Select the newly created parameter set
        vtkKWMenuButton *menuButton = 
          this->ParameterSetMenuButton->GetWidget();
        if (menuButton->GetMenu()->GetNumberOfItems() == nb_of_sets + 1)
          {
          menuButton->GetMenu()->SelectItem(nb_of_sets);
          }
        }
      }
    }
  else
    {
    mrmlManager->SetLoadedParameterSetIndex(index);
    }
  
  vtkEMSegmentAnatomicalStructureStep *anat_step = 
    this->GetGUI()->GetAnatomicalStructureStep();
  if (anat_step && 
      anat_step->GetAnatomicalStructureTree() && 
      anat_step->GetAnatomicalStructureTree()->IsCreated())
    {
    anat_step->GetAnatomicalStructureTree()->GetWidget()->DeleteAllNodes();
    }
}
*/
//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::InstallCallbacks()
{
}
//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::PopulateControls()
{
}
//-----------------------------------------------------------------------------
void vtkPerkStationValidateStep::ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData)
{
  static unsigned int clickNum = 0;
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  // first identify if the step is pertinent, i.e. current step of wizard workflow is actually calibration step
  if (!wizard_widget ||
      wizard_widget->GetWizardWorkflow()->GetCurrentStep() != 
      this)
    {
    return;
    }

  vtkSlicerInteractorStyle *s = vtkSlicerInteractorStyle::SafeDownCast(caller);
  vtkSlicerInteractorStyle *istyle0 = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyleSecondary = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetSecondaryMonitor()->GetRenderWindowInteractor()->GetInteractorStyle());

  vtkRenderWindowInteractor *rwi;
  vtkMatrix4x4 *matrix;
  vtkRenderer *renderer = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer();

  if ((s == istyleSecondary) && (event == vtkCommand::LeftButtonPressEvent))
    {
    ++clickNum;
    if (s == istyle0)
      {
      // mouse click happened in the axial slice view      
      vtkSlicerSliceGUI *sliceGUI = vtkSlicerApplicationGUI::SafeDownCast(this->GetGUI()->GetApplicationGUI())->GetMainSliceGUI("Red");
      rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();    
      matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
      }
    else if (s == istyleSecondary)
      {   
      rwi = this->GetGUI()->GetSecondaryMonitor()->GetRenderWindowInteractor();
      matrix = this->GetGUI()->GetSecondaryMonitor()->GetXYToRAS(); 
      }
    
    
    // capture the point
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
      this->GetGUI()->GetMRMLNode()->SetValidateEntryPoint(ras);
      this->GetGUI()->GetMRMLNode()->CalculateEntryPointError();
      
      }
    else if (clickNum == 2)
      {
      this->TargetPoint->GetWidget(0)->SetValueAsDouble(ras[0]);
      this->TargetPoint->GetWidget(1)->SetValueAsDouble(ras[1]);
      this->TargetPoint->GetWidget(2)->SetValueAsDouble(ras[2]);
      
      // record value in the MRML node
      this->GetGUI()->GetMRMLNode()->SetValidateTargetPoint(ras);      
      this->GetGUI()->GetMRMLNode()->CalculateTargetPointError();
      
      clickNum = 0;

      // if in clinical mode, the insertion depth should be automatically calculated & displayed
      if (this->GetGUI()->GetMode() == vtkPerkStationModuleGUI::ModeId::Clinical)
        {   
        double rasEntry[3];
        double rasTarget[3];
        this->GetGUI()->GetMRMLNode()->GetValidateEntryPoint(rasEntry);
        this->GetGUI()->GetMRMLNode()->GetValidateTargetPoint(rasTarget);
        double insDepth = sqrt( (rasTarget[0] - rasEntry[0])*(rasTarget[0] - rasEntry[0]) + (rasTarget[1] - rasEntry[1])*(rasTarget[1] - rasEntry[1]) + (rasTarget[2] - rasEntry[2])*(rasTarget[2] - rasEntry[2]) );
        this->InsertionDepth->GetWidget()->SetValueAsDouble(insDepth);
        }       


      }


    }

}
//------------------------------------------------------------------------------
void vtkPerkStationValidateStep::Reset()
{
 
  // reset parameters of mrml node
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }

  double ras[3] = {0.0,0.0,0.0};
  mrmlNode->SetValidateEntryPoint(ras);
  mrmlNode->SetValidateTargetPoint(ras);
  mrmlNode->CalculateEntryPointError();
  mrmlNode->CalculateTargetPointError();

  // reset local member variables to defaults
 
  // reset gui controls
  this->ResetControls();
  
}
//------------------------------------------------------------------------------
void vtkPerkStationValidateStep::ResetControls()
{
  if (this->EntryPoint)
    {
    this->EntryPoint->GetWidget(0)->SetValue("");
    this->EntryPoint->GetWidget(1)->SetValue("");
    this->EntryPoint->GetWidget(2)->SetValue("");
    }

  if (this->TargetPoint)
    {  
    this->TargetPoint->GetWidget(0)->SetValue("");
    this->TargetPoint->GetWidget(1)->SetValue("");
    this->TargetPoint->GetWidget(2)->SetValue(""); 
    }

  if (this->InsertionDepth)
    {  
    this->InsertionDepth->GetWidget()->SetValue("");
    }
}
