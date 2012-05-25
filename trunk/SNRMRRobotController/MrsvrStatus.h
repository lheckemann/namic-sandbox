//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrStatus.h,v $
// $Revision: 1.5 $ 
// $Author: junichi $
// $Date: 2006/01/19 07:35:14 $
//====================================================================


//====================================================================
// Description:
//    Class to share controller status informations between GUI and 
//    main controller.
//====================================================================


#ifndef _INC_MRSVR_STATUS
#define _INC_MRSVR_STATUS

#include <string.h>
#include "MrsvrSharedData.h"
#include "MrsvrDev.h"

#ifdef DEBUG
#define DEBUG_MRSVR_STATUS
#endif //DEBUG

#define MAX_INFO_TEXT_SIZE            512
#define NUM_INFO_TEXT_BUFFER          20

#define LEN_UNIT_STR                  10 

#define NUM_WARNING_TYPE              30


// Automatic Calibration Sequence

#define AUTOCALIB_HOME_POSITION   0
#define NUM_PROC_AUTOCALIB        1

#define NUM_CALIB_POINTS          6


typedef struct {
  int       mode; 
  // actuators
  float     voltage[NUM_ACTUATORS];   // [v]

  // actuator status
  bool      power[NUM_ACTUATORS];     // power supplied ?
  bool      lock[NUM_ACTUATORS];      // the actuator is locked ?
  int       lifetime[NUM_ACTUATORS];  // driving time of actuator (unit depends on the type of the actuator)
  int       expiration[NUM_ACTUATORS];// expiration of the actuator

  // encoders
  float     velocity[NUM_ENCODERS];    // [mm/s], [angle/s], etc ... 
  float     position[NUM_ENCODERS];    // [mm], [angle], etc ...
  float     setPoint[NUM_ENCODERS];
  float     limitPosMax[NUM_ENCODERS];
  float     limitPosMin[NUM_ENCODERS];

  float     tipPosition[3];          // tip position in 3D space

  // for progress dialog information
  int       infoTextIdx;
  char      infoText[NUM_INFO_TEXT_BUFFER][MAX_INFO_TEXT_SIZE];
  int       progress;

  int       setAngleReady;
  int       calibReady;
  
  // Warnings
  int       fOutOfRange[NUM_ENCODERS];  // Out of range flag
                  // return  0 if pos is in the range,
                  //        -1 if pos is less than the minimum, 
                  //        +1 if pos is grater than the minimum, 
  int       fReached;        // Reach at the target
  int       fLimiter[NUM_ENCODERS];  // Limiter present flag
  int       fCalibration;    // Calibration status flag; (1: done)

} MrsvrStatusInfo;

#define SHARED_MEMORY_SIZE_STATUS   sizeof(MrsvrStatusInfo)


class MrsvrStatus : public MrsvrSharedData {

 public:
  enum {
    START_UP,
    CALIBRATION,
    HOLD,
    ACTIVE,
    EMERGENCY,
    RESET,
  };

 protected:
  // static informations
  // name of units for each encoder
  static const char* encoderName[];
  static const char* velUnitName[];
  static const char* posUnitName[];

  //// shared data
 protected:
  MrsvrStatusInfo*         statusInfo;

 public:
  MrsvrStatus(key_t);
  ~MrsvrStatus();
  
};


class MrsvrStatusReader : public MrsvrStatus {

 public:
  inline const char* getEncoderName(int i) { return encoderName[i]; } 
  inline const char* getVelUnitName(int i) { return velUnitName[i]; } 
  inline const char* getPosUnitName(int i) { return posUnitName[i]; } 

  inline int   getMode()             { return statusInfo->mode;        };
  inline float getVoltage(int i)     { return statusInfo->voltage[i];  };
  inline float getVelocity(int i)    { return statusInfo->velocity[i]; };
  inline float getPosition(int i)    { return statusInfo->position[i]; };
  inline float getSetPoint(int i)    { return statusInfo->setPoint[i]; };

  inline float getLimitPosMax(int i) { return statusInfo->limitPosMax[i]; };  
  inline float getLimitPosMin(int i) { return statusInfo->limitPosMin[i]; };  

  inline float getTipPosition(int i) { return statusInfo->tipPosition[i]; };

  inline int   getOutOfRange(int i)  { return statusInfo->fOutOfRange[i]; };

  inline int   getReached()          { return statusInfo->fReached; };

  inline int   getLimiter(int i)     { return statusInfo->fLimiter[i]; };

  inline int   getCalibration()      { return statusInfo->fCalibration; };


  inline int   getProgress()         { return statusInfo->progress;    };

  inline bool  getActuatorPowerStatus(int i)
    { return statusInfo->power[i];      };
  inline bool  getActuatorLockStatus(int i)
    { return statusInfo->lock[i];       };
  inline int   getActuatorLifetime(int i)
    { return statusInfo->lifetime[i];   };
  inline int   getActuatorExpiration(int i)
    { return statusInfo->expiration[i]; };
  inline int   getInfoTextIdx()      {
    return (statusInfo->infoTextIdx < NUM_INFO_TEXT_BUFFER && 
            statusInfo->infoTextIdx >= 0) ? statusInfo->infoTextIdx: -1; };

  inline char* getInfoText(int i)    { return statusInfo->infoText[i]; };
  inline char* getInfoTextLatest()
    { return statusInfo->infoText[statusInfo->infoTextIdx];  };

  inline int   getSetAngleReadyIndex() { return statusInfo->setAngleReady; };
  inline int   getCalibReadyIndex()    { return statusInfo->calibReady;    };


  /*  Following code has been discarded on 02/01/2007
  // check current position is out of the range of motion
  // return  0 if pos is in the range,
  //        -1 if pos is less than the minimum, 
  //        +1 if pos is grater than the minimum, 
  inline int isOutOfRangePos(int i) {  
    return ((statusInfo->limitPosMax[i] < statusInfo->position[i])? 
            1: ((statusInfo->limitPosMin[i] > statusInfo->position[i])? 
                -1 : 0));  
  };
  */

 public:
  MrsvrStatusReader(key_t);
  ~MrsvrStatusReader();
};


class MrsvrStatusWriter : public MrsvrStatusReader {

 public:
  inline void  setMode(int v)              { statusInfo->mode =  v;       };
  inline void  setVoltage(int i, float v)  { statusInfo->voltage[i] = v;  };
  inline void  setVelocity(int i, float v) { statusInfo->velocity[i] = v; };
  inline void  setPosition(int i, float v) { statusInfo->position[i] = v; };
  inline void  setSetPoint(int i, float v) { statusInfo->setPoint[i] = v; };
  inline void  setLimitPos(int i, float min, float max) {
    statusInfo->limitPosMax[i] = max; statusInfo->limitPosMin[i] = min;
  };
  inline void  setLimitPos(float* mins, float* maxs) { 
      memcpy((void*)(statusInfo->limitPosMax), (void*)maxs, 
             sizeof(float)*NUM_ENCODERS);
      memcpy((void*)(statusInfo->limitPosMin), (void*)mins, 
             sizeof(float)*NUM_ENCODERS);
  };

  inline void setTipPosition(int i, float v) { statusInfo->tipPosition[i] = v; };

  inline void setOutOfRange(int i, int f) {
    statusInfo->fOutOfRange[i] = f;
  };

  inline void  setReached(int v)          { statusInfo->fReached = v; };

  inline void  setLimiter(int i, int v)   { statusInfo->fLimiter[i] = v; };

  inline void  setCalibration(int v)      { statusInfo->fCalibration = v; };


  void         setInfoText(char* str);
  inline void  setInfoTextIdx(int n)  { statusInfo->infoTextIdx = n; };
  inline void  setProgress(int n)     { statusInfo->progress    = n; };

  inline void  setActuatorPowerStatus(int i, bool pw) { 
    statusInfo->power[i] = pw;
  };
  inline void  setActuatorLockStatus(int i, bool lk)  {
    statusInfo->lock[i] = lk;
  };
  inline void  setActuatorLifetime(int i, int lt)     { 
    statusInfo->lifetime[i] = lt;
  };
  inline void  incActuatorLifetime(int i, int d)      {
    statusInfo->lifetime[i] += d;
  }; 
  inline void  setActuatorExpiration(int i, int exp)  {
    statusInfo->expiration[i] = exp;
  };

  inline void  setSetAngleReadyIndex(int n) { statusInfo->setAngleReady = n; };
  inline void  setCalibReadyIndex(int n) { statusInfo->calibReady = n; };
 public:
  MrsvrStatusWriter(key_t);
  ~MrsvrStatusWriter();
};
  


#endif //_INC_MRSVR_STATUS
