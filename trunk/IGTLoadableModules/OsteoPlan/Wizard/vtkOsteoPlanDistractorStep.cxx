/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#include "vtkOsteoPlanDistractorStep.h"

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
#include "vtkKWCheckButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWMenuButton.h"

#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLOsteoPlanNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLInteractionNode.h"

#include "vtkTransformPolyDataFilter.h"

#define DELETE_IF_NULL_WITH_SETPARENT_NULL(obj) \
  if (obj)                                      \
    {                                           \
    obj->SetParent(NULL);                       \
    obj->Delete();                              \
    obj = NULL;                                 \
    };

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkOsteoPlanDistractorStep);
vtkCxxRevisionMacro(vtkOsteoPlanDistractorStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkOsteoPlanDistractorStep::vtkOsteoPlanDistractorStep()
{
  //------------------------------
  // Wizard Step

  this->SetTitle("Distractor");
  this->SetDescription("Place and actuate distractor");

  this->TitleBackgroundColor[0]  = 0.8;
  this->TitleBackgroundColor[1]  = 0.8;
  this->TitleBackgroundColor[2]  = 0.8;

  //------------------------------
  // Widgets

  this->MovingScale            = NULL;
  this->LoadDistractorButton   = NULL;
  this->BonePlateModelSelector = NULL;
  this->BoneRailModelSelector  = NULL;
  this->ApplyDistractorToBones = NULL;
  this->PlaceFiduButton        = NULL;
  this->RegisterButton         = NULL;
  this->DistractorMenuSelector = NULL;
  this->DistractorSelector     = NULL;

  //------------------------------
  // Transformations

  this->SliderTransformNode   = NULL;
  this->PistonTransformNode   = NULL;
  this->CylinderTransformNode = NULL;
  this->BonePlateTransform    = NULL;

  //------------------------------
  // Registration

  this->boolPlacingFiducials     = false;
  this->RegistrationFiducialList = NULL;
  this->DistToBones              = NULL;

  //------------------------------
  // Models

  this->BonePlateModel = NULL;
  this->BoneRailModel  = NULL;

  //------------------------------
  // Paths

  this->RailModelPath     = "";
  this->SliderModelPath   = "";
  this->PistonModelPath   = "";
  this->CylinderModelPath = "";
  this->xmlPath           = "";

  //------------------------------
  // Distractor

  this->DistractorList     = vtkCollection::New();
  this->DistractorSelected = NULL;
  this->DistractorObject   = NULL;

  //------------------------------
  // Callback

  this->ProcessingCallback       = false;

}

//----------------------------------------------------------------------------
vtkOsteoPlanDistractorStep::~vtkOsteoPlanDistractorStep()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->MovingScale);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->LoadDistractorButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->DistractorSelector);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->BonePlateModelSelector);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->BoneRailModelSelector);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->ApplyDistractorToBones);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->PlaceFiduButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->RegisterButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(this->DistractorMenuSelector);

  if(this->SliderTransformNode)
    {
    this->SliderTransformNode->Delete();
    }

  if(this->PistonTransformNode)
    {
    this->PistonTransformNode->Delete();
    }

  if(this->CylinderTransformNode)
    {
    this->CylinderTransformNode->Delete();
    }

  if(this->BonePlateTransform)
    {
    this->BonePlateTransform->Delete();
    }

  if(this->RegistrationFiducialList)
    {
    this->RegistrationFiducialList->Delete();
    }

  if(this->DistToBones)
    {
    this->DistToBones->Delete();
    this->DistToBones = NULL;
    }

  if(this->DistractorList)
    {
    this->DistractorList->RemoveAllItems();
    this->DistractorList->Delete();
    }

  if(this->DistractorSelected)
    {
    this->SetDistractorSelected(NULL);
    }
}

//----------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWidget          *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();

  if(!this->MovingScale)
    {
    this->MovingScale = vtkKWScale::New();
    }
  if(!this->MovingScale->IsCreated())
    {
    this->MovingScale->SetParent(parent);
    this->MovingScale->Create();
    this->MovingScale->SetRange(-180,180);
    }

  if(!this->LoadDistractorButton)
    {
    this->LoadDistractorButton = vtkKWPushButton::New();
    }
  if(!this->LoadDistractorButton->IsCreated())
    {
    this->LoadDistractorButton->SetParent(parent);
    this->LoadDistractorButton->Create();
    this->LoadDistractorButton->SetText("Load Distractor");
    }

  vtkKWLabel* PlateModelLabel = vtkKWLabel::New();
  PlateModelLabel->SetParent(parent);
  PlateModelLabel->Create();
  PlateModelLabel->SetText("Plate Bone:");
  PlateModelLabel->SetAnchorToWest();

  if(!this->BonePlateModelSelector)
    {
    this->BonePlateModelSelector = vtkSlicerNodeSelectorWidget::New();
    }
  if(!this->BonePlateModelSelector)
    {
    this->BonePlateModelSelector->SetParent(parent);
    this->BonePlateModelSelector->Create();
    this->BonePlateModelSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
    this->BonePlateModelSelector->SetNewNodeEnabled(false);
    this->BonePlateModelSelector->SetMRMLScene(this->GetGUI()->GetMRMLScene());
    this->BonePlateModelSelector->UpdateMenu();
    }

  vtkKWLabel* RailModelLabel = vtkKWLabel::New();
  RailModelLabel->SetParent(parent);
  RailModelLabel->Create();
  RailModelLabel->SetText("Rail Bone:");
  RailModelLabel->SetAnchorToWest();

  if(!this->BoneRailModelSelector)
    {
    this->BoneRailModelSelector = vtkSlicerNodeSelectorWidget::New();
    }
  if(!this->BoneRailModelSelector)
    {
    this->BoneRailModelSelector->SetParent(parent);
    this->BoneRailModelSelector->Create();
    this->BoneRailModelSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
    this->BoneRailModelSelector->SetNewNodeEnabled(false);
    this->BoneRailModelSelector->SetMRMLScene(this->GetGUI()->GetMRMLScene());
    this->BoneRailModelSelector->UpdateMenu();
    }

  if(!this->ApplyDistractorToBones)
    {
    this->ApplyDistractorToBones = vtkKWCheckButton::New();
    }
  if(!this->ApplyDistractorToBones->IsCreated())
    {
    this->ApplyDistractorToBones->SetParent(parent);
    this->ApplyDistractorToBones->Create();
    this->ApplyDistractorToBones->SetText("Apply transformation to bones");
    this->ApplyDistractorToBones->SelectedStateOff();
    }

  if(!this->DistractorMenuSelector)
    {
    this->DistractorMenuSelector = vtkKWMenuButtonWithLabel::New();
    }
  if(!this->DistractorMenuSelector->IsCreated())
    {
    this->DistractorMenuSelector->SetParent(parent);
    this->DistractorMenuSelector->Create();
    this->DistractorMenuSelector->SetLabelText("Distractor:");
    this->DistractorMenuSelector->GetWidget()->GetMenu()->AddRadioButton("None");
    this->DistractorMenuSelector->GetWidget()->SetValue("None");
    }


  vtkKWFrame* RegistrationFrame = vtkKWFrame::New();
  RegistrationFrame->SetParent(parent);
  RegistrationFrame->Create();


  if(!this->PlaceFiduButton)
    {
    this->PlaceFiduButton = vtkKWPushButton::New();
    }
  if(!this->PlaceFiduButton->IsCreated())
    {
    this->PlaceFiduButton->SetParent(RegistrationFrame);
    this->PlaceFiduButton->Create();
    this->PlaceFiduButton->SetText("Place Fiducials");
    this->PlaceFiduButton->SetWidth(8);
    }

  if(!this->RegisterButton)
    {
    this->RegisterButton = vtkKWPushButton::New();
    }
  if(!this->RegisterButton->IsCreated())
    {
    this->RegisterButton->SetParent(RegistrationFrame);
    this->RegisterButton->Create();
    this->RegisterButton->SetText("Register");
    }

  this->Script("pack %s %s -side left -fill x -expand y -padx 2 -pady 2",
               this->PlaceFiduButton->GetWidgetName(),
               this->RegisterButton->GetWidgetName());

  this->Script("pack %s %s %s %s -side top -fill x -expand y -padx 2 -pady 2",
               this->DistractorMenuSelector->GetWidgetName(),
               this->LoadDistractorButton->GetWidgetName(),
               this->MovingScale->GetWidgetName(),
               RegistrationFrame->GetWidgetName());



  this->AddGUIObservers();

  UpdateGUI();

  RegistrationFrame->Delete();
  RailModelLabel->Delete();
  PlateModelLabel->Delete();


}

//----------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::HandleMouseEvent(vtkSlicerInteractorStyle* style)
{
}

//----------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::ProcessGUIEvents(vtkObject *caller,
                                                  unsigned long event, void *callData)
{
  //----------------------------------------------------------------------
  // Moving Scale

  if(this->MovingScale == vtkKWScale::SafeDownCast(caller)
     && event == vtkKWScale::ScaleValueChangingEvent)
    {
    if(this->GetDistractorSelected()->GetDistractorSlider() &&
       this->GetDistractorSelected()->GetDistractorPiston() &&
       this->GetDistractorSelected()->GetDistractorCylinder())
      {
      // Create transforms if not already existing
      if(!this->SliderTransformNode)
        {
        this->SliderTransformNode = vtkMRMLLinearTransformNode::New();
        this->GetGUI()->GetMRMLScene()->AddNode(this->SliderTransformNode);
        }
      if(!this->PistonTransformNode)
        {
        this->PistonTransformNode = vtkMRMLLinearTransformNode::New();
        this->GetGUI()->GetMRMLScene()->AddNode(this->PistonTransformNode);
        }
      if(!this->CylinderTransformNode)
        {
        this->CylinderTransformNode = vtkMRMLLinearTransformNode::New();
        this->GetGUI()->GetMRMLScene()->AddNode(this->CylinderTransformNode);
        }

      // Move Distractor
      if(this->DistToBones)
        {
        this->MoveDistractor(this->MovingScale->GetValue(),
                             this->GetDistractorSelected()->GetDistractorSlider(), this->SliderTransformNode,
                             this->GetDistractorSelected()->GetDistractorPiston(), this->PistonTransformNode,
                             this->GetDistractorSelected()->GetDistractorCylinder(), this->CylinderTransformNode,
                             this->DistToBones->GetLandmarkTransformMatrix());
        }
      else
        {
        this->MoveDistractor(this->MovingScale->GetValue(),
                             this->GetDistractorSelected()->GetDistractorSlider(), this->SliderTransformNode,
                             this->GetDistractorSelected()->GetDistractorPiston(), this->PistonTransformNode,
                             this->GetDistractorSelected()->GetDistractorCylinder(), this->CylinderTransformNode,
                             NULL);

        }

      // Slider Transform
      this->GetDistractorSelected()->GetDistractorSlider()->SetAndObserveTransformNodeID(this->SliderTransformNode->GetID());
      this->GetDistractorSelected()->GetDistractorSlider()->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);

      // Piston Transform
      this->GetDistractorSelected()->GetDistractorPiston()->SetAndObserveTransformNodeID(this->PistonTransformNode->GetID());
      this->GetDistractorSelected()->GetDistractorPiston()->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);

      // Cylinder Transform
      this->GetDistractorSelected()->GetDistractorCylinder()->SetAndObserveTransformNodeID(this->CylinderTransformNode->GetID());
      this->GetDistractorSelected()->GetDistractorCylinder()->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);

      this->GetGUI()->GetMRMLScene()->InvokeEvent(vtkMRMLScene::SceneEditedEvent);
      this->GetGUI()->GetApplicationGUI()->GetActiveViewerWidget()->Render();
      }

    // Apply transformation to bones
    if(this->ApplyDistractorToBones->GetSelectedState())
      {
      if(this->BonePlateModel)
        {
        if(!this->BonePlateTransform)
          {
          this->BonePlateTransform = vtkMRMLLinearTransformNode::New();
          this->GetGUI()->GetMRMLScene()->AddNode(this->BonePlateTransform);
          this->BonePlateModel->SetAndObserveTransformNodeID(this->BonePlateTransform->GetID());
          this->BonePlateModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
          }
        if(this->BonePlateTransform)
          {
          this->MoveBones(this->BonePlateModel, this->BonePlateTransform, this->MovingScale->GetValue());
          }
        }
      }
    }


  //----------------------------------------------------------------------
  // Load Distractor Button

  if(this->LoadDistractorButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
    this->DistractorSelector = vtkKWFileBrowserDialog::New();
    this->DistractorSelector->SetApplication(this->GetApplication());
    this->DistractorSelector->Create();
    this->DistractorSelector->SaveDialogOff();
    this->DistractorSelector->ChooseDirectoryOn();
    this->DistractorSelector->MultipleSelectionOff();
    this->DistractorSelector->Invoke();

    if(this->DistractorSelector->GetStatus() == vtkKWDialog::StatusOK)
      {
      if(this->DistractorSelector->GetFileName())
        {
        this->OpenDistractorFile(this->DistractorSelector->GetFileName());

        vtkSlicerModelsLogic* ModelsLogic = vtkSlicerModelsLogic::New();
        ModelsLogic->SetMRMLScene(this->GetGUI()->GetMRMLScene());

        if(strcmp(this->DistName.c_str(),"")   &&
           strcmp(this->RailModelPath.c_str(),"")    &&
           strcmp(this->SliderModelPath.c_str(),"")  &&
           strcmp(this->PistonModelPath.c_str(), "") &&
           strcmp(this->CylinderModelPath.c_str(),""))
          {
          if(this->GetDistractorSelected())
            {
            this->GetDistractorSelected()->SetDistractorRail(ModelsLogic->AddModel(this->RailModelPath.c_str()));
            this->GetDistractorSelected()->SetDistractorSlider(ModelsLogic->AddModel(this->SliderModelPath.c_str()));
            this->GetDistractorSelected()->SetDistractorPiston(ModelsLogic->AddModel(this->PistonModelPath.c_str()));
            this->GetDistractorSelected()->SetDistractorCylinder(ModelsLogic->AddModel(this->CylinderModelPath.c_str()));
            }


          if(this->DistractorMenuSelector)
            {
            this->DistractorMenuSelector->GetWidget()->GetMenu()->AddRadioButton(this->GetDistractorSelected()->GetDistractorName());
            this->DistractorMenuSelector->GetWidget()->SetValue(this->GetDistractorSelected()->GetDistractorName());
            }

          // this->MovingScale->SetRange(this->GetDistractorSelected()->GetRange());
          this->MovingScale->SetRange(-180,180);

          this->MovingScale->SetValue(0.0);

          ModelsLogic->Delete();
          }
        }
      }
    this->DistractorSelector->Delete();
    this->DistractorSelector = NULL;
    }


  //----------------------------------------------------------------------
  // Bone Plate Selector

  if(this->BonePlateModelSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    if(this->BonePlateModelSelector->GetSelected())
      {
      this->BonePlateModel = vtkMRMLModelNode::SafeDownCast(this->BonePlateModelSelector->GetSelected());
      }
    }

  //----------------------------------------------------------------------
  // Bone Rail Selector

  if(this->BoneRailModelSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    if(this->BoneRailModelSelector->GetSelected())
      {
      this->BoneRailModel = vtkMRMLModelNode::SafeDownCast(this->BoneRailModelSelector->GetSelected());
      }
    }

  //----------------------------------------------------------------------
  // Apply Distractor to Bones

  if(this->ApplyDistractorToBones == vtkKWCheckButton::SafeDownCast(caller)
     && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {

    if(this->BoneRailModel && this->BonePlateModel &&
       this->BoneRailModel != this->BonePlateModel)
      {
      }
    else
      {
      this->ApplyDistractorToBones->SelectedStateOff();
      }
    }

  //----------------------------------------------------------------------
  // Place Fiducials Button (for registration)

  if(this->PlaceFiduButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->boolPlacingFiducials)
      {
      // Stop Placing fiducials
      this->PlaceFiduButton->SetReliefToGroove();

      this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetCurrentInteractionMode(vtkMRMLInteractionNode::ViewTransform);
      this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetSelected(0);
      this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetPlaceModePersistence(0);
      }
    else
      {
      // Start Placing fiducials
      this->PlaceFiduButton->SetReliefToSunken();
      if(!this->RegistrationFiducialList)
        {
        this->RegistrationFiducialList = vtkMRMLFiducialListNode::New();
        this->RegistrationFiducialList->SetGlyphTypeFromString("Sphere3D");
        this->RegistrationFiducialList->SetSymbolScale(2.0);
        this->RegistrationFiducialList->SetTextScale(0);

        this->GetGUI()->GetMRMLScene()->AddNode(this->RegistrationFiducialList);
        }

      if(this->RegistrationFiducialList)
        {
        this->GetGUI()->GetApplicationLogic()->GetSelectionNode()->SetActiveFiducialListID(this->RegistrationFiducialList->GetID());
        }

      this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetCurrentInteractionMode(vtkMRMLInteractionNode::Place);
      this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetSelected(1);
      this->GetGUI()->GetApplicationLogic()->GetInteractionNode()->SetPlaceModePersistence(1);
      }
    this->boolPlacingFiducials = !this->boolPlacingFiducials;
    }

  //----------------------------------------------------------------------
  // Register Button (to register Distractor and Bones)

  if(this->RegisterButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
    // Check number of points if pair on fiducial list
    if(this->RegistrationFiducialList && (this->RegistrationFiducialList->GetNumberOfFiducials()%2 == 0))
      {
      // Create Registration
      this->DistToBones = vtkIGTPat2ImgRegistration::New();
      int nPoints = this->RegistrationFiducialList->GetNumberOfFiducials();
      this->DistToBones->SetNumberOfPoints(nPoints/2);
      for(int i=0;i<(nPoints/2);i++)
        {
        float* source = this->RegistrationFiducialList->GetNthFiducialXYZ(i);
        float* target = this->RegistrationFiducialList->GetNthFiducialXYZ(i + nPoints/2);

        this->DistToBones->AddPoint(i,
                                    target[0], target[1], target[2],
                                    source[0], source[1], source[2]);
        }

      this->DistToBones->DoRegistration();

      //**************************************** TEST ****************************************
      // Matrix shouldn't be applied to models
      this->RegistrationFiducialList->SetAllFiducialsVisibility(0);

      vtkMatrix4x4* RegistrationMatrix = this->DistToBones->GetLandmarkTransformMatrix();
      this->GetDistractorSelected()->GetDistractorRail()->ApplyTransform(RegistrationMatrix);
      this->GetDistractorSelected()->GetDistractorSlider()->ApplyTransform(RegistrationMatrix);
      this->GetDistractorSelected()->GetDistractorPiston()->ApplyTransform(RegistrationMatrix);
      this->GetDistractorSelected()->GetDistractorCylinder()->ApplyTransform(RegistrationMatrix);

      this->GetDistractorSelected()->UpdateAnchors(RegistrationMatrix);
      //**************************************************************************************

      //this->DistToBones->Delete();
      //this->DistToBones = NULL;
      this->RegistrationFiducialList->RemoveAllFiducials();
      }
    }

  //----------------------------------------------------------------------
  // Distractor Selector

  if(this->DistractorMenuSelector->GetWidget()->GetMenu() == vtkKWMenu::SafeDownCast(caller)
     && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    vtkCollection* dList = this->GetDistractorList();
    for(int i=0; i<dList->GetNumberOfItems();i++)
      {
      vtkDistractorDefinition* DistractorDef = vtkDistractorDefinition::SafeDownCast(dList->GetItemAsObject(i));
      if(!strcmp(DistractorDef->GetDistractorName(),this->DistractorMenuSelector->GetWidget()->GetValue()))
        {
        this->GetDistractorSelected()->Hide();
        this->SetDistractorSelected(DistractorDef);
        this->GetDistractorSelected()->Show();
        }
      }
    }


}

//-----------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::ProcessMRMLEvents(vtkObject* caller,
                                                   unsigned long event,
                                                   void* vtkNotUsed(callData))
{
}
//-----------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::AddGUIObservers()
{
  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // MRML

  //// TODO: ADD MRML EVENT IF FIDUCIAL LIST DELETED -> FIDUCIL LIST = NULL

  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);

  if (this->GetGUI()->GetMRMLScene() != NULL)
    {
    this->GetGUI()->SetAndObserveMRMLSceneEvents(this->GetGUI()->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

  this->MovingScale
    ->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand);

  this->LoadDistractorButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->BonePlateModelSelector
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->BoneRailModelSelector
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->ApplyDistractorToBones
    ->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);


  //--------------------

  this->PlaceFiduButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->RegisterButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->DistractorMenuSelector->GetWidget()->GetMenu()
    ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);

}

//-----------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::RemoveGUIObservers()
{
  if (this->MovingScale)
    {
    this->MovingScale
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->LoadDistractorButton)
    {
    this->LoadDistractorButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->BonePlateModelSelector)
    {
    this->BonePlateModelSelector
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->BoneRailModelSelector)
    {
    this->BoneRailModelSelector
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->ApplyDistractorToBones)
    {
    this->ApplyDistractorToBones
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }


  //--------------------

  if(this->PlaceFiduButton)
    {
    this->PlaceFiduButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->RegisterButton)
    {
    this->RegisterButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if(this->DistractorMenuSelector)
    {
    this->DistractorMenuSelector->GetWidget()->GetMenu()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
}

//--------------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::UpdateGUI()
{
}

//----------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::HideUserInterface()
{
  TearDownGUI(); // HideUserInterface deletes the reference to the scene, so TearDownGUI shall be done before calling HideUserInterface
  Superclass::HideUserInterface();
}

//----------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::TearDownGUI()
{
  RemoveGUIObservers();
}

//---------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::MoveDistractor(double value,
                                                vtkMRMLModelNode* Slider, vtkMRMLLinearTransformNode* SliderTransformationNode,
                                                vtkMRMLModelNode* Piston, vtkMRMLLinearTransformNode* PistonTransformationNode,
                                                vtkMRMLModelNode* Cylinder, vtkMRMLLinearTransformNode* CylinderTransformationNode,
                                                vtkMatrix4x4* mat)
{

  this->MoveSlider(value,Slider,SliderTransformationNode, mat);

  this->MovePiston(value,Piston,PistonTransformationNode, mat);

  this->MoveCylinder(value,Cylinder,CylinderTransformationNode, mat);

}


//---------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::MoveSlider(double value, vtkMRMLModelNode* Slider, vtkMRMLLinearTransformNode* SliderTransformationNode, vtkMatrix4x4* mat)
{
  double SliderCenter[3];
  Slider->GetPolyData()->GetCenter(SliderCenter);

  vtkMatrix4x4* SliderTransformationMatrix = vtkMatrix4x4::New();
  SliderTransformationMatrix->Identity();


  vtkTransform* SliderTranslation = vtkTransform::New();
  SliderTranslation->Translate(-this->GetDistractorSelected()->GetRailAnchor()[0],
                               -this->GetDistractorSelected()->GetRailAnchor()[1],
                               -this->GetDistractorSelected()->GetRailAnchor()[2]);

  vtkTransform* SliderRotation = vtkTransform::New();

  if(mat == NULL)
    {
    SliderRotation->RotateY(value);
    }
  else
    {
    SliderRotation->RotateWXYZ(value, mat->GetElement(0,1), mat->GetElement(1,1), mat->GetElement(2,1));
    }

  vtkTransform* SliderInvertTranslation = vtkTransform::New();
  SliderInvertTranslation->Translate(this->GetDistractorSelected()->GetRailAnchor()[0],
                                     this->GetDistractorSelected()->GetRailAnchor()[1],
                                     this->GetDistractorSelected()->GetRailAnchor()[2]);

  SliderInvertTranslation->PreMultiply();
  SliderInvertTranslation->Concatenate(SliderRotation);
  SliderInvertTranslation->PreMultiply();
  SliderInvertTranslation->Concatenate(SliderTranslation);
  SliderInvertTranslation->GetMatrix(SliderTransformationMatrix);

  SliderTranslation->Delete();
  SliderRotation->Delete();
  SliderInvertTranslation->Delete();

  SliderTransformationNode->SetAndObserveMatrixTransformToParent(SliderTransformationMatrix);

  // Drop transformation node under other transforms (if exists)
  if(Slider->GetParentTransformNode() && Slider->GetParentTransformNode()!=SliderTransformationNode)
    {
    SliderTransformationNode->SetAndObserveTransformNodeID(Slider->GetParentTransformNode()->GetID());
    }


  double newSliderAnchorX =
    SliderTransformationMatrix->GetElement(0,0)*this->GetDistractorSelected()->GetSliderAnchor()[0]+
    SliderTransformationMatrix->GetElement(0,1)*this->GetDistractorSelected()->GetSliderAnchor()[1]+
    SliderTransformationMatrix->GetElement(0,2)*this->GetDistractorSelected()->GetSliderAnchor()[2]+
    SliderTransformationMatrix->GetElement(0,3)*1;

  double newSliderAnchorY =
    SliderTransformationMatrix->GetElement(1,0)*this->GetDistractorSelected()->GetSliderAnchor()[0]+
    SliderTransformationMatrix->GetElement(1,1)*this->GetDistractorSelected()->GetSliderAnchor()[1]+
    SliderTransformationMatrix->GetElement(1,2)*this->GetDistractorSelected()->GetSliderAnchor()[2]+
    SliderTransformationMatrix->GetElement(1,3)*1;

  double newSliderAnchorZ =
    SliderTransformationMatrix->GetElement(2,0)*this->GetDistractorSelected()->GetSliderAnchor()[0]+
    SliderTransformationMatrix->GetElement(2,1)*this->GetDistractorSelected()->GetSliderAnchor()[1]+
    SliderTransformationMatrix->GetElement(2,2)*this->GetDistractorSelected()->GetSliderAnchor()[2]+
    SliderTransformationMatrix->GetElement(2,3)*1;

  double newSliderAnchor[3] = {newSliderAnchorX, newSliderAnchorY, newSliderAnchorZ};

  this->GetDistractorSelected()->SetNewSliderAnchor(newSliderAnchor);

  // Delete
  SliderTransformationMatrix->Delete();
}


//---------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::MovePiston(double value, vtkMRMLModelNode* Piston, vtkMRMLLinearTransformNode* PistonTransformationNode, vtkMatrix4x4* mat)
{
  double PistonAnchor[3] = {this->GetDistractorSelected()->GetPistonAnchor()[0],
                            this->GetDistractorSelected()->GetPistonAnchor()[1],
                            this->GetDistractorSelected()->GetPistonAnchor()[2]};

  vtkTransform* PistonTranslation = vtkTransform::New();
  PistonTranslation->Translate(PistonAnchor[0],PistonAnchor[1],PistonAnchor[2]);

  double* nSliderAnchor = this->GetDistractorSelected()->GetNewSliderAnchor();

  double gamma = atan2((nSliderAnchor[2]-PistonAnchor[2]),
                       (nSliderAnchor[0]-PistonAnchor[0]));

  double beta = atan2((this->GetDistractorSelected()->GetSliderAnchor()[2]-PistonAnchor[2]),
                      (this->GetDistractorSelected()->GetSliderAnchor()[0]-PistonAnchor[0]));

  this->GetDistractorSelected()->SetPistonRotationAngle_deg((beta-gamma)*180/M_PI);

  vtkTransform* PistonRotation = vtkTransform::New();

  if(mat == NULL)
    {
    PistonRotation->RotateY(this->GetDistractorSelected()->GetPistonRotationAngle_deg());
    }
  else
    {
    PistonRotation->RotateWXYZ(this->GetDistractorSelected()->GetPistonRotationAngle_deg(),mat->GetElement(0,1), mat->GetElement(1,1), mat->GetElement(2,1));
    }

  vtkMatrix4x4* PistonTransformationMatrix = vtkMatrix4x4::New();
  PistonTransformationMatrix->Identity();

  vtkTransform* PistonInvertTranslation = vtkTransform::New();
  PistonInvertTranslation->Translate(-PistonAnchor[0],-PistonAnchor[1],-PistonAnchor[2]);
  PistonInvertTranslation->PostMultiply();
  PistonInvertTranslation->Concatenate(PistonRotation);
  PistonInvertTranslation->PostMultiply();
  PistonInvertTranslation->Concatenate(PistonTranslation);
  PistonInvertTranslation->GetMatrix(PistonTransformationMatrix);

  PistonTransformationNode->SetAndObserveMatrixTransformToParent(PistonTransformationMatrix);

  // Drop transformation node under other transforms (if exists)
  if(Piston->GetParentTransformNode() && Piston->GetParentTransformNode()!=PistonTransformationNode)
    {
    PistonTransformationNode->SetAndObserveTransformNodeID(Piston->GetParentTransformNode()->GetID());
    }

  // Delete
  PistonTranslation->Delete();
  PistonRotation->Delete();
  PistonInvertTranslation->Delete();
  PistonTransformationMatrix->Delete();
}



//---------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::MoveCylinder(double value, vtkMRMLModelNode* Cylinder, vtkMRMLLinearTransformNode* CylinderTransformationNode, vtkMatrix4x4* mat)
{
  double tx = this->GetDistractorSelected()->GetNewSliderAnchor()[0] - this->GetDistractorSelected()->GetCylinderAnchor()[0];
  double ty = 0.0;
  double tz = this->GetDistractorSelected()->GetNewSliderAnchor()[2] - this->GetDistractorSelected()->GetCylinderAnchor()[2];

  vtkTransform* CylinderInvertTranslation = vtkTransform::New();
  CylinderInvertTranslation->Translate(this->GetDistractorSelected()->GetNewSliderAnchor()[0],
                                       this->GetDistractorSelected()->GetNewSliderAnchor()[1],//this->GetDistractorSelected()->GetCylinderAnchor()[1],
                                       this->GetDistractorSelected()->GetNewSliderAnchor()[2]);

  vtkTransform* CylinderTranslation = vtkTransform::New();
  CylinderTranslation->Translate(-this->GetDistractorSelected()->GetNewSliderAnchor()[0],
                                 -this->GetDistractorSelected()->GetNewSliderAnchor()[1],//-this->GetDistractorSelected()->GetCylinderAnchor()[1],
                                 -this->GetDistractorSelected()->GetNewSliderAnchor()[2]);

  vtkTransform* CylinderRotation = vtkTransform::New();

  if(mat == NULL)
    {
    CylinderRotation->RotateY(this->GetDistractorSelected()->GetPistonRotationAngle_deg());
    }
  else
    {
    CylinderRotation->RotateWXYZ(this->GetDistractorSelected()->GetPistonRotationAngle_deg(), mat->GetElement(0,1), mat->GetElement(1,1), mat->GetElement(2,1));
    }

  vtkMatrix4x4* CylinderTransformationMatrix = vtkMatrix4x4::New();
  CylinderTransformationMatrix->Identity();

  vtkTransform* CylinderTransformation = vtkTransform::New();
  CylinderTransformation->Translate(tx,ty,tz);
  CylinderTransformation->PreMultiply();
  CylinderTransformation->Concatenate(CylinderTranslation);
  CylinderTransformation->PostMultiply();
  CylinderTransformation->Concatenate(CylinderRotation);
  CylinderTransformation->PostMultiply();
  CylinderTransformation->Concatenate(CylinderInvertTranslation);
  CylinderTransformation->GetMatrix(CylinderTransformationMatrix);

  CylinderTransformationNode->SetAndObserveMatrixTransformToParent(CylinderTransformationMatrix);

  // Drop transformation node under other transforms (if exists)
  if(Cylinder->GetParentTransformNode() && Cylinder->GetParentTransformNode()!=CylinderTransformationNode)
    {
    CylinderTransformationNode->SetAndObserveTransformNodeID(Cylinder->GetParentTransformNode()->GetID());
    }

  // Delete
  CylinderInvertTranslation->Delete();
  CylinderTranslation->Delete();
  CylinderRotation->Delete();
  CylinderTransformationMatrix->Delete();
  CylinderTransformation->Delete();
}


//---------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::OpenDistractorFile(const char* xmlFile)
{
  this->DistractorObject = vtkDistractorDefinition::New();

  // Test Read XML
  std::filebuf *fbuf;
  long size;
  char *buffer;

  std::stringstream DistractorPath;
  DistractorPath << xmlFile;
  this->xmlPath = DistractorPath.str();
  DistractorPath << "/Distractor.xml";

  // Avoid compiler warning about last argument of XML_Parse is NULL
  int dummy = 0;

  std::ifstream file_in(DistractorPath.str().c_str(), ios::in);

  XML_Parser parser = XML_ParserCreate(NULL);
  XML_SetUserData(parser, this);
  XML_SetElementHandler(parser, startElement, endElement);
  fbuf = file_in.rdbuf();
  size = fbuf->pubseekoff (0,ios::end,ios::in);
  fbuf->pubseekpos (0,ios::in);
  buffer = new char[size];
  fbuf->sgetn (buffer, size);
  XML_Parse(parser, buffer, size, dummy);

  file_in.close();
  XML_ParserFree(parser);
  free(buffer);


  this->GetDistractorList()->AddItem(this->DistractorObject);

  if(this->GetDistractorSelected())
    {
    this->GetDistractorSelected()->Hide();
    }

  this->SetDistractorSelected(this->DistractorObject);
  this->GetDistractorSelected()->Show();
  this->DistractorObject->Delete();
}

//---------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::startElement(void *userData, const XML_Char *name, const XML_Char **atts) {
  int i;
  vtkOsteoPlanDistractorStep* LoadClass = (vtkOsteoPlanDistractorStep*)userData;

  for (i=0; atts[i] ; i+=2)
    {

    // Distractor Name
    if(!strcmp(name,"Distractor"))
      {
      if(!strcmp(atts[i],"name"))
        {
        std::stringstream DName;
        DName << atts[i+1];

        LoadClass->DistractorObject->SetDistractorName(DName.str().c_str());
        LoadClass->DistName = DName.str();
        }
      }

    // VTK Files
    if(!strcmp(atts[i],"VTKFile"))
      {
      std::stringstream pathFile;
      pathFile << LoadClass->xmlPath << "/" << atts[i+1];

      if(!strcmp(name,"Rail"))
        {
        LoadClass->RailModelPath = pathFile.str();
        }
      else if(!strcmp(name,"Slider"))
        {
        LoadClass->SliderModelPath = pathFile.str();
        }
      else if(!strcmp(name,"Piston"))
        {
        LoadClass->PistonModelPath = pathFile.str();
        }
      else if(!strcmp(name,"Cylinder"))
        {
        LoadClass->CylinderModelPath = pathFile.str();
        }
      }

    // Anchors
    if(!strcmp(atts[i],"Anchor"))
      {
      std::stringstream anchor;
      anchor << atts[i+1];
      std::string buf;
      std::vector<std::string> tokens;

      while(anchor >> buf)
        {
        tokens.push_back(buf);
        }

      if(!strcmp(name,"Rail"))
        {
        double rAnchor[3] = {atof(tokens[0].c_str()), atof(tokens[1].c_str()), atof(tokens[2].c_str())};
        LoadClass->DistractorObject->SetRailAnchor(rAnchor);
        }
      else if(!strcmp(name,"Slider"))
        {
        double sAnchor[3] = {atof(tokens[0].c_str()), atof(tokens[1].c_str()), atof(tokens[2].c_str())};
        LoadClass->DistractorObject->SetSliderAnchor(sAnchor);
        }
      else if(!strcmp(name,"Piston"))
        {
        double pAnchor[3] = {atof(tokens[0].c_str()), atof(tokens[1].c_str()), atof(tokens[2].c_str())};
        LoadClass->DistractorObject->SetPistonAnchor(pAnchor);
        }
      else if(!strcmp(name,"Cylinder"))
        {
        double cAnchor[3] = {atof(tokens[0].c_str()), atof(tokens[1].c_str()), atof(tokens[2].c_str())};
        LoadClass->DistractorObject->SetCylinderAnchor(cAnchor);
        }
      }

    // Range of motion
    if(!strcmp(name,"Range"))
      {
      if(!strcmp(atts[i],"min"))
        {
        LoadClass->DistractorObject->SetRangeMin(atof(atts[i+1]));
        }
      else if(!strcmp(atts[i],"max"))
        {
        LoadClass->DistractorObject->SetRangeMax(atof(atts[i+1]));
        }
      }
    }

}


//---------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::endElement(void *userData, const XML_Char *name) {
}


//---------------------------------------------------------------------------
void vtkOsteoPlanDistractorStep::MoveBones(vtkMRMLModelNode* BonePlateModel, vtkMRMLLinearTransformNode* BonePlateTransform,
                                           double value)
{
  // Create BoneAnchor and newBoneAnchor on Distractor1 structure
  // Calculate newBoneAnchor = R*BoneAnchor
  // Translate Rotation Center of the bone (BoneAnchor)
  // Rotate Bone as Slider
  // Translate Rotation Center of the bone back to new position (newBoneAnchor)
  // Update BoneAnchor

  double BoneCenter[3];
  BonePlateModel->GetPolyData()->GetCenter(BoneCenter);

  vtkMatrix4x4* BoneTransformationMatrix = BonePlateTransform->GetMatrixTransformToParent();

  double BoneAnchor[3] = {BoneCenter[1],//+BoneTransformationMatrix->GetElement(0,3),
                          BoneCenter[2],//+BoneTransformationMatrix->GetElement(1,3),
                          BoneCenter[3]};//+BoneTransformationMatrix->GetElement(2,3)};

  vtkTransform* BoneTranslation = vtkTransform::New();
  BoneTranslation->Translate(BoneAnchor[0] + this->GetDistractorSelected()->GetRailAnchor()[0],
                             BoneAnchor[1] + this->GetDistractorSelected()->GetRailAnchor()[1],
                             BoneAnchor[2] + this->GetDistractorSelected()->GetRailAnchor()[2]);

  vtkTransform* BoneRotation = vtkTransform::New();
  BoneRotation->RotateY(value);


  double newBoneAnchorX =
    BoneRotation->GetMatrix()->GetElement(0,0)*BoneAnchor[0]+
    BoneRotation->GetMatrix()->GetElement(0,1)*BoneAnchor[1]+
    BoneRotation->GetMatrix()->GetElement(0,2)*BoneAnchor[2]+
    BoneRotation->GetMatrix()->GetElement(0,3)*1;

  double newBoneAnchorY =
    BoneRotation->GetMatrix()->GetElement(1,0)*BoneAnchor[0]+
    BoneRotation->GetMatrix()->GetElement(1,1)*BoneAnchor[1]+
    BoneRotation->GetMatrix()->GetElement(1,2)*BoneAnchor[2]+
    BoneRotation->GetMatrix()->GetElement(1,3)*1;

  double newBoneAnchorZ =
    BoneRotation->GetMatrix()->GetElement(2,0)*BoneAnchor[0]+
    BoneRotation->GetMatrix()->GetElement(2,1)*BoneAnchor[1]+
    BoneRotation->GetMatrix()->GetElement(2,2)*BoneAnchor[2]+
    BoneRotation->GetMatrix()->GetElement(2,3)*1;


  vtkTransform* BoneInvertTranslation = vtkTransform::New();
  BoneInvertTranslation->Translate(-newBoneAnchorX - this->GetDistractorSelected()->GetRailAnchor()[0],
                                   -newBoneAnchorY - this->GetDistractorSelected()->GetRailAnchor()[1],
                                   -newBoneAnchorZ - this->GetDistractorSelected()->GetRailAnchor()[2]);

  BoneInvertTranslation->PostMultiply();
  BoneInvertTranslation->Concatenate(BoneRotation);
  BoneInvertTranslation->PostMultiply();
  BoneInvertTranslation->Concatenate(BoneTranslation);
  BoneInvertTranslation->GetMatrix(BoneTransformationMatrix);

  BoneTranslation->Delete();
  BoneRotation->Delete();
  BoneInvertTranslation->Delete();

}

