/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkStereoCalibGUI_h
#define __vtkStereoCalibGUI_h

#ifdef WIN32
#include "vtkStereoCalibWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkStereoCalibLogic.h"


#include "vtkStereoCalibViewerWidget.h"
#include "vtkPlaneSource.h"
#include "vtkMath.h"
//#include "vtkPolyDataMapper.h"
#include "vtkMultiThreader.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "vtkImageImport.h"

// 6/6/2010 ayamada
#include "vtkTextActor.h"

//chessLoad
#include <vector>               // 
#include <string>               // 
#include <algorithm>            // 
#include <stdio.h>              // 
#include <ctype.h>              // 100603-komura
// #include <cxmisc.h>
//

#include "vtkKWScale.h"         // 100617-komura
#include "vtkStereoCalibCVClass.h" // 100621-komura

#define FOCAL_LENGTH 900.0 // 5/5/2010 ayamada is it dammy?
#define FOA  31.76 //27.0 //31.76 //40.3 //Endoscope 
#define VIEW_SIZE_X 640.0
#define VIEW_SIZE_Y 480.0


//chessLoad 100603-komura
// #define ADAPTIVE_THRESH 0                        // CV_CALIB_CB_ADAPTIVE_THRESH Setting parameta
// #define NORMALIZE_IMAGE 0                        // CV_CALIB_CB_NORMALIZE_IMAGE Setting parameta
// #define FILTER_QUADS 0                                // CV_CALIB_CB_FILTER_QUADS Setting parameta
// #define CORNER_WIDTH 9                                // number of side coner 
// #define CORNER_HEIGHT 6                                  // 
// #define CORNER_NUMBER ( CORNER_WIDTH * CORNER_HEIGHT ) // sum of all coner
//100607-komura
//

class vtkKWPushButton;
class vtkKWCheckButton;         // 100615-komura
class vtkKWScaleWithLabel;      // 100617-komura

class VTK_StereoCalib_EXPORT vtkStereoCalibGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkStereoCalibGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkStereoCalibLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkStereoCalibGUI ( );
  virtual ~vtkStereoCalibGUI ( );

 private:
  vtkStereoCalibGUI ( const vtkStereoCalibGUI& ); // Not implemented.
  void operator = ( const vtkStereoCalibGUI& ); //Not implemented.

 /////////////
 // public  //
 /////////////
 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkStereoCalibGUI* New ();
  void Init();
  virtual void Enter ( );
  virtual void Exit ( );
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
  void BuildGUIForTestFrame1();
  void BuildGUIForTestFrame2();
  void BuildGUIForTestFrame3(); // 100603-komura

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();

    //----------------------------------------------------------------
    // Save Matrices
    //----------------------------------------------------------------
    // 6/6/2010 ayamada
    vtkTextActor *textActorSavePath;
    vtkTextActor *textActorSavePath2;
    vtkTextActor *textActorSavePathH;    
    char snapShotSavePath[500];
    char snapShotSavePath2[500];
    int snapShotNumber;
    int snapShotShutter;    
    
    

 ///////////////
 // protected //
 ///////////////
 protected:
  
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWPushButton* TestButton11;
  vtkKWPushButton* TestButton12;
  vtkKWPushButton* TestButton21;
  vtkKWPushButton* TestButton22;
  vtkKWPushButton* TestButton31;
  // vtkSlicerSecondaryViewerWindow* SecondaryViewerWindow;
  vtkStereoCalibViewerWidget* SecondaryViewerWindow;
  vtkStereoCalibViewerWidget* SecondaryViewerWindow2x;

    // 6/6/2010 ayamada
    vtkKWEntryWithLabel *saveCameraImageEntry;
    

  //----------------------------------------------------------------
  // MultiThread
  //----------------------------------------------------------------
  int makeCameraThread(const char* );
 vtkCamera* fileCamera;
 int               ThreadID;
 vtkMultiThreader* Thread;
 static void *thread_CameraThread(void*);

  //----------------------------------------------------------------
  // CameraFocusPlane
  //----------------------------------------------------------------
 void CameraFocusPlane(vtkCamera *, double);
 vtkMatrix4x4 * ExtrinsicMatrix;
 double Pos[3];
 double Focal[3];
 double F;
 double ViewAngle;
 double h;
 double fx;
 double fy;
 double focal_point_x;  //adding at 10. 02. 22 - smkim
 double focal_point_y;  //adding at 10. 02. 22 - smkim
 double focal_length;   // 
 // vtkPlaneSource *FocalPlaneSource;       // 
 // vtkPolyDataMapper *FocalPlaneMapper;    // 
 vtkPlaneSource *FocalPlaneSource[2];       // 
 vtkPolyDataMapper *FocalPlaneMapper[2];    // 100603-komura

    //----------------------------------------------------------------
    // cameraThread
    //----------------------------------------------------------------
    CvSize  imageSize;
    double planeRatio;
    int makeThread;
    // unsigned char* idata;        // 
    // vtkImageImport *importer;    // 
    // vtkTexture *atext;           // 
    // vtkActor *actor;             // 
    unsigned char* idata[2];        // 
    vtkImageImport *importer[2];    // 
    vtkTexture *atext[2];           // 
    vtkActor *actor[2];             // 100603-komura
    
    //----------------------------------------------------------------
    // StereoCalib
    //----------------------------------------------------------------
    // void chessLoad(IplImage*, CvSize&, int);  // 
    int createFindChessboardCornersFlag();    // 
    // void displayChessboard(IplImage*);        //
    // void stereoCalib();                       // 
    vtkKWPushButton* displayChessboardButton; // 
    int displayChessboardFlag;                //
    int captureChessboardFlag;                //
    int stereoCalibFlag;                      // 100603-komura 
    // void displayStereoCalib(int);                // 100607-komura

    //----------------------------------------------------------------
    // SecondWindowMode
    //----------------------------------------------------------------

    vtkKWCheckButton *stereoOneWindowCheckButton; // 
    vtkKWCheckButton *stereoTwoWindowCheckButton; // 
    vtkKWCheckButton *stereoOneWindowLayeredCheckButton; // 100615-komura

    vtkKWScaleWithLabel *rightOpacityBar; //
    double rightOpacity;                  //
    vtkKWScaleWithLabel *leftOpacityBar; //
    double leftOpacity;                  //
    vtkKWScaleWithLabel *gapGraphicsBar;     //
    double gapGraphics;                      // 100617-komura
    vtkStereoCalibCVClass *CVClass;          // 100621-komura

    //----------------------------------------------------------------
    // Logic Values
    //----------------------------------------------------------------
    vtkStereoCalibLogic *Logic;
    vtkCallbackCommand *DataCallbackCommand;
    int                        CloseScene;
    

};

#endif
