/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL$
  Date:      $Date$
  Version:   $Revision$

==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkControl4DGUI.h"
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
vtkStandardNewMacro (vtkControl4DGUI );
vtkCxxRevisionMacro ( vtkControl4DGUI, "$Revision$");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkControl4DGUI::vtkControl4DGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkControl4DGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->TestButton11 = NULL;
  this->TestButton12 = NULL;
  this->TestButton21 = NULL;
  this->TestButton22 = NULL;
  
  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkControl4DGUI::~vtkControl4DGUI ( )
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
void vtkControl4DGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::Enter()
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
void vtkControl4DGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "Control4DGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::RemoveGUIObservers ( )
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
void vtkControl4DGUI::AddGUIObservers ( )
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
void vtkControl4DGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkControl4DGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkControl4DLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkControl4DGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessGUIEvents(vtkObject *caller,
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
    std::cerr << "TestButton11 is pressed." << std::endl;
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


void vtkControl4DGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkControl4DGUI *self = reinterpret_cast<vtkControl4DGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkControl4DGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkControl4DLogic::SafeDownCast(caller))
    {
    if (event == vtkControl4DLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkControl4DGUI::ProcessTimerEvents()
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
void vtkControl4DGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "Control4D", "Control4D", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForTestFrame1();
  BuildGUIForTestFrame2();

}


void vtkControl4DGUI::BuildGUIForHelpFrame ()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Control4D" );

  // Define your help text here.
  const char *help = 
    " Write your help text here.";

  // ----------------------------------------------------------------
  // HELP FRAME         
  // ----------------------------------------------------------------
  vtkSlicerModuleCollapsibleFrame *Control4DHelpFrame = vtkSlicerModuleCollapsibleFrame::New ( );
  Control4DHelpFrame->SetParent ( page );
  Control4DHelpFrame->Create ( );
  Control4DHelpFrame->CollapseFrame ( );
  Control4DHelpFrame->SetLabelText ("Help");
  app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                Control4DHelpFrame->GetWidgetName(), page->GetWidgetName());
    
  // configure the parent classes help text widget
  this->HelpText->SetParent ( Control4DHelpFrame->GetFrame() );
  this->HelpText->Create ( );
  this->HelpText->SetHorizontalScrollbarVisibility ( 0 );
  this->HelpText->SetVerticalScrollbarVisibility ( 1 );
  this->HelpText->GetWidget()->SetText ( help );
  this->HelpText->GetWidget()->SetReliefToFlat ( );
  this->HelpText->GetWidget()->SetWrapToWord ( );
  this->HelpText->GetWidget()->ReadOnlyOn ( );
  this->HelpText->GetWidget()->QuickFormattingOn ( );
  this->HelpText->GetWidget()->SetBalloonHelpString ( "" );
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 4",
                this->HelpText->GetWidgetName ( ) );

  Control4DHelpFrame->Delete();

}

//---------------------------------------------------------------------------
void vtkControl4DGUI::BuildGUIForTestFrame1()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
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
void vtkControl4DGUI::BuildGUIForTestFrame2 ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Control4D");
  
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
void vtkControl4DGUI::UpdateAll()
{
}


//----------------------------------------------------------------------------
void vtkControl4DGUI::SetForeground(int index)
{

  vtkSlicerApplicationGUI *appGUI = this->GetGUI()->GetApplicationGUI();
  
  const char* nodeID = this->GetLogic()->SwitchNodeFG(index);

  appGUI->GetMainSliceGUI("Red")->GetLogic()->GetForegroundLayer()->SetVolumeNode(nodeID);
  appGUI->GetMainSliceGUI("Yellow")->GetLogic()->GetForegroundLayer()->SetVolumeNode(nodeID);
  appGUI->GetMainSliceGUI("Green")->GetLogic()->GetForegroundLayer()->SetVolumeNode(nodeID);

}


//----------------------------------------------------------------------------
void vtkControl4DGUI::SetBackground(int index)
{

  vtkSlicerApplicationGUI *appGUI = this->GetGUI()->GetApplicationGUI();
  
  const char* nodeID = this->GetLogic()->SwitchNodeBG(index);

  appGUI->GetMainSliceGUI("Red")->GetLogic()->GetBackgroundLayer()->SetVolumeNode(nodeID);
  appGUI->GetMainSliceGUI("Yellow")->GetLogic()->GetBackgroundLayer()->SetVolumeNode(nodeID);
  appGUI->GetMainSliceGUI("Green")->GetLogic()->GetBackgroundLayer()->SetVolumeNode(nodeID);

}

