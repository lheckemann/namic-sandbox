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
  //semaphore
  wMutex = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
  
  //task
  kernelTimeSlice(1);
  taskSpawn("tIFOutSlicerTask", 95, VX_FP_TASK, 50000, (FUNCPTR)write,
        (int)this, (int)&IF_clientSock, (int)&wMutex,0,0,0,0,0,0,0);
  
  cout<<"Interface done."<<endl;
}

SlicerInterface::~SlicerInterface(){
  semDelete(wMutex);
  IF_Instance = NULL;
    taskDelete(taskNameToId("tIFOutSlicerTask"));
    cout<<"End Interface."<<endl;
}

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


void
SlicerInterface::send(EE_POSITION_S* data){
  writeData.Pee = *data;
  semGive(wMutex);
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
