//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrMessageServer.cpp,v $
// $Revision: 1.3 $ 
// $Author: junichi $
// $Date: 2006/01/20 03:15:48 $
//====================================================================


//====================================================================
// Description: 
//    Message interface for MRI guided robot control system.
//====================================================================

#include "MrsvrMessageServer.h"

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstring>


const char* MrsvrMessageServer::svrStatusStr[] = {
  "Sleeping",
  "Waiting",
  "Connected",
};

#define MSG_SVR_NUM_MODE  8

const char* MrsvrMessageServer::robotModeStr[] = {
  "STOP",
  "M.CLB",
  "A.CLB",
  "MOVETO",
  "PAUSE",
  "MANUAL",
  "REMOTE",
  "RESET",
};

MrsvrMessageServer::MrsvrMessageServer(int port) : MrsvrThread()
{
  this->port = port;
  init();
  pthread_mutex_init(&mtxCommand, NULL);

}


MrsvrMessageServer::~MrsvrMessageServer()
{
  
}

void MrsvrMessageServer::init()
{
  currentPos  = new MrsvrRASWriter(SHM_RAS_CURRENT);
  setPoint    = new MrsvrRASWriter(SHM_RAS_SETPOINT);
  robotStatus = new MrsvrStatusReader(SHM_STATUS);

  //masterBufferedFD = NULL;
  fSetTargetMatrix      = false;
  fSetCalibrationMatrix = false;
  nextRobotMode    = -1;

  this->connectionStatus =  SVR_STOP;
  this->fRunServer = 1;
}


void MrsvrMessageServer::process()
{
  this->fRunServer = 1;
  
  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  int r = serverSocket->CreateServer(port);

  // Change the status to "WAIT"
  this->connectionStatus = SVR_WAIT;

  if (r < 0)
    {
#ifdef DEBUG    
    perror("MrsvrMessageServer::process(): ERROR: Cannot create a server socket.");
#endif // DEBUG
    return;
    }

  //igtl::Socket::Pointer socket;

  while (this->fRunServer == 1) {
    //------------------------------------------------------------
    // Waiting for Connection
    socket = serverSocket->WaitForConnection(1000);

    if (socket.IsNotNull()) {// if client connected
#ifdef DEBUG_MRSVR_MESSAGE_SERVER
      printf("Master process connected to the robot module.\n");
      fflush(stdout);
#endif

      // Change the status to "CONNECTED"
      this->connectionStatus = SVR_CONNECTED;
      
      // Create a message buffer to receive header
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();
      
      //------------------------------------------------------------
      // loop
      while (this->fRunServer == 1) {
        
        // Initialize receive buffer
        headerMsg->InitPack();
        
        // Receive generic header from the socket
        int r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
        if (r == 0) {
          //------------------------------------------------------------
          // Close connection (The example code never reaches to this section ...)
          break;
        }
        if (r != headerMsg->GetPackSize()) {
          continue;
        }

        // Deserialize the header
        headerMsg->Unpack();

        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0){
          onRcvMsgMaster(socket, headerMsg);
        } else {
          // if the data type is unknown, skip reading.
          std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
        }
      }
      

      // Change the status to "WAIT"
      socket->CloseSocket();
      this->connectionStatus = SVR_WAIT;
    }
  }
  this->connectionStatus = SVR_STOP;

}


void MrsvrMessageServer::stop()
{
  this->fRunServer = 0;

  MrsvrThread::stop();
  /*
  fd_set readfds, testfds;
  for (int i = 0; i < FD_SETSIZE; i ++) {
    if (FD_ISSET(i, &testfds)) {
      close(i);
      FD_CLR(i, &readfds);
      if (i == masterSockFD) {
        masterSockFD = -1;
      }
    }
  }
  */

  init();
}


int MrsvrMessageServer::onRcvMsgMaster(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{

#ifdef DEBUG_MRSVR_MESSAGE_SERVER
  fprintf(stderr, "MrsvrMessageServer::onRcvMsgMaster():Receiving TRANSFORM data type.\n");
#endif
  
  // Create a message buffer to receive transform data
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetMessageHeader(header);
  transMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = transMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) { // if CRC check is OK
    // Retrive the transform data
    igtl::Matrix4x4 matrix;
    transMsg->GetMatrix(matrix);
    // Check the device name in the OpenIGTLink header
    if (strcmp(transMsg->GetDeviceName(), "TARGET") == 0 ||
        strcmp(transMsg->GetDeviceName(), "ProstateNavRobotTarg") == 0){
      setTargetMatrix(matrix);
      //if (result == TARGET_ACCEPTED) {
      //} else if (result == TARGET_OUT_OF_RANGE) {
      //}
    } else if (strcmp(transMsg->GetDeviceName(), "ZFrameTransform") == 0){
      setCalibrationMatrix(matrix);
    }

    
    return 1;
  }
  
  return 0;
}


int MrsvrMessageServer::getSvrStatus()
{
  return this->connectionStatus;
}


const char* MrsvrMessageServer::getSvrStatusStr()
{
  return svrStatusStr[this->connectionStatus];
}


bool MrsvrMessageServer::getTargetMatrix(Matrix4x4& matrix)
{
  pthread_mutex_lock(&mtxCommand);  
  if (fSetTargetMatrix == false) {
    pthread_mutex_unlock(&mtxCommand);  
    return false;
  }
  fSetTargetMatrix = false;
  for (int i = 0; i < 4; i ++) {
    for (int j = 0; j < 4; j ++) { 
      matrix[i][j] = targetMatrix[i][j];
    }
  }
  pthread_mutex_unlock(&mtxCommand);  
  return true;
}


bool MrsvrMessageServer::getCalibrationMatrix(Matrix4x4& matrix)
{
  pthread_mutex_lock(&mtxCommand);  
  if (fSetCalibrationMatrix == false) {
    pthread_mutex_unlock(&mtxCommand);  
    return false;
  }
  fSetCalibrationMatrix = false;
  for (int i = 0; i < 4; i ++) {
    for (int j = 0; j < 4; j ++) { 
      matrix[i][j] = calibrationMatrix[i][j];
    }
  }
  pthread_mutex_unlock(&mtxCommand);  
  return true;
}


bool MrsvrMessageServer::getMode(int* next)
{
  bool r;
  *next = -1;
  pthread_mutex_lock(&mtxCommand);  
  if (nextRobotMode < 0) {
    r = false;
  } else {
    r = true;
    *next = nextRobotMode;
  }
  pthread_mutex_unlock(&mtxCommand);  
  return r;
}

int MrsvrMessageServer::sendCurrentPosition(igtl::Matrix4x4& current)
{
  if (this->connectionStatus == SVR_CONNECTED)
    {
    igtl::TransformMessage::Pointer transMsg;
    transMsg = igtl::TransformMessage::New();
    transMsg->SetDeviceName("CURRENT");
    igtl::TimeStamp::Pointer ts;
    ts = igtl::TimeStamp::New();
    transMsg->SetMatrix(current);
    transMsg->SetTimeStamp(ts);
    transMsg->Pack();
    socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
    }
}

int MrsvrMessageServer::setTargetMatrix(igtl::Matrix4x4& matrix)
{
  pthread_mutex_lock(&mtxCommand);  
  for (int j = 0; j < 4; j ++) {
    for (int i = 0; i < 4; i ++) {
      targetMatrix[i][j] = matrix[i][j];
    }
  }
  fSetTargetMatrix = true;
  pthread_mutex_unlock(&mtxCommand);
  //
  // The received target should be validated based on
  // physical condition. This will be implemented in future.
  //
  return TARGET_ACCEPTED;
}


int MrsvrMessageServer::setCalibrationMatrix(igtl::Matrix4x4& matrix)
{
  pthread_mutex_lock(&mtxCommand);  
  for (int i = 0; i < 4; i ++) {
    for (int j = 0; j < 4; j ++) {
      calibrationMatrix[i][j] = matrix[i][j];
    }
  }
  fSetCalibrationMatrix = true;
  pthread_mutex_unlock(&mtxCommand);
  //
  // The received target should be validated based on
  // physical condition. This will be implemented in future.
  //
  return TARGET_ACCEPTED;
}


int MrsvrMessageServer::setMode(const char* param)
{
  int mode = -1;
  pthread_mutex_lock(&mtxCommand);
  for (int i = 0; i < MSG_SVR_NUM_MODE; i ++) {
    //fprintf(stderr, "MrsvrMessageServer::setMode(): compare %s vs %s",
    //MrsvrMessageServer::robotModeStr[i], param);
    if (strncmp(MrsvrMessageServer::robotModeStr[i], param, 
                strlen(MrsvrMessageServer::robotModeStr[i])) == 0) {
      mode = i;
      break;
    }
  }
  if (mode >= 0) {
    nextRobotMode = mode;
  }

  pthread_mutex_unlock(&mtxCommand);  

  return mode;
}


void MrsvrMessageServer::getRobotStatus(int* mode, int* outrange, int* lock)
{
  *mode     = robotStatus->getMode();
  *outrange = 0;
  for (int i = 0; i < NUM_ENCODERS; i ++) {
    if (robotStatus->getOutOfRange(i) != 0) {
      *outrange = 1;
    }
  }
  for (int i = 0; i < NUM_ACTUATORS; i ++) {
    if (robotStatus->getActuatorLockStatus(i)) {
      lock[i] = 1;
    } else {
      lock[i] = 0;
    }
  }
}

