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
  this->Logic = NULL;
  this->AbdoNavNode = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData(reinterpret_cast<void*> (this));
  this->DataCallbackCommand->SetCallback(vtkAbdoNavGUI::DataCallback);
  this->TimerFlag = 0;

  //----------------------------------------------------------------
  // Connection frame.
  this->TrackerNodeSelectorWidget = NULL;
  this->TrackerComboxBox = NULL;
  this->ResetConnectionPushButton = NULL;
  this->ConfigureConnectionPushButton = NULL;

  //----------------------------------------------------------------
  // Registration frame.
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
  // if Logic is NULL, the class was only instatiated but never used,
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
  if (this->TrackerNodeSelectorWidget)
    {
    this->TrackerNodeSelectorWidget->SetParent(NULL);
    this->TrackerNodeSelectorWidget->Delete();
    }
  if (this->TrackerComboxBox)
    {
    this->TrackerComboxBox->SetParent(NULL);
    this->TrackerComboxBox->Delete();
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
  //----------------------------------------------------------------
  // Print all publicly accessible instance variables.
  //----------------------------------------------------------------

  this->vtkObject::PrintSelf(os, indent);
  os << indent << "AbdoNavGUI: " << this->GetClassName() << "\n";
  os << indent << "Logic: " << this->GetLogic() << "\n";
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::Init()
{
  //----------------------------------------------------------------
  // Not implemented.
  //----------------------------------------------------------------
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::Enter()
{
  //----------------------------------------------------------------
  // Define behavior at module startup.
  //----------------------------------------------------------------

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
  //----------------------------------------------------------------
  // Not implemented.
  //----------------------------------------------------------------
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::AddGUIObservers()
{
  //----------------------------------------------------------------
  // Set observers on widgets and GUI classes.
  //----------------------------------------------------------------

  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // Set observers on slice views.
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
  this->ResetConnectionPushButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->ConfigureConnectionPushButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand*)this->GUICallbackCommand);

  //----------------------------------------------------------------
  // Registration frame.
  this->Point1RadioButton->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);
  this->Point2RadioButton->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand*)this->GUICallbackCommand);

  this->AddLogicObservers();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::RemoveGUIObservers()
{
  //----------------------------------------------------------------
  // Remove GUI observers.
  //----------------------------------------------------------------

  //----------------------------------------------------------------
  // Remove observers from slice views.
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
  if (this->Point1RadioButton)
    {
    this->Point1RadioButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }
  if (this->Point2RadioButton)
    {
    this->Point2RadioButton->RemoveObserver((vtkCommand*)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::AddLogicObservers()
{
  //----------------------------------------------------------------
  // Set observers on AbdoNavLogic's different event types.
  //----------------------------------------------------------------

  this->RemoveLogicObservers();

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkAbdoNavLogic::StatusUpdateEvent, (vtkCommand*)this->LogicCallbackCommand);
    // fill in
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::RemoveLogicObservers()
{
  //----------------------------------------------------------------
  // Remove logic observers.
  //----------------------------------------------------------------

  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent, (vtkCommand*)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::AddMRMLObservers()
{
  //----------------------------------------------------------------
  // Set observers on MRML nodes and the scene.
  //----------------------------------------------------------------

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
  // React to GUI events.
  //----------------------------------------------------------------

  // react to mouse events observed in one of the slice views
  vtkSlicerInteractorStyle* style = vtkSlicerInteractorStyle::SafeDownCast(caller);
  if (style != NULL && event == vtkCommand::LeftButtonPressEvent)
    {
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

    int xyPos[2];
    rwi->GetLastEventPosition(xyPos);
    double xyVec[4] = {xyPos[0], xyPos[1], 0, 1};
    double rasVec[4];
    vtkMatrix4x4* matrix = sliceGUI->GetLogic()->GetSliceNode()->GetXYToRAS();
    matrix->MultiplyPoint(xyVec, rasVec);

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

    return;
    }

  //----------------------------------------------------------------
  // Connection frame.
  else if (this->ResetConnectionPushButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
    this->TrackerNodeSelectorWidget->SetSelected(NULL);
    this->TrackerNodeSelectorWidget->SetEnabled(true);
    this->TrackerComboxBox->GetWidget()->SetValue("");
    this->TrackerComboxBox->SetEnabled(true);
    this->UpdateMRMLFromGUI();
    }
  else if (this->ConfigureConnectionPushButton == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
    this->UpdateMRMLFromGUI();
    }

  //----------------------------------------------------------------
  // Registration frame.
  else if (this->Point1RadioButton == vtkKWRadioButton::SafeDownCast(caller) && event == vtkKWRadioButton::SelectedStateChangedEvent)
    {
    // only one radio button allowed to be selected at a time
    if (this->Point1RadioButton->GetSelectedState() && this->Point2RadioButton->GetSelectedState())
      {
      this->Point2RadioButton->SelectedStateOff();
      }
    }
  else if (this->Point2RadioButton == vtkKWRadioButton::SafeDownCast(caller) && event == vtkKWRadioButton::SelectedStateChangedEvent)
    {
    // only one radio button allowed to be selected at a time
    if (this->Point1RadioButton->GetSelectedState() && this->Point2RadioButton->GetSelectedState())
      {
      this->Point1RadioButton->SelectedStateOff();
      }
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::ProcessLogicEvents(vtkObject* caller, unsigned long event, void* vtkNotUsed(callData))
{
  //----------------------------------------------------------------
  // React to logic events.
  //----------------------------------------------------------------

  if (this->GetLogic() == vtkAbdoNavLogic::SafeDownCast(caller))
    {
    if (event == vtkAbdoNavLogic::StatusUpdateEvent)
      {
      // fill in
      }
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::ProcessMRMLEvents(vtkObject* caller, unsigned long event, void* callData)
{
  //----------------------------------------------------------------
  // React to MRML events.
  //----------------------------------------------------------------

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    // fill in
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::ProcessTimerEvents()
{
  //----------------------------------------------------------------
  // React to timer events.
  //----------------------------------------------------------------

  if (this->TimerFlag)
    {
    // fill in

    //std::cout << "TimerFlag:     " << this->TimerFlag     << std::endl;
    //std::cout << "TimerInterval: " << this->TimerInterval << std::endl;
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), this->TimerInterval, this, "ProcessTimerEvents");
    }
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::DataCallback(vtkObject* vtkNotUsed(caller), unsigned long vtkNotUsed(eventid), void* clientData, void* vtkNotUsed(callData))
{
  //----------------------------------------------------------------
  // Not used (UpdateAll() isn't implemented).
  //----------------------------------------------------------------

  vtkAbdoNavGUI* self = reinterpret_cast<vtkAbdoNavGUI*>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkAbdoNavGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::UpdateAll()
{
  //----------------------------------------------------------------
  // Not implemented.
  //----------------------------------------------------------------
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::UpdateMRMLFromGUI()
{
  vtkMRMLAbdoNavNode* node = this->AbdoNavNode;
  if (node == NULL)
    {
    // no AbdoNav node present yet, thus create a new one
    node = vtkMRMLAbdoNavNode::New();
    this->GetMRMLScene()->AddNode(node);
    // set an observe new node in Logic
    this->Logic->SetAndObserveAbdoNavNode(node);
    vtkSetAndObserveMRMLNodeMacro(this->AbdoNavNode, node);
    // TODO: should this be moved to vtkAbdoNavGUI::~vtkAbdoNavGUI() ?
    node->Delete();
   }

  // save node parameters for Undo
  this->GetLogic()->GetMRMLScene()->SaveStateForUndo(node);

  // set node parameters from GUI widgets
  // TODO: fill in
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::UpdateGUIFromMRML()
{
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::BuildGUI()
{
  //----------------------------------------------------------------
  // Build the GUI.
  //----------------------------------------------------------------

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
  //----------------------------------------------------------------
  // Build the GUI's help frame.
  //----------------------------------------------------------------

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
  //----------------------------------------------------------------
  // Build the GUI's connection frame.
  //----------------------------------------------------------------

  vtkKWWidget* page = this->UIPanel->GetPageWidget("AbdoNav");

  // create a collapsible connection frame
  vtkSlicerModuleCollapsibleFrame* connectionFrame = vtkSlicerModuleCollapsibleFrame::New();
  connectionFrame->SetParent(page);
  connectionFrame->Create();
  connectionFrame->SetLabelText("Connection");
  connectionFrame->CollapseFrame();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", connectionFrame->GetWidgetName(), page->GetWidgetName());

  // create a labelled frame to to hold the tracker transform node and the tracking system combo box
  vtkKWFrameWithLabel* TrackerFrame = vtkKWFrameWithLabel::New();
  TrackerFrame->SetParent(connectionFrame->GetFrame());
  TrackerFrame->SetLabelText("Specify tracking information");
  TrackerFrame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", TrackerFrame->GetWidgetName());

  // create a tracker transform node selector widget
  this->TrackerNodeSelectorWidget = vtkSlicerNodeSelectorWidget::New();
  this->TrackerNodeSelectorWidget->SetParent(TrackerFrame->GetFrame());
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
  this->TrackerComboxBox = vtkKWComboBoxWithLabel::New();
  this->TrackerComboxBox->SetParent(TrackerFrame->GetFrame());
  this->TrackerComboxBox->Create();
  this->TrackerComboxBox->SetLabelText("Tracking system used:\t\t");
  this->TrackerComboxBox->GetWidget()->ReadOnlyOn();
  this->TrackerComboxBox->GetWidget()->AddValue("NDI Aurora");
  this->TrackerComboxBox->GetWidget()->AddValue("NDI Polaris Vicra");
  this->TrackerComboxBox->SetBalloonHelpString("Select the tracking system being used in order to compensate for different coordinate system definitions.");

  // add tracking system combo box
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", this->TrackerComboxBox->GetWidgetName());

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
  TrackerFrame->Delete();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::BuildGUIRegistrationFrame()
{
  //----------------------------------------------------------------
  // Build the GUI's registration frame.
  //----------------------------------------------------------------

  vtkKWWidget* page = this->UIPanel->GetPageWidget("AbdoNav");

  // create a collapsible registration frame
  vtkSlicerModuleCollapsibleFrame* registrationFrame = vtkSlicerModuleCollapsibleFrame::New();
  registrationFrame->SetParent(page);
  registrationFrame->Create();
  registrationFrame->SetLabelText("Registration");
  registrationFrame->CollapseFrame();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", registrationFrame->GetWidgetName(), page->GetWidgetName());

  // create a labelled frame to to hold the GUI elements of both, the guidance needle tip and the the second point on the guidance needle
  vtkKWFrameWithLabel* GuidanceNeedleFrame = vtkKWFrameWithLabel::New();
  GuidanceNeedleFrame->SetParent(registrationFrame->GetFrame());
  GuidanceNeedleFrame->SetLabelText("Identify guidance needle");
  GuidanceNeedleFrame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", GuidanceNeedleFrame->GetWidgetName());

  // create a frame to hold the guidance needle tip radio button and tip position entries
  vtkKWFrame* Point1Frame = vtkKWFrame::New();
  Point1Frame->SetParent(GuidanceNeedleFrame->GetFrame());
  Point1Frame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", Point1Frame->GetWidgetName());
  // create a radio button for the guidance needle tip
  this->Point1RadioButton = vtkKWRadioButton::New();
  this->Point1RadioButton->SetParent(Point1Frame);
  this->Point1RadioButton->Create();
  this->Point1RadioButton->SetText("Identify guidance needle tip:\t\t");
  // create an entry for the R coordinate of the guidance needle tip
  this->Point1REntry = vtkKWEntry::New();
  this->Point1REntry->SetParent(Point1Frame);
  this->Point1REntry->Create();
  this->Point1REntry->SetWidth(8);
  this->Point1REntry->SetRestrictValueToDouble();
  this->Point1REntry->SetBalloonHelpString("Guidance needle tip, R coordinate.");
  // create an entry for the A coordinate of the guidance needle tip
  this->Point1AEntry = vtkKWEntry::New();
  this->Point1AEntry->SetParent(Point1Frame);
  this->Point1AEntry->Create();
  this->Point1AEntry->SetWidth(8);
  this->Point1AEntry->SetRestrictValueToDouble();
  this->Point1AEntry->SetBalloonHelpString("Guidance needle tip, A coordinate.");
  // create an entry for the S coordinate of the guidance needle tip
  this->Point1SEntry = vtkKWEntry::New();
  this->Point1SEntry->SetParent(Point1Frame);
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
  vtkKWFrame* Point2Frame = vtkKWFrame::New();
  Point2Frame->SetParent(GuidanceNeedleFrame->GetFrame());
  Point2Frame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", Point2Frame->GetWidgetName());
  // create a radio button for the second point on the guidance needle
  this->Point2RadioButton = vtkKWRadioButton::New();
  this->Point2RadioButton->SetParent(Point2Frame);
  this->Point2RadioButton->Create();
  this->Point2RadioButton->SetText("Identify second point on needle:\t");
  // create an entry for the R coordinate of the second point
  this->Point2REntry = vtkKWEntry::New();
  this->Point2REntry->SetParent(Point2Frame);
  this->Point2REntry->Create();
  this->Point2REntry->SetWidth(8);
  this->Point2REntry->SetRestrictValueToDouble();
  this->Point2REntry->SetBalloonHelpString("Second point on guidance needle, R coordinate.");
  // create an entry for the A coordinate of the second point
  this->Point2AEntry = vtkKWEntry::New();
  this->Point2AEntry->SetParent(Point2Frame);
  this->Point2AEntry->Create();
  this->Point2AEntry->SetWidth(8);
  this->Point2AEntry->SetRestrictValueToDouble();
  this->Point2AEntry->SetBalloonHelpString("Second point on guidance needle, A coordinate.");
  // create an entry for the S coordinate of the second point
  this->Point2SEntry = vtkKWEntry::New();
  this->Point2SEntry->SetParent(Point2Frame);
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
  GuidanceNeedleFrame->Delete();
  Point1Frame->Delete();
  Point2Frame->Delete();
}


//---------------------------------------------------------------------------
void vtkAbdoNavGUI::BuildGUINavigationFrame()
{
  //----------------------------------------------------------------
  // Build the GUI's navigation frame.
  //----------------------------------------------------------------

  vtkKWWidget* page = this->UIPanel->GetPageWidget("AbdoNav");

  // create a collapsible navigation frame
  vtkSlicerModuleCollapsibleFrame* navigationFrame = vtkSlicerModuleCollapsibleFrame::New();
  navigationFrame->SetParent(page);
  navigationFrame->Create();
  navigationFrame->SetLabelText("Navigation");
  navigationFrame->CollapseFrame();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", navigationFrame->GetWidgetName(), page->GetWidgetName());

  // create a labelled frame for the locator display options
  vtkKWFrameWithLabel* LocatorDisplayFrame = vtkKWFrameWithLabel::New();
  LocatorDisplayFrame->SetParent(navigationFrame->GetFrame());
  LocatorDisplayFrame->SetLabelText("Locator Display Options");
  LocatorDisplayFrame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", LocatorDisplayFrame->GetWidgetName());

  // create a frame to hold the locator mode check buttons
  vtkKWFrame* LocatorModeFrame = vtkKWFrame::New();
  LocatorModeFrame->SetParent(LocatorDisplayFrame->GetFrame());
  LocatorModeFrame->Create();
  this->Script("pack %s -side top -anchor c -padx 2 -pady 2", LocatorModeFrame->GetWidgetName());

  // create a check button to show/hide the locator model
  this->ShowLocatorCheckButton = vtkKWCheckButton::New();
  this->ShowLocatorCheckButton->SetParent(LocatorModeFrame);
  this->ShowLocatorCheckButton->Create();
  this->ShowLocatorCheckButton->SelectedStateOff();
  this->ShowLocatorCheckButton->SetText("Show Locator");

  // create a check button to freeze/unfreeze the locator model
  this->FreezeLocatorCheckButton = vtkKWCheckButton::New();
  this->FreezeLocatorCheckButton->SetParent(LocatorModeFrame);
  this->FreezeLocatorCheckButton->Create();
  this->FreezeLocatorCheckButton->SelectedStateOff();
  this->FreezeLocatorCheckButton->SetText("Freeze Locator");

  // create a check button to show/hide a crosshair in the slice views corresponding to the locator's tip position
  this->ShowCrosshairCheckButton = vtkKWCheckButton::New();
  this->ShowCrosshairCheckButton->SetParent(LocatorModeFrame);
  this->ShowCrosshairCheckButton->Create();
  this->ShowCrosshairCheckButton->SelectedStateOff();
  this->ShowCrosshairCheckButton->SetText("Show Crosshair");

  // add show locator, freeze locator and show crosshair check buttons
  this->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2",
                ShowLocatorCheckButton->GetWidgetName(),
                FreezeLocatorCheckButton->GetWidgetName(),
                ShowCrosshairCheckButton->GetWidgetName());

  // create a labelled frame for the slice driver options
  vtkKWFrameWithLabel* SliceDriverFrame = vtkKWFrameWithLabel::New();
  SliceDriverFrame->SetParent(navigationFrame->GetFrame());
  SliceDriverFrame->SetLabelText("Slice Driver");
  SliceDriverFrame->Create();
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", SliceDriverFrame->GetWidgetName());

  // create a frame to hold the slice view menu buttons
  vtkKWFrame* SliceFrame = vtkKWFrame::New();
  SliceFrame->SetParent(SliceDriverFrame->GetFrame());
  SliceFrame->Create();
  this->Script("pack %s -side top -anchor c -padx 2 -pady 2", SliceFrame->GetWidgetName());

  // get slice view colors red, green and yellow
  vtkSlicerColor* color = ((vtkSlicerApplication*)this->GetApplication())->GetSlicerTheme()->GetSlicerColors();
  // create a red slice view (axial) menu button
  this->RedSliceMenuButton = vtkKWMenuButton::New();
  this->RedSliceMenuButton->SetParent(SliceFrame);
  this->RedSliceMenuButton->Create();
  this->RedSliceMenuButton->SetWidth(13);
  this->RedSliceMenuButton->SetBackgroundColor(color->SliceGUIRed);
  this->RedSliceMenuButton->SetActiveBackgroundColor(color->SliceGUIRed);
  this->RedSliceMenuButton->GetMenu()->AddRadioButton("User");
  this->RedSliceMenuButton->GetMenu()->AddRadioButton("Locator");
  this->RedSliceMenuButton->SetValue("User");
  // create a yellow slice view (sagittal) menu button
  this->YellowSliceMenuButton = vtkKWMenuButton::New();
  this->YellowSliceMenuButton->SetParent(SliceFrame);
  this->YellowSliceMenuButton->Create();
  this->YellowSliceMenuButton->SetWidth(13);
  this->YellowSliceMenuButton->SetBackgroundColor(color->SliceGUIYellow);
  this->YellowSliceMenuButton->SetActiveBackgroundColor(color->SliceGUIYellow);
  this->YellowSliceMenuButton->GetMenu()->AddRadioButton("User");
  this->YellowSliceMenuButton->GetMenu()->AddRadioButton("Locator");
  this->YellowSliceMenuButton->SetValue("User");
  // create a green slice view (coronal) menu button
  this->GreenSliceMenuButton = vtkKWMenuButton::New();
  this->GreenSliceMenuButton->SetParent(SliceFrame);
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
  vtkKWFrame* SliceModeFrame = vtkKWFrame::New();
  SliceModeFrame->SetParent(SliceDriverFrame->GetFrame());
  SliceModeFrame->Create();
  this->Script("pack %s -side top -anchor c -padx 2 -pady 2", SliceModeFrame->GetWidgetName());

  // create a drive all slice views by locator push button
  this->SetLocatorAllPushButton = vtkKWPushButton::New();
  this->SetLocatorAllPushButton->SetParent(SliceModeFrame);
  this->SetLocatorAllPushButton->Create();
  this->SetLocatorAllPushButton->SetText("Locator All");
  this->SetLocatorAllPushButton->SetWidth(12);
  // create a drive all slice views by user push button
  this->SetUserAllPushButton = vtkKWPushButton::New();
  this->SetUserAllPushButton->SetParent(SliceModeFrame);
  this->SetUserAllPushButton->Create();
  this->SetUserAllPushButton->SetText("User All");
  this->SetUserAllPushButton->SetWidth(12);
  // create a freeze slice views check button
  this->FreezeSliceCheckButton = vtkKWCheckButton::New();
  this->FreezeSliceCheckButton->SetParent(SliceModeFrame);
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
  LocatorDisplayFrame->Delete();
  LocatorModeFrame->Delete();
  SliceDriverFrame->Delete();
  SliceFrame->Delete();
  SliceModeFrame->Delete();
}
