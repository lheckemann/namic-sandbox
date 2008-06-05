/***************************************************************************
 * FileName      : interface.cpp
 * Created       : 2007/11/21
 * LastModified  : 2007/
 * Author        : Hiroaki KOZUKA jumpei arata
 * Aim           : Conect other layer and core layer
 *                 Interface class
 * OS            : VxWorks 5.5.1
 ***************************************************************************/
#include "interface.h"

IF_DATA INTERFACE::IF_Data;
int INTERFACE::writeStatFlag;
int INTERFACE::readStatFlag;
IGTL_POSITION INTERFACE::writeData;
IGTL_POSITION INTERFACE::readData;

INTERFACE*
INTERFACE::createInterface(char *myName, char* yourName, int outPort, int inPort){
    if(IF_Instance ==NULL){
    //IF_Data.serverClient = CLIENT;
        IF_Data.hostName = yourName;
        IF_Data.outPortNum = outPort;
        IF_Data.inPortNum = inPort;
        IF_Data.packetSize = sizeof(IGTL_POSITION);
        IF_Data.packetNum = 50;
        IF_Data.buffSize = IF_Data.packetSize * IF_Data.packetNum;

        // IGTLink Protocol
        writeData.header.V = htons(0);
        char dataType[] = "position";
        int num = sizeof(dataType);
        for(int i=0; i<num; i++){
            writeData.header.TYPE[i] = dataType[i];
        }
        num = sizeof(myName);
        for(int i=0; i<num; i++){
            writeData.header.DEVICE_NAME[i] = myName[i];
        }
        writeData.header.TIME_STAMP = (unsigned long long)0;
        writeData.header.BODY_SIZE = (unsigned long long)htonll(sizeof(EE_POSITION_S));
        writeData.header.CRC = (unsigned long long)0;
        IF_Instance = new INTERFACE();
    }
    int data = 12345;
    logMsg("Data:%X",htonll((unsigned long long)data),0,0,0,0,0);
    return IF_Instance;
}

unsigned long long
INTERFACE::htonll(unsigned long long data){
#if defined BIG__ENDIAN
    return (data);
#else
    unsigned long long swapData = 0;
    cout<<"datalong: "<<data<<endl;
    for(int n=0; n<4; n++){
        swapData += ( 0x00000000000000FF<<(8*n) & data>>( 8*(7-2*n) ) );
        cout<<"data"<<n<<": "<<swapData<<endl;
    }
    for(int n=4; n<8; n++){
        swapData += ( 0x00000000000000FF<<(8*n) & data<<( 8*(2*n-7) ) );
        cout<<"data"<<n<<": "<<swapData<<endl;
    }
    return(swapData);
#endif
}


#if defined IGTL__
INTERFACE::INTERFACE():IF_serverSock(SERVER, IF_Data.hostName, IF_Data.inPortNum),
                       IF_clientSock(CLIENT, IF_Data.hostName, IF_Data.outPortNum){
#elif defined TCP_SOCK
INTERFACE::INTERFACE():IF_serverSock(IF_Data.inPortNum){
    
}
INTERFACE::INTERFACE():IF_clientSock(IF_Data.hostName, IF_Data.outPortNum){
    
}
 
 
#elif defined UDP_SOCK
INTERFACE::INTERFACE():IF_serverSock(SERVER, IF_Data.hostName, IF_Data.inPortNum),
                       IF_clientSock(CLIENT, IF_Data.hostName, IF_Data.outPortNum){
    //logMsg("Initialize Interface Class...\n",0,0,0,0,0,0);
    cout<<"Initialize Interface Class..."<<endl;
    bzero( &IF_Data , sizeof(IF_Data));
    writeStatFlag = 0;
    readStatFlag = 0;
    //semaphore
    exclusMutex = semMCreate(SEM_Q_FIFO);
    wMutex = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    
    //create messege Q
    IF_R_MsgQID = msgQCreate( IF_Data.buffSize, IF_Data.buffSize, MSG_Q_FIFO);
    
    //task
    kernelTimeSlice(1);
    taskSpawn("tIFInSockTask", 100, VX_FP_TASK, 50000, (FUNCPTR)read,(int)this,
          (int)&IF_serverSock, (int)&IF_R_MsgQID,(int)&rMutex,0,0,0,0,0,0);
    taskSpawn("tIFOutSockTask", 100, VX_FP_TASK, 50000, (FUNCPTR)write,
             (int)this, (int)&IF_clientSock, (int)&wMutex,0,0,0,0,0,0,0);
    //logMsg("Interface Done.\n",0,0,0,0,0,0);
    cout<<"Interface done."<<endl;
    start(1);
    start(0);
    cout<<"Interface Done."<<endl;
}
#endif

INTERFACE::~INTERFACE(){
    bzero( &IF_Data , sizeof(IF_Data));
    writeStatFlag = 0;
    readStatFlag = 0;
    semDelete(exclusMutex);
    msgQDelete(IF_R_MsgQID);
    taskDelete(taskNameToId("tIFInSockTask"));
    taskDelete(taskNameToId("tIFOutSockTask"));
    //logMsg("End Interface.\n",0,0,0,0,0,0);
    cout<<"End Interface."<<endl;
}

void
INTERFACE::read(void* thisFunc, void* sock, void* msgQ, void* rMutex){
    int num;
    char buff[IF_Data.buffSize];
#if defined IGTL__
    SOCK *readSock = (SOCK*) sock;
#elif defined TCP_SOCK
    SOCK *readSock = (SOCK*) sock;
#elif defined UDP_SOCK
    udpSOCK *readSock = (udpSOCK*) sock;
#endif
    MSG_Q_ID *IF_MsgQID = (MSG_Q_ID*) msgQ;
    //SEM_ID* rMutex = (SEM_ID*) mutex;
    while(true){
    if(readStatFlag){
        num = recv(readSock->getSd(), buff, IF_Data.packetSize, MSG_PEEK);
            //if(num>)
        //read data from socket
        readSock->Read(buff, IF_Data.packetSize);
        //write data to msgQ
        msgQSend(*IF_MsgQID, buff, IF_Data.packetSize, WAIT_FOREVER, MSG_PRI_NORMAL);
    }
    else{
        
    }
    }
}

void
INTERFACE::write(void* thisFunc, void* sock, void* mutex){
    //
#if defined IGTL__
    SOCK *writeSock = (SOCK*) sock;
#elif defined TCP_SOCK
    SOCK *writeSock = (SOCK*) sock;
#elif defined UDP_SOCK
    udpSOCK *writeSock = (udpSOCK*) sock;
#endif
    SEM_ID* wMutex = (SEM_ID*) mutex;
    int err;
    while(true){
        if(writeStatFlag){
            semTake( *wMutex, WAIT_FOREVER);
            err = writeSock->Write( (char*)&writeData, IF_Data.packetSize);
        }
        else{
            //semTake( *wMutex, WAIT_FOREVER);
        }
    }
}

void
INTERFACE::start(int R_W){
    //sem flag
    semTake(exclusMutex, WAIT_FOREVER);
    if(R_W){
        readStatFlag = START;
        //semGive( wMutex );
    }
    else{
    //semGive( wMutex );
        writeStatFlag = START;
    }
    semGive(exclusMutex);
}

void
INTERFACE::stop(int R_W){
    //sem flag
    semTake(exclusMutex, WAIT_FOREVER);
    if(R_W) readStatFlag = STOP;
    else    writeStatFlag = STOP;
    semGive( exclusMutex );
}

void
INTERFACE::send(EE_POSITION_S* data){
    writeData.body = *data;
    semGive( wMutex );
}

void
INTERFACE::recieve(EE_POSITION_S* data){
    msgQReceive( IF_R_MsgQID, (char*)&readData, IF_Data.packetSize, NO_WAIT );
    *data = readData.body;
}

