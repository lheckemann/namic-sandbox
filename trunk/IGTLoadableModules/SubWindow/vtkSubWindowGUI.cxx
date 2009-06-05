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

#include "vtkSubWindowGUI.h"
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


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkSubWindowGUI );
vtkCxxRevisionMacro ( vtkSubWindowGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkSubWindowGUI::vtkSubWindowGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkSubWindowGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->TestButton11 = NULL;
  this->TestButton12 = NULL;
  this->TestButton21 = NULL;
  this->TestButton22 = NULL;

  this->SecondaryMonitor = NULL;

  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkSubWindowGUI::~vtkSubWindowGUI ( )
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

  if (this->SecondaryMonitor)
    {
    this->SecondaryMonitor->Withdraw();
    this->SecondaryMonitor->SetApplication(NULL);
    this->SecondaryMonitor->Delete();
    this->SecondaryMonitor = NULL;
    }

  if (this->TestButton11)
    {
    this->TestButton11->SetParent(NULL);
    this->TestButton11->Delete();
    }

  if (this->TestButton12)
    {
    this->TestButton12->SetParent(NULL);
    this->TestButton12->Delete();
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
void vtkSubWindowGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkSubWindowGUI::Enter()
{
  // Fill in
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }

  this->SecondaryMonitor->DisplayOnSecondaryMonitor();

}


//---------------------------------------------------------------------------
void vtkSubWindowGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkSubWindowGUI::TearDownGUI() 
{
  if (this->SecondaryMonitor)
    {  
    this->SecondaryMonitor->Withdraw();
    this->SecondaryMonitor->SetApplication(NULL);
    }

  /*
  this->PrimaryMonitorRobotViewerWidget->SetApplication(NULL);
  this->PrimaryMonitorRobotViewerWidget->SetMainViewerWidget(NULL);
  this->PrimaryMonitorRobotViewerWidget->SetMRMLScene(NULL);
  */
}


//---------------------------------------------------------------------------
void vtkSubWindowGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "SubWindowGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkSubWindowGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->TestButton11)
    {
    this->TestButton11
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TestButton12)
    {
    this->TestButton12
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
void vtkSubWindowGUI::AddGUIObservers ( )
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

  this->TestButton11
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton12
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton21
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->TestButton22
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkSubWindowGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkSubWindowGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkSubWindowLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkSubWindowGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkSubWindowGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  
  if (this->TestButton11 == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    
    



    }
  else if (this->TestButton12 == vtkKWPushButton::SafeDownCast(caller)
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "TestButton12 is pressed." << std::endl;
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


void vtkSubWindowGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkSubWindowGUI *self = reinterpret_cast<vtkSubWindowGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkSubWindowGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkSubWindowGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkSubWindowLogic::SafeDownCast(caller))
    {
    if (event == vtkSubWindowLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkSubWindowGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }

  //if (node != NULL && this->GetMRMLNode() == node && event == vtkCommand::ModifiedEvent)
  //{
  //  int targetIndex=node->GetCurrentTargetIndex();
  //  this->SecondaryMonitor->SetCurrentTarget(node->GetTargetDescriptorAtIndex(targetIndex));
  //}
  
}


//---------------------------------------------------------------------------
void vtkSubWindowGUI::ProcessTimerEvents()
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
void vtkSubWindowGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "SubWindow", "SubWindow", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForTestFrame1();
  BuildGUIForTestFrame2();

  this->SecondaryMonitor = vtkSlicerSecondaryMonitor::New();
  this->SecondaryMonitor->SetApplication(this->GetApplication());
  this->SecondaryMonitor->Create();

}


void vtkSubWindowGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:SubWindow</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "SubWindow" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkSubWindowGUI::BuildGUIForTestFrame1()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("SubWindow");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Test Frame 1");
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

  this->TestButton11 = vtkKWPushButton::New ( );
  this->TestButton11->SetParent ( frame->GetFrame() );
  this->TestButton11->Create ( );
  this->TestButton11->SetText ("Test 11");
  this->TestButton11->SetWidth (12);

  this->TestButton12 = vtkKWPushButton::New ( );
  this->TestButton12->SetParent ( frame->GetFrame() );
  this->TestButton12->Create ( );
  this->TestButton12->SetText ("Tset 12");
  this->TestButton12->SetWidth (12);

  this->Script("pack %s %s -side left -padx 2 -pady 2", 
               this->TestButton11->GetWidgetName(),
               this->TestButton12->GetWidgetName());

  conBrowsFrame->Delete();
  frame->Delete();

}


//---------------------------------------------------------------------------
void vtkSubWindowGUI::BuildGUIForTestFrame2 ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("SubWindow");
  
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
void vtkSubWindowGUI::UpdateAll()
{
}

