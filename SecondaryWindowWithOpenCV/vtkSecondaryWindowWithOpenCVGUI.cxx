/*==========================================================================
 
 Portions (c) Copyright 2010 Atsushi Yamada (Fujimoto Lab, Nagoya Institute of Technology (NIT)) 
                 and M. Komura (NIT) All Rights Reserved.
 
 Acknowledgement: K. Chinzei (AIST), Y. Hayashi (Nagoya Univ.), T. Takeuchi (SFC Corp.), H. Liu (BWH), J. Tokuda (BWH), N. Hata (BWH), and H. Fujimoto (NIT). 
 CMakeLists.txt, FindOpenCV.cmake, and FindOpenIGTLink.cmake are contributions of K. Chinzei(AIST) and T. Takeuchi (SFC Corp.).

 !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!
 This module is based on the "Secondary Window" module by J. Tokuda (BWH).
 !+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!+!
 
 See README.txt
 or http://www.slicer.org/copyright/copyright.txt for details.
 
 Program:   SecondaryWindowWithOpenCV
 Module:    $HeadURL: $
 Date:      $Date:01/25/2010 $
 Version:   $Revision: $
 
 ==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkSecondaryWindowWithOpenCVGUI.h"
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

// 10.01.25 ayamada
#include "vtkProperty.h"

// 4/23/2010 ayamada
#include "vtkMRMLLinearTransformNode.h"

// 5/6/2010 ayamada
#include "vtkKWScale.h"        //adding at 09. 9. 8 - smkim


int first=0;//10.01.12-komura
//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkSecondaryWindowWithOpenCVGUI );
vtkCxxRevisionMacro ( vtkSecondaryWindowWithOpenCVGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkSecondaryWindowWithOpenCVGUI::vtkSecondaryWindowWithOpenCVGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkSecondaryWindowWithOpenCVGUI::DataCallback);
  
  //----------------------------------------------------------------
  // GUI widgets
  
  // 5/7/2010 ayamada
  //this->ShowSecondaryWindowWithOpenCVButton = NULL;
  //this->HideSecondaryWindowWithOpenCVButton = NULL;

  this->SecondaryViewerWindow = NULL;
  this->SecondaryViewerWindow2 = NULL; // 6/23/2010 ayamada

    // 5/6/2010 ayamada for videoOverlay
    this->NS_ImageData = NULL;     // adding at 09.08.19 - smkim
    this->VolumeHeaderWidget = NULL;       // adding at 09. 9. 2 - smkim
    this->DetailsFrame = NULL;        //Frame Details    // adding at 09. 9. 4 - smkim
    
    this->RotationAngleX = NULL;        // adding at 09. 9. 8 - smkim
    this->RotationAngleY = NULL;        // adding at 09. 9. 8 - smkim
    this->RotationAngleZ = NULL;        // adding at 09. 9. 8 - smkim
    
    this->TranslationX = NULL;        // adding at 09. 9. 14 - smkim
    this->TranslationY = NULL;        // adding at 09. 9. 14 - smkim
    this->TranslationZ = NULL;        // adding at 09. 9. 14 - smkim
    
    this->TransformNodeNameEntry = NULL;    // adding at 09. 9. 16 - smkim
    
    this->selectDriveSource = NULL;     // adding at 09. 9. 17 - smkim
    
    this->showCaptureData = NULL;
    this->closeCaptureData = NULL;
    
    this->loadLensDistortionCoefButton = NULL;    // adding at 10. 2. 23 - smkim
    this->loadIntrinsicParameterButton = NULL;    // adding at 10. 2. 23 - smkim         

    // 5/17/2010 ayamada
    this->captureCameraImage = NULL;
    this->saveCameraImageEntry = NULL;         

    
    //----------------------------------------------------------------
  // Locator  (MRML)
  this->TimerFlag = 0;

//----10.01.12-komura
  this->threadLock = 0;
  this->ThreadID = -1;
  this->Mutex = vtkMutexLock::New();
  this->Thread = vtkMultiThreader::New();

// 10.01.24 ayamada
  this->updateView = 0;
  this->updateViewTriger = 0;
  this->camNumber = 0;
  this->secView = 0;  // 5/5/2010 ayamada

/*    // 5/16/2010 ayamada
    captureImage = NULL;
    RGBImage = NULL;
    captureImageTmp = NULL;
*/

    // 6/20/2010 ayamada
    for(int i=0; i<2; i++){
    idata[i] = NULL;
    importer[i] = vtkImageImport::New();
    atext[i] = vtkTexture::New();
    actor[i] = vtkActor::New();
    }

    planeSource = vtkPlaneSource::New();
    planeMapper = vtkPolyDataMapper::New();

    // 10.01.25 ayamada
    planeSourceLeftPane = vtkPlaneSource::New();
    planeMapperLeftPane = vtkPolyDataMapper::New();
    actorLeftPane = vtkActor::New();

    // 5/7/2010 ayamada
    // for the function "CameraFocusPlane"
    FocalPlaneSource = vtkPlaneSource::New();
    FocalPlaneMapper = vtkPolyDataMapper::New();    
    ExtrinsicMatrix = vtkMatrix4x4::New();
    this->Pos[0] = 0.0;
    this->Pos[1] = 0.0;
    this->Pos[2] = 0.0;
    this->Focal[0] = 0.0;
    this->Focal[1] = 0.0;
    this->Focal[2] = 0.0;
    this->F = 0.0;
    this->ViewAngle = 0.0;
    this->h=0.0;
    this->fx=0.0;
    this->fy=0.0;
    
    
    // 4/25/2010 ayamada
    // for text overlay
    textActor = vtkTextActor::New();
    textActor1 = vtkTextActor::New();
    textActor2 = vtkTextActor::New();
    textActor3 = vtkTextActor::New();
    
    // 5/16/2010 ayamada
    textActorCamera = vtkTextActor::New();
    textActorCamera1 = vtkTextActor::New();

    // 5/17/2010 ayamada
    textActorSavePath = vtkTextActor::New();
    textActorSavePathH = vtkTextActor::New();
    
    this->runThread = 0;//10.01.21-komura
    
    // 5/5/2010 ayamada
    // for videoOverlay
    //atext = vtkTexture::New();
    plane = vtkPlaneSource::New();
    volume = vtkVolume::New();    
    polyActor = vtkActor::New();    //adding at 10. 02. 01 - smkim
    polyActor1 = vtkActor::New();    //adding at 10. 02. 05 - smkim

    // 5/11/2010 ayamada
   polyReader = vtkPolyDataReader::New(); 
   polyNormal = vtkPolyDataNormals::New(); 
   polyMapper = vtkPolyDataMapper::New(); 
   polyReader1 = vtkPolyDataReader::New(); 
   polyNormal1 = vtkPolyDataNormals::New(); 
   polyMapper1 = vtkPolyDataMapper::New();     
    
    
    ren = vtkRenderer::New();
    ren1 = vtkRenderer::New();            // adding at 09. 9. 14 - smkim
    captureRen = vtkRenderer::New();
    captureRenWin = vtkRenderWindow::New();
    
    tempMatrix = vtkMatrix4x4::New();
    tempMatrix->Identity();  
    
    compensationMatrix = vtkMatrix4x4::New();
    compensationMatrix->Identity();
    
    cameraTransform = vtkTransform::New();
    
    cameraMatrix = new double[16];
    
    
    // Query image from camera
    
    // 5/16/2010 ayamada
    //captureImage = NULL;
    //RGBImage = NULL;
    
    //captureImageTmp = NULL;
    //undistortionImage = NULL;
    
//    intrinsicMatrix = cvCreateMat( 3, 3, CV_64FC1 );    // initialization of intrinsic matrix    //adding at 09. 12. 15 - smkim
//    distortionCoefficient = cvCreateMat( 4, 1, CV_64FC1 );    // initialization of distortion coefficient    //adding at 09. 12. 15 - smkim
    
    /*
     //    for microscope
     // Setting element value of intrinsic matrix
     //    cvSetIdentity( intrinsicMatrix );
     cvmSet( intrinsicMatrix, 0, 0, 3533.685468 );
     cvmSet( intrinsicMatrix, 0, 1, 0.0 );
     cvmSet( intrinsicMatrix, 0, 2, 463.686979 );
     
     cvmSet( intrinsicMatrix, 1, 0, 0.0 );
     cvmSet( intrinsicMatrix, 1, 1, 3471.553424 );
     cvmSet( intrinsicMatrix, 1, 2, 57.813819 );
     
     cvmSet( intrinsicMatrix, 2, 0, 0.0 );
     cvmSet( intrinsicMatrix, 2, 1, 0.0 );
     cvmSet( intrinsicMatrix, 2, 2, 1.0 );
     
     // Setting element value of distortion coefficient
     cvmSet( distortionCoefficient, 0, 0, -2.645990 );
     cvmSet( distortionCoefficient, 1, 0, 32.091185 );
     cvmSet( distortionCoefficient, 2, 0, 0.066980 );
     cvmSet( distortionCoefficient, 3, 0, -0.065260 );
     */
    
    //    for endoscope
    // Setting element value of intrinsic matrix
    //    cvSetIdentity( intrinsicMatrix );

    for(int k = 0; k<2; k++)
    {
        intrinsicMatrix[k] = cvCreateMat( 3, 3, CV_64FC1 );    // initialization of intrinsic matrix    //adding at 09. 12. 15 - smkim
        distortionCoefficient[k] = cvCreateMat( 4, 1, CV_64FC1 );    // initialization of distortion coefficient    //adding at 09. 12. 15 - smkim
        
        
    cvmSet( intrinsicMatrix[k], 0, 0, 906.759458 );
    cvmSet( intrinsicMatrix[k], 0, 1, 0.0 );
    cvmSet( intrinsicMatrix[k], 0, 2, 299.418431 );
    
    cvmSet( intrinsicMatrix[k], 1, 0, 0.0 );
    cvmSet( intrinsicMatrix[k], 1, 1, 895.109642 );
    cvmSet( intrinsicMatrix[k], 1, 2, 178.543373 );
    
    cvmSet( intrinsicMatrix[k], 2, 0, 0.0 );
    cvmSet( intrinsicMatrix[k], 2, 1, 0.0 );
    cvmSet( intrinsicMatrix[k], 2, 2, 1.0 );
    
    // Setting element value of distortion coefficient
    cvmSet( distortionCoefficient[k], 0, 0, -0.747239 );
    cvmSet( distortionCoefficient[k], 1, 0, 0.521642 );
    cvmSet( distortionCoefficient[k], 2, 0, -0.003403 );
    cvmSet( distortionCoefficient[k], 3, 0, 0.002532 );
    }
    // calculating focal point with intrinsic matrix
    // adding at 10. 02. 22 - smkim
    focal_point_x = (VIEW_SIZE_X / 2.0) - cvmGet(intrinsicMatrix[0], 0, 2);    //299.418431;    //intrinsicMatrix->GetElement(0, 2);
    focal_point_y = (VIEW_SIZE_Y / 2.0) - cvmGet(intrinsicMatrix[0], 1, 2);    //178.543373;    //intrinsicMatrix->GetElement(1, 2);
    
    focal_length = FOCAL_LENGTH;
        
    capture[0] = NULL;
    capture[1] = NULL;
    
    
    // Timer Handling
    
//    this->DataCallbackCommand = vtkCallbackCommand::New();
//    this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
//    this->DataCallbackCommand->SetCallback(vtkvideoOverlayLogic::DataCallback);
    
    this->volumenode = NULL;    //adding at 09. 8. 19 - smkim    
    this->volumeProperty = NULL;   //adding at 09. 8. 28 - smkim
    
    this->rotationAngleX = 0.0;    // adding at 09. 9. 8 - smkim
    this->rotationAngleY = 0.0;    // adding at 09. 9. 8 - smkim
    this->rotationAngleZ = 0.0;    // adding at 09. 9. 8 - smkim
    
    this->translationX = 0.0;    // adding at 09. 9. 14 - smkim
    this->translationY = 0.0;     // adding at 09. 9. 14 - smkim
    this->translationZ = 0.0;    // adding at 09. 9. 14 - smkim
    
    this->TransformNodeName = NULL;    // adding at 09. 9. 16 - smkim
    this->OriginalTrackerNode = NULL;    // adding at 09. 9. 16 - smkim
    
    this->TransformEditorWidget = NULL;    // adding at 09. 12. 24 - smkim
    
    this->m_bVolumeRendering = false;    // initializing volume rendering flag (false: before volume rendering)     // adding at 10. 02. 01 - smkim
    this->m_bSurfaceRendering = false;    // initializing surface rendering flag (false: before surface rendering)     // adding at 10. 02. 01 - smkim
    
    this->m_bDriveSource = false;        // initializing drive souce flag (false: manual)    // adding at 09. 9. 17 - smkim
    this->m_bOpenSecondaryWindow = false;    // initializing secondary window flag     // adding at 10. 01. 19 - smkim
    
    this->transformMatrix = vtkMatrix4x4::New();        // adding at 09. 9. 16 - smkim
    this->volumeTransformMatrix = vtkMatrix4x4::New();    // adding at 09. 10. 26 - smkim
    
    // 5/16/2010 ayamada
    this->closeWindowFlag = 1;
    
    // 5/17/2010 ayamada
    this->snapShotNumber = 0;
    this->snapShotShutter = 0;
    
    // 6/13/2010 ayamada
    this->undistortionFlag = 0;
    
    // 6/22/2010 ayamada
    // secondary window mode
    this->singleWindowCheckButton = NULL;
    this->stereoWindowCheckButton = NULL;
    
    // 6/23/2010 ayamada
    this->singleOn = 0;
    this->stereoOn = 0;
    this->firstOn = 0;

    
    //----
}

//---------------------------------------------------------------------------
vtkSecondaryWindowWithOpenCVGUI::~vtkSecondaryWindowWithOpenCVGUI ( )
{
    
    // 5/16/2010 ayamada
    this->closeWindowFlag = 2;
    
    // 5/16/2010 ayamada
    while(this->closeWindowFlag==3){};

//    cvReleaseCapture(&capture);   

/*        // 5/15/2010 ayamada
    polyActor->Delete();
    polyActor = NULL;
    polyActor1->Delete();
    polyActor1 = NULL;
    
    polyMapper->Delete();
    polyMapper = NULL;
    polyMapper1->Delete();
    polyMapper1 = NULL;
    
    polyNormal->Delete();
    polyNormal = NULL;
    polyNormal1->Delete();
    polyNormal1 = NULL;
    
    polyReader->Delete();
    polyReader = NULL;
    polyReader1->Delete();
    polyReader1 = NULL;
    
    
    Actor->Delete();
    Actor=NULL;
    planeMapper->Delete();
    planeMapper = NULL;
    planeSource->Delete();
    planeSource = NULL;
    atext[i]->Delete(); 
    atext[i]=NULL;
    importer[i]->Delete();
    importer[i]=NULL;
*/        
 
    // 5/15/2010 ayamada
    // 5/15/2010 ayamada for release image
    if (this->SecondaryViewerWindow)
    {
        this->SecondaryViewerWindow->Withdraw();
        this->SecondaryViewerWindow->SetApplication(NULL);
        this->SecondaryViewerWindow->Delete();
        this->SecondaryViewerWindow = NULL;
    }

    // 6/23/2010 ayamada for release image
    if (this->SecondaryViewerWindow2)
    {
        this->SecondaryViewerWindow2->Withdraw();
        this->SecondaryViewerWindow2->SetApplication(NULL);
        this->SecondaryViewerWindow2->Delete();
        this->SecondaryViewerWindow2 = NULL;
    }
    
    
    // 5/15/2010 ayamada
    //idata[i]->Delete();
    //idata[i] = NULL;    
    
    FocalPlaneMapper->Delete();
    FocalPlaneMapper = NULL;
    FocalPlaneSource->Delete();
    FocalPlaneSource = NULL;

    for(int i=0; i<2; i++){
    actor[i]->Delete();
    actor[i]=NULL;    
    atext[i]->Delete(); 
    atext[i]=NULL;
    }
    
//    importer[i]->Delete();
//    importer[i]=NULL;
        
    if (this->Thread)
    {
        this->Thread->Delete();
    }
    
    
    if (this->Mutex)
    {
        this->Mutex->Delete();
    }


    // 5/16/2010 ayamada    
    textActor->Delete();
    textActor1->Delete();
    textActor2->Delete();
    textActor3->Delete();
    textActorCamera->Delete();
    textActorCamera1->Delete();

    // 5/17/2010 ayamada
    textActorSavePath->Delete();
    textActorSavePathH->Delete();
    
    
  //----------------------------------------------------------------
  // Remove Callbacks

  // 5/11/2010 ayamada  
    
  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  //----------------------------------------------------------------
  // Remove Observers

  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // Remove GUI widgets

        
  // 5/7/2010 ayamada
  
  if (this->ShowSecondaryWindowWithOpenCVButton)
    {
    this->ShowSecondaryWindowWithOpenCVButton->SetParent(NULL);
    this->ShowSecondaryWindowWithOpenCVButton->Delete();
    }

  if (this->HideSecondaryWindowWithOpenCVButton)
    {
    this->HideSecondaryWindowWithOpenCVButton->SetParent(NULL);
    this->HideSecondaryWindowWithOpenCVButton->Delete();
    }
 
    
  /*    // 5/11/2010 ayamada
    
    // 5/6/2010 ayamada for videoOverlay
    if (this->NS_ImageData)
    {
        this->NS_ImageData->SetParent(NULL);
        this->NS_ImageData->Delete();
        this->NS_ImageData=NULL;
    }    //adding at 09. 8. 19 - smkim
    
    if (this->VolumeHeaderWidget)
    {
        this->VolumeHeaderWidget->SetParent(NULL );
        this->VolumeHeaderWidget->Delete ( );
    }   //adding at 09. 9. 2 - smkim
    
    if(this->DetailsFrame)
    {
        this->DetailsFrame->Delete();
        this->DetailsFrame=NULL;
    }    //adding at 09. 9. 4 - smkim
    
    if (this->RotationAngleX)
    {
        this->RotationAngleX->SetParent(NULL);
        this->RotationAngleX->Delete();
        this->RotationAngleX = NULL;
    }    //adding at 09. 9. 8 - smkim
    
    if (this->RotationAngleY)
    {
        this->RotationAngleY->SetParent(NULL);
        this->RotationAngleY->Delete();
        this->RotationAngleY = NULL;
    }    //adding at 09. 9. 8 - smkim
    
    if (this->RotationAngleZ)
    {
        this->RotationAngleZ->SetParent(NULL);
        this->RotationAngleZ->Delete();
        this->RotationAngleZ = NULL;
    }    //adding at 09. 9. 8 - smkim
    
    if (this->TranslationX)
    {
        this->TranslationX->SetParent(NULL);
        this->TranslationX->Delete();
        this->TranslationX = NULL;
    }    //adding at 09. 9. 14 - smkim
    
    if (this->TranslationY)
    {
        this->TranslationY->SetParent(NULL);
        this->TranslationY->Delete();
        this->TranslationY = NULL;
    }    //adding at 09. 9. 14 - smkim
    
    if (this->TranslationZ)
    {
        this->TranslationZ->SetParent(NULL);
        this->TranslationZ->Delete();
        this->TranslationZ = NULL;
    }    //adding at 09. 9. 14 - smkim
    
    if (this->TransformNodeNameEntry)
    {
        this->TransformNodeNameEntry->SetParent(NULL);
        this->TransformNodeNameEntry->Delete();
    }    // adding at 09. 9. 16 - smkim
    
    if (this->selectDriveSource)
    {
        this->selectDriveSource->SetParent(NULL);
        this->selectDriveSource->Delete();
    }    // adding at 09. 9. 17 - smkim
    
    if (this->showCaptureData)
    {
        this->showCaptureData->SetParent(NULL);
        this->showCaptureData->Delete();
    }
    
    if (this->closeCaptureData)
    {
        this->closeCaptureData->SetParent(NULL);
        this->closeCaptureData->Delete();
    }
    
    if (this->loadLensDistortionCoefButton)
    {
        this->loadLensDistortionCoefButton->SetParent(NULL);
        this->loadLensDistortionCoefButton->Delete();
        this->loadLensDistortionCoefButton = NULL;
    }    // adding at 10. 2. 23 - smkim
    
    if (this->loadIntrinsicParameterButton)
    {
        this->loadIntrinsicParameterButton->SetParent(NULL);
        this->loadIntrinsicParameterButton->Delete();
        this->loadIntrinsicParameterButton = NULL;
    }    // adding at 10. 2. 23 - smkim

   */
   
    
    
//----10.01.12-komura

        
/*  // 5/15/2010 ayamada
 
    // 5/7/2010 ayamada
    // for the function "CameraFocusPlane"
    FocalPlaneSource->Delete();
    FocalPlaneSource = NULL;
    FocalPlaneMapper->Delete();
    FocalPlaneMapper = NULL;
    ExtrinsicMatrix->Delete();
    ExtrinsicMatrix = NULL;
    
    // 10.01.25 ayamada
    planeSourceLeftPane->Delete();
    planeSourceLeftPane = NULL;
    planeMapperLeftPane->Delete();
    planeMapperLeftPane = NULL;
    ActorLeftPane->Delete();
    ActorLeftPane=NULL;
*/
    // 5/5/2010 ayamada
    // for videoOverlay

/*        // 5/15/2010 ayamada
    if (this->volumenode)
    {
        //this->volumenode->SetParent(NULL);
        this->volumenode->Delete();
        this->volumenode = NULL;
    }    //adding at 09. 8. 19 - smkim
    
    if (this->volumeProperty)
    {
        //this->volumeProperty->SetParent(NULL);
        this->volumeProperty->Delete();
        this->volumeProperty = NULL;
    }    //adding at 09. 8. 19 - smkim
    
    if (this->TransformEditorWidget)
    {
        this->TransformEditorWidget->RemoveWidgetObservers ( );
        this->TransformEditorWidget->SetParent (NULL);
        this->TransformEditorWidget->Delete ( );
    }    //adding at 09. 12. 2    4 - smkim
    
    // adding at 09. 9. 16 - smkim
    transformMatrix->Delete();
    transformMatrix = NULL;
    
    // adding at 09. 10. 26 - smkim
    volumeTransformMatrix->Delete();
    volumeTransformMatrix = NULL;
*/

    
    // second window checkbox
    // 6/22/2010 ayamada
    if (this->singleWindowCheckButton)
    {
        this->singleWindowCheckButton->SetParent(NULL );
        this->singleWindowCheckButton->Delete ( );
    }
    if (this->stereoWindowCheckButton)
    {
        this->stereoWindowCheckButton->SetParent(NULL );
        this->stereoWindowCheckButton->Delete ( );
    }
    
    
    
    //----------------------------------------------------------------
    // Unregister Logic class
    
    this->SetModuleLogic ( NULL );
    


    
    //----
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::Init()
{
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::Enter()
{
  // Fill in
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
//    this->TimerInterval = 100;  // 100 ms
    this->TimerInterval = 10;  // 10 ms 4/25/2010 ayamada
    ProcessTimerEvents();
    }

}


//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::Exit ( )
{
  // Fill in
}

//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::TearDownGUI() 
{
  /*
  this->PrimaryMonitorRobotViewerWidget->SetApplication(NULL);
  this->PrimaryMonitorRobotViewerWidget->SetMainViewerWidget(NULL);
  this->PrimaryMonitorRobotViewerWidget->SetMRMLScene(NULL);
  */
}



//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "SecondaryWindowWithOpenCVGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  // 5/7/2010 ayamada
  /*
  if (this->ShowSecondaryWindowWithOpenCVButton)
    {
    this->ShowSecondaryWindowWithOpenCVButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->HideSecondaryWindowWithOpenCVButton)
    {
    this->HideSecondaryWindowWithOpenCVButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
   */

    // 5/6/2010 ayamada
    if (this->NS_ImageData)
    {
        this->NS_ImageData->RemoveObservers(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);
    }    //adding at 09. 8. 19 - smkim
    
    if (this->selectDriveSource)
    {
        this->selectDriveSource->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }    // adding at 09. 9. 17 - smkim
    
    if (this->showCaptureData)
    {
        this->showCaptureData->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
    
    if (this->closeCaptureData)
    {
        this->closeCaptureData->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
   
    // 5/17/2010 ayamada
    if (this->captureCameraImage)
    {
        this->captureCameraImage->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
    
    
    // load lens distortion coefficient
    // adding at 10. 2. 23 - smkim
    if (this->loadLensDistortionCoefButton)
    {
        this->loadLensDistortionCoefButton
        ->GetWidget()->GetLoadSaveDialog()->RemoveObservers ( vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
    
    // load intrinsic parameters of camera
    // adding at 10. 2. 23 - smkim
    if (this->loadIntrinsicParameterButton)
    {
        this->loadIntrinsicParameterButton
        ->GetWidget()->GetLoadSaveDialog()->RemoveObservers ( vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }

    // 5/17/2010 ayamada
/*
    if (this->saveCameraImageEntry)
    {
        this->saveCameraImageEntry
        ->GetWidget()->GetLoadSaveDialog()->RemoveObservers ( vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
*/    
  
    // adding removeobserver for RotationAngleX, scale with label 
    // adding at 09. 9. 8 - smkim
    this->RotationAngleX->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RotationAngleX->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RotationAngleX->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    
    // adding removeobserver for RotationAngleY, scale with label 
    // adding at 09. 9. 8 - smkim
    this->RotationAngleY->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RotationAngleY->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RotationAngleY->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    
    // adding removeobserver for RotationAngleZ, scale with label 
    // adding at 09. 9. 8 - smkim
    this->RotationAngleZ->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RotationAngleZ->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RotationAngleZ->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    
    // adding removeobserver for RotationAngleZ, scale with label 
    // adding at 09. 9. 14 - smkim
    this->TranslationX->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->TranslationX->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->TranslationX->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    
    // adding removeobserver for RotationAngleZ, scale with label 
    // adding at 09. 9. 14 - smkim
    this->TranslationY->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->TranslationY->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->TranslationY->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    
    // adding removeobserver for RotationAngleZ, scale with label 
    // adding at 09. 9. 14 - smkim
    this->TranslationZ->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->TranslationZ->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->TranslationZ->GetWidget()->RemoveObservers(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    
    // secondary Window Mode Checkbox
    // 6/22/2010 ayamada
    if (this->singleWindowCheckButton)
    {
        this->singleWindowCheckButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
    if (this->stereoWindowCheckButton)
    {
        this->stereoWindowCheckButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
    
    this->RemoveLogicObservers();
    
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::AddGUIObservers ( )
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

  // 5/7/2010 ayamada
  /*  
  this->ShowSecondaryWindowWithOpenCVButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->HideSecondaryWindowWithOpenCVButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  */
   
    // 5/6/2010 ayamada
    
    // adding addobserver for RotationAngleX, scale with label 
    // adding at 09. 9. 8 - smkim
    this->RotationAngleX->GetWidget()->AddObserver(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RotationAngleX->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RotationAngleX->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    
    // adding addobserver for RotationAngleY, scale with label 
    // adding at 09. 9. 8 - smkim
    this->RotationAngleY->GetWidget()->AddObserver(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RotationAngleY->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RotationAngleY->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    
    // adding addobserver for RotationAngleZ, scale with label 
    // adding at 09. 9. 8 - smkim
    this->RotationAngleZ->GetWidget()->AddObserver(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RotationAngleZ->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RotationAngleZ->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    
    // adding removeobserver for RotationAngleZ, scale with label 
    // adding at 09. 9. 14 - smkim
    this->TranslationX->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->TranslationX->GetWidget()->AddObserver(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->TranslationX->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    
    // adding removeobserver for RotationAngleZ, scale with label 
    // adding at 09. 9. 14 - smkim
    this->TranslationY->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->TranslationY->GetWidget()->AddObserver(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->TranslationY->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    
    // adding removeobserver for RotationAngleZ, scale with label 
    // adding at 09. 9. 14 - smkim
    this->TranslationZ->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->TranslationZ->GetWidget()->AddObserver(vtkKWScale::ScaleValueStartChangingEvent, (vtkCommand *)this->GUICallbackCommand );
    this->TranslationZ->GetWidget()->AddObserver(vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    
    
    //Selecting Volume Node
    this->NS_ImageData->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );     // adding at 09. 8. 19 - smkim
    
    this->selectDriveSource->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);    // adding at 09. 9. 17 - smkim
    
    this->showCaptureData->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->closeCaptureData->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

    // 5/17/2010 ayamada
    this->captureCameraImage->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    
    // load lens distortion coefficient
    // adding at 10. 2. 23 - smkim
    this->loadLensDistortionCoefButton
    ->GetWidget()->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand);
    
    // load intrinsic parameters of camera
    // adding at 10. 2. 23 - smkim
    this->loadIntrinsicParameterButton
    ->GetWidget()->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand);

/*    
    // 5/17/2010 ayamada
    this->saveCameraImageEntry
    ->GetWidget()->GetLoadSaveDialog()->AddObserver(vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand);
 */   
    //this->saveCameraImageEntry->AddObserver(vtkKWEntryWithLabel::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);

    
    // secondary Window Mode CheckBox
    // 6/22/2010 ayamada
    this->singleWindowCheckButton
    ->AddObserver ( vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->stereoWindowCheckButton
    ->AddObserver ( vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}




//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkSecondaryWindowWithOpenCVLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

    
// 5/7/2010 ayamada    
/*  
  if (this->ShowSecondaryWindowWithOpenCVButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    if (this->SecondaryViewerWindow)
      {  
      //this->makeCameraThread();//10.01.12-komura //10.01.21-komura
      this->SecondaryViewerWindow->DisplayOnSecondaryMonitor();
          
          secView=1;    // 5/5/2010 ayamada
          
          this->m_bOpenSecondaryWindow = true; // 5/6/2010 ayamada
                    
      }
    }
  else if (this->HideSecondaryWindowWithOpenCVButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    if (this->SecondaryViewerWindow)
      {  
      //this->SecondaryViewerWindow->Withdraw();
          
          // read MRML; 4/25/2010 ayamada
          vtkMRMLScene* scene = this->GetMRMLScene();
          if (scene)
          {
              // test network connection check (baloon sensor)
              vtkMRMLNode* node = scene->GetNodeByID("vtkMRMLLinearTransformNode4");
              vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(node);
              
              // test baloon sensor connection check (suction tube)
              vtkMRMLNode* node2 = scene->GetNodeByID("vtkMRMLLinearTransformNode5");
              vtkMRMLLinearTransformNode* tnode2 = vtkMRMLLinearTransformNode::SafeDownCast(node2);

              vtkMRMLNode *nnode = NULL; // TODO: is this OK?
              vtkIntArray* nodeEvents = vtkIntArray::New();
              nodeEvents->InsertNextValue(vtkMRMLLinearTransformNode::TransformModifiedEvent);
              vtkSetAndObserveMRMLNodeEventsMacro(nnode,tnode,nodeEvents);
              
              vtkMRMLNode *nnode2 = NULL; // TODO: is this OK?
              vtkIntArray* nodeEvents2 = vtkIntArray::New();
              nodeEvents2->InsertNextValue(vtkMRMLLinearTransformNode::TransformModifiedEvent);
              vtkSetAndObserveMRMLNodeEventsMacro(nnode2,tnode2,nodeEvents2);
              
              nodeEvents->Delete();
              nodeEvents2->Delete();              
              
          }
          
      }
    }
*/
    
    // 5/6/2010 ayamada for videoOverlay
    if (this->showCaptureData == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        std::cerr << "showCaptureData button is pressed." << std::endl;
        
        // 5/6/2010 ayamada        
        // for display capture data
        //if (this->SecondaryViewerWindow)
        if (this->singleWindowCheckButton->GetSelectedState() == 1)
        {  
            //this->makeCameraThread();//10.01.12-komura //10.01.21-komura
            //this->SecondaryViewerWindow2->DisplayOnSecondaryMonitor();
            this->SecondaryViewerWindow->DisplayOnSecondaryMonitor();
            //this->SecondaryViewerWindow2->DisplayOnSecondaryMonitor();
            
            this->ConfigurationOfSecondaryWindow(1); // 6/23/2010 ayamada            
            
            // 6/23/2010 ayamada
            if(this->SecondaryViewerWindow2){
                this->SecondaryViewerWindow2->Withdraw();
            }
            
            secView=1;    // 5/5/2010 ayamada            
            this->m_bOpenSecondaryWindow = true;  // 5/7/2010 ayamada
        
            this->updateViewTriger = 1;
            
            // 6/23/2010 ayamada
            this->singleOn = 1;
            this->stereoOn = 0;
            
            this->firstOn = 1;
      
            /*
            // change decondary monitor size
            // 100622-komura
            if(singleWindowCheckButton->GetSelectedState() != 1){
                this->SecondaryViewerWindow->changeSecondaryMonitorSize(640, 480);
            }
            else{
                this->SecondaryViewerWindow->changeSecondaryMonitorSize(1280, 480);
            }            
            */

        }
        
        if(this->stereoWindowCheckButton->GetSelectedState() == 1)
        {
            this->SecondaryViewerWindow->DisplayOnSecondaryMonitor();
            this->SecondaryViewerWindow2->DisplayOnSecondaryMonitor();
            
            this->ConfigurationOfSecondaryWindow(2); // 6/23/2010 ayamada            

            secView=1;    // 5/5/2010 ayamada            
            this->m_bOpenSecondaryWindow = true;  // 5/7/2010 ayamada
            
            this->updateViewTriger = 1;
            
            // 6/23/2010 ayamada
            this->stereoOn = 1;
            this->singleOn = 0;

            this->firstOn = 1;
            
        }
        
/*
        if(this->SecondaryViewerWindow2){
            this->SecondaryViewerWindow2->DisplayOnSecondaryMonitor();
        }
*/
        
/*        
        // 6/23/2010 ayamada
        if(this->stereoWindowCheckButton->GetSelectedState() == 1){
        //    if(this->SecondaryViewerWindow2){
                this->SecondaryViewerWindow2->DisplayOnSecondaryMonitor();
        //    }
        }           
*/        
        
    }
    
    else if (this->closeCaptureData == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        std::cerr << "The secondary window off button is pressed." << std::endl;
        
        // 5/16/2010 ayamada
        this->closeWindowFlag=2;

        // 6/23/2010 ayamada
        this->singleOn = 0;
        this->stereoOn = 0;
        this->firstOn = 0;
        
        std::cerr << "closeWindowFlag=" << this->closeWindowFlag << std::endl;

        // 5/16/2010 ayamada
        if (this->SecondaryViewerWindow)
        {  
            this->SecondaryViewerWindow->Withdraw();
            // 5/16/2010
            first = 0;

        }
        if (this->SecondaryViewerWindow2)
        {  
            this->SecondaryViewerWindow2->Withdraw();
            
        }
        
        
    }

    // 5/17/2010 ayamada
    else if (this->captureCameraImage == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        std::cerr << "The snapshot button is pressed." << std::endl;                
                
        // 5/17/2010 ayamada
        
        if(this->saveCameraImageEntry->GetWidget()->GetValue() != NULL){
            this->snapShotShutter=1;
            this->snapShotNumber++;
            sprintf(this->snapShotSavePath,"%s%d.bmp"
                    ,this->saveCameraImageEntry->GetWidget()->GetValue(),this->snapShotNumber);
            textActorSavePath->SetInput(this->snapShotSavePath);            
        }
        
    }

    // 5/17/2010 ayamada    
    // save camera images
/*
    else if (this->saveCameraImageEntry->GetWidget()->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && event == vtkKWTopLevel::WithdrawEvent )
    {

        // 5/17/2010 ayamada

        const char * filepath = this->loadLensDistortionCoefButton->GetWidget()->GetFileName();
        
        if(filepath)
        {
            ifstream ldstream;            //file input stream        
            ldstream.open(filepath);            //open input file
            
            double m_dLensDistortionCoef[4];
            
            for(int i = 0; i < 4; i++)
            {
                ldstream >> m_dLensDistortionCoef[i];
                cvmSet( this->distortionCoefficient, i, 0, m_dLensDistortionCoef[i] );
            }
            
            ldstream.close();
        }
        
        
    }    
  */ 
    
    // to select drive source ( manual / data of optical tracking system )
    // adding at 09. 9. 17 - smkim
    else if (this->selectDriveSource == vtkKWPushButton::SafeDownCast(caller) && event == vtkKWPushButton::InvokedEvent)
    {
        
        if(this->m_bOpenSecondaryWindow == false)
        {
            std::cerr << "The secondary window is not open." << std::endl;
            return;
        }
        
        std::cerr << "selectDriveSource button is pressed." << std::endl;
        
        if (this->SecondaryViewerWindow)
        {  
            // 5/7/2010 ayamada
            
            // read MRML; 4/25/2010 ayamada
            vtkMRMLScene* scene = this->GetMRMLScene();
            if (scene)
            {
                // test network connection check (baloon sensor)
                vtkMRMLNode* node = scene->GetNodeByID("vtkMRMLLinearTransformNode4");
                vtkMRMLLinearTransformNode* tnode = vtkMRMLLinearTransformNode::SafeDownCast(node);
                
                // test baloon sensor connection check (suction tube)
                vtkMRMLNode* node2 = scene->GetNodeByID("vtkMRMLLinearTransformNode5");
                vtkMRMLLinearTransformNode* tnode2 = vtkMRMLLinearTransformNode::SafeDownCast(node2);
                
                vtkMRMLNode *nnode = NULL; // TODO: is this OK?
                vtkIntArray* nodeEvents = vtkIntArray::New();
                nodeEvents->InsertNextValue(vtkMRMLLinearTransformNode::TransformModifiedEvent);
                vtkSetAndObserveMRMLNodeEventsMacro(nnode,tnode,nodeEvents);
                
                vtkMRMLNode *nnode2 = NULL; // TODO: is this OK?
                vtkIntArray* nodeEvents2 = vtkIntArray::New();
                nodeEvents2->InsertNextValue(vtkMRMLLinearTransformNode::TransformModifiedEvent);
                vtkSetAndObserveMRMLNodeEventsMacro(nnode2,tnode2,nodeEvents2);
                
                nodeEvents->Delete();
                nodeEvents2->Delete();              
                
            }
            
        }
        
        if ( this->m_bDriveSource == false )
            this->m_bDriveSource = true;
        else
            this->m_bDriveSource = false;
        
    }
    
    
    // to get VolumeNode with VolumeSelectorWidget        at 09. 8. 19 - smkim
    else if (this->NS_ImageData == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent ) 
    {
        
        
        if(this->m_bOpenSecondaryWindow == false)
        {
            std::cerr << "The secondary window is not open." << std::endl;
            return;
        }
        
        //getting VolumeNode from MRML of Slicer3 with VolumeSelectorWidget
        vtkMRMLVolumeNode *volume = vtkMRMLVolumeNode::SafeDownCast(this->NS_ImageData->GetSelected());
        
        if (volume != NULL)
        {
            // Deactivate GradientsEditor, as it should only enabled when activenode is a DWI
            this->volumenode = volume;    // syncronizing with the volume node instance of Logic class   // adding at 09. 8. 19 - smkim
            
            //Add observer to trigger update of transform
            this->volumenode->AddObserver(vtkMRMLTransformableNode::TransformModifiedEvent,(vtkCommand *) this->MRMLCallbackCommand);
            //NH
            this->volumenode->AddObserver(vtkMRMLScalarVolumeNode::ImageDataModifiedEvent, (vtkCommand *)this->MRMLCallbackCommand );
            
            this->ApplicationLogic->GetSelectionNode()->SetActiveLabelVolumeID( volume->GetID() );
            
            //------------------------------------------------------------------
            //  Invoking Volume Rendering function
            //------------------------------------------------------------------
            
            //this->vtkRayCastingVolumeRender();
            // 5/8/2010 ayamada
            //this->vtkTexture3DVolumeRender();
            //this->vtkCUDAVolumeRender();
            
            // 5/8/2010 ayamada
            this->vtkSurfaceModelRender();
            this->UpdateFramesFromMRML();
            
        }
        
    }
    
    // --------------------------
    // make secondary window type
    // --------------------------
    
    else if (this->stereoWindowCheckButton == vtkKWCheckButton::SafeDownCast(caller) && 
                 event == vtkKWCheckButton::SelectedStateChangedEvent )
    {
        
        if(this->stereoWindowCheckButton->GetSelectedState() == 1)
        {
            this->singleWindowCheckButton->SelectedStateOff();
            
        }else if(this->stereoWindowCheckButton->GetSelectedState() == 0)
        {
            this->singleWindowCheckButton->SelectedStateOn();
            this->stereoWindowCheckButton->SelectedStateOff();
        }

        if(singleOn == 1 && firstOn == 1){
            this->SecondaryViewerWindow2->DisplayOnSecondaryMonitor(); // 6/23/2010 ayamada            
        }
        
        this->ConfigurationOfSecondaryWindow(2); // 6/23/2010 ayamada
        
        // 6/23/2010 ayamada
//        if( this->singleOn == 0 && this->stereoOn == 0 ){ 
            this->singleOn = 0;
            this->stereoOn = 1;
//        }
        
    }

    else if (this->singleWindowCheckButton == vtkKWCheckButton::SafeDownCast(caller) && 
             event == vtkKWCheckButton::SelectedStateChangedEvent )
    {
        
        if(this->singleWindowCheckButton->GetSelectedState() == 1)
        {
            this->stereoWindowCheckButton->SelectedStateOff();
            
        }else if(this->singleWindowCheckButton->GetSelectedState() == 0)
        {
            this->stereoWindowCheckButton->SelectedStateOn();
            this->singleWindowCheckButton->SelectedStateOff();
        }

        if(stereoOn == 1 && firstOn == 1){
            this->SecondaryViewerWindow2->Withdraw(); // 6/23/2010 ayamada                        
        }
        
        this->ConfigurationOfSecondaryWindow(1); // 6/23/2010 ayamada

        // 6/23/2010 ayamada
//        if( this->singleOn == 0 && this->stereoOn == 0 ){ 
            this->singleOn = 1;
            this->stereoOn = 0;
//        }
        
        
    }
    
    
    
    // to get the rotation angle from scale widget and to apply the value to the rendering volume
    // adding at 09. 9. 8 - smkim
    else if ( (vtkKWScale::SafeDownCast(caller) == this->RotationAngleX->GetWidget() 
               || vtkKWScale::SafeDownCast(caller) == this->RotationAngleY->GetWidget() 
               || vtkKWScale::SafeDownCast(caller) == this->RotationAngleZ->GetWidget() )
             && static_cast<int>(event) == vtkKWScale::ScaleValueChangingEvent )
    {
        this->rotationAngleX = this->RotationAngleX->GetWidget()->GetValue();
        this->rotationAngleY = this->RotationAngleY->GetWidget()->GetValue();
        this->rotationAngleZ = this->RotationAngleZ->GetWidget()->GetValue();
        
        // for applying the value of scale control to the volume rendering at 09. 10. 30 - smkim
        //this->volume->SetOrientation(this->rotationAngleX, this->rotationAngleY, this->rotationAngleZ);

        // 5/30/2010 ayamada
        this->volume->SetOrientation(this->rotationAngleX+180.0, -this->rotationAngleY-180.0, this->rotationAngleZ);
        
        
        // for applying the value of scale control to the surface rendering at 10. 02. 01 - smkim
        this->polyActor->SetOrientation(this->rotationAngleX, this->rotationAngleY, this->rotationAngleZ);
        
        this->importer[0]->Update();
        
    }
    
    // to get the translation distances from scale widget and to apply the value to the rendering volume
    // adding at 09. 9. 14 - smkim
    else if ( (vtkKWScale::SafeDownCast(caller) == this->TranslationX->GetWidget() 
               || vtkKWScale::SafeDownCast(caller) == this->TranslationY->GetWidget() 
               || vtkKWScale::SafeDownCast(caller) == this->TranslationZ->GetWidget() )
             && static_cast<int>(event) == vtkKWScale::ScaleValueChangingEvent )
    {
        this->translationX = this->TranslationX->GetWidget()->GetValue();
        this->translationY = this->TranslationY->GetWidget()->GetValue();
        this->translationZ = this->TranslationZ->GetWidget()->GetValue();
        
        // for applying the value of scale control to the volume rendering at 09. 10. 30 - smkim
        this->volume->SetPosition(this->translationX, this->translationY, this->translationZ);
        
        // for applying the value of scale control to the surface rendering at 10. 02. 01 - smkim
        this->polyActor->SetPosition(this->translationX, this->translationY, this->translationZ);
        
        this->importer[0]->Update();
        
    }
    
    // load lens distortion coefficient
    // adding at 10. 2. 23 - smkim
    else if (this->loadLensDistortionCoefButton->GetWidget()->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && event == vtkKWTopLevel::WithdrawEvent )
    {
        const char * filepath = this->loadLensDistortionCoefButton->GetWidget()->GetFileName();
        
        if(filepath)
        {

        // 6/10/2010 ayamada
            this->distortionCoefficient[0] = (CvMat*) cvLoad(filepath);    
            this->distortionCoefficient[1] = (CvMat*) cvLoad(filepath);    
            
        /*    
            ifstream ldstream;            //file input stream        
            ldstream.open(filepath);            //open input file
            
            double m_dLensDistortionCoef[4];
            
            for(int i = 0; i < 4; i++)
            {
                ldstream >> m_dLensDistortionCoef[i];
                cvmSet( this->distortionCoefficient, i, 0, m_dLensDistortionCoef[i] );
            }
            
            ldstream.close();
         */
            
        // 6/13/2010 ayamada
        this->undistortionFlag = 1;
            
            
        }
        
        
    }
    
    // load intrinsic parameters of camera
    // adding at 10. 2. 23 - smkim
    else if (this->loadIntrinsicParameterButton->GetWidget()->GetLoadSaveDialog() == vtkKWLoadSaveDialog::SafeDownCast(caller) && event == vtkKWTopLevel::WithdrawEvent )
    {
        const char * filepath = this->loadIntrinsicParameterButton->GetWidget()->GetFileName();
        
        if(filepath)
        {
            // 6/10/2010 ayamada
            /*
            ifstream ipstream;            //file input stream        
            ipstream.open(filepath);            //open input file
            double m_dIntrinsicParameter[3][3];
            
            
            for(int i = 0; i < 3; i++)
            {
                ipstream >> m_dIntrinsicParameter[i][0] >> m_dIntrinsicParameter[i][1] >> m_dIntrinsicParameter[i][2] ;
            }
            
            ipstream.close();
            
            for(int i = 0; i < 3; i++)
                for(int j = 0; j < 3; j++)
                {
                    cvmSet( this->intrinsicMatrix, i, j, m_dIntrinsicParameter[i][j] );
                }
            */
            /*
            for(int i = 0; i < 3; i++)
            {
                for(int j = 0; j < 3; j++)
                {
                    cvmSet( this->intrinsicMatrix, i, j, this->intrinsicMatrixTmp[i][j] );
                }
            }
            */
            
            // 6/10/2010 ayamada
            for(int k = 0; k<2; k++)
            {
            this->intrinsicMatrix[k] = (CvMat*) cvLoad(filepath);                
                        
            // calculating focal point with intrinsic matrix
            // adding at 10. 02. 22 - smkim
            this->focal_point_x = (VIEW_SIZE_X / 2.0) - cvmGet(this->intrinsicMatrix[k], 0, 2);
            this->focal_point_y = (VIEW_SIZE_Y / 2.0) - cvmGet(this->intrinsicMatrix[k], 1, 2);
            
            //this->focal_length = (m_dIntrinsicParameter[0][0] + m_dIntrinsicParameter[1][1]) / 2.0;
            // 6/10/2010 ayamada
            this->focal_length = (cvmGet(this->intrinsicMatrix[k], 0, 0) + cvmGet(this->intrinsicMatrix[k], 1, 1)) / 2.0;
            
            // 6/13/2010 ayamada
            this->undistortionFlag = 1;
            }
        }
        
    }
    
    
    
    
} 


//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::DataCallback(vtkObject *caller, 
                                                   unsigned long eid, void *clientData, void *callData)
{
  vtkSecondaryWindowWithOpenCVGUI *self = reinterpret_cast<vtkSecondaryWindowWithOpenCVGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkSecondaryWindowWithOpenCVGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkSecondaryWindowWithOpenCVLogic::SafeDownCast(caller))
    {
    if (event == vtkSecondaryWindowWithOpenCVLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
  // Fill in
    // 4/23/2010 ayamada
    if (event == vtkMRMLLinearTransformNode::TransformModifiedEvent)
    {
        // 5/7/2010 ayamada
        //std::cerr << "TransformModifiedEvent is invoked." << std::endl;
        
        
        vtkMRMLLinearTransformNode* node = vtkMRMLLinearTransformNode::SafeDownCast(caller);
        vtkMatrix4x4* transformToParent = node->GetMatrixTransformToParent();
        
        vtkMRMLLinearTransformNode* node2 = vtkMRMLLinearTransformNode::SafeDownCast(caller);
        vtkMatrix4x4* transformToParent2 = node2->GetMatrixTransformToParent();
        
        tx = transformToParent->GetElement(0, 0);
        ty = transformToParent->GetElement(1, 0);
        tz = transformToParent->GetElement(2, 0);
        t0 = transformToParent->GetElement(3, 0);
        sx = transformToParent->GetElement(0, 1);
        sy = transformToParent->GetElement(1, 1);
        sz = transformToParent->GetElement(2, 1);
        s0 = transformToParent->GetElement(3, 1);
        nx = transformToParent->GetElement(0, 2);
        ny = transformToParent->GetElement(1, 2);
        nz = transformToParent->GetElement(2, 2);
        n0 = transformToParent->GetElement(3, 2);
        px = transformToParent->GetElement(0, 3);
        py = transformToParent->GetElement(1, 3);
        pz = transformToParent->GetElement(2, 3);
        p0 = transformToParent->GetElement(3, 3);
        
        tx2 = transformToParent2->GetElement(0, 0);
        ty2 = transformToParent2->GetElement(1, 0);
        tz2 = transformToParent2->GetElement(2, 0);
        t02 = transformToParent2->GetElement(3, 0);
        sx2 = transformToParent2->GetElement(0, 1);
        sy2 = transformToParent2->GetElement(1, 1);
        sz2 = transformToParent2->GetElement(2, 1);
        s02 = transformToParent2->GetElement(3, 1);
        nx2 = transformToParent2->GetElement(0, 2);
        ny2 = transformToParent2->GetElement(1, 2);
        nz2 = transformToParent2->GetElement(2, 2);
        n02 = transformToParent2->GetElement(3, 2);
        px2 = transformToParent2->GetElement(0, 3);
        py2 = transformToParent2->GetElement(1, 3);
        pz2 = transformToParent2->GetElement(2, 3);
        p02 = transformToParent2->GetElement(3, 3);

        /*
        std::cerr << "tx  = "  << tx << std::endl;
        std::cerr << "ty  = "  << ty << std::endl;
        std::cerr << "tz  = "  << tz << std::endl;
        std::cerr << "t0  = "  << t0 << std::endl;
        std::cerr << "sx  = "  << sx << std::endl;
        std::cerr << "sy  = "  << sy << std::endl;
        std::cerr << "sz  = "  << sz << std::endl;
        std::cerr << "s0  = "  << s0 << std::endl;
        std::cerr << "nx  = "  << nx << std::endl;
        std::cerr << "ny  = "  << ny << std::endl;
        std::cerr << "nz  = "  << nz << std::endl;
        std::cerr << "n0  = "  << n0 << std::endl;
        std::cerr << "px  = "  << px << std::endl;
        std::cerr << "py  = "  << py << std::endl;
        std::cerr << "pz  = "  << pz << std::endl;
        std::cerr << "p0  = "  << p0 << std::endl;
        
        std::cerr << "tx2  = "  << tx2 << std::endl;
        std::cerr << "ty2  = "  << ty2 << std::endl;
        std::cerr << "tz2  = "  << tz2 << std::endl;
        std::cerr << "t02  = "  << t02 << std::endl;
        std::cerr << "sx2  = "  << sx2 << std::endl;
        std::cerr << "sy2  = "  << sy2 << std::endl;
        std::cerr << "sz2  = "  << sz2 << std::endl;
        std::cerr << "s02  = "  << s02 << std::endl;
        std::cerr << "nx2  = "  << nx2 << std::endl;
        std::cerr << "ny2  = "  << ny2 << std::endl;
        std::cerr << "nz2  = "  << nz2 << std::endl;
        std::cerr << "n02  = "  << n02 << std::endl;
        std::cerr << "px2  = "  << px2 << std::endl;
        std::cerr << "py2  = "  << py2 << std::endl;
        std::cerr << "pz2  = "  << pz2 << std::endl;
        std::cerr << "p02  = "  << p02 << std::endl;
        */
        
        // display the status 4/25/2010 ayamada
        char bufA[100],bufB[100],bufC[100];
        
        sprintf(bufA, "Position X:   %f",px);
        textActor1->SetInput(bufA);
                
        sprintf(bufB, "Position Y:   %f ",py);
        textActor2->SetInput(bufB);

        sprintf(bufC,"Position Z:    %f ",pz);
        textActor3->SetInput(bufC);

        // 5/17/2010 ayamada for test
        //this->saveCameraImageEntry->GetWidget()->GetValue ();

        
        
        
    }        

  if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
    
    // 5/6/2010 ayamada
    if(event == vtkMRMLTransformableNode::TransformModifiedEvent)
    {
        if(event == vtkMRMLScalarVolumeNode::ImageDataModifiedEvent)
        {
            //    this->GetApplicationGUI()->GetViewerWidget()->RequestRender();
            // Adding 10.5.5 ayamada 
            this->GetViewerWidget()->RequestRender();
          }
    }
    
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    
//    if(SecondaryViewerWindow->rw->IsMapped() == 1){ //----10.01.21-komura
    if(this->updateViewTriger==1){ // 10.01.25 ayamada
        if(this->runThread == 0){                  

            // 5/16/2010 ayamada
            this->closeWindowFlag = 1;
            
            this->makeCameraThread("cameraThread"); // 5/5/2010 ayamada             
            this->runThread = 1;               
        }else{
            
            if(singleWindowCheckButton->GetSelectedState()){
            
            if(updateView==1){    // 10.01.25 ayamada
                this->SecondaryViewerWindow->rw->Render();//10.01.12-komura
            }
            if(secView==1){ // 5/5/2010 ayamada
                this->SecondaryViewerWindow->rwLeft->Render();  // 5/5/2010 ayamada
            }
                
                
            }
            
            // 6/22/2010 ayamada
            if(this->stereoWindowCheckButton->GetSelectedState()){
                
                this->SecondaryViewerWindow->rw->Render();  // 5/5/2010 ayamada
//                this->SecondaryViewerWindow->lw->Render();  // 5/5/2010 ayamada
//                this->SecondaryViewerWindow->rwLeft->Render();  // 5/5/2010 ayamada
                this->SecondaryViewerWindow2->lw->Render();  // 6/22/2010 ayamada
//                this->SecondaryViewerWindow2->rw->Render();  // 6/22/2010 ayamada
//                this->SecondaryViewerWindow2->rwLeft->Render();  // 6/22/2010 ayamada

            
            }
        }
    }
    //----
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "SecondaryWindowWithOpenCV", "SecondaryWindowWithOpenCV", NULL );

  BuildGUIForHelpFrame();
  // 5/6/2010 ayamada
  BuildGUIForNodeSelectorFrame();
  BuildGUIForCaptureCameraImageFrame();
    
  // 5/7/2010 ayamada 
  //BuildGUIForWindowConfigurationFrame();

  // 5/6/2010 ayamada
//  BuildGUIForCaptureCameraImageFrame();
  //    BuildGUIForNavigationFrame();            // adding at 09. 9. 16 - smkim
  BuildGUIForTransformationFrame(); 
  BuildGUIForVolumeInfoFrame();
  BuildGUIForTransformation();    
    
  this->SecondaryViewerWindow = vtkSlicerSecondaryViewerWindow::New();
  this->SecondaryViewerWindow->SetApplication(this->GetApplication());
  this->SecondaryViewerWindow->Create();

  // 6/23/2010 ayamada
  this->SecondaryViewerWindow2 = vtkSlicerSecondaryViewerWindow::New();
  this->SecondaryViewerWindow2->SetApplication(this->GetApplication());
  this->SecondaryViewerWindow2->Create();
    
    
  // 5/16/2010 ayamada
  this->updateViewTriger = 0;

}


void vtkSecondaryWindowWithOpenCVGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:SecondaryWindowWithOpenCV</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "SecondaryWindowWithOpenCV" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::BuildGUIForWindowConfigurationFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("SecondaryWindowWithOpenCV");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Secondary Window with OpenCV Configuration");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Secondary Window child frame

  vtkKWFrame *switchframe = vtkKWFrame::New();
  switchframe->SetParent(conBrowsFrame->GetFrame());
  switchframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 switchframe->GetWidgetName() );

  // -----------------------------------------
  // Push buttons

  this->ShowSecondaryWindowWithOpenCVButton = vtkKWPushButton::New ( );
  this->ShowSecondaryWindowWithOpenCVButton->SetParent ( switchframe );
  this->ShowSecondaryWindowWithOpenCVButton->Create ( );
  this->ShowSecondaryWindowWithOpenCVButton->SetText ("ON");
  this->ShowSecondaryWindowWithOpenCVButton->SetWidth (12);

  this->HideSecondaryWindowWithOpenCVButton = vtkKWPushButton::New ( );
  this->HideSecondaryWindowWithOpenCVButton->SetParent ( switchframe );
  this->HideSecondaryWindowWithOpenCVButton->Create ( );
  //this->HideSecondaryWindowWithOpenCVButton->SetText ("OFF");
  this->HideSecondaryWindowWithOpenCVButton->SetText ("OBSERVE MRML");  // 4/25/2010 ayamada
  this->HideSecondaryWindowWithOpenCVButton->SetWidth (12);

  this->Script("pack %s %s -side left -padx 2 -pady 2", 
               this->ShowSecondaryWindowWithOpenCVButton->GetWidgetName(),
               this->HideSecondaryWindowWithOpenCVButton->GetWidgetName());

  conBrowsFrame->Delete();
  switchframe->Delete();

}


//----10.01.12-komura
// 5/5/2010 ayamada
int vtkSecondaryWindowWithOpenCVGUI::makeCameraThread(const char* nodeName)
{
    
    // 5/5/2010 ayamada    
    // for videoOverlay
    
    //const char* cameraThread; // 5/5/2010 ayamada   
    
    vtkMRMLModelNode           *locatorModel;
    vtkMRMLModelDisplayNode    *locatorDisp;
    
    
    locatorModel = vtkMRMLModelNode::New();
    locatorDisp = vtkMRMLModelDisplayNode::New();
    
    GetMRMLScene()->SaveStateForUndo();
    GetMRMLScene()->AddNode(locatorDisp);
    GetMRMLScene()->AddNode(locatorModel);  
    
    locatorDisp->SetScene(GetMRMLScene());
    
    locatorModel->SetName(nodeName);    
    locatorModel->SetScene(GetMRMLScene());
    locatorModel->SetAndObserveDisplayNodeID(locatorDisp->GetID());
    locatorModel->SetHideFromEditors(0);
             
     //------------------------------------------------------------------
     //  Setting up camera paramethers for rendering
     //------------------------------------------------------------------
     
     fileCamera = vtkCamera::New();
     //fileCamera->PerspectiveProjectionOff();
     fileCamera->SetViewUp (0.0, 0.0, 1.0);
     fileCamera->SetClippingRange( 0, 5 * focal_length );
     fileCamera->SetPosition ( 0.0, focal_length / 4.0, 0.0 );
     fileCamera->SetFocalPoint (0.0, 0.0, 0.0);
     fileCamera->SetViewAngle ( FOA );
     fileCamera->Zoom(1.0);    
     //fileCamera->ComputeViewPlaneNormal();

    //------------------------------------------------------------------
    //   starting thread for capturing camera image
    //------------------------------------------------------------------
  
    // 5/11/2010 ayamada
    this->ThreadID = this->Thread->SpawnThread((vtkThreadFunctionType) &vtkSecondaryWindowWithOpenCVGUI::thread_CameraThread, this);
    //sleep(1);    // 10.01.23 ayamada
    locatorDisp->Delete();        
    return 1;
    
}

void *vtkSecondaryWindowWithOpenCVGUI::thread_CameraThread(void* t)
{

    int deviceNum = 0; // 6/21/2010 ayamada
    
    // 5/15/ayamada
    CvCapture* capture[2];
    
    char bufCamera[100],bufCamera1[100];
    
    // 5/15/2010 ayamada
    IplImage*    captureImage[2];
    IplImage*    RGBImage[2];
    IplImage*    captureImageTmp[2];
    IplImage*    undistortionImage[2];    //adding at 09. 12. 15 - smkim

    // 6/6/2010 ayamada
    CvMat* mx1;
    CvMat* my1;    
    
    for(int j = 0; j<2; j++)
    {
    captureImage[j] = NULL;
    RGBImage[j] = NULL;
    captureImageTmp[j] = NULL;
    undistortionImage[j] = NULL;
    }
    
    vtkMultiThreader::ThreadInfo* vinfo = 
    static_cast<vtkMultiThreader::ThreadInfo*>(t);
    vtkSecondaryWindowWithOpenCVGUI* pGUI = 
    static_cast<vtkSecondaryWindowWithOpenCVGUI*>(vinfo->UserData);
 
/*    
    // 5/5/2010 ayamada
    vtkSecondaryWindowWithOpenCVGUI* pGUI 
    = (vtkSecondaryWindowWithOpenCVGUI*)
    (((vtkMultiThreader::ThreadInfo *)(t))->UserData);
*/    

    
    //**************************************************************************
    //   getting camera image initially
    //**************************************************************************
    // 5/15/2010 ayamada
    // for videoOverlay
    
    int i=0;
    
    if(first == 0){

        /*
        while(i<=10){// 5/16/2010 ayamada
            if( (NULL==(capture = cvCaptureFromCAM(i))))    // 10.01.25 ayamada
            {
                fprintf(stdout, "\n\nCan Not Find A Camera\n\n");    // 10.01.25 ayamada
                i++;                
            }else{
                // 5/16/2010 ayamada
                sprintf(bufCamera, "Connected Camera Device No: %d",i);
                pGUI->textActorCamera->SetInput(bufCamera);
                break;
            }
         */
        
            while(i<=10){// 6/20/2010 ayamada
                if( (NULL==(capture[deviceNum] = cvCaptureFromCAM(i))))    // 10.01.25 ayamada
                {
                    fprintf(stdout, "\n\nCan Not Find A First Camera\n\n");    // 10.01.25 ayamada
                    i++;                
                }else{
                    // 5/16/2010 ayamada
                    sprintf(bufCamera, "Connected A First Camera Device No: %d",i);
                    pGUI->textActorCamera->SetInput(bufCamera);

                    fprintf(stdout, "Connected A First Camera Device\n\n");    // 6/22/2010 ayamada

                    
                    deviceNum = 1;
                    i++; // if i++, the viewer can not obtain the image!!
                    
                    break;
                }
                
        
            }// end of while

        while(i<=10){// 6/22/2010 ayamada
            if( (NULL==(capture[deviceNum] = cvCaptureFromCAM(i))))    // 10.01.25 ayamada
            {
                fprintf(stdout, "\n\nCan Not Find A Second Camera\n\n");    // 10.01.25 ayamada
                i++;                
            }else{
                // 5/16/2010 ayamada
                sprintf(bufCamera, "Connected A Second Camera Device No: %d",i);
                pGUI->textActorCamera->SetInput(bufCamera);

                fprintf(stdout, "Connected A Second Camera Device\n\n");    // 6/22/2010 ayamada

                
                deviceNum = 2; // 6/22/2010 ayamada

                break;
            }
            
            
        }// end of while

        // 5/16/2010 ayamada
        //if(i==11 && deviceNum == 0){ // 6/22/2010 ayamada 
        if(i==11 && deviceNum == 0){ // 6/22/2010 ayamada 
            sprintf(bufCamera, "Can Not Find Camera Device!!");
            pGUI->textActorCamera->SetInput(bufCamera);
        }


    // 5/16/2010 ayamada
//    if((capture[0] != NULL) && (capture[1] != NULL)){ // 6/22/2010 ayamada
//        if(capture[0] != NULL){ // 6/22/2010 ayamada
        
        for(int n=0;n<deviceNum;n++){   // 6/22/2010 ayamada
            if(capture[n] != NULL){        
        
        while(1){//10.01.20-komura
            
            // 5/15/2010 ayamada
            if(NULL == (captureImageTmp[n] = cvQueryFrame( capture[n] ))){

                sleep(2);    // 5/18/2010 ayamada

                fprintf(stdout, "\n\nCan Not Take A Picture\n\n");
                // 5/15/2010 ayamada
                continue;
            }        
            
            // 5/6/2010 ayamada creating RGB image and capture image
            pGUI->imageSize = cvGetSize( captureImageTmp[n] );
            captureImage[n] = cvCreateImage(pGUI->imageSize, IPL_DEPTH_8U,3);    
            RGBImage[n] = cvCreateImage(pGUI->imageSize, IPL_DEPTH_8U, 3);    
            
            // create rgb image
            // 5/6/2010 ayamada for videoOverlay
            pGUI->imageSize = cvGetSize( captureImageTmp[n] );
            cvFlip(captureImageTmp[n], captureImage[n], 0);

            // 6/6/2010 ayamada
            mx1 = cvCreateMat(pGUI->imageSize.height,pGUI->imageSize.width, CV_32F);
            my1 = cvCreateMat(pGUI->imageSize.height,pGUI->imageSize.width, CV_32F);
            
            // 5/6/2010 for videoOverlay ayamada
            undistortionImage[n] = cvCreateImage( pGUI->imageSize, IPL_DEPTH_8U, 3);
            
            // capture image
            // 5/6/2010 ayamada for videoOverlay
            cvUndistort2( captureImage[n], undistortionImage[n], pGUI->intrinsicMatrix[n], pGUI->distortionCoefficient[n] );            
            
            cvCvtColor( undistortionImage[n], RGBImage[n], CV_BGR2RGB);    //comment not to undistort    at 10. 01. 07 - smkim
            
            // 5/6/2010 ayamada ok for videoOverlay
            pGUI->idata[n] = (unsigned char*) RGBImage[n]->imageData;
            
            pGUI->importer[n]->SetWholeExtent(0,pGUI->imageSize.width-1,0,pGUI->imageSize.height-1,0,0);
            //this->importer[i]->SetDataSpacing( 2.0, 2.0, 2.0); // 5/6/2010 ayamada for videoOverlay
            pGUI->importer[n]->SetDataExtentToWholeExtent();
            pGUI->importer[n]->SetDataScalarTypeToUnsignedChar();
            pGUI->importer[n]->SetNumberOfScalarComponents(3);
            pGUI->importer[n]->SetImportVoidPointer(pGUI->idata[n]);
            
            // 5/6/2010 ayamada fot videoOverlay
            pGUI->atext[n]->SetInputConnection(pGUI->importer[n]->GetOutputPort());
            pGUI->atext[n]->InterpolateOn();            
            
            pGUI->importer[n]->Update();
            
            /*            
             // 10.01.24 ayamada
             double planeX = -1.0;
             double planeY = -1.0;
             double planeLength = 1.0;
             
             this->planeSource->SetOrigin(planeX,planeY,0.0);    
             this->planeSource->SetCenter(0.0,0.0,0.0);    
             this->planeSource->SetResolution(1,1);
             this->planeSource->SetPoint1(planeLength,planeY,0.0);
             this->planeSource->SetPoint2(planeX,planeLength,0.0);
             
             this->planeMapper->SetInputConnection(this->planeSource->GetOutputPort());
             this->Actor->SetMapper(this->planeMapper);   // plane source mapper
             this->Actor->SetTexture(this->atext[i]);        // texture mapper
             this->Actor->SetMapper(this->planeMapper);
             //Actor->SetMapper(mapper); //10.01.16-komura
             this->SecondaryViewerWindow->rw->AddViewProp(this->Actor);
             
             // 10.01.25 ayamada
             //        pGUI->Mutex->Lock();
             //        pGUI->SecondaryViewerWindow->rw->Render();
             //        pGUI->Mutex->Unlock();
             this->updateView=1;
             
             sleep(1);    // 10.01.25 ayamada
             
             //pGUI->SecondaryViewerWindow->rw->ResetCamera(); //10.01.27-komura
             //    pGUI->SecondaryViewerWindow->rw->SetCameraPosition(0,0,0);
             
             // pGUI->Mutex->Unlock();
             first = 1;
             fprintf(stdout, "\nget camera handle\n");//10.01.20-komura
             */
            // 5/15/2010 ayamada
            first = 1;
            break;//10.01.20-komura
        }
    }
 //       }
    
/*    
    // 4/25/2010 ayamada
    pGUI->textActor->SetInput("Marker Position");
    pGUI->textActor->GetTextProperty()->SetFontSize(14);
    pGUI->textActor->GetTextProperty()->BoldOn();
    pGUI->textActor->SetPosition(10,70);    
    
    //this->SecondaryViewerWindow->rwLeft->AddViewProp(this->textActor);
    // 5/8/2010 ayamada
    pGUI->SecondaryViewerWindow->rwLeft->GetRenderer()->AddActor(pGUI->textActor);
    
    
    pGUI->textActor1->SetInput("Position X:");
    pGUI->textActor1->GetTextProperty()->SetFontSize(14);
    pGUI->textActor1->SetPosition(10,50);
    
    pGUI->textActor2->SetInput("Position Y:");
    pGUI->textActor2->GetTextProperty()->SetFontSize(14);
    pGUI->textActor2->SetPosition(10,30);
    
    pGUI->textActor3->SetInput("Position Z:");
    pGUI->textActor3->GetTextProperty()->SetFontSize(14);
    pGUI->textActor3->SetPosition(10,10);

    // for camera information
    // 5/16/2010 ayamada
    pGUI->textActorCamera->GetTextProperty()->SetFontSize(14);
    pGUI->textActorCamera->GetTextProperty()->BoldOn();
    pGUI->textActorCamera->SetPosition(200,70);    
    pGUI->SecondaryViewerWindow->rwLeft->GetRenderer()->AddActor(pGUI->textActorCamera);

    // for camera information
    // 5/16/2010 ayamada
    pGUI->textActorSavePathH->SetInput("Snapshot Save Path:");
    pGUI->textActorSavePathH->GetTextProperty()->SetFontSize(14);
    pGUI->textActorSavePathH->GetTextProperty()->BoldOn();
    pGUI->textActorSavePathH->SetPosition(200,50);    
    pGUI->SecondaryViewerWindow->rwLeft->GetRenderer()->AddActor(pGUI->textActorSavePathH);
        
    pGUI->textActorSavePath->GetTextProperty()->SetFontSize(12);
    pGUI->textActorSavePath->SetPosition(200,30);    
    pGUI->SecondaryViewerWindow->rwLeft->GetRenderer()->AddActor(pGUI->textActorSavePath);
*/
        
        
    //**************************************************************************

    //**************************************************************************
    //  Setting visualization environment for camera image up
    //**************************************************************************
    // 5/15/2010 ayamada
    // for videoOverlay
    
    pGUI->planeRatio = VIEW_SIZE_X / VIEW_SIZE_Y;
    
    // 5/6/2010 ayamada for videoOverlay
    //CameraFocusPlane(this->fileCamera, this->planeRatio, this->Actor);
    pGUI->CameraFocusPlane(pGUI->fileCamera, pGUI->planeRatio);
    
    
    // 5/7/2010 ayamada
    pGUI->FocalPlaneMapper->SetInput(pGUI->FocalPlaneSource->GetOutput());
    pGUI->actor[n]->SetMapper(pGUI->FocalPlaneMapper);
    pGUI->actor[n]->SetUserMatrix(pGUI->ExtrinsicMatrix);
    
    // 5/6/2010 ayamada for videoOverlay
    // 10.01.24 ayamada
    pGUI->actor[n]->SetTexture(pGUI->atext[n]);        // texture mapper
    //this->Actor->GetProperty()->SetOpacity(1.0);// configuration property
    //this->SecondaryViewerWindow->rw->AddViewProp(this->Actor);
    // 5/8/2010 ayamada
/////    pGUI->SecondaryViewerWindow->rw->GetRenderer()->AddActor(pGUI->actor[n]); // 6/23/2010 ayamada
    
    
    // 10.01.25 ayamada
    //        pGUI->Mutex->Lock();
    //        pGUI->SecondaryViewerWindow->rw->Render();
    //        pGUI->Mutex->Unlock();
/////    pGUI->updateView=1; // 6/23/2010 ayamada 
    
    //sleep(1);    // 10.01.25 ayamada
    
    //      pGUI->SecondaryViewerWindow->rw->ResetCamera(); //10.01.27-komura
    //    pGUI->SecondaryViewerWindow->rw->SetCameraPosition(0,0,0);
    
/////    fprintf(stdout, "\nget camera handle\n");//10.01.20-komura // 6/23/2010 ayamada
    
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
    //pGUI->SecondaryViewerWindow->rw->GetRenderer()->GetActiveCamera()->ParallelProjectionOff();
/////    pGUI->SecondaryViewerWindow->rw->GetRenderer()->SetActiveCamera( pGUI->fileCamera ); // 6/23/2010 ayamada

    //**************************************************************************
    
    }    // 5/16/2010 if(capture != NULL)

        
    } // end of for 6/22/2010 ayamada
    

    // connect to renderer 6/23/2010 ayamada
    pGUI->SecondaryViewerWindow->rw->GetRenderer()->AddActor(pGUI->actor[1]);
    fprintf(stdout, "\nget right camera handle\n");//10.01.20-komura
    pGUI->SecondaryViewerWindow->rw->GetRenderer()->SetActiveCamera( pGUI->fileCamera );

    
    pGUI->SecondaryViewerWindow2->lw->GetRenderer()->AddActor(pGUI->actor[0]);
    fprintf(stdout, "\nget left camera handle\n");//10.01.20-komura
    pGUI->SecondaryViewerWindow2->lw->GetRenderer()->SetActiveCamera( pGUI->fileCamera );
    
    
    pGUI->updateView=1;
    
   
    // 4/25/2010 ayamada
    pGUI->textActor->SetInput("Marker Position");
    pGUI->textActor->GetTextProperty()->SetFontSize(14);
    pGUI->textActor->GetTextProperty()->BoldOn();
    pGUI->textActor->SetPosition(10,70);    
    
    //this->SecondaryViewerWindow->rwLeft->AddViewProp(this->textActor);
    // 5/8/2010 ayamada
    pGUI->SecondaryViewerWindow->rwLeft->GetRenderer()->AddActor(pGUI->textActor);
    
    pGUI->textActor1->SetInput("Position X:");
    pGUI->textActor1->GetTextProperty()->SetFontSize(14);
    pGUI->textActor1->SetPosition(10,50);
    
    pGUI->textActor2->SetInput("Position Y:");
    pGUI->textActor2->GetTextProperty()->SetFontSize(14);
    pGUI->textActor2->SetPosition(10,30);
    
    pGUI->textActor3->SetInput("Position Z:");
    pGUI->textActor3->GetTextProperty()->SetFontSize(14);
    pGUI->textActor3->SetPosition(10,10);
    
    // for camera information
    // 5/16/2010 ayamada
    pGUI->textActorCamera->GetTextProperty()->SetFontSize(14);
    pGUI->textActorCamera->GetTextProperty()->BoldOn();
    pGUI->textActorCamera->SetPosition(200,70);    
    pGUI->SecondaryViewerWindow->rwLeft->GetRenderer()->AddActor(pGUI->textActorCamera);
    
    // for camera information
    // 5/16/2010 ayamada
    pGUI->textActorSavePathH->SetInput("Snapshot Save Path:");
    pGUI->textActorSavePathH->GetTextProperty()->SetFontSize(14);
    pGUI->textActorSavePathH->GetTextProperty()->BoldOn();
    pGUI->textActorSavePathH->SetPosition(200,50);    
    pGUI->SecondaryViewerWindow->rwLeft->GetRenderer()->AddActor(pGUI->textActorSavePathH);
    
    pGUI->textActorSavePath->GetTextProperty()->SetFontSize(12);
    pGUI->textActorSavePath->SetPosition(200,30);    
    pGUI->SecondaryViewerWindow->rwLeft->GetRenderer()->AddActor(pGUI->textActorSavePath);
    
    
    // 6/6/2010 ayamada
    cvInitUndistortMap(pGUI->intrinsicMatrix[0], pGUI->distortionCoefficient[0], mx1,my1);    
    cvInitUndistortMap(pGUI->intrinsicMatrix[1], pGUI->distortionCoefficient[1], mx1,my1);    
    
     // 5/11/2010 ayamada
    while(pGUI->closeWindowFlag==1){

        for(int i2 = 0; i2 < deviceNum; i2++){ 
        
        if(capture[i2] != NULL){    // 5/16/2010 ayamada
        
        // 5/6/2010 ayamada for videoOverlay
        if ( pGUI->m_bDriveSource == false )    // driven by manual
        {
            //m_pLogic->volume->SetOrientation(m_pLogic->rotationAngleX, m_pLogic->rotationAngleY, m_pLogic->rotationAngleZ);        // adding at 09. 9. 8 - smkim
            //m_pLogic->volume->SetPosition(m_pLogic->translationX, m_pLogic->translationY, m_pLogic->translationZ);            // adding at 09. 9. 14 - smkim
            
            //m_pLogic->fileCamera->Yaw(m_pLogic->rotationAngleZ);
            
        }
        else    // driven by optical tracking system
        {
        
            // 5/15/2010 ayamada            
            pGUI->GetCurrentTransformMatrix ();
            pGUI->CameraSet(pGUI->fileCamera, pGUI->cameraMatrix, FOA);
            // 5/7/2010 ayamada
            pGUI->CameraFocusPlane(pGUI->fileCamera, pGUI->planeRatio);
            
            // 5/29/2010 ayamada
            // the following codes are needed with the CameraFocusPlane
            pGUI->FocalPlaneMapper->SetInput(pGUI->FocalPlaneSource->GetOutput());
            pGUI->actor[i2]->SetMapper(pGUI->FocalPlaneMapper);
            pGUI->actor[i2]->SetUserMatrix(pGUI->ExtrinsicMatrix);                        
            
                                 
        }                
        
        }
            
            for(int i3 = 0; i3 < deviceNum; i3++){ 
            
            
    // 5/15/2010 ayamada
    captureImageTmp[i3] = cvQueryFrame( capture[i3] );    // 10.01.23 ayamada
////        pGUI->imageSize = cvGetSize( pGUI->captureImageTmp );
        
        cvFlip(captureImageTmp[i3], captureImage[i3], 0);        

        //cvUndistort2( captureImage, undistortionImage, pGUI->intrinsicMatrix, pGUI->distortionCoefficient );        
        // 6/6/2010 ayamada
        if(pGUI->undistortionFlag==1){
            cvInitUndistortMap(pGUI->intrinsicMatrix[i3], pGUI->distortionCoefficient[i3], mx1,my1);
            //cvInitUndistortMap(pGUI->intrinsicMatrix[1], pGUI->distortionCoefficient[1], mx1,my1);
            pGUI->undistortionFlag=0;
        }
        cvRemap(captureImage[i3], undistortionImage[i3],mx1,my1);
            
        // 5/7/2010 ayamada
        //cvCvtColor( pGUI->captureImage, pGUI->RGBImage, CV_BGR2RGB);
        // 5/15/2010 ayamada
        cvCvtColor( undistortionImage[i3], RGBImage[i3], CV_BGR2RGB);
              
        pGUI->idata[i3] = (unsigned char*) RGBImage[i3]->imageData;
        pGUI->importer[i3]->Modified();        
        
// 10.01.26-komura
//
//        if(pGUI->SecondaryViewerWindow->rw->GetApplication()->EvaluateBooleanExpression(
//        "expr {[winfo exists %s] && [winfo ismapped %s]}", 
//        pGUI->SecondaryViewerWindow->rw->GetWidgetName(), pGUI->SecondaryViewerWindow->rw->GetWidgetName())
//        == 0){
//    
//        fprintf(stdout,"\nbreak\n");//10.01.20-komura
//        break;
//        }


// 10.01.26-komura

//IsMapped function returns 0 when "rw" is disappeared.
//But this function isn't stability, so I stoped using this.

//        if(pGUI->SecondaryViewerWindow->rw->IsMapped() == 0){//10.01.21-komura
//        //if(pGUI->SecondaryViewerWindow->rw->IsAlive() == 0){//10.01.21-komura
//        //if(pGUI->SecondaryViewerWindow->rw->IsCreated() == 0){//10.01.21-komura
//        fprintf(stdout,"\nbreak\n");//10.01.20-komura
//        break;
//        }


        }    // 5/16/2010 ayamada    if(capture != NULL) 2        

                
        pGUI->SecondaryViewerWindow->rwLeft->GetRenderer()->AddActor(pGUI->textActor1);
        pGUI->SecondaryViewerWindow->rwLeft->GetRenderer()->AddActor(pGUI->textActor2);
        pGUI->SecondaryViewerWindow->rwLeft->GetRenderer()->AddActor(pGUI->textActor3);
        
        //pGUI->SecondaryViewerWindow->rwLeft->AddViewProp(pGUI->textActor1);
        
        // 5/17/2010 ayamada get snapshot 
        if(pGUI->snapShotShutter==1){
            pGUI->Mutex->Lock();        
            cvSaveImage(pGUI->snapShotSavePath, captureImageTmp);
            pGUI->snapShotShutter = 0;
            pGUI->Mutex->Unlock();            
        }
        
        } // end of for loop i3, 6/22/2010 ayamada
            
    }

        
            
    // 5/16/2010 ayamada
//    pGUI->closeWindowFlag = 3;
    
    std::cerr << "thread was finished!!" << std::endl;
    
    // 5/16/2010 ayamada
    pGUI->updateViewTriger = 0;    
    pGUI->runThread = 0;//10.01.21-komura

/*    
    // 5/15/2010 ayamada
    cvReleaseImage(&RGBImage);
    cvReleaseImage(&undistortionImage);    
    cvReleaseImage(&captureImage);            
    cvReleaseImage(&captureImageTmp);        
*/
    
    if(capture[0] != NULL){    
        // 5/15/2010 ayamada
        cvReleaseCapture(&capture[0]);  
    }

    // 6/22/2010 ayamada
    if(capture[1] != NULL){    
        // 5/15/2010 ayamada
        cvReleaseCapture(&capture[1]);  
    }
    
    
    // 5/16/2010 ayamada
    pGUI->closeWindowFlag = 3;
        
    return NULL;
     
}


//---------------------------------------------------------------------------
// for calculating position and orientation of camera image plane
// adding at 09. 11. 5 - from wang
//void vtkSecondaryWindowWithOpenCVGUI::CameraFocusPlane(vtkCamera * cam, double Ratio, vtkActor * Actor_CFP)
void vtkSecondaryWindowWithOpenCVGUI::CameraFocusPlane(vtkCamera * cam, double Ratio)
{

    //double Pos[3];
    //double Focal[3];
    //vtkMatrix4x4 * ExtrinsicMatrix = vtkMatrix4x4::New();
    
    cam->GetPosition(this->Pos);
    cam->GetFocalPoint(this->Focal);

    this->ExtrinsicMatrix->DeepCopy(cam->GetViewTransformMatrix());
    this->ExtrinsicMatrix->Invert();

    //double F = sqrt(vtkMath::Distance2BetweenPoints(Pos, Focal));
    //double ViewAngle = cam->GetViewAngle() / 2;
    //double h = 2 * tan(vtkMath::DoubleDegreesToRadians() * (ViewAngle / 2.0) ) * F;

    this->F = sqrt(vtkMath::Distance2BetweenPoints(this->Pos, this->Focal));
    this->ViewAngle = cam->GetViewAngle() / 2.0;    // 5/7/2010 ayamada 2->2.0
    // 5/7/2010 ayamada; changing fuction from DoubleDegreesToRadians() to RadiansFromDegrees()  
    
    // 5/11/2010 ayamada
    //this->h = 2.0 * tan(vtkMath::DoubleDegreesToRadians() * (this->ViewAngle / 2.0) ) * F; // 5/7/2010 ayamada 2->2.0
    this->h = 2.0 * tan(vtkMath::RadiansFromDegrees(this->ViewAngle / 2.0)) * F; // 5/7/2010 ayamada 2->2.0
    
    //vtkPlaneSource * FocalPlaneSource = vtkPlaneSource::New();
    
    //double fx = ( 2*h * Ratio * focal_point_x ) / 640.0;
    //double fy = ( 2*h * focal_point_y ) / 480.0;

    this->fx = ( 2.0*this->h * Ratio * this->focal_point_x ) / 640.0; // 5/7/2010 ayamada 2->2.0
    this->fy = ( 2.0*this->h * this->focal_point_y ) / 480.0; // 5/7/2010 ayamada 2->2.0
    
    this->FocalPlaneSource->SetOrigin(-this->h * Ratio, -this->h, -this->F);
    this->FocalPlaneSource->SetPoint1(this->h * Ratio, -this->h, -this->F);
    this->FocalPlaneSource->SetPoint2(-this->h * Ratio, this->h, -this->F);
    
    this->FocalPlaneSource->SetCenter(0.0, 0.0, -this->F);
    
     /*
     double fx = ( 2*h * Ratio * focal_point_x ) / 640.0;
     double fy = ( 2*h * focal_point_y ) / 480.0;
     
     FocalPlaneSource->SetOrigin(-h * Ratio + fx, -h + fy, -F);
     FocalPlaneSource->SetPoint1(h * Ratio + fx, -h + fy, -F);
     FocalPlaneSource->SetPoint2(-h * Ratio + fx, h + fy, -F);
     
     FocalPlaneSource->SetCenter(fx, fy, -F);
     */    
    
    //vtkPolyDataMapper * focalPlaneMapper = vtkPolyDataMapper::New();

    //this->Mutex->Lock();
    // 5/7/2010 ayamada
    ///FocalPlaneMapper->SetInput(FocalPlaneSource->GetOutput());
    //focalPlaneMapper->SetInput(FocalPlaneSource->GetOutput());
    //focalPlaneMapper->SetInputConnection(FocalPlaneSource->GetOutputPort());
    ///FocalPlaneSource->Delete();

    ///Actor_CFP->SetMapper(FocalPlaneMapper);
    //FocalPlaneMapper->Delete();
    //Actor_CFP->SetMapper(focalPlaneMapper);
    //focalPlaneMapper->Delete();
    //Actor_CFP->SetUserMatrix(ExtrinsicMatrix);
    //ExtrinsicMatrix->Delete();
    ///Actor_CFP->SetUserMatrix(ExtrinsicMatrix);
    //ExtrinsicMatrix->Delete();
    //this->Mutex->Unlock();        
    
    
}

//---------------------------------------------------------------------------
// Getting Transform Matrix from OpenIGTLinkIF module
// adding at 09. 9. 16 - smkim
void vtkSecondaryWindowWithOpenCVGUI::GetCurrentTransformMatrix()
{
    
    this->OriginalTrackerNode = vtkMRMLLinearTransformNode::SafeDownCast(this->TransformEditorWidget->GetTransformEditSelectorWidget()->GetSelected());    //adding at 09. 12. 24. - smkim
    
    if (! this->OriginalTrackerNode)
    {
        return;
    }
    
    transformMatrix = this->OriginalTrackerNode->GetMatrixTransformToParent();
    
    if (transformMatrix)
    {
        // set volume orientation
        cameraMatrix[0] = transformMatrix->GetElement(0, 0);
        cameraMatrix[1] = transformMatrix->GetElement(0, 1);
        cameraMatrix[2] = transformMatrix->GetElement(0, 2);
        cameraMatrix[3] = transformMatrix->GetElement(0, 3);
        
        cameraMatrix[4] = transformMatrix->GetElement(1, 0);
        cameraMatrix[5] = transformMatrix->GetElement(1, 1);
        cameraMatrix[6] = transformMatrix->GetElement(1, 2);
        cameraMatrix[7] = transformMatrix->GetElement(1, 3);
        
        cameraMatrix[8] = transformMatrix->GetElement(2, 0);
        cameraMatrix[9] = transformMatrix->GetElement(2, 1);
        cameraMatrix[10] = transformMatrix->GetElement(2, 2);
        cameraMatrix[11] = transformMatrix->GetElement(2, 3);
        
        cameraMatrix[12] = transformMatrix->GetElement(3, 0);
        cameraMatrix[13] = transformMatrix->GetElement(3, 1);
        cameraMatrix[14] = transformMatrix->GetElement(3, 2);
        cameraMatrix[15] = transformMatrix->GetElement(3, 3);
    }
    
}

//---------------------------------------------------------------------------
// for calculating camera position and orientation
// adding at 09. 11. 5 - from wang
void vtkSecondaryWindowWithOpenCVGUI::CameraSet(vtkCamera *NaviCamera, double *Matrix, double FOV)
{
    
    double F = focal_length;
    double * m = Matrix;
    //vtkMatrix4x4::Invert(Matrix, m);
    double pos[3] = {m[3], m[7], m[11]};
    double focal[3] = {    pos[0] + m[0] * focal_point_x + m[1] * focal_point_y + m[2] * (F), 
        pos[1] + m[4] * focal_point_x + m[5] * focal_point_y + m[6] * (F), 
        pos[2] + m[8] * focal_point_x + m[9] * focal_point_y + m[10] * (F)    };
    
    double viewup[3] = {-m[1], -m[5], -m[9]};
    
    NaviCamera->SetPosition(pos);
    NaviCamera->SetFocalPoint(focal);
    NaviCamera->SetViewUp(viewup);
    NaviCamera->SetClippingRange(0, 5 * F);
    NaviCamera->SetViewAngle(FOV);
    
}

//**************************************************************************
//  Surface Rendering of Model
//**************************************************************************
// adding at 10. 01. 29 - smkim
void vtkSecondaryWindowWithOpenCVGUI::vtkSurfaceModelRender()
{
    if(!this->m_bSurfaceRendering)
    {
        //-------------------------------------------------------------
        //to add MRML node on Slicer3
        //adding at 09. 9. 4 - smkim
         vtkMRMLModelNode           *ModelNode;
        vtkMRMLModelDisplayNode    *DispNode;
        
        
        ModelNode = vtkMRMLModelNode::New();
        DispNode = vtkMRMLModelDisplayNode::New();
        
        GetMRMLScene()->SaveStateForUndo();
        GetMRMLScene()->AddNode(DispNode);
        GetMRMLScene()->AddNode(ModelNode);  
        
        DispNode->SetScene(this->GetMRMLScene());
        
        
        ModelNode->SetName( "Model on Camera Image" );
        ModelNode->SetScene(this->GetMRMLScene());
        ModelNode->SetAndObserveDisplayNodeID(DispNode->GetID());
        ModelNode->SetHideFromEditors(0);
        
        
        //-------------------------------------------------------------
        //to add MRML node for multi-object rendering on Slicer3
        //adding at 10. 2. 5 - smkim
         vtkMRMLModelNode           *ModelNode1;
        vtkMRMLModelDisplayNode    *DispNode1;
        
        
        ModelNode1 = vtkMRMLModelNode::New();
        DispNode1 = vtkMRMLModelDisplayNode::New();
        
        GetMRMLScene()->SaveStateForUndo();
        GetMRMLScene()->AddNode(DispNode1);
        GetMRMLScene()->AddNode(ModelNode1);  
        
        DispNode1->SetScene(this->GetMRMLScene());
        
        
        ModelNode1->SetName( "2nd Model on Camera Image" );
        ModelNode1->SetScene(this->GetMRMLScene());
        ModelNode1->SetAndObserveDisplayNodeID(DispNode1->GetID());
        ModelNode1->SetHideFromEditors(0);
    }
    
    //vtkPolyDataReader* polyReader = vtkPolyDataReader::New();
    //polyReader->SetFileName( "/home/sungminkim/model.vtk" );    //target.vtk
      //polyReader->SetFileName( "/Users/ayamada/slicerData05082010/Muscles.vtk" );    //target.vtk
    polyReader->SetFileName( "/Users/ayamada/Downloads/100530-videoAndSecondAxialTest/SlicerMRML_MRI/Bone.vtk" );    //target.vtk
    
    //vtkPolyDataNormals* polyNormal = vtkPolyDataNormals::New();
    polyNormal->SetInputConnection( polyReader->GetOutputPort() );
    polyNormal->SetFeatureAngle( 70.0 );
    
    //vtkPolyDataMapper* polyMapper = vtkPolyDataMapper::New();
    polyMapper->SetInputConnection( polyNormal->GetOutputPort() );
    polyMapper->ScalarVisibilityOff();
    
    polyActor->SetMapper( polyMapper );
    polyActor->GetProperty()->SetColor( 0.0, 1.0, 0.0 );
    polyActor->GetProperty()->SetOpacity( 0.2 );
    polyActor->SetOrientation( 0.0, 0.0, 0.0 );

    // 5/6/2010 ayamada, need to change
    //    ren->AddActor( polyActor );
    //this->SecondaryViewerWindow->rw->AddViewProp(polyActor);
    // 5/8/2010 ayamada
    this->SecondaryViewerWindow->rw->GetRenderer()->AddActor(polyActor);
    this->SecondaryViewerWindow2->lw->GetRenderer()->AddActor(polyActor);
    
    
    // for multi-object rendering
    // adding at 10. 02. 05 - smkim
    //vtkPolyDataReader* polyReader1 = vtkPolyDataReader::New();
    polyReader1->SetFileName( "/home/sungminkim/model1.vtk" );    //obstacle.vtk
    
    //vtkPolyDataNormals* polyNormal1 = vtkPolyDataNormals::New();
    polyNormal1->SetInputConnection( polyReader1->GetOutputPort() );
    polyNormal1->SetFeatureAngle( 70.0 );
    
    //vtkPolyDataMapper* polyMapper1 = vtkPolyDataMapper::New();
    polyMapper1->SetInputConnection( polyNormal1->GetOutputPort() );
    polyMapper1->ScalarVisibilityOff();
    
    polyActor1->SetMapper( polyMapper1 );
    polyActor1->GetProperty()->SetColor( 1.0, 0.0, 0.0 );
    polyActor1->GetProperty()->SetOpacity( 0.2 );
    polyActor1->SetOrientation( 0.0, 0.0, 0.0 );
    
    // 5/6/2010 ayamada, need to change
    //    ren->AddActor( polyActor1 );
    //this->SecondaryViewerWindow->rw->AddViewProp(polyActor1);
    // 5/8/2010 ayamada
    // 5/10/2010 ayamada
    //this->SecondaryViewerWindow->rw->GetRenderer()->AddActor(polyActor1);
    
    
    if( !this->m_bSurfaceRendering )
        this->m_bSurfaceRendering = true;    //setting the flag of surface rendering up    // adding at 10. 02. 01 - smkim
    
}

//---------------------------------------------------------------------------
// adding for volume transformation frame gui
// adding at 09. 9. 8 - smkim
void vtkSecondaryWindowWithOpenCVGUI::BuildGUIForTransformationFrame ()
{
 
    // 5/6/2010 ayamada
    vtkKWWidget *page = this->UIPanel->GetPageWidget ("SecondaryWindowWithOpenCV");

    vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();
    
    conBrowsFrame->SetParent(page);
    conBrowsFrame->Create();
    conBrowsFrame->SetLabelText("Volume Transformation");
    //    conBrowsFrame->CollapseFrame();
    this->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", conBrowsFrame->GetWidgetName(), page->GetWidgetName());
    
    // -----------------------------------------
    // frame with label
    
    
    // Navigation FRAME
    // adding at 09. 9. 16 - smkim            
    vtkKWFrameWithLabel *trackingFrame = vtkKWFrameWithLabel::New();
    trackingFrame->SetParent(conBrowsFrame->GetFrame());
    trackingFrame->Create ( );
    trackingFrame->SetLabelText ("Navigation");
    this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", trackingFrame->GetWidgetName() );
    
    // rotation frame    adding at 09. 9. 14 - smkim
    vtkKWFrameWithLabel *RotationFrame = vtkKWFrameWithLabel::New();
    RotationFrame->SetParent(conBrowsFrame->GetFrame());
    RotationFrame->Create();
    RotationFrame->SetLabelText ("Rotation");
    this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", RotationFrame->GetWidgetName() );
    
    // translation frame    adding at 09. 9. 14 - smkim
    vtkKWFrameWithLabel *TranslationFrame = vtkKWFrameWithLabel::New();
    TranslationFrame->SetParent(conBrowsFrame->GetFrame());
    TranslationFrame->Create();
    TranslationFrame->SetLabelText ("Translation");
    this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", TranslationFrame->GetWidgetName() );
    
    
    // -----------------------------------------
    // Transform node name entry
    this->TransformNodeNameEntry = vtkKWEntryWithLabel::New();
    this->TransformNodeNameEntry->SetParent(trackingFrame->GetFrame());
    this->TransformNodeNameEntry->Create();
    this->TransformNodeNameEntry->SetWidth(40);
    this->TransformNodeNameEntry->SetLabelWidth(30);
    this->TransformNodeNameEntry->SetLabelText("Input (Transform) Node Name:");
    this->TransformNodeNameEntry->GetWidget()->SetValue ( "Tracker" );
    this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2", this->TransformNodeNameEntry->GetWidgetName());
    
    
    // selecting drive source ( manual / data of optical tracking system )
    // adding at 09. 9. 17 - smkim
    this->selectDriveSource = vtkKWPushButton::New ( );
    this->selectDriveSource->SetParent ( trackingFrame->GetFrame() );
    this->selectDriveSource->Create ( );
    this->selectDriveSource->SetText ("Selecting Drive Source");
    this->selectDriveSource->SetWidth (24);
    
    this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", this->selectDriveSource->GetWidgetName() );
    
    // -----------------------------------------
    // ScaleWithLabel

    
    // related to rotations
    this->RotationAngleX = vtkKWScaleWithLabel::New();
    this->RotationAngleX->SetParent ( RotationFrame->GetFrame() );
    this->RotationAngleX->Create ( );
    this->RotationAngleX->SetLabelText("Rotation X");
    this->RotationAngleX->GetWidget()->SetRange(0,360);
    this->RotationAngleX->GetWidget()->SetResolution(1.0);
    this->RotationAngleX->SetBalloonHelpString("set rotation angle on X axis.");
    this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2", this->RotationAngleX->GetWidgetName(), RotationFrame->GetFrame()->GetWidgetName());
    
    this->RotationAngleY = vtkKWScaleWithLabel::New();
    this->RotationAngleY->SetParent ( RotationFrame->GetFrame() );
    this->RotationAngleY->Create ( );
    this->RotationAngleY->SetLabelText("Rotation Y");
    this->RotationAngleY->GetWidget()->SetRange(0,360);
    this->RotationAngleY->GetWidget()->SetResolution(1.0);
    this->RotationAngleY->SetBalloonHelpString("set rotation angle on Y axis.");
    this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2", this->RotationAngleY->GetWidgetName(), RotationFrame->GetFrame()->GetWidgetName());
    
    this->RotationAngleZ = vtkKWScaleWithLabel::New();
    this->RotationAngleZ->SetParent ( RotationFrame->GetFrame() );
    this->RotationAngleZ->Create ( );
    this->RotationAngleZ->SetLabelText("Rotation Z");
    this->RotationAngleZ->GetWidget()->SetRange(0,360);
    this->RotationAngleZ->GetWidget()->SetResolution(1.0);
    this->RotationAngleZ->SetBalloonHelpString("set rotation angle on Z axis.");
    this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2", this->RotationAngleZ->GetWidgetName(), RotationFrame->GetFrame()->GetWidgetName());
    
    
    // related to rotations    // adding at 09. 9. 14 - smkim
    this->TranslationX = vtkKWScaleWithLabel::New();
    this->TranslationX->SetParent ( TranslationFrame->GetFrame() );
    this->TranslationX->Create ( );
    this->TranslationX->SetLabelText("Translation X");
    this->TranslationX->GetWidget()->SetRange(-100, 100);
    this->TranslationX->GetWidget()->SetResolution(1.0);
    this->TranslationX->SetBalloonHelpString("set translation on X axis.");
    this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2", this->TranslationX->GetWidgetName(), TranslationFrame->GetFrame()->GetWidgetName());
    
    this->TranslationY = vtkKWScaleWithLabel::New();
    this->TranslationY->SetParent ( TranslationFrame->GetFrame() );
    this->TranslationY->Create ( );
    this->TranslationY->SetLabelText("Translation Y");
    this->TranslationY->GetWidget()->SetRange(-100, 100);
    this->TranslationY->GetWidget()->SetResolution(1.0);
    this->TranslationY->SetBalloonHelpString("set translation on Y axis.");
    this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2", this->TranslationY->GetWidgetName(), TranslationFrame->GetFrame()->GetWidgetName());
    
    this->TranslationZ = vtkKWScaleWithLabel::New();
    this->TranslationZ->SetParent ( TranslationFrame->GetFrame() );
    this->TranslationZ->Create ( );
    this->TranslationZ->SetLabelText("Translation Z");
    this->TranslationZ->GetWidget()->SetRange(-1000, 1000);
    this->TranslationZ->GetWidget()->SetResolution(1.0);
    this->TranslationZ->SetBalloonHelpString("set translation on Z axis.");
    this->Script ( "pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2", this->TranslationZ->GetWidgetName(), TranslationFrame->GetFrame()->GetWidgetName());
    
   

    conBrowsFrame->Delete();
    RotationFrame->Delete();
    TranslationFrame->Delete();

}

//---------------------------------------------------------------------------
// adding for volume information frame gui
// adding at 09. 9. 2 - smkim
void vtkSecondaryWindowWithOpenCVGUI::BuildGUIForVolumeInfoFrame ()
{
    // 5/6/2010 ayamada
    vtkKWWidget *page = this->UIPanel->GetPageWidget ("SecondaryWindowWithOpenCV");
    
    vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();
    
    conBrowsFrame->SetParent(page);
    conBrowsFrame->Create();
    conBrowsFrame->SetLabelText("Volume Information");
    //conBrowsFrame->CollapseFrame();
    this->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", conBrowsFrame->GetWidgetName(), page->GetWidgetName());
    
    // -----------------------------------------
    // Test child frame
    
    this->VolumeHeaderWidget = vtkSlicerVolumeHeaderWidget::New();
    this->VolumeHeaderWidget->AddNodeSelectorWidgetOff();
    this->VolumeHeaderWidget->SetMRMLScene(this->GetMRMLScene());
    this->VolumeHeaderWidget->SetParent(conBrowsFrame->GetFrame());
    this->VolumeHeaderWidget->Create();
    this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", this->VolumeHeaderWidget->GetWidgetName(), conBrowsFrame->GetFrame()->GetWidgetName());
    
    
    conBrowsFrame->Delete();
    
    
}

//---------------------------------------------------------------------------
// adding for volume transformation frame gui
// adding at 09. 9. 4 - smkim
void vtkSecondaryWindowWithOpenCVGUI::BuildGUIForTransformation ()
{
    // 5/6/2010 ayamada
    vtkKWWidget *page = this->UIPanel->GetPageWidget ("SecondaryWindowWithOpenCV");
    
    vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();
    
    conBrowsFrame->SetParent(page);
    conBrowsFrame->Create();
    conBrowsFrame->SetLabelText("Volume Transformation");
    //conBrowsFrame->CollapseFrame();
    this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", conBrowsFrame->GetWidgetName(), page->GetWidgetName());
    
    // -----------------------------------------
    // Test child frame
    
    this->TransformEditorWidget = vtkSlicerTransformEditorWidget::New ( );
    this->TransformEditorWidget->SetAndObserveMRMLScene(this->GetMRMLScene() );
    this->TransformEditorWidget->SetParent ( conBrowsFrame->GetFrame() );
    this->TransformEditorWidget->Create ( );
    this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", this->TransformEditorWidget->GetWidgetName(), conBrowsFrame->GetFrame()->GetWidgetName());
    
    conBrowsFrame->Delete();
    
}

// to make node selector frame   // adding at 09. 8. 19 - smkim
void vtkSecondaryWindowWithOpenCVGUI::BuildGUIForNodeSelectorFrame()
{
    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    // 5/6/2010 ayamada
    vtkKWWidget *page = this->UIPanel->GetPageWidget ("SecondaryWindowWithOpenCV");
    
    vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();
    
    conBrowsFrame->SetParent(page);
    conBrowsFrame->Create();
    conBrowsFrame->SetLabelText("Volume Node Selctor");
    //conBrowsFrame->CollapseFrame();
    app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", conBrowsFrame->GetWidgetName(), page->GetWidgetName());
    
    // -----------------------------------------
    
    //NodeSelector for Node from MRML Scene    //adding at 09. 8. 19 - smkim 
    this->NS_ImageData=vtkSlicerNodeSelectorWidget::New();
    this->NS_ImageData->SetParent( conBrowsFrame->GetFrame() );
    this->NS_ImageData->Create();
    this->NS_ImageData->SetNodeClass("vtkMRMLVolumeNode", NULL, NULL, NULL);
    this->NS_ImageData->SetMRMLScene(this->GetMRMLScene());
    this->NS_ImageData->SetBorderWidth(2);
    this->NS_ImageData->GetWidget()->GetWidget()->IndicatorVisibilityOff();
    this->NS_ImageData->GetWidget()->GetWidget()->SetWidth(24);
    this->NS_ImageData->SetLabelText( "Active Volume: ");
    this->NS_ImageData->SetBalloonHelpString("Select a volume from the current scene.");
    app->Script("pack %s -side top -fill x -anchor nw -padx 2 -pady 2", this->NS_ImageData->GetWidgetName());
    
    
    conBrowsFrame->Delete();
    
    
}

//---------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::BuildGUIForCaptureCameraImageFrame()
{
    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
    // 5/6/2010 ayamada
    vtkKWWidget *page = this->UIPanel->GetPageWidget ("SecondaryWindowWithOpenCV");
    
    vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();
    
    conBrowsFrame->SetParent(page);
    conBrowsFrame->Create();
    conBrowsFrame->SetLabelText("Camera Calibration Frame");
    //conBrowsFrame->CollapseFrame();
    app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s", conBrowsFrame->GetWidgetName(), page->GetWidgetName());
    
    // -----------------------------------------
    // Test child frame
    
    vtkKWFrameWithLabel *cameraCalibrationframe = vtkKWFrameWithLabel::New();
    cameraCalibrationframe->SetParent(conBrowsFrame->GetFrame());
    cameraCalibrationframe->Create();
    cameraCalibrationframe->SetLabelText ("Camera Calibrattion Matrices Loader");
    this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", cameraCalibrationframe->GetWidgetName() );
    
    
    
    vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
    frame->SetParent(conBrowsFrame->GetFrame());
    frame->Create();
    frame->SetLabelText ("Secondary Window Control");

    // 6/22/2010 ayamada
    vtkKWFrameWithLabel *frame2 = vtkKWFrameWithLabel::New();
    frame2->SetParent(conBrowsFrame->GetFrame());
    frame2->Create();
    frame2->SetLabelText ("Secondary Window Mode");
    
    this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", frame->GetWidgetName() );
    this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", frame2->GetWidgetName() );

    // --------------------------------------------
    // secondaey window mode control check box
    // 6/22/2010 ayamada
    this->singleWindowCheckButton = vtkKWCheckButton::New();
    this->singleWindowCheckButton->SetParent(frame2->GetFrame());
    this->singleWindowCheckButton->Create();
    this->singleWindowCheckButton->SelectedStateOn();
    this->singleWindowCheckButton->SetText("For Single Camera");
    
    this->stereoWindowCheckButton = vtkKWCheckButton::New();
    this->stereoWindowCheckButton->SetParent(frame2->GetFrame());
    this->stereoWindowCheckButton->Create();
    this->stereoWindowCheckButton->SelectedStateOff();
    this->stereoWindowCheckButton->SetText("For Stereo Camera");
        
    this->Script("pack %s %s -side left -padx 2 -pady 2", 
                 this->singleWindowCheckButton->GetWidgetName(),
                 this->stereoWindowCheckButton->GetWidgetName());  

    // -----------------------------------------
    // Test push button
    
    // add a file browser for loading lens distortion coefficient        adding at 10. 2. 23 - smkim
    this->loadLensDistortionCoefButton = vtkKWLoadSaveButtonWithLabel::New ( );
    this->loadLensDistortionCoefButton->SetParent ( cameraCalibrationframe->GetFrame() );
    this->loadLensDistortionCoefButton->Create ( );
    this->loadLensDistortionCoefButton->SetWidth(50);
    this->loadLensDistortionCoefButton->GetWidget()->SetText ("Load Lens distortion Coef.");
    this->loadLensDistortionCoefButton->GetWidget()->GetLoadSaveDialog()->SetTitle("Lens Distortion Coef.");
    // 6/21/2010 ayamada
    this->loadLensDistortionCoefButton->SetLabelText("Single/Left:");


    // 6/10/2010 ayamada
    //this->loadLensDistortionCoefButton->GetWidget()->GetLoadSaveDialog()->SetFileTypes("{ {data} {*.dat} }");
    this->loadLensDistortionCoefButton->GetWidget()->GetLoadSaveDialog()->SetFileTypes("{ {xml} {*.xml} }");
    this->loadLensDistortionCoefButton->GetWidget()->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    app->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
                this->loadLensDistortionCoefButton->GetWidgetName());
    
    // add a file browser for loading Intrinsic parameters of camera        adding at 10. 2. 23 - smkim
    this->loadIntrinsicParameterButton = vtkKWLoadSaveButtonWithLabel::New ( );
    this->loadIntrinsicParameterButton->SetParent ( cameraCalibrationframe->GetFrame() );
    this->loadIntrinsicParameterButton->Create ( );
    this->loadIntrinsicParameterButton->SetWidth(50);
    this->loadIntrinsicParameterButton->GetWidget()->SetText ("Load Intrinsic Parameters");
    this->loadIntrinsicParameterButton->GetWidget()->GetLoadSaveDialog()->SetTitle("Intrinsic Parameters");
    // 6/21/2010 ayamada
    this->loadIntrinsicParameterButton->SetLabelText("Single/Left:");

    // 6/10/2010 ayamada
    //this->loadIntrinsicParameterButton->GetWidget()->GetLoadSaveDialog()->SetFileTypes("{ {data} {*.dat} }");
    this->loadIntrinsicParameterButton->GetWidget()->GetLoadSaveDialog()->SetFileTypes("{ {xml} {*.xml} }");
    this->loadIntrinsicParameterButton->GetWidget()->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    app->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
                this->loadIntrinsicParameterButton->GetWidgetName());

    // 6/21/2010 ayamada
    this->loadLensDistortionCoefButton2 = vtkKWLoadSaveButtonWithLabel::New ( );
    this->loadLensDistortionCoefButton2->SetParent ( cameraCalibrationframe->GetFrame() );
    this->loadLensDistortionCoefButton2->Create ( );
    this->loadLensDistortionCoefButton2->SetWidth(50);
    this->loadLensDistortionCoefButton2->GetWidget()->SetText ("Load Lens distortion Coef.");
    this->loadLensDistortionCoefButton2->GetWidget()->GetLoadSaveDialog()->SetTitle("Lens Distortion Coef.");
    this->loadLensDistortionCoefButton2->SetLabelText("        Right:");

    this->loadLensDistortionCoefButton2->GetWidget()->GetLoadSaveDialog()->SetFileTypes("{ {xml} {*.xml} }");
    this->loadLensDistortionCoefButton2->GetWidget()->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    app->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
            this->loadLensDistortionCoefButton2->GetWidgetName());


    // 6/21/2010 ayamada
    this->loadIntrinsicParameterButton2 = vtkKWLoadSaveButtonWithLabel::New ( );
    this->loadIntrinsicParameterButton2->SetParent ( cameraCalibrationframe->GetFrame() );
    this->loadIntrinsicParameterButton2->Create ( );
    this->loadIntrinsicParameterButton2->SetWidth(50);
    this->loadIntrinsicParameterButton2->GetWidget()->SetText ("Load Intrinsic Parameters");
    this->loadIntrinsicParameterButton2->GetWidget()->GetLoadSaveDialog()->SetTitle("Intrinsic Parameters");
    this->loadIntrinsicParameterButton2->SetLabelText("        Right:");

    this->loadIntrinsicParameterButton2->GetWidget()->GetLoadSaveDialog()->SetFileTypes("{ {xml} {*.xml} }");
    this->loadIntrinsicParameterButton2->GetWidget()->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");
    app->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", 
                this->loadIntrinsicParameterButton2->GetWidgetName());

    
    this->showCaptureData = vtkKWPushButton::New ( );
    this->showCaptureData->SetParent ( frame->GetFrame() );
    this->showCaptureData->Create ( );
    this->showCaptureData->SetText ("Secondary Window On");
    this->showCaptureData->SetWidth (24);
    
    this->closeCaptureData = vtkKWPushButton::New ( );
    this->closeCaptureData->SetParent ( frame->GetFrame() );
    this->closeCaptureData->Create ( );
    this->closeCaptureData->SetText ("Camera Connection/Disconnection");
    //    this->closeCaptureData->SetText ("MRML observer on");
    this->closeCaptureData->SetWidth (35);
    
//    this->Script("pack %s %s -side left -padx 2 -pady 2", this->showCaptureData->GetWidgetName(), this->closeCaptureData->GetWidgetName());
    // 5/16/2010 ayamada
    this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", this->showCaptureData->GetWidgetName());
    this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", this->closeCaptureData->GetWidgetName());

    // 5/17/2010 ayamada    
    // add file browser for saving camera images
/*
    this->saveCameraImageButton = vtkKWLoadSaveButtonWithLabel::New ( );
    this->saveCameraImageButton =vtkKWSaveImageDialog::New ( );
    this->saveCameraImageButton->SetParent ( frame->GetFrame() );
    this->saveCameraImageButton->Create ( );
    this->saveCameraImageButton->SetWidth(50);

    this->saveCameraImageButton->GetWidget()->SetText ("Save Path of Captured Camera Images");
    this->saveCameraImageButton->GetWidget()->GetLoadSaveDialog()->SetTitle("Save Path of Captured Camera Images");
    this->saveCameraImageButton->GetWidget()->GetLoadSaveDialog()->SetFileTypes("{ {All Files} {*} }");
    this->saveCameraImageButton->GetWidget()->GetLoadSaveDialog()->RetrieveLastPathFromRegistry("OpenPath");       

    // 5/17/2010 ayamada
    this->saveCameraImageButton->GetWidget()->GetLoadSaveDialog()->SetInitialFileName("CapturedImage");
    this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", this->saveCameraImageButton->GetWidgetName());
*/
    
    // 5/17/2010 ayamada
    this->saveCameraImageEntry = vtkKWEntryWithLabel::New();
    this->saveCameraImageEntry->SetParent(frame->GetFrame());
    this->saveCameraImageEntry->Create();
    this->saveCameraImageEntry->SetWidth(40);
    this->saveCameraImageEntry->SetLabelWidth(30);
    this->saveCameraImageEntry->SetLabelText("Save Path of Snapshot:");
    //this->saveCameraImageEntry->GetWidget()->SetValue ( "" );
    this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", this->saveCameraImageEntry->GetWidgetName());
    
    // 5/17/2010 ayamada
    this->captureCameraImage = vtkKWPushButton::New ( );
    this->captureCameraImage->SetParent ( frame->GetFrame() );
    this->captureCameraImage->Create ( );
    this->captureCameraImage->SetText ("Get Snapshot");
    this->captureCameraImage->SetWidth (35);    
    this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2", this->captureCameraImage->GetWidgetName());
    
    conBrowsFrame->Delete();
    cameraCalibrationframe->Delete();
    frame->Delete();
    
}


//----------------------------------------------------------------------------
void vtkSecondaryWindowWithOpenCVGUI::UpdateAll()
{
}

//to update frames from MRML of Slicer3        at 09. 8. 6 - smkim
void vtkSecondaryWindowWithOpenCVGUI::UpdateFramesFromMRML()
{
    
    //to get VolumeNode
    vtkMRMLVolumeNode *refNode = vtkMRMLVolumeNode::SafeDownCast(this->NS_ImageData->GetSelected());
    
    // Update Volume Header and Display Widget
    // TODO: this may not be needed once the parts above are doing the right things
    if (refNode != NULL)
    {
        this->VolumeHeaderWidget->SetVolumeNode(refNode);
        this->VolumeHeaderWidget->UpdateWidgetFromMRML();
        
        // 5/6/2010 ayamada
        this->volumenode = vtkMRMLVolumeNode::SafeDownCast(this->NS_ImageData->GetSelected());
        
    }
    
}

//---------------------------------------------------------------------------
// Getting position data from OpenIGTLinkIF module
// adding at 09. 9. 16 - smkim
void vtkSecondaryWindowWithOpenCVGUI::GetCurrentPosition(double *px, double *py, double *pz)
{
    *px = 0.0;
    *py = 0.0;
    *pz = 0.0;
    
    this->OriginalTrackerNode = vtkMRMLLinearTransformNode::SafeDownCast(this->TransformEditorWidget->GetTransformEditSelectorWidget()->GetSelected());    //adding at 09. 12. 24. - smkim
    
    if (! this->OriginalTrackerNode)
    {
        return;
    }
    
    vtkMatrix4x4* transform;
    //transform = transformNode->GetMatrixTransformToParent();
    transform = this->OriginalTrackerNode->GetMatrixTransformToParent();
    
    if (transform)
    {
        // set volume orientation
        *px = transform->GetElement(0, 3);
        *py = transform->GetElement(1, 3);
        *pz = transform->GetElement(2, 3);
    }
}

void vtkSecondaryWindowWithOpenCVGUI::UpdateTransformNodeByName(const char *name)
{
    if (name)
    {
        this->SetTransformNodeName(name);
        
        vtkMRMLScene* scene = this->GetApplicationLogic()->GetMRMLScene();
        vtkCollection* collection = scene->GetNodesByName(this->TransformNodeName);
        
        if (collection != NULL && collection->GetNumberOfItems() == 0)
        {
            // the node name does not exist in the MRML tree
            return;
        }
        
        this->OriginalTrackerNode = vtkMRMLLinearTransformNode::SafeDownCast(collection->GetItemAsObject(0));
        //if (this->Pat2ImgReg && this->UseRegistration)
        //  {
        //  this->UpdateLocatorTransform();
        //  }
        
    }
    
}

//---------------------------------------------------------------------------
// Estimating sample distances
// adding at 09. 8. 20 - smkim
double vtkSecondaryWindowWithOpenCVGUI::EstimateSampleDistances(void)
{
    
    //to get the active volume node     // adding at 09. 8. 20 - smkim
    vtkImageData *volumedata = volumenode->GetImageData();   // adding at 09. 8. 20 - smkim
    double* spacing = volumedata->GetSpacing();
    
    double minSpace = spacing[0];
    double maxSpace = spacing[0];
    
    for(int i = 1; i < 3; i++)
    {
        if (spacing[i] > maxSpace)
            maxSpace = spacing[i];
        if (spacing[i] < minSpace)
            minSpace = spacing[i];
    }
    
    return minSpace * 0.5f;
    
}

//---------------------------------------------------------------------------
// Estimating intensity range
// adding at 09. 8. 20 - smkim
double* vtkSecondaryWindowWithOpenCVGUI::EstimateScalarRange(void)
{
    //to get the active volume node     // adding at 09. 8. 20 - smkim
    vtkImageData *volumedata = volumenode->GetImageData();   // adding at 09. 8. 20 - smkim
    
    double scalarRange[2];
    
    volumedata->GetPointData()->GetScalars()->GetRange(scalarRange, 0);
    
    return scalarRange;
}

//**************************************************************************
//  Volume Rendering with CUDA Redering Method
//**************************************************************************
// adding at 09. 8. 26 - smkim
void vtkSecondaryWindowWithOpenCVGUI::vtkCUDAVolumeRender()
{
    
    
    //-------------------------------------------------------------
    //to add MRML node on Slicer3
    //adding at 09. 9. 4 - smkim
    vtkMRMLModelNode           *locatorModel;
    vtkMRMLModelDisplayNode    *locatorDisp;
    
    
    locatorModel = vtkMRMLModelNode::New();
    locatorDisp = vtkMRMLModelDisplayNode::New();
    
    this->GetMRMLScene()->SaveStateForUndo();
    this->GetMRMLScene()->AddNode(locatorDisp);
    this->GetMRMLScene()->AddNode(locatorModel);  
    
    locatorDisp->SetScene(this->GetMRMLScene());
    
    locatorModel->SetName( "Volume on Camera Image" );
    locatorModel->SetScene(this->GetMRMLScene());
    locatorModel->SetAndObserveDisplayNodeID(locatorDisp->GetID());
    locatorModel->SetHideFromEditors(0);
    
    
    //--------------------------------------------------------------------------------------------------------------
    //to get the active volume node     
    // adding at 09. 8. 20 - smkim
    vtkImageData *volumedata = volumenode->GetImageData();   // adding at 09. 8. 20 - smkim
    
    
    //----------------------------------------------------------------------------------------------------------------
    //to get the properties of volume data   
    // adding at 09. 9. 3 - smkim
    int* dimension = volumedata->GetDimensions();                 //getting volume dimensions
    double* spacing = volumedata->GetSpacing();                   //getting volume spacing: dimensions of resolution
    volumedata->SetOrigin( -dimension[0]*spacing[0]/2.0, -dimension[1]*spacing[1]/2.0, -dimension[2]*spacing[2]/2.0  );   //setting up the origin of volume data
    
    //getting scalar range of volume data
    double scalarRange[2];
    volumedata->GetPointData()->GetScalars()->GetRange(scalarRange);
    double RangeofScalar = scalarRange[1] - scalarRange[0];
    
    vtkThreshold *thresh = vtkThreshold::New();
    thresh->ThresholdByUpper(0);
    thresh->AllScalarsOff();
    thresh->SetInput( volumedata  );                                  // if volume data is obtained from volume 
    vtkDataSetTriangleFilter *trifilter = vtkDataSetTriangleFilter::New();
    trifilter->SetInputConnection(thresh->GetOutputPort());
    
    vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint(scalarRange[0], 0.0);
    opacityTransferFunction->AddPoint(scalarRange[0] + RangeofScalar * 0.31, 0.0);
    opacityTransferFunction->AddPoint(scalarRange[0] + RangeofScalar * 0.5, 0.2);
    opacityTransferFunction->AddPoint(scalarRange[1], 0.2);
    
    vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint(scalarRange[0], 0.3, 0.3, 1.0);
    colorTransferFunction->AddRGBPoint(scalarRange[0] + RangeofScalar * 0.47, 0.0, 0.0, 1.0);
    colorTransferFunction->AddRGBPoint(scalarRange[0] + RangeofScalar * 0.63, 1.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(scalarRange[0] + RangeofScalar * 0.78, 0.0, 1.0, 0.0);
    colorTransferFunction->AddRGBPoint(scalarRange[1], 1.0, 0.3, 0.3);
    
    volumeProperty = vtkVolumeProperty::New();
    volumeProperty->SetColor( colorTransferFunction );
    volumeProperty->SetScalarOpacity( opacityTransferFunction );
    volumeProperty->ShadeOff();
    volumeProperty->SetInterpolationTypeToLinear();
    
    
    vtkCudaVolumeMapper *CudaVolumeMapper = vtkCudaVolumeMapper::New();
    //CudaVolumeMapper->SetInputConnection( trifilter->GetOutputPort() );    
    CudaVolumeMapper->SetInput( volumedata );
    CudaVolumeMapper->SetIntendedFrameRate(10.0);
    volume->SetMapper( CudaVolumeMapper );
    volume->SetProperty( volumeProperty );
    
    // 5/8/2010 ayamada
    //ren->AddVolume( volume );
    this->SecondaryViewerWindow->rw->GetRenderer()->AddVolume( volume );
    // 6/23/2010 ayamada
    this->SecondaryViewerWindow2->lw->GetRenderer()->AddVolume( volume );
    
}

//**************************************************************************
//  Volume Rendering with Texture Mapping Method
//**************************************************************************
// adding at 09. 8. 25 - smkim
void vtkSecondaryWindowWithOpenCVGUI::vtkTexture3DVolumeRender()
{
    
    if(!this->m_bVolumeRendering)
    {
        //-------------------------------------------------------------
        //to add MRML node on Slicer3
        //adding at 09. 9. 4 - smkim
         vtkMRMLModelNode           *ModelNode;
        vtkMRMLModelDisplayNode    *DispNode;
        
        
        ModelNode = vtkMRMLModelNode::New();
        DispNode = vtkMRMLModelDisplayNode::New();
        
        this->GetMRMLScene()->SaveStateForUndo();
        this->GetMRMLScene()->AddNode(DispNode);
        this->GetMRMLScene()->AddNode(ModelNode);  
        
        DispNode->SetScene(this->GetMRMLScene());
        
        
        ModelNode->SetName( "Volume on Camera Image" );
        ModelNode->SetScene(this->GetMRMLScene());
        ModelNode->SetAndObserveDisplayNodeID(DispNode->GetID());
        ModelNode->SetHideFromEditors(0);
    }
    
    /*        //to add MRML node on Slicer3
     //adding at 09. 9. 4 - smkim
     //        vtkMRMLModelNode           *locatorModel;
     vtkMRMLModelDisplayNode    *locatorDisp;
     
     
     //    locatorModel = vtkMRMLModelNode::New();
     locatorDisp = vtkMRMLModelDisplayNode::New();
     
     this->GetMRMLScene()->SaveStateForUndo();
     this->GetMRMLScene()->AddNode(locatorDisp);
     this->GetMRMLScene()->AddNode(volumenode);  
     
     locatorDisp->SetScene(this->GetMRMLScene());
     
     volumenode->SetName( "Volume on Camera" );
     volumenode->SetScene(this->GetMRMLScene());
     volumenode->SetAndObserveDisplayNodeID(locatorDisp->GetID());
     volumenode->SetHideFromEditors(0);
     */    // comment out because of some problem related to change segmented model    // comment out at 09. 9. 16 - smkim
    
    //--------------------------------------------------------------------------------------------------------------
    //to get the active volume node     
    // adding at 09. 8. 20 - smkim
    vtkImageData *volumedata = volumenode->GetImageData();   // adding at 09. 8. 20 - smkim
    
    
    //----------------------------------------------------------------------------------------------------------------
    //to get the properties of volume data   
    // adding at 09. 9. 3 - smkim
    int* dimension = volumedata->GetDimensions();        // getting volume dimensions
    double* spacing = volumenode->GetSpacing();        // getting volume spacing: dimensions of resolution // getting from selected volumenode // modifying at 09. 10. 16 - smkim
    double* origin = volumenode->GetOrigin();        // getting volume origin     // adding at 09. 10. 20 - smkim
    
    // setting volume center & volume spacing
    volumedata->SetOrigin( -dimension[0]*spacing[0]/2.0, -dimension[1]*spacing[1]/2.0, -dimension[2]*spacing[2]/2.0 );    // setting origin of volume // adding at 09. 10. 20 - smkim
    volumedata->SetSpacing( spacing[0], spacing[1], spacing[2] );                                // setting spacing of volume // adding at 09. 10. 20 - smkim
    
    // adding for debugging at 09. 10. 16 - smkim
    std::cerr << "dimension \t" << dimension[0] << "\t" << dimension[1] << "\t" << dimension[2] << std::endl;
    std::cerr << "spacing \t" << spacing[0] << "\t" << spacing[1] << "\t" << spacing[2] << std::endl;
    std::cerr << "origin \t" << origin[0] << "\t" << origin[1] << "\t" << origin[2] << std::endl;        // adding at 09. 10. 20 - smkim
    
    //getting scalar range of volume data
    double scalarRange[2];
    volumedata->GetPointData()->GetScalars()->GetRange(scalarRange);
    double RangeofScalar = scalarRange[1] - scalarRange[0];
    
    /*    // volume transformation 
     // adding at 09. 10. 16 - smkim
     vtkTransform *transform = vtkTransform::New();
     transform->Scale( 1.0, 1.0, spacing[0]/spacing[2] );
     //transform->Translate( origin[0], origin[1], -origin[2] );
     
     // reslicing 
     // adding at 09. 10. 16 - smkim
     vtkImageReslice *reslice = vtkImageReslice::New();
     reslice->SetInput( volumedata );
     reslice->SetResliceTransform( transform );
     reslice->SetInterpolationModeToCubic();
     reslice->WrapOn();
     reslice->AutoCropOutputOn();
     */
    vtkThreshold *thresh = vtkThreshold::New();
    thresh->ThresholdByUpper(0);
    thresh->AllScalarsOff();
    thresh->SetInput( volumedata  );                                  // if volume data is obtained from volume 
    //thresh->SetInputConnection( reslice->GetOutputPort() );
    vtkDataSetTriangleFilter *trifilter = vtkDataSetTriangleFilter::New();
    trifilter->SetInputConnection(thresh->GetOutputPort());
    
    /*    vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
     opacityTransferFunction->AddPoint(scalarRange[0], 0.0);
     opacityTransferFunction->AddPoint(scalarRange[0] + RangeofScalar * 0.31, 0.0);
     opacityTransferFunction->AddPoint(scalarRange[0] + RangeofScalar * 0.5, 0.2);
     opacityTransferFunction->AddPoint(scalarRange[1], 0.2);
     */
    
    // to display 4D ultrasound imaging 
    // at 09. 9. 17 - smkim
    vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
    /*    opacityTransferFunction->AddPoint(scalarRange[0], 0.0);
     opacityTransferFunction->AddPoint(74.551, 0.0);
     opacityTransferFunction->AddPoint(112.67, 0.4);
     opacityTransferFunction->AddPoint(scalarRange[1], 0.4);
     */    
    ////comment at 10. 01. 06 - smkim
    //opacityTransferFunction->AddPoint(0.0, 0.0);
    //opacityTransferFunction->AddPoint(508.45, 0.0);
    //opacityTransferFunction->AddPoint(5337.3, 0.25);
    //opacityTransferFunction->AddPoint(27783, 0.25);
    
    opacityTransferFunction->AddPoint(scalarRange[0], 0.0);
    opacityTransferFunction->AddPoint(0.01, 0.0);
    //opacityTransferFunction->AddPoint(0.01, 1.0);    
    opacityTransferFunction->AddPoint(scalarRange[1], 0.45);
    
    /*    opacityTransferFunction->AddPoint(0.0, 0.0);
     opacityTransferFunction->AddPoint(0.01, 0.0);
     opacityTransferFunction->AddPoint(0.01, 1.0);
     opacityTransferFunction->AddPoint(27783, 1.0);
     */
    vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint(scalarRange[0], 0.3, 0.3, 1.0);
    colorTransferFunction->AddRGBPoint(scalarRange[0] + RangeofScalar * 0.47, 0.0, 0.0, 1.0);
    colorTransferFunction->AddRGBPoint(scalarRange[0] + RangeofScalar * 0.63, 1.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(scalarRange[0] + RangeofScalar * 0.78, 0.0, 1.0, 0.0);
    colorTransferFunction->AddRGBPoint(scalarRange[1], 1.0, 0.3, 0.3);
    
    volumeProperty = vtkVolumeProperty::New();
    volumeProperty->SetColor( colorTransferFunction );
    volumeProperty->SetScalarOpacity( opacityTransferFunction );
    volumeProperty->ShadeOff();
    volumeProperty->SetInterpolationTypeToLinear();
    
    
    vtkSlicerVolumeTextureMapper3D2 *MapperTexture = vtkSlicerVolumeTextureMapper3D2::New();
    //    MapperTexture->SetInputConnection( reslice->GetOutputPort() );
    MapperTexture->SetInput( volumedata );
    //    MapperTexture->SetSampleDistance( EstimateSampleDistances() );
    volume->SetMapper( MapperTexture );
    volume->SetProperty( volumeProperty );
    //    volume->SetOrientation(0.0, 0.0, 180.0);    //using for data taken with axial direction - smkim
    //    volume->SetOrientation(-90.0, 0.0, 180.0);        //using for data taken with sagittal direction - smkim
    //volume->SetOrientation(-90.0, 180.0, 90.0);        //using for data taken with sagittal direction (CARS Phantom) 10. 01. 06 - smkim    

    // 5/30/2010 ayamada
    // warning: this is only for the initial figure
    volume->SetOrientation(0.0, 0.0, -180.0);
    
    
    if( !this->m_bVolumeRendering )
        this->m_bVolumeRendering = true;    //setting the flag of volume rendering up    // adding at 10. 02. 01 - smkim
    
    // 5/8/2010 ayamada
    //ren->AddVolume( volume );
    this->SecondaryViewerWindow->rw->GetRenderer()->AddVolume( volume );

    // 6/23/2010 ayamada
    this->SecondaryViewerWindow->rw->GetRenderer()->AddVolume( volume );
    
    
}


//**************************************************************************
//  Volume Rendering with software Ray Casting Method
//**************************************************************************
// adding at 09. 8. 18 - smkim
void vtkSecondaryWindowWithOpenCVGUI::vtkRayCastingVolumeRender()
{
    
    //-------------------------------------------------------------
    //to add MRML node on Slicer3
    //adding at 09. 9. 4 - smkim
    vtkMRMLModelNode           *locatorModel;
    vtkMRMLModelDisplayNode    *locatorDisp;
    
    
    locatorModel = vtkMRMLModelNode::New();
    locatorDisp = vtkMRMLModelDisplayNode::New();
    
    this->GetMRMLScene()->SaveStateForUndo();
    this->GetMRMLScene()->AddNode(locatorDisp);
    this->GetMRMLScene()->AddNode(locatorModel);  
    
    locatorDisp->SetScene(this->GetMRMLScene());
    
    locatorModel->SetName( "Volume on Camera Image" );
    locatorModel->SetScene(this->GetMRMLScene());
    locatorModel->SetAndObserveDisplayNodeID(locatorDisp->GetID());
    locatorModel->SetHideFromEditors(0);
    
    //--------------------------------------------------------------------------------------------------------------
    //to get the active volume node     
    // adding at 09. 8. 20 - smkim
    vtkImageData *volumedata = volumenode->GetImageData();   // adding at 09. 8. 20 - smkim
    
    
    //----------------------------------------------------------------------------------------------------------------
    //to get the properties of volume data   
    // adding at 09. 9. 3 - smkim
    int* dimension = volumedata->GetDimensions();                 //getting volume dimensions
    double* spacing = volumedata->GetSpacing();                   //getting volume spacing: dimensions of resolution
    volumedata->SetOrigin( -dimension[0]*spacing[0]/2.0, -dimension[1]*spacing[1]/2.0, -dimension[2]*spacing[2]/2.0  );   //setting up the origin of volume data
    
    //getting scalar range of volume data
    double scalarRange[2];
    volumedata->GetPointData()->GetScalars()->GetRange(scalarRange);
    double RangeofScalar = scalarRange[1] - scalarRange[0];
    
    
    
    
    vtkThreshold *thresh = vtkThreshold::New();
    thresh->ThresholdByUpper(0);
    thresh->AllScalarsOff();
    thresh->SetInput( volumedata  );                                  // if volume data is obtained from volume 
    vtkDataSetTriangleFilter *trifilter = vtkDataSetTriangleFilter::New();
    trifilter->SetInputConnection(thresh->GetOutputPort());
    
    vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint(scalarRange[0], 0.0);
    opacityTransferFunction->AddPoint(scalarRange[0] + RangeofScalar * 0.31, 0.0);
    opacityTransferFunction->AddPoint(scalarRange[0] + RangeofScalar * 0.5, 0.2);
    opacityTransferFunction->AddPoint(scalarRange[1], 0.2);
    
    vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint(scalarRange[0], 0.3, 0.3, 1.0);
    colorTransferFunction->AddRGBPoint(scalarRange[0] + RangeofScalar * 0.47, 0.0, 0.0, 1.0);
    colorTransferFunction->AddRGBPoint(scalarRange[0] + RangeofScalar * 0.63, 1.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(scalarRange[0] + RangeofScalar * 0.78, 0.0, 1.0, 0.0);
    colorTransferFunction->AddRGBPoint(scalarRange[1], 1.0, 0.3, 0.3);
    
    volumeProperty = vtkVolumeProperty::New();
    volumeProperty->SetColor( colorTransferFunction );
    volumeProperty->SetScalarOpacity( opacityTransferFunction );
    volumeProperty->ShadeOff();
    volumeProperty->SetInterpolationTypeToLinear();
    
    
    vtkUnstructuredGridVolumeRayCastMapper *volumeMapper = vtkUnstructuredGridVolumeRayCastMapper::New();
    volumeMapper->SetInputConnection( trifilter->GetOutputPort() );
    volume->SetMapper( volumeMapper );
    volume->SetProperty( volumeProperty );
    
    // 5/7/2010 ayamada
    //ren->AddVolume( volume );
    this->SecondaryViewerWindow->rw->GetRenderer()->AddVolume( volume );

    // 6/23/2010 ayamada
    this->SecondaryViewerWindow2->lw->GetRenderer()->AddVolume( volume );
    
    
}

// 6/23/2010 ayamada
void vtkSecondaryWindowWithOpenCVGUI::ConfigurationOfSecondaryWindow(int i){
    
    if(i==1){
        this->SecondaryViewerWindow->SetTitle ("3D Slicer -- Secondary Window -- ");
        this->SecondaryViewerWindow->changeSecondaryMonitorSize(1280, 480);
        this->SecondaryViewerWindow->Script("place %s -relx 0 -rely 0 -anchor nw -relwidth 0.5 -relheight 0.8", 
                                        this->SecondaryViewerWindow->MainFrame->GetWidgetName() );//10.01.25 ayamada
        this->SecondaryViewerWindow->Script("place %s -relx 0.5 -rely 0.0 -anchor nw -relwidth 0.5 -relheight 1", 
                                        this->SecondaryViewerWindow->rw->GetWidgetName());//10.01.25 ayamada
        this->SecondaryViewerWindow->Script("place %s -relx 0 -rely 0.8 -anchor nw -relwidth 0.5 -relheight 0.2", 
                                        this->SecondaryViewerWindow->rwLeft->GetWidgetName());
        this->SecondaryViewerWindow->Script("place %s -relx 0 -rely 0.0 -anchor nw -relwidth 0.0 -relheight 0.0", 
                                        this->SecondaryViewerWindow->lw->GetWidgetName());
    }else if(i==2){
        
        this->SecondaryViewerWindow->SetTitle ("3D Slicer -- Secondary Window -- ");
        this->SecondaryViewerWindow->changeSecondaryMonitorSize(640, 480);
        
        this->SecondaryViewerWindow->Script("place %s -relx 0 -rely 0 -anchor nw -relwidth 0.0 -relheight 0.0", 
                                            this->SecondaryViewerWindow->MainFrame->GetWidgetName() );//10.01.25 ayamada
        this->SecondaryViewerWindow->Script("place %s -relx 0.0 -rely 0.0 -anchor nw -relwidth 1 -relheight 1", 
                                            this->SecondaryViewerWindow->rw->GetWidgetName());//10.01.25 ayamada
        this->SecondaryViewerWindow->Script("place %s -relx 0 -rely 0.0 -anchor nw -relwidth 0.0 -relheight 0.0", 
                                            this->SecondaryViewerWindow->rwLeft->GetWidgetName());        
        
        this->SecondaryViewerWindow->Script("place %s -relx 0 -rely 0.0 -anchor nw -relwidth 0.0 -relheight 0.0", 
                                            this->SecondaryViewerWindow->lw->GetWidgetName());        
        
        
        //this->SecondaryViewerWindow2->DisplayOnSecondaryMonitor(); // 6/23/2010 ayamada
        
        this->SecondaryViewerWindow2->SetTitle ("3D Slicer -- Secondary Window2 -- ");
        this->SecondaryViewerWindow2->changeSecondaryMonitorSize(640, 480);
        
        this->SecondaryViewerWindow2->Script("place %s -relx 0 -rely 0 -anchor nw -relwidth 0.0 -relheight 0.0", 
                                             this->SecondaryViewerWindow2->MainFrame->GetWidgetName() );//10.01.25 ayamada
        this->SecondaryViewerWindow2->Script("place %s -relx 0.0 -rely 0.0 -anchor nw -relwidth 1 -relheight 1", 
                                             this->SecondaryViewerWindow2->rw->GetWidgetName());//10.01.25 ayamada
        this->SecondaryViewerWindow2->Script("place %s -relx 0 -rely 0.0 -anchor nw -relwidth 0.0 -relheight 0.0", 
                                             this->SecondaryViewerWindow2->rwLeft->GetWidgetName());        
        this->SecondaryViewerWindow2->Script("place %s -relx 0.0 -rely 0.0 -anchor nw -relwidth 0 -relheight 0", 
                                             this->SecondaryViewerWindow2->lw->GetWidgetName());//6/23/2010 ayamada
                
    }
    
}


