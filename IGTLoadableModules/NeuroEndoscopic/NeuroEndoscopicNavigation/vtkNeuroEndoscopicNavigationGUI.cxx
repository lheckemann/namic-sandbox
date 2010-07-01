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

#include "vtkNeuroEndoscopicNavigationGUI.h"
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

#include <igtlOSUtil.h>
#include <igtlTransformMessage.h>
#include <igtlClientSocket.h>


//---------------------------------------------------------------------------
vtkStandardNewMacro ( vtkNeuroEndoscopicNavigationGUI );
vtkCxxRevisionMacro ( vtkNeuroEndoscopicNavigationGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkNeuroEndoscopicNavigationGUI::vtkNeuroEndoscopicNavigationGUI()
{
  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkNeuroEndoscopicNavigationGUI::DataCallback);

  // OpenIGTLink
  this->OpenIGTLinkHost = NULL;
  this->OpenIGTLinkPort = NULL;
  this->OpenIGTLinkConnectButton = NULL;
  this->OpenIGTLinkDisconnectButton = NULL;
  
  // RS232
  this->RS232BitRate = NULL;
  this->RS232Parity = NULL;
  this->RS232ApplyButton = NULL;

  // Send Commands
  this->CommandType = NULL;
  this->Commands = NULL;
  this->SendCommandButton = NULL;
  this->StopReceivingData = NULL;

}

//---------------------------------------------------------------------------
vtkNeuroEndoscopicNavigationGUI::~vtkNeuroEndoscopicNavigationGUI()
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

    // OpenIGTLink
  if(this->OpenIGTLinkHost)
    {
    this->OpenIGTLinkHost->SetParent(NULL);
    this->OpenIGTLinkHost->Delete();
    }

  if(this->OpenIGTLinkPort)
    {
    this->OpenIGTLinkPort->SetParent(NULL);
    this->OpenIGTLinkPort->Delete();
    }
  
  if(this->OpenIGTLinkConnectButton)
    {
    this->OpenIGTLinkConnectButton->SetParent(NULL);
    this->OpenIGTLinkConnectButton->Delete();
    }

  if(this->OpenIGTLinkDisconnectButton)
    {
    this->OpenIGTLinkDisconnectButton->SetParent(NULL);
    this->OpenIGTLinkDisconnectButton->Delete();
    }

    // RS232

  if(this->RS232BitRate)
    {
    this->RS232BitRate->SetParent(NULL);
    this->RS232BitRate->Delete();
    }
 
  if(this->RS232Parity)
    {
    this->RS232Parity->SetParent(NULL);
    this->RS232Parity->Delete();
    }

  if(this->RS232ApplyButton)
    {
    this->RS232ApplyButton->SetParent(NULL);
    this->RS232ApplyButton->Delete();
    }

    // Send Commands

  if(this->CommandType)
    {
    this->CommandType->SetParent(NULL);
    this->CommandType->Delete();
    }

  if(this->Commands)
    {
    this->Commands->SetParent(NULL);
    this->Commands->Delete();
    }

  if(this->SendCommandButton)
    {
    this->SendCommandButton->SetParent(NULL);
    this->SendCommandButton->Delete();
    }
  
  if(this->StopReceivingData)
    {
    this->StopReceivingData->SetParent(NULL);
    this->StopReceivingData->Delete();
    }

  this->SetModuleLogic(NULL);

}

//---------------------------------------------------------------------------
void vtkNeuroEndoscopicNavigationGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkNeuroEndoscopicNavigationGUI::Enter()
{
 
}


//---------------------------------------------------------------------------
void vtkNeuroEndoscopicNavigationGUI::Exit ( )
{
  
}


//---------------------------------------------------------------------------
void vtkNeuroEndoscopicNavigationGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "NeuroEndoscopicNavigationGUI: " << this->GetClassName ( ) << "\n";
  // os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//----------------------------------------------------------------
// Observer Management
//----------------------------------------------------------------

void vtkNeuroEndoscopicNavigationGUI::RemoveGUIObservers()
{

  // OpenIGTLink 
  if (this->OpenIGTLinkConnectButton)
    {
    this->OpenIGTLinkConnectButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->OpenIGTLinkDisconnectButton)
    {
    this->OpenIGTLinkDisconnectButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }


  // RS232
  if (this->RS232BitRate)
    {
    this->RS232BitRate
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->RS232Parity)
    {
    this->RS232Parity
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->RS232ApplyButton)
    {
    this->RS232ApplyButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  // Send Commands
  if (this->CommandType)
    {
    this->CommandType
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->Commands)
    {
    this->Commands
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->SendCommandButton)
    {
    this->SendCommandButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  // this->RemoveLogicObservers();

}

void vtkNeuroEndoscopicNavigationGUI::AddGUIObservers()
{
  //TODO: Check not NULL before adding observer
  this->RemoveGUIObservers();

  // GUI Observers
    // OpenIGTLink
  if(this->OpenIGTLinkConnectButton)
    {
    this->OpenIGTLinkConnectButton
        ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->OpenIGTLinkDisconnectButton)
    {  
    this->OpenIGTLinkDisconnectButton
        ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
 
    // RS232

  if(this->RS232ApplyButton)
    {
    this->RS232ApplyButton
        ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

    // Send Commands

  if(this->SendCommandButton)
    {
    this->SendCommandButton
        ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if(this->StopReceivingData)
    {   
    this->StopReceivingData
        ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }   

      // this->AddLogicObservers();
 
}


//----------------------------------------------------------------
// Event Handler
//----------------------------------------------------------------

void vtkNeuroEndoscopicNavigationGUI::ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData )
{

  //const char *eventName = vtkCommand::GetStringFromEventId(event);

/*
  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }
*/

  // OpenIGTLink
  
  if (this->OpenIGTLinkConnectButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent){
      
    std::cout << "OpenIGTLink: Connection Button has been pressed." << std::endl;

    if(this->OpenIGTLinkHost->GetWidget()->GetValue() && this->OpenIGTLinkPort->GetWidget()->GetValueAsInt())
      {
      igtl::ClientSocket::Pointer socket;
      socket = igtl::ClientSocket::New();
      int r = socket->ConnectToServer(this->OpenIGTLinkHost->GetWidget()->GetValue(),(int)this->OpenIGTLinkPort->GetWidget()->GetValueAsInt());
 
      if(r!=0)
     {
        std::cout << "# Connexion to the server FAILED" << std::endl;
     }
      else
     {
     std::cout << "# Connexion to the server SUCCESSED" << std::endl;
     }

      }
  }

  if (this->OpenIGTLinkDisconnectButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent){
      
    std::cout << "OpenIGTLink: Disconnection Button has been pressed." << std::endl;

    //TODO: Close Socket
  }

  // RS232

  if (this->RS232ApplyButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent){
      
    std::cout << "RS232: Apply Settings Button has been pressed." << std::endl;
  }

  // Send Commands

  if (this->SendCommandButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent){
      
    std::cout << "Commands: Start Data Retrieving Button has been pressed." << std::endl;
  }

  if (this->StopReceivingData == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent){
      
    std::cout << "Commands: Stop Data Retrieving Button has been pressed." << std::endl;
  }



}
 

void vtkNeuroEndoscopicNavigationGUI::DataCallback(vtkObject *caller, 
                           unsigned long eid, void *clientData, void *callData)
{
  vtkNeuroEndoscopicNavigationGUI *self = reinterpret_cast<vtkNeuroEndoscopicNavigationGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkNeuroEndoscopicNavigationGUI DataCallback");
  // self->UpdateAll();
}

 
//----------------------------------------------------------------
// Build Frames
//----------------------------------------------------------------

void vtkNeuroEndoscopicNavigationGUI::BuildGUI()
{

  this->UIPanel->AddPage("NeuroEndoscopicNavigation","NeuroEndoscopicNavigation",NULL);

  BuildGUIForHelpFrame();
  BuildNeuroEndoscopicNavigationFrame();

}

void vtkNeuroEndoscopicNavigationGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "No Help for the moment. Sorry";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "NeuroEndoscopicNavigation" );
  this->BuildHelpAndAboutFrame (page, help, about);
}

void vtkNeuroEndoscopicNavigationGUI::BuildNeuroEndoscopicNavigationFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("NeuroEndoscopicNavigation");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("NeuroEndoscopic Navigation");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());


  // OpenIGTLink Frame


  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText("OpenIGTLink Connection"); 

  this->Script ( "pack %s -side top -fill x -anchor nw -expand n -padx 2 -pady 2",
                 frame->GetWidgetName());
 

  vtkKWFrame *frame2 = vtkKWFrame::New();
  frame2->SetParent(frame->GetFrame());
  frame2->Create();

  vtkKWFrame *frame3 = vtkKWFrame::New();
  frame3->SetParent(frame->GetFrame());
  frame3->Create();

  this->Script ( "pack %s %s -side top -fill x -anchor nw -expand n -padx 2 -pady 2",
                 frame2->GetWidgetName(),
                 frame3->GetWidgetName());

  this->OpenIGTLinkHost = vtkKWEntryWithLabel::New();
  this->OpenIGTLinkHost->SetParent(frame2);
  this->OpenIGTLinkHost->Create();
  this->OpenIGTLinkHost->SetLabelText("Host:");
  this->OpenIGTLinkHost->SetLabelPositionToTop();
  this->OpenIGTLinkHost->GetWidget()->SetValue("localhost");

  this->OpenIGTLinkPort = vtkKWEntryWithLabel::New();
  this->OpenIGTLinkPort->SetParent(frame2);
  this->OpenIGTLinkPort->Create();
  this->OpenIGTLinkPort->SetLabelText("Port:");
  this->OpenIGTLinkPort->SetLabelPositionToTop();
  this->OpenIGTLinkPort->GetWidget()->SetWidth(5);
  this->OpenIGTLinkPort->GetWidget()->SetRestrictValueToInteger();
  this->OpenIGTLinkPort->GetWidget()->SetValue("18944");

  this->Script("pack %s %s -side left -fill x -padx 2 -pady 2", 
            this->OpenIGTLinkHost->GetWidgetName(),
               this->OpenIGTLinkPort->GetWidgetName());

  this->OpenIGTLinkConnectButton = vtkKWPushButton::New();
  this->OpenIGTLinkConnectButton->SetParent(frame3);
  this->OpenIGTLinkConnectButton->Create();
  this->OpenIGTLinkConnectButton->SetText("Connect");

  this->OpenIGTLinkDisconnectButton = vtkKWPushButton::New();
  this->OpenIGTLinkDisconnectButton->SetParent(frame3);
  this->OpenIGTLinkDisconnectButton->Create();
  this->OpenIGTLinkDisconnectButton->SetText("Disconnect");


  this->Script("pack %s %s -side right -anchor nw -expand n -padx 2 -pady 2", 
            this->OpenIGTLinkDisconnectButton->GetWidgetName(),
               this->OpenIGTLinkConnectButton->GetWidgetName());


  // RS232 Frame

  vtkKWFrameWithLabel *frame4 = vtkKWFrameWithLabel::New();
  frame4->SetParent(conBrowsFrame->GetFrame());
  frame4->Create();
  frame4->SetLabelText("RS232 Connection"); 

  this->Script ( "pack %s -side top -fill x -anchor nw -expand n -padx 2 -pady 2",
                 frame4->GetWidgetName());
 

  vtkKWFrame *frame5 = vtkKWFrame::New();
  frame5->SetParent(frame4->GetFrame());
  frame5->Create();

  vtkKWFrame *frame6 = vtkKWFrame::New();
  frame6->SetParent(frame4->GetFrame());
  frame6->Create();

  this->Script ( "pack %s %s -side top -fill x -anchor nw -expand n -padx 2 -pady 2",
                 frame5->GetWidgetName(),
                 frame6->GetWidgetName());

  this->RS232BitRate = vtkKWMenuButtonWithLabel::New();
  this->RS232BitRate->SetParent(frame5);
  this->RS232BitRate->Create();
  this->RS232BitRate->SetLabelText("Bit Rate (bauds):");
  this->RS232BitRate->SetLabelPositionToTop();
  this->RS232BitRate->GetWidget()->SetWidth(15);
  this->RS232BitRate->GetWidget()->GetMenu()->AddRadioButton("9600");
  this->RS232BitRate->GetWidget()->GetMenu()->AddRadioButton("19200");
  this->RS232BitRate->GetWidget()->GetMenu()->AddRadioButton("38400");
  this->RS232BitRate->GetWidget()->GetMenu()->AddRadioButton("57600");
  this->RS232BitRate->GetWidget()->GetMenu()->AddRadioButton("76800");
  this->RS232BitRate->GetWidget()->GetMenu()->AddRadioButton("115200");
  this->RS232BitRate->GetWidget()->SetValue("115200");

  this->RS232Parity = vtkKWMenuButtonWithLabel::New();
  this->RS232Parity->SetParent(frame5);
  this->RS232Parity->Create();
  this->RS232Parity->SetLabelText("Parity:");
  this->RS232Parity->SetLabelPositionToTop();
  this->RS232Parity->GetWidget()->SetWidth(15);
  this->RS232Parity->GetWidget()->GetMenu()->AddRadioButton("Ignore");
  this->RS232Parity->GetWidget()->GetMenu()->AddRadioButton("Odd");
  this->RS232Parity->GetWidget()->GetMenu()->AddRadioButton("Even");
  this->RS232Parity->GetWidget()->SetValue("Ignore");

  this->Script("pack %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
               this->RS232BitRate->GetWidgetName(),
               this->RS232Parity->GetWidgetName());

  this->RS232ApplyButton = vtkKWPushButton::New();
  this->RS232ApplyButton->SetParent(frame6);
  this->RS232ApplyButton->Create();
  this->RS232ApplyButton->SetText("Apply Settings");

  this->Script("pack %s -side right -anchor nw -expand n -padx 2 -pady 2", 
            this->RS232ApplyButton->GetWidgetName());


  // Send Commands Frame
  
  vtkKWFrameWithLabel *frame7 = vtkKWFrameWithLabel::New();
  frame7->SetParent(conBrowsFrame->GetFrame());
  frame7->Create();
  frame7->SetLabelText("Send Commands"); 

  this->Script ( "pack %s -side top -fill x -anchor nw -expand n -padx 2 -pady 2",
                 frame7->GetWidgetName());
 

  vtkKWFrame *frame8 = vtkKWFrame::New();
  frame8->SetParent(frame7->GetFrame());
  frame8->Create();

  vtkKWFrame *frame9 = vtkKWFrame::New();
  frame9->SetParent(frame7->GetFrame());
  frame9->Create();

  this->Script ( "pack %s %s -side top -fill x -anchor nw -expand n -padx 2 -pady 2",
                 frame8->GetWidgetName(),
                 frame9->GetWidgetName());

  this->CommandType = vtkKWMenuButtonWithLabel::New();
  this->CommandType->SetParent(frame8);
  this->CommandType->Create();
  this->CommandType->SetLabelText("Data Type:");
  this->CommandType->SetLabelPositionToTop();
  this->CommandType->GetWidget()->SetWidth(15);
  this->CommandType->GetWidget()->GetMenu()->AddRadioButton("Stream");
  this->CommandType->GetWidget()->GetMenu()->AddRadioButton("Point by Point");
  this->CommandType->GetWidget()->SetValue("Point by Point");

  this->Commands = vtkKWMenuButtonWithLabel::New();
  this->Commands->SetParent(frame8);
  this->Commands->Create();
  this->Commands->SetLabelText("Data Format:");
  this->Commands->SetLabelPositionToTop();
  this->Commands->GetWidget()->SetWidth(15);
  this->Commands->GetWidget()->GetMenu()->AddRadioButton("Position");
  this->Commands->GetWidget()->GetMenu()->AddRadioButton("Quaternion");
  this->Commands->GetWidget()->GetMenu()->AddRadioButton("Matrix 4x4");
  this->Commands->GetWidget()->SetValue("Matrix 4x4");

  this->Script("pack %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
               this->CommandType->GetWidgetName(),
               this->Commands->GetWidgetName());

  this->SendCommandButton = vtkKWPushButton::New();
  this->SendCommandButton->SetParent(frame9);
  this->SendCommandButton->Create();
  this->SendCommandButton->SetText("Start Data Retrieving");

  this->StopReceivingData = vtkKWPushButton::New();
  this->StopReceivingData->SetParent(frame9);
  this->StopReceivingData->Create();
  this->StopReceivingData->SetText("Stop Data Retrieving");

  this->Script("pack %s %s -side right -anchor nw -expand n -padx 2 -pady 2", 
            this->StopReceivingData->GetWidgetName(),
               this->SendCommandButton->GetWidgetName());
  

  conBrowsFrame->Delete();
  frame2->Delete();
  frame3->Delete();
  frame->Delete();

  frame4->Delete();
  frame5->Delete();
  frame6->Delete();  

  frame7->Delete();
  frame8->Delete();
  frame9->Delete();  


}

