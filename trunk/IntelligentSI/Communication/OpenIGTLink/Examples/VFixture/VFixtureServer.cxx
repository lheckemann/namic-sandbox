/*=========================================================================

  Program:   Open IGT Link -- Example for Tracker Client Program
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>

#include "igtlOSUtil.h"
#include "igtlVFixtureMessage.h"
#include "igtlServerSocket.h"

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 3) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port> <fps> <imgdir>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <fps>      : Frequency (fps) to send coordinate" << std::endl;
    exit(0);
    }

  int    port     = atoi(argv[1]);
  double fps      = atof(argv[2]);
  int    interval = (int) (1000.0 / fps);

  //------------------------------------------------------------
  // Prepare server socket
  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  serverSocket->CreateServer(port);

  igtl::Socket::Pointer socket;
  
  while (1)
    {
    //------------------------------------------------------------
    // Waiting for Connection
    socket = serverSocket->WaitForConnection(1000);
    
    if (socket.IsNotNull()) // if client connected
      {
      //------------------------------------------------------------
      // loop
      for (int i = 0; i < 100; i ++)
        {
        //------------------------------------------------------------
        // Create a new IMAGE type message
        igtl::VFixtureMessage::Pointer vfMsg = igtl::VFixtureMessage::New();
        vfMsg->SetNumberOfSpheres(1);
        vfMsg->SetHardness(2.0);
        vfMsg->SetCenter(0, 3.0, 4.0, 5.0);
        vfMsg->SetRadius(0, 6.0);
        vfMsg->SetDeviceName("VFClient");
        vfMsg->AllocateScalars();
        
        //------------------------------------------------------------
        // Pack (serialize) and send
        vfMsg->Pack();
        socket->Send(vfMsg->GetPackPointer(), vfMsg->GetPackSize());
        
        igtl::Sleep(interval); // wait
        }
      }
    }
  
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  socket->CloseSocket();

}



