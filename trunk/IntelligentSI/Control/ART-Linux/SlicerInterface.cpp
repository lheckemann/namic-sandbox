/***************************************************************************
 * FileName      : SlicerInterface.cpp
 * Created       : 2008/7/21
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : Interface for connecting slicer from slave robot
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "SlicerInterface.h"

IF_DATA SlicerInterface::IF_Data;
IGTL_PHANTOM SlicerInterface::writeData;
SlicerInterface* SlicerInterface::IF_Instance=NULL;

#ifdef __linux__ //if __linux__
using namespace std;
#endif //if __linux__

SlicerInterface*
SlicerInterface::createInterface(char *myName, char* yourName, int outPort, int inPort){
  if(IF_Instance ==NULL){
    IF_Data.clientName = yourName;
    IF_Data.outPortNum = outPort;
    IF_Data.inPortNum = inPort;
    IF_Data.packetSize = sizeof(IGTL_PHANTOM);
    IF_Data.packetNum = 10;
    
    // IGTLink Protocol
    writeData.header.V = 1;
    char dataType[] = "Data";
    int num = sizeof(dataType);

    for(int i=0; i<num; i++){
      writeData.header.TYPE[i] = dataType[i];
    }
    num = sizeof(myName);
    for(int i=0; i<num; i++){
      writeData.header.DEVICE_NAME[i] = myName[i];
    }
    writeData.header.TIME_STAMP = 0;
    writeData.header.BODY_SIZE = (unsigned long long)(sizeof(writeData.Pee));
    writeData.header.CRC = 0;//crc32();
    
    IF_Instance = new SlicerInterface();
    
  }
  return IF_Instance;
}

SlicerInterface::SlicerInterface():IF_clientSock(CLIENT, IF_Data.clientName, IF_Data.outPortNum){
  cout<<"Initialize Interface Class..."<<endl;

#ifndef __linux__ //if not __linux__
  //semaphore
  wMutex = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
  
  //task
  kernelTimeSlice(1);
  taskSpawn("tIFOutSlicerTask", 95, VX_FP_TASK, 50000, (FUNCPTR)write,
        (int)this, (int)&IF_clientSock, (int)&wMutex,0,0,0,0,0,0,0);
#else  //if __linux__
  //semaphore
  pthread_mutex_init(&wMutex, NULL);     // for 'write' pthread sync
  sem_init(&wSem,0,0);                   // for 'write' pthread sync

        //task (thread here)
        int ret=0;
        pthread_attr_t pt_attr;
        struct sched_param pt_priority_param;
        if ((ret = pthread_attr_init(&pt_attr))) 
          cerr << "SlicerInterface: ERROR initialising the pthread attr parameter; returned " << ret << endl;
        // a root is needed to set some thread parameters; using defaults if not root
        if (geteuid() == 0) {
          //2==SCHED_RR which is RoundRobin; 
          if ((ret = pthread_attr_setschedpolicy(&pt_attr, 2))) 
          cerr << "SlicerInterface: ERROR setting the pthread scheduling policy; returned " << ret << endl; 
          //priority
          int maxprio = sched_get_priority_max(2);
          int minprio = sched_get_priority_min(2);
          /* 4/10 comes from vxworks' priority 100 in (max=0;min=255); 
     * TODO: confirm: to use similar priority as in the taskSpawn call above
     * we need vxw 95, which is 5 higher than 100 (the PNTM_intf task), 
     * so we use 5 higher in ARTLinux too (usually min=1, max=99). 
     */
          pt_priority_param.sched_priority = maxprio - (maxprio - minprio)*4/10 + 5;
          if (pt_priority_param.sched_priority > maxprio) pt_priority_param.sched_priority = maxprio; //just in case
          if ((ret = pthread_attr_setschedparam(&pt_attr, &pt_priority_param))) 
          cerr << "SlicerInterface: ERROR setting the pthread priority; returned " << ret << endl; 
          //stack size
          if ((ret = pthread_attr_setstacksize(&pt_attr, 50000)))
          cerr << "SlicerInterface: ERROR setting the pthread stacksize; returned " << ret << endl; 
        }
        outSlicerThreadData.sock = &IF_clientSock;
        outSlicerThreadData.xMutex = &wMutex; //TODO: confirm if we need these
        outSlicerThreadData.xSem = &wSem; 
        if ((ret = pthread_create(&tIFOutSlicerThread, &pt_attr, write, (void *) &outSlicerThreadData)))
          cerr << "SlicerInterface: ERROR creating the write thread; returned " << ret << endl;
        pthread_attr_destroy(&pt_attr);
#endif //if not __linux__
  
  cout<<"Interface done."<<endl;
}

SlicerInterface::~SlicerInterface(){
#ifndef __linux__ //if not __linux__
    semDelete(wMutex);
    IF_Instance = NULL;
    taskDelete(taskNameToId("tIFOutSlicerTask"));
#else  //if __linux__
    pthread_mutex_destroy(&wMutex);
    sem_destroy(&wSem);
    pthread_cancel(tIFOutSlicerThread);
#endif //if not __linux__
    cout<<"End Interface."<<endl;
}

#ifndef __linux__ //if not __linux__
void
SlicerInterface::write(void* thisFunc, void* sock, void* mutex){
  udpSOCK *writeSock = (udpSOCK*) sock;
  SEM_ID* wMutex = (SEM_ID*) mutex;
  int err;
  while(true){
    semTake( *wMutex, WAIT_FOREVER);
    err = writeSock->Write( (char*)&writeData, IF_Data.packetSize);
  }
}
#else  //if __linux__
void*
SlicerInterface::write(void* pt_arg){
    SlicerThreadData *threadData;
    threadData = (SlicerThreadData *) pt_arg;
    int err;
    IGTL_PHANTOM wDataCopy;
    while(true){
      sem_wait(threadData->xSem);
      pthread_mutex_lock(threadData->xMutex);
      wDataCopy = writeData;
      pthread_mutex_unlock(threadData->xMutex);
      err = threadData->sock->Write( (char*)&wDataCopy, IF_Data.packetSize);
      //TODO: handle the error if any
    }
    return(NULL);
}
#endif //if not __linux__


void
SlicerInterface::send(EE_POSITION_S* data){
#ifdef __linux__ //if __linux__
        pthread_mutex_lock(&wMutex);
#endif //if __linux__

  writeData.Pee = *data;

#ifndef __linux__ //if not __linux__
  semGive(wMutex);

#else  //if __linux__
        pthread_mutex_unlock(&wMutex); 
        sem_post(&wSem); 
#endif //if not __linux__
}

float
SlicerInterface::W_BitSwap( const float* ptr){
  float data = 0;
  const unsigned char *p  = (unsigned char *)ptr;
  unsigned char * p_ = (unsigned char*)&data;
  p_+= 3;
  for(int i=0; i<4; ++i, ++p, --p_){
    *p_ = *p;
  }
  return data;
}
