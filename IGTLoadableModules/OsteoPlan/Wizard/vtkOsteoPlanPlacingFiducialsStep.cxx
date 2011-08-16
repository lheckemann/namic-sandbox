/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#include "vtkOsteoPlanPlacingFiducialsStep.h"

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

#include "vtkCylinderWidget.h"
#include "vtkRenderer.h"

#include "vtkCylinder.h"
#include "vtkClipPolyData.h"
#include "vtkPlane.h"
#include "vtkImplicitBoolean.h"
#include "vtkTesselateBoundaryLoops.h"

#include "vtkTransformPolyDataFilter.h"


#define DELETE_IF_NULL_WITH_SETPARENT_NULL(obj) \
  if (obj)                                      \
    {                                           \
    obj->SetParent(NULL);                       \
    obj->Delete();                              \
    obj = NULL;                                 \
    };

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkOsteoPlanPlacingFiducialsStep);
vtkCxxRevisionMacro(vtkOsteoPlanPlacingFiducialsStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkOsteoPlanPlacingFiducialsStep::vtkOsteoPlanPlacingFiducialsStep()
{
  this->SetTitle("Place Fiducials");
  this->SetDescription("Place fiducials to mark screws position");

  this->TitleBackgroundColor[0]   = 1;
  this->TitleBackgroundColor[1]   = 0.98;
  this->TitleBackgroundColor[2]   = 0.74;

  this->MainFrame                 = NULL;
  this->FiducialOnModel           = NULL;
  this->StartPlacingFiducials     = NULL;
  this->SelectedModel             = NULL;
  this->ScrewDiameterScale        = NULL;
  this->ScrewHeightScale          = NULL;
  this->ScrewCylinder             = NULL;
  this->ApplyScrewButton          = NULL;

  this->ScrewDiameter             = 1;
  this->ScrewHeight               = 40;

  this->bPlacingFiducials         = false;
  //  this->modelNodeInsideCollection = false;

  this->ProcessingCallback        = false;

}

//----------------------------------------------------------------------------
vtkOsteoPlanPlacingFiducialsStep::~vtkOsteoPlanPlacingFiducialsStep()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->MainFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->FiducialOnModel);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->StartPlacingFiducials);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->ScrewDiameterScale);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->ScrewHeightScale);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->ApplyScrewButton);

  if(this->ScrewCylinder)
    {
    this->ScrewCylinder->Delete();
    }

}

//----------------------------------------------------------------------------
void vtkOsteoPlanPlacingFiducialsStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWidget           *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  vtkSlicerApplication  *app    = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  vtkSlicerColor        *color  = app->GetSlicerTheme()->GetSlicerColors();

  //-------------------------------------------------------
  // Placing Screws Frame

  if(!this->FiducialOnModel)
    {
    this->FiducialOnModel = vtkSlicerNodeSelectorWidget::New();
    }
  if(!this->FiducialOnModel->IsCreated())
    {
    this->FiducialOnModel->SetParent(parent);
    this->FiducialOnModel->Create();
    this->FiducialOnModel->SetNewNodeEnabled(0);
    this->FiducialOnModel->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
    this->FiducialOnModel->SetMRMLScene(this->GetLogic()->GetMRMLScene());
    this->FiducialOnModel->UpdateMenu();
    }

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

  if(!this->StartPlacingFiducials)
    {
    this->StartPlacingFiducials = vtkKWPushButton::New();
    }
  if(!this->StartPlacingFiducials->IsCreated())
    {
    this->StartPlacingFiducials->SetParent(parent);
    this->StartPlacingFiducials->Create();
    this->StartPlacingFiducials->SetText("Show Screw");
    this->StartPlacingFiducials->SetBackgroundColor(color->SliceGUIGreen);
    this->StartPlacingFiducials->SetActiveBackgroundColor(color->SliceGUIGreen);
    this->StartPlacingFiducials->SetEnabled(1);
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
               this->FiducialOnModel->GetWidgetName(),
               this->ScrewDiameterScale->GetWidgetName(),
               this->ScrewHeightScale->GetWidgetName(),
               this->StartPlacingFiducials->GetWidgetName(),
               this->ApplyScrewButton->GetWidgetName());

  //-------------------------------------------------------

  this->AddGUIObservers();

  UpdateGUI();
}

//----------------------------------------------------------------------------
void vtkOsteoPlanPlacingFiducialsStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkOsteoPlanPlacingFiducialsStep::HandleMouseEvent(vtkSlicerInteractorStyle* style)
{
}


//---------------------------------------------------------------------------
void vtkOsteoPlanPlacingFiducialsStep::ProcessMRMLEvents ( vtkObject *caller,
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
void vtkOsteoPlanPlacingFiducialsStep::ProcessGUIEvents(vtkObject *caller,
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
/*
  if(this->SelectedModel)
  {

  if(this->StartPlacingFiducials)
  {
  vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

  this->StartPlacingFiducials->SetText("Show Screw");
  this->StartPlacingFiducials->SetBackgroundColor(color->SliceGUIGreen);
  this->StartPlacingFiducials->SetActiveBackgroundColor(color->SliceGUIGreen);
  this->StartPlacingFiducials->SetEnabled(1);
  }

  }
  else
  {
  // Disable button
  if(this->StartPlacingFiducials)
  {
  vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

  this->StartPlacingFiducials->SetText("Select a model where to place fiducials");
  this->StartPlacingFiducials->SetBackgroundColor(color->White);
  this->StartPlacingFiducials->SetActiveBackgroundColor(color->White);
  this->StartPlacingFiducials->SetEnabled(0);
  }
  }
*/
      }

    //--------------------------------------------------
    // Show Screw Button

    if(this->StartPlacingFiducials == vtkKWPushButton::SafeDownCast(caller)
       && event == vtkKWPushButton::InvokedEvent)
      {
      if(!this->bPlacingFiducials)
        {
        this->bPlacingFiducials = true;

        vtkSlicerApplication *app   = vtkSlicerApplication::SafeDownCast(this->GetApplication());
        vtkSlicerColor       *color = app->GetSlicerTheme()->GetSlicerColors();

        this->StartPlacingFiducials->SetText("Hide Screw");
        this->StartPlacingFiducials->SetBackgroundColor(color->LightestRed);
        this->StartPlacingFiducials->SetActiveBackgroundColor(color->LightestRed);

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
        this->bPlacingFiducials = false;

        vtkSlicerApplication *app   = vtkSlicerApplication::SafeDownCast(this->GetApplication());
        vtkSlicerColor       *color = app->GetSlicerTheme()->GetSlicerColors();

        this->StartPlacingFiducials->SetText("Show Screw");
        this->StartPlacingFiducials->SetBackgroundColor(color->SliceGUIGreen);
        this->StartPlacingFiducials->SetActiveBackgroundColor(color->SliceGUIGreen);

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


    }
}


//-----------------------------------------------------------------------------
void vtkOsteoPlanPlacingFiducialsStep::AddGUIObservers()
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

  if(this->StartPlacingFiducials)
    {
    this->StartPlacingFiducials->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
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
void vtkOsteoPlanPlacingFiducialsStep::RemoveGUIObservers()
{
  if(this->FiducialOnModel)
    {
    this->FiducialOnModel->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  if(this->StartPlacingFiducials)
    {
    this->StartPlacingFiducials->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
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
void vtkOsteoPlanPlacingFiducialsStep::UpdateGUI()
{

}

//----------------------------------------------------------------------------
void vtkOsteoPlanPlacingFiducialsStep::HideUserInterface()
{
  TearDownGUI(); // HideUserInterface deletes the reference to the scene, so TearDownGUI shall be done before calling HideUserInterface
  Superclass::HideUserInterface();
}

//----------------------------------------------------------------------------
void vtkOsteoPlanPlacingFiducialsStep::TearDownGUI()
{
  RemoveGUIObservers();
}


//----------------------------------------------------------------------------
// MarkScrewPosition:
//  - Check if model has been moved
//    - If yes, applied transform (to be able to clip)
//  - Create Clipper (finite cylinder)
//  - Clip model
void vtkOsteoPlanPlacingFiducialsStep::MarkScrewPosition()
{
  // Check if model has a transform
  //   = is on vtkCollection
  for(int i = 0; i < this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetNumberOfItems(); i++)
    {
    vtkMRMLModelNode* mNode = vtkMRMLModelNode::SafeDownCast(this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetItemAsObject(i));
    if(this->SelectedModel == mNode)
      {
      // Node is present
      //  = Apply his transform
      vtkMRMLLinearTransformNode* tNode = vtkMRMLLinearTransformNode::SafeDownCast(this->GetGUI()->GetOsteoPlanNode()->GetListOfTransforms()->GetItemAsObject(i));
      this->SelectedModel->ApplyTransform(tNode->GetMatrixTransformToParent());

      this->SelectedModel->SetAndObserveTransformNodeID(NULL);
      this->SelectedModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
      this->GetGUI()->GetMRMLScene()->InvokeEvent(vtkMRMLScene::SceneEditedEvent);
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

  // Apply clipper to polydata (model)
  vtkClipPolyData* ScrewHole = vtkClipPolyData::New();
  ScrewHole->SetClipFunction(createCylinder);
  ScrewHole->GenerateClippedOutputOn();
  ScrewHole->SetInput(this->SelectedModel->GetPolyData());
  this->SelectedModel->SetAndObservePolyData(ScrewHole->GetOutput());
  this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->Render();

  createCylinder->Delete();
  plane1->Delete();
  plane2->Delete();
  ScrewHole->Delete();
  cylinderAlgo->Delete();

}

//----------------------------------------------------------------------------
/*
void vtkOsteoPlanPlacingFiducialsStep::AddPairModelFiducial()
{
    if(this->FiducialOnModel && this->SelectedModel)
    {
    if(this->bPlacingFiducials == true)
    {
    this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetCurrentInteractionMode(vtkMRMLInteractionNode::Place);
    this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetSelected(1);
    this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetPlaceModePersistence(1);

    this->modelNodeInsideCollection = false;
    int modelPosition = 0;

    for(int i = 0; i < this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetNumberOfItems();i++)
    {
    vtkMRMLModelNode* listModel = reinterpret_cast<vtkMRMLModelNode*>(this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetItemAsObject(i));
    if(!strcmp(this->SelectedModel->GetID(),listModel->GetID()))
    {
    this->modelNodeInsideCollection = true;
    modelPosition = i;
    }
    }

    if(!this->modelNodeInsideCollection)
    {
    // Add Model to the List of models who have a fiducial list associated
    this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->AddItem(this->SelectedModel);

    // Create the fiducial list with the name of the model
    vtkMRMLFiducialListNode* fiducialListConnectedToModel = vtkMRMLFiducialListNode::New();
    char fiducialListName[128];
    sprintf(fiducialListName,"%s-fiducialList",this->SelectedModel->GetName());
    fiducialListConnectedToModel->SetName(fiducialListName);
    fiducialListConnectedToModel->SetGlyphTypeFromString("Sphere3D");
    fiducialListConnectedToModel->SetSymbolScale(2.0);
    fiducialListConnectedToModel->SetTextScale(0);

    // Add Fiducial list to the list of fiducial list who have a model associated
    this->GetGUI()->GetOsteoPlanNode()->GetListOfFiducialLists()->AddItem(fiducialListConnectedToModel);

    // Add fiducial list to the scene and set it as active
    this->GetGUI()->GetMRMLScene()->AddNode(fiducialListConnectedToModel);
    this->GetGUI()->GetApplicationLogic()->GetSelectionNode()->SetActiveFiducialListID(fiducialListConnectedToModel->GetID());
    fiducialListConnectedToModel->Delete();
    }
    else
    {
    // Set fiducial list corresponding to the model as active
    vtkMRMLFiducialListNode* fiducialListAlreadyConnectedToModel = reinterpret_cast<vtkMRMLFiducialListNode*>(this->GetGUI()->GetOsteoPlanNode()->GetListOfFiducialLists()->GetItemAsObject(modelPosition));
    this->GetGUI()->GetApplicationLogic()->GetSelectionNode()->SetActiveFiducialListID(fiducialListAlreadyConnectedToModel->GetID());
    }

    }
    else
    {
    this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetCurrentInteractionMode(vtkMRMLInteractionNode::ViewTransform);
    this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetSelected(0);
    this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetPlaceModePersistence(0);

    //      this->placeMarkersButton->SetText("Start Placing Markers");
    }
    }
}
*/
