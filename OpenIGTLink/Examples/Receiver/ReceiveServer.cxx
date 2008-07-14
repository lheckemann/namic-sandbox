/*=========================================================================

  Program:   Open IGT Link -- Example for Data Receiving Server Program
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
#include "igtlTransformMessage.h"
#include "igtlServerSocket.h"
#include "igtlClientSocket.h"

void GetRandomTestMatrix(igtl::Matrix4x4& matrix);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 2) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  int    port     = atoi(argv[1]);

  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetDeviceName("Tracker");

  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  serverSocket->CreateServer(port);

  igtl::ClientSocket::Pointer socket;
  
  while (1)
    {
    //------------------------------------------------------------
    // Waiting for Connection
    socket = serverSocket->WaitForConnection(1000);
    
    if (socket.IsNotNull()) // if client connected
      {
      // Create a message buffer to receive header
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();

      //------------------------------------------------------------
      // loop
      for (int i = 0; i < 100; i ++)
        {

        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the socket
        int r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
        if (r != headerMsg->GetPackSize())
          {
          std::cerr << "Error: receiving data." << std::endl;
          }

        // Deserialize the header
        headerMsg->Unpack();

        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM"))
          {
          // Create a message buffer to receive transform data
          igtl::TransformMessage::Pointer transMsg;
          transMsg = igtl::TransformMessage::New();
          transMsg->SetMessageHeader(headerMsg);
          transMsg->AllocatePack();

          // Receive transform data from the socket
          socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());

          // Deserialize the transform data
          transMsg->Unpack();

          // Retrive the transform data
          igtl::Matrix4x4 matrix;
          transMsg->GetMatrix(matrix);
          igtl::PrintMatrix(matrix);

          // Delete message class
          //transMsg->Delete();
          }
        else if (strcmp(headerMsg->GetDeviceType(), "IMAGE"))
          {
          }
        else
          {
          }

        }
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  transMsg->Delete();
  socket->CloseSocket();
  socket->Delete();

}


