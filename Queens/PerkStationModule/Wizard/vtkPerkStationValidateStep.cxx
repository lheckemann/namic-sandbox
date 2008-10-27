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
  this->WizardGUICallbackCommand->SetCallback(vtkPerkStationValidateStep::WizardGUICallback);

  this->ResetFrame = NULL;
  this->ResetValidationButton = NULL;

  this->EntryPointFrame = NULL;
  this->EntryPointLabel = NULL;
  this->EntryPoint = NULL;
  this->TargetPointFrame = NULL;
  this->TargetPointLabel = NULL;
  this->TargetPoint = NULL;
  this->InsertionDepth = NULL;
  
  this->EntryTargetAcquired = false;
  this->ClickNumber = 0;
  this->ProcessingCallback = false;

}

//----------------------------------------------------------------------------
vtkPerkStationValidateStep::~vtkPerkStationValidateStep()
{
  if (this->ResetFrame)
    {
    this->ResetFrame->Delete();
    this->ResetFrame = NULL;
    }
  if (this->ResetValidationButton)
    {
    this->ResetValidationButton->Delete();
    this->ResetValidationButton = NULL;
    }


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

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();
  wizard_widget->GetCancelButton()->SetEnabled(0);
  vtkKWWidget *parent = wizard_widget->GetClientArea();
  int enabled = parent->GetEnabled();

  // clear controls
  if (this->ResetFrame)
    {
    this->Script("pack forget %s", 
                    this->ResetFrame->GetWidgetName());
    }

  if (this->ResetValidationButton)
    {
    this->Script("pack forget %s", 
                    this->ResetValidationButton->GetWidgetName());
    }


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

       // additional reset button

      // frame for reset button
      if (!this->ResetFrame)
        {
        this->ResetFrame = vtkKWFrame::New();
        }
      if (!this->ResetFrame->IsCreated())
        {
        this->ResetFrame->SetParent(parent);
        this->ResetFrame->Create();     
        }
      this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                        this->ResetFrame->GetWidgetName());
      
      if (!this->ResetValidationButton)
        {
        this->ResetValidationButton = vtkKWPushButton::New();
        }
      if(!this->ResetValidationButton->IsCreated())
        {
        this->ResetValidationButton->SetParent(this->ResetFrame);
        this->ResetValidationButton->SetText("Reset validation");
        this->ResetValidationButton->SetBorderWidth(2);
        this->ResetValidationButton->SetReliefToRaised();      
        this->ResetValidationButton->SetHighlightThickness(2);
        this->ResetValidationButton->SetBackgroundColor(0.85,0.85,0.85);
        this->ResetValidationButton->SetActiveBackgroundColor(1,1,1);      
        this->ResetValidationButton->SetImageToPredefinedIcon(vtkKWIcon::IconTrashcan);
        this->ResetValidationButton->Create();
        }
      
      this->Script("pack %s -side top -padx 2 -pady 4", 
                        this->ResetValidationButton->GetWidgetName());
      
      

      break;
    }
  
  this->SetDescription("Mark actual entry point and target hit");  

  
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
void vtkPerkStationValidateStep::AddGUIObservers()
{
  this->RemoveGUIObservers();

  if (this->ResetValidationButton)
    {
    this->ResetValidationButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
 
}
//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::RemoveGUIObservers()
{
  if (this->ResetValidationButton)
    {
    this->ResetValidationButton->RemoveObservers(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->WizardGUICallbackCommand );
    }
  

}

//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::InstallCallbacks()
{
    // Configure the OK button to start
  if (this->GetGUI()->GetMode() == vtkPerkStationModuleGUI::ModeId::Clinical)
    {
    vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

    if (wizard_widget->GetOKButton())
      {
      wizard_widget->GetOKButton()->SetText("Start over");
      wizard_widget->GetOKButton()->SetCommand(
      this, "StartOverNewExperiment");
      wizard_widget->GetOKButton()->SetBalloonHelpString(
      "Save and do another experiment");
      }

    }

  this->AddGUIObservers();

 
}
//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::StartOverNewExperiment()
{
 this->GetGUI()->ResetAndStartNewExperiment();
}
//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::PopulateControls()
{
}
//-----------------------------------------------------------------------------
void vtkPerkStationValidateStep::ProcessImageClickEvents(vtkObject *caller, unsigned long event, void *callData)
{
  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  // first identify if the step is pertinent, i.e. current step of wizard workflow is actually calibration step
  if (!wizard_widget ||
      wizard_widget->GetWizardWorkflow()->GetCurrentStep() != 
      this)
    {
    return;
    }

  if (!this->GetGUI()->GetMRMLNode() || !this->GetGUI()->GetMRMLNode()->GetValidationVolumeNode() || strcmp(this->GetGUI()->GetMRMLNode()->GetVolumeInUse(), "Validation")!=0)
    {
    // TO DO: what to do on failure
    return;
    }

  if(this->EntryTargetAcquired)
    {
    return;
    }

  vtkSlicerInteractorStyle *s = vtkSlicerInteractorStyle::SafeDownCast(caller);
  vtkSlicerInteractorStyle *istyle0 = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle *istyleSecondary = vtkSlicerInteractorStyle::SafeDownCast(this->GetGUI()->GetSecondaryMonitor()->GetRenderWindowInteractor()->GetInteractorStyle());

  vtkRenderWindowInteractor *rwi;
  vtkMatrix4x4 *matrix;
  vtkRenderer *renderer = this->GetGUI()->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetOverlayRenderer();

  if ((s == istyle0) && (event == vtkCommand::LeftButtonPressEvent))
    {
    ++this->ClickNumber;
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
    if (this->ClickNumber ==1)
      {
      // entry point specification by user
      this->EntryPoint->GetWidget(0)->SetValueAsDouble(ras[0]);
      this->EntryPoint->GetWidget(1)->SetValueAsDouble(ras[1]);
      this->EntryPoint->GetWidget(2)->SetValueAsDouble(ras[2]);

      // record value in mrml node
      this->GetGUI()->GetMRMLNode()->SetValidateEntryPoint(ras);
      this->GetGUI()->GetMRMLNode()->CalculateEntryPointError();
      
      }
    else if (this->ClickNumber == 2)
      {
      this->TargetPoint->GetWidget(0)->SetValueAsDouble(ras[0]);
      this->TargetPoint->GetWidget(1)->SetValueAsDouble(ras[1]);
      this->TargetPoint->GetWidget(2)->SetValueAsDouble(ras[2]);
      
      // record value in the MRML node
      this->GetGUI()->GetMRMLNode()->SetValidateTargetPoint(ras);      
      this->GetGUI()->GetMRMLNode()->CalculateTargetPointError();
      
      this->ClickNumber = 0;

      double rasEntry[3];
      double rasTarget[3];
      this->GetGUI()->GetMRMLNode()->GetValidateEntryPoint(rasEntry);
      this->GetGUI()->GetMRMLNode()->GetValidateTargetPoint(rasTarget);
      double insDepth = sqrt( (rasTarget[0] - rasEntry[0])*(rasTarget[0] - rasEntry[0]) + (rasTarget[1] - rasEntry[1])*(rasTarget[1] - rasEntry[1]) + (rasTarget[2] - rasEntry[2])*(rasTarget[2] - rasEntry[2]) );
      this->GetGUI()->GetMRMLNode()->SetValidateInsertionDepth(insDepth);   
      this->InsertionDepth->GetWidget()->SetValueAsDouble(insDepth);

      this->EntryTargetAcquired = true;
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
  this->EntryTargetAcquired = false;
  this->ClickNumber = 0;
  this->ProcessingCallback = false;
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

//-----------------------------------------------------------------------------
void vtkPerkStationValidateStep::LoadValidation(istream &file)
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }  

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
      if (!strcmp(attName, " ValidateInsertionDepth"))
        {
        std::stringstream ss;
        ss << attValue;
        double val;
        ss >> val;
        mrmlNode->SetValidateInsertionDepth(val);       
        paramSetCount++;
        }
      else if (!strcmp(attName, " ValidateEntryPoint"))
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
          double point[3];
          for (unsigned int i = 0; i < tmpVec.size(); i++)
            point[i] = tmpVec[i];
          mrmlNode->SetValidateEntryPoint(point[0], point[1], point[2]);
          paramSetCount++;
          }
        else
          {
          // error in file?
          }     
        }
      else if (!strcmp(attName, " ValidateTargetPoint"))
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
          double point[3];
          for (unsigned int i = 0; i < tmpVec.size(); i++)
            point[i] = tmpVec[i];
          mrmlNode->SetValidateTargetPoint(point[0], point[1], point[2]);
          paramSetCount++;
          }
        else
          {
          // error in file?
          }     
        }
      
      }// end if testing for it is a valid attName

    } // end while going through the file
  
  if (paramSetCount == 3)
    {
    // all params correctly read from file
    
    // reflect the values of params in GUI controls
//  this->PopulateControlsOnLoadCalibration();
    // set any state variables required to be set
    }
  else
    {
    // error reading file, not all values set
    int error = -1;
    }
}

//-----------------------------------------------------------------------------
void vtkPerkStationValidateStep::SaveValidation(ostream& of)
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }  

  
  // entry point
  of << " ValidateEntryPoint=\"" ;
  double entryPoint[3];
  mrmlNode->GetValidateEntryPoint(entryPoint);
  for(int i = 0; i < 3; i++)
      of << entryPoint[i] << " ";
  of << "\" \n";
     
  // target point
  of << " ValidateTargetPoint=\""; 
  double targetPoint[3];
  mrmlNode->GetValidateTargetPoint(targetPoint);
  for(int i = 0; i < 3; i++)
      of << targetPoint[i] << " ";
  of << "\" \n";

  // insertion depth
  of << " ValidateInsertionDepth=\"";
  double depth = mrmlNode->GetValidateInsertionDepth();  
  of << depth << " ";
  of << "\" \n";


  

}



//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::WizardGUICallback(vtkObject *caller, unsigned long event, void *clientData, void *callData )
{
    vtkPerkStationValidateStep *self = reinterpret_cast<vtkPerkStationValidateStep *>(clientData);
    if (self) { self->ProcessGUIEvents(caller, event, callData); }
}

//----------------------------------------------------------------------------
void vtkPerkStationValidateStep::ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData)
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetGUI()->GetMRMLNode();

  if(!mrmlNode)
      return;

  if(!mrmlNode->GetPlanningVolumeNode() || strcmp(mrmlNode->GetVolumeInUse(), "Validation")!=0)
      return;

  
  if (this->ProcessingCallback)
    {
    return;
    }

  this->ProcessingCallback = true;
  
  // reset plan button
  if (this->ResetValidationButton && this->ResetValidationButton == vtkKWPushButton::SafeDownCast(caller) && (event == vtkKWPushButton::InvokedEvent))
    {
    this->Reset();
    }
  this->ProcessingCallback = false;
}
