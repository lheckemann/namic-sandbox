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

#include "vtkUDPServerGUI.h"
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
#include "vtkKWEntry.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWPushButton.h"
#include "vtkCornerAnnotation.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkUDPServerGUI );
vtkCxxRevisionMacro ( vtkUDPServerGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkUDPServerGUI::vtkUDPServerGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkUDPServerGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->ConnectButton = NULL;
  this->PortEntry = NULL;
  this->DataTable = NULL;
  
  //----------------------------------------------------------------
  //Message Variables
  this->ServerConnected = 0;
  this->TimerFlag = 0;
  this->received = 0;
  this->TimerInterval = 1000;  // 100 ms

}

//---------------------------------------------------------------------------
vtkUDPServerGUI::~vtkUDPServerGUI ( )
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

  if (this->ConnectButton)
    {
    this->ConnectButton->SetParent(NULL);
    this->ConnectButton->Delete();
    }

  if (this->PortEntry)
    {
    this->PortEntry->SetParent(NULL);
    this->PortEntry->Delete();
    }

  if (this->DataTable)
    {
    this->DataTable->SetParent(NULL);
    this->DataTable->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::Enter()
{

}

//---------------------------------------------------------------------------
void vtkUDPServerGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "UDPServerGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->ConnectButton)
    {
    this->ConnectButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->PortEntry)
    {
    this->PortEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
    
  if (this->DataTable)
    {
    this->DataTable
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::AddGUIObservers ( )
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

  this->ConnectButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->PortEntry
     ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
  
  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkUDPServerGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkUDPServerLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkUDPServerGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  if (this->ConnectButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "Connect/Disconnect Button has been pressed." << std::endl;
    this->ServerConnected = !(this->ServerConnected);
    if (this->ServerConnected == 1)
      {
      //Call server connect loop to connect
      this->GetLogic()->Start(this->PortEntry->GetValueAsInt());
      if (!this->GetLogic()->GetServerStopFlag())
        {
        this->ConnectButton->SetText("Disconnect");
        }
      }
    else
      {
      this->GetLogic()->Stop();
      if (this->GetLogic()->GetServerStopFlag())
        {
        this->ConnectButton->SetText("Connect");
        }
      }
    }
  else if (this->PortEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    std::cerr << "Port has been modified." << std::endl;
    }
}

void vtkUDPServerGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkUDPServerGUI *self = reinterpret_cast<vtkUDPServerGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkUDPServerGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkUDPServerLogic::SafeDownCast(caller))
    {
    if (event == vtkUDPServerLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}

//---------------------------------------------------------------------------
void vtkUDPServerGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    // -----------------------------------------
    // Get incoming new data
    //TODO: check return value
    //this->GetLogic()->ImportData(this->PortEntry->GetValueAsInt());
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}

//---------------------------------------------------------------------------
void vtkUDPServerGUI::BuildGUI ( )
{

  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "UDPServer", "UDPServer", NULL );

  //Call Build GUI methods
  BuildGUIForHelpFrame();
  BuildGUIForServerFrame();
  BuildGUIForDataFrame();
}

//--------------------------------------------------------------------------
void vtkUDPServerGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:UDPServer</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "UDPServer" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::BuildGUIForServerFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("UDPServer");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Server Connection");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Server Frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Server Configuration");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  // -----------------------------------------
  // Port Entry Textbox
  
  vtkKWFrame *portFrame = vtkKWFrame::New();
  portFrame->SetParent(frame->GetFrame());
  portFrame->Create();
  
  vtkKWFrame *connectFrame = vtkKWFrame::New();
  connectFrame->SetParent(frame->GetFrame());
  connectFrame->Create();
  app->Script ( "pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
                portFrame->GetWidgetName(),
                connectFrame->GetWidgetName());

  vtkKWLabel *portLabel = vtkKWLabel::New();
  portLabel->SetParent(portFrame);
  portLabel->Create();
  portLabel->SetWidth(8);
  portLabel->SetText("Port: ");

  this->PortEntry = vtkKWEntry::New();
  this->PortEntry->SetParent(portFrame);
  this->PortEntry->Create();
  this->PortEntry->SetWidth(6);
  this->PortEntry->SetRestrictValueToInteger();
  this->PortEntry->SetValueAsInt(3000);
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              portLabel->GetWidgetName() , this->PortEntry->GetWidgetName());
  portLabel->Delete();

  //--------------------------------------------
  // Connect Button
  
  this->ConnectButton = vtkKWPushButton::New( );
  this->ConnectButton->SetParent(connectFrame);
  this->ConnectButton->Create( );
  this->ConnectButton->SetText("Connect");
  this->ConnectButton->SetWidth(12);

  this->Script("pack %s -padx 2 -pady 2", 
               this->ConnectButton->GetWidgetName());

  connectFrame->Delete();
  conBrowsFrame->Delete();
  frame->Delete();

}

//----------------------------------------------------------------------------
void vtkUDPServerGUI::BuildGUIForDataFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget("UDPServer");
  
  vtkSlicerModuleCollapsibleFrame *dataBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  dataBrowsFrame->SetParent(page);
  dataBrowsFrame->Create();
  dataBrowsFrame->SetLabelText("Data Frame");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               dataBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Data Table Widget

  this->DataTable = vtkKWMultiColumnListWithScrollbars::New();
  this->DataTable->SetParent(dataBrowsFrame->GetFrame());
  this->DataTable->Create();
  this->DataTable->SetHeight(1);
  this->DataTable->GetWidget()->SetSelectionTypeToRow();
  this->DataTable->GetWidget()->SetSelectionModeToSingle();
  this->DataTable->GetWidget()->MovableRowsOff();
  this->DataTable->GetWidget()->MovableColumnsOff();

  const char* labels[] =
    { "Date", "Time", "Smoothed", "Beta", "Gamma"};
  const int widths[] = 
    { 8, 8, 11, 7, 7 };

  for (int col = 0; col < 5; col ++)
    {
    this->DataTable->GetWidget()->AddColumn(labels[col]);
    this->DataTable->GetWidget()->SetColumnWidth(col, widths[col]);
    this->DataTable->GetWidget()->SetColumnAlignmentToLeft(col);
    this->DataTable->GetWidget()->ColumnEditableOff(col);
    this->DataTable->GetWidget()->SetColumnEditWindowToSpinBox(col);
    }
  this->DataTable->GetWidget()->SetColumnEditWindowToCheckButton(0);
  
   app->Script ("pack %s -fill both -expand true",  
               this->DataTable->GetWidgetName());
  
  dataBrowsFrame->Delete();
}
  


//----------------------------------------------------------------------------
void vtkUDPServerGUI::UpdateAll()
{
}

