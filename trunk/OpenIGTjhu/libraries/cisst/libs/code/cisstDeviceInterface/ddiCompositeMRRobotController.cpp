/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: ddiThreeLoPoMoCo.cpp,v 1.5 2006/05/13 21:29:21 kapoor Exp $

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
\                                                                               /
/               Developed by the Engineering Research Center for                \
\         Computer-Integrated Surgical Systems & Technology (CISST)             /
/                                                                               \
\           Copyright(c) 1995-2001, The Johns Hopkins University                /
/                          All Rights Reserved.                                 \
\                                                                               /
/                    Author: Ankur Kapoor (kapoor@cs.jhu.edu)                   \
\                           Created on: 2004-03-16                              /
/                                                                               \
\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
*/

#include <cisstConfig.h>

#if CISST_HAS_LOPOMOCO

#include <cisstCommon/cmnXMLPath.h>
#include <cisstDeviceInterface/ddiCompositeMRRobotController.h>
#include <cisstDeviceInterface/ddiLoPoMoCo.h>
#include <cisstDeviceInterface/ddiDMM16AT.h>
#include <cisstDeviceInterface/ddiRMM1612.h>

#include <vector>
#include <string>


CMN_IMPLEMENT_SERVICES(ddiCompositeMRRobotController);


ddiCompositeMRRobotController::ddiCompositeMRRobotController(const char* deviceName):
    ddiCompositeDevice(deviceName)
{
  // DAC
    ddiDeviceInterface *dev1 = new ddiRMM1612();
    ddiCompositeDevice::AddDevice(dev1);
  //ADC
    ddiDeviceInterface *dev2 = new ddiDMM16AT();
    ddiCompositeDevice::AddDevice(dev2);
  //FPGA
  ddiDeviceInterface *dev3 = new ddiLoPoMoCo();
    ddiCompositeDevice::AddDevice(dev3);

}



ddiCompositeMRRobotController::~ddiCompositeMRRobotController()
{
}


void ddiCompositeMRRobotController::Configure(const char* filename)
{
    CMN_LOG_CLASS(3) << "Configuring ddiCompositeMRRobotController" << std::endl;

    // if no file name has been provided, use defaults
    //if (!filename) {
  //CMN_LOG_CLASS(2) << "Configuring ddiCompositeMRRobotController with default LoPoMoCo{1,2,3}.xml"
  //     <<std::endl;
  (ddiCompositeDevice::GetDevice(0))->Configure("./RMM1612_TestXMLConfig/RMM1612_Test.xml");
  (ddiCompositeDevice::GetDevice(1))->Configure("./DMM16AT_TestXMLConfig/DMM16AT_Test.xml");
  (ddiCompositeDevice::GetDevice(2))->Configure("./LoPoMoCoTestXMLConfig/LoPoMoCo_Test.xml");

  //return;
    //}

    /*
  // load configuration file for ddiCompositeMRRobotController
    CMN_LOG_CLASS(3) << "Configuring ddiCompositeMRRobotController with file: " << filename << std::endl;
    cmnXMLPath xmlConfig;
    xmlConfig.SetInputSource(filename);
    char path[30];
    std::string context("/Config/Device[@Name=\"");
    context = context + GetName() + "\"]";
    std::vector<std::string> configFiles(SIZE);
    unsigned int boardIndex;

    bool allFilesDefined = true;

    // get names of files first
    for (boardIndex = 0; boardIndex < SIZE; boardIndex++) {
  sprintf(path, "ConfigFile[%d]/@Name", boardIndex + 1);
  allFilesDefined &= xmlConfig.GetXMLValue(context.c_str(), path, configFiles[boardIndex]);
    }
    if (allFilesDefined) {
  for (boardIndex = 0; boardIndex < SIZE; boardIndex++) {
        configFiles[boardIndex].insert(0, RelativePathToConfigFiles);
      CMN_LOG_CLASS(3) << "Configuring board " << boardIndex + 1
           << " with " << configFiles[boardIndex] << std::endl;
      (ddiCompositeDevice::GetDevice(boardIndex))->Configure(configFiles[boardIndex].c_str());
  }
    } else {
  CMN_LOG_CLASS(1) << "Couldn't find all the required ConfigFile in " << filename << std::endl;
    }
*/
    CMN_LOG_CLASS(3) << "Configuring ddiCompositeMRRobotController finished" << std::endl;
}


#endif // CISST_HAS_LOPOMOCO


// $Log: ddiThreeLoPoMoCo.cpp,v $
// Revision 1.5  2006/05/13 21:29:21  kapoor
// cisstDeviceInterface and LoPoMoCo: Added a relative path member, that
// is appended to the config file name while opening config file.
//
// Revision 1.4  2006/05/11 18:45:36  anton
// ddiMEI and ddiLoPoMoCo: Updated for new class registration.
//
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/03/01 16:32:02  anton
// cisstDevice LoPoMoCo: Updated ddiThree and ddiFourLoPoMoCo to allow user
// defined configuration files.  See ticket #129.
//
// Revision 1.1  2005/02/03 21:48:37  anton
// cisstDeviceInterface: Renamed ddiTroisLoPoMoCo to ddiThreeLoPoMoCo
// and ddiQuartLoPoMoCo to ddiFourLoPoMoCo.
//
