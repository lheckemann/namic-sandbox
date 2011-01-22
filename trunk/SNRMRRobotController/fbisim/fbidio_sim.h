/* ***************************************************************
   fbidio.h 
   ---------------------------------------------------------------
   Version 2.00-02
   ---------------------------------------------------------------
   Date March 12, 2002
   ---------------------------------------------------------------
   Copyright 2001, 2002 Interface Corporation. All rights reserved.
   *************************************************************** */

#ifdef __cplusplus
extern "C" {
#endif

#define FBIDIO_IN1_8       0x00000001
#define FBIDIO_IN9_16      0x00000002
#define FBIDIO_IN17_24     0x00000004
#define FBIDIO_IN25_32     0x00000008
#define FBIDIO_IN33_40     0x00000010
#define FBIDIO_IN41_48     0x00000020
#define FBIDIO_IN49_56     0x00000040
#define FBIDIO_IN57_64     0x00000080
#define FBIDIO_IN1_16      0x00000003
#define FBIDIO_IN17_32     0x0000000c
#define FBIDIO_IN33_48     0x00000030
#define FBIDIO_IN49_64     0x000000c0
#define FBIDIO_IN1_32      0x0000000f
#define FBIDIO_IN33_64     0x000000f0

#define FBIDIO_OUT1_8      0x00000100
#define FBIDIO_OUT9_16     0x00000200
#define FBIDIO_OUT17_24    0x00000400
#define FBIDIO_OUT25_32    0x00000800
#define FBIDIO_OUT33_40    0x00001000
#define FBIDIO_OUT41_48    0x00002000
#define FBIDIO_OUT49_56    0x00004000
#define FBIDIO_OUT57_64    0x00008000
#define FBIDIO_OUT1_16     0x00000300
#define FBIDIO_OUT17_32    0x00000c00
#define FBIDIO_OUT33_48    0x00003000
#define FBIDIO_OUT49_64    0x0000c000
#define FBIDIO_OUT1_32     0x00000f00
#define FBIDIO_OUT33_64    0x0000f000

#define FBIDIO_FLAG_SHARE       1
#define FBIDIO_RSTIN_MASK       1

//--------------------------------------------------------------------------
//
//             Error Code
//
//--------------------------------------------------------------------------
#define FBIDIO_ERROR_SUCCESS                    0x00000000
#define FBIDIO_ERROR_NOT_DEVICE                 0xC0000001
#define FBIDIO_ERROR_NOT_OPEN                   0xC0000002
#define FBIDIO_ERROR_INVALID_DEVICE_NUMBER      0xC0000003
#define FBIDIO_ERROR_ALREADY_OPEN               0xC0000004
#define FBIDIO_ERROR_NOT_SUPPORTED              0xC0000009
#define FBIDIO_ERROR_PARAMETER                  0xC0001021
#define FBIDIO_ERROR_INVALID_CALL               0xC0001002
#define FBIDIO_ERROR_DEVICE_HANDLE              0xFFFFFFFF

//--------------------------------------------------------------------------
//
//       prototype 
//
//--------------------------------------------------------------------------
typedef void(* LPDIOCALLBACK)(unsigned long lUserData, unsigned char bEvent,
                                unsigned short nDeviceNum );
typedef void(* LPDIOCALLBACKEX)(unsigned long lUserData, unsigned char bEvent,
                                unsigned long Eint1, unsigned long Eint2,
                                unsigned short nDeviceNum );

int DioOpen                   (int nDevice, unsigned long reserved);
int DioClose                  (int nDevice);
int DioInputPoint             (int nDevice, int*  pBuffer,
                                            unsigned long   ulStartNum, unsigned long ulNum);
int DioOutputPoint            (int nDevice, int*  pBuffer, 
                                            unsigned long   ulStartNum, unsigned long ulNum);
int DioInputByte              (int nDevice, unsigned long   nNo, unsigned char  * pbValue);
int DioInputWord              (int nDevice, unsigned long   nNo, unsigned short * pnValue);
int DioInputDword             (int nDevice, unsigned long   nNo, unsigned long  * plValue);
int DioOutputByte             (int nDevice, unsigned long   nNo, unsigned char     bValue);
int DioOutputWord             (int nDevice, unsigned long   nNo, unsigned short    nValue);
int DioOutputDword            (int nDevice, unsigned long   nNo, unsigned long     lValue);
int DioGetAckStatus           (int nDevice, unsigned char*  pbAckStatus);
int DioSetAckPulseCommand     (int nDevice, unsigned char    bCommand);
int DioGetStbStatus           (int nDevice, unsigned char*  pbStbStatus);
int DioSetStbPulseCommand     (int nDevice, unsigned char    bCommand);
int DioSetIrqMask             (int nDevice, unsigned char    bIrqMask);
int DioGetIrqMask             (int nDevice, unsigned char*  pbIrqMask);
int DioSetIrqConfig           (int nDevice, unsigned char    bIrqConfig);
int DioGetIrqConfig           (int nDevice, unsigned char*  pbIrqConfig);
int DioGetDeviceConfig        (int nDevice, unsigned long*  plDeviceConfig);
int DioGetDeviceConfigEx      (int nDevice, unsigned long*  plDeviceConfig,
                               unsigned long *plDeviceConfigEx);
int DioSetTimerConfig         (int nDevice, unsigned char    bTimerConfigValue);
int DioGetTimerConfig         (int nDevice, unsigned char*  pbTimerConfigValue);
int DioGetTimerCount          (int nDevice, unsigned char*  pbTimerCount);
int DioSetLatchStatus         (int nDevice, unsigned char    bLatchStatus);
int DioGetLatchStatus         (int nDevice, unsigned char*  pbLatchStatus);
int DioGetResetInStatus       (int nDevice, unsigned char*  pbResetInStatus);
int DioRegistIsr              (int nDevice, unsigned long    lUserData,      
                               LPDIOCALLBACK pCallBackProc );
int DioRegistIsrEx            (int nDevice, unsigned long    lUserData,      LPDIOCALLBACKEX pCallBackProc );
int 
DioCommonGetPciDeviceInfo (int nDevice, 
                        unsigned short* pnDeviceID, unsigned short* pnVendorID,
                        unsigned long*  plClassCode, unsigned char*  pbRevisionID,
                        unsigned long*  plBaseAddress0, unsigned long*  plBaseAddress1,
                        unsigned long*  plBaseAddress2, unsigned long*  plBaseAddress3,
                        unsigned long*  plBaseAddress4, unsigned long*  plBaseAddress5,
                        unsigned short* pnSubsystemID, unsigned short* pnSubsystemVendorID,
                        unsigned char*  pbInterruptLine, unsigned long*  plBoardID);
int DioEintSetIrqMask(int Device, unsigned long IrqMask);
int DioEintGetIrqMask(int Device, unsigned long *IrqMask);
int DioEintSetEdgeConfig(int Device, unsigned long FallEdge,
                         unsigned long RiseEdge);
int DioEintGetEdgeConfig(int Device, unsigned long *FallEdge,
                         unsigned long *RiseEdge);
int DioEintInputPoint(int Device, int *Buffer, 
                      unsigned long StartNum, unsigned long Num);
int DioEintInputByte(int Device, int No, unsigned char *FallValue ,
                     unsigned char *RiseValue);
int DioEintInputWord(int Device, int No, unsigned short *FallValue ,
                     unsigned short *RiseValue);
int DioEintInputDword(int Device, int No, unsigned long *FallValue,
                      unsigned long *RiseValue);
int DioEintSetFilterConfig(int Device, int No, unsigned long FilterConfig);
int DioEintGetFilterConfig(int Device, int No, unsigned long *FilterConfig);
int DioSetRstinMask(int Device, unsigned long ulRstinMask);
int DioGetRstinMask(int Device, unsigned long *ulRstinMask);
#ifdef __cplusplus
}
#endif




