/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/IGTLoadableModules/MotionTracker/vtkMotionTrackerGUI.h $
  Date:      $Date: 2009-02-03 12:05:00 -0500 (Tue, 03 Feb 2009) $
  Version:   $Revision: 3633 $

==========================================================================*/

#ifndef __vtkMotionTrackerGUI_h
#define __vtkMotionTrackerGUI_h

#ifdef WIN32
#include "vtkMotionTrackerWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkDoubleArray.h"

#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkMotionTrackerLogic.h"
#include "vtkMutexLock.h"
#include "vtkMultiThreader.h"
#include "vtkImageActor.h"

#include "vtkKWRadioButtonSet.h"
#include "vtkKWPushButton.h"

#include "vtkKWMultiColumnListWithScrollbars.h"

#include "vtkMotionTrackerIcons.h"

#include <map>

// 5/31/2011 ayamada
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

class vtkKWPushButton;
class vtkKWMenuButton;
class vtkKWSpinBox;
class vtkKWProgressDialog;
class vtkKWRadioButtonSet;
class vtkKWRadioButton;
class vtkKWRange;
class vtkKWEntry;
class vtkKWScaleWithEntry;
class vtkKWLoadSaveButtonWithLabel;
class vtkKWCheckButtonWithLabel;
class vtkKWRadioButtonSetWithLabel;
class vtkKWMenuButtonWithLabel;

class VTK_MotionTracker_EXPORT vtkMotionTrackerGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkMotionTrackerGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkMotionTrackerLogic );
  virtual void SetModuleLogic ( vtkMotionTrackerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }
  virtual void SetAndObserveModuleLogic ( vtkMotionTrackerLogic *logic )
  { this->SetAndObserveLogic ( vtkObjectPointer (&this->Logic), logic ); }
  virtual void SetModuleLogic( vtkSlicerLogic *logic )
  { this->SetModuleLogic (reinterpret_cast<vtkMotionTrackerLogic*> (logic)); }
  

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkMotionTrackerGUI ( );
  virtual ~vtkMotionTrackerGUI ( );

 private:
  vtkMotionTrackerGUI ( const vtkMotionTrackerGUI& ); // Not implemented.
  void operator = ( const vtkMotionTrackerGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

// 6/17/2011 ayamada
int counterForShowImage;

  static vtkMotionTrackerGUI* New ();
  void Init();
  virtual void Enter ( );
  //BTX
  using vtkSlicerComponentGUI::Enter; 
  //ETX
  virtual void Exit ( );
  void PrintSelf (ostream& os, vtkIndent indent );

  // Get the categorization of the module.
  const char *GetCategory() const { return "4D"; }

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
  //BTX
  using vtkSlicerComponentGUI::BuildGUI; 
  //ETX
  virtual void TearDownGUI ( );
  void BuildGUIForHelpFrame();
  void BuildGUIForLoadFrame(int show);
  void BuildGUIForActiveBundleSelectorFrame();
  void BuildGUIForFrameControlFrame(int show);
  void BuildGUIForFrameFrameEditor(int show);

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();
  void SelectActiveTimeSeriesBundle(vtkMRMLTimeSeriesBundleNode* bundleNode);


  //----------------------------------------------------------------
  // Viewer control
  //----------------------------------------------------------------

  void SetForeground(const char* bundleID, int index);
  void SetBackground(const char* bundleID, int index);
  void SetWindowLevelForCurrentFrame();
  //void UpdateSeriesSelectorMenus();

  //----------------------------------------------------------------
  // Editor
  //----------------------------------------------------------------
  
  void UpdateFrameList(const char* bundleID, int selectColumn=-1);
  void UpdateTimeStamp(const char* bundleID);
  int  ImportFramesFromScene(const char* bundleID, const char* format, int min, int max);

  void BuildGUIForWindowConfigurationFrame();
  void BuildGUIForOpticalFlowFrame();


// 6/17/2011 ayamada
  void Gray16toBGR24(unsigned char*& pBGR24,
unsigned short* pGray16,
const unsigned int width,
const unsigned int height);

  //----------------------------------------------------------------
  // Other utility functions
  //----------------------------------------------------------------
  const char* GetActiveTimeSeriesBundleNodeID();
  vtkMRMLTimeSeriesBundleNode *GetActiveTimeSeriesBundleNode ();

 protected:

// 5/31/2011 ayamada
  int ViewerBackgroundOn(vtkSlicerViewerWidget* vwidget, vtkImageData* imageData);
  int ViewerBackgroundOff(vtkSlicerViewerWidget* vwidget);
  int StartCamera(int channel, const char* path);
  int StopCamera();
  int CameraHandler();
  int ProcessMotion(CvPoint2D32f* vector, CvPoint2D32f* position, int n);

// 6/10/2011 ayamada
int switchImage;
  
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;  // ms

// 5/31/2011 ayamada
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

// 6/11/2011 ayamada
IplImage*     ImageFromScannerTmp;
IplImage*     ImageFromScanner;
IplImage*     captureImageforHighGUI;
int makeImageFromScanner;




  vtkImageData* VideoImageData;
  vtkImageData* CaptureImageData;

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

  vtkKWProgressDialog *ProgressDialog;

// 5/31/2011 ayamada
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


  // -----------------------------------------
  // Icons
  vtkMotionTrackerIcons* Icons;

  // -----------------------------------------
  // Active 4D Bundle selector
  vtkSlicerNodeSelectorWidget*  ActiveTimeSeriesBundleSelectorWidget;

  // -----------------------------------------
  // Load / Save / Active frame
  vtkKWLoadSaveButtonWithLabel* SelectInputDirectoryButton;
  vtkKWPushButton*              LoadImageButton;
  vtkKWLoadSaveButtonWithLabel* SelectOutputDirectoryButton;
  vtkKWPushButton*              SaveImageButton;
  vtkKWRadioButtonSetWithLabel* LoadOptionButtonSet;
  vtkKWEntryWithLabel*          LoadTimePointsEntry;
  vtkKWEntryWithLabel*          LoadSlicesEntry;
  vtkKWEntryWithLabel*          LoadChannelsEntry;
  vtkKWEntryWithLabel*          LoadFileFilterEntry;
  vtkKWMenuButtonWithLabel*     LoadFileOrderMenu;

  // -----------------------------------------
  // Frame control
  vtkKWRange *WindowLevelRange;
  vtkKWRange *ThresholdRange;

  vtkKWScaleWithEntry* ForegroundVolumeSelectorScale;
  vtkKWScaleWithEntry* BackgroundVolumeSelectorScale;

  vtkKWPushButton*     AutoPlayFGButton;
  vtkKWPushButton*     AutoPlayBGButton;
  vtkKWEntry*          AutoPlayIntervalEntry;

  // -----------------------------------------
  // Frame editor
  vtkKWMultiColumnListWithScrollbars* FrameList;
  vtkKWPushButton*                    FrameMoveUpButton;
  vtkKWPushButton*                    FrameMoveDownButton;
  vtkKWPushButton*                    RemoveFrameButton;
  vtkSlicerNodeSelectorWidget*        AddFrameNodeSelector;
  vtkKWPushButton*                    AddFrameNodeButton;
  vtkKWRadioButtonSetWithLabel*       TimeStampMethodButtonSet;
  vtkKWEntry*                         TimeStepEntry;
  vtkKWEntry*                         ImportFrameFormatEntry;
  vtkKWEntry*                         ImportFrameRangeMinEntry;
  vtkKWEntry*                         ImportFrameRangeMaxEntry;
  vtkKWPushButton*                    ImportFrameNodeButton;


  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  //BTX
  vtkMotionTrackerLogic::RegistrationParametersType DefaultAffineRegistrationParam;
  vtkMotionTrackerLogic::RegistrationParametersType DefaultRegistrationParam;
  //ETX
  
  vtkMotionTrackerLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

  double  RangeLower;
  double  RangeUpper;

  double  Window;
  double  Level;
  double  ThresholdUpper;
  double  ThresholdLower;


  // Auto play functions
  int     AutoPlayFG;
  int     AutoPlayBG;
  int     AutoPlayInterval;        // interval = TimerInterval * AutoPlayInterval; 
  int     AutoPlayIntervalCounter;

  //BTX
  typedef std::vector<int> WindowLevelUpdateStatusType;
  typedef std::vector<std::string> NodeIDListType;
  //ETX
  
  WindowLevelUpdateStatusType WindowLevelUpdateStatus;

  NodeIDListType MaskNodeIDList;
  NodeIDListType BundleNodeIDList;

  int BundleNameCount; // used to name 4D bundle

};



#endif
