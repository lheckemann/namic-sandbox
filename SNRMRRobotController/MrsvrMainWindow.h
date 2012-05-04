//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
// Copyright (C) 2005-2006 by Shiga University of Medical Science,
// All Right Reserved.
// Copyright (C) 2006-2012 by Brigham and Women's Hospital,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrMainWindow.h,v $
// $Revision: 1.16 $ 
// $Author: junichi $
// $Date: 2006/01/20 03:15:48 $
//====================================================================


//====================================================================
// Description:
//    MrsvrWindow module creates a window on a display to visualize 
//    hardware status.
//====================================================================


//--------------------------------------------------------------------
// Compile options
//--------------------------------------------------------------------

#ifndef _INC_MRSVR_MAIN_WINDOW
#define _INC_MRSVR_MAIN_WINDOW

#ifdef DEBUG
#define DEBUG_MRSVR_MAIN_WINDOW
#endif


//--------------------------------------------------------------------
// Header files
//--------------------------------------------------------------------

#include <iostream>
#include <fx.h>
#include <cstring>

#include "MrsvrPlotCanvas.h"
#include "MrsvrStatus.h"
#include "MrsvrLog.h"
#include "MrsvrCommand.h"
#include "MrsvrMessageServer.h"
#include "shmKeys.h"

//--------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------

// convert real parameters to progress bar paramters
#define POS2PROG(v)          (int)(((v-MIN_POSITION)/(MAX_POSITION-MIN_POSITION))*100.0)
#define VEL2PROG(v)          (int)((fabs(v)/fabs(MAX_VELOCITY))*100.0)
#define VOL2PROG(v)          (int)((fabs(v)/MAX_ACTUATOR_VOL_V)* 100.0)

// debugging macros
#if (__GNUC__ >= 3)  // in case of GCC 3.x
  #ifdef DEBUG_MRSVR_MAIN_WINDOW
    #define DBG_MMW_PRINT(...)   fprintf(stderr, "DEBUG:MrsvrMainWindow >> " __VA_ARGS__)
  #else
    #define DBG_MMW_PRINT(...)
  #endif
#else // in case of GCC 2.x
  #ifdef DEBUG_MRSVR_MAIN_WINDOW
    #define DBG_MMW_PRINT(s...)   fprintf(stderr, "DEBUG:MrsvrMainWindow >> " s)
  #else
    #define DBG_MMW_PRINT(s...)
  #endif
#endif


//--------------------------------------------------------------------
// PARAMETERS
//--------------------------------------------------------------------

// Control paramters
#define MANUAL_VOL_STEPS     20
#define INITIAL_VOL          5

// Screen paramters
#define INFCNV_W             1280
#define INFCNV_H             120
//#define INFCNV_ITEM_Y        0.3
#define INFCNV_ITEM_Y        0.2
//#define INFCNV_VALUE_Y       0.7
#define INFCNV_VALUE_Y       0.65
#define INFCNV_VALUE1_Y      0.5
#define INFCNV_VALUE2_Y      0.8
#define INFCNV_MODE_X        0.02
#define INFCNV_TARGET_X      0.16
#define INFCNV_WARNING_X     0.48
//#define INFCNV_TIME_X        0.750
#define INFCNV_TIME_X        0.875
//#define INFCNV_RTIME_X       0.875
#define INFCNV_FONT_H        30

#define INFCNV_TARGET_DIR_W  30
#define INFCNV_TARGET_VAL_W  100

#define INFCNV_WARN_LOCK_X       0.48
#define INFCNV_WARN_LOCK_Y       INFCNV_VALUE1_Y
#define INFCNV_WARN_OUTOFRANGE_X 0.48
#define INFCNV_WARN_OUTOFRANGE_Y INFCNV_VALUE2_Y
#define INFCNV_WARN_NOLOCSVR_X   0.60
#define INFCNV_WARN_NOLOCSVR_Y   INFCNV_VALUE1_Y

#define AXIALCNV_W           280
#define AXIALCNV_H           280
#define NEEDLE_CANVAS_W       580
#define NEEDLE_CANVAS_H       240

#define WARNID_LOCK          0
#define WARNID_OUTOFRANGE    1
#define NUM_WARNID           2
//#define WARNID_NOLOCSVR      2


// Parameters for axial canvas
#define AXIALCNV_MARGIN           0.05

// Parameters for needle canvas
#define NEEDLECNV_DEPTH_LABEL_X   0.03
#define NEEDLECNV_DEPTH_LABEL_Y   0.1

#define NEEDLECNV_DEPTH_VALUE_X   0.03
#define NEEDLECNV_DEPTH_VALUE_Y   0.3

#define NEEDLECNV_DEPTH_UNIT_X    0.3
#define NEEDLECNV_DEPTH_UNIT_Y    0.305

#define NEEDLECNV_RANGE_X         200
#define NEEDLECNV_RANGE_Y         200

// ICONS
#define NUM_ICONS                 4
#define ICON_INDEX_HOLD           0
#define ICON_INDEX_ACTIVE         1
#define ICON_INDEX_EMERGENCY      2
#define ICON_INDEX_MOVE           3

#define ICON_PNG_HOLD           "icon/hold100x100_black.png"     
#define ICON_PNG_ACTIVE         "icon/active100x100_black.png"   
#define ICON_PNG_EMERGENCY      "icon/emergency100x100_black.png"
#define ICON_PNG_MOVE           "icon/move100x100_black.png"     

//--------------------------------------------------------------------
// class MrsvrMainWindow
//--------------------------------------------------------------------

class MrsvrMainWindow : public FXMainWindow {
  FXDECLARE(MrsvrMainWindow);

//------------------------ Public constants ------------------------//
public:

  // Messages
  enum {
    ID_DOWNSIZE = FXMainWindow::ID_LAST,
    ID_DELETE,
    ID_SAVE_CALIB_POINTS,
    ID_FILEDLG_OPEN,
    ID_FILEDLG_SAVEAS,
    ID_FILE_CLOSE,
    ID_ABOUT,
    ID_CANVAS,
    ID_AXIAL_CANVAS,
    ID_NEEDLE_CANVAS,

    ID_PAINT_TARGET,
    ID_RESTORE,
    ID_REPAINT_TRIGGER,
    ID_UPDATE_PARAMETER,
    ID_UPDATE_ACTUATOR_VOLTAGE,
    ID_UPDATE_MINI_PLOT,
    ID_TABLE,
    ID_UPDATE_TIMER,
    ID_START_UPDATE,
    ID_STOP_UPDATE,
    ID_CMD_CALIBRATION,     // Do not change this order
    ID_CMD_HOLD,            // Do not change this order
    ID_CMD_ACTIVE,          // Do not change this order
    ID_CMD_EMERGENCY,       // Do not change this order
    ID_CMD_RESET,           // Do not change this order
    ID_CMD_CALIBRATE,
    ID_CMD_START_COM,
    ID_CMD_STOP_COM,
    ID_CMD_SAVE_DEFAULT,
    ID_CMD_RESET_DEFAULT,

    ID_CALIB_LEFT_BTN,
    ID_CALIB_RIGHT_BTN,
    ID_CALIB_ZERO_BTN,

    ID_CMD_CHOOSE_END_EFFECTOR_CONFIG,
    ID_SET_ENDEFFECT_BTN,
    ID_CANCEL_ENDEFFECT_BTN,

    ID_MANUAL_LEFT_BTN,
    ID_MANUAL_RIGHT_BTN,
    ID_MANUAL_VOLTAGE,

    ID_SET_TARGET_BTN,
    ID_CANCEL_TARGET_BTN,

    // Added on 02/02/2007
    ID_UPDATE_NEEDLE_APPR_OFFSET,
    ID_NEEDLE_APPR_BTN,
    ID_NEEDLE_LEAVE_BTN,
    // -end

    ID_SHUTDOWN_MRSVR_BTN,

    ID_MANUAL_PW_OFF_0,
    ID_MANUAL_PW_ON_0,
    ID_MANUAL_PW_OFF_1,
    ID_MANUAL_PW_ON_1,
    ID_MANUAL_PW_OFF_2,
    ID_MANUAL_PW_ON_2,
    ID_MANUAL_PW_OFF_3, //reserved
    ID_MANUAL_PW_ON_3,  //reserved
    ID_MANUAL_PW_OFF_4, //reserved
    ID_MANUAL_PW_ON_4,  //reserved

    ID_SHOWDIALOG, //Maier Navigation without Slicer


    ID_LAST
  };

  enum {
    CTRL_PNL_COM = 0,
    CTRL_PNL_CALIB,
    CTRL_PNL_SYSTEM,
    NUM_CTRL_PNL
  };

  // Constants for screenMode
  enum {
    SCR_NORMAL,
    SCR_CALIBRATION,
  };


//--------------------- GUI widgets variables ----------------------//
protected:
  FXMenuBar*         menubar;            // Menu bar
  FXToolBar*         toolbar;            // Tool bar
  
  FXStatusBar*       statusbar;
  FXTabBook*         mainTab;

  FXCanvas*          infoCanvas;         // main information display // on the top of the main window                  
  FXCanvas*          axialCanvas;        // display that shows the current actuator position
  FXCanvas*          needleCanvas;       // display needle information

  FXImage*           infoOffs;           // offscreen buffer
  FXImage*           axialOffs;           // offscreen buffer
  FXImage*           needleOffs;           // offscreen buffer

  // font for information display 
  FXFont*            infoFont0;          // for small
  FXFont*            infoFont1;          // for large
  FXFont*            infoFont2;          // for very large (for time)

  FXFont*            axialFont;          // for axial canvas


  // Buttons for manual controlling test
  FXButton*          btCalibLeft[NUM_ACTUATORS];
  FXButton*          btCalibRight[NUM_ACTUATORS];
  FXButton*          btCalibZero[NUM_ENCODERS]; // NOTE # of elements is NUM_ENCODERS
  FXButton*          btManualLeft[NUM_ACTUATORS];
  FXButton*          btManualRight[NUM_ACTUATORS];
  FXShutterItem*     siControlPanel[NUM_CTRL_PNL];
  FXShutter*         shtControlPanel;
  FXText*            textRtcpLog;
  FXText*            textInterfaceLog;

  MrsvrPlotCanvas*   plotCanvasA;     // plot canvas for translation
  MrsvrPlotCanvas*   plotCanvasB;     // plot canvas for rotation
  // for progress bar in status bar
  FXProgressBar      *statusProg;
  FXLabel            *textStatusProg;

//----------------------- Private variables ------------------------//
private:
  char               infoTime[16];
  char               infoRTime[16];
  int                infoCurrentMode;
  //int                infoCurrentWarn;
  time_t             infoStartTime;

  char               infoTargetR[16];
  char               infoTargetA[16]; 
  char               infoTargetS[16];
  char               infoCurrentX[16];   // in magnet coordinate
  char               infoCurrentY[16];   // in magnet coordinate
  char               infoCurrentZ[16];   // in magnet coordinate

  bool               infoWarning[NUM_WARNID];
  bool               prevInfoWarning[NUM_WARNID];

  int                debugLevel;
  /*-------------------- NOTES on debugLevel ------------------------+
  | 'debugLevel' determins how much information to print on the log  |
  | console window. There are three levels:                          |
  |   level 0  -- Print only critical errors and informations which  |
  |               should be recorded in crinical routine.            |
  |   level 1  -- In addition to level 0, print every user           |
  |               interactions events.                               |
  |   level 2  -- Print all debug informations.                      |
  +-----------------------------------------------------------------*/


//----------------------- Constant strings -------------------------//
private:
  static const char* autoCalibProcNameText[];
  static const char* infoModeText[];   
  static const char* actuatorStatusText[];
  static const char* infoWarnText[];   

  static const char* iconPNGFile[];

//---------------------------- Modules -----------------------------//
private:
  // robot control information
  MrsvrRASReader*         setPointRAS;
  MrsvrStatusReader*      robotStatus;
  MrsvrCommandWriter*     robotCommand; 
  MrsvrLogReader*         robotLog;
  MrsvrMessageServer*     extMsgSvr;
  MrsvrTransform*         transform;
  
  int                     prevSvrStatus; 


//--------------- Screen update flags and variables ---------------//
private:
  // update flags for infomation canvas 
  int                fUpdateInfoItem;
  int                fUpdateInfoMode;
  int                fUpdateInfoWarn;
  int                fUpdateInfoTarget;
  int                fUpdateInfoTime;  
  int                fUpdateInfoRTime;  

  int                infoCnvH;
  int                infoCnvW;
  int                infoCnvItemY; 
  int                infoCnvValueY;
  int                infoCnvValue1Y;
  int                infoCnvValue2Y;
  int                infoCnvModeX;
  int                infoCnvWarningX;
  int                infoCnvTargetX;
  int                infoCnvTimeX;
  int                infoCnvRTimeX;
  int                infoWarnTextX[NUM_WARNID];
  int                infoWarnTextY[NUM_WARNID];

  //FXTable*           tblTcpInfo;        // TCP/IP information table 


  // Timer to update the screen
  FXApp*             application;
  int                updateInterval;     // update interval status information (ms)
  FXTimer*           updateTimer;
  int                screenMode;

  int                manualPowerSw[NUM_ACTUATORS];

  //------------- Data targets and variables section 1 -------------//
  // Control Panel -> TCP/IP
  // -- Server Thread
  FXDataTarget*      dtConPortNo; 
  FXDataTarget*      dtConStatus;
  int                valConPortNo;
  FXString           valConStatus;

  // -- Locator server
  //FXDataTarget*      dtLocServPortNo;
  //FXDataTarget*      dtLocServHostName;
  //FXDataTarget*      dtLocServStatus;
  //FXDataTarget*      dtLocServInterval;
  //int                valLocServPortNo;
  //FXString           valLocServHostName;
  //FXString           valLocServStatus;
  //int                valLocServInterval;  // (ms)

  // Calibration information
  FXDataTarget*      dtCalibrationStatus;
  FXDataTarget*      dtCalibrationMatrix[16];

  FXString           valCalibrationStatus;
  float              valCalibrationMatrix[16];


  // Control Panel -> Calibration
  FXDataTarget*      dtAutoCalibProcSelect[NUM_PROC_AUTOCALIB];
  FXint              valAutoCalibProcSelect[NUM_PROC_AUTOCALIB];

  // Control Panel -> End effector
  float              valNeedleOffset[3];
  // -- default needle orientation
  FXDataTarget*      dtNeedleOrientation[3];
  float              valNeedleOrientation[3];

  // Control Panel -> Manual
  FXDataTarget*      dtManualActuatorVol[NUM_ACTUATORS];
  int                valManualActuatorVol[NUM_ACTUATORS]; 

  FXDataTarget*      dtTargetPosition[3];
  FXDataTarget*      dtCurrentPosition[3];
  FXDataTarget*      dtDeltaPosition[3];

  float              valTargetPosition[3];
  float              valCurrentPosition[3];
  float              valDeltaPosition[3];

  // Normalized current and target position for visualization
  FXDataTarget*      dtNormTargetPosition[3];
  FXDataTarget*      dtNormCurrentPosition[3];
  float              valNormTargetPosition[3];  //  [0.0, 1.0]
  float              valNormCurrentPosition[3]; //  [0.0, 1.0]
  
  FXDataTarget*      dtTipApprOffset;
  float              valTipApprOffset;

  // Hardware monitor -> Actuators / Encoders
  FXDataTarget*      dtTxActuatorVol[NUM_ACTUATORS];
  FXDataTarget*      dtTxPosition[NUM_ENCODERS];
  FXDataTarget*      dtTxSetPoint[NUM_ENCODERS];
  FXDataTarget*      dtTxVelocity[NUM_ENCODERS];
  FXDataTarget*      dtPrActuatorVol[NUM_ACTUATORS];
  FXDataTarget*      dtPrPosition[NUM_ENCODERS];
  FXDataTarget*      dtPrSetPoint[NUM_ENCODERS];
  FXDataTarget*      dtPrVelocity[NUM_ENCODERS];
  FXDataTarget*      dtTxActRev[NUM_ACTUATORS];
  FXDataTarget*      dtTxActStatus[NUM_ACTUATORS];

  float              valActuatorVol[NUM_ACTUATORS];
  float              valPosition[NUM_ENCODERS];
  float              valSetPoint[NUM_ENCODERS];
  float              valVelocity[NUM_ENCODERS];
  int                valPrActuatorVol[NUM_ACTUATORS];
  int                valPrPosition[NUM_ENCODERS];
  int                valPrSetPoint[NUM_ENCODERS];
  int                valPrVelocity[NUM_ENCODERS];
  int                valTotalActRev[NUM_ACTUATORS];
  FXString           valActStatus[NUM_ACTUATORS];
  
  // for displaying encoder position value
  static const float positionOffsets[];
  static const float positionCoeffs[];

  // Plot
  FXDataTarget*      dtPlotAGain;
  FXDataTarget*      dtPlotAShift;
  FXDataTarget*      dtPlotBGain;
  FXDataTarget*      dtPlotBShift;
  int                valPlotAGain;
  int                valPlotAShift;
  int                valPlotBGain;
  int                valPlotBShift;

  // Status bar
  FXDataTarget       *dtStatusProg;
  int                valStatusProg;


  //--------- Data targets and variables section 2 ----------//
  // Data targets and values for default settings

  // -- TCP/IP
  FXDataTarget*      dtDefConPortNo; 
  int                valDefConPortNo;

  FXDataTarget*      dtDefLocServPortNo;
  FXDataTarget*      dtDefLocServHostName;
  FXDataTarget*      dtDefLocServInterval;
  int                valDefLocServPortNo;
  FXString           valDefLocServHostName;
  int                valDefLocServInterval;  // (ms)

  // -- Calibration
  FXDataTarget*      dtDefAutoCalibProcSelect[NUM_PROC_AUTOCALIB];
  FXint              valDefAutoCalibProcSelect[NUM_PROC_AUTOCALIB];

  // -- Configuration
  FXDataTarget*      dtPatientEntry;
  FXDataTarget*      dtBedDock;
  FXDataTarget*      dtRobotDock;

  int                valPatientEntry;
  int                valBedDock;
  int                valRobotDock;

  int                currentNeedleId;
  float              valDefNeedleOffset[3];
  float              valDefNeedleOrientation[3];
  FXString           valNeedleConfName;

  FXDataTarget*      dtNeedleConfName;
  FXDataTarget*      dtDefNeedleOffset[3];
  FXDataTarget*      dtDefNeedleOrientation[3];

  float              prevNeedleDepth;
  float              valNeedleDepth;
  FXDataTarget*      dtNeedleDepth;
  char               strNeedleDepth[16];

  // -- Manual
  
  // -- System

  // -- Motion Range
  FXDataTarget*      dtMotionRangeMin[NUM_ENCODERS];
  FXDataTarget*      dtMotionRangeMax[NUM_ENCODERS];
  float              valMotionRangeMin[NUM_ENCODERS];
  float              valMotionRangeMax[NUM_ENCODERS];

  // ICONS
  FXIcon*            icons[NUM_ICONS];


//------------------- Constructor & Destructor ---------------------//
protected:
  MrsvrMainWindow(){};

public:
  MrsvrMainWindow(FXApp* a, int, int);
  virtual ~MrsvrMainWindow();


//---------------------- Private functions -------------------------//
private:
  int    buildCommunicationControlPanel(FXComposite*);
  int    buildCalibrationControlPanel(FXComposite*);
  int    buildConfigurationControlPanel(FXComposite*);
  int    buildManualControlPanel(FXComposite*);
  int    buildSystemControlPanel(FXComposite*);
  int    buildControlPanel(FXComposite*);

  int    buildSwPanel(FXComposite*);
  int    buildHardwareMonitor(FXComposite*);

  int    buildQuickPanel(FXComposite*);
  int    getObjectIndex(FXObject*, FXObject**, int);
  int    updateTcpInfoTbl();
  int    initSharedInfo();
  void   initParameters();
  void   loadIcons();
  void   loadRegistry();
  void   storeRegistry();
  void   setDataTargets();
  void   setDefaultParameters();
  void   updateParameters();
  void   updateExternalCommands();
  void   consolePrint(int, bool, const char*,...);
  void   setTargetPositionRAS(float[3]);
  void   setCalibrationMatrix(float[16]);

//----------------------- Public functions -------------------------//
public:
  virtual void create();
  inline void  setExtMsgSvr(MrsvrMessageServer* p) {
    extMsgSvr = p; prevSvrStatus = -1;
  };
  //inline void  setLocClient(MrsvrLocatorClient* p) {
  //  locClient = p;
  //}

//------------------------ Event Handlers --------------------------//
public:
  // Message handlers
  //long onCmdFileDlgOpen(FXObject*, FXSelector, void*);
  //long onCmdFileDlgSaveAs(FXObject*, FXSelector, void*);
  //long onCmdFileClose(FXObject*, FXSelector, void*);
  long onCmdAbout(FXObject*, FXSelector, void*);
  long onCanvasRepaint(FXObject*, FXSelector,void*);
  long onAxialCanvasRepaint(FXObject*, FXSelector,void*);
  long onNeedleCanvasRepaint(FXObject*, FXSelector,void*);

  long onUpdateActuatorVoltage(FXObject*, FXSelector,void*);
  long onMiniPlotUpdate(FXObject*, FXSelector,void*);
  long onStartUpdate(FXObject*, FXSelector,void*);
  long onStopUpdate(FXObject*, FXSelector, void*);
  long onUpdateTimer(FXObject*, FXSelector, void*);
  long onCmdHold(FXObject*, FXSelector, void*);
  long onCmdActive(FXObject*, FXSelector, void*);
  long onCmdEmergency(FXObject*, FXSelector, void*);
  long onCmdReset(FXObject*, FXSelector, void*);
  long onCmdCalibrate(FXObject*, FXSelector, void*);

  long onCmdStartCom(FXObject*, FXSelector, void*);
  long onCmdStopCom(FXObject*, FXSelector, void*);
  long onCmdLocServCon(FXObject*, FXSelector, void*);
  long onCmdLocServDiscon(FXObject*, FXSelector, void*);
  long onCmdSaveDefault(FXObject*, FXSelector, void*);
  long onCmdResetDefault(FXObject*, FXSelector, void*);

  long onCalibLeftBtnPressed(FXObject*, FXSelector, void*);
  long onCalibRightBtnPressed(FXObject*, FXSelector, void*);
  long onCalibLeftBtnReleased(FXObject*, FXSelector, void*);
  long onCalibRightBtnReleased(FXObject*, FXSelector, void*);
  long onCalibZeroBtnPressed(FXObject*, FXSelector, void*);
  long onCalibZeroBtnReleased(FXObject*, FXSelector, void*);

  long onManualLeftBtnPressed(FXObject*, FXSelector, void*);
  long onManualRightBtnPressed(FXObject*, FXSelector, void*);
  long onManualLeftBtnReleased(FXObject*, FXSelector, void*);
  long onManualRightBtnReleased(FXObject*, FXSelector, void*);

  long onShutdownMrsvrBtnReleased(FXObject*, FXSelector, void*);

  long onSetTargetBtnReleased(FXObject*, FXSelector, void*);
  long onCancelTargetBtnReleased(FXObject*, FXSelector, void*);

  long onUpdateNeedleApprOffset(FXObject*, FXSelector, void*);
  long onCmdManualPowerSw(FXObject*, FXSelector, void*);
  long onUpdateManualPowerSw(FXObject*, FXSelector, void*);
  long onCmdShowDialog(FXObject*,FXSelector,void*); //Maier 

};


#endif // _INC_MRSVR_MAIN_WINDOW
