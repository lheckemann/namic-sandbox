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

#include "vtkSecondaryWindowGUI.h"
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
#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkMRMLCameraNode.h"

#include "vtkKWPushButton.h"

#include "vtkCornerAnnotation.h"


#include <cv.h>
#include <cxcore.h>
#include <highgui.h>


// for test
#include "vtkJPEGReader.h"
#include "vtkImageCanvasSource2D.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkSecondaryWindowGUI );
vtkCxxRevisionMacro ( vtkSecondaryWindowGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkSecondaryWindowGUI::vtkSecondaryWindowGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkSecondaryWindowGUI::DataCallback);

  //----------------------------------------------------------------
  // GUI widgets
  this->ShowSecondaryWindowButton = NULL;
  this->HideSecondaryWindowButton = NULL;
  this->SecondaryViewerWindow = NULL;

  this->StartCaptureButton = NULL;
  this->StopCaptureButton  = NULL;

  //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;
  
  //----10.01.12-komura
  this->ThreadLock = 0;
  this->ThreadID = -1;
  this->Mutex = vtkMutexLock::New();
  this->Thread = vtkMultiThreader::New();

}


//---------------------------------------------------------------------------
vtkSecondaryWindowGUI::~vtkSecondaryWindowGUI ( )
{

  //----------------------------------------------------------------
  // Thread
  
  if (this->Thread)
    {
    this->Thread->Delete();
    }
  if (this->Mutex)
    {
    this->Mutex->Delete();
    }
  
  
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

  if (this->SecondaryViewerWindow)
    {
    this->SecondaryViewerWindow->Withdraw();
    this->SecondaryViewerWindow->SetApplication(NULL);
    this->SecondaryViewerWindow->Delete();
    this->SecondaryViewerWindow = NULL;
    }

  if (this->ShowSecondaryWindowButton)
    {
    this->ShowSecondaryWindowButton->SetParent(NULL);
    this->ShowSecondaryWindowButton->Delete();
    }

  if (this->HideSecondaryWindowButton)
    {
    this->HideSecondaryWindowButton->SetParent(NULL);
    this->HideSecondaryWindowButton->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::Enter()
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
void vtkSecondaryWindowGUI::Exit ( )
{
  // Fill in
}

//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::TearDownGUI() 
{
  /*
  this->PrimaryMonitorRobotViewerWidget->SetApplication(NULL);
  this->PrimaryMonitorRobotViewerWidget->SetMainViewerWidget(NULL);
  this->PrimaryMonitorRobotViewerWidget->SetMRMLScene(NULL);
  */
}



//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "SecondaryWindowGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->ShowSecondaryWindowButton)
    {
    this->ShowSecondaryWindowButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->HideSecondaryWindowButton)
    {
    this->HideSecondaryWindowButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->StartCaptureButton)
    {
    this->StartCaptureButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->StopCaptureButton)
    {
    this->StopCaptureButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::AddGUIObservers ( )
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

  this->ShowSecondaryWindowButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->HideSecondaryWindowButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  if (this->CameraNodeSelector)
    {
    this->CameraNodeSelector
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    this->CameraNodeSelector
      ->AddObserver(vtkSlicerNodeSelectorWidget::NewNodeEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    }

  this->StartCaptureButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->StopCaptureButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkSecondaryWindowLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  
  if (this->ShowSecondaryWindowButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    if (this->SecondaryViewerWindow)
      {  
      this->SecondaryViewerWindow->DisplayOnSecondaryMonitor();
      }
    }
  else if (this->HideSecondaryWindowButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    if (this->SecondaryViewerWindow)
      {  
      this->SecondaryViewerWindow->Withdraw();
      }
    }
  else if (this->CameraNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent )
    {
    vtkMRMLCameraNode* cameraNode = 
      vtkMRMLCameraNode::SafeDownCast(this->CameraNodeSelector->GetSelected());
    this->SecondaryViewerWindow->SetCameraNode(cameraNode);
    }
  else if (this->CameraNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NewNodeEvent) 
    {
    vtkMRMLCameraNode* cameraNode = 
      vtkMRMLCameraNode::SafeDownCast(this->CameraNodeSelector->GetSelected());
    this->SecondaryViewerWindow->SetCameraNode(cameraNode);
    }
  else if (this->StartCaptureButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    vtkSlicerViewerWidget* vwidget = this->GetApplicationGUI()->GetNthViewerWidget(0);
    SwitchViewerBackground(vwidget, 1);
    //this->StartCameraThread();
    }
  else if (this->StopCaptureButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    vtkSlicerViewerWidget* vwidget = this->GetApplicationGUI()->GetNthViewerWidget(0);
    SwitchViewerBackground(vwidget, 0);
    //this->StopCameraThread();
    }

} 


//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkSecondaryWindowGUI *self = reinterpret_cast<vtkSecondaryWindowGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkSecondaryWindowGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkSecondaryWindowLogic::SafeDownCast(caller))
    {
    if (event == vtkSecondaryWindowLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::ProcessTimerEvents()
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
void vtkSecondaryWindowGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "SecondaryWindow", "SecondaryWindow", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForWindowConfigurationFrame();

  this->SecondaryViewerWindow = vtkSecondaryWindowViwerWindow::New();
  this->SecondaryViewerWindow->SetApplication(this->GetApplication());
  this->SecondaryViewerWindow->Create();

  vtkMRMLCameraNode* cameraNode = vtkMRMLCameraNode::New();
  this->SecondaryViewerWindow->SetCameraNode(cameraNode);

  //// no view in the scene and local
  //// create an active camera
  //vtkMRMLViewNode* node = vtkMRMLViewNode::New();
  //this->MRMLScene->AddNode(node);
  //this->SecondaryViewerWindow->SetViewNode(node);
  ////this->SetAndObserveViewNode (node);
  //node->Delete();

}


void vtkSecondaryWindowGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:SecondaryWindow</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "SecondaryWindow" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowGUI::BuildGUIForWindowConfigurationFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("SecondaryWindow");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Secondary Window Configuration");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Switch frame

  vtkKWFrame *switchframe = vtkKWFrame::New();
  switchframe->SetParent(conBrowsFrame->GetFrame());
  switchframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 switchframe->GetWidgetName() );

  // -----------------------------------------
  // Push buttons

  this->ShowSecondaryWindowButton = vtkKWPushButton::New ( );
  this->ShowSecondaryWindowButton->SetParent ( switchframe );
  this->ShowSecondaryWindowButton->Create ( );
  this->ShowSecondaryWindowButton->SetText ("ON");
  this->ShowSecondaryWindowButton->SetWidth (12);

  this->HideSecondaryWindowButton = vtkKWPushButton::New ( );
  this->HideSecondaryWindowButton->SetParent ( switchframe );
  this->HideSecondaryWindowButton->Create ( );
  this->HideSecondaryWindowButton->SetText ("OFF");
  this->HideSecondaryWindowButton->SetWidth (12);

  this->Script("pack %s %s -side left -padx 2 -pady 2", 
               this->ShowSecondaryWindowButton->GetWidgetName(),
               this->HideSecondaryWindowButton->GetWidgetName());

  // -----------------------------------------
  // Node select frame

  vtkKWFrame *selectframe = vtkKWFrame::New();
  selectframe->SetParent(conBrowsFrame->GetFrame());
  selectframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 selectframe->GetWidgetName() );

  this->CameraNodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->CameraNodeSelector->SetParent(selectframe);
  this->CameraNodeSelector->Create();
  this->CameraNodeSelector->SetNodeClass("vtkMRMLCameraNode", NULL, NULL, NULL);
  this->CameraNodeSelector->SetMRMLScene(this->GetMRMLScene());
  this->CameraNodeSelector->SetBorderWidth(2);
  this->CameraNodeSelector->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->CameraNodeSelector->GetWidget()->GetWidget()->SetWidth(24);
  this->CameraNodeSelector->SetLabelText( "Camera node : ");
  this->CameraNodeSelector->SetBalloonHelpString("Select or create a camera.");

  this->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
               this->CameraNodeSelector->GetWidgetName());

  conBrowsFrame->Delete();
  switchframe->Delete();

  // -----------------------------------------
  // Push buttons

  vtkKWFrame *cvframe = vtkKWFrame::New();
  cvframe->SetParent(conBrowsFrame->GetFrame());
  cvframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 cvframe->GetWidgetName() );

  this->StartCaptureButton = vtkKWPushButton::New ( );
  this->StartCaptureButton->SetParent ( cvframe );
  this->StartCaptureButton->Create ( );
  this->StartCaptureButton->SetText ("ON");
  this->StartCaptureButton->SetWidth (12);

  this->StopCaptureButton = vtkKWPushButton::New ( );
  this->StopCaptureButton->SetParent ( cvframe );
  this->StopCaptureButton->Create ( );
  this->StopCaptureButton->SetText ("OFF");
  this->StopCaptureButton->SetWidth (12);

  this->Script("pack %s %s -side left -padx 2 -pady 2", 
               this->StartCaptureButton->GetWidgetName(),
               this->StopCaptureButton->GetWidgetName());

  cvframe->Delete();


}


//----------------------------------------------------------------------------
void vtkSecondaryWindowGUI::UpdateAll()
{
}


//----------------------------------------------------------------------------
int vtkSecondaryWindowGUI::SwitchViewerBackground(vtkSlicerViewerWidget* vwidget, int sw)
{
  
  //vtkSlicerViewerWidget* vwidget = this->GetApplicationGUI()->GetNthViewerWidget(0);
  vtkKWRenderWidget* rwidget;
  vtkRenderWindow* rwindow;

  if (vwidget&&
      (rwidget = vwidget->GetMainViewer()) &&
      (rwindow = rwidget->GetRenderWindow()))
    {
    if (sw)
      {
      if (rwidget->GetNumberOfRenderers() == 1)
        {
        vtkJPEGReader* jpegReader = vtkJPEGReader::New();
        const char* filename = "/Users/junichi/igtdev/slicer_dev/test.jpg";
        if( !jpegReader->CanReadFile( filename ) )
          {
          std::cerr << "Error reading file " << filename << std::endl;
          return EXIT_FAILURE;
          }
        jpegReader->SetFileName ( filename );
        
        jpegReader->Update();
        
        vtkImageData* imageData = jpegReader->GetOutput();
        vtkImageActor* imageActor = vtkImageActor::New();
        imageActor->SetInput(imageData);
        
        vtkRenderer* backgroundRenderer = vtkRenderer::New();
        backgroundRenderer->SetLayer(0);
        backgroundRenderer->InteractiveOff();

        rwidget->GetNthRenderer(0)->SetLayer(1);
        rwidget->AddRenderer(backgroundRenderer);
        backgroundRenderer->AddActor(imageActor);
        rwindow->Render();
        return 0;
        }
      }
    else
      {
      if (rwidget->GetNumberOfRenderers() > 1)
        {
        rwidget->RemoveNthRenderer(1);
        rwidget->GetNthRenderer(0)->SetLayer(0);
        rwindow->Render();
        return 0;
        }
      }
    }
  return 0;
}


//----------------------------------------------------------------------------
// Launch Camera thread
int vtkSecondaryWindowGUI::StartCamera()
{
  if (this->ThreadID < 0)
    {
    this->fRunThread = 1;
    this->ThreadID = this->Thread->SpawnThread((vtkThreadFunctionType) &vtkSecondaryWindowGUI::CameraThread, this);
    //sleep(1); // 10.01.23 ayamada
    return 1;
    }
  else
    {
    return 0;
    }

  return 0;
  // for test

} 

//----------------------------------------------------------------------------
// Stop Camera thread
int vtkSecondaryWindowGUI::StopCamera()
{
  this->fRunThread = 0;
  this->Thread->TerminateThread(this->ThreadID);
  this->ThreadID = -1;
  return 1;
} 


//----------------------------------------------------------------------------
// Camera thread / Originally created by A. Yamada
void *vtkSecondaryWindowGUI::CameraThread(void* t)
{
  // 5/15/ayamada
  CvCapture* capture;
  
  char bufCamera[100],bufCamera1[100];
  
  // 5/15/2010 ayamada
  IplImage*     captureImage;
  IplImage*     RGBImage;
  IplImage*     captureImageTmp;
  IplImage*     undistortionImage;      //adding at 09. 12. 15 - smkim

  CvSize        imageSize;
  
  captureImage = NULL;
  RGBImage = NULL;
  captureImageTmp = NULL;
  undistortionImage = NULL;
  
  
  vtkMultiThreader::ThreadInfo* vinfo = 
    static_cast<vtkMultiThreader::ThreadInfo*>(t);
  vtkSecondaryWindowGUI* pGUI = 
    static_cast<vtkSecondaryWindowGUI*>(vinfo->UserData);
  
  //**************************************************************************
  //   getting camera image initially
  //**************************************************************************
  // 5/15/2010 ayamada
  // for videoOverlay


  // Camera initialization
  int i = 0;
  while (i<=10)
    {// 5/16/2010 ayamada
    if (NULL==(capture = cvCaptureFromCAM(i)))  // 10.01.25 ayamada
      {
      fprintf(stdout, "\n\nCouldn't find a camera\n\n");// 10.01.25 ayamada
      i++;                
      }
    else
      {
      // 5/16/2010 ayamada
      sprintf(bufCamera, "Connected camera device No: %d", i);
      //pGUI->textActorCamera->SetInput(bufCamera);
      break;
      }
    }
  if (i >= 11)
    {
    sprintf(bufCamera, "Couldn't find camera device!!");
    //pGUI->textActorCamera->SetInput(bufCamera);
    }
    
  while (pGUI->fRunThread && capture)
    {
    // 5/15/2010 ayamada
    if(NULL == (captureImageTmp = cvQueryFrame( capture )))
      {
      sleep(2); // 5/18/2010 ayamada
      fprintf(stdout, "\n\nCouldn't take a picture\n\n");
      // 5/15/2010 ayamada
      continue;
      }
    
    // 5/6/2010 ayamada creating RGB image and capture image
    imageSize = cvGetSize( captureImageTmp );
    captureImage = cvCreateImage(imageSize, IPL_DEPTH_8U,3);    
    RGBImage = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);       

    // create rgb image
    // 5/6/2010 ayamada for videoOverlay
    cvFlip(captureImageTmp, captureImage, 0);
    
    // 5/6/2010 for videoOverlay ayamada
    undistortionImage = cvCreateImage( imageSize, IPL_DEPTH_8U, 3);
    
    // capture image
    // 5/6/2010 ayamada for videoOverlay
    
    /*
    cvUndistort2( captureImage, undistortionImage, pGUI->intrinsicMatrix, pGUI->distortionCoefficient );            
    */
    
    cvCvtColor( undistortionImage, RGBImage, CV_BGR2RGB);       //comment not to undistort      at 10. 01. 07 - smkim

    unsigned char* idata;    
    // 5/6/2010 ayamada ok for videoOverlay
    idata = (unsigned char*) RGBImage->imageData;
    
    pGUI->importer->SetWholeExtent(0,imageSize.width-1,0,imageSize.height-1,0,0);
    //this->importer->SetDataSpacing( 2.0, 2.0, 2.0); // 5/6/2010 ayamada for videoOverlay
    pGUI->importer->SetDataExtentToWholeExtent();
    pGUI->importer->SetDataScalarTypeToUnsignedChar();
    pGUI->importer->SetNumberOfScalarComponents(3);
    pGUI->importer->SetImportVoidPointer(idata);
    
    // 5/6/2010 ayamada fot videoOverlay
    /*
    pGUI->atext->SetInputConnection(pGUI->importer->GetOutputPort());
    pGUI->atext->InterpolateOn();            
    
    pGUI->importer->Update();
    */
    
    //first = 1;
    break;//10.01.20-komura
    }

#if 0
    
  //**************************************************************************
  //**************************************************************************
  //  Setting visualization environment for camera image up
  //**************************************************************************
  // 5/15/2010 ayamada
  // for videoOverlay
  
  pGUI->planeRatio = VIEW_SIZE_X / VIEW_SIZE_Y;
  
  // 5/6/2010 ayamada for videoOverlay
  //CameraFocusPlane(this->fileCamera, this->planeRatio, this->actor);
  pGUI->CameraFocusPlane(pGUI->fileCamera, pGUI->planeRatio);
  
  
  // 5/7/2010 ayamada
  pGUI->FocalPlaneMapper->SetInput(pGUI->FocalPlaneSource->GetOutput());
  pGUI->actor->SetMapper(pGUI->FocalPlaneMapper);
  pGUI->actor->SetUserMatrix(pGUI->ExtrinsicMatrix);
  
  // 5/6/2010 ayamada for videoOverlay
  // 10.01.24 ayamada
  pGUI->actor->SetTexture(pGUI->atext);        // texture mapper
  //this->actor->GetProperty()->SetOpacity(1.0);// configuration property
  //this->SecondaryViewerWindow->rw->AddViewProp(this->actor);
  // 5/8/2010 ayamada
  pGUI->SecondaryViewerWindow->rw->GetRenderer()->AddActor(pGUI->actor);
  
  
  // 10.01.25 ayamada
  //        pGUI->Mutex->Lock();
  //        pGUI->SecondaryViewerWindow->rw->Render();
  //        pGUI->Mutex->Unlock();
  pGUI->updateView=1;
  
  //sleep(1);   // 10.01.25 ayamada
  
  //      pGUI->SecondaryViewerWindow->rw->ResetCamera(); //10.01.27-komura
  //    pGUI->SecondaryViewerWindow->rw->SetCameraPosition(0,0,0);
  
  fprintf(stdout, "\nget camera handle\n");//10.01.20-komura
  
  //**************************************************************************
  
  // 5/8/2010 ayamada
  //vtkRayCastingVolumeRender();
  //vtkTexture3DVolumeRender();
  //vtkCUDAVolumeRender();
  
  //------------------------------------------------------------------
  //   Setting up visualization environment for rendering
  //------------------------------------------------------------------
  // 5/15/2010 ayamada
  // camera position
  // 5/6/2010 ayamada for videoOverlay. 
  // The details are in the following URL: http://www.paraview.org/doc/nightly/html/classvtkKWRenderWidget.html
  pGUI->SecondaryViewerWindow->rw->GetRenderer()->GetActiveCamera()->ParallelProjectionOff();
  pGUI->SecondaryViewerWindow->rw->GetRenderer()->SetActiveCamera( pGUI->fileCamera );
  
  //**************************************************************************
  
}       // 5/16/2010 if(capture != NULL)

  
  if (capture != NULL)
    {   // 5/16/2010 ayamada
    // 5/6/2010 ayamada for videoOverlay
    if ( pGUI->m_bDriveSource == false )        // driven by manual
      {
      //m_pLogic->volume->SetOrientation(m_pLogic->rotationAngleX, m_pLogic->rotationAngleY, m_pLogic->rotationAngleZ);         // adding at 09. 9. 8 - smkim
      //m_pLogic->volume->SetPosition(m_pLogic->translationX, m_pLogic->translationY, m_pLogic->translationZ);                  // adding at 09. 9. 14 - smkim
      
      //m_pLogic->fileCamera->Yaw(m_pLogic->rotationAngleZ);
      
      }
    else        // driven by optical tracking system
      {
      // 5/15/2010 ayamada                      
      pGUI->GetCurrentTransformMatrix ();
      pGUI->CameraSet(pGUI->fileCamera, pGUI->cameraMatrix, FOA);
      // 5/7/2010 ayamada
      pGUI->CameraFocusPlane(pGUI->fileCamera, pGUI->planeRatio);                               
      }                
    
    
    // 5/15/2010 ayamada
    captureImageTmp = cvQueryFrame( capture );  // 10.01.23 ayamada
    ////pGUI->imageSize = cvGetSize( pGUI->captureImageTmp );
    
    cvFlip(captureImageTmp, captureImage, 0);        
    
    cvUndistort2( captureImage, undistortionImage, pGUI->intrinsicMatrix, pGUI->distortionCoefficient );
    
    // 5/7/2010 ayamada
    //cvCvtColor( pGUI->captureImage, pGUI->RGBImage, CV_BGR2RGB);
    // 5/15/2010 ayamada
    cvCvtColor( undistortionImage, RGBImage, CV_BGR2RGB);
    
    pGUI->idata = (unsigned char*) RGBImage->imageData;
    pGUI->importer->Modified();         
    
    }   // 5/16/2010 ayamada    if(capture != NULL) 2           
    
    
if(capture != NULL)
    {   
    // 5/15/2010 ayamada
    cvReleaseCapture(&capture);  
    }
  
#endif

  return NULL;
         
}



