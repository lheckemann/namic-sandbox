/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: ddiTwoLoPoMoCo.cpp,v 1.5 2006/05/13 21:29:21 kapoor Exp $

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
#include <cisstDeviceInterface/ddiLoPoMoCo.h>
#include <cisstDeviceInterface/ddiTwoLoPoMoCo.h>

#include <vector>
#include <string>


CMN_IMPLEMENT_SERVICES(ddiTwoLoPoMoCo);


ddiTwoLoPoMoCo::ddiTwoLoPoMoCo(const char* deviceName,
             const char* component1Name,
             const char* component2Name,
                   const char* relativePathToConfigFiles):
    ddiCompositeDevice(deviceName)
{
    ddiDeviceInterface *dev1 = new ddiLoPoMoCo(component1Name);
    ddiCompositeDevice::AddDevice(dev1);
    ddiDeviceInterface *dev2 = new ddiLoPoMoCo(component2Name);
    ddiCompositeDevice::AddDevice(dev2);
    strncpy(RelativePathToConfigFiles, relativePathToConfigFiles, 255);
}


ddiTwoLoPoMoCo::~ddiTwoLoPoMoCo()
{
}


void ddiTwoLoPoMoCo::Configure(const char* filename)
{
    CMN_LOG_CLASS(3) << "Configuring ddiTwoLoPoMoCo" << std::endl;

    // if no file name has been provided, use defaults
    if (!filename) {
  CMN_LOG_CLASS(2) << "Configuring ddiTwoLoPoMoCo with default LoPoMoCo{1,2}.xml"
       <<std::endl;
  (ddiCompositeDevice::GetDevice(0))->Configure("LoPoMoCo1.xml");
  (ddiCompositeDevice::GetDevice(1))->Configure("LoPoMoCo2.xml");
  return;
    }

    // load configuration file for ddiTwoLoPoMoCo
    CMN_LOG_CLASS(3) << "Configuring ddiTwoLoPoMoCo with file: " << filename << std::endl;
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

    CMN_LOG_CLASS(3) << "Configuring ddiTwoLoPoMoCo finished" << std::endl;
}

#endif // CISST_HAS_LOPOMOCO


// $Log: ddiTwoLoPoMoCo.cpp,v $
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
// Revision 1.1  2005/02/28 22:35:04  anton
// cisstDeviceInterface:  Added ddiTwoLoPoMoCo.
//
