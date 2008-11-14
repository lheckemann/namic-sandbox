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

#ifndef __ServerThread_h
#define __ServerThread_h

#include <vector>
#include <queue>

#include "igtlMath.h"
#include "igtlSocket.h"
#include "igtlMutexLock.h"
#include "igtlMultiThreader.h"
#include "igtlWin32Header.h"
#include "igtlMessageHeader.h"

namespace igtl
{

class IGTLCommon_EXPORT ServerThread : public  Object
{
 public:

  typedef std::queue<int> CommandListType; 
  typedef std::queue<int> ErrorListType;

 public:

  /** Standard class typedefs. */
  typedef ServerThread              Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  igtlNewMacro(Self);  
  igtlTypeMacro(MultiThreader, Object);
  
  void PrintSelf(std::ostream& os);
  
 protected:

  ServerThread();
  ~ServerThread();


 public:
  void SetServerPort(int p) { this->m_Port = p; };
  int  GetNextCommand();
  int  PushErrorMessage();
  int  SetCurrentStatus();

  int  SetCurrentPosition(const Matrix4x4& matrix);
  int  GetCurrentPosition(Matrix4x4& matrix);
  int  SetCurrentPosition(float* position, float* quaternion);
  int  GetCurrentPosition(float* position, float* quaternion);

  int  SetTargetPosition(const Matrix4x4& matrix);
  int  GetTargetPosition(Matrix4x4& matrix);
  int  SetTargetPosition(float* position, float* quaternion);
  int  GetTargetPosition(float* position, float* quaternion);

  int  SetCurrentWorkphase(int workphase);
  int  GetCurrentWorkphase();
  int  SetNextWorkphase(int workphase);
  int  GetNextWorkphase();

  int  Start();
  int  Stop();

  static void* ThreadFunction(void* ptr);
  int  ReceiveTransform(Socket::Pointer& socket, MessageHeader::Pointer& header);
  int  ReceivePosition(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header);
  // int  ReceiveStatus(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header);
  
  int  SendCurrentPosition();
  int  SendCurrentWorkphase();

  //int  WaitForClient(int port);
  //int  GetCommandList(CommandListType* comList);

private:
  
  MultiThreader::Pointer m_Thread;
  Socket::Pointer        m_Socket;
  MutexLock::Pointer     m_DataMutex;
  MutexLock::Pointer     m_SocketMutex;
  int                    m_ThreadID;

  int                    m_Port;            // port number

  CommandListType        m_CommandList;     // List of commands received
  ErrorListType          m_ErrorList;       // List of errors to send
  int                    m_CurrentStatus;   // Current status
  Matrix4x4              m_TargetPosition;  // Target position
  Matrix4x4              m_CurrentPosition; // Current position

  int                    m_NextWorkphase;   // Next workphase
  int                    m_CurrentWorkphase;// Current workphase

  int                    m_ServerStopFlag;

};

}

#endif
