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
#include "igtlClientSocket.h"


int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 4) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <fps> <imgdir>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <fps>      : Frequency (fps) to send coordinate" << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);
  double fps      = atof(argv[3]);
  int    interval = (int) (1000.0 / fps);

  //------------------------------------------------------------
  // Establish Connection
  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostname, port);

  if (r != 0)
    {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
    }

  //------------------------------------------------------------
  // loop
  for (int i = 0; i < 100; i ++)
    {

    //------------------------------------------------------------
    // size parameters
    int size = 3;
    unsigned char value[3] = {1,2,3};
    int scalarType = igtl::ScalarValueMessage::TYPE_UINT8;// scalar type

    //------------------------------------------------------------
    // Create a new ScalarValue type message
    igtl::ScalarValueMessage::Pointer svMsg = igtl::ScalarValueMessage::New();
    svMsg->SetDimension( size );
    svMsg->SetScalarType(scalarType);
    svMsg->SetDeviceName("ScalarValueClient");
    svMsg->AllocateScalars();

    //------------------------------------------------------------
    // Set Scalar Value
    memcpy((unsigned char *)svMsg->GetScalarPointer(), value, sizeof(unsigned char) * size);

    //------------------------------------------------------------
    // Pack (serialize) and send
    svMsg->Pack();
    socket->Send(svMsg->GetPackPointer(), svMsg->GetPackSize());

    igtl::Sleep(interval); // wait

    }

  //------------------------------------------------------------
  // Close connection
  socket->CloseSocket();

}
