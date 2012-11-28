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
  Module:    $RCSfile: igtlSocket.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*=========================================================================

  Program:   Open ITK Link Library
  Module:    $RCSfile: itkImage.h,v $
  Language:  C++
  Date:      $Date: 2008/01/13 19:48:38 $
  Version:   $Revision: 1.142 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlMexSocket.h"

#if defined(_WIN32) && !defined(__CYGWIN__)
  #include <windows.h>
  #include <winsock2.h> 
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <sys/time.h>
  #include <string.h>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#define WSA_VERSION MAKEWORD(1,1)
#define igtlCloseSocketMacro(sock) (closesocket(sock))
#else
#define igtlCloseSocketMacro(sock) (close(sock))
#endif

namespace igtl
{

//-----------------------------------------------------------------------------
MexSocket::MexSocket()
{
  this->m_SocketDescriptor = -1;
  this->m_SendTimeoutFlag = 0;
  this->m_ReceiveTimeoutFlag = 0;
}

//-----------------------------------------------------------------------------
MexSocket::~MexSocket()
{
  // MexSocket class does not close the socket when it is destroyed
  // so that socket can exist after exiting the Mex function.
  /*
  if (this->m_SocketDescriptor != -1)
    {
    this->CloseSocket(this->m_SocketDescriptor);
    this->m_SocketDescriptor = -1;
    }
  */
}

//-----------------------------------------------------------------------------
int MexSocket::CreateSocket()
{
#if defined(_WIN32) && !defined(__CYGWIN__)
  // Declare variables
  WSADATA wsaData;
  /* currently not used.
  SOCKET ListenSocket;
  sockaddr_in service; */

  //---------------------------------------
  // Initialize Winsock
  int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
  if( iResult != NO_ERROR )
    {
    return -1;
    }
#endif

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  // Elimate windows 0.2 second delay sending (buffering) data.
  int on = 1;
  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on)))
    {
    return -1;
    }
  return sock;
}

//-----------------------------------------------------------------------------
int MexSocket::BindSocket(int socketdescriptor, int port)
{
  struct sockaddr_in server;

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  // Allow the socket to be bound to an address that is already in use
  int opt=1;
#ifdef _WIN32
  setsockopt(socketdescriptor, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(int));
#elif defined(VTK_HAVE_SO_REUSEADDR)
  setsockopt(socketdescriptor, SOL_SOCKET, SO_REUSEADDR, (void *) &opt, sizeof(int));
#endif

  if ( bind(socketdescriptor, reinterpret_cast<sockaddr*>(&server), sizeof(server)) )
    {
    return -1;
    }
  return 0;
}

//-----------------------------------------------------------------------------
int MexSocket::Accept(int socketdescriptor)
{
  if (socketdescriptor < 0)
    {
    return -1;
    }
  return accept(socketdescriptor, 0, 0);
}

//-----------------------------------------------------------------------------
int MexSocket::Listen(int socketdescriptor)
{
  if (socketdescriptor < 0)
    {
    return -1;
    }
  return listen(socketdescriptor, 1);
}

//-----------------------------------------------------------------------------
int MexSocket::SelectSocket(int socketdescriptor, unsigned long msec)
{
  if (socketdescriptor < 0 )
    {
    // invalid socket descriptor.
    return -1;
    }
  
  fd_set rset;
  struct timeval tval;
  struct timeval* tvalptr = 0;
  if ( msec > 0 )
    {
    tval.tv_sec = msec / 1000;
    tval.tv_usec = (msec % 1000)*1000;
    tvalptr = &tval;
    }
  FD_ZERO(&rset);
  FD_SET(socketdescriptor, &rset);
  int res = select(socketdescriptor + 1, &rset, 0, 0, tvalptr);
  if(res == 0)
    {
    return 0;//for time limit expire
    }

  if ( res < 0 || !(FD_ISSET(socketdescriptor, &rset)) )
    {
    // Some error.
    return -1;
    }
  // The indicated socket has some activity on it.
  return 1;
}

//-----------------------------------------------------------------------------
int MexSocket::SelectSockets(const int* sockets_to_select, int size,
    unsigned long msec, int* selected_index)
{
  int i;
  int max_fd = -1;
  *selected_index = -1;
  if (size <  0)
    {
    return -1;
    }
  
  fd_set rset;
  struct timeval tval;
  struct timeval* tvalptr = 0;
  if ( msec > 0 )
    {
    tval.tv_sec = msec / 1000;
    tval.tv_usec = msec % 1000;
    tvalptr = &tval;
    }
  FD_ZERO(&rset);
  for (i=0; i<size; i++)
    {
    FD_SET(sockets_to_select[i],&rset);
    max_fd = (sockets_to_select[i] > max_fd)? sockets_to_select[i] : max_fd;
    }
  
  int res = select(max_fd + 1, &rset, 0, 0, tvalptr);
  if (res == 0)
    {
    return 0; //Timeout
    }
  if (res < 0)
    {
    // SelectSocket error.
    return -1;
    }
  
  //check which socket has some activity.
  for (i=0; i<size; i++)
    {
    if ( FD_ISSET(sockets_to_select[i],&rset) )
      {
      *selected_index = i;
      return 1;
      }
    }
  return -1; 
}

//-----------------------------------------------------------------------------
int MexSocket::Connect(int socketdescriptor, const char* hostName, int port)
{
  if (socketdescriptor < 0)
    {
    return -1;
    }

  struct hostent* hp;
  hp = gethostbyname(hostName);
  if (!hp)
    {
    unsigned long addr = inet_addr(hostName);
    hp = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
    }
 
  if (!hp)
    {
    // vtkErrorMacro("Unknown host: " << hostName);
    return -1;
    }

  struct sockaddr_in name;
  name.sin_family = AF_INET;
  memcpy(&name.sin_addr, hp->h_addr, hp->h_length);
  name.sin_port = htons(port);

  return connect(socketdescriptor, reinterpret_cast<sockaddr*>(&name), 
    sizeof(name));
}

//-----------------------------------------------------------------------------
int MexSocket::GetPort(int sock)
{
  struct sockaddr_in sockinfo;
  memset(&sockinfo, 0, sizeof(sockinfo));
#if defined(VTK_HAVE_GETSOCKNAME_WITH_SOCKLEN_T)
  socklen_t sizebuf = sizeof(sockinfo);
#else
  int sizebuf = sizeof(sockinfo);
#endif
//  FIXME: Setup configuration for VTK_HAVE_GETSOCKNAME_WITH_SOCKLEN_T so we can uncomment these lines
//  if(getsockname(sock, reinterpret_cast<sockaddr*>(&sockinfo), &sizebuf) != 0)
//    {
//    return 0;
//    }
  return ntohs(sockinfo.sin_port);
}

//-----------------------------------------------------------------------------
void MexSocket::CloseSocket(int socketdescriptor)
{
  if (socketdescriptor < 0)
    {
    return;
    }
  igtlCloseSocketMacro(socketdescriptor);
}

//-----------------------------------------------------------------------------
int MexSocket::Send(const void* data, int length)
{
  if (!this->GetConnected())
    {
    return 0;
    }
  if (length == 0)
    {
    // nothing to send.
    return 1;
    }
  const char* buffer = reinterpret_cast<const char*>(data);
  int total = 0;
  do
    {
    int flags;
#if defined(_WIN32) && !defined(__CYGWIN__)
    flags = 0;
#else
    // disabling, since not present on SUN.
    // flags = MSG_NOSIGNAL; //disable signal on Unix boxes.
    flags = 0;
#endif
    int n = send(this->m_SocketDescriptor, buffer+total, length-total, flags);
    if(n < 0)
      {
      // FIXME : Use exceptions ?  igtlErrorMacro("Socket Error: Send failed.");
      return 0;
      }
    total += n;
    } while(total < length);
  return 1;
}

//-----------------------------------------------------------------------------
int MexSocket::Receive(void* data, int length, int readFully/*=1*/)
{
  if (!this->GetConnected())
    {
    return 0;
    }

  char* buffer = reinterpret_cast<char*>(data);
  int total = 0;
  do
    {
#if defined(_WIN32) && !defined(__CYGWIN__)
    int trys = 0;
#endif
    int n = recv(this->m_SocketDescriptor, buffer+total, length-total, 0);
    if (n < 0 && this->m_ReceiveTimeoutFlag) // Timeout
      {
      return -1;
      }
    if(n < 1)
      {
#if defined(_WIN32) && !defined(__CYGWIN__)
      // On long messages, Windows recv sometimes fails with WSAENOBUFS, but
      // will work if you try again.
      int error = WSAGetLastError();
      if ((error == WSAENOBUFS) && (trys++ < 1000))
        {
        Sleep(1);
        continue;
        }
#endif
      // FIXME : Use exceptions ?  igtlErrorMacro("Socket Error: Receive failed.");
      return 0;
      }
    total += n;
    } while(readFully && total < length);
  return total;
}


//-----------------------------------------------------------------------------
int MexSocket::SetTimeout(int timeout)
{
  if (SetReceiveTimeout(timeout) && SetSendTimeout(timeout))
    {
    return 1;
    }
  else
    {
    return 0;
    }
}


//-----------------------------------------------------------------------------
int MexSocket::SetReceiveTimeout(int timeout)
{
  if (!this->GetConnected())
    {
    return 0;
    }
  
#if defined(_WIN32) && !defined(__CYGWIN__)
  this->m_ReceiveTimeout = timeout;
  int len;
#else
  this->m_ReceiveTimeout.tv_sec  = timeout/1000;          /* second */
  this->m_ReceiveTimeout.tv_usec = (timeout%1000) * 1000; /* microsecond */
  socklen_t len;
#endif
  if ( timeout > 0 )
    {
    getsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
               (char*)&(this->m_OrigReceiveTimeout), &len);
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
               (char*)&(this->m_ReceiveTimeout), sizeof(this->m_ReceiveTimeout));
    this->m_ReceiveTimeoutFlag = 1;
    }
  else if (this->m_ReceiveTimeoutFlag)
    {
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
               (char*)&(this->m_OrigReceiveTimeout), sizeof(this->m_OrigReceiveTimeout));
    this->m_ReceiveTimeoutFlag = 0;
    }

  return timeout;
}


//-----------------------------------------------------------------------------
int MexSocket::SetSendTimeout(int timeout)
{
  if (!this->GetConnected())
    {
    return 0;
    }
  
#if defined(_WIN32) && !defined(__CYGWIN__)
  this->m_SendTimeout = timeout;
  int len;
#else
  this->m_SendTimeout.tv_sec  = timeout/1000;          /* second */
  this->m_SendTimeout.tv_usec = (timeout%1000) * 1000; /* microsecond */
  socklen_t len;
#endif
  if ( timeout > 0 )
    {
    getsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
               (char*)&(this->m_OrigSendTimeout), &len);
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
               (char*)&(this->m_SendTimeout), sizeof(this->m_SendTimeout));
    this->m_SendTimeoutFlag = 1;
    }
  else if (this->m_SendTimeoutFlag)
    {
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
               (char*)&(this->m_OrigSendTimeout), sizeof(this->m_OrigSendTimeout));
    this->m_SendTimeoutFlag = 0;
    }

  return timeout;
}


//-----------------------------------------------------------------------------
int MexSocket::Skip(int length, int skipFully/*=1*/)
{
  unsigned char dummy[256];
  int block  = 256;
  int n      = 0;
  int remain = length;

  do
    {
    if (remain < block)
      {
      block = remain;
      }
    
    n = this->Receive(dummy, block, skipFully);
    remain -= n;
    }
  while (remain > 0 || (skipFully && n < block));

  return (length - remain);

}


//-----------------------------------------------------------------------------
int MexSocket::SetDescriptor(int sd)
{
  if (!sd)
    {
    igtlErrorMacro("Failed to create socket.");
    return -1;
    }

  if (this->m_SocketDescriptor != -1)
    {
    igtlWarningMacro("Connection already exists. Closing it.");
    this->CloseSocket(this->m_SocketDescriptor);
    this->m_SocketDescriptor = -1;
    }
  
  this->m_SocketDescriptor = sd;

  return 0;
}


//-----------------------------------------------------------------------------
void MexSocket::PrintSelf(std::ostream& os)
{
  this->Superclass::PrintSelf(os);
}


} // end of igtl namespace
