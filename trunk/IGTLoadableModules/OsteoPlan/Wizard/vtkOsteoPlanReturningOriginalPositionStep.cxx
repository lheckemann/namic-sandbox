/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#include "vtkOsteoPlanReturningOriginalPositionStep.h"

#include "vtkObject.h"

#include "vtkOsteoPlanGUI.h"
#include "vtkOsteoPlanLogic.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerInteractorStyle.h"
#include "vtkSlicerSliceLogic.h"
#include "vtkSlicerTheme.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLModelNode.h"

#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWPushButton.h"
#include "vtkKWEntryWithLabel.h"

#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkMatrix4x4.h"

#include "vtkMRMLOsteoPlanNode.h"

#define DELETE_IF_NULL_WITH_SETPARENT_NULL(obj) \
  if (obj)                                      \
    {                                           \
    obj->SetParent(NULL);                       \
    obj->Delete();                              \
    obj = NULL;                                 \
    };

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkOsteoPlanReturningOriginalPositionStep);
vtkCxxRevisionMacro(vtkOsteoPlanReturningOriginalPositionStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkOsteoPlanReturningOriginalPositionStep::vtkOsteoPlanReturningOriginalPositionStep()
{
  this->SetTitle("Back to Original");
  this->SetDescription("Return model to original position");

  this->MainFrame=NULL;
  this->modelSelector = NULL;
  this->BackOriginalButton = NULL;
  this->SelectedModel = NULL;

  this->TitleBackgroundColor[0] = 1;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 0.8;

  this->ProcessingCallback = false;
}

//----------------------------------------------------------------------------
vtkOsteoPlanReturningOriginalPositionStep::~vtkOsteoPlanReturningOriginalPositionStep()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(MainFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(modelSelector);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(BackOriginalButton);
}

//----------------------------------------------------------------------------
void vtkOsteoPlanReturningOriginalPositionStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWidget* parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

  //-------------------------------------------------------
  //-------------------------------------------------------
  //          Return to Original Position Frame


  if(!this->modelSelector)
    {
      this->modelSelector = vtkSlicerNodeSelectorWidget::New();
    }
  if(!this->modelSelector->IsCreated())
    {
      this->modelSelector->SetParent(parent);
      this->modelSelector->Create();
      this->modelSelector->SetNewNodeEnabled(0);
      this->modelSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
      this->modelSelector->SetMRMLScene(this->GetLogic()->GetMRMLScene());
      this->modelSelector->UpdateMenu();
    }

  if(!this->BackOriginalButton)
    {
      this->BackOriginalButton = vtkKWPushButton::New();
    }
  if(!this->BackOriginalButton->IsCreated())
    {
      this->BackOriginalButton->SetParent(parent);
      this->BackOriginalButton->Create();
      this->BackOriginalButton->SetText("Select a model");
      this->BackOriginalButton->SetBackgroundColor(color->White);
      this->BackOriginalButton->SetActiveBackgroundColor(color->White);
      this->BackOriginalButton->SetEnabled(0);
    }

  this->Script("pack %s %s -side top -fill x -padx 0 -pady 2",
         this->modelSelector->GetWidgetName(),
         this->BackOriginalButton->GetWidgetName());

  //-------------------------------------------------------

  this->AddGUIObservers();
  
  UpdateGUI();
}

//----------------------------------------------------------------------------
void vtkOsteoPlanReturningOriginalPositionStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkOsteoPlanReturningOriginalPositionStep::HandleMouseEvent(vtkSlicerInteractorStyle* style)
{

}


//----------------------------------------------------------------------------
void vtkOsteoPlanReturningOriginalPositionStep::ProcessGUIEvents(vtkObject *caller,
                                                      unsigned long event, void *callData)
{
  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    }
  else
    {
      if(this->modelSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
   && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
  {
    this->SelectedModel = vtkMRMLModelNode::SafeDownCast(this->modelSelector->GetSelected());
    if(this->SelectedModel)
      {
        vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
        vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

        this->BackOriginalButton->SetText("Back to Original Position");
        this->BackOriginalButton->SetBackgroundColor(color->SliceGUIGreen);
        this->BackOriginalButton->SetActiveBackgroundColor(color->SliceGUIGreen);
        this->BackOriginalButton->SetEnabled(1);



      }
    else
      {
        vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
        vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

        this->BackOriginalButton->SetText("Select a model");
        this->BackOriginalButton->SetBackgroundColor(color->White);
        this->BackOriginalButton->SetActiveBackgroundColor(color->White);
        this->BackOriginalButton->SetEnabled(0);

      }
  }

      if(this->BackOriginalButton == vtkKWPushButton::SafeDownCast(caller)
   && event == vtkKWPushButton::InvokedEvent)
  {
    if(this->SelectedModel)
      {
        // Get Parent Transform
        vtkMRMLTransformNode* ModelTransform = this->SelectedModel->GetParentTransformNode();
        if(ModelTransform)
    {
      vtkMRMLLinearTransformNode* ModelLinearTransform = vtkMRMLLinearTransformNode::SafeDownCast(ModelTransform);
      BackModelToOriginalPosition(ModelLinearTransform, this->SelectedModel);
    }
      }
  }

    }
}


//-----------------------------------------------------------------------------
void vtkOsteoPlanReturningOriginalPositionStep::AddGUIObservers()
{
  this->RemoveGUIObservers();

  if(this->modelSelector)
    {
      this->modelSelector->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand*)this->GUICallbackCommand);
    }

  if(this->BackOriginalButton)
    {
      this->BackOriginalButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }
  
}
//-----------------------------------------------------------------------------
void vtkOsteoPlanReturningOriginalPositionStep::RemoveGUIObservers()
{
  if(this->modelSelector)
    {
      this->modelSelector->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->BackOriginalButton)
    {
      this->BackOriginalButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  
}

//--------------------------------------------------------------------------------
void vtkOsteoPlanReturningOriginalPositionStep::UpdateGUI()
{

}

//----------------------------------------------------------------------------
void vtkOsteoPlanReturningOriginalPositionStep::HideUserInterface()
{
  TearDownGUI(); // HideUserInterface deletes the reference to the scene, so TearDownGUI shall be done before calling HideUserInterface
  Superclass::HideUserInterface();
}

//----------------------------------------------------------------------------
void vtkOsteoPlanReturningOriginalPositionStep::TearDownGUI()
{  
  RemoveGUIObservers();
}

//----------------------------------------------------------------------------
void vtkOsteoPlanReturningOriginalPositionStep::BackModelToOriginalPosition(vtkMRMLLinearTransformNode *ParentTransform, vtkMRMLModelNode* Model)
{
  // Get Tranformation Matrix
  vtkMatrix4x4* OriginalParentMatrix = ParentTransform->GetMatrixTransformToParent();
  
  // Create a Copy
  vtkMatrix4x4* CopiedParentMatrix = vtkMatrix4x4::New();
  CopiedParentMatrix->DeepCopy(OriginalParentMatrix);

  // Set Original to Identity
  OriginalParentMatrix->Identity();

  // Update Transform
  OriginalParentMatrix->Modified();

  // Invert Copied Matrix
  CopiedParentMatrix->Invert();

  // Check if model is in the list with fudicuals
  if(this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetNumberOfItems() == this->GetGUI()->GetOsteoPlanNode()->GetListOfFiducialLists()->GetNumberOfItems())
    {
    // Lists should be synchronized
      for(int i = 0; i < this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetNumberOfItems();i++)
      {
  vtkMRMLModelNode* listModel = vtkMRMLModelNode::SafeDownCast(this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetItemAsObject(i));
      if(listModel && listModel->GetID() == Model->GetID())
        {
        // The model is already in the list, which means fiducials should have been added on this model
        // Select the corresponding fiducial list
    vtkMRMLFiducialListNode* FiducialListModel = vtkMRMLFiducialListNode::SafeDownCast(this->GetGUI()->GetOsteoPlanNode()->GetListOfFiducialLists()->GetItemAsObject(i));
        if(FiducialListModel)
          {
      if(FiducialListModel->GetParentTransformNode())
        {
    // Fiducial List have already a parent transformation
    CopiedParentMatrix->Delete();
    return;
        }
  
   
      // Create new transformation node
      vtkMRMLLinearTransformNode* FiducialTransform = vtkMRMLLinearTransformNode::New();
      FiducialTransform->SetAndObserveMatrixTransformToParent(CopiedParentMatrix);
      
      // Set Name
      std::string mName = ParentTransform->GetName();
      std::string tName = mName + "-Inverted";
      FiducialTransform->SetName(tName.c_str());

      // Add it to the scene
      this->GetGUI()->GetMRMLScene()->AddNode(FiducialTransform);
      
      // Update Transform Node
      FiducialTransform->Modified();
  
      // Fiducial List found
      // Drop it under new transformation node (invert of the original)
      FiducialListModel->SetAndObserveTransformNodeID(FiducialTransform->GetID());
      FiducialListModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
      this->GetGUI()->GetMRMLScene()->InvokeEvent(vtkMRMLScene::SceneEditedEvent);

      // Drop Inverted transformation under original transformation (Identity)
      FiducialTransform->SetAndObserveTransformNodeID(ParentTransform->GetID());

      // Update scene by invoking event
      FiducialTransform->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
      this->GetGUI()->GetMRMLScene()->InvokeEvent(vtkMRMLScene::SceneEditedEvent);

      FiducialTransform->Delete();
          }
        }                     
      }
    } 
  
  CopiedParentMatrix->Delete();
}
