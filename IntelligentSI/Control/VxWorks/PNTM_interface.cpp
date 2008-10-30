/***************************************************************************
 * FileName      : PNTM_interface.cpp
 * Created       : 2008/2/5
 * LastModified  : 2008/
 * Author        : Hiroaki KOZUKA
 * Aim           : Interface for connecting slave robot from PHANToM
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "PNTM_interface.h"

EVENT PNTM_INTERFACE::stateFlag = E_CTRL_RUN_STOP;

IF_DATA PNTM_INTERFACE::IF_Data;
IGTL_PHANTOM PNTM_INTERFACE::writeData;
IGTL_PHANTOM PNTM_INTERFACE::readData;

PNTM_INTERFACE*
PNTM_INTERFACE::createInterface(char *myName, char* yourName, int outPort, int inPort){
  //if(IF_Instance ==NULL){
    IF_Data.clientName = yourName;
    IF_Data.outPortNum = outPort;
    IF_Data.inPortNum = inPort;
    IF_Data.packetSize = sizeof(IGTL_PHANTOM);
    
    IF_Data.packetNum = 10;
    //IF_Data.buffSize = IF_Data.packetSize * IF_Data.packetNum;
    
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
    
    IF_Instance = new PNTM_INTERFACE();
    
  //}
  return IF_Instance;
}

PNTM_INTERFACE::PNTM_INTERFACE():IF_serverSock(SERVER, IF_Data.clientName, IF_Data.inPortNum),
                       IF_clientSock(CLIENT, IF_Data.clientName, IF_Data.outPortNum){
  //logMsg("Initialize Interface Class...\n",0,0,0,0,0,0);
  cout<<"Initialize Interface Class..."<<endl;
  //semaphore
  exclusMutex = semMCreate(SEM_Q_FIFO);
  wMutex = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
  Mutex = semBCreate(SEM_Q_FIFO, SEM_FULL );
  //create messege Q
  IF_R_MsgQID = msgQCreate( IF_Data.packetNum, IF_Data.packetSize, MSG_Q_FIFO );
  //IF_W_MsgQID = msgQCreate( IF_Data.buffSize, IF_Data.buffSize, MSG_Q_FIFO );
  semTake(Mutex, WAIT_FOREVER);
  //stateFlag = E.CTRL_STOP_START;
  semGive(Mutex);
  
  //task
  kernelTimeSlice(1);
  taskSpawn("tIFInSockTask", 95, VX_FP_TASK, 50000, (FUNCPTR)read,(int)this,
        (int)&IF_serverSock, (int)&IF_R_MsgQID,0,0,0,0,0,0,0);
  
  taskSpawn("tIFOutSockTask", 100, VX_FP_TASK, 50000, (FUNCPTR)write,
        (int)this, (int)&IF_clientSock, (int)&wMutex,0,0,0,0,0,0,0);
  
  //logMsg("Interface Done.\n",0,0,0,0,0,0);
  cout<<"Interface done."<<endl;
}


PNTM_INTERFACE::~PNTM_INTERFACE(){
    semDelete(exclusMutex);
  semDelete(Mutex);
  semDelete(wMutex);
    msgQDelete(IF_R_MsgQID);
    taskDelete(taskNameToId("tIFInSockTask"));
    taskDelete(taskNameToId("tIFOutSockTask"));
    //logMsg("End Interface.\n",0,0,0,0,0,0);
    cout<<"End Interface."<<endl;
}

void
PNTM_INTERFACE::read(void* thisFunc, void* sock, void* msgQ){
    int num;
    char buff[IF_Data.packetSize];
    udpSOCK *readSock = (udpSOCK*) sock;
    MSG_Q_ID *IF_MsgQID = (MSG_Q_ID*) msgQ;
    while(true){
    num = recv(readSock->getSd(), buff, IF_Data.packetSize, MSG_PEEK);
    //read data from socket
    readSock->Read(buff, IF_Data.packetSize);
    //write data to msgQ
    if(msgQNumMsgs(*IF_MsgQID)<IF_Data.packetNum)
      msgQSend(*IF_MsgQID, buff, IF_Data.packetSize, WAIT_FOREVER, MSG_PRI_NORMAL);
    else{
      do{
        msgQReceive(*IF_MsgQID, buff, IF_Data.packetSize, NO_WAIT);
      }while(msgQNumMsgs(*IF_MsgQID) > 1);
    }
    
    }
}

void
PNTM_INTERFACE::write(void* thisFunc, void* sock, void* mutex){
  udpSOCK *writeSock = (udpSOCK*) sock;
  SEM_ID* wMutex = (SEM_ID*) mutex;
  int err;
  while(true){
    semTake( *wMutex, WAIT_FOREVER);
    err = writeSock->Write( (char*)&writeData, IF_Data.packetSize);
  }
}


void
PNTM_INTERFACE::send(EE_POSITION_S* data){
  writeData.Pee = *data;
  semGive(wMutex);
}

void
PNTM_INTERFACE::recieve(EE_POSITION_S* data){
  msgQReceive( IF_R_MsgQID, (char*)&readData, IF_Data.packetSize, NO_WAIT );
  if(readData.header.V == 1 && readData.header.BODY_SIZE == sizeof(EE_POSITION_S) ){  
    *data = readData.Pee;
    if(readData.Pee.alpha==1){
      semTake(Mutex, NO_WAIT);
      stateFlag = E_CTRL_RUN_START;
      semGive(Mutex);
    }
    else{
      semTake(Mutex, NO_WAIT);
      stateFlag = E_CTRL_RUN_STOP;
      semGive(Mutex);
    }
  }
  
}

EVENT
PNTM_INTERFACE::eventRead(){
  EVENT flag = E_CTRL_RUN_STOP;
  semTake(Mutex, NO_WAIT);
  flag = stateFlag;
  semGive(Mutex);
  return flag;
}

float
PNTM_INTERFACE::W_BitSwap( const float* ptr){
  float data = 0;
  const unsigned char *p  = (unsigned char *)ptr;
  unsigned char * p_ = (unsigned char*)&data;
  p_+= 3;
  for(int i=0; i<4; ++i, ++p, --p_){
    *p_ = *p;
  }
  return data;
}
