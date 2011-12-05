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
  this->RailSelector     = NULL;
  this->SliderSelector   = NULL;
  this->PistonSelector   = NULL;
  this->CylinderSelector = NULL;

  this->RailModel     = NULL;
  this->SliderModel   = NULL;
  this->PistonModel   = NULL;
  this->CylinderModel = NULL;

  this->SliderTransformNode   = NULL;
  this->PistonTransformNode   = NULL;
  this->CylinderTransformNode = NULL;

  this->MovingScale         = NULL;

  this->LoadDistractorButton = NULL;

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

  if(this->RailSelector)
    {
    this->RailSelector->SetParent(NULL);
    this->RailSelector->Delete();
    this->RailSelector = NULL;
    }

  if(this->SliderSelector)
    {
    this->SliderSelector->SetParent(NULL);
    this->SliderSelector->Delete();
    this->SliderSelector = NULL;
    }

  if(this->PistonSelector)
    {
    this->PistonSelector->SetParent(NULL);
    this->PistonSelector->Delete();
    this->PistonSelector = NULL;
    }

  if(this->CylinderSelector)
    {
    this->CylinderSelector->SetParent(NULL);
    this->CylinderSelector->Delete();
    this->CylinderSelector = NULL;
    }


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
  if (this->RailSelector)
    {
    this->RailSelector
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->SliderSelector)
    {
    this->SliderSelector
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->PistonSelector)
    {
    this->PistonSelector
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->CylinderSelector)
    {
    this->CylinderSelector
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

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

  this->RailSelector
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->SliderSelector
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->PistonSelector
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->CylinderSelector
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->MovingScale
    ->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand);

  this->LoadDistractorButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

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

  if(this->RailSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    this->RailModel = vtkMRMLModelNode::SafeDownCast(this->RailSelector->GetSelected());
    }

  if(this->SliderSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    this->SliderModel = vtkMRMLModelNode::SafeDownCast(this->SliderSelector->GetSelected());
    }

  if(this->PistonSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    this->PistonModel = vtkMRMLModelNode::SafeDownCast(this->PistonSelector->GetSelected());
    }

  if(this->CylinderSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
     && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    this->CylinderModel = vtkMRMLModelNode::SafeDownCast(this->CylinderSelector->GetSelected());
    }


  if(this->MovingScale == vtkKWScale::SafeDownCast(caller)
     && event == vtkKWScale::ScaleValueChangingEvent)
    {
    if(this->SliderModel && this->PistonModel && this->CylinderModel)
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
                                       this->SliderModel, this->SliderTransformNode,
                                       this->PistonModel, this->PistonTransformNode,
                                       this->CylinderModel, this->CylinderTransformNode);

      this->SliderModel->SetAndObserveTransformNodeID(this->SliderTransformNode->GetID());
      this->SliderModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);

      this->PistonModel->SetAndObserveTransformNodeID(this->PistonTransformNode->GetID());
      this->PistonModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);

      this->CylinderModel->SetAndObserveTransformNodeID(this->CylinderTransformNode->GetID());
      this->CylinderModel->InvokeEvent(vtkMRMLTransformableNode::TransformModifiedEvent);

      this->GetMRMLScene()->InvokeEvent(vtkMRMLScene::SceneEditedEvent);
      this->GetApplicationGUI()->GetActiveViewerWidget()->Render();

      }
    }

  if(this->LoadDistractorButton == vtkKWPushButton::SafeDownCast(caller)
     && event == vtkKWPushButton::InvokedEvent)
    {
    this->GetLogic()->OpenDistractorFile();

    vtkSlicerModelsLogic* ModelsLogic = vtkSlicerModelsLogic::New();
    ModelsLogic->SetMRMLScene(this->GetMRMLScene());

    if(strcmp(this->GetLogic()->RailModelPath.c_str(),"") &&
       strcmp(this->GetLogic()->SliderModelPath.c_str(),"") &&
       strcmp(this->GetLogic()->PistonModelPath.c_str(), "") &&
       strcmp(this->GetLogic()->CylinderModelPath.c_str(),""))
      {
      vtkMRMLModelNode* RailLoadedModel =  ModelsLogic->AddModel(this->GetLogic()->RailModelPath.c_str());
      vtkMRMLModelNode* SliderLoadedModel =  ModelsLogic->AddModel(this->GetLogic()->SliderModelPath.c_str());
      vtkMRMLModelNode* PistonLoadedModel =  ModelsLogic->AddModel(this->GetLogic()->PistonModelPath.c_str());
      vtkMRMLModelNode* CylinderLoadedModel =  ModelsLogic->AddModel(this->GetLogic()->CylinderModelPath.c_str());

      ModelsLogic->Delete();

      if(this->RailSelector && RailLoadedModel &&
         this->SliderSelector && SliderLoadedModel &&
         this->PistonSelector && PistonLoadedModel &&
         this->CylinderSelector && CylinderLoadedModel)
        {
        this->RailSelector->SetSelected(RailLoadedModel);
        this->SliderSelector->SetSelected(SliderLoadedModel);
        this->PistonSelector->SetSelected(PistonLoadedModel);
        this->CylinderSelector->SetSelected(CylinderLoadedModel);
        }

      if(this->GetLogic()->Distractor1.Range[0] != -1102.0 &&
         this->GetLogic()->Distractor1.Range[1] != -1102.0 &&
         this->MovingScale)
        {
        this->MovingScale->SetRange(this->GetLogic()->Distractor1.Range[0],
                                    this->GetLogic()->Distractor1.Range[1]);
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
  conBrowsFrame->SetLabelText("Test Frame 1");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Test child frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Test child frame");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  // -----------------------------------------
  // Test push button

  vtkKWLabel* RailLabel = vtkKWLabel::New();
  RailLabel->SetParent(frame->GetFrame());
  RailLabel->Create();
  RailLabel->SetText("Rail:");
  RailLabel->SetAnchorToWest();

  this->RailSelector = vtkSlicerNodeSelectorWidget::New();
  this->RailSelector->SetParent(frame->GetFrame());
  this->RailSelector->Create();
  this->RailSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
  this->RailSelector->SetNewNodeEnabled(false);
  this->RailSelector->SetMRMLScene(this->GetMRMLScene());
  this->RailSelector->UpdateMenu();

  vtkKWLabel* SliderLabel = vtkKWLabel::New();
  SliderLabel->SetParent(frame->GetFrame());
  SliderLabel->Create();
  SliderLabel->SetText("Slider:");
  SliderLabel->SetAnchorToWest();

  this->SliderSelector = vtkSlicerNodeSelectorWidget::New();
  this->SliderSelector->SetParent(frame->GetFrame());
  this->SliderSelector->Create();
  this->SliderSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
  this->SliderSelector->SetNewNodeEnabled(false);
  this->SliderSelector->SetMRMLScene(this->GetMRMLScene());
  this->SliderSelector->UpdateMenu();

  vtkKWLabel* PistonLabel = vtkKWLabel::New();
  PistonLabel->SetParent(frame->GetFrame());
  PistonLabel->Create();
  PistonLabel->SetText("Piston:");
  PistonLabel->SetAnchorToWest();

  this->PistonSelector = vtkSlicerNodeSelectorWidget::New();
  this->PistonSelector->SetParent(frame->GetFrame());
  this->PistonSelector->Create();
  this->PistonSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
  this->PistonSelector->SetNewNodeEnabled(false);
  this->PistonSelector->SetMRMLScene(this->GetMRMLScene());
  this->PistonSelector->UpdateMenu();

  vtkKWLabel* CylinderLabel = vtkKWLabel::New();
  CylinderLabel->SetParent(frame->GetFrame());
  CylinderLabel->Create();
  CylinderLabel->SetText("Cylinder:");
  CylinderLabel->SetAnchorToWest();

  this->CylinderSelector = vtkSlicerNodeSelectorWidget::New();
  this->CylinderSelector->SetParent(frame->GetFrame());
  this->CylinderSelector->Create();
  this->CylinderSelector->SetNodeClass("vtkMRMLModelNode",NULL,NULL,NULL);
  this->CylinderSelector->SetNewNodeEnabled(false);
  this->CylinderSelector->SetMRMLScene(this->GetMRMLScene());
  this->CylinderSelector->UpdateMenu();


  this->MovingScale = vtkKWScale::New();
  this->MovingScale->SetParent(frame->GetFrame());
  this->MovingScale->Create();
  this->MovingScale->SetRange(-180,180);

  this->LoadDistractorButton = vtkKWPushButton::New();
  this->LoadDistractorButton->SetParent(frame->GetFrame());
  this->LoadDistractorButton->Create();
  this->LoadDistractorButton->SetText("Load Distractor");

  this->Script("pack %s %s %s %s %s %s %s %s %s %s -side top -fill x -expand y -padx 2 -pady 2",
               RailLabel->GetWidgetName(),
               this->RailSelector->GetWidgetName(),
               SliderLabel->GetWidgetName(),
               this->SliderSelector->GetWidgetName(),
               PistonLabel->GetWidgetName(),
               this->PistonSelector->GetWidgetName(),
               CylinderLabel->GetWidgetName(),
               this->CylinderSelector->GetWidgetName(),
               this->MovingScale->GetWidgetName(),
               this->LoadDistractorButton->GetWidgetName());


  RailLabel->Delete();
  SliderLabel->Delete();
  PistonLabel->Delete();
  CylinderLabel->Delete();
  conBrowsFrame->Delete();
  frame->Delete();

}

//----------------------------------------------------------------------------
void vtkDistractorModelingGUI::UpdateAll()
{
}

