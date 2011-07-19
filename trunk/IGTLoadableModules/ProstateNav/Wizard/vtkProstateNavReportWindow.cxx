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

#include "vtkProstateNavReportWindow.h"

#include "vtkSlicerApplication.h"

#include "vtkKWFrameWithLabel.h"
#include "vtkKWRadioButton.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWPushButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWText.h"
#include "vtkKWTextWithScrollbars.h"

#include "igtlOSUtil.h"
#include "igtlMath.h"
#include "igtlTransformMessage.h"
#include "igtlServerSocket.h"

#include "vtkMRMLTransPerinealProstateTemplateNode.h"
#include "vtkMRMLProstateNavManagerNode.h"
#include "vtkProstateNavTargetDescriptor.h"


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkProstateNavReportWindow);
vtkCxxRevisionMacro(vtkProstateNavReportWindow, "$Revision: 1.0 $");
//----------------------------------------------------------------------------
vtkProstateNavReportWindow::vtkProstateNavReportWindow()
{
  
  // Logic values
  this->WindowPosition[0]=0;
  this->WindowPosition[1]=0;
  this->WindowSize[0]=600;
  this->WindowSize[1]=600;

  // GUI widgets
  this->MainFrame = vtkKWFrame::New();
  this->MultipleMonitorsAvailable = false; 

  this->ModeButtonSet = NULL;
  this->SelectTrackingFileButton = NULL;

  this->StartTrackingButton  = NULL;
  this->StopTrackingButton = NULL;
  this->CloseButton    = NULL;

  //this->CurrentTarget=NULL;

  // GUI callback command
  this->InGUICallbackFlag = 0;
  this->IsObserverAddedFlag = 0;

  this->GUICallbackCommand = vtkCallbackCommand::New();
  this->GUICallbackCommand->SetClientData( reinterpret_cast<void *>(this) );
  this->GUICallbackCommand->SetCallback(&vtkProstateNavReportWindow::GUICallback);

}


//----------------------------------------------------------------------------
vtkProstateNavReportWindow::~vtkProstateNavReportWindow()
{

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
  if (this->SelectTrackingFileButton)
    {
    this->SelectTrackingFileButton->SetParent(NULL);
    this->SelectTrackingFileButton->Delete();
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
void vtkProstateNavReportWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkProstateNavReportWindow::GUICallback( vtkObject *caller,
                           unsigned long eid, void *clientData, void *callData )
{
  
  vtkProstateNavReportWindow *self = reinterpret_cast<vtkProstateNavReportWindow *>(clientData);
  
  if (self->GetInGUICallbackFlag())
    {
    }

  self->SetInGUICallbackFlag(1);
  self->ProcessGUIEvents(caller, eid, callData);
  self->SetInGUICallbackFlag(0);
  
}

//----------------------------------------------------------------------------
void vtkProstateNavReportWindow::ProcessGUIEvents(vtkObject *caller, unsigned long event, void *callData)
{

  if (this->ModeButtonSet->GetWidget(0) == vtkKWRadioButton::SafeDownCast(caller)
      && event == vtkKWRadioButton::SelectedStateChangedEvent)
    {
    if (this->ModeButtonSet->GetWidget(0)->GetSelectedState() == 1)
      {
      }
    }
  else if (this->ModeButtonSet->GetWidget(1) == vtkKWRadioButton::SafeDownCast(caller)
      && event == vtkKWRadioButton::SelectedStateChangedEvent)
    {
    if (this->ModeButtonSet->GetWidget(1)->GetSelectedState() == 1)
      {
      }
    }
  else if (this->StartTrackingButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    }
  else if (this->StopTrackingButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    }
  else if (this->CloseButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    this->Withdraw();
    }

}


//----------------------------------------------------------------------------
void vtkProstateNavReportWindow::ProcessTimerEvents()
{
  // NOTE: this function has to be called by an external timer event handler
  // e.g. vtkOpenIGTLinkIFGUI::ProcessTimerEvents();
  
}


//----------------------------------------------------------------------------
void vtkProstateNavReportWindow::AddGUIObservers()
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


//----------------------------------------------------------------------------
void vtkProstateNavReportWindow::RemoveGUIObservers()
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
void vtkProstateNavReportWindow::CreateWidget()
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
  this->SetApplication ( app );
  //this->SetBorderWidth ( 1 );
  //this->SetReliefToFlat();

  //this->SetParent (this->GetApplicationGUI()->GetMainSlicerWindow());

  this->SetTitle ("OpenIGTLink Test Server");
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
  app->Script ( "pack %s -fill both -expand n ",  
                dataSourceFrame->GetWidgetName());

  vtkKWFrame *modeFrame = vtkKWFrame::New();
  modeFrame->SetParent(dataSourceFrame->GetFrame());
  modeFrame->Create();
  app->Script ( "pack %s -fill both -expand n",  
                modeFrame->GetWidgetName());

  this->ModeButtonSet = vtkKWRadioButtonSet::New();
  this->ModeButtonSet->SetParent(modeFrame);
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

  vtkKWFrame *trackingFileFrame = vtkKWFrame::New();
  trackingFileFrame->SetParent(dataSourceFrame->GetFrame());
  trackingFileFrame->Create();
  app->Script ( "pack %s -fill both -expand n",  
                trackingFileFrame->GetWidgetName());

  this->SelectTrackingFileButton = vtkKWLoadSaveButtonWithLabel::New();
  this->SelectTrackingFileButton->SetParent(trackingFileFrame);
  this->SelectTrackingFileButton->Create();
  this->SelectTrackingFileButton->SetWidth(50);
  this->SelectTrackingFileButton->GetWidget()->SetText ("Tracking File");
  //this->SelectTrackingFileButton->GetWidget()->TrimPathFromFileNameOn();
  //this->SelectTrackingFileButton->GetWidget()->GetLoadSaveDialog()->ChooseDirectoryOn();
  this->SelectTrackingFileButton->GetWidget()->GetLoadSaveDialog()->SetFileTypes("{ {OpenIGTLinkIF} {*.csv} }");
  this->SelectTrackingFileButton->GetWidget()->GetLoadSaveDialog()
    ->RetrieveLastPathFromRegistry("OpenPath");

  this->SelectTrackingFileButton->SetEnabled(0);

  this->Script("pack %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->SelectTrackingFileButton->GetWidgetName());


  // --------------------------------------------------
  // Control Buttons

  vtkKWFrame *buttonFrame = vtkKWFrame::New();
  buttonFrame->SetParent(this->MainFrame);
  buttonFrame->Create();
  app->Script ( "pack %s -side top -fill both -expand n",  
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
  this->CloseButton->SetWidth (10);

  app->Script ( "pack %s %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
                this->StartTrackingButton->GetWidgetName(),
                this->StopTrackingButton->GetWidgetName(),
                this->CloseButton->GetWidgetName());

  buttonFrame->Delete();

  // --------------------------------------------------
  // Status Frame
  vtkKWFrameWithLabel *reportFrame = vtkKWFrameWithLabel::New();
  reportFrame->SetParent(this->MainFrame);
  reportFrame->SetLabelText ("Target Report");
  reportFrame->Create();
  app->Script ( "pack %s -side top -fill both -expand true",  
                reportFrame->GetWidgetName());

  this->ReportText = vtkKWTextWithScrollbars::New();
  this->ReportText->SetParent(reportFrame->GetFrame());
  this->ReportText->HorizontalScrollbarVisibilityOff();
  this->ReportText->GetWidget()->SetReadOnly(1);
  this->ReportText->Create();
  //this->ReportText->SetWidth(8);
  app->Script("pack %s -side top -fill both -expand true", 
              this->ReportText->GetWidgetName());

}


//----------------------------------------------------------------------------
void vtkProstateNavReportWindow::DisplayOnWindow()
{
  //-- display
  this->DeIconify();
  this->Raise();

  this->SetPosition(this->WindowPosition[0], this->WindowPosition[1]);
  this->SetSize(this->WindowSize[0], this->WindowSize[1]);

  if (!this->IsObserverAddedFlag)
    {
    this->IsObserverAddedFlag = 1;
    this->AddGUIObservers();
    }

}



//----------------------------------------------------------------------------
void vtkProstateNavReportWindow::GenerateReport(vtkMRMLTransPerinealProstateTemplateNode* robot,
                                                vtkProstateNavTargetDescriptor* desc)
{
  vtkKWText* text = this->ReportText->GetWidget();
  char buf[256];

  //-- Clear the text field
  text->SetText("");

  //--- Title
  text->AppendText("==================================================\n");
  text->AppendText("               BIOPSY TARGET REPORT \n\n");
  text->AppendText("       Generaged by 3D Slicer + ProstateNav\n");
  text->AppendText("==================================================\n\n");

  //--- Date
  time_t t;
  t = time(NULL);
  sprintf(buf, "Time Created: %s\n", ctime(&t));
  text->AppendText(buf);
  //sprintf(buf, "Number of Targets: %d\n");
  //text->AppendText(buf);

  //int numTargets = manager->GetTotalNumberOfTargets();

  //for (int i = 0; i < numTargets; i ++)
  //  {
  //  vtkProstateNavTargetDescriptor* targetDesc=this->GetProstateNavManager()->GetTargetDescriptorAtIndex(i);
  //  if (targetDesc==NULL)
  //  {
  //    vtkErrorMacro("Target descriptor not found");
  //    return;
  //  }
  //  double* xyz=targetDesc->GetRASLocation();
  //
  //  vtkMatrix4x4* matrix = vtkMatrix4x4::New();
  //  matrix->Identity();
  //  
  //  matrix->SetElement(0, 3, xyz[0]);
  //  matrix->SetElement(1, 3, xyz[1]);
  //  matrix->SetElement(2, 3, xyz[2]);
  //  
  //  vtkMatrix4x4* transformToParent = transformNode->GetMatrixTransformToParent();
  //  transformToParent->DeepCopy(matrix);
  //  robot->SetTargetPosition(matrix);
  //  text->AppendText(robot->GetScreenMessage());
  //  }
  text->AppendText(robot->GetTargetReport(desc));

}

