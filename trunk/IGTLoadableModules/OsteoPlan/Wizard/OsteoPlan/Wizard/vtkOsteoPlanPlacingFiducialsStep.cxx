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


#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWPushButton.h"
 

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

  this->MainFrame=NULL;

  this->FiducialOnModel = NULL;
  this->StartPlacingFiducials = NULL;
  this->SelectedModel = NULL;

  this->bPlacingFiducials = false;
  this->modelNodeInsideCollection = false;

  this->ListOfModels = vtkCollection::New();
  this->ListOfFiducialLists = vtkCollection::New();

  this->TitleBackgroundColor[0] = 0.8;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 0.8;

  this->ProcessingCallback = false;
}

//----------------------------------------------------------------------------
vtkOsteoPlanPlacingFiducialsStep::~vtkOsteoPlanPlacingFiducialsStep()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(MainFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(FiducialOnModel);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(StartPlacingFiducials);

  if(this->ListOfModels)
    {
      this->ListOfModels->Delete();
    }

  if(this->ListOfFiducialLists)
    {
      this->ListOfFiducialLists->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkOsteoPlanPlacingFiducialsStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWidget* parent = this->GetGUI()->GetWizardWidget()->GetClientArea();
  vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();

  //-------------------------------------------------------
  //-------------------------------------------------------
  //                     Placing Fiducials Frame

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

  if(!this->StartPlacingFiducials)
    {
      this->StartPlacingFiducials = vtkKWPushButton::New();
    }
  if(!this->StartPlacingFiducials->IsCreated())
    {
      this->StartPlacingFiducials->SetParent(parent);
      this->StartPlacingFiducials->Create();
      this->StartPlacingFiducials->SetText("Select a model where to place fiducials");
      this->StartPlacingFiducials->SetBackgroundColor(color->White);
      this->StartPlacingFiducials->SetActiveBackgroundColor(color->White);
      this->StartPlacingFiducials->SetEnabled(0);
    }

  this->Script("pack %s %s -side top -fill x -padx 0 -pady 2",
         this->FiducialOnModel->GetWidgetName(),
         this->StartPlacingFiducials->GetWidgetName());
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
  // Fill in
  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }

  if(this->MRMLScene == vtkMRMLScene::SafeDownCast(caller) 
     && event == vtkMRMLScene::NodeRemovedEvent)
    {
    // Check vtkCollections are still synchronized
    if(this->ListOfModels->GetNumberOfItems() == this->ListOfFiducialLists->GetNumberOfItems())
      {
      for(int i = 0; i < this->ListOfModels->GetNumberOfItems(); i++)
        {
        // Check which node has been removed;
        if(this->ListOfModels->GetItemAsObject(i) == callData || this->ListOfFiducialLists->GetItemAsObject(i) == callData)
          {
          this->ListOfModels->RemoveItem(i);
          this->ListOfFiducialLists->RemoveItem(i);
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
      if(this->FiducialOnModel == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
   && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
  {
    this->SelectedModel = vtkMRMLModelNode::SafeDownCast(this->FiducialOnModel->GetSelected());
    if(this->SelectedModel)
      {
        // Enable button
        if(this->StartPlacingFiducials)
    {
      vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
      vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();
    
      this->StartPlacingFiducials->SetText("Start Placing Fiducials");
      this->StartPlacingFiducials->SetBackgroundColor(color->SliceGUIGreen);
      this->StartPlacingFiducials->SetActiveBackgroundColor(color->SliceGUIGreen);
      this->StartPlacingFiducials->SetEnabled(1);
    }

        // Check model list
        /*
        if (this->ListOfModels)
    {
      for(int i = 0; i < this->ListOfModels->GetNumberOfItems(); i++)
        {
          if(this->SelectedModel == this->ListOfModels->GetItemAsObject(i))
      {
        vtkMRMLFiducialListNode* correspondingFiducialList = reinterpret_cast<vtkMRMLFiducialListNode*>(this->ListOfFiducialLists->GetItemAsObject(i));
        this->GetGUI()->GetApplicationLogic()->GetSelectionNode()->SetActiveFiducialListID(correspondingFiducialList->GetID());
      }
          else
      {
        AddPairModelFiducial();
      }
        }
    }
        */

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
  }

      if(this->StartPlacingFiducials == vtkKWPushButton::SafeDownCast(caller)
   && event == vtkKWPushButton::InvokedEvent)
  {
    if(!this->bPlacingFiducials)
      {
        this->bPlacingFiducials = true;

        vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
        vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();
      
        this->StartPlacingFiducials->SetText("Stop Placing Fiducials");
        this->StartPlacingFiducials->SetBackgroundColor(color->LightestRed);
        this->StartPlacingFiducials->SetActiveBackgroundColor(color->LightestRed);

        AddPairModelFiducial();

      }
    else
      {
        this->bPlacingFiducials = false;

        vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
        vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();
      
        this->StartPlacingFiducials->SetText("Start Placing Fiducials");
        this->StartPlacingFiducials->SetBackgroundColor(color->SliceGUIGreen);
        this->StartPlacingFiducials->SetActiveBackgroundColor(color->SliceGUIGreen);

        AddPairModelFiducial();
      }
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
  //events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  //events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
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
void vtkOsteoPlanPlacingFiducialsStep::AddPairModelFiducial()
{
  if(this->FiducialOnModel && this->SelectedModel)
    {
    if(this->bPlacingFiducials == true)
      {
  this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetCurrentInteractionMode(vtkMRMLInteractionNode::Place);
      this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetSelected(1);
      this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetPlaceModePersistence(1);

      //      this->placeMarkersButton->SetText("Stop Placing Markers");

      //vtkMRMLModelNode* selectedModelNode = reinterpret_cast<vtkMRMLModelNode*>(this->modelSelector->GetSelected());
      this->modelNodeInsideCollection = false;
      int modelPosition = 0;

      for(int i = 0; i < this->ListOfModels->GetNumberOfItems();i++)
        {
        vtkMRMLModelNode* listModel = reinterpret_cast<vtkMRMLModelNode*>(this->ListOfModels->GetItemAsObject(i));
        if(!strcmp(this->SelectedModel->GetID(),listModel->GetID()))
          {
          this->modelNodeInsideCollection = true;
          modelPosition = i;
          }
        }

      std::cerr << "INSIDE: " << this->modelNodeInsideCollection << std::endl;

      if(!this->modelNodeInsideCollection)
        {
        // Add Model to the List of models who have a fiducial list associated
        this->ListOfModels->AddItem(this->SelectedModel);

        // Create the fiducial list with the name of the model
        vtkMRMLFiducialListNode* fiducialListConnectedToModel = vtkMRMLFiducialListNode::New();
        char fiducialListName[128];
        sprintf(fiducialListName,"%s-fiducialList",this->SelectedModel->GetName());
        fiducialListConnectedToModel->SetName(fiducialListName);
      
        // Add Fiducial list to the list of fiducial list who have a model associated
        this->ListOfFiducialLists->AddItem(fiducialListConnectedToModel);

        // Add fiducial list to the scene and set it as active
        this->GetGUI()->GetMRMLScene()->AddNode(fiducialListConnectedToModel);
        this->GetGUI()->GetApplicationLogic()->GetSelectionNode()->SetActiveFiducialListID(fiducialListConnectedToModel->GetID());
        fiducialListConnectedToModel->Delete();
        }
      else
        {
        // Set fiducial list corresponding to the model as active
        vtkMRMLFiducialListNode* fiducialListAlreadyConnectedToModel = reinterpret_cast<vtkMRMLFiducialListNode*>(this->ListOfFiducialLists->GetItemAsObject(modelPosition));
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
