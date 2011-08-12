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
#include "vtkCollisionDetectionFilter.h"
#include "vtkRenderer.h"
#include "vtkProperty.h"

#include "vtkCylinder.h"
#include "vtkClipPolyData.h"
#include "vtkPlane.h"

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

  // this->ListOfModels = vtkCollection::New();
  // this->ListOfFiducialLists = vtkCollection::New();

  this->TitleBackgroundColor[0] = 1;
  this->TitleBackgroundColor[1] = 0.98;
  this->TitleBackgroundColor[2] = 0.74;

  this->ProcessingCallback = false;

  this->ScrewDiameterScale = NULL;
  this->ScrewDiameter = 1;
  this->ScrewHeightScale = NULL;
  this->ScrewHeight = 20;
  this->ScrewCylinder = NULL;

  this->ApplyScrewButton = NULL;

}

//----------------------------------------------------------------------------
vtkOsteoPlanPlacingFiducialsStep::~vtkOsteoPlanPlacingFiducialsStep()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->MainFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->FiducialOnModel);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->StartPlacingFiducials);

  // if(this->ListOfModels)
  //   {
  //   this->ListOfModels->Delete();
  //   }

  // if(this->ListOfFiducialLists)
  //   {
  //   this->ListOfFiducialLists->Delete();
  //   }


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
    this->ScrewHeightScale->GetWidget()->SetRange(1,40);
    this->ScrewHeightScale->GetWidget()->SetValue(20);
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
  // Fill in
  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }

  if(this->MRMLScene == vtkMRMLScene::SafeDownCast(caller) 
     && event == vtkMRMLScene::NodeRemovedEvent)
    {
    // Check vtkCollections are still synchronized
    if(this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetNumberOfItems() == this->GetGUI()->GetOsteoPlanNode()->GetListOfFiducialLists()->GetNumberOfItems())
      {
      for(int i = 0; i < this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetNumberOfItems(); i++)
        {
        // Check which node has been removed;
        if(this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->GetItemAsObject(i) == callData || this->GetGUI()->GetOsteoPlanNode()->GetListOfFiducialLists()->GetItemAsObject(i) == callData)
          {
          this->GetGUI()->GetOsteoPlanNode()->GetListOfModels()->RemoveItem(i);
          this->GetGUI()->GetOsteoPlanNode()->GetListOfFiducialLists()->RemoveItem(i);
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
    
          this->StartPlacingFiducials->SetText("Show Screw");
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
      
        this->StartPlacingFiducials->SetText("Hide Screw");
        this->StartPlacingFiducials->SetBackgroundColor(color->LightestRed);
        this->StartPlacingFiducials->SetActiveBackgroundColor(color->LightestRed);

  this->ApplyScrewButton->SetBackgroundColor(color->SliceGUIGreen);
  this->ApplyScrewButton->SetActiveBackgroundColor(color->SliceGUIGreen);
  this->ApplyScrewButton->SetEnabled(1);

    /////////
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
    /////////

    //AddPairModelFiducial();

        }
      else
        {
        this->bPlacingFiducials = false;

        vtkSlicerApplication* app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
        vtkSlicerColor* color = app->GetSlicerTheme()->GetSlicerColors();
      
        this->StartPlacingFiducials->SetText("Show Screw");
        this->StartPlacingFiducials->SetBackgroundColor(color->SliceGUIGreen);
        this->StartPlacingFiducials->SetActiveBackgroundColor(color->SliceGUIGreen);

  this->ApplyScrewButton->SetBackgroundColor(color->White);
  this->ApplyScrewButton->SetActiveBackgroundColor(color->White);
  this->ApplyScrewButton->SetEnabled(0);

  /////////
  if(this->ScrewCylinder)
    {
      this->ScrewCylinder->SetEnabled(0);
    }
    this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->Render();

  ////////

        //AddPairModelFiducial();
        }
      }


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


//----------------------------------------------------------------------------
void vtkOsteoPlanPlacingFiducialsStep::MarkScrewPosition()
{
  //TODO: Use collision detection ? Clipping ?
  /*
  vtkPolyData* cylinderPolydata = vtkPolyData::New();
  this->ScrewCylinder->GetPolyData(cylinderPolydata);

  vtkMatrix4x4* matrix0 = vtkMatrix4x4::New();
  vtkMatrix4x4* matrix1 = vtkMatrix4x4::New();

  vtkCollisionDetectionFilter* ScrewMark = vtkCollisionDetectionFilter::New();
  ScrewMark->SetInput(0,cylinderPolydata);
  ScrewMark->SetMatrix(0,matrix0);
  ScrewMark->SetInput(1,this->SelectedModel->GetPolyData());
  ScrewMark->SetMatrix(1,matrix1);
  ScrewMark->SetCollisionModeToAllContacts();

  vtkPolyDataMapper* scMapper = vtkPolyDataMapper::New();
  scMapper->SetInputConnection(ScrewMark->GetOutputPort(0));
  //scMapper->SetInputConnection(ScrewMark->GetContactsOutputPort());
  //scMapper->SetResolveCoincidentTopologyToPolygonOffset();

  vtkActor* scActor = vtkActor::New();
  scActor->SetMapper(scMapper);
  (scActor->GetProperty())->SetColor(1.0 ,0.0 ,0.0);
  (scActor->GetProperty())->SetLineWidth(3.0);
  (scActor->GetProperty())->BackfaceCullingOn();

  this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(scActor);
  this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->Render();


  scMapper->Delete();
  scActor->Delete();
  matrix0->Delete();
  matrix1->Delete();
  cylinderPolydata->Delete();
  */


  vtkCylinder* cylinderAlgo = vtkCylinder::New();
  this->ScrewCylinder->GetCylinder(cylinderAlgo);

  vtkPlane* plane1 = vtkPlane::New();
  this->ScrewCylinder->GetPlane1(plane1);

  vtkPlane* plane2 = vtkPlane::New();
  this->ScrewCylinder->GetPlane2(plane2);

  vtkClipPolyData* ScrewPlane1 = vtkClipPolyData::New();
  ScrewPlane1->SetClipFunction(plane1);
  ScrewPlane1->GenerateClippedOutputOn();
  ScrewPlane1->SetInput(this->SelectedModel->GetPolyData());

  vtkClipPolyData* ScrewPlane2 = vtkClipPolyData::New();
  ScrewPlane2->SetClipFunction(plane2);
  ScrewPlane2->GenerateClippedOutputOn();
  ScrewPlane2->SetInput(ScrewPlane1->GetClippedOutput());

  vtkClipPolyData* ScrewHole = vtkClipPolyData::New();
  ScrewHole->SetClipFunction(cylinderAlgo);
  ScrewHole->GenerateClippedOutputOn();
  ScrewHole->GenerateClipScalarsOn();
  ScrewHole->InsideOutOn();
  ScrewHole->SetInput(ScrewPlane2->GetClippedOutput());

  vtkPolyDataMapper* hMapper = vtkPolyDataMapper::New();
  hMapper->SetInput(ScrewHole->GetOutput());

  vtkActor* hActor = vtkActor::New();
  hActor->SetMapper(hMapper);
  hActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
  hActor->SetVisibility(1);

  this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->GetMainViewer()->GetRenderer()->AddActor(hActor);
  this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->Render();


  plane1->Delete();
  plane2->Delete();
  ScrewPlane1->Delete();
  ScrewPlane2->Delete();
  hActor->Delete();
  hMapper->Delete();
  ScrewHole->Delete();
  cylinderAlgo->Delete();


}
