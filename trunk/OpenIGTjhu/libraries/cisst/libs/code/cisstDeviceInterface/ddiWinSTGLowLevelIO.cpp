/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiWinSTGLowLevelIO.cpp,v 1.2 2005/09/26 15:41:46 anton Exp $
//
//  Author(s):  Ankur Kapoor
//  Created on: 2004-04-30
//
//
//              Developed by the Engineering Research Center for
//          Computer-Integrated Surgical Systems & Technology (CISST)
//
//               Copyright(c) 2004, The Johns Hopkins University
//                          All Rights Reserved.
//
//  Experimental Software - Not certified for clinical use.
//  For use only by CISST sponsored research projects.  For use outside of
//  CISST, please contact Dr. Russell Taylor, CISST Director, at rht@cs.jhu.edu
//  
// ****************************************************************************


#include <cisstDeviceInterface/ddiWinSTGLowLevelIO.h>
#if CISST_HAS_WINSTG


ddiWinSTGLowLevelIO::ddiWinSTGLowLevelIO()//:IoCtlBuf(0),hDriver(INVALID_HANDLE_VALUE)
{
}


ddiWinSTGLowLevelIO::~ddiWinSTGLowLevelIO() {
  CloseStgDriver();
}


int STGTYPEMOD ddiWinSTGLowLevelIO::OpenStgDriver(void) {
  hDriver = CreateFile("\\\\.\\StgDrvr",
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL
                         );

  if (hDriver == INVALID_HANDLE_VALUE)
    return STG_FAILURE;
  return STG_SUCCESS;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::CloseStgDriver(void) {
  if (hDriver != INVALID_HANDLE_VALUE)
    CloseHandle(hDriver);          //lint !e534
  hDriver = INVALID_HANDLE_VALUE;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::ShutDown(void) {
  SetTestMode(0);
  SetServoModeAll(0);
  // should probably set I/O to input.
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetTestMode(unsigned short nMode) {
  IoCtlBuf.nCommand = SET_TEST_MODE;
  IoCtlBuf.nAxis = nMode;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetDioDir(eDir_typ ePort, long lDir) {
  IoCtlBuf.nCommand = SET_RAW_DIO_DIR;
  IoCtlBuf.nAxis = static_cast<unsigned short>(ePort);
  IoCtlBuf.alParams[0] = lDir;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetDioOut(ePort_typ ePort, long lBitIndex, long lHighOrLow) {
  IoCtlBuf.nCommand = SET_DIO_OUT;
  IoCtlBuf.nAxis = static_cast<unsigned short>(ePort);
  IoCtlBuf.alParams[0] = lBitIndex;
  IoCtlBuf.alParams[1] = lHighOrLow;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetDioOutAll(long bitvec[]) {
  IoCtlBuf.nCommand = SET_DIO_OUT_ALL;
  IoCtlBuf.nAxis = 0;
  for (int i = 0; i < 32; i++)
    IoCtlBuf.alParams[i] = bitvec[i];
  SendStg(&IoCtlBuf);
  return;
}


long STGTYPEMOD ddiWinSTGLowLevelIO::GetDioIn2(ePort_typ ePort, long lBitOffset) {
  GetDioIn();
    
  // both assert for development, and check in release
  assert ( (lBitOffset >= 0) && (lBitOffset < 8) );
    
  if ( (lBitOffset >= 0) && (lBitOffset < 8) )
    return IoCtlBuf.alParams[ static_cast<int>(ePort) * 8 + lBitOffset ];
    
  return 0;  // something out of range
}


void STGTYPEMOD ddiWinSTGLowLevelIO::GetDioIn(void) {
  IoCtlBuf.nAxis = 0;  // just to have a valid number
  IoCtlBuf.nCommand = GET_DIO_IN;
  SendStg(&IoCtlBuf);
  return;
}


//
// After Calling GetDioIn(), use this to get the data
//
long STGTYPEMOD ddiWinSTGLowLevelIO::DioIn(int nBit) {
  return IoCtlBuf.alParams[nBit];
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetDAC(unsigned short wPort, long lCounts) {
  IoCtlBuf.nCommand = SET_DAC_ONE;
  IoCtlBuf.nAxis = wPort;
  IoCtlBuf.alParams[0] = lCounts;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetInterpolateAll(long lMode) {
  IoCtlBuf.nCommand = SET_INTERPOLATE_ALL;
  IoCtlBuf.lDataCount = 0;
  IoCtlBuf.alParams[0] = lMode;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetInterpolateOne(AxisType eAxis, long lMode) {
  IoCtlBuf.nCommand = SET_INTERPOLATE_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  IoCtlBuf.lDataCount = 0;
  IoCtlBuf.alParams[0] = lMode;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetInterpPlus2(AxisType eAxis, long lMode, long lParam1, long lParam2) {
  IoCtlBuf.nCommand = SET_INTERPOLATE_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  IoCtlBuf.lDataCount = 2;
  IoCtlBuf.alParams[0] = lMode;
  IoCtlBuf.alParams[1] = lParam1;
  IoCtlBuf.alParams[2] = lParam2;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetGainAll(AxisType eAxis, long lPgain, long lIgain,
                        long lDgain, long lIlimit, long lOgain) {
  IoCtlBuf.nCommand = SET_GAINS_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  IoCtlBuf.alParams[0] = lPgain;
  IoCtlBuf.alParams[1] = lIgain;
  IoCtlBuf.alParams[2] = lDgain;
  IoCtlBuf.alParams[3] = lIlimit;
  IoCtlBuf.alParams[4] = lOgain;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetDemandPosOne(AxisType eAxis, long Position) {
  IoCtlBuf.nCommand = SET_DEMAND_POS_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  IoCtlBuf.alParams[0] = Position;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetDemandPosAll(long Position[]) {
  IoCtlBuf.nCommand = SET_DEMAND_POS_ALL;
  for (unsigned int i = 0; i < MAX_AXIS; i++) {
    IoCtlBuf.alParams[i] = Position[i];
  }
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetDemandVelOne(AxisType eAxis, long lVelocity) {
  IoCtlBuf.nCommand = SET_DEMAND_VEL_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  IoCtlBuf.alParams[0] = lVelocity;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetDemandVelAll(long lVelocity[]) {
  IoCtlBuf.nCommand = SET_DEMAND_VEL_ALL;
  for (unsigned int i = 0; i < MAX_AXIS; i++) {
    IoCtlBuf.alParams[i] = lVelocity[i];
  }
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetDemandAccOne(AxisType eAxis, long lAcceleration) {
  IoCtlBuf.nCommand = SET_DEMAND_ACC_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  IoCtlBuf.alParams[0] = lAcceleration;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetDemandAccAll(long lAcceleration[]) {
  IoCtlBuf.nCommand = SET_DEMAND_ACC_ALL;
  for (unsigned int i = 0; i < MAX_AXIS; i++) {
    IoCtlBuf.alParams[i] = lAcceleration[i];
  }
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::BeginClosedLoopControl(AxisType axis) {
  SetServoModeOne(axis, 1);
}


void STGTYPEMOD ddiWinSTGLowLevelIO::EndClosedLoopControl(AxisType axis) {
  SetServoModeOne(axis, 0);
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetServoModeOne(AxisType eAxis, long lMode) {
  IoCtlBuf.nCommand = SET_SERVO_MODE_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  IoCtlBuf.alParams[0] = lMode;
  SendStg(&IoCtlBuf);
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetServoModeAll(long lMode) {
  IoCtlBuf.nCommand = SET_SERVO_MODE_ALL;
  for (int i = 0; i < 8; i++)
    IoCtlBuf.alParams[i] = lMode;
  SendStg(&IoCtlBuf);
}


void STGTYPEMOD ddiWinSTGLowLevelIO::LoadParameters(AxisType axis) {
  SetStartStopOne(axis, 1);
}


void STGTYPEMOD ddiWinSTGLowLevelIO::StopAndHold(AxisType axis) {
  SetStartStopOne(axis, 0);
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetStartStopOne(AxisType eAxis, long lSet) {
  IoCtlBuf.nCommand = SET_START_STOP_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  IoCtlBuf.alParams[0] = lSet;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetStartStopAll(long lSet) {
  IoCtlBuf.nCommand = SET_START_STOP_ALL;
  IoCtlBuf.alParams[0] = lSet;
  IoCtlBuf.alParams[1] = lSet;
  IoCtlBuf.alParams[2] = lSet;
  IoCtlBuf.alParams[3] = lSet;
  IoCtlBuf.alParams[4] = lSet;
  IoCtlBuf.alParams[5] = lSet;
  IoCtlBuf.alParams[6] = lSet;
  IoCtlBuf.alParams[7] = lSet;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetEncoderOne(AxisType eAxis, long lCounts) {
  IoCtlBuf.nCommand = SET_ENCODER_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  IoCtlBuf.alParams[0] = lCounts;
  SendStg(&IoCtlBuf);
}


void STGTYPEMOD ddiWinSTGLowLevelIO::ZeroEncoderOne(AxisType eAxis) {
  IoCtlBuf.nCommand = ZERO_ENC_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  SendStg(&IoCtlBuf);
}


void STGTYPEMOD ddiWinSTGLowLevelIO::ZeroEncodersAll() {
  IoCtlBuf.nCommand = ZERO_ENC_ALL;
  SendStg(&IoCtlBuf);
}


long STGTYPEMOD ddiWinSTGLowLevelIO::PollMoveDoneOne(AxisType eAxis) {
  IoCtlBuf.nCommand = POLL_MOVE_DONE_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  SendStg(&IoCtlBuf);
  return IoCtlBuf.alParams[0];
}


void STGTYPEMOD ddiWinSTGLowLevelIO::PollMoveDoneAll(void) {
  IoCtlBuf.nCommand = POLL_MOVE_DONE_ALL;
  SendStg(&IoCtlBuf);
  return;
}


long STGTYPEMOD ddiWinSTGLowLevelIO::GetPollResult(AxisType eAxis) {
  return IoCtlBuf.alParams[eAxis];
}

void STGTYPEMOD ddiWinSTGLowLevelIO::WaitTillMoveDoneOne(AxisType eAxis) {
  IoCtlBuf.nCommand = WAIT_TILL_MOVE_DONE_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::WaitTillMoveDoneAll(long lA1, long lA2, long lA3, long lA4,
                             long lA5, long lA6, long lA7, long lA8) {
  IoCtlBuf.nCommand = WAIT_TILL_MOVE_DONE_ALL;
  IoCtlBuf.alParams[0] = lA1;
  IoCtlBuf.alParams[1] = lA2;
  IoCtlBuf.alParams[2] = lA3;
  IoCtlBuf.alParams[3] = lA4;
  IoCtlBuf.alParams[4] = lA5;
  IoCtlBuf.alParams[5] = lA6;
  IoCtlBuf.alParams[6] = lA7;
  IoCtlBuf.alParams[7] = lA8;
  SendStg(&IoCtlBuf);
  return;
}


long STGTYPEMOD ddiWinSTGLowLevelIO::GetEncoderOne(AxisType eAxis) {
  IoCtlBuf.nCommand = GET_ENCODER_ALL;
  SendStg(&IoCtlBuf);
  return IoCtlBuf.alParams[eAxis];
}


void STGTYPEMOD ddiWinSTGLowLevelIO::GetEncoderAll(long *AppBuffer) {
  int i;
    
  IoCtlBuf.nCommand = GET_ENCODER_ALL;
  SendStg(&IoCtlBuf);
  for (i = 0; i < 8; i++)
    AppBuffer[i] = IoCtlBuf.alParams[i];
  return;
}

void STGTYPEMOD ddiWinSTGLowLevelIO::GetVelocityAll(long *buffer) {
  int i;
    
  IoCtlBuf.nCommand = GET_VELOCITY_ALL;
  SendStg(&IoCtlBuf);
  for (i = 0; i < 8; i++)
    buffer[i] = IoCtlBuf.alParams[i];
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::GetErrorAll(long *buffer) {
  int i;
    
  IoCtlBuf.nCommand = GET_ERROR_ALL;
  SendStg(&IoCtlBuf);
  for (i = 0; i < 8; i++)
    buffer[i] = IoCtlBuf.alParams[i];
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::GetAdcAll(void) {
  IoCtlBuf.nCommand = GET_ADC_ALL;
  SendStg(&IoCtlBuf);
  return;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::GetAdcAllBlock(void) {
  IoCtlBuf.nCommand = GET_ADC_ALL_BLOCK;
  SendStg(&IoCtlBuf);
  return;
}

long STGTYPEMOD ddiWinSTGLowLevelIO::AdcVal(AxisType eAxis) {
  return IoCtlBuf.alParams[eAxis];
}


long STGTYPEMOD ddiWinSTGLowLevelIO::AdcFiltered(AxisType eAxis) {
  return IoCtlBuf.alParams[eAxis + 8];
}


void STGTYPEMOD ddiWinSTGLowLevelIO::LocateHomeSwitch(AxisType eAxis, ePort_typ ePort,
                                                      long lBit, eHomeDir_typ eHomeDir) {
  IoCtlBuf.nCommand = LOCATE_HOME_SWITCH;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  IoCtlBuf.alParams[0] = static_cast<long>(ePort);
  IoCtlBuf.alParams[1] = lBit;
  IoCtlBuf.alParams[2] = static_cast<long>(eHomeDir);
  SendStg(&IoCtlBuf);
  SetStartStopOne(eAxis, 1);
}


void STGTYPEMOD ddiWinSTGLowLevelIO::LocateIndexPulse(AxisType eAxis) {
  IoCtlBuf.nCommand = LOCATE_INDEX_PULSE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  SendStg(&IoCtlBuf);
  return;
}


UCHAR STGTYPEMOD ddiWinSTGLowLevelIO::StgIrr(void) {
  IoCtlBuf.nCommand = INDEX_READ_IRR;
  SendStg(&IoCtlBuf);
  return static_cast<UCHAR>(IoCtlBuf.alParams[0]);
}


void STGTYPEMOD ddiWinSTGLowLevelIO::GetAllInfoOne(AxisType eAxis) {
  IoCtlBuf.nCommand = GET_ALL_INFO_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  SendStg(&IoCtlBuf);
  return;
}

// after calling GetAllInfoOne, use these next functions
// to get the data.

long STGTYPEMOD ddiWinSTGLowLevelIO::StgInfoPgain()     { return IoCtlBuf.alParams[0]; }

long STGTYPEMOD ddiWinSTGLowLevelIO::StgInfoIgain()     { return IoCtlBuf.alParams[1]; }

long STGTYPEMOD ddiWinSTGLowLevelIO::StgInfoDgain()     { return IoCtlBuf.alParams[2]; }

long STGTYPEMOD ddiWinSTGLowLevelIO::StgInfoIlimit()    { return IoCtlBuf.alParams[3]; }

long STGTYPEMOD ddiWinSTGLowLevelIO::StgInfoOgain()     { return IoCtlBuf.alParams[4]; }

long STGTYPEMOD ddiWinSTGLowLevelIO::StgInfoPosition()  { return IoCtlBuf.alParams[5]; }

long STGTYPEMOD ddiWinSTGLowLevelIO::StgInfoError()     { return IoCtlBuf.alParams[6]; }

long STGTYPEMOD ddiWinSTGLowLevelIO::StgInfoOutCounts() { return IoCtlBuf.alParams[7]; }

long STGTYPEMOD ddiWinSTGLowLevelIO::StgInfoSetPoint()  { return IoCtlBuf.alParams[8]; }

long STGTYPEMOD ddiWinSTGLowLevelIO::StgInfoVelocity()  { return IoCtlBuf.alParams[9]; }

long STGTYPEMOD ddiWinSTGLowLevelIO::StgInfoTimeStamp() { return IoCtlBuf.lTimeStamp;  }

/*! No buffer functions for now
int STGTYPEMOD ddiWinSTGLowLevelIO::BufferPreLoad(FuncGen* f)
{
int nDone;
long *pX, *pY, *pZ, *pDO;

IoCtlBuf.nCommand = BUFFER_PRELOAD;
IoCtlBuf.lTimeStamp = f->DOEnable();
pX = &IoCtlBuf.alParams[0];
pY = &IoCtlBuf.alParams[500];
pZ = &IoCtlBuf.alBuffer[0];
pDO = &IoCtlBuf.alBuffer[500];
for (int i = 0; i < 500; i++)
nDone = f->Evaluate(*pX++, *pY++, *pZ++, *pDO++);
SendStg(&IoCtlBuf);
IoCtlBuf.nAxis = 0;   // reset this.  The next command might
// not check it. The driver bails out
// when it sees a strange number here.
return nDone;
}

int STGTYPEMOD ddiWinSTGLowLevelIO::BufferLoadAndGo(FuncGen* f)
{
int nDone;
long *pX, *pY, *pZ, *pDO;

IoCtlBuf.nCommand = BUFFER_LOAD_AND_GO;
IoCtlBuf.lTimeStamp = f->DOEnable();
pX = &IoCtlBuf.alParams[0];
pY = &IoCtlBuf.alParams[500];
pZ = &IoCtlBuf.alBuffer[0];
pDO = &IoCtlBuf.alBuffer[500];
for (int i = 0; i < 500; i++)
nDone = f->Evaluate(*pX++, *pY++, *pZ++, *pDO++);
SendStg(&IoCtlBuf);
IoCtlBuf.nAxis = 0;
return nDone;
}
*/

long STGTYPEMOD ddiWinSTGLowLevelIO::GetHomeSwitchCountsOne(AxisType eAxis) {
  IoCtlBuf.nCommand = GET_HOME_SWITCH_COUNTS_ONE;
  IoCtlBuf.nAxis = (unsigned short)eAxis;
  SendStg(&IoCtlBuf);
  return IoCtlBuf.alParams[0];
}


void STGTYPEMOD ddiWinSTGLowLevelIO::GetHomeSwitchCountsAll(void) {
  IoCtlBuf.nCommand = GET_HOME_SWITCH_COUNTS_ALL;
  SendStg(&IoCtlBuf);
}


// After calling the above command, call the following one
// to get results for each axis.
long STGTYPEMOD ddiWinSTGLowLevelIO::HomeSwitchCounts(AxisType eAxis) {
  assert (IoCtlBuf.nCommand == GET_HOME_SWITCH_COUNTS_ALL);
  return IoCtlBuf.alParams[eAxis];
}

/*! No buffer functions for now
void STGTYPEMOD ddiWinSTGLowLevelIO::BufferWaitTillDone(void)
{
IoCtlBuf.nCommand = BUFFER_WAIT_TILL_DONE;
SendStg(&IoCtlBuf);
return;
}
*/

unsigned char STGTYPEMOD ddiWinSTGLowLevelIO::Stg_inp(unsigned short nPortAddress) {
  long lDummy;
  return Stg_inp2(nPortAddress, lDummy);
}


unsigned char STGTYPEMOD ddiWinSTGLowLevelIO::Stg_inp2(unsigned short nPortAddress, long& lErrorCode) {
  IoCtlBuf.nCommand = STG_INP;
  IoCtlBuf.alParams[0] = nPortAddress;
  SendStg(&IoCtlBuf);
  lErrorCode = IoCtlBuf.lErrorCode;
  return static_cast<unsigned char>(IoCtlBuf.alParams[0]);
}


unsigned short STGTYPEMOD ddiWinSTGLowLevelIO::Stg_inpw(unsigned short nPortAddress) {
  long lDummy;
  return Stg_inpw2(nPortAddress, lDummy);
}


unsigned short STGTYPEMOD ddiWinSTGLowLevelIO::Stg_inpw2(unsigned short nPortAddress, long& lErrorCode) {
  IoCtlBuf.nCommand = STG_INPW;
  IoCtlBuf.alParams[0] = nPortAddress;
  SendStg(&IoCtlBuf);
  lErrorCode = IoCtlBuf.lErrorCode;
  return static_cast<unsigned short>(IoCtlBuf.alParams[0]);
}


long STGTYPEMOD ddiWinSTGLowLevelIO::Stg_outp(unsigned short nPortAddress,
                        unsigned char bData) {
  IoCtlBuf.nCommand = STG_OUTP;
  IoCtlBuf.alParams[0] = nPortAddress;
  IoCtlBuf.alParams[1] = bData;
  SendStg(&IoCtlBuf);
  return IoCtlBuf.lErrorCode;
}


long STGTYPEMOD ddiWinSTGLowLevelIO::Stg_outpw(unsigned short nPortAddress,
                         unsigned short nData) {
  IoCtlBuf.nCommand = STG_OUTPW;
  IoCtlBuf.alParams[0] = nPortAddress;
  IoCtlBuf.alParams[1] = nData;
  SendStg(&IoCtlBuf);
  return IoCtlBuf.lErrorCode;
}


void STGTYPEMOD ddiWinSTGLowLevelIO::SetUpdateRate(long rate_usec) {
  IoCtlBuf.nCommand = SET_UPDATE_RATE;
  IoCtlBuf.alParams[0] = rate_usec;
  SendStg(&IoCtlBuf);
}

void ddiWinSTGLowLevelIO::SendStg(IOCTLVxdData *pIoctlData) {
  unsigned long ReturnedLength;
  BOOL r;
    
  r = DeviceIoControl(hDriver,
                        IOCTL_STG_DIRECT,
    pIoctlData,
    sizeof(IOCTLVxdData),
    pIoctlData,
    sizeof(IOCTLVxdData),
    &ReturnedLength,
    0
    );
  assert (r);
  return;
};


#endif // CISST_HAS_WINSTG


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiWinSTGLowLevelIO.cpp,v $
// Revision 1.2  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.1  2004/05/27 17:09:25  anton
// Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
// The build process will have to be redone whenever will will add dynamic
// loading.
//
//
// Revision 1.1  2004/04/08 06:11:39  kapoor
// Added MEI and WinSTG DI
//
// ****************************************************************************
