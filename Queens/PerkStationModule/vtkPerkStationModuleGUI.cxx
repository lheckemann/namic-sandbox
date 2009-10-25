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


#include "itkMetaDataObject.h"

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
#include "vtkKWFrameWithLabel.h"
#include "vtkKWScale.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWMessageDialog.h"

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

  this->LoadPlanningVolumeButton = NULL;
  this->LoadValidationVolumeButton = NULL;

  this->LoadExperimentFileButton = NULL;
  this->SaveExperimentFileButton = NULL;

  //logic and mrml nodes
  this->Logic = NULL;
  this->MRMLNode = NULL;

  this->WizardWidget = vtkKWWizardWidget::New();

  // wizard workflow
  this->WizardFrame = NULL;  
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
  this->DisplayVolumeLevelValue = NULL;
  this->DisplayVolumeWindowValue = NULL;

  this->Built = false;
  this->SliceOffset = 0;

  this->ObserverCount = 0;
}

//----------------------------------------------------------------------------
vtkPerkStationModuleGUI::~vtkPerkStationModuleGUI()
{
  this->RemoveGUIObservers();

  
  vtkSlicerSliceLogic *sliceLogic = this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetLogic();
  if (sliceLogic)
    {  
    sliceLogic->GetSliceNode()->RemoveObservers(vtkCommand::ModifiedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ModeListMenu)
    {
    this->ModeListMenu->Delete();
    this->ModeListMenu = NULL;
    }
  if (this->LoadPlanningVolumeButton)
    {
    this->LoadPlanningVolumeButton->Delete();
    this->LoadPlanningVolumeButton = NULL;
    }
  if (this->LoadValidationVolumeButton)
    {
    this->LoadValidationVolumeButton->Delete();
    this->LoadValidationVolumeButton = NULL;
    }
  if (this->LoadExperimentFileButton)
    {
    this->LoadExperimentFileButton->Delete();
    this->LoadExperimentFileButton = NULL;
    }
  if (this->SaveExperimentFileButton)
    {
    this->SaveExperimentFileButton->Delete();
    this->SaveExperimentFileButton = NULL;
    }
  if (this->StateButtonSet)
    {
    this->StateButtonSet->Delete();
    this->StateButtonSet = NULL;
    }

  if ( this->VolumeSelector ) 
    {
    this->VolumeSelector->Delete();
    this->VolumeSelector = NULL;
    }
  if ( this->PSNodeSelector ) 
    {
    this->PSNodeSelector->Delete();
    this->PSNodeSelector = NULL;
    }

  if (this->WizardFrame)
    {    
    this->WizardFrame->Delete();
    this->WizardFrame = NULL;
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
  if (this->DisplayVolumeLevelValue)
    {
    this->DisplayVolumeLevelValue->Delete();
    this->DisplayVolumeLevelValue = NULL;
    }
  if (this->DisplayVolumeWindowValue)
    {
    this->DisplayVolumeWindowValue->Delete();
    this->DisplayVolumeWindowValue = NULL;
    }

  this->SetLogic (NULL);

  if (this->MRMLNode)
    {
    /*if (this->MRMLNode->GetCalibrationMRMLTransformNode())
        {
        this->Logic->GetMRMLScene()->RemoveNode(this->MRMLNode->GetCalibrationMRMLTransformNode());
        }
    if (this->MRMLNode->GetPlanMRMLFiducialListNode())
        {
        this->Logic->GetMRMLScene()->RemoveNode(this->MRMLNode->GetPlanMRMLFiducialListNode());
        }
    if (this->MRMLNode->GetPlanningVolumeNode())
        {
        this->Logic->GetMRMLScene()->RemoveNode(this->MRMLNode->GetPlanningVolumeNode());
        }
    if (this->MRMLNode->GetValidationVolumeNode())
        {
        this->Logic->GetMRMLScene()->RemoveNode(this->MRMLNode->GetValidationVolumeNode());
        }*/
    // do we need to delete the MRML node here? (The ReferenceCount of this object seems inconsistent, when it's deleted here.)
    //this->MRMLNode->Delete();
    vtkSetMRMLNodeMacro(this->MRMLNode, NULL);
    }
 
  this->SecondaryMonitor->Delete();
  this->SecondaryMonitor = NULL;
  
  
  
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
    nodeEvents->Delete();
    // add MRMLFiducialListNode to the scene
    this->GetLogic()->GetMRMLScene()->SaveStateForUndo();
    this->GetLogic()->GetMRMLScene()->AddNode(this->MRMLNode->GetPlanMRMLFiducialListNode());

    // add listener to the slice logic, so that any time user makes change to slice viewer in laptop, the display needs to be updated on secondary monitor
    // e.g. user may move to a certain slice in a series of slices
  
    vtkSlicerSliceLogic *sliceLogic = this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetLogic();
    if (sliceLogic)
      {  
      sliceLogic->GetSliceNode()->AddObserver(vtkCommand::ModifiedEvent, (vtkCommand *)this->GUICallbackCommand);
      }

    

    /*
    // read the config file
    bool fileFound = this->GetLogic()->ReadConfigFile();
    if (fileFound)
      {
      this->WizardWidget->SetErrorText("Config file found!");
      this->WizardWidget->Update();
      }
    else
      {
      this->WizardWidget->SetErrorText("Config file not found!");
      this->WizardWidget->Update();
      }
    
    */
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

  if (this->GetMode() == vtkPerkStationModuleGUI::ModeId::Clinical)
    {
    this->SecondaryMonitor->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::KeyPressEvent, (vtkCommand *)this->GUICallbackCommand);    
    }
  else
    {
    this->SecondaryMonitor->GetRenderWindowInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
    }

 
  
  // load volumes buttons
  if (this->LoadPlanningVolumeButton)
    {
    this->LoadPlanningVolumeButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->LoadValidationVolumeButton)
    {
    this->LoadValidationVolumeButton->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }

  // wizard workflow
  this->WizardWidget->GetWizardWorkflow()->AddObserver( vtkKWWizardWorkflow::CurrentStateChangedEvent, static_cast<vtkCommand *>(this->GUICallbackCommand));  

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

  
  this->DisplayVolumeLevelValue->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->DisplayVolumeWindowValue->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->ObserverCount++;

}



//---------------------------------------------------------------------------
void vtkPerkStationModuleGUI::RemoveGUIObservers ( )
{
  //this->GetApplicationGUI()->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);
  //this->GetApplicationGUI()->GetMainSliceGUI0()->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver(vtkCommand::CharEvent, (vtkCommand *)this->GUICallbackCommand);
  
  this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
  this->SecondaryMonitor->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand); 
  if (this->GetMode() == vtkPerkStationModuleGUI::ModeId::Clinical)
    {
    this->SecondaryMonitor->GetRenderWindowInteractor()->GetInteractorStyle()->RemoveObserver((vtkCommand *)this->GUICallbackCommand);    
    }

  this->WizardWidget->GetWizardWorkflow()->RemoveObserver( static_cast<vtkCommand *>(this->GUICallbackCommand));

  this->VolumeSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );

  this->PSNodeSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );    

  this->ModeListMenu->GetWidget()->GetMenu()->RemoveObservers ( vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->DisplayVolumeLevelValue->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->DisplayVolumeWindowValue->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);

  if (this->LoadPlanningVolumeButton)
    {
    this->LoadPlanningVolumeButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->LoadValidationVolumeButton)
    {
    this->LoadValidationVolumeButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }

  if (this->LoadExperimentFileButton)
    {
    this->LoadExperimentFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->SaveExperimentFileButton)
    {
    this->SaveExperimentFileButton->GetLoadSaveDialog()->RemoveObservers(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }

  this->ObserverCount--;
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
  else if (this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetLogic()->GetSliceNode() == vtkMRMLSliceNode::SafeDownCast(caller) && event == vtkCommand::ModifiedEvent && !vtkPerkStationModuleLogic::DoubleEqual(this->SliceOffset, this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetLogic()->GetSliceOffset()))
   {
   this->SliceOffset = this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetLogic()->GetSliceOffset();
   this->SecondaryMonitor->UpdateImageDataOnSliceOffset(this->SliceOffset);
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
  else if (this->LoadPlanningVolumeButton && this->LoadPlanningVolumeButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    // load planning volume dialog button
    this->LoadPlanningVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("PSOpenPathPlanVol");
    const char *fileName = this->LoadPlanningVolumeButton->GetLoadSaveDialog()->GetFileName();    
    if ( fileName ) 
      {
      this->LoadPlanningVolumeButton->GetLoadSaveDialog()->SaveLastPathToRegistry("PSOpenPathPlanVol");
      // call the callback function
      this->LoadPlanningVolumeButtonCallback(fileName); 
      this->LoadPlanningVolumeButton->SetText ("Plan volume loaded");
      } 
    }
  else if (this->LoadValidationVolumeButton && this->LoadValidationVolumeButton->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && (event == vtkKWTopLevel::WithdrawEvent))
    {
    // load validation volume dialog button    
    const char *fileName = this->LoadValidationVolumeButton->GetLoadSaveDialog()->GetFileName();    
    if ( fileName ) 
      {      
      // call the callback function
      this->LoadValidationVolumeButtonCallback(fileName);   
      this->LoadValidationVolumeButton->SetText ("Validation volume loaded");
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
  else if (this->DisplayVolumeLevelValue && this->DisplayVolumeLevelValue == vtkKWScaleWithEntry::SafeDownCast(caller) && (event == vtkKWScale::ScaleValueChangedEvent))
    {
    this->GetMRMLNode()->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->SetAutoWindowLevel(0);
    this->GetMRMLNode()->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->SetLevel(this->DisplayVolumeLevelValue->GetValue());
    this->GetMRMLNode()->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->Modified();
    this->SecondaryMonitor->UpdateImageDisplay();
    }
  else if (this->DisplayVolumeWindowValue && this->DisplayVolumeWindowValue == vtkKWScaleWithEntry::SafeDownCast(caller) && (event == vtkKWScale::ScaleValueChangedEvent))
    {
    this->GetMRMLNode()->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->SetAutoWindowLevel(0);
    this->GetMRMLNode()->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->SetWindow(this->DisplayVolumeWindowValue->GetValue());
    this->GetMRMLNode()->GetActiveVolumeNode()->GetScalarVolumeDisplayNode()->Modified();
    this->SecondaryMonitor->UpdateImageDisplay();
    }
  else
    {  
     
    vtkSlicerNodeSelectorWidget *selector = vtkSlicerNodeSelectorWidget::SafeDownCast(caller);  
    if (selector == this->VolumeSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent && this->VolumeSelector->GetSelected() != NULL) 
      { 
      //this->UpdateMRML();
      // handle here, when user changes the selection of the active volume
      // the selection has to be propagated in Slicer's viewers and enviroment
      // the system state has to updated that the volume in use is the one that is selected
      // the secondary monitor has to be updated
      // all this to be done here
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

    // add listener to the slice logic, so that any time user makes change to slice viewer in laptop, the display needs to be updated on secondary monitor
    // e.g. user may move to a certain slice in a series of slices
  
    vtkSlicerSliceLogic *sliceLogic = this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetLogic();
    if (sliceLogic)
      {  
      sliceLogic->GetSliceNode()->AddObserver(vtkCommand::ModifiedEvent, (vtkCommand *)this->GUICallbackCommand);
      }

    

    /*
    // read the config file
    bool fileFound = this->GetLogic()->ReadConfigFile();
    if (fileFound)
      {
      this->WizardWidget->SetErrorText("Config file found!");
      this->WizardWidget->Update();
      }
    else
      {
      this->WizardWidget->SetErrorText("Config file not found!");
      this->WizardWidget->Update();
      }
    
    */
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
      
      // set the window/level controls values from the scalar volume display node
      this->DisplayVolumeLevelValue->SetValue(n->GetPlanningVolumeNode()->GetScalarVolumeDisplayNode()->GetLevel());
      this->DisplayVolumeWindowValue->SetValue(n->GetPlanningVolumeNode()->GetScalarVolumeDisplayNode()->GetWindow());
    
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

       // set the window/level controls values from the scalar volume display node
      this->DisplayVolumeLevelValue->SetValue(n->GetValidationVolumeNode()->GetScalarVolumeDisplayNode()->GetLevel());
      this->DisplayVolumeWindowValue->SetValue(n->GetValidationVolumeNode()->GetScalarVolumeDisplayNode()->GetWindow());


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
  vtkKWFrameWithLabel *loadSaveExptFrame = vtkKWFrameWithLabel::New ( );
  loadSaveExptFrame->SetParent(modulePage);
  loadSaveExptFrame->Create();
  loadSaveExptFrame->SetLabelText("Experiment frame");
  //loadSaveExptFrame->ExpandFrame(); 
  //loadSaveExptFrame->
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
  this->VolumeSelector->SetLabelText( "Active Volume: ");
  this->VolumeSelector->SetBalloonHelpString("select an input volume from the current mrml scene.");
  app->Script("pack %s -side top -anchor e -padx 2 -pady 4", 
                this->VolumeSelector->GetWidgetName());

  // load volume buttons
  vtkKWFrame *loadVolFrame = vtkKWFrame::New();
  loadVolFrame->SetParent(loadSaveExptFrame->GetFrame());
  loadVolFrame->Create();     
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    loadVolFrame->GetWidgetName());

  // create the load file dialog button
  if (!this->LoadPlanningVolumeButton)
    {
    this->LoadPlanningVolumeButton = vtkKWLoadSaveButton::New();
    }
  if (!this->LoadPlanningVolumeButton->IsCreated())
    {
    this->LoadPlanningVolumeButton->SetParent(loadVolFrame);
    this->LoadPlanningVolumeButton->Create();
    this->LoadPlanningVolumeButton->SetBorderWidth(2);
    this->LoadPlanningVolumeButton->SetReliefToRaised();       
    this->LoadPlanningVolumeButton->SetHighlightThickness(2);
    this->LoadPlanningVolumeButton->SetBackgroundColor(0.85,0.85,0.85);
    this->LoadPlanningVolumeButton->SetActiveBackgroundColor(1,1,1);
    this->LoadPlanningVolumeButton->SetText("Load planning volume");
    this->LoadPlanningVolumeButton->SetImageToPredefinedIcon(vtkKWIcon::IconPresetLoad);
    this->LoadPlanningVolumeButton->SetBalloonHelpString("Click to load a planning image or a volume");
    this->LoadPlanningVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    this->LoadPlanningVolumeButton->TrimPathFromFileNameOn();
    this->LoadPlanningVolumeButton->SetMaximumFileNameLength(256);
    this->LoadPlanningVolumeButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode
    this->LoadPlanningVolumeButton->GetLoadSaveDialog()->SetFileTypes("{ {DICOM Files} {*} }");      
    }
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                        this->LoadPlanningVolumeButton->GetWidgetName());

   // create the load file dialog button
  if (!this->LoadValidationVolumeButton)
    {
    this->LoadValidationVolumeButton = vtkKWLoadSaveButton::New();
    }
  if (!this->LoadValidationVolumeButton->IsCreated())
    {
    this->LoadValidationVolumeButton->SetParent(loadVolFrame);
    this->LoadValidationVolumeButton->Create();
    this->LoadValidationVolumeButton->SetBorderWidth(2);
    this->LoadValidationVolumeButton->SetReliefToRaised();       
    this->LoadValidationVolumeButton->SetHighlightThickness(2);
    this->LoadValidationVolumeButton->SetBackgroundColor(0.85,0.85,0.85);
    this->LoadValidationVolumeButton->SetActiveBackgroundColor(1,1,1);
    this->LoadValidationVolumeButton->SetText("Load validation volume");
    this->LoadValidationVolumeButton->SetImageToPredefinedIcon(vtkKWIcon::IconPresetLoad);
    this->LoadValidationVolumeButton->SetBalloonHelpString("Click to load a validation image or a volume");
    this->LoadValidationVolumeButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    this->LoadPlanningVolumeButton->TrimPathFromFileNameOn();
    this->LoadValidationVolumeButton->SetMaximumFileNameLength(256);
    this->LoadValidationVolumeButton->GetLoadSaveDialog()->SaveDialogOff(); // load mode
    this->LoadValidationVolumeButton->GetLoadSaveDialog()->SetFileTypes("{ {DICOM Files} {*} }");
    this->LoadValidationVolumeButton->SetEnabled(0);
    }
  this->Script("pack %s -side top -anchor ne -padx 2 -pady 2", 
                        this->LoadValidationVolumeButton->GetWidgetName());

  vtkKWFrameWithLabel *windowLevelFrame = vtkKWFrameWithLabel::New();
  windowLevelFrame->SetParent(loadSaveExptFrame->GetFrame());
  windowLevelFrame->SetLabelText("Window/Level");
  windowLevelFrame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 0 -pady 2", 
                    windowLevelFrame->GetWidgetName());


  if (!this->DisplayVolumeWindowValue)
    {
    this->DisplayVolumeWindowValue = vtkKWScaleWithEntry::New();
    }
  if (!this->DisplayVolumeWindowValue->IsCreated())
    {
    this->DisplayVolumeWindowValue->SetParent(windowLevelFrame->GetFrame());
    this->DisplayVolumeWindowValue->Create();
    this->DisplayVolumeWindowValue->SetRange(0.0, 8192.0);
    this->DisplayVolumeWindowValue->SetResolution(10.0);
    this->DisplayVolumeWindowValue->SetLength(150);
    this->DisplayVolumeWindowValue->SetLabelText("Window"); 
    this->DisplayVolumeWindowValue->SetLabelPositionToTop();
    this->DisplayVolumeWindowValue->SetEntryPositionToTop();
    }
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", 
                        this->DisplayVolumeWindowValue->GetWidgetName());

  if (!this->DisplayVolumeLevelValue)
    {
    this->DisplayVolumeLevelValue = vtkKWScaleWithEntry::New();
    }
  if (!this->DisplayVolumeLevelValue->IsCreated())
    {
    this->DisplayVolumeLevelValue->SetParent(windowLevelFrame->GetFrame());
    this->DisplayVolumeLevelValue->Create();
    this->DisplayVolumeLevelValue->SetRange(0.0, 2048.0);
    this->DisplayVolumeLevelValue->SetResolution(10.0);
    this->DisplayVolumeLevelValue->SetLength(150);
    this->DisplayVolumeLevelValue->SetLabelText("Level");   
    this->DisplayVolumeLevelValue->SetLabelPositionToTop();
    this->DisplayVolumeLevelValue->SetEntryPositionToTop();
    
    }
  this->Script("pack %s -side top -anchor ne -padx 2 -pady 2", 
                        this->DisplayVolumeLevelValue->GetWidgetName());



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


  windowLevelFrame->Delete();
  volSelFrame->Delete();
  loadSaveFrame->Delete();
  loadSaveExptFrame->Delete();
  loadVolFrame->Delete();
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
  if (this->ModeListMenu)
    {
    this->ModeListMenu->SetParent(NULL);
    }
  if (this->LoadPlanningVolumeButton)
    {
    this->LoadPlanningVolumeButton->SetParent(NULL);
    if(this->LoadPlanningVolumeButton->IsCreated())
      {      
      // vtkKWLoadSaveButton set itself as a parent of its member function LoadSaveDialog automatically when it is created by LoadSaveDialog->SetParent(this),       
      // but it does not do LoadSaveDialog->SetParent(NULL) in its destructor, so we do it here.
      this->LoadPlanningVolumeButton->GetLoadSaveDialog()->SetParent(NULL);
      }
    }
  if (this->LoadValidationVolumeButton)
    {
    this->LoadValidationVolumeButton->SetParent(NULL);
    if(this->LoadValidationVolumeButton->IsCreated())
      {
      // same as the above reason
      this->LoadValidationVolumeButton->GetLoadSaveDialog()->SetParent(NULL);
      }
    }
  if (this->LoadExperimentFileButton)
    {
    this->LoadExperimentFileButton->SetParent(NULL);
    if(this->LoadExperimentFileButton->IsCreated())
      {
      // same as the above reason
      this->LoadExperimentFileButton->GetLoadSaveDialog()->SetParent(NULL);
      }
    }
  if (this->SaveExperimentFileButton)
    {
    this->SaveExperimentFileButton->SetParent(NULL);
    if(this->SaveExperimentFileButton->IsCreated())
      {
      // same as the above reason
      this->SaveExperimentFileButton->GetLoadSaveDialog()->SetParent(NULL);
      }    
    }
  if (this->WizardFrame)
    {    
    this->WizardFrame->SetParent(NULL);    
    }  
  if (this->WizardWidget)
    {
    this->WizardWidget->SetParent(NULL);
    }
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
  if (this->VolumeSelector)
    {
    if(this->VolumeSelector->IsCreated())
      {
      // vtkKWMenuButtonWithSpinButtonsWithLabel::CreateWidget() set itself as a parent of its Widget by Widget->SetParent(this).       
      // But it does not do Widget->SetParent(NULL) in its destructor, so we do it here.
      this->VolumeSelector->GetWidget()->SetParent(NULL);
      // vtkKWWidgetWithLabel::CreateLabel() set itself as a parent of its Widget by label->SetParent(this).       
      // But it does not do label->SetParent(NULL) in its destructor, so we do it here.
      this->VolumeSelector->GetLabel()->SetParent(NULL);
      }
    this->VolumeSelector->SetParent(NULL);
    }
  if (this->PSNodeSelector)
    {
    if(this->PSNodeSelector->IsCreated())
      {
      // same as the above reason
      this->PSNodeSelector->GetWidget()->SetParent(NULL);
      this->PSNodeSelector->GetLabel()->SetParent(NULL);
      }
    this->PSNodeSelector->SetParent(NULL);
    }
  if (this->DisplayVolumeLevelValue)
    {
    this->DisplayVolumeLevelValue->SetParent(NULL);
    }
  if (this->DisplayVolumeWindowValue)
    {
    this->DisplayVolumeWindowValue->SetParent(NULL);
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
        this->SaveVolumeInformation(of);
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
  // reset before you load
  // 1) Reset individual work phase steps to bring to fresh state, who are in turn, responsible for reseting MRML node parameters
  this->CalibrateStep->Reset();
  this->PlanStep->Reset();
  this->InsertStep->Reset();
  this->ValidateStep->Reset();

  // load individual steps
  this->CalibrateStep->LoadCalibration(file);
  this->PlanStep->LoadPlanning(file);
  this->InsertStep->LoadInsertion(file);
  this->ValidateStep->LoadValidation(file);

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

//---------------------------------------------------------------------------------
void vtkPerkStationModuleGUI::SaveVolumeInformation(ostream& of)
{
  vtkMRMLPerkStationModuleNode *mrmlNode = this->GetMRMLNode();
  if (!mrmlNode)
    {
    // TO DO: what to do on failure
    return;
    }  

  vtkMRMLScalarVolumeNode *planVol = mrmlNode->GetPlanningVolumeNode();
  if (planVol == NULL)
    {
    return;
    }
  const itk::MetaDataDictionary &planVolDictionary = planVol->GetMetaDataDictionary();

  std::string tagValue;



  // following information needs to be written to the file

  // 1) Volume location file path
  // 2) Series number from dictionary
  // 3) Series description from dictionary
  // 4) FOR UID from dictionary
  // 5) Origin from node
  // 6) Pixel spacing from node
  
  // entry point
  of << " PlanVolumeFileLocation=\"" ;
//  of <<  << " ";
  of << "\" \n";
  
  // series number
  tagValue.clear(); itk::ExposeMetaData<std::string>( planVolDictionary, "0020|0011", tagValue );
  of << " PlanVolumeSeriesNumber=\"" ;
  of << tagValue << " ";
  of << "\" \n";

  // series description
  tagValue.clear(); itk::ExposeMetaData<std::string>( planVolDictionary, "0008|103e", tagValue );
  of << " PlanVolumeSeriesDescription=\"" ;
  of << tagValue << " ";
  of << "\" \n";
   
  // frame of reference uid
  tagValue.clear(); itk::ExposeMetaData<std::string>( planVolDictionary, "0020|0052", tagValue );
  of << " PlanVolumeFORUID=\"" ;
  of << tagValue << " ";
  of << "\" \n";

  // origin
  of << " PlanVolumeOrigin=\"" ;
  double origin[3];
  planVol->GetOrigin(origin);
  for(int i = 0; i < 3; i++)
      of << origin[i] << " ";
  of << "\" \n";
  

  // spacing
  of << " PlanVolumeSpacing=\"" ;
  double spacing[3];
  planVol->GetSpacing(spacing);
  for(int i = 0; i < 3; i++)
      of << spacing[i] << " ";
  of << "\" \n";


  vtkMRMLScalarVolumeNode *validateVol = mrmlNode->GetValidationVolumeNode();
  if (validateVol == NULL)
    {
    return;
    }
  const itk::MetaDataDictionary &validateVolDictionary = validateVol->GetMetaDataDictionary();

  


  // following information needs to be written to the file

  // 1) Volume location file path
  // 2) Series number from dictionary
  // 3) Series description from dictionary
  // 4) FOR UID from dictionary
  // 5) Origin from node
  // 6) Pixel spacing from node
  
  // entry point
  of << " ValidationVolumeFileLocation=\"" ;
//  of <<  << " ";
  of << "\" \n";
  
  // series number
  tagValue.clear(); itk::ExposeMetaData<std::string>( validateVolDictionary, "0020|0011", tagValue );
  of << " ValidateVolumeSeriesNumber=\"" ;
  of << tagValue << " ";
  of << "\" \n";

  // series description
  tagValue.clear(); itk::ExposeMetaData<std::string>( validateVolDictionary, "0008|103e", tagValue );
  of << " ValidateVolumeSeriesDescription=\"" ;
  of << tagValue << " ";
  of << "\" \n";
   
  // frame of reference uid
  tagValue.clear(); itk::ExposeMetaData<std::string>( validateVolDictionary, "0020|0052", tagValue );
  of << " ValidateVolumeFORUID=\"" ;
  of << tagValue << " ";
  of << "\" \n";

  // origin
  of << " ValidateVolumeOrigin=\"" ; 
  validateVol->GetOrigin(origin);
  for(int i = 0; i < 3; i++)
      of << origin[i] << " ";
  of << "\" \n";
  

  // spacing
  of << " ValidateVolumeSpacing=\"" ; 
  validateVol->GetSpacing(spacing);
  for(int i = 0; i < 3; i++)
      of << spacing[i] << " ";
  of << "\" \n";

  

  

}


//--------------------------------------------------------------------------------
void vtkPerkStationModuleGUI::LoadPlanningVolumeButtonCallback(const char *fileName)
{
  std::string fileString(fileName);
  for (unsigned int i = 0; i < fileString.length(); i++)
    {
    if (fileString[i] == '\\')
      {
      fileString[i] = '/';
      }
    }  

  bool planningVolumePreExists = false;

  // in case the planning volume already exists, it is just that the user has loaded another dicom series
  // as a new planning volume
  // then later in the code, this implies that the experiment has to be started over
  if (this->MRMLNode->GetPlanningVolumeNode())
    {
    planningVolumePreExists = true;
    }

  this->MRMLNode->SetVolumeInUse("Planning");

  vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>(this->GetApplication());
  vtkMRMLScalarVolumeNode *volumeNode = this->GetLogic()->AddVolumeToScene(app,fileString.c_str(), VOL_CALIBRATE_PLAN);
        
  if (volumeNode)
    {
    
    vtkMRMLScalarVolumeDisplayNode *node = NULL;
    vtkSetAndObserveMRMLNodeMacro(node, volumeNode->GetScalarVolumeDisplayNode());

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
        
    // set the window/level controls values from the scalar volume display node
    this->DisplayVolumeLevelValue->SetValue(volumeNode->GetScalarVolumeDisplayNode()->GetLevel());
    this->DisplayVolumeWindowValue->SetValue(volumeNode->GetScalarVolumeDisplayNode()->GetWindow());


    this->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID( volumeNode->GetID() );
    this->GetApplicationLogic()->PropagateVolumeSelection();

    this->VolumeSelector->SetSelected(volumeNode);
    const char *strName = this->VolumeSelector->GetSelected()->GetName();
    std::string strPlan = std::string(strName) + "-Plan";
    this->VolumeSelector->GetSelected()->SetName(strPlan.c_str());
    this->VolumeSelector->GetSelected()->SetDescription("Planning image/volume; created by PerkStation module");
    this->VolumeSelector->GetSelected()->Modified();
    this->VolumeSelector->UpdateMenu();

    // enable the load validation volume button
    this->EnableLoadValidationVolumeButton(true);
    }
  else 
    {
    vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
    //dialog->SetParent ( this-> );
    dialog->SetStyleToMessage();
    std::string msg = std::string("Unable to read volume file ") + std::string(fileName);
    dialog->SetText(msg.c_str());
    dialog->Create ( );
    dialog->Invoke();
    dialog->Delete();
    }
}
//--------------------------------------------------------------------------------
void vtkPerkStationModuleGUI::LoadValidationVolumeButtonCallback(const char *fileName)
{
  std::string fileString(fileName);
  for (unsigned int i = 0; i < fileString.length(); i++)
    {
    if (fileString[i] == '\\')
      {
      fileString[i] = '/';
      }
    }  

  bool validationVolumePreExists = false;

  // in case the planning volume already exists, it is just that the user has loaded another dicom series
  // as a new planning volume
  // then later in the code, this implies that the experiment has to be started over
  if (this->MRMLNode->GetValidationVolumeNode())
    {
    validationVolumePreExists = true;
    }

  this->MRMLNode->SetVolumeInUse("Validation");

  vtkSlicerApplication *app = static_cast<vtkSlicerApplication *>(this->GetApplication());
  vtkMRMLScalarVolumeNode *volumeNode = this->GetLogic()->AddVolumeToScene(app,fileString.c_str(), VOL_VALIDATION);
        
  if (volumeNode)
    {

    // as of now, don't want to disturb display on the secondary monitor
    // i.e. don't want the secondary monitor to display or overlay the validation image
    // if such a request come from clinician, it will be handled then

    // set the window/level controls values from the scalar volume display node
    this->DisplayVolumeLevelValue->SetValue(volumeNode->GetScalarVolumeDisplayNode()->GetLevel());
    this->DisplayVolumeWindowValue->SetValue(volumeNode->GetScalarVolumeDisplayNode()->GetWindow());


    this->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID( volumeNode->GetID() );
    this->GetApplicationLogic()->PropagateVolumeSelection();

    this->VolumeSelector->SetSelected(volumeNode);
    const char *strName = this->VolumeSelector->GetSelected()->GetName();
    std::string strPlan = std::string(strName) + "-Validation";
    this->VolumeSelector->GetSelected()->SetName(strPlan.c_str());
    this->VolumeSelector->GetSelected()->SetDescription("Validation image/volume; created by PerkStation module");
    this->VolumeSelector->GetSelected()->Modified();

    this->VolumeSelector->UpdateMenu();

    

    }
  else 
    {
    vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
    //dialog->SetParent ( this-> );
    dialog->SetStyleToMessage();
    std::string msg = std::string("Unable to read volume file ") + std::string(fileName);
    dialog->SetText(msg.c_str());
    dialog->Create ( );
    dialog->Invoke();
    dialog->Delete();
    // default to planning
    this->MRMLNode->SetVolumeInUse("Planning");
    }
}
//---------------------------------------------------------------------------------
void vtkPerkStationModuleGUI::EnableLoadValidationVolumeButton(bool enable)
{
  if (this->LoadValidationVolumeButton)
      this->LoadValidationVolumeButton->SetEnabled(enable);
}
