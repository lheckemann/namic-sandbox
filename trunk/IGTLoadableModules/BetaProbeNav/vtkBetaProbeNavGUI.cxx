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

#include "vtkBetaProbeNavGUI.h"
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
#include  "vtkSlicerNodeSelectorWidget.h"

#include "vtkCornerAnnotation.h"
#include "vtkMRMLModelNode.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkBetaProbeNavGUI );
vtkCxxRevisionMacro ( vtkBetaProbeNavGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkBetaProbeNavGUI::vtkBetaProbeNavGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkBetaProbeNavGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->StartButton = NULL;
  this->StopButton = NULL;
  this->TransformNode = NULL;
  this->CountNode = NULL;
  
  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;
  this->TimerInterval = 1000; //ms

}

//---------------------------------------------------------------------------
vtkBetaProbeNavGUI::~vtkBetaProbeNavGUI ( )
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

  if (this->StartButton)
    {
    this->StartButton->SetParent(NULL);
    this->StartButton->Delete();
    }

  if (this->StopButton)
    {
    this->StopButton->SetParent(NULL);
    this->StopButton->Delete();
    }

  if (this->TransformNode)
    {
    this->TransformNode->SetParent(NULL);
    this->TransformNode->Delete();
    }

  if (this->CountNode)
    {
    this->CountNode->SetParent(NULL);
    this->CountNode->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::Enter()
{
  // Fill in

}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "BetaProbeNavGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->StartButton)
    {
    this->StartButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->StopButton)
    {
    this->StopButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TransformNode)
    {
    this->TransformNode
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->CountNode)
    {
    this->CountNode
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  //events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  //events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  
  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

  this->StartButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->StopButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TransformNode
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->CountNode
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkBetaProbeNavLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  
  if (this->StartButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "Start button is pressed." << std::endl;
    //Make sure Transform Node and Count Node is selected
    if ((TransformNode) && (CountNode))
      {
      //Start Timer to collect data points
      this->TimerFlag = 1;
      ProcessTimerEvents();
      }
    }
  else if (this->StopButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "Stop button is pressed." << std::endl;
    if ((TransformNode) && (CountNode))
      {
      //Stop Timer to stop data point collection
      this->TimerFlag = 0;
      //Represent the tool
      vtkMRMLModelNode* mnode = vtkMRMLModelNode::New();
      mnode = this->GetLogic()->RepresentData(mnode);
      this->GetMRMLScene()->AddNode(mnode);
      this->GetMRMLScene()->Modified();
      mnode->Modified();
      mnode->Delete();
      }
    }
  else if (this->TransformNode == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    std::cerr << "Transform node menu is selected." << std::endl;
    }
  else if (this->CountNode == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    std::cerr << "Count Node menu is selected." << std::endl;
    }

} 

//--------------------------------------------------------------------------
void vtkBetaProbeNavGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkBetaProbeNavGUI *self = reinterpret_cast<vtkBetaProbeNavGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkBetaProbeNavGUI DataCallback");
  self->UpdateAll();
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkBetaProbeNavLogic::SafeDownCast(caller))
    {
    if (event == vtkBetaProbeNavLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}

//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    if ((this->TransformNode) && (this->CountNode))
      {
      this->GetLogic()->CollectData(this->TransformNode->GetSelected(), this->CountNode->GetSelected());
      }
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "BetaProbeNav", "BetaProbeNav", NULL );

  BuildGUIForImportDataFrame();

}

//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:BetaProbeNav</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "BetaProbeNav" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkBetaProbeNavGUI::BuildGUIForImportDataFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("BetaProbeNav");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Import Data Frame");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Data Selection Frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Data Selection frame");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  //Transform Node Menu
  vtkKWFrame *tnodeFrame = vtkKWFrame::New();
  tnodeFrame->SetParent(frame->GetFrame());
  tnodeFrame->Create();
  app->Script ( "pack %s -fill both -expand true",
                tnodeFrame->GetWidgetName());

  vtkKWLabel *tnodeLabel = vtkKWLabel::New();
  tnodeLabel->SetParent(tnodeFrame);
  tnodeLabel->Create();
  tnodeLabel->SetWidth(15);
  tnodeLabel->SetText("Transform Node: ");

  this->TransformNode = vtkSlicerNodeSelectorWidget::New();
  this->TransformNode->SetParent(tnodeFrame);
  this->TransformNode->Create();
  this->TransformNode->SetWidth(20);
  this->TransformNode->SetNewNodeEnabled(0);
  this->TransformNode->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, NULL);
  this->TransformNode->NoneEnabledOn();
  this->TransformNode->SetShowHidden(1);
  this->TransformNode->Create();
  this->TransformNode->SetMRMLScene(this->Logic->GetMRMLScene());
  this->TransformNode->UpdateMenu();
  this->TransformNode->SetBorderWidth(0);
  this->TransformNode->SetBalloonHelpString("Select a transform from the Scene");
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              tnodeLabel->GetWidgetName() , this->TransformNode->GetWidgetName());
  
  tnodeLabel->Delete();
  tnodeFrame->Delete();
  
  //Count Node Menu
  vtkKWFrame *cnodeFrame = vtkKWFrame::New();
  cnodeFrame->SetParent(frame->GetFrame());
  cnodeFrame->Create();
  app->Script ( "pack %s -fill both -expand true",
                cnodeFrame->GetWidgetName());

  vtkKWLabel *cnodeLabel = vtkKWLabel::New();
  cnodeLabel->SetParent(cnodeFrame);
  cnodeLabel->Create();
  cnodeLabel->SetWidth(15);
  cnodeLabel->SetText("Counts Node: ");

  this->CountNode = vtkSlicerNodeSelectorWidget::New();
  this->CountNode->SetParent(cnodeFrame);
  this->CountNode->Create();
  this->CountNode->SetWidth(20);
  this->CountNode->SetNewNodeEnabled(0);
  this->CountNode->SetNodeClass("vtkMRMLUDPServerNode", NULL, NULL, NULL);
  this->CountNode->NoneEnabledOn();
  this->CountNode->SetShowHidden(1);
  this->CountNode->Create();
  this->CountNode->SetMRMLScene(this->Logic->GetMRMLScene());
  this->CountNode->UpdateMenu();
  this->CountNode->SetBorderWidth(0);
  this->CountNode->SetBalloonHelpString("Select a count node from the Scene");
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              cnodeLabel->GetWidgetName() , this->CountNode->GetWidgetName());
  
  cnodeLabel->Delete();
  cnodeFrame->Delete();

  //Start button
  vtkKWFrame *startFrame = vtkKWFrame::New();
  startFrame->SetParent(frame->GetFrame());
  startFrame->Create();
  app->Script ( "pack %s -padx 2 -pady 2",
                startFrame->GetWidgetName());
  
  this->StartButton = vtkKWPushButton::New( );
  this->StartButton ->SetParent(startFrame);
  this->StartButton ->Create( );
  this->StartButton ->SetText("Start");
  this->StartButton ->SetWidth(12);

  //Stop Button
  this->StopButton = vtkKWPushButton::New( );
  this->StopButton ->SetParent(startFrame);
  this->StopButton ->Create( );
  this->StopButton ->SetText("Stop");
  this->StopButton ->SetWidth(12);

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              this->StartButton->GetWidgetName(),
              this->StopButton->GetWidgetName());

  startFrame->Delete();
  
  conBrowsFrame->Delete();
  frame->Delete();

}

//----------------------------------------------------------------------------
void vtkBetaProbeNavGUI::UpdateAll()
{
}

