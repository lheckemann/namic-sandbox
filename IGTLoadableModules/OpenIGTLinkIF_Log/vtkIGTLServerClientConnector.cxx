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
#include "vtkIGTLServerClientConnector.h"

vtkStandardNewMacro(vtkIGTLServerClientConnector);
vtkCxxRevisionMacro(vtkIGTLServerClientConnector, "$Revision: 1.0 $");

//---------------------------------------------------------------------------
vtkIGTLServerClientConnector::vtkIGTLServerClientConnector()
{
  this->ServerHostname = "localhost";
  this->ServerPort = 18944;
  this->Mutex = vtkMutexLock::New();
}

//---------------------------------------------------------------------------
vtkIGTLServerClientConnector::~vtkIGTLServerClientConnector()
{
  if (this->Mutex)
    {
    this->Mutex->Delete();
    }
}

//---------------------------------------------------------------------------
void vtkIGTLServerClientConnector::PrintSelf(ostream& os, vtkIndent indent)
{
}

//---------------------------------------------------------------------------
int vtkIGTLServerClientConnector::SetTypeServer(int port)
{
  this->SetType(TYPE_SERVER);
  this->ServerPort = port;
  return 1;
}

//---------------------------------------------------------------------------
int vtkIGTLServerClientConnector::SetTypeClient(char* hostname, int port)
{
  this->SetType(TYPE_CLIENT);
  this->ServerPort = port;
  this->ServerHostname = hostname;
  return 1;
}

//---------------------------------------------------------------------------
int vtkIGTLServerClientConnector::SetTypeClient(std::string hostname, int port)
{
  this->SetType(TYPE_CLIENT);
  this->ServerPort = port;
  this->ServerHostname = hostname;
  return 1;
}

//---------------------------------------------------------------------------
void* vtkIGTLServerClientConnector::StartThread()
{
  this->SetState(STATE_WAIT_CONNECTION);
  if (this->GetType() == TYPE_SERVER)
    {
    this->ServerSocket = igtl::ServerSocket::New();
    this->ServerSocket->CreateServer(this->ServerPort);
    }

  // Communication -- common to both Server and Client
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

  if (this->GetType() == TYPE_SERVER && this->ServerSocket.IsNotNull())
    {
    this->ServerSocket->CloseSocket();
    }

  return NULL;
}

//---------------------------------------------------------------------------
int vtkIGTLServerClientConnector::WaitForConnection()
{
  if (this->GetType() == TYPE_CLIENT)
    {
    this->Socket = igtl::ClientSocket::New();
    }

  while (!this->GetConnectorStopFlag())
    {
    if (this->GetType() == TYPE_SERVER)
      {
      //vtkErrorMacro("vtkIGTLConnector: Waiting for client @ port #" << this->ServerPort);
      this->Socket = this->ServerSocket->WaitForConnection(1000);      
      if (this->Socket.IsNotNull()) // if client connected
        {
        //vtkErrorMacro("vtkIGTLConnector: connected.");
        return 1;
        }
      }
    else if (this->GetType() == TYPE_CLIENT) // if this->Type == TYPE_CLIENT
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
    else
      {
      this->SetConnectorStopFlag(true);
      }
    }

  if (this->Socket.IsNotNull())
    {
    //vtkErrorMacro("vtkOpenIGTLinkLogic::WaitForConnection(): Socket Closed.");
    this->Socket->CloseSocket();
    }
 
  return 0;
}

//---------------------------------------------------------------------------
int vtkIGTLServerClientConnector::ReceiveData(void* data, int size, int readFully /*=1*/)
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
int vtkIGTLServerClientConnector::SendData(int size, unsigned char* data)
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


