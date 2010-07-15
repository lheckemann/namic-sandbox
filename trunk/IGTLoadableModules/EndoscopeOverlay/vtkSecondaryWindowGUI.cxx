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
#include "vtkMRMLLinearTransformNode.h"


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

  this->TimerFlag = 0;
  this->CameraActiveFlag = 0;


  this->VideoImageData     = NULL;
  this->BackgroundRenderer = NULL;

  this->OpticalFlowTrackingFlag = 1;
  this->GrayImage = NULL;
  this->PrevGrayImage = NULL;
  this->Pyramid = NULL;
  this->PrevPyramid = NULL;
  this->SwapTempImage = NULL;
  this->PyrFlag = 0;

}


//---------------------------------------------------------------------------
vtkSecondaryWindowGUI::~vtkSecondaryWindowGUI ( )
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
    this->TimerInterval = 30;  // 100 ms
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

  if (this->TransformNodeSelector)
    {
    this->TransformNodeSelector
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    this->TransformNodeSelector
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
  else if (this->TransformNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent )
    {
    /*
    vtkMRMLCameraNode* cameraNode = 
      vtkMRMLCameraNode::SafeDownCast(this->TransformNodeSelector->GetSelected());
    if (this->SecondaryViewerWindow)
      {
      this->SecondaryViewerWindow->SetCameraNode(cameraNode);
      }
    */
    }
  else if (this->TransformNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NewNodeEvent) 
    {
    /*
    vtkMRMLCameraNode* cameraNode = 
      vtkMRMLCameraNode::SafeDownCast(this->TransformNodeSelector->GetSelected());
    if (this->SecondaryViewerWindow)
      {
      this->SecondaryViewerWindow->SetCameraNode(cameraNode);
      }
    */
    }
  else if (this->StartCaptureButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    this->StartCamera();
    }
  else if (this->StopCaptureButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    this->StopCamera();
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
    if (this->CameraActiveFlag)
      {
      CameraHandler();
      }
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

  /*
  this->SecondaryViewerWindow = vtkSecondaryWindowViwerWindow::New();
  this->SecondaryViewerWindow->SetApplication(this->GetApplication());
  this->SecondaryViewerWindow->Create();
  */

  vtkMRMLCameraNode* cameraNode = vtkMRMLCameraNode::New();
  //this->SecondaryViewerWindow->SetCameraNode(cameraNode);

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

  this->TransformNodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->TransformNodeSelector->SetParent(selectframe);
  this->TransformNodeSelector->Create();
  this->TransformNodeSelector->SetNodeClass("vtkMRMLLinearTransformNode", NULL, NULL, "LinearTransform");
  this->TransformNodeSelector->SetNewNodeEnabled(1);
  this->TransformNodeSelector->SetMRMLScene(this->GetMRMLScene());
  this->TransformNodeSelector->SetBorderWidth(2);
  this->TransformNodeSelector->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->TransformNodeSelector->GetWidget()->GetWidget()->SetWidth(24);
  this->TransformNodeSelector->SetLabelText( "Transform : ");
  this->TransformNodeSelector->SetBalloonHelpString("Select or create a camera.");

  this->Script("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
               this->TransformNodeSelector->GetWidgetName());

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
int vtkSecondaryWindowGUI::ViewerBackgroundOn(vtkSlicerViewerWidget* vwidget, vtkImageData* imageData)
{
  
  vtkKWRenderWidget* rwidget;
  vtkRenderWindow* rwindow;

  if (vwidget&&
      (rwidget = vwidget->GetMainViewer()) &&
      (rwindow = rwidget->GetRenderWindow()))
    {
    if (rwidget->GetNumberOfRenderers() == 1)
      {
      this->BackgroundRenderer = vtkRenderer::New();
      this->BackgroundActor = vtkImageActor::New();
      this->BackgroundActor->SetInput(imageData);
      this->BackgroundRenderer->AddActor(this->BackgroundActor);
      this->BackgroundRenderer->InteractiveOff();
      this->BackgroundRenderer->SetLayer(0);

      // Adjust camera position so that image covers the draw area.

      this->BackgroundActor->Modified();
      rwidget->GetNthRenderer(0)->SetLayer(1);
      rwidget->AddRenderer(this->BackgroundRenderer);
      rwindow->Render();

      vtkCamera* camera = this->BackgroundRenderer->GetActiveCamera();
      double x, y, z;
      camera->GetPosition(x, y, z);
      camera->SetViewAngle(90.0);
      camera->SetPosition(x, y, y); 

      return 1;
      }
    }
  return 0;
}


//----------------------------------------------------------------------------
int vtkSecondaryWindowGUI::ViewerBackgroundOff(vtkSlicerViewerWidget* vwidget)
{
  vtkKWRenderWidget* rwidget;
  vtkRenderWindow* rwindow;

  if (vwidget&&
      (rwidget = vwidget->GetMainViewer()) &&
      (rwindow = rwidget->GetRenderWindow()))
    {
    if (rwidget->GetNumberOfRenderers() > 1)
      {
      rwidget->RemoveNthRenderer(1);
      rwidget->GetNthRenderer(0)->SetLayer(0);
      rwindow->Render();
      this->BackgroundRenderer = NULL;
      this->BackgroundActor = NULL;
      }
    }

  return 0;

}  




//----------------------------------------------------------------------------
// Launch Camera thread
int vtkSecondaryWindowGUI::StartCamera()
{

  this->capture      = NULL;
  this->captureImage = NULL;
  this->RGBImage     = NULL;
  this->undistortionImage = NULL;
  this->imageSize.width = 0;
  this->imageSize.height = 0;

  vtkSlicerViewerWidget* vwidget = this->GetApplicationGUI()->GetNthViewerWidget(0);

  if (!this->VideoImageData)
    {
    this->VideoImageData = vtkImageData::New();
    this->VideoImageData->SetDimensions(64, 64, 1);
    this->VideoImageData->SetExtent(0, 63, 0, 63, 0, 0 );
    this->VideoImageData->SetSpacing(1.0, 1.0, 1.0);
    this->VideoImageData->SetOrigin(0.0, 0.0, 0.0);
    this->VideoImageData->SetNumberOfScalarComponents(3);
    this->VideoImageData->SetScalarTypeToUnsignedChar();
    this->VideoImageData->AllocateScalars();
    }
  this->VideoImageData->Update();
  ViewerBackgroundOn(vwidget, this->VideoImageData);

  //**************************************************************************
  //   getting camera image initially
  //**************************************************************************
  // 5/15/2010 ayamada
  // for videoOverlay
  // Camera initialization
  int i = 0;

  while (i<=10)
    {// 5/16/2010 ayamada
    if (NULL==(this->capture = cvCaptureFromCAM(i)))  // 10.01.25 ayamada
      {
      fprintf(stdout, "\n\nCouldn't find a camera\n\n");// 10.01.25 ayamada
      i++;                
      }
    else
      {
      // 5/16/2010 ayamada
      //sprintf(bufCamera, "Connected camera device No: %d", i);
      //pGUI->textActorCamera->SetInput(bufCamera);
      break;
      }
    }

  if (i >= 11)
    {
    //sprintf(bufCamera, "Couldn't find camera device!!");
    //pGUI->textActorCamera->SetInput(bufCamera);
    }

  this->CameraActiveFlag = 1;

  return 0;
  // for test
} 

//----------------------------------------------------------------------------
// Stop Camera thread
int vtkSecondaryWindowGUI::StopCamera()
{
  this->CameraActiveFlag = 0;

  cvReleaseCapture(&(this->capture));

  vtkSlicerViewerWidget* vwidget = this->GetApplicationGUI()->GetNthViewerWidget(0);
  ViewerBackgroundOff(vwidget);

  return 1;
} 

#define NGRID_X  20
#define NGRID_Y  20
#define TIME_POINTS (30*30)

//----------------------------------------------------------------------------
// Camera thread / Originally created by A. Yamada
int vtkSecondaryWindowGUI::CameraHandler()
{
  IplImage* captureImageTmp = NULL;
  CvSize   newImageSize;
  
  if (this->capture)
    {
    // 5/15/2010 ayamada
    if(NULL == (captureImageTmp = cvQueryFrame( capture )))
      {
      fprintf(stdout, "\n\nCouldn't take a picture\n\n");
      return 0;
      }

    // 5/6/2010 ayamada creating RGB image and capture image
    newImageSize = cvGetSize( captureImageTmp );

    // check if the image size is changed
    if (newImageSize.width != this->imageSize.width ||
        newImageSize.height != this->imageSize.height)
      {
      this->imageSize.width = newImageSize.width;
      this->imageSize.height = newImageSize.height;
      this->captureImage = cvCreateImage(this->imageSize, IPL_DEPTH_8U,3);
      this->RGBImage = cvCreateImage(imageSize, IPL_DEPTH_8U, 3);
      this->undistortionImage = cvCreateImage( this->imageSize, IPL_DEPTH_8U, 3);

      this->VideoImageData->SetDimensions(newImageSize.width, newImageSize.height, 1);
      this->VideoImageData->SetExtent(0, newImageSize.width-1, 0, newImageSize.height-1, 0, 0 );
      this->VideoImageData->SetNumberOfScalarComponents(3);
      this->VideoImageData->SetScalarTypeToUnsignedChar();
      this->VideoImageData->AllocateScalars();
      this->VideoImageData->Update();

      vtkSlicerViewerWidget* vwidget = this->GetApplicationGUI()->GetNthViewerWidget(0);
      ViewerBackgroundOff(vwidget);
      ViewerBackgroundOn(vwidget, this->VideoImageData);


      // for optical flow
      this->Pyramid       = cvCreateImage( this->imageSize , IPL_DEPTH_8U, 1 );
      this->PrevPyramid   = cvCreateImage( this->imageSize , IPL_DEPTH_8U, 1 );
      this->GrayImage     = cvCreateImage( this->imageSize , IPL_DEPTH_8U, 1 );
      this->PrevGrayImage = cvCreateImage( this->imageSize , IPL_DEPTH_8U, 1 );
      this->SwapTempImage = cvCreateImage( this->imageSize , IPL_DEPTH_8U, 1 );

      double gridSpaceX = (double)newImageSize.width / (double)(NGRID_X+1);
      double gridSpaceY = (double)newImageSize.height / (double)(NGRID_Y+1);

      this->Points[0] = 0;
      this->Points[1] = 0;
      this->GridPoints[0] = (CvPoint2D32f*)cvAlloc(NGRID_X*NGRID_Y*sizeof(this->Points[0][0]));
      this->GridPoints[1] = (CvPoint2D32f*)cvAlloc(NGRID_X*NGRID_Y*sizeof(this->Points[0][0]));
      this->RVector = (CvPoint2D32f*)cvAlloc(NGRID_X*NGRID_Y*sizeof(this->Points[0][0]));

      for (int i = 0; i < NGRID_X; i ++)
        {
        for (int j = 0; j < NGRID_Y; j ++)
          {
          this->GridPoints[0][i+j*NGRID_X].x = gridSpaceX*i + gridSpaceX;
          this->GridPoints[0][i+j*NGRID_Y].y = gridSpaceY*j + gridSpaceY;
          }
        }

      this->OpticalFlowStatus = (char*)cvAlloc(NGRID_X*NGRID_Y);
      }
    
    // create rgb image
    // 5/6/2010 ayamada for videoOverlay
    cvFlip(captureImageTmp, this->captureImage, 0);
    
    //cvUndistort2( captureImage, undistortionImage, pGUI->intrinsicMatrix, pGUI->distortionCoefficient );            
    //cvCvtColor( undistortionImage, RGBImage, CV_BGR2RGB);       //comment not to undistort      at 10. 01. 07 - smkim
    cvCvtColor( captureImage, RGBImage, CV_BGR2RGB);       //comment not to undistort      at 10. 01. 07 - smkim
    
    if (this->OpticalFlowTrackingFlag)
      {
      int win_size = 10;
      int count = NGRID_X*NGRID_Y;
      cvCvtColor(this->RGBImage, this->GrayImage, CV_RGB2GRAY );
      cvCalcOpticalFlowPyrLK( this->PrevGrayImage, this->GrayImage, this->PrevPyramid, this->Pyramid,
                              this->GridPoints[0], this->GridPoints[1], count, cvSize(win_size,win_size), 3, this->OpticalFlowStatus, 0,
                              cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), this->PyrFlag );
      this->PyrFlag |= CV_LKFLOW_PYR_A_READY;
      
      double dx = 0.0;
      double dy = 0.0;
      int frameCount = 0;

      for(int i =  0; i < count; i++ )
        {
        if( !this->OpticalFlowStatus[i] )
          {
          this->GridPoints[1][i].x = this->GridPoints[0][i].x;
          this->GridPoints[1][i].y = this->GridPoints[0][i].y;
          }
        dx = this->GridPoints[1][i].x - this->GridPoints[0][i].x;
        dy = this->GridPoints[1][i].y - this->GridPoints[0][i].y;
        
        if (sqrt(dx*dx + dy*dy) > 50.0)
          {
          this->GridPoints[1][i].x = this->GridPoints[0][i].x;
          this->GridPoints[1][i].y = this->GridPoints[0][i].y;
          dx = 0.0;
          dy = 0.0;
          }
        
        this->RVector[i].x = dx;
        this->RVector[i].y = dy;
        cvCircle(this->RGBImage, cvPointFrom32f(this->GridPoints[0][i]), 3, CV_RGB(0,255,255), -1, 8,0);
        cvLine(this->RGBImage, cvPointFrom32f(this->GridPoints[0][i]), cvPointFrom32f(this->GridPoints[1][i]), CV_RGB(0,255,0), 2);
        }
      
      CV_SWAP( this->PrevGrayImage, this->GrayImage, this->SwapTempImage );
      CV_SWAP( this->PrevPyramid, this->Pyramid, this->SwapTempImage );
      CV_SWAP( this->Points[0], this->Points[1], this->SwapPoints );

      ProcessMotion(this->RVector, this->GridPoints[0], count);
      }
    
    unsigned char* idata;    
    // 5/6/2010 ayamada ok for videoOverlay
    idata = (unsigned char*) RGBImage->imageData;
    
    int dsize = this->imageSize.width*this->imageSize.height*3;
    memcpy((void*)this->VideoImageData->GetScalarPointer(), (void*)this->RGBImage->imageData, dsize);

    if (this->VideoImageData && this->BackgroundRenderer)
      {
      //this->VideoImageData->Update();
      this->VideoImageData->Modified();
      this->BackgroundRenderer->GetRenderWindow()->Render();
      }

    }
  
  return 1;
  
}


//----------------------------------------------------------------------------
int vtkSecondaryWindowGUI::ProcessMotion(CvPoint2D32f* vector, CvPoint2D32f* position, int n)
{
  float threshold = 5.0;
  CvPoint2D32f mean;

  mean.x = 0.0;
  mean.y = 0.0;

  // Use 10% vectors to calculate translation
  for (int i = 0; i < n; i ++)
    {
    float x = vector[i].x;
    float y = vector[i].y;
    float len = sqrtf(x*x + y*y);
    if (len > threshold)
      {
      mean.x += x;
      mean.y += y;
      }
    }
  mean.x /= (float)n;
  mean.y /= (float)n;

  vtkMRMLLinearTransformNode* transformNode = 
    vtkMRMLLinearTransformNode::SafeDownCast(this->TransformNodeSelector->GetSelected());
  if (transformNode)
    {
    vtkMatrix4x4* transform = transformNode->GetMatrixTransformToParent();
    if (transform)
      {
      float x = transform->GetElement(0, 3) - mean.x*2.0;
      float y = transform->GetElement(2, 3) + mean.y*2.0;
      transform->SetElement(0, 3, x);
      transform->SetElement(2, 3, y);
      }
    }
  
}



