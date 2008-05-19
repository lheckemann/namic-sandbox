/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiATIDAQFT.cpp,v 1.4 2005/09/26 15:41:46 anton Exp $
//
//  Author(s):  Ankur Kapoor
//  Created on: 2004-04-30
//
//
//              Developed by the Engineering Research Center for
//          Computer-Integrated Surgical Systems & Technology (CISST)
//
//               Copyright(c) 2004, The Johns Hopkins University
//                          All Rights Reserved.
//
//  Experimental Software - Not certified for clinical use.
//  For use only by CISST sponsored research projects.  For use outside of
//  CISST, please contact Dr. Russell Taylor, CISST Director, at rht@cs.jhu.edu
//  
// ****************************************************************************

#include <cisstConfig.h>
#if CISST_HAS_ATIDAQFT

#include <vector>
#include <string>
#include <iostream>
  // idsp includes windows.h which includes msxml.h which conflicts with Xerces/xalan. So we must include
  // XMLPath last. talk of namespace polution and bill gates thinks he knows c++.
extern "C" {
#include "nidaq.h"
#include "nidaqex.h"
#include "nidaqerr.h"
}

#include <cisstDeviceInterface/ddiATIDAQFT.h>
#include <cisstCommon/cmnXMLPath.h>
CMN_IMPLEMENT_SERVICES(ddiATIDAQFT, 5);

ddiATIDAQFT::ddiATIDAQFT():
    ddiDeviceInterface("ATIDAQFT"),
    DAQStatus(0), RetVal(0), Device(1), Channel(0), Gain(1),
    IgnoreWarning(0), NumberChannels(7), IsCalibrated(false),
    IsBiased(false), CalibrationIndex(1), NumSamples(20), 
    Sequence(0) {
  for (int i=0; i < NumberChannels; i++) {
    ChannelVector[i] = i;
    GainVector[i] = 1;
    BiasVoltage[i] = 0;
  }
    
  ReadNameMap["ForceTorques"] = FORCETORQUES;
  ReadOperations.resize(ReadOperations.size() + 2);
  ReadOperations[FORCETORQUES] = new ddiCommand<ddiATIDAQFT>(&ddiATIDAQFT::GetForcesTorques, this, "FT");
    
  WriteNameMap["Bias"] = BIAS;
  WriteOperations.resize(ReadOperations.size() + 2);
  WriteOperations[BIAS] = new ddiCommand<ddiATIDAQFT>(&ddiATIDAQFT::SetNewBias, this, "BIAS");
    
  ConversionNameMap["FTCountsToFTNewtons"] = FTCOUNTSTOFTNEWTONS;
  ConversionOperations.resize(ConversionOperations.size() + 2);
  ConversionOperations[FTCOUNTSTOFTNEWTONS] = new ddiCommand2<ddiATIDAQFT>(&ddiATIDAQFT::ConvertFTCountsToFTNewtons, this, "FTCTON");
}


ddiATIDAQFT::~ddiATIDAQFT() {
}


std::vector<std::string> ddiATIDAQFT::Provides (void) {
  std::vector<std::string> *provides = new std::vector<std::string>;
  OperationNameMapType::iterator iter;
  for (iter = ReadNameMap.begin(); iter != ReadNameMap.end(); ++iter) {
    provides->push_back(iter->first);
  }
  return *provides;
}


void ddiATIDAQFT::Configure(const char *filename) {
  // the calibration file should probably come from a CISST config file
  if (!LoadCalibration(filename)) {
    CMN_LOG_CLASS(1) << "Error in loading calibartion file" << filename << std::endl;
  }
  LowLevelObject.SetForceUnits(CalibrationStruct, "N");
  LowLevelObject.SetTorqueUnits(CalibrationStruct, "N-m");
  //double SampleTT[6]={0,0,20,45,0,0};
  //LowLevelObject.SetToolTransform(CalibrationStruct, SampleTT, "mm", "degrees");
}


bool ddiATIDAQFT::GetForcesTorques(cmnDataObject &obj) {
  cmnFTDataObject *pdata = dynamic_cast<cmnFTDataObject *>(&obj);
  for (int counter3 = 0;  counter3 <= 6; counter3++)  {
    ChannelVector[counter3] = counter3;
    GainVector[counter3] = 1;
  }
    
  DAQStatus = SCAN_Setup (Device, NumberChannels, ChannelVector, GainVector);
  DAQStatus = AI_VRead_Scan (Device, pdata->FTData);
  RetVal = NIDAQErrorHandler(DAQStatus, "AI_VRead", IgnoreWarning);
  Sequence++;
  return true;
}


bool ddiATIDAQFT::ConvertFTCountsToFTNewtons(cmnDataObject &from, cmnDataObject &to) {
  cmnFTDataObject *dataFrom = dynamic_cast<cmnFTDataObject *>(&from);
  cmnFNTNmDataObject *dataTo = static_cast<cmnFNTNmDataObject *>(&to);
  dataTo->Sequence = Sequence;
  dataTo->Condition = 0;
  double rawforces[7],forces[7];
  for (unsigned int i = 0; i < 7; i++) {
    rawforces[i] = (double)dataFrom->FTData[i];
    }
  LowLevelObject.ConvertToFT(CalibrationStruct, rawforces, forces);
  dataTo->Forces[0] = forces[0];
    dataTo->Forces[1] = forces[1];
    dataTo->Forces[2] = forces[2];
  dataTo->Torques[0] = forces[3];
    dataTo->Torques[1] = forces[4];
    dataTo->Torques[2] = forces[5];
  return true;
}


bool ddiATIDAQFT::SetNewBias(cmnDataObject &obj) {
  cmnFTDataObject rawVoltage;
  int counter, counter2;
  for (counter = 0; counter < 7; counter++) {
    BiasVoltage[counter] = 0;
  }
  for (counter2 = 0; counter2 < NumSamples; counter2++) {
    GetForcesTorques(rawVoltage);
    for(counter=0; counter<7; counter++) 
      BiasVoltage[counter] += (float)rawVoltage.FTData[counter];
  }
  for (counter = 0; counter < 7; counter++) {
    BiasVoltage[counter] /= NumSamples;
  }
  LowLevelObject.Bias(CalibrationStruct, BiasVoltage);
  IsBiased=true;
  return true;
}


bool ddiATIDAQFT::LoadCalibration(const char *calFilePath)
{
  CalibrationStruct = LowLevelObject.CreateCalibration(calFilePath, CalibrationIndex);
  if (CalibrationStruct != NULL) {
    IsCalibrated = true;
    return true;
  } else {
    ATIStatus = CAL_FILE_ERROR;
  }
  return false;
}


bool ddiATIDAQFT::SetForceUnits(const std::string &units)
{
  int ret;
  ret = LowLevelObject.SetForceUnits(CalibrationStruct, units);
  if (ret > 0) {
    if (ret == 1) ATIStatus = CAL_FILE_ERROR;
    else if (ret == 2) ATIStatus = INVALID_UNITS;
    else ATIStatus = UNKOWN_ERROR;
  }
  return true;
}


bool ddiATIDAQFT::SetTorqueUnits(const std::string &units)
{
  int ret;
  ret = LowLevelObject.SetTorqueUnits(CalibrationStruct, units);
  if (ret > 0) {
    if (ret == 1) ATIStatus = CAL_FILE_ERROR;
    else if (ret == 2) ATIStatus = INVALID_UNITS;
    else ATIStatus = UNKOWN_ERROR;
  }
  return true;
}


bool ddiATIDAQFT::SetTemperatureCompensation(bool yes)
{
  int ret;
  ret = LowLevelObject.SetTempComp(CalibrationStruct, (yes)?1 :0);
  if (ret > 0) {
    ATIStatus = FAILED_TEMP_COMP;
  }
  return true;
}


#endif // CISST_HAS_ATIDAQFT


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiATIDAQFT.cpp,v $
// Revision 1.4  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/09/07 08:22:09  kapoor
// ddiDeviceInterface: Minor fix in ATI DAQ interface. Added SI to Enc and Enc To SI conversion for MEI.
//
// Revision 1.2  2005/06/19 21:39:32  kapoor
// ddiDeviceInterface: a working version of ati ft daq (windows) interface.
//
// Revision 1.1  2004/05/27 17:09:25  anton
// Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
// The build process will have to be redone whenever will will add dynamic
// loading.
//
//
// Revision 1.1  2004/04/08 06:06:47  kapoor
// Added ATI F/T DAQ DI
//
// Revision 1.2  2004/03/24 21:00:28  kapoor
// Return a command object instead of a member function pointer. This was done
// to handle composite devices. The command object contains the information
// on the deivce that has to handle the operation besides just the operation,
// which is needed to resolve who should handle the operaion in case of a
// composite device.
//
// Revision 1.1  2004/03/22 00:30:53  kapoor
// Added new devices.
//
// ****************************************************************************
