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
#ifndef __vtkSecondaryWindowWithOpenCVGUI_h
#define __vtkSecondaryWindowWithOpenCVGUI_h

#ifdef WIN32
#include "vtkSecondaryWindowWithOpenCVWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkSecondaryWindowWithOpenCVLogic.h"
#include "vtkSlicerSecondaryViewerWindow.h"

//----10.01.12-komura
#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <math.h>
#include <stdlib.h>
#include "vtkImageImport.h"
#include "vtkTexture.h"
#include "vtkPlaneSource.h"
#include "vtkImageMapper.h"
#include "vtkImageImport.h"
#include "vtkActor2D.h"
#include "vtkPolyDataMapper.h"
#include "vtkKWRenderWidget.h"

// 10.01.25 ayamada
#include "vtkTextActor3D.h"

// 10.04.23 ayamada
//#include "igtlOSUtil.h"
//#include "igtlImageMessage.h"
//#include "igtlServerSocket.h"
#include <vtkTextProperty.h>
#include <vtkTextSource.h>
#include <vtkTextActor.h>
#include <vtkTextMapper.h>

// 5/16/2010 ayamada
// 5.5.10 ayamada
//#include "igtlOSUtil.h"
//#include "igtlImageMessage.h"
//#include "igtlServerSocket.h"

#include <stdio.h>

// 5/16/2010 ayamada
//#include <unistd.h>
#include "unistd-i.h"

#include <stdlib.h>
//#include <pthread.h>

#include <vtkTextSource.h>
#include "vtkActor.h"

#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataNormals.h>
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkCellArray.h"

#include <vtkTextProperty.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>

#include "vtkImageBlend.h"
#include "vtkVolume16Reader.h"
#include "vtkMetaImageReader.h"
#include "vtkStructuredPointsReader.h"

#include "vtkPNGWriter.h"

#include "vtkContourFilter.h"
#include "vtkWindowToImageFilter.h"
#include "vtkDataSetTriangleFilter.h"
#include "vtkThreshold.h"

#include "vtkPolyDataNormals.h"

#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"

#include "vtkPolyDataMapper.h"
#include "vtkUnstructuredGridVolumeRayCastMapper.h"
#include "vtkFixedPointVolumeRayCastMapper.h"
#include <vtkTextMapper.h>

#include "vtkPolyDataReader.h"        //adding at 10. 02. 01 - smkim
#include "vtkSlicerVolumeTextureMapper3D2.h"     //adding at 09. 8. 20 - smkim
#include "vtkCudaVolumeMapper.h"                //adding at 09. 8. 26 - smkim
#include "vtkSlicerNodeSelectorWidget.h"    //adding at 09. 8. 19 - smkim
#include "vtkMRMLLinearTransformNode.h"    //To get patient coordinates    // adding at 09. 9. 14 - smkim
#include "vtkSlicerTransformEditorWidget.h"     //adding at 09. 12. 24 - smkim
#include "vtkHomogeneousTransform.h"    //adding at 09. 10. 28 - smkim
#include "vtkMath.h"

#include "vtkRenderer.h"

#include "vtkKWScaleWithLabel.h"        //adding at 09. 9. 8 - smkim

#include "vtkSlicerNodeSelectorWidget.h"    //adding at 09. 8. 19 - smkim
#include "vtkSlicerVolumeHeaderWidget.h"        //adding at 09. 9. 2 - smkim
#include "vtkSlicerTransformEditorWidget.h"     //adding at 09. 9. 4 - smkim
#include "vtkSlicerTransformManagerWidget.h"    //adding at 09. 9. 4 - smkim

#include "vtkKWLoadSaveButtonWithLabel.h"    //Adding at 10. 2. 23 - smkim



// 5.5.10 ayamada
// Intrinsic Parameters of Camera
#define FOCAL_LENGTH    900.0 // 5/5/2010 ayamada is it dammy?
//#define FOA        7.36    //Microscope
// 5/5/2010 ayamada FOA is lead at the l.962 in .cxx file.
#define FOA        31.76    //27.0    //31.76    //40.3    //Endoscope 
//#define FOA        45.0    //35.0    //usb camera
//#define Focal_Point_X    (320-299.418431)    //-35.0    //463.686970    //-(463.686979-320.0)    //2.0
//#define Focal_Point_Y    (240-178.543373)    //45.0    //57.813819    //-(57.813819-240.0)    //72.0

#define VIEW_SIZE_X    640.0
#define VIEW_SIZE_Y    480.0

//----
class vtkKWPushButton;
class vtkKWCheckButton;  // 6/22/2010 ayamada

class VTK_SecondaryWindowWithOpenCV_EXPORT vtkSecondaryWindowWithOpenCVGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkSecondaryWindowWithOpenCVGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkSecondaryWindowWithOpenCVLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

    // Adding 10.5.5 ayamada
    vtkSlicerViewerWidget* GetViewerWidget () { return this->ViewerWidget; };
    
    
    //----------------------------------------------------------------
    // New method, Initialization etc.
    //----------------------------------------------------------------
    
    static vtkSecondaryWindowWithOpenCVGUI* New ();
    void Init();
    virtual void Enter ( );
    virtual void Exit ( );
    
    virtual void TearDownGUI();
    
    void PrintSelf (ostream& os, vtkIndent indent );
    
    //----------------------------------------------------------------
    // Observer Management
    //----------------------------------------------------------------
    
    virtual void AddGUIObservers ( );
    virtual void RemoveGUIObservers ( );
    void AddLogicObservers ( );
    void RemoveLogicObservers ( );
    
    //----------------------------------------------------------------
    // Event Handlers
    //----------------------------------------------------------------
    
    virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );
    virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
    virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
    void ProcessTimerEvents();
    void HandleMouseEvent(vtkSlicerInteractorStyle *style);
    static void DataCallback(vtkObject *caller, 
                             unsigned long eid, void *clientData, void *callData);
    
    //----------------------------------------------------------------
    // Build Frames
    //----------------------------------------------------------------
    
    virtual void BuildGUI ( );
    void BuildGUIForHelpFrame();
    void BuildGUIForWindowConfigurationFrame();

    // 5/6/2010 ayamada
    void BuildGUIForNodeSelectorFrame();
    void BuildGUIForCaptureCameraImageFrame();
    void BuildGUIForTransformationFrame();
    void BuildGUIForVolumeInfoFrame();
    void BuildGUIForTransformation();

/*
    void BuildGUIForNavigationFrame();    // adding at 09. 9. 16 - smkim
*/    
    
    //----------------------------------------------------------------
    // Update routines
    //----------------------------------------------------------------
    
    void UpdateAll();
    // 5/6/2010 ayamada
    void UpdateFramesFromMRML();
    
    // 5/16/2010 ayamada
    int closeWindowFlag;

    // 6/13/2010 ayamada
    int undistortionFlag;
    
    // Adding to get patient coordinates...
    // adding at 09. 9. 16 - smkim
    vtkSetStringMacro(TransformNodeName); 
    vtkGetStringMacro(TransformNodeName);
    

    // Adding to get patient coordinates...    
    // adding at 09. 9. 16 - smkim
    void GetCurrentPosition( double *px, double *py, double *pz);
    void GetCurrentTransformMatrix();
    void UpdateTransformNodeByName(const char *name);
    
    
    //----10.01.12-komura
    int threadLock;
    int               ThreadID;
    vtkMutexLock*     Mutex;
    vtkMultiThreader* Thread;
    static void *thread_CameraThread(void*);
//    int makeCameraThread(void);
    int makeCameraThread(const char*); // 5/5/2010 ayamada
    
    
    /* // 5/16/2010 ayamada
    IplImage*    captureImage;
    IplImage*    RGBImage;
    IplImage*    captureImageTmp;
    */
    
    CvSize        imageSize;

    // 6/20/2010 ayamada
    unsigned char* idata[2];
    vtkImageImport *importer[2];
    vtkTexture *atext[2];
    vtkActor *actor[2];

    // 5/15/2010 ayamada
    CvCapture* capture[2];
//    CvCapture* capture[2]; // 6/21/2010 ayamada
    vtkPlaneSource *planeSource;
    vtkPolyDataMapper *planeMapper;
    
    // 5/7/2010 ayamada
    // for the function "CameraFocusPlane" at .cxx file
    vtkPlaneSource *FocalPlaneSource;
    vtkPolyDataMapper *FocalPlaneMapper;
    vtkMatrix4x4 * ExtrinsicMatrix;
    double Pos[3];
    double Focal[3];
    double F;
    double ViewAngle;
    double h;
    double fx;
    double fy;
    
    // 5/17/2010
    char snapShotSavePath[500];
    //const char* snapShotSavePath;
    int snapShotNumber;
    int snapShotShutter;
    
    // 5/11/2010 ayamada
    // for function "vtkSurfaceModelRender" at .cxx file
    vtkPolyDataReader *polyReader;
    vtkPolyDataNormals *polyNormal;
    vtkPolyDataMapper *polyMapper;
    vtkPolyDataReader *polyReader1;
    vtkPolyDataNormals *polyNormal1;
    vtkPolyDataMapper *polyMapper1;
    
    
    // 10.01.25 ayamada
    vtkPlaneSource *planeSourceLeftPane;
    vtkPolyDataMapper *planeMapperLeftPane;
    vtkActor *actorLeftPane;
    
    int runThread;//10.01.21-komura
    
    // 10.01.24 ayamada
    int updateView;
    int updateViewTriger;
    int camNumber;
    
    // 5/5/2010 ayamada
    int secView;    // flag for slicer 3d view
    
    // 4/25/2010 ayamada
    // for text overlay
    vtkTextActor *textActor;
    vtkTextActor *textActor1;
    vtkTextActor *textActor2;
    vtkTextActor *textActor3;
    
    // 5/16/2010 ayamada
    vtkTextActor *textActorCamera;
    vtkTextActor *textActorCamera1;
    
    // 5/17/2010 ayamada
    vtkTextActor *textActorSavePath;
    vtkTextActor *textActorSavePathH;    
    
    //vtkTextActor *textActor6;
    //vtkActor2D *testActor;
    
    // 4/23/2010 ayamada
    float tx;
    float ty;
    float tz;
    float t0;
    float sx;
    float sy;
    float sz;
    float s0;
    float nx;
    float ny;
    float nz;
    float n0;
    float px;
    float py;
    float pz;
    float p0;
    
    float tx2;
    float ty2;
    float tz2;
    float t02;
    float sx2;
    float sy2;
    float sz2;
    float s02;
    float nx2;
    float ny2;
    float nz2;
    float n02;
    float px2;
    float py2;
    float pz2;
    float p02;
    
    
    // 5/5/2010 ayamada
    // for videoOverlay
    // variables for rendering
    //vtkTexture *atext;
    vtkPlaneSource *plane;
    vtkVolume *volume;
    vtkActor* polyActor;    // adding at 10. 02. 01 - smkim
    vtkActor* polyActor1;    // adding at 10. 02. 05 - smkim    
    vtkCamera *fileCamera;    // adding at 09. 10. 26 - smkim
    
    vtkActor2D *captureActor;
    vtkImageMapper *captureMapper;
    
    vtkActor *planeActor;
    //vtkPolyDataMapper *planeMapper;    
    
    vtkRenderer *ren;
    vtkRenderer *ren1;        // adding at 09. 9. 14 - smkim
    vtkRenderer *captureRen;
    vtkRenderWindow *captureRenWin;
    
    vtkMRMLVolumeNode* volumenode;
    vtkVolumeProperty *volumeProperty;
    
    double focal_point_x;        //adding at 10. 02. 22 - smkim
    double focal_point_y;        //adding at 10. 02. 22 - smkim
    
    double rotationAngleX;    // adding at 09. 9. 8 - smkim
    double rotationAngleY;    // adding at 09. 9. 8 - smkim
    double rotationAngleZ;    // adding at 09. 9. 8 - smkim
    
    double translationX;        // adding at 09. 9. 14 - smkim
    double translationY;        // adding at 09. 9. 14 - smkim
    double translationZ;        // adding at 09. 9. 14 - smkim
    
    vtkMatrix4x4* transformMatrix;    // adding at 09. 9. 16 - smkim
    vtkMatrix4x4* volumeTransformMatrix;    // adding at 09. 10. 26 - smkim
    
    vtkMatrix4x4* tempMatrix;
    vtkMatrix4x4* compensationMatrix;
    
    double* cameraMatrix;    // adding at 09. 11. 5 - smkim
    
    //for setting up camera image plane
    double planeRatio;
    
    vtkTransform* cameraTransform;
    vtkMRMLLinearTransformNode *OriginalTrackerNode;    // adding at 09. 9. 16 - smkim
    vtkSlicerTransformEditorWidget *TransformEditorWidget;   //adding at 09. 12. 24 - smkim
    
    char *TransformNodeName;                // adding at 09. 9. 16 - smkim    
    bool m_bVolumeRendering;    // false: before volume rendering, true: after volume rendering    // adding at 10. 02. 01 - smkim
    bool m_bSurfaceRendering;    // false: before surface rendering, true: after surface rendering    // adding at 10. 02. 01 - smkim
    bool m_bDriveSource;        // false: manual, true: data of optical tracking system        // adding at 09. 9. 17 - smkim
    bool m_bOpenSecondaryWindow;    //false: not open, true: open    // adding at 10. 01. 19 - smkim
    
    //--------------------------------------------------------------------------------------------
    // variables for capturing camera image
    // Query image from camera
    ///IplImage*    captureImage;
    //IplImage*    RGBImage;
    //IplImage*    captureImageTmp;
    // 5/16/2010 ayamada
    //IplImage*    undistortionImage;    //adding at 09. 12. 15 - smkim
    
    // 5/6/2010 ayamada
    // the above pointer is used at cvGetImageFromCamera in .cxx file. 
    
    //CvSize        imageSize;
    //unsigned char* idata;
    
    //CvCapture* capture;
    CvMat* intrinsicMatrix[2];    //for intrinsic matrix in camera calibration    //at 09. 12. 15 - smkim
    CvMat* distortionCoefficient[2];    //for distortion coefficient in camera calibration    //at 09. 12. 15 - smkim
        
    double focal_length;
    
    //--------------------------------------------------------------------------------------------
    // the fuctions for capturing camera image

    // 5/5/2010 ayamada 
    // Though the concrete codes are not written, if the comments are out, the errors are occured. 16:42
    
    //void cvCaptureAndView(void); // 5/16/2010 ayamada
 //   vtkMRMLModelNode* initCapture(const char* nodeName, double r, double g, double b);
    //void cvGetImageFromCamera(void); // 5/16/2010 ayamada
 
    //---------------------------------------------------------------------------------------------
    // the functions for volume rendering 
    // adding at 09. 8. 25 - smkim

    void vtkRayCastingVolumeRender();   // function for volume rendering with ray casting method
    void vtkTexture3DVolumeRender();    // function for volume rendering with texture mapping method
    void vtkCUDAVolumeRender();         // function for volume rendering with CUDA
    double EstimateSampleDistances(void);   //the function for estimating the dimension of resolution   //adding at 09. 9. 3 - smkim
    double* EstimateScalarRange(void);   //the function for estimating the range of scalar value   //adding at 09. 9. 3 - smkim
    
    void vtkSurfaceModelRender();   // function for surface rendering of model    //adding at 10. 01. 29 - smkim

    void CameraSet(vtkCamera *NaviCamera, double *Matrix, double FOV);    //for calculating camera position and orientation    //adding at 09. 11. 5 - from wang
    
    // 5/7/2010 ayamada
    //void CameraFocusPlane(vtkCamera * cam, double Ratio, vtkActor * actor);    //for calculating position and orientation of camera image plane    //adding at 09. 11. 5 - from wang
    void CameraFocusPlane(vtkCamera * cam, double Ratio);    //for calculating position and orientation of camera image plane    //adding at 09. 11. 5 - from wang, 5/7/2010 improvement by ayamada
    
    //----    
    

 private:
  vtkSecondaryWindowWithOpenCVGUI ( const vtkSecondaryWindowWithOpenCVGUI& ); // Not implemented.
  void operator = ( const vtkSecondaryWindowWithOpenCVGUI& ); //Not implemented.


protected:

    //----------------------------------------------------------------
    // Constructor / Destructor (proctected/private) 
    //----------------------------------------------------------------
    
    vtkSecondaryWindowWithOpenCVGUI ( );
    virtual ~vtkSecondaryWindowWithOpenCVGUI ( );

    // Adding 10.5.5 ayamada
    vtkSlicerViewerWidget* ViewerWidget; // 3D viewer widget    
    
    
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWPushButton* ShowSecondaryWindowWithOpenCVButton;
  vtkKWPushButton* HideSecondaryWindowWithOpenCVButton;

  vtkSlicerSecondaryViewerWindow* SecondaryViewerWindow;


    // 5/6/2010 ayamda for videoOverlay
    
    vtkSlicerNodeSelectorWidget* NS_ImageData;       //adding at 09. 8. 19 - smkim
    vtkSlicerVolumeHeaderWidget *VolumeHeaderWidget;         //adding at 09. 9. 2 - smkim
    
    vtkKWScaleWithLabel  *RotationAngleX;    //adding at 09. 9. 8 - smkim
    vtkKWScaleWithLabel  *RotationAngleY;    //adding at 09. 9. 8 - smkim
    vtkKWScaleWithLabel  *RotationAngleZ;    //adding at 09. 9. 8 - smkim
    
    vtkKWScaleWithLabel  *TranslationX;    //adding at 09. 9. 14 - smkim
    vtkKWScaleWithLabel  *TranslationY;    //adding at 09. 9. 14 - smkim
    vtkKWScaleWithLabel  *TranslationZ;    //adding at 09. 9. 14 - smkim  
    
    vtkKWPushButton* showCaptureData;
    vtkKWPushButton* closeCaptureData;
    
    vtkKWPushButton* selectDriveSource;    // adding at 09. 9. 17 - smkim
    
    vtkKWEntryWithLabel *TransformNodeNameEntry;    // adding at 09. 9. 16 - smkim
    
    vtkKWLoadSaveButtonWithLabel *loadLensDistortionCoefButton;    //adding at 10. 2. 23 - smkim
    vtkKWLoadSaveButtonWithLabel *loadIntrinsicParameterButton;    //adding at 10. 2. 23 - smkim

    // 6/21/2010 ayamada
    vtkKWLoadSaveButtonWithLabel *loadLensDistortionCoefButton2;
    vtkKWLoadSaveButtonWithLabel *loadIntrinsicParameterButton2;
    
    // 6/22/2010 ayamada
    vtkKWCheckButton *singleWindowCheckButton; // 
    vtkKWCheckButton *stereoWindowCheckButton; // 


    // 5/17/2010 ayamada
    vtkKWPushButton* captureCameraImage;
    //vtkKWLoadSaveButtonWithLabel *saveCameraImageButton;
    vtkKWEntryWithLabel *saveCameraImageEntry;
    //Frame Details
    vtkSlicerModuleCollapsibleFrame *DetailsFrame;
    
  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkSecondaryWindowWithOpenCVLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};



#endif
