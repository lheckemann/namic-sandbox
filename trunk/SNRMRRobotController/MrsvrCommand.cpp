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
// $RCSfile: MrsvrCommand.cpp,v $
// $Revision: 1.3 $ 
// $Author: junichi $
// $Date: 2005/09/01 05:33:33 $
//====================================================================


//====================================================================
// Description:
// 
//====================================================================


#include "MrsvrCommand.h"

MrsvrCommand::MrsvrCommand(key_t shmkey)  :
  MrsvrSharedData(shmkey, SHARED_MEMORY_SIZE_STATUS)
{
  cmdInfo = (MrsvrCommandInfo*)sharedMemory;
}

MrsvrCommand::~MrsvrCommand()
{
}


MrsvrCommandReader::MrsvrCommandReader(key_t shmkey) :
  MrsvrCommand(shmkey)
{
  cmdInfo->shutdown = 0;
  prevModeRequestID = 0;
  prevCalibrationCommandID = 0;
}


MrsvrCommandReader::~MrsvrCommandReader()
{
}


MrsvrCommandWriter::MrsvrCommandWriter(key_t shmkey) :
  MrsvrCommandReader(shmkey) 
{
  int i;

  cmdInfo->mode      = MrsvrStatus::HOLD;
  cmdInfo->commandBy = VOLTAGE;
  for (i = 0; i < NUM_ACTUATORS; i ++) {
    cmdInfo->actuatorActive[i] = false;
    cmdInfo->voltage[i] = 0.0;
    cmdInfo->lmtVoltage[i] = 0.0;
  }
  for (i = 0; i < NUM_ENCODERS; i ++) {
    cmdInfo->velocity[i] = 0.0;
    cmdInfo->position[i] = 0.0;
    cmdInfo->setPoint[i] = 0.0; 
    cmdInfo->setZeroFlag[i] = 0;
    cmdInfo->encLimitMin[i] = 0.0;
    cmdInfo->encLimitMax[i] = 0.0;
  }
  for (i = 0; i < 3; i ++) {
    cmdInfo->tipApprOrient[i] = 0.0;
  }
  cmdInfo->tipApprOffset = 0.0;

  cmdInfo->modeRequestID = 0;
  cmdInfo->calibrationCommandID = 0;
}





MrsvrCommandWriter::~MrsvrCommandWriter()
{
}
