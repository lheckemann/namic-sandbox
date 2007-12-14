/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: SocketServer.cxx,v $

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

static const int scMsgLength = 6;  // three versor components, three vector components.

static void CleanUp(vtkSocketCommunicator* comm, vtkSocketController* contr)
{
  comm->CloseConnection();
  comm->Delete();
  contr->Delete();
}

int main(int argc, char** argv)
{
  if( argc < 2 )
    {
    std::cerr << "Usage : " << std::endl;
    std::cerr << argv[0] << " portNumber " << std::endl;
    return -1;
    }

  vtkSocketController* contr = vtkSocketController::New();
  contr->Initialize();

  vtkSocketCommunicator* comm = vtkSocketCommunicator::New();

  int port=11111;

  int i;

  // Get the port from the command line arguments
  port = atoi(argv[1]);
  std::cout << "Port = " << port << std::endl;

  // Establish connection
  if (!comm->WaitForConnection(port))
    {
    cerr << "Server error: Wait timed out or could not initialize socket."
         << endl;
    comm->Delete();
    contr->Delete();
    return 1;
    }


  // Test receiving float arrays
  float dataf[scMsgLength];
  int tag = 17;
  if (!comm->Receive(dataf, scMsgLength, 1, tag))
    {
    cerr << "Server error: Error receiving data." << endl;
    CleanUp(comm, contr);
    return 1;
    }
  for (i=0; i<scMsgLength; i++)
    {
    std::cout << "Server Received float [" << i << "] = " << dataf[i] << std::endl;
    if (dataf[i] != static_cast<float>(i))
      {
      cerr << "Server error: Corrupt float array." << endl;
      CleanUp(comm, contr);
      return 1;
      }
    }

  CleanUp(comm, contr);

  return 0;
}
