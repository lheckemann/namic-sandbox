/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#include "vtkOsteoPlanPlacingScrewsStep.h"

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
#include "vtkCollection.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkMRMLInteractionNode.h"

#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLOsteoPlanNode.h"

#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWPushButton.h"
#include "vtkKWScaleWithLabel.h"
#include "vtkKWScale.h"
#include "vtkKWListBox.h"
#include "vtkKWOptions.h"

#include "vtkCylinderWidget.h"
#include "vtkRenderer.h"

#include "vtkCylinder.h"
#include "vtkClipPolyData.h"
#include "vtkPlane.h"
#include "vtkImplicitBoolean.h"
#include "vtkTransformPolyDataFilter.h"

#include "vtkIntArray.h"

#define DELETE_IF_NULL_WITH_SETPARENT_NULL(obj) \
  if (obj)                                      \
    {                                           \
    obj->SetParent(NULL);                       \
    obj->Delete();                              \
    obj = NULL;                                 \
    };

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkOsteoPlanPlacingScrewsStep);
vtkCxxRevisionMacro(vtkOsteoPlanPlacingScrewsStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkOsteoPlanPlacingScrewsStep::vtkOsteoPlanPlacingScrewsStep()
{
  this->SetTitle("Place Screws");
  this->SetDescription("Place screws on bones");

  this->TitleBackgroundColor[0]   = 1;
  this->TitleBackgroundColor[1]   = 0.98;
  this->TitleBackgroundColor[2]   = 0.74;

  this->MainFrame          = NULL;
  this->FiducialOnModel    = NULL;
  this->ListOfModelsFrame  = NULL;
  this->ButtonsFrame       = NULL;
  this->StartPlacingScrews = NULL;
  this->SelectedModel      = NULL;
  this->ScrewDiameterScale = NULL;
  this->ScrewHeightScale   = NULL;
  this->ScrewCylinder      = NULL;
  this->ApplyScrewButton   = NULL;

  this->AddModelButton    = NULL;
  this->RemoveModelButton = NULL;
  this->ClearListButton   = NULL;
  this->ModelsToClip      = NULL;
  this->ModelsToClipCollection = vtkCollection::New();

  this->ScrewDiameter             = 1;
  this->ScrewHeight               = 40;

  this->bPlacingScrews         = false;

  this->ProcessingCallback        = false;

}

//----------------------------------------------------------------------------
vtkOsteoPlanPlacingScrewsStep::~vtkOsteoPlanPlacingScrewsStep()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->MainFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->FiducialOnModel);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->ListOfModelsFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->ButtonsFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->StartPlacingScrews);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->ScrewDiameterScale);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->ScrewHeightScale);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->ApplyScrewButton);

  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->AddModelButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->RemoveModelButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->ClearListButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->ModelsToClip);


  if(this->ScrewCylinder)
    {
    this->ScrewCylinder->Delete();
    }

  if(this->ModelsToClipCollection)
    {
    this->ModelsToClipCollection->Delete();
    }

}

//----------------------------------------------------------------------------
void vtkOsteoPlanPlacingScrewsStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWidget           *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  vtkSlicerApplication  *app    = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  vtkSlicerColor        *color  = app->GetSlicerTheme()->GetSlicerColors();

  //-------------------------------------------------------
  // Placing Screws Frame

  if(!this->ListOfModelsFrame)
    {
    this->ListOfModelsFrame = vtkKWFrameWithLabel::New();
    }
  if(!this->ListOfModelsFrame->IsCreated())
    {
    this->ListOfModelsFrame->SetParent(parent);
    this->ListOfModelsFrame->Create();
    this->ListOfModelsFrame->SetLabelText("Models to clip");
    }

  // Node Selector
  if(!this->FiducialOnModel)
    {
    this->FiducialOnModel = vtkSlicerNodeSelectorWidget::New();
    }
  if(!this->FiducialOnModel->IsCreated())
    {
    this->FiducialOnModel->SetParent(ListOfModelsFrame->GetFrame());
    this->FiducialOnModel->Create();
    this->FiducialOnModel->SetNewNodeEnabled(0);
    this->FiducialOnModel->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
    this->FiducialOnModel->SetMRMLScene(this->GetLogic()->GetMRMLScene());
    this->FiducialOnModel->UpdateMenu();
    }

  // Buttons (Add, Remove, Clear)
  if(!this->ButtonsFrame)
    {
    this->ButtonsFrame = vtkKWFrame::New();
    }
  if(!this->ButtonsFrame->IsCreated())
    {
    this->ButtonsFrame->SetParent(this->ListOfModelsFrame->GetFrame());
    this->ButtonsFrame->Create();
    }

  if(!this->AddModelButton)
    {
    this->AddModelButton = vtkKWPushButton::New();
    }
  if(!this->AddModelButton->IsCreated())
    {
    this->AddModelButton->SetParent(this->ButtonsFrame);
    this->AddModelButton->Create();
    this->AddModelButton->SetText("Add");
    this->AddModelButton->SetEnabled(1);
    }

  if(!this->RemoveModelButton)
    {
    this->RemoveModelButton = vtkKWPushButton::New();
    }
  if(!this->RemoveModelButton->IsCreated())
    {
    this->RemoveModelButton->SetParent(this->ButtonsFrame);
    this->RemoveModelButton->Create();
    this->RemoveModelButton->SetText("Remove");
    this->RemoveModelButton->SetEnabled(1);
    }

  if(!this->ClearListButton)
    {
    this->ClearListButton = vtkKWPushButton::New();
    }
  if(!this->ClearListButton->IsCreated())
    {
    this->ClearListButton->SetParent(this->ButtonsFrame);
    this->ClearListButton->Create();
    this->ClearListButton->SetText("Clear");
    this->ClearListButton->SetEnabled(1);
    }

  this->Script("pack %s %s %s -side left -expand t -fill x -padx 2 -pady 2",
               this->AddModelButton->GetWidgetName(),
               this->RemoveModelButton->GetWidgetName(),
               this->ClearListButton->GetWidgetName());

  // List box
  if(!this->ModelsToClip)
    {
    this->ModelsToClip = vtkKWListBox::New();
    }
  if(!this->ModelsToClip->IsCreated())
    {
    this->ModelsToClip->SetParent(this->ListOfModelsFrame->GetFrame());
    this->ModelsToClip->Create();
    this->ModelsToClip->SetSelectionMode(vtkKWOptions::SelectionModeSingle);
    }

  this->Script("pack %s %s %s -side top -fill x -padx 2 -pady 2",
               this->FiducialOnModel->GetWidgetName(),
               this->ButtonsFrame->GetWidgetName(),
               this->ModelsToClip->GetWidgetName());

  // Screw parameters
  if(!this->ScrewDiameterScale)
    {
    this->ScrewDiameterScale = vtkKWScaleWithLabel::New();
    }
  if(!this->ScrewDiameterScale->IsCreated())
    {
    this->ScrewDiameterScale->SetParent(parent);
    this->ScrewDiameterScale->Create();
    this->ScrewDiameterScale->SetLabelText("Screw Diameter");
    this->ScrewDiameterScale->GetWidget()->SetRange(1,5);
    }

  if(!this->ScrewHeightScale)
    {
    this->ScrewHeightScale = vtkKWScaleWithLabel::New();
    }
  if(!this->ScrewHeightScale->IsCreated())
    {
    this->ScrewHeightScale->SetParent(parent);
    this->ScrewHeightScale->Create();
    this->ScrewHeightScale->SetLabelText("Screw Length");
    this->ScrewHeightScale->GetWidget()->SetRange(1,80);
    this->ScrewHeightScale->GetWidget()->SetValue(40);
    }

  if(!this->StartPlacingScrews)
    {
    this->StartPlacingScrews = vtkKWPushButton::New();
    }
  if(!this->StartPlacingScrews->IsCreated())
    {
    this->StartPlacingScrews->SetParent(parent);
    this->StartPlacingScrews->Create();
    this->StartPlacingScrews->SetText("Show Screw");
    this->StartPlacingScrews->SetBackgroundColor(color->SliceGUIGreen);
    this->StartPlacingScrews->SetActiveBackgroundColor(color->SliceGUIGreen);
    this->StartPlacingScrews->SetEnabled(1);
    }

  if(!this->ApplyScrewButton)
    {
    this->ApplyScrewButton = vtkKWPushButton::New();
    }
  if(!this->ApplyScrewButton->IsCreated())
    {
    this->ApplyScrewButton->SetParent(parent);
    this->ApplyScrewButton->Create();
    this->ApplyScrewButton->SetText("Apply Screw");
    this->ApplyScrewButton->SetBackgroundColor(color->White);
    this->ApplyScrewButton->SetActiveBackgroundColor(color->White);
    this->ApplyScrewButton->SetEnabled(0);
    }

  this->Script("pack %s %s %s %s %s -side top -fill x -padx 0 -pady 2",
               this->ListOfModelsFrame->GetWidgetName(),
               this->ScrewDiameterScale->GetWidgetName(),
               this->ScrewHeightScale->GetWidgetName(),
               this->StartPlacingScrews->GetWidgetName(),
               this->ApplyScrewButton->GetWidgetName());

  //-------------------------------------------------------

  this->AddGUIObservers();

  UpdateGUI();
}

//----------------------------------------------------------------------------
void vtkOsteoPlanPlacingScrewsStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkOsteoPlanPlacingScrewsStep::HandleMouseEvent(vtkSlicerInteractorStyle* style)
{
}


//---------------------------------------------------------------------------
void vtkOsteoPlanPlacingScrewsStep::ProcessMRMLEvents ( vtkObject *caller,
                                                        unsigned long event, void *callData )
{
  //--------------------------------------------------
  // Scene Closed

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }

  //--------------------------------------------------
  // Node Removed

  if(this->MRMLScene == vtkMRMLScene::SafeDownCast(caller)
     && event == vtkMRMLScene::NodeRemovedEvent)
    {
    // Check vtkCollections are still synchronized
    if(this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetNumberOfItems() == this->GetGUI()->GetOsteoPlanNode()->GetListOfTransforms()->GetNumberOfItems())
      {
      for(int i = 0; i < this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetNumberOfItems(); i++)
        {
        // Check which node has been removed;
        if(this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetItemAsObject(i) == callData || this->GetGUI()->GetOsteoPlanNode()->GetListOfTransforms()->GetItemAsObject(i) == callData)
          {
          this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->RemoveItem(i);
          this->GetGUI()->GetOsteoPlanNode()->GetListOfTransforms()->RemoveItem(i);
          }
        }
      }
    }
}



//----------------------------------------------------------------------------
void vtkOsteoPlanPlacingScrewsStep::ProcessGUIEvents(vtkObject *caller,
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

    //--------------------------------------------------
    // Model to place screws selected

    if(this->FiducialOnModel == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
       && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
      {
      this->SelectedModel = vtkMRMLModelNode::SafeDownCast(this->FiducialOnModel->GetSelected());
      }

    //--------------------------------------------------
    // Show Screw Button

    if(this->StartPlacingScrews == vtkKWPushButton::SafeDownCast(caller)
       && event == vtkKWPushButton::InvokedEvent)
      {
      if(!this->bPlacingScrews)
        {
        this->bPlacingScrews = true;

        vtkSlicerApplication *app   = vtkSlicerApplication::SafeDownCast(this->GetApplication());
        vtkSlicerColor       *color = app->GetSlicerTheme()->GetSlicerColors();

        this->StartPlacingScrews->SetText("Hide Screw");
        this->StartPlacingScrews->SetBackgroundColor(color->LightestRed);
        this->StartPlacingScrews->SetActiveBackgroundColor(color->LightestRed);

        this->ApplyScrewButton->SetBackgroundColor(color->SliceGUIGreen);
        this->ApplyScrewButton->SetActiveBackgroundColor(color->SliceGUIGreen);
        this->ApplyScrewButton->SetEnabled(1);

        // Create cylinder (screw) if not already existing (else enable it)
        if(!this->ScrewCylinder)
          {
          this->ScrewCylinder = vtkCylinderWidget::New();
          this->ScrewCylinder->SetDefaultRenderer(this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer());
          this->ScrewCylinder->SetCurrentRenderer(this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer());
          this->ScrewCylinder->SetInteractor(this->GetGUI()->GetApplicationGUI()->GetActiveRenderWindowInteractor());
          this->ScrewCylinder->HandlesOn();
          this->ScrewCylinder->SetScalingEnabled(0);
          this->ScrewCylinder->PlaceWidget(0.0, 50.0 ,0.0 ,50.0 ,0.0 ,50.0);
          this->ScrewCylinder->SetRadius(this->ScrewDiameter/2.0);
          this->ScrewCylinder->SetHeight(this->ScrewHeight);
          this->ScrewCylinder->PositionHandles();
          this->ScrewCylinder->SetEnabled(1);
          }
        else
          {
          this->ScrewCylinder->SetEnabled(1);
          }
        this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->Render();
        }
      else
        {
        this->bPlacingScrews = false;

        vtkSlicerApplication *app   = vtkSlicerApplication::SafeDownCast(this->GetApplication());
        vtkSlicerColor       *color = app->GetSlicerTheme()->GetSlicerColors();

        this->StartPlacingScrews->SetText("Show Screw");
        this->StartPlacingScrews->SetBackgroundColor(color->SliceGUIGreen);
        this->StartPlacingScrews->SetActiveBackgroundColor(color->SliceGUIGreen);

        this->ApplyScrewButton->SetBackgroundColor(color->White);
        this->ApplyScrewButton->SetActiveBackgroundColor(color->White);
        this->ApplyScrewButton->SetEnabled(0);

        // Disable cylinder (screw)
        if(this->ScrewCylinder)
          {
          this->ScrewCylinder->SetEnabled(0);
          }
        this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->Render();
        }
      }


    //--------------------------------------------------
    // Change Screw Diameter

    if(this->ScrewDiameterScale->GetWidget() == vtkKWScale::SafeDownCast(caller)
       && event == vtkKWScale::ScaleValueChangingEvent)
      {
      this->ScrewDiameter = this->ScrewDiameterScale->GetWidget()->GetValue();
      if(this->ScrewCylinder)
        {
        this->ScrewCylinder->SetRadius(this->ScrewDiameter/2.0);
        this->ScrewCylinder->PositionHandles();
        this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->Render();
        }
      }

    //--------------------------------------------------
    // Change Screw Length (Height)

    if(this->ScrewHeightScale->GetWidget() == vtkKWScale::SafeDownCast(caller)
       && event == vtkKWScale::ScaleValueChangingEvent)
      {
      this->ScrewHeight = this->ScrewHeightScale->GetWidget()->GetValue();
      if(this->ScrewCylinder)
        {
        this->ScrewCylinder->SetHeight(this->ScrewHeight);
        this->ScrewCylinder->PositionHandles();
        this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->Render();
        }
      }

    //--------------------------------------------------
    // Apply Screw Button

    if(this->ApplyScrewButton == vtkKWPushButton::SafeDownCast(caller)
       && event == vtkKWPushButton::InvokedEvent)
      {
      MarkScrewPosition();
      }

    //--------------------------------------------------
    // List of Nodes

    if(this->AddModelButton == vtkKWPushButton::SafeDownCast(caller)
       && event == vtkKWPushButton::InvokedEvent)
      {
      if(this->SelectedModel && this->ModelsToClip && this->ModelsToClipCollection)
        {
        this->ModelsToClipCollection->AddItem(this->SelectedModel);
        this->ModelsToClip->Append(this->SelectedModel->GetName());
        }
      }


    if(this->RemoveModelButton == vtkKWPushButton::SafeDownCast(caller)
       && event == vtkKWPushButton::InvokedEvent)
      {
      if(this->ModelsToClip && this->ModelsToClipCollection)
        {
        int selection_index = this->ModelsToClip->GetSelectionIndex();
        if(selection_index >= 0)
          {
          for(int i = 0; i < this->ModelsToClipCollection->GetNumberOfItems(); i++)
            {
            vtkMRMLNode* selectionNode = vtkMRMLNode::SafeDownCast(this->ModelsToClipCollection->GetItemAsObject(i));
            if(!strcmp(this->ModelsToClip->GetSelection(),selectionNode->GetName()))
              {
              this->ModelsToClipCollection->RemoveItem(i);
              this->ModelsToClip->DeleteRange(selection_index, selection_index);
              }
            }
          }
        }
      }


    if(this->ClearListButton == vtkKWPushButton::SafeDownCast(caller)
       && event == vtkKWPushButton::InvokedEvent)
      {
      if(this->ModelsToClip && this->ModelsToClipCollection)
        {
        this->ModelsToClipCollection->RemoveAllItems();
        this->ModelsToClip->DeleteAll();
        }
      }
    }
}


//-----------------------------------------------------------------------------
void vtkOsteoPlanPlacingScrewsStep::AddGUIObservers()
{
  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);

  if (this->GetGUI()->GetMRMLScene() != NULL)
    {
    this->GetGUI()->SetAndObserveMRMLSceneEvents(this->GetGUI()->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

  if(this->FiducialOnModel)
    {
    this->FiducialOnModel->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand*)this->GUICallbackCommand);
    }

  if(this->AddModelButton)
    {
    this->AddModelButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }

  if(this->RemoveModelButton)
    {
    this->RemoveModelButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }

  if(this->ClearListButton)
    {
    this->ClearListButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }


  if(this->StartPlacingScrews)
    {
    this->StartPlacingScrews->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }

  this->GetGUI()->GetApplicationGUI()->GetActiveRenderWindowInteractor()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand *)this->GUICallbackCommand);

  if(this->ScrewDiameterScale)
    {
    this->ScrewDiameterScale->GetWidget()
      ->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand*)this->GUICallbackCommand);
    }

  if(this->ScrewHeightScale)
    {
    this->ScrewHeightScale->GetWidget()
      ->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand*)this->GUICallbackCommand);
    }

  if(this->ApplyScrewButton)
    {
    this->ApplyScrewButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
    }


}
//-----------------------------------------------------------------------------
void vtkOsteoPlanPlacingScrewsStep::RemoveGUIObservers()
{
  if(this->FiducialOnModel)
    {
    this->FiducialOnModel->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->AddModelButton)
    {
    this->AddModelButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->RemoveModelButton)
    {
    this->RemoveModelButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->ClearListButton)
    {
    this->ClearListButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->StartPlacingScrews)
    {
    this->StartPlacingScrews->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->ScrewDiameterScale)
    {
    this->ScrewDiameterScale->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->ScrewHeightScale)
    {
    this->ScrewHeightScale->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->ApplyScrewButton)
    {
    this->ApplyScrewButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
}

//--------------------------------------------------------------------------------
void vtkOsteoPlanPlacingScrewsStep::UpdateGUI()
{

}

//----------------------------------------------------------------------------
void vtkOsteoPlanPlacingScrewsStep::HideUserInterface()
{
  TearDownGUI(); // HideUserInterface deletes the reference to the scene, so TearDownGUI shall be done before calling HideUserInterface
  Superclass::HideUserInterface();
}

//----------------------------------------------------------------------------
void vtkOsteoPlanPlacingScrewsStep::TearDownGUI()
{
  RemoveGUIObservers();
}


//----------------------------------------------------------------------------
// MarkScrewPosition:
//  - Check if model has been moved
//    - If yes, applied transform (to be able to clip)
//  - Create Clipper (finite cylinder)
//  - Clip model
void vtkOsteoPlanPlacingScrewsStep::MarkScrewPosition()
{

  // Check if all models selected to be clipped (ModelsToClipCollection) have their transformations applied (necessary to clip)
  // If not, try to find models in the collection of models moved
  // And apply the corresponding transformation
  if(this->ModelsToClipCollection)
    {
    for(int j = 0; j < this->ModelsToClipCollection->GetNumberOfItems(); j++)
      {
      vtkMRMLModelNode* ClipModel = vtkMRMLModelNode::SafeDownCast(this->ModelsToClipCollection->GetItemAsObject(j));

      for(int k = 0; k < this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetNumberOfItems(); k++)
        {
        vtkMRMLModelNode* TransformedModel = vtkMRMLModelNode::SafeDownCast(this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetItemAsObject(k));
        if(ClipModel == TransformedModel)
          {
          if(!strcmp(TransformedModel->GetAttribute("TransformApplied"),"false"))
            {
            vtkMRMLLinearTransformNode* tNode = vtkMRMLLinearTransformNode::SafeDownCast(this->GetGUI()->GetOsteoPlanNode()->GetListOfTransforms()->GetItemAsObject(k));

            if(tNode && tNode->GetMatrixTransformToParent())
              {
              ClipModel->ApplyTransform(tNode->GetMatrixTransformToParent());
              ClipModel->SetAndObserveTransformNodeID(NULL);
              ClipModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
              this->GetGUI()->GetMRMLScene()->InvokeEvent(vtkMRMLScene::SceneEditedEvent);

              TransformedModel->SetAttribute("TransformApplied","true");
              }
            }
          }
        }
      }
    }



  // Create clipper (finite cylinder)
  vtkCylinder* cylinderAlgo = vtkCylinder::New();
  this->ScrewCylinder->GetCylinder(cylinderAlgo);

  vtkPlane* plane1 = vtkPlane::New();
  this->ScrewCylinder->GetPlane1(plane1);

  vtkPlane* plane2 = vtkPlane::New();
  this->ScrewCylinder->GetPlane2(plane2);

  vtkImplicitBoolean* createCylinder = vtkImplicitBoolean::New();
  createCylinder->AddFunction(cylinderAlgo);
  createCylinder->AddFunction(plane1);
  createCylinder->AddFunction(plane2);
  createCylinder->SetOperationTypeToIntersection();


  if(this->ModelsToClipCollection)
    {
    for(int i = 0; i < this->ModelsToClipCollection->GetNumberOfItems(); i++)
      {
      vtkMRMLModelNode *NodeToClip = vtkMRMLModelNode::SafeDownCast(this->ModelsToClipCollection->GetItemAsObject(i));

      // Apply clipper to polydata (model)
      vtkClipPolyData* ScrewHole = vtkClipPolyData::New();
      ScrewHole->SetClipFunction(createCylinder);
      ScrewHole->GenerateClippedOutputOn();
      ScrewHole->SetInput(NodeToClip->GetPolyData());
      NodeToClip->SetAndObservePolyData(ScrewHole->GetOutput());
      ScrewHole->Delete();

      }

    this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->Render();
    }

  createCylinder->Delete();
  plane1->Delete();
  plane2->Delete();
  cylinderAlgo->Delete();
}
