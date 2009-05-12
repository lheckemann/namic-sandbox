/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkTRProstateBiopsyGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"
#include "vtkSlicerFiducialsGUI.h"
#include "vtkSlicerFiducialsLogic.h"
#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWApplication.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkTRProstateBiopsyStep.h"
#include "vtkTRProstateBiopsyCalibrationStep.h"
#include "vtkTRProstateBiopsySegmentationStep.h"
#include "vtkTRProstateBiopsyTargetingStep.h"
#include "vtkTRProstateBiopsyVerificationStep.h"
#include "vtkTRProstateBiopsyUSBOpticalEncoder.h"
#include "vtkMath.h"

#include "vtkSlicerInteractorStyle.h"
#include "vtkIGTDataManager.h"
#include "vtkIGTPat2ImgRegistration.h"

#include "vtkKWRenderWidget.h"
#include "vtkKWWidget.h"
#include "vtkKWMenuButton.h"
#include "vtkKWCheckButton.h"
#include "vtkKWPushButton.h"
#include "vtkKWPushButtonSet.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWLoadSaveDialog.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWMenu.h"
#include "vtkKWLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWEvent.h"
#include "vtkKWOptions.h"

#include "vtkKWTkUtilities.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkCylinderSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkCornerAnnotation.h"

// for images
#include "vtkSlicerColorLogic.h"

// for 3D object display
#include "vtkCylinderSource.h"
#include "vtkMRMLLinearTransformNode.h"


#include <vector>
#include <sstream>
#include <ctime>
#include <io.h>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().

#include "itkMetaDataObject.h"
//---------------------------------------------------------------------------
/*vtkStandardNewMacro(vtkTRProstateBiopsyGUI);
vtkCxxRevisionMacro(vtkTRProstateBiopsyGUI, "$Revision: 1.0 $");
*/
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Button Colors and Labels for Work Phase Control
const double vtkTRProstateBiopsyGUI::WorkPhaseColor[vtkTRProstateBiopsyLogic::NumPhases][3] =
  {
  /* Cl */ { 1.0, 0.6, 1.0 },
  /* Sg */ { 1.0, 0.8, 0.8 },
  /* Tg */ { 1.0, 1.0, 0.6 },
  /* Ve */ { 0.6, 0.6, 1.0 },
  };

const double vtkTRProstateBiopsyGUI::WorkPhaseColorActive[vtkTRProstateBiopsyLogic::NumPhases][3] =
  {
  /* Cl */ { 1.0, 0.4, 1.0 },
  /* Sg */ { 1.0, 0.5, 0.5 },
  /* Tg */ { 1.0, 1.0, 0.4 },
  /* Ve */ { 0.4, 0.4, 1.0 },
  };

const double vtkTRProstateBiopsyGUI::WorkPhaseColorDisabled[vtkTRProstateBiopsyLogic::NumPhases][3] =
  {
  /* Cl */ { 1.0, 0.95, 1.0 },
  /* Sg */ { 1.0, 0.95, 0.95 },
  /* Tg */ { 1.0, 1.0, 0.95 },
  /* Ve */ { 0.95, 0.95, 1.0 },
  };

const char *vtkTRProstateBiopsyGUI::WorkPhaseStr[vtkTRProstateBiopsyLogic::NumPhases] =
  {
  /* Cl */ "Calibration",
  /* Sg */ "Segmentation",
  /* Tg */ "Targeting",
  /* Ve */ "Verification",
  };
//------------------------------------------------------------------------------
vtkTRProstateBiopsyGUI* vtkTRProstateBiopsyGUI::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTRProstateBiopsyGUI");
  if(ret)
    {
      return (vtkTRProstateBiopsyGUI*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkTRProstateBiopsyGUI;
}

//---------------------------------------------------------------------------
vtkTRProstateBiopsyGUI::vtkTRProstateBiopsyGUI()
{
  ////slicerCerr("vtkTRProstateBiopsyGUI Constructor");

   // gui elements
  this->VolumeSelector = vtkSlicerNodeSelectorWidget::New();
  this->TRNodeSelector = vtkSlicerNodeSelectorWidget::New();

  this->LoadExperimentFileButton = NULL;
  this->SaveExperimentFileButton = NULL;

  //----------------------------------------------------------------
  // Logic values
  
  //logic and mrml nodes
  this->Logic = NULL;
  this->MRMLNode = NULL;

  this->OpticalTimerEventId = NULL;
  this->TimerProcessing = false;
  
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData(reinterpret_cast<void *>(this));
  this->DataCallbackCommand->SetCallback(vtkTRProstateBiopsyGUI::DataCallback);
  
  //----------------------------------------------------------------
  // Workphase Frame
  
  this->WorkPhaseButtonSet = NULL;

  //----------------------------------------------------------------  
  // Wizard Frame
  
  this->WizardWidget = vtkKWWizardWidget::New();
  this->WizardSteps = new vtkTRProstateBiopsyStep*[vtkTRProstateBiopsyLogic::NumPhases];
  for (int i = 0; i < vtkTRProstateBiopsyLogic::NumPhases; i ++)
    {
    this->WizardSteps[i] = NULL;
    }
  
  
  
}

//---------------------------------------------------------------------------
vtkTRProstateBiopsyGUI::~vtkTRProstateBiopsyGUI()
{
  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  this->RemoveGUIObservers();

  if ( this->VolumeSelector ) 
    {
    this->VolumeSelector->SetParent(NULL);
    this->VolumeSelector->Delete();
    this->VolumeSelector = NULL;
    }
  if ( this->TRNodeSelector ) 
    {
    this->TRNodeSelector->SetParent(NULL);
    this->TRNodeSelector->Delete();
    this->TRNodeSelector = NULL;
    }
  if (this->LoadExperimentFileButton)
    {
    this->LoadExperimentFileButton->SetParent(NULL);
    this->LoadExperimentFileButton->Delete();
    this->LoadExperimentFileButton = NULL;
    }
  if (this->SaveExperimentFileButton)
    {
    this->SaveExperimentFileButton->SetParent(NULL);
    this->SaveExperimentFileButton->Delete();
    this->SaveExperimentFileButton = NULL;
    }


  //----------------------------------------------------------------
  // Workphase Frame

  if (this->WorkPhaseButtonSet)
    {
    this->WorkPhaseButtonSet->SetParent(NULL);
    this->WorkPhaseButtonSet->Delete();
    }


  //----------------------------------------------------------------
  // Wizard Frame

  if (this->WizardWidget)
    {
    this->WizardWidget->Delete();
    this->WizardWidget = NULL;
    }

  this->SetModuleLogic(NULL);  
  vtkSetMRMLNodeMacro(this->MRMLNode, NULL);

  if (this->OpticalTimerEventId)
    {
    vtkKWTkUtilities::CancelTimerHandler(vtkKWApplication::GetMainInterp(), this->OpticalTimerEventId); 
    }
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::SetModuleLogic(vtkSlicerLogic* logic)
{
  this->SetLogic(dynamic_cast<vtkTRProstateBiopsyLogic*>(logic));
//  this->GetLogic()->GetMRMLManager()->SetMRMLScene(this->GetMRMLScene()); 
//  this->SetMRMLManager(this->GetLogic()->GetMRMLManager());
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  
  os << indent << "TRProstateBiopsyGUI: " << this->GetClassName() << "\n";
  os << indent << "Logic: " << this->GetLogic() << "\n";
   
  // print widgets?
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::RemoveGUIObservers()
{
  this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
  this->GetApplicationGUI()->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
  this->GetApplicationGUI()->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);


  this->VolumeSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );

  this->TRNodeSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );    

  if (this->LoadExperimentFileButton)
    {
    this->LoadExperimentFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->SaveExperimentFileButton)
    {
    this->SaveExperimentFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }

  
  // Workphase Frame
  if (this->WorkPhaseButtonSet)
    {
    for (int i = 0; i < this->WorkPhaseButtonSet->GetNumberOfWidgets(); i ++)
      {
      this->WorkPhaseButtonSet->GetWidget(i)->RemoveObserver(static_cast<vtkCommand *>(this->GUICallbackCommand));
      }
    }
    

  // Wizard Frame
  this->WizardWidget->GetWizardWorkflow()->RemoveObserver(static_cast<vtkCommand *>(this->GUICallbackCommand));

  this->RemoveLogicObservers();
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::RemoveLogicObservers()
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(
      vtkCommand::ModifiedEvent,
      static_cast<vtkCommand *>(this->LogicCallbackCommand));
    }
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::AddGUIObservers()
{
  this->RemoveGUIObservers();

  this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  this->GetApplicationGUI()->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  this->GetApplicationGUI()->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);

  
  this->VolumeSelector->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );

  this->TRNodeSelector->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand ); 

  if (this->LoadExperimentFileButton)
    {
    this->LoadExperimentFileButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->SaveExperimentFileButton)
    {
    this->SaveExperimentFileButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }

  // Workphase Frame

  for (int i = 0; i < this->WorkPhaseButtonSet->GetNumberOfWidgets(); i++)
    {
        this->WorkPhaseButtonSet->GetWidget(i)->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent,(vtkCommand *)this->GUICallbackCommand);
    }
  
  // Wizard Frame

  this->WizardWidget->GetWizardWorkflow()->AddObserver( vtkKWWizardWorkflow::CurrentStateChangedEvent, static_cast<vtkCommand *>(this->GUICallbackCommand));

  this->AddLogicObservers();
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::AddLogicObservers()
{
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
  ////slicerCerr("vtkTRProstateBiopsyGUI::HandleMouseEvent");
}


//---------------------------------------------------------------------------
/*void vtkTRProstateBiopsyGUI::Init()
{
}

*/
//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::DataCallback(vtkObject *caller, 
        unsigned long eid, void *clientData, void *callData)
{
  vtkTRProstateBiopsyGUI *self =
    reinterpret_cast<vtkTRProstateBiopsyGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkTRProstateBiopsyGUI DataCallback");

  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::ProcessLogicEvents(vtkObject *caller,
    unsigned long event, void *callData)
{
  if (this->GetLogic() == vtkTRProstateBiopsyLogic::SafeDownCast(caller))
    {
    // Fill in
    }
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::ProcessGUIEvents(
  vtkObject *caller, unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  vtkKWWizardWorkflow *wizardWorkflow = this->WizardWidget->GetWizardWorkflow();
  

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    int phase = this->GetLogic()->GetCurrentPhase();
    if (phase < vtkTRProstateBiopsyLogic::NumPhases)
      {
      this->WizardSteps[phase]->ProcessImageClickEvents(caller, event, callData);      
      }
    } 
  else if ( (wizardWorkflow == vtkKWWizardWorkflow::SafeDownCast(caller)) &&  (event == vtkKWWizardWorkflow::CurrentStateChangedEvent))
    {
    // Wizard Frame
    int phase = vtkTRProstateBiopsyLogic::Targeting;
    vtkKWWizardStep* step = this->WizardWidget->GetWizardWorkflow()->GetCurrentStep();

    for (int i = 0; i < vtkTRProstateBiopsyLogic::NumPhases; i++)
      {
      if (step == vtkKWWizardStep::SafeDownCast(this->WizardSteps[i]))
        {
        phase = i;      
        }
      }
    if (phase != this->Logic->GetCurrentPhase())
      {
      this->ChangeWorkPhase(phase);   
      }
    }
  else if (event == vtkKWRadioButton::SelectedStateChangedEvent)
    {
    // Check Work Phase Transition Buttons
    int phase;
    for (phase = 0; phase < this->WorkPhaseButtonSet->GetNumberOfWidgets();  phase++)
      {
      if (this->WorkPhaseButtonSet->GetWidget(phase) == vtkKWRadioButton::SafeDownCast(caller) && this->WorkPhaseButtonSet->GetWidget(phase)->GetSelectedState()==1)
        {
        break;      
        }
      }
    if (phase < vtkTRProstateBiopsyLogic::NumPhases) // if pressed one of them
      {
      if (phase != this->Logic->GetCurrentPhase())
        {
        this->ChangeWorkPhase(phase, 1);
        }
      }
    }    
  else if (this->LoadExperimentFileButton && this->LoadExperimentFileButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    // load calib dialog button
    const char *fileName = this->LoadExperimentFileButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
      //this->CalibFilePath = std::string(this->LoadCalibrationFileButton->GetLoadSaveDialog()->GetLastPath());
      // indicates ok has been pressed with a file name
      //this->CalibFileName = std::string(fileName);

      // call the callback function
      this->LoadExperimentButtonCallback(fileName);
    
      }
    
    // reset the file browse button text
    this->LoadExperimentFileButton->SetText ("Load experiment");
   
    }  
  else if (this->SaveExperimentFileButton && this->SaveExperimentFileButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {    
    // save calib dialog button
    const char *fileName = this->SaveExperimentFileButton->GetLoadSaveDialog()->GetFileName();
    if ( fileName ) 
      {
    
      std::string fullFileName = std::string(fileName) + ".xml";
      // get the file name and file path
      this->SaveExperimentFileButton->GetLoadSaveDialog()->SaveLastPathToRegistry("OpenPath");
        
      // call the callback function
      this->SaveExperimentButtonCallback(fullFileName.c_str());

    
      }
    
    // reset the file browse button text
    this->SaveExperimentFileButton->SetText ("Save experiment");
   
    }  
  else if (vtkSlicerNodeSelectorWidget::SafeDownCast(caller) == this->VolumeSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent && this->VolumeSelector->GetSelected() != NULL) 
    { 
     // volume slicer node selector widget
    this->UpdateMRML();
    }  
  else if (vtkSlicerNodeSelectorWidget::SafeDownCast(caller) == this->TRNodeSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  && this->TRNodeSelector->GetSelected() != NULL) 
    { 
    // module slicer node selector widget
    vtkMRMLTRProstateBiopsyModuleNode* n = vtkMRMLTRProstateBiopsyModuleNode::SafeDownCast(this->TRNodeSelector->GetSelected());
    this->Logic->SetAndObserveTRProstateBiopsyModuleNode(n);
    vtkSetAndObserveMRMLNodeMacro( this->MRMLNode, n);
    this->UpdateGUI();
    }  
  else
    {
    // Process Wizard GUI (Active step only)
    // TO DO: siddharth
    // need to eliminate this, as the wizard GUI widgets should have their corresponding GUI callback commands
    int phase = this->Logic->GetCurrentPhase();
//    this->WizardSteps[phase]->ProcessGUIEvents(caller, event, callData);
    }
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::ProcessMRMLEvents(vtkObject *caller,
    unsigned long event, void *callData)
{
    // Fill in
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::UpdateMRML ()
{
  vtkMRMLTRProstateBiopsyModuleNode* n = this->GetMRMLNode();
  
  if (n == NULL)
    {
    // no parameter node selected yet, create new
    this->TRNodeSelector->SetSelectedNew("vtkMRMLTRProstateBiopsyModuleNode");
    this->TRNodeSelector->ProcessNewNodeCommand("vtkMRMLTRProstateBiopsyModuleNode", "TransRectal");
    n = vtkMRMLTRProstateBiopsyModuleNode::SafeDownCast(this->TRNodeSelector->GetSelected());

    // set an observe new node in Logic
    this->Logic->SetAndObserveTRProstateBiopsyModuleNode(n);
    vtkSetAndObserveMRMLNodeMacro(this->MRMLNode, n);

     // add MRMLFiducialListNode to the scene
    this->GetLogic()->GetMRMLScene()->SaveStateForUndo();
    // one node for calibration fiducials list
    this->GetLogic()->GetMRMLScene()->AddNode(this->MRMLNode->GetCalibrationFiducialsList());
    // as many nodes as there are targeting fiducials lists
    for (unsigned int i = 0; i < this->MRMLNode->GetNumberOfNeedles(); i++)
      {
      this->GetLogic()->GetMRMLScene()->AddNode(this->MRMLNode->GetTargetingFiducialsList(i));
      }


    }

  // save node parameters for Undo
  this->GetLogic()->GetMRMLScene()->SaveStateForUndo(n);
  
  
  if (this->VolumeSelector->GetSelected() != NULL)
    {    
    // TO DO: siddharth
    // handle this later or make this control read-only, so that we can modify this only programmatically, not through user

    int phase = this->Logic->GetCurrentPhase();
    
    switch(phase)
      {
      case vtkTRProstateBiopsyLogic::Calibration:
        break;
      case vtkTRProstateBiopsyLogic::Targeting:
        break;
      case vtkTRProstateBiopsyLogic::Verification:
        break;
      }
    
    /*
    // see in what state wizard gui is, act accordingly
    if (this->WizardWidget->GetWizardWorkflow()->GetCurrentStep() == this->WizardSteps[phase])
      {

      bool planningVolumePreExists = false;
      // inside calibrate step

      // what if the volume selected is actually validation
      // in that case just return
     if (n->GetValidationVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        if (strcmpi(n->GetValidationVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Validation");          
          return;
          }
        }

      if (n->GetPlanningVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        if (strcmpi(n->GetPlanningVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Planning");   
          return;
          }
        // this implies that a planning volume already existed but now there is new image/volume chosen as planning volume in calibrate step
        planningVolumePreExists = true;
        }

      

      // calibrate/planning volume set
      n->SetPlanningVolumeRef(this->VolumeSelector->GetSelected()->GetID());
      n->SetPlanningVolumeNode(vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(n->GetPlanningVolumeRef())));
      n->SetVolumeInUse("Planning");
      

      vtkMRMLScalarVolumeDisplayNode *node = NULL;
      vtkSetAndObserveMRMLNodeMacro(node, n->GetPlanningVolumeNode()->GetScalarVolumeDisplayNode());

      const char *strName = this->VolumeSelector->GetSelected()->GetName();
      std::string strPlan = std::string(strName) + "-Plan";
      this->VolumeSelector->GetSelected()->SetName(strPlan.c_str());
      this->VolumeSelector->GetSelected()->SetDescription("Planning image/volume; created by PerkStation module");
      this->VolumeSelector->GetSelected()->Modified();

      this->VolumeSelector->UpdateMenu();

      // set up the image on secondary monitor    
      this->SecondaryMonitor->SetupImageData();
      
      if (!planningVolumePreExists)
        {
        // repopulate/enable/disable controls now that volume has been loaded
        this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->ShowUserInterface();
        }
      else
        {
        this->ResetAndStartNewExperiment();
        }

      }
    else if (this->WizardWidget->GetWizardWorkflow()->GetCurrentStep() == this->ValidateStep)
      {
      // what if the volume selected is actually planning
      // in that case just return
      if (n->GetPlanningVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        if (strcmpi(n->GetPlanningVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Planning");          
          return;
          }
        }

      // in case, the validation volume already exists
      if (n->GetValidationVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        // in case, the selected volume is actually validation volume
        if (strcmpi(n->GetValidationVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Validation");  
          return;
          }
        // this implies that a validation volume already existed but now there is new image/volume chosen as validation volume in validate step
        }
           

      // validate volume set
      n->SetValidationVolumeRef(this->VolumeSelector->GetSelected()->GetID());    
      n->SetValidationVolumeNode(vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(n->GetValidationVolumeRef())));
      n->SetVolumeInUse("Validation");

      const char *strName = this->VolumeSelector->GetSelected()->GetName();
      std::string strPlan = std::string(strName) + "-Validation";
      this->VolumeSelector->GetSelected()->SetName(strPlan.c_str());
      this->VolumeSelector->GetSelected()->SetDescription("Validation image/volume; created by PerkStation module");
      this->VolumeSelector->GetSelected()->Modified();

      this->VolumeSelector->UpdateMenu();


      }
    else
      {
    
      // situation, when the user in neither calibration step, nor validation step
      // he could be fiddling around with GUI, and be in either planning or insertion step or evaluation step

      // what if the volume selected is actually validation
      // in that case just return
      if (n->GetValidationVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        if (strcmpi(n->GetValidationVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Validation");          
          return;
          }
        }

      bool planningVolumePreExists = false;

      // what if the volume selected is actually planning
      // in that case just return
      if (n->GetPlanningVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        if (strcmpi(n->GetPlanningVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Planning");          
          return;
          }
        // this implies that a planning volume already existed but now there is new image/volume chosen as planning volume in calibrate step
        planningVolumePreExists = true;
        }
      
      // calibrate/planning volume set
      n->SetPlanningVolumeRef(this->VolumeSelector->GetSelected()->GetID());
      n->SetPlanningVolumeNode(vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(n->GetPlanningVolumeRef())));
      n->SetVolumeInUse("Planning");
     
      vtkMRMLScalarVolumeDisplayNode *node = NULL;
      vtkSetAndObserveMRMLNodeMacro(node, n->GetPlanningVolumeNode()->GetScalarVolumeDisplayNode());

       // set up the image on secondary monitor    
      this->SecondaryMonitor->SetupImageData();

      if (!planningVolumePreExists)
        {
        // bring the wizard GUI back to Calibrate step
        // the volume selection has changed/added, so make sure that the wizard is in the intial calibration state!
        while (this->WizardWidget->GetWizardWorkflow()->GetCurrentState()!= this->WizardWidget->GetWizardWorkflow()->GetInitialState())
          {
          this->WizardWidget->GetWizardWorkflow()->AttemptToGoToPreviousStep();
          }
        this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->ShowUserInterface();
        }
      else
        {
        this->ResetAndStartNewExperiment();
        }
      }    
    */
    }

  



}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::UpdateGUI ()
{
  vtkMRMLTRProstateBiopsyModuleNode* n = this->GetMRMLNode();
  if (n != NULL)
    {
    // set GUI widgest from parameter node
    /*this->ConductanceScale->SetValue(n->GetConductance());
    
    this->TimeStepScale->SetValue(n->GetTimeStep());
    
    this->NumberOfIterationsScale->SetValue(n->GetNumberOfIterations());

    this->UseImageSpacing->SetSelectedState(n->GetUseImageSpacing());*/
    
    // update in the mrml scene with latest transform that we have
    // TODO:
    //this->GetLogic()->GetMRMLScene()->UpdateNode();
    }
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::UpdateAll()
{
        // Fill in
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::Enter()
{
  ////slicerCerr("vtkTRProstateBiopsyGUI::Enter();"); 


  /*
  //----------------------------------------------------------------
  // Create calibration and target fiducials
  vtkTRProstateBiopsyLogic *logic = this->GetLogic();


  if (!logic->GetCalibrationFiducialListNodeID() &&
      !logic->GetTargetFiducialListNodeID())
    {
    vtkSlicerApplication *app =
      static_cast<vtkSlicerApplication *>(this->GetApplication());

    // Get a pointer to the Fiducials module
    vtkSlicerFiducialsGUI *fidGUI
      = static_cast<vtkSlicerFiducialsGUI *>(
        app->GetModuleGUIByName("Fiducials"));

    fidGUI->Enter();

    // Create New Fiducial lists for Prostate Module
    vtkSlicerFiducialsLogic *fidLogic =
      static_cast<vtkSlicerFiducialsLogic *>(fidGUI->GetLogic());

    // Loop twice, once for calibration fiducials, again for targets
    for (int listCount = 0; listCount < 2; listCount++)
      {
      static char *listName[] = { "F", "T" };

      vtkMRMLFiducialListNode *newList =
        fidLogic->AddFiducialList();

      if (newList != NULL)
        {
        // Change the name of the list
        newList->SetName(
          this->GetMRMLScene()->GetUniqueNameByString(listName[listCount]));

        fidGUI->SetFiducialListNodeID(newList->GetID());
        newList->Delete();
        }
      else
        {
        vtkErrorMacro("Unable to add a new fid list\n");
        }

      // now get the newly active node
      char *fidNodeID = fidGUI->GetFiducialListNodeID();
      vtkMRMLFiducialListNode *fidNode =
        static_cast<vtkMRMLFiducialListNode *>(
          this->GetMRMLScene()->GetNodeByID(fidNodeID));
       
      if (fidNode == NULL)
        {
        vtkErrorMacro("Unable to create fiducial list node");
        return;
        }

      if (listCount == 0)
        {
        logic->SetCalibrationFiducialListNodeID(fidNodeID);
        logic->SetCalibrationFiducialListNode(fidNode); 
        }
      else if (listCount == 1)
        {
        logic->SetTargetFiducialListNodeID(fidNodeID);
        logic->SetTargetFiducialListNode(fidNode); 
        }
      }

    // start with the calibration fiducials
    if (logic->GetCalibrationFiducialListNodeID())
      {
      fidGUI->SetFiducialListNodeID(logic->GetCalibrationFiducialListNodeID());
      }

    }
*/
    
    

  // to initiate a new MRML node here
  // the reason, why this is being done here, is because, otherwise, usually a GUI event would trigger creation of MRML node
  // e.g. creation of new node parameters from parameter selector widget
  // or addition of new volume to scene, which in turn, generates SelectedNode event in VolumeSelector widget, which in turns calls UpdateMRML()
  // which then generates the node if it is not present.

  // for this particular module, since the volume could be added by "Browse" on Calibrate Wizard GUI, which in turn, calls methods in Logic to open a volume
  // wherein 'Logic' assumes that there already exists a MRML node to operate upon...
  // hence the need to create a MRML node at the very beginning before anything else happens
  vtkMRMLTRProstateBiopsyModuleNode* n = this->GetMRMLNode();
  
  if (n == NULL)
    {
    // no parameter node selected yet, create new
    this->TRNodeSelector->SetSelectedNew("vtkMRMLTRProstateBiopsyModuleNode");
    this->TRNodeSelector->ProcessNewNodeCommand("vtkMRMLTRProstateBiopsyModuleNode", "TransRectal");
    n = vtkMRMLTRProstateBiopsyModuleNode::SafeDownCast(this->TRNodeSelector->GetSelected());

    // set an observe new node in Logic
    this->Logic->SetAndObserveTRProstateBiopsyModuleNode(n);
    vtkSetAndObserveMRMLNodeMacro(this->MRMLNode, n);

     // add MRMLFiducialListNode to the scene
    this->GetLogic()->GetMRMLScene()->SaveStateForUndo();

    //read the config file
    this->GetLogic()->ReadConfigFile();

    // set up fiducial lists
    n->SetupTargetingFiducialsList();

    // one node for calibration fiducials list
    this->GetLogic()->GetMRMLScene()->AddNode(this->MRMLNode->GetCalibrationFiducialsList());
    // as many nodes as there are targeting fiducials lists
    for (unsigned int i = 0; i < this->MRMLNode->GetNumberOfNeedles(); i++)
      {
      this->GetLogic()->GetMRMLScene()->AddNode(this->MRMLNode->GetTargetingFiducialsList(i));
      }


    }

  

  // first try to open up communication at usb port with optical encoder for readout
  this->Logic->InitializeOpticalEncoder();

  if (this->Logic->IsOpticalEncoderInitialized())
      {
      // start the timer event for connecting to and reading optical encoders
      const char *tmpId = vtkKWTkUtilities::CreateTimerHandler(this->GetApplication()->GetMainInterp(),
                                        500,
                                        this,
                                        "OpticalEncoderTimerEvent");
      this->OpticalTimerEventId = new char[strlen(tmpId)+1];
      strcpy(this->OpticalTimerEventId, tmpId);
      //this->Script("after idle \"%s OpticalEncoderTimerEvent\"", this->GetTclName());
    
      // Note: the above call will call the OpticalEncoderTimerEvent function only once,
      // to maintain timer event, after 'delay' has to be called inside the function to make it recursive call
      }

    
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::Exit()
{
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::OpticalEncoderTimerEvent()
{
  if (this->TimerProcessing)
      return;
  
  // sort of locking mechanism
  this->TimerProcessing = true;

  // If you change the next values, you'll need to re-calculate all the indexes!
  static char sDevRotation[]  = "Device Rotation: Something"; // don't change this!
  static char sNeedleAngle[]  = "Needle Angle: Something"; // don't change this!

  // Device Rotation
  int devRotationValue = this->Logic->GetUSBEncoder()->GetChannelValue(0);

  if (devRotationValue<0) 
    { // Encoder off?
    // how to stop timer??
    //USBencoderTimer.Stop();
    this->Logic->GetUSBEncoder()->CloseUSBdevice();
    
    // to update text actors here
    //Text_DeviceRotation->SetLabel("No optical encoder found!");
    //Text_NeedleAngle->SetLabel("");
    // to upate secondary monitor window!
    /*if (ProjectionWindow) 
      {
      ProjectionWindow->SetDisplayDev("");
      ProjectionWindow->SetDisplayAngle("0");
      }*/
    this->TimerProcessing = false;
    return;
    }
  
  if (this->Logic->GetDeviceRotation() != devRotationValue) 
    {// value changed from last read out
    this->Logic->SetDeviceRotation (devRotationValue);
    int devRotationMaxValue = this->Logic->GetUSBEncoder()->GetChannelMaxValue(0); // wrap around value
    for (int i=17;i<25;i++) 
        sDevRotation[i]=0;
    if (devRotationValue>=0 && devRotationMaxValue>=0) 
      {
      // DevRotationValue: -180..180, 0=0
      int devRotationConverted = devRotationValue;
      if (devRotationConverted> (4*360) ) 
        { // I don't like this
        devRotationConverted=devRotationConverted-devRotationMaxValue;
        }
            //_itoa(DevRotationConverted/4, sDevRotation+17, 10);
      sprintf(sDevRotation+17,"%0.1f",devRotationConverted/4.0);
      }
    
    // update text actor
    //Text_DeviceRotation->SetLabel(sDevRotation);

    // update on secondary window
    /*if (ProjectionWindow) 
      {
      ProjectionWindow->SetDisplayDev(sDevRotation);
      }*/
    }

    
    
  // Needle Angle

  int needleAngleValue = this->Logic->GetUSBEncoder()->GetChannelValue(1);

  if (needleAngleValue<0) 
    { // Encoder off?
    // how to stop timer??
    //USBencoderTimer.Stop();
    this->Logic->GetUSBEncoder()->CloseUSBdevice();
    // update text actors for gui display
    //Text_DeviceRotation->SetLabel("No optical encoder found!");
    //Text_NeedleAngle->SetLabel("");
    this->TimerProcessing = false;
    return;
    }

  
  if (this->Logic->GetNeedleAngle() != needleAngleValue) 
    {
    this->Logic->SetNeedleAngle(needleAngleValue);
    int needleAngleMaxValue = this->Logic->GetUSBEncoder()->GetChannelMaxValue(1); // wrap around value

    for (int i=14;i<22;i++) 
        sNeedleAngle[i]=0;
    
    if (needleAngleValue>=0 && needleAngleMaxValue>=0) 
      {
      // NeedleAngleValue: 17.5..40 -- calculations from Axel
      double usb_y=14.5; // device specific
                
      // Cheat or not to cheat?
      double alpha_ini_degree = 37; // where the counter is 0

      // get the angle calculated from registration
      /*if (RegistrationAngle>0) 
        {
        alpha_ini_degree = RegistrationAngle; // where the counter is 0
        }
      */
      double resolution = 0.125; // depends on optical encoder
      double alpha_ini = alpha_ini_degree/180.0*vtkMath::Pi();
      double x_ini = usb_y/tan(alpha_ini);

      int needleAngleConverted = needleAngleValue;

      if (needleAngleConverted> (needleAngleMaxValue/2)) 
        { // I don't like this
        needleAngleConverted = needleAngleConverted-needleAngleMaxValue;
        }
      
      double x_delta = needleAngleConverted*resolution;
      double usb_x = x_ini-x_delta;
      double needle_angle_rad = atan2(usb_y,usb_x);
      double needle_angle = needle_angle_rad*180.0/vtkMath::Pi();
            //_itoa((int)needle_angle, sNeedleAngle+14, 10);
            //_itoa(NeedleAngleValue, sNeedleAngle+14, 10);
      sprintf(sNeedleAngle+14,"%0.1f",needle_angle);
      }
    
    // to update text actors on gui
    //Text_NeedleAngle->SetLabel(sNeedleAngle);
      
    // update on secondary monitor window
    /*if (ProjectionWindow) 
        {
        ProjectionWindow->SetDisplayAngle(sNeedleAngle);
        }*/
    }

  this->TimerProcessing = false;
  // start the timer event for connecting to and reading optical encoders
      const char *tmpId = vtkKWTkUtilities::CreateTimerHandler(this->GetApplication()->GetMainInterp(),
                                        500,
                                        this,
                                        "OpticalEncoderTimerEvent");
      this->OpticalTimerEventId = new char[strlen(tmpId)+1];
      strcpy(this->OpticalTimerEventId, tmpId);
}
//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::SaveExperimentButtonCallback(const char *fileName)
{
    ofstream file(fileName);

    //this->SaveExperiment(file);
    
    file.close();
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::SaveExperiment(ostream & of)
{
  // save the volume information
  this->SaveVolumesToExperimentFile(of);

  for (int phase = vtkTRProstateBiopsyLogic::WorkPhase::Calibration; phase < vtkTRProstateBiopsyLogic::WorkPhase::NumPhases; phase++)
    {
    this->WizardSteps[phase]->SaveToExperimentFile(of);
    } 
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::LoadExperiment(istream &file)
{
  // load the volumes
  this->LoadVolumesAsInExperimentFile(file);

  for (int phase = vtkTRProstateBiopsyLogic::WorkPhase::Calibration; phase < vtkTRProstateBiopsyLogic::WorkPhase::NumPhases; phase++)
    {
    this->WizardSteps[phase]->LoadFromExperimentFile(file);
    }
}
//-----------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::LoadExperimentButtonCallback(const char *fileName)
{
    ifstream file(fileName);    
    //this->LoadExperiment(file);
    file.close();
}
//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::BuildGUI()
{
  // ---
  // MODULE GUI FRAME 
  // create a page

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();


   //register MRML PS node
  vtkMRMLTRProstateBiopsyModuleNode* TRNode = vtkMRMLTRProstateBiopsyModuleNode::New();
  this->Logic->GetMRMLScene()->RegisterNodeClass(TRNode);
  TRNode->Delete();

  this->UIPanel->AddPage("TRProstateBiopsy", "TRProstateBiopsy", NULL);
  vtkKWWidget *modulePage =  this->UIPanel->GetPageWidget("TRProstateBiopsy");

  // help page
  const char *help = "**Trans-Rectal Prostate Biopsy Module:** **Under Construction** This module provides an interface for using MRI to localize prostate biopsy targets and to perform the biopsies with an intra-rectal robot.";
  const char *about = "This module was developed at Quee's University, Canada and is supported supported by NA-MIC, NAC, BIRN, NCIGT, and the Slicer Community. See <a>http://www.slicer.org</a> for details.";
  this->BuildHelpAndAboutFrame(modulePage, help, about);
  vtkKWLabel *NACLabel = vtkKWLabel::New();
  NACLabel->SetParent ( this->GetLogoFrame() );
  NACLabel->Create();
  NACLabel->SetImageToIcon ( this->GetAcknowledgementIcons()->GetNACLogo() );

  vtkKWLabel *NAMICLabel = vtkKWLabel::New();
  NAMICLabel->SetParent ( this->GetLogoFrame() );
  NAMICLabel->Create();
  NAMICLabel->SetImageToIcon ( this->GetAcknowledgementIcons()->GetNAMICLogo() );    

  vtkKWLabel *NCIGTLabel = vtkKWLabel::New();
  NCIGTLabel->SetParent ( this->GetLogoFrame() );
  NCIGTLabel->Create();
  NCIGTLabel->SetImageToIcon ( this->GetAcknowledgementIcons()->GetNCIGTLogo() );

  vtkKWLabel *BIRNLabel = vtkKWLabel::New();
  BIRNLabel->SetParent ( this->GetLogoFrame() );
  BIRNLabel->Create();
  BIRNLabel->SetImageToIcon ( this->GetAcknowledgementIcons()->GetBIRNLogo() );

  app->Script ( "grid %s -row 0 -column 0 -padx 2 -pady 2 -sticky w", NAMICLabel->GetWidgetName());
  app->Script ("grid %s -row 0 -column 1 -padx 2 -pady 2 -sticky w",  NACLabel->GetWidgetName());
  app->Script ( "grid %s -row 1 -column 0 -padx 2 -pady 2 -sticky w", BIRNLabel->GetWidgetName());
  app->Script ( "grid %s -row 1 -column 1 -padx 2 -pady 2 -sticky w", NCIGTLabel->GetWidgetName());
  
  NACLabel->Delete();
  NAMICLabel->Delete();
  NCIGTLabel->Delete();
  BIRNLabel->Delete();


  this->BuildGUIForWorkPhaseFrame();
  this->BuildGUIForModuleParamsVolumeAndExperimentFrame();
  this->BuildGUIForWizardFrame();
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::BuildGUIForWizardFrame()
{
  vtkKWWidget *page = this->UIPanel->GetPageWidget("TRProstateBiopsy");
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

  // ----------------------------------------------------------------
  // WIZARD FRAME         
  // ----------------------------------------------------------------

  vtkSlicerModuleCollapsibleFrame *wizardFrame = 
      vtkSlicerModuleCollapsibleFrame::New();
  wizardFrame->SetParent(page);
  wizardFrame->Create();
  wizardFrame->SetLabelText("Wizard");
  wizardFrame->ExpandFrame();

  app->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
              wizardFrame->GetWidgetName(), 
              page->GetWidgetName());
   
  this->WizardWidget->SetParent(wizardFrame->GetFrame());
  this->WizardWidget->Create();
  this->WizardWidget->GetSubTitleLabel()->SetHeight(1);
  this->WizardWidget->SetClientAreaMinimumHeight(80);
  this->WizardWidget->NextButtonVisibilityOn();
  this->WizardWidget->BackButtonVisibilityOn();
  this->WizardWidget->OKButtonVisibilityOff();
  this->WizardWidget->CancelButtonVisibilityOff();
  this->WizardWidget->FinishButtonVisibilityOff();
  this->WizardWidget->HelpButtonVisibilityOn();

  app->Script("pack %s -side top -anchor nw -fill both -expand y",
              this->WizardWidget->GetWidgetName());

  wizardFrame->Delete();

  // -----------------------------------------------------------------
  // Add the steps to the workflow

  vtkKWWizardWorkflow *wizardWorkflow = 
    this->WizardWidget->GetWizardWorkflow();

 
  // -----------------------------------------------------------------
  // Calibration step

  
  if (!this->WizardSteps[vtkTRProstateBiopsyLogic::Calibration])
    {
    this->WizardSteps[vtkTRProstateBiopsyLogic::Calibration] = vtkTRProstateBiopsyCalibrationStep::New();
    }
  wizardWorkflow->AddStep(this->WizardSteps[vtkTRProstateBiopsyLogic::Calibration]);
  

  // -----------------------------------------------------------------
  // Segmentation step

  
  if (!this->WizardSteps[vtkTRProstateBiopsyLogic::Segmentation])
    {
    this->WizardSteps[vtkTRProstateBiopsyLogic::Segmentation] = vtkTRProstateBiopsySegmentationStep::New();
    }
  wizardWorkflow->AddNextStep(this->WizardSteps[vtkTRProstateBiopsyLogic::Segmentation]);



  // -----------------------------------------------------------------
  // Targeting step

  if (!this->WizardSteps[vtkTRProstateBiopsyLogic::Targeting])
    {
    this->WizardSteps[vtkTRProstateBiopsyLogic::Targeting] = vtkTRProstateBiopsyTargetingStep::New();
    }
  wizardWorkflow->AddNextStep(this->WizardSteps[vtkTRProstateBiopsyLogic::Targeting]);

  // -----------------------------------------------------------------
  // Config File step

  if (!this->WizardSteps[vtkTRProstateBiopsyLogic::Verification])
    {
    this->WizardSteps[vtkTRProstateBiopsyLogic::Verification] = vtkTRProstateBiopsyVerificationStep::New();
    }
  wizardWorkflow->AddNextStep(this->WizardSteps[vtkTRProstateBiopsyLogic::Verification]);
  // -----------------------------------------------------------------
  // Set GUI/Logic to each step and add to workflow

  for (int i = 0; i < vtkTRProstateBiopsyLogic::NumPhases; i++)
    {
    this->WizardSteps[i]->SetGUI(this);
    //this->WizardSteps[i]->SetLogic(this->Logic);

    // Set color for the wizard title:

    this->WizardSteps[i]->SetTitleBackgroundColor(0.8, 0.8, 1.0);
    this->WizardSteps[i]->SetTitleBackgroundColor(WorkPhaseColor[i][0],
                                                 WorkPhaseColor[i][1],
                                                  WorkPhaseColor[i][2]);
    
    }


  // -----------------------------------------------------------------
  // Initial and finish step

  wizardWorkflow->SetFinishStep(this->WizardSteps[vtkTRProstateBiopsyLogic::Verification]);
  wizardWorkflow->CreateGoToTransitionsToFinishStep();
  wizardWorkflow->SetInitialStep(this->WizardSteps[vtkTRProstateBiopsyLogic::Calibration]);

  // -----------------------------------------------------------------
  // Show the user interface
  //this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->ShowUserInterface();
}


//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::BuildGUIForModuleParamsVolumeAndExperimentFrame()
{
   vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
   vtkKWWidget *modulePage =  this->UIPanel->GetPageWidget("TRProstateBiopsy");

   // collapsible? frame for volume node selection, and parameters selection?
  vtkSlicerModuleCollapsibleFrame *loadSaveExptFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  loadSaveExptFrame->SetParent(modulePage);
  loadSaveExptFrame->Create();
  loadSaveExptFrame->SetLabelText("Experiment frame");
  loadSaveExptFrame->ExpandFrame();  
  app->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
              loadSaveExptFrame->GetWidgetName(), modulePage->GetWidgetName());
  

  // Create the frame
  vtkKWFrame *volSelFrame = vtkKWFrame::New();
  volSelFrame->SetParent(loadSaveExptFrame->GetFrame());
  volSelFrame->Create();     
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    volSelFrame->GetWidgetName());

  //MRML node
  this->TRNodeSelector->SetNodeClass("vtkMRMLTRProstateBiopsyModuleNode", NULL, NULL, "TR Parameters");
  this->TRNodeSelector->SetNewNodeEnabled(1);
  this->TRNodeSelector->NoneEnabledOn();
  this->TRNodeSelector->SetShowHidden(1);
  this->TRNodeSelector->SetParent( volSelFrame );
  this->TRNodeSelector->Create();
  this->TRNodeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
  this->TRNodeSelector->UpdateMenu();

  this->TRNodeSelector->SetBorderWidth(2);
  this->TRNodeSelector->SetLabelText( "TR Biopsy Parameters");
  this->TRNodeSelector->SetBalloonHelpString("select a PS node from the current mrml scene.");
  app->Script("pack %s -side left -anchor w -padx 2 -pady 4", 
                this->TRNodeSelector->GetWidgetName());

  //input volume selector
  this->VolumeSelector->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, NULL);
  this->VolumeSelector->SetParent( volSelFrame );
  this->VolumeSelector->Create();
  this->VolumeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
  this->VolumeSelector->UpdateMenu();

  this->VolumeSelector->SetBorderWidth(2);
  this->VolumeSelector->SetLabelText( "Active Volume: ");
  this->VolumeSelector->SetBalloonHelpString("select an input volume from the current mrml scene.");
  app->Script("pack %s -side top -anchor e -padx 2 -pady 4", 
                this->VolumeSelector->GetWidgetName());


  // Create the frame
  vtkKWFrame *loadSaveFrame = vtkKWFrame::New();
  loadSaveFrame->SetParent(loadSaveExptFrame->GetFrame());
  loadSaveFrame->Create();     
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    loadSaveFrame->GetWidgetName());

    // create the load file dialog button
  if (!this->LoadExperimentFileButton)
    {
    this->LoadExperimentFileButton = vtkKWLoadSaveButton::New();
    }
  if (!this->LoadExperimentFileButton->IsCreated())
    {
    this->LoadExperimentFileButton->SetParent(loadSaveFrame);
    this->LoadExperimentFileButton->Create();
    this->LoadExperimentFileButton->SetBorderWidth(2);
    this->LoadExperimentFileButton->SetReliefToRaised();       
    this->LoadExperimentFileButton->SetHighlightThickness(2);
    this->LoadExperimentFileButton->SetBackgroundColor(0.85,0.85,0.85);
    this->LoadExperimentFileButton->SetActiveBackgroundColor(1,1,1);
    this->LoadExperimentFileButton->SetText("Load experiment");
    this->LoadExperimentFileButton->SetImageToPredefinedIcon(vtkKWIcon::IconPresetLoad);
    this->LoadExperimentFileButton->SetBalloonHelpString("Click to load a previous experiment file");
    this->LoadExperimentFileButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    this->LoadExperimentFileButton->TrimPathFromFileNameOff();
    this->LoadExperimentFileButton->SetMaximumFileNameLength(256);
    this->LoadExperimentFileButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode
    this->LoadExperimentFileButton->GetLoadSaveDialog()->SetFileTypes("{{XML File} {.xml}} {{All Files} {*.*}}");      
    }
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                        this->LoadExperimentFileButton->GetWidgetName());

  // create the load file dialog button
  if (!this->SaveExperimentFileButton)
    {
    this->SaveExperimentFileButton = vtkKWLoadSaveButton::New();
    }
  if (!this->SaveExperimentFileButton->IsCreated())
    {
    this->SaveExperimentFileButton->SetParent(loadSaveFrame);
    this->SaveExperimentFileButton->Create();
    this->SaveExperimentFileButton->SetText("Save experiment");
    this->SaveExperimentFileButton->SetBorderWidth(2);
    this->SaveExperimentFileButton->SetReliefToRaised();       
    this->SaveExperimentFileButton->SetHighlightThickness(2);
    this->SaveExperimentFileButton->SetBackgroundColor(0.85,0.85,0.85);
    this->SaveExperimentFileButton->SetActiveBackgroundColor(1,1,1);               
    this->SaveExperimentFileButton->SetImageToPredefinedIcon(vtkKWIcon::IconFloppy);
    this->SaveExperimentFileButton->SetBalloonHelpString("Click to save experiment in a file");
    this->SaveExperimentFileButton->GetLoadSaveDialog()->SaveDialogOn(); // save mode
    this->SaveExperimentFileButton->TrimPathFromFileNameOff();
    this->SaveExperimentFileButton->SetMaximumFileNameLength(256);
    this->SaveExperimentFileButton->GetLoadSaveDialog()->SetFileTypes("{{XML File} {.xml}} {{All Files} {*.*}}");      
    this->SaveExperimentFileButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    }
  this->Script("pack %s -side top -anchor ne -padx 2 -pady 2", 
                        this->SaveExperimentFileButton->GetWidgetName());


  volSelFrame->Delete();
  loadSaveFrame->Delete();
  loadSaveExptFrame->Delete();

}
//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::BuildGUIForHelpFrame()
{
  vtkSlicerApplication *app =
    static_cast<vtkSlicerApplication *>(this->GetApplication());

  vtkKWWidget *page = this->UIPanel->GetPageWidget("TRProstateBiopsy");

  // Define your help text here.
  const char *help = 
    "The **TRProstateBiopsy Module** is for MR-Guided Trans-Rectal "
    "Prostate Biopsies: It interfaces with the MRI compatible robot "
    "developed by Axel Krieger at JHU.  Module and Logic mainly coded "
    "by David Gobbi and Csaba Csoma"; 

  // ----------------------------------------------------------------
  // HELP FRAME         
  // ----------------------------------------------------------------
  vtkSlicerModuleCollapsibleFrame *TRProstateBiopsyHelpFrame =
    vtkSlicerModuleCollapsibleFrame::New();

  TRProstateBiopsyHelpFrame->SetParent(page);
  TRProstateBiopsyHelpFrame->Create();
  TRProstateBiopsyHelpFrame->CollapseFrame();
  TRProstateBiopsyHelpFrame->SetLabelText("Help");
  app->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
      TRProstateBiopsyHelpFrame->GetWidgetName(), page->GetWidgetName());

  // configure the parent classes help text widget
  this->HelpText->SetParent(TRProstateBiopsyHelpFrame->GetFrame());
  this->HelpText->Create();
  this->HelpText->SetHorizontalScrollbarVisibility(0);
  this->HelpText->SetVerticalScrollbarVisibility(1);
  this->HelpText->GetWidget()->SetText(help);
  this->HelpText->GetWidget()->SetReliefToFlat();
  this->HelpText->GetWidget()->SetWrapToWord();
  this->HelpText->GetWidget()->ReadOnlyOn();
  this->HelpText->GetWidget()->QuickFormattingOn();
  this->HelpText->GetWidget()->SetBalloonHelpString("");
  app->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 4",
      this->HelpText->GetWidgetName());

  TRProstateBiopsyHelpFrame->Delete();
}

//---------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::BuildGUIForWorkPhaseFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget("TRProstateBiopsy");
  
  vtkSlicerModuleCollapsibleFrame *workphaseFrame =
          vtkSlicerModuleCollapsibleFrame::New();
  workphaseFrame->SetParent(page);
  workphaseFrame->Create();
  workphaseFrame->SetLabelText("Workphase Frame");
  workphaseFrame->ExpandFrame();
  app->Script("pack %s -side top -anchor center -fill x -padx 2 -pady 2 -in %s",
              workphaseFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Frames

  vtkKWFrame *buttonFrame = vtkKWFrame::New();
  buttonFrame->SetParent(workphaseFrame->GetFrame());
  buttonFrame->Create();

  app->Script("pack %s -side top -anchor center -fill x -padx 2 -pady 1",
                buttonFrame->GetWidgetName());

  // -----------------------------------------
  // Work Phase Transition Buttons Frame

  this->WorkPhaseButtonSet = vtkKWRadioButtonSet::New();
  this->WorkPhaseButtonSet->SetParent(buttonFrame);
  this->WorkPhaseButtonSet->Create();
  this->WorkPhaseButtonSet->PackHorizontallyOn();
  this->WorkPhaseButtonSet->SetBorderWidth(2);
  this->WorkPhaseButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(vtkTRProstateBiopsyLogic::NumPhases);
  this->WorkPhaseButtonSet->SetWidgetsInternalPadX(2);  
  this->WorkPhaseButtonSet->UniformColumnsOn();
  this->WorkPhaseButtonSet->UniformRowsOn();
  
  for (int i = 0; i < vtkTRProstateBiopsyLogic::NumPhases; i ++)
    {
    this->WorkPhaseButtonSet->AddWidget(i);
    //this->WorkPhaseButtonSet->GetWidget(i)->SetWidth(16);
    this->WorkPhaseButtonSet->GetWidget(i)->SetText(WorkPhaseStr[i]);
    this->WorkPhaseButtonSet->GetWidget(i)->SetBackgroundColor(WorkPhaseColor[i][0],WorkPhaseColor[i][1],WorkPhaseColor[i][2]);
    this->WorkPhaseButtonSet->GetWidget(i)->SetActiveBackgroundColor(WorkPhaseColor[i][0],WorkPhaseColor[i][1],WorkPhaseColor[i][2]);
    this->WorkPhaseButtonSet->GetWidget(i)->SetBorderWidth(2);
    this->WorkPhaseButtonSet->GetWidget(i)->SetReliefToSunken();
    this->WorkPhaseButtonSet->GetWidget(i)->SetOffReliefToRaised();
    this->WorkPhaseButtonSet->GetWidget(i)->SetHighlightThickness(2);
    this->WorkPhaseButtonSet->GetWidget(i)->IndicatorVisibilityOff();

    }

  //this->WorkPhaseButtonSet->GetWidget(0)->SetReliefToSunken();
  this->WorkPhaseButtonSet->GetWidget(0)->SetSelectedState(1);
  
  this->Script("pack %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->WorkPhaseButtonSet->GetWidgetName());
  
  workphaseFrame->Delete();
  buttonFrame->Delete();
}

//----------------------------------------------------------------------------
int vtkTRProstateBiopsyGUI::ChangeWorkPhase(int phase, int fChangeWizard)
{
  ////slicerCerr("vtkTRProstateBiopsyGUI::ChangeWorkPhase");

  if (!this->Logic->SwitchWorkPhase(phase)) // Set next phase
    {
    cerr << "ChangeWorkPhase: Cannot make transition!" << endl;
    return 0;
    }
  
  for (int i = 0; i < vtkTRProstateBiopsyLogic::NumPhases; i ++)
    {
    vtkKWRadioButton *pb = this->WorkPhaseButtonSet->GetWidget(i);
    if (i == this->Logic->GetCurrentPhase())
      {
      
      pb->SetSelectedState(1);
      
      }
    else if (this->Logic->IsPhaseTransitable(i))
      {
      pb->SetReliefToGroove();
      pb->SetStateToNormal();
      pb->SetBackgroundColor(WorkPhaseColor[i][0],
                             WorkPhaseColor[i][1],
                             WorkPhaseColor[i][2]);
      }
    else
      {
      pb->SetReliefToGroove();
      pb->SetStateToDisabled();
      pb->SetBackgroundColor(WorkPhaseColorDisabled[i][0],
                             WorkPhaseColorDisabled[i][1],
                             WorkPhaseColorDisabled[i][2]);
      }
    }

  // Switch Wizard Frame
  if (fChangeWizard)
    {
    vtkKWWizardWorkflow *wizard = 
          this->WizardWidget->GetWizardWorkflow();
        
    int toStep = this->Logic->GetCurrentPhase();
    int fromStep = this->Logic->GetPrevPhase();
        
    int steps =  toStep - fromStep;
    if (steps > 0)
      {
      for (int i = 0; i < steps; i++) 
        {
        wizard->AttemptToGoToNextStep();
        }
      }
    else
      {
      steps = -steps;
      for (int i = 0; i < steps; i++)
        {
        wizard->AttemptToGoToPreviousStep();
        }
      }
    wizard->GetCurrentStep()->ShowUserInterface();
    }

  return 1;
}



//----------------------------------------------------------------------------
char *vtkTRProstateBiopsyGUI::CreateFileName()
{
    // create a folder for current date, if not already created
    // get the system calendar time
    std::time_t tt = std::time(0);
    // convert it into tm struct
    std::tm ttm = *std::localtime(&tt);
    // extract the values for day, month, year
    char dirName[9] = "";
    sprintf(dirName, "%4d%2d%2d", ttm.tm_year, ttm.tm_mon+1, ttm.tm_mday);
    if (access(dirName,0) ==0)
        {
        struct stat status;
        stat( dirName, &status );
        if ( status.st_mode & S_IFDIR )
        {
        // directory exists
        }
        else
        {
        //create directory
        }
        }
    else
        {
        //create directory
            ::CreateDirectory(dirName,NULL);
        }

    // after directory has been created
    // create the file name
    // extract time in hrs, mins, secs  


    // generate the unique file name
    // to take into account
    // 1) student/trainee name
    // 2) current time of experiment
    // 3) trial number
    //
  return dirName;
}

//----------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::SaveVolumesToExperimentFile(ostream& of)
{
  vtkMRMLTRProstateBiopsyModuleNode *mrmlNode = this->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }  
  
  this->GetLogic()->SaveVolumesToExperimentFile(of);  

}


//----------------------------------------------------------------------------
void vtkTRProstateBiopsyGUI::LoadVolumesAsInExperimentFile(istream &file)
{
}
