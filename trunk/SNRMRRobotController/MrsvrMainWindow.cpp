//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
// Copyright (C) 2005-2006 by Shiga University of Medical Science,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrMainWindow.cpp,v $
// $Revision: 1.19 $ 
// $Author: junichi $
// $Date: 2006/01/20 03:15:47 $
//====================================================================


//====================================================================
// Description:
//
//====================================================================


#include "MrsvrMainWindow.h"
#include "MrsvrTransform.h"
#include "MrsvrZFrameRegistrationDialog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace std;

// Map
FXDEFMAP(MrsvrMainWindow) MrsvrMainWindowMap[] = {
  //Message_Type___________ID_______________________________Message_Handler__________________
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_ABOUT,           MrsvrMainWindow::onCmdAbout),
  FXMAPFUNC(SEL_PAINT,    MrsvrMainWindow::ID_CANVAS,          MrsvrMainWindow::onCanvasRepaint),
  FXMAPFUNC(SEL_PAINT,    MrsvrMainWindow::ID_AXIAL_CANVAS,    MrsvrMainWindow::onAxialCanvasRepaint),
  FXMAPFUNC(SEL_PAINT,    MrsvrMainWindow::ID_NEEDLE_CANVAS,   MrsvrMainWindow::onNeedleCanvasRepaint),

  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_UPDATE_ACTUATOR_VOLTAGE,
                                                               MrsvrMainWindow::onUpdateActuatorVoltage),
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_UPDATE_MINI_PLOT,MrsvrMainWindow::onMiniPlotUpdate),
  FXMAPFUNC(SEL_TIMEOUT,  MrsvrMainWindow::ID_UPDATE_TIMER,    MrsvrMainWindow::onUpdateTimer),
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_START_UPDATE,    MrsvrMainWindow::onStartUpdate),
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_STOP_UPDATE,     MrsvrMainWindow::onStopUpdate),

  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_CMD_HOLD,        MrsvrMainWindow::onCmdHold),
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_CMD_ACTIVE,      MrsvrMainWindow::onCmdActive),
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_CMD_EMERGENCY,   MrsvrMainWindow::onCmdEmergency),
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_CMD_RESET,       MrsvrMainWindow::onCmdReset),

  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_CMD_CALIBRATE,   MrsvrMainWindow::onCmdCalibrate),  
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_CMD_START_COM,   MrsvrMainWindow::onCmdStartCom),
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_CMD_STOP_COM,    MrsvrMainWindow::onCmdStopCom),

  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_CMD_SAVE_DEFAULT,MrsvrMainWindow::onCmdSaveDefault),
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_CMD_RESET_DEFAULT,
                                                               MrsvrMainWindow::onCmdResetDefault),

  FXMAPFUNC(SEL_CHANGED,  MrsvrMainWindow::ID_CMD_CHOOSE_NEEDLE_INFO,
                                                               MrsvrMainWindow::onCmdChooseNeedleInfo),
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_CMD_ADD_NEEDLE_INFO,
                                                               MrsvrMainWindow::onCmdAddNeedleInfo),
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_CMD_UPDATE_NEEDLE_INFO,
                                                               MrsvrMainWindow::onCmdUpdateNeedleInfo),
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_CMD_DELETE_NEEDLE_INFO,
                                                               MrsvrMainWindow::onCmdDeleteNeedleInfo),

  FXMAPFUNCS(SEL_COMMAND, MrsvrMainWindow::ID_MANUAL_PW_OFF_0, MrsvrMainWindow::ID_MANUAL_PW_ON_4,
                          MrsvrMainWindow::onCmdManualPowerSw),
  FXMAPFUNCS(SEL_UPDATE,  MrsvrMainWindow::ID_MANUAL_PW_OFF_0, MrsvrMainWindow::ID_MANUAL_PW_ON_4,
                          MrsvrMainWindow::onUpdateManualPowerSw),

  FXMAPFUNC(SEL_LEFTBUTTONPRESS,
                          MrsvrMainWindow::ID_CALIB_LEFT_BTN,  MrsvrMainWindow::onCalibLeftBtnPressed),
  FXMAPFUNC(SEL_LEFTBUTTONPRESS,
                          MrsvrMainWindow::ID_CALIB_RIGHT_BTN, MrsvrMainWindow::onCalibRightBtnPressed),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,
                          MrsvrMainWindow::ID_CALIB_LEFT_BTN,  MrsvrMainWindow::onCalibLeftBtnReleased),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,
                          MrsvrMainWindow::ID_CALIB_RIGHT_BTN, MrsvrMainWindow::onCalibRightBtnReleased),
  FXMAPFUNC(SEL_LEFTBUTTONPRESS,
                          MrsvrMainWindow::ID_CALIB_ZERO_BTN,  MrsvrMainWindow::onCalibZeroBtnPressed),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,
                          MrsvrMainWindow::ID_CALIB_ZERO_BTN,  MrsvrMainWindow::onCalibZeroBtnReleased),

  FXMAPFUNC(SEL_CHANGED,  MrsvrMainWindow::ID_CMD_CHOOSE_END_EFFECTOR_CONFIG,
                                                               MrsvrMainWindow::onCmdChooseEndEffectorConfig),

  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,
                          MrsvrMainWindow::ID_SET_ENDEFFECT_BTN,   MrsvrMainWindow::onSetEndEffectorBtnReleased),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,
                          MrsvrMainWindow::ID_CANCEL_ENDEFFECT_BTN,MrsvrMainWindow::onCancelEndEffectorBtnReleased),

  FXMAPFUNC(SEL_LEFTBUTTONPRESS,
                          MrsvrMainWindow::ID_MANUAL_LEFT_BTN, MrsvrMainWindow::onManualLeftBtnPressed),
  FXMAPFUNC(SEL_LEFTBUTTONPRESS,
                          MrsvrMainWindow::ID_MANUAL_RIGHT_BTN,MrsvrMainWindow::onManualRightBtnPressed),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,
                          MrsvrMainWindow::ID_MANUAL_LEFT_BTN, MrsvrMainWindow::onManualLeftBtnReleased),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,
                          MrsvrMainWindow::ID_MANUAL_RIGHT_BTN,MrsvrMainWindow::onManualRightBtnReleased),

  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,
                          MrsvrMainWindow::ID_SET_TARGET_BTN,   MrsvrMainWindow::onSetTargetBtnReleased),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,
                          MrsvrMainWindow::ID_CANCEL_TARGET_BTN,MrsvrMainWindow::onCancelTargetBtnReleased),

  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_UPDATE_NEEDLE_APPR_OFFSET,
                                                               MrsvrMainWindow::onUpdateNeedleApprOffset),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,
                          MrsvrMainWindow::ID_SHUTDOWN_MRSVR_BTN,MrsvrMainWindow::onShutdownMrsvrBtnReleased),
  
  //FXMAPFUNC(SEL_PAINT,     MrsvrMainWindow::ID_IMAGEWINDOW,  MrsvrMainWindow::onImageRepaint)

  //Maier Show Dialog
  FXMAPFUNC(SEL_COMMAND,  MrsvrMainWindow::ID_SHOWDIALOG,      MrsvrMainWindow::onCmdShowDialog), 
};

// Object implementation
FXIMPLEMENT(MrsvrMainWindow, FXMainWindow, 
            MrsvrMainWindowMap, ARRAYNUMBER(MrsvrMainWindowMap));


const char* MrsvrMainWindow::quickPanelString[] = {
  "HOLD",
  //"PAUSE",            //hide Buttons Maier
  //"MOVE TO TARGET",   //hide Buttons Maier
  //"ACTIVE \nCONTROL",
  "ACTIVE",
  "EMERGENCY"
};

const char* MrsvrMainWindow::quickPanelGIF[] = {
  "icon/stop100x100.gif",
  //"icon/pause100x100.gif",  //hide Buttons Maier
  //"icon/start100x100.gif",  //hide Buttons Maier
  //"icon/manual100x100.gif",
  "icon/auto100x100.gif",   
  "icon/emergency100x100.gif"
};


const char* MrsvrMainWindow::autoCalibProcNameText[] = {
  "Home Position",
};


// warning text -- corresponds to MrsvrStatus
const char* MrsvrMainWindow::infoModeText[] = {
  "START-UP",
  "CALIBRATION",
  "HOLD",
  "ACTIVE",
  "REMOTE",
  "EMERGENCY",
  "RESET"
};

const char* MrsvrMainWindow::actuatorStatusText[] = {
  "OK",
  "ERROR: LOCKED",
  "ERROR: NO POWER",
  "WARNING: EXPIRED",
};

#define ACTUATOR_STATUS_TXT_OK       0
#define ACTUATOR_STATUS_TXT_LOCKED   1
#define ACTUATOR_STATUS_TXT_NOPOWER  2
#define ACTUATOR_STATUS_TXT_EXPIRED  3

const char* MrsvrMainWindow::infoWarnText[] = {
  "LOCKED",
  "OUT OF RANGE",
  "NO LOCATOR"
};

const float MrsvrMainWindow::positionOffsets[] = {
  0.0,      // x stage
  0.0,      // y stage
  0.0,      // z stage
  0.0,      // encoder theta
  0.0,      // encoder phi
};

const float MrsvrMainWindow::positionCoeffs[] = {
  1.0,      // x stage        (mm -> mm)
  1.0,      // y stage        (mm -> mm)
  1.0,      // z stage        (mm -> mm)
  180.0/PI, // encoder theta  (radian -> degree)
  180.0/PI, // encoder phi
};


#define SIZE_LOCAL_LOG_BUFFER   10240


MrsvrMainWindow::MrsvrMainWindow(FXApp* app, int w, int h) 
#ifdef FULLSCREEN
  : FXMainWindow(app, "MR Servo Robot Controller", NULL, NULL, DECOR_NONE, 0, 0,  w, h)
#else
  //  : FXMainWindow(app, "MR Servo Robot Controller", NULL, NULL, DECOR_ALL, 0, 0,  w-20, h-20)
  : FXMainWindow(app, "MR Servo Robot Controller", NULL, NULL, DECOR_ALL, 0, 0,  1260, 1004)
#endif // FULLSCREEN
{

  application = app;

  defNeedleInfo = NULL;

  extMsgSvr = NULL;
  setPointRAS = NULL;
  robotStatus = NULL;
  robotCommand = NULL; 
  robotLog = NULL;
  extMsgSvr = NULL;
  transform = NULL;

  infoCanvas = NULL;
  axialCanvas = NULL;
  needleCanvas = NULL;

  // prepare to share control and status information
  cerr << "loading registory..." << endl;
  loadRegistry();
  cerr << "attaching shared memory interface..." << endl;
  initSharedInfo();
  cerr << "Initializing paramters..." << endl;  
  initParameters();
  cerr << "Setting data targets..." << endl;  
  setDataTargets();
  // initialize the timer parameter
  updateInterval = 100;

//  // Menu bar
//  menubar  = new FXMenuBar(this, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);
//
//  // Menu : "File"
//  menuFile = new FXMenuPane(this);
//  new FXMenuTitle(menubar, "&File", NULL, menuFile);
//  new FXMenuCommand(menuFile, "Save Calibration Points",
//                    NULL, this, ID_SAVE_CALIB_POINTS);
//  new FXMenuSeparator(menuFile);
//  new FXMenuCommand(menuFile, "&Open File\tCtl-O\tOpen document file.",
//                    NULL, this, ID_FILEDLG_OPEN);
//  new FXMenuCommand(menuFile, "Save As...", NULL, this, ID_FILEDLG_SAVEAS);
//  new FXMenuCommand(menuFile, "Close", NULL, this, ID_FILE_CLOSE);
//  new FXMenuCommand(menuFile, "Exit", NULL, getApp(), FXApp::ID_QUIT, 0);
//
//  // Menu : "Help"
//  menuHelp = new FXMenuPane(this);
//  new FXMenuTitle(menubar, "&Help", NULL, menuHelp);
//  new FXMenuCommand(menuHelp, "&About This Software...", NULL, this, ID_ABOUT, 0);
//
  // Status bar
  statusbar = 
    new FXStatusBar(this, 
                    FRAME_RAISED|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|LAYOUT_RIGHT);
  FXMatrix* mtStatus = 
    new FXMatrix(statusbar,2,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|LAYOUT_RIGHT|
                 //LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_RIGHT,
                 LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW,
                 0, 0, 0, 0, 0, 0, 0, 0);

  textStatusProg = new FXLabel(mtStatus, " ", NULL, 
                               LAYOUT_FILL_Y|LAYOUT_FILL_X|
                               LAYOUT_LEFT|FRAME_SUNKEN|LAYOUT_FIX_WIDTH|
                               JUSTIFY_LEFT, 
                               0,0,500, 0);
  statusProg = new FXProgressBar(mtStatus,dtStatusProg,
                                 FXDataTarget::ID_VALUE,
                                 LAYOUT_SIDE_BOTTOM|LAYOUT_FIX_WIDTH|LAYOUT_FILL_X|
                                 FRAME_SUNKEN|LAYOUT_FILL_Y,
                                 0, 0, 150, 0);
  statusProg->setBarStyle(PROGRESSBAR_HORIZONTAL|PROGRESSBAR_PERCENTAGE);

  // Information display
  infoFont0 = new FXFont(this->getApp(), "helvetica", 9);
  infoFont1 = new FXFont(this->getApp(), "helvetica", 20);
  infoFont2 = new FXFont(this->getApp(), "helvetica", 26);
  infoCurrentMode   = -1;

  axialFont = new FXFont(this->getApp(), "helvetica", 9);

  //infoCurrentWarn   = -1;
  infoStartTime     = 0;

  for (int i = 0; i < NUM_WARNID; i ++) {
    infoWarning[i] = false;
    prevInfoWarning[i] = false;
  }

  fUpdateInfoMode   = 0;
  fUpdateInfoWarn   = 1;
  fUpdateInfoTarget = 0;
  fUpdateInfoTime   = 0;  
  fUpdateInfoRTime  = 0;  
  fUpdateInfoItem   = 1;

  infoOffs = new FXImage(this->getApp(), NULL, 
                         IMAGE_SHMI|IMAGE_SHMP|IMAGE_KEEP, INFCNV_W, INFCNV_H);


  FXVerticalFrame* frInfoDisplay  = 
    new FXVerticalFrame(this, LAYOUT_SIDE_TOP|
                        LAYOUT_FILL_X|FRAME_RAISED|
                        LAYOUT_FIX_HEIGHT, 0, 0, 1000, INFCNV_H);
  
  infoCanvas=new FXCanvas(frInfoDisplay,this,ID_CANVAS,
                          FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|
                          LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT, 0, 0, 
                          INFCNV_W, INFCNV_H);

  //// Main frame sectoin
  //
  //  +---------------------------------------------+
  //  |               Info Display                  |
  //  +--------------+------------------------------+
  //  |              |                              |
  //  |              |                              |
  //  |              |                              |
  //  |              |     H a r d w a r e          |
  //  |  Control     |                              |
  //  |       Panel  |           M o n i t o r      |
  //  |              |                              |
  //  |              |                              |
  //  |              |                              |
  //  +--------------+------------------------------+
  //  |   SW Panel   |        Quick Panel           |
  //  +--------------+------------------------------+
  //

  FXHorizontalFrame *frMain = 
    new FXHorizontalFrame(this,
                          LAYOUT_FILL_X|LAYOUT_FILL_Y);

  FXVerticalFrame *frMainLeft = 
    new FXVerticalFrame(frMain,LAYOUT_FILL_Y);

  // Control Panel
  buildControlPanel(frMainLeft);

  // SW Panel
  buildSwPanel(frMainLeft);

  // Hardware Monitor & Quick Panel
  FXVerticalFrame *frMainRight = 
    new FXVerticalFrame(frMain,
                        LAYOUT_FILL_X|LAYOUT_FILL_Y);

  buildHardwareMonitor(frMainRight);
  buildQuickPanel(frMainRight);

  //---------- Start screen updatnig ----------
  screenMode = SCR_NORMAL;
  application->addTimeout(this, MrsvrMainWindow::ID_UPDATE_TIMER, updateInterval); 

}



int MrsvrMainWindow::buildControlPanel(FXComposite* comp)
{
  FXVerticalFrame* frControlPanel = 
    new FXVerticalFrame(comp, 
                        LAYOUT_FILL_Y|/*LAYOUT_FILL_X|*/LAYOUT_FIX_WIDTH|FRAME_RAISED|
                        LAYOUT_TOP|LAYOUT_LEFT, 0, 0, 260, 0);
  shtControlPanel = 
    new FXShutter(frControlPanel, NULL, 0, 
                  FRAME_SUNKEN|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                  LAYOUT_TOP|LAYOUT_LEFT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

  siControlPanel[CTRL_PNL_COM] = 
    new FXShutterItem(shtControlPanel, "Communication", NULL, 
                      LAYOUT_FILL_X|LAYOUT_FILL_Y|
                      LAYOUT_TOP|LAYOUT_LEFT);
  buildCommunicationControlPanel(siControlPanel[CTRL_PNL_COM]->getContent());

//  siControlPanel[CTRL_PNL_PLOT] = 
//    new FXShutterItem(shtControlPanel, "Plotting", NULL, 
//                      LAYOUT_FILL_X|LAYOUT_FILL_Y|
//                      LAYOUT_TOP|LAYOUT_LEFT);
  siControlPanel[CTRL_PNL_CALIB] = 
    new FXShutterItem(shtControlPanel, "Calibration", NULL, 
                      LAYOUT_FILL_X|LAYOUT_FILL_Y|
                      LAYOUT_TOP|LAYOUT_LEFT);
  buildCalibrationControlPanel(siControlPanel[CTRL_PNL_CALIB]->getContent());

  siControlPanel[CTRL_PNL_CONFIG] = 
    new FXShutterItem(shtControlPanel, "Configuration", NULL, 
                      LAYOUT_FILL_X|LAYOUT_FILL_Y|
                      LAYOUT_TOP|LAYOUT_LEFT);
  buildConfigurationControlPanel(siControlPanel[CTRL_PNL_CONFIG]->getContent());

  
  //siControlPanel[CTRL_PNL_MANUAL] = 
  //  new FXShutterItem(shtControlPanel, "Hardware Control", NULL, 
  //                    LAYOUT_FILL_X|LAYOUT_FILL_Y|
  //                    LAYOUT_TOP|LAYOUT_LEFT);
  //buildManualControlPanel(siControlPanel[CTRL_PNL_MANUAL]->getContent());


  siControlPanel[CTRL_PNL_SYSTEM] = 
    new FXShutterItem(shtControlPanel, "System", NULL, 
                      LAYOUT_FILL_X|LAYOUT_FILL_Y|
                      LAYOUT_TOP|LAYOUT_LEFT);
  buildSystemControlPanel(siControlPanel[CTRL_PNL_SYSTEM]->getContent());
  return 1;
}


int MrsvrMainWindow::buildCommunicationControlPanel(FXComposite* comp)
{
  //---------- Server Group ---------- 
  FXGroupBox* gpSvr = 
    new FXGroupBox(comp, "Server for Remote Control",
                   LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_X);
  gpSvr->setBackColor(getApp()->getShadowColor());
  FXVerticalFrame* frSvr = 
    new FXVerticalFrame(gpSvr,
                        LAYOUT_FILL_Y|LAYOUT_FILL_X|
                        LAYOUT_TOP|LAYOUT_CENTER_X, 0, 0, 0, 0, 0, 0, 0, 0);
  frSvr->setBackColor(getApp()->getShadowColor());
  FXMatrix* mtSvr1 = 
    new FXMatrix(frSvr,2,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
                 LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|
                 LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  mtSvr1->setBackColor(getApp()->getShadowColor());
  FXLabel* lb = new FXLabel(mtSvr1, "Status:");
  lb->setBackColor(getApp()->getShadowColor());
  new FXTextField(mtSvr1,12,dtConStatus,
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_READONLY|JUSTIFY_CENTER_X|FRAME_SUNKEN, 
                  0, 0, 50, 15);

  gpSvr->setBackColor(getApp()->getShadowColor());
  FXMatrix* mtTcpIp = 
    new FXMatrix(frSvr,2,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
                 LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|
                 LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  mtTcpIp->setBackColor(getApp()->getShadowColor());
  
  lb = new FXLabel(mtTcpIp, "Port #:");
  lb->setBackColor(getApp()->getShadowColor());
  new FXTextField(mtTcpIp,10,dtConPortNo,
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_INTEGER|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  FXMatrix* mtSvr2 = 
    new FXMatrix(frSvr,2,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
                 LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_CENTER_X|
                 LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  mtSvr2->setBackColor(getApp()->getShadowColor());
  new FXButton(mtSvr2, "Start", NULL, this, 
               ID_CMD_START_COM,FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);
  new FXButton(mtSvr2, "Stop", NULL, this, 
               ID_CMD_STOP_COM,FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);

  return 1;
}


int MrsvrMainWindow::buildCalibrationControlPanel(FXComposite* comp)
{
  char name[128];
  FXLabel *lb;

  //////// Homeing
  FXGroupBox* gpManualCalib =
    new FXGroupBox(comp, "Homing",
                   LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_X);
  gpManualCalib->setBackColor(getApp()->getShadowColor());
  FXVerticalFrame *frManualCalib = 
    new FXVerticalFrame(gpManualCalib,
                          LAYOUT_FILL_X|LAYOUT_FILL_Y);
  frManualCalib->setBackColor(getApp()->getShadowColor());

  FXMatrix* mtManualCalib = 
    new FXMatrix(frManualCalib,4,
                   MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
                   LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|
                   LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);

  mtManualCalib->setBackColor(getApp()->getShadowColor());
  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    sprintf(name, "actuator #%d", i);
    FXLabel* lb = new FXLabel(mtManualCalib, name);
    lb->setBackColor(getApp()->getShadowColor());
    btCalibLeft[i] = new FXButton(mtManualCalib, "<<", NULL, this, ID_CALIB_LEFT_BTN,
                                  FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
                                  LAYOUT_CENTER_Y|LAYOUT_FILL_X);
    btCalibZero[i] = new FXButton(mtManualCalib, "Set 0", NULL, this, ID_CALIB_ZERO_BTN,
                               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
                               LAYOUT_CENTER_Y|LAYOUT_FILL_X);
    btCalibRight[i] = new FXButton(mtManualCalib, ">>", NULL, this, ID_CALIB_RIGHT_BTN,
                                  FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
                                  LAYOUT_CENTER_Y|LAYOUT_FILL_X);
  }

  new FXButton(frManualCalib, "Automatic Homing", NULL, this, 
               ID_CMD_CALIBRATE,FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);


  //////// Z-Frame Calibration
  FXGroupBox* gpZFrameCalib  = 
    new FXGroupBox(comp, "Z-Frame Calibration",
                   LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_X);
  gpZFrameCalib->setBackColor(getApp()->getShadowColor());

  FXVerticalFrame *frZFrameCalib = 
    new FXVerticalFrame(gpZFrameCalib,
                          LAYOUT_FILL_X|LAYOUT_FILL_Y);
  frZFrameCalib->setBackColor(getApp()->getShadowColor());

  FXMatrix* mtZFrameMatrix  = 
    new FXMatrix(frZFrameCalib,4,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
                 LAYOUT_TOP|LAYOUT_CENTER_X);

  mtZFrameMatrix->setBackColor(getApp()->getShadowColor());

  for (int i = 0; i < 16; i ++) {
    new FXTextField(mtZFrameMatrix,4,dtCalibrationMatrix[i],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|TEXTFIELD_READONLY|JUSTIFY_RIGHT|FRAME_SUNKEN,
                  0, 0, 20, 15);
  }

  new FXButton(frZFrameCalib, "Manual Z-Frame Calibration", NULL, this,
               ID_SHOWDIALOG,
               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);
  

  return 1;
}



int MrsvrMainWindow::buildConfigurationControlPanel(FXComposite* comp)
{
  FXGroupBox* gpEndEffector  = 
    new FXGroupBox(comp, "End Effector",
                   LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_X);
  gpEndEffector->setBackColor(getApp()->getShadowColor());

  FXMatrix* mtEndEffector = 
    new FXMatrix(gpEndEffector, 4,
                 MATRIX_BY_ROWS|LAYOUT_FILL_Y|
                 LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|
                 LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  mtEndEffector->setBackColor(getApp()->getShadowColor());

  FXMatrix* mtEndEffectorName =
    new FXMatrix(mtEndEffector,2,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
                 LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|
                 LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  mtEndEffectorName->setBackColor(getApp()->getShadowColor());

  FXLabel* lbcn = new FXLabel(mtEndEffectorName, "Name:");
  lbcn->setBackColor(getApp()->getShadowColor());

  lbEndEffectorName =new FXListBox(mtEndEffectorName,this,ID_CMD_CHOOSE_END_EFFECTOR_CONFIG,
                                   FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP,0,0,20,0);
  
  lbEndEffectorName->setNumVisible(5);
  NeedleInfo* p = defNeedleInfo;
  while (p) {
    lbEndEffectorName->appendItem(p->name);
    p = p->next;
  }

  FXMatrix* mtOffset = 
    new FXMatrix(mtEndEffector,3,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
                 LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|
                 LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  mtOffset->setBackColor(getApp()->getShadowColor());

  FXLabel* lba = new FXLabel(mtOffset, "Offset a:");
  lba->setBackColor(getApp()->getShadowColor());
  new FXTextField(mtOffset,5,dtNeedleOffset[0],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  lba = new FXLabel(mtOffset, "mm");
  lba->setBackColor(getApp()->getShadowColor());

  FXLabel* lbb = new FXLabel(mtOffset, "Offset b:");
  lbb->setBackColor(getApp()->getShadowColor());
  new FXTextField(mtOffset,5,dtNeedleOffset[1],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  lbb = new FXLabel(mtOffset, "mm");
  lbb->setBackColor(getApp()->getShadowColor());

  FXLabel* lbc = new FXLabel(mtOffset, "Offset c:");
  lbc->setBackColor(getApp()->getShadowColor());
  new FXTextField(mtOffset,5,dtNeedleOffset[2],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  lbc = new FXLabel(mtOffset, "mm");
  lbc->setBackColor(getApp()->getShadowColor());

  FXMatrix* mtOrient = 
    new FXMatrix(mtEndEffector,2,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
                 LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|
                 LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  mtOrient->setBackColor(getApp()->getShadowColor());
  FXLabel* lbo = new FXLabel(mtOrient, "Orient.: ");
  lbo->setBackColor(getApp()->getShadowColor());
  lbo = new FXLabel(mtOrient, " n = (na, nb, nc)");
  lbo->setBackColor(getApp()->getShadowColor());

  lbc = new FXLabel(mtOrient, "na = ");
  lbc->setBackColor(getApp()->getShadowColor());
  new FXTextField(mtOrient,5,dtNeedleOrientation[0],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 30, 15);

  lbc = new FXLabel(mtOrient, "nb = ");
  lbc->setBackColor(getApp()->getShadowColor());
  new FXTextField(mtOrient,5,dtNeedleOrientation[1],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 30, 15);

  lbc = new FXLabel(mtOrient, "nc = ");
  lbc->setBackColor(getApp()->getShadowColor());
  new FXTextField(mtOrient,5,dtNeedleOrientation[2],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 30, 15);

  FXMatrix* mtSetEndEffectorBtn = 
    new FXMatrix(mtEndEffector, 2,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
                 LAYOUT_CENTER_X);
  mtSetEndEffectorBtn->setBackColor(getApp()->getShadowColor());
  
  new FXButton(mtSetEndEffectorBtn, " Set ", NULL, this,
               ID_SET_ENDEFFECT_BTN,
               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);
  
  new FXButton(mtSetEndEffectorBtn, "Cancel", NULL, this,
               ID_CANCEL_ENDEFFECT_BTN,
               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);

  return 1;
}


//int MrsvrMainWindow::buildManualControlPanel(FXComposite* comp)
//{
//  for (int i = 0; i < NUM_ACTUATORS; i ++) {
//    char name[128];
//    sprintf(name, "actuator #%d", i);
//    FXGroupBox* gpTestActuator  = 
//      new FXGroupBox(comp, name,
//                     LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_X);
//    gpTestActuator->setBackColor(getApp()->getShadowColor());
//
//    FXVerticalFrame* frTestActuator = 
//      new FXVerticalFrame(gpTestActuator,
//                          LAYOUT_FILL_Y|LAYOUT_FILL_X|
//                          LAYOUT_TOP|LAYOUT_LEFT, 0, 0, 0, 0, 0, 0, 0, 0);
//    frTestActuator->setBackColor(getApp()->getShadowColor());
//
//    FXMatrix* mtTestActuator = 
//      new FXMatrix(frTestActuator,2,
//                   MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
//                   LAYOUT_FILL_X|LAYOUT_LEFT|
//                   LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
//    mtTestActuator->setBackColor(getApp()->getShadowColor());
//
//    /*
//    // actuator power switch
//    FXLabel* lb = new FXLabel(mtTestActuator, "POWER:");
//    lb->setBackColor(getApp()->getShadowColor());
//    //lb = new FXLabel(mtTestActuator, "[OFF]   [ON]");
//
//    FXMatrix *mtTestActuatorPw = 
//      new FXMatrix(mtTestActuator,3,FRAME_RIDGE|MATRIX_BY_COLUMNS|LAYOUT_FILL_X);
//    
//    new FXButton(mtTestActuatorPw,"OFF",NULL,this,ID_MANUAL_PW_OFF_0+i*2,
//                 BUTTON_TOOLBAR|JUSTIFY_CENTER_X|FRAME_RAISED|
//                 LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_COLUMN);
//    new FXButton(mtTestActuatorPw,"ON",NULL,this,ID_MANUAL_PW_ON_0+i*2,
//                 BUTTON_TOOLBAR|JUSTIFY_CENTER_X|FRAME_RAISED|
//                 LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_COLUMN);
//    */
//
//    // Voltage adjustor
//    FXLabel* lb = new FXLabel(mtTestActuator, "Vol:");
//    lb->setBackColor(getApp()->getShadowColor());
//
//    FXSlider* slider=new FXSlider(mtTestActuator,dtManualActuatorVol[i],FXDataTarget::ID_VALUE,
//                                  LAYOUT_TOP|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|
//                                  SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR|SLIDER_TICKS_BOTTOM,
//                                  0,0,120,20);
//    slider->setBaseColor(getApp()->getShadowColor());
//    slider->setBackColor(getApp()->getShadowColor());
//    slider->setRange(0,MANUAL_VOL_STEPS-1);
//
//    // Move buttons
//    lb = new FXLabel(mtTestActuator, "Control:");
//    lb->setBackColor(getApp()->getShadowColor());
//
//    FXHorizontalFrame* frTestActuatorBt = 
//      new FXHorizontalFrame(mtTestActuator, 
//                          //LAYOUT_FILL_Y|LAYOUT_FILL_X|
//                          LAYOUT_FILL_X|
//                          LAYOUT_TOP|LAYOUT_LEFT);
//    frTestActuatorBt->setBackColor(getApp()->getShadowColor());
//
////    FXMatrix* mtTestActuatorBt = 
////      new FXMatrix(frTestActuator,2,
////                   MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
////                   LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT,
////                   LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
////    mtTestActuatorBt->setBackColor(getApp()->getShadowColor());
////
//    btManualLeft[i] = 
//      new FXButton(frTestActuatorBt, "  <<  ", NULL, this, ID_MANUAL_LEFT_BTN,
//                   FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
//                   LAYOUT_CENTER_Y|LAYOUT_FILL_X);
//    btManualRight[i] = 
//      new FXButton(frTestActuatorBt, "  >>  ", NULL,  this, ID_MANUAL_RIGHT_BTN,
//                   FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
//                   LAYOUT_CENTER_Y|LAYOUT_FILL_X);
//
//  }
//
//  return 1;
//}


int MrsvrMainWindow::buildSystemControlPanel(FXComposite* comp)
{
  FXGroupBox* gpShutdown = 
    new FXGroupBox(comp, "Shutdown",
                   LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_X|
                   LAYOUT_CENTER_X);
  gpShutdown->setBackColor(getApp()->getShadowColor());
  new FXButton(gpShutdown, "Shutdown MRSVR", NULL, this,
               ID_SHUTDOWN_MRSVR_BTN,
               BUTTON_NORMAL|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);

//Maier Button to startup Nav without Slicer
//    FXGroupBox* gpNoSlicer = 
//    new FXGroupBox(comp, "Nav without Slicer",
//                   LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_X|
//                   LAYOUT_CENTER_X);
//    gpNoSlicer->setBackColor(getApp()->getShadowColor());
//    new FXButton(gpNoSlicer, "Nav without Slicer", NULL, this,
//               ID_SHOWDIALOG,
//               BUTTON_NORMAL|LAYOUT_CENTER_X|
//               LAYOUT_CENTER_Y|LAYOUT_FILL_X);
//End Maier
  return 1;
}


int MrsvrMainWindow::buildSwPanel(FXComposite* comp)
{
  FXHorizontalFrame* frActuatorSw =
    new FXHorizontalFrame(comp,
                          LAYOUT_FILL_X|FRAME_RAISED|
                          LAYOUT_TOP|LAYOUT_LEFT|
                          LAYOUT_FIX_WIDTH,  0, 0, 260, 0);
  FXGroupBox* gpActuatorSw = 
    new FXGroupBox(frActuatorSw, "POWER",
                   LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_X|
                   LAYOUT_CENTER_X);

  FXMatrix* mtActuatorSw = 
    new FXMatrix(gpActuatorSw,3,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|LAYOUT_FILL_X);

  for(int i = 0; i < NUM_ACTUATORS; i ++) {
    char str[16];
    sprintf(str, "Actuator #%d", i);
    new FXLabel(mtActuatorSw, str);
    new FXButton(mtActuatorSw,"OFF",NULL,this,ID_MANUAL_PW_OFF_0+i*2,
                 JUSTIFY_CENTER_X|FRAME_RAISED|
                 LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_COLUMN);
    new FXButton(mtActuatorSw,"ON",NULL,this,ID_MANUAL_PW_ON_0+i*2,
                 JUSTIFY_CENTER_X|FRAME_RAISED|
                 LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_COLUMN);
  }

  return 1;
}


int MrsvrMainWindow::buildHardwareMonitor(FXComposite* comp)
{

  //--------------------------------------------------------------------------------
  // Hardware/Software Monitor

  mainTab= new FXTabBook(comp,NULL,0,
                         LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXTabItem* tabHardware = 
    new FXTabItem(mainTab, 
                  "Hardware/Software Monitor\tActuator/Encoder status\tActuator/Sensor status", 
                  0,
                  LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT);

  FXVerticalFrame* frMonitor = 
    new FXVerticalFrame(mainTab, 
                          FRAME_RAISED|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                          LAYOUT_TOP|LAYOUT_LEFT, 0, 0, 0, 0, 0, 0, 0, 0);

  FXVerticalFrame* frMonitorUp = 
    new FXVerticalFrame(frMonitor, 
                        LAYOUT_FILL_X|
                        LAYOUT_TOP|LAYOUT_LEFT);

  FXGroupBox* gpActuatorsAndEncoders = 
  new FXGroupBox(frMonitorUp, "Actuators/Encoders",
                 FRAME_RIDGE|LAYOUT_SIDE_LEFT|LAYOUT_FILL_Y);

  FXVerticalFrame* frActuatorsAndEncoders = 
    new FXVerticalFrame(gpActuatorsAndEncoders, 
                        LAYOUT_FILL_X|
                        LAYOUT_TOP|LAYOUT_LEFT);

  FXHorizontalFrame* frActuators =
    new FXHorizontalFrame(frActuatorsAndEncoders,
                        LAYOUT_FILL_Y|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT);

  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    char name[128];
    sprintf(name, "actuator #%d", i);
    FXGroupBox* gpActuator  = 
      new FXGroupBox(frActuators, name,
                     LAYOUT_SIDE_TOP|FRAME_GROOVE);

    FXHorizontalFrame* frActuator = 
      new FXHorizontalFrame(gpActuator,
                            LAYOUT_FILL_Y||LAYOUT_TOP|LAYOUT_LEFT);

    FXVerticalFrame* frActuatorMeter =
      new FXVerticalFrame(frActuator,
                          LAYOUT_FILL_Y|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT);
    
    new FXProgressBar(frActuatorMeter,dtPrActuatorVol[i],FXDataTarget::ID_VALUE,
                      PROGRESSBAR_PERCENTAGE|PROGRESSBAR_DIAL|
                      LAYOUT_FILL_Y|LAYOUT_FIX_HEIGHT|
                      LAYOUT_FIX_WIDTH,0,0,60,60);

    FXMatrix* mtActuator = 
      new FXMatrix(frActuator,2,
                   MATRIX_BY_COLUMNS|LAYOUT_FILL_Y);
    new FXLabel(mtActuator, "Cum. Rev:");
    new FXTextField(mtActuator,16,dtTxActRev[i],
                    FXDataTarget::ID_VALUE,
                    TEXTFIELD_REAL|TEXTFIELD_READONLY|JUSTIFY_RIGHT|FRAME_SUNKEN, 
                    0, 0, 50, 15);

    new FXLabel(mtActuator, "Status:");
    new FXTextField(mtActuator,16,dtTxActStatus[i],
                    FXDataTarget::ID_VALUE,
                    TEXTFIELD_REAL|TEXTFIELD_READONLY|JUSTIFY_LEFT|FRAME_SUNKEN, 
                    0, 0, 50, 15);
  }

  FXHorizontalFrame* frEncoders =
    new FXHorizontalFrame(frActuatorsAndEncoders,
                            LAYOUT_FILL_Y|LAYOUT_FILL_X|
                            LAYOUT_TOP|LAYOUT_LEFT);

  for (int i = 0; i < NUM_ENCODERS; i ++) {
    char name[128];
    sprintf(name, "encoder #%d", i);
    FXGroupBox* gpEncoder  = 
      new FXGroupBox(frEncoders, name,
                     LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_X);
    FXMatrix* mtEncoder = 
      new FXMatrix(gpEncoder,2,
                   MATRIX_BY_ROWS|LAYOUT_FILL_Y|
                   LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|
                   LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
    FXMatrix* mtEncoderBar = 
      new FXMatrix(mtEncoder,4,
                   MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|
                   LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
    
    new FXLabel(mtEncoderBar, "Position:");
    new FXTextField(mtEncoderBar,6,dtTxPosition[i],
                    FXDataTarget::ID_VALUE,
                    TEXTFIELD_REAL|TEXTFIELD_READONLY|JUSTIFY_RIGHT|FRAME_SUNKEN, 
                    0, 0, 50, 15);
    new FXLabel(mtEncoderBar, robotStatus->getPosUnitName(i));
    new FXProgressBar(mtEncoderBar,dtPrPosition[i],FXDataTarget::ID_VALUE,
                      LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|FRAME_SUNKEN|
                      LAYOUT_FIX_HEIGHT, 0, 0, 400, 15);
    
    new FXLabel(mtEncoderBar, "Velocity:");
    new FXTextField(mtEncoderBar,6,dtTxVelocity[i],
                    FXDataTarget::ID_VALUE,
                    TEXTFIELD_REAL|TEXTFIELD_READONLY|JUSTIFY_RIGHT|FRAME_SUNKEN,
                    0, 0, 50, 15);
    new FXLabel(mtEncoderBar, robotStatus->getVelUnitName(i));
    new FXProgressBar(mtEncoderBar,dtPrVelocity[i],FXDataTarget::ID_VALUE,
                      LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|FRAME_SUNKEN|
                      LAYOUT_FIX_HEIGHT, 0, 0, 400, 15);
    
    //PROGRESSBAR_PERCENTAGE);
  }

  FXHorizontalFrame* frMonitorLo =
    new FXHorizontalFrame(frMonitor,
                          LAYOUT_FILL_Y|LAYOUT_FILL_X|
                          LAYOUT_TOP|LAYOUT_LEFT);

  //// Remote Controller Info.
  //FXGroupBox* gpRemoteInfo  = 
  //  new FXGroupBox(frMonitorLo, "Remote Controller Info.",
  //                 FRAME_RIDGE|LAYOUT_FILL_Y|
  //                 LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_TOP);
  //
  //FXMatrix* mtRemoteInfo =  
  //  new FXMatrix(gpRemoteInfo,2,
  //               MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
  //               LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|
  //               LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  //
  //new FXLabel(mtRemoteInfo, "Software:");
  //new FXTextField(mtRemoteInfo,18,dtRemoteSoftware,
  //                FXDataTarget::ID_VALUE,
  //                TEXTFIELD_REAL|TEXTFIELD_READONLY|JUSTIFY_LEFT|FRAME_SUNKEN,
  //                0, 0, 50, 15);
  //new FXLabel(mtRemoteInfo, "OS:");
  //new FXTextField(mtRemoteInfo,18,dtRemoteOS,
  //                FXDataTarget::ID_VALUE,
  //                TEXTFIELD_REAL|TEXTFIELD_READONLY|JUSTIFY_LEFT|FRAME_SUNKEN,
  //                0, 0, 50, 15);
  //new FXLabel(mtRemoteInfo, "Host name:");
  //new FXTextField(mtRemoteInfo,18,dtRemoteHostName,
  //                FXDataTarget::ID_VALUE,
  //                TEXTFIELD_REAL|TEXTFIELD_READONLY|JUSTIFY_LEFT|FRAME_SUNKEN,
  //                0, 0, 50, 15);
  //new FXLabel(mtRemoteInfo, "IP address:");
  //new FXTextField(mtRemoteInfo,18,dtRemoteIpAddr,
  //                FXDataTarget::ID_VALUE,
  //                TEXTFIELD_REAL|TEXTFIELD_READONLY|JUSTIFY_LEFT|FRAME_SUNKEN,
  //                0, 0, 50, 15);
  //new FXLabel(mtRemoteInfo, "Hostname:");
  //new FXTextField(mtRemoteInfo,18,dtRemoteHostName,
  //                FXDataTarget::ID_VALUE,
  //                TEXTFIELD_REAL|TEXTFIELD_READONLY|JUSTIFY_LEFT|FRAME_SUNKEN,
  //                0, 0, 50, 15);
  //new FXLabel(mtRemoteInfo, "Port number:");
  //new FXTextField(mtRemoteInfo,18,dtRemotePortNumber,
  //                FXDataTarget::ID_VALUE,
  //                TEXTFIELD_REAL|TEXTFIELD_READONLY|JUSTIFY_LEFT|FRAME_SUNKEN,
  //                0, 0, 50, 15);

  //// Calibration Information

  //FXGroupBox* gpCalibration  = 
  //  new FXGroupBox(frMonitorLo, "Calibration Info.",
  //                 FRAME_RIDGE|LAYOUT_FILL_Y|
  //                 LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_TOP);
  //
  //FXVerticalFrame* frCalibrationInfo =
  //  new FXVerticalFrame(gpCalibration,
  //                      LAYOUT_FILL_Y|LAYOUT_FILL_X|
  //                          LAYOUT_TOP|LAYOUT_LEFT);
  //
  //FXMatrix* mtCalibrationInfo =  
  //  new FXMatrix(frCalibrationInfo,2,
  //               MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|
  //               LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|
  //               LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  //
  //new FXLabel(mtCalibrationInfo, "Status:");
  //new FXTextField(mtCalibrationInfo,12,dtCalibrationStatus,
  //                FXDataTarget::ID_VALUE,
  //                TEXTFIELD_REAL|TEXTFIELD_READONLY|FRAME_SUNKEN,
  //                0, 0, 50, 15);
  //
  //updateTcpInfoTbl();


  //// Manual Control
  FXGroupBox* gpManualControl  = 
    new FXGroupBox(frMonitorLo, "Manual Control",
                   FRAME_RIDGE|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_TOP);

  FXHorizontalFrame* frManualControl =
    new FXHorizontalFrame(gpManualControl,
                          LAYOUT_FILL_Y|LAYOUT_FILL_X|
                          LAYOUT_TOP|LAYOUT_LEFT);
  

  FXMatrix* mtxManualControl = new FXMatrix(frManualControl,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y); 

  //Canvas RA-Position
  FXGroupBox* gpRAPos = 
    new FXGroupBox(mtxManualControl, "Position in Axial plane",
                   FRAME_RIDGE|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_LEFT);

  axialOffs = new FXImage(this->getApp(), NULL, 
                         IMAGE_SHMI|IMAGE_SHMP|IMAGE_KEEP, AXIALCNV_W, AXIALCNV_H);
  
  axialCanvas = new FXCanvas(gpRAPos,this,ID_AXIAL_CANVAS,
                             FRAME_SUNKEN|FRAME_THICK|LAYOUT_FIX_HEIGHT|LAYOUT_FIX_WIDTH|
                             LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN,
                             0,0,AXIALCNV_W,AXIALCNV_H);
  
  //RA-Position Sliders 
  FXGroupBox* gbPlateA = 
    new FXGroupBox(mtxManualControl, "A-P",
                   FRAME_RIDGE|LAYOUT_RIGHT|LAYOUT_FILL_Y);
  
  FXGroupBox* gbPlateR =
    new FXGroupBox(mtxManualControl, "R-A",
                   FRAME_RIDGE|LAYOUT_BOTTOM|LAYOUT_FILL_X);
  
  new FXButton(mtxManualControl, "Home",
               NULL,this,ID_PAINT_TARGET,BUTTON_NORMAL|LAYOUT_CENTER_X|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  
  
  FXRealSlider* slider=new FXRealSlider(gbPlateR,dtNormTargetPosition[0],FXDataTarget::ID_VALUE,
                                        LAYOUT_LEFT|LAYOUT_FIX_HEIGHT|LAYOUT_FIX_WIDTH|
                                        SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR,
                                        0,0,AXIALCNV_W,20);

  slider->setRange(0.0,1.0);
  FXRealSlider* slider2=new FXRealSlider(gbPlateA,dtNormTargetPosition[1],FXDataTarget::ID_VALUE,
                                         LAYOUT_TOP|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|
                                         SLIDER_VERTICAL|SLIDER_INSIDE_BAR,
                                         0,0,20,AXIALCNV_H);
  slider2->setRange(0.0,1.0);
  slider2->setIncrement(0.005);

  FXVerticalFrame* frManualControlRt =
    new FXVerticalFrame(frManualControl,
                          LAYOUT_FILL_Y|LAYOUT_FILL_X|
                          LAYOUT_TOP|LAYOUT_LEFT);

  FXHorizontalFrame* frTarget =
    new FXHorizontalFrame(frManualControlRt,
                          LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT);

  FXGroupBox* gbSlPos = 
    new FXGroupBox(frTarget, "Current",
                   FRAME_RIDGE|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_LEFT);
  
  FXMatrix* mtxCurrentTarget=new FXMatrix(gbSlPos,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|
                                                 LAYOUT_FILL_X|LAYOUT_FILL_Y);
  
  new FXLabel(mtxCurrentTarget,"R",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(mtxCurrentTarget,10,dtCurrentPosition[0],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN|TEXTFIELD_READONLY, 
                  0, 0, 50, 15);
  
  new FXLabel(mtxCurrentTarget,"A",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(mtxCurrentTarget,10,dtCurrentPosition[1],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN|TEXTFIELD_READONLY, 
                  0, 0, 50, 15);
  
  new FXLabel(mtxCurrentTarget,"S",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(mtxCurrentTarget,10,dtCurrentPosition[2],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN|TEXTFIELD_READONLY, 
                  0, 0, 50, 15);
  
  new FXLabel(frTarget,"+",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  FXGroupBox* gbDeltaTarget = 
    new FXGroupBox(frTarget, "Delta",
                   FRAME_RIDGE|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_LEFT);
  
  FXMatrix* mtxTargetDelta=new FXMatrix(gbDeltaTarget,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|
                                                LAYOUT_FILL_X|LAYOUT_FILL_Y);
  
  new FXLabel(mtxTargetDelta,"R",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  new FXTextField(mtxTargetDelta,10,dtDeltaPosition[0],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  new FXLabel(mtxTargetDelta,"A",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(mtxTargetDelta,10,dtDeltaPosition[1],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  
  new FXLabel(mtxTargetDelta,"S",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(mtxTargetDelta,10,dtDeltaPosition[2],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  
  new FXLabel(frTarget,"=",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  FXGroupBox* gbSumTarget = 
    new FXGroupBox(frTarget, "New Target",
                   FRAME_RIDGE|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_LEFT);
  
  FXMatrix* mtxNewTarget=new FXMatrix(gbSumTarget,2,MATRIX_BY_COLUMNS|LAYOUT_SIDE_TOP|
                                              LAYOUT_FILL_X|LAYOUT_FILL_Y);

  new FXLabel(mtxNewTarget,"R",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(mtxNewTarget,10,dtTargetPosition[0],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  
  new FXLabel(mtxNewTarget,"A",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(mtxNewTarget,10,dtTargetPosition[1],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  

  new FXLabel(mtxNewTarget,"S",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);
  
  new FXTextField(mtxNewTarget,10,dtTargetPosition[2],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);

  new FXLabel(frTarget," ",NULL,LAYOUT_CENTER_Y|LAYOUT_CENTER_X|JUSTIFY_RIGHT|LAYOUT_FILL_ROW);

  char buf[4096];
  FILE* fp = fopen("icon/start100x100.gif", "rb");
  fread(buf, 1, 4096, fp);
  fclose(fp);

  new FXButton(frTarget, "Move to Target",
               new FXGIFIcon(this->getApp(), (void*)buf),this,MrsvrMainWindow::ID_SET_TARGET_BTN,
               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);

  needleOffs = new FXImage(this->getApp(), NULL, 
                         IMAGE_SHMI|IMAGE_SHMP|IMAGE_KEEP, NEEDLE_CANVAS_W, NEEDLE_CANVAS_H);

  needleCanvas = new FXCanvas(frManualControlRt,this,ID_NEEDLE_CANVAS,
                              FRAME_SUNKEN|FRAME_THICK|LAYOUT_FIX_HEIGHT|LAYOUT_FIX_WIDTH|
                              LAYOUT_FILL_ROW|LAYOUT_FILL_COLUMN,
                              0,0,NEEDLE_CANVAS_W, NEEDLE_CANVAS_H);

  
  //new FXButton(frTarget,"&Move to Target",NULL,this,MrsvrMainWindow::ID_SET_TARGET_BTN,FRAME_RAISED|
  //             FRAME_THICK| LAYOUT_CENTER_X|LAYOUT_CENTER_Y|LAYOUT_FILL_X);


  


  //--------------------------------------------------------------------------------
  // RTCP Log Tab

  FXTabItem* tabRtcpLog = new FXTabItem(mainTab, 
                                    "RTCP Log\tRTCP Log\tReal-time Control Process Message",
                                    0,
                                    LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT);
  FXHorizontalFrame* frLog = 
    new FXHorizontalFrame(mainTab,
                          FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y|
                          LAYOUT_TOP|LAYOUT_LEFT);

  textRtcpLog = new FXText(frLog, NULL, 0,
                       FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  textRtcpLog->setVisibleColumns(60);
  textRtcpLog->setEditable(FALSE);


  FXTabItem* tabInterfaceLog = new FXTabItem(mainTab, 
                                    "Interface Log\tInterface Log\tInterface Log massages",
                                    0,
                                    LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT);
  FXHorizontalFrame* frIfLog = 
    new FXHorizontalFrame(mainTab,
                          FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y|
                          LAYOUT_TOP|LAYOUT_LEFT);

  textInterfaceLog = new FXText(frIfLog, NULL, 0,
                       FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  textInterfaceLog->setVisibleColumns(60);
  textInterfaceLog->setEditable(FALSE);

  
  FXTabItem* tabPlot = new FXTabItem(mainTab, 
                                     "Motion Plotter\tMotion Plotter\tActuator Motion Plot",
                        0,
                        LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT);

  FXVerticalFrame* frPlot = 
    new FXVerticalFrame(mainTab,
                          FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y|
                          LAYOUT_TOP|LAYOUT_LEFT);

  // Translation plot
  FXGroupBox* gpPlotA = 
    new FXGroupBox(frPlot, "Translations",
                   FRAME_RIDGE|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_LEFT);

  FXHorizontalFrame* frPlotA = 
    new FXHorizontalFrame(gpPlotA,
                          /*FRAME_SUNKEN|FRAME_THICK|*/LAYOUT_FILL_X|
                          LAYOUT_FILL_Y,
                          0,0,400,400, 0,0,0,0);

  FXHorizontalFrame* frPlotCanvasA = 
    new FXHorizontalFrame(frPlotA,
                          FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|
                          LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);
  plotCanvasA = 
    new MrsvrPlotCanvas(frPlotCanvasA, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|
                        LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT);
                        //LAYOUT_FIX_HEIGHT|LAYOUT_FIX_WIDTH,
                         //0,0, 500, 300);

  plotCanvasA->setPlotMode(MrsvrPlotCanvas::MODE_SWEEP);
  plotCanvasA->newData(500, "Encoder #0");
  plotCanvasA->newData(500, "Encoder #1");
  plotCanvasA->newData(500, "Encoder #2");
  plotCanvasA->setLineProperty(0, FXRGB(255,0,0), LINE_SOLID, 1);
  plotCanvasA->setLineProperty(1, FXRGB(0,0,255), LINE_SOLID, 1);
  plotCanvasA->setLineProperty(2, FXRGB(255,0,255), LINE_SOLID, 1);

  plotCanvasA->setLabel("Time [ms]", "Translation [mm]");
  plotCanvasA->setPlotMode(MrsvrPlotCanvas::MODE_SWEEP);

  //plotCanvasA->setAxisScaleY(0.0001*4, 20);
  plotCanvasA->setAxisScaleX(0.002, 50, 20);
  
  FXVerticalFrame* frPlotADial1 = 
    new FXVerticalFrame(frPlotA,LAYOUT_FILL_Y,0, 0, 0, 0, 0, 0, 0, 0);

  FXDial* plotAGainDial = 
    new FXDial(frPlotADial1,dtPlotAGain, FXDataTarget::ID_VALUE,
               FRAME_SUNKEN|FRAME_THICK|DIAL_VERTICAL|LAYOUT_FIX_WIDTH|
               /*LAYOUT_FIX_HEIGHT|*/LAYOUT_CENTER_Y|DIAL_HAS_NOTCH|
               LAYOUT_FILL_Y|LAYOUT_CENTER_X,
               0,0,14,200,0,0,0,0);
  valPlotAGain = 100;
  plotAGainDial->setRange(100, 750);
  plotAGainDial->setNotchOffset(0);

  new FXLabel(frPlotADial1, "Gn", NULL, LABEL_NORMAL|LAYOUT_CENTER_X,
              0, 0, 0, 0, 0, 0, 0, 0);

  FXVerticalFrame* frPlotADial2 = 
    new FXVerticalFrame(frPlotA,LAYOUT_FILL_Y,0, 0, 0, 0, 0, 0, 0, 0);
  FXDial* plotAShiftDial = 
    new FXDial(frPlotADial2,dtPlotAShift, FXDataTarget::ID_VALUE,
               FRAME_SUNKEN|FRAME_THICK|DIAL_VERTICAL|LAYOUT_FIX_WIDTH|
               /*LAYOUT_FIX_HEIGHT|*/LAYOUT_CENTER_Y|DIAL_HAS_NOTCH|
               LAYOUT_FILL_Y|LAYOUT_CENTER_X,
               0,0,14,200,0,0,0,0);
  new FXLabel(frPlotADial2, "Sh", NULL, LABEL_NORMAL|LAYOUT_CENTER_X,
              0, 0, 0, 0, 0, 0, 0, 0);
  plotAShiftDial->setRange(-100, 100);
  plotAShiftDial->setNotchOffset(0);


  // Rotation plot
  FXGroupBox* gpPlotB = 
    new FXGroupBox(frPlot, "Rotations",
                   FRAME_RIDGE|LAYOUT_FILL_Y|LAYOUT_FILL_X|
                   LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_SIDE_LEFT);

  FXHorizontalFrame* frPlotB = 
    new FXHorizontalFrame(gpPlotB,
                          /*FRAME_SUNKEN|FRAME_THICK|*/LAYOUT_FILL_X|
                          LAYOUT_FILL_Y,
                          0,0,400,400, 0,0,0,0);

  FXHorizontalFrame* frPlotCanvasB = 
    new FXHorizontalFrame(frPlotB,
                          FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|
                          LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);
  plotCanvasB = 
    new MrsvrPlotCanvas(frPlotCanvasB, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|
                        LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT);

  plotCanvasB->setPlotMode(MrsvrPlotCanvas::MODE_SWEEP);
  plotCanvasB->newData(500, "Encoder #3");
  plotCanvasB->newData(500, "Encoder #4");
  plotCanvasB->setLineProperty(0, FXRGB(255,0,0), LINE_SOLID, 1);
  plotCanvasB->setLineProperty(1, FXRGB(0,0,255), LINE_SOLID, 1);

  plotCanvasB->setLabel("Time [ms]", "Rotation [rad]");
  plotCanvasB->setPlotMode(MrsvrPlotCanvas::MODE_SWEEP);

  //plotCanvasB->setAxisScaleY(0.0001*4, 20);
  plotCanvasB->setAxisScaleX(0.002, 50, 20);
  
  FXVerticalFrame* frPlotBDial1 = 
    new FXVerticalFrame(frPlotB,LAYOUT_FILL_Y,0, 0, 0, 0, 0, 0, 0, 0);

  FXDial* plotBGainDial = 
    new FXDial(frPlotBDial1,dtPlotBGain, FXDataTarget::ID_VALUE,
               FRAME_SUNKEN|FRAME_THICK|DIAL_VERTICAL|LAYOUT_FIX_WIDTH|
               /*LAYOUT_FIX_HEIGHT|*/LAYOUT_CENTER_Y|DIAL_HAS_NOTCH|
               LAYOUT_FILL_Y|LAYOUT_CENTER_X,
               0,0,14,200,0,0,0,0);
  valPlotBGain = 100;
  plotBGainDial->setRange(100, 750);
  plotBGainDial->setNotchOffset(0);

  new FXLabel(frPlotBDial1, "Gn", NULL, LABEL_NORMAL|LAYOUT_CENTER_X,
              0, 0, 0, 0, 0, 0, 0, 0);

  FXVerticalFrame* frPlotBDial2 = 
    new FXVerticalFrame(frPlotB,LAYOUT_FILL_Y,0, 0, 0, 0, 0, 0, 0, 0);
  FXDial* plotBShiftDial = 
    new FXDial(frPlotBDial2,dtPlotBShift, FXDataTarget::ID_VALUE,
               FRAME_SUNKEN|FRAME_THICK|DIAL_VERTICAL|LAYOUT_FIX_WIDTH|
               /*LAYOUT_FIX_HEIGHT|*/LAYOUT_CENTER_Y|DIAL_HAS_NOTCH|
               LAYOUT_FILL_Y|LAYOUT_CENTER_X,
               0,0,14,200,0,0,0,0);
  new FXLabel(frPlotBDial2, "Sh", NULL, LABEL_NORMAL|LAYOUT_CENTER_X,
              0, 0, 0, 0, 0, 0, 0, 0);
  plotBShiftDial->setRange(-100, 100);
  plotBShiftDial->setNotchOffset(0);


  onMiniPlotUpdate(NULL, (FXSelector) NULL, NULL);

  FXTabItem* tabCustom = new FXTabItem(mainTab, 
                                    "Customize\tCustomize\tCustomize the software",
                                    0,
                                    LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT);

  //  FXMatrix* mtLog = 
  //    new FXMatrix(mainTab,2,
  //                 FRAME_RAISED|MATRIX_BY_COLUMNS|
  //                 LAYOUT_FILL_Y|LAYOUT_FILL_X|
  //                 LAYOUT_TOP|LAYOUT_LEFT);
  FXHorizontalFrame* frCustom = 
    new FXHorizontalFrame(mainTab,
                          FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y|
                          LAYOUT_TOP|LAYOUT_LEFT);

  FXGroupBox* gpDefaults  = 
    new FXGroupBox(frCustom, "Default paramters",
                   LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_Y);
  FXVerticalFrame* frDefaults = 
    new FXVerticalFrame(gpDefaults,
                          LAYOUT_FILL_X|LAYOUT_FILL_Y|
                          LAYOUT_TOP|LAYOUT_LEFT);
  FXGroupBox* gpComParam  = 
    new FXGroupBox(frDefaults, "Communication",
                   LAYOUT_SIDE_TOP|FRAME_GROOVE);
  FXMatrix* mtComParam = 
    new FXMatrix(gpComParam,2,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y);

  new FXLabel(mtComParam, "Remote Control Server port #:");
  new FXTextField(mtComParam,12,dtDefConPortNo,
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_INTEGER|JUSTIFY_RIGHT|JUSTIFY_RIGHT,
                  0, 0, 50, 15);
  new FXLabel(mtComParam, "Locator Server host:");
  new FXTextField(mtComParam,12,dtDefLocServHostName,
                  FXDataTarget::ID_VALUE,
                  JUSTIFY_LEFT|FRAME_SUNKEN, 
                  0, 0, 50, 15);
  new FXLabel(mtComParam, "Locator Server port #:");
  new FXTextField(mtComParam,12,dtDefLocServPortNo,
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_INTEGER|JUSTIFY_RIGHT|JUSTIFY_RIGHT,
                  0, 0, 50, 15);
  new FXLabel(mtComParam, "Locator interval(ms):");
  new FXTextField(mtComParam,12,dtDefLocServInterval,
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_INTEGER|JUSTIFY_RIGHT|JUSTIFY_RIGHT,
                  0, 0, 50, 15);

  FXGroupBox* gpCalibParam  = 
    new FXGroupBox(frDefaults, "Calibration",
                   LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_X);
  FXVerticalFrame *frCalibParam = 
    new FXVerticalFrame(gpCalibParam,
                        LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_FILL_X);
  FXGroupBox* gpDefCalibProcs  = 
    new FXGroupBox(frCalibParam, "Auto Calibration",
                   LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_X);

  FXMatrix* mtDefCalibProcs = 
    new FXMatrix(gpDefCalibProcs,2,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|LAYOUT_FILL_X);
  
  for (int i = 0; i < NUM_PROC_AUTOCALIB; i ++) {
    new FXLabel(mtDefCalibProcs, autoCalibProcNameText[i]);
    FXPopup* popup = new FXPopup(this);
    new FXOption(popup,"Disable",NULL,dtDefAutoCalibProcSelect[i],FXDataTarget::ID_OPTION+0,
                 JUSTIFY_HZ_APART|ICON_AFTER_TEXT);    
    new FXOption(popup,"Enable",NULL,dtDefAutoCalibProcSelect[i],FXDataTarget::ID_OPTION+1,
                 JUSTIFY_HZ_APART|ICON_AFTER_TEXT);
    FXOptionMenu *options=new FXOptionMenu(mtDefCalibProcs,popup,
                                           LAYOUT_TOP|FRAME_RAISED|FRAME_THICK|JUSTIFY_HZ_APART|
                                           ICON_AFTER_TEXT);
    options->setTarget(dtDefAutoCalibProcSelect[i]);
    options->setSelector(FXDataTarget::ID_VALUE);
  }

  FXGroupBox* gpDefCalibPoints  = 
    new FXGroupBox(frCalibParam, "Calibration Points",
                   LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_X);

  FXMatrix* mtDefCalibPoints = 
    new FXMatrix(gpDefCalibPoints,4,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|LAYOUT_FILL_X);

  new FXLabel(mtDefCalibPoints, " ");
  new FXLabel(mtDefCalibPoints, "X");
  new FXLabel(mtDefCalibPoints, "Y");
  new FXLabel(mtDefCalibPoints, "Z");

  for (int i = 0; i < NUM_CALIB_POINTS; i ++) {
    char str[16];
    sprintf(str, "#%d", i);
    new FXLabel(mtDefCalibPoints, str);
    for (int j = 0; j < 3; j ++) {
      new FXTextField(mtDefCalibPoints,4,dtDefAutoCalibPoints[i][j],
                      FXDataTarget::ID_VALUE,
                      TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                      FRAME_SUNKEN, 
                      0, 0, 50, 15);
    }
  }

  FXMatrix* mtDefSetButtons = 
    new FXMatrix(frDefaults,2,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y|LAYOUT_FILL_X);
  new FXButton(mtDefSetButtons, "Save", NULL, this, 
               ID_CMD_SAVE_DEFAULT,FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);
  new FXButton(mtDefSetButtons, "Reset", NULL, this, 
               ID_CMD_RESET_DEFAULT,FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);
  

  FXGroupBox* gpDevConf  = 
    new FXGroupBox(frCustom, "Device configurations",
                   LAYOUT_SIDE_TOP|FRAME_GROOVE|LAYOUT_FILL_Y);
  FXVerticalFrame* frDevConf = 
    new FXVerticalFrame(gpDevConf,
                          LAYOUT_FILL_X|LAYOUT_FILL_Y|
                          LAYOUT_TOP|LAYOUT_LEFT);
  FXGroupBox* gpEndEffector  = 
    new FXGroupBox(frDevConf, "End effector",
                   LAYOUT_SIDE_TOP|FRAME_GROOVE);
  FXVerticalFrame* frEndEffector = 
    new FXVerticalFrame(gpEndEffector,
                          LAYOUT_FILL_X|LAYOUT_FILL_Y|
                          LAYOUT_TOP|LAYOUT_LEFT);
  FXMatrix* mtEndEffector = 
    new FXMatrix(frEndEffector,2,
                 MATRIX_BY_COLUMNS|LAYOUT_FILL_Y);

  new FXLabel(mtEndEffector, "Name:");
  lbEndEffector =new FXListBox(mtEndEffector,this,ID_CMD_CHOOSE_NEEDLE_INFO,
                                       FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_FILL_X,0,0,200,0);
  
  lbEndEffector->setNumVisible(5);
  NeedleInfo* p = defNeedleInfo;

  while (p) {
    lbEndEffector->appendItem(p->name);
    p = p->next;
  }

  valNeedleConfName = defNeedleInfo->name;
  valDefNeedleOffset[0] = defNeedleInfo->valOffset[0];
  valDefNeedleOffset[1] = defNeedleInfo->valOffset[1];
  valDefNeedleOffset[2] = defNeedleInfo->valOffset[2];
  valDefNeedleOrientation[0] = defNeedleInfo->valOrient[0];
  valDefNeedleOrientation[1] = defNeedleInfo->valOrient[1];
  valDefNeedleOrientation[2] = defNeedleInfo->valOrient[2];

  new FXLabel(mtEndEffector, "Name:");
  new FXTextField(mtEndEffector ,20, dtNeedleConfName,
                  FXDataTarget::ID_VALUE, JUSTIFY_LEFT|FRAME_SUNKEN,
                  0, 0, 100, 15);

  new FXLabel(mtEndEffector, "Offset A (mm):");
  new FXTextField(mtEndEffector,5,dtDefNeedleOffset[0],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  //new FXLabel(mtEndEffector, "mm");

  new FXLabel(mtEndEffector, "Offset B (mm):");
  new FXTextField(mtEndEffector,5,dtDefNeedleOffset[1],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  //new FXLabel(mtEndEffector, "mm");

  new FXLabel(mtEndEffector, "Offset C (mm):");
  new FXTextField(mtEndEffector,5,dtDefNeedleOffset[2],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);
  //new FXLabel(mtEndEffector, "mm");


  new FXLabel(mtEndEffector, "Orient na:");
  new FXTextField(mtEndEffector,5,dtDefNeedleOrientation[0],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);

  new FXLabel(mtEndEffector, "Orient nb:");
  new FXTextField(mtEndEffector,5,dtDefNeedleOrientation[1],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);

  new FXLabel(mtEndEffector, "Orient nc:");
  new FXTextField(mtEndEffector,5,dtDefNeedleOrientation[2],
                  FXDataTarget::ID_VALUE,
                  TEXTFIELD_REAL|JUSTIFY_RIGHT|JUSTIFY_RIGHT|
                  FRAME_SUNKEN, 
                  0, 0, 50, 15);


  FXHorizontalFrame* frEndEffectorBtn = 
    new FXHorizontalFrame(frEndEffector,
                          LAYOUT_FILL_X|LAYOUT_FILL_Y|
                          LAYOUT_TOP|LAYOUT_LEFT);
  new FXButton(frEndEffectorBtn, "Add", NULL, this,
               ID_CMD_ADD_NEEDLE_INFO,
               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);

  new FXButton(frEndEffectorBtn, "Update", NULL, this,
               ID_CMD_UPDATE_NEEDLE_INFO,
               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);

  new FXButton(frEndEffectorBtn, "Delete", NULL, this,
               ID_CMD_DELETE_NEEDLE_INFO,
               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);

  mainTab->setTabStyle(TABBOOK_TOPTABS);
  tabHardware->setTabOrientation(TAB_TOP);
  tabRtcpLog->setTabOrientation(TAB_TOP);
  tabInterfaceLog->setTabOrientation(TAB_TOP);
  tabPlot->setTabOrientation(TAB_TOP);
  tabCustom->setTabOrientation(TAB_TOP);

  return 1;
}


int MrsvrMainWindow::buildQuickPanel(FXComposite* comp)
{
  FXHorizontalFrame* frQuickButton =
    new FXHorizontalFrame(comp,
                          LAYOUT_FILL_X|FRAME_RAISED|
                          LAYOUT_TOP|LAYOUT_LEFT);
  
  //int id = ID_CMD_HOLD;
  //for (int i = 0; i < nQuickPanelItems; i ++) {
  //  char buf[4096];
  //  FILE* fp = fopen(quickPanelGIF[i], "rb");
  //  fread(buf, 1, 4096, fp);
  //  fclose(fp);
  //  new FXButton(frQuickButton, quickPanelString[i],
  //               new FXGIFIcon(this->getApp(), (void*)buf),this,id,
  //               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
  //               LAYOUT_CENTER_Y|LAYOUT_FILL_X);
  //  id ++;
  //}
  char buf[4096];

  // HOLD
  FILE* fp = fopen(quickPanelGIF[0], "rb");
  fread(buf, 1, 4096, fp);
  fclose(fp);
  new FXButton(frQuickButton, quickPanelString[0],
               new FXGIFIcon(this->getApp(), (void*)buf),this,ID_CMD_HOLD,
               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);

  // Active
  fp = fopen(quickPanelGIF[1], "rb");
  fread(buf, 1, 4096, fp);
  fclose(fp);
  new FXButton(frQuickButton, quickPanelString[1],
               new FXGIFIcon(this->getApp(), (void*)buf),this,ID_CMD_ACTIVE,
               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);

  // Space
  new FXLabel(frQuickButton, " ");

  // Emergency
  fp = fopen(quickPanelGIF[2], "rb");
  fread(buf, 1, 4096, fp);
  fclose(fp);
  new FXButton(frQuickButton, quickPanelString[2],
               new FXGIFIcon(this->getApp(), (void*)buf),this,ID_CMD_EMERGENCY,
               FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X|
               LAYOUT_CENTER_Y|LAYOUT_FILL_X);
  
  return 1;
}


// Find a object from a object table and return its index number.
// Return -1 if the object cannot be found in the object table.
int MrsvrMainWindow::getObjectIndex(FXObject* obj, FXObject** tbl, int num)
{
  for (int i = 0; i < num; i ++) {
    if (obj == tbl[i]) {
      return i;
    }
  }
  return -1;
}



int MrsvrMainWindow::updateTcpInfoTbl()
{
  //char buf[128];
  if (extMsgSvr && 
      extMsgSvr->getSvrStatus() != prevSvrStatus) {
    prevSvrStatus = extMsgSvr->getSvrStatus();
    //valRemotePortNumber.format("%d", extMsgSvr->getRemotePort());
    
  }

  return 1;
}


MrsvrMainWindow::~MrsvrMainWindow()
{
  delete mainTab;
}



int MrsvrMainWindow::initSharedInfo()
{
  //setPointRAS  = new MrsvrRASReader(SHM_RAS_SETPOINT);
  robotStatus  = new MrsvrStatusReader(SHM_STATUS);
  robotCommand = new MrsvrCommandWriter(SHM_COMMAND);
  robotLog     = new MrsvrLogReader(SHM_LOG);
  robotCommand->setMode(MrsvrStatus::HOLD);
  
  for (int i = 0; i < 3; i ++) {
    robotCommand->setTipOffset(i, 0);
  }

  for(int i = 0; i < NUM_ACTUATORS; i ++) {
    robotCommand->setLmtVoltage(i, INITIAL_VOL);
  }

  return 0;
} 


void MrsvrMainWindow::initParameters()
{
  int i;

  debugLevel = 2;
  setDefaultParameters();

  // initialize transform matrix class
  transform = new MrsvrTransform();
  for (int i = 0; i < 16; i ++) {
    valCalibrationMatrix[i] = (i%5 == 0) ? 1.0 : 0.0;
  }

  transform->setCalibrationMatrix(valCalibrationMatrix);

  // actuators' parameters
  for (i = 0; i < NUM_ACTUATORS; i ++) {
    // values
    valActuatorVol[i] = 0.0;
    valPrActuatorVol[i] = 0;
    valManualActuatorVol[i] = INITIAL_VOL;
  }

  // encorders' parameters
  for (i = 0; i < NUM_ENCODERS; i ++) {
   // values
    valPosition[i] = 0.0*positionCoeffs[i] + positionOffsets[i];
    valSetPoint[i] = 0.0*positionCoeffs[i] + positionOffsets[i];
    valVelocity[i] = 0.0*positionCoeffs[i] + positionOffsets[i];
    valPrPosition[i] = POS2PROG(valPosition[i]);
    valPrSetPoint[i] = POS2PROG(valSetPoint[i]);
    valPrVelocity[i] = VEL2PROG(valVelocity[i]);
  }

  // for status progress meter
  valStatusProg = 0;

  // for manual->power switch
  for (i = 0;i < NUM_ACTUATORS; i ++) {
    manualPowerSw[i] = 1;
    robotCommand->activateActuator(i);
    valActStatus[i] = actuatorStatusText[ACTUATOR_STATUS_TXT_OK];
  }
  
}


void MrsvrMainWindow::loadRegistry()
{
  // load initial values from FOX registry
  // Communication paramters
  valDefConPortNo
    = getApp()->reg().readIntEntry("COMMUNICATION", "REMOTE_CONTROL_SERVER_PORT",
                                   DEFAULT_SVR_PORT);

//  valDefLocServPortNo
//    = getApp()->reg().readIntEntry("COMMUNICATION", "LOCATOR_SERVER_PORT",
//                                   0);
//  valDefLocServHostName
//    = getApp()->reg().readStringEntry("COMMUNICATION", "LOCATOR_SERVER_HOST",
//                                      DEFAULT_LSERVER_HOST);
//  valDefLocServInterval
//    = getApp()->reg().readIntEntry("COMMUNICATION", "LOCATOR_SERVER_INTERVAL",
//                                   DEFAULT_LSERVER_INTERVAL_MS);

  // Calibration
  char str[128];
  for (int i = 0; i < NUM_PROC_AUTOCALIB; i ++) {
    sprintf(str, "PROC_AUTOCALIB_%d", i);    
    valDefAutoCalibProcSelect[i]
      = getApp()->reg().readIntEntry("CALIBRATION", str, 0);
  }

  for (int i = 0; i < NUM_CALIB_POINTS; i ++) {
    sprintf(str, "P%d_X", i);
    valDefAutoCalibPoints[i][0] = 
      (float)getApp()->reg().readRealEntry("CALIBRATION", str, 0);
    sprintf(str, "P%d_Y", i);
    valDefAutoCalibPoints[i][1] = 
      (float)getApp()->reg().readRealEntry("CALIBRATION", str, 0);
    sprintf(str, "P%d_Z", i);
    valDefAutoCalibPoints[i][2] = 
      (float)getApp()->reg().readRealEntry("CALIBRATION", str, 0);
  }
  
  // End effector configuration
  /*
  valDefNeedleOffset[0] = 
    (float)getApp()->reg().readRealEntry("END_EFFECTOR",
                                         "OFFSET_A", 0.0);
  valDefNeedleOffset[1] = 
    (float)getApp()->reg().readRealEntry("END_EFFECTOR",
                                         "OFFSET_B", 0.0);
  valDefNeedleOffset[2] = 
    (float)getApp()->reg().readRealEntry("END_EFFECTOR",
                                         "OFFSET_C", 0.0);
  */

  int n =  (int)getApp()->reg().readIntEntry("END_EFFECTOR",
                                              "NUM_CONFIGS", 0);

  if (defNeedleInfo == NULL) {
    defNeedleInfo = new NeedleInfo;
  }

  NeedleInfo* p = defNeedleInfo;
  // dummy config at start of the list
  p->name = "Default                 ";
  p->valOffset[0] = 0.0;
  p->valOffset[1] = 0.0;
  p->valOffset[2] = 0.0;

  p->valOrient[0] = 0.0;
  p->valOrient[1] = 0.0;
  p->valOrient[2] = 1.0;

  for (int i = 0; i < n; i ++) {
    p->next = new NeedleInfo;
    p = p->next;
    sprintf(str, "NAME_%03d", i);
    p->name = 
      (FXString) getApp()->reg().readStringEntry("END_EFFECTOR", str, "--");

    sprintf(str, "OFFSET_A_%03d", i);
    p->valOffset[0] = 
      (float)getApp()->reg().readRealEntry("END_EFFECTOR", str, 0.0);
    sprintf(str, "OFFSET_B_%03d", i);
    p->valOffset[1] = 
      (float)getApp()->reg().readRealEntry("END_EFFECTOR", str, 0.0);
    sprintf(str, "OFFSET_C_%03d", i);
    p->valOffset[2] =
      (float)getApp()->reg().readRealEntry("END_EFFECTOR", str, 0.0);

    sprintf(str, "ORIENT_A_%03d", i);
    p->valOrient[0] = 
      (float)getApp()->reg().readRealEntry("END_EFFECTOR", str, 0.0);
    sprintf(str, "ORIENT_B_%03d", i);
    p->valOrient[1] = 
      (float)getApp()->reg().readRealEntry("END_EFFECTOR", str, 0.0);
    sprintf(str, "ORIENT_C_%03d", i);
    p->valOrient[2] =
      (float)getApp()->reg().readRealEntry("END_EFFECTOR", str, 1.0);

  }
  p->next = NULL;

}


void MrsvrMainWindow::storeRegistry()
{
  char str[128];

  // store values into FOX registry
  // Communication paramters
  getApp()->reg().writeIntEntry("COMMUNICATION", "REMOTE_CONTROL_SERVER_PORT",
                                valDefConPortNo);
  getApp()->reg().writeIntEntry("COMMUNICATION", "LOCATOR_SERVER_PORT",
                               valDefLocServPortNo);
  getApp()->reg().writeStringEntry("COMMUNICATION", "LOCATOR_SERVER_HOST",
                                   valDefLocServHostName.text());
  getApp()->reg().writeIntEntry("COMMUNICATION", "REMOTE_CONTROL_INTERVAL",
                               valDefLocServInterval);

  // Calibration
  for (int i = 0; i < NUM_PROC_AUTOCALIB; i ++) {
    sprintf(str, "PROC_AUTOCALIB_%d", i);    
    getApp()->reg().writeIntEntry("CALIBRATION", str,
                                 valDefAutoCalibProcSelect[i]);
  }

  for (int i = 0; i < NUM_CALIB_POINTS; i ++) {
    sprintf(str, "P%d_X", i);
    getApp()->reg().writeRealEntry("CALIBRATION", str,
                                  valDefAutoCalibPoints[i][0]);
    sprintf(str, "P%d_Y", i);
    getApp()->reg().writeRealEntry("CALIBRATION", str,
                                  valDefAutoCalibPoints[i][1]);
    sprintf(str, "P%d_Z", i);
    getApp()->reg().writeRealEntry("CALIBRATION", str,
                                  valDefAutoCalibPoints[i][2]);
  }

  // End effector configuration
  /*
  getApp()->reg().readRealEntry("END_EFFECTOR",
                                "OFFSET_A", valDefNeedleOffset[0]);
  getApp()->reg().readRealEntry("END_EFFECTOR",
                                "OFFSET_B", valDefNeedleOffset[1]);
  getApp()->reg().readRealEntry("END_EFFECTOR",
                                "OFFSET_C", valDefNeedleOffset[2]);
  */
  NeedleInfo* p = defNeedleInfo->next;
  int count = 0;
  while (p != NULL) {
    sprintf(str, "NAME_%03d", count);
    getApp()->reg().writeStringEntry("END_EFFECTOR",
                                     str, p->name.text());
    sprintf(str, "OFFSET_A_%03d", count);
    getApp()->reg().writeRealEntry("END_EFFECTOR",
                                  str, p->valOffset[0]);
    sprintf(str, "OFFSET_B_%03d", count);
    getApp()->reg().writeRealEntry("END_EFFECTOR",
                                  str, p->valOffset[1]);
    sprintf(str, "OFFSET_C_%03d", count);
    getApp()->reg().writeRealEntry("END_EFFECTOR",
                                  str, p->valOffset[2]);
    sprintf(str, "ORIENT_A_%03d", count);
    getApp()->reg().writeRealEntry("END_EFFECTOR",
                                  str, p->valOrient[0]);
    sprintf(str, "ORIENT_B_%03d", count);
    getApp()->reg().writeRealEntry("END_EFFECTOR",
                                  str, p->valOrient[1]);
    sprintf(str, "ORIENT_C_%03d", count);
    getApp()->reg().writeRealEntry("END_EFFECTOR",
                                  str, p->valOrient[2]);

    p = p->next;
    count ++;
  }
  
  getApp()->reg().writeIntEntry("END_EFFECTOR",
                               "NUM_CONFIGS", count);

  getApp()->reg().write();

}


void MrsvrMainWindow::setDataTargets()
{
  // Status bar: progress meter
  dtStatusProg = new FXDataTarget(valStatusProg, this, (FXSelector)NULL);

  // Communication -> Server Thread configuration
  dtConPortNo  = new FXDataTarget(valConPortNo, this, (FXSelector)NULL);
  dtConStatus  = new FXDataTarget(valConStatus, this, (FXSelector)NULL);

  dtCalibrationStatus = 
    new FXDataTarget(valCalibrationStatus, this, ID_UPDATE_PARAMETER);

  for (int i = 0; i < 16; i ++) {
    dtCalibrationMatrix[i] = 
      new FXDataTarget(valCalibrationMatrix[i], this, ID_UPDATE_PARAMETER);
  }

  
  // Calibration
  for (int i = 0; i < NUM_PROC_AUTOCALIB; i ++) {
    dtAutoCalibProcSelect[i] = new FXDataTarget(valAutoCalibProcSelect[i],
                                                this, ID_UPDATE_PARAMETER);
  }
  for (int i = 0; i < NUM_CALIB_POINTS; i ++) {
    for (int j = 0; j < 3; j ++) {
      dtAutoCalibPoints[i][j] = new FXDataTarget(valAutoCalibPoints[i][j],
                                                 this, ID_UPDATE_PARAMETER);
      dtDefAutoCalibPoints[i][j] = 
        new FXDataTarget(valDefAutoCalibPoints[i][j], this,
                         ID_UPDATE_PARAMETER);
    }
  }
  // actuators' parameters
  for (int i = 0; i < NUM_ACTUATORS; i++) {
    // data target for text field
    dtTxActuatorVol[i] = 
      new FXDataTarget(valActuatorVol[i], this, ID_UPDATE_PARAMETER);
    // data target for progress bar
    dtPrActuatorVol[i] = 
      new FXDataTarget(valPrActuatorVol[i], this, ID_UPDATE_PARAMETER);

    dtTxActRev[i] =  new FXDataTarget(valTotalActRev[i], this, ID_UPDATE_PARAMETER);
    dtTxActStatus[i] = new FXDataTarget(valActStatus[i], this, ID_UPDATE_PARAMETER);
    dtManualActuatorVol[i] = 
      new FXDataTarget(valManualActuatorVol[i], this, ID_UPDATE_ACTUATOR_VOLTAGE);
  }

  // encorders' parameters
  for (int i = 0; i < NUM_ENCODERS; i ++) {
    // data targets for text fields
    dtTxPosition[i] = 
      new FXDataTarget(valPosition[i], this, ID_UPDATE_PARAMETER);
    dtTxSetPoint[i] =
      new FXDataTarget(valSetPoint[i], this, ID_UPDATE_PARAMETER);
    dtTxVelocity[i] =
      new FXDataTarget(valVelocity[i], this, ID_UPDATE_PARAMETER);
    // data targets for progress bars
    dtPrPosition[i] = 
      new FXDataTarget(valPrPosition[i], this, ID_UPDATE_PARAMETER);
    dtPrSetPoint[i] =
      new FXDataTarget(valPrSetPoint[i], this, ID_UPDATE_PARAMETER);
    dtPrVelocity[i] =
      new FXDataTarget(valPrVelocity[i], this, ID_UPDATE_PARAMETER);
  }

  for (int i = 0; i < 3; i ++) {
    dtNeedleOffset[i] = 
      new FXDataTarget(valNeedleOffset[i], this, ID_UPDATE_PARAMETER);
    dtNeedleOrientation[i] = 
      new FXDataTarget(valNeedleOrientation[i], this, ID_UPDATE_PARAMETER);
  }

  //plot
  dtPlotAGain = 
    new FXDataTarget(valPlotAGain, this, ID_UPDATE_MINI_PLOT);
  dtPlotAShift = 
    new FXDataTarget(valPlotAShift, this, ID_UPDATE_MINI_PLOT);
  dtPlotBGain = 
    new FXDataTarget(valPlotBGain, this, ID_UPDATE_MINI_PLOT);
  dtPlotBShift = 
    new FXDataTarget(valPlotBShift, this, ID_UPDATE_MINI_PLOT);


  // Customization parameters
  dtDefConPortNo = 
    new FXDataTarget(valDefConPortNo, this, ID_UPDATE_PARAMETER);
  dtDefLocServPortNo = 
    new FXDataTarget(valDefLocServPortNo, this, ID_UPDATE_PARAMETER);
  dtDefLocServHostName =
    new FXDataTarget(valDefLocServHostName, this, ID_UPDATE_PARAMETER);
  dtDefLocServInterval =
    new FXDataTarget(valDefLocServInterval, this, ID_UPDATE_PARAMETER);
  
  // -- Calibration
  for (int i = 0; i < NUM_PROC_AUTOCALIB; i ++) {
    dtDefAutoCalibProcSelect[i] = 
      new FXDataTarget(valDefAutoCalibProcSelect[i], this,
                       ID_UPDATE_PARAMETER);
  }
  for (int i = 0; i < NUM_CALIB_POINTS; i ++) {
    for (int j = 0; j < 3; j ++) {
      dtDefAutoCalibPoints[i][j] = 
        new FXDataTarget(valDefAutoCalibPoints[i][j], this,
                       ID_UPDATE_PARAMETER);
    }
  }
  // -- End effector
  for (int i = 0; i < 3; i ++) {
    dtDefNeedleOffset[i] =
      new FXDataTarget(valDefNeedleOffset[i], this,
                       ID_UPDATE_PARAMETER);
    dtDefNeedleOrientation[i] = 
      new FXDataTarget(valDefNeedleOrientation[i], this,
                       ID_UPDATE_PARAMETER);
  }
  dtNeedleConfName = new FXDataTarget(valNeedleConfName, this, 
                                     ID_UPDATE_PARAMETER);

  dtPatientEntry = 
    new FXDataTarget(valPatientEntry, this,
                     ID_UPDATE_PARAMETER);
  dtBedDock = 
    new FXDataTarget(valBedDock, this,
                      ID_UPDATE_PARAMETER);
  dtRobotDock = 
    new FXDataTarget(valRobotDock, this,
                     ID_UPDATE_PARAMETER);
  
  // -- Hardware control
  for (int i = 0; i < 3; i ++) {
    valTargetPosition[i]  = 0.0;
    valCurrentPosition[i] = 0.0;
    valDeltaPosition[i]   = 0.0;
    valNormTargetPosition[i] = 0.5;
    valNormCurrentPosition[i] = 0.5;

    dtTargetPosition[i] =
      new FXDataTarget(valTargetPosition[i], this,
                       ID_UPDATE_PARAMETER);
    dtCurrentPosition[i] =
      new FXDataTarget(valCurrentPosition[i], this,
                       ID_UPDATE_PARAMETER);
    dtDeltaPosition[i] =
      new FXDataTarget(valDeltaPosition[i], this,
                       ID_UPDATE_PARAMETER);

    dtNormTargetPosition[i] = 
      new FXDataTarget(valNormTargetPosition[i], this,
                       ID_UPDATE_PARAMETER);
    dtNormCurrentPosition[i] = 
      new FXDataTarget(valNormCurrentPosition[i], this,
                       ID_UPDATE_PARAMETER);
  }

  dtTipApprOffset = 
    //new FXDataTarget(valTipApprOffset, this, ID_UPDATE_PARAMETER);
    new FXDataTarget(valTipApprOffset, this, ID_UPDATE_NEEDLE_APPR_OFFSET);

}


void MrsvrMainWindow::setDefaultParameters()
{
  // Communication -> Server Thread configuration
  valConPortNo = valDefConPortNo;
  valConStatus = "Launching";

  // Calibration
  for (int i = 0; i < NUM_PROC_AUTOCALIB; i ++) {
    valAutoCalibProcSelect[i] = valDefAutoCalibProcSelect[i];
  }

  for (int i = 0; i < NUM_CALIB_POINTS; i ++) {
    for (int j = 0; j < 3; j ++) {
      valAutoCalibPoints[i][j] = valDefAutoCalibPoints[i][j];
    }
  }

  /*
  for (int i = 0; i < 3; i ++) {
    valNeedleOffset[i] = valDefNeedleOffset[i];
  }
  */
  valTipApprOffset = 0.0;
 
}


void MrsvrMainWindow::updateParameters()
{
  int i;

  // get actuators' paramters 
  for (i = 0; i < NUM_ACTUATORS; i ++) {
    float vol = robotStatus->getVoltage(i);
    valActuatorVol[i] = vol;
    valPrActuatorVol[i] = (int)(fabs(vol)/MAX_ACTUATOR_VOL_V * 100.0);
  }

  for (i = 0; i < NUM_ENCODERS; i ++) {
    valPosition[i] = robotStatus->getPosition(i)*positionCoeffs[i] + positionOffsets[i];
    valSetPoint[i] = robotStatus->getSetPoint(i)*positionCoeffs[i] + positionOffsets[i];
    valVelocity[i] = robotStatus->getVelocity(i)*positionCoeffs[i] + positionOffsets[i]; 
    valPrPosition[i] = POS2PROG(valPosition[i]);
    valPrSetPoint[i] = POS2PROG(valSetPoint[i]);
    valPrVelocity[i] = VEL2PROG(valVelocity[i]);
  }
}


void MrsvrMainWindow::updateExternalCommands()
{
  if (extMsgSvr != NULL) {  // Message Server
    MrsvrMessageServer::Matrix4x4 target;
    if (extMsgSvr->getTargetMatrix(target)) {
      for (int i = 0; i < 3; i ++) {
        valTargetPosition[i] = target[i][3];
      }
      setTargetPositionRAS(valTargetPosition);
    }

    int mode;
    if (extMsgSvr->getMode(&mode)) {
      robotCommand->setMode(mode);
    }

    MrsvrMessageServer::Matrix4x4 calibration;
    float matrix[16];
    if (extMsgSvr->getCalibrationMatrix(calibration)){
      for (int i = 0; i < 4; i ++) {
        for (int j = 0; j < 4; j ++) {
          valCalibrationMatrix[i*4+j] = calibration[i][j];
          matrix[i*4+j] = calibration[i][j];
        }
      }
      transform->setCalibrationMatrix(matrix);
    }
  }
}


void MrsvrMainWindow::setTargetPositionRAS(float pos[3])
{
  float robotPos[3];

  transform->transform(pos, robotPos);
  for (int i = 0; i < 3; i ++) {
    robotCommand->setSetPoint(i, robotPos[i]);
  }
}


void MrsvrMainWindow::setCalibrationMatrix(float* matrix)
{
  transform->setCalibrationMatrix(matrix);
}


void MrsvrMainWindow::consolePrint(int level, bool fTime, const char* format, ...)
{

  if (level <= debugLevel) {
    va_list args;
    char    buf[MAX_SINGLE_LOG_MESSAGE+1];
    time_t  ct = 0;
    struct tm ctm;

    va_start(args, format);
    if (fTime) {
      time(&ct);
      localtime_r(&ct, &ctm);
      sprintf(buf, "[%04d/%02d/%02d %02d:%02d:%02d] ",
              ctm.tm_year+1900, ctm.tm_mon+1, ctm.tm_mday,
              ctm.tm_hour, ctm.tm_min, ctm.tm_sec);
      textInterfaceLog->appendText(buf, strlen(buf));
    }
    vsnprintf(buf, MAX_SINGLE_LOG_MESSAGE+1, format, args);
    
    textInterfaceLog->appendText(buf, strlen(buf));
    textInterfaceLog->makePositionVisible(textInterfaceLog->getLength());
    va_end(args);
  }
}


long MrsvrMainWindow::onCmdAbout(FXObject*, FXSelector, void*) {
  FXMessageBox::information(this,MBOX_OK,
      "About MR servo robot monitor",
      "Copyright (C) 2003-2005 by Junichi Tokuda, All Right Reserved.\n"
      "Copyright (C) 2003-2005 by ATRE Lab., The University of Tokyo, All Right Reserved.");
  return 1;
}


void MrsvrMainWindow::create()
{
  FXMainWindow::create();
  show(PLACEMENT_SCREEN);
  infoOffs->create();
  infoFont0->create();
  infoFont1->create();
  infoFont2->create();
  plotCanvasA->create();
  plotCanvasB->create();
  infoCanvas->create();

  axialCanvas->create();
  axialOffs->create();
  axialFont->create();

  needleCanvas->create();
  needleOffs->create();



  infoCnvH        = infoCanvas->getHeight();
  infoCnvW        = infoCanvas->getWidth();
  float w         = (float)infoCnvW;
  float h         = (float)infoCnvH;

  infoCnvItemY    = (int)(h*INFCNV_ITEM_Y);
  infoCnvValueY   = (int)(h*INFCNV_VALUE_Y);
  infoCnvValue1Y  = (int)(h*INFCNV_VALUE1_Y);
  infoCnvValue2Y  = (int)(h*INFCNV_VALUE2_Y);
  infoCnvModeX    = (int)(w*INFCNV_MODE_X);
  infoCnvWarningX = (int)(w*INFCNV_WARNING_X);
  infoCnvTargetX  = (int)(w*INFCNV_TARGET_X);
  infoCnvTimeX    = (int)(w*INFCNV_TIME_X);
  
  infoWarnTextX[WARNID_LOCK]  = (int)(w*INFCNV_WARN_LOCK_X);
  infoWarnTextY[WARNID_LOCK]  = (int)(h*INFCNV_WARN_LOCK_Y);
  infoWarnTextX[WARNID_OUTOFRANGE]  = (int)(w*INFCNV_WARN_OUTOFRANGE_X);
  infoWarnTextY[WARNID_OUTOFRANGE]  = (int)(h*INFCNV_WARN_OUTOFRANGE_Y);
  //infoWarnTextX[WARNID_NOLOCSVR]  = (int)(w*INFCNV_WARN_NOLOCSVR_X);
  //infoWarnTextY[WARNID_NOLOCSVR]  = (int)(h*INFCNV_WARN_NOLOCSVR_Y);

  //  infoCnvRTimeX   = (int)(w*INFCNV_RTIME_X);

  infoCanvas->update();

  plotCanvasA->update();
  plotCanvasB->update();
}


long MrsvrMainWindow::onCanvasRepaint(FXObject*, FXSelector,void* ptr)
// Repaint Info Display
{
  FXEvent *event=(FXEvent*)ptr;
  
  // We caused a redraw, so redo it all
  if(event->synthetic){
    FXDCWindow dc(infoOffs);
    if (fUpdateInfoItem) {
      fUpdateInfoItem = 0;
      dc.setForeground(FXRGB(0,0,0));
      dc.fillRectangle(0,0,infoOffs->getWidth(),infoOffs->getHeight());
      dc.setBackground(FXRGB(0,0,0));
      dc.setFont(infoFont0);
      dc.setForeground(FXRGB(255,255,255));
      dc.drawText(infoCnvModeX, infoCnvItemY, 
                  "MODE:", 5);
      dc.drawText(infoCnvWarningX, infoCnvItemY, 
                  "WARNING:", 8);
      dc.drawText(infoCnvTargetX, infoCnvItemY, 
                  "CURRENT / TARGET POSITION:", 26);
      dc.drawText(infoCnvTimeX, infoCnvItemY, 
                  "TIME:", 5);
      //      dc.drawText(infoCnvRTimeX, infoCnvItemY, 
      //                  "OPERATING TIME:", 15);
    }
    dc.setFont(infoFont1);
    if (fUpdateInfoMode) {
      fUpdateInfoMode = 0;
      dc.setForeground(FXRGB(0,0,0));
      dc.fillRectangle(infoCnvModeX, infoCnvValueY-INFCNV_FONT_H,
                       infoCnvTargetX-infoCnvModeX,INFCNV_FONT_H);
      if (infoCurrentMode >= 0) {
        dc.setForeground(FXRGB(255,255,0));
        dc.drawText(infoCnvModeX, infoCnvValueY, 
                    infoModeText[infoCurrentMode], 
                    strlen(infoModeText[infoCurrentMode]));
      }
    }
    if (fUpdateInfoTarget) {
      fUpdateInfoTarget = 0;
      dc.setForeground(FXRGB(0,0,0));
      dc.fillRectangle(infoCnvTargetX, infoCnvItemY+1,
                       infoCnvWarningX-infoCnvTargetX,INFCNV_H-infoCnvItemY);

      // current position
      dc.setForeground(FXRGB(150,150,150));
      dc.drawText(infoCnvTargetX, infoCnvValue1Y, 
                  "R:", 2);
      dc.drawText(infoCnvTargetX+INFCNV_TARGET_DIR_W+INFCNV_TARGET_VAL_W, 
                  infoCnvValue1Y, "A:", 2);
      dc.drawText(infoCnvTargetX+(INFCNV_TARGET_DIR_W+INFCNV_TARGET_VAL_W)*2, 
                  infoCnvValue1Y, "S:", 2);
      dc.setForeground(FXRGB(150,150,0));
      dc.drawText(infoCnvTargetX+INFCNV_TARGET_DIR_W,
                  infoCnvValue1Y, infoCurrentX, strlen(infoCurrentX));
      dc.drawText(infoCnvTargetX+INFCNV_TARGET_DIR_W*2+INFCNV_TARGET_VAL_W, 
                  infoCnvValue1Y, infoCurrentY, strlen(infoCurrentY));
      dc.drawText(infoCnvTargetX+INFCNV_TARGET_DIR_W*3+INFCNV_TARGET_VAL_W*2, 
                  infoCnvValue1Y, infoCurrentZ, strlen(infoCurrentZ));

      // target position
      dc.setForeground(FXRGB(150,150,150));
      dc.drawText(infoCnvTargetX, infoCnvValue2Y, 
                  "R:", 2);
      dc.drawText(infoCnvTargetX+INFCNV_TARGET_DIR_W+INFCNV_TARGET_VAL_W, 
                  infoCnvValue2Y, "A:", 2);
      dc.drawText(infoCnvTargetX+(INFCNV_TARGET_DIR_W+INFCNV_TARGET_VAL_W)*2, 
                  infoCnvValue2Y, "S:", 2);
      dc.setForeground(FXRGB(0, 150,150));
      dc.drawText(infoCnvTargetX+INFCNV_TARGET_DIR_W,
                  infoCnvValue2Y, infoTargetR, strlen(infoTargetR));
      dc.drawText(infoCnvTargetX+INFCNV_TARGET_DIR_W*2+INFCNV_TARGET_VAL_W, 
                  infoCnvValue2Y, infoTargetA, strlen(infoTargetA));
      dc.drawText(infoCnvTargetX+INFCNV_TARGET_DIR_W*3+INFCNV_TARGET_VAL_W*2, 
                  infoCnvValue2Y, infoTargetS, strlen(infoTargetS));

    }
    if (fUpdateInfoWarn) {
      fUpdateInfoWarn = 0;
      dc.setForeground(FXRGB(0,0,0));
      dc.fillRectangle(infoCnvWarningX, infoCnvValueY-INFCNV_FONT_H,
                       infoCnvTimeX-infoCnvWarningX,INFCNV_FONT_H);
      for (int i = 0; i < NUM_WARNID; i ++) {
        if (infoWarning[i]) {
          dc.setForeground(FXRGB(255,150,0));
        } else {
          dc.setForeground(FXRGB(50,50,50));
        }
        dc.drawText(infoWarnTextX[i], infoWarnTextY[i], 
                    infoWarnText[i], strlen(infoWarnText[i]));
      }
      /*
      if (infoCurrentWarn >= 0) {
        dc.setForeground(FXRGB(100,255,100));
        dc.drawText(infoCnvWarningX, infoCnvValueY, 
                    infoWarnText[infoCurrentWarn], 
                    strlen(infoWarnText[infoCurrentWarn]));
      }
      */
    }

    dc.setFont(infoFont2);
    if (fUpdateInfoTime) {
      fUpdateInfoTime = 0;
      dc.setForeground(FXRGB(0,0,0));
      dc.fillRectangle(infoCnvTimeX, infoCnvValueY-INFCNV_FONT_H,
                       infoCnvRTimeX-infoCnvTimeX,INFCNV_FONT_H);
      dc.setForeground(FXRGB(100,255,100));
      dc.drawText(infoCnvTimeX, infoCnvValueY, 
                  infoTime, strlen(infoTime));
    }
//    if (fUpdateInfoRTime) {
//      fUpdateInfoRTime = 0;
//      dc.setForeground(FXRGB(0,0,0));
//      dc.fillRectangle(infoCnvRTimeX, infoCnvValueY-INFCNV_FONT_H,
//                       infoCnvW-infoCnvRTimeX,INFCNV_FONT_H);
//      dc.setForeground(FXRGB(100,255,100));
//      dc.drawText(infoCnvRTimeX, infoCnvValueY, 
//                  infoRTime, strlen(infoRTime));
//    }
  }
  
  // Now repaint the screen
  FXDCWindow sdc(infoCanvas,event);
  // Paint image
  sdc.drawImage(infoOffs,0,0);

  return 1;
}


long MrsvrMainWindow::onAxialCanvasRepaint(FXObject*, FXSelector, void* ptr)
{

  if (axialCanvas) {
    FXDCWindow dc(axialOffs);
    dc.setForeground(FXRGB(0,0,0));
    dc.fillRectangle(0, 0, AXIALCNV_W, AXIALCNV_H);
    
    dc.setForeground(FXRGB(150,150,150));

    // Draw a frame and directions
    int margin = (int)((float)AXIALCNV_W*AXIALCNV_MARGIN);
    dc.drawLine(margin, margin, AXIALCNV_W-margin, margin);
    dc.drawLine(AXIALCNV_W-margin, margin, AXIALCNV_W-margin, AXIALCNV_H-margin);
    dc.drawLine(AXIALCNV_W-margin, AXIALCNV_H-margin, margin, AXIALCNV_H-margin);
    dc.drawLine(margin, AXIALCNV_H-margin, margin, margin);

    dc.setFont(axialFont);
    dc.drawText(AXIALCNV_W/2-4, margin-2, "A", 1);
    dc.drawText(AXIALCNV_W/2-4, AXIALCNV_H-2, "P", 1);
    dc.drawText(2, AXIALCNV_H/2+4, "R", 1);
    dc.drawText(AXIALCNV_W-margin+2, AXIALCNV_H/2+4, "L", 1);

    //Draw Target [Zeroposition=(-5,-5)]
    int tx = (int)(valNormTargetPosition[0]*(float)(AXIALCNV_W-margin*2))+margin;
    int ty = (int)((1.0-valNormTargetPosition[1])*(float)(AXIALCNV_W-margin*2))+margin;

    dc.setForeground(FXRGB(255,0,0));
    dc.drawPoint(tx, ty);
    
    //Draw Current position
    int cx = (int)(valNormCurrentPosition[0]*(float)(AXIALCNV_W-margin*2))+margin;
    int cy = (int)((1.0-valNormCurrentPosition[1])*(float)(AXIALCNV_W-margin*2))+margin;

    dc.setForeground(FXRGB(0,0,255));
    dc.drawPoint(cx, cy);
    
    //for (int i = 0; i < 3; i ++) { 
    //  valTargetPosition[i]=(valDeltaPosition[i]+valCurrentPosition[i]);
    //}

    //draw Text NewTarget
    dc.setForeground(FXRGB(0,0,255));
    dc.setFont(axialFont);
    dc.drawText(cx+2, cy-2,"Current", 7);
    
    //draw Text OldTarget
    dc.setForeground(FXRGB(255,0,0));
    dc.setFont(axialFont);
    dc.drawText(tx+2, ty-2,"Target", 6);

    // Now repaint the screen
    FXDCWindow sdc(axialCanvas);
    // Paint image
    sdc.drawImage(axialOffs,0,0);
  }
  return 1;

}


long MrsvrMainWindow::onNeedleCanvasRepaint(FXObject*, FXSelector,void*)
{
  if (needleCanvas) {
    FXDCWindow dc(needleOffs);
    dc.setForeground(FXRGB(0,0,0));

    dc.fillRectangle(0, 0, NEEDLE_CANVAS_W, NEEDLE_CANVAS_H);
    
    //draw Text NewTarget
    dc.setForeground(FXRGB(255,255,255));
    dc.setFont(infoFont0);
    dc.drawText(((float)NEEDLE_CANVAS_W*NEEDLECNV_DEPTH_LABEL_X),
                ((float)NEEDLE_CANVAS_H*NEEDLECNV_DEPTH_LABEL_Y), "NEEDLE INSERNTION DEPTH:", 24);
    
    dc.setForeground(FXRGB(255,255,0));
    dc.setFont(infoFont2);
    dc.drawText(((float)NEEDLE_CANVAS_W*NEEDLECNV_DEPTH_VALUE_X),
                ((float)NEEDLE_CANVAS_H*NEEDLECNV_DEPTH_VALUE_Y), "  128.00", 8);
    
    dc.setForeground(FXRGB(150,150,150));
    dc.setFont(infoFont1);
    dc.drawText(((float)NEEDLE_CANVAS_W*NEEDLECNV_DEPTH_UNIT_X),
                ((float)NEEDLE_CANVAS_H*NEEDLECNV_DEPTH_UNIT_Y), "mm", 2);
    
    ////draw Text OldTarget
    //dc.setForeground(FXRGB(0,0,255));
    //dc.setFont(axialFont);
    //dc.drawText(valCurrentPosition[0]-25, 200-valCurrentPosition[1]+15,"Target", 6);
    
    // Now repaint the screen
    FXDCWindow sdc(needleCanvas);
    // Paint image
    sdc.drawImage(needleOffs,0,0);
    
  }
  return 1;
  
}



long MrsvrMainWindow::onUpdateActuatorVoltage(FXObject* obj, FXSelector sel,void*)
{
  int actuator = getObjectIndex(obj, (FXObject**)dtManualActuatorVol, NUM_ACTUATORS);

  std::cout << "onUpdateActuatorVoltage() act = " << actuator << std::endl;

  if (actuator >= 0 || actuator < NUM_ACTUATORS) {
    float vol;
    vol = ((float) valManualActuatorVol[actuator] / 
           (float) (MANUAL_VOL_STEPS - 1)) * MAX_ACTUATOR_VOL_V;
    robotCommand->setLmtVoltage(actuator, vol);
  }
  return 0;
}




long MrsvrMainWindow::onMiniPlotUpdate(FXObject*, FXSelector,void* ptr)
{
#ifdef DEBUG_MRSVR_MAIN_WINDOW  
  for (int i=0; i < NUM_ACTUATORS; i ++) {
    DBG_MMW_PRINT("valVelocity[%d] = %d\n", i, valVelocity[i]);
  }
  DBG_MMW_PRINT("valPlotAGain = %d\n", valPlotAGain);
  DBG_MMW_PRINT("valPlotAShift = %d\n", valPlotAShift);
  DBG_MMW_PRINT("valPlotBGain = %d\n", valPlotBGain);
  DBG_MMW_PRINT("valPlotBShift = %d\n", valPlotBShift);
#endif // DEBUG_MRSVR_MAIN_WINDOW  
  float unit = 0.00001*valPlotAGain;
  int max    = (int)(1.0/(unit*12));
  int gsp = max/10 * 10;
  if (gsp < 5) gsp = 5;
  plotCanvasA->setAxisScaleY(unit, gsp);
  //profileWindow->setPlotScaleAll((float)valEchoPlotGain/100.0);
  plotCanvasA->update();

  unit = 0.00001*valPlotBGain;
  max  = (int)(1.0/(unit*12));
  gsp  = max/10 * 10;
  if (gsp < 5) gsp = 5;
  plotCanvasB->setAxisScaleY(unit, gsp);
  //profileWindow->setPlotScaleAll((float)valEchoPlotGain/100.0);
  plotCanvasB->update();

  return 1;
}


long MrsvrMainWindow::onStartUpdate(FXObject*, FXSelector,void*)
{
  application->addTimeout(this, MrsvrMainWindow::ID_UPDATE_TIMER, updateInterval);
  return 1;
}


long MrsvrMainWindow::onStopUpdate(FXObject*, FXSelector,void*)
{
  updateTimer = NULL;
  return 1;
}


long MrsvrMainWindow::onUpdateTimer(FXObject* obj, FXSelector sel,void* ptr)
{
  static int  prevInfoMode = -1;
  static int  prevMode     = -1;
  //static int  prevInfoWarn[] = {false, false, false};
  static bool prevActuatorLockStatus[] = {false, false, false};
  static bool prevActuatorActive[] = {true, true, true};
  static int  prevCalibReadyIndex;
  static int  prevSetAngleReadyIndex;

  bool   fModeUpdated = false;

  updateParameters();
  updateExternalCommands();

  int currentMode = robotStatus->getMode();
  if (prevMode != currentMode) {
    fModeUpdated = true;
    prevMode = currentMode;
    robotCommand->setMode(currentMode);
  }

  // do mode specific processes
  switch(screenMode) {
  case SCR_CALIBRATION:
    break;
  default:
    break;
  }

  char *sit;
  static int pi = 0;
  int ci = robotStatus->getInfoTextIdx();
  if (pi != ci && ci >= 0) {
    sit = robotStatus->getInfoText(ci);
    textStatusProg->setText(sit);
    valStatusProg = robotStatus->getProgress();
    pi = ci;
  }

  // update information canvas
  infoCurrentMode = robotStatus->getMode();
  if (infoCurrentMode != prevInfoMode) {
    fUpdateInfoMode = 1;
    prevInfoMode = infoCurrentMode;
  }
  
  // update out of range information
  infoWarning[WARNID_OUTOFRANGE] = false;
  for (int i = 0; i < NUM_ENCODERS; i ++) {
    if (robotStatus->getOutOfRange(i) != 0) {
      infoWarning[WARNID_OUTOFRANGE] = true;
    }
  }
  if (prevInfoWarning[WARNID_OUTOFRANGE] != infoWarning[WARNID_OUTOFRANGE]) {
    fUpdateInfoWarn = 1;
    prevInfoWarning[WARNID_OUTOFRANGE] = infoWarning[WARNID_OUTOFRANGE];
    if (infoWarning[WARNID_OUTOFRANGE]) fprintf(stderr, "\a");
 }
  
  static float ptp[3] = {5.0, 5.0, 5.0};   // previous target position
  static float pcp[3] = {5.0, 5.0, 5.0};   // previous current position
  float  tp[3];
  float  cp[3];

  float robotP[3];
  float mrP[3];
  int i;

  // target position
  for (i = 0; i < 3; i ++) {
    robotP[i] = robotCommand->getSetPoint(i);
  }
  transform->invTransform(robotP, mrP);
  for (i = 0; i < 3; i ++) {
    tp[i] = mrP[i];
  }

  // current position
  for (int i = 0; i < 3; i ++) {
    robotP[i] = robotStatus->getTipPosition(i);
  }
  transform->invTransform(robotP, mrP);
  for (i = 0; i < 3; i ++) {
    cp[i] = mrP[i];
  }

  if (tp[0] != ptp[0] || tp[1] != ptp[1] || tp[2] != ptp[2]) {
    snprintf(infoTargetR, 16, "%6.1f", tp[0]);
    snprintf(infoTargetA, 16, "%6.1f", tp[1]);
    snprintf(infoTargetS, 16, "%6.1f", tp[2]);
    for (i = 0; i < 3; i ++) {
      ptp[i] = tp[i];
    }
    fUpdateInfoTarget = 1;
  }
  if (cp[0] != pcp[0] || cp[1] != pcp[1] || cp[2] != pcp[2]) {
    snprintf(infoCurrentX, 16, "%6.1f", cp[0]);
    snprintf(infoCurrentY, 16, "%6.1f", cp[1]);
    snprintf(infoCurrentZ, 16, "%6.1f", cp[2]);
    for (i = 0; i < 3; i ++) {
      pcp[i] = cp[i];
    }
    fUpdateInfoTarget = 1;
  }


  static time_t pt = 0;
  static time_t ct = 0;
  struct tm ctm;
  time(&ct);
  if (ct-pt > 0.8) {
    pt = ct;
    localtime_r(&ct, &ctm);
    snprintf(infoTime, 16, "%02d:%02d:%02d", 
             ctm.tm_hour, ctm.tm_min, ctm.tm_sec);
    // calcurate running time
    if (infoStartTime != 0) {
      time_t df = ct - infoStartTime;
      localtime_r(&df, &ctm);
      snprintf(infoRTime, 16, "%02d:%02d:%02d", 
             ctm.tm_hour, ctm.tm_min, ctm.tm_sec);
    } else {
      strcpy(infoRTime, "00:00:00");
    }
    fUpdateInfoTime = 1;
    fUpdateInfoRTime = 1;
  }

  infoCanvas->update();

  // update actuator status (+ actuator power switch)
  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    if (robotStatus->getActuatorLifetime(i) 
        > robotStatus->getActuatorExpiration(i)) {
      valActStatus[i] = actuatorStatusText[ACTUATOR_STATUS_TXT_EXPIRED];
    }
    if (prevActuatorActive[i] != robotStatus->getActuatorPowerStatus(i)) {
      if (robotStatus->getActuatorPowerStatus(i)) { // Power off -> Power on
        //cerr << "Power off -> Power on" << endl;
        manualPowerSw[i] = 1; 
        valActStatus[i] = actuatorStatusText[ACTUATOR_STATUS_TXT_OK];
        prevActuatorActive[i] = true;
      } else {                                      // Power on -> Power off
        //cerr << "Power on -> Power off" << endl;
        manualPowerSw[i] = 0; 
        valActStatus[i] = actuatorStatusText[ACTUATOR_STATUS_TXT_NOPOWER];
        prevActuatorActive[i] = false;
        robotCommand->deactivateActuator(i);
      }
    }
    if (prevActuatorLockStatus[i] !=
        robotStatus->getActuatorLockStatus(i)) {
      if (robotStatus->getActuatorLockStatus(i)) {  // Normal -> Locked
        //cerr << "Normal -> Locked" << endl;
        valActStatus[i] =  actuatorStatusText[ACTUATOR_STATUS_TXT_LOCKED];
        prevActuatorLockStatus[i] = true;
        if (manualPowerSw[i] > 0) {
          manualPowerSw[i] = 0;
          prevActuatorActive[i] = false;
        }
        fUpdateInfoWarn = 1;
      } else {                                      // Locked -> Normal
        //cerr << "Locked -> Normal" << endl;
        valActStatus[i] = actuatorStatusText[ACTUATOR_STATUS_TXT_OK];
        prevActuatorLockStatus[i] = false;
        fUpdateInfoWarn = 1;
      }
    }
  }
  if (fUpdateInfoWarn) {
    infoWarning[WARNID_LOCK] = false;
    for (int i = 0; i < NUM_ACTUATORS; i ++) {
      if (prevActuatorLockStatus[i]) {
        infoWarning[WARNID_LOCK] = true;
        fprintf(stderr, "\a");
      }
    }
    prevInfoWarning[WARNID_LOCK] = infoWarning[WARNID_LOCK];
  }

  // update external connection server status
  if (extMsgSvr != NULL) {
    valConStatus = extMsgSvr->getSvrStatusStr();
    updateTcpInfoTbl();
  }

  // update plot canvas
  while (robotLog->next()) {
    for (int i = 0; i < NUM_ACTUATORS; i ++) {
      plotCanvasA->updateData(robotLog->getPosition(i), i);
    }
    for (int i = 0; i < NUM_ENCODERS - NUM_ACTUATORS; i ++) {
      plotCanvasB->updateData(robotLog->getPosition(i+NUM_ACTUATORS), i);
    }
  }
  plotCanvasA->update();
  plotCanvasB->update();

  // update log text
  static char lbuf[SIZE_LOCAL_LOG_BUFFER];

  robotLog->getLatestLogText(lbuf, SIZE_LOCAL_LOG_BUFFER);
  if (strlen(lbuf) > 0) {
    textRtcpLog->appendText(lbuf, strlen(lbuf));
    textRtcpLog->makePositionVisible(textRtcpLog->getLength());
  }

  //--------------------------------------------------
  // update target values
  float rtarget[3];  // Target in robot coordinate system
  float ptarget[3];  // Target in patient coordinate system
  rtarget[0] = (1.0-valNormTargetPosition[0]) * 75;
  rtarget[1] = valNormTargetPosition[1] * 95;
  rtarget[2] = valNormTargetPosition[2] * 180;
  transform->invTransform(rtarget, ptarget);
  for (int i = 0; i < 3; i ++) {
    valTargetPosition[i] = ptarget[i];
    valDeltaPosition[i] = valTargetPosition[i] - valCurrentPosition[i];
  }

  // update axial canvas
  onAxialCanvasRepaint(obj, sel, ptr);
  onNeedleCanvasRepaint(obj, sel, ptr);
  
  // update timer
  application->addTimeout(this, 
        MrsvrMainWindow::ID_UPDATE_TIMER, updateInterval);

  return 1;
}


long MrsvrMainWindow::onCmdHold(FXObject*, FXSelector,void*)
{
  DBG_MMW_PRINT("onCmdHold()\n");
  consolePrint(1, true, "HOLD command received.\n");

  robotCommand->setMode(MrsvrStatus::HOLD);
  screenMode = SCR_NORMAL;
  return 1;
}


long MrsvrMainWindow::onCmdActive(FXObject*, FXSelector,void*)
{
  DBG_MMW_PRINT("onCmdActive()\n");
  consolePrint(1, true, "ACTIVE command received.\n");
  
  robotCommand->setMode(MrsvrStatus::HOLD);
  robotCommand->setCommandBy(MrsvrCommand::VOLTAGE);
  for (int i = 0; i < NUM_ACTUATORS; i ++) { // initialize voltage parameters.
    robotCommand->setVoltage(i, 0.0);
  }
  robotCommand->setMode(MrsvrStatus::ACTIVE);
  //robotStatus->setMode(MrsvrStatus::PAUSE);

  //// Open Active Control Panel if it is closed .
  //if (shtControlPanel->getCurrent() != CTRL_PNL_ACTIVE) {
  //  shtControlPanel->onOpenItem(siControlPanel[CTRL_PNL_ACTIVE], SEL_COMMAND, NULL);
  //  //shtControlPanel->setCurrent(CTRL_PNL_ACTIVE);
  //}
  return 1;
}


long MrsvrMainWindow::onCmdEmergency(FXObject*, FXSelector,void*)
{
  DBG_MMW_PRINT("onCmdEmergency()\n");
  consolePrint(1, true, "EMERGENCY command received.\n");
  
  robotCommand->setMode(MrsvrStatus::EMERGENCY);
  return 1;
}

long MrsvrMainWindow::onCmdReset(FXObject*, FXSelector,void*)
{
  DBG_MMW_PRINT("onCmdReset()\n");
  consolePrint(1, true, "RESET command received.\n");
  robotCommand->setMode(MrsvrStatus::RESET);
  return 1;
}



long MrsvrMainWindow::onCmdCalibrate(FXObject*, FXSelector, void*)
{
  DBG_MMW_PRINT("onCmdCalibrate()\n");
  consolePrint(1, true, "CALIBRATION command received.\n");

  //  progDlg->setMessage("Starting calibration....");
  //  progDlg->show(PLACEMENT_OWNER);
  //textStatusProg->setText("Starting automatic calibration...");
  //valStatusProg = 0;
  //robotCommand->setMode(MrsvrStatus::AUTO_CALIB);
  //screenMode =  SCR_CALIBRATION;
  return 1;
}


long MrsvrMainWindow::onCmdStartCom(FXObject*, FXSelector, void*)
{
  DBG_MMW_PRINT("onCmdStartCom()\n");
  consolePrint(1, true, "Starting Remote Control Server...\n");

  if (extMsgSvr) {
    if (extMsgSvr->getSvrStatus() == MrsvrMessageServer::SVR_STOP) {
      extMsgSvr->setPortNo(valConPortNo);
      extMsgSvr->run();
      consolePrint(1, true, "Remote Control Started.\n");
    }
    return 1;
  } else {
    return 0;
  }
}


long MrsvrMainWindow::onCmdStopCom(FXObject*, FXSelector, void*)
{
  DBG_MMW_PRINT("onCmdStopCom()\n");
  consolePrint(1, true, "Stopping Remote Control Server...\n");

  if (extMsgSvr) {
    if (extMsgSvr->getSvrStatus() == MrsvrMessageServer::SVR_WAIT ||
        extMsgSvr->getSvrStatus() == MrsvrMessageServer::SVR_CONNECTED) {
      extMsgSvr->stop();
    }
    return 1;
  } else {
    return 0;
  }
}


long MrsvrMainWindow::onCmdSaveDefault(FXObject*, FXSelector, void*)
{
  storeRegistry();
  return 1;
}


long MrsvrMainWindow::onCmdResetDefault(FXObject*, FXSelector, void*)
{
  loadRegistry();
  return 1;
}


long MrsvrMainWindow::onCmdChooseNeedleInfo(FXObject* obj, FXSelector, void*)
{
  int id = lbEndEffector->getCurrentItem();
  consolePrint(1, true, "onCmdChooseNeedleInfo %d...\n", id);  

  // find selected item
  NeedleInfo* p = defNeedleInfo;
  for (int i = 0; i < id; i ++) {
    if (p == NULL)
      return 1;
    p = p->next;
  }

  if (p) {
    // set values to configuration panel
    valNeedleConfName = p->name;
    valDefNeedleOffset[0] = p->valOffset[0];
    valDefNeedleOffset[1] = p->valOffset[1];
    valDefNeedleOffset[2] = p->valOffset[2];
    valDefNeedleOrientation[0] = p->valOrient[0];
    valDefNeedleOrientation[1] = p->valOrient[1];
    valDefNeedleOrientation[2] = p->valOrient[2];
  }
  return 1;
}

long MrsvrMainWindow::onCmdAddNeedleInfo(FXObject*, FXSelector, void*)
{
  consolePrint(1, true, "onCmdAddNeedleInfo...\n");  

  // find end of the list
  NeedleInfo* p = defNeedleInfo;
  while (p->next) {
    p = p->next;
  }
  p->next = new NeedleInfo;
  p = p->next;
  p->next = NULL;

  p->name = valNeedleConfName;
  p->valOffset[0] = valDefNeedleOffset[0];
  p->valOffset[1] = valDefNeedleOffset[1];
  p->valOffset[2] = valDefNeedleOffset[2];
  p->valOrient[0] = valDefNeedleOrientation[0];
  p->valOrient[1] = valDefNeedleOrientation[1];
  p->valOrient[2] = valDefNeedleOrientation[2];

  // add to list box on device configuration panel
  int id = lbEndEffector->appendItem(p->name);
  lbEndEffector->setCurrentItem(id);

  // add to list box on configuration panel
  lbEndEffectorName->appendItem(p->name);
  //lbEndEffector->setCurrentItem(id);

  return 1;
}


long MrsvrMainWindow::onCmdUpdateNeedleInfo(FXObject*, FXSelector, void*)
{
  int currentNeedleId = lbEndEffector->getCurrentItem();

  if (currentNeedleId == 0) {
    valNeedleConfName = defNeedleInfo->name;
    valDefNeedleOffset[0] = defNeedleInfo->valOffset[0];
    valDefNeedleOffset[1] = defNeedleInfo->valOffset[1];
    valDefNeedleOffset[2] = defNeedleInfo->valOffset[2];
    valDefNeedleOrientation[0] = defNeedleInfo->valOrient[0];
    valDefNeedleOrientation[1] = defNeedleInfo->valOrient[1];
    valDefNeedleOrientation[2] = defNeedleInfo->valOrient[2];
    return 1;
  }

  // find selected item
  NeedleInfo* p = defNeedleInfo;
  for (int i = 0; i < currentNeedleId; i ++) {
    if (p == NULL)
      return 1;
    p = p->next;
  }

  if (p) {
    p->name = valNeedleConfName;
    p->valOffset[0] = valDefNeedleOffset[0];
    p->valOffset[1] = valDefNeedleOffset[1];
    p->valOffset[2] = valDefNeedleOffset[2];
    p->valOrient[0] = valDefNeedleOrientation[0];
    p->valOrient[1] = valDefNeedleOrientation[1];
    p->valOrient[2] = valDefNeedleOrientation[2];

    lbEndEffector->setItemText(currentNeedleId, p->name);
    lbEndEffectorName->setItemText(currentNeedleId, p->name);
  }
  
  return 1;
}


long MrsvrMainWindow::onCmdDeleteNeedleInfo(FXObject*, FXSelector, void*)
{
  int currentNeedleId = lbEndEffector->getCurrentItem();

  if (currentNeedleId == 0) {
    return 1;
  }

  // find the item next to the selected item
  NeedleInfo* p = defNeedleInfo;
  for (int i = 0; i < currentNeedleId-1; i ++) {
    if (p == NULL)
      return 1;
    p = p->next;
  }

  if (p->next) {
    NeedleInfo* d = p->next;
    p->next = d->next;

    lbEndEffector->removeItem(currentNeedleId);
    lbEndEffector->setCurrentItem(0);

    lbEndEffectorName->removeItem(currentNeedleId); 
    lbEndEffectorName->setCurrentItem(0);

    valNeedleConfName = defNeedleInfo->name;
    valDefNeedleOffset[0] = defNeedleInfo->valOffset[0];
    valDefNeedleOffset[1] = defNeedleInfo->valOffset[1];
    valDefNeedleOffset[2] = defNeedleInfo->valOffset[2];
    valDefNeedleOrientation[0] = defNeedleInfo->valOrient[0];
    valDefNeedleOrientation[1] = defNeedleInfo->valOrient[1];
    valDefNeedleOrientation[2] = defNeedleInfo->valOrient[2];

    delete d;
  }

  return 1;
}


long MrsvrMainWindow::onCalibLeftBtnPressed(FXObject* obj, FXSelector sel,void* p)
{
  int actuator =  getObjectIndex(obj, (FXObject**)btCalibLeft, NUM_ACTUATORS);
  DBG_MMW_PRINT("onCalibLeftBtnPressed() for Actuator #%d.\n", actuator);
  consolePrint(2, true, "Moving actuator #%d in direction '-' for calibration.\n", actuator);

  robotCommand->setCalibrationCommand(MrsvrCommand::CALIBRATION_MANUAL);
  robotCommand->setMode(MrsvrStatus::CALIBRATION);
  robotCommand->setCommandBy(MrsvrCommand::VOLTAGE);
  if (actuator >= 0 || actuator < NUM_ACTUATORS) {
    float vol;
    vol = ((float) valManualActuatorVol[actuator] / 
           (float) (MANUAL_VOL_STEPS - 1)) * MAX_ACTUATOR_VOL_V;
    robotCommand->setVoltage(actuator, -vol);
  }

  return 0;
}


long MrsvrMainWindow::onCalibRightBtnPressed(FXObject* obj, FXSelector sel,void* p)
{
  int actuator =  getObjectIndex(obj, (FXObject**)btCalibRight, NUM_ACTUATORS);
  DBG_MMW_PRINT("onCalibRightBtnPressed() for Actuator #%d.\n", actuator);
  consolePrint(2, true, "Move actuator #%d in direction '+' for calibration.\n", actuator);

  robotCommand->setCalibrationCommand(MrsvrCommand::CALIBRATION_MANUAL);
  robotCommand->setMode(MrsvrStatus::CALIBRATION);
  robotCommand->setCommandBy(MrsvrCommand::VOLTAGE);
  if (actuator >= 0 || actuator < NUM_ACTUATORS) {
    float vol;
    vol = ((float) valManualActuatorVol[actuator] / 
           (float) (MANUAL_VOL_STEPS - 1)) * MAX_ACTUATOR_VOL_V;
    robotCommand->setVoltage(actuator, vol);
  }

  return 0;
}


long MrsvrMainWindow::onCalibLeftBtnReleased(FXObject* obj, FXSelector sel,void* p)
{
  int actuator =  getObjectIndex(obj, (FXObject**)btCalibLeft, NUM_ACTUATORS);
  DBG_MMW_PRINT("onCalibLeftBtnReleased() for Actuator #%d.\n", actuator);
  consolePrint(2, true, "Stop actuator #%d.\n", actuator);

  robotCommand->setCalibrationCommand(MrsvrCommand::CALIBRATION_STOP);
  robotCommand->setMode(MrsvrStatus::CALIBRATION);
  robotCommand->setCommandBy(MrsvrCommand::VOLTAGE);
  if (actuator >= 0 || actuator < NUM_ACTUATORS) {
    robotCommand->setVoltage(actuator, 0.0);
  }

  return 0;
}


long MrsvrMainWindow::onCalibRightBtnReleased(FXObject* obj, FXSelector sel,void* p)
{
  int actuator =  getObjectIndex(obj, (FXObject**)btCalibRight, NUM_ACTUATORS);
  DBG_MMW_PRINT("onCalibRightBtnReleased() for Actuator #%d.\n", actuator);
  consolePrint(2, true, "Stop actuator #%d.\n", actuator);

  robotCommand->setCalibrationCommand(MrsvrCommand::CALIBRATION_STOP);
  robotCommand->setMode(MrsvrStatus::CALIBRATION);
  robotCommand->setCommandBy(MrsvrCommand::VOLTAGE);
  if (actuator >= 0 || actuator < NUM_ACTUATORS) {
    robotCommand->setVoltage(actuator, 0.0);
  }

  return 0;
}


long MrsvrMainWindow::onCalibZeroBtnPressed(FXObject* obj, FXSelector sel,void* p)
{
  int encoder =  getObjectIndex(obj, (FXObject**)btCalibZero, NUM_ENCODERS);

  DBG_MMW_PRINT("onCalibZeroBtnPressed() for Encoder #%d.\n", encoder);
  consolePrint(2, true, "Set encoder #%d zero.\n", encoder);

  if (encoder >= 0 || encoder < NUM_ENCODERS) {
    robotCommand->setMode(MrsvrStatus::CALIBRATION);
    robotCommand->setZeroFlagOn(encoder);
  }
  return 0;
}


long MrsvrMainWindow::onCalibZeroBtnReleased(FXObject* obj, FXSelector sel,void* p)
{
  int encoder =  getObjectIndex(obj, (FXObject**)btCalibZero, NUM_ENCODERS);
  DBG_MMW_PRINT("onCalibZeroBtnReleased() for Encoder #%d.\n", encoder);
  consolePrint(2, true, "Set encoder #%d zero.\n", encoder);

  if (encoder >= 0 || encoder < NUM_ENCODERS) {
    robotCommand->setMode(MrsvrStatus::CALIBRATION);
    robotCommand->setZeroFlagOff(encoder);
  }
  return 0;
}


long MrsvrMainWindow::onCmdChooseEndEffectorConfig(FXObject* obj, FXSelector, void*)
{
  int id = lbEndEffectorName->getCurrentItem();
  consolePrint(1, true, "onCmdChooseEndEffectorConfig %d...\n", id);  

  // find selected item
  NeedleInfo* p = defNeedleInfo;
  for (int i = 0; i < id; i ++) {
    if (p == NULL)
      return 1;
    p = p->next;
  }

  if (p) {
    // set values to configuration panel
    valNeedleOffset[0] = p->valOffset[0];
    valNeedleOffset[1] = p->valOffset[1];
    valNeedleOffset[2] = p->valOffset[2];
    valNeedleOrientation[0] = p->valOrient[0];
    valNeedleOrientation[1] = p->valOrient[1];
    valNeedleOrientation[2] = p->valOrient[2];
  }
  return 1;
}


long MrsvrMainWindow::onSetEndEffectorBtnReleased(FXObject* obj, FXSelector sel,void* p)
{
  DBG_MMW_PRINT("onSetEndEffectorBtnReleased().\n");
  consolePrint(2, true, "Setting end effector informations...\n");

  for (int i = 0; i < 3; i ++) {
    robotCommand->setTipOffset(i, valNeedleOffset[i]);
  }

  // calculate normal vector of default needle orientation
  float na, nb, nc, l;
  na = valNeedleOrientation[0];
  nb = valNeedleOrientation[1];
  nc = valNeedleOrientation[2];
  l  = sqrt(na*na + nb*nb + nc*nc);

  valNeedleOrientation[0] = na / l;
  valNeedleOrientation[1] = nb / l;
  valNeedleOrientation[2] = nc / l;

  for (int i = 0; i < 3; i ++) {
    robotCommand->setTipApprOrient(i, valNeedleOrientation[i]);
  }
  return 0;
}


long MrsvrMainWindow::onCancelEndEffectorBtnReleased(FXObject* obj, FXSelector sel,void* p)
{
  DBG_MMW_PRINT("onSetEndEffectorBtnRelased().\n");
  //consolePrint(2, true, "Setting end effector informations...\n");
  for (int i = 0; i < 3; i ++) {
    valNeedleOffset[i] = robotCommand->getTipOffset(i);
  }
  return 0;
}


long MrsvrMainWindow::onManualLeftBtnPressed(FXObject* obj, FXSelector sel,void* p)
{
  int actuator =  getObjectIndex(obj, (FXObject**)btManualLeft, NUM_ACTUATORS);
  DBG_MMW_PRINT("onManualLeftBtnPressed() for Actuator #%d.\n", actuator);
  consolePrint(2, true, "Move actuator #%d in direction '-'.\n", actuator);


  if (actuator >= 0 || actuator < NUM_ACTUATORS) {
    float vol;
    vol = ((float) valManualActuatorVol[actuator] / 
           (float) (MANUAL_VOL_STEPS - 1)) * MAX_ACTUATOR_VOL_V;
    robotCommand->setVoltage(actuator, -vol);
  }

  // Return 0 to make message handler in FXButton to run the original 
  // message handling process.
  return 0;
}


long MrsvrMainWindow::onManualRightBtnPressed(FXObject* obj, FXSelector sel, void* p)
{
  int actuator =  getObjectIndex(obj, (FXObject**)btManualRight, NUM_ACTUATORS);
  DBG_MMW_PRINT("onManualRightBtnPressed() for Actuator #%d.\n", actuator);
  consolePrint(2, true, "Move actuator #%d in direction '+'.\n", actuator);

  if (actuator >= 0 || actuator < NUM_ACTUATORS) {
    float vol;
    vol = ((float) valManualActuatorVol[actuator] / 
           (float) (MANUAL_VOL_STEPS - 1)) * MAX_ACTUATOR_VOL_V;
    robotCommand->setVoltage(actuator, vol);
  }
  return 0;
}


long MrsvrMainWindow::onManualLeftBtnReleased(FXObject* obj, FXSelector sel, void* p)
{
  int actuator =  getObjectIndex(obj, (FXObject**)btManualLeft, NUM_ACTUATORS);
  DBG_MMW_PRINT("onManualLeftBtnReleased() for Actuator #%d.\n", actuator);
  //consolePrint(2, true, "Move actuator #%d in direction '+'.\n", actuator);

  if (actuator >= 0 || actuator < NUM_ACTUATORS) {
    robotCommand->setVoltage(actuator, 0.0);
  }

  // Return 0 to make message handler in FXButton to run the original 
  // message handling process.
  return 0;
}


long MrsvrMainWindow::onManualRightBtnReleased(FXObject* obj, FXSelector sel, void* p)
{
  int actuator =  getObjectIndex(obj, (FXObject**)btManualRight, NUM_ACTUATORS);
  DBG_MMW_PRINT("onManualRightBtnReleased() for Actuator #%d.\n", actuator);
  //consolePrint(2, true, "Move actuator #%d in direction '+'.\n", actuator);

  if (actuator >= 0 || actuator < NUM_ACTUATORS) {
    robotCommand->setVoltage(actuator, 0.0);
  }

  // Return 0 to make message handler in FXButton to run the original 
  // message handling process.
  return 0;
}


long MrsvrMainWindow::onSetTargetBtnReleased(FXObject* obj, FXSelector sel,void* p)
{
  DBG_MMW_PRINT("onSetTargetBtnReleased().\n");
  consolePrint(2, true, "Setting target position...\n");

  setTargetPositionRAS(valTargetPosition);
  return 0;
}


long MrsvrMainWindow::onCancelTargetBtnReleased(FXObject* obj, FXSelector sel,void* p)
{
  DBG_MMW_PRINT("onCancelTargetBtnReleased().\n");
  consolePrint(2, true, "Reset tareget position.\n");

  for (int i = 0; i < 3; i ++) {
    valTargetPosition[i] = robotCommand->getSetPoint(i);
  }
  return 0;
}

long MrsvrMainWindow::onUpdateNeedleApprOffset(FXObject* obj, FXSelector sel, void* p)
{
  robotCommand->setTipApprOffset(valTipApprOffset);
  consolePrint(2, true, "Needle approach offset: %f.\n", valTipApprOffset);
  return 0;
}


/*
long MrsvrMainWindow::onNeedleApprBtnPressed(FXObject* obj, FXSelector sel,void* p)
{
  DBG_MMW_PRINT("onNeedleApprBtnPressed().\n");
  //consolePrint(2, true, "Reset tareget position.\n");
  robotCommand->setTipApprSw(NEEDLE_APPROACH);
  return 0;
}

long MrsvrMainWindow::onNeedleApprBtnReleased(FXObject* obj, FXSelector sel,void* p)
{
  DBG_MMW_PRINT("onNeedleApprBtnReleased().\n");
  //consolePrint(2, true, "Reset tareget position.\n");
  robotCommand->setTipApprSw(NEEDLE_STOP);
  return 0;
}

long MrsvrMainWindow::onNeedleLeaveBtnPressed(FXObject* obj, FXSelector sel,void* p)
{
  DBG_MMW_PRINT("onNeedleLeaveBtnPressed().\n");
  //consolePrint(2, true, "Reset tareget position.\n");
  robotCommand->setTipApprSw(NEEDLE_LEAVE);
  return 0;
}

long MrsvrMainWindow::onNeedleLeaveBtnReleased(FXObject* obj, FXSelector sel,void* p)
{
  DBG_MMW_PRINT("onNeedleLeaveBtnReleased().\n");
  //consolePrint(2, true, "Reset tareget position.\n");
  robotCommand->setTipApprSw(NEEDLE_STOP);
  return 0;
}
*/

long MrsvrMainWindow::onShutdownMrsvrBtnReleased(FXObject* obj, FXSelector sel, void* p)
{
  DBG_MMW_PRINT("onShutdownMrsvrBtnReleased().\n");
  consolePrint(2, true, "Shutting down MRSVR....\n");

  robotCommand->setShutdown(true);
  
  for (int actuator = 0; actuator < NUM_ACTUATORS; actuator ++) {
    robotCommand->setVoltage(actuator, 0.0);
  }

  storeRegistry();

  this->getApp()->exit();

  return 0;
  
}


long MrsvrMainWindow::onCmdManualPowerSw(FXObject* obj, FXSelector sel, void* p)
{
  //  cerr << "onCmdManualPowerSw(" << int(obj) << "," << int(sel) << "," << int(p) << endl;
  int actuator = (FXSELID(sel) - ID_MANUAL_PW_OFF_0) / 2;
  int swState =  (FXSELID(sel) - ID_MANUAL_PW_OFF_0) % 2;

  if (actuator < NUM_ACTUATORS) {
    manualPowerSw[actuator] = swState;
    if (swState > 0) {
      robotCommand->activateActuator(actuator);
      //cerr << "activate #" << int(actuator) << endl;
    } else {
      robotCommand->deactivateActuator(actuator);
      //cerr << "deactivate #" << int(actuator) << endl;
    }
  }
  return 1;
}


long MrsvrMainWindow::onUpdateManualPowerSw(FXObject* obj, FXSelector sel, void* p)
{
  //  cerr << "onUpdateManualPowerSw(" << int(obj) << "," << int(sel) << "," << int(p) << endl;
  int actuator = (FXSELID(sel) - ID_MANUAL_PW_OFF_0) / 2;
  int onoff =  (FXSELID(sel) - ID_MANUAL_PW_OFF_0) % 2;

  if (actuator < NUM_ACTUATORS) {
    if (manualPowerSw[actuator] == onoff) {
      obj->handle(this,FXSEL(SEL_COMMAND,ID_CHECK),NULL);
    } else {
      obj->handle(this,FXSEL(SEL_COMMAND,ID_UNCHECK),NULL);
    }
  }
  return 1;
}


//Shows new Dialog Box
long MrsvrMainWindow::onCmdShowDialog(FXObject*,FXSelector,void*)
{
  robotCommand->setMode(MrsvrStatus::CALIBRATION);

  MrsvrZFrameRegistrationDialog* modaldialog = new MrsvrZFrameRegistrationDialog(this);
  modaldialog->execute(PLACEMENT_OWNER);
  
  float matrix[16];
  modaldialog->getRegistrationMatrix(matrix);
  
  for (int i = 0; i < 16; i ++) {
    valCalibrationMatrix[i] = matrix[i];
  }
  transform->setCalibrationMatrix(matrix);
  
  delete modaldialog;
  
  return 1;
}


