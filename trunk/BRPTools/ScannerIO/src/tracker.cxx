/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifdef WIN32
#define NOMINMAX
#ifdef USE_RAWINPUT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#endif
#endif

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string>
#include <math.h>

#include <iostream>

#include "igtlMath.h"
#include "igtlOSUtil.h"
//#include "ScannerSim.h"

#include "AcquisitionTrackingSimulator.h"
#include "TransferOpenIGTLink.h"

int main(int argc, char **argv)
{

  if (argc < 3) {
    std::cerr << "Invalid arguments!" << std::endl;
    std::cerr << "Usage: " << argv[0] << "<fps> <hostname>"
              << std::endl;
    std::cerr << "    <fps>           : Frame rate (frames per second)." << std::endl;
    std::cerr << "    <hostname>      : hostname (port# is fixed to 18944)" << std::endl;
    exit(-1);
  }

  float fps  = atof(argv[1]);
  char* host = argv[2];


  std::cerr << "Creating new Tracker..." << std::endl;

  AcquisitionTrackingSimulator* acquisition = new AcquisitionTrackingSimulator;
  TransferOpenIGTLink*  transfer  = new TransferOpenIGTLink;

  acquisition->SetPostProcessThread(dynamic_cast<Thread*>(transfer));
  acquisition->SetFrameRate(fps);
  transfer->SetServer(host, 18944);
  transfer->SetAcquisitionThread(acquisition);

  transfer->Connect();
  acquisition->Run();
  transfer->Run();

  // run 100 sec
  //sleep(100);
  igtl::Sleep(100*1000);

  acquisition->Stop();
  transfer->Stop();
  transfer->Disconnect();

}

