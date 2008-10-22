/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkPerkStationModuleGUI.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <io.h>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().

#include "vtkObjectFactory.h"

#include "vtkPerkStationModuleGUI.h"
#include "vtkPerkStationModuleLogic.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLFiducialListNode.h"

#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkMRMLLayoutNode.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSlicerModuleCollapsibleFrame.h"

#include "vtkCommand.h"
#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkWin32OpenGLRenderWindow.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageChangeInformation.h"
#include "vtkImageReslice.h"
#include "vtkMatrixToHomogeneousTransform.h"

#include "vtkKWApplication.h"
#include "vtkKWLabel.h"
#include "vtkKWWidget.h"
#include "vtkKWFrame.h"
#include "vtkKWComboBox.h"
#include "vtkKWComboBoxWithLabel.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWLoadSaveButton.h"

#include "vtkPerkStationCalibrateStep.h"
#include "vtkPerkStationPlanStep.h"
#include "vtkPerkStationInsertStep.h"
#include "vtkPerkStationValidateStep.h"
#include "vtkPerkStationEvaluateStep.h"

#include "vtkPerkStationSecondaryMonitor.h"




//------------------------------------------------------------------------------
vtkPerkStationModuleGUI* vtkPerkStationModuleGUI::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkPerkStationModuleGUI");
  if(ret)
    {
      return (vtkPerkStationModuleGUI*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkPerkStationModuleGUI;
}


//----------------------------------------------------------------------------
vtkPerkStationModuleGUI::vtkPerkStationModuleGUI()
{
  // gui elements
  this->VolumeSelector = vtkSlicerNodeSelectorWidget::New();
  this->PSNodeSelector = vtkSlicerNodeSelectorWidget::New();

  
  this->LoadExperimentFileButton = NULL;
  this->SaveExperimentFileButton = NULL;

  //logic and mrml nodes
  this->Logic = NULL;
  this->MRMLNode = NULL;

  this->WizardWidget = vtkKWWizardWidget::New();

  // wizard workflow
  this->CalibrateStep = NULL;
  this->PlanStep = NULL;
  this->InsertStep = NULL;
  this->ValidateStep = NULL;
  this->EvaluateStep = NULL;

  // secondary monitor
  this->SecondaryMonitor = NULL;

  this->SecondaryMonitor = vtkPerkStationSecondaryMonitor::New();
  this->SecondaryMonitor->SetGUI(this);
  this->SecondaryMonitor->Initialize();

  // state descriptors
  this->ModeListMenu = NULL;
  this->Mode = vtkPerkStationModuleGUI::ModeId::Training;
  this->StateButtonSet = NULL;
  this->State = vtkPerkStationModuleGUI::StateId::Calibrate;  

  this->Built = false;
}

//----------------------------------------------------------------------------
vtkPerkStationModuleGUI::~vtkPerkStationModuleGUI()
{
  
  this->SetLogic (NULL);
  vtkSetMRMLNodeMacro(this->MRMLNode, NULL);

  if (this->ModeListMenu)
    {
    this->ModeListMenu->SetParent(NULL);
    this->ModeListMenu->Delete();
    this->ModeListMenu = NULL;
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
  if (this->StateButtonSet)
    {
    this->StateButtonSet->SetParent(NULL);
    this->StateButtonSet->Delete();
    this->StateButtonSet = NULL;
    }

  if ( this->VolumeSelector ) 
    {
    this->VolumeSelector->SetParent(NULL);
    this->VolumeSelector->Delete();
    this->VolumeSelector = NULL;
    }
  if ( this->PSNodeSelector ) 
    {
    this->PSNodeSelector->SetParent(NULL);
    this->PSNodeSelector->Delete();
    this->PSNodeSelector = NULL;
    }

  if (this->WizardWidget)
    {
    this->WizardWidget->Delete();
    this->WizardWidget = NULL;
    }

  if (this->CalibrateStep)
    {
    this->CalibrateStep->Delete();
    this->CalibrateStep = NULL;
    }

  if (this->PlanStep)
    {
    this->PlanStep->Delete();
    this->PlanStep = NULL;
    }

  if (this->InsertStep)
    {
    this->InsertStep->Delete();
    this->InsertStep = NULL;
    }

  if (this->ValidateStep)
    {
    this->ValidateStep->Delete();
    this->ValidateStep = NULL;
    }

  if (this->EvaluateStep)
    {
    this->EvaluateStep->Delete();
    this->EvaluateStep = NULL;
    }
  
}

//----------------------------------------------------------------------------
void vtkPerkStationModuleGUI::PrintSelf(ostream& os, vtkIndent indent)
{
  
}

//------------------------------------------------------------------------------
void vtkPerkStationModuleGUI::Enter()
{
  if ( this->GetApplicationGUI() != NULL )
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));
    p->RepackMainViewer ( vtkMRMLLayoutNode::SlicerLayoutOneUpSliceView, "Red");       
    }
 
}
//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::AddGUIObservers ( ) 
{
  this->RemoveGUIObservers();

  vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  // add listener to main slicer's red slice view
  appGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  // add listener to own render window created for secondary monitor
  this->SecondaryMonitor->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  // wizard workflow
  this->WizardWidget->GetWizardWorkflow()->AddObserver( vtkKWWizardWorkflow::CurrentStateChangedEvent, static_cast<vtkCommand *>(this->GUICallbackCommand));
  

  if (this->GetMode() == vtkPerkStationModuleGUI::ModeId::Clinical)
      this->SecondaryMonitor->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::KeyPressEvent, (vtkCommand *)this->GUICallbackCommand);

  this->VolumeSelector->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );

  this->PSNodeSelector->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );    

  this->ModeListMenu->GetWidget()->GetMenu()->AddObserver ( vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  if (this->LoadExperimentFileButton)
    {
    this->LoadExperimentFileButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->SaveExperimentFileButton)
    {
    this->SaveExperimentFileButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
}



//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::RemoveGUIObservers ( )
{
  //this->GetApplicationGUI()->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  //this->GetApplicationGUI()->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver(vtkCommand::CharEvent, (vtkCommand *)this->GUICallbackCommand);
  
  this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
  this->SecondaryMonitor->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);

  this->WizardWidget->GetWizardWorkflow()->RemoveObserver( static_cast<vtkCommand *>(this->GUICallbackCommand));

  this->VolumeSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );

  this->PSNodeSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );    

  this->ModeListMenu->GetWidget()->GetMenu()->RemoveObservers ( vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  if (this->LoadExperimentFileButton)
    {
    this->LoadExperimentFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->SaveExperimentFileButton)
    {
    this->SaveExperimentFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
}



//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::ProcessGUIEvents ( vtkObject *caller,
                                           unsigned long event,
                                           void *callData ) 
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);
  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {    
    this->CalibrateStep->ProcessImageClickEvents(caller, event, callData);  
    this->PlanStep->ProcessImageClickEvents(caller, event, callData);
    this->ValidateStep->ProcessImageClickEvents(caller, event, callData);
    }
  else if (strcmp(eventName, "KeyPressEvent") == 0)
    {
    this->CalibrateStep->ProcessKeyboardEvents(caller, event, callData);
    }
  else if (this->WizardWidget->GetWizardWorkflow() == vtkKWWizardWorkflow::SafeDownCast(caller) && event == vtkKWWizardWorkflow::CurrentStateChangedEvent)
    {
    // TO DO        
    if (this->WizardWidget->GetWizardWorkflow()->GetCurrentStep() == this->CalibrateStep)
      {
      this->State = vtkPerkStationModuleGUI::StateId::Calibrate;      
      }
    else if (this->WizardWidget->GetWizardWorkflow()->GetCurrentStep() == this->PlanStep)
      {
      this->State = vtkPerkStationModuleGUI::StateId::Plan;  
      }
    else if (this->WizardWidget->GetWizardWorkflow()->GetCurrentStep() == this->InsertStep)
      {
      this->State = vtkPerkStationModuleGUI::StateId::Insert;
      }
    else if (this->WizardWidget->GetWizardWorkflow()->GetCurrentStep() == this->ValidateStep)
      {
      this->State = vtkPerkStationModuleGUI::StateId::Validate;
      }
    else if (this->WizardWidget->GetWizardWorkflow()->GetCurrentStep() == this->EvaluateStep)
      {
      this->State = vtkPerkStationModuleGUI::StateId::Evaluate;
      }
    this->StateButtonSet->GetWidget(this->State)->SetSelectedState(1);

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
  
  else
    {  
     
    vtkSlicerNodeSelectorWidget *selector = vtkSlicerNodeSelectorWidget::SafeDownCast(caller);  
    if (selector == this->VolumeSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent && this->VolumeSelector->GetSelected() != NULL) 
      { 
      this->UpdateMRML();
      }
    
    if (selector == this->PSNodeSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  && this->PSNodeSelector->GetSelected() != NULL) 
      { 
      vtkMRMLPerkStationModuleNode* n = vtkMRMLPerkStationModuleNode::SafeDownCast(this->PSNodeSelector->GetSelected());
      this->Logic->SetAndObservePerkStationModuleNode(n);
      vtkSetAndObserveMRMLNodeMacro( this->MRMLNode, n);
      this->UpdateGUI();
      }
      
    if (this->ModeListMenu->GetWidget()->GetMenu() ==  vtkKWMenu::SafeDownCast(caller) &&     event == vtkKWMenu::MenuItemInvokedEvent)   
      {
        const char *val = this->ModeListMenu->GetWidget()->GetValue();
        if (!strcmp(val, "CLINICAL") && this->Mode == vtkPerkStationModuleGUI::ModeId::Training)
          {
          this->Mode = vtkPerkStationModuleGUI::ModeId::Clinical;
          this->SetUpPerkStationMode();
          // disable the option of changing the Mode now on
          this->ModeListMenu->GetWidget()->SetEnabled(0);
          }
        else if (!strcmp(val, "TRAINING") && this->Mode == vtkPerkStationModuleGUI::ModeId::Clinical)
          {
          this->Mode = vtkPerkStationModuleGUI::ModeId::Training;
          this->SetUpPerkStationMode();       
          // disable the option of changing the Mode now on
          this->ModeListMenu->GetWidget()->SetEnabled(0);
          }
        
      }
    
    }
}

//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::UpdateMRML ()
{
  vtkMRMLPerkStationModuleNode* n = this->GetMRMLNode();
  
  if (n == NULL)
    {
    // no parameter node selected yet, create new
    this->PSNodeSelector->SetSelectedNew("vtkMRMLPerkStationModuleNode");
    this->PSNodeSelector->ProcessNewNodeCommand("vtkMRMLPerkStationModuleNode", "PS");
    n = vtkMRMLPerkStationModuleNode::SafeDownCast(this->PSNodeSelector->GetSelected());

    // set an observe new node in Logic
    this->Logic->SetAndObservePerkStationModuleNode(n);
    vtkSetAndObserveMRMLNodeMacro(this->MRMLNode, n);

    // add the transform of mrml node to the MRML scene
    this->GetLogic()->GetMRMLScene()->SaveStateForUndo();
    this->GetLogic()->GetMRMLScene()->AddNode(this->MRMLNode->GetCalibrationMRMLTransformNode());
    vtkMRMLLinearTransformNode *node = NULL;
    vtkIntArray* nodeEvents = vtkIntArray::New();
    nodeEvents->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
    vtkSetAndObserveMRMLNodeMacro(node, this->MRMLNode->GetCalibrationMRMLTransformNode());
    vtkSetAndObserveMRMLNodeEventsMacro(node,this->MRMLNode->GetCalibrationMRMLTransformNode(),nodeEvents);
    // add MRMLFiducialListNode to the scene
    this->GetLogic()->GetMRMLScene()->SaveStateForUndo();
    this->GetLogic()->GetMRMLScene()->AddNode(this->MRMLNode->GetPlanMRMLFiducialListNode());


    }

  // save node parameters for Undo
  this->GetLogic()->GetMRMLScene()->SaveStateForUndo(n);
   

  /*n->SetConductance(this->ConductanceScale->GetValue());
  
  n->SetTimeStep(this->TimeStepScale->GetValue());
  
  n->SetNumberOfIterations((int)floor(this->NumberOfIterationsScale->GetValue()));

  n->SetUseImageSpacing(this->UseImageSpacing->GetState());*/
  
  if (this->VolumeSelector->GetSelected() != NULL)
    {    
    
    // see in what state wizard gui is, act accordingly
    if (this->WizardWidget->GetWizardWorkflow()->GetCurrentStep() == this->CalibrateStep)
      {

      if (n->GetPlanningVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        if (strcmpi(n->GetPlanningVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Planning");          
          }
        return;
        }

      // calibrate/planning volume set
      n->SetPlanningVolumeRef(this->VolumeSelector->GetSelected()->GetID());
      n->SetPlanningVolumeNode(vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(n->GetPlanningVolumeRef())));
      n->SetVolumeInUse("Planning");
      

      vtkMRMLScalarVolumeDisplayNode *node = NULL;
      vtkSetAndObserveMRMLNodeMacro(node, n->GetPlanningVolumeNode()->GetScalarVolumeDisplayNode());

      // set up the image on secondary monitor    
      this->SecondaryMonitor->SetupImageData();
      
      // repopulate/enable/disable controls now that volume has been loaded
      this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->ShowUserInterface();

      }
    else if (this->WizardWidget->GetWizardWorkflow()->GetCurrentStep() == this->ValidateStep)
      {
      if (n->GetValidationVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        if (strcmpi(n->GetValidationVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Validation");          
          }
        return;
        }
           

      // validate volume set
      n->SetValidationVolumeRef(this->VolumeSelector->GetSelected()->GetID());    
      n->SetValidationVolumeNode(vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(n->GetValidationVolumeRef())));
      n->SetVolumeInUse("Validation");

      }
    else
      {
    
      // situation, when the user in neither calibration step, nor validation step
      // he could be fiddling around with GUI, and be in either planning or insertion step or evaluation step

      // only to handle case when planning volume has not been set so far, in which case we set the planning volume

      if (n->GetPlanningVolumeRef()!=NULL && this->VolumeSelector->GetSelected()->GetID()!=NULL)
        {
        if (strcmpi(n->GetPlanningVolumeRef(),this->VolumeSelector->GetSelected()->GetID()) == 0)
          {
          n->SetVolumeInUse("Planning");          
          }
        return;
        }

      // calibrate/planning volume set
      n->SetPlanningVolumeRef(this->VolumeSelector->GetSelected()->GetID());
      n->SetPlanningVolumeNode(vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(n->GetPlanningVolumeRef())));
      n->SetVolumeInUse("Planning");
     
      vtkMRMLScalarVolumeDisplayNode *node = NULL;
      vtkSetAndObserveMRMLNodeMacro(node, n->GetPlanningVolumeNode()->GetScalarVolumeDisplayNode());

       // set up the image on secondary monitor    
      this->SecondaryMonitor->SetupImageData();

      // bring the wizard GUI back to Calibrate step
      // the volume selection has changed/added, so make sure that the wizard is in the intial calibration state!
      while (this->WizardWidget->GetWizardWorkflow()->GetCurrentState()!= this->WizardWidget->GetWizardWorkflow()->GetInitialState())
        {
        this->WizardWidget->GetWizardWorkflow()->AttemptToGoToPreviousStep();
        }
      this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->ShowUserInterface();
      }    
    
    }

  



}

//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::UpdateGUI ()
{
  vtkMRMLPerkStationModuleNode* n = this->GetMRMLNode();
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
void vtkPerkStationModuleGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event,
                                            void *callData ) 
{
  // if parameter node has been changed externally, update GUI widgets with new values
  vtkMRMLPerkStationModuleNode* node = vtkMRMLPerkStationModuleNode::SafeDownCast(caller);
  vtkMRMLLinearTransformNode *transformNode = vtkMRMLLinearTransformNode::SafeDownCast(caller);
  vtkMRMLScalarVolumeDisplayNode *displayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(caller);

  if (node != NULL && this->GetMRMLNode() == node ) 
  {
    this->UpdateGUI();
  }

  if (displayNode != NULL && this->GetMRMLNode()->GetPlanningVolumeNode()->GetScalarVolumeDisplayNode() == displayNode && event == vtkCommand::ModifiedEvent)
    {
    this->GetSecondaryMonitor()->UpdateImageDisplay();
    }
  /*if (transformNode == this->GetMRMLNode()->GetCalibrationMRMLTransformNode() && event == vtkMRMLTransformableNode::TransformModifiedEvent)
    {
    // this is when the user modifies the calibration transform matrix from "Transforms" module
    // update our internal CalibrationTransform
    // set node parameters from MRML scene
  
    // get the scene collection
    vtkCollection* collection = this->GetLogic()->GetMRMLScene()->GetNodesByName("PerkStationCalibrationTransform");
    int nItems = collection->GetNumberOfItems();
    
    // traverse through scene collection to get the transform node
    if (nItems > 0)
      {
      // pick the very first, well, in fact, there should be only 1
      // i.e. if nItems > 1, there is something wrong
      transformNode = vtkMRMLLinearTransformNode::SafeDownCast(collection->GetItemAsObject(0));        
//    vtkMatrix4x4* matrix = transformNode->GetMatrixTransformToParent();
      this->GetMRMLNode()->SetTransformNodeMatrix(transformNode->GetMatrixTransformToParent());
      //matrix->Delete();
      // actually transform the volume!
      // TO DO:
      }
    }*/
    

 
}




//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::BuildGUI ( ) 
{
  if ( this->Built)
      return;

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

  //register MRML PS node
  vtkMRMLPerkStationModuleNode* PSNode = vtkMRMLPerkStationModuleNode::New();
  this->Logic->GetMRMLScene()->RegisterNodeClass(PSNode);
  PSNode->Delete();

  this->UIPanel->AddPage ( "PerkStationModule", "PerkStationModule", NULL );
  vtkKWWidget *modulePage =  this->UIPanel->GetPageWidget("PerkStationModule");
  
  // ---
  // MODULE GUI FRAME 
  // ---
  // Define your help text and build the help frame here.
  const char *help = "**PERK Station Module:** **Under Construction** Use this module to perform image overlay guided percutaneous interventions ....";
  const char *about = "This work was supported by NA-MIC, NAC, BIRN, NCIGT, and the Slicer Community. See <a>http://www.slicer.org</a> for details. ";
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "PerkStationModule" );
  this->BuildHelpAndAboutFrame ( page, help, about );
  
  // add individual collapsible pages/frames


  // Mode collapsible frame with combo box label
  vtkSlicerModuleCollapsibleFrame *modeFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  modeFrame->SetParent(modulePage);
  modeFrame->Create();
  modeFrame->SetLabelText("Mode frame");
  modeFrame->ExpandFrame();
  
  app->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
              modeFrame->GetWidgetName(), modulePage->GetWidgetName());
  
  // add combo box in the frame
  this->ModeListMenu = vtkKWMenuButtonWithLabel::New();
  this->ModeListMenu->SetParent(modeFrame->GetFrame());
  this->ModeListMenu->Create();
  this->ModeListMenu->SetLabelText("Mode");
  this->ModeListMenu->SetBalloonHelpString("This indicates whether the PERK Station module is being used in CLINICAL or TRAINING mode");
  this->ModeListMenu->GetWidget()->GetMenu()->AddRadioButton("TRAINING");
  this->ModeListMenu->GetWidget()->GetMenu()->AddRadioButton("CLINICAL");
  this->ModeListMenu->GetWidget()->SetValue("TRAINING");
 

  app->Script("pack %s -side top -anchor nw -expand n -padx 4 -pady 4", 
    this->ModeListMenu->GetWidgetName(), modeFrame->GetWidgetName());

  modeFrame->Delete();

  // Work phase collapsible frame with push buttons/radio buttons set
  vtkSlicerModuleCollapsibleFrame *workPhaseFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  workPhaseFrame->SetParent(modulePage);
  workPhaseFrame->Create();
  workPhaseFrame->SetLabelText("Work phase frame");
  workPhaseFrame->ExpandFrame();
  
  app->Script("pack %s -side top -fill x -expand y -anchor w -padx 4 -pady 2",
              workPhaseFrame->GetWidgetName(), modulePage->GetWidgetName());
 
  // add radio buttons set
  this->StateButtonSet = vtkKWRadioButtonSet::New();
  this->StateButtonSet->SetParent(workPhaseFrame->GetFrame());
  this->StateButtonSet->Create();
  this->StateButtonSet->SetBorderWidth(2);
  //this->StateButtonSet->SetReliefToGroove();
  this->StateButtonSet->SetPackHorizontally(1);
  this->StateButtonSet->SetWidgetsInternalPadX(2);
  this->StateButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(5);
  const char *buffer[]= {"Calibrate", "Plan", "Insert", "Validate", "Evaluate"};
  for (int id = 0; id < 5; id++)
    {
    vtkKWRadioButton *radiob = this->StateButtonSet->AddWidget(id);
    radiob->SetText(buffer[id]);
    radiob->SetBorderWidth(2);
    radiob->SetReliefToSunken();
    radiob->SetOffReliefToRaised();
    radiob->SetHighlightThickness(2);
    radiob->IndicatorVisibilityOff();
    //radiob->SetStateToDisabled();
    radiob->SetBackgroundColor(0.85,0.85,0.85);
    radiob->SetActiveBackgroundColor(1,1,1);
    }
  this->StateButtonSet->GetWidget(0)->SetSelectedState(1);
 

  app->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
    this->StateButtonSet->GetWidgetName());

  workPhaseFrame->Delete();
  
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
  this->PSNodeSelector->SetNodeClass("vtkMRMLPerkStationModuleNode", NULL, NULL, "PS Parameters");
  this->PSNodeSelector->SetNewNodeEnabled(1);
  this->PSNodeSelector->NoneEnabledOn();
  this->PSNodeSelector->SetShowHidden(1);
  this->PSNodeSelector->SetParent( volSelFrame );
  this->PSNodeSelector->Create();
  this->PSNodeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
  this->PSNodeSelector->UpdateMenu();

  this->PSNodeSelector->SetBorderWidth(2);
  this->PSNodeSelector->SetLabelText( "PERK Parameters");
  this->PSNodeSelector->SetBalloonHelpString("select a PS node from the current mrml scene.");
  app->Script("pack %s -side left -anchor w -padx 2 -pady 4", 
                this->PSNodeSelector->GetWidgetName());

  //input volume selector
  this->VolumeSelector->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, NULL);
  this->VolumeSelector->SetParent( volSelFrame );
  this->VolumeSelector->Create();
  this->VolumeSelector->SetMRMLScene(this->Logic->GetMRMLScene());
  this->VolumeSelector->UpdateMenu();

  this->VolumeSelector->SetBorderWidth(2);
  this->VolumeSelector->SetLabelText( "Input Volume: ");
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

  // Wizard collapsible frame with individual steps inside
  this->WizardFrame = vtkSlicerModuleCollapsibleFrame::New();
  this->WizardFrame->SetParent(modulePage);
  this->WizardFrame->Create();
  this->WizardFrame->SetLabelText("Wizard");
  this->WizardFrame->ExpandFrame();

  app->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
              this->WizardFrame->GetWidgetName(), 
              modulePage->GetWidgetName());

    // individual page/collapsible frame with their own widgets inside:
  this->WizardWidget->SetParent(this->WizardFrame->GetFrame());
  this->WizardWidget->Create();  
  this->WizardWidget->GetSubTitleLabel()->SetHeight(1);
  this->WizardWidget->SetClientAreaMinimumHeight(320);
  //this->WizardWidget->SetButtonsPositionToTop();
  this->WizardWidget->HelpButtonVisibilityOn();
  this->GetApplication()->Script("pack %s -side top -anchor nw -fill both -expand y",
              this->WizardWidget->GetWidgetName());
  

  vtkNotUsed(vtkKWWizardWidget *wizard_widget = this->WizardWidget;);
  vtkKWWizardWorkflow *wizard_workflow = this->WizardWidget->GetWizardWorkflow();
  
  // -----------------------------------------------------------------
  // Calibration step

  if (!this->CalibrateStep)
    {
    this->CalibrateStep = vtkPerkStationCalibrateStep::New();
    this->CalibrateStep->SetGUI(this);
    }

  wizard_workflow->AddStep(this->CalibrateStep);
  

  
  // -----------------------------------------------------------------
  // Plan step

  if (!this->PlanStep)
    {
    this->PlanStep = vtkPerkStationPlanStep::New();
    this->PlanStep->SetGUI(this);
    }
  wizard_workflow->AddNextStep(this->PlanStep);

  // -----------------------------------------------------------------
  // Insert step

  if (!this->InsertStep)
    {
    this->InsertStep = vtkPerkStationInsertStep::New();
    this->InsertStep->SetGUI(this);
    }
  wizard_workflow->AddNextStep(this->InsertStep);

  // -----------------------------------------------------------------
  // Validate step

  if (!this->ValidateStep)
    {
    this->ValidateStep = vtkPerkStationValidateStep::New();
    this->ValidateStep->SetGUI(this);
    }
  wizard_workflow->AddNextStep(this->ValidateStep);


  // -----------------------------------------------------------------
  // Evaluate step

  if (!this->EvaluateStep)
    {
    this->EvaluateStep = vtkPerkStationEvaluateStep::New();
    this->EvaluateStep->SetGUI(this);
    }
  wizard_workflow->AddNextStep(this->EvaluateStep);


  switch (this->Mode)
    {
    case vtkPerkStationModuleGUI::ModeId::Training:
        wizard_workflow->SetFinishStep(this->EvaluateStep);
        break;

    case vtkPerkStationModuleGUI::ModeId::Clinical:     
        wizard_workflow->SetFinishStep(this->ValidateStep);
        
        break;

    }

  // -----------------------------------------------------------------
  // Initial step and transitions  
  wizard_workflow->CreateGoToTransitionsToFinishStep();
  wizard_workflow->SetInitialStep(this->CalibrateStep);    
  
 
  this->SetUpPerkStationWizardWorkflow();
  
  this->Built = true;
  
}


//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::TearDownGUI() 
{
  if (this->CalibrateStep)
    {
    this->CalibrateStep->SetGUI(NULL);
    }

  if (this->PlanStep)
    {
    this->PlanStep->SetGUI(NULL);
    }

  if (this->InsertStep)
    {
    this->InsertStep->SetGUI(NULL);
    }

  if (this->ValidateStep)
    {
    this->ValidateStep->SetGUI(NULL);
    }

  if (this->EvaluateStep)
    {
    this->EvaluateStep->SetGUI(NULL);
    }
}


//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::SetUpPerkStationMode()
{
  // reset flip/scale/rotate/translate on secondary display
  this->SecondaryMonitor->ResetCalibration();

  // note this can only be done during or before calibrate step
  // the following tasks need to be done
  // Wizard workflow wizard needs to be re-done i.e. steps/transitions as well
  this->SetUpPerkStationWizardWorkflow(); 
 

}
//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::SetUpPerkStationWizardWorkflow()
{  
  

  vtkNotUsed(vtkKWWizardWidget *wizard_widget = this->WizardWidget;);
  vtkKWWizardWorkflow *wizard_workflow = this->WizardWidget->GetWizardWorkflow();
  

  
  switch (this->Mode)
    {
    case vtkPerkStationModuleGUI::ModeId::Training:
        wizard_workflow->SetFinishStep(this->EvaluateStep);
        break;

    case vtkPerkStationModuleGUI::ModeId::Clinical:
        // create transition/program validate button such that it doesn't go further
        wizard_workflow->SetFinishStep(this->ValidateStep);
        this->SecondaryMonitor->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::AnyEvent, (vtkCommand *)this->GUICallbackCommand);
        break;

    }
  
  this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->ShowUserInterface();
  
}
//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::RenderSecondaryMonitor()
{
  if (this->SecondaryMonitor->IsSecondaryMonitorActive())
    {
    vtkRenderer *ren1 = vtkRenderer::New();
    vtkWin32OpenGLRenderWindow *renWin = vtkWin32OpenGLRenderWindow::New();
    renWin->AddRenderer(ren1);
    renWin->SetBorders(0);
    
    // positioning window
    int virtualScreenPos[2];
    this->SecondaryMonitor->GetVirtualScreenCoord(virtualScreenPos[0], virtualScreenPos[1]);
    renWin->SetPosition(virtualScreenPos[0], virtualScreenPos[1]);

    // window size
    unsigned int winSize[2];
    this->SecondaryMonitor->GetScreenDimensions(winSize[0], winSize[1]);
    renWin->SetSize(winSize[0], winSize[1]);
    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

    vtkImageMapper *imageMapper = vtkImageMapper::New();
    imageMapper->SetColorWindow(255);
    imageMapper->SetColorLevel(127.5);
    
    vtkDICOMImageReader *reader = vtkDICOMImageReader::New();
    reader->SetFileName("C:\\Work\\SliceVolReg\\Data\\Pt1\\SR301\\I0022_1");
    reader->Update();
    
    float *direction = reader->GetImageOrientationPatient();

    int extent[6];
    double spacing[3];
    double origin[3];
    reader->GetOutput()->GetWholeExtent(extent);
    reader->GetOutput()->GetSpacing(spacing);
    reader->GetOutput()->GetOrigin(origin);

    /*
    vtkImageData *image = this->GetApplicationGUI()->GetMainSliceGUI0()->GetLogic()->GetImageData();
    imageMapper->SetInput(image);
    */

    vtkImageData *image = reader->GetOutput();
    

    
    double center[3];
    center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]); 
    center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]); 
    center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]); 

    vtkImageReslice *reslice = vtkImageReslice::New();
    reslice->SetInput(image);
    reslice->SetOutputDimensionality(2);
    reslice->SetOutputExtent(0, winSize[0]-1, 0, winSize[1]-1,0,0);
    //reslice->SetOutputSpacing(spacing);
    //reslice->SetOutputOrigin(center[0], center[1],0);

    //reslice->SetResliceTransform(transform);
    reslice->SetInterpolationModeToLinear();

    reslice->Update();

    



    imageMapper->SetInput(reslice->GetOutput());

    
    vtkActor2D *imageActor = vtkActor2D::New();
    imageActor->SetMapper(imageMapper);
    //imageActor->SetPosition(winSize[0]/2, winSize[1]/2, 0);

    ren1->AddActor(imageActor);
    renWin->Render();

    //iren->Start();
    }
  else
    {
    vtkErrorMacro("No secondary monitor detected");
    }

  
}

//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::ResetAndStartNewExperiment()
{
  // objectives:
  // 1) Reset individual work phase steps to bring to fresh state, who are in turn, responsible for reseting MRML node parameters
  // 2) Go back to the first step in GUI in workflow widget

  // 1) Reset individual work phase steps to bring to fresh state, who are in turn, responsible for reseting MRML node parameters
  this->CalibrateStep->Reset();
  this->PlanStep->Reset();
  this->InsertStep->Reset();
  this->ValidateStep->Reset();
  this->EvaluateStep->Reset();

  // 2)Go back to the first step in GUI in workflow widget
  while (this->WizardWidget->GetWizardWorkflow()->GetCurrentState()!= this->WizardWidget->GetWizardWorkflow()->GetInitialState())
    {
    this->WizardWidget->GetWizardWorkflow()->AttemptToGoToPreviousStep();
    }
  this->WizardWidget->GetWizardWorkflow()->GetCurrentStep()->ShowUserInterface();



}
//----------------------------------------------------------------------------
void vtkPerkStationModuleGUI::SaveExperiment(ostream& of)
{
  if (!this->GetMRMLNode())
      return;

 

  switch(this->Mode)
    {
    case vtkPerkStationModuleGUI::ModeId::Training:
        this->GetMRMLNode()->WriteXML(of, 0);
        break;
    
    case vtkPerkStationModuleGUI::ModeId::Clinical:
        // save information about image volumes used
        // save information about calibration
        this->CalibrateStep->SaveCalibration(of);
        // save information about planning
        this->PlanStep->SavePlanning(of);
        // save information about insertion
        this->InsertStep->SaveInsertion(of);
        // save information about validation 
        this->ValidateStep->SaveValidation(of);
        break;
    }
  
}

//----------------------------------------------------------------------------
void vtkPerkStationModuleGUI::LoadExperiment(istream &file)
{
}
//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::SaveExperimentButtonCallback(const char *fileName)
{
    ofstream file(fileName);

    this->SaveExperiment(file);
    
    file.close();
}

//-----------------------------------------------------------------------------
void vtkPerkStationModuleGUI::LoadExperimentButtonCallback(const char *fileName)
{
    ifstream file(fileName);    
    this->LoadExperiment(file);
    file.close();
}

//-----------------------------------------------------------------------------------
char *vtkPerkStationModuleGUI::CreateFileName()
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
