//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrStatus.cpp,v $
// $Revision: 1.1.1.1 $ 
// $Author: junichi $
// $Date: 2005/01/11 11:38:33 $
//====================================================================


//====================================================================
// Description:
//
//====================================================================


#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "MrsvrStatus.h"

const char* MrsvrStatus::encoderName[] = {
  "X stage",
  "Y stage",
  "Z stage",
  "Theta",
  "Phi",
};
const char* MrsvrStatus::velUnitName[] = {
  "mm/s",                 // x stage
  "mm/s",                 // y stage 
  "mm/s",                 // z stage
  "deg/s",                // theta
  "deg/s",                // phi
};
const char* MrsvrStatus::posUnitName[] = {
  "mm",                   // x stage
  "mm",                   // y stage 
  "mm",                   // z stage
  "deg",                  // theta
  "deg",                  // phi
};


MrsvrStatus::MrsvrStatus(key_t shmkey)  :
  MrsvrSharedData(shmkey, SHARED_MEMORY_SIZE_STATUS)
{
  statusInfo = (MrsvrStatusInfo*)sharedMemory;
}



MrsvrStatus::~MrsvrStatus()
{
}



MrsvrStatusReader::MrsvrStatusReader(key_t shmkey) :
  MrsvrStatus(shmkey)
{
}


MrsvrStatusReader::~MrsvrStatusReader()
{
  
}



MrsvrStatusWriter::MrsvrStatusWriter(key_t shmkey) :
  MrsvrStatusReader(shmkey)
{
  int i;

  for (i = 0; i < NUM_ACTUATORS; i ++) {
    statusInfo->voltage[i] = 0.0;
  }
  for (i = 0; i < NUM_ENCODERS; i ++) {
    statusInfo->velocity[i] = 0.0;
    statusInfo->position[i] = 0.0;
    statusInfo->setPoint[i] = 0.0; 
    statusInfo->limitPosMax[i] = 0.0;
    statusInfo->limitPosMin[i] = 0.0;
    statusInfo->fOutOfRange[i] = 0;
  }
  statusInfo->infoTextIdx = 0;
  statusInfo->progress    = 0;
  for (i = 0; i < NUM_INFO_TEXT_BUFFER; i++) {
    statusInfo->infoText[i][0] = '\0';
  }
  for (i = 0; i < NUM_ACTUATORS; i ++) {
    setActuatorPowerStatus(i, true);
    setActuatorLockStatus(i, false);

    // Followings were temporally implemented
    setActuatorLifetime(i, 0);
    setActuatorExpiration(i, 1000);
  }

}


MrsvrStatusWriter::~MrsvrStatusWriter()
{
}


void MrsvrStatusWriter::setInfoText(char* str) 
{
  int idx;
  idx = (statusInfo->infoTextIdx+1)%NUM_INFO_TEXT_BUFFER;
  strncpy(statusInfo->infoText[idx], str, MAX_INFO_TEXT_SIZE); 
  statusInfo->infoText[idx][MAX_INFO_TEXT_SIZE-1] = '\0';
  statusInfo->infoTextIdx = idx;
}
