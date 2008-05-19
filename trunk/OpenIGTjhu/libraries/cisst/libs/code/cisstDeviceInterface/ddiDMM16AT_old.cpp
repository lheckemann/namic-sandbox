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
//#include "nidaq.h"
//#include "nidaqex.h"
//#include "nidaqerr.h"
#include "dscud.h"
}

#include <cisstDeviceInterface/ddiDMM16AT.h>
#include <cisstCommon/cmnXMLPath.h>
CMN_IMPLEMENT_SERVICES(ddiDMM16AT, 5);

ddiDMM16AT::ddiDMM16AT():
    ddiDeviceInterface("DMM16AT"),
    DAQStatus(0), RetVal(0), Device(1), Channel(0), Gain(1),
    IgnoreWarning(0), NumberChannels(7), IsCalibrated(false),
    IsBiased(false), CalibrationIndex(1), NumSamples(20), 
    Sequence(0) {
  for (int i=0; i < NumberChannels; i++) {
    ChannelVector[i] = i;
    GainVector[i] = 1;
    BiasVoltage[i] = 0;
  }
    /*
  ReadNameMap["ForceTorques"] = FORCETORQUES;
  ReadOperations.resize(ReadOperations.size() + 2);
  ReadOperations[FORCETORQUES] = new ddiCommand<ddiATIDAQFT>(&ddiATIDAQFT::GetForcesTorques, this, "FT");
    
  WriteNameMap["Bias"] = BIAS;
  WriteOperations.resize(ReadOperations.size() + 2);
  WriteOperations[BIAS] = new ddiCommand<ddiATIDAQFT>(&ddiATIDAQFT::SetNewBias, this, "BIAS");
    
  ConversionNameMap["FTCountsToFTNewtons"] = FTCOUNTSTOFTNEWTONS;
  ConversionOperations.resize(ConversionOperations.size() + 2);
  ConversionOperations[FTCOUNTSTOFTNEWTONS] = new ddiCommand2<ddiATIDAQFT>(&ddiATIDAQFT::ConvertFTCountsToFTNewtons, this, "FTCTON");
  */
}


ddiDMM16AT::~ddiDMM16AT() {
  // Only once per program, not per board
  dscFree();
}


/*
std::vector<std::string> ddiDMM16AT::Provides (void) {
  
  std::vector<std::string> *provides = new std::vector<std::string>;
  OperationNameMapType::iterator iter;
  for (iter = ReadNameMap.begin(); iter != ReadNameMap.end(); ++iter) {
    provides->push_back(iter->first);
  }
  return *provides;
}
*/


void ddiDMM16AT::Configure(void) {

  // Only once per program, not per board
  dscInit( DSC_VERSION );
  
  // Retrieve address from higher level program
  dsccb.base_address = 0x300;
  dsccb.int_level = 3;

  dscInitBoard(DSC_DMM16AT, &dsccb, &dscb);

  dscadsettings.range = RANGE_BIPOLAR_10;
  dscadsettings.polarity = BIPOLAR;
  dscadsettings.gain = GAIN_1;
  dscadsettings.load_cal = (BYTE)FALSE;
  
  // Below will change
  dscadsettings.current_channel = 0;

  result = dscADSetSettings( dscb, &dscadsettings );
    
}



bool ddiDMM16AT::GetSingleVoltage(BYTE channel, DFLOAT &voltage) {
  
  DSCSAMPLE sample; // sample reading

  //dscadsettings.current_channel = 0;
  //result = dscADSetSettings( dscb, &dscadsettings );

  // Get raw reading
    result = dscADSample( dscb, &sample );
  
  // Convert to voltage reading
  dscADCodeToVoltage(dscb, dscadsettings, sample, &voltage);

  return true;
}

/*
bool ddiDMM16AT::GetVoltages(BYTE low_channel, BYTE high_channel, DFLOAT &voltage) {
  
  DSCSAMPLE sample; // sample reading

  dscADSetChannel(dscb, low_channel, high_channel);

  // Get raw reading
    result = dscADSample( dscb, &sample );
  
  // Convert to voltage reading
  dscADCodeToVoltage(dscb, dscadsettings, sample, &voltage);

  return true;
}
*/


bool ddiDMM16AT::ConvertVoltageToPressure(cmnDataObject &from, cmnDataObject &to) {
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



#endif // CISST_HAS_DMM16AT


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

