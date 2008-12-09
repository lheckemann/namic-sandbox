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


void printUsage(const char* name)
{
    std::cerr << "Invalid arguments!" << std::endl;
    std::cerr << "Usage: " << name << " s <port> <delay> <flowctrl>" << std::endl;
    std::cerr << "       " << name << " c <address> <port> <delay> <flowctrl>" << std::endl;
    std::cerr << "  <server>: OpenIGTLink server hostname" << std::endl;
    std::cerr << "  <delay> : delay of image acuiqisition (ms)" << std::endl;
    std::cerr << "  <flow>  : flow control -- 0: off / 1: on" << std::endl;
    std::cerr << std::endl;
}

int main(int argc, char **argv)
{

  if (argc < 5) {
    printUsage(argv[0]);
    exit(-1);
  }

  int fserver = 0;
  char* hostname = argv[1];
  int port;
  int delay = atoi(argv[2]);
  int flow = atoi(argv[3]);

  if (argv[2][0] == 's') // server mode
    {
      fserver  = 1;
      port     = atoi(argv[2]);
      delay    = atoi(argv[3]);
      flow     = atoi(argv[4]);
    }
  else
    {
      if (argc < 6)
        {
        printUsage(argv[0]);
        exit(-1);
        }
      fserver  = 0;
      hostname = argv[2];
      port     = atoi(argv[3]);
      delay    = atoi(argv[4]);
      flow     = atoi(argv[5]);
    }


  std::cerr << "Creating new Scanner..." << std::endl;
  //AcquisitionSimulator* acquisition = new AcquisitionSimulator;
  AcquisitionGEExcite* acquisition = new AcquisitionGEExcite;
  TransferOpenIGTLink* transfer    = new TransferOpenIGTLink;

  acquisition->SetRdsHost("10.121.1.100", 5007);
  acquisition->SetRevision(AcquisitionGEExcite::REV_14_X);
  acquisition->SetChannels(8);
  acquisition->SetDelay(delay);
  //acquisition->SetViewsXfer(int v);
  //acquisition->SetValidate(bool s);
  //acquisition->SetDataOrder();
  acquisition->SetLineOrder("out_in"); // or "down_up"
  acquisition->SetPostProcessThread(dynamic_cast<Thread*>(transfer));
  acquisition->Init();


  transfer->FlowControl(flow);
    
  if (fserver)  // server mode
    {
    transfer->SetServerMode(port);
    }
  else // client mode
    {
    //transfer->SetServer("localhost", 18944);
    //transfer->SetServer("10.121.1.31", 18944);
    transfer->SetClientMode(hostname, port);
    }

  transfer->SetAcquisitionThread(acquisition);
  //transfer->Connect();
  
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
