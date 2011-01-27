//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: mainController.cpp,v $
// $Revision: 1.13 $ 
// $Author: junichi $
// $Date: 2006/01/19 07:35:15 $
//====================================================================


//====================================================================
// Description: 
//    Main Controller for MRI guided robot control system.
//====================================================================


// Flag to use real-time API of ART Linux
#if !defined(WITHOUT_ART) && !defined(USE_ART)
  #define USE_ART
#endif

#define _WITHOUT_LIMIT_SENSOR 

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#ifdef __LINUX__
  #include <sys/io.h>
#endif
#ifdef USE_ART
  #include <linux/art_task.h>
#endif
#include <iostream>

#include "MrsvrStatus.h"
#include "MrsvrLog.h"
#include "MrsvrCommand.h"
#include "MrsvrDev.h"
#include "MrsvrRAS.h"
//#include "MrsvrTransform.h"
#include "shmKeys.h"

using namespace std;

#define _USE_MRSVR_CONSOLE

#ifdef DEBUG
 #ifdef _USE_MRSVR_CONSOLE
  #define _XDEBUG_MAIN_CONTROLLER
 #else
  #define _DEBUG_MAIN_CONTROLLER
 #endif
#endif

#define SOFTWARE_NAME "Real-time Control Process (RTCP) for ATRE MR-Servo Robot."
#define COPYRIGHT_STR "Copyright (C) 2003-2005, ATRE Lab. The University of Tokyo. All Rights Reserved.\nCopyright (C) 2005-2011, Shiga University of Medical Science. All Right Reserved.\nCopyrigth (C) 2006-2011, Surgical Navigation and Robotics Lab., Brigham and Women's Hospital."

// Macro to output debug strings
// NOTE: DBG_PRINT can be use only when 'MrsvrLogWriter' 
// instance is available
#if (__GNUC__ >= 3)  // in case of GCC 3.x
  #ifdef _DEBUG_MAIN_CONTROLLER
    #define DBG_PRINT(...)   fprintf(stderr, "DEBUG:mainController >> " __VA_ARGS__)
  #elif defined(_XDEBUG_MAIN_CONTROLLER)
    #define DBG_PRINT(...)   slog->addLogText("DEBUG:mainController >> " __VA_ARGS__)
  #else
    #define DBG_PRINT(...)   
  #endif
  #ifdef _USE_MRSVR_CONSOLE
    #define CONSOLE_PRINT(...) slog->addLogText(__VA_ARGS__)
  #else
    #define CONSOLE_PRINT(...) printf(__VA_ARGS__)
  #endif
#elif defined(__GNUC__) // in case of GCC 2.x
  #ifdef _DEBUG_MAIN_CONTROLLER
    #define DBG_PRINT(s...)   fprintf(stderr, "DEBUG:mainController >> " s)
  #elif defined(_XDEBUG_MAIN_CONTROLLER)
    #define DBG_PRINT(s...)   slog->addLogText("DEBUG:mainController >> " s)
  #else
    #define DBG_PRINT(s...)   
  #endif
  #ifdef _USE_MRSVR_CONSOLE
    #define CONSOLE_PRINT(s...) slog->addLogText(s)
  #else
    #define CONSOLE_PRINT(s...) printf(s)
  #endif
#endif



#ifdef _USE_MRSVR_CONSOLE
  #define DUMP_POSITION(mp) {\
      printf("  nx  = %d\n", mp->getNX());\
      printf("  ny  = %d\n", mp->getNY());\
      printf("  nz  = %d\n", mp->getNZ());\
      printf("  tx  = %d\n", mp->getTX());\
      printf("  ty  = %d\n", mp->getTY());\
      printf("  tz  = %d\n", mp->getTZ());\
      printf("  px  = %d\n", mp->getPX());\
      printf("  py  = %d\n", mp->getPY());\
      printf("  pz  = %d\n", mp->getPZ());\
      printf("=====================\n");}
#else
  #define DUMP_POSITION(mp) {\
      slog->addLogText("  nx  = %d\n", mp->getNX());\
      slog->addLogText("  ny  = %d\n", mp->getNY());\
      slog->addLogText("  nz  = %d\n", mp->getNZ());\
      slog->addLogText("  tx  = %d\n", mp->getTX());\
      slog->addLogText("  ty  = %d\n", mp->getTY());\
      slog->addLogText("  tz  = %d\n", mp->getTZ());\
      slog->addLogText("  px  = %d\n", mp->getPX());\
      slog->addLogText("  py  = %d\n", mp->getPY());\
      slog->addLogText("  pz  = %d\n", mp->getPZ());\
      slog->addLogText("=====================\n");}
#endif


// shared data
MrsvrStatusWriter*            status;
MrsvrCommandReader*           command;
MrsvrLogWriter*               slog;
MrsvrRASReader*               setPoint;
MrsvrRASReader*               currentRAS;


// control module
MrsvrDev*                     dev;

// Log mode  :  enabled when the log file name is specified in second argument
int logmode;
char* logfilename;


// RAS-XYZ transformation 
//MrsvrTransform*               trans;

// roop interval
#define INTERVAL_UNIT         0.000001        // [s]
long    interval;             // Main roop interval in micro seconds
float   intervalf;            // Main roop interval in seconds
float   secDisplIntv;         // Duration to measure displacements
                              // calcurate average velocity in seconds.
bool    fModeChange;          // flag for mode change.
                              // This flag becomes TRUE only one cycle
                              // of main loop after mode has been changed.

// Position history data to calcurate average velocity
#define NUM_POSITION_HISTORY  5
static float posHist[NUM_ENCODERS][NUM_POSITION_HISTORY];
static int   pPosHist;

// current position and velocity
static float curPos[NUM_ENCODERS];
static float curVel[NUM_ENCODERS];
static int   fOutOfRange[NUM_ENCODERS]; 
                  // =0: in range, >0: greater than maximum, <0: less than minimum


// Out of Range flag
//static int   fOutOfRange[NUM_ENCODERS];

inline int procStop();
int  trapCtrl(MrsvrVector, float);
int  trapCtrl2(MrsvrVector, float);
int  remoteCtrl(MrsvrVector, float);
void getActuatorTarget(MrsvrVector& target, MrsvrVector setPoint,
                       float theta, float phi);



inline void printDate()
{
  time_t ct = 0;
  struct tm ctm;
  time(&ct);
  localtime_r(&ct, &ctm);
#ifdef _USE_MRSVR_CONSOLE
  slog->addLogText("[%04d/%02d/%02d %02d:%02d:%02d] ",
                   ctm.tm_year+1900, ctm.tm_mon+1, ctm.tm_mday,
                   ctm.tm_hour, ctm.tm_min, ctm.tm_sec);
#else
  fprintf(stderr, "[%04d/%02d/%02d %02d:%02d:%02d] ",
          ctm.tm_year+1900, ctm.tm_mon+1, ctm.tm_mday,
          ctm.tm_hour, ctm.tm_min, ctm.tm_sec);
#endif
}


int init()
{
  cout << "Starting log interface..." << endl;
  slog        = new MrsvrLogWriter(SHM_LOG);

  CONSOLE_PRINT("Logging shared memory interface have been attached.\n\n");
  CONSOLE_PRINT(SOFTWARE_NAME "\n");
  CONSOLE_PRINT(COPYRIGHT_STR "\n\n");
  
#ifdef USE_ART
  //cout << "Use ART Linux Real-time APIs." << endl;
  CONSOLE_PRINT("Use ART Linux Real-time APIs.\n");
#endif  

  // initialize position history
  for (int i = 0; i < NUM_ENCODERS; i ++) { 
    for (int j = 0; j < NUM_POSITION_HISTORY; j ++) {
      posHist[i][j] = 0.0;
    }
    curPos[i] = 0.0; 
    fOutOfRange[i] = 0;
  }
  pPosHist = 0;

  printDate();
  CONSOLE_PRINT("Initializing hardwares...\n");
  dev        = new MrsvrDev();

  printDate();
  CONSOLE_PRINT("Attaching shared information memory...\n");
  status     = new MrsvrStatusWriter(SHM_STATUS);
  CONSOLE_PRINT("    - hardware status\n");
  command    = new MrsvrCommandReader(SHM_COMMAND);
  CONSOLE_PRINT("    - external command\n");
  setPoint   = new MrsvrRASReader(SHM_RAS_SETPOINT);
  CONSOLE_PRINT("    - set point\n");
  currentRAS = new MrsvrRASReader(SHM_RAS_CURRENT);
  CONSOLE_PRINT("    - current position\n");
  CONSOLE_PRINT("  OK.\n");
  //trans  = new MrsvrTransform();

  // register the limit of motion to the shared memory
  status->setLimitPos((float*)dev->getLimitMins(), (float*)dev->getLimitMaxs());
  procStop();
  return 1;
}


inline int procStop()
{
  if (status->getMode() != MrsvrStatus::STOP) {
    status->setMode(MrsvrStatus::STOP);
  }
  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    dev->setVoltage(i, 0.0);
    status->setVoltage(i, 0.0);
    DBG_PRINT("setVoltage(%d, %f) \n", i, command->getVoltage(i));
  }
  status->setInfoText("");
  status->setProgress(0);

  return 1;
}


//
// inline int procAutoCalib()
// 
// A subroutine for automatic calibration.
// The calibration consists of two phases: reset encoder counters 
// (CALIB_ZERO) and registration of robot coordinate system and image
// coordinate system (CALIB_REGISTRATION).
//
// To reset encode counters at the position of zero limitter, this routine
// performs following steps:
//   1. Move an actuator into range of motion.
//   2. Move the actuator to the zero limitter, and set encoder counter
//      to zero.
//

#define WAIT_LOCATOR     5000000   // (us)
#define WAIT_ANGLE_SET   500000    // (us)

inline int procAutoCalib(bool init)
{
  enum {
    CALIB_INTORANGE,
    CALIB_TOHOME,
    CALIB_SETANGLE,
    CALIB_REGISTRATION,
    CALIB_ADJUST,
  };
  
  static char strbuf[MAX_INFO_TEXT_SIZE];
  static int  calibStatus;
  static bool fUpdateCalibStatus; // True if calibStatus is updated.
  static bool calibProcSwitch[6];
  static int ls;
  static int actuator;
  //  static MrsvrVector angleSetPoint = { 0.0, 0.0, 0.0 };
  MrsvrVector angleSetPoint;
  static MrsvrVector calibPoint;
  // add variable to recode each RAS position
  static int calibPointIndex;
  static int waitAngleSet;
  static int waitLocator;

  if (init) {
    printDate();
    CONSOLE_PRINT("Automatic calibration started.\n");
    CONSOLE_PRINT("  Preparing automatic calibration...\n");
    status->setInfoText("Start to calibrate actuators and sensors ...");
    calibStatus = CALIB_INTORANGE;
    actuator    = 0;
    status->setProgress(0);
    //sprintf(strbuf, "Moving actuator #%d to the home position...", actuator);
    //status->setInfoText(strbuf);
    //CONSOLE_PRINT(strbuf);
    status->setMode(MrsvrStatus::AUTO_CALIB);
    status->setCurrentCalibPointIndex(-1);

    fUpdateCalibStatus = true;

    // calibration process switch
    for (int i = 0; i < 6; i ++) {
      calibProcSwitch[i] = true;
    }
    if (!command->isAutoCalibProcEnable(AUTOCALIB_PROC_HOME)) {
      calibProcSwitch[CALIB_TOHOME] = false;
    }
    if (!command->isAutoCalibProcEnable(AUTOCALIB_PROC_ANGLE)) {
      calibProcSwitch[CALIB_SETANGLE] = false;
    }
    if (!command->isAutoCalibProcEnable(AUTOCALIB_PROC_REGISTRATION)) {
      calibProcSwitch[CALIB_REGISTRATION] = false;
    }
    status->setSetAngleReadyIndex(-1);
    status->setCalibReadyIndex(-1);
  }

  switch (calibStatus) {
  case CALIB_INTORANGE:
    if (fUpdateCalibStatus) {
      actuator = 0;
      fUpdateCalibStatus = false;
      if (!calibProcSwitch[calibStatus]) {
        calibStatus ++;
      }
    }

#ifdef _ENABLE_ACTUATOR_LOCK_DETECTION
    if (dev->isActuatorLocked(actuator)) {
      //printf("actuator #%d is locked.", actuator);
      ls = 0;
    } else 
#endif //_ENABLE_ACTUATOR_LOCK_DETECTION
    {
      ls = dev->getLimitSensorStatus(actuator);
    }
    if (ls > 0) {
      dev->setVoltage(actuator, -1.5);
      status->setVoltage(actuator, dev->getVoltage(actuator));
    } else if (ls < 0) {
      dev->setVoltage(actuator, 1.5);
      status->setVoltage(actuator, dev->getVoltage(actuator));
    } else {
      dev->setVoltage(actuator, 0.0);
      status->setVoltage(actuator, dev->getVoltage(actuator));
      actuator ++;
      if (actuator >= NUM_ACTUATORS) {
        calibStatus ++;
        fUpdateCalibStatus = true;
        sprintf(strbuf, "Moving actuator #%d into range...", actuator);
        status->setInfoText(strbuf);
      } else {
        sprintf(strbuf, "Moving actuator #%d into range...", actuator);
        status->setInfoText(strbuf);
      }
    }
    status->setProgress(10);
    break;
  case CALIB_TOHOME:
    if (fUpdateCalibStatus) {
      actuator = 0;
      fUpdateCalibStatus = false;
      if (!calibProcSwitch[calibStatus]) {
        calibStatus ++;
        printDate();
        CONSOLE_PRINT("Setting HOME POSITION skipped.\n");
      } else {
        sprintf(strbuf, "Moving actuator #%d to the home position...", actuator);
        status->setInfoText(strbuf);
      }
    }
#ifdef _ENABLE_ACTUATOR_LOCK_DETECTION
    if (dev->isActuatorLocked(actuator)) {
      //printf("actuator #%d is locked.", actuator);
      ls = -1;
    } else
#endif //_ENABLE_ACTUATOR_LOCK_DETECTION
    {
      ls = dev->getLimitSensorStatus(actuator);
    }
    if (ls == 0) {
      dev->setVoltage(actuator, -1.5);
      status->setVoltage(actuator, dev->getVoltage(actuator));
    } else {
      dev->setVoltage(actuator, 0.0);
      status->setVoltage(actuator, dev->getVoltage(actuator));
      dev->setZero(actuator);  // reset encoder to zero
      actuator ++;
      if (actuator >= NUM_ACTUATORS) {
        calibStatus ++;
        fUpdateCalibStatus = true;
      } else {
      }
    }
    status->setProgress(20);
    break;
  case CALIB_SETANGLE:
    if (fUpdateCalibStatus) {
      actuator = 0;
      fUpdateCalibStatus = false;
      waitLocator = WAIT_LOCATOR / interval;
      waitAngleSet = WAIT_ANGLE_SET / interval;
      status->setSetAngleReadyIndex(-1);
      if (!calibProcSwitch[calibStatus]) {
        calibStatus ++;
        printDate();
        CONSOLE_PRINT("Setting ANGLE skipped.\n");
      }
    }
    command->getAutoCalibPoint(0, angleSetPoint);
    if (trapCtrl(angleSetPoint, dev->getVmax(0)) <= 0) { 
      status->setInfoText("Reading locator information...");
      // when stage is arrive at the destination
      waitLocator --;
      if (waitLocator < 0) {
        // -- call flashpoint reading fucntion here !! --
        status->setSetAngleReadyIndex(0);
        if (command->getSetAngleDoneIndex() == 0) {
          float angles[2];
          status->setSetAngleReadyIndex(-1);
          command->getSetAngles(angles);
          CONSOLE_PRINT("Encoder #3: %f, Encoder #4: %f.\n", angles[0], angles[1]);
          dev->setPosition(NUM_ACTUATORS,   angles[0]);
          dev->setPosition(NUM_ACTUATORS+1, angles[1]);

          calibStatus ++;
          fUpdateCalibStatus = true;
          status->setProgress(40);
        }
      }
    } else { 
      // while stage is moving to the destination
      status->setInfoText("Setting end effector angle...");
      status->setProgress(30);
    }
    break;

  case CALIB_REGISTRATION:
    if (fUpdateCalibStatus) {
      actuator = 0;
      fUpdateCalibStatus = false;
      calibPointIndex = 0;
      waitLocator = WAIT_LOCATOR / interval;
      status->setCurrentCalibPointIndex(-1);
      status->setCalibReadyIndex(-1);
      if (!calibProcSwitch[calibStatus]) {
        calibStatus ++;
        printDate();
        CONSOLE_PRINT("REGISTRATION skipped.\n");
      }
    }
    if (calibPointIndex >= NUM_CALIB_POINTS) {
      calibStatus ++;
      fUpdateCalibStatus = true;
      status->setCurrentCalibPointIndex(-1);
    } else if (calibPointIndex == NUM_CALIB_POINTS) {
      // waiting for transform module to calicurate matrix.
      if (command->getCalibDoneIndex() == calibPointIndex) { 
        calibPointIndex ++;
      }
    } else {
      command->getAutoCalibPoint(calibPointIndex, calibPoint);
      status->setCurrentCalibPointIndex(calibPointIndex);
      if (trapCtrl(calibPoint, dev->getVmax(0)) <= 0) {
        sprintf(strbuf, "Setting fiducial point #%d...", calibPointIndex);
        status->setInfoText(strbuf);
        waitLocator --;
        if (waitLocator < 0) {
          // -- notify to IF that calibration is ready
          status->setCalibReadyIndex(calibPointIndex);
          if (command->getCalibDoneIndex() == calibPointIndex) { 
            // After reading locator information 
            waitLocator = WAIT_LOCATOR / interval;
            calibPointIndex ++;
          }
        }
      } else {
        sprintf(strbuf, "Moving to fiducial point #%d...", calibPointIndex);
        status->setInfoText(strbuf);
      }
    }
    break;
  default:
    if (fUpdateCalibStatus) {
      waitLocator = WAIT_LOCATOR / interval;
      fUpdateCalibStatus = false;
    }
    waitLocator --;
    status->setInfoText("Calibration completed.");
    status->setProgress(100);
    if (waitLocator < 0) {
      calibPointIndex = -1;
      status->setCalibReadyIndex(-1);
    }
    break;
  }
  return 1;
}


inline int procPause()
{
  if (status->getMode() != MrsvrStatus::PAUSE) {
    status->setMode(MrsvrStatus::PAUSE);
  }
  // stop all actuators
  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    dev->setVoltage(i, 0.0);
    status->setVoltage(i, dev->getVoltage(i));
    DBG_PRINT("setVoltage(%d, %f) \n", i, dev->getVoltage(i));
  }
  return 1;
}


inline int procMoveTo()
{
  //  static int done = -1;
  static MrsvrVector spim, sprb;
  static float t;

  if (status->getMode() != MrsvrStatus::MOVE_TO) {
    status->setMode(MrsvrStatus::MOVE_TO);
    //    spim[0] = setPoint->getPR();
    //    spim[1] = setPoint->getPA();
    //    spim[2] = setPoint->getPS();

    spim[0] = command->getSetPoint(0);
    spim[1] = command->getSetPoint(1);
    spim[2] = command->getSetPoint(2);

    //trans->transform(sprb, spim);
    getActuatorTarget(sprb, spim, curPos[3], curPos[4]);    

    t = 0.0;
    //cerr << "x = " << spim[0] << ", y = " << spim[1]
    //<< ", z = " << spim[2] << endl;
    status->setMode(MrsvrStatus::MOVE_TO);
  }
  if (trapCtrl(sprb, dev->getVmax(0)) <= 0) {
    //if (trapCtrl(spim, dev->getVmax(0)) <= 0) {
    status->setMode(MrsvrStatus::PAUSE);
  }
  /*
  for (int i = 0; i < NUM_ENCODERS; i ++) {
    float sv;
    sv = dev->getVoltage(i);
    status->setVoltage(i, sv);
  }
  */
  return 1;
}


inline int procManualCalib()
{
  if (status->getMode() != MrsvrStatus::MANUAL_CALIB) {
    status->setMode(MrsvrStatus::MANUAL_CALIB);
  }
  int f = 0;
  for (int i = 0; i < NUM_ENCODERS; i ++) {
    if (command->getZeroFlag(i)) {
      dev->setZero(i);
      f = 1;
    }
  }
  if (f) return 1;
  switch(command->getCommandBy()) {
  case MrsvrCommand::VOLTAGE:
    for (int i = 0; i < NUM_ACTUATORS; i ++) {
      dev->setVoltage(i, command->getVoltage(i));
      status->setVoltage(i, dev->getVoltage(i));
      DBG_PRINT("setVoltage(%d, %f) \n", i, dev->getVoltage(i));
    }
    break;
  case MrsvrCommand::VELOCITY:
    break;
  case MrsvrCommand::POSITION:
    break;
  default:
    break;
  }

  return 1;
}

inline int procManual()
{
  if (status->getMode() != MrsvrStatus::MANUAL) {
    status->setMode(MrsvrStatus::MANUAL);
  }
  unsigned short swst;
  switch(command->getCommandBy()) {
  case MrsvrCommand::VOLTAGE:
    swst = dev->getSwitchStatus();
    for (int i = 0; i < NUM_ACTUATORS; i ++) {
      //int limit = status->isOutOfRangePos(i);
      int limit = fOutOfRange[i];
      float cmd = 0;
      float lmtv = command->getLmtVoltage(i);
      if (dev->isFwSwitchOn(swst, i)) {
        cmd += lmtv;
      }
      if (dev->isBwSwitchOn(swst, i)) {
        cmd -= lmtv;
      }
      if (cmd == 0.0) {
        cmd = command->getVoltage(i);
      }

      if (limit == 0 ||
          cmd * limit < 0) { // if the signs are different
        dev->setVoltage(i, cmd);
        status->setVoltage(i, dev->getVoltage(i));
        DBG_PRINT("setVoltage(%d, %f) \n", i, dev->getVoltage(i));
      } else {
        dev->setVoltage(i, 0.0);
        status->setVoltage(i, dev->getVoltage(i));
        DBG_PRINT("setVoltage(%d, %f) \n", i, dev->getVoltage(i));
      }
    }
    break;
  case MrsvrCommand::VELOCITY:
    break;
  case MrsvrCommand::POSITION:
    break;
  default:
    break;
  }

  return 1;
}


inline int procRemote()
{
  //static MrsvrVector spim, sprb;
  static MrsvrVector spim;
  int i;
  //  static float dv;

  if (status->getMode() != MrsvrStatus::REMOTE) {
    status->setMode(MrsvrStatus::REMOTE);
  }
  /*
  spim[0] = setPoint->getPR();
  spim[1] = setPoint->getPA();
  spim[2] = setPoint->getPS();
  */
  for (i = 0; i < 3; i ++) {
    spim[i] = command->getSetPoint(i);
  }
  //trans->transform(sprb, spim);
  //rcmCtrl(sprb, dev->getVmax(0));
  remoteCtrl(spim, dev->getVmax(0));
  return 1;
}


inline int procReset()
{
  return 1;
}

#define TH_REACH_ERROR  0.05

// trapezoidal control
int trapCtrl(MrsvrVector setPoint, float vmax)
{
  float newa, newv;
  int reach = 0;
  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    float dist  = setPoint[i] - curPos[i];
    if (fabs(dist) < TH_REACH_ERROR)  {
      reach ++;
      dev->setVelocity(i, 0.0);
      status->setVoltage(i, dev->getVoltage(i));
    } else {
      float  dir  = (dist > 0.0)? 1.0 : -1.0;
      dist        = fabs(dist); 
      float astd  = dev->getAstd(i);
      float vmin  = dev->getVmin(i);
      float dmarg = dev->getDmarg(i);
      float cvel  = dev->getSetVelocity(i);
      float cvs   = (cvel > 0.0)? 1.0 : -1.0;
      float d1    = 0.5*cvel*cvel/astd;
      if (dist < d1 - dmarg) {  // case 1
        newa = 0.5*cvel*cvel/dist;
        newv = cvs*cvel - newa*intervalf;
        //newv = cvel - astd*intervalf;
        //newv = cvel - astd*secDisplIntv/2;
      } else if (dist < vmax*vmax/astd - d1 - dmarg) { // case 2
        newv = cvs*cvel + astd*intervalf;
        //newv = cvel + astd*secDisplIntv/2;
      } else { // case 3
        newv = cvs*cvel + astd*intervalf;
        //newv = cvel + astd*secDisplIntv/2;
      }
      if (newv < vmin) {
        newv = vmin;
      } else if (newv > vmax) {
        newv = vmax;
      }
      float sv = dev->setVelocity(i, dir*newv);
      status->setVoltage(i, sv);
      
    }
  }
  return (NUM_ACTUATORS - reach);
}


// get actuator set point from needle tip target
void getActuatorTarget(MrsvrVector& target, MrsvrVector setPoint,
                       float theta, float phi)
{
  float a, b, c;
  //  MrsvrVector offset;
  float naori_a, naori_b, naori_c;
  float oa, ob, oc;
  
  float naoffset = command->getTipApprOffset();
  if (naoffset != 0.0) {
    naori_a = command->getTipApprOrient(0);
    naori_b = command->getTipApprOrient(1);
    naori_c = command->getTipApprOrient(2);
    if (fabs(naori_a*naori_a + naori_b*naori_b + naori_c*naori_c - 1.0) < 0.001) {
      oa = naoffset * naori_a;
      ob = naoffset * naori_b;
      oc = naoffset * naori_c;
    } else {
      oa = ob = oc = 0.0;
    }
  } else {
    oa = ob = oc = 0.0;
  }

  a = command->getTipOffset(0) - oa;
  b = command->getTipOffset(1) - ob;
  c = command->getTipOffset(2) - oc;
  
  target[0] = setPoint[0] + a*cos(phi) + c*cos(theta)*sin(phi);
  target[1] = setPoint[1] + b*cos(theta) - c*sin(theta);

  target[2] = setPoint[2] + c*cos(theta)*cos(phi) +
    b*sin(theta)*cos(phi) - a*sin(phi);
//target[0] = setPoint[0] + a*cos(phi) + c*cos(theta)*sin(phi) - b*sin(theta);
//  target[2] = setPoint[2] + c*cos(theta)*cos(phi) - 
//    b*sin(theta)*cos(phi) - a*sin(phi);

}


void getTipPosition(MrsvrVector& tipPoint)
{
  float a, b, c;
  //  MrsvrVector offset;
  //float naori_a, naori_b, naori_c;
  //float oa, ob, oc;
  float theta, phi;

  theta = curPos[3];
  phi   = curPos[4];
  
  a = command->getTipOffset(0);
  b = command->getTipOffset(1);
  c = command->getTipOffset(2);
  
  tipPoint[0] = curPos[0] - (a*cos(phi) + c*cos(theta)*sin(phi));
  tipPoint[1] = curPos[1] - (b*cos(theta) - c*sin(theta));

  tipPoint[2] = curPos[2]  - (c*cos(theta)*cos(phi) +
                              b*sin(theta)*cos(phi) - a*sin(phi));
//target[0] = setPoint[0] + a*cos(phi) + c*cos(theta)*sin(phi) - b*sin(theta);
//  target[2] = setPoint[2] + c*cos(theta)*cos(phi) - 
//    b*sin(theta)*cos(phi) - a*sin(phi);

}



// trapezoidal control
//#define NEEDLE_LENGTH     150.0
int trapCtrl2(MrsvrVector setPoint, float vmax)
{
  MrsvrVector  asp;
  float newa, newv;
  int reach = 0;

  // calcurate actuator set point
  /*
  theta = -curPos[3];
  phi = -curPos[4];
  asp[0] = setPoint[0] + NEEDLE_LENGTH * cos(phi) * sin(theta);
  asp[1] = setPoint[1] + NEEDLE_LENGTH * sin(phi);
  asp[2] = setPoint[2] - NEEDLE_LENGTH + NEEDLE_LENGTH*cos(phi)*cos(theta);
  */

  getActuatorTarget(asp, setPoint, curPos[3], curPos[4]);

  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    float dist  = asp[i] - curPos[i];
    if (fabs(dist) < TH_REACH_ERROR)  {
      reach ++;
      dev->setVelocity(i, 0.0);
      status->setVoltage(i, dev->getVoltage(i));
    } else {
      float  dir  = (dist > 0.0)? 1.0 : -1.0;
      dist        = fabs(dist); 
      float astd  = dev->getAstd(i);
      float vmin  = dev->getVmin(i);
      float dmarg = dev->getDmarg(i);
      float cvel  = dev->getSetVelocity(i);
      float cvs   = (cvel > 0.0)? 1.0 : -1.0;
      float d1    = 0.5*cvel*cvel/astd;

      //if (dir*cvs > 0) {  // if actuator is moving towards the target
        if (dist < d1 - dmarg) {  // case 1
          newa = 0.5*cvel*cvel/dist;
          newv = cvs*cvel - newa*intervalf;
          //newv = cvel - astd*intervalf;
          //newv = cvel - astd*secDisplIntv/2;
        } else if (dist < vmax*vmax/astd - d1 - dmarg) { // case 2
          newv = cvs*cvel + astd*intervalf;
          //newv = cvel + astd*secDisplIntv/2;
        } else { // case 3
          newv = cvs*cvel + astd*intervalf;
          //newv = cvel + astd*secDisplIntv/2;
        }
        /*
      } else {   // if actuator is moving against the target
        newv = cvs*cvel - astd*intervalf;
        dir = cvs;
      }
        */
      if (newv < vmin) {
        newv = vmin;
      } else if (newv > vmax) {
        newv = vmax;
      }
      float sv = dev->setVelocity(i, dir*newv);
      status->setVoltage(i, sv);
    }
  }
  return (NUM_ACTUATORS - reach);
}


// control sequence in REMOTE mode.
#define REMOTE_CTRL_KP 0.09
#define REMOTE_CTRL_KD 1.5
#define FSIGN(v)    ((v < 0.0) ? -1.0 : 1.0)
int remoteCtrl(MrsvrVector setPoint, float vmax)
{
  static float asp[NUM_ACTUATORS];  // actuator set points
  float pasp[NUM_ACTUATORS];        // previous actuator set points
  int reach = 0;
  int i;

  // save previous setpoint;
  for (i = 0; i < NUM_ACTUATORS; i ++) {
    pasp[i] = asp[i];
  }
  
  // calcurate actuator set point
  /*
  theta = -curPos[3];
  phi = -curPos[4];
  asp[0] = setPoint[0] + NEEDLE_LENGTH * cos(phi) * sin(theta);
  asp[1] = setPoint[1] + NEEDLE_LENGTH * sin(phi);
  asp[2] = setPoint[2] - NEEDLE_LENGTH + NEEDLE_LENGTH*cos(phi)*cos(theta);
  */
  getActuatorTarget(asp, setPoint, curPos[3], curPos[4]);


  //cout << "======================" << endl;
  for (i = 0; i < NUM_ACTUATORS; i ++) {
    //int limit = status->isOutOfRangePos(i);
    int limit = fOutOfRange[i];

    float cv   = dev->getSetVelocity(i);  // current set velocity
    float nv   = REMOTE_CTRL_KD* (asp[i] - pasp[i]) / intervalf + 
      REMOTE_CTRL_KP * (asp[i] - curPos[i]) / intervalf;
    float a    = (nv - cv) / intervalf;
    float sa   = FSIGN(a);
    float snv  = FSIGN(nv);
    int zflag  = 0;
    //cout << "act# " << i << ", cv=" << cv << ", nv=" << nv << ", a=" << a << endl;
    if (sa*a > dev->getAmax(i)) { // if exceeds maximum accel.
      //cout << "*************" << endl;
      if (nv*cv >= 0.0) {
        nv = cv + dev->getAmax(i) * sa * intervalf;
      } else {
        if (fabs(cv) <= dev->getVmin(i)) {
          nv = 0.0;
          zflag = 1;
        } else {
          nv = cv + dev->getAmax(i) * sa * intervalf;
        }
      }
      snv = FSIGN(nv);
    }
    if (snv*nv > vmax) {   // if set velocity exceeds maximum speed.
      nv = snv*vmax;
    } else if (snv*nv < dev->getVmin(i) && !zflag) {
      nv = snv*dev->getVmin(i);
    }
    if (fabs(asp[i] - curPos[i]) < TH_REACH_ERROR) {
      nv = 0.0;
      reach  ++;
    }
    //cout << "act# " << i << ", cv=" << cv << ", nv=" << nv << endl;
    if (limit == 0 || nv*limit < 0) {  
      // note: nv*limit<0 means that the signs of 'nv' and 'limit' are different.
      float sv = dev->setVelocity(i, nv);
      status->setVoltage(i, sv);
    } else { // in the case of one of the actuators reaches stroke limit
      float sv = dev->setVelocity(i, 0.0);
      status->setVoltage(i, sv);
    }
  }

  return (NUM_ACTUATORS - reach);
}


// Read encoders and calcurate velocities 
inline void getPositions()
{
  pPosHist = (pPosHist+1) % NUM_POSITION_HISTORY;
  dev->getPositions(curPos);
  for (int i = 0; i < NUM_ENCODERS; i ++) {
    float p = curPos[i];
    float delta = p - posHist[i][pPosHist];
    posHist[i][pPosHist] = p;
    status->setPosition(i, p);
    curVel[i] = delta / secDisplIntv;
    status->setVelocity(i, curVel[i]);
    fOutOfRange[i] = dev->isOutOfRange(i, p);
    status->setOutOfRange(i, fOutOfRange[i]);
    /*
    if (fOutOfRange[i] != 0) {
      printDate();
      CONSOLE_PRINT("Encoder # %d is out of range!\n", i);
    }
    */
  }
  MrsvrVector cp;
  getTipPosition(cp);
  for (int i = 0; i < 3; i ++) {
    status->setTipPosition(i, cp[i]);
  }
}


inline void writeLog()
{
  slog->next();
  for (int i = 0; i < NUM_ENCODERS; i ++) {
    slog->addPosition(i, posHist[i][pPosHist]);
    slog->addSetPoint(i, 0.0);
  }
}


int clear()
{
  //delete targetLogPos;
  delete status;
  delete command;
  return 1;
}


int main(int argc, char* argv[]) 
{
  int m;
  FILE* logfp;
  int logging = 0;
  //  static time_t ct = 0;
  struct tm ctm;
  static int prevMode = -1;
  static bool prevLockState[NUM_ACTUATORS];
  static bool prevActiveState[NUM_ACTUATORS];


  cout << endl << SOFTWARE_NAME << endl;
  cout << COPYRIGHT_STR << endl;
  sleep(1);
  cout << endl << "Starting RTCP..." << endl;


  if ((argc == 2 || argc == 3)&& (interval = atoi(argv[1]))) {
    if (interval < 100) {
      cout << "Interval is too small." << endl;
      exit(1);
    }
    secDisplIntv = ((float)(interval*(NUM_POSITION_HISTORY))* INTERVAL_UNIT);
    intervalf = (float)interval / 1000000.0;
    cout << "Loop interval is " << interval << " us." << endl;

    if (argc == 3) {
      logfilename = argv[2];
      logfp = fopen(logfilename, "a");
      if (logfp == NULL) {
        cout << "Connot open log file. " << endl;
        exit(1);
      }
      logmode = 1;
      cout << "Log Mode: ON."  << endl;
      time_t ct = 0;
      time(&ct);
      localtime_r(&ct, &ctm);
      fprintf(logfp, "=======================================\n");
      fprintf(logfp, " RTCP Start at: \n");
      fprintf(logfp, "   %04d/%02d/%02d %02d:%02d:%02d\n", 
              ctm.tm_year+1900, ctm.tm_mon+1, ctm.tm_mday, 
              ctm.tm_hour, ctm.tm_min, ctm.tm_sec);
      fprintf(logfp, " Interval:\n"); 
      fprintf(logfp, "   %d us.\n", (int) interval);
      fprintf(logfp, "=======================================\n");      
      fclose(logfp);
    } else {
      cout << "Log Mode: OFF."  << endl;
      logmode = 0;
    }
  } else {
    cout << "Invalid argument" << endl;
    cout << "Usage: " << argv[0] << " <intv>" << endl;
    cout << "   intv: interval in micro second." << endl;
    exit(1);
  }
  init();
  slog->setInterval(interval);


#ifdef USE_ART
  if (art_enter(ART_PRIO_MAX, ART_TASK_PERIODIC, interval) == -1) {
    perror("art_enter");
    exit(1);
  }
#endif //USE_ART

  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    printDate();
    CONSOLE_PRINT("Activating actuator #%d...",i);
    dev->activateActuator(i);
    status->setActuatorPowerStatus(i, true);
#ifdef _ENABLE_ACTUATOR_LOCK_DETECTION
    prevLockState[i] = dev->isActuatorLocked(i);
#endif //_ENABLE_ACTUATOR_LOCK_DETECTION
    prevActiveState[i] = command->isActuatorActive(i);
    CONSOLE_PRINT("OK.\n");
  }

  // main loooooooooooooooooooooooooooooooooop
  while(1) {

#ifdef USE_ART    
    if (art_wait() == -1) {
      perror("art_wait");
      exit(1);
    }
#else
    sleep(1);   // for test
#endif // if USE_ART else  

    // check shutdown flag
    if (command->getShutdown()) {
      exit(1);
    }

    getPositions();
    m = command->getMode();
    
    if (m != prevMode) {
      fModeChange = true;
    } else {
      fModeChange = false;
    }
    prevMode = m;

    // print mode information
    if (fModeChange) {
      printDate();
      switch(m) {
      case MrsvrStatus::STOP:
        CONSOLE_PRINT("Entering STOP mode...\n");
        break;
      case MrsvrStatus::MANUAL_CALIB:
        CONSOLE_PRINT("Entering MANUAL CALIBRATION mode...\n");
        break;
      case MrsvrStatus::AUTO_CALIB:
        CONSOLE_PRINT("Entering AUTO CALIBRATION mode...\n");
        break;
      case MrsvrStatus::PAUSE:
        CONSOLE_PRINT("Entering PAUSE mode...\n");
        break;
      case MrsvrStatus::MOVE_TO:
        CONSOLE_PRINT("Entering MOVE TO mode...\n");
        break;
      case MrsvrStatus::MANUAL:
        CONSOLE_PRINT("Entering MANUAL mode...\n");
        break;
      case MrsvrStatus::REMOTE:
        CONSOLE_PRINT("Entering REMOTE mode...\n");
        break;
      case MrsvrStatus::RESET:
        CONSOLE_PRINT("Entering RESET mode...\n");
        break;
      default:
        break;
      }
    }

    if (logmode) {
      if (m == MrsvrStatus::REMOTE) {
        if (!logging) {
          if ((logfp = fopen(logfilename, "a")) == NULL) {
            cout << "Connot open log file. " << endl;
            exit(1);
          }
          logging = 1;
          time_t ct = 0;
          time(&ct);
          localtime_r(&ct, &ctm);
          fprintf(logfp, "----%04d/%02d/%02d %02d:%02d:%02d----\n", 
                  ctm.tm_year+1900, ctm.tm_mon+1, ctm.tm_mday, 
                  ctm.tm_hour, ctm.tm_min, ctm.tm_sec);
          dev->setLogTrigHigh();
        }
#if (NUM_ENCODERS==5)
        fprintf(logfp, "%f,  %f, %f, %f, %f\n", 
                curPos[0], curPos[1], curPos[2], curPos[3], curPos[4]);
#endif
      } else if (logging && logfp) {
        dev->setLogTrigLow();
        logging = 0;
        fclose(logfp);
      }
    }

    switch (m) {
    case MrsvrStatus::STOP:
      procStop();
      break;
    case MrsvrStatus::MANUAL_CALIB:
      procManualCalib();
      break;
    case MrsvrStatus::AUTO_CALIB:
      if (fModeChange) {
        procAutoCalib(true);
      } else {
        procAutoCalib(false);
      }
      break;
    case MrsvrStatus::PAUSE:
      procPause();
      break;
    case MrsvrStatus::MOVE_TO:
      procMoveTo();
      break;
    case MrsvrStatus::MANUAL:
      procManual();
      break;
    case MrsvrStatus::REMOTE:
      procRemote();
      break;
    case MrsvrStatus::RESET:
      procReset();
      break;
    default:
      break;
    }

#ifdef _ENABLE_ACTUATOR_LOCK_DETECTION
    dev->detectActuatorLock();
#endif

    for (int i = 0; i < NUM_ACTUATORS; i ++) {
#ifdef _ENABLE_ACTUATOR_LOCK_DETECTION
      if (dev->isActuatorLocked(i) && !prevLockState[i]) {
        printDate();
        CONSOLE_PRINT("Lock detected on actuator #%d\n", i);
        status->setActuatorLockStatus(i, true);
        prevLockState[i] = true;
        dev->deactivateActuator(i); // stop actuator
        status->setActuatorPowerStatus(i, false);
      }
#endif
      // check actuator power switch on user console screen
      // Has the switch changed?
      //CONSOLE_PRINT("Checking actuator switch %d# sw=%d\n", i, command->isActuatorActive(i)?1:0);
      if (command->isActuatorActive(i) != prevActiveState[i]) {
        if (command->isActuatorActive(i)) {
          dev->activateActuator(i);
          status->setActuatorPowerStatus(i, true);
          printDate();
          CONSOLE_PRINT("Actuator #%d activated.\n", i);
          fprintf(stderr, "Actuator #%d activated.\n", i);
#ifdef _ENABLE_ACTUATOR_LOCK_DETECTION
          if (status->getActuatorLockStatus(i)) {
            dev->resetActuatorLock(i);
            status->setActuatorLockStatus(i, false);
            prevLockState[i] = false;
          }
#endif //_ENABLE_ACTUATOR_LOCK_DETECTION
          prevActiveState[i] = true;
        } else {
          dev->deactivateActuator(i);
          status->setActuatorPowerStatus(i, false);
          printDate();
          fprintf(stderr, "Actuator #%d deactivated.\n", i);
          CONSOLE_PRINT("Actuator #%d deactivated.\n", i);
          prevActiveState[i] = false;
        }
      }

    }

    
    // write current data to shared log memory
    writeLog();
    //DUMP_POSITION(targetLogPos);
  }

#ifdef USE_ART
  if (art_exit() == -1) {
    perror("art_exit");
    exit(1);
  }
#endif //USE_ART

  clear();
}

