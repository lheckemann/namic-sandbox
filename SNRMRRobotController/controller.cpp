//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2006-2011 by Brigham and Women's Hospital,
// Copyrigth (C) 2005-2010 by Shiga University of Medical Science,
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

//====================================================================
// Mode Transition Diagram
//====================================================================
//
//  1. START_UP:     Boot-up sequence (Hardware / Software)
//  2. CALIBRATION:  Cailbration mode
//  3. HOLD:         Hardware/software ready, no command accepted
//  4. ACTIVE:       Targeting
//  5. EMERGENCY:    Halt system
//  6. RESET:        Restart system
//
//                    +-------> ACTIVE ------+
//                    |           ^          |
//                    |           |          |
//                    |           v          v      
//  START_UP -> CALIBRATION <-> HOLD --> EMERGENCY 
//      ^                         |          |(Safety pause)
//      |                         +-------+  |
//      |                                 |  |
//      |                                 v  v
//      +-------------------------------- RESET
//       (Automatically transition to START_UP)
//
//  Fig. 1: State transition diagram.
//
//
//
// Steps in CALIBRATION
// ---------------------
//
// 05/11/2012 by J.T.: The following description assumes that the sensor becomes
// high when it detects the stage. The sensor status can be flipped by changing
// STAGE_X_HOME_DEFAULT, STAGE_X_LIMIT_DEFAULT, ... in MrsvrDev.h
//
// The CALIBRATION mode consists of two steps in order to ensure the accuracy
// of homing:
//   (0) check the limit sensor. If it is HIGH, the sensor has to be moved away
//       fron the limiter. If the sensor stays HIGH even if it is moved,
//       the homing process should be aborted for user's inspection.
//       (CALIBRATION_MOVE_FROM_LIMITER)
//   (1) move the stage towards the home limiter of the sensor with high speed,
//       until the sensor status becomes HIGH. (CALIBRATION_MOVE_HOME_FAST)
//   (2) move the stage towards the upper limiter of the sensor, until
//       the sensor status becomes LOW (or a few seconds after the sensor status
//       becomes LOW). (CALIBRATION_MOVE_CENTER)
//   (3) move the starge towards the home limiter with low speed (see bellow)
//       until the sensor reading becomes HIGH. (CALIBRATION_MOVE_HOME_SLOW)
//
// Step 2 is necessary, because the HIGH sensor status does not always
// mean that the stage is at the home position (Fig. 2); 
//
// 
//            Stage
//            [__] -->
//                           Linear Stage 
//    "HIGH"     |--------------------------------|
//              Home limiter                     Upper limiter
//                                   
//            
//              Stage
//              [__] -->
//                           Linear Stage 
//    "HIGH"     |--------------------------------|
//              Home limiter                     Upper limiter
//    
//    
//                 Stage
//                 [__] -->
//                           Linear Stage
//    "LOW"      |--------------------------------|
//              Home limiter                     Upper limiter
//    
//    Fig. 2: Actuator position and sensor status. The sensor keeps HIGH
//     status, while the stage is on the homing sensor.
//
// In step 3, the maximum error of calibration depends on the velocity of
// the actuator (V), the timer resolution (R) and travel distance (d(V))
// to stop the stage:
// 
//     E_max = V * R + d(V)
//
// If the required calibration accuracy is 0.1 mm, the timer resolution is
// 10 ms, and d(V) = 0.05 mm (assumed to be constant), V must be
//
//     V = (0.1 - 0.05) / 0.01 = 5 (mm/s)
//



//===========================================================================
// Header files
//===========================================================================

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



//===========================================================================
// Global variables
//===========================================================================

// Shered data
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

int logging;
FILE* logfp;

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


//===========================================================================
// Constants
//===========================================================================
#define CALIBRATION_MOVE_STOP         0
#define CALIBRATION_MOVE_FROM_LIMITER 1
#define CALIBRATION_MOVE_HOME_FAST    2
#define CALIBRATION_MOVE_CENTER       3
#define CALIBRATION_MOVE_HOME_SLOW    4
#define CALIBRATION_COMPLETE          5
#define CALIBRATION_ERROR             6

#define CALIBRATION_VELOCITY_FAST     15
#define CALIBRATION_VELOCITY_SLOW     5


//===========================================================================
// Declarations of functions
//===========================================================================

int  procStartUp(int init);
int  procCalibration(int init);
int  procHold(int init);
int  procActive(int init);
int  procEmergency(int init);
int  procReset(int init);
int  trapCtrl(MrsvrVector, float);
int  trapCtrl2(MrsvrVector, float);
void getActuatorTarget(MrsvrVector& target, MrsvrVector setPoint);
int  updateEncoderCalibration();

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


//===========================================================================
// Initialization / finialization functions
//===========================================================================

int initLogInterface()
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
}


int initHistory()
{
  // initialize position history
  for (int i = 0; i < NUM_ENCODERS; i ++) { 
    for (int j = 0; j < NUM_POSITION_HISTORY; j ++) {
      posHist[i][j] = 0.0;
    }
    curPos[i] = 0.0; 
    fOutOfRange[i] = 0;
  }
  pPosHist = 0;
}


int initCommandInterface()
{
  printDate();
  CONSOLE_PRINT("Initializing hardware interfaces...\n");

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
  status->setMode(MrsvrStatus::START_UP);
  status->setLimitPos((float*)dev->getLimitMins(), (float*)dev->getLimitMaxs());
  status->setReached(0);
  procHold(1);

  return 1;
}


int clear()
{
  //delete targetLogPos;
  delete status;
  delete command;
  return 1;
}


//===========================================================================
// Procedures for each mode
//===========================================================================

inline int procStartUp(int init)
{
  
  //
  // Put start-up procedure here.
  //
  if (init) {
  }
  status->setMode(MrsvrStatus::CALIBRATION);
  status->setCalibration(0);
  return 1;
}


inline int procCalibration(int init)
{

  static int step;
  static int prev_step;
  static int remCycles[NUM_ACTUATORS];
  static int counter;
  static int counterLimit;

  counterLimit = (int) (1.0 / intervalf);

  // limit sensor position (1: lower limit; -1 : upper limit)
  // TODO: move this to device class
  static int dir[]={-1,1,0};

  int f = 0;

  if (init) {
    //step = CALIBRATION_MOVE_HOME_FAST;
    step = CALIBRATION_MOVE_FROM_LIMITER;
    prev_step = -1;
    int ncyclesToStop = 100000 / interval; // 100 ms
    for (int i = 0; i < NUM_ACTUATORS; i ++) {
      remCycles[i] = ncyclesToStop;
    }
  }

  for (int i = 0; i < NUM_ENCODERS; i ++) {
    if (command->getZeroFlag(i)) {
      dev->setZero(i);
      f = 1;
    }
  }
  if (f) return 1;

  switch (command->getCalibrationCommand()) {
  case MrsvrCommand::CALIBRATION_STOP:
    for (int i = 0; i < NUM_ACTUATORS; i ++) {
      dev->setVoltage(i, 0);
      status->setVoltage(i, 0);
      DBG_PRINT("setVoltage(%d, %f) \n", i, dev->getVoltage(i));
    }
    break;

  case MrsvrCommand::CALIBRATION_HOME:
    if (step == CALIBRATION_MOVE_FROM_LIMITER) {
      // Step 0: check the limit sensor. If it is HIGH, the sensor has to be moved away
      //         fron the limiter. If the sensor stays HIGH even if it is moved,
      //         the homing process should be aborted for user's inspection.
      if (prev_step != step) {
         CONSOLE_PRINT("CLIB: CALIBRATION_MOVE_FROM_LIMITER\n");
         prev_step = step;
         counter = 0;
      }
      counter ++;
      int end = 1;
      int err = 0;
      float sv;
      for (int i = 0; i < NUM_ACTUATORS; i ++) {
        if (dev->getLimitSensorStatus(i) == - dir [i]) {
          if (counter < counterLimit) {
            sv = dev->setVelocity(i, (CALIBRATION_VELOCITY_FAST)*dir[i]);
            end = 0;
          } else {
            sv = dev->setVelocity(i, 0);
            err = 1;
          }
        } else {
          sv = dev->setVelocity(i, 0);
        }
        status->setVoltage(i, sv);
      }
      if (err) {
        CONSOLE_PRINT("CLIB ERROR: No limit sensor response.\n");
        step = CALIBRATION_ERROR;
        for (int i = 0; i < NUM_ACTUATORS; i ++) {
          sv = dev->setVelocity(i, 0);
          status->setVoltage(i, sv);
        }
      } else if (end) {
        step = CALIBRATION_MOVE_HOME_FAST;
      }
    } else if (step == CALIBRATION_MOVE_HOME_FAST) {
      // Step 1: move the stage towards the home limiter of the sensor with high speed,
      //         until the sensor status becomes HIGH. (CALIBRATION_MOVE_HOME_FAST)
      if (prev_step != step) {
         CONSOLE_PRINT("CLIB: CALIBRATION_MOVE_HOME_FAST\n");
         prev_step = step;
      }
      int end = 1;
      for (int i = 0; i < NUM_ACTUATORS; i ++) {
        float sv;
        if (dev->getLimitSensorStatus(i) == - dir [i]) {
          sv = dev->setVelocity(i, 0);
        } else {
          end = 0;
          sv = dev->setVelocity(i, (-CALIBRATION_VELOCITY_FAST)*dir[i]);
        }
        status->setVoltage(i, sv);
      }
      if (end) { // move to the next step
        step = CALIBRATION_MOVE_CENTER;
      }
    } else if (step == CALIBRATION_MOVE_CENTER) {
      // Step 2: move the stage towards the upper limiter of the sensor, until
      //         the sensor status becomes LOW (or a few seconds after
      //         the sensor status becomes LOW).  (CALIBRATION_MOVE_CENTER)
      if (prev_step != step) {
         CONSOLE_PRINT("CLIB: CALIBRATION_MOVE_CENTER\n");
         prev_step = step;
      }
      int end = 1;
      for (int i = 0; i < NUM_ACTUATORS; i ++) {
        float sv;
        if (dev->getLimitSensorStatus(i) == 0) {
          if (remCycles[i] > 0) {
            sv = dev->setVelocity(i, CALIBRATION_VELOCITY_FAST*dir[i]);
            end = 0;
            remCycles[i] --;
          } else {
            sv = dev->setVelocity(i, 0);
          }
        } else {
          sv = dev->setVelocity(i, CALIBRATION_VELOCITY_FAST*dir[i]);
        }
        status->setVoltage(i, sv);
      }
      if (end) { // move to the next step
        step = CALIBRATION_MOVE_HOME_SLOW;
      }
    } else if (step == CALIBRATION_MOVE_HOME_SLOW) {
      // Step 3: move the starge towards the home limiter with low speed
      //         until the sensor reading becomes HIGH. (CALIBRATION_MOVE_HOME_SLOW)
      if (prev_step != step) {
         CONSOLE_PRINT("CLIB: CALIBRATION_MOVE_HOME_SLOW\n");
         prev_step = step;
      }
      int end = 1;
      for (int i = 0; i < NUM_ACTUATORS; i ++) {
        float sv;
        if (dev->getLimitSensorStatus(i) == - dir[i]) {
          sv = dev->setVelocity(i, 0);
        } else {
          end = 0;
          sv = dev->setVelocity(i, - CALIBRATION_VELOCITY_SLOW*dir[i]);
        }
        status->setVoltage(i, sv);
      }
      if (end) { // move to the next step
        step = CALIBRATION_COMPLETE;
        CONSOLE_PRINT("CLIB: CALIBRATION_COMPLETE\n");
        status->setCalibration(1);
      }
    } else if (step == CALIBRATION_COMPLETE) {
      for (int i = 0; i < NUM_ACTUATORS; i ++) {
        if (dir[i] > 0) {
          CONSOLE_PRINT("CLIB: dev->setEncoderLowerLimit() \n");
          dev->setEncoderLowerLimit(i);
        } else {
          CONSOLE_PRINT("CLIB: dev->setEncoderUpperLimit() \n");
          dev->setEncoderUpperLimit(i);
        }
      }
      status->setMode(MrsvrStatus::HOLD);
    } else {
      // Error handling.
      CONSOLE_PRINT("CLIB: CALIBRATION ERROR() \n");
    }
    break;

  case MrsvrCommand::CALIBRATION_MANUAL:
    for (int i = 0; i < NUM_ACTUATORS; i ++) {
      dev->setVoltage(i, command->getVoltage(i));
      status->setVoltage(i, dev->getVoltage(i));
      DBG_PRINT("setVoltage(%d, %f) \n", i, dev->getVoltage(i));
    }
    break;
  }

  return 1;
}


inline int procHold(int init)
{
  if (init) {
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


inline int procActive(int init)
{

  unsigned short swst;
  MrsvrVector spim, sprb;

  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    spim[i] = command->getSetPoint(i);
  }

  //getActuatorTarget(sprb, spim);
  if (trapCtrl(spim, dev->getVmax(0)) <= 0) {
    status->setReached(1);
    //status->setMode(MrsvrStatus::MANUAL);
  } else {
    status->setReached(0);
  }

  return 1;

}


inline int procReset(int init)
{
  if (init) {
  }
  
  status->setMode(MrsvrStatus::START_UP);
  return 1;
}


inline int procEmergency(int init)
{

  if (init) {
  }

  // stop all actuators
  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    dev->setVoltage(i, 0.0);
    status->setVoltage(i, dev->getVoltage(i));
    DBG_PRINT("setVoltage(%d, %f) \n", i, dev->getVoltage(i));
  }
  return 1;
}



//===========================================================================
// Actuator control
//===========================================================================

#define TH_REACH_ERROR  0.05

// trapezoidal control
int trapCtrl(MrsvrVector setPoint, float vmax)
{
  float newa, newv;
  int reach = 0;

  if (dev->getSwitchStatus(0) == 0) {
    for (int i = 0; i < NUM_ACTUATORS; i ++) {
      float dist  = setPoint[i] - curPos[i];
      if (fabs(dist) < TH_REACH_ERROR)  {
        reach ++;
      }
      dev->setVelocity(i, 0.0);
      status->setVoltage(i, dev->getVoltage(i));
    }
    return NUM_ACTUATORS - reach;
  }

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

      // If the actuator tries to move beyond the home limit,
      // the controller stops the actuator.
      if (dev->getLimitSensorStatus(i) * dir > 0) {
        float sv = dev->setVelocity(i, 0);
        status->setVoltage(i, sv);
      } else {
        float sv = dev->setVelocity(i, dir*newv);
        status->setVoltage(i, sv);
      }
    }
  }
  return (NUM_ACTUATORS - reach);
}


//===========================================================================
// Sensor control
//===========================================================================

void getActuatorTarget(MrsvrVector& target, MrsvrVector setPoint)
// Get actuator set point from needle tip target
//   setPoint: target position in the robot coordinate system
//   target:   actuator positions
{
  //float a = command->getTipOffset(0);
  //float b = command->getTipOffset(1);
  //float c = command->getTipOffset(2);

  target[0] = setPoint[0];
  target[1] = setPoint[1];
  target[2] = setPoint[2];
}


void getTipPosition(MrsvrVector& tipPoint)
{
  //float a = command->getTipOffset(0);
  //float b = command->getTipOffset(1);
  //float c = command->getTipOffset(2);
  
  tipPoint[0] = curPos[0];
  tipPoint[1] = curPos[1];
  tipPoint[2] = curPos[2];
}


// trapezoidal control
int trapCtrl2(MrsvrVector setPoint, float vmax)
{
  MrsvrVector  asp;
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

      //if (dir*cvs > 0) {  // if actuator is moving towards the target
        if (dist < d1 - dmarg) {  // case 1
          newa = 0.5*cvel*cvel/dist;
          newv = cvs*cvel - newa*intervalf;
        } else if (dist < vmax*vmax/astd - d1 - dmarg) { // case 2
          newv = cvs*cvel + astd*intervalf;
        } else { // case 3
          newv = cvs*cvel + astd*intervalf;
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


// Read encoders and calcurate velocities 
inline void getPositions()
{
  // limit sensor position (1: lower limit; -1 : upper limit)
  // TODO: move this to device class
  static int dir[]={-1,1,0};

  pPosHist = (pPosHist+1) % NUM_POSITION_HISTORY;
  dev->getPositions(curPos);
  for (int i = 0; i < NUM_ENCODERS; i ++) {
    float p = curPos[i];
    float delta = p - posHist[i][pPosHist];
    posHist[i][pPosHist] = p;
    status->setPosition(i, p);
    curVel[i] = delta / secDisplIntv;
    status->setVelocity(i, curVel[i]);
    fOutOfRange[i] = dev->isOutOfRange(i);
    status->setOutOfRange(i, fOutOfRange[i]);
    if (fOutOfRange[i] != 0) {
      printDate();
      CONSOLE_PRINT("Encoder # %d is out of range: p = %f!\n", i, curPos[i]);
    }

    if (dev->getLimitSensorStatus(i) == - dir [i]) {
      // No lmiter found
      status->setLimiter(i, 0);
    } else {
      status->setLimiter(i, 1);
    }
    
  }
  MrsvrVector cp;
  getTipPosition(cp);
  for (int i = 0; i < 3; i ++) {
    status->setTipPosition(i, cp[i]);
  }

  status->setFootSwitch(dev->getSwitchStatus(0));

}


inline int checkModeTransition(int currentMode, int newMode)
{
  if (currentMode == newMode) {
    return 0;
  }

  int fAccept = 0;

  // Please refer "Mode Transition Diagram" at the top of this file
  switch(currentMode) {
  case MrsvrStatus::START_UP:
    break;
  case MrsvrStatus::CALIBRATION:
    if (newMode == MrsvrStatus::HOLD ||
        newMode == MrsvrStatus::ACTIVE) {
      fAccept = 1;
    }
    break;
  case MrsvrStatus::HOLD:
    if (newMode == MrsvrStatus::CALIBRATION || newMode == MrsvrStatus::ACTIVE ) {
      fAccept = 1;
    }
    break;
  case MrsvrStatus::ACTIVE:
    if (newMode == MrsvrStatus::HOLD ||
        newMode == MrsvrStatus::EMERGENCY ||
        newMode == MrsvrStatus::RESET) {
      fAccept = 1;
    }
    break;
  case MrsvrStatus::EMERGENCY:
    if (newMode == MrsvrStatus::RESET) {
      fAccept = 1;
    }
    break;
  case MrsvrStatus::RESET:
    break;
  default:
    break;
  }
  return fAccept;
}


//===========================================================================
// Print / Log
//===========================================================================

inline void writeLog()
{
  slog->next();
  for (int i = 0; i < NUM_ENCODERS; i ++) {
    slog->addPosition(i, posHist[i][pPosHist]);
    slog->addSetPoint(i, 0.0);
  }
}

inline void writeEncoderLog(int newMode)
{
  struct tm ctm;

  if (newMode == MrsvrStatus::ACTIVE) {
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


inline void printModeTransition(int newMode)
{
  printDate();
  switch(newMode) {
  case MrsvrStatus::START_UP:
    CONSOLE_PRINT("Entering START_UP mode...\n");
    break;
  case MrsvrStatus::CALIBRATION:
    CONSOLE_PRINT("Entering CALIBRATION mode...\n");
    break;
  case MrsvrStatus::HOLD:
    CONSOLE_PRINT("Entering HOLD mode...\n");
    break;
  case MrsvrStatus::ACTIVE:
    CONSOLE_PRINT("Entering ACTIVE mode...\n");
    break;
  case MrsvrStatus::EMERGENCY:
    CONSOLE_PRINT("Entering EMERGENCY mode...\n");
    break;
  case MrsvrStatus::RESET:
    CONSOLE_PRINT("Entering RESET mode...\n");
    break;
  default:
    break;
  }
}


int updateEncoderCalibration()
{
  for (int i = 0; i < NUM_ENCODERS; i ++) {      
    if ((command->getEncLimitMin(i) != dev->getEncLimitMin(i)) ||
        (command->getEncLimitMax(i) != dev->getEncLimitMax(i)))
      {
      CONSOLE_PRINT("Changing encoder #%d limit ...\n");
      dev->setEncLimit(i, command->getEncLimitMin(i), command->getEncLimitMax(i));
    }
  }
}


//===========================================================================
// Main
//===========================================================================

int main(int argc, char* argv[]) 
{
  //  static time_t ct = 0;
  struct tm ctm;
  static bool prevLockState[NUM_ACTUATORS];
  static bool prevActiveState[NUM_ACTUATORS];

  logging = 0;

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

  initLogInterface();
  initCommandInterface();
  initHistory();

  CONSOLE_PRINT("Entering START_UP mode...\n");
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

  //--------------------------------------------------
  // main loop
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

    int initMode = 0; 
    int newMode = command->getNewMode(); // returns -1, if the new node is not requested
    int currentMode = status->getMode();
    if (newMode > 0) {
      if (checkModeTransition(currentMode, newMode) == 1) {
        initMode = 1;
        status->setMode(newMode);
        currentMode = newMode;
        printModeTransition(currentMode);
      }
    }
    
    if (logmode) {
      writeEncoderLog(currentMode);
    }

    //CONSOLE_PRINT("ENC: %d %d %d...\n", dev->getLimitSensorStatus(0), dev->getLimitSensorStatus(1),dev->getLimitSensorStatus(2));
    //CONSOLE_PRINT("Switch: %d ...\n", dev->getSwitchStatus(0));

    switch (status->getMode()) {
      case MrsvrStatus::START_UP:
        procStartUp(initMode);
        break;
      case MrsvrStatus::CALIBRATION:
        procCalibration(initMode);
        break;
      case MrsvrStatus::HOLD:
        procHold(initMode);
        break;
      case MrsvrStatus::ACTIVE:
        procActive(initMode);
        break;
      case MrsvrStatus::EMERGENCY:
        procEmergency(initMode);
        break;
      case MrsvrStatus::RESET:
        procReset(initMode);
        break;
      default:
        break;
    }

#ifdef _ENABLE_ACTUATOR_LOCK_DETECTION
    dev->detectActuatorLock();
#endif

    //updateEncoderCalibration();
    
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

