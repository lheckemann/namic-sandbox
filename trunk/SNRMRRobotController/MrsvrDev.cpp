//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrDev.cpp,v $
// $Revision: 1.4 $ 
// $Author: junichi $
// $Date: 2006/01/19 07:35:14 $
//====================================================================


//====================================================================
// Description: 
//    Device I/O controller
//====================================================================


#include <stdio.h>
#include "MrsvrDev.h"

const unsigned short MrsvrDev::dioInStgHomeMsk[] = {
  STAGE_X_HOME,
  STAGE_Y_HOME,
  STAGE_Z_HOME,
};

const unsigned short MrsvrDev::dioInStgLmtMsk[] = {
  STAGE_X_LIMIT,
  STAGE_Y_LIMIT,
  STAGE_Z_LIMIT,
};

const unsigned short MrsvrDev::dioInSwFwMsk[] = {
  SWITCH_X_FW,
  SWITCH_Y_FW,
  SWITCH_Z_FW,
};

const unsigned short MrsvrDev::dioInSwBwMsk[] = {
  SWITCH_X_BW,
  SWITCH_Y_BW,
  SWITCH_Z_BW,
};

const unsigned short MrsvrDev::dioOutFwMsk[] = {
  ACTUATOR_X_FW,
  ACTUATOR_Y_FW,
  ACTUATOR_Z_FW,
};

const unsigned short MrsvrDev::dioOutBwMsk[] = {
  ACTUATOR_X_BW,
  ACTUATOR_Y_BW,
  ACTUATOR_Z_BW,
};

const unsigned short MrsvrDev::daOutVelCh[] = {
  ACTUATOR_X_VEL,
  ACTUATOR_Y_VEL,
  ACTUATOR_Z_VEL,
};

const unsigned short MrsvrDev::encCntMsk[] = {
  ENC_CNT_X,
  ENC_CNT_Y,
  ENC_CNT_Z,
  ENC_CNT_THETA,
  ENC_CNT_PHI,
};

const int MrsvrDev::encCh[] = {
  ENC_CH_X,
  ENC_CH_Y,
  ENC_CH_Z,
  ENC_CH_THETA,
  ENC_CH_PHI,
};

const int MrsvrDev::encMode[] = {
  ENC_MODE_X,
  ENC_MODE_Y,
  ENC_MODE_Z,
  ENC_MODE_THETA,
  ENC_MODE_PHI,
};

const int MrsvrDev::encDir[] = {
  ENC_DIR_X,
  ENC_DIR_Y,
  ENC_DIR_Z,
  ENC_DIR_THETA,
  ENC_DIR_PHI,
};

const float MrsvrDev::encRate[] = {
  ENC_RATE_X,
  ENC_RATE_Y,
  ENC_RATE_Z,
  ENC_RATE_THETA,
  ENC_RATE_PHI,
};
  
/*
*/
//const float MrsvrDev::encLimitMax[] = {
//  MAX_POSITION_X,
//  MAX_POSITION_Y,
//  MAX_POSITION_Z,
//};
//
//const float MrsvrDev::encLimitMin[] = {
//  MIN_POSITION_X,
//  MIN_POSITION_Y,
//  MIN_POSITION_Z,
//};
//
//const int MrsvrDev::encLimitMinCnt[] = {
//  INIT_CNT_LOWER_X,
//  INIT_CNT_LOWER_Y,
//  INIT_CNT_LOWER_Z,
//};
//
//const int MrsvrDev::encLimitMaxCnt[] = {
//  INIT_CNT_UPPER_X,
//  INIT_CNT_UPPER_Y,
//  INIT_CNT_UPPER_Z,
//};


const int MrsvrDev::EnableLockDetect[] = {
  LOCK_DETECT_X,
  LOCK_DETECT_Y,
  //  LOCK_DETECT_Z,
};


//------------------- Constructor & Destructor ---------------------//

MrsvrDev::MrsvrDev() 
//
// Constructor for MrsvrDev module.
// Note that all actuators are disable after this routine.
//
{
  
  initDio(); 
  initDa();
  initEncCount();

  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    deactivateActuator(i);
  }
 
  vmax[0] = MAXRPS_X * PITCH_X;
  vmax[1] = MAXRPS_Y * PITCH_Y;
  vmax[2] = MAXRPS_Z * PITCH_Z;

  vmin[0] = MINRPS_X * PITCH_X;
  vmin[1] = MINRPS_Y * PITCH_Y;
  vmin[2] = MINRPS_Z * PITCH_Z;

  vmaxvol[0] = MAXRPSVOL_X;
  vmaxvol[1] = MAXRPSVOL_Y;
  vmaxvol[2] = MAXRPSVOL_Z;

  vminvol[0] = MINRPSVOL_X;
  vminvol[1] = MINRPSVOL_Y;
  vminvol[2] = MINRPSVOL_Z;

  astd[0] = ASTD_X;
  astd[1] = ASTD_Y;
  astd[2] = ASTD_Z;

  amax[0] = AMAX_X;
  amax[1] = AMAX_Y;
  amax[2] = AMAX_Z;
 
  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    grad[i]  = (vmaxvol[i] - vminvol[i])/(vmax[i] - vmin[i]);
    dmarg[i] = 0.5 * (vmin[i]*vmin[i] / astd[i]);
  }

  encLimitMax[0] = MAX_POSITION_X;
  encLimitMax[1] = MAX_POSITION_Y;
  encLimitMax[2] = MAX_POSITION_Z;

  encLimitMin[0] = MIN_POSITION_X;
  encLimitMin[1] = MIN_POSITION_Y;
  encLimitMin[2] = MIN_POSITION_Z;

  encLimitMinCnt[0] = INIT_CNT_LOWER_X;
  encLimitMinCnt[1] = INIT_CNT_LOWER_Y;
  encLimitMinCnt[2] = INIT_CNT_LOWER_Z;

  encLimitMaxCnt[0] = INIT_CNT_UPPER_X;
  encLimitMaxCnt[1] = INIT_CNT_UPPER_Y;
  encLimitMaxCnt[2] = INIT_CNT_UPPER_Z;

  //setEncLimitMin(0, MIN_POSITION_X);
  //setEncLimitMax(0, MAX_POSITION_X);
  //setEncLimitMin(1, MIN_POSITION_Y);
  //setEncLimitMax(1, MAX_POSITION_Y);
  //setEncLimitMin(2, MIN_POSITION_Z);
  //setEncLimitMax(2, MAX_POSITION_Z);
}


MrsvrDev::~MrsvrDev()
//
// Destructor for MrsvrDev Module.
//
{
  delete daOutValues;
  delete daConfs;

  DioClose(DEV_DIO);
  DaClose(DEV_DA);
  PencClose(DEV_ENC_COUNT);
}


int MrsvrDev::init()
{
  initDio(); 
  initDa();
  initEncCount();
  return 0;
}


// Initialize Digital I/O board (PCI-2725, Interface Corp.)
int MrsvrDev::initDio()
{
  int ret;

  if ((ret = DioOpen(DEV_DIO, 0))) {
#ifdef DEBUG_MRSVR_DEV
    printf("MrsvrDev::initDio(): Error on DioOpen(): Error code = %d \n", ret);
#endif //DEBUG_MRSVR_DEV
  }
  return ret;
}


// Initialize D/A board (PCI-3338, Interface Corp.)
int MrsvrDev::initDa()
{
  int ret;
  int i;

  // D/A 
  if ((ret = DaOpen(DEV_DA))) {
#ifdef DEBUG_MRSVR_DEV
    printf("MrsvrDev::initDa(): Error on DaOpen(): Error code = %d \n", ret);
#endif //DEBUG_MRSVR_DEV
    return ret;
  }    
  if ((ret = DaGetDeviceInfo(DEV_DA, &daSpec))) {
#ifdef DEBUG_MRSVR_DEV
    printf("MrsvrDev::initDa(): Error on DaGetDeviceInfo(): Error code = %d \n", ret);
#endif //DEBUG_MRSVR_DEV
    return ret;
  }

  if (daSpec.ulResolution == 12) {
    daFullRange = 0xfff;
  } else {
    daFullRange = 0xffff;
  }

  // Stop All actuators
  dioOutValue  = 0x0000;
  DioOutputWord(DEV_DIO, FBIDIO_OUT1_16, dioOutValue);

  // Preparation for DaOutputDA()
  daOutValues    = new unsigned short[NUM_ACTUATORS];
  daConfs        = new DASMPLCHREQ[NUM_ACTUATORS];
  for (i = 0; i < NUM_ACTUATORS; i ++) {
    //daOutValues[i]     = 0x0000;
    //(unsigned short) ((float) daFullRange * (0+5.0)/ (MAX_ACTUATOR_VOLTAGE*2));
    daConfs[i].ulChNo  = daOutVelCh[i];
    daConfs[i].ulRange = DA_5V;
    actuatorVol[i]     = 0.0;
  }
  DaOutputDA(DEV_DA, NUM_DA_DEVICES, &daConfs[0], &daOutValues[0]);
  
  return ret;
}   
  

// Initialize encorder counter board (PCI-6205C, Interface Corp.)
int MrsvrDev::initEncCount()
{
  // open encoder counter board
  if (PencOpen(DEV_ENC_COUNT, 0)) {
#ifdef DEBUG_MRSVR_DEV
    printf("MrsvrDev::initEncCount(): Error on PencOpen() \n");
#endif //DEBUG_MRSVR_DEV
    return 0;
  }
  
  for (int i = 0; i < NUM_ENCODERS; i ++) { 
    if (PencSetMode(DEV_ENC_COUNT, encCh[i], encMode[i], encDir[i], 
                    ENC_COMP, ENC_LATCH_MODE)) {
#ifdef DEBUG_MRSVR_DEV
      printf("MrsvrDev::initEncCount(): Error on PencSetMode(%d, %d, %d, %d, %d, %d) \n", 
             DEV_ENC_COUNT, encCh[i], encMode[i], encDir[i], ENC_COMP, ENC_LATCH_MODE);
#endif //DEBUG_MRSVR_DEV
      return 0;
    }

    if (PencSetCounter(DEV_ENC_COUNT, encCh[i], INIT_CNT)) {
#ifdef DEBUG_MRSVR_DEV
      printf("MrsvrDev::initEncCount(): Error on PencSetCounter(%d, %d, %d) \n", 
             DEV_ENC_COUNT, encCh[i], 0);
#endif //DEBUG_MRSVR_DEV
      return 0;
    }
    encZeroCnt[i] = INIT_CNT;
  }
  return 1;
}


int MrsvrDev::setVoltage(int n, float vol)
{
  static float prevVol[3];
  int r;  // return value

#ifdef DEBUG_MRSVR_DEV
  cout << "setVoltage(" << n << "," << vol << ")" << endl;
#endif //DEBUG_MRSVR_DEV
  if (n >= 0 && n < NUM_ACTUATORS) {
    r = 1;
//#ifdef _ENABLE_ACTUATOR_LOCK_DETECTION
//    if (isActuatorLocked(n)) { 
//      // if the actuator is locked, set voltage 0.
//      vol = 0.0;
//      r = -1; // return -1 when the actuator is locked
//    }
//#endif
    if (!isActive(n)) { 
      // if the actuator is not active, set voltage 0.
      vol = 0.0;
      r = -1; // set return value -1
    }

    actuatorVol[n] = vol;
    if (vol > TS_ACTUATOR_VOLTAGE) {
      dioOutValue |= dioOutFwMsk[n];
      dioOutValue &= ~dioOutBwMsk[n];
    } else if (vol > -TS_ACTUATOR_VOLTAGE) {
      dioOutValue &= ~dioOutFwMsk[n];
      dioOutValue &= ~dioOutBwMsk[n];
      vol = 0.0;
    } else {
      dioOutValue &= ~dioOutFwMsk[n];
      dioOutValue |= dioOutBwMsk[n];
      vol = -vol;
    }
    daOutValues[n] = 
      (unsigned short) ((float) daFullRange * (vol+5.0)/ (MAX_ACTUATOR_VOLTAGE*2));
    DaOutputDA(DEV_DA, NUM_DA_DEVICES, &daConfs[0], &daOutValues[0]);
    DioOutputWord(DEV_DIO, FBIDIO_OUT1_16, dioOutValue);
    //printf("daFullRange = %d,  vol = %f, max = %f", daFullRange, vol, MAX_ACTUATOR_VOLTAGE);
    //printf("DaOutputDA(%d) = %d \n", daOutValues[n], ret);
#ifdef _ENABLE_ACTUATOR_LOCK_DETECTION
    if (vol != prevVol[n]) {
      timeCounter[n] = 0;
      startPosition[n] = 0.0;
      prevVol[n] = vol;
    }
#endif
    return r;
  } else {
    return 0;
  }
}


float MrsvrDev::getPosition(int n)
{
  unsigned long cnt;
  float rt;
  if (n >= 0 && n < NUM_ENCODERS) {
    if (PencGetCounter(DEV_ENC_COUNT, encCh[n], &cnt) 
        != PENC_ERROR_SUCCESS) {
#ifdef DEBUG_MRSVR_DEV
      printf("MrsvrDev::getPosition(): Error on PencGetCounter(%d, %d, %d) \n", 
             DEV_ENC_COUNT, encCh[n], &cnt);
#endif //DEBUG_MRSVR_DEV
      return 0.0;
    }
    rt = (float)((double)((long)cnt-(long)encZeroCnt[n]) * encRate[n]);
  } else {
    rt = 0.0;
  }
  return rt;
}


int MrsvrDev::getPositions(float* pos)
{
  unsigned long cnt[8];

  PencGetCounterEx(DEV_ENC_COUNT, ENC_CNT_ALL, cnt);
  for (int i = 0; i < NUM_ENCODERS; i ++) {
    pos[i] = ((float)((long)cnt[encCh[i]-1]-(long)encZeroCnt[i]) * encRate[i]);
#ifdef DEBUG_MRSVR_DEV
    printf("MrsvrDev::getPositions():  encoder = %d\n", cnt[i]);
#endif //DEBUG_MRSVR_DEV
  }

  return 1;
}


int MrsvrDev::setPosition(int n, float v)
{
  unsigned long cnt;
  if (n > 0 && n < NUM_ENCODERS) {
    cnt = (long) (v / encRate[n]) + encZeroCnt[n];
    setCounter(n, cnt);
    return 1;
  } else {
    return 0;
  }
}


int MrsvrDev::setEncLimitMin(unsigned int enc, float val)
{
  printf("MrsvrDev::setEncLimitMin(%d, %f)\n", enc, val);
  if (enc < NUM_ENCODERS) {
    encLimitMin[enc] = val;
    encLimitMinCnt[enc] = (INIT_CNT - (int)(((encLimitMax[enc] - encLimitMin[enc])/2.0)/encRate[enc]));
    //encLimitMinCnt[enc] = INIT_CNT + (int)(val / encRate[enc]);
    printf("  encLimitMin[] = %f, %f \n", encLimitMin[enc], val);
    printf("  encLimitMinCnt[] = %d\n", encLimitMinCnt[enc]);
    return 1;
  }
  return 0;
}


int MrsvrDev::setEncLimitMax(unsigned int enc, float val)
{
  printf("MrsvrDev::setEncLimitMax(%d, %f)\n", enc, val);
  if (enc < NUM_ENCODERS) {
    encLimitMax[enc] = val;
    encLimitMaxCnt[enc] = ((int)(((encLimitMax[enc] - encLimitMin[enc])/2.0)/encRate[enc]) + INIT_CNT);
    //encLimitMaxCnt[enc] = INIT_CNT + (int)(val / encRate[enc]);
    printf("  encLimitMax[] = %f /  %f\n", encLimitMax[enc], val);
    printf("  encLimitMaxCnt[] = %d\n", encLimitMaxCnt[enc]);
    return 1;
  }
  return 0;
}


int MrsvrDev::getLimitSensorStatus(int i)
{
  DioInputWord(DEV_DIO, FBIDIO_IN1_16, &dioInValue);
  if (dioInStgHomeMsk[i]&dioInValue) {
    return -1;
  } else if (dioInStgLmtMsk[i]&dioInValue) {
    return 1;
  } else {
    return 0;
  } 
  return 0;
}


unsigned short MrsvrDev::getSwitchStatus()
{
  DioInputWord(DEV_DIO, FBIDIO_IN1_16, &dioInValue);
  return (dioInValue & SWITCH_MASK_XYZ);
}



