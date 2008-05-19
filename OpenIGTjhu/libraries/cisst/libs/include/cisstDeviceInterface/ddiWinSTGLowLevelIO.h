/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiWinSTGLowLevelIO.h,v 1.3 2005/09/26 15:41:46 anton Exp $
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


/*!
  \file
  \brief Defines the low level interface for WinSTG
  \ingroup cisstDeviceInterface
*/

#ifndef _ddiWinSTGLowLevelIO_h
#define _ddiWinSTGLowLevelIO_h

#include <cisstConfig.h>
#if CISST_HAS_WINSTG || defined DOXYGEN


#define STG_SUCCESS  0
#define STG_FAILURE  1

#define NUM_AXIS_IN_SYS         8
#define MAX_AXIS                8

#define ERROR_STRING_SIZE       100
#define MAX_CMD_PARAMS          1000


//
// if you're compiling the VxD, some definitions from ntddk.h
// aren't here.  So, I copied them from ntddk.h
//

#ifndef CTL_CODE

//
// Macro definition for defining IOCTL and FSCTL function control codes.  Note
// that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers.
//

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
  ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
  )

//
// Define the method codes for how buffers are passed for I/O and FS controls
//

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

//
// Define the access check value for any access
//
//
// The FILE_READ_ACCESS and FILE_WRITE_ACCESS constants are also defined in
// ntioapi.h as FILE_READ_DATA and FILE_WRITE_DATA. The values for these
// constants *MUST* always be in sync.
//


#define FILE_ANY_ACCESS                 0
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe

#endif // ifndef CTL_CODE

/*
* define a device IO control codes
*/

#define FILE_DEVICE_STG   42651UL

#define IOCTL_STG_BUFFERED       CTL_CODE(FILE_DEVICE_STG, 0xd00, \
  METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_STG_DIRECT         CTL_CODE(FILE_DEVICE_STG, 0xd01, \
  METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

//
// Here's a way to calculate the amount of data sent
//

#define BigAsNeeded(n) ( sizeof(StgBaseStruct) + (n) * sizeof(long) )

#define STG_PORT_A        0x01
#define STG_PORT_B        0x02
#define STG_PORT_C_LO     0x04
#define STG_PORT_C_HI     0x08
#define STG_PORT_C        (STG_PORT_C_LO | STG_PORT_C_HI)
#define STG_PORT_D_LO     0x10
#define STG_PORT_D_HI     0x20
#define STG_PORT_D        (STG_PORT_D_LO | STG_PORT_D_HI)
#define STG_PORT_INPUT    0
#define STG_PORT_OUTPUT   1

/* Error codes */
#define SERVO_ERROR_NULL_POINTER              9001
#define SERVO_ERROR_PARAM_OUT_OF_RANGE        9002
#define SERVO_ERROR_CMD_PARAM_OUT_OF_RANGE    9003

/* constants for interpolation mode */
#define FIRST_INTERPOLATION_METHOD 0
#define NO_INTERPOLATE             0
#define P_INTERPOLATE              0
#define PV_INTERPOLATE             1
#define PVA_INTERPOLATE            2
#define FIND_INDEX                 3
#define FIND_HOME_SWITCH           4
#define GET_FROM_BUFFER            5
// Leave room for more "standard methods, start at 10. for A/D based  GP
#define  GET_VELO_COMMAND_FROM_AD  10
#define  GET_POSN_COMMAND_FROM_AD  11
#define  LAST_INTERPOLATION_METHOD 11 

/* Command codes */
#define SET_DIO_OUT_ALL            110     //    32   set all bits, or don't care
#define SET_RAW_DIO_OUT            111     //    1    32 bits set each output
#define SET_DIO_OUT                111     //    1    32 bits set each output
#define SET_RAW_DIO_DIR            112     //    1    5 bits to set A,B,C,DH, DL ports to in or out                    
#define SET_DIO_DIR                112     //    1    5 bits to set A,B,C,DH, DL ports to in or out                    
#define SET_RAW_DIO_POL            113     //    1    32 bits sets DI+O polarity (normal or inverted)
#define SET_DIO_POL                113     //    1    32 bits sets DI+O polarity (normal or inverted)
#define SET_RAW_DAC_ONE            114     //    1    -max..max, for a single dac channel
#define SET_DAC_ONE                114     //    1    -max..max, for a single dac channel
#define SET_RAW_DAC_ALL            115     //    8    -max..max, for all dac channels
#define SET_DAC_ALL                115     //    8    -max..max, for all dac channels

#define SET_P_GAIN_ONE             130     //    1    0..max, for a single axis
#define SET_I_GAIN_ONE             131     //    1    0..max, for a single axis
#define SET_D_GAIN_ONE             132     //    1    0..max, for a single axis
#define SET_I_LIMIT_ONE            133     //    1    0..max, for a single axis
#define SET_O_SCALE_ONE            134     //    1    0..max, for a single axis
#define SET_GAINS_ONE              135     //    5    0..max, set all gains for one axis, p,i,d,il,og
#define SET_CONTROL_VECTOR         137

#define SET_INTERPOLATE_ALL        140
#define SET_INTERPOLATE            140     //    1    0=pos (none), 1=pos+vel, 2=pos+vel+accel
#define SET_INTERPOLATE_ONE        146
#define SET_SERVO_MODE_ONE         141     //    1    0=don't servo, 1=servo, for one axis
#define CLOSE_LOOP_ONE             141
#define SET_SERVO_MODE_ALL         142     //    8    0=don't servo, 1=servo, for all axis
#define CLOSE_LOOP_ALL             142
#define STG_OPEN_LOOP_MODE           0     //    constant that can be used with SET_SERVO_MODE_...
#define STG_CLOSED_LOOP_MODE         1
#define SET_UPDATE_RATE            143     //    1    .5,1,2,3,4,5,10,100,1000 sample period in uS
#define SET_MOTION_MODE            145     //    1    0=abs pos (default), 1=pos rel, 2=vel

#define SET_DEMAND_POS_ONE         150     //    1    -max..max, for one axis in encoder counts
#define SET_DEMAND_POS_ALL         151     //    8    -max..max, for all axis in encoder counts
#define SET_DEMAND_VEL_ONE         152     //    1    -max..max, for one axis in counts/tick
#define SET_DEMAND_VEL_ALL         153     //    8    -max..max, for all axis in counts/tick
#define SET_DEMAND_ACC_ONE         154     //    1    -max..max, for one axis in counts/tick/tick
#define SET_DEMAND_ACC_ALL         155     //    8    -max..max, for all axis in counts/tick/tick
//#define SET_STEP_POS_ONE          156     //    1    reset history buffer

#define SET_STOP_METHOD_ONE        160     //    1    0=immediate, 1=with decel, both keep servoing
#define SET_STOP_METHOD_ALL        161     //    8    0=immediate, 1=with decel, both keep servoing
#define SET_START_STOP_ONE         162     //    1    0=stop motion, 1=start motion
#define LOAD_COEFFICIENTS_ONE      162
#define SET_START_STOP_ALL         163     //    8    0=stop motion, 1=start motion
#define LOAD_COEFFICIENTS_ALL      163
#define POLL_MOVE_DONE_ONE         165
#define POLL_MOVE_DONE_ALL         166
#define WAIT_TILL_MOVE_DONE_ONE    167
#define WAIT_TILL_MOVE_DONE_ALL    168

#define ZERO_ENC_ONE               170        
#define ZERO_ENC_ALL               171
#define LOCATE_HOME_SWITCH         172
#define LOCATE_INDEX_PULSE         173
#define INDEX_READ_IRR             174

#define SET_ENCODER_POINTER        180
#define SET_ENCODER_ONE            181
#define SET_ENCODER_ALL            182

#define SET_TEST_MODE              185
#define STG_OUTP                   186
#define STG_OUTPW                  187

#define BUFFER_PRELOAD             190
#define BUFFER_LOAD_AND_GO         191
#define BUFFER_WAIT_TILL_DONE      192

#define SET_AND_GET_BOUNDRY        200     //    Place marker between "set" and "get" command sets

#define GET_RAW_ADC_ONE            210     //
#define GET_ADC_ONE                210     //
#define GET_RAW_ADC_ALL            211     //    8    -max..+max
#define GET_ADC_ALL                211     //    8    -max..+max
#define GET_ADC_ALL_BLOCK          225
#define GET_RAW_DIO_IN             212     //    1 32 bits containing digitial input from all ports
#define GET_DIO_IN                 212
#define GET_ENCODER_ONE            213
#define GET_ENCODER_ALL            214     //    8    -max..+max
#define GET_ERROR_ONE              215
#define GET_ERROR_ALL              216     //    8    -max..+max
#define GET_OUT_VOLTAGE_ONE        217
#define GET_OUT_VOLTAGE_ALL        218     //    8    -max..+max
#define GET_NUM_AXIS_IN_SYS        219
#define GET_TIME_STAMP             220     // *1    0..max
#define GET_VELOCITY_ONE           221     // *1    -max..max
#define GET_VELOCITY_ALL           222     // *8    -max..max
#define GET_SERVO_MODE_ONE         223     // *1    1 means that axis loop is closed...
#define GET_SERVO_MODE_ALL         224     // *8    0 means that axis loop is open

#define GET_BOARD_PRESENCE         230     // 1 a 1 means board is present, 0 means not present
#define GET_GAIN_ALL               231     // 5  Order: p,i,d,il,og    For a single axis. 
#define GET_UPDATE_RATE            232     // 1 returns the sample period in uS
#define GET_INDEX_FLAGS            233     // returns all index pulse flags 
// 0=~done,~found, 1=done,~found,  2=done,found
#define GET_INDEX_COUNTS_ONE       234
#define GET_INDEX_COUNTS_ALL       235
#define GET_HOME_SWITCH_COUNTS_ONE 236
#define GET_HOME_SWITCH_COUNTS_ALL 237
#define GET_CONTROLLER_DATA        238

#define GET_HISTORY_ONE            240     // Size: lesser of OUT_BUFFER_SIZE and MAX_CMD_PARAMS
// Order:least recent to most recent
// if param[0] = 0, then returns encoders
// if param[0] = 1, then returns V Out
// if param[0] = 2, then returns error

#define GET_ALL_INFO_ONE           250     // 8    p,i,d,il,og, enc, err, Vout, demand, Vel for one axis
#define GET_ALL_INFO_ALL           251     // 56    p,i,d,il,og, enc, err, Vout, demand, Vel    for all axis 0 to N
#define GET_EVENT                  260     // 9, encoders and DI

#define STG_INP                    286
#define STG_INPW                   287

//
// function prototypes
//

#define STGTYPEMOD

#include <windows.h>
#include <winioctl.h>
#include <assert.h>

class ddiWinSTGLowLevelIO {
  /*!
  the following structure is used to pass data to and obtain data from
  the VxD via the DeviceIOControl API funtion.  This strucure and the dll
  that uses it does not need to be recompiled when additional commands are
  added to the system.
  */


  typedef struct 
  {                                            // INPUT info   (copied to output info on return)
    unsigned short  nCommand;                // the command code
    unsigned short  nAxis;                   // the axis for which the cmd applies to
    // OUTPUT info
    long    lErrorCode;                      // these are filled in every time IOCTL is called.
    char    acErrorString[ERROR_STRING_SIZE];// if errorCode_l!=0 then msg appears here
    long    lTimeStamp;                      // the sample period this info was valid in
    long    lDataCount;                      // Number of data items actually written to array

    long    alParams[MAX_CMD_PARAMS];        // these are used for both INPUT and OUTPUT
    long    alBuffer[MAX_CMD_PARAMS];        // these are used for both INPUT and OUTPUT
  } StgBuffer, IOCTLVxdData, *IOCTLVxdDataPtr;                    

  /*! Here's a struct that's the same as the one above, but without the big arrays
  of parameters.  We'll use it when we want to send the minimum amount of data.
  */
  typedef struct 
  {                                            // INPUT info   (copied to output info on return)
    unsigned short  nCommand;                // the command code
    unsigned short  nAxis;                   // the axis for which the cmd applies to
    // OUTPUT info
    long    lErrorCode;                      // these are filled in every time IOCTL is called.
    char    acErrorString[ERROR_STRING_SIZE];// if errorCode_l!=0 then msg appears here
    long    lTimeStamp;                      // the sample period this info was valid in
    long    lDataCount;                      // Number of data items actually written to array
  } StgBaseStruct;                             //lint !e756  we don't referance this anywhere

  IOCTLVxdData IoCtlBuf;
  HANDLE hDriver;
  void   SendStg(IOCTLVxdData *pIoctlData);

 public:
  typedef enum {axis0, axis1, axis2, axis3, axis4, axis5, axis6, axis7, axisEnd} AxisType;
  typedef enum {pgain, igain, dgain, ilimit, ogain, last_pid_val} PidParams;

  //
  // digital I/O port stuff
  //
  // begin to use the enumerated type, rather than the defines
  //
  typedef enum { A_dir = 0x01, B_dir = 0x02, C_lo_dir = 0x04,
    C_hi_dir = 0x08, C_dir = 0x0c, D_lo_dir = 0x10,
    D_hiDir = 0x20, D_dir = 0x30 }   eDir_typ;
  typedef enum { A_port, B_port, C_port, D_port } ePort_typ;
  typedef enum { HomeSwitchFORWARD, HomeSwitchREVERSE } eHomeDir_typ;

  ddiWinSTGLowLevelIO();
  ~ddiWinSTGLowLevelIO();
  void   STGTYPEMOD SetDioDir(eDir_typ ePort, long lDir);
  void   STGTYPEMOD SetDioOut(ePort_typ ePort, long lBitIndex, long lHighOrLow);
  void   STGTYPEMOD SetDioOutAll(long bitvec[]);
  long   STGTYPEMOD GetDioIn2(ePort_typ ePort, long lBitOffset);
  void   STGTYPEMOD GetDioIn(void);
  long   STGTYPEMOD DioIn(int nBit);
  void   STGTYPEMOD SetInterpolateAll(long lMode);
  void   STGTYPEMOD SetInterpolateOne(AxisType eAxis, long lMode);
  void   STGTYPEMOD SetInterpPlus2(AxisType eAxis, long lMode, long lParam1, long lParam2);
  void   STGTYPEMOD SetDAC(unsigned short wPort, long lCounts);
  void   STGTYPEMOD SetGainAll(AxisType eAxis, long lPgain, long lIgain, long lDgain,
    long lIlimit, long lOgain);
  void   STGTYPEMOD SetDemandPosOne(AxisType eAxis, long Position);
  void   STGTYPEMOD SetDemandPosAll(long Position[]);
  void   STGTYPEMOD SetDemandVelOne(AxisType eAxis, long lVelocity);
  void   STGTYPEMOD SetDemandVelAll(long lVelocity[]);
  void   STGTYPEMOD SetDemandAccOne(AxisType eAxis, long lAcceleration);
  void   STGTYPEMOD SetDemandAccAll(long lAcceleration[]);
  void   STGTYPEMOD BeginClosedLoopControl(AxisType axis);
  void   STGTYPEMOD EndClosedLoopControl(AxisType axis);
  void   STGTYPEMOD SetServoModeOne(AxisType eAxis, long lMode);
  void   STGTYPEMOD SetServoModeAll(long lMode);
  void   STGTYPEMOD LoadParameters(AxisType axis);
  void   STGTYPEMOD StopAndHold(AxisType axis);
  void   STGTYPEMOD SetStartStopOne(AxisType eAxis, long lSet);
  void   STGTYPEMOD SetStartStopAll(long lSet);
  void   STGTYPEMOD SetTestMode(unsigned short nMode);
  void   STGTYPEMOD SetEncoderOne(AxisType eAxis, long lCounts);
  void   STGTYPEMOD ZeroEncoderOne(AxisType eAxis);
  void   STGTYPEMOD ZeroEncodersAll(void);
  long   STGTYPEMOD PollMoveDoneOne(AxisType eAxis);
  void   STGTYPEMOD PollMoveDoneAll(void);
  long   STGTYPEMOD GetPollResult(AxisType eAxis);
  void   STGTYPEMOD WaitTillMoveDoneOne(AxisType eAxis);
  void   STGTYPEMOD WaitTillMoveDoneAll(long lA1, long lA2, long lA3, long lA4,
    long lA5, long lA6, long lA7, long lA8);
  long   STGTYPEMOD GetEncoderOne(AxisType eAxis);
  void   STGTYPEMOD GetEncoderAll(long *AppBuffer);
  void   STGTYPEMOD GetVelocityAll(long *Buffer);
  void   STGTYPEMOD GetErrorAll(long *Buffer);
  void   STGTYPEMOD GetAdcAll(void);
  void   STGTYPEMOD GetAdcAllBlock(void);
  long   STGTYPEMOD AdcVal(AxisType eAxis);
  long   STGTYPEMOD AdcFiltered(AxisType eAxis);
  void   STGTYPEMOD LocateHomeSwitch(AxisType eAxis, ePort_typ ePort, long lBit, eHomeDir_typ eHomeDir);
  void   STGTYPEMOD LocateIndexPulse(AxisType eAxis);
  unsigned char  STGTYPEMOD StgIrr(void);
  int    STGTYPEMOD OpenStgDriver(void);
  void   STGTYPEMOD CloseStgDriver(void);
  void   STGTYPEMOD ShutDown(void);
  void   STGTYPEMOD GetAllInfoOne(AxisType eAxis);
  long   STGTYPEMOD StgInfoPgain(void);
  long   STGTYPEMOD StgInfoIgain(void);
  long   STGTYPEMOD StgInfoDgain(void);
  long   STGTYPEMOD StgInfoIlimit(void);
  long   STGTYPEMOD StgInfoOgain(void);
  long   STGTYPEMOD StgInfoPosition(void);
  long   STGTYPEMOD StgInfoError(void);
  long   STGTYPEMOD StgInfoOutCounts(void);
  long   STGTYPEMOD StgInfoSetPoint(void);
  long   STGTYPEMOD StgInfoVelocity(void);
  long   STGTYPEMOD StgInfoTimeStamp(void);
  /*! No buffer functions for now
  int    STGTYPEMOD BufferPreLoad(FuncGen* f);
  int    STGTYPEMOD BufferLoadAndGo(FuncGen* f);
  void   STGTYPEMOD BufferWaitTillDone(void);
  */
  long   STGTYPEMOD GetHomeSwitchCountsOne(AxisType eAxis);
  void   STGTYPEMOD GetHomeSwitchCountsAll(void);
  long   STGTYPEMOD HomeSwitchCounts(AxisType eAxis);
  void   STGTYPEMOD Test1(void);

  unsigned char STGTYPEMOD Stg_inp2(unsigned short nPortAddress, long& lErrorCode);
  unsigned char STGTYPEMOD Stg_inp(unsigned short nPortAddress);
  unsigned short STGTYPEMOD Stg_inpw2(unsigned short nPortAddress, long& lErrorCode);
  unsigned short STGTYPEMOD Stg_inpw(unsigned short nPortAddress);
  long   STGTYPEMOD Stg_outp(unsigned short nPortAddress, unsigned char bData);
  long   STGTYPEMOD Stg_outpw(unsigned short nPortAddress, unsigned short nData);
  void   STGTYPEMOD SetUpdateRate(long rate_usec);
};

#endif // CISST_HAS_WINSTG

#endif // _ddiWinSTGLowLevelIO_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiWinSTGLowLevelIO.h,v $
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2004/05/27 19:34:22  anton
// Added || defined DOXYGEN to all #ifdef CISST_HAS_XYZ so that doxygen can
// create the documentation.
//
// Revision 1.1  2004/05/27 17:09:25  anton
// Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
// The build process will have to be redone whenever will will add dynamic
// loading.
//
//
// Revision 1.1  2004/04/08 06:16:20  kapoor
// Added MEI and WinSTG DI
//
// ****************************************************************************
