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

#include "vtkCaptureBetaProbeGUI.h"
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
#include "vtkKWFileBrowserDialog.h"
#include "vtkKWEvent.h"

#include "vtkKWPushButton.h"
#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkMRMLLinearTransformNode.h"
#include "vtkMatrix4x4.h"

#include "vtkCornerAnnotation.h"


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkCaptureBetaProbeGUI );
vtkCxxRevisionMacro ( vtkCaptureBetaProbeGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkCaptureBetaProbeGUI::vtkCaptureBetaProbeGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkCaptureBetaProbeGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->Capture = NULL;
  this->Start_Button = NULL;
  this->Stop_Button = NULL;
  this->CounterNode = NULL;
  this->TrackerNode = NULL;
  this->Capture_status = NULL;
  this->FileSelector = NULL;
  this->SelectFile = NULL;
  this->CloseFile = NULL;

  this->Counts = NULL;
  this->Probe_Position = NULL;
  this->Probe_Matrix = NULL;
  
  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

  this->SetContinuousMode(false);

  //----------------------------------------------------------------
  // File
  //this->BetaProbeCountsWithTimestamp = NULL;
  // this->BetaProbeCountsWithTimestamp << "Smoothed \t Beta \t Gamma \t X \t Y \t Z \t Time" << std::endl;
}

//---------------------------------------------------------------------------
vtkCaptureBetaProbeGUI::~vtkCaptureBetaProbeGUI ( )
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

  if (this->Capture)
    {
    this->Capture->SetParent(NULL);
    this->Capture->Delete();
    }

  if (this->Start_Button)
    {
    this->Start_Button->SetParent(NULL);
    this->Start_Button->Delete();
    }

  if (this->Stop_Button)
    {
    this->Stop_Button->SetParent(NULL);
    this->Stop_Button->Delete();
    }

  if (this->CounterNode)
    {
    this->CounterNode->SetParent(NULL);
    this->CounterNode->Delete();
    }

  if (this->TrackerNode)
    {
    this->TrackerNode->SetParent(NULL);
    this->TrackerNode->Delete();
    }

  if (this->Capture_status)
    {
    this->Capture_status->SetParent(NULL);
    this->Capture_status->Delete();
    }

  if (this->FileSelector)
    {
    this->FileSelector->SetParent(NULL);
    this->FileSelector->Delete();
    }

  if (this->SelectFile)
    {
    this->SelectFile->SetParent(NULL);
    this->SelectFile->Delete();
    }

  if (this->CloseFile)
    {
    this->CloseFile->SetParent(NULL);
    this->CloseFile->Delete();
    }
  /*  

  // DO NOT DELETE THESE ITEMS, BECAUSE THEY ARE JUST CAST, AND IF YOU DELETE THEM,
  // THEY COULDN'T BE DELETED ON THEIR OWN PROGRAM, CREATING A SEGFAULT WHEN SLICER QUIT.

  if(this->Counts)
    {
    this->Counts->Delete();
    }
  
  if(this->Probe_Position)
    {
    this->Probe_Position->Delete();
    }
  */
  if(this->Probe_Matrix)
    {
    this->Probe_Matrix->Delete();
    }
  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

  if(this->BetaProbeCountsWithTimestamp.is_open())
    {  
    this->BetaProbeCountsWithTimestamp.close();
    }
}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::Enter()
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
void vtkCaptureBetaProbeGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "CaptureBetaProbeGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->Capture)
    {
    this->Capture
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->Start_Button)
    {
    this->Start_Button
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->Stop_Button)
    {
    this->Stop_Button
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->CounterNode)
    {
    this->CounterNode
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->TrackerNode)
    {
    this->TrackerNode
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->FileSelector)
    {
    this->FileSelector
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  
  if (this->SelectFile)
    {
    this->SelectFile
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  
  if (this->CloseFile)
    {
    this->CloseFile
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::AddGUIObservers ( )
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
  if(this->Capture)
    {
  this->Capture
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->Start_Button)
    {
  this->Start_Button
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->Stop_Button)
    {
  this->Stop_Button
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }    

  if(this->CounterNode)
    {
this->CounterNode
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->TrackerNode)
    {
  this->TrackerNode
    ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->FileSelector)
    {
  this->FileSelector
    ->AddObserver(vtkKWFileBrowserDialog::FileNameChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->SelectFile)
    {
this->SelectFile
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->CloseFile)
    {
this->CloseFile
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkCaptureBetaProbeLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  
  if (this->Capture == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "Capture is pressed." << std::endl;
    if(this->Counts && this->Probe_Position)
      {
      this->Capture_Data();
      }
    else
      {
      this->Capture_status->SetText("Sorry. Node is missing (BetaProbe or Tracker)");
      }
    }
  else if (this->Start_Button == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "Start Button is pressed." << std::endl;
    if(this->Counts && this->Probe_Position)
      {
      this->SetContinuousMode(true);
      this->Capture->SetState(0);
      this->Start_Button->SetState(0);
      this->Stop_Button->SetState(1);
      }
    else
      {
      this->Capture_status->SetText("Sorry. Node is missing (BetaProbe or Tracker)");
      }   
    }
  else if (this->Stop_Button == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "Start Button is pressed." << std::endl;
    if(this->Counts && this->Probe_Position)
      {
      this->SetContinuousMode(false);
      this->Capture->SetState(1);
      this->Start_Button->SetState(1);
      this->Stop_Button->SetState(0);
      }
    else
      {
      this->Capture_status->SetText("Sorry. Node is missing (BetaProbe or Tracker)");
      }   
    } 
  else if (this->SelectFile == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    { 
    this->FileSelector = vtkKWFileBrowserDialog::New();
    this->FileSelector->SetApplication(this->GetApplication()); 
    this->FileSelector->Create(); 
    this->FileSelector->SaveDialogOn();   
    this->FileSelector->Invoke();

    if (this->FileSelector->GetStatus()==vtkKWDialog::StatusOK)
      {
      std::string filename = this->FileSelector->GetFileName();
      this->BetaProbeCountsWithTimestamp.open(filename.c_str());
      if(this->BetaProbeCountsWithTimestamp.is_open())
     {
     std::stringstream open_file_succeed;
     open_file_succeed << "File opened [" << filename.c_str() << "]";
     this->Capture_status->SetText(open_file_succeed.str().c_str());
     }
      else
     {
     this->Capture_status->SetText("Failed to open file");
     }
      }

    }
  else if (this->CloseFile == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
      if(this->BetaProbeCountsWithTimestamp.is_open())
      {
      std::string filename_c = this->FileSelector->GetFileName();
      this->BetaProbeCountsWithTimestamp.close(); 
      std::stringstream close_file_succeed;
      close_file_succeed << "File closed [" << filename_c.c_str() << "]";
      this->Capture_status->SetText(close_file_succeed.str().c_str());
      }
    }
  else if (this->CounterNode == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    std::cerr << "Counter Node changed." << std::endl;
    if(this->CounterNode->GetSelected())
      {
      this->Counts = vtkMRMLUDPServerNode::SafeDownCast(this->CounterNode->GetSelected());
      std::stringstream counter_selected;
      counter_selected << this->CounterNode->GetSelected()->GetName();
      counter_selected << " selected";
      this->Capture_status->SetText(counter_selected.str().c_str());
      }
    }
  else if (this->TrackerNode == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
      && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent)
    {
    std::cerr << "Tracker Node changed." << std::endl;
    if(this->TrackerNode->GetSelected())
      {
      this->Probe_Position = vtkMRMLLinearTransformNode::SafeDownCast(this->TrackerNode->GetSelected());
      std::stringstream tracker_selected;
      tracker_selected << this->TrackerNode->GetSelected()->GetName();
      tracker_selected << " selected";
      this->Capture_status->SetText(tracker_selected.str().c_str());
      } 
    }

} 


void vtkCaptureBetaProbeGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkCaptureBetaProbeGUI *self = reinterpret_cast<vtkCaptureBetaProbeGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkCaptureBetaProbeGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkCaptureBetaProbeLogic::SafeDownCast(caller))
    {
    if (event == vtkCaptureBetaProbeLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }

  if(this->GetContinuousMode())
    {
      this->Capture_Data();
    }
  
}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "CaptureBetaProbe", "CaptureBetaProbe", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForTestFrame1();
  //  BuildGUIForTestFrame2();

}


void vtkCaptureBetaProbeGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:CaptureBetaProbe</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "CaptureBetaProbe" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::BuildGUIForTestFrame1()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("CaptureBetaProbe");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Capturing Data from Beta Probe");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Test child frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Capturing");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  vtkKWFrame *frame4 = vtkKWFrame::New();
  frame4->SetParent(frame->GetFrame());
  frame4->Create();
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
       frame4->GetWidgetName() );


  this->CounterNode = vtkSlicerNodeSelectorWidget::New();
  this->CounterNode->SetParent(frame4);
  this->CounterNode->Create();
  this->CounterNode->SetWidth(30);
  this->CounterNode->SetNewNodeEnabled(0);
  this->CounterNode->SetNodeClass("vtkMRMLUDPServerNode",NULL,NULL,NULL);
  this->CounterNode->SetMRMLScene(this->Logic->GetMRMLScene());
  this->CounterNode->UpdateMenu();

  this->TrackerNode = vtkSlicerNodeSelectorWidget::New();
  this->TrackerNode->SetParent(frame4);
  this->TrackerNode->Create();
  this->TrackerNode->SetWidth(30);
  this->TrackerNode->SetNewNodeEnabled(0);
  this->TrackerNode->SetNodeClass("vtkMRMLLinearTransformNode",NULL,NULL,NULL);
  this->TrackerNode->SetMRMLScene(this->Logic->GetMRMLScene());
  this->TrackerNode->UpdateMenu();

  app->Script("pack %s %s -fill x -side top -padx 2 -pady 2", 
               this->TrackerNode->GetWidgetName(),
               this->CounterNode->GetWidgetName());

  vtkKWFrame *frame5 = vtkKWFrame::New();
  frame5->SetParent(frame->GetFrame());
  frame5->Create();
  app->Script("pack %s -fill x -side top -padx 2 -pady 2", 
             frame5->GetWidgetName());

  this->SelectFile = vtkKWPushButton::New();
  this->SelectFile->SetParent(frame5);
  this->SelectFile->Create();
  this->SelectFile->SetText("Select output file");
  this->SelectFile->SetWidth(30);

  this->CloseFile = vtkKWPushButton::New();
  this->CloseFile->SetParent(frame5);
  this->CloseFile->Create();
  this->CloseFile->SetText("Close file");
  this->CloseFile->SetWidth(30);

  app->Script("pack %s %s -fill x -side left -padx 2 -pady 2", 
           this->SelectFile->GetWidgetName(),
              this->CloseFile->GetWidgetName());
          

  vtkKWFrame *frame2 = vtkKWFrame::New();
  frame2->SetParent(frame->GetFrame());
  frame2->Create();
  app->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
       frame2->GetWidgetName() );


  this->Capture = vtkKWPushButton::New();           
  this->Capture->SetParent(frame2);      
  this->Capture->Create();                     
  this->Capture->SetText("Capture");                
  this->Capture->SetWidth(18);                     
                                                        
  this->Start_Button = vtkKWPushButton::New();      
  this->Start_Button->SetParent(frame2);      
  this->Start_Button->Create();                
  this->Start_Button->SetState(1);                
  this->Start_Button->SetText("Start");           
  this->Start_Button->SetWidth(18);                
                                                       
  this->Stop_Button = vtkKWPushButton::New();           
  this->Stop_Button->SetParent(frame2);      
  this->Stop_Button->Create();                
  this->Stop_Button->SetState(0);                
  this->Stop_Button->SetText("Stop");                
  this->Stop_Button->SetWidth(19);                      

  app->Script("pack %s %s %s -fill x -side left -padx 2 -pady 2", 
               this->Capture->GetWidgetName(),
            this->Start_Button->GetWidgetName(),
            this->Stop_Button->GetWidgetName());


  vtkKWFrame *frame3 = vtkKWFrame::New();
  frame3->SetParent(frame->GetFrame());
  frame3->Create();
  app->Script("pack %s -fill x -side top -padx 2 -pady 2", 
             frame3->GetWidgetName());
 

  this->Capture_status = vtkKWLabel::New();             
  this->Capture_status->SetParent(frame3);
  this->Capture_status->Create();

  app->Script("pack %s -fill x -side top -padx 2 -pady 2", 
               this->Capture_status->GetWidgetName());

  conBrowsFrame->Delete();
  frame->Delete();
  frame2->Delete();
  frame3->Delete();
  frame4->Delete();
  frame5->Delete();
}


//---------------------------------------------------------------------------
/*
void vtkCaptureBetaProbeGUI::BuildGUIForTestFrame2 ()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("CaptureBetaProbe");
  
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

*/
//----------------------------------------------------------------------------
void vtkCaptureBetaProbeGUI::UpdateAll()
{
}

void vtkCaptureBetaProbeGUI::Capture_Data()
{                                                            
  if(this->BetaProbeCountsWithTimestamp.is_open())
    {   
    this->Probe_Matrix = vtkMatrix4x4::New();                                   
    
    time_t now = time(0);                         
    struct tm *current = localtime(&now);                                      
    char mytime[20];                                                  
    sprintf(mytime, "%.2d:%.2d:%.2d", current->tm_hour, current->tm_min, current->tm_sec);

    this->Probe_Position->GetMatrixTransformToWorld(this->Probe_Matrix);                 
    this->BetaProbeCountsWithTimestamp << this->Counts->GetSmoothedCounts()   << "\t\t"          
                           << this->Counts->GetBetaCounts()       << "\t\t"          
                           << this->Counts->GetGammaCounts()      << "\t\t"          
                           << this->Probe_Matrix->GetElement(0,3) << "\t\t"          
                           << this->Probe_Matrix->GetElement(1,3) << "\t\t"          
                           << this->Probe_Matrix->GetElement(2,3) << "\t\t"          
                                       << mytime                              
                                       << std::endl;
  
    this->Probe_Matrix->Delete();                                
    this->Probe_Matrix = NULL;     

    this->Capture_status->SetText("Data captured");                                              }
  else
    {
    this->Capture_status->SetText("Select a file first");
    }
}





