/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkClientSocket.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME igtlClientSocket - Encapsulates a client socket.



/*=========================================================================

  igtlMexClientSocket class

  This is a modified version of igtlClientSocket class; the difference
  from original igtlClientSocket is that it allows to get and set socket
  descrptor. This allows opening, sending/receiving and closing socket
  in different MEX files.
=========================================================================*/


#ifndef __igtlMexClientSocket_h
#define __igtlMexClientSocket_h

#include "igtlMexSocket.h"
#include "igtlWin32Header.h"

namespace igtl
{

class ServerSocket;

class IGTLCommon_EXPORT MexClientSocket : public MexSocket
{
public:
  typedef MexClientSocket              Self;
  typedef MexSocket                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::MexClientSocket, igtl::MexSocket)
  igtlNewMacro(igtl::MexClientSocket);

  void PrintSelf(std::ostream& os);

  // Description:
  // Connects to host. Returns 0 on success, -1 on error.
  int ConnectToServer(const char* hostname, int port); 

  int GetDescriptor() {return this->m_SocketDescriptor; };

  int SetDescriptor(int sd);
  
protected:
  MexClientSocket();
  ~MexClientSocket();

//BTX
  friend class ServerSocket;
//ETX
private:
  MexClientSocket(const MexClientSocket&); // Not implemented.
  void operator=(const MexClientSocket&); // Not implemented.
};

}

#endif

