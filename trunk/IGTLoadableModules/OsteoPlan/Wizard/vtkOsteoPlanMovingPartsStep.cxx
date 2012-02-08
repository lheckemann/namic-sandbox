/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#include "vtkOsteoPlanMovingPartsStep.h"

#include "vtkObject.h"

#include "vtkOsteoPlanGUI.h"
#include "vtkOsteoPlanLogic.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerInteractorStyle.h"
#include "vtkSlicerSliceLogic.h"
#include "vtkMRMLSliceNode.h"
#include "vtkSlicerTheme.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSlicerTransformEditorWidget.h"

#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWPushButton.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWScaleWithLabel.h"
#include "vtkKWScale.h"

#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLOsteoPlanNode.h"

#include "vtkTransformPolyDataFilter.h"

#define DELETE_IF_NULL_WITH_SETPARENT_NULL(obj) \
  if (obj)                                      \
    {                                           \
    obj->SetParent(NULL);                       \
    obj->Delete();                              \
    obj = NULL;                                 \
    };

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkOsteoPlanMovingPartsStep);
vtkCxxRevisionMacro(vtkOsteoPlanMovingPartsStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkOsteoPlanMovingPartsStep::vtkOsteoPlanMovingPartsStep()
{
  this->SetTitle("Moving Parts");
  this->SetDescription("Move different parts");

  this->MainFrame                = NULL;
  this->SelectingPartToMove      = NULL;
  this->ModelToMoveLabel         = NULL;
  this->ModelToMoveSelector      = NULL;
  this->ModelToMove              = NULL;
  this->StartMovingButton        = NULL;
  this->MovingFrame              = NULL;
  this->ParentTransformationNode = NULL;
  this->TransformationEditor     = NULL;

  this->TitleBackgroundColor[0]  = 0.8;
  this->TitleBackgroundColor[1]  = 1;
  this->TitleBackgroundColor[2]  = 0.8;

  this->ProcessingCallback       = false;
  this->StartMoving              = false;

}

//----------------------------------------------------------------------------
vtkOsteoPlanMovingPartsStep::~vtkOsteoPlanMovingPartsStep()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(MainFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(SelectingPartToMove);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(ModelToMoveLabel);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(ModelToMoveSelector);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(StartMovingButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(MovingFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TransformationEditor);
}

//----------------------------------------------------------------------------
void vtkOsteoPlanMovingPartsStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWidget          *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  vtkSlicerApplication *app    = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  vtkSlicerColor       *color  = app->GetSlicerTheme()->GetSlicerColors();

  //-------------------------------------------------------
  // Select Model Frame

  if(!this->SelectingPartToMove)
    {
    this->SelectingPartToMove = vtkKWFrameWithLabel::New();
    this->SelectingPartToMove->SetParent(parent);
    this->SelectingPartToMove->Create();
    this->SelectingPartToMove->SetLabelText("Select Part to move");
    }

  this->Script("pack %s -side top -fill x -anchor nw -padx 0 -pady 2",
               this->SelectingPartToMove->GetWidgetName());

  if(!this->ModelToMoveLabel)
    {
    this->ModelToMoveLabel = vtkKWLabel::New();
    }
  if(!this->ModelToMoveLabel->IsCreated())
    {
    this->ModelToMoveLabel->SetParent(this->SelectingPartToMove->GetFrame());
    this->ModelToMoveLabel->Create();
    this->ModelToMoveLabel->SetText("Part to move:");
    this->ModelToMoveLabel->SetAnchorToWest();
    }

  if(!this->ModelToMoveSelector)
    {
    this->ModelToMoveSelector = vtkSlicerNodeSelectorWidget::New();
    }
  if(!this->ModelToMoveSelector->IsCreated())
    {
    this->ModelToMoveSelector->SetParent(this->SelectingPartToMove->GetFrame());
    this->ModelToMoveSelector->Create();
    this->ModelToMoveSelector->SetNewNodeEnabled(0);
    this->ModelToMoveSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
    this->ModelToMoveSelector->SetMRMLScene(this->GetLogic()->GetMRMLScene());
    this->ModelToMoveSelector->UpdateMenu();
    }

  if(!this->StartMovingButton)
    {
    this->StartMovingButton = vtkKWPushButton::New();
    }
  if(!this->StartMovingButton->IsCreated())
    {
    this->StartMovingButton->SetParent(this->SelectingPartToMove->GetFrame());
    this->StartMovingButton->Create();
    this->StartMovingButton->SetBackgroundColor(color->White);
    this->StartMovingButton->SetActiveBackgroundColor(color->White);
    this->StartMovingButton->SetText("Please, select a model to move");
    this->StartMovingButton->SetEnabled(0);
    }

  this->Script("pack %s %s %s -side top -fill x -padx 0 -pady 2",
               this->ModelToMoveLabel->GetWidgetName(),
               this->ModelToMoveSelector->GetWidgetName(),
               this->StartMovingButton->GetWidgetName());


  //-------------------------------------------------------
  // Moving Model Frame

  if(!this->MovingFrame)
    {
    this->MovingFrame = vtkKWFrameWithLabel::New();
    this->MovingFrame->SetParent(parent);
    this->MovingFrame->Create();
    this->MovingFrame->SetLabelText("Moving frame");
    this->MovingFrame->SetEnabled(0);
    }

  this->Script("pack %s -side top -fill x -anchor nw -padx 0 -pady 2",
               this->MovingFrame->GetWidgetName());

  if(!this->TransformationEditor)
    {
    this->TransformationEditor = vtkSlicerTransformEditorWidget::New();
    }
  if(!this->TransformationEditor->IsCreated())
    {
    this->TransformationEditor->SetAndObserveMRMLScene(this->GetGUI()->GetMRMLScene());
    this->TransformationEditor->SetParent(this->MovingFrame->GetFrame());
    this->TransformationEditor->Create();
    this->TransformationEditor->SetEnabled(0);
    }

  this->Script("pack %s -side top -fill x -anchor nw -padx 0 -pady 2",
               this->TransformationEditor->GetWidgetName());

  //-------------------------------------------------------

  this->AddGUIObservers();

  UpdateGUI();
}

//----------------------------------------------------------------------------
void vtkOsteoPlanMovingPartsStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkOsteoPlanMovingPartsStep::HandleMouseEvent(vtkSlicerInteractorStyle* style)
{
}

//----------------------------------------------------------------------------
void vtkOsteoPlanMovingPartsStep::ProcessGUIEvents(vtkObject *caller,
                                                   unsigned long event, void *callData)
{

  //--------------------------------------------------
  // Model to move selected

  if(this->ModelToMoveSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    if(this->StartMovingButton)
      {
      vtkSlicerApplication *app   = vtkSlicerApplication::SafeDownCast(this->GetApplication());
      vtkSlicerColor       *color = app->GetSlicerTheme()->GetSlicerColors();

      this->StartMovingButton->SetBackgroundColor(color->SliceGUIGreen);
      this->StartMovingButton->SetActiveBackgroundColor(color->SliceGUIGreen);
      this->StartMovingButton->SetText("Start moving model");
      this->StartMovingButton->SetEnabled(1);
      }
    }

  //--------------------------------------------------
  // Start Moving Button

  if(this->StartMovingButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
    vtkSlicerApplication *app   = vtkSlicerApplication::SafeDownCast(this->GetApplication());
    vtkSlicerColor       *color = app->GetSlicerTheme()->GetSlicerColors();

    if(!this->StartMoving)
      {
      // Enable motion
      this->MovingFrame->SetEnabled(1);
      this->TransformationEditor->SetEnabled(1);
      this->ModelToMoveSelector->SetEnabled(0);

      this->StartMovingButton->SetBackgroundColor(color->SliceGUIOrange);
      this->StartMovingButton->SetActiveBackgroundColor(color->SliceGUIOrange);
      this->StartMovingButton->SetText("Stop moving model");
      this->StartMoving = true;

      MoveModel();
      }
    else
      {
      // Disable motion
      this->MovingFrame->SetEnabled(0);
      this->TransformationEditor->SetEnabled(0);
      this->ModelToMoveSelector->SetEnabled(1);

      this->StartMovingButton->SetBackgroundColor(color->SliceGUIGreen);
      this->StartMovingButton->SetActiveBackgroundColor(color->SliceGUIGreen);
      this->StartMovingButton->SetText("Start moving model");
      this->StartMoving = false;
      }
    }
}

//-----------------------------------------------------------------------------
void vtkOsteoPlanMovingPartsStep::ProcessMRMLEvents(vtkObject* caller,
                                                    unsigned long event,
                                                    void* vtkNotUsed(callData))
{
}
//-----------------------------------------------------------------------------
void vtkOsteoPlanMovingPartsStep::AddGUIObservers()
{
  this->RemoveGUIObservers();

  if(this->ModelToMoveSelector)
    {
    this->ModelToMoveSelector->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand*)this->GUICallbackCommand);
    }

  if(this->StartMovingButton)
    {
    this->StartMovingButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }

}
//-----------------------------------------------------------------------------
void vtkOsteoPlanMovingPartsStep::RemoveGUIObservers()
{

  if(this->ModelToMoveSelector)
    {
    this->ModelToMoveSelector->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->StartMovingButton)
    {
    this->StartMovingButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
}

//--------------------------------------------------------------------------------
void vtkOsteoPlanMovingPartsStep::UpdateGUI()
{
}

//----------------------------------------------------------------------------
void vtkOsteoPlanMovingPartsStep::HideUserInterface()
{
  TearDownGUI(); // HideUserInterface deletes the reference to the scene, so TearDownGUI shall be done before calling HideUserInterface
  Superclass::HideUserInterface();

  // Remove all previous transforms
  if(this->GetGUI()->GetOsteoPlanNode())
    {
    //this->GetGUI()->GetOsteoPlanNode()->GetListOfTransforms()->RemoveAllItems();

    // Read all the models transformed and store their transformations
    for(int i = 0; i < this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetNumberOfItems(); i++)
      {
      vtkMRMLModelNode* mNode = vtkMRMLModelNode::SafeDownCast(this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetItemAsObject(i));
      if(mNode)
        {
        if(mNode->GetAttribute("TransformApplied") == NULL)
          {
          vtkMRMLLinearTransformNode* tNode = vtkMRMLLinearTransformNode::SafeDownCast(mNode->GetParentTransformNode());
          if(tNode)
            {
            mNode->SetAttribute("TransformApplied","false");
            this->GetGUI()->GetOsteoPlanNode()->GetListOfTransforms()->AddItem(tNode);
            }
          }
        }
      }
    }
}

//----------------------------------------------------------------------------
void vtkOsteoPlanMovingPartsStep::TearDownGUI()
{
  RemoveGUIObservers();
}

//----------------------------------------------------------------------------
// MoveModel:
//  - Check if transformation parent node of model exist
//  - If yes, check the name (pattern: modelName-transform)
//  - If the same, select the node in the transformation widget
//  - If not create a new one and select it
//  - If no transformation node as parent, create a new one and select it
void vtkOsteoPlanMovingPartsStep::MoveModel()
{
  if(this->ModelToMoveSelector->GetSelected())
    {
    vtkMRMLModelNode* SelectedModel = vtkMRMLModelNode::SafeDownCast(this->ModelToMoveSelector->GetSelected());
    if(SelectedModel)
      {
      std::string ModelName = SelectedModel->GetName();
      std::string transformName = ModelName + "-transform";

      vtkMRMLTransformNode* transformationNode = SelectedModel->GetParentTransformNode();
      if(transformationNode)          // Node already existing ?
        {
        // Yes
        if(!strcmp(transformationNode->GetName(),transformName.c_str()))       // Same Name ?
          {
          // Yes -> Select It
          this->ParentTransformationNode = vtkMRMLLinearTransformNode::SafeDownCast(transformationNode);
          }
        else
          {
          // No -> Create a new one
          this->ParentTransformationNode = vtkMRMLLinearTransformNode::New();
          this->ParentTransformationNode->SetName(transformName.c_str());
          this->GetGUI()->GetMRMLScene()->AddNode(this->ParentTransformationNode);
          this->ParentTransformationNode->Delete();

          // TODO: Move all transformation until top, not just the first parent
          vtkMRMLTransformNode* previousParentNode = vtkMRMLTransformNode::SafeDownCast(SelectedModel->GetParentTransformNode());
          this->ParentTransformationNode->SetAndObserveTransformNodeID(previousParentNode->GetID());

          // Drop Model under TransformationNode
          SelectedModel->SetAndObserveTransformNodeID(this->ParentTransformationNode->GetID());
          SelectedModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
          this->GetGUI()->GetMRMLScene()->InvokeEvent(vtkMRMLScene::SceneEditedEvent);

          // Add model to the list of transformed models
          this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->AddItem(SelectedModel);
          }
        }
      else
        {
        // No -> Create a new one
        this->ParentTransformationNode = vtkMRMLLinearTransformNode::New();
        this->ParentTransformationNode->SetName(transformName.c_str());
        this->GetGUI()->GetMRMLScene()->AddNode(this->ParentTransformationNode);
        this->ParentTransformationNode->Delete();

        // Drop Model under TransformationNode
        SelectedModel->SetAndObserveTransformNodeID(this->ParentTransformationNode->GetID());
        SelectedModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
        this->GetGUI()->GetMRMLScene()->InvokeEvent(vtkMRMLScene::SceneEditedEvent);

        // Add model to the list of transformed models
        this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->AddItem(SelectedModel);

        }

      // Select Transformation Node (Selected or Created)
      if(this->TransformationEditor)
        {
        this->TransformationEditor->GetTransformEditSelectorWidget()->SetSelected(this->ParentTransformationNode);
        }

      }
    }

}
