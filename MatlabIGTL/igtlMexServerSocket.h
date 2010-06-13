/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlMexServerSocket.h $
  Language:  C++
  Date:      $Date: 2010-06-09 16:16:36 -0400 (Wed, 09 Jun 2010) $
  Version:   $Revision: 6525 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMexServerSocket.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/*=========================================================================

  igtlMexClientSocket class

  This is a modified version of igtlClientSocket class; the difference
  from original igtlClientSocket is that it allows to get and set socket
  descrptor. This allows opening, sending/receiving and closing socket
  in different MEX files.
=========================================================================*/

// .NAME igtlMexServerSocket - Encapsulate a socket that accepts connections.
// .SECTION Description
//

#ifndef __igtlMexServerSocket_h
#define __igtlMexServerSocket_h

#include "igtlMexSocket.h"
#include "igtlMexClientSocket.h"
#include "igtlWin32Header.h"

namespace igtl
{

class IGTLCommon_EXPORT MexServerSocket : public MexSocket
{
public:
  typedef MexServerSocket              Self;
  typedef MexSocket  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::MexServerSocket, igtl::MexSocket);
  igtlNewMacro(igtl::MexServerSocket);

  void PrintSelf(std::ostream& os);

  // Description:
  // Creates a server socket at a given port and binds to it.
  // Returns -1 on error. 0 on success.
  int CreateServer(int port);

  // Description:
  // Waits for a connection. When a connection is received
  // a new ClientSocket object is created and returned.
  // Returns NULL on timeout. 
  //ClientSocket* WaitForConnection(unsigned long msec=0);
  MexClientSocket::Pointer WaitForConnection(unsigned long msec=0);

  // Description:
  // Returns the port on which the server is running.
  int GetServerPort();

protected:
  MexServerSocket();
  ~MexServerSocket();


private:
  MexServerSocket(const MexServerSocket&); // Not implemented.
  void operator=(const MexServerSocket&); // Not implemented.
};

} // end of igtl namespace 


#endif

