/***************************************************************************
 * FileName      : IFmanager.cpp
 * Created       : 2007/12/2
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : management interface classes
 *                 IFmanager class
 * OS            : VxWorks 5.5.1
 ******************z*********************************************************/
#include "IFmanager.h"

PNTM_INTERFACE*  PNTM_INTERFACE::IF_Instance = NULL;
//STATE_MACHINE IF_MANAGER::flag;

#ifdef __linux__ //if __linux__
using namespace std;
#endif //if __linux__

#define sq(x) ((x)*(x))

IF_MANAGER::IF_MANAGER(){ 
  //logMsg("Initialize Interface Manager Class...\n",0,0,0,0,0,0);
  cout<<"Initialize Interface Manager Class..."<<endl;
  
  // semaphore
#ifndef __linux__ //if not __linux__
  exclusMutex   = semBCreate(SEM_Q_FIFO, SEM_FULL);
#else //if __linux__
        pthread_mutex_init(&exclusMutex, NULL); //TODO: confirm if 'fast' mutex is OK 
#endif //if not __linux__
  
  // create socket interface
  // Master:sumatra , Slave:jawa
  slave = PNTM_INTERFACE::createInterface( "jawa", "sumatra",
             SLAVE_TO_MASTER_PORT, MASTER_TO_SLAVE_PORT  );
  slicer = SlicerInterface::createInterface( "jawa", "navi", 50000, 50001  );
  
  keyCon = new KEYCONSOLE();
  
  //logMsg("Interface Manager Done.\n",0,0,0,0,0,0);
  cout<<"Interface Manager Done."<<endl;
}

IF_MANAGER::~IF_MANAGER(){
  //sem
#ifndef __linux__ //if not __linux__
  semDelete( exclusMutex );
#else //if __linux__
        pthread_mutex_destroy(&exclusMutex); 
#endif //if not __linux__
  
  //I/F
  delete slave;
  delete slicer;
  delete keyCon;
  PNTM_INTERFACE::IF_Instance = NULL;
  //logMsg("End Interface Manager.\n",0,0,0,0,0,0);
  cout<<"End Interface Manager."<<endl;
}

float
IF_MANAGER::F_BitSwap( const float* ptr){
  float data = 0;
  const unsigned char *p  = (unsigned char *)ptr;
  unsigned char * p_ = (unsigned char*)&data;
  p_+= 3;
  for(int i=0; i<4; ++i, ++p, --p_){
    *p_ = *p;
  }
  return data;
}

void
IF_MANAGER::EEPositionSend(EE_POSITION* PEE){
  sPee.x = (float)PEE->x;
  sPee.y = (float)PEE->y;
  sPee.z = (float)PEE->z; 
  sPee.alpha = (float)atan( -PEE->y/PEE->z );
  sPee.beta  = (float)atan( PEE->x/PEE->z );
  sPee.gamma = (float)PEE->gamma;
  slicer->send(&sPee);
  //cout<<"send slicer."<<endl;
}


void
IF_MANAGER::robotDataSend(ARM_DATA* arm){
  
}

void
IF_MANAGER::jointDataSend(JOINT_DATA* joint){
  //
  jointData = *joint;
  //
  //etc...
}

void
IF_MANAGER::driverDataSend(DRIVER_STAT_DATA* driver){
  
}

int
IF_MANAGER::destEEPositionGet(int jID, EE_POSITION* destPosition){
  double scale = 0.1;
  EE_POSITION position = *destPosition;
  slave->recieve(&rPee);
  
  destPosition->y = rPee.x*scale;
  destPosition->z = rPee.y*scale;
  destPosition->x = rPee.z*scale;
  destPosition->alpha = 0;
  destPosition->beta  = 0;
  destPosition->gamma = 0;
  
  return 0;
}

EVENT
IF_MANAGER::eventRead(STATE state ){
  EVENT keyFlag = keyCon->eventRead();
  EVENT slaveFlag = slave->eventRead();
  EVENT flag = keyFlag;

  if( (keyFlag == E_CTRL_STOP_START || keyFlag == E_PRGM_START ) && ( state == S_CTRL_STOP || state == S_CTRL_RUN ) ){
    flag = slaveFlag;
  }

  return flag;
}
