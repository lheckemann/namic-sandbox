/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkLapOverlayGUI_h
#define __vtkLapOverlayGUI_h

#ifdef WIN32
#include "vtkLapOverlayWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkLapOverlayLogic.h"
//#include "vtkSecondaryWindowViwerWindow.h"
#include "vtkMutexLock.h"
#include "vtkMultiThreader.h"
#include "vtkImageActor.h"

#include "vtkKWRadioButtonSet.h"
#include "vtkKWPushButton.h"


#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

class vtkKWPushButton;

class VTK_LapOverlay_EXPORT vtkLapOverlayGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkLapOverlayGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkLapOverlayLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkLapOverlayGUI ( );
  virtual ~vtkLapOverlayGUI ( );

 private:
  vtkLapOverlayGUI ( const vtkLapOverlayGUI& ); // Not implemented.
  void operator = ( const vtkLapOverlayGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkLapOverlayGUI* New ();
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
  void BuildGUIForOpticalFlowFrame();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();

 protected:

  int ViewerBackgroundOn(vtkSlicerViewerWidget* vwidget, vtkImageData* imageData);
  int ViewerBackgroundOff(vtkSlicerViewerWidget* vwidget);
  int StartCamera(int channel, const char* path);
  int StopCamera();
  int CameraHandler();
  int ProcessMotion(CvPoint2D32f* vector, CvPoint2D32f* position, int n);

  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // Video import
  //----------------------------------------------------------------

  vtkRenderer*   BackgroundRenderer;
  vtkImageActor* BackgroundActor;
  int CameraActiveFlag;
  CvCapture* capture;

  // 5/15/2010 ayamada
  CvSize        imageSize;
  IplImage*     captureImage;
  IplImage*     RGBImage;
  IplImage*     undistortionImage;      //adding at 09. 12. 15 - smkim

  vtkImageData* VideoImageData;

  // Optical Tracking
  int           OpticalFlowTrackingFlag;
  IplImage*     GrayImage;
  IplImage*     PrevGrayImage;
  IplImage*     Pyramid;
  IplImage*     PrevPyramid;
  IplImage*     SwapTempImage;
  int           PyrFlag;

  CvPoint2D32f* Points[2];
  CvPoint2D32f* SwapPoints;
  CvPoint2D32f* GridPoints[2];// = {0,0};
  CvPoint2D32f* RVector;
  char*         OpticalFlowStatus;


  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWLabel* CameraChannelLabel;
  vtkKWEntry* CameraChannelEntry;

  vtkKWLabel* VideoFileLabel;
  vtkKWEntry* VideoFileEntry;
  vtkKWPushButton* VideoFileSelectButton;
  vtkKWRadioButtonSet* VideoSourceButtonSet;

  vtkKWPushButton* StartCaptureButton;
  vtkKWPushButton* StopCaptureButton;

  //vtkKWPushButton* ShowSecondaryWindowButton;
  //vtkKWPushButton* HideSecondaryWindowButton;
  //vtkSecondaryWindowViwerWindow* SecondaryViewerWindow;

  vtkSlicerNodeSelectorWidget* TransformNodeSelector;
  vtkKWRadioButtonSet* OpticalFlowStatusButtonSet;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkLapOverlayLogic *Logic;
  vtkCallbackCommand      *DataCallbackCommand;
  int                     CloseScene;


};



#endif
