//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrDev.h,v $
// $Revision: 1.6 $ 
// $Author: junichi $
// $Date: 2006/01/19 07:35:14 $

//====================================================================

//====================================================================
// Description: 
//    Device I/O controller
//====================================================================


#ifndef _INC_MRSVR_DEV
#define _INC_MRSVR_DEV


//#include "MrsvrStatus.h"
#include "MrsvrTransform.h"  // for MrsvrVector
#include <math.h>

//#include "iostream.h"
#ifdef _USE_FBISIM
 #include "fbidio_sim.h"
 #include "fbida_sim.h"
 #include "fbipenc_sim.h"
#else
 #include "fbidio.h"
 #include "fbida.h"
 #include "fbipenc.h"
#endif

#include <stdio.h>


#ifndef PI
#define PI 3.1415926535
#endif


#ifdef DEBUG
#define DEBUG_MRSVR_DEV
#endif //DEBUG

// actuator lock detection
#define _ENABLE_ACTUATOR_LOCK_DETECTION

#ifdef _ENABLE_ACTUATOR_LOCK_DETECTION
  #define LOCK_DETECT_COUNT         20
  #define LOCK_DETECT_VOLTAGE       0.1
  #define LOCK_DETECT_TH_POS_COUNT  0.1
#endif


// Device Numbers
#define DEV_DIO          1
#define DEV_DA           2 
#define DEV_ENC_COUNT    3

#define NUM_ACTUATORS        2
#define NUM_ENCODERS         2

// Bit masks for digital Input masks
// Note : Low   (ON)  -> 1 
//      : Hight (OFF) -> 0 
//#define STAGE_X_HOME     0x0001
//#define STAGE_X_LIMIT    0x0002
//#define STAGE_Y_HOME     0x0004
//#define STAGE_Y_LIMIT    0x0008
//#define STAGE_Z_HOME     0x0010
//#define STAGE_Z_LIMIT    0x0020

#define STAGE_X_HOME     0x0001  // IN 2
#define STAGE_X_LIMIT    0x0002
#define STAGE_Y_HOME     0x0004  // IN 3
#define STAGE_Y_LIMIT    0x0008
#define STAGE_Z_HOME     0x0010  // IN 5
#define STAGE_Z_LIMIT    0x0020

// Sensor default status (high/low when activated)
// This configuration depends on the L<->D switch on
// the Omron sennsor.
#define STAGE_X_HOME_DEFAULT   1
#define STAGE_X_LIMIT_DEFAULT  0
#define STAGE_Y_HOME_DEFAULT   0
#define STAGE_Y_LIMIT_DEFAULT  1
#define STAGE_Z_HOME_DEFAULT   1
#define STAGE_Z_LIMIT_DEFAULT  1

//#define SWITCH_X_FW      0x0040
//#define SWITCH_X_BW      0x0080
//#define SWITCH_Y_FW      0x0100
//#define SWITCH_Y_BW      0x0200
//#define SWITCH_Z_FW      0x0400
//#define SWITCH_Z_BW      0x0800

#define SWITCH_X_FW      0x0100
#define SWITCH_X_BW      0x0200
#define SWITCH_Y_FW      0x0400
#define SWITCH_Y_BW      0x0800
#define SWITCH_Z_FW      0x0040
#define SWITCH_Z_BW      0x0080

#define SWITCH_MASK_XYZ  0x0FC0

// Bit masks for digital output masks
// Note : 1 (ON)  -> Low
//        0 (ON)  -> High
#define ACTUATOR_X_FW    0x0002
#define ACTUATOR_X_BW    0x0001
#define ACTUATOR_Y_FW    0x0008
#define ACTUATOR_Y_BW    0x0004
#define ACTUATOR_Z_FW    0x0010
#define ACTUATOR_Z_BW    0x0020

//// Original
//#define ACTUATOR_X_FW    0x0008
//#define ACTUATOR_X_BW    0x0004
//#define ACTUATOR_Y_FW    0x0020
//#define ACTUATOR_Y_BW    0x0010
//#define ACTUATOR_Z_FW    0x0002
//#define ACTUATOR_Z_BW    0x0001

//#define ACTUATOR_X_FW    0x0008
//#define ACTUATOR_X_BW    0x0004
//#define ACTUATOR_Y_FW    0x0002
//#define ACTUATOR_Y_BW    0x0001
//#define ACTUATOR_Z_FW    0x0020
//#define ACTUATOR_Z_BW    0x0010

#define LOG_TRIG         0x0040


// Channel number for D/A output masks
#define NUM_DA_DEVICES   (NUM_ACTUATORS+0)
#define ACTUATOR_X_VEL   1
#define ACTUATOR_Y_VEL   (ACTUATOR_X_VEL+1)
#define ACTUATOR_Z_VEL   (ACTUATOR_Y_VEL+1)

//// Original
//#define ACTUATOR_X_VEL   2
//#define ACTUATOR_Y_VEL   3
//#define ACTUATOR_Z_VEL   1

//#define ACTUATOR_X_VEL   2
//#define ACTUATOR_Y_VEL   1
//#define ACTUATOR_Z_VEL   3

// Encoder counter masks
#define ENC_CNT_X        0x0001
#define ENC_CNT_Y        0x0002
#define ENC_CNT_Z        0x0004 

#define ENC_CNT_THETA    0x0008
#define ENC_CNT_PHI      0x0010

#define ENC_CNT_ALL      (ENC_CNT_X|ENC_CNT_Y|ENC_CNT_Z|ENC_CNT_THETA|ENC_CNT_PHI)

// Encoder channel #
// Note that ENC_CH_* depend on ENC_CNT_*
#define ENC_CH_X         1
#define ENC_CH_Y         2
#define ENC_CH_Z         3

//#define ENC_CH_X         2
//#define ENC_CH_Y         3
//#define ENC_CH_Z         1

#define ENC_CH_THETA     4
#define ENC_CH_PHI       5

// Encoder mode   
// Note that you need to adjust PULSES_REV_* if you change ENC_MODE_*.
#define ENC_MODE_X       0x00000004
#define ENC_MODE_Y       0x00000004
#define ENC_MODE_Z       0x00000004
#define ENC_MODE_THETA   0x00000006
#define ENC_MODE_PHI     0x00000006

// Encoder counter direction
#define ENC_DIR_X        1
#define ENC_DIR_Y        1
#define ENC_DIR_Z        0
#define ENC_DIR_THETA    0
#define ENC_DIR_PHI      0

// Flag to detect lock
#define LOCK_DETECT_X    0
#define LOCK_DETECT_Y    0
#define LOCK_DETECT_Z    0

// actuator property
#define MAXRPS_X         1.75
#define MAXRPS_Y         1.75
#define MAXRPS_Z         1.75

#define MINRPS_X         0.5
#define MINRPS_Y         0.5
#define MINRPS_Z         0.5

#define MAXRPSVOL_X      2.0
#define MAXRPSVOL_Y      2.0
#define MAXRPSVOL_Z      2.0 

#define MINRPSVOL_X      0.75
#define MINRPSVOL_Y      0.75
#define MINRPSVOL_Z      0.75 


#define ASTD_X           (MAXRPS_X * PITCH_X / 1.0)
#define ASTD_Y           (MAXRPS_Y * PITCH_Y / 1.0)
#define ASTD_Z           (MAXRPS_Z * PITCH_Z / 1.0)

#define AMAX_X           (MAXRPS_X * PITCH_X / 0.2)
#define AMAX_Y           (MAXRPS_Y * PITCH_Y / 0.2)
#define AMAX_Z           (MAXRPS_Z * PITCH_Z / 0.2)

//// STAGE PITCH
//#define PITCH_X          20.0    // [mm]
//#define PITCH_Y          20.0    // [mm]
//#define PITCH_Z          20.0    // [mm]

#define PITCH_X          1.99571453  // [mm/rev]
#define PITCH_Y          1.99571453  // [mm/rev]
#define PITCH_Z          1.99571453  // [mm/rev]

// Pulses/revolution of encoder
#define PULSES_REV_X     1000.0
#define PULSES_REV_Y     1000.0
#define PULSES_REV_Z     1000.0
#define PULSES_REV_THETA (1000.0*4)
#define PULSES_REV_PHI   (1000.0*4)

// Encoder : unit / counter
#define ENC_RATE_X       (PITCH_X/PULSES_REV_X)   // [mm]
#define ENC_RATE_Y       (PITCH_Y/PULSES_REV_Y)   // [mm]
#define ENC_RATE_Z       (PITCH_Z/PULSES_REV_Z)   // [mm]
#define ENC_RATE_THETA   (2*PI/PULSES_REV_THETA)  // [rad]
#define ENC_RATE_PHI     (2*PI/PULSES_REV_PHI)    // [rad]

// initial counter value
#define INIT_CNT         0x008FFFFF

#define INIT_CNT_LOWER_X (INIT_CNT - (int)(((MAX_POSITION_X - MIN_POSITION_X)/2.0)/ENC_RATE_X))
#define INIT_CNT_LOWER_Y (INIT_CNT - (int)(((MAX_POSITION_Y - MIN_POSITION_Y)/2.0)/ENC_RATE_Y))
#define INIT_CNT_LOWER_Z (INIT_CNT - (int)(((MAX_POSITION_Z - MIN_POSITION_Z)/2.0)/ENC_RATE_Z))

#define INIT_CNT_UPPER_X ((int)(((MAX_POSITION_X - MIN_POSITION_X)/2.0)/ENC_RATE_X) + INIT_CNT)
#define INIT_CNT_UPPER_Y ((int)(((MAX_POSITION_Y - MIN_POSITION_Y)/2.0)/ENC_RATE_Y) + INIT_CNT)
#define INIT_CNT_UPPER_Z ((int)(((MAX_POSITION_Z - MIN_POSITION_Z)/2.0)/ENC_RATE_Z) + INIT_CNT)

// Use comparator in encoder counter board?
// 0: off      1: on
#define ENC_COMP         0

// Latch mode
// 0: software latch  1: external latch 
#define ENC_LATCH_MODE   0

// voltage threshold to swtich off the actuators
#define TS_ACTUATOR_VOLTAGE      0.01
#define MAX_ACTUATOR_VOLTAGE     5.0
#define ACTUATOR_VOLTAGE_OFFSET  1.0

// Motion ranges
#define MIN_POSITION_X       0.0              // mm
#define MAX_POSITION_X       73.68            // mm
#define MIN_POSITION_Y       0.0              // mm
//#define MAX_POSITION_Y       97.12            // mm
#define MAX_POSITION_Y       97.62            // mm
#define MIN_POSITION_Z       0.0              // mm
#define MAX_POSITION_Z       180.0            // mm
#define MIN_POSITION_THETA   -0.5*PI        // rad
#define MAX_POSITION_THETA   0.5*PI         // rad
#define MIN_POSITION_PHI     -0.5*PI        // rad
#define MAX_POSITION_PHI     0.5*PI         // rad

//#define MIN_SETPOINT       MIN_POSITION   // mm
//#define MAX_SETPOINT       MIN_POSITION   // mm
#define MIN_POSITION         0.0            // mm
#define MAX_POSITION         200.0          // mm
#define MIN_VELOCITY         (-100.0)        // mm/s
#define MAX_VELOCITY         (100.0)         // mm/s
#define MAX_ACTUATOR_VOL_V   5.0             // V

#define ZFRAME_OFFSET_X      (0.0)
#define ZFRAME_OFFSET_Y      (0.0)
#define ZFRAME_OFFSET_Z      (32.5)


class MrsvrDev {

 private:
  // for D/A 
  DABOARDSPEC       daSpec;
  DASMPLCHREQ*      daConfs;
  unsigned short    daFullRange;

  unsigned short    dioInValue;
  unsigned short    dioOutValue;

  unsigned short*   daOutValues;

  // Table to convert from actuator # to I/O masks
  static const unsigned short    dioInStgHomeMsk[];
  static const unsigned short    dioInStgLmtMsk[];
  static const unsigned short    homeSensorDefault[];
  static const unsigned short    limitSensorDefault[];

  static const unsigned short    dioInSwFwMsk[];
  static const unsigned short    dioInSwBwMsk[];
  static const unsigned short    dioOutFwMsk[];
  static const unsigned short    dioOutBwMsk[];

  static const unsigned short    daOutVelCh[];

  static const unsigned short    encCntMsk[]; 
  static const int               encCh[];
  static const int               encMode[];
  static const int               encDir[];
  static const float             encRate[];

  unsigned long                  encZeroCnt[NUM_ENCODERS];

  float                          actuatorVol[NUM_ACTUATORS];
  float                          actVolOff[NUM_ACTUATORS];
  static const int               EnableLockDetect[NUM_ACTUATORS];


  //static const float             encLimitMax[];  
  //static const float             encLimitMin[];  
  float                          encLimitMax[NUM_ENCODERS];  
  float                          encLimitMin[NUM_ENCODERS];  
  //static const int               encLimitMinCnt[];
  //static const int               encLimitMaxCnt[];
  int                            encLimitMinCnt[NUM_ENCODERS];
  int                            encLimitMaxCnt[NUM_ENCODERS];

  //  int                            actuatorKill[NUM_ACTUATORS];
  bool                           actuatorActive[NUM_ACTUATORS];
  
  // vaiables for velocity control
  float                          vmax[NUM_ACTUATORS];    // maximum velocities
  float                          vmin[NUM_ACTUATORS];    // minimum velocities
  float                          vmaxvol[NUM_ACTUATORS]; // voltages on maximum velocities
  float                          vminvol[NUM_ACTUATORS]; // voltages on minimum velocities
  float                          astd[NUM_ACTUATORS];    // standard accelaration
  float                          amax[NUM_ACTUATORS];    // maximum accelaration
  float                          dmarg[NUM_ACTUATORS];   // 
  float                          grad[NUM_ACTUATORS];    // voltage / velocity

#ifdef _ENABLE_ACTUATOR_LOCK_DETECTION
  int                            timeCounter[NUM_ACTUATORS];
  float                          startPosition[NUM_ACTUATORS];
  bool                           lockDetect[NUM_ACTUATORS];
#endif

 private:
  inline float vel2vol(int n, float v) {
    return (v >= 0.0) ? 
      (grad[n]*(v-vmin[n]) + vminvol[n]) :
      (grad[n]*(v+vmin[n]) - vminvol[n]);
  };


  inline float vol2vel(int n, float v) {
    float s = (v > 0.0)? 1.0 : -1.0;
    float av = fabs(v);
    if (av > vmaxvol[n]) return s*vmax[n];
    if (av < vminvol[n]) return 0.0;
    return s * ((av - vminvol[n]) / grad[n] + vmin[n]);

  };

    
 private:
  int initDio();
  int initDa();
  int initEncCount();
  inline int setCounter(int enc, unsigned long cnt) 
    {  return PencSetCounter(DEV_ENC_COUNT, encCh[enc], cnt);  };


 public:
  MrsvrDev();
  ~MrsvrDev();
  int init();

 public:
  int          setVoltage(int, float);
  inline float getVoltage(int n) { return actuatorVol[n]; };

  inline void setZero(int i) { setCounter(i, INIT_CNT); };
  inline void setEncoderLowerLimit(int i) { this->setCounter(i, encLimitMinCnt[i]); };
  inline void setEncoderUpperLimit(int i) { this->setCounter(i, encLimitMaxCnt[i]); };
  inline int getEncoderUpperLimit(int i) { return encLimitMaxCnt[i]; };
  inline int getEncoderLowerLimit(int i) { return encLimitMinCnt[i]; };


  // setVelocity() returns voltage
  inline float setVelocity(int n, float v) {
#ifdef DEBUG_MRSVR_DEV
    cout << "setVelocity(" << n << "," << v << ")" << endl;
#endif //DEBUG_MRSVR_DEV
    float sign = (v > 0.0)? 1.0 : -1.0;
    float ret;
    if (sign*v > vmax[n]) {
      ret = sign * vmaxvol[n];
    } else if (sign*v < vmin[n]) {
      ret =  (sign*v < vmin[n] / 2)? 0.0 : sign * vmin[n];
    } else {
      ret = vel2vol(n, v);
    }
    setVoltage(n, ret);
    return getVoltage(n);
  };

  inline float getSetVelocity(int i) {
    return vol2vel(i, actuatorVol[i]);
  };
  

  float getPosition(int);
  int   getPositions(float*);
  int   setPosition(int, float);
  int   setEncLimitMin(unsigned int enc, float val);
  int   setEncLimitMax(unsigned int enc, float val);

  inline float getEncLimitMin(unsigned int enc) {
    if (enc < NUM_ENCODERS) {
      return encLimitMin[enc];
    } else {
      return 0.0;
    }
  }
  inline float getEncLimitMax(unsigned int enc) {
    if (enc < NUM_ENCODERS) {
      return encLimitMax[enc];
    } else {
      return 0.0;
    }
  }

  // Check if actuators are locked.
  // Call this routine from main control loop.
#ifdef _ENABLE_ACTUATOR_LOCK_DETECTION
  inline void detectActuatorLock() {
    for (int i = 0; i < NUM_ACTUATORS; i ++) {
      if (EnableLockDetect[i]) {
        if (isActive(i) && fabs(getVoltage(i)) > LOCK_DETECT_VOLTAGE) {
          timeCounter[i] ++;
          //printf("timeCounter[%d]  = %d\n", i, timeCounter[i]);
          if (timeCounter[i] % LOCK_DETECT_COUNT == 0) {
            //printf("judging lock \n");
            if (fabs(getPosition(i) - startPosition[i]) < LOCK_DETECT_TH_POS_COUNT) {
              lockDetect[i] = true;
              //printf("lock detected\n");
            }
            timeCounter[i] = 0;
            startPosition[i] = getPosition(i);
          }
        }
      }
    }
  };

  inline bool isActuatorLocked(int i)    { return lockDetect[i]; };
  inline void resetActuatorLock(int i)   { lockDetect[i] = false; };
#endif


  // check optical limit sensor 
  // return  0 if the stage is in the range,
  //        -1 if the stage is on the home (minimum) position,
  //        +1 if the stage is on the limit (maximum) position.
  int   getLimitSensorStatus(int);

  // check the current position is in the range of motion
  // return  0 if pos is in the range,
  //        -1 if pos is less than the minimum, 
  //        +1 if pos is grater than the minimum, 
  inline int isOutOfRange(int i) {  
    float p = getPosition(i); 
    printf("ch = %d; p = %f; min = %f; max = %f\n",
           i, p, encLimitMax[i], encLimitMin[i]);
    return ((encLimitMax[i] < p)? 1: ((encLimitMin[i] > p)? -1 : 0));
  };
  inline int isOutOfRange(int i, float p) {
    return ((encLimitMax[i] < p)? 1: ((encLimitMin[i] > p)? -1 : 0));
  };

  unsigned short getSwitchStatus();
  inline unsigned short isFwSwitchOn(unsigned short status, int a) 
    { return (status&dioInSwFwMsk[a]); };
  inline unsigned short isBwSwitchOn(unsigned short status, int a) 
    { return (status&dioInSwBwMsk[a]); };

  inline float  getLimitMax(int enc) { return encLimitMax[enc]; };
  inline float  getLimitMin(int enc) { return encLimitMin[enc]; };
  inline const float* getLimitMaxs() { return encLimitMax;      };
  inline const float* getLimitMins() { return encLimitMin;      };


  inline float  getVmax(int n) { return vmax[n]; };
  inline float  getVmin(int n) { return vmin[n]; };
  inline float  getAstd(int n) { return astd[n]; };
  inline float  getAmax(int n) { return amax[n]; };
  inline float  getDmarg(int n) {return dmarg[n]; };

  inline void setLogTrigHigh() {dioOutValue &= ~LOG_TRIG; };
  inline void setLogTrigLow()  {dioOutValue |= LOG_TRIG; };
  
  inline void activateActuator(int n)   { actuatorActive[n] = true;  };
  inline void deactivateActuator(int n) { actuatorActive[n] = false; };
  inline bool isActive(int n) { return actuatorActive[n]; };


};


#endif _INC_MRSVR_DEV
