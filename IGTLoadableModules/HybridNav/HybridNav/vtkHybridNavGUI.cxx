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

#include "vtkHybridNavGUI.h"
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

#include "vtkCornerAnnotation.h"

#include "vtkPivotCalibration.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkHybridNavGUI );
vtkCxxRevisionMacro ( vtkHybridNavGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkHybridNavGUI::vtkHybridNavGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkHybridNavGUI::DataCallback);

  //----------------------------------------------------------------
  // GUI widgets
  this->ToolNodeSelectorMenu = NULL;
  this->StartCalibrateButton = NULL;
  this->numPointsEntry = NULL;
  this->TestButton21 = NULL;
  this->TestButton22 = NULL;

  //----------------------------------------------------------------
  // Variables
  this->pivot = vtkPivotCalibration::New();

  //----------------------------------------------------------------
  // Locator  (MRML)  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkHybridNavGUI::~vtkHybridNavGUI ( )
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

  if (this->ToolNodeSelectorMenu)
    {
    this->ToolNodeSelectorMenu->SetParent(NULL);
    this->ToolNodeSelectorMenu->Delete();
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

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkHybridNavGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkHybridNavGUI::Enter()
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
void vtkHybridNavGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkHybridNavGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "HybridNavGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkHybridNavGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->ToolNodeSelectorMenu)
    {
    this->ToolNodeSelectorMenu
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

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkHybridNavGUI::AddGUIObservers ( )
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

  this->ToolNodeSelectorMenu
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->numPointsEntry
     ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->StartCalibrateButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkHybridNavGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkHybridNavGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkHybridNavLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkHybridNavGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkHybridNavGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  //Events related with node selection
  if (this->ToolNodeSelectorMenu == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    std::cerr << "Node Selector is pressed." << std::endl;
    }

  //Events related with changing numPoints text box
  else if (this->numPointsEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    std::cerr << "numPoints has been modified." << std::endl;
    }

  //Events related with pressing calibration button
  else if (this->StartCalibrateButton == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    //Acknowledge the button has been pressed
    std::cerr << "StartCalibrateButton is pressed." << std::endl;

    //Check if a node has been selected
    if (this->ToolNodeSelectorMenu->GetSelected())
      {
      vtkMRMLNode* node = this->ToolNodeSelectorMenu->GetSelected();

      //Remove any current observers on node

      //Add observer on selected node
      vtkIntArray* nodeEvents = vtkIntArray::New();
      nodeEvents->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
      vtkMRMLNode* nullNode = NULL;
      vtkSetAndObserveMRMLNodeEventsMacro(nullNode, node, nodeEvents);
      nodeEvents->Delete();

      //Initialize pivot calibration object
      pivot->Initialize(this->numPointsEntry->GetValueAsInt(), node);
      }
    }
}

//----------------------------------------------------------------------------------------
void vtkHybridNavGUI::DataCallback(vtkObject *caller,
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkHybridNavGUI *self = reinterpret_cast<vtkHybridNavGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkHybridNavGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkHybridNavGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkHybridNavLogic::SafeDownCast(caller))
    {
    if (event == vtkHybridNavLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkHybridNavGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Detect if something has happened in the MRML scene
  if (caller != NULL)
    {
    std::cerr<< "MRML event called" << std::endl;
    vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);

    //Check to see if calibration transform node has been updated
    if ((node == dynamic_cast<vtkMRMLNode*>(pivot->transformNode) ) && (event == vtkMRMLTransformableNode::TransformModifiedEvent))
      {
      //Print node in terminal and send node to calibration vector
      std::cerr << "Calibration Node has been updated" << std::endl;
      int i = pivot->AcquireTransform();
      if (i == 1)
        {
        std::cerr << "Calibration matrix: ";
        pivot->CalibrationTransform->Print(std::cerr);
        }
      //node->RemoveObservers(vtkMRMLTransformableNode::TransformModifiedEvent, (vtkCommand *)this->GUICallbackCommand);
      //node->RemoveAllObservers();
      }
    }
}

//---------------------------------------------------------------------------
void vtkHybridNavGUI::ProcessTimerEvents()
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
void vtkHybridNavGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME
  // create a page
  this->UIPanel->AddPage ( "HybridNav", "HybridNav", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForCalibrationFrame();

}


void vtkHybridNavGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help =
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:HybridNav</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "HybridNav" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkHybridNavGUI::BuildGUIForCalibrationFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("HybridNav");

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

  vtkKWLabel* nodeLabel = vtkKWLabel::New();
  nodeLabel->SetParent(nodeFrame);
  nodeLabel->Create();
  nodeLabel->SetText("Select Transform: ");
  nodeLabel->SetWidth(15);

  this->ToolNodeSelectorMenu = vtkSlicerNodeSelectorWidget::New();
  this->ToolNodeSelectorMenu->SetParent(nodeFrame);
  this->ToolNodeSelectorMenu->Create();
  this->ToolNodeSelectorMenu->SetWidth(15);
  this->ToolNodeSelectorMenu->SetNewNodeEnabled(0);
  this->ToolNodeSelectorMenu->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, NULL);
  this->ToolNodeSelectorMenu->NoneEnabledOn();
  this->ToolNodeSelectorMenu->SetShowHidden(1);
  this->ToolNodeSelectorMenu->Create();
  this->ToolNodeSelectorMenu->SetMRMLScene(this->Logic->GetMRMLScene());
  this->ToolNodeSelectorMenu->UpdateMenu();
  this->ToolNodeSelectorMenu->SetBorderWidth(0);
  //this->NodeSelectorMenu->SetLabelText( "Select Transform");
  this->ToolNodeSelectorMenu->SetBalloonHelpString("Select a transform from the Scene");

  this->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
               nodeLabel->GetWidgetName(), this->ToolNodeSelectorMenu->GetWidgetName());

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
  this->numPointsEntry->SetWidth(6);
  this->numPointsEntry->SetValueAsInt(20);

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
  this->StartCalibrateButton->SetText ("Start Calibration");
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
  nodeLabel->Delete();

}

//----------------------------------------------------------------------------
void vtkHybridNavGUI::UpdateAll()
{
}

