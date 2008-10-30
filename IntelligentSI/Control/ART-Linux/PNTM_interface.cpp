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

#ifdef __linux__ //if __linux__
using namespace std;
#endif //if __linux__


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

#ifndef __linux__ //if not __linux__
  //semaphore
  exclusMutex = semMCreate(SEM_Q_FIFO);
  wMutex = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
  Mutex = semBCreate(SEM_Q_FIFO, SEM_FULL);
  //create messege Q
  IF_R_MsgQID = msgQCreate( IF_Data.packetNum, IF_Data.packetSize, MSG_Q_FIFO );
  //IF_W_MsgQID = msgQCreate( IF_Data.buffSize, IF_Data.buffSize, MSG_Q_FIFO );

  semTake(Mutex, WAIT_FOREVER);
#else  //if __linux__
       // semaphore
  pthread_mutex_init(&exclusMutex, NULL);
  pthread_mutex_init(&Mutex, NULL);     
  pthread_mutex_init(&wMutex, NULL);     // for 'write' pthread sync
  sem_init(&wSem,0,0);                   // for 'write' pthread sync

  //create message queue
  key_t key_r;
  if((key_r = ftok(KEY_PATHNAME, KEY_PROJID)) == -1) {
    cerr << "INTERFACE: ERROR creating message queue: ftok failed errno=" << errno << endl;
    //TODO: handle properly
  }
  if((IF_R_MsgQID = msgget(key_r, 0644 | IPC_CREAT)) == -1) {
    cerr << "INTERFACE: ERROR creating message queue: msgget failed errno=" << errno << endl;
    //TODO: handle properly
  }
  if((r_msgbuff = (sv_msgbuf *)malloc(sizeof(sv_msgbuf) + IF_Data.packetSize)) == NULL) {
    cerr << "INTERFACE: ERROR creating message queue: couldn't allocate memory for r_msgbuf." << endl;
    //TODO: handle properly
  }
  r_msgbuff->mtype=1;
  memset(r_msgbuff->mtext, '\0', sizeof(r_msgbuff->mtext));

  pthread_mutex_lock(&Mutex);
#endif //if not __linux__

  //stateFlag = E.CTRL_STOP_START;

#ifndef __linux__ //if not __linux__
  semGive(Mutex);
#else  //if __linux__
  pthread_mutex_unlock(&Mutex);
#endif //if not __linux__
  

#ifndef __linux__ //if not __linux__
  //task
  kernelTimeSlice(1);
  taskSpawn("tIFInSockTask", 95, VX_FP_TASK, 50000, (FUNCPTR)read,(int)this,
        (int)&IF_serverSock, (int)&IF_R_MsgQID,0,0,0,0,0,0,0);
  
  taskSpawn("tIFOutSockTask", 100, VX_FP_TASK, 50000, (FUNCPTR)write,
        (int)this, (int)&IF_clientSock, (int)&wMutex,0,0,0,0,0,0,0);

#else  //if __linux__

        //task (threads here)
        int ret=0;
        pthread_attr_t pt_attr;
        struct sched_param pt_priority_param;
        if ((ret = pthread_attr_init(&pt_attr))) 
          cerr << "INTERFACE: ERROR initialising the pthread attr parameter; returned " << ret << endl;
        // a root is needed to set some thread parameters; using defaults if not root
        if (geteuid() == 0) {
          //2==SCHED_RR which is RoundRobin; 
          if ((ret = pthread_attr_setschedpolicy(&pt_attr, 2))) 
          cerr << "INTERFACE: ERROR setting the pthread scheduling policy; returned " << ret << endl; 
          //priority
          int maxprio = sched_get_priority_max(2);
          int minprio = sched_get_priority_min(2);
          // 4/10 comes from vxworks' priority 100 in (max=0;min=255); should keep it same as in the taskSpawn calls above
          pt_priority_param.sched_priority = maxprio - (maxprio - minprio)*4/10;
          if ((ret = pthread_attr_setschedparam(&pt_attr, &pt_priority_param))) 
          cerr << "INTERFACE: ERROR setting the pthread priority; returned " << ret << endl; 
          //stack size
          if ((ret = pthread_attr_setstacksize(&pt_attr, 50000)))
          cerr << "INTERFACE: ERROR setting the pthread stacksize; returned " << ret << endl; 
        }
        inSockThreadData.msgQid = &IF_R_MsgQID;
        inSockThreadData.sock = &IF_serverSock;
        inSockThreadData.xMutex = NULL;  //not used
        inSockThreadData.xSem = NULL;    //not used
        if ((ret = pthread_create(&tIFInSockThread, &pt_attr, read, (void *) &inSockThreadData)))
          cerr << "INTERFACE: ERROR creating the read thread; returned " << ret << endl;
        outSockThreadData.sock = &IF_clientSock;
        outSockThreadData.xMutex = &wMutex; //TODO: confirm if we need these
        outSockThreadData.xSem = &wSem; 
        if ((ret = pthread_create(&tIFOutSockThread, &pt_attr, write, (void *) &outSockThreadData)))
          cerr << "INTERFACE: ERROR creating the write thread; returned " << ret << endl;
        pthread_attr_destroy(&pt_attr);
#endif //if not __linux__

  
  //logMsg("Interface Done.\n",0,0,0,0,0,0);
  cout<<"Interface done."<<endl;
}


PNTM_INTERFACE::~PNTM_INTERFACE(){
#ifndef __linux__ //if not __linux__
    semDelete(exclusMutex);
    semDelete(Mutex);
    semDelete(wMutex);
    msgQDelete(IF_R_MsgQID);
    taskDelete(taskNameToId("tIFInSockTask"));
    taskDelete(taskNameToId("tIFOutSockTask"));
#else  //if __linux__
    pthread_mutex_destroy(&exclusMutex);
    pthread_mutex_destroy(&Mutex);
    pthread_mutex_destroy(&wMutex);
    sem_destroy(&wSem);
    msgctl(IF_R_MsgQID, IPC_RMID, NULL);
    pthread_cancel(tIFOutSockThread);
    pthread_cancel(tIFInSockThread);
#endif //if not __linux__
    //logMsg("End Interface.\n",0,0,0,0,0,0);
    cout<<"End Interface."<<endl;
}

#ifndef __linux__ //if not __linux__
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
#else  //if __linux__
void *
PNTM_INTERFACE::read(void* pt_arg){
    SockThreadData *threadData;
    threadData = (SockThreadData *) pt_arg;
    sv_msgbuf *svbuff, *svbuff_tmp; 
    if ((svbuff = (sv_msgbuf *)malloc(sizeof(sv_msgbuf) + IF_Data.packetSize)) == NULL) {
      cerr << "INTERFACE(read): ERROR: couldn't allocate memory for the message buffer." << endl;
      //TODO: handle properly
    }
    if ((svbuff_tmp = (sv_msgbuf *)malloc(sizeof(sv_msgbuf) + IF_Data.packetSize)) == NULL) {
      cerr << "INTERFACE(read): ERROR: couldn't allocate memory for the temporary message buffer." << endl;
      //TODO: handle properly
    }
    svbuff->mtype=1;
    memset(svbuff->mtext, '\0', sizeof(svbuff->mtext));
    svbuff_tmp->mtype=1;
    memset(svbuff_tmp->mtext, '\0', sizeof(svbuff_tmp->mtext));
    msqid_ds msq_info;
    int num;
    while(true){
      num = recv(threadData->sock->getSd(), svbuff->mtext, IF_Data.packetSize, MSG_PEEK);
      //read data from socket
      threadData->sock->Read(svbuff->mtext, IF_Data.packetSize); // TODO: check retval?
      //write data to msgQ
      if (msgsnd(*(threadData->msgQid), (struct msgbuf *)svbuff, IF_Data.packetSize, IPC_NOWAIT) == -1) {
  if (errno == EAGAIN) {
    msgctl(*(threadData->msgQid), IPC_STAT, &msq_info);
    do{
      msgrcv(*(threadData->msgQid), (struct msgbuf *)svbuff_tmp, IF_Data.packetSize, 0, IPC_NOWAIT);
      //TODO: put checks here to avoid getting stuck in the loop
    } while(msq_info.msg_qnum > 1);
    // TODO: shouldn't we send the message here? Keep similar to vxworks' code now.
    //msgsnd(threadData->msgQid, (struct msgbuf *)svbuff, IF_Data.packetSize, IPC_NOWAIT);
  }
  else {
    cerr << "INTERFACE(read): ERROR: couldn't send message to the msg_queue." << endl;
    //TODO: handle properly
  }
      } 
    }
}
#endif //if not __linux__





#ifndef __linux__ //if not __linux__
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
#else  //if __linux__
void *
PNTM_INTERFACE::write(void* pt_arg){
    SockThreadData *threadData;
    threadData = (SockThreadData *) pt_arg;
    int err;

#if defined IGTL__HEADER
  IGTL_PHANTOM wDataCopy;
#else
  MASTER_PHANTOM_DATA wDataCopy;
#endif

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
PNTM_INTERFACE::send(EE_POSITION_S* data){
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

void
PNTM_INTERFACE::recieve(EE_POSITION_S* data){
#ifndef __linux__ //if not __linux__
  msgQReceive( IF_R_MsgQID, (char*)&readData, IF_Data.packetSize, NO_WAIT );
#else  //if __linux__
  msgrcv(IF_R_MsgQID, (struct msgbuf *)r_msgbuff, IF_Data.packetSize, 0, IPC_NOWAIT);
  memcpy((void*)&readData, (const void*) &(r_msgbuff->mtext), IF_Data.packetSize);
  /* TODO: confirm the byte order again; 
   * From the last test: the header data comes in big endian order; should switch the numbers in little endian here 
   */
  readData.header.V = ntohs(readData.header.V); // unsigned short V
  readData.header.TIME_STAMP = (((unsigned long long) ntohl(readData.header.TIME_STAMP)) << 32) 
                               + ntohl(readData.header.TIME_STAMP >> 32); // unsigned long long TIME_STAMP
  readData.header.BODY_SIZE = (((unsigned long long) ntohl(readData.header.BODY_SIZE)) << 32) 
                               + ntohl(readData.header.BODY_SIZE >> 32); // unsigned long long BODY_SIZE
  readData.header.CRC = (((unsigned long long) ntohl(readData.header.CRC)) << 32) 
                                     + ntohl(readData.header.CRC >> 32); // unsigned long long BODY_SIZE
  //cout << "V=" << readData.header.V;
  //cout << " TS=" << readData.header.TIME_STAMP;
  //cout << " BS=" << readData.header.BODY_SIZE;
  //cout << " CR=" << readData.header.CRC << endl;
  // the EE_POSITION_S seems to come in little endian order

#endif //if not __linux__

  if(readData.header.V == 1 && readData.header.BODY_SIZE == sizeof(EE_POSITION_S) ){  
    *data = readData.Pee;
    if(readData.Pee.alpha==1){
#ifndef __linux__ //if not __linux__
      semTake(Mutex, NO_WAIT);
#else  //if __linux__
      pthread_mutex_lock(&Mutex); 
      // TODO:  check if non-blocking use is what is wanted
      //if (pthread_mutex_trylock(&Mutex) != EBUSY) {
#endif //if not __linux__

      stateFlag = E_CTRL_RUN_START;

#ifndef __linux__ //if not __linux__
      semGive(Mutex);
#else  //if __linux__
      pthread_mutex_unlock(&Mutex); 
      //}
#endif //if not __linux__
    }
    else{
#ifndef __linux__ //if not __linux__
      semTake(Mutex, NO_WAIT);
#else  //if __linux__
      pthread_mutex_lock(&Mutex);
      // TODO: check if non-blocking use is what is wanted
      //if (pthread_mutex_trylock(&Mutex) != EBUSY) {
#endif //if not __linux__

      stateFlag = E_CTRL_RUN_STOP;

#ifndef __linux__ //if not __linux__
      semGive(Mutex);
#else  //if __linux__
      pthread_mutex_unlock(&Mutex);
      //}
#endif //if not __linux__
    }
  }
  
}

EVENT
PNTM_INTERFACE::eventRead(){
  EVENT flag = E_CTRL_RUN_STOP;
#ifndef __linux__ //if not __linux__
  semTake(Mutex, NO_WAIT);
#else  //if __linux__
  pthread_mutex_lock(&Mutex);
  // TODO: check if non-blocking use is what is wanted
  //if (pthread_mutex_trylock(&Mutex) != EBUSY) {
#endif //if not __linux__
  flag = stateFlag;
#ifndef __linux__ //if not __linux__
  semGive(Mutex);
#else  //if __linux__
  pthread_mutex_unlock(&Mutex);
  //}
#endif //if not __linux__
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
