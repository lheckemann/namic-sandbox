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
#include "igtlMultiThreader.h"
#include "igtlWin32Header.h"

namespace igtl
{


class IGTLCommon_EXPORT ServerThread : public : OpenIGTLink
{
 public:
  typedef queue<int> CommandListType; 
  typedef queue<int> ErrorListType;

 public:
  /** Standard class typedefs. */
  typedef MultiThreader             Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  igtlNewMacro(Self);  
  igtlTypeMacro(MultiThreader, Object);
  
  void PrintSelf(std::ostream& os);
  
 protected:
  ServerThread();
  ~ServerThread();

  int  Run();
  int  Stop();

  int  WaitForClient(int port);
  int  GetTarget();
  int  GetCommandList(CommandListType* comList);

  int  SendPosition();
  int  SendStatus();
  int  SendError();

private:
  
  MultiThreader::Pointer m_Thread;
  Socket::Pointer        m_Socket;

  CommandListType m_CommandList;     // List of commands received
  ErrorListType   m_ErrorList;       // List of errors to send
  int             m_CurrentStatus;   // Current status
  Matrix4x4       m_TargetPosition;  // Target position
  Matrix4x4       m_CurrentPosition; // Current position

  int             m_ServerStopFlag;
  

};

}

#endif
