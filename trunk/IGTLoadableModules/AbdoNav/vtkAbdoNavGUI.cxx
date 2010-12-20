/*==========================================================================

  Portions (c) Copyright 2010-2011 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

/* AbdoNav includes */
#include "vtkAbdoNavGUI.h"

/* MRML includes */
#include "vtkMRMLLinearTransformNode.h"

/* Slicer includes */
#include "vtkSlicerApplication.h"
#include "vtkSlicerColor.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSlicerTheme.h"

/* KWWidgets includes */
#include "vtkKWComboBox.h"
#include "vtkKWComboBoxWithLabel.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWRadioButton.h"
#include "vtkKWTkUtilities.h"

//---------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkAbdoNavGUI, "$Revision: $");
vtkStandardNewMacro(vtkAbdoNavGUI);


//---------------------------------------------------------------------------
vtkAbdoNavGUI::vtkAbdoNavGUI()
{
  //----------------------------------------------------------------
  // Initialize logic values.
  //----------------------------------------------------------------
  this->Logic = NULL;
  this->AbdoNavNode = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData(reinterpret_cast<void*> (this));
  this->DataCallbackCommand->SetCallback(vtkAbdoNavGUI::DataCallback);
  this->TimerFlag = 0;

  //----------------------------------------------------------------
  // Connection frame.
  //----------------------------------------------------------------
  this->TrackerNodeSelectorWidget = NULL;
  this->TrackerComboBox = NULL;
  this->ResetConnectionPushButton = NULL;
  this->ConfigureConnectionPushButton = NULL;

  //----------------------------------------------------------------
  // Registration frame.
  //----------------------------------------------------------------
  this->Point1RadioButton = NULL;
  this->Point1REntry = NULL;
  this->Point1AEntry = NULL;
  this->Point1SEntry = NULL;
  this->Point2RadioButton = NULL;
  this->Point2REntry = NULL;
  this->Point2AEntry = NULL;
  this->Point2SEntry = NULL;
  this->ResetRegistrationPushButton = NULL;
  this->PerformRegistrationPushButton = NULL;

  //----------------------------------------------------------------
  // Navigation frame.
  //----------------------------------------------------------------
  this->ShowLocatorCheckButton = NULL;
  this->FreezeLocatorCheckButton = NULL;
  this->ShowCrosshairCheckButton = NULL;
  this->RedSliceMenuButton = NULL;
  this->YellowSliceMenuButton = NULL;
  this->GreenSliceMenuButton = NULL;
  this->SetLocatorAllPushButton = NULL;
  this->SetUserAllPushButton = NULL;
  this->FreezeSliceCheckButton = NULL;
}


//---------------------------------------------------------------------------
vtkAbdoNavGUI::~vtkAbdoNavGUI()
{
  //----------------------------------------------------------------
  // Cleanup logic values.
  //----------------------------------------------------------------
  // if Logic is NULL, the class was only instantiated but never used,
  // e.g. Slicer was launched with option --ignore_module set to AbdoNav
  if (this->Logic)
    {
    this->RemoveGUIObservers();
    }
  this->SetModuleLogic(NULL);

  vtkSetMRMLNodeMacro(this->AbdoNavNode, NULL);

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  //----------------------------------------------------------------
  // Connection frame.
  //----------------------------------------------------------------
  if (this->TrackerNodeSelectorWidget)
    {
    this->TrackerNodeSelectorWidget->SetParent(NULL);
    this->TrackerNodeSelectorWidget->Delete();
    }
  if (this->TrackerComboBox)
    {
    this->TrackerComboBox->SetParent(NULL);
    this->TrackerComboBox->Delete();
    }
  if (this->ResetConnectionPushButton)
    {
    this->ResetConnectionPushButton->SetParent(NULL);
    this->ResetConnectionPushButton->Delete();
    }
  if (this->ConfigureConnectionPushButton)
    {
    this->ConfigureConnectionPushButton->SetParent(NULL);
    this->ConfigureConnectionPushButton->Delete();
    }

  //----------------------------------------------------------------
  // Registration frame.
  //----------------------------------------------------------------
  if (this->Point1RadioButton)
    {
    this->Point1RadioButton->SetParent(NULL);
    this->Point1RadioButton->Delete();
    }
  if (this->Point1REntry)
    {
    this->Point1REntry->SetParent(NULL);
    this->Point1REntry->Delete();
    }
  if (this->Point1AEntry)
    {
    this->Point1AEntry->SetParent(NULL);
    this->Point1AEntry->Delete();
    }
  if (this->Point1SEntry)
    {
    this->Point1SEntry->SetParent(NULL);
    this->Point1SEntry->Delete();
    }
  if (this->Point2RadioButton)
    {
    this->Point2RadioButton->SetParent(NULL);
    this->Point2RadioButton->Delete();
    }
  if (this->Point2REntry)
    {
    this->Point2REntry->SetParent(NULL);
    this->Point2REntry->Delete();
    }
  if (this->Point2AEntry)
    {
    this->Point2AEntry->SetParent(NULL);
    this->Point2AEntry->Delete();
    }
  if (this->Point2SEntry)
    {
    this->Point2SEntry->SetParent(NULL);
    this->Point2SEntry->Delete();
    }
  if (this->ResetRegistrationPushButton)
    {
    this->ResetRegistrationPushButton->SetParent(NULL);
    this->ResetRegistrationPushButton->Delete();
    }
  if (this->PerformRegistrationPushButton)
    {
    this->PerformRegistrationPushButton->SetParent(NULL);
    this->PerformRegistrationPushButton->Delete();
    }

  //----------------------------------------------------------------
  // Navigation frame.
  //----------------------------------------------------------------
  if (this->ShowLocatorCheckButton)
    {
    this->ShowLocatorCheckButton->SetParent(NULL);
    this->ShowLocatorCheckButton->Delete();
    }
  if (this->FreezeLocatorCheckButton)
    {
    this->FreezeLocatorCheckButton->SetParent(NULL);
    this->FreezeLocatorCheckButton->Delete();
    }
  if (this->ShowCrosshairCheckButton)
    {
    this->ShowCrosshairCheckButton->SetParent(NULL);
    this->ShowCrosshairCheckButton->Delete();
    }
  if (this->RedSliceMenuButton)
    {
    this->RedSliceMenuButton->SetParent(NULL);
    this->RedSliceMenuButton->Delete();
    }
  if (this->YellowSliceMenuButton)
    {
    this->YellowSliceMenuButton->SetParent(NULL);
    this->YellowSliceMenuButton->Delete();
    }
  if (this->GreenSliceMenuButton)
    {
    this->GreenSliceMenuButton->SetParent(NULL);
    this->GreenSliceMenuButton->Delete();
    }
  if (this->SetLocatorAllPushButton)
    {
    this->SetLocatorAllPushButton->SetParent(NULL);
    this->SetLocatorAllPushButton->Delete();
    }
  if (this->SetUserAllPushButton)
    {
    this->SetUserAllPushButton->SetParent(NULL);
    this->SetUserAllPushButton->Delete();
    }
  if (this->FreezeSliceCheckButton)
    {
    this->FreezeSliceCheckButton->SetParent(NULL);
    this->FreezeSliceCheckButton->Delete();
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "AbdoNavGUI: " << this->GetClassName() << "\n";
  os << indent << "AbdoNavLogic: " << this->GetLogic() << "\n";
  os << indent << "AbdoNavNode: " << this->AbdoNavNode << "\n";
  os << indent << "DataCallbackCommand: " << this->DataCallbackCommand << "\n";
  os << indent << "TimerFlag: " << this->TimerFlag << "\n";
  os << indent << "TimerInterval: " << this->TimerInterval << "\n";
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::Init()
{
  // this is necessary so that an AbdoNavNode can be created from its class name,
  // e.g. when a previously saved scene (containing an AbdoNavNode) is (re-)loaded
  vtkMRMLAbdoNavNode* node = vtkMRMLAbdoNavNode::New();
  this->Logic->GetMRMLScene()->RegisterNodeClass(node);
  node->Delete();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::Enter()
{
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 50; // 50 ms
    ProcessTimerEvents();
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::Exit()
{
  // TODO: implement or delete!
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::AddGUIObservers()
{
  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // Set observers on slice views.
  //----------------------------------------------------------------
  vtkSlicerApplicationGUI* appGUI = this->GetApplicationGUI();

  appGUI->GetMainSliceGUI("Red")
    ->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand*)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI("Yellow")
    ->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand*)this->GUICallbackCommand);
  appGUI->GetMainSliceGUI("Green")
    ->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle()
    ->AddObserver(vtkCommand::LeftButtonPressEvent, (vtkCommand*)this->GUICallbackCommand);

  //----------------------------------------------------------------
  // Connection frame.
  //----------------------------------------------------------------
  this->ResetConnectionPushButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->ConfigureConnectionPushButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);

  //----------------------------------------------------------------
  // Registration frame.
  //----------------------------------------------------------------
  this->Point1RadioButton->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->Point2RadioButton->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);

  // must be called manually!
  this->AddMRMLObservers();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::RemoveGUIObservers()
{
  //----------------------------------------------------------------
  // Remove observers from slice views.
  //----------------------------------------------------------------
  vtkSlicerApplicationGUI* appGUI = this->GetApplicationGUI();

  if (appGUI && appGUI->GetMainSliceGUI("Red"))
    {
    appGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()
      ->GetInteractorStyle()->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (appGUI && appGUI->GetMainSliceGUI("Yellow"))
    {
    appGUI->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()
      ->GetInteractorStyle()->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (appGUI && appGUI->GetMainSliceGUI("Yellow"))
    {
    appGUI->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()
      ->GetInteractorStyle()->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  //----------------------------------------------------------------
  // Connection frame.
  //----------------------------------------------------------------
  if (this->ResetConnectionPushButton)
    {
    this->ResetConnectionPushButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->ConfigureConnectionPushButton)
    {
    this->ConfigureConnectionPushButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  //----------------------------------------------------------------
  // Registration frame.
  //----------------------------------------------------------------
  if (this->Point1RadioButton)
    {
    this->Point1RadioButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->Point2RadioButton)
    {
    this->Point2RadioButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  // must be called manually!
  this->RemoveLogicObservers();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::AddLogicObservers()
{
  this->RemoveLogicObservers();

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkAbdoNavLogic::StatusUpdateEvent, (vtkCommand*)this->LogicCallbackCommand);
    // TODO: fill in or delete (don't forget to delete RemoveLogicObservers())!
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::RemoveLogicObservers()
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent, (vtkCommand*)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::AddMRMLObservers()
{
  // TODO: implement RemoveMRMLObservers and process MRML events or delete!

  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::NewSceneEvent);
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);

  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::ProcessGUIEvents(vtkObject* caller, unsigned long event, void* callData)
{
  //----------------------------------------------------------------
  // Main slice views.
  //
  // If the user clicked in one of the slice views, transform xy
  // mouse coordinates into RAS coordinates.
  //----------------------------------------------------------------
  vtkSlicerInteractorStyle* style = vtkSlicerInteractorStyle::SafeDownCast(caller);
  if (style != NULL && event == vtkCommand::LeftButtonPressEvent)
    {
    // first, find out in which slice view the user clicked
    vtkSlicerSliceGUI* sliceGUI = this->GetApplicationGUI()->GetMainSliceGUI("Red");
    vtkRenderWindowInteractor* rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();

    int index = 0;
    while (style != rwi->GetInteractorStyle() && index < 2)
      {
      index++;
      if (index == 1)
        {
        sliceGUI = this->GetApplicationGUI()->GetMainSliceGUI("Yellow");
        }
      else
        {
        sliceGUI = this->GetApplicationGUI()->GetMainSliceGUI("Green");
        }
      rwi = sliceGUI->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();
      }

    // second, transform xy mouse coordinates into RAS coordinates
    int xyPos[2];
    rwi->GetLastEventPosition(xyPos);
    double xyVec[4] = {xyPos[0], xyPos[1], 0, 1};
    double rasVec[4];
    vtkMatrix4x4* matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
    matrix->MultiplyPoint(xyVec, rasVec);

    // third, display RAS coordinates in the GUI
    if (this->Point1RadioButton->GetSelectedState())
      {
      Point1REntry->SetValueAsDouble(rasVec[0]);
      Point1AEntry->SetValueAsDouble(rasVec[1]);
      Point1SEntry->SetValueAsDouble(rasVec[2]);
      }
    else if (this->Point2RadioButton->GetSelectedState())
      {
      Point2REntry->SetValueAsDouble(rasVec[0]);
      Point2AEntry->SetValueAsDouble(rasVec[1]);
      Point2SEntry->SetValueAsDouble(rasVec[2]);
      }

    // fourth (and last), update MRML node
    this->UpdateMRMLFromGUI();
    }

  //----------------------------------------------------------------
  // Connection frame.
  //----------------------------------------------------------------
  else if (this->ResetConnectionPushButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
    this->TrackerNodeSelectorWidget->SetSelected(NULL);
    this->TrackerNodeSelectorWidget->SetEnabled(true);
    this->TrackerComboBox->GetWidget()->SetValue("");
    this->TrackerComboBox->SetEnabled(true);
    this->ConfigureConnectionPushButton->SetEnabled(true);
    this->UpdateMRMLFromGUI();
    }
  else if (this->ConfigureConnectionPushButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
    bool updateMRML = false;
    vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->TrackerNodeSelectorWidget->GetSelected());

    // check tracker transform node selection
    if (tnode != NULL && tnode->GetDescription() != NULL)
      {
      // only update MRML node if transform node was created by OpenIGTLinkIF
      if (strcmp(tnode->GetDescription(), "Received by OpenIGTLink") == 0)
        {
        updateMRML = true;
        }
      }
    else
      {
      updateMRML = false;
      vtkKWMessageDialog::PopupMessage(this->GetApplication(),
                                       this->GetApplicationGUI()->GetMainSlicerWindow(),
                                       "AbdoNav",
                                       "No OpenIGTLinkIF tracker transform node selected!",
                                       vtkKWMessageDialog::ErrorIcon);
      }

    // check tracking system selection
    if (strcmp(this->TrackerComboBox->GetWidget()->GetValue(), "") != 0)
      {
      updateMRML = updateMRML && true;
      }
    else
      {
      updateMRML = false;
      vtkKWMessageDialog::PopupMessage(this->GetApplication(),
                                       this->GetApplicationGUI()->GetMainSlicerWindow(),
                                       "AbdoNav",
                                       "No tracking system selected!",
                                       vtkKWMessageDialog::ErrorIcon);
      }

    // update MRML node
    if (updateMRML == true)
      {
      std::cout << "updating MRML ..." << std::endl;
      this->TrackerNodeSelectorWidget->SetEnabled(false);
      this->TrackerComboBox->SetEnabled(false);
      this->ConfigureConnectionPushButton->SetEnabled(false);
      this->UpdateMRMLFromGUI();
      }
    }

  //----------------------------------------------------------------
  // Registration frame.
  //----------------------------------------------------------------
  else if (this->Point1RadioButton == vtkKWRadioButton::SafeDownCast(caller) && event == vtkKWRadioButton::SelectedStateChangedEvent)
    {
    // mimic radio button set behavior, i.e. only one radio button allowed to be selected at a time
    if (this->Point1RadioButton->GetSelectedState() && this->Point2RadioButton->GetSelectedState())
      {
      this->Point2RadioButton->SelectedStateOff();
      }
    }
  else if (this->Point2RadioButton == vtkKWRadioButton::SafeDownCast(caller) && event == vtkKWRadioButton::SelectedStateChangedEvent)
    {
    // mimic radio button set behavior, i.e. only one radio button allowed to be selected at a time
    if (this->Point1RadioButton->GetSelectedState() && this->Point2RadioButton->GetSelectedState())
      {
      this->Point1RadioButton->SelectedStateOff();
      }
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::ProcessLogicEvents(vtkObject* caller, unsigned long event, void* vtkNotUsed(callData))
{
  if (this->GetLogic() == vtkAbdoNavLogic::SafeDownCast(caller))
    {
    if (event == vtkAbdoNavLogic::StatusUpdateEvent)
      {
      // TODO: fill in or delete!
      }
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData)
{
  // if an AbdoNavNode was added, then update the GUI; an AbdoNavNode is only added
  // when the user loads a previously saved scene that contains an AbdoNavNode
  if (event == vtkMRMLScene::NodeAddedEvent)
    {
    vtkMRMLAbdoNavNode* node = vtkMRMLAbdoNavNode::SafeDownCast((vtkObject*)callData);
    if (node && node->IsA("vtkMRMLAbdoNavNode"))
      {
      // a new AbdoNavNode was created and added
      if (this->AbdoNavNode == NULL)
        {
        // set and observe the new node in Logic
        this->Logic->SetAndObserveAbdoNavNode(node);
        // set and observe the new node in GUI
        vtkSetAndObserveMRMLNodeMacro(this->AbdoNavNode, node);
        }
      this->UpdateGUIFromMRML();
      }
    }
  // existing node has been modified
  else if (event == vtkCommand::ModifiedEvent)
    {
    vtkMRMLAbdoNavNode* node = vtkMRMLAbdoNavNode::SafeDownCast(caller);
    if (node)
      {
      this->UpdateGUIFromMRML();
      }
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    // TODO: fill in!

    // calls ProcessTimerEvents() at regular intervals (specified by TimerInterval)
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), this->TimerInterval, this, "ProcessTimerEvents");
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::DataCallback(vtkObject* vtkNotUsed(caller), unsigned long vtkNotUsed(eventid), void* clientData, void* vtkNotUsed(callData))
{
  // TODO: use (UpdateAll() isn't implemented) or delete!

  vtkAbdoNavGUI* self = reinterpret_cast<vtkAbdoNavGUI*>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkAbdoNavGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::UpdateAll()
{
  // TODO: implement or delete!
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::UpdateMRMLFromGUI()
{
  vtkMRMLAbdoNavNode* node = this->AbdoNavNode;
  if (node == NULL)
    {
    // no AbdoNav node present yet, thus create a new one
    node = vtkMRMLAbdoNavNode::New();
    // add the new node to this MRML scene but don't notify:
    // this way, it is known that a node added event is only
    // invoked when the user loads a previously saved scene
    // containing an AbdoNavNode
    this->GetMRMLScene()->AddNodeNoNotify(node);
    // set and observe the new node in Logic
    this->Logic->SetAndObserveAbdoNavNode(node);
    // set and observe the new node in GUI
    vtkSetAndObserveMRMLNodeMacro(this->AbdoNavNode, node);
    // TODO: should this be moved to vtkAbdoNavGUI::~vtkAbdoNavGUI() ?
    node->Delete();
   }

  // save old node parameters for undo mechanism
  this->GetLogic()->GetMRMLScene()->SaveStateForUndo(node);

  // set new node parameters from GUI widgets:
  // make sure that only ONE modified event is invoked (if
  // at all) instead of one modified event per changed value
  int modifiedFlag = node->StartModify();
  vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->TrackerNodeSelectorWidget->GetSelected());
  if (tnode != NULL)
    {
    node->SetTrackerTransformNodeID(tnode->GetID());
    }
  node->SetTrackingSystemUsed(this->TrackerComboBox->GetWidget()->GetValue());
  node->SetGuidanceNeedleTip(Point1REntry->GetValueAsDouble(),
                             Point1AEntry->GetValueAsDouble(),
                             Point1SEntry->GetValueAsDouble());
  node->SetGuidanceNeedleSecond(Point2REntry->GetValueAsDouble(),
                                Point2AEntry->GetValueAsDouble(),
                                Point2SEntry->GetValueAsDouble());
  node->EndModify(modifiedFlag);
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::UpdateGUIFromMRML()
{
  vtkMRMLAbdoNavNode* node = this->AbdoNavNode;
  if (node != NULL)
    {
    // set GUI widgets from AbdoNav parameter node
    vtkMRMLNode* tnode = this->GetMRMLScene()->GetNodeByID(node->GetTrackerTransformNodeID());
    this->TrackerNodeSelectorWidget->SetSelected(tnode);
    this->TrackerComboBox->GetWidget()->SetValue(node->GetTrackingSystemUsed());

    double* guidanceTip = node->GetGuidanceNeedleTip();
    this->Point1REntry->SetValueAsDouble(guidanceTip[0]);
    this->Point1AEntry->SetValueAsDouble(guidanceTip[1]);
    this->Point1SEntry->SetValueAsDouble(guidanceTip[2]);

    double* guidanceSecond = node->GetGuidanceNeedleSecond();
    this->Point2REntry->SetValueAsDouble(guidanceSecond[0]);
    this->Point2AEntry->SetValueAsDouble(guidanceSecond[1]);
    this->Point2SEntry->SetValueAsDouble(guidanceSecond[2]);
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::BuildGUI()
{
  // create a page
  this->UIPanel->AddPage("AbdoNav", "AbdoNav", NULL);

  // build the different GUI frames
  BuildGUIHelpFrame();
  BuildGUIConnectionFrame();
  BuildGUIRegistrationFrame();
  BuildGUINavigationFrame();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::BuildGUIHelpFrame()
{
  // help text
  const char* help =
    "The **AbdoNav** module is tailored to abdominal cryosurgeries for liver and kidney tumor treatment. "
    "The module helps you to set up a connection to the tracking device via the OpenIGTLinkIF module, to "
    "plan cryoprobe insertion using the Measurements module, to register tracking and scanner coordinate "
    "systems and visualizes the current cryoprobe to be inserted."
    "\n"
    "See <a>http://www.slicer.org/slicerWiki/index.php/Modules:AbdoNav-Documentation-3.6</a> for details "
    "about the module.";
  // about text
  const char* about =
    "The **AbdoNav** module was contributed by Christoph Ammann (Karlsruhe Institute of Technology, KIT) "
    "and Nobuhiko Hata, PhD (Surgical Navigation and Robotics Laboratory, SNR).";

  vtkKWWidget* page = this->UIPanel->GetPageWidget("AbdoNav");
  this->BuildHelpAndAboutFrame(page, help, about);
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::BuildGUIConnectionFrame()
{
  vtkKWWidget* page = this->UIPanel->GetPageWidget("AbdoNav");

  // create a collapsible connection frame
  vtkSlicerModuleCollapsibleFrame* connectionFrame = vtkSlicerModuleCollapsibleFrame::New();
  connectionFrame->SetParent(page);
  connectionFrame->Create();
  connectionFrame->SetLabelText("Connection");
  connectionFrame->CollapseFrame();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", connectionFrame->GetWidgetName(), page->GetWidgetName());

  // create a labelled frame to to hold the tracker transform node and the tracking system combo box
  vtkKWFrameWithLabel* trackerFrame = vtkKWFrameWithLabel::New();
  trackerFrame->SetParent(connectionFrame->GetFrame());
  trackerFrame->SetLabelText("Specify tracking information");
  trackerFrame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", trackerFrame->GetWidgetName());

  // create a tracker transform node selector widget
  this->TrackerNodeSelectorWidget = vtkSlicerNodeSelectorWidget::New();
  this->TrackerNodeSelectorWidget->SetParent(trackerFrame->GetFrame());
  this->TrackerNodeSelectorWidget->Create();
  this->TrackerNodeSelectorWidget->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, "LinearTransform");
  // explicitly indicate that the user is not allowed to create a new linear transform node
  this->TrackerNodeSelectorWidget->SetNewNodeEnabled(0);
  this->TrackerNodeSelectorWidget->SetDefaultEnabled(0);
  this->TrackerNodeSelectorWidget->SetMRMLScene(this->GetMRMLScene());
  this->TrackerNodeSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->TrackerNodeSelectorWidget->SetLabelText("Tracker transform node:\t\t");
  this->TrackerNodeSelectorWidget->SetBalloonHelpString("Select the transform node created by OpenIGTLinkIF that holds the tracking data of the current cryoprobe relative to the guidance needle.");

  // add tracker transform node selector widget
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", this->TrackerNodeSelectorWidget->GetWidgetName());

  // create a combo box to specify the tracking system being used
  this->TrackerComboBox = vtkKWComboBoxWithLabel::New();
  this->TrackerComboBox->SetParent(trackerFrame->GetFrame());
  this->TrackerComboBox->Create();
  this->TrackerComboBox->SetLabelText("Tracking system used:\t\t");
  this->TrackerComboBox->GetWidget()->ReadOnlyOn();
  this->TrackerComboBox->GetWidget()->AddValue("NDI Aurora");
  this->TrackerComboBox->GetWidget()->AddValue("NDI Polaris Vicra");
  this->TrackerComboBox->SetBalloonHelpString("Select the tracking system being used in order to compensate for different coordinate system definitions.");

  // add tracking system combo box
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", this->TrackerComboBox->GetWidgetName());

  // create a reset button
  this->ResetConnectionPushButton = vtkKWPushButton::New();
  this->ResetConnectionPushButton->SetParent(connectionFrame->GetFrame());
  this->ResetConnectionPushButton->Create();
  this->ResetConnectionPushButton->SetText("Reset Connection");
  this->ResetConnectionPushButton->SetBalloonHelpString("Reset currently selected tracker transform node.");

  // add reset button
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", this->ResetConnectionPushButton->GetWidgetName());

  // create a configure button
  this->ConfigureConnectionPushButton = vtkKWPushButton::New();
  this->ConfigureConnectionPushButton->SetParent(connectionFrame->GetFrame());
  this->ConfigureConnectionPushButton->Create();
  this->ConfigureConnectionPushButton->SetText("Configure Connection");
  this->ConfigureConnectionPushButton->SetBalloonHelpString("Set currently selected tracker transform node.");

  // add configure button
  this->Script("pack %s -side right -anchor ne -padx 2 -pady 2", this->ConfigureConnectionPushButton->GetWidgetName());

  // clean up
  connectionFrame->Delete();
  trackerFrame->Delete();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::BuildGUIRegistrationFrame()
{
  vtkKWWidget* page = this->UIPanel->GetPageWidget("AbdoNav");

  // create a collapsible registration frame
  vtkSlicerModuleCollapsibleFrame* registrationFrame = vtkSlicerModuleCollapsibleFrame::New();
  registrationFrame->SetParent(page);
  registrationFrame->Create();
  registrationFrame->SetLabelText("Registration");
  registrationFrame->CollapseFrame();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", registrationFrame->GetWidgetName(), page->GetWidgetName());

  // create a labelled frame to to hold the GUI elements of both, the guidance needle tip and the the second point on the guidance needle
  vtkKWFrameWithLabel* guidanceNeedleFrame = vtkKWFrameWithLabel::New();
  guidanceNeedleFrame->SetParent(registrationFrame->GetFrame());
  guidanceNeedleFrame->SetLabelText("Identify guidance needle");
  guidanceNeedleFrame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", guidanceNeedleFrame->GetWidgetName());

  // create a frame to hold the guidance needle tip radio button and tip position entries
  vtkKWFrame* point1Frame = vtkKWFrame::New();
  point1Frame->SetParent(guidanceNeedleFrame->GetFrame());
  point1Frame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", point1Frame->GetWidgetName());
  // create a radio button for the guidance needle tip
  this->Point1RadioButton = vtkKWRadioButton::New();
  this->Point1RadioButton->SetParent(point1Frame);
  this->Point1RadioButton->Create();
  this->Point1RadioButton->SetText("Identify guidance needle tip:\t\t");
  // create an entry for the R coordinate of the guidance needle tip
  this->Point1REntry = vtkKWEntry::New();
  this->Point1REntry->SetParent(point1Frame);
  this->Point1REntry->Create();
  this->Point1REntry->SetWidth(8);
  this->Point1REntry->SetRestrictValueToDouble();
  this->Point1REntry->SetBalloonHelpString("Guidance needle tip, R coordinate.");
  // create an entry for the A coordinate of the guidance needle tip
  this->Point1AEntry = vtkKWEntry::New();
  this->Point1AEntry->SetParent(point1Frame);
  this->Point1AEntry->Create();
  this->Point1AEntry->SetWidth(8);
  this->Point1AEntry->SetRestrictValueToDouble();
  this->Point1AEntry->SetBalloonHelpString("Guidance needle tip, A coordinate.");
  // create an entry for the S coordinate of the guidance needle tip
  this->Point1SEntry = vtkKWEntry::New();
  this->Point1SEntry->SetParent(point1Frame);
  this->Point1SEntry->Create();
  this->Point1SEntry->SetWidth(8);
  this->Point1SEntry->SetRestrictValueToDouble();
  this->Point1SEntry->SetBalloonHelpString("Guidance needle tip, S coordinate.");
  // add guidance needle radio button
  this->Script ("pack %s -side left -anchor nw  -padx 2 -pady 2", this->Point1RadioButton->GetWidgetName());
  // add guidance needle tip position entries
  this->Script ("pack %s %s %s -side right -anchor ne -padx 2 -pady 2",
                 this->Point1SEntry->GetWidgetName(),
                 this->Point1AEntry->GetWidgetName(),
                 this->Point1REntry->GetWidgetName());

  // create a frame to hold the radio button and position entries for the second point on the guidance needle
  vtkKWFrame* point2Frame = vtkKWFrame::New();
  point2Frame->SetParent(guidanceNeedleFrame->GetFrame());
  point2Frame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", point2Frame->GetWidgetName());
  // create a radio button for the second point on the guidance needle
  this->Point2RadioButton = vtkKWRadioButton::New();
  this->Point2RadioButton->SetParent(point2Frame);
  this->Point2RadioButton->Create();
  this->Point2RadioButton->SetText("Identify second point on needle:\t");
  // create an entry for the R coordinate of the second point
  this->Point2REntry = vtkKWEntry::New();
  this->Point2REntry->SetParent(point2Frame);
  this->Point2REntry->Create();
  this->Point2REntry->SetWidth(8);
  this->Point2REntry->SetRestrictValueToDouble();
  this->Point2REntry->SetBalloonHelpString("Second point on guidance needle, R coordinate.");
  // create an entry for the A coordinate of the second point
  this->Point2AEntry = vtkKWEntry::New();
  this->Point2AEntry->SetParent(point2Frame);
  this->Point2AEntry->Create();
  this->Point2AEntry->SetWidth(8);
  this->Point2AEntry->SetRestrictValueToDouble();
  this->Point2AEntry->SetBalloonHelpString("Second point on guidance needle, A coordinate.");
  // create an entry for the S coordinate of the second point
  this->Point2SEntry = vtkKWEntry::New();
  this->Point2SEntry->SetParent(point2Frame);
  this->Point2SEntry->Create();
  this->Point2SEntry->SetWidth(8);
  this->Point2SEntry->SetRestrictValueToDouble();
  this->Point2SEntry->SetBalloonHelpString("Second point on guidance needle, S coordinate.");
  // add second point radio button
  this->Script ("pack %s -side left -anchor nw  -padx 2 -pady 2", this->Point2RadioButton->GetWidgetName());
  // add second point position entries
  this->Script ("pack %s %s %s -side right -anchor ne -padx 2 -pady 2",
                 this->Point2SEntry->GetWidgetName(),
                 this->Point2AEntry->GetWidgetName(),
                 this->Point2REntry->GetWidgetName());

  // create a reset registration button
  this->ResetRegistrationPushButton = vtkKWPushButton::New();
  this->ResetRegistrationPushButton->SetParent(registrationFrame->GetFrame());
  this->ResetRegistrationPushButton->Create();
  this->ResetRegistrationPushButton->SetText("Reset Registration");
  this->ResetRegistrationPushButton->SetBalloonHelpString("Redo identification of guidance needle.");

  // add reset registration button
  this->Script("pack %s -side left -anchor nw -padx 2 -pady 2", this->ResetRegistrationPushButton->GetWidgetName());

  // create a perform registration button
  this->PerformRegistrationPushButton = vtkKWPushButton::New();
  this->PerformRegistrationPushButton->SetParent(registrationFrame->GetFrame());
  this->PerformRegistrationPushButton->Create();
  this->PerformRegistrationPushButton->SetText("Perform Registration");
  this->PerformRegistrationPushButton->SetBalloonHelpString("Perform registration based on current identification of guidance needle.");

  // add perform registration button
  this->Script("pack %s -side right -anchor ne -padx 2 -pady 2", this->PerformRegistrationPushButton->GetWidgetName());

  // clean up
  registrationFrame->Delete();
  guidanceNeedleFrame->Delete();
  point1Frame->Delete();
  point2Frame->Delete();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::BuildGUINavigationFrame()
{
  vtkKWWidget* page = this->UIPanel->GetPageWidget("AbdoNav");

  // create a collapsible navigation frame
  vtkSlicerModuleCollapsibleFrame* navigationFrame = vtkSlicerModuleCollapsibleFrame::New();
  navigationFrame->SetParent(page);
  navigationFrame->Create();
  navigationFrame->SetLabelText("Navigation");
  navigationFrame->CollapseFrame();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", navigationFrame->GetWidgetName(), page->GetWidgetName());

  // create a labelled frame for the locator display options
  vtkKWFrameWithLabel* locatorDisplayFrame = vtkKWFrameWithLabel::New();
  locatorDisplayFrame->SetParent(navigationFrame->GetFrame());
  locatorDisplayFrame->SetLabelText("Locator Display Options");
  locatorDisplayFrame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", locatorDisplayFrame->GetWidgetName());

  // create a frame to hold the locator mode check buttons
  vtkKWFrame* locatorModeFrame = vtkKWFrame::New();
  locatorModeFrame->SetParent(locatorDisplayFrame->GetFrame());
  locatorModeFrame->Create();
  this->Script("pack %s -side top -anchor c -padx 2 -pady 2", locatorModeFrame->GetWidgetName());

  // create a check button to show/hide the locator model
  this->ShowLocatorCheckButton = vtkKWCheckButton::New();
  this->ShowLocatorCheckButton->SetParent(locatorModeFrame);
  this->ShowLocatorCheckButton->Create();
  this->ShowLocatorCheckButton->SelectedStateOff();
  this->ShowLocatorCheckButton->SetText("Show Locator");

  // create a check button to freeze/unfreeze the locator model
  this->FreezeLocatorCheckButton = vtkKWCheckButton::New();
  this->FreezeLocatorCheckButton->SetParent(locatorModeFrame);
  this->FreezeLocatorCheckButton->Create();
  this->FreezeLocatorCheckButton->SelectedStateOff();
  this->FreezeLocatorCheckButton->SetText("Freeze Locator");

  // create a check button to show/hide a crosshair in the slice views corresponding to the locator's tip position
  this->ShowCrosshairCheckButton = vtkKWCheckButton::New();
  this->ShowCrosshairCheckButton->SetParent(locatorModeFrame);
  this->ShowCrosshairCheckButton->Create();
  this->ShowCrosshairCheckButton->SelectedStateOff();
  this->ShowCrosshairCheckButton->SetText("Show Crosshair");

  // add show locator, freeze locator and show crosshair check buttons
  this->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2",
                ShowLocatorCheckButton->GetWidgetName(),
                FreezeLocatorCheckButton->GetWidgetName(),
                ShowCrosshairCheckButton->GetWidgetName());

  // create a labelled frame for the slice driver options
  vtkKWFrameWithLabel* sliceDriverFrame = vtkKWFrameWithLabel::New();
  sliceDriverFrame->SetParent(navigationFrame->GetFrame());
  sliceDriverFrame->SetLabelText("Slice Driver");
  sliceDriverFrame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", sliceDriverFrame->GetWidgetName());

  // create a frame to hold the slice view menu buttons
  vtkKWFrame* sliceFrame = vtkKWFrame::New();
  sliceFrame->SetParent(sliceDriverFrame->GetFrame());
  sliceFrame->Create();
  this->Script("pack %s -side top -anchor c -padx 2 -pady 2", sliceFrame->GetWidgetName());

  // get slice view colors red, green and yellow
  vtkSlicerColor* color = ((vtkSlicerApplication*)this->GetApplication())->GetSlicerTheme()->GetSlicerColors();
  // create a red slice view (axial) menu button
  this->RedSliceMenuButton = vtkKWMenuButton::New();
  this->RedSliceMenuButton->SetParent(sliceFrame);
  this->RedSliceMenuButton->Create();
  this->RedSliceMenuButton->SetWidth(13);
  this->RedSliceMenuButton->SetBackgroundColor(color->SliceGUIRed);
  this->RedSliceMenuButton->SetActiveBackgroundColor(color->SliceGUIRed);
  this->RedSliceMenuButton->GetMenu()->AddRadioButton("User");
  this->RedSliceMenuButton->GetMenu()->AddRadioButton("Locator");
  this->RedSliceMenuButton->SetValue("User");
  // create a yellow slice view (sagittal) menu button
  this->YellowSliceMenuButton = vtkKWMenuButton::New();
  this->YellowSliceMenuButton->SetParent(sliceFrame);
  this->YellowSliceMenuButton->Create();
  this->YellowSliceMenuButton->SetWidth(13);
  this->YellowSliceMenuButton->SetBackgroundColor(color->SliceGUIYellow);
  this->YellowSliceMenuButton->SetActiveBackgroundColor(color->SliceGUIYellow);
  this->YellowSliceMenuButton->GetMenu()->AddRadioButton("User");
  this->YellowSliceMenuButton->GetMenu()->AddRadioButton("Locator");
  this->YellowSliceMenuButton->SetValue("User");
  // create a green slice view (coronal) menu button
  this->GreenSliceMenuButton = vtkKWMenuButton::New();
  this->GreenSliceMenuButton->SetParent(sliceFrame);
  this->GreenSliceMenuButton->Create();
  this->GreenSliceMenuButton->SetWidth(13);
  this->GreenSliceMenuButton->SetBackgroundColor(color->SliceGUIGreen);
  this->GreenSliceMenuButton->SetActiveBackgroundColor(color->SliceGUIGreen);
  this->GreenSliceMenuButton->GetMenu()->AddRadioButton("User");
  this->GreenSliceMenuButton->GetMenu()->AddRadioButton("Locator");
  this->GreenSliceMenuButton->SetValue("User");

  // add red, green and yellow menu buttons
  this->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2",
                RedSliceMenuButton->GetWidgetName(),
                YellowSliceMenuButton->GetWidgetName(),
                GreenSliceMenuButton->GetWidgetName());

  // create a frame to hold the buttons for the different slice view modes
  vtkKWFrame* sliceModeFrame = vtkKWFrame::New();
  sliceModeFrame->SetParent(sliceDriverFrame->GetFrame());
  sliceModeFrame->Create();
  this->Script("pack %s -side top -anchor c -padx 2 -pady 2", sliceModeFrame->GetWidgetName());

  // create a drive all slice views by locator push button
  this->SetLocatorAllPushButton = vtkKWPushButton::New();
  this->SetLocatorAllPushButton->SetParent(sliceModeFrame);
  this->SetLocatorAllPushButton->Create();
  this->SetLocatorAllPushButton->SetText("Locator All");
  this->SetLocatorAllPushButton->SetWidth(12);
  // create a drive all slice views by user push button
  this->SetUserAllPushButton = vtkKWPushButton::New();
  this->SetUserAllPushButton->SetParent(sliceModeFrame);
  this->SetUserAllPushButton->Create();
  this->SetUserAllPushButton->SetText("User All");
  this->SetUserAllPushButton->SetWidth(12);
  // create a freeze slice views check button
  this->FreezeSliceCheckButton = vtkKWCheckButton::New();
  this->FreezeSliceCheckButton->SetParent(sliceModeFrame);
  this->FreezeSliceCheckButton->Create();
  this->FreezeSliceCheckButton->SetText("Freeze");
  this->FreezeSliceCheckButton->SelectedStateOff();

  // add drive all slice views by locator or user push buttons and freeze all slice views check button
  this->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2",
                SetLocatorAllPushButton->GetWidgetName(),
                SetUserAllPushButton->GetWidgetName(),
                FreezeSliceCheckButton->GetWidgetName());

  // clean up
  navigationFrame->Delete();
  locatorDisplayFrame->Delete();
  locatorModeFrame->Delete();
  sliceDriverFrame->Delete();
  sliceFrame->Delete();
  sliceModeFrame->Delete();
}
