/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkDistractorModelingGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"

#include "vtkKWPushButton.h"

#include "vtkCornerAnnotation.h"

#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLInteractionNode.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkDistractorModelingGUI );
vtkCxxRevisionMacro ( vtkDistractorModelingGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkDistractorModelingGUI::vtkDistractorModelingGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkDistractorModelingGUI::DataCallback);

  //----------------------------------------------------------------
  // GUI widgets
  this->SliderTransformNode   = NULL;
  this->PistonTransformNode   = NULL;
  this->CylinderTransformNode = NULL;

  this->MovingScale = NULL;

  this->LoadDistractorButton = NULL;
  this->DistractorSelector   = NULL;

  this->BonePlateModelSelector = NULL;
  this->BoneRailModelSelector  = NULL;
  this->BonePlateModel         = NULL;
  this->BoneRailModel          = NULL;
  this->BonePlateTransform     = NULL;

  this->ApplyDistractorToBones = NULL;


  //--------------------

  this->PlaceFiduButton = NULL;
  this->RegisterButton  = NULL;

  this->boolPlacingFiducials     = false;
  this->RegistrationFiducialList = NULL;
  this->DistractorMenuSelector   = NULL;

  this->DistToBones = NULL;


  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkDistractorModelingGUI::~vtkDistractorModelingGUI ( )
{

  //----------------------------------------------------------------
  // Remove Callbacks

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  //----------------------------------------------------------------
  // Remove Observers

  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // Remove GUI widgets

  if(this->MovingScale)
    {
    this->MovingScale->SetParent(NULL);
    this->MovingScale->Delete();
    this->MovingScale = NULL;
    }

  if(this->LoadDistractorButton)
    {
    this->LoadDistractorButton->SetParent(NULL);
    this->LoadDistractorButton->Delete();
    this->LoadDistractorButton = NULL;
    }

  if(this->DistractorSelector)
    {
    this->DistractorSelector->SetParent(NULL);
    this->DistractorSelector->Delete();
    this->DistractorSelector = NULL;
    }

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

  if(this->BonePlateModelSelector)
    {
    this->BonePlateModelSelector->SetParent(NULL);
    this->BonePlateModelSelector->Delete();
    this->BonePlateModelSelector = NULL;
    }

  if(this->BoneRailModelSelector)
    {
    this->BoneRailModelSelector->SetParent(NULL);
    this->BoneRailModelSelector->Delete();
    this->BoneRailModelSelector = NULL;
    }

  if(this->ApplyDistractorToBones)
    {
    this->ApplyDistractorToBones->SetParent(NULL);
    this->ApplyDistractorToBones->Delete();
    this->ApplyDistractorToBones = NULL;
    }


  //--------------------

  if(this->PlaceFiduButton)
    {
    this->PlaceFiduButton->SetParent(NULL);
    this->PlaceFiduButton->Delete();
    this->PlaceFiduButton = NULL;
    }

  if(this->RegisterButton)
    {
    this->RegisterButton->SetParent(NULL);
    this->RegisterButton->Delete();
    this->RegisterButton = NULL;
    }

  if(this->RegistrationFiducialList)
    {
    this->RegistrationFiducialList->Delete();
    this->RegistrationFiducialList = NULL;
    }

  if(this->DistToBones)
    {
    this->DistToBones->Delete();
    this->DistToBones = NULL;
    }

  if(this->DistractorMenuSelector)
    {
    this->DistractorMenuSelector->SetParent(NULL);
    this->DistractorMenuSelector->Delete();
    this->DistractorMenuSelector = NULL;
    }


  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::Enter()
{
  // Fill in
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }

}


//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "DistractorModelingGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::RemoveGUIObservers ( )
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


//// TODO: ADD MRML EVENT IF FIDUCIAL LIST DELETED -> FIDUCIL LIST = NULL


  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);

  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
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

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkDistractorModelingLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::ProcessGUIEvents(vtkObject *caller,
                                                unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  if(this->MovingScale == vtkKWScale::SafeDownCast(caller)
     && event == vtkKWScale::ScaleValueChangingEvent)
    {
    if(this->GetLogic()->GetDistractorSelected()->GetDistractorSlider() &&
       this->GetLogic()->GetDistractorSelected()->GetDistractorPiston() &&
       this->GetLogic()->GetDistractorSelected()->GetDistractorCylinder())
      {
      if(!this->SliderTransformNode)
        {
        this->SliderTransformNode = vtkMRMLLinearTransformNode::New();
        this->GetMRMLScene()->AddNode(this->SliderTransformNode);
        }
      if(!this->PistonTransformNode)
        {
        this->PistonTransformNode = vtkMRMLLinearTransformNode::New();
        this->GetMRMLScene()->AddNode(this->PistonTransformNode);
        }
      if(!this->CylinderTransformNode)
        {
        this->CylinderTransformNode = vtkMRMLLinearTransformNode::New();
        this->GetMRMLScene()->AddNode(this->CylinderTransformNode);
        }

      this->GetLogic()->MoveDistractor(this->MovingScale->GetValue(),
                                       this->GetLogic()->GetDistractorSelected()->GetDistractorSlider(), this->SliderTransformNode,
                                       this->GetLogic()->GetDistractorSelected()->GetDistractorPiston(), this->PistonTransformNode,
                                       this->GetLogic()->GetDistractorSelected()->GetDistractorCylinder(), this->CylinderTransformNode);

      this->GetLogic()->GetDistractorSelected()->GetDistractorSlider()->SetAndObserveTransformNodeID(this->SliderTransformNode->GetID());
      this->GetLogic()->GetDistractorSelected()->GetDistractorSlider()->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);

      this->GetLogic()->GetDistractorSelected()->GetDistractorPiston()->SetAndObserveTransformNodeID(this->PistonTransformNode->GetID());
      this->GetLogic()->GetDistractorSelected()->GetDistractorPiston()->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);

      this->GetLogic()->GetDistractorSelected()->GetDistractorCylinder()->SetAndObserveTransformNodeID(this->CylinderTransformNode->GetID());
      this->GetLogic()->GetDistractorSelected()->GetDistractorCylinder()->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);

      this->GetMRMLScene()->InvokeEvent(vtkMRMLScene::SceneEditedEvent);
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();

      }

    if(this->ApplyDistractorToBones->GetSelectedState())
      {
      if(this->BonePlateModel)
        {
        if(!this->BonePlateTransform)
          {
          this->BonePlateTransform = vtkMRMLLinearTransformNode::New();
          this->GetMRMLScene()->AddNode(this->BonePlateTransform);
          this->BonePlateModel->SetAndObserveTransformNodeID(this->BonePlateTransform->GetID());
          this->BonePlateModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);
          }
        if(this->BonePlateTransform)
          {
          this->GetLogic()->MoveBones(this->BonePlateModel, this->BonePlateTransform, this->MovingScale->GetValue());
          }
        }
      }
    }

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
        this->GetLogic()->OpenDistractorFile(this->DistractorSelector->GetFileName());

        vtkSlicerModelsLogic* ModelsLogic = vtkSlicerModelsLogic::New();
        ModelsLogic->SetMRMLScene(this->GetMRMLScene());

        if(strcmp(this->GetLogic()->DistName.c_str(),"")   &&
           strcmp(this->GetLogic()->RailModelPath.c_str(),"")    &&
           strcmp(this->GetLogic()->SliderModelPath.c_str(),"")  &&
           strcmp(this->GetLogic()->PistonModelPath.c_str(), "") &&
           strcmp(this->GetLogic()->CylinderModelPath.c_str(),""))
          {
          if(this->GetLogic()->GetDistractorSelected())
            {
            this->GetLogic()->GetDistractorSelected()->SetDistractorRail(ModelsLogic->AddModel(this->GetLogic()->RailModelPath.c_str()));
            this->GetLogic()->GetDistractorSelected()->SetDistractorSlider(ModelsLogic->AddModel(this->GetLogic()->SliderModelPath.c_str()));
            this->GetLogic()->GetDistractorSelected()->SetDistractorPiston(ModelsLogic->AddModel(this->GetLogic()->PistonModelPath.c_str()));
            this->GetLogic()->GetDistractorSelected()->SetDistractorCylinder(ModelsLogic->AddModel(this->GetLogic()->CylinderModelPath.c_str()));
            }


          if(this->DistractorMenuSelector)
            {
            this->DistractorMenuSelector->GetWidget()->GetMenu()->AddRadioButton(this->GetLogic()->GetDistractorSelected()->GetDistractorName());
            this->DistractorMenuSelector->GetWidget()->SetValue(this->GetLogic()->GetDistractorSelected()->GetDistractorName());
            }

          this->MovingScale->SetRange(this->GetLogic()->GetDistractorSelected()->GetRange());
          this->MovingScale->SetValue(0.0);

          ModelsLogic->Delete();
          }
        }
      }

    this->DistractorSelector->Delete();
    this->DistractorSelector = NULL;
    }

  if(this->BonePlateModelSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    if(this->BonePlateModelSelector->GetSelected())
      {
      this->BonePlateModel = vtkMRMLModelNode::SafeDownCast(this->BonePlateModelSelector->GetSelected());
      }
    }

  if(this->BoneRailModelSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    if(this->BoneRailModelSelector->GetSelected())
      {
      this->BoneRailModel = vtkMRMLModelNode::SafeDownCast(this->BoneRailModelSelector->GetSelected());
      }
    }

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


  if(this->PlaceFiduButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
    if(this->boolPlacingFiducials)
      {
      // Stop Placing fiducials
      this->PlaceFiduButton->SetReliefToGroove();

      this->GetApplicationLogic()->GetInteractionNode()->SetCurrentInteractionMode(vtkMRMLInteractionNode::ViewTransform);
      this->GetApplicationLogic()->GetInteractionNode()->SetSelected(0);
      this->GetApplicationLogic()->GetInteractionNode()->SetPlaceModePersistence(0);
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

        this->GetMRMLScene()->AddNode(this->RegistrationFiducialList);
        }

      if(this->RegistrationFiducialList)
        {
        this->GetApplicationLogic()->GetSelectionNode()->SetActiveFiducialListID(this->RegistrationFiducialList->GetID());
        }

      this->GetApplicationLogic()->GetInteractionNode()->SetCurrentInteractionMode(vtkMRMLInteractionNode::Place);
      this->GetApplicationLogic()->GetInteractionNode()->SetSelected(1);
      this->GetApplicationLogic()->GetInteractionNode()->SetPlaceModePersistence(1);
      }
    this->boolPlacingFiducials = !this->boolPlacingFiducials;
    }

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


      // Test: Matrix shouldn't be applied to models
      this->RegistrationFiducialList->SetAllFiducialsVisibility(0);

      vtkMatrix4x4* RegistrationMatrix = this->DistToBones->GetLandmarkTransformMatrix();
      this->GetLogic()->GetDistractorSelected()->GetDistractorRail()->ApplyTransform(RegistrationMatrix);
      this->GetLogic()->GetDistractorSelected()->GetDistractorSlider()->ApplyTransform(RegistrationMatrix);
      this->GetLogic()->GetDistractorSelected()->GetDistractorPiston()->ApplyTransform(RegistrationMatrix);
      this->GetLogic()->GetDistractorSelected()->GetDistractorCylinder()->ApplyTransform(RegistrationMatrix);

      }
    }

  if(this->DistractorMenuSelector->GetWidget()->GetMenu() == vtkKWMenu::SafeDownCast(caller)
     && event == vtkKWMenu::MenuItemInvokedEvent)
    {
    vtkCollection* dList = this->GetLogic()->GetDistractorList();

    for(int i=0; i<dList->GetNumberOfItems();i++)
      {
      vtkDistractorDefinition* DistractorDef = vtkDistractorDefinition::SafeDownCast(dList->GetItemAsObject(i));

      if(!strcmp(DistractorDef->GetDistractorName(),this->DistractorMenuSelector->GetWidget()->GetValue()))
        {
        this->GetLogic()->GetDistractorSelected()->Hide();
        this->GetLogic()->SetDistractorSelected(DistractorDef);
        this->GetLogic()->GetDistractorSelected()->Show();
        }

      }

    }
}


void vtkDistractorModelingGUI::DataCallback(vtkObject *caller,
                                            unsigned long eid, void *clientData, void *callData)
{
  vtkDistractorModelingGUI *self = reinterpret_cast<vtkDistractorModelingGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkDistractorModelingGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::ProcessLogicEvents ( vtkObject *caller,
                                                    unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkDistractorModelingLogic::SafeDownCast(caller))
    {
    if (event == vtkDistractorModelingLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::ProcessMRMLEvents ( vtkObject *caller,
                                                   unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(),
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");
    }
}


//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME
  // create a page
  this->UIPanel->AddPage ( "DistractorModeling", "DistractorModeling", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForTestFrame1();

}


void vtkDistractorModelingGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help =
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:DistractorModeling</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "DistractorModeling" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkDistractorModelingGUI::BuildGUIForTestFrame1()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("DistractorModeling");

  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Distractor");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Test child frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Distractor frame");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  // -----------------------------------------

  this->MovingScale = vtkKWScale::New();
  this->MovingScale->SetParent(frame->GetFrame());
  this->MovingScale->Create();
  this->MovingScale->SetRange(-180,180);

  this->LoadDistractorButton = vtkKWPushButton::New();
  this->LoadDistractorButton->SetParent(frame->GetFrame());
  this->LoadDistractorButton->Create();
  this->LoadDistractorButton->SetText("Load Distractor");

  vtkKWLabel* PlateModelLabel = vtkKWLabel::New();
  PlateModelLabel->SetParent(frame->GetFrame());
  PlateModelLabel->Create();
  PlateModelLabel->SetText("Plate Bone:");
  PlateModelLabel->SetAnchorToWest();

  this->BonePlateModelSelector = vtkSlicerNodeSelectorWidget::New();
  this->BonePlateModelSelector->SetParent(frame->GetFrame());
  this->BonePlateModelSelector->Create();
  this->BonePlateModelSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
  this->BonePlateModelSelector->SetNewNodeEnabled(false);
  this->BonePlateModelSelector->SetMRMLScene(this->GetMRMLScene());
  this->BonePlateModelSelector->UpdateMenu();

  vtkKWLabel* RailModelLabel = vtkKWLabel::New();
  RailModelLabel->SetParent(frame->GetFrame());
  RailModelLabel->Create();
  RailModelLabel->SetText("Rail Bone:");
  RailModelLabel->SetAnchorToWest();

  this->BoneRailModelSelector = vtkSlicerNodeSelectorWidget::New();
  this->BoneRailModelSelector->SetParent(frame->GetFrame());
  this->BoneRailModelSelector->Create();
  this->BoneRailModelSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
  this->BoneRailModelSelector->SetNewNodeEnabled(false);
  this->BoneRailModelSelector->SetMRMLScene(this->GetMRMLScene());
  this->BoneRailModelSelector->UpdateMenu();

  this->ApplyDistractorToBones = vtkKWCheckButton::New();
  this->ApplyDistractorToBones->SetParent(frame->GetFrame());
  this->ApplyDistractorToBones->Create();
  this->ApplyDistractorToBones->SetText("Apply transformation to bones");
  this->ApplyDistractorToBones->SelectedStateOff();



  this->DistractorMenuSelector = vtkKWMenuButtonWithLabel::New();
  this->DistractorMenuSelector->SetParent(frame->GetFrame());
  this->DistractorMenuSelector->Create();
  this->DistractorMenuSelector->SetLabelText("Distractor:");
  this->DistractorMenuSelector->GetWidget()->GetMenu()->AddRadioButton("None");
  this->DistractorMenuSelector->GetWidget()->SetValue("None");




  vtkKWFrame* RegistrationFrame = vtkKWFrame::New();
  RegistrationFrame->SetParent(frame->GetFrame());
  RegistrationFrame->Create();


  this->PlaceFiduButton = vtkKWPushButton::New();
  this->PlaceFiduButton->SetParent(RegistrationFrame);
  this->PlaceFiduButton->Create();
  this->PlaceFiduButton->SetText("Place Fiducials");
  this->PlaceFiduButton->SetWidth(8);

  this->RegisterButton = vtkKWPushButton::New();
  this->RegisterButton->SetParent(RegistrationFrame);
  this->RegisterButton->Create();
  this->RegisterButton->SetText("Register");

  this->Script("pack %s %s -side left -fill x -expand y -padx 2 -pady 2",
               this->PlaceFiduButton->GetWidgetName(),
               this->RegisterButton->GetWidgetName());


  this->Script("pack %s %s %s %s -side top -fill x -expand y -padx 2 -pady 2",
               this->DistractorMenuSelector->GetWidgetName(),
               this->LoadDistractorButton->GetWidgetName(),
               this->MovingScale->GetWidgetName(),
               RegistrationFrame->GetWidgetName());


  RegistrationFrame->Delete();

  RailModelLabel->Delete();
  PlateModelLabel->Delete();
  conBrowsFrame->Delete();
  frame->Delete();

}

//----------------------------------------------------------------------------
void vtkDistractorModelingGUI::UpdateAll()
{
}

