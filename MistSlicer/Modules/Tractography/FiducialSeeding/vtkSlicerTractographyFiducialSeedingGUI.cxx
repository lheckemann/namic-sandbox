/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkSlicerTractographyFiducialSeedingGUI.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>

#include "vtkSlicerTractographyFiducialSeedingGUI.h"

#include "vtkObjectFactory.h"
#include "vtkIntArray.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkImageData.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkMath.h"
#include "vtkCommand.h"

#include "vtkDiffusionTensorMathematics.h"
#include "vtkNRRDReader.h"
#include "vtkNRRDWriter.h"
#include "vtkSeedTracts.h"

#include "vtkMRMLTransformableNode.h"
#include "vtkMRMLDiffusionTensorVolumeNode.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLFiberBundleNode.h"
#include "vtkMRMLTransformNode.h"

#include "vtkKWApplication.h"
#include "vtkKWWidget.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWCheckButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWScaleWithLabel.h"
#include "vtkKWScale.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkKWFrame.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"

#include "vtkSlicerTractographyFiducialSeedingLogic.h"

//------------------------------------------------------------------------------
vtkSlicerTractographyFiducialSeedingGUI* vtkSlicerTractographyFiducialSeedingGUI::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkSlicerTractographyFiducialSeedingGUI");
  if(ret)
    {
      return (vtkSlicerTractographyFiducialSeedingGUI*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkSlicerTractographyFiducialSeedingGUI;
}


//----------------------------------------------------------------------------
vtkSlicerTractographyFiducialSeedingGUI::vtkSlicerTractographyFiducialSeedingGUI()
{
  this->VolumeSelector = vtkSlicerNodeSelectorWidget::New();
  this->OutFiberSelector = vtkSlicerNodeSelectorWidget::New();
  this->FiducialSelector = vtkSlicerNodeSelectorWidget::New();
  this->SeedButton  = vtkKWCheckButton::New();
  this->StoppingModeMenu = vtkKWMenuButtonWithLabel::New();
  this->StoppingValueScale = vtkKWScaleWithLabel::New();
  this->StoppingCurvatureScale = vtkKWScaleWithLabel::New();
  this->IntegrationStepLengthScale = vtkKWScaleWithLabel::New();
  this->RegionSizeScale = vtkKWScaleWithLabel::New();
  this->RegionSampleSizeScale = vtkKWScaleWithLabel::New();

  this->TractographyFiducialSeedingNodeSelector = vtkSlicerNodeSelectorWidget::New();

  this->FiducialListNode = NULL;
  
  this->StoppingMode = NULL;
  this->StoppingThreshold=0.15;
  this->MaximumPropagationDistance = 600;
  this->OverwritePolyDataWarning =1;

  this->RegisteredNode = 0;
  this->TractographyFiducialSeedingNode = NULL;

  UpdatingMRML = 0;
  UpdatingGUI = 0;

}

//----------------------------------------------------------------------------
vtkSlicerTractographyFiducialSeedingGUI::~vtkSlicerTractographyFiducialSeedingGUI()
{
    
  if ( this->VolumeSelector ) 
  {
    this->VolumeSelector->SetParent(NULL);
    this->VolumeSelector->Delete();
    this->VolumeSelector = NULL;
  }
  if ( this->OutFiberSelector ) 
  {
    this->OutFiberSelector->SetParent(NULL);
    this->OutFiberSelector->Delete();
    this->OutFiberSelector = NULL;
  }
  if ( this->FiducialSelector ) 
  {
    this->FiducialSelector->SetParent(NULL);
    this->FiducialSelector->Delete();
    this->FiducialSelector = NULL;
  }
  
  if ( this->SeedButton ) 
  {
    this->SeedButton->SetParent(NULL);
    this->SeedButton->Delete();
    this->SeedButton = NULL;
  }  
  
  if ( this->StoppingModeMenu ) 
  {
    this->StoppingModeMenu->SetParent(NULL);
    this->StoppingModeMenu->Delete();
    this->StoppingModeMenu = NULL;
  }
  
  if ( this->StoppingValueScale ) 
  {
    this->StoppingValueScale->SetParent(NULL);
    this->StoppingValueScale->Delete();
    this->StoppingValueScale = NULL;
  }
    if ( this->StoppingCurvatureScale) 
  {
    this->StoppingCurvatureScale->SetParent(NULL);
    this->StoppingCurvatureScale->Delete();
    this->StoppingCurvatureScale = NULL;
  }
    if ( this->IntegrationStepLengthScale ) 
  {
    this->IntegrationStepLengthScale->SetParent(NULL);
    this->IntegrationStepLengthScale->Delete();
    this->IntegrationStepLengthScale = NULL;
  }
    if ( this->RegionSizeScale ) 
  {
    this->RegionSizeScale->SetParent(NULL);
    this->RegionSizeScale->Delete();
    this->RegionSizeScale = NULL;
  }
    if ( this->RegionSampleSizeScale ) 
  {
    this->RegionSampleSizeScale->SetParent(NULL);
    this->RegionSampleSizeScale->Delete();
    this->RegionSampleSizeScale = NULL;
  }
  
  if ( this->TractographyFiducialSeedingNodeSelector ) 
  {
    this->TractographyFiducialSeedingNodeSelector->SetParent(NULL);
    this->TractographyFiducialSeedingNodeSelector->Delete();
    this->TractographyFiducialSeedingNodeSelector = NULL;
  }

  vtkSetAndObserveMRMLNodeMacro(this->FiducialListNode, NULL);
  
  vtkSetMRMLNodeMacro(this->TractographyFiducialSeedingNode, NULL);

  if (this->StoppingMode)
    {
    delete [] this->StoppingMode;
    }
}

//----------------------------------------------------------------------------
void vtkSlicerTractographyFiducialSeedingGUI::PrintSelf(ostream& os, vtkIndent indent)
{
  
}

//---------------------------------------------------------------------------
vtkIntArray* vtkSlicerTractographyFiducialSeedingGUI::NewObservableEvents()
{
  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::NewSceneEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  return events;
}


//---------------------------------------------------------------------------
void vtkSlicerTractographyFiducialSeedingGUI::AddGUIObservers ( ) 
{
  this->VolumeSelector->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );  

  this->OutFiberSelector->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );  

  this->FiducialSelector->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand ); 
  
  this->SeedButton->AddObserver (vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand ); 
  
  this->StoppingValueScale->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  
  this->StoppingCurvatureScale->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  
  this->IntegrationStepLengthScale->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  
  this->RegionSizeScale->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  
  this->RegionSampleSizeScale->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );

  this->TractographyFiducialSeedingNodeSelector->AddObserver (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );  

}



//---------------------------------------------------------------------------
void vtkSlicerTractographyFiducialSeedingGUI::RemoveGUIObservers ( )
{
  this->VolumeSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );  

  this->OutFiberSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );  

  this->FiducialSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );  

  this->SeedButton->RemoveObservers (vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand ); 

  this->StoppingValueScale->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  
  this->StoppingCurvatureScale->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  
  this->IntegrationStepLengthScale->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
 
  this->RegionSizeScale->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  
  this->RegionSampleSizeScale->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );

  this->TractographyFiducialSeedingNodeSelector->RemoveObservers (vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );  

}



//---------------------------------------------------------------------------
void vtkSlicerTractographyFiducialSeedingGUI::ProcessGUIEvents ( vtkObject *caller,
                                           unsigned long event,
                                           void *callData ) 
{
  vtkSlicerNodeSelectorWidget *selector = vtkSlicerNodeSelectorWidget::SafeDownCast(caller);

  if (selector == this->VolumeSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent &&
      this->VolumeSelector->GetSelected() != NULL) 
    { 
    this->CreateTracts();
    }
  else if (selector == this->OutFiberSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  &&
    this->OutFiberSelector->GetSelected() != NULL) 
    { 
    vtkMRMLFiberBundleNode *fiberNode = vtkMRMLFiberBundleNode::SafeDownCast(this->OutFiberSelector->GetSelected());
    
    int createFiber = 1;
    vtkMRMLDiffusionTensorVolumeNode *volumeNode = vtkMRMLDiffusionTensorVolumeNode::SafeDownCast(this->VolumeSelector->GetSelected());
    vtkMRMLFiducialListNode *fiducialListNode = vtkMRMLFiducialListNode::SafeDownCast(this->FiducialSelector->GetSelected());
  

    if (this->OverwritePolyDataWarning && volumeNode && fiducialListNode && fiberNode && fiberNode->GetPolyData() != NULL)
      {
      vtkKWMessageDialog *message = vtkKWMessageDialog::New();
      message->SetParent ( this->UIPanel->GetPageWidget ( "Tractography" ) );
      message->SetStyleToYesNo();
      std::string msg = "TractographyFiducialSeeding is about to create new tracks for " + std::string(fiberNode->GetName()) + " which contains polydata. Do you want to override it?";
      message->SetText(msg.c_str());
      message->Create();
      createFiber = message->Invoke();
      message->Delete();
      }
    if (createFiber)
      {
      this->CreateTracts();
      }
    }
  if (selector == this->FiducialSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  &&
    this->FiducialSelector->GetSelected() != NULL) 
    { 
    vtkMRMLFiducialListNode* n = vtkMRMLFiducialListNode::SafeDownCast(this->FiducialSelector->GetSelected());
    vtkIntArray *events = vtkIntArray::New();
    //events->InsertNextValue( vtkCommand::ModifiedEvent);
    events->InsertNextValue( vtkMRMLFiducialListNode::FiducialModifiedEvent);
    events->InsertNextValue( vtkMRMLTransformableNode::TransformModifiedEvent);
    vtkSetAndObserveMRMLNodeEventsMacro(this->FiducialListNode, n, events);
    events->Delete();

    this->CreateTracts();
    }

  if (selector == this->TractographyFiducialSeedingNodeSelector && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent  &&
    this->TractographyFiducialSeedingNodeSelector->GetSelected() != NULL) 
    { 
    vtkMRMLTractographyFiducialSeedingNode* n = vtkMRMLTractographyFiducialSeedingNode::SafeDownCast(this->TractographyFiducialSeedingNodeSelector->GetSelected());
    vtkSetAndObserveMRMLNodeMacro( this->TractographyFiducialSeedingNode, n);
    this->UpdateGUI();
    }

  else if ( this->SeedButton == vtkKWCheckButton::SafeDownCast(caller) &&
          event == vtkKWCheckButton::SelectedStateChangedEvent ) 
    {
    this->CreateTracts();
    }
  else if (event == vtkKWScale::ScaleValueChangedEvent ) 
    {
    this->CreateTracts();
    }

}

//---------------------------------------------------------------------------
void vtkSlicerTractographyFiducialSeedingGUI::UpdateGUI ()
{
  if (this->UpdatingMRML)
    {
    return;
    }
  this->UpdatingGUI = 1;

  vtkMRMLTractographyFiducialSeedingNode* n = this->GetTractographyFiducialSeedingNode();
  if (n != NULL && this->GetMRMLScene() != NULL)
    {
    // set GUI widgest from parameter node
    vtkMRMLNode *s = this->GetMRMLScene()->GetNodeByID(n->GetInputVolumeRef());
    this->VolumeSelector->SetSelected(s);

    s = this->GetMRMLScene()->GetNodeByID(n->GetInputFiducialRef());
    this->FiducialSelector->SetSelected(s);

    s = this->GetMRMLScene()->GetNodeByID(n->GetOutputFiberRef());
    this->OutFiberSelector->SetSelected(s);
    
    if(n->GetStoppingMode() == 0)
      {
      this->StoppingModeMenu->GetWidget()->SetValue("Linear Measurement");
      }
    else
      {
      this->StoppingModeMenu->GetWidget()->SetValue ( "Fractional Anisotropy");
      }
    this->StoppingValueScale->GetWidget()->SetValue(n->GetStoppingValue());
    this->StoppingCurvatureScale->GetWidget()->SetValue(n->GetStoppingCurvature());
    this->IntegrationStepLengthScale->GetWidget()->SetValue(n->GetIntegrationStep());
    this->RegionSizeScale->GetWidget()->SetValue(n->GetSeedingRegionSize());
    this->RegionSampleSizeScale->GetWidget()->SetValue(n->GetSeedingRegionStep());
    }

  this->UpdatingGUI = 0;
}
//---------------------------------------------------------------------------
void vtkSlicerTractographyFiducialSeedingGUI::UpdateMRML ()
{
  if (this->UpdatingGUI)
    {
    return;
    }
  this->UpdatingMRML = 1;

  vtkMRMLTractographyFiducialSeedingNode* n = this->GetTractographyFiducialSeedingNode();
  if (n == NULL)
    {
    // no parameter node selected yet, create new
    this->TractographyFiducialSeedingNodeSelector->SetSelectedNew("vtkMRMLTractographyFiducialSeedingNode");
    this->TractographyFiducialSeedingNodeSelector->ProcessNewNodeCommand("vtkMRMLTractographyFiducialSeedingNode", "Parameters");
    n = vtkMRMLTractographyFiducialSeedingNode::SafeDownCast(this->TractographyFiducialSeedingNodeSelector->GetSelected());

    // set an observe new node
    vtkSetAndObserveMRMLNodeMacro(this->TractographyFiducialSeedingNode, n);
   }
  // save node parameters for Undo
  this->GetMRMLScene()->SaveStateForUndo(n);

  if (this->VolumeSelector->GetSelected())
    {
    n->SetInputVolumeRef(this->VolumeSelector->GetSelected()->GetID());
    }
  if (this->FiducialSelector->GetSelected())
    {
    n->SetInputFiducialRef(this->FiducialSelector->GetSelected()->GetID());
    }
  if (this->OutFiberSelector->GetSelected())
    {
    n->SetOutputFiberRef(this->OutFiberSelector->GetSelected()->GetID());
    }
    
  if(!strcmp(this->StoppingModeMenu->GetWidget()->GetValue(), "Linear Measurement"))
    {
    n->SetStoppingMode(0);
    }
  else
    if(!strcmp(this->StoppingModeMenu->GetWidget()->GetValue(), "Fractional Anisotropy"))
    {
    n->SetStoppingMode(1);
    }
  n->SetStoppingValue(this->StoppingValueScale->GetWidget()->GetValue() );
  n->SetStoppingCurvature( this->StoppingCurvatureScale->GetWidget()->GetValue() );
  n->SetIntegrationStep( this->IntegrationStepLengthScale->GetWidget()->GetValue() );
  n->SetSeedingRegionSize( this->RegionSizeScale->GetWidget()->GetValue() );
  n->SetSeedingRegionStep( this->RegionSampleSizeScale->GetWidget()->GetValue() );

  this->UpdatingMRML = 0;

 }

//---------------------------------------------------------------------------
void vtkSlicerTractographyFiducialSeedingGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event,
                                            void *callData ) 
{
  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    vtkSetAndObserveMRMLNodeMacro( this->TractographyFiducialSeedingNode, NULL);
    this->AddFiducialListNodeObserver(NULL);
    return;
    }
  // if parameter node has been added, update GUI widgets with new values
  vtkMRMLTractographyFiducialSeedingNode* snode = reinterpret_cast<vtkMRMLTractographyFiducialSeedingNode*> (callData);
  if (snode && event == vtkMRMLScene::NodeAddedEvent && snode->IsA("vtkMRMLTractographyFiducialSeedingNode") &&
    this->GetTractographyFiducialSeedingNode() == NULL)
    {
    vtkSetAndObserveMRMLNodeMacro( this->TractographyFiducialSeedingNode, snode);
   
    vtkMRMLFiducialListNode *fn = NULL;
    if (this->GetMRMLScene() && this->TractographyFiducialSeedingNode->GetInputFiducialRef())
      {
      fn = vtkMRMLFiducialListNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(
          this->TractographyFiducialSeedingNode->GetInputFiducialRef()));
    
      }
    if (fn)
      {
      this->AddFiducialListNodeObserver(fn);
      }
    this->UpdateGUI();
    this->CreateTracts();
    return;
    }

  // if parameter node has been changed externally, update GUI widgets with new values
  snode = vtkMRMLTractographyFiducialSeedingNode::SafeDownCast(caller);
  if (snode != NULL && this->GetTractographyFiducialSeedingNode() == snode) 
    {
    vtkMRMLFiducialListNode *fn = NULL;
    if (this->GetMRMLScene() && this->TractographyFiducialSeedingNode->GetInputFiducialRef())
      {
      fn = vtkMRMLFiducialListNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(
          this->TractographyFiducialSeedingNode->GetInputFiducialRef()));
    
      }
    if (fn != this->FiducialListNode)
      {
      this->AddFiducialListNodeObserver(fn);
      }
    this->UpdateGUI();
    this->CreateTracts();
    return;
    }

  // if parameter node has been changed externally, update GUI widgets with new values
  vtkMRMLFiducialListNode* node = vtkMRMLFiducialListNode::SafeDownCast(caller);
  if (node != NULL && this->FiducialListNode == node) 
    {
    this->CreateTracts();
    }
}

//---------------------------------------------------------------------------
void vtkSlicerTractographyFiducialSeedingGUI::AddFiducialListNodeObserver(vtkMRMLFiducialListNode *n)
{
    vtkIntArray *events = vtkIntArray::New();
    //events->InsertNextValue( vtkCommand::ModifiedEvent);
    events->InsertNextValue( vtkMRMLFiducialListNode::FiducialModifiedEvent);
    events->InsertNextValue( vtkMRMLTransformableNode::TransformModifiedEvent);
    vtkSetAndObserveMRMLNodeEventsMacro(this->FiducialListNode, n, events);
    events->Delete();
  }

//---------------------------------------------------------------------------
void vtkSlicerTractographyFiducialSeedingGUI:: CreateTracts()
{
  this->UpdateMRML();
  if ( this->SeedButton->GetSelectedState() == 0) 
    {
    return;
    }
  
  vtkMRMLDiffusionTensorVolumeNode *volumeNode = vtkMRMLDiffusionTensorVolumeNode::SafeDownCast(this->VolumeSelector->GetSelected());
  vtkMRMLFiducialListNode *fiducialListNode = vtkMRMLFiducialListNode::SafeDownCast(this->FiducialSelector->GetSelected());
  vtkMRMLFiberBundleNode *fiberNode = vtkMRMLFiberBundleNode::SafeDownCast(this->OutFiberSelector->GetSelected());
  
  std::string stopingMode = this->StoppingModeMenu->GetWidget()->GetValue ();

  if(volumeNode == NULL || fiducialListNode == NULL || fiberNode == NULL) return;

  vtkSlicerTractographyFiducialSeedingLogic::CreateTracts(volumeNode, fiducialListNode, fiberNode,
                                                          stopingMode.c_str(),
                                                          this->StoppingValueScale->GetWidget()->GetValue(),
                                                          this->StoppingCurvatureScale->GetWidget()->GetValue(),
                                                          this->IntegrationStepLengthScale->GetWidget()->GetValue(),
                                                          this->RegionSizeScale->GetWidget()->GetValue(),
                                                          this->RegionSampleSizeScale->GetWidget()->GetValue());  
}

//---------------------------------------------------------------------------
void vtkSlicerTractographyFiducialSeedingGUI::RegisterTractographyFiducialSeedingNode ( ) 
{
  if (this->RegisteredNode || this->GetMRMLScene() == NULL)
    {
    return;
    }
  vtkMRMLTractographyFiducialSeedingNode* gadNode = vtkMRMLTractographyFiducialSeedingNode::New();
  this->GetMRMLScene()->RegisterNodeClass(gadNode);
  gadNode->Delete();
  this->RegisteredNode = 1;
}

//---------------------------------------------------------------------------
void vtkSlicerTractographyFiducialSeedingGUI::BuildGUI ( ) 
{
  if (this->Built)
    {
    return;
    }

  this->RegisterTractographyFiducialSeedingNode();

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

  this->UIPanel->AddPage ( "Tractography", "Tractography", NULL );
  // ---
  // MODULE GUI FRAME 
  // ---
  // Define your help text and build the help frame here.
  const char *help = "The FiducialSeeding module....";
  const char *about = "This work was supported by NA-MIC, NAC, BIRN, NCIGT, and the Slicer Community. See <a>http://www.slicer.org</a> for details. ";
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Tractography" );
  this->BuildHelpAndAboutFrame ( page, help, about );
    
  vtkSlicerModuleCollapsibleFrame *moduleFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  moduleFrame->SetParent ( this->UIPanel->GetPageWidget ( "Tractography" ) );
  moduleFrame->Create ( );
  moduleFrame->SetLabelText ("Tractography Seeding From Fiducial");
  moduleFrame->ExpandFrame ( );
  app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                moduleFrame->GetWidgetName(), this->UIPanel->GetPageWidget("Tractography")->GetWidgetName());
  
  this->TractographyFiducialSeedingNodeSelector->SetNodeClass("vtkMRMLTractographyFiducialSeedingNode", NULL, NULL, "Parameters");
  this->TractographyFiducialSeedingNodeSelector->SetNewNodeEnabled(1);
  this->TractographyFiducialSeedingNodeSelector->NoneEnabledOff();
  this->TractographyFiducialSeedingNodeSelector->SetShowHidden(1);
  this->TractographyFiducialSeedingNodeSelector->SetParent( moduleFrame->GetFrame() );
  this->TractographyFiducialSeedingNodeSelector->Create();
  this->TractographyFiducialSeedingNodeSelector->SetMRMLScene(this->GetMRMLScene());
  this->TractographyFiducialSeedingNodeSelector->UpdateMenu();

  this->TractographyFiducialSeedingNodeSelector->SetBorderWidth(2);
  this->TractographyFiducialSeedingNodeSelector->SetLabelText( "Parameters");
  this->TractographyFiducialSeedingNodeSelector->SetBalloonHelpString("select a parameter node from the current mrml scene.");
  app->Script("pack %s -side top -anchor e -padx 20 -pady 4", 
                this->TractographyFiducialSeedingNodeSelector->GetWidgetName());


  this->VolumeSelector->SetNodeClass("vtkMRMLDiffusionTensorVolumeNode", NULL, NULL, NULL);
  this->VolumeSelector->SetParent( moduleFrame->GetFrame() );
  this->VolumeSelector->Create();
  this->VolumeSelector->SetMRMLScene(this->GetApplicationLogic()->GetMRMLScene());
  this->VolumeSelector->UpdateMenu();
  this->VolumeSelector->SetBorderWidth(2);
  this->VolumeSelector->SetLabelText( "Select DTI Volume: ");
  this->VolumeSelector->SetBalloonHelpString("select an input DT volume from the current mrml scene.");
  app->Script("pack %s -side top -anchor e -padx 20 -pady 4", 
                this->VolumeSelector->GetWidgetName());
  
  this->FiducialSelector->SetNodeClass("vtkMRMLFiducialListNode", NULL, NULL, NULL);
  this->FiducialSelector->SetNewNodeEnabled(0);
  this->FiducialSelector->NoneEnabledOn();
  this->FiducialSelector->SetShowHidden(1);
  this->FiducialSelector->SetParent( moduleFrame->GetFrame() );
  this->FiducialSelector->Create();
  this->FiducialSelector->SetMRMLScene(this->GetApplicationLogic()->GetMRMLScene());
  this->FiducialSelector->UpdateMenu();
  this->FiducialSelector->SetBorderWidth(2);
  this->FiducialSelector->SetLabelText( "Select FiducialList");
  this->FiducialSelector->SetBalloonHelpString("select a fiducial list for seeding.");
  app->Script("pack %s -side top -anchor e -padx 20 -pady 4", 
                this->FiducialSelector->GetWidgetName());


  this->OutFiberSelector->SetNodeClass("vtkMRMLFiberBundleNode", NULL, NULL, "FiberBundleNode");
  this->OutFiberSelector->SetNewNodeEnabled(1);
  this->OutFiberSelector->SetParent( moduleFrame->GetFrame() );
  this->OutFiberSelector->Create();
  this->OutFiberSelector->SetMRMLScene(this->GetApplicationLogic()->GetMRMLScene());
  this->OutFiberSelector->UpdateMenu();
  this->OutFiberSelector->SetBorderWidth(2);
  this->OutFiberSelector->SetLabelText( "Output FiberBundleNode: ");
  this->OutFiberSelector->SetBalloonHelpString("select an output fiber bundle node.");
  app->Script("pack %s -side top -anchor e -padx 20 -pady 4", 
                this->OutFiberSelector->GetWidgetName());

  this->StoppingModeMenu->SetParent( moduleFrame->GetFrame());
  this->StoppingModeMenu->Create();
  this->StoppingModeMenu->SetWidth(20);
  this->StoppingModeMenu->SetLabelWidth(12);
  this->StoppingModeMenu->SetLabelText("Stopping Mode:");
  this->StoppingModeMenu->GetWidget()->GetMenu()->AddRadioButton ( "Linear Measurement");
  this->StoppingModeMenu->GetWidget()->GetMenu()->AddRadioButton ( "Fractional Anisotropy");
  this->StoppingModeMenu->GetWidget()->SetValue ( "Linear Measurement" );
  this->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
    this->StoppingModeMenu->GetWidgetName());

  this->StoppingValueScale->SetParent ( moduleFrame->GetFrame() );
  this->StoppingValueScale->Create ( );
  this->StoppingValueScale->SetLabelText("Stopping Value");
  this->StoppingValueScale->GetWidget()->SetRange(0.01,1);
  this->StoppingValueScale->GetWidget()->SetResolution(0.01);
  this->StoppingValueScale->GetWidget()->SetValue(0.1);
  this->StoppingValueScale->SetBalloonHelpString("Tractography will stop when the stopping measurement drops below this value.");
  this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2",
                 this->StoppingValueScale->GetWidgetName() );

  this->StoppingCurvatureScale->SetParent ( moduleFrame->GetFrame() );
  this->StoppingCurvatureScale->Create ( );
  this->StoppingCurvatureScale->SetLabelText("Stopping Track Curvature");
  this->StoppingCurvatureScale->GetWidget()->SetRange(0,10);
  this->StoppingCurvatureScale->GetWidget()->SetResolution(0.1);
  this->StoppingCurvatureScale->GetWidget()->SetValue(0.8);
  this->StoppingCurvatureScale->SetBalloonHelpString("Tractography will stop if radius of curvature becomes smaller than this number units are degrees per mm.");
  this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2",
                 this->StoppingCurvatureScale->GetWidgetName() );

  this->IntegrationStepLengthScale->SetParent ( moduleFrame->GetFrame() );
  this->IntegrationStepLengthScale->Create ( );
  this->IntegrationStepLengthScale->SetLabelText("Integration Step Length (mm)");
  this->IntegrationStepLengthScale->GetWidget()->SetRange(0.1,10);
  this->IntegrationStepLengthScale->GetWidget()->SetResolution(0.1);
  this->IntegrationStepLengthScale->GetWidget()->SetValue(0.5);
  this->IntegrationStepLengthScale->SetBalloonHelpString("Integration step size.");
  this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2",
                 this->IntegrationStepLengthScale->GetWidgetName() );

  this->RegionSizeScale->SetParent ( moduleFrame->GetFrame() );
  this->RegionSizeScale->Create ( );
  this->RegionSizeScale->SetLabelText("Seeding Region Size (mm)");
  this->RegionSizeScale->GetWidget()->SetRange(0,10);
  this->RegionSizeScale->GetWidget()->SetResolution(0.5);
  this->RegionSizeScale->GetWidget()->SetValue(1.0);
  this->RegionSizeScale->SetBalloonHelpString("The size of the seeding region.");
  this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2",
                 this->RegionSizeScale->GetWidgetName() );

  this->RegionSampleSizeScale->SetParent ( moduleFrame->GetFrame() );
  this->RegionSampleSizeScale->Create ( );
  this->RegionSampleSizeScale->SetLabelText("Seeding Step Size (mm)");
  this->RegionSampleSizeScale->GetWidget()->SetRange(1,10);
  this->RegionSampleSizeScale->GetWidget()->SetResolution(0.5);
  this->RegionSampleSizeScale->GetWidget()->SetValue(0.5);
  this->RegionSampleSizeScale->SetBalloonHelpString("Step between seedin samples");
  this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2",
                 this->RegionSampleSizeScale->GetWidgetName() );

  this->SeedButton->SetParent(moduleFrame->GetFrame());
  this->SeedButton->Create();
  this->SeedButton->SelectedStateOn();
  this->SeedButton->SetText("Seed Tracts");
  this->Script(
    "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
    this->SeedButton->GetWidgetName());

  moduleFrame->Delete();

  this->Built = 1;
}

void vtkSlicerTractographyFiducialSeedingGUI::SetVolumeSelector(vtkMRMLNode *node)
  {
  this->VolumeSelector->SetSelected(node);
  }

void vtkSlicerTractographyFiducialSeedingGUI::SetFiducialSelector(vtkMRMLNode *node)
  {
  this->FiducialSelector->SetSelected(node);
  }

void vtkSlicerTractographyFiducialSeedingGUI::SetOutFiberSelector(vtkMRMLNode *node)
  {
  this->OutFiberSelector->SetSelected(node);
  }

//---------------------------------------------------------------------------
void vtkSlicerTractographyFiducialSeedingGUI::Init ( ) 
{
}
