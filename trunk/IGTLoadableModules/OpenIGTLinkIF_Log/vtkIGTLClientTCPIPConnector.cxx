/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.
  
  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $
  
==========================================================================*/

#include "vtkObjectFactory.h"

#include <vtksys/SystemTools.hxx>

#include "igtlServerSocket.h"
#include "igtlClientSocket.h"
#include "vtkMutexLock.h"
#include "vtkImageData.h"

#include "igtlOSUtil.h"
#include "igtlMessageBase.h"
#include "igtlMessageHeader.h"

#include "vtkIGTLConnector.h"
#include "vtkIGTLClientTCPIPConnector.h"

vtkStandardNewMacro(vtkIGTLClientTCPIPConnector);
vtkCxxRevisionMacro(vtkIGTLClientTCPIPConnector, "$Revision: 1.0 $");

//---------------------------------------------------------------------------
vtkIGTLClientTCPIPConnector::vtkIGTLClientTCPIPConnector()
{
  this->ServerHostname = "localhost";
  this->ServerPort = 18944;
  this->Mutex = vtkMutexLock::New();
}

//---------------------------------------------------------------------------
vtkIGTLClientTCPIPConnector::~vtkIGTLClientTCPIPConnector()
{
  if (this->Mutex)
    {
    this->Mutex->Delete();
    }
}

//---------------------------------------------------------------------------
void vtkIGTLClientTCPIPConnector::PrintSelf(ostream& os, vtkIndent indent)
{
}

//---------------------------------------------------------------------------
int vtkIGTLClientTCPIPConnector::SetTypeClient(char* hostname, int port)
{
  this->SetType(TYPE_CLIENT);
  this->ServerPort = port;
  this->ServerHostname = hostname;
  return 1;
}

//---------------------------------------------------------------------------
int vtkIGTLClientTCPIPConnector::SetTypeClient(std::string hostname, int port)
{
  this->SetType(TYPE_CLIENT);
  this->ServerPort = port;
  this->ServerHostname = hostname;
  return 1;
}

//---------------------------------------------------------------------------
void* vtkIGTLClientTCPIPConnector::StartThread()
{
  this->SetState(STATE_WAIT_CONNECTION);

  // Communication
  while (!this->GetConnectorStopFlag())
    {
    //vtkErrorMacro("vtkOpenIGTLinkIFLogic::ThreadFunction(): alive.");
    this->Mutex->Lock();
    this->WaitForConnection();
    this->Mutex->Unlock();
    if (this->Socket.IsNotNull())
      {
      this->SetState(STATE_CONNECTED);
      //vtkErrorMacro("vtkOpenIGTLinkIFLogic::ThreadFunction(): Client Connected.");
      this->ReceiveController();
      this->SetState(STATE_WAIT_CONNECTION);
      }
    }

  //Check and Close sockets
  if (this->Socket.IsNotNull())
    {
    this->Socket->CloseSocket();
    }

//  if (this->GetType() == TYPE_SERVER && this->ServerSocket.IsNotNull())
//    {
//    this->ServerSocket->CloseSocket();
//    }

  return NULL;
}

//---------------------------------------------------------------------------
int vtkIGTLClientTCPIPConnector::WaitForConnection()
{
  if (this->GetType() == TYPE_CLIENT)
    {
    this->Socket = igtl::ClientSocket::New();
    }

  while (!this->GetConnectorStopFlag())
    {
    //vtkErrorMacro("vtkIGTLConnector: Connecting to server...");
    int r = this->Socket->ConnectToServer(this->ServerHostname.c_str(), this->ServerPort);
    if (r == 0) // if connected to server
      {
      return 1;
      }
    else
      {
      break;
      }
    }
//    else
//      {
//      this->SetConnectorStopFlag(true);
//      }

  if (this->Socket.IsNotNull())
    {
    //vtkErrorMacro("vtkOpenIGTLinkLogic::WaitForConnection(): Socket Closed.");
    this->Socket->CloseSocket();
    }
 
  return 0;
}

//---------------------------------------------------------------------------
int vtkIGTLClientTCPIPConnector::ReceiveData(void* data, int size, int readFully /*=1*/)
{

  //Check socket and connection
  if (this->Socket.IsNull())
    {
    return 0;
    }
 
  if (!this->Socket->GetConnected())
    {
    return 0;
    }

  //Receive data from TCP/IP connection
  int r = this->Socket->Receive(data, size, readFully);
  
  return r;
}
  

//---------------------------------------------------------------------------
int vtkIGTLClientTCPIPConnector::SendData(int size, unsigned char* data)
{
  
  if (this->Socket.IsNull())
    {
    return 0;
    }
  
  // check if connection is alive
  if (!this->Socket->GetConnected())
    {
    return 0;
    }

  return this->Socket->Send(data, size);  // return 1 on success, otherwise 0.

}


