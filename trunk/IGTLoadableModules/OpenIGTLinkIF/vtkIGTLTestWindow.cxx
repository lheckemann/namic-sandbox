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
#include "vtkSlicerTheme.h"

#include "vtkRenderer.h"

#include "vtkCornerAnnotation.h"

#include "vtkKWFrameWithLabel.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"

#include "vtkMRMLIGTLQueryNode.h"
#include "vtkMRMLImageMetaListNode.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLTestWindow);
vtkCxxRevisionMacro(vtkIGTLTestWindow, "$Revision: 1.0 $");
//----------------------------------------------------------------------------
vtkIGTLTestWindow::vtkIGTLTestWindow()
{
  this->MainFrame = vtkKWFrame::New();
  this->MultipleMonitorsAvailable = false; 
  this->WindowPosition[0]=0;
  this->WindowPosition[1]=0;
  this->WindowSize[0]=0;
  this->WindowSize[1]=0;

  // GUI widgets
  this->ModeButtonSet = NULL;


  this->RemoteDataList  = NULL;
  this->StartTrackingButton  = NULL;
  this->StopTrackingButton = NULL;
  this->CloseButton    = NULL;

  //this->CurrentTarget=NULL;

  // GUI callback command
  this->InGUICallbackFlag = 0;
  this->InMRMLCallbackFlag = 0;
  this->IsObserverAddedFlag = 0;

  this->GUICallbackCommand = vtkCallbackCommand::New();
  this->GUICallbackCommand->SetClientData( reinterpret_cast<void *>(this) );
  this->GUICallbackCommand->SetCallback(&vtkIGTLTestWindow::GUICallback);

  this->MRMLObserverManager = vtkObserverManager::New();
  this->MRMLObserverManager->GetCallbackCommand()->SetClientData( reinterpret_cast<void *> (this) );
  this->MRMLObserverManager->GetCallbackCommand()->SetCallback(vtkIGTLTestWindow::MRMLCallback);
  this->MRMLCallbackCommand = this->MRMLObserverManager->GetCallbackCommand();

  this->ModuleGUI = NULL; 
  this->MRMLScene = NULL;

  this->Connector = NULL;
  this->TrackingDataQueryNode = NULL;

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
    this->RemoteDataList->Delete();
    }

  if (this->RemoteDataList)
    {
    this->RemoteDataList->SetParent(NULL);
    this->RemoteDataList->Delete();
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

  this->MRMLObserverManager->Delete();
  this->MainFrame->Delete();
  this->SetApplication(NULL);

  this->ModuleGUI = NULL; 
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
void vtkIGTLTestWindow::MRMLCallback(vtkObject *caller, 
                                    unsigned long eid, void *clientData, void *callData)
{
  
  vtkIGTLTestWindow *self = reinterpret_cast<vtkIGTLTestWindow *>(clientData);
  
  if (self->GetInMRMLCallbackFlag())
    {
    return;
    }

  self->SetInMRMLCallbackFlag(1);
  self->ProcessMRMLEvents(caller, eid, callData);
  self->SetInMRMLCallbackFlag(0);
}


void vtkIGTLTestWindow::ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData)
{

  if (this->ModeButtonSet->GetWidget(0) == vtkKWRadioButton::SafeDownCast(caller)
      && event == vtkKWRadioButton::SelectedStateChangedEvent
      && this->ModeButtonSet->GetWidget(0)->GetSelectedState() == 1)
    {
    
    }

  else if (this->StartTrackingButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    if (this->MRMLScene && this->Connector)
      {
      if (this->TrackingDataQueryNode == NULL)
        {
        this->TrackingDataQueryNode = vtkMRMLIGTLQueryNode::New();
        this->TrackingDataQueryNode->SetIGTLName("TDATA");
        this->TrackingDataQueryNode->SetNoNameQuery(1);
        //this->TrackingDataQueryNode->SetIGTLName(igtl::ImageMetaMessage::GetDeviceType());
        this->MRMLScene->AddNode(this->TrackingDataQueryNode);
        this->TrackingDataQueryNode->AddObserver(vtkMRMLIGTLQueryNode::ResponseEvent,this->MRMLCallbackCommand);
        }
      this->TrackingDataQueryNode->SetQueryStatus(vtkMRMLIGTLQueryNode::STATUS_PREPARED);
      this->TrackingDataQueryNode->SetQueryType(vtkMRMLIGTLQueryNode::TYPE_START);
      this->Connector->PushQuery((vtkMRMLIGTLQueryNode*)this->TrackingDataQueryNode);
      }
    }
  else if (this->StopTrackingButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {

    if (this->MRMLScene && this->Connector)
      {
      if (this->TrackingDataQueryNode == NULL)
        {
        this->TrackingDataQueryNode = vtkMRMLIGTLQueryNode::New();
        this->TrackingDataQueryNode->SetIGTLName("TDATA");
        this->TrackingDataQueryNode->SetNoNameQuery(1);
        //this->TrackingDataQueryNode->SetIGTLName(igtl::ImageMetaMessage::GetDeviceType());
        this->MRMLScene->AddNode(this->TrackingDataQueryNode);
        this->TrackingDataQueryNode->AddObserver(vtkMRMLIGTLQueryNode::ResponseEvent,this->MRMLCallbackCommand);
        }
      this->TrackingDataQueryNode->SetQueryStatus(vtkMRMLIGTLQueryNode::STATUS_PREPARED);
      this->TrackingDataQueryNode->SetQueryType(vtkMRMLIGTLQueryNode::TYPE_STOP);
      this->Connector->PushQuery((vtkMRMLIGTLQueryNode*)this->TrackingDataQueryNode);
      }
    }
  else if (this->CloseButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    this->Withdraw();
    }

}


void vtkIGTLTestWindow::ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData)
{
  vtkMRMLIGTLQueryNode* qnode = vtkMRMLIGTLQueryNode::SafeDownCast(caller);
  if (qnode)
    {
    if (qnode == this->TrackingDataQueryNode)
      {
      switch (event)
        {
        case vtkMRMLIGTLQueryNode::ResponseEvent:
          //this->UpdateRemoteDataList();
          break;
        default:
          break;
        }
      }
    else if (event == vtkMRMLIGTLQueryNode::ResponseEvent &&
             qnode->GetQueryStatus() == vtkMRMLIGTLQueryNode::STATUS_SUCCESS)
      {
      std::list<vtkMRMLIGTLQueryNode*>::iterator iter;
      for (iter = this->ImageQueryNodeList.begin();
           iter != this->ImageQueryNodeList.end();
           iter ++)
        {
        if (qnode == *iter)
          {
          (*iter)->RemoveObserver(this->MRMLCallbackCommand);
          if (this->MRMLScene)
            {
            this->MRMLScene->RemoveNode(*iter);
            }
          this->ImageQueryNodeList.remove(*iter);
          break;
          }
        }
      }
    }
}


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


void vtkIGTLTestWindow::RemoveGUIObservers()
{
  if (this->ModeButtonSet)
    {
    this->ModeButtonSet->GetWidget(0)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->ModeButtonSet->GetWidget(1)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
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

  
  // Connector Property -- Connector type (server or client)
  vtkKWFrameWithLabel *modeFrame = vtkKWFrameWithLabel::New();
  modeFrame->SetParent(this->MainFrame);
  modeFrame->Create();
  modeFrame->SetLabelText ("Tracking Test Data Source");
  app->Script ( "pack %s -fill both -expand true",  
                modeFrame->GetWidgetName());

  this->ModeButtonSet = vtkKWRadioButtonSet::New();
  this->ModeButtonSet->SetParent(modeFrame->GetFrame());
  this->ModeButtonSet->Create();
  this->ModeButtonSet->PackHorizontallyOn();
  this->ModeButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->ModeButtonSet->UniformColumnsOn();
  this->ModeButtonSet->UniformRowsOn();

  this->ModeButtonSet->AddWidget(0);
  this->ModeButtonSet->GetWidget(0)->SetText("Random");
  this->ModeButtonSet->AddWidget(1);
  this->ModeButtonSet->GetWidget(1)->SetText("From file");

  app->Script("pack %s -side left -anchor w -fill x -padx 2 -pady 2", 
              this->ModeButtonSet->GetWidgetName());


  // Connector Property -- Connector type (server or client)
  vtkKWFrame *buttonFrame = vtkKWFrame::New();
  buttonFrame->SetParent(this->MainFrame);
  buttonFrame->Create();
  app->Script ( "pack %s -side top -fill both -expand true",  
                buttonFrame->GetWidgetName());

  this->StartTrackingButton = vtkKWPushButton::New();
  this->StartTrackingButton->SetParent(buttonFrame);
  this->StartTrackingButton->Create();
  this->StartTrackingButton->SetText( "Start tracking" );
  this->StartTrackingButton->SetWidth (10);

  this->StopTrackingButton = vtkKWPushButton::New();
  this->StopTrackingButton->SetParent(buttonFrame);
  this->StopTrackingButton->Create();
  this->StopTrackingButton->SetText( "Stop tracking" );
  this->StopTrackingButton->SetWidth (10);

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

  /*
  if (this->MultipleMonitorsAvailable)
  {
    this->HideDecorationOn();
  }
  */

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
