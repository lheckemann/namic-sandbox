/*=========================================================================

  Program:   OpenIGTLink interface for GE Excite System
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

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

#include "AcquisitionSimulator.h"
#include "AcquisitionGEExcite.h"
#include "TransferOpenIGTLink.h"


int main(int argc, char **argv)
{

  if (argc < 1) {
    std::cerr << "Invalid arguments!" << std::endl;
    std::cerr << "Usage: " << argv[0] << " <x> <y> <z> <type> <rx> <ry> <rz>"
              << " <fname_temp> <bindex> <eindex> <fps> <hostname>"
              << std::endl;
    exit(-1);
  }

  std::cerr << "Creating new Scanner..." << std::endl;
  //AcquisitionSimulator* acquisition = new AcquisitionSimulator;
  AcquisitionGEExcite* acquisition = new AcquisitionGEExcite;
  TransferOpenIGTLink* transfer    = new TransferOpenIGTLink;

  acquisition->SetRdsHost("10.121.1.100", 5007);
  acquisition->SetRevision(AcquisitionGEExcite::REV_14_X);
  acquisition->SetChannels(8);
  //acquisition->SetViewsXfer(int v);
  //acquisition->SetValidate(bool s);
  //acquisition->SetDataOrder();
  acquisition->SetLineOrder("out_in"); // or "down_up"
  acquisition->SetPostProcessThread(dynamic_cast<Thread*>(transfer));

  transfer->SetServer("localhost", 18944);
  transfer->SetAcquisitionThread(acquisition);


  transfer->Connect();

  acquisition->Init();
  acquisition->StartScan();
  transfer->Run();

  while (1) {
    //sleep(10);
    igtl::Sleep(1000);
  }

  acquisition->Stop();
  transfer->Stop();
  transfer->Disconnect();

}
