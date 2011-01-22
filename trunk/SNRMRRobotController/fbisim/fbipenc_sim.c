#include "fbipenc_sim.h"

int PencOpen(int nDevice, unsigned long fulFlags)
{
  return 0;
}


int PencClose(int nDevice)
{
  return 0;
}


int PencSetMode(int nDevice, int nChannel, unsigned int nMode,
                                int nDirection, int nEqual, int nLatch )
{
  return 0;
}


int PencGetMode(int nDevice, int nChannel, unsigned int *pnMode,
                                int *pnDirection, int *pnEqual, int *pnLatch )
{
  return 0;
}


int PencSetCounter(int nDevice, int nChannel, unsigned long  ulCounter )
{
  return 0;
}


int PencGetCounter(int nDevice, int nChannel, unsigned long *pulCounter )
{
  return 0;
}


int PencSetCounterEx(int nDevice, unsigned int nChSel,
                     unsigned long *pulCounter )
{
  return 0;
}


int PencGetCounterEx(int nDevice, unsigned int nChSel,
                     unsigned long *pulCounter )
{
  return 0;
}


int PencSetComparator(int nDevice, int nChannel,
                      unsigned long  ulComparator )
{
  return 0;
}


int PencGetComparator(int nDevice, int nChannel,
                      unsigned long *pulComparator)
{
  return 0;
}


int PencSetZMode(int nDevice, int nChannel, unsigned int nZMode )
{
  return 0;
}


int PencGetZMode(int nDevice, int nChannel, unsigned int *pnZMode )
{
  return 0;
}


int PencEnableCount(int nDevice, unsigned int nChSel, int nEnable )
{
  return 0;
}


int PencGetStatus(int nDevice, int nChannel, unsigned int *pnStatus)
{
  return 0;
}


int PencGetStatusEx(int nDevice, unsigned int nChSel,
                    unsigned long *pulCounter,
                                        unsigned long *pulStatus )
{
  return 0;
}


int PencReset(int nDevice, int nChannel )
{
  return 0;
}


int PencSetEventMask(int nDevice, int nChannel,
                     int nEventMask, int nTimerMask)
{
  return 0;
}


int PencGetEventMask(int nDevice, int nChannel,
                     int *pnEventMask, int *pnTimerMask)
{
  return 0;
}


int PencSetEventEx(int nDevice, LPPENCCALLBACKEX lpEventProcEx,
                   unsigned long ulUser)
{
  return 0;
}


int PencKillEvent(int nDevice)
{
  return 0;
}


int PencSetTimerConfig(int nDevice, unsigned char ucTimerConfig)
{
  return 0;
}


int PencGetTimerConfig(int nDevice, unsigned char *pucTimerConfig)
{
  return 0;
}


int PencGetTimerCount(int nDevice, unsigned char *pucTimerCount)
{
  return 0;
}


