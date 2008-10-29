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
#include "igtlClientSocket.h"

void GetRandomTestMatrix(igtl::Matrix4x4& matrix);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc < 9 || (argc >= 9 && (argc - 5 - 4) % 4 != 0)) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <fps> <h> [<x> <y> <z> <r>] ..." << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <fps>      : Frequency (fps) to send coordinate" << std::endl;
    std::cerr << "    <h>        : Hardness"                           << std::endl;
    std::cerr << "    <x>,<y>,<z>: Center position"                    << std::endl;
    std::cerr << "    <r>        : Radius"                             << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);
  double fps      = atof(argv[3]);
  int    interval = (int) (1000.0 / fps);
  float  hardness = atof(argv[4]);

  int    ndata    = (argc - 5) / 4;
  float* centerx  = new float[ndata];
  float* centery  = new float[ndata];
  float* centerz  = new float[ndata];
  float* radius   = new float[ndata];

  for (int i = 0; i < ndata; i ++)
    {
    centerx[i] = atof(argv[5+i*4]);
    centery[i] = atof(argv[5+i*4+1]);
    centerz[i] = atof(argv[5+i*4+2]);
    radius[i]  = atof(argv[5+i*4+3]);
    }

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
  for (int i = 0; i < 10; i ++)
    {
    //------------------------------------------------------------
    // Create a new IMAGE type message
    igtl::VFixtureMessage::Pointer vfMsg = igtl::VFixtureMessage::New();
    vfMsg->SetNumberOfSpheres(ndata);
    vfMsg->SetHardness(hardness);
    for (int j = 0; j < ndata; j ++)
      {
      vfMsg->SetCenter(j, centerx[j], centery[j], centerz[j]);
      vfMsg->SetRadius(j, radius[j]);
      }

    vfMsg->SetDeviceName("VFClient");
    vfMsg->AllocateScalars();

    //------------------------------------------------------------
    // Pack (serialize) and send
    vfMsg->Pack();
    socket->Send(vfMsg->GetPackPointer(), vfMsg->GetPackSize());

    igtl::Sleep(interval); // wait

    }

  //------------------------------------------------------------
  // Close connection
  socket->CloseSocket();

}

