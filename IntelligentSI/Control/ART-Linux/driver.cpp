/***************************************************************************
 * FileName      : driver.cpp
 * Created       : 2007/08/27
 * LastModified  : 2007/10/
 * Author        : hiroaki KOZUKA
 * Aim           : driver class for a board control
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "driver.h"

#if defined VX__DRIVER
//driver
//ACP420* ACP420::m_Instance = NULL;
//ACP550* ACP550::m_Instance = NULL;
//ACP560* ACP560::m_Instance = NULL;
#endif

#ifndef __linux__ //if not __linux__
const HARDWARE_DATA_ DRIVER::HW[jNum];
#else  //if __linux__
const HARDWARE_DATA_ DRIVER::HW[jNum] ={
  {10, -10, 0, 1, revl, minus, 67548},
  {10, -10, 0, 1, revl, minus, 67548},
  {10, -10, 0, 1, revl, plus, 23552}, //TODO: check if not this: {10, -10, 0, 1, revl, minus, 2252.8}
};
#endif //if not __linux__


DRIVER::DRIVER(){
    //logMsg("Initialaze Driver Class...\n",0,0,0,0,0,0);
    cout<<"Initialize Driver Class..."<<endl;
   
#if defined VX__DRIVER
  // initialize Counter(ACP420), A/D(ACp550), D/A(ACp560)
    CNT_D = ACP420::getInstance();
    AD_D = ACP550::getInstance();
    DA_D = ACP560::getInstance();
#elif defined ART__DRIVER
    hrpifb = HRPIFB::getInstance();
#endif
}

DRIVER::~DRIVER(){
#if defined VX__DRIVER
    //delete board driver
  ACP420::m_Instance = NULL;
  ACP550::m_Instance = NULL;
  ACP560::m_Instance = NULL;
    delete CNT_D;
    delete AD_D;
    delete DA_D;
#elif defined ART__DRIVER
    HRPIFB::m_Instance = NULL;
    delete hrpifb;
#endif
    //logMsg("End Driver class.\n",0,0,0,0,0,0);
    cout<<"End Driver class."<<endl;
}

double
DRIVER::directionDataShow(int jid){
     return ( HW[jid].direction );
}

double
DRIVER::angleGet(int jid){
    return(rAngle[jid]);
}

void
DRIVER::angleSet(int jid, double setAngle){
   wAngle[jid] = setAngle;
}

void
DRIVER::angleReadWrite(int R_W){
    long EncData[jNum];
    if(R_W == 0){
        for(int jID=0; jID<jNum; jID++){
            EncData[jID] = (long)(wAngle[jID] * (HW[jID].pulseNum*4) / HW[jID].revl_linear);
        }
#if defined VX__DRIVER
        CNT_D->Write(0, EncData, sizeof(long)*4 );
#elif defined ART__DRIVER
  hrpifb->Write(0, EncData); 
#endif
    }
    else if(R_W == 1){
#if defined VX__DRIVER
        CNT_D->Read(0, EncData, sizeof(long)*4 );
#elif defined ART__DRIVER
  hrpifb->Read(0, EncData); //TODO: data size?
#endif
        for(int jID=0; jID < jNum; jID++){
            rAngle[jID] = HW[jID]. revl_linear*(double)EncData[jID]/(HW[jID].pulseNum*4);
        }
    }
}

void
DRIVER::speedSet(int jid, double speedData){
    wVolt[jid] = converSpeed2Volt( jid, speedData);
}

void
DRIVER::speedWrite(){
#if defined VX__DRIVER
    DA_D->Output(0, wVolt[0], wVolt[1],  wVolt[2], 0);
  //DA_D->Output(0, 0, 0, 0, 0);
#elif defined ART__DRIVER
    //hrpifb->Output(0, wVolt[0], wVolt[1], 0, wVolt[2]);
    hrpifb->Output(0, wVolt[0], wVolt[1], wVolt[2], 0);
#endif
}

double
DRIVER::converSpeed2Volt(int jid, double speed){
    //Conversion from speed to volt
    double volt = HW[jid].speedtoVolt*speed;
    
    //Regulation volt
    if( volt > HW[jid].maxVolt )
  volt = HW[jid].maxVolt;
    else if( volt < HW[jid].minVolt )
  volt = HW[jid].minVolt;
    else if((volt < HW[jid].maxVolt*0.01 ) && (volt > HW[jid].minVolt * 0.01) )
  volt = HW[jid].Vo;
    
    return(volt);
}

void
DRIVER::angleZeroSet(){
    long EncData[jNum];
    for(int jID=0; jID<jNum; jID++){
  EncData[jID] = 0;
    }
#if defined VX__DRIVER
    CNT_D->Write(0, EncData , sizeof(long)*4);
#elif defined ART__DRIVER
    hrpifb->Write(0, EncData);  //TODO: data size?
#endif
}

void
DRIVER::stop(){
#if defined VX__DRIVER
    DA_D->Output(0,0,0,0,0);
#elif defined ART__DRIVER
    hrpifb->Output(0,0,0,0,0);
#endif
}


