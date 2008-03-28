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

#include "igtl_header.h"
#include "igtl_image.h"

#include "igtlMath.h"
//#include "ScannerSim.h"

#include "AcquisitionSimulator.h"
#include "TransferOpenIGTLink.h"

int main(int argc, char **argv)
{

  if (argc < 8) {
    std::cerr << "Invalid arguments!" << std::endl;
    std::cerr << "Usage: " << argv[0] << " <x> <y> <z> <type> <rx> <ry> <rz>"
              << " <fname_temp> <bindex> <eindex> <fps> <hostname>"
              << std::endl;
    std::cerr << "    <x>, <y>, <z>   : Number of pixels in x, y and z direction." << std::endl;
    std::cerr << "    <type>          : Type (2:int8 3:int8 4:int16 5:uint16 6:int32 7:uint32" << std::endl;
    std::cerr << "    <rx>, <ry>, <rz>: resolution (pixel size) in x, y and z direction" << std::endl;
    std::cerr << "    <fname_temp>    : File name template (e.g. \"RawImage_\%04d.raw\")." << std::endl;
    std::cerr << "    <bindex>        : Begin index." << std::endl;
    std::cerr << "    <eindex>        : End  index." << std::endl;
    std::cerr << "    <fps>           : Frame rate (frames per second)." << std::endl;
    std::cerr << "    <hostname>      : hostname (port# is fixed to 18944)" << std::endl;
    exit(-1);
  }

  int size[3];
  size[0] = atoi(argv[1]);
  size[1] = atoi(argv[2]);
  size[2] = atoi(argv[3]);
  int type = atoi(argv[4]);
  
  float spacing[3];
  spacing[0] = atoi(argv[5]);
  spacing[1] = atoi(argv[6]);
  spacing[2] = atoi(argv[7]);

  char* filenameTemp = argv[8];
  int bindex = atoi(argv[9]);
  int eindex = atoi(argv[10]);
  float fps  = atof(argv[11]);
  char* host = argv[12];

  int nframe = eindex - bindex + 1;
  int psize = 0;

  if (type == 2 || type == 3) {
    psize = 1;
  } else if (type == 4 || type == 5) {
    psize = 2;
  } else {
    psize = 4;
  }

  //int interval_ms = (int) (1000 / fps);

  std::cerr << "Creating new Scanner..." << std::endl;

  //ScannerSim* scanner = new ScannerSim();
  std::cerr << "Loading data..." << std::endl;
  /*
  scanner->Init();
  scanner->Start();
  */

  AcquisitionSimulator* acquisition = new AcquisitionSimulator;
  TransferOpenIGTLink*  transfer  = new TransferOpenIGTLink;

  int r = acquisition->LoadImageData(filenameTemp, bindex, eindex,
                                 type, size, spacing);
  if (r == 0) {
    std::cerr << "Data load error" << std::endl;
    exit(-1);
  }

  size[2] = 10;
  acquisition->SetSubVolumeDimension(size);
  acquisition->SetPostProcessThread(dynamic_cast<Thread*>(transfer));
  acquisition->SetFrameRate(fps);
  transfer->SetServer(host, 18944);
  transfer->SetAcquisitionThread(acquisition);


  transfer->Connect();

  acquisition->Run();
  transfer->Run();

  // run 100 sec
  sleep(100);

  acquisition->Stop();
  transfer->Stop();
  transfer->Disconnect();

}

