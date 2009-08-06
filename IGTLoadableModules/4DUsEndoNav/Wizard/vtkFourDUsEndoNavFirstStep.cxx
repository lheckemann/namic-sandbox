/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#define DEBUG_FLAG

#include "vtkFourDUsEndoNavFirstStep.h"

#include "vtkFourDUsEndoNavGUI.h"
#include "vtkFourDUsEndoNavLogic.h"

#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWEntry.h"
#include "vtkKWCheckButton.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWPushButton.h"
#include "vtkKWLoadSaveButton.h"
#include "vtkKWLoadSaveButtonWithLabel.h"

#include "vtkOpenIGTLinkIFLogic.h"
#include "vtkOpenIGTLinkIFGUI.h"

//==============================================================================

#include "vtkMRMLModelNode.h"
#include <pthread.h>
#include <vtkImageImport.h>
#include "vtkImageMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkActor2D.h"
#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"


// mutex
pthread_mutex_t work_mutex;


// OpenCV capture refresh.
static void *thread_cvGetImageFromCamera(vtkMultiThreader::ThreadInfo *data);

// thread exit flag
int thread_exit_flag = 0;

//==============================================================================

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkFourDUsEndoNavFirstStep);
vtkCxxRevisionMacro(vtkFourDUsEndoNavFirstStep, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkFourDUsEndoNavFirstStep::vtkFourDUsEndoNavFirstStep()
{

  this->SetName("1/5. First");
  this->SetDescription("Perform system configuration.");

  this->RobotFrame          = NULL;
  this->RobotLabel1         = NULL;
  this->RobotLabel2         = NULL;
  this->RobotAddressEntry   = NULL;
  this->RobotPortEntry      = NULL;
  this->RobotConnectButton  = NULL;

  this->ScannerFrame        = NULL;
  this->ScannerLabel1       = NULL;
  this->ScannerLabel2       = NULL;
  this->ScannerAddressEntry = NULL;
  this->ScannerPortEntry    = NULL;
  this->ScannerConnectButton  = NULL;

  //Query image from camera
  this->Capture = NULL;
  this->RGBImage = NULL;
  this->CaptureImage = NULL;
  this->CaptureImageTmp = NULL;

  this->PlayerThreader = vtkMultiThreader::New();;
  this->PlayerThreadId = -1;
  this->Capturing = false;

  this->CaptureNode= NULL;

  this->CaptureMapper = NULL;
  this->CaptureActor = NULL;
  this->CaptureRen = NULL;
  this->CaptureRenWin = NULL;

  this->Importer = vtkImageImport::New();

  this->Idata = NULL;

  // for renderer window capture
  this->RenWinForCapture = vtkRenderWindow::New();
  this->RenForCapture = vtkRenderer::New();

}


//----------------------------------------------------------------------------
vtkFourDUsEndoNavFirstStep::~vtkFourDUsEndoNavFirstStep()
{

  if (this->RobotFrame)
    {
    this->RobotFrame->SetParent(NULL);
    this->RobotFrame->Delete();
    this->RobotFrame = NULL;
    }
  if (this->RobotLabel1)
    {
    this->RobotLabel1->SetParent(NULL);
    this->RobotLabel1->Delete();
    this->RobotLabel1 = NULL;
    }
  if (this->RobotLabel2)
    {
    this->RobotLabel2->SetParent(NULL);
    this->RobotLabel2->Delete();
    this->RobotLabel2 = NULL;
    }
  if (this->RobotAddressEntry)
    {
    this->RobotAddressEntry->SetParent(NULL);
    this->RobotAddressEntry->Delete();
    this->RobotAddressEntry = NULL;
    }
  if (this->RobotPortEntry)
    {
    this->RobotPortEntry->SetParent(NULL);
    this->RobotPortEntry->Delete();
    this->RobotPortEntry = NULL;
    }
  if (this->RobotConnectButton)
    {
    this->RobotConnectButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->RobotConnectButton->SetParent(NULL);
    this->RobotConnectButton->Delete();
    this->RobotConnectButton = NULL;
    }
  if (this->ScannerFrame)
    {
    this->ScannerFrame->SetParent(NULL);
    this->ScannerFrame->Delete();
    this->ScannerFrame = NULL;
    }
  if (this->ScannerLabel1)
    {
    this->ScannerLabel1->SetParent(NULL);
    this->ScannerLabel1->Delete();
    this->ScannerLabel1 = NULL;
    }
  if (this->ScannerLabel2)
    {
    this->ScannerLabel2->SetParent(NULL);
    this->ScannerLabel2->Delete();
    this->ScannerLabel2 = NULL;
    }
  if (this->ScannerAddressEntry)
    {
    this->ScannerAddressEntry->SetParent(NULL);
    this->ScannerAddressEntry->Delete();
    this->ScannerAddressEntry = NULL;
    }
  if (this->ScannerPortEntry)
    {
    this->ScannerPortEntry->SetParent(NULL);
    this->ScannerPortEntry->Delete();
    this->ScannerPortEntry = NULL;
    }
  if (this->ScannerConnectButton)
    {
    this->ScannerConnectButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->ScannerConnectButton->SetParent(NULL);
    this->ScannerConnectButton->Delete();
    this->ScannerConnectButton = NULL;
    }

  if(this->PlayerThreader)
    {
    this->PlayerThreader->Delete();
    this->PlayerThreader = NULL;
    }

  if(this->CaptureMapper)
    {
    this->CaptureMapper->Delete();
    this->CaptureMapper = NULL;
    }
  if(this->CaptureActor)
    {
    this->CaptureActor->Delete();
    this->CaptureActor = NULL;
    }
  if(this->CaptureRen)
    {
    this->CaptureRen->Delete();
    this->CaptureRen = NULL;
    }
  if(this->CaptureRenWin)
    {
    this->CaptureRenWin->Delete();
    this->CaptureRenWin = NULL;
    }

  if(this->RGBImage)
      {
      cvReleaseImage(&this->RGBImage);
      this->RGBImage = NULL;
      }
  if(this->CaptureImage)
    {
    cvReleaseImage(&this->CaptureImage);
    this->CaptureImage = NULL;
    }
  if(this->CaptureImageTmp)
    {
    cvReleaseImage(&this->CaptureImageTmp);
    this->CaptureImageTmp = NULL;
    }
  if(this->Capture)
    {
    cvReleaseCapture(&this->Capture);
    this->Capture = NULL;
    }


  if(this->Importer)
    {
    this->Importer->Delete();
    this->Importer = NULL;
    }

  if(this->CaptureNode)
    {
    this->CaptureNode->Delete();
    this->CaptureNode = NULL;
    }

  if(this->RenWinForCapture)
    {
    this->RenWinForCapture->Delete();
    this->RenWinForCapture = NULL;
    }

  if(this->RenForCapture)
    {
    this->RenForCapture->Delete();
    this->RenForCapture = NULL;
    }

}

//----------------------------------------------------------------------------
void vtkFourDUsEndoNavFirstStep::ShowUserInterface()
{

  this->Superclass::ShowUserInterface();
  vtkKWWizardWidget *wizardWidget = this->GetGUI()->GetWizardWidget();
  vtkKWWidget *parent = wizardWidget->GetClientArea();

  if (!this->RobotFrame)
    {
    this->RobotFrame = vtkKWFrame::New();
    this->RobotFrame->SetParent ( parent );
    this->RobotFrame->Create ( );
    }

  this->Script ( "pack %s -side top -fill x",
                 this->RobotFrame->GetWidgetName());

  if (!this->RobotLabel1)
    {
    this->RobotLabel1 = vtkKWLabel::New();
    this->RobotLabel1->SetParent(this->RobotFrame);
    this->RobotLabel1->Create();
    this->RobotLabel1->SetWidth(15);
    this->RobotLabel1->SetText("Robot Addr: ");
    }

  if (!this->RobotAddressEntry)
    {
    this->RobotAddressEntry = vtkKWEntry::New();
    this->RobotAddressEntry->SetParent(this->RobotFrame);
    this->RobotAddressEntry->Create();
    this->RobotAddressEntry->SetWidth(15);
    }

  if (!this->RobotLabel2)
    {
    this->RobotLabel2 = vtkKWLabel::New();
    this->RobotLabel2->SetParent(this->RobotFrame);
    this->RobotLabel2->Create();
    this->RobotLabel2->SetWidth(1);
    this->RobotLabel2->SetText(":");
    }
  if (!this->RobotPortEntry)
    {
    this->RobotPortEntry = vtkKWEntry::New();
    this->RobotPortEntry->SetParent(this->RobotFrame);
    this->RobotPortEntry->Create();
    this->RobotPortEntry->SetWidth(10);
    this->RobotPortEntry->SetRestrictValueToInteger();
    }
  
  if (!this->RobotConnectButton)
    {
    this->RobotConnectButton = vtkKWPushButton::New();
    this->RobotConnectButton->SetParent (this->RobotFrame);
    this->RobotConnectButton->Create();
    this->RobotConnectButton->SetText("OFF");
    this->RobotConnectButton->SetBalloonHelpString("Connect to Robot");
    this->RobotConnectButton->AddObserver(vtkKWPushButton::InvokedEvent,
                                          (vtkCommand *)this->GUICallbackCommand);
    }


  this->Script("pack %s %s %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->RobotLabel1->GetWidgetName(), this->RobotAddressEntry->GetWidgetName(),
               this->RobotLabel2->GetWidgetName(), this->RobotPortEntry->GetWidgetName(),
               this->RobotConnectButton->GetWidgetName());

  if (!this->ScannerFrame)
    {
    this->ScannerFrame = vtkKWFrame::New();
    this->ScannerFrame->SetParent ( parent );
    this->ScannerFrame->Create ( );
    }

  this->Script ( "pack %s -side top -fill x",  
                 this->ScannerFrame->GetWidgetName());

  if (!this->ScannerLabel1)
    {
    this->ScannerLabel1 = vtkKWLabel::New();
    this->ScannerLabel1->SetParent(this->ScannerFrame);
    this->ScannerLabel1->Create();
    this->ScannerLabel1->SetWidth(15);
    this->ScannerLabel1->SetText("Scanner Addr: ");
    }

  if (!this->ScannerAddressEntry)
    {
    this->ScannerAddressEntry = vtkKWEntry::New();
    this->ScannerAddressEntry->SetParent(this->ScannerFrame);
    this->ScannerAddressEntry->Create();
    this->ScannerAddressEntry->SetWidth(15);
    }


  if (!this->ScannerLabel2)
    {
    this->ScannerLabel2 = vtkKWLabel::New();
    this->ScannerLabel2->SetParent(this->ScannerFrame);
    this->ScannerLabel2->Create();
    this->ScannerLabel2->SetWidth(1);
    this->ScannerLabel2->SetText(":");
    }

  if (!this->ScannerPortEntry)
    {
    this->ScannerPortEntry = vtkKWEntry::New();
    this->ScannerPortEntry->SetParent(this->ScannerFrame);
    this->ScannerPortEntry->Create();
    this->ScannerPortEntry->SetWidth(10);
    this->ScannerPortEntry->SetRestrictValueToInteger();
    }

  if (!this->ScannerConnectButton)
    {
    this->ScannerConnectButton = vtkKWPushButton::New();
    this->ScannerConnectButton->SetParent (this->ScannerFrame);
    this->ScannerConnectButton->Create();
    this->ScannerConnectButton->SetText("OFF");
    this->ScannerConnectButton->SetBalloonHelpString("Connect to Scanner");
    this->ScannerConnectButton->AddObserver(vtkKWPushButton::InvokedEvent,
                                          (vtkCommand *)this->GUICallbackCommand);
    }
  
  this->Script("pack %s %s %s %s %s -side left -anchor w -fill x -padx 2 -pady 2", 
               this->ScannerLabel1->GetWidgetName(), this->ScannerAddressEntry->GetWidgetName(),
               this->ScannerLabel2->GetWidgetName(), this->ScannerPortEntry->GetWidgetName(),
               this->ScannerConnectButton->GetWidgetName());



}


//----------------------------------------------------------------------------
void vtkFourDUsEndoNavFirstStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkFourDUsEndoNavFirstStep::ProcessGUIEvents( vtkObject *caller,
                                         unsigned long event, void *callData )
{

  if (this->RobotConnectButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    if (strcmp(this->RobotConnectButton->GetText(), "OFF") == 0)
      {
//      const char* address = this->RobotAddressEntry->GetValue();
//      int port    = this->RobotPortEntry->GetValueAsInt();
//      if (strlen(address) > 0 && port > 0)
//        {
//        vtkOpenIGTLinkIFGUI* igtlGUI =
//          vtkOpenIGTLinkIFGUI::SafeDownCast(vtkSlicerApplication::SafeDownCast(this->GetApplication())
//                                            ->GetModuleGUIByName("OpenIGTLink IF"));
//        if (igtlGUI)
//          {
//          //igtlGUI->GetLogic()->AddClientConnector("BRPRobot", address, port);
//          this->RobotConnectButton->SetText("ON ");
//          }
//        }
    std::cerr << "showCaptureData button is pressed." << std::endl;

          // initialize mutex
          int resGlobal;
          resGlobal = pthread_mutex_init(&work_mutex, NULL);

          // for display capture data
          this->CaptureNode = this->SetVisibilityOfCaptureData("capture", 1);
          this->RobotConnectButton->SetText("ON ");
      }
    else
      {

    #ifdef DEBUG_FLAG
      cerr << "Clicked Button again to close window" << endl;
    #endif

      this->SetCloseOfCaptureData(this->CaptureNode->GetName());
      this->CaptureNode->Delete();
      this->RobotConnectButton->SetText("OFF");
      }


    }
  else if (this->ScannerConnectButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent )
    {
    if (strcmp(this->ScannerConnectButton->GetText(), "OFF") == 0)
      {
      const char* address = this->ScannerAddressEntry->GetValue();
      int port    = this->ScannerPortEntry->GetValueAsInt();
      if (strlen(address) > 0 && port > 0)
        {
        if (strlen(address) > 0 && port > 0)
          {
          vtkOpenIGTLinkIFGUI* igtlGUI = 
            vtkOpenIGTLinkIFGUI::SafeDownCast(vtkSlicerApplication::SafeDownCast(this->GetApplication())
                                              ->GetModuleGUIByName("OpenIGTLink IF"));
          if (igtlGUI)
            {
            //igtlGUI->GetLogic()->AddClientConnector("BRPScanner", address, port);
            this->ScannerConnectButton->SetText("ON ");
            }
          }
        }
      }
    else
      {
      this->ScannerConnectButton->SetText("OFF");
      }
    }

}

//==============================================================================

// thread function
static void *thread_cvGetImageFromCamera(vtkMultiThreader::ThreadInfo *data)
{

#ifdef DEBUG_FLAG
  cerr << "Thread started" << endl;
#endif

  vtkFourDUsEndoNavFirstStep *self = (vtkFourDUsEndoNavFirstStep *)(data->UserData);
  int activeFlag = 1;

  do
    {
#ifdef DEBUG_FLAG
  cerr << "Loop started" << endl;
#endif

        self->SetCaptureImageTmp( cvQueryFrame( self->GetCapture() ) );

        self->SetImageSize( cvGetSize( self->GetCaptureImageTmp() ) );
//        self->GetCapture()Image = cvCloneImage(self->GetCaptureImageTmp());
//        self->GetRGBImage() = cvCreateImage( self->GetImageSize(), IPL_DEPTH_8U, 3);

        //Jan Gumprecht 4 Aug 2009
        //cvFlip(self->GetCaptureImageTmp(), self->GetCaptureImage(), -1);
        cvFlip(self->GetCaptureImageTmp(), self->GetCaptureImage(), 0);


        cvCvtColor( self->GetCaptureImage(), self->GetRGBImage(), CV_BGR2RGB);
    //    cvCvtColor( self->GetCaptureImage(), self->GetRGBImage(), CV_BGR2GRAY);

        // rgb image into vtk
        //unsigned char* self->GetIdata() = NULL;
        self->SetIdata( (unsigned char*) self->GetRGBImage()->imageData );

        //    vtkImageImport *this->Importer = vtkImageImport::New();
        self->GetImporter()->SetWholeExtent(0,self->GetImageSize().width-1,0,self->GetImageSize().height-1,0,0);
        self->GetImporter()->SetDataExtentToWholeExtent();
        //    self->GetImporter()->SetDataScalarTypeToUnsignedShort();
        self->GetImporter()->SetDataScalarTypeToUnsignedChar();
        self->GetImporter()->SetNumberOfScalarComponents(3);
        //    self->GetImporter()->SetNumberOfScalarComponents(1);
        self->GetImporter()->SetImportVoidPointer(self->GetIdata());

        self->GetImporter()->Update();
        self->GetCaptureRenWin()->Render();


//        self->GetCaptureRen()->AddActor(textActor);
//        self->GetCaptureRen()->AddActor(textActor4);
//        self->GetCaptureRen()->AddActor(textActor6);
//
//        self->GetCaptureRen()->AddActor(suctionActor);
//        self->GetCaptureRen()->AddActor(suctionActor1);
//        self->GetCaptureRen()->AddActor(suctionActor2);
//        self->GetCaptureRen()->AddActor(suctionActor3);
//        self->GetCaptureRen()->AddActor(suctionActor4);
//        self->GetCaptureRen()->AddActor(suctionActor5);
//        self->GetCaptureRen()->AddActor(suctionActor6);
//        self->GetCaptureRen()->AddActor(suctionActor7);
//
//        self->GetCaptureRen()->AddActor(baloonActor);
//        self->GetCaptureRen()->AddActor(baloonActor1);
//        self->GetCaptureRen()->AddActor(baloonActor2);
//        self->GetCaptureRen()->AddActor(baloonActor3);
//        self->GetCaptureRen()->AddActor(baloonActor4);
//        self->GetCaptureRen()->AddActor(baloonActor5);
//        self->GetCaptureRen()->AddActor(baloonActor6);
//        self->GetCaptureRen()->AddActor(baloonActor7);
//
//
//
//        if(textSignalFlag <= 10)
//        {
//
//            self->GetCaptureRen()->AddActor(textActor1);
//            self->GetCaptureRen()->AddActor(textActor2);
//            self->GetCaptureRen()->AddActor(textActor3);
//            self->GetCaptureRen()->AddActor(textActor5);
//
//        }else if(textSignalFlag <= 20){
//
//            if(textActor1Flag == 0){
//                self->GetCaptureRen()->RemoveActor(textActor1);
//            }
//
//            if(textActor2Flag == 0){
//                self->GetCaptureRen()->RemoveActor(textActor2);
//            }
//
//            if(textActor3Flag == 0){
//                self->GetCaptureRen()->RemoveActor(textActor3);
//            }
//
//            if(textActor5Flag == 0){
//                self->GetCaptureRen()->RemoveActor(textActor5);
//            }
//
//        }else{
//            textSignalFlag=0;
//        }
//
//        textSignalFlag++;
//        //}


//        if(thread_exit_flag == 1)
//        {
//        //    pthread_exit ("close the second window.");
//        }

        data->ActiveFlagLock->Lock();
        activeFlag = *(data->ActiveFlag);
        data->ActiveFlagLock->Unlock();

    }
    while(activeFlag != 0);

}

void vtkFourDUsEndoNavFirstStep::cvGetImageFromCamera(void)
{

        this->CaptureImageTmp = cvQueryFrame( this->Capture );

        //Jan Gumprecht 4 Aug 2009
        cvFlip(this->CaptureImageTmp, this->CaptureImage, 0);


        cvCvtColor( this->CaptureImage, this->RGBImage, CV_BGR2RGB);
        //    cvCvtColor( this->CaptureImage, this->RGBImage, CV_BGR2GRAY);

        // rgb image into vtk
        //unsigned char* this->Idata = NULL;
        this->Idata = (unsigned char*) this->RGBImage->imageData;

        //    vtkImageImport *this->Importer = vtkImageImport::New();
        this->Importer->SetWholeExtent(0,this->ImageSize.width-1,0,this->ImageSize.height-1,0,0);
        this->Importer->SetDataExtentToWholeExtent();
        //    this->Importer->SetDataScalarTypeToUnsignedShort();
        this->Importer->SetDataScalarTypeToUnsignedChar();
        this->Importer->SetNumberOfScalarComponents(3);
        //    this->Importer->SetNumberOfScalarComponents(1);
        this->Importer->SetImportVoidPointer(this->Idata);

        this->Importer->Update();

        this->CaptureRenWin->Render();

}

//------------------------------------------------------------------------------
vtkMRMLModelNode* vtkFourDUsEndoNavFirstStep::SetVisibilityOfCaptureData(const char* nodeName, int v)
{
  #ifdef DEBUG_FLAG
    cerr << "SetVisibilityOfCaptureData" << endl;
  #endif

    vtkMRMLModelNode* locatorModel;

    locatorModel = initCapture(nodeName, 1.0, 0.0, 1.0);

    return locatorModel;
}

//------------------------------------------------------------------------------
vtkMRMLModelNode* vtkFourDUsEndoNavFirstStep::initCapture(const char* nodeName, double r, double g, double b)
{

#ifdef DEBUG_FLAG
  cerr << "initCapture" << endl;
#endif

    // for threading

    vtkMRMLModelNode           *locatorModel;
    vtkMRMLModelDisplayNode    *locatorDisp;


    locatorModel = vtkMRMLModelNode::New();
    locatorDisp = vtkMRMLModelDisplayNode::New();

    this->GetGUI()->GetMRMLScene()->SaveStateForUndo();
    this->GetGUI()->GetMRMLScene()->AddNode(locatorDisp);
    this->GetGUI()->GetMRMLScene()->AddNode(locatorModel);

    locatorDisp->SetScene(this->GetGUI()->GetMRMLScene());

    locatorModel->SetName(nodeName);
    locatorModel->SetScene(this->GetGUI()->GetMRMLScene());
    locatorModel->SetAndObserveDisplayNodeID(locatorDisp->GetID());
    locatorModel->SetHideFromEditors(0);

    if(!this->CaptureMapper)
      {
      this->CaptureMapper = vtkImageMapper::New();
      }
    if(!this->CaptureActor)
      {
      this->CaptureActor = vtkActor2D::New();
      }
    if(!this->CaptureRen)
      {
      this->CaptureRen = vtkRenderer::New();
      }
    if(!this->CaptureRenWin)
      {
      this->CaptureRenWin = vtkRenderWindow::New();
      }

//    textActor4->SetInput("Status [DEMO]");
//    textActor4->GetTextProperty()->SetFontSize(12);
//    textActor4->SetPosition(10,170);
//    textActor4->GetTextProperty()->BoldOn();
//
//    textActor6->SetInput("Camera Connection: ON");
//    textActor6->GetTextProperty()->SetFontSize(12);
//    textActor6->SetPosition(10,150);
//
//    textActor2->SetInput("Robot Connection: OFF");
//    textActor2->GetTextProperty()->SetFontSize(12);
//    textActor2->SetPosition(10,130);
//
//    textActor3->SetInput("Network Connection: OFF");
//    textActor3->GetTextProperty()->SetFontSize(12);
//    textActor3->SetPosition(10,110);
//
//    textActor1->SetInput("Baloon Sensor: OFF");
//    textActor1->GetTextProperty()->SetFontSize(12);
//    textActor1->SetPosition(10,90);
//
//    textActor5->SetInput("Suction Tube: OFF");
//    textActor5->GetTextProperty()->SetFontSize(12);
//    textActor5->SetPosition(10,70);
//
//    textActor->SetInput("Robot Console ver. 0.1-Alpha Based on Slicer3.4");
//    textActor->GetTextProperty()->BoldOn();
//    textActor->GetTextProperty()->ItalicOn();
//    textActor->SetPosition(10,0);
//
//    textActor->GetTextProperty()->SetFontSize(14);
//    textActor->GetTextProperty()->SetOpacity(0.5);
//
//    // display status about suction tube
//    suctionActor->SetInput("Suction Tube Status");
//    suctionActor->GetTextProperty()->SetFontSize(12);
//    suctionActor->SetPosition(200,170);
//    suctionActor->GetTextProperty()->BoldOn();
//
//    suctionActor1->SetInput("Regulator / Suction Pressure");
//    suctionActor1->GetTextProperty()->SetFontSize(12);
//    suctionActor1->SetPosition(200,150);
//
//    suctionActor2->SetInput("Suction Tube Switch");
//    suctionActor2->GetTextProperty()->SetFontSize(12);
//    suctionActor2->SetPosition(200,130);
//
//    suctionActor3->SetInput("Scalpel cut");
//    suctionActor3->GetTextProperty()->SetFontSize(12);
//    suctionActor3->SetPosition(200,110);
//
//    suctionActor4->SetInput("Scalpel coag");
//    suctionActor4->GetTextProperty()->SetFontSize(12);
//    suctionActor4->SetPosition(200,90);
//
//    suctionActor5->SetInput("Scalpel On/Off");
//    suctionActor5->GetTextProperty()->SetFontSize(12);
//    suctionActor5->SetPosition(200,70);
//
//    suctionActor6->SetInput("Scalpel Time");
//    suctionActor6->GetTextProperty()->SetFontSize(12);
//    suctionActor6->SetPosition(200,50);
//
//    suctionActor7->SetInput("System Status");
//    suctionActor7->GetTextProperty()->SetFontSize(12);
//    suctionActor7->SetPosition(200,30);
//
//    // display status about baloon sensor
//    baloonActor->SetInput("Baloon Sensor Status");
//    baloonActor->GetTextProperty()->SetFontSize(12);
//    baloonActor->SetPosition(600,170);
//    baloonActor->GetTextProperty()->BoldOn();
//
//    baloonActor1->SetInput("Status");
//    baloonActor1->GetTextProperty()->SetFontSize(12);
//    baloonActor1->SetPosition(600,150);
//
//    baloonActor2->SetInput("Pressure");
//    baloonActor2->GetTextProperty()->SetFontSize(12);
//    baloonActor2->SetPosition(600,130);
//
//    baloonActor3->SetInput("Volume");
//    baloonActor3->GetTextProperty()->SetFontSize(12);
//    baloonActor3->SetPosition(600,110);
//
//    baloonActor4->SetInput("Diff_Pressure");
//    baloonActor4->GetTextProperty()->SetFontSize(12);
//    baloonActor4->SetPosition(600,90);
//
//    cubeSource->SetCenter(0.0,-200.0,0.0);
//
//    polyDataNormals->SetInputConnection( cubeSource->GetOutputPort() );
//    polyDataMapperTexture->SetInputConnection( polyDataNormals->GetOutputPort() );
//
//    actorTexture->SetMapper(polyDataMapperTexture);
//
//    blendBMPReader->SetFileName("/Users/ayamada/Desktop/videoOpenIGTLink/bmpData/1.bmp");
//    blendBMPReader->SetDataSpacing(1.0,1.0,1.0);
//    reslice->SetInput(blendBMPReader->GetOutput());
//
//    reslice->SetOutputSpacing(0.5, 0.5, 0.5);
//
//    texture2->SetInputConnection(reslice->GetOutputPort());
//    actorTexture->SetTexture(texture2);
//
//    actorTexture->GetProperty()->SetOpacity(0.2);
//
//
//    // for window capture
////    this->RenForCapture->AddActor(actorTexture);
////    this->RenWinForCapture->AddRenderer(this->RenForCapture);
//
////    w2if->SetInput(this->RenWinForCapture);
////    wr->SetInputConnection(w2if->GetOutputPort());
//    //    wr->SetFileName("Capture.png");
//    //    wr->Write();
//    //    w2if->Delete();
//    //    wr->Delete();
//
//
////    polyData2IS->SetInputConnection(polyDataNormals->GetOutputPort());
//
//    // image blend test
//    captureBlend->SetInputConnection(this->Importer->GetOutputPort());
////    captureBlend->AddInputConnection(wr->GetOutputPort());
////    captureBlend->AddInputConnection(blendBMPReader->GetOutputPort());
//    captureBlend->SetBlendMode(1);
//    captureBlend->SetOpacity(1,0.4);
//
//    blendMapper->SetInputConnection(captureBlend->GetOutputPort());
//    blendMapper->SetZSlice(0);
//    blendMapper->SetColorWindow(512);
//    blendMapper->SetColorLevel(256);
//
//    blendActor->SetMapper(blendMapper);
//    blendActor->GetProperty()->SetOpacity(0.7);

#ifdef DEBUG_FLAG
  cerr << "Capture Set up" << endl;
#endif

    // capture setup
    if(NULL==(this->Capture = cvCreateCameraCapture(0)))
    {
        cerr << "cannot find a camera" << endl;
        //    return -1;
    }

#ifdef DEBUG_FLAG
  cerr << "Camera found" << endl;
#endif

    //Prepare Capture Window----------------------------------------------------
    this->CaptureMapper->SetInputConnection(this->Importer->GetOutputPort() );

    this->CaptureMapper->SetZSlice(0);
    this->CaptureMapper->SetColorWindow(512);
    this->CaptureMapper->SetColorLevel(256);

    this->CaptureActor->SetMapper( this->CaptureMapper );


    this->CaptureRen->SetBackground( 0, 0, 0 );

    this->CaptureRen->AddActor( this->CaptureActor );

    this->CaptureRenWin->AddRenderer( this->CaptureRen );
    this->CaptureRenWin->SetSize(640,480);

    this->CaptureRenWin->SetPosition(100,100);

    this->CaptureImageTmp = cvQueryFrame( this->Capture );

    this->ImageSize = cvGetSize( this->CaptureImageTmp );
    this->CaptureImage = cvCreateImage(this->ImageSize, IPL_DEPTH_8U,3);
    this->RGBImage = cvCreateImage( this->ImageSize, IPL_DEPTH_8U, 3);

    // initial data
    cvGetImageFromCamera();
    this->Importer->Update();

//    char message[] ="thread test";

    // start thread
//    res = pthread_create(&a_thread, NULL, thread_cvGetImageFromCamera, (void *)message);
//    if(res != 0) {
//        perror("Thread creation faild");
//    //    exit(EXIT_FAILURE);
//    }
////    res = pthread_join(a_thread, &thread_result);

#ifdef DEBUG_FLAG
  cerr << "Start Thread" << endl;
#endif

    this->PlayerThreadId =
              this->PlayerThreader->SpawnThread((vtkThreadFunctionType)\
                  thread_cvGetImageFromCamera, this);

    if(this->PlayerThreadId != -1)
      {
      cerr << "Second Window Thread started" << endl;
      this->Capturing = true;
      }
    else
      {
      cerr << "Second Window Thread failed to start" << endl;
      this->Capturing = false;
      }

    locatorDisp->Delete();
    return locatorModel;

}

//---------------------------------------------------------------------------
int vtkFourDUsEndoNavFirstStep::StopCapture()
{

#ifdef DEBUG_FLAG
  cerr << "StopCapture() entered" << endl;
#endif

  if(this->Capturing)
    {
    this->PlayerThreader->TerminateThread(this->PlayerThreadId);
    this->PlayerThreadId = -1;
    }

#ifdef DEBUG_FLAG
  cerr << "Leave StopCapture()" << endl;
#endif

return 0;
}

//---------------------------------------------------------------------------
vtkMRMLModelNode* vtkFourDUsEndoNavFirstStep::SetCloseOfCaptureData(const char* nodeName)
{

    #ifdef DEBUG_FLAG
      cerr << "SetCloseOfCaptureData entered" << endl;
    #endif

    vtkMRMLModelNode*   locatorModel;

    this->StopCapture();

    #ifdef DEBUG_FLAG
      cerr << "Thread stopped" << endl;
    #endif

    if(this->CaptureMapper)
      {
      this->CaptureMapper->Delete();
      this->CaptureMapper = NULL;
      }
    if(this->CaptureActor)
      {
      this->CaptureActor->Delete();
      this->CaptureActor = NULL;
      }
    if(this->CaptureRen)
      {
      this->CaptureRen->Delete();
      this->CaptureRen = NULL;
      }
    if(this->CaptureRenWin)
      {
      this->CaptureRenWin->Delete();
      this->CaptureRenWin = NULL;
      }

#ifdef DEBUG_FLAG
  cerr << "Capture Instances deleted" << endl;
#endif

  if(this->RGBImage)
    {
    cvReleaseImage(&this->RGBImage);
    this->RGBImage = NULL;
    }
  if(this->CaptureImage)
    {
    cvReleaseImage(&this->CaptureImage);
    this->CaptureImage = NULL;
    }
  if(this->Capture)
    {
    cvReleaseCapture(&this->Capture);
    this->Capture = NULL;
    }

#ifdef DEBUG_FLAG
  cerr << "cvReleases Done" << endl;
#endif

    return locatorModel;
}

