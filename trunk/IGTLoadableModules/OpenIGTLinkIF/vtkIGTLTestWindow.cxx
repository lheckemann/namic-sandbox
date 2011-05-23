/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkOpenIGTLinkIFGUI.h $
  Date:      $Date: 2010-04-01 11:42:15 -0400 (Thu, 01 Apr 2010) $
  Version:   $Revision: 12582 $

==========================================================================*/


#include "vtkObjectFactory.h"

#include "vtkIGTLTestWindow.h"

#include "vtkSlicerApplication.h"

#include "vtkKWFrameWithLabel.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWPushButton.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLTestWindow);
vtkCxxRevisionMacro(vtkIGTLTestWindow, "$Revision: 1.0 $");
//----------------------------------------------------------------------------
vtkIGTLTestWindow::vtkIGTLTestWindow()
{
  // Logic values
  this->WindowPosition[0]=0;
  this->WindowPosition[1]=0;
  this->WindowSize[0]=0;
  this->WindowSize[1]=0;

  this->DefaultPort = 18944;
  this->DefaultFrameRate = 5.0;

  // GUI widgets
  this->MainFrame = vtkKWFrame::New();
  this->MultipleMonitorsAvailable = false; 

  this->ModeButtonSet = NULL;
  this->PortEntry = NULL;
  this->FrameRateEntry = NULL;

  this->StartTrackingButton  = NULL;
  this->StopTrackingButton = NULL;
  this->CloseButton    = NULL;

  //this->CurrentTarget=NULL;

  // GUI callback command
  this->InGUICallbackFlag = 0;
  this->IsObserverAddedFlag = 0;

  this->GUICallbackCommand = vtkCallbackCommand::New();
  this->GUICallbackCommand->SetClientData( reinterpret_cast<void *>(this) );
  this->GUICallbackCommand->SetCallback(&vtkIGTLTestWindow::GUICallback);

}


//----------------------------------------------------------------------------
vtkIGTLTestWindow::~vtkIGTLTestWindow()
{
  //if (this->CurrentTarget)
  //{
  //  this->CurrentTarget->UnRegister(this);
  //  this->CurrentTarget=NULL;
  //}

  if ( this->GUICallbackCommand != NULL )
    {
    this->GUICallbackCommand->Delete ( );
    this->GUICallbackCommand = NULL;
    }

  if (this->ModeButtonSet)
    {
    this->ModeButtonSet->SetParent(NULL);
    this->ModeButtonSet->Delete();
    }
  if (PortEntry)
    {
    this->PortEntry->SetParent(NULL);
    this->PortEntry->Delete();
    }
  if (FrameRateEntry)
    {
    this->FrameRateEntry->SetParent(NULL);
    this->FrameRateEntry->Delete();
    }
  if (this->StartTrackingButton)
    {
    this->StartTrackingButton->SetParent(NULL);
    this->StartTrackingButton->Delete();
    }
  if (this->StopTrackingButton)
    {
    this->StopTrackingButton->SetParent(NULL);
    this->StopTrackingButton->Delete();
    }
  if (this->CloseButton)
    {
    this->CloseButton->SetParent(NULL);
    this->CloseButton->Delete();
    }

  this->MainFrame->Delete();
  this->SetApplication(NULL);

}


//----------------------------------------------------------------------------
void vtkIGTLTestWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkIGTLTestWindow::GUICallback( vtkObject *caller,
                           unsigned long eid, void *clientData, void *callData )
{
  
  vtkIGTLTestWindow *self = reinterpret_cast<vtkIGTLTestWindow *>(clientData);
  
  if (self->GetInGUICallbackFlag())
    {
    }

  self->SetInGUICallbackFlag(1);
  self->ProcessGUIEvents(caller, eid, callData);
  self->SetInGUICallbackFlag(0);
  
}

//----------------------------------------------------------------------------
void vtkIGTLTestWindow::ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData)
{

  if (this->ModeButtonSet->GetWidget(0) == vtkKWRadioButton::SafeDownCast(caller)
      && event == vtkKWRadioButton::SelectedStateChangedEvent
      && this->ModeButtonSet->GetWidget(0)->GetSelectedState() == 1)
    {
    }
  else if (this->PortEntry == vtkKWEntry::SafeDownCast(caller)
           && event == vtkKWEntry::EntryValueChangedEvent)
    {
    }
  else if (this->FrameRateEntry == vtkKWEntry::SafeDownCast(caller)
           && event == vtkKWEntry::EntryValueChangedEvent)
    {
    }
  else if (this->StartTrackingButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    StartServer(1000, 10.0);
    }
  else if (this->StopTrackingButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    StopServer();
    }
  else if (this->CloseButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    this->Withdraw();
    }

}


//----------------------------------------------------------------------------
void vtkIGTLTestWindow::AddGUIObservers()
{
  this->RemoveGUIObservers();

  if (this->ModeButtonSet)
    {
    this->ModeButtonSet->GetWidget(0)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->ModeButtonSet->GetWidget(1)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->PortEntry)
    {
    this->PortEntry
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);    
    }
  if (this->FrameRateEntry)
    {
    this->FrameRateEntry
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);    
    }
  if (this->StartTrackingButton)
    {
    this->StartTrackingButton->AddObserver(vtkKWPushButton::InvokedEvent,(vtkCommand *)this->GUICallbackCommand);
    }

  if (this->StopTrackingButton)
    {
    this->StopTrackingButton->AddObserver(vtkKWPushButton::InvokedEvent,(vtkCommand *)this->GUICallbackCommand);
    }

  if (this->CloseButton)
    {
    this->CloseButton->AddObserver(vtkKWPushButton::InvokedEvent,(vtkCommand *)this->GUICallbackCommand);
    }

}


//----------------------------------------------------------------------------
void vtkIGTLTestWindow::RemoveGUIObservers()
{
  if (this->ModeButtonSet)
    {
    this->ModeButtonSet->GetWidget(0)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->ModeButtonSet->GetWidget(1)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->PortEntry)
    {
    this->PortEntry->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->FrameRateEntry)
    {
    this->FrameRateEntry->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->StartTrackingButton)
    {
    this->StartTrackingButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->StopTrackingButton)
    {
    this->StopTrackingButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CloseButton)
    {
    this->CloseButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
}



//----------------------------------------------------------------------------
void vtkIGTLTestWindow::CreateWidget()
{
  vtkSlicerApplication *app = vtkSlicerApplication::SafeDownCast(this->GetApplication());
  if (app==NULL)
  {
  vtkErrorMacro("CreateWindow: application is invalid");
  return;
  }

  if (this->IsCreated())
  {
    return;
  }

  vtkKWTopLevel::CreateWidget();

  //this->SetMasterWindow (this->GetServerMenuButton() );
  this->SetApplication ( app );
  //this->Create();
  this->SetBorderWidth ( 1 );
  this->SetReliefToFlat();

  //this->SetParent (this->GetApplicationGUI()->GetMainSlicerWindow());

  this->SetTitle ("OpenIGTLink Test");
  //this->SetSize (400, 100);
  this->Withdraw();

  this->MainFrame->SetParent ( this );
  this->MainFrame->Create();
  this->MainFrame->SetBorderWidth ( 1 );
  this->Script ( "pack %s -side top -anchor nw -fill both -expand 1 -padx 0 -pady 1", this->MainFrame->GetWidgetName() ); 


  // --------------------------------------------------
  // Tracking Test Data Source (Ramdom or File)
  vtkKWFrameWithLabel *dataSourceFrame = vtkKWFrameWithLabel::New();
  dataSourceFrame->SetParent(this->MainFrame);
  dataSourceFrame->Create();
  dataSourceFrame->SetLabelText ("Tracking Test Data Source");
  app->Script ( "pack %s -fill both -expand true",  
                dataSourceFrame->GetWidgetName());

  this->ModeButtonSet = vtkKWRadioButtonSet::New();
  this->ModeButtonSet->SetParent(dataSourceFrame->GetFrame());
  this->ModeButtonSet->Create();
  this->ModeButtonSet->PackHorizontallyOn();
  this->ModeButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->ModeButtonSet->UniformColumnsOn();
  this->ModeButtonSet->UniformRowsOn();

  this->ModeButtonSet->AddWidget(0);
  this->ModeButtonSet->GetWidget(0)->SetText("Random");
  this->ModeButtonSet->AddWidget(1);
  this->ModeButtonSet->GetWidget(1)->SetText("From file");

  this->ModeButtonSet->GetWidget(0)->SelectedStateOn();

  app->Script("pack %s -side left -anchor w -fill x -padx 2 -pady 2", 
              this->ModeButtonSet->GetWidgetName());


  // --------------------------------------------------
  // Connection Setting

  vtkKWFrameWithLabel *connectionFrame = vtkKWFrameWithLabel::New();
  connectionFrame->SetParent(this->MainFrame);
  connectionFrame->Create();
  connectionFrame->SetLabelText ("Connection Setting");
  app->Script ( "pack %s -fill both -expand true",  
                connectionFrame->GetWidgetName());

  // --- Server port entry
  vtkKWFrame *portFrame = vtkKWFrame::New();
  portFrame->SetParent(connectionFrame->GetFrame());
  portFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                portFrame->GetWidgetName());

  vtkKWLabel *portLabel = vtkKWLabel::New();
  portLabel->SetParent(portFrame);
  portLabel->Create();
  portLabel->SetWidth(8);
  portLabel->SetText("Port: ");

  this->PortEntry = vtkKWEntry::New();
  this->PortEntry->SetParent(portFrame);
  this->PortEntry->SetRestrictValueToInteger();
  this->PortEntry->Create();
  this->PortEntry->SetWidth(8);
  this->PortEntry->SetValueAsInt(this->DefaultPort);

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              portLabel->GetWidgetName() , this->PortEntry->GetWidgetName());

  // --- Frame rate
  vtkKWFrame *frameRateFrame = vtkKWFrame::New();
  frameRateFrame->SetParent(connectionFrame->GetFrame());
  frameRateFrame->Create();
  app->Script ( "pack %s -fill both -expand true",  
                frameRateFrame->GetWidgetName());

  vtkKWLabel *frameRateLabel = vtkKWLabel::New();
  frameRateLabel->SetParent(frameRateFrame);
  frameRateLabel->Create();
  frameRateLabel->SetWidth(8);
  frameRateLabel->SetText("Rate (fps): ");

  this->FrameRateEntry = vtkKWEntry::New();
  this->FrameRateEntry->SetParent(frameRateFrame);
  this->FrameRateEntry->SetRestrictValueToDouble();
  this->FrameRateEntry->Create();
  this->FrameRateEntry->SetWidth(8);
  this->FrameRateEntry->SetValueAsDouble(this->DefaultFrameRate);

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
              frameRateLabel->GetWidgetName() , this->FrameRateEntry->GetWidgetName());


  // Connector Property -- Connector type (server or client)
  vtkKWFrame *buttonFrame = vtkKWFrame::New();
  buttonFrame->SetParent(this->MainFrame);
  buttonFrame->Create();
  app->Script ( "pack %s -side top -fill both -expand true",  
                buttonFrame->GetWidgetName());

  this->StartTrackingButton = vtkKWPushButton::New();
  this->StartTrackingButton->SetParent(buttonFrame);
  this->StartTrackingButton->Create();
  this->StartTrackingButton->SetText( "Start" );
  this->StartTrackingButton->SetWidth (10);

  this->StopTrackingButton = vtkKWPushButton::New();
  this->StopTrackingButton->SetParent(buttonFrame);
  this->StopTrackingButton->Create();
  this->StopTrackingButton->SetText( "Stop" );
  this->StopTrackingButton->SetWidth (10);
  this->StopTrackingButton->SetEnabled(0);

  this->CloseButton = vtkKWPushButton::New();
  this->CloseButton->SetParent(buttonFrame);
  this->CloseButton->Create();
  this->CloseButton->SetText( "Close" );
  this->CloseButton->SetWidth (6);

  app->Script ( "pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
                this->StartTrackingButton->GetWidgetName(),
                this->StopTrackingButton->GetWidgetName(),
                this->CloseButton->GetWidgetName());

  buttonFrame->Delete();
}


//----------------------------------------------------------------------------
void vtkIGTLTestWindow::DisplayOnWindow()
{
  //-- display
  this->DeIconify();
  this->Raise();

  this->SetPosition(this->WindowPosition[0], this->WindowPosition[1]);
  //this->SetSize(this->WindowSize[0], this->WindowSize[1]);

  if (!this->IsObserverAddedFlag)
    {
    this->IsObserverAddedFlag = 1;
    this->AddGUIObservers();
    }

}


//----------------------------------------------------------------------------
void vtkIGTLTestWindow::SwitchMode(int mode)
{
  //
  // mode: either MODE_RANDOM or MODE_FILE
  //

  if (mode == this->Mode)
    {
    return;
    }
  else if (mode == MODE_RANDOM)
    {
    // do something
    }
  else //if (mode == MODE_FILE)
    {
    // do something
    }

}

//----------------------------------------------------------------------------
void vtkIGTLTestWindow::StartServer(int port, float rate)
{
  // --------------------------------------------------
  // Change GUI status
  this->StartTrackingButton->SetEnabled(0);
  this->StopTrackingButton->SetEnabled(1);
  this->PortEntry->SetEnabled(0);
  this->FrameRateEntry->SetEnabled(0);

  // --------------------------------------------------
  // Start Server
  
}

//----------------------------------------------------------------------------
void vtkIGTLTestWindow::StopServer()
{
  // --------------------------------------------------
  // Change GUI status
  this->StartTrackingButton->SetEnabled(1);
  this->StopTrackingButton->SetEnabled(0);
  this->PortEntry->SetEnabled(1);
  this->FrameRateEntry->SetEnabled(1);

  // --------------------------------------------------
  // Stop Server
  
}


