/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: SocketClient.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkDebugLeaks.h"
#include "vtkSocketCommunicator.h"
#include "vtkSocketController.h"

static const int scMsgLength = 6; // three versor components, three vector components.

static void CleanUp(vtkSocketCommunicator* comm, vtkSocketController* contr)
{
  // This will close the connection as well as delete 
  // the communicator
  comm->Delete();
  contr->Delete();
}

int main(int argc, char** argv)
{
  if( argc < 3 )
    {
    std::cerr << "Usage : " << std::endl;
    std::cerr << argv[0] << " hostName portNumber " << std::endl;
    return -1;
    }


  vtkSocketController* contr = vtkSocketController::New();
  contr->Initialize();

  vtkSocketCommunicator* comm = vtkSocketCommunicator::New();

  int i;

  std::cout << " Hostname = " << argv[1] << std::endl;

  int port = atoi(argv[2]);
  std::cout << " Port = " << port << std::endl;

  // Establish connection
  if (!comm->ConnectTo(argv[1], port))
    {
    cerr << "Client error: Could not connect to the server." << endl;
    comm->Delete();
    contr->Delete();
    return 1;
    }

  // Test receiving some supported types of arrays
  float dataf[scMsgLength];
  for (i=0; i<scMsgLength; i++)
    {
    dataf[i] = static_cast<float>(i);
    }
  int tag = 17;
  if (!comm->Send(dataf, scMsgLength, 1, tag))
    {
    cerr << "Client error: Error sending data." << endl;
    CleanUp(comm, contr);
    return 1;
    }

  CleanUp(comm, contr);

  return 0;
}
