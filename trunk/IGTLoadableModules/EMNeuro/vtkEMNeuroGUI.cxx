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

#include "vtkEMNeuroGUI.h"
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
#include "vtkKWEntry.h"

#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkMatrix4x4.h"
#include "vtkCornerAnnotation.h"
#include "vtkMRMLNode.h"
#include "vtkPivotCalibration.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkEMNeuroGUI );
vtkCxxRevisionMacro ( vtkEMNeuroGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkEMNeuroGUI::vtkEMNeuroGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkEMNeuroGUI::DataCallback);

  //----------------------------------------------------------------
  // GUI widgets
  this->NodeSelectorMenu = NULL;
  this->StartCalibrateButton = NULL;
  this->numPointsEntry = NULL;
  this->TestButton21 = NULL;
  this->TestButton22 = NULL;

  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

  //----------------------------------------------------------------
  // Variables
  this->pivot = vtkPivotCalibration::New();

}

//---------------------------------------------------------------------------
vtkEMNeuroGUI::~vtkEMNeuroGUI ( )
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

  if (this->NodeSelectorMenu)
    {
    this->NodeSelectorMenu->SetParent(NULL);
    this->NodeSelectorMenu->Delete();
    }

  if (this->numPointsEntry)
    {
    this->numPointsEntry->SetParent(NULL);
    this->numPointsEntry->Delete();
    }

  if (this->StartCalibrateButton)
    {
    this->StartCalibrateButton->SetParent(NULL);
    this->StartCalibrateButton->Delete();
    }

  if (this->TestButton21)
    {
    this->TestButton21->SetParent(NULL);
    this->TestButton21->Delete();
    }

  if (this->TestButton22)
    {
    this->TestButton22->SetParent(NULL);
    this->TestButton22->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkEMNeuroGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkEMNeuroGUI::Enter()
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
void vtkEMNeuroGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkEMNeuroGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "EMNeuroGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkEMNeuroGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->NodeSelectorMenu)
    {
    this->NodeSelectorMenu
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->numPointsEntry)
    {
    this->numPointsEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->StartCalibrateButton)
    {
    this->StartCalibrateButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TestButton21)
    {
    this->TestButton21
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TestButton22)
    {
    this->TestButton22
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }


  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkEMNeuroGUI::AddGUIObservers ( )
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

  this->NodeSelectorMenu
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->numPointsEntry
     ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->StartCalibrateButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton21
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton22
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkEMNeuroGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkEMNeuroGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkEMNeuroLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkEMNeuroGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkEMNeuroGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  if (this->NodeSelectorMenu == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    std::cerr << "Node Selector is pressed." << std::endl;
    }
  else if (this->numPointsEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    std::cerr << "numPoints has been modified." << std::endl;
    }
  else if (this->StartCalibrateButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "StartCalibrateButton is pressed." << std::endl;
    //Check if node has been selected
    if (this->NodeSelectorMenu->GetSelected())
      {
      //Observe selected node
      vtkMRMLNode* node = this->NodeSelectorMenu->GetSelected();
      vtkIntArray* nodeEvents = vtkIntArray::New();
      nodeEvents->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
      vtkMRMLNode* nullNode = NULL;
      vtkSetAndObserveMRMLNodeEventsMacro(nullNode, node, nodeEvents);
      nodeEvents->Delete();
      //Initialize Pivot Calibration
      pivot->Initialize(10, node);
      }
    }
  else if (this->TestButton21 == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "TestButton21 is pressed." << std::endl;
    }
  else if (this->TestButton22 == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "TestButton22 is pressed." << std::endl;
    }

}


void vtkEMNeuroGUI::DataCallback(vtkObject *caller,
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkEMNeuroGUI *self = reinterpret_cast<vtkEMNeuroGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkEMNeuroGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkEMNeuroGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkEMNeuroLogic::SafeDownCast(caller))
    {
    if (event == vtkEMNeuroLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkEMNeuroGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Detect if something has happened in the MRML scene
  if (caller != NULL)
    {
    std::cerr<< "MRML event called" << std::endl;
    vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);

    //Check to see if calibration node has been updated
    if (node == NodeSelectorMenu->GetSelected())
      {
      //Print node in terminal and send node to calibration vector
      std::cerr << "Calibration Node has been updated" << std::endl;
      node->Print(std::cerr);
      pivot->AcquireTransform();
      }
    else
      {
    //GetMatrixTransformtoParent()
    //else remove observer on node
      }
    }

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkEMNeuroGUI::ProcessTimerEvents()
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
void vtkEMNeuroGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME
  // create a page
  this->UIPanel->AddPage ( "EMNeuro", "EMNeuro", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForCalibrationFrame();
  BuildGUIForTestFrame2();

}


void vtkEMNeuroGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help =
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:EMNeuro</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "EMNeuro" );
  this->BuildHelpAndAboutFrame (page, help, about);
}

//---------------------------------------------------------------------------
void vtkEMNeuroGUI::BuildGUIForCalibrationFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("EMNeuro");

  vtkSlicerModuleCollapsibleFrame *calibrationFrame = vtkSlicerModuleCollapsibleFrame::New();

  calibrationFrame->SetParent(page);
  calibrationFrame->Create();
  calibrationFrame->SetLabelText("Pivot Calibration");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               calibrationFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Calibration Options Frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(calibrationFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Calibration Options");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  //Node Selector Menu
  vtkKWFrame *nodeFrame = vtkKWFrame::New();
  nodeFrame->SetParent(frame->GetFrame());
  nodeFrame->Create();
  //nodeFrame->SetWidth(20);
  app->Script ( "pack %s -fill both -expand true",
                nodeFrame->GetWidgetName());

  this->NodeSelectorMenu = vtkSlicerNodeSelectorWidget::New();
  this->NodeSelectorMenu->SetParent(nodeFrame);
  this->NodeSelectorMenu->Create();
  this->NodeSelectorMenu->SetWidth(15);
  this->NodeSelectorMenu->SetNewNodeEnabled(0);
  this->NodeSelectorMenu->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, NULL);
  this->NodeSelectorMenu->NoneEnabledOn();
  this->NodeSelectorMenu->SetShowHidden(1);
  this->NodeSelectorMenu->Create();
  this->NodeSelectorMenu->SetMRMLScene(this->Logic->GetMRMLScene());
  this->NodeSelectorMenu->UpdateMenu();
  this->NodeSelectorMenu->SetBorderWidth(0);
  this->NodeSelectorMenu->SetLabelText( "Select Transform");
  this->NodeSelectorMenu->SetBalloonHelpString("Select a trasnform from the Scene");

  this->Script("pack %s -side left -padx 2 -pady 2",
               this->NodeSelectorMenu->GetWidgetName());

  //Number of points for calibration entry
  vtkKWFrame* pointsFrame = vtkKWFrame::New();
  pointsFrame->SetParent(frame->GetFrame());
  pointsFrame->Create();
  this->Script ( "pack %s -fill both -expand true",
                 pointsFrame->GetWidgetName());

  vtkKWLabel* pointsLabel = vtkKWLabel::New();
  pointsLabel->SetParent(pointsFrame);
  pointsLabel->Create();
  pointsLabel->SetWidth(15);
  pointsLabel->SetText("Number of Points: ");

  this->numPointsEntry = vtkKWEntry::New();
  this->numPointsEntry->SetParent(pointsFrame);
  this->numPointsEntry->SetRestrictValueToInteger();
  this->numPointsEntry->Create();
  this->numPointsEntry->SetWidth(8);

  this->Script ( "pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
                 pointsLabel->GetWidgetName(),
                 this->numPointsEntry->GetWidgetName());

  //Calibration button
  vtkKWFrame* startCalibrationFrame = vtkKWFrame::New();
  startCalibrationFrame->SetParent(calibrationFrame->GetFrame());
  startCalibrationFrame->Create();
  this->Script ( "pack %s -fill both -expand true",
                 startCalibrationFrame->GetWidgetName());

  this->StartCalibrateButton = vtkKWPushButton::New ( );
  this->StartCalibrateButton->SetParent ( startCalibrationFrame );
  this->StartCalibrateButton->Create ( );
  this->StartCalibrateButton->SetText ("Get Transfer");
  this->StartCalibrateButton->SetWidth (15);
  this->Script ( "pack %s -side left -expand true",
                 StartCalibrateButton->GetWidgetName());

  //Delete widget pointers
  calibrationFrame->Delete();
  nodeFrame->Delete();
  pointsFrame->Delete();
  pointsLabel->Delete();
  startCalibrationFrame->Delete();
  frame->Delete();

}


//---------------------------------------------------------------------------
void vtkEMNeuroGUI::BuildGUIForTestFrame2 ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("EMNeuro");

  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Test Frame 2");
  //conBrowsFrame->CollapseFrame();
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

  this->TestButton21 = vtkKWPushButton::New ( );
  this->TestButton21->SetParent ( frame->GetFrame() );
  this->TestButton21->Create ( );
  this->TestButton21->SetText ("Test 21");
  this->TestButton21->SetWidth (12);

  this->TestButton22 = vtkKWPushButton::New ( );
  this->TestButton22->SetParent ( frame->GetFrame() );
  this->TestButton22->Create ( );
  this->TestButton22->SetText ("Tset 22");
  this->TestButton22->SetWidth (12);

  this->Script("pack %s %s -side left -padx 2 -pady 2",
               this->TestButton21->GetWidgetName(),
               this->TestButton22->GetWidgetName());


  conBrowsFrame->Delete();
  frame->Delete();
}


//----------------------------------------------------------------------------
void vtkEMNeuroGUI::UpdateAll()
{
}

