#include "fbidio_sim.h"

int DioOpen (int nDevice, unsigned long reserved)
{
  return 0;
}


int DioClose (int nDevice)
{
  return 0;
}


int DioInputPoint (int nDevice, int*  pBuffer,
                   unsigned long   ulStartNum, unsigned long ulNum)
{
  return 0;
}


int DioOutputPoint (int nDevice, int*  pBuffer, 
                    unsigned long   ulStartNum, unsigned long ulNum)
{
  return 0;
}


int DioInputByte (int nDevice, unsigned long nNo,
                  unsigned char  * pbValue)
{
  return 0;
}


int DioInputWord (int nDevice, unsigned long nNo,
                  unsigned short * pnValue)
{
  return 0;
}


int DioInputDword (int nDevice, unsigned long nNo,
                   unsigned long  * plValue)
{
  return 0;
}


int DioOutputByte (int nDevice, unsigned long nNo,
                   unsigned char     bValue)
{
  return 0;
}


int DioOutputWord (int nDevice, unsigned long nNo,
                   unsigned short    nValue)
{
  return 0;
}


int DioOutputDword (int nDevice, unsigned long nNo, unsigned long lValue)
{
  return 0;
}


int DioGetAckStatus (int nDevice, unsigned char* pbAckStatus)
{
  return 0;
}


int DioSetAckPulseCommand (int nDevice, unsigned char bCommand)
{
  return 0;
}


int DioGetStbStatus (int nDevice, unsigned char* pbStbStatus)
{
  return 0;
}


int DioSetStbPulseCommand (int nDevice, unsigned char bCommand)
{
  return 0;
}


int DioSetIrqMask (int nDevice, unsigned char    bIrqMask)
{
  return 0;
}


int DioGetIrqMask (int nDevice, unsigned char*  pbIrqMask)
{
  return 0;
}


int DioSetIrqConfig (int nDevice, unsigned char    bIrqConfig)
{
  return 0;
}


int DioGetIrqConfig (int nDevice, unsigned char*  pbIrqConfig)
{
  return 0;
}


int DioGetDeviceConfig (int nDevice, unsigned long*  plDeviceConfig)
{
  return 0;
}

int DioGetDeviceConfigEx (int nDevice, unsigned long*  plDeviceConfig,
                          unsigned long *plDeviceConfigEx)
{
  return 0;
}


int DioSetTimerConfig (int nDevice, unsigned char bTimerConfigValue)
{
  return 0;
}


int DioGetTimerConfig (int nDevice, unsigned char*  pbTimerConfigValue)
{
  return 0;
}

int DioGetTimerCount (int nDevice, unsigned char*  pbTimerCount)
{
  return 0;
}

int DioSetLatchStatus (int nDevice, unsigned char bLatchStatus)
{
  return 0;
}

int DioGetLatchStatus (int nDevice, unsigned char*  pbLatchStatus)
{
  return 0;
}

int DioGetResetInStatus  (int nDevice, unsigned char*  pbResetInStatus)
{
  return 0;
}

int DioRegistIsr (int nDevice, unsigned long lUserData,      
                  LPDIOCALLBACK pCallBackProc )
{
  return 0;
}

int DioRegistIsrEx (int nDevice, unsigned long lUserData, 
                    LPDIOCALLBACKEX pCallBackProc )
{
  return 0;
}

int DioCommonGetPciDeviceInfo (int nDevice, 
                               unsigned short* pnDeviceID,
                               unsigned short* pnVendorID,
                               unsigned long*  plClassCode,
                               unsigned char*  pbRevisionID,
                               unsigned long*  plBaseAddress0,
                               unsigned long*  plBaseAddress1,
                               unsigned long*  plBaseAddress2,
                               unsigned long*  plBaseAddress3,
                               unsigned long*  plBaseAddress4,
                               unsigned long*  plBaseAddress5,
                               unsigned short* pnSubsystemID,
                               unsigned short* pnSubsystemVendorID,
                               unsigned char*  pbInterruptLine,
                               unsigned long*  plBoardID)
{
  return 0;
}


int DioEintSetIrqMask(int Device, unsigned long IrqMask)
{
  return 0;
}


int DioEintGetIrqMask(int Device, unsigned long *IrqMask)
{
  return 0;
}


int DioEintSetEdgeConfig(int Device, unsigned long FallEdge,
                         unsigned long RiseEdge)
{
  return 0;
}


int DioEintGetEdgeConfig(int Device, unsigned long *FallEdge,
                         unsigned long *RiseEdge)
{
  return 0;
}


int DioEintInputPoint(int Device, int *Buffer, 
                      unsigned long StartNum, unsigned long Num)
{
  return 0;
}


int DioEintInputByte(int Device, int No, unsigned char *FallValue ,
                     unsigned char *RiseValue)
{
  return 0;
}


int DioEintInputWord(int Device, int No, unsigned short *FallValue ,
                     unsigned short *RiseValue)
{
  return 0;
}

  
int DioEintInputDword(int Device, int No, unsigned long *FallValue,
                      unsigned long *RiseValue)
{
  return 0;
}


int DioEintSetFilterConfig(int Device, int No, unsigned long FilterConfig)
{
  return 0;
}


int DioEintGetFilterConfig(int Device, int No, unsigned long *FilterConfig)
{
  return 0;
}


int DioSetRstinMask(int Device, unsigned long ulRstinMask)
{
  return 0;
}


int DioGetRstinMask(int Device, unsigned long *ulRstinMask)
{
  return 0;
}

