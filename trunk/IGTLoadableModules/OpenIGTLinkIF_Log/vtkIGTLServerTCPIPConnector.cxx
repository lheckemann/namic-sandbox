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
#include "vtkMutexLock.h"
#include "vtkImageData.h"

#include "igtlOSUtil.h"
#include "igtlMessageBase.h"
#include "igtlMessageHeader.h"

#include "vtkIGTLConnector.h"
#include "vtkIGTLServerTCPIPConnector.h"

vtkStandardNewMacro(vtkIGTLServerTCPIPConnector);
vtkCxxRevisionMacro(vtkIGTLServerTCPIPConnector, "$Revision: 1.0 $");

//---------------------------------------------------------------------------
vtkIGTLServerTCPIPConnector::vtkIGTLServerTCPIPConnector()
{
  this->ServerHostname = "localhost";
  this->ServerPort = 18944;
  this->Mutex = vtkMutexLock::New();
}

//---------------------------------------------------------------------------
vtkIGTLServerTCPIPConnector::~vtkIGTLServerTCPIPConnector()
{
  if (this->Mutex)
    {
    this->Mutex->Delete();
    }
}

//---------------------------------------------------------------------------
void vtkIGTLServerTCPIPConnector::PrintSelf(ostream& os, vtkIndent indent)
{
}

//---------------------------------------------------------------------------
void* vtkIGTLServerTCPIPConnector::StartThread()
{
  //Create Server Socket
  this->SetState(STATE_WAIT_CONNECTION);
  this->ServerSocket = igtl::ServerSocket::New();
  this->ServerSocket->CreateServer(this->ServerPort);

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

  //Close sockets
  if (this->Socket.IsNotNull())
    {
    this->Socket->CloseSocket();
    }

  if (this->ServerSocket.IsNotNull())
    {
    this->ServerSocket->CloseSocket();
    }

  return NULL;
}

//---------------------------------------------------------------------------
int vtkIGTLServerTCPIPConnector::WaitForConnection()
{

  while (!this->GetConnectorStopFlag())
    {
    //vtkErrorMacro("vtkIGTLConnector: Waiting for client @ port #" << this->ServerPort);
    this->Socket = this->ServerSocket->WaitForConnection(1000);      
    if (this->Socket.IsNotNull()) // if client connected
      {
      //vtkErrorMacro("vtkIGTLConnector: connected.");
      return 1;
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
int vtkIGTLServerTCPIPConnector::ReceiveData(void* data, int size, int readFully /*=1*/)
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
int vtkIGTLServerTCPIPConnector::SendData(int size, unsigned char* data)
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


