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

#ifndef __igtlMexClientSocket_h
#define __igtlMexClientSocket_h

#include "igtlSocket.h"
#include "igtlWin32Header.h"

namespace igtl
{

class ServerSocket;

class IGTLCommon_EXPORT MexClientSocket : public Socket
{
public:
  typedef MexClientSocket              Self;
  typedef Socket                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::MexClientSocket, igtl::Socket)
  igtlNewMacro(igtl::MexClientSocket);

  void PrintSelf(std::ostream& os);

  // Description:
  // Connects to host. Returns 0 on success, -1 on error.
  int ConnectToServer(const char* hostname, int port); 
  
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

