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
#include "vtkCallbackCommand.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"

#include "vtkUDPServerLogic.h"
#include "vtkMultiThreader.h"

#include <string>

vtkCxxRevisionMacro(vtkUDPServerLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkUDPServerLogic);

//---------------------------------------------------------------------------
vtkUDPServerLogic::vtkUDPServerLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkUDPServerLogic::DataCallback);
  this->ServerStopFlag = false;
  this->ThreadID = -1;
  this->Thread = vtkMultiThreader::New();
}


//---------------------------------------------------------------------------
vtkUDPServerLogic::~vtkUDPServerLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }
}

//---------------------------------------------------------------------------
void vtkUDPServerLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkUDPServerLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkUDPServerLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkUDPServerLogic *self = reinterpret_cast<vtkUDPServerLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkUDPServerLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkUDPServerLogic::UpdateAll()
{

}

//---------------------------------------------------------------------------
int vtkUDPServerLogic::StartServerConnection()
{
  //Create the UDP socket
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) 
    {
    std::cerr << "Failed to create socket" << std::endl;
    return 0;
    }
  else
    {
    // Construct the server sockaddr_in structure
    memset(&echoserver, 0, sizeof(echoserver));     // Clear struct
    echoserver.sin_family = AF_INET;                // Internet IP
    echoserver.sin_addr.s_addr = htonl(INADDR_ANY); // Any IP address
    //echoserver.sin_addr.s_addr = inet_addr("134.174.54.47");
    echoserver.sin_port = htons(port);              // server port
    
    // Setup the socket option to reuse
    int on = 1;
    setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    /*//Non blocking flag
    int flags = fcntl(sock, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(sock, F_SETFL, flags);*/
    
    // Bind the socket
    serverlen = sizeof(echoserver);
    if (bind(sock, (struct sockaddr *) &echoserver, serverlen) < 0) 
      {
      std::cerr << "Failed to bind server socket" << std::endl;
      return 0;
      }
    return 1;
    }
}

//--------------------------------------------------------------------------
void vtkUDPServerLogic::ImportData()
{
  // Receive a message from the client
  clientlen = sizeof(echoclient);
  if ((received = recvfrom(sock, buffer, BUFFSIZE, 0, (struct sockaddr *) &echoclient, &clientlen)) < 0) 
    {
    std::cerr << "Failed to receive message" << std::endl;
    }
  //fprintf(stderr, "Client connected: %s\n", inet_ntoa(echoclient.sin_addr));
  buffer[received]= '\0';
  //Print out received data
  //std::cerr << received << " bytes received" << std::endl;
  this->ImportedData = buffer;
}

//-------------------------------------------------------------------------------
int vtkUDPServerLogic::Start(int p)
{
  //Assign port value
  this->port = p;
  this->ServerStopFlag = false;
  
  // Check if thread is detached
  if (this->ThreadID >= 0)
    {
    std::cerr << "Thread already exists" << std::endl;
    return 0;
    }
  //Start Thread
  this->ThreadID = this->Thread->SpawnThread((vtkThreadFunctionType) &vtkUDPServerLogic::ThreadFunction, this);
  std::cerr << "Thread ID: " << this->ThreadID << std::endl;
  return 1;
}

//---------------------------------------------------------------------------
void* vtkUDPServerLogic::ThreadFunction(void* ptr)
{
  std::cerr << "Starting Thread Function..." << std::endl;
  vtkMultiThreader::ThreadInfo* vinfo = static_cast<vtkMultiThreader::ThreadInfo*>(ptr);
  vtkUDPServerLogic* server = static_cast<vtkUDPServerLogic*>(vinfo->UserData);
  
  //Start the Server Connection
  int state = server->StartServerConnection();
  std::cerr << "State: " << state<< std::endl;
  if (state == 0)
    {
    server->ServerStopFlag = true;
    }
  
  // Communication -- common to both Server and Client
  while (!server->ServerStopFlag)
    {
    server->ImportData();
    }

  server->ThreadID = -1;
  return NULL;
}

//---------------------------------------------------------------------------
int vtkUDPServerLogic::Stop()
{
  // Check if thread exists
  std::cerr << "Thread ID: " << this->ThreadID << std::endl;
  if (this->ThreadID >= 0)
    {
    this->ServerStopFlag = true;
    this->Thread->TerminateThread(this->ThreadID);
    this->ThreadID = -1;
    std::cerr << "Socket Disconnected" << std::endl;
    return 1;
    }
  else
    {
    return 0;
    }
}

