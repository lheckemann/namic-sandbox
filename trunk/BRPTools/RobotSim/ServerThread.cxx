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
#include "Workphase.h"

#include "igtlServerSocket.h"
#include "igtlMultiThreader.h"
#include "igtlMutexLock.h"
#include "igtlObjectFactory.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlStatusMessage.h"

namespace igtl {

ServerThread::ServerThread() : Object()
{
  this->m_Thread      = MultiThreader::New();
  this->m_ThreadID    = -1;
  this->m_DataMutex   = MutexLock::New();
  this->m_SocketMutex = MutexLock::New();
  this->m_Port        = 18944;

  this->m_CurrentStatus    = 0;
  this->m_CurrentWorkphase = WP_STARTUP;

  igtl::IdentityMatrix(this->m_CurrentPosition);
  igtl::IdentityMatrix(this->m_TargetPosition);
}


ServerThread::~ServerThread()
{
}

int ServerThread::GetNextCommand()
{
  int cmd;
  
  this->m_DataMutex->Lock();
  if (this->m_CommandList.empty())
    {
    cmd = -1;
    }
  else
    {
    cmd = this->m_CommandList.front();
    this->m_CommandList.pop();
    }
  this->m_DataMutex->Unlock();

  return cmd;
}


int ServerThread::PushErrorMessage()
{
  return 1;
}

int ServerThread::SetCurrentStatus()
{
  return 1;
}

int ServerThread::SetCurrentWorkphase(int workphase)
{
  this->m_CurrentWorkphase = workphase;
  return workphase;
}

int ServerThread::GetCurrentWorkphase()
{
  return this->m_CurrentWorkphase;
}

int ServerThread::SetNextWorkphase(int workphase)
{
  this->m_NextWorkphase = workphase;
  return workphase;
}

int ServerThread::GetNextWorkphase()
{
  return this->m_NextWorkphase;
}

int ServerThread::SetCurrentPosition(const Matrix4x4& matrix)
{
  this->m_DataMutex->Lock();

  this->m_CurrentPosition[0][0] = matrix[0][0];
  this->m_CurrentPosition[1][0] = matrix[1][0];
  this->m_CurrentPosition[2][0] = matrix[2][0];
  this->m_CurrentPosition[3][0] = matrix[3][0];
  this->m_CurrentPosition[0][1] = matrix[0][1];
  this->m_CurrentPosition[1][1] = matrix[1][1];
  this->m_CurrentPosition[2][1] = matrix[2][1];
  this->m_CurrentPosition[3][1] = matrix[3][1];
  this->m_CurrentPosition[0][2] = matrix[0][2];
  this->m_CurrentPosition[1][2] = matrix[1][2];
  this->m_CurrentPosition[2][2] = matrix[2][2];
  this->m_CurrentPosition[3][2] = matrix[3][2];
  this->m_CurrentPosition[0][3] = matrix[0][3];
  this->m_CurrentPosition[1][3] = matrix[1][3];
  this->m_CurrentPosition[2][3] = matrix[2][3];
  this->m_CurrentPosition[3][3] = matrix[3][3];

  this->m_DataMutex->Unlock();

  return 1;
}


int ServerThread::GetCurrentPosition(Matrix4x4& matrix)
{
  this->m_DataMutex->Lock();

  matrix[0][0] = this->m_CurrentPosition[0][0];
  matrix[1][0] = this->m_CurrentPosition[1][0];
  matrix[2][0] = this->m_CurrentPosition[2][0];
  matrix[3][0] = this->m_CurrentPosition[3][0];
  matrix[0][1] = this->m_CurrentPosition[0][1];
  matrix[1][1] = this->m_CurrentPosition[1][1];
  matrix[2][1] = this->m_CurrentPosition[2][1];
  matrix[3][1] = this->m_CurrentPosition[3][1];
  matrix[0][2] = this->m_CurrentPosition[0][2];
  matrix[1][2] = this->m_CurrentPosition[1][2];
  matrix[2][2] = this->m_CurrentPosition[2][2];
  matrix[3][2] = this->m_CurrentPosition[3][2];
  matrix[0][3] = this->m_CurrentPosition[0][3];
  matrix[1][3] = this->m_CurrentPosition[1][3];
  matrix[2][3] = this->m_CurrentPosition[2][3];
  matrix[3][3] = this->m_CurrentPosition[3][3];

  this->m_DataMutex->Unlock();

  return 1;
}


int ServerThread::SetCurrentPosition(float* position, float* quaternion)
{
  // substitute current position
  this->m_DataMutex->Lock();

  QuaternionToMatrix(quaternion, this->m_CurrentPosition);

  this->m_CurrentPosition[0][3] = position[0];
  this->m_CurrentPosition[1][3] = position[1];
  this->m_CurrentPosition[2][3] = position[2];

  this->m_DataMutex->Unlock();

  return 1;
}


int ServerThread::GetCurrentPosition(float* position, float* quaternion)
{
  this->m_DataMutex->Lock();

  position[0] = this->m_CurrentPosition[0][3];
  position[1] = this->m_CurrentPosition[1][3];
  position[2] = this->m_CurrentPosition[2][3];

  MatrixToQuaternion(this->m_CurrentPosition, quaternion);

  this->m_DataMutex->Unlock();

  return 1;
}


int ServerThread::SetTargetPosition(const Matrix4x4& matrix)
{
  this->m_DataMutex->Lock();

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

  this->m_DataMutex->Unlock();
  return 1;
}


int ServerThread::GetTargetPosition(Matrix4x4& matrix)
{
  this->m_DataMutex->Lock();

  matrix[0][0] = this->m_TargetPosition[0][0];
  matrix[1][0] = this->m_TargetPosition[1][0];
  matrix[2][0] = this->m_TargetPosition[2][0];
  matrix[3][0] = this->m_TargetPosition[3][0];
  matrix[0][1] = this->m_TargetPosition[0][1];
  matrix[1][1] = this->m_TargetPosition[1][1];
  matrix[2][1] = this->m_TargetPosition[2][1];
  matrix[3][1] = this->m_TargetPosition[3][1];
  matrix[0][2] = this->m_TargetPosition[0][2];
  matrix[1][2] = this->m_TargetPosition[1][2];
  matrix[2][2] = this->m_TargetPosition[2][2];
  matrix[3][2] = this->m_TargetPosition[3][2];
  matrix[0][3] = this->m_TargetPosition[0][3];
  matrix[1][3] = this->m_TargetPosition[1][3];
  matrix[2][3] = this->m_TargetPosition[2][3];
  matrix[3][3] = this->m_TargetPosition[3][3];

  this->m_DataMutex->Unlock();
  
  return 1;
}


int ServerThread::SetTargetPosition(float* position, float* quaternion)
{
  this->m_DataMutex->Lock();

  QuaternionToMatrix(quaternion, this->m_TargetPosition);

  this->m_TargetPosition[0][3] = position[0];
  this->m_TargetPosition[1][3] = position[1];
  this->m_TargetPosition[2][3] = position[2];

  this->m_DataMutex->Unlock();

  return 1;

}


int ServerThread::GetTargetPosition(float* position, float* quaternion)
{
  this->m_DataMutex->Lock();

  position[0] = this->m_TargetPosition[0][3];
  position[1] = this->m_TargetPosition[1][3];
  position[2] = this->m_TargetPosition[2][3];

  MatrixToQuaternion(this->m_TargetPosition, quaternion);

  this->m_DataMutex->Unlock();

  return 1;
}


int ServerThread::Start()
{
  // if thread is already running
  if (this->m_ThreadID >= 0)
    {
    return 0;
    }
  this->m_ServerStopFlag = 0;
  this->m_ThreadID = this->m_Thread->SpawnThread((igtlThreadFunctionType) &ServerThread::ThreadFunction, this);

  return 1;
}


int ServerThread::Stop()
{
  if (this->m_ThreadID >= 0)
    {
    this->m_ServerStopFlag = true;
    this->m_SocketMutex->Lock();
    if (this->m_Socket)
      {
      this->m_Socket->CloseSocket();
      }
    this->m_SocketMutex->Unlock();
    return 1;
    }
  else
    {
    return 0;
    }
}


void* ServerThread::ThreadFunction(void* ptr)
{
  MultiThreader::ThreadInfo* tinfo = 
    static_cast<MultiThreader::ThreadInfo*>(ptr);
  ServerThread* pst = static_cast<ServerThread*>(tinfo->UserData);

  while (!pst->m_ServerStopFlag)
    {
    ServerSocket::Pointer serverSocket;
    serverSocket = ServerSocket::New();
    int port = pst->m_Port;
    serverSocket->CreateServer(port);
    
    //------------------------------------------------------------
    // Waiting for Connection
    pst->m_Socket = serverSocket->WaitForConnection(1000);
    
    if (pst->m_Socket.IsNotNull()) // if client connected
      {
      // Create a message buffer to receive header
      MessageHeader::Pointer headerMsg;
      headerMsg = MessageHeader::New();

      //------------------------------------------------------------
      // loop
      while (!pst->m_ServerStopFlag)
        {
        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the socket
        int r = pst->m_Socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
        if (r != headerMsg->GetPackSize())
          {
          std::cerr << "Error: receiving data." << std::endl;
          }

        // Deserialize the header
        headerMsg->Unpack();

        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
          {
          pst->ReceiveTransform(pst->m_Socket, headerMsg);
          }
        //else if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
        //  {
        //  ReceiveImage(socket, headerMsg);
        //  }
        else if (strcmp(headerMsg->GetDeviceType(), "STATUS") == 0)
          {
          //pst->ReceiveStatus(pst->m_Socket, headerMsg);
          }
        else
          {
          // if the data type is unknown, skip reading.
          pst->m_Socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          }
        }
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  pst->m_Socket->CloseSocket();

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

    this->m_DataMutex->Lock();

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

    this->m_DataMutex->Unlock();
    
    return 1;
    }

  return 0;

}


int ServerThread::ReceivePosition(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving POSITION data type." << std::endl;
  
  // Create a message buffer to receive transform data
  igtl::PositionMessage::Pointer positionMsg;
  positionMsg = igtl::PositionMessage::New();
  positionMsg->SetMessageHeader(header);
  positionMsg->AllocatePack();
  
  // Receive position position data from the socket
  socket->Receive(positionMsg->GetPackBodyPointer(), positionMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = positionMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    // Retrive the transform data
    float position[3];
    float quaternion[4];

    positionMsg->GetPosition(position);
    positionMsg->GetQuaternion(quaternion);

    std::cerr << "position   = (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
    std::cerr << "quaternion = (" << quaternion[0] << ", " << quaternion[1] << ", "
              << quaternion[2] << ", " << quaternion[3] << ")" << std::endl << std::endl;

    return 1;
    }

  return 0;

}


int ReceiveStatus(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving STATUS data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::StatusMessage::Pointer statusMsg;
  statusMsg = igtl::StatusMessage::New();
  statusMsg->SetMessageHeader(header);
  statusMsg->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(statusMsg->GetPackBodyPointer(), statusMsg->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = statusMsg->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    std::cerr << "========== STATUS ==========" << std::endl;
    std::cerr << " Code      : " << statusMsg->GetCode() << std::endl;
    std::cerr << " SubCode   : " << statusMsg->GetSubCode() << std::endl;
    std::cerr << " Error Name: " << statusMsg->GetErrorName() << std::endl;
    std::cerr << " Status    : " << statusMsg->GetStatusString() << std::endl;
    std::cerr << "============================" << std::endl;
    }

  return 0;

}


int ServerThread::SendCurrentPosition()
{
  int r;

  // prepare transform message
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetDeviceName("Robot");

  this->m_DataMutex->Lock();
  transMsg->SetMatrix(this->m_CurrentPosition);
  this->m_DataMutex->Unlock();

  // send data
  this->m_SocketMutex->Lock();
  if (this->m_Socket->GetConnected())
    {
    r = this->m_Socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
    }
  this->m_SocketMutex->Unlock();

  return r;

}


}




