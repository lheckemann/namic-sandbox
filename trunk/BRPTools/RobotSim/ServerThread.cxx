/*=========================================================================

  Program:   Simple Robot Simulator / Server Thread class
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "ServerThread.h"

#include "igtlMultiThreader.h"
#include "igtlMutexLock.h"
#include "igtlObjectFactory.h"

namespace igtl {

ServerThread::ServerThread() : Object()
{
  this->m_Thread = new MultiThreader();
  this->m_ThreadID = -1;
}

ServerThread::~ServerThread()
{
}

int ServerThread::Run()
{
  // if thread is already running
  if (this->m_ThreadID >= 0)
    {
    return 0;
    }

  this->m_ServerStopFlag = 0;
  this->ThreadID = this->Thread->SpawnThread((igtlThreadFunctionType) &ServerThread::ThreadFunction, this);
  return 1;
}


int  ServerThread::Stop()
{
  if (this->ThreadID >= 0)
    {
    this->m_ServerStopFlag = true;
    this->m_Mutex->Lock();
    if (this->m_Socket)
      {
      this->m_Socket->CloseSocket();
      }
    this->Mutex->Unlock();
    return 1;
    }
  else
    {
    return 0;
    }
}

int  ServerThread::ThreadFunction()
{
  while (!this->m_ServerStopFlag)
    {
    ServerSocket::Pointer serverSocket;
    serverSocket = ServerSocket::New();
    int port = this->m_Port;
    serverSocket->CreateServer(port);
    
    //------------------------------------------------------------
    // Waiting for Connection
    this->m_Socket = serverSocket->WaitForConnection(1000);
    
    if (socket.IsNotNull()) // if client connected
      {
      // Create a message buffer to receive header
      MessageHeader::Pointer headerMsg;
      headerMsg = MessageHeader::New();

      //------------------------------------------------------------
      // loop
      while (!this->m_ServerStopFlag)
        {
        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the socket
        int r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
        if (r != headerMsg->GetPackSize())
          {
          std::cerr << "Error: receiving data." << std::endl;
          }

        // Deserialize the header
        headerMsg->Unpack();

        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
          {
          ReceiveTransform(socket, headerMsg);
          }
        else if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
          {
          ReceiveImage(socket, headerMsg);
          }
        else if (strcmp(headerMsg->GetDeviceType(), "STATUS") == 0)
          {
          ReceiveStatus(socket, headerMsg);
          }
        else
          {
          // if the data type is unknown, skip reading.
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          }
        }
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  socket->CloseSocket();

}


int ServerThread::ReceiveTransform(Socket::Pointer& socket, MessageHeader::Pointer& header)
{
  std::cerr << "Receiving TRANSFORM data type." << std::endl;
  
  // Create a message buffer to receive transform data
  TransformMessage::Pointer transMsg;
  transMsg = TransformMessage::New();
  transMsg->SetMessageHeader(header);
  transMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = transMsg->Unpack(1);
  
  if (c & MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the transform data
    Matrix4x4 matrix;
    transMsg->GetMatrix(matrix);
    PrintMatrix(matrix);

    this->m_TargetPosition[0][0] = matrix[0][0];
    this->m_TargetPosition[1][0] = matrix[1][0];
    this->m_TargetPosition[2][0] = matrix[2][0];
    this->m_TargetPosition[3][0] = matrix[3][0];
    this->m_TargetPosition[0][1] = matrix[0][1];
    this->m_TargetPosition[1][1] = matrix[1][1];
    this->m_TargetPosition[2][1] = matrix[2][1];
    this->m_TargetPosition[3][1] = matrix[3][1];
    this->m_TargetPosition[0][2] = matrix[0][2];
    this->m_TargetPosition[1][2] = matrix[1][2];
    this->m_TargetPosition[2][2] = matrix[2][2];
    this->m_TargetPosition[3][2] = matrix[3][2];
    this->m_TargetPosition[0][3] = matrix[0][3];
    this->m_TargetPosition[1][3] = matrix[1][3];
    this->m_TargetPosition[2][3] = matrix[2][3];
    this->m_TargetPosition[3][3] = matrix[3][3];

    return 1;
    }

  return 0;

}



}




