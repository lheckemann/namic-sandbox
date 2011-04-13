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

#include "vtkUltrasound4DGUI.h"
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
#include "vtkKWTopLevel.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"
#include "vtkKWFileBrowserDialog.h"
#include "vtkKWPushButton.h"

#include "vtkCornerAnnotation.h"

#include "vtkMRMLIGTLConnectorNode.h"

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstdio>

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkUltrasound4DGUI );
vtkCxxRevisionMacro ( vtkUltrasound4DGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
vtkUltrasound4DGUI::vtkUltrasound4DGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkUltrasound4DGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  this->VolumeRenderingButton = NULL;
  this->SelectDirectoryButton = NULL;
  
  //----------------------------------------------------------------
  // File Selector widget

  this->fileSelector = NULL;

  //----------------------------------------------------------------
  // OpenIGTLink Connection

  this->OpenIGTLinkConnectButton = NULL;
  this->OpenIGTLinkServerNode = NULL;

  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkUltrasound4DGUI::~vtkUltrasound4DGUI ( )
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

  if (this->VolumeRenderingButton)
    {
      this->VolumeRenderingButton->SetParent(NULL);
      this->VolumeRenderingButton->Delete();
    }

  if (this->SelectDirectoryButton)
    {
      this->SelectDirectoryButton->SetParent(NULL);
      this->SelectDirectoryButton->Delete();
    }

  if (this->OpenIGTLinkConnectButton)
    {
      this->OpenIGTLinkConnectButton->SetParent(NULL);
      this->OpenIGTLinkConnectButton->Delete();
    }

  if (this->fileSelector)
    {
      this->fileSelector->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::Enter()
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
void vtkUltrasound4DGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "Ultrasound4DGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->VolumeRenderingButton)
    {
      this->VolumeRenderingButton
     ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->SelectDirectoryButton)
    {
      this->SelectDirectoryButton
     ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->OpenIGTLinkConnectButton)
    {
      this->OpenIGTLinkConnectButton
     ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->fileSelector)
    {
      this->fileSelector
     ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }


  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::AddGUIObservers ( )
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

  this->VolumeRenderingButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->SelectDirectoryButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->OpenIGTLinkConnectButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->fileSelector
    ->AddObserver(vtkKWFileBrowserDialog::FileNameChangedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
      this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                         (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
      this->GetLogic()->AddObserver(vtkUltrasound4DLogic::StatusUpdateEvent,
                        (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::ProcessGUIEvents(vtkObject *caller,
                           unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
      vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
      HandleMouseEvent(style);
      return;
    }

  
  if (this->VolumeRenderingButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {    

    }

  if (this->OpenIGTLinkConnectButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {    
      int server_started =  CreateOpenIGTLinkServerNode();

      // Wait to be sure the server is well created 
      igtl::Sleep(1000);
  
      if(server_started)
     {
       int client_started = StartOpenIGTLinkClient();
     }

    }


  if (this->SelectDirectoryButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {    
      // -----------------------------------------
      // Open Dialog box to select raw data

      this->fileSelector->Invoke();
    }

  if (this->fileSelector == vtkKWFileBrowserDialog::SafeDownCast(caller)
      && event == vtkKWFileBrowserDialog::FileNameChangedEvent)
    {
      if (this->fileSelector->GetNumberOfFileNames() > 0)
     {
       this->SelectDirectoryButton->SetBackgroundColor(0,1,0);
       this->OpenIGTLinkConnectButton->SetEnabled(1);
     }
      else
     {
       this->SelectDirectoryButton->SetBackgroundColor(1,0,0);
       this->OpenIGTLinkConnectButton->SetEnabled(0);
     }
    }

} 


void vtkUltrasound4DGUI::DataCallback(vtkObject *caller, 
                          unsigned long eid, void *clientData, void *callData)
{
  vtkUltrasound4DGUI *self = reinterpret_cast<vtkUltrasound4DGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkUltrasound4DGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::ProcessLogicEvents ( vtkObject *caller,
                               unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkUltrasound4DLogic::SafeDownCast(caller))
    {
      if (event == vtkUltrasound4DLogic::StatusUpdateEvent)
     {
       //this->UpdateDeviceStatus();
     }
    }
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::ProcessMRMLEvents ( vtkObject *caller,
                              unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::ProcessTimerEvents()
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
void vtkUltrasound4DGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "Ultrasound4D", "Ultrasound4D", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForLoadingRAWFiles();
  CreateFileSelector();
}


void vtkUltrasound4DGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:Ultrasound4D</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Ultrasound4D" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkUltrasound4DGUI::BuildGUIForLoadingRAWFiles()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Ultrasound4D");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Simulator");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Loading NRRD Files frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Load RAW Files");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  // -----------------------------------------
  // Select Directory Button

  vtkKWFrame* frame2 = vtkKWFrame::New();
  frame2->SetParent(frame->GetFrame());
  frame2->Create();
  this->Script( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
          frame2->GetWidgetName() );

  this->SelectDirectoryButton = vtkKWPushButton::New();
  this->SelectDirectoryButton->SetParent(frame2);
  this->SelectDirectoryButton->Create();
  this->SelectDirectoryButton->SetText("Select RAW Files");
  
  this->Script("pack %s -side left -expand y -padx 2 -pady 2",
            this->SelectDirectoryButton->GetWidgetName());

  // -----------------------------------------
  // OpenIGTLink Connect  button

  vtkKWFrame* frame4 = vtkKWFrame::New();
  frame4->SetParent(frame->GetFrame());
  frame4->Create();
  this->Script( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
          frame4->GetWidgetName() );

  this->OpenIGTLinkConnectButton = vtkKWPushButton::New ( );
  this->OpenIGTLinkConnectButton->SetParent ( frame4 );
  this->OpenIGTLinkConnectButton->Create ( );
  this->OpenIGTLinkConnectButton->SetText ("Connect OpenIGTLink");
  this->OpenIGTLinkConnectButton->SetEnabled(0);

  this->Script("pack %s -side left -expand y -padx 2 -pady 2", 
               this->OpenIGTLinkConnectButton->GetWidgetName());


  // -----------------------------------------
  // Volume Rendering button

  vtkKWFrame* frame3 = vtkKWFrame::New();
  frame3->SetParent(frame->GetFrame());
  frame3->Create();
  this->Script( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
          frame3->GetWidgetName() );

  this->VolumeRenderingButton = vtkKWPushButton::New ( );
  this->VolumeRenderingButton->SetParent ( frame3 );
  this->VolumeRenderingButton->Create ( );
  this->VolumeRenderingButton->SetText ("Volume Rendering");
  this->VolumeRenderingButton->SetEnabled(0);

  this->Script("pack %s -side left -expand y -padx 2 -pady 2", 
               this->VolumeRenderingButton->GetWidgetName());
 
  conBrowsFrame->Delete();
  frame->Delete();
  frame2->Delete();
  frame3->Delete();
  frame4->Delete();
}

//----------------------------------------------------------------------------
void vtkUltrasound4DGUI::UpdateAll()
{
}

void vtkUltrasound4DGUI::CreateFileSelector()
{
  if(!this->fileSelector)
    {
      this->fileSelector = vtkKWFileBrowserDialog::New();
      this->fileSelector->SetApplication(this->GetApplication());
      this->fileSelector->Create();       
      this->fileSelector->ChooseDirectoryOff();
      this->fileSelector->MultipleSelectionOn();  
      this->fileSelector->SetFileTypes("{{RAW Data} {.raw}}");
    }
}

int vtkUltrasound4DGUI::CreateOpenIGTLinkServerNode()
{
  int return_value = 0;

  vtkMRMLIGTLConnectorNode* OpenIGTLinkServerNode = vtkMRMLIGTLConnectorNode::New();
  OpenIGTLinkServerNode->SetServerHostname("localhost");
  OpenIGTLinkServerNode->SetTypeServer(18944); 
  OpenIGTLinkServerNode->Start();

  this->GetMRMLScene()->AddNode(OpenIGTLinkServerNode);

  if (OpenIGTLinkServerNode->GetState() == vtkMRMLIGTLConnectorNode::STATE_WAIT_CONNECTION
      && this->OpenIGTLinkConnectButton)
    {
      this->OpenIGTLinkConnectButton->SetBackgroundColor(1,0.5,0);
      return_value = 1;
    }

  OpenIGTLinkServerNode->Delete();

  return return_value;
}

int vtkUltrasound4DGUI::StartOpenIGTLinkClient()
{
  //------------------------------------------------------------
  // Establish Connection
  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer("localhost",18944);

  if (r != 0)
    {
      std::cerr << "Cannot connect to the server." << std::endl;
      return 0;
    }

  //------------------------------------------------------------
  // loop
  for (int i = 0; i < 300; i ++)
    {

      //------------------------------------------------------------
      // size parameters
      int   size[]     = {256, 256, 256};       // image dimension
      float spacing[]  = {1, 1, 1};     // spacing (mm/pixel)
      int   svsize[]   = {256, 256, 256};       // sub-volume size
      int   svoffset[] = {0, 0, 0};           // sub-volume offset
      int   scalarType = igtl::ImageMessage::TYPE_UINT16;// scalar type

      //------------------------------------------------------------
      // Create a new IMAGE type message
      igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
      imgMsg->SetDimensions(size);
      imgMsg->SetSpacing(spacing);
      imgMsg->SetScalarType(scalarType);
      imgMsg->SetDeviceName("ImagerClient");
      imgMsg->SetSubVolume(svsize, svoffset);
      imgMsg->AllocateScalars();

      //------------------------------------------------------------
      // Set image data (See GetTestImage() bellow for the details)
      if(this->fileSelector)
     {
       GetTestImage(imgMsg, i % this->fileSelector->GetNumberOfFileNames());
     }

      //------------------------------------------------------------
      // Get random orientation matrix and set it.
      igtl::Matrix4x4 matrix;
      GetRandomTestMatrix(matrix);
      imgMsg->SetMatrix(matrix);

      //------------------------------------------------------------
      // Pack (serialize) and send
      imgMsg->Pack();
      socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());

      if(this->fileSelector)
     {
       igtl::Sleep(1000/this->fileSelector->GetNumberOfFileNames()); // wait
     }
      else
     {
       igtl::Sleep(500);
     }
    }

  //------------------------------------------------------------
  // Close connection
  socket->CloseSocket();
  return 1;
}

//------------------------------------------------------------
// Function to read test image data
int vtkUltrasound4DGUI::GetTestImage(igtl::ImageMessage::Pointer& msg,  int i)
{

  //------------------------------------------------------------
  // Check if image index is in the range
  if(this->fileSelector)
    {
      if (i < 0 || i >= this->fileSelector->GetNumberOfFileNames()+1) 
     {
       std::cerr << "Image index is invalid." << std::endl;
       return 0;
     }
    
      //------------------------------------------------------------
      // Generate path to the raw image file
      char* filename = (char*)this->fileSelector->GetNthFileName(i);
      std::cerr << "Reading " << filename << "...";

      //------------------------------------------------------------
      // Load raw data from the file
      FILE *fp = fopen(filename, "rb");
      if (fp == NULL)
     {
       std::cerr << "File opeining error: " << filename << std::endl;
       return 0;
     }
      int fsize = msg->GetImageSize();
      size_t b = fread(msg->GetScalarPointer(), 1, fsize, fp);

      if (b <= 0)
     {
       std::cerr << "Error while reading file: " << filename << std::endl;
       return 0;
     }

      fclose(fp);

      std::cerr << "done." << std::endl;
    }
  return 1;
}

//------------------------------------------------------------
// Function to generate random matrix.
void vtkUltrasound4DGUI::GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{ 
  matrix[0][0] = -1.0;  matrix[0][1] = 0.0;  matrix[0][2] = 0.0; matrix[0][3] = 0.0;
  matrix[1][0] = 0.0; matrix[1][1] = -1.0;  matrix[1][2] = 0.0; matrix[1][3] = 0.0;
  matrix[2][0] = 0.0;  matrix[2][1] = 0.0;  matrix[2][2] = 1.0; matrix[2][3] = 0.0;
  matrix[3][0] = 0.0;  matrix[3][1] = 0.0;  matrix[3][2] = 0.0; matrix[3][3] = 1.0;
  
  igtl::PrintMatrix(matrix);
}

