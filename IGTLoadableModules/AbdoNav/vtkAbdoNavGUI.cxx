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
#include "vtkSlicerNodeSelectorWidget.h"

/* KWWidgets includes */
#include "vtkKWFrameWithLabel.h"
#include "vtkKWSeparator.h"
#include "vtkKWTkUtilities.h"

//---------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkAbdoNavGUI, "$Revision: $");
vtkStandardNewMacro(vtkAbdoNavGUI);


//---------------------------------------------------------------------------
vtkAbdoNavGUI::vtkAbdoNavGUI()
{
  //----------------------------------------------------------------
  // Initialize logic values.
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData(reinterpret_cast<void*> (this));
  this->DataCallbackCommand->SetCallback(vtkAbdoNavGUI::DataCallback);

  this->TimerFlag = 0;

  //----------------------------------------------------------------
  // Connection frame.
  this->GuidanceNeedleSelectorWidget = NULL;
  this->CryoprobeSelectorWidget = NULL;
  this->SeparatorBeforeButtons = NULL;
  this->PausePushButton = NULL;
  this->ResetPushButton = NULL;
  this->ConfigurePushButton = NULL;
}


//---------------------------------------------------------------------------
vtkAbdoNavGUI::~vtkAbdoNavGUI()
{
  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  // if Logic is NULL, the class was only instatiated but never used,
  // e.g. Slicer was launched with option --ignore_module set to AbdoNav
  if (this->Logic)
    {
    this->RemoveGUIObservers();
    }

  this->SetModuleLogic(NULL);

  //----------------------------------------------------------------
  // Connection frame.
  if (this->GuidanceNeedleSelectorWidget)
    {
    this->GuidanceNeedleSelectorWidget->SetParent(NULL);
    this->GuidanceNeedleSelectorWidget->Delete();
    }
  if (this->CryoprobeSelectorWidget)
    {
    this->CryoprobeSelectorWidget->SetParent(NULL);
    this->CryoprobeSelectorWidget->Delete();
    }
  if (this->SeparatorBeforeButtons)
    {
    this->SeparatorBeforeButtons->SetParent(NULL);
    this->SeparatorBeforeButtons->Delete();
    }
  if (this->PausePushButton)
    {
    this->PausePushButton->SetParent(NULL);
    this->PausePushButton->Delete();
    }
  if (this->ResetPushButton)
    {
    this->ResetPushButton->SetParent(NULL);
    this->ResetPushButton->Delete();
    }
  if (this->ConfigurePushButton)
    {
    this->ConfigurePushButton->SetParent(NULL);
    this->ConfigurePushButton->Delete();
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

  // fill in

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

  // fill in

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
  const char* eventName = vtkCommand::GetStringFromEventId(event);
  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle* style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  // fill in
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
void vtkAbdoNavGUI::HandleMouseEvent(vtkSlicerInteractorStyle* style)
{
  //----------------------------------------------------------------
  // React to mouse events observed in one of the slice views. Currently not used.
  //----------------------------------------------------------------

  vtkSlicerApplicationGUI* appGUI = this->GetApplicationGUI();
  vtkSlicerInteractorStyle* istyle0 = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI("Red")
      ->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle* istyle1 = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI("Yellow")
      ->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());
  vtkSlicerInteractorStyle* istyle2 = vtkSlicerInteractorStyle::SafeDownCast(appGUI->GetMainSliceGUI("Green")
      ->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle());

  vtkCornerAnnotation* anno = NULL;
  if (style == istyle0)
    {
    anno = appGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }
  else if (style == istyle1)
    {
    anno = appGUI->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }
  else if (style == istyle2)
    {
    anno = appGUI->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()->GetCornerAnnotation();
    }

  // change slice sliew corner annotation here
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
    "\n\n"
    "See <a>http://www.slicer.org/slicerWiki/index.php/Modules:AbdoNav-Documentation-3.6</a> for details "
    "about the module.";
  // about text
  const char* about =
    "The **AbdoNav** module was contributed by Christoph Ammann (Karlsruhe Institute of Technology, KIT) "
    "and Nobuhiko Hata, PhD (Surgical Navigation and Robotics Laboratory, SNR).";

  vtkKWWidget* page = this->UIPanel->GetPageWidget("AbdoNav");
  this->BuildHelpAndAboutFrame(page, help, about);
}


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

  // create a guidance needle tracker selector widget
  this->GuidanceNeedleSelectorWidget = vtkSlicerNodeSelectorWidget::New() ;
  this->GuidanceNeedleSelectorWidget->SetParent(connectionFrame->GetFrame());
  this->GuidanceNeedleSelectorWidget->Create();
  this->GuidanceNeedleSelectorWidget->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, "LinearTransform");
  // explicitly indicate that the user is not allowed to create a new linear transform node
  this->GuidanceNeedleSelectorWidget->SetNewNodeEnabled(0);
  this->GuidanceNeedleSelectorWidget->SetDefaultEnabled(0);
  this->GuidanceNeedleSelectorWidget->SetMRMLScene(this->GetMRMLScene());
  this->GuidanceNeedleSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->GuidanceNeedleSelectorWidget->SetLabelText("Guidance needle tracker:\t\t");
  this->GuidanceNeedleSelectorWidget->SetBalloonHelpString("Select the tracker transform node corresponding to the guidance needle.");

  // add guidance needle tracker selector widget
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", this->GuidanceNeedleSelectorWidget->GetWidgetName());

  // create a cryoprobe tracker selector widget
  this->CryoprobeSelectorWidget = vtkSlicerNodeSelectorWidget::New() ;
  this->CryoprobeSelectorWidget->SetParent(connectionFrame->GetFrame());
  this->CryoprobeSelectorWidget->Create();
  this->CryoprobeSelectorWidget->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, "LinearTransform");
  // explicitly indicate that the user is not allowed to create a new linear transform node
  this->CryoprobeSelectorWidget->SetNewNodeEnabled(0);
  this->CryoprobeSelectorWidget->SetDefaultEnabled(0);
  this->CryoprobeSelectorWidget->SetMRMLScene(this->GetMRMLScene());
  this->CryoprobeSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->CryoprobeSelectorWidget->SetLabelText("Cryoprobe(s) tracker:\t\t");
  this->CryoprobeSelectorWidget->SetBalloonHelpString("Select the tracker transform node corresponding to the cryoprobes.");

  // add cryoprobe tracker selector widget
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2", this->CryoprobeSelectorWidget->GetWidgetName());

  // create a separator between selector widgets and buttons
  this->SeparatorBeforeButtons = vtkKWSeparator::New();
  this->SeparatorBeforeButtons->SetParent(connectionFrame->GetFrame());
  this->SeparatorBeforeButtons->Create();

  // add separator
  this->Script("pack %s -side top -anchor nw -fill x -pady {20 2}", this->SeparatorBeforeButtons->GetWidgetName());

  // create a pause button
  this->PausePushButton = vtkKWPushButton::New();
  this->PausePushButton->SetParent(connectionFrame->GetFrame());
  this->PausePushButton->Create();
  this->PausePushButton->SetText("Pause Connection");
  this->PausePushButton->SetBalloonHelpString("Pause reception from currently stored tracker transforms.");

  // create a reset button
  this->ResetPushButton = vtkKWPushButton::New();
  this->ResetPushButton->SetParent(connectionFrame->GetFrame());
  this->ResetPushButton->Create();
  this->ResetPushButton->SetText("Reset Connection");
  this->ResetPushButton->SetBalloonHelpString("Reset currently stored tracker transforms.");

  // add pause and reset button
  this->Script("pack %s %s -side left -anchor nw -padx 2 -pady 2", this->PausePushButton->GetWidgetName(), this->ResetPushButton->GetWidgetName());

  // create a configure button
  this->ConfigurePushButton = vtkKWPushButton::New();
  this->ConfigurePushButton->SetParent(connectionFrame->GetFrame());
  this->ConfigurePushButton->Create();
  this->ConfigurePushButton->SetText("Configure Connection");
  this->ConfigurePushButton->SetBalloonHelpString("Configure connection based on chosen tracker transforms.");

  // add configure button
  this->Script("pack %s -side right -anchor ne -padx 2 -pady 2", this->ConfigurePushButton->GetWidgetName());

  // clean up
  connectionFrame->Delete();
}
