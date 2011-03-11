#include "fbida_sim.h"

int DaOpen(int nDevice)
{
  return 0;
}

int DaClose(int nDevice)
{
  return 0;
}

int DaCloseEx(int nDevice, int nFinalState)
{
  return 0;
}

int DaGetDeviceInfo(int nDevice, PDABOARDSPEC pDaBoardSpec)
{
  return 0;
}

int DaSetBoardConfig(int nDevice, unsigned long ulSmplBufferSize, 
                     void* pReserved, PLPDACALLBACK pEventProc, int dwUser)
{
  return 0;
}

int DaGetBoardConfig(int nDevice, unsigned long *ulSmplBufferSize, 
                     unsigned long  *ulSmplEventFactor)
{
  return 0;
}

int DaSetSamplingConfig(int nDevice, PDASMPLREQ pDaSmplConfig)
{
  return 0;
}

int DaGetSamplingConfig(int nDevice, PDASMPLREQ pDaSmplConfig)
{
  return 0;
}

int DaSetFifoConfig(int nDevice, PDAFIFOREQ pDaFifoConfig)
{
  return 0;
}

int DaGetFifoConfig(int nDevice, PDAFIFOREQ pDaFifoConfig)
{
  return 0;
}

int DaSetMode(int nDevice, PDAMODEREQ pDaMode)
{
  return 0;
}

int DaGetMode(int nDevice, PDAMODEREQ pDaMode)
{
  return 0;
}

int DaSetSamplingData(int nDevice, void * pSmplData, unsigned long ulSmplDataNum)
{
  return 0;
}

int DaClearSamplingData(int nDevice)
{
  return 0;
}

int DaStartSampling(int nDevice, unsigned long ulSyncFlag)
{
  return 0;
}

int DaSyncSampling(int nDevice, unsigned long ulMode)
{
  return 0;
}

int DaStopSampling(int nDevice)
{
  return 0;
}

int DaGetStatus(int nDevice, unsigned long *ulDaSmplStatus,
                unsigned long *ulDaSmplCount, 
                unsigned long *ulDaAvailCount, unsigned long *ulDaAvailRepeat)
{
  return 0;
}
  
int DaGetOutputMode(int nDevice, unsigned long *ulMode)
{
  return 0;
}

int DaSetOutputMode(int nDevice, unsigned long ulMode)
{
  return 0;
}

int DaOutputDA(int nDevice, unsigned long ulCh, 
               PDASMPLCHREQ pDaSmplChReq, void * lpData)
{
  return 0;
}

int DaInputDI(int nDevice, unsigned long *ulData)
{
  return 0;
}

int DaOutputDO(int nDevice, unsigned long ulData)
{
  return 0;
}

int DaDataConv(unsigned long p1, void *p2, unsigned long p3,  PDASMPLREQ p4,
               unsigned long p5, void* p6, unsigned long* p7, PDASMPLREQ p8,
               unsigned long p9, unsigned long p10, PDACONVPROC p11)
{
  return 0;
}


int DaSetInterval(int nDevice, unsigned long ulInterval)
{
  return 0;
}

int DaGetInterval(int nDevice, unsigned long *ulInterval)
{
  return 0;
}

int DaSetFunction(int nDevice, unsigned int unCnNo, unsigned long ulFunction)
{
  return 0;
}

int DaGetFunction(int nDevice, unsigned int unCnNo, unsigned long *ulFunction)
{
  return 0;
}

