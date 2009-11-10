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
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWMultiColumnList.h"

#include "vtkCornerAnnotation.h"
#include "vtkPivotCalibration.h"
#include "vtkMRMLHybridNavToolNode.h"

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

  //----------------------------------------------------------------
  //Tools Frame
  this->ToolList = NULL;
  this->AddToolButton = NULL;
  this->DeleteToolButton = NULL;
  this->ToolNameEntry = NULL;
  this->ToolNodeSelectorMenu = NULL;
  this->ToolDescriptionEntry = NULL;

  //----------------------------------------------------------------
  //Pivot Calibration Frame
  this->CalibrationNodeSelectorMenu = NULL;
  this->StartCalibrateButton = NULL;
  this->numPointsEntry = NULL;

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

  //----------------------------------------------------------------
  //Tools Frame
  if (this->ToolList)
    {
    this->ToolList->SetParent(NULL);
    this->ToolList->Delete();
    }
  if (this->AddToolButton)
    {
    this->AddToolButton->SetParent(NULL);
    this->AddToolButton->Delete();
    }
  if (this->DeleteToolButton)
    {
    this->DeleteToolButton->SetParent(NULL);
    this->DeleteToolButton->Delete();
    }
  if (this->ToolNameEntry)
    {
    this->ToolNameEntry->SetParent(NULL);
    this->ToolNameEntry->Delete();
    }
  if (this->ToolNodeSelectorMenu)
    {
    this->ToolNodeSelectorMenu->SetParent(NULL);
    this->ToolNodeSelectorMenu->Delete();
    }
  if (this->ToolDescriptionEntry)
    {
    this->ToolDescriptionEntry->SetParent(NULL);
    this->ToolDescriptionEntry->Delete();
    }

  //-----------------------------------------------------------------
  //Pivot Calibration Frame
  if (this->CalibrationNodeSelectorMenu)
    {
    this->CalibrationNodeSelectorMenu->SetParent(NULL);
    this->CalibrationNodeSelectorMenu->Delete();
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
  vtkMRMLScene* scene = this->GetMRMLScene();

  vtkMRMLHybridNavToolNode* toolNode = vtkMRMLHybridNavToolNode::New();
  scene->RegisterNodeClass(toolNode);
  toolNode->Delete();

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

  //-------------------------------------------------------------------------
  //Tools Frame
  if (this->ToolList && this->ToolList->GetWidget())
    {
    this->ToolList->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->AddToolButton)
    {
    this->AddToolButton
       ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->DeleteToolButton)
    {
    this->DeleteToolButton
       ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ToolNameEntry)
    {
    this->ToolNameEntry
       ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ToolNodeSelectorMenu)
    {
    this->ToolNodeSelectorMenu
       ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ToolDescriptionEntry)
    {
    this->ToolDescriptionEntry
       ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  //--------------------------------------------------------------------------
  //Calibration Frame
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
  if (this->CalibrationNodeSelectorMenu)
    {
    this->CalibrationNodeSelectorMenu
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkHybridNavGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();
 // vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);

  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

  //----------------------------------------------------------------
  //Tool Frame
  this->ToolList->GetWidget()
    ->AddObserver(vtkKWMultiColumnList::SelectionChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->AddToolButton
   ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->DeleteToolButton
   ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->ToolNameEntry
     ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->ToolNodeSelectorMenu
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->ToolDescriptionEntry
     ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);

  //----------------------------------------------------------------
  //Calibration frame
  this->CalibrationNodeSelectorMenu
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

  //--------------------------------------------------------------------------
  //Tool Frame
  else if (this->ToolList->GetWidget() == vtkKWMultiColumnList::SafeDownCast(caller)
           && event == vtkKWMultiColumnList::SelectionChangedEvent)
    {
    std::cerr << "Tool selected" << std::endl;
    }
  else if (this->AddToolButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "Tool has been added" << std::endl;
    //Create Tool object, populate list and add node to scene
    vtkMRMLHybridNavToolNode* tool = vtkMRMLHybridNavToolNode::New();
    this->GetMRMLScene()->AddNode(tool);
    tool->Modified();
    tool->Delete();
    }
  else if (this->DeleteToolButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "Tool has been deleted" << std::endl;
    int selected = this->ToolList->GetWidget()->GetIndexOfFirstSelectedRow();
      if (selected >= 0 && selected < (int)this->ToolNodeList.size())
        {
        vtkMRMLHybridNavToolNode* tnode
          = vtkMRMLHybridNavToolNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->ToolNodeList[selected]));
        if (tnode)
          {
          this->GetMRMLScene()->RemoveNode(tnode);
          this->GetMRMLScene()->Modified();
          //tnode->Delete();
          int nrow = this->ToolList->GetWidget()->GetNumberOfRows();
          if (selected >= nrow)
            {
            selected = nrow - 1;
            }
          this->ToolList->GetWidget()->SelectSingleRow(selected);
          //UpdateToolPropertyFrame(selected);
          }
        }
    }
  else if (this->ToolNameEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    std::cerr << "Tool Name has been modified." << std::endl;
    int selected = this->ToolList->GetWidget()->GetIndexOfFirstSelectedRow();
    if (selected >= 0 && selected < (int)this->ToolNodeList.size())
      {
      vtkMRMLHybridNavToolNode* tool
        = vtkMRMLHybridNavToolNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->ToolNodeList[selected]));
      if (tool)
        {
        tool->SetName(this->ToolNameEntry->GetValue());
        UpdateToolList(UPDATE_SELECTED_ONLY);
        }
      }
    }
  else if (this->ToolNodeSelectorMenu == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    std::cerr << "Node Selector is pressed." << std::endl;
    int selected = this->ToolList->GetWidget()->GetIndexOfFirstSelectedRow();
    if (selected >= 0 && selected < (int)this->ToolNodeList.size())
      {
      vtkMRMLHybridNavToolNode* tool
        = vtkMRMLHybridNavToolNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->ToolNodeList[selected]));
      if (tool)
        {
        tool->SetToolNode(this->ToolNodeSelectorMenu->GetSelected());
        UpdateToolList(UPDATE_SELECTED_ONLY);
        }
      }
    }
  else if (this->ToolDescriptionEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    std::cerr << "Tool description has been modified." << std::endl;
    int selected = this->ToolList->GetWidget()->GetIndexOfFirstSelectedRow();
    if (selected >= 0 && selected < (int)this->ToolNodeList.size())
      {
      vtkMRMLHybridNavToolNode* tool
        = vtkMRMLHybridNavToolNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->ToolNodeList[selected]));
      if (tool)
        {
        tool->SetToolDescription(this->ToolDescriptionEntry->GetValue());
        UpdateToolList(UPDATE_SELECTED_ONLY);
        }
      }
    }

  //--------------------------------------------------------------------------
  //Calibration Frame
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
    if (this->CalibrationNodeSelectorMenu->GetSelected())
      {
      vtkMRMLNode* node = this->CalibrationNodeSelectorMenu->GetSelected();

      //Remove any current observers on node

      //Add observer on selected node
      vtkIntArray* nodeEvents = vtkIntArray::New();
      nodeEvents->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
      vtkSetAndObserveMRMLNodeEventsMacro(node, node, nodeEvents);
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
  // -----------------------------------------
  // Adding a new node
  if (event == vtkMRMLScene::NodeAddedEvent)
    {
    vtkObject* obj = (vtkObject*)callData;
    vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(obj);
    if (node && strcmp(node->GetNodeTagName(), "HybridNavTool") == 0)
      {
      std::cerr << "Node event" << std::endl;
      vtkMRMLHybridNavToolNode* tnode = vtkMRMLHybridNavToolNode::SafeDownCast(node);
      // obtain the list of tools in the scene
      UpdateToolNodeList();
      UpdateToolList(UPDATE_ALL);
      int select = this->ToolList->GetWidget()->GetNumberOfRows() - 1;
      this->ToolList->GetWidget()->SelectSingleRow(select);
      UpdateToolPropertyFrame(select);
      }
    else
      {
      //UpdateRealTimeImageSourceMenu();
      }
    }

  //---------------------------------------------
  // Removing a node
  else if (event == vtkMRMLScene::NodeRemovedEvent)
    {
    std::cerr << "Tool Node to be deleted" << std::endl;
    //vtkObject* obj = (vtkObject*)callData;
    /*vtkMRMLHybridNavToolNode* node = vtkMRMLHybridNavToolNode::SafeDownCast(obj);
    if (node)
      {
      //UpdateToolNodeList();
      //UpdateToolList(UPDATE_ALL);
      }*/
    }

  // Detect if something has happened in the MRML scene
  /*if (caller != NULL)
    {
    std::cerr<< "MRML event called" << std::endl;
    //vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(caller);

    //Check to see if calibration transform node has been updated
    /*if ((node == dynamic_cast<vtkMRMLNode*>(pivot->transformNode) ) && (event == vtkMRMLTransformableNode::TransformModifiedEvent))
      {
      std::cerr<< "Node event called" << std::endl;
      //Print node in terminal and send node to calibration vector
      std::cerr << "Calibration Node has been updated" << std::endl;
      int i = pivot->AcquireTransform();
      if (i == 1)
        {
        std::cerr << "Calibration matrix: ";
        pivot->CalibrationTransform->Print(std::cerr);
        //Remove observer on selected node
        vtkSetAndObserveMRMLNodeEventsMacro(node, node, NULL);
        }
      }
    }*/
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
  BuildGUIForToolFrame();
  BuildGUIForCalibrationFrame();
}

//---------------------------------------------------------------------------
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
void vtkHybridNavGUI::BuildGUIForToolFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("HybridNav");

  vtkSlicerModuleCollapsibleFrame *ToolFrame = vtkSlicerModuleCollapsibleFrame::New();

  ToolFrame->SetParent(page);
  ToolFrame->Create();
  ToolFrame->SetLabelText("Tracked Tools");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               ToolFrame->GetWidgetName(), page->GetWidgetName());

  // ----------------------------------------------------------------------
  // Tracked Tools List Frame

  //Frame with Label
  vtkKWFrameWithLabel *listFrame = vtkKWFrameWithLabel::New();
  listFrame->SetParent(ToolFrame->GetFrame());
  listFrame->Create();
  listFrame->SetLabelText ("Tracked Tools");
  app->Script ( "pack %s -fill both -expand true",
                listFrame->GetWidgetName());
  listFrame->Delete();

  //Tool List Widget
  this->ToolList = vtkKWMultiColumnListWithScrollbars::New();
  this->ToolList->SetParent(listFrame->GetFrame());
  this->ToolList->Create();
  this->ToolList->SetHeight(1);
  this->ToolList->GetWidget()->SetSelectionTypeToRow();
  this->ToolList->GetWidget()->SetSelectionModeToSingle();
  this->ToolList->GetWidget()->MovableRowsOff();
  this->ToolList->GetWidget()->MovableColumnsOff();
  const char* labels[] =
    { "Name", "Tracking Node", "Calibrated?", "Description"};
  const int widths[] =
    { 10, 13, 10, 15 };
  for (int col = 0; col < 4; col ++)
    {
    this->ToolList->GetWidget()->AddColumn(labels[col]);
    this->ToolList->GetWidget()->SetColumnWidth(col, widths[col]);
    this->ToolList->GetWidget()->SetColumnAlignmentToLeft(col);
    this->ToolList->GetWidget()->ColumnEditableOff(col);
    this->ToolList->GetWidget()->SetColumnEditWindowToSpinBox(col);
    }
  this->ToolList->GetWidget()->SetColumnEditWindowToCheckButton(0);

  //ToolList Buttons Frame
  vtkKWFrame *listButtonsFrame = vtkKWFrame::New();
  listButtonsFrame->SetParent(listFrame->GetFrame());
  listButtonsFrame->Create();
  listButtonsFrame->Delete();
  app->Script ("pack %s %s -fill both -expand true",
               this->ToolList->GetWidgetName(), listButtonsFrame->GetWidgetName());

  //ToolList Buttons
  this->AddToolButton = vtkKWPushButton::New();
  this->AddToolButton->SetParent(listButtonsFrame);
  this->AddToolButton->Create();
  this->AddToolButton->SetText( "Add" );
  this->AddToolButton->SetWidth (6);

  this->DeleteToolButton = vtkKWPushButton::New();
  this->DeleteToolButton->SetParent(listButtonsFrame);
  this->DeleteToolButton->Create();
  this->DeleteToolButton->SetText( "Delete" );
  this->DeleteToolButton->SetWidth (6);

  app->Script( "pack %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
               this->AddToolButton->GetWidgetName(), this->DeleteToolButton->GetWidgetName());

  // ---------------------------------------------------------------------------------
  // Tool Property frame

  vtkKWFrameWithLabel *controlFrame = vtkKWFrameWithLabel::New();
  controlFrame->SetParent(ToolFrame->GetFrame());
  controlFrame->Create();
  controlFrame->SetLabelText ("Tool Properties");
  app->Script ( "pack %s -fill both -expand true",
                controlFrame->GetWidgetName());

  // Tool Property -- Tool name
  vtkKWFrame *nameFrame = vtkKWFrame::New();
  nameFrame->SetParent(controlFrame->GetFrame());
  nameFrame->Create();
  app->Script ( "pack %s -fill both -expand true",
                nameFrame->GetWidgetName());

  vtkKWLabel *nameLabel = vtkKWLabel::New();
  nameLabel->SetParent(nameFrame);
  nameLabel->Create();
  nameLabel->SetWidth(12);
  nameLabel->SetText("Name: ");

  this->ToolNameEntry = vtkKWEntry::New();
  this->ToolNameEntry->SetParent(nameFrame);
  this->ToolNameEntry->Create();
  this->ToolNameEntry->SetWidth(25);
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              nameLabel->GetWidgetName() , this->ToolNameEntry->GetWidgetName());
  nameLabel->Delete();
  nameFrame->Delete();

  //Tool Property -- Tool Tracking Node
  vtkKWFrame *nodeFrame = vtkKWFrame::New();
  nodeFrame->SetParent(controlFrame->GetFrame());
  nodeFrame->Create();
  app->Script ( "pack %s -fill both -expand true",
                nodeFrame->GetWidgetName());

  vtkKWLabel *nodeLabel = vtkKWLabel::New();
  nodeLabel->SetParent(nodeFrame);
  nodeLabel->Create();
  nodeLabel->SetWidth(12);
  nodeLabel->SetText("Node: ");

  this->ToolNodeSelectorMenu = vtkSlicerNodeSelectorWidget::New();
  this->ToolNodeSelectorMenu->SetParent(nodeFrame);
  this->ToolNodeSelectorMenu->Create();
  this->ToolNodeSelectorMenu->SetWidth(20);
  this->ToolNodeSelectorMenu->SetNewNodeEnabled(0);
  this->ToolNodeSelectorMenu->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, NULL);
  this->ToolNodeSelectorMenu->NoneEnabledOn();
  this->ToolNodeSelectorMenu->SetShowHidden(1);
  this->ToolNodeSelectorMenu->Create();
  this->ToolNodeSelectorMenu->SetMRMLScene(this->Logic->GetMRMLScene());
  this->ToolNodeSelectorMenu->UpdateMenu();
  this->ToolNodeSelectorMenu->SetBorderWidth(0);
  this->ToolNodeSelectorMenu->SetBalloonHelpString("Select a transform from the Scene");
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              nodeLabel->GetWidgetName() , this->ToolNodeSelectorMenu->GetWidgetName());
  nodeLabel->Delete();
  nodeFrame->Delete();

  //Tool Property -- Tool Description
  vtkKWFrame* descriptionFrame = vtkKWFrame::New();
  descriptionFrame->SetParent(controlFrame->GetFrame());
  descriptionFrame->Create();
  this->Script ( "pack %s -fill both -expand true",
                 descriptionFrame->GetWidgetName());

  vtkKWLabel* descriptionLabel = vtkKWLabel::New();
  descriptionLabel->SetParent(descriptionFrame);
  descriptionLabel->Create();
  descriptionLabel->SetWidth(12);
  descriptionLabel->SetText("Description: ");

  this->ToolDescriptionEntry = vtkKWEntry::New();
  this->ToolDescriptionEntry->SetParent(descriptionFrame);
  this->ToolDescriptionEntry->Create();
  this->ToolDescriptionEntry->SetWidth(25);

  this->Script ( "pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
                 descriptionLabel->GetWidgetName(),
                 this->ToolDescriptionEntry->GetWidgetName());

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

  this->CalibrationNodeSelectorMenu = vtkSlicerNodeSelectorWidget::New();
  this->CalibrationNodeSelectorMenu->SetParent(nodeFrame);
  this->CalibrationNodeSelectorMenu->Create();
  this->CalibrationNodeSelectorMenu->SetWidth(15);
  this->CalibrationNodeSelectorMenu->SetNewNodeEnabled(0);
  this->CalibrationNodeSelectorMenu->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, NULL);
  this->CalibrationNodeSelectorMenu->NoneEnabledOn();
  this->CalibrationNodeSelectorMenu->SetShowHidden(1);
  this->CalibrationNodeSelectorMenu->Create();
  this->CalibrationNodeSelectorMenu->SetMRMLScene(this->Logic->GetMRMLScene());
  this->CalibrationNodeSelectorMenu->UpdateMenu();
  this->CalibrationNodeSelectorMenu->SetBorderWidth(0);
  this->CalibrationNodeSelectorMenu->SetBalloonHelpString("Select a transform from the Scene");

  this->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
               nodeLabel->GetWidgetName(), this->CalibrationNodeSelectorMenu->GetWidgetName());

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

//----------------------------------------------------------------------------
void vtkHybridNavGUI::UpdateToolNodeList()
{
// update the list of tools in the scene, by saving their ID in a vector
  std::cerr << "Tool node list updated" << std::endl;
  std::vector<vtkMRMLNode*> nodes;
  const char* className = this->GetMRMLScene()->GetClassNameByTag("HybridNavTool");
  this->GetMRMLScene()->GetNodesByClass(className, nodes);

  this->ToolNodeList.clear();
  std::vector<vtkMRMLNode*>::iterator iter;
  for (iter = nodes.begin(); iter != nodes.end(); iter ++)
    {
    vtkMRMLHybridNavToolNode* ptr = vtkMRMLHybridNavToolNode::SafeDownCast(*iter);
    if (ptr)
      {
      this->ToolNodeList.push_back(ptr->GetID());
      }
    }
  return;
}

//---------------------------------------------------------------------------
void vtkHybridNavGUI::UpdateToolList(int updateLevel)
{
  std::cerr << "Tool list updated" << std::endl;
  // Update Tool List
  if (this->ToolList == NULL)
    {
    return;
    }

  //----------------------------------------------------------------
  // Change number of rows (UPDATE_ALL only)

  if (updateLevel >= UPDATE_ALL)
    {

    // Adjust number of rows in ToolList
    int numRows = this->ToolList->GetWidget()->GetNumberOfRows();
    int numTools = this->ToolNodeList.size();

    if (numRows < numTools)
      {
      this->ToolList->GetWidget()->AddRows(numTools-numRows);
      }
    else if (numRows > numTools)
      {
      int ndel = numRows-numTools;
      for (int i = 0; i < ndel; i ++)
        {
        this->ToolList->GetWidget()->DeleteRow(numTools);
        }
      }
    }
  int numItems = this->ToolNodeList.size();

  //----------------------------------------------------------------
  // Update rows (UPDATE_ALL, UPDATE_PROPERTIES_ALL and UPDATE_SELECTED_ONLY)

  // Generate list of rows to update
  std::vector<int> updateRows;
  updateRows.clear();
  if (updateLevel >= UPDATE_ALL)
    {
    for (int i = 0; i < numItems; i ++)
      {
      updateRows.push_back(i);
      }
    }
  else if (updateLevel >= UPDATE_SELECTED_ONLY)
    {
    updateRows.push_back(this->ToolList->GetWidget()->GetIndexOfFirstSelectedRow());
    }

  //Update the Tool List row by row
  std::vector<int>::iterator iter;
  for (iter = updateRows.begin(); iter != updateRows.end(); iter ++)
    {
    int i = *iter;
    if (i >= 0 && i < (int)this->ToolNodeList.size())
      {
      vtkMRMLHybridNavToolNode* tool
        = vtkMRMLHybridNavToolNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->ToolNodeList[i]));
      if (tool)
        {
        // Tool Name
        this->ToolList->GetWidget()
          ->SetCellText(i,0, tool->GetName());

        // Tool Node
        this->ToolList->GetWidget()
          ->SetCellText(i,1, tool->GetToolNodeAsChar());

        //Tool Calibrated
        const char* c;
        if(tool->GetCalibrated())
            c="Y";
        else
            c="N";
        this->ToolList->GetWidget()
          ->SetCellText(i,2, c);

        //Tool Description
        this->ToolList->GetWidget()
          ->SetCellText(i,3, tool->GetToolDescription());
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkHybridNavGUI::UpdateToolPropertyFrame(int i)
{
  // if i < 0, all fields are deactivated.

  int numRows = this->ToolList->GetWidget()->GetNumberOfRows();
  if (i >= 0 && i >= numRows)
    {
    return;
    }

  //----------------------------------------------------------------
  // No connector is registered
  if (i < 0 || numRows <= 0)
    {
    // Deactivate everything
    // Tool Name entry
    this->ToolNameEntry->SetValue("");
    this->ToolNameEntry->EnabledOff();
    this->ToolNameEntry->UpdateEnableState();

    // Tool Node
    //this->ToolNodeSelectorMenu->SetNoneEnabled(0);
    //this->ToolNodeSelectorMenu->UpdateEnableState();

    // Tool Description Entry
    this->ToolDescriptionEntry->SetValue("");
    this->ToolDescriptionEntry->EnabledOff();
    this->ToolDescriptionEntry->UpdateEnableState();

    return;
    }

  //----------------------------------------------------------------
  // A connector is selected on the list

  if (i < 0 || i >= (int)this->ToolNodeList.size())
    {
    return;
    }

  vtkMRMLHybridNavToolNode* tool
    = vtkMRMLHybridNavToolNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->ToolNodeList[i]));

  if (tool == NULL)  // if the connector has already beeen removed
    {
    UpdateToolNodeList();
    UpdateToolPropertyFrame(i);
    return;
    }

  // Tool Name entry
  this->ToolNameEntry->SetValue(tool->GetName());

  // Tool node
  if (tool->GetToolNode())
    {
    this->ToolNodeSelectorMenu->SetSelected((vtkMRMLNode*)tool->GetToolNode());
    }

  // Tool Description entry
  this->ToolDescriptionEntry->SetValue(tool->GetToolDescription());

}
