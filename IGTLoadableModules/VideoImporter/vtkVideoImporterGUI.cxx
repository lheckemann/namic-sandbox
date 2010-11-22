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

#include "vtkVideoImporterGUI.h"
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
#include "vtkKWRadioButtonSet.h"
#include "vtkKWRadioButton.h"

#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkMRMLCameraNode.h"

#include "vtkKWPushButton.h"

#include "vtkCornerAnnotation.h"
#include "vtkMRMLLinearTransformNode.h"


// for test
#include "vtkJPEGReader.h"
#include "vtkImageCanvasSource2D.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkVideoImporterGUI );
vtkCxxRevisionMacro ( vtkVideoImporterGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkVideoImporterGUI::vtkVideoImporterGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkVideoImporterGUI::DataCallback);

  //----------------------------------------------------------------
  // GUI widgets
  //this->ShowSecondaryWindowButton = NULL;
  //this->HideSecondaryWindowButton = NULL;
  //this->SecondaryViewerWindow = NULL;

  this->StartCaptureButton = NULL;
  this->StopCaptureButton  = NULL;

  this->TimerFlag = 0;
  this->CameraActiveFlag = 0;

  this->CameraChannelLabel = NULL;
  this->CameraChannelEntry = NULL;
  this->VideoFileLabel     = NULL;
  this->VideoFileEntry     = NULL;
  this->VideoFileSelectButton = NULL;
  this->VideoSourceButtonSet = NULL;
  this->OpticalFlowStatusButtonSet = NULL;

  this->VideoImageData     = NULL;
  this->BackgroundRenderer = NULL;

  this->OpticalFlowTrackingFlag = 0;
  this->GrayImage = NULL;
  this->PrevGrayImage = NULL;
  this->Pyramid = NULL;
  this->PrevPyramid = NULL;
  this->SwapTempImage = NULL;
  this->PyrFlag = 0;

}


//---------------------------------------------------------------------------
vtkVideoImporterGUI::~vtkVideoImporterGUI ( )
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

  /*
  if (this->SecondaryViewerWindow)
    {
    this->SecondaryViewerWindow->Withdraw();
    this->SecondaryViewerWindow->SetApplication(NULL);
    this->SecondaryViewerWindow->Delete();
    this->SecondaryViewerWindow = NULL;
    }
  */

  //if (this->ShowSecondaryWindowButton)
  //  {
  //  this->ShowSecondaryWindowButton->SetParent(NULL);
  //  this->ShowSecondaryWindowButton->Delete();
  //  }
  //
  //if (this->HideSecondaryWindowButton)
  //  {
  //  this->HideSecondaryWindowButton->SetParent(NULL);
  //  this->HideSecondaryWindowButton->Delete();
  //  }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkVideoImporterGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkVideoImporterGUI::Enter()
{
  // Fill in
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 30;  // 100 ms
    ProcessTimerEvents();
    }

}


//---------------------------------------------------------------------------
void vtkVideoImporterGUI::Exit ( )
{
  // Fill in
}

//---------------------------------------------------------------------------
void vtkVideoImporterGUI::TearDownGUI() 
{
  /*
  this->PrimaryMonitorRobotViewerWidget->SetApplication(NULL);
  this->PrimaryMonitorRobotViewerWidget->SetMainViewerWidget(NULL);
  this->PrimaryMonitorRobotViewerWidget->SetMRMLScene(NULL);
  */
}



//---------------------------------------------------------------------------
void vtkVideoImporterGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "VideoImporterGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkVideoImporterGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  /*
  if (this->ShowSecondaryWindowButton)
    {
    this->ShowSecondaryWindowButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->HideSecondaryWindowButton)
    {
    this->HideSecondaryWindowButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  */

  if (this->VideoSourceButtonSet)
    {
    this->VideoSourceButtonSet->GetWidget(0)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->VideoSourceButtonSet->GetWidget(1)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->VideoFileSelectButton)
    {
    this->VideoFileSelectButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->StartCaptureButton)
    {
    this->StartCaptureButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->StopCaptureButton)
    {
    this->StopCaptureButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->OpticalFlowStatusButtonSet)
    {
    this->OpticalFlowStatusButtonSet->GetWidget(0)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->OpticalFlowStatusButtonSet->GetWidget(1)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->AddLogicObservers();

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkVideoImporterGUI::AddGUIObservers ( )
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

  /*
  this->ShowSecondaryWindowButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->HideSecondaryWindowButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  */

  if (this->VideoSourceButtonSet)
    {
    this->VideoSourceButtonSet->GetWidget(0)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->VideoSourceButtonSet->GetWidget(1)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->VideoFileSelectButton)
    {
    this->VideoFileSelectButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TransformNodeSelector)
    {
    this->TransformNodeSelector
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    this->TransformNodeSelector
      ->AddObserver(vtkSlicerNodeSelectorWidget::NewNodeEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->StartCaptureButton)
    {
    this->StartCaptureButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->StopCaptureButton)
    {
    this->StopCaptureButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if (this->OpticalFlowStatusButtonSet)
    {
    this->OpticalFlowStatusButtonSet->GetWidget(0)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->OpticalFlowStatusButtonSet->GetWidget(1)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkVideoImporterGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkVideoImporterGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkVideoImporterLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkVideoImporterGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkVideoImporterGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  //if (this->ShowSecondaryWindowButton == vtkKWPushButton::SafeDownCast(caller) 
  //    && event == vtkKWPushButton::InvokedEvent)
  //  {
  //
  //    if (this->SecondaryViewerWindow)
  //    {  
  //    this->SecondaryViewerWindow->DisplayOnSecondaryMonitor();
  //    }
  //
  //  }
  //else
  //if (this->HideSecondaryWindowButton == vtkKWPushButton::SafeDownCast(caller)
  //         && event == vtkKWPushButton::InvokedEvent)
  //  {
  //  /*
  //    if (this->SecondaryViewerWindow)
  //    {  
  //    this->SecondaryViewerWindow->Withdraw();
  //    }
  //  */
  //  }
  //else

  if (this->VideoSourceButtonSet->GetWidget(0)
      == vtkKWRadioButton::SafeDownCast(caller)
      && event == vtkKWRadioButton::SelectedStateChangedEvent
      && this->VideoSourceButtonSet->GetWidget(0)->GetSelectedState() == 1)
    {
    this->CameraChannelEntry->EnabledOn();
    this->VideoFileSelectButton->EnabledOff();
    this->VideoFileEntry->EnabledOff();
    }
  else if (this->VideoSourceButtonSet->GetWidget(1)
           == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->VideoSourceButtonSet->GetWidget(1)->GetSelectedState() == 1)
    {
    this->CameraChannelEntry->EnabledOff();
    this->VideoFileSelectButton->EnabledOn();
    this->VideoFileEntry->EnabledOn();
    }
  else if (this->VideoFileSelectButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    vtkKWFileBrowserDialog* fbrowse = vtkKWFileBrowserDialog::New();
    fbrowse->SetParent(this->GetApplicationGUI()->GetActiveViewerWidget());
    fbrowse->Create();
    fbrowse->SetFileTypes("{{Video} {.mov .avi}}");
    fbrowse->MultipleSelectionOff();
    if (fbrowse->Invoke())
      {
      const char* path = fbrowse->GetFileName();
      this->VideoFileEntry->SetValue(path);
      }
    fbrowse->Delete();
    }
  else if (this->TransformNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent )
    {
    /*
    vtkMRMLCameraNode* cameraNode = 
      vtkMRMLCameraNode::SafeDownCast(this->TransformNodeSelector->GetSelected());
    if (this->SecondaryViewerWindow)
      {
      this->SecondaryViewerWindow->SetCameraNode(cameraNode);
      }
    */
    }
  else if (this->TransformNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NewNodeEvent) 
    {
    /*
    vtkMRMLCameraNode* cameraNode = 
      vtkMRMLCameraNode::SafeDownCast(this->TransformNodeSelector->GetSelected());
    if (this->SecondaryViewerWindow)
      {
      this->SecondaryViewerWindow->SetCameraNode(cameraNode);
      }
    */
    }
  else if (this->StartCaptureButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    if (this->VideoSourceButtonSet->GetWidget(0)->GetSelectedState())
      { // Camera is used as a video source
      int channel = this->CameraChannelEntry->GetValueAsInt();
      if (this->StartCamera(channel, NULL))
        { // Success
        this->StartCaptureButton->EnabledOff();
        this->StopCaptureButton->EnabledOn();
        }
      else
        {
        }
      }
    else
      { // File is used as a video source
      if (this->StartCamera(-1, this->VideoFileEntry->GetValue()))
        {
        this->StartCaptureButton->EnabledOff();
        this->StopCaptureButton->EnabledOn();
        }
      }
    }
  else if (this->StopCaptureButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    this->StopCamera();
    this->StartCaptureButton->EnabledOn();
    this->StopCaptureButton->EnabledOff();
    }
  else if (this->OpticalFlowStatusButtonSet->GetWidget(0)
      == vtkKWRadioButton::SafeDownCast(caller)
      && event == vtkKWRadioButton::SelectedStateChangedEvent
      && this->OpticalFlowStatusButtonSet->GetWidget(0)->GetSelectedState() == 1)
    {
    this->OpticalFlowTrackingFlag = 1;
    }
  else if (this->OpticalFlowStatusButtonSet->GetWidget(1)
      == vtkKWRadioButton::SafeDownCast(caller)
           && event == vtkKWRadioButton::SelectedStateChangedEvent
           && this->OpticalFlowStatusButtonSet->GetWidget(1)->GetSelectedState() == 1)
    {
    this->OpticalFlowTrackingFlag = 0;
    }
} 


//---------------------------------------------------------------------------
void vtkVideoImporterGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkVideoImporterGUI *self = reinterpret_cast<vtkVideoImporterGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkVideoImporterGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkVideoImporterGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkVideoImporterLogic::SafeDownCast(caller))
    {
    if (event == vtkVideoImporterLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkVideoImporterGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkVideoImporterGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    if (this->CameraActiveFlag)
      {
      CameraHandler();
      }
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");
    }
}


//---------------------------------------------------------------------------
void vtkVideoImporterGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "VideoImporter", "VideoImporter", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForWindowConfigurationFrame();
  BuildGUIForOpticalFlowFrame();

  /*
  this->SecondaryViewerWindow = vtkSecondaryWindowViwerWindow::New();
  this->SecondaryViewerWindow->SetApplication(this->GetApplication());
  this->SecondaryViewerWindow->Create();
  */

  //vtkMRMLCameraNode* cameraNode = vtkMRMLCameraNode::New();
  //this->SecondaryViewerWindow->SetCameraNode(cameraNode);

  //// no view in the scene and local
  //// create an active camera
  //vtkMRMLViewNode* node = vtkMRMLViewNode::New();
  //this->MRMLScene->AddNode(node);
  //this->SecondaryViewerWindow->SetViewNode(node);
  ////this->SetAndObserveViewNode (node);
  //node->Delete();

}


void vtkVideoImporterGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:VideoImporter</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "VideoImporter" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkVideoImporterGUI::BuildGUIForWindowConfigurationFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("VideoImporter");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Video Import Configuration");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Video Import frame
  
  vtkKWFrame *importframe = vtkKWFrame::New();
  importframe->SetParent(conBrowsFrame->GetFrame());
  importframe->Create();
  app->Script ( "pack %s -fill both -expand true",  
                 importframe->GetWidgetName() );

  vtkKWLabel *sourceLabel = vtkKWLabel::New();
  sourceLabel->SetParent(importframe);
  sourceLabel->Create();
  sourceLabel->SetWidth(8);
  sourceLabel->SetText("Source: ");

  this->VideoSourceButtonSet = vtkKWRadioButtonSet::New();
  this->VideoSourceButtonSet->SetParent(importframe);
  this->VideoSourceButtonSet->Create();
  this->VideoSourceButtonSet->PackHorizontallyOn();
  this->VideoSourceButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->VideoSourceButtonSet->UniformColumnsOn();
  this->VideoSourceButtonSet->UniformRowsOn();

  this->VideoSourceButtonSet->AddWidget(0);
  this->VideoSourceButtonSet->GetWidget(0)->SetText("CAM");
  this->VideoSourceButtonSet->AddWidget(1);
  this->VideoSourceButtonSet->GetWidget(1)->SetText("File");
  this->VideoSourceButtonSet->GetWidget(0)->SelectedStateOn();
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              sourceLabel->GetWidgetName() , this->VideoSourceButtonSet->GetWidgetName());

  importframe->Delete();

  vtkKWFrame *camchframe = vtkKWFrame::New();
  camchframe->SetParent(conBrowsFrame->GetFrame());
  camchframe->Create();
  app->Script ( "pack %s -fill both -expand true",  
                 camchframe->GetWidgetName() );

  this->CameraChannelLabel = vtkKWLabel::New();
  this->CameraChannelLabel->SetParent(camchframe);
  this->CameraChannelLabel->Create();
  this->CameraChannelLabel->SetWidth(8);
  this->CameraChannelLabel->SetText("CAM #: ");

  this->CameraChannelEntry = vtkKWEntry::New();
  this->CameraChannelEntry->SetParent(camchframe);
  this->CameraChannelEntry->Create();
  this->CameraChannelEntry->SetRestrictValueToInteger();
  this->CameraChannelEntry->SetWidth(18);
  this->CameraChannelEntry->SetValue("0");

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              this->CameraChannelLabel->GetWidgetName(),
              this->CameraChannelEntry->GetWidgetName());

  camchframe->Delete();
  

  vtkKWFrame* fileFrame = vtkKWFrame::New();
  fileFrame->SetParent(conBrowsFrame->GetFrame());
  fileFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                fileFrame->GetWidgetName());

  this->VideoFileLabel = vtkKWLabel::New();
  this->VideoFileLabel->SetParent(fileFrame);
  this->VideoFileLabel->Create();
  this->VideoFileLabel->SetWidth(8);
  this->VideoFileLabel->SetText("File: ");

  this->VideoFileEntry = vtkKWEntry::New();
  this->VideoFileEntry->SetParent(fileFrame);
  this->VideoFileEntry->Create();
  this->VideoFileEntry->SetWidth(30);

  this->VideoFileSelectButton = vtkKWPushButton::New ( );
  this->VideoFileSelectButton->SetParent ( fileFrame );
  this->VideoFileSelectButton->Create ( );
  this->VideoFileSelectButton->SetText ("Browse");
  this->VideoFileSelectButton->SetWidth (12);

  app->Script("pack %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              this->VideoFileLabel->GetWidgetName(),
              this->VideoFileEntry->GetWidgetName(),
              this->VideoFileSelectButton->GetWidgetName());

  this->VideoFileLabel->EnabledOff();
  this->VideoFileEntry->EnabledOff();
  this->VideoFileSelectButton->EnabledOff();


  fileFrame->Delete();


  // -----------------------------------------
  // Push buttons

  vtkKWFrame *cvframe = vtkKWFrame::New();
  cvframe->SetParent(conBrowsFrame->GetFrame());
  cvframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 cvframe->GetWidgetName() );

  this->StartCaptureButton = vtkKWPushButton::New ( );
  this->StartCaptureButton->SetParent ( cvframe );
  this->StartCaptureButton->Create ( );
  this->StartCaptureButton->SetText ("ON");
  this->StartCaptureButton->SetWidth (12);

  this->StopCaptureButton = vtkKWPushButton::New ( );
  this->StopCaptureButton->SetParent ( cvframe );
  this->StopCaptureButton->Create ( );
  this->StopCaptureButton->SetText ("OFF");
  this->StopCaptureButton->SetWidth (12);

  this->Script("pack %s %s -side left -padx 2 -pady 2", 
               this->StartCaptureButton->GetWidgetName(),
               this->StopCaptureButton->GetWidgetName());

  this->StartCaptureButton->EnabledOn();
  this->StopCaptureButton->EnabledOff();

  cvframe->Delete();


  /*
    vtkKWFileBrowserDialog* fbrowse = vtkKWFileBrowserDialog::New();
    fbrowse->SetParent(this->GetApplicationGUI()->GetActiveViewerWidget());
    fbrowse->Create();
    fbrowse->SetFileTypes("{{Array data} {.txt .csv}}");
    fbrowse->MultipleSelectionOff();
    if (fbrowse->Invoke())
      {
      const char* path = fbrowse->GetFileName();
      ImportPlotNode(path);
      }
    fbrowse->Delete();
  */


  
  // -----------------------------------------
  // Push buttons

  /*
  this->ShowSecondaryWindowButton = vtkKWPushButton::New ( );
  this->ShowSecondaryWindowButton->SetParent ( switchframe );
  this->ShowSecondaryWindowButton->Create ( );
  this->ShowSecondaryWindowButton->SetText ("ON");
  this->ShowSecondaryWindowButton->SetWidth (12);

  this->HideSecondaryWindowButton = vtkKWPushButton::New ( );
  this->HideSecondaryWindowButton->SetParent ( switchframe );
  this->HideSecondaryWindowButton->Create ( );
  this->HideSecondaryWindowButton->SetText ("OFF");
  this->HideSecondaryWindowButton->SetWidth (12);

  this->Script("pack %s %s -side left -padx 2 -pady 2", 
               this->ShowSecondaryWindowButton->GetWidgetName(),
               this->HideSecondaryWindowButton->GetWidgetName());
  */

  conBrowsFrame->Delete();
  //switchframe->Delete();

}


//----------------------------------------------------------------------------
void vtkVideoImporterGUI::BuildGUIForOpticalFlowFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("VideoImporter");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Optical Flow");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Video Import frame
  
  vtkKWFrame *statusframe = vtkKWFrame::New();
  statusframe->SetParent(conBrowsFrame->GetFrame());
  statusframe->Create();
  app->Script ( "pack %s -fill both -expand true",  
                statusframe->GetWidgetName() );

  vtkKWLabel *statusLabel = vtkKWLabel::New();
  statusLabel->SetParent(statusframe);
  statusLabel->Create();
  statusLabel->SetWidth(8);
  statusLabel->SetText("Status: ");

  this->OpticalFlowStatusButtonSet = vtkKWRadioButtonSet::New();
  this->OpticalFlowStatusButtonSet->SetParent(statusframe);
  this->OpticalFlowStatusButtonSet->Create();
  this->OpticalFlowStatusButtonSet->PackHorizontallyOn();
  this->OpticalFlowStatusButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->OpticalFlowStatusButtonSet->UniformColumnsOn();
  this->OpticalFlowStatusButtonSet->UniformRowsOn();

  this->OpticalFlowStatusButtonSet->AddWidget(0);
  this->OpticalFlowStatusButtonSet->GetWidget(0)->SetText("Enabled");
  this->OpticalFlowStatusButtonSet->AddWidget(1);
  this->OpticalFlowStatusButtonSet->GetWidget(1)->SetText("Disabled");
  this->OpticalFlowStatusButtonSet->GetWidget(1)->SelectedStateOn();
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              statusLabel->GetWidgetName() , this->OpticalFlowStatusButtonSet->GetWidgetName());
  
  statusframe->Delete();


  // -----------------------------------------
  // Node select frame

  vtkKWFrame *selectframe = vtkKWFrame::New();
  selectframe->SetParent(conBrowsFrame->GetFrame());
  selectframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 selectframe->GetWidgetName() );

  this->TransformNodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->TransformNodeSelector->SetParent(selectframe);
  this->TransformNodeSelector->Create();
  this->TransformNodeSelector->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, "LinearTransform");
  this->TransformNodeSelector->SetNewNodeEnabled(1);
  this->TransformNodeSelector->SetMRMLScene(this->GetMRMLScene());
  this->TransformNodeSelector->SetBorderWidth(2);
  this->TransformNodeSelector->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->TransformNodeSelector->GetWidget()->GetWidget()->SetWidth(24);
  this->TransformNodeSelector->SetLabelText( "Transform : ");
  this->TransformNodeSelector->SetBalloonHelpString("Select or create a camera.");

  this->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
               this->TransformNodeSelector->GetWidgetName());

}


//----------------------------------------------------------------------------
void vtkVideoImporterGUI::UpdateAll()
{
}


//----------------------------------------------------------------------------
int vtkVideoImporterGUI::ViewerBackgroundOn(vtkSlicerViewerWidget* vwidget, vtkImageData* imageData)
{
  
  vtkKWRenderWidget* rwidget;
  vtkRenderWindow* rwindow;

  if (vwidget&&
      (rwidget = vwidget->GetMainViewer()) &&
      (rwindow = rwidget->GetRenderWindow()))
    {
    if (rwidget->GetNumberOfRenderers() == 1)
      {
      this->BackgroundRenderer = vtkRenderer::New();
      this->BackgroundActor = vtkImageActor::New();
      this->BackgroundActor->SetInput(imageData);
      this->BackgroundRenderer->AddActor(this->BackgroundActor);
      this->BackgroundRenderer->InteractiveOff();
      this->BackgroundRenderer->SetLayer(0);

      // Adjust camera position so that image covers the draw area.

      this->BackgroundActor->Modified();
      rwidget->GetNthRenderer(0)->SetLayer(1);
      rwidget->AddRenderer(this->BackgroundRenderer);
      rwindow->Render();

      vtkCamera* camera = this->BackgroundRenderer->GetActiveCamera();
      double x, y, z;
      camera->GetPosition(x, y, z);
      camera->SetViewAngle(90.0);
      camera->SetPosition(x, y, y); 

      // The following code fixes a issue that
      // video doesn't show up on the viewer.
      vtkCamera* fcamera = rwidget->GetNthRenderer(0)->GetActiveCamera();
      if (fcamera)
        {
        fcamera->Modified();
        }

      return 1;
      }
    }
  return 0;
}


//----------------------------------------------------------------------------
int vtkVideoImporterGUI::ViewerBackgroundOff(vtkSlicerViewerWidget* vwidget)
{
  vtkKWRenderWidget* rwidget;
  vtkRenderWindow* rwindow;

  if (vwidget&&
      (rwidget = vwidget->GetMainViewer()) &&
      (rwindow = rwidget->GetRenderWindow()))
    {
    if (rwidget->GetNumberOfRenderers() > 1)
      {
      rwidget->RemoveNthRenderer(1);
      rwidget->GetNthRenderer(0)->SetLayer(0);
      rwindow->Render();
      this->BackgroundRenderer = NULL;
      this->BackgroundActor = NULL;
      }
    }

  return 0;

}  




//----------------------------------------------------------------------------
// Start Camera
// if channel = -1, OpenCV will read image from the video file specified by path
int vtkVideoImporterGUI::StartCamera(int channel, const char* path)
{

  this->capture      = NULL;
  this->captureImage = NULL;
  this->RGBImage     = NULL;
  this->undistortionImage = NULL;
  this->imageSize.width = 0;
  this->imageSize.height = 0;

  vtkSlicerViewerWidget* vwidget = this->GetApplicationGUI()->GetNthViewerWidget(0);

  if (channel < 0 && path != NULL)
    {
    this->capture = cvCaptureFromAVI( path );
    }
  else 
    {
    this->capture = cvCaptureFromCAM(channel);
    }

  if (this->capture == NULL)
    {
    return 0;
    }


  this->CameraActiveFlag = 1;

  if (!this->VideoImageData)
    {
    this->VideoImageData = vtkImageData::New();
    this->VideoImageData->SetDimensions(64, 64, 1);
    this->VideoImageData->SetExtent(0, 63, 0, 63, 0, 0 );
    this->VideoImageData->SetSpacing(1.0, 1.0, 1.0);
    this->VideoImageData->SetOrigin(0.0, 0.0, 0.0);
    this->VideoImageData->SetNumberOfScalarComponents(3);
    this->VideoImageData->SetScalarTypeToUnsignedChar();
    this->VideoImageData->AllocateScalars();
    }
  this->VideoImageData->Update();
  ViewerBackgroundOn(vwidget, this->VideoImageData);

  return 1;
} 


//----------------------------------------------------------------------------
// Stop Camera thread
int vtkVideoImporterGUI::StopCamera()
{
  this->CameraActiveFlag = 0;

  cvReleaseCapture(&(this->capture));

  vtkSlicerViewerWidget* vwidget = this->GetApplicationGUI()->GetNthViewerWidget(0);
  ViewerBackgroundOff(vwidget);

  return 1;
} 


#define NGRID_X  20
#define NGRID_Y  20
#define TIME_POINTS (30*30)

//----------------------------------------------------------------------------
// Camera thread / Originally created by A. Yamada
int vtkVideoImporterGUI::CameraHandler()
{
  IplImage* captureImageTmp = NULL;
  CvSize   newImageSize;

  if (this->capture)
    {
    // 5/15/2010 ayamada
    if(NULL == (captureImageTmp = cvQueryFrame( capture )))
      {
      fprintf(stdout, "\n\nCouldn't take a picture\n\n");
      return 0;
      }

    // 5/6/2010 ayamada creating RGB image and capture image
    newImageSize = cvGetSize( captureImageTmp );

    // check if the image size is changed
    if (newImageSize.width != this->imageSize.width ||
        newImageSize.height != this->imageSize.height)
      {
      this->imageSize.width = newImageSize.width;
      this->imageSize.height = newImageSize.height;
      this->captureImage = cvCreateImage(this->imageSize, IPL_DEPTH_8U,3);
      this->RGBImage = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
      this->undistortionImage = cvCreateImage( this->imageSize, IPL_DEPTH_8U, 3);

      this->VideoImageData->SetDimensions(newImageSize.width, newImageSize.height, 1);
      this->VideoImageData->SetExtent(0, newImageSize.width-1, 0, newImageSize.height-1, 0, 0 );
      this->VideoImageData->SetNumberOfScalarComponents(3);
      this->VideoImageData->SetScalarTypeToUnsignedChar();
      this->VideoImageData->AllocateScalars();
      this->VideoImageData->Update();

      vtkSlicerViewerWidget* vwidget = this->GetApplicationGUI()->GetNthViewerWidget(0);
      ViewerBackgroundOff(vwidget);
      ViewerBackgroundOn(vwidget, this->VideoImageData);

      // for optical flow
      this->Pyramid       = cvCreateImage( this->imageSize , IPL_DEPTH_8U, 1 );
      this->PrevPyramid   = cvCreateImage( this->imageSize , IPL_DEPTH_8U, 1 );
      this->GrayImage     = cvCreateImage( this->imageSize , IPL_DEPTH_8U, 1 );
      this->PrevGrayImage = cvCreateImage( this->imageSize , IPL_DEPTH_8U, 1 );
      this->SwapTempImage = cvCreateImage( this->imageSize , IPL_DEPTH_8U, 1 );

      double gridSpaceX = (double)newImageSize.width / (double)(NGRID_X+1);
      double gridSpaceY = (double)newImageSize.height / (double)(NGRID_Y+1);

      this->Points[0] = 0;
      this->Points[1] = 0;
      this->GridPoints[0] = (CvPoint2D32f*)cvAlloc(NGRID_X*NGRID_Y*sizeof(this->Points[0][0]));
      this->GridPoints[1] = (CvPoint2D32f*)cvAlloc(NGRID_X*NGRID_Y*sizeof(this->Points[0][0]));
      this->RVector = (CvPoint2D32f*)cvAlloc(NGRID_X*NGRID_Y*sizeof(this->Points[0][0]));

      for (int i = 0; i < NGRID_X; i ++)
        {
        for (int j = 0; j < NGRID_Y; j ++)
          {
          this->GridPoints[0][i+j*NGRID_X].x = gridSpaceX*i + gridSpaceX;
          this->GridPoints[0][i+j*NGRID_Y].y = gridSpaceY*j + gridSpaceY;
          }
        }

      this->OpticalFlowStatus = (char*)cvAlloc(NGRID_X*NGRID_Y);
      }
    
    // create rgb image
    // 5/6/2010 ayamada for videoOverlay
    cvFlip(captureImageTmp, this->captureImage, 0);
    
    //cvUndistort2( captureImage, undistortionImage, pGUI->intrinsicMatrix, pGUI->distortionCoefficient );            
    //cvCvtColor( undistortionImage, RGBImage, CV_BGR2RGB);       //comment not to undistort      at 10. 01. 07 - smkim
    cvCvtColor( captureImage, RGBImage, CV_BGR2RGB);       //comment not to undistort      at 10. 01. 07 - smkim
    
    if (this->OpticalFlowTrackingFlag)
      {
      int win_size = 10;
      int count = NGRID_X*NGRID_Y;
      cvCvtColor(this->RGBImage, this->GrayImage, CV_RGB2GRAY );
      cvCalcOpticalFlowPyrLK( this->PrevGrayImage, this->GrayImage, this->PrevPyramid, this->Pyramid,
                              this->GridPoints[0], this->GridPoints[1], count, cvSize(win_size,win_size), 3, this->OpticalFlowStatus, 0,
                              cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), this->PyrFlag );
      this->PyrFlag |= CV_LKFLOW_PYR_A_READY;
      
      double dx = 0.0;
      double dy = 0.0;
      //int frameCount = 0;

      for(int i =  0; i < count; i++ )
        {
        if( !this->OpticalFlowStatus[i] )
          {
          this->GridPoints[1][i].x = this->GridPoints[0][i].x;
          this->GridPoints[1][i].y = this->GridPoints[0][i].y;
          }
        dx = this->GridPoints[1][i].x - this->GridPoints[0][i].x;
        dy = this->GridPoints[1][i].y - this->GridPoints[0][i].y;
        
        if (sqrt(dx*dx + dy*dy) > 50.0)
          {
          this->GridPoints[1][i].x = this->GridPoints[0][i].x;
          this->GridPoints[1][i].y = this->GridPoints[0][i].y;
          dx = 0.0;
          dy = 0.0;
          }
        
        this->RVector[i].x = dx;
        this->RVector[i].y = dy;
        cvCircle(this->RGBImage, cvPointFrom32f(this->GridPoints[0][i]), 3, CV_RGB(0,255,255), -1, 8,0);
        cvLine(this->RGBImage, cvPointFrom32f(this->GridPoints[0][i]), cvPointFrom32f(this->GridPoints[1][i]), CV_RGB(0,255,0), 2);
        }
      
      CV_SWAP( this->PrevGrayImage, this->GrayImage, this->SwapTempImage );
      CV_SWAP( this->PrevPyramid, this->Pyramid, this->SwapTempImage );
      CV_SWAP( this->Points[0], this->Points[1], this->SwapPoints );

      ProcessMotion(this->RVector, this->GridPoints[0], count);
      }
    
    unsigned char* idata;    
    // 5/6/2010 ayamada ok for videoOverlay
    idata = (unsigned char*) RGBImage->imageData;
    
    int dsize = this->imageSize.width*this->imageSize.height*3;
    memcpy((void*)this->VideoImageData->GetScalarPointer(), (void*)this->RGBImage->imageData, dsize);

    if (this->VideoImageData && this->BackgroundRenderer)
      {
      //this->VideoImageData->Update();
      this->VideoImageData->Modified();
      this->BackgroundRenderer->GetRenderWindow()->Render();
      }

    }
  
  return 1;
  
}


//----------------------------------------------------------------------------
int vtkVideoImporterGUI::ProcessMotion(CvPoint2D32f* vector, CvPoint2D32f* position, int n)
{
  float threshold = 5.0;
  CvPoint2D32f mean;

  mean.x = 0.0;
  mean.y = 0.0;

  // Use 10% vectors to calculate translation
  for (int i = 0; i < n; i ++)
    {
    float x = vector[i].x;
    float y = vector[i].y;
    float len = sqrtf(x*x + y*y);
    if (len > threshold)
      {
      mean.x += x;
      mean.y += y;
      }
    }
  mean.x /= (float)n;
  mean.y /= (float)n;

  vtkMRMLLinearTransformNode* transformNode = 
    vtkMRMLLinearTransformNode::SafeDownCast(this->TransformNodeSelector->GetSelected());
  if (transformNode)
    {
    vtkMatrix4x4* transform = transformNode->GetMatrixTransformToParent();
    if (transform)
      {
      float x = transform->GetElement(0, 3) - mean.x*2.0;
      float y = transform->GetElement(2, 3) + mean.y*2.0;
      transform->SetElement(0, 3, x);
      transform->SetElement(2, 3, y);
      }
    }

  return 1;
}




