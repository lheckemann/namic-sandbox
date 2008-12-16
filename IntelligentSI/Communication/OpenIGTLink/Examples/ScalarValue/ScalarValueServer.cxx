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
#include "igtlScalarValueMessage.h"
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
  char*  filedir  = argv[3];
    int i;
  
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
        // size parameters
                int     size = 3;
                int     value[ 3 ] = {0, 1, 2};
        int   scalarType = igtl::ScalarValueMessage::TYPE_UINT8;// scalar type
        
        //------------------------------------------------------------
        // Create a new IMAGE type message
        igtl::ScalarValueMessage::Pointer svMsg = igtl::ScalarValueMessage::New();
                svMsg->SetDimension( size );
        svMsg->SetScalarType(scalarType);
        svMsg->SetDeviceName("ScalarValueClient");
        svMsg->AllocateScalars();
        
        //------------------------------------------------------------
            // Set Scalar Value
                memcpy(svMsg->GetScalarPointer(), value, 1 * size);
                //for ( int j = 0 ; j < size ; j ++ )
                //{
                //  *svMsg->GetScalarPointer = (i * j);
                //  svMsg->GetScalarPointer++:
                //}
        
        //------------------------------------------------------------
        // Pack (serialize) and send
        svMsg->Pack();
        socket->Send(svMsg->GetPackPointer(), svMsg->GetPackSize());
        
        igtl::Sleep(interval); // wait
        }
      }
    }
  
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  socket->CloseSocket();

}

