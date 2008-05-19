/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiLoPoMoCo.cpp,v 1.20 2006/07/09 04:08:47 kapoor Exp $
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


#include <cisstDeviceInterface/ddiDMM16AT.h>
//#if CISST_HAS_LOPOMOCO

#include <cisstCommon/cmnXMLPath.h>
#include <cisstDeviceInterface/BoardIO.h>
#include <cisstDeviceInterface/Offsets.h>
#include <limits.h>


CMN_IMPLEMENT_SERVICES(ddiDMM16AT);


ddiDMM16AT::ddiDMM16AT(const char* deviceName):
    ddiDeviceInterface(deviceName)
{
    /*
  MotorVoltages.Data.SetAll(0);
    DDI_ADD_METHOD_COMMAND0(ddiLoPoMoCo, LatchEncoders);
    DDI_ADD_METHOD_COMMAND0(ddiLoPoMoCo, StartMotorCurrentConv);
    DDI_ADD_METHOD_COMMAND0(ddiLoPoMoCo, StartPotFeedbackConv);

    DDI_ADD_METHOD_COMMAND1(ddiLoPoMoCo, GetPositions);
    DDI_ADD_METHOD_COMMAND1(ddiLoPoMoCo, GetVelocities);
    DDI_ADD_METHOD_COMMAND1(ddiLoPoMoCo, GetMotorCurrents);
    DDI_ADD_METHOD_COMMAND1(ddiLoPoMoCo, GetPotFeedbacks);

    DDI_ADD_METHOD_COMMAND1(ddiLoPoMoCo, SetMotorVoltages);
    DDI_ADD_METHOD_COMMAND1(ddiLoPoMoCo, SetCurrentLimits);
    DDI_ADD_METHOD_COMMAND1(ddiLoPoMoCo, Enable);
    DDI_ADD_METHOD_COMMAND1(ddiLoPoMoCo, Disable);
    DDI_ADD_METHOD_COMMAND1(ddiLoPoMoCo, ResetEncoders);
    DDI_ADD_METHOD_COMMAND1(ddiLoPoMoCo, SetPositions);

    DDI_ADD_METHOD_COMMAND0(ddiLoPoMoCo, LoadMotorVoltages);
    DDI_ADD_METHOD_COMMAND0(ddiLoPoMoCo, LoadCurrentLimits);
    DDI_ADD_METHOD_COMMAND0(ddiLoPoMoCo, LoadMotorVoltagesCurrentLimits);

    DDI_ADD_METHOD_COMMAND2(ddiLoPoMoCo, FrequencyToRPS);
    DDI_ADD_METHOD_COMMAND2(ddiLoPoMoCo, ADCToMotorCurrents);
    DDI_ADD_METHOD_COMMAND2(ddiLoPoMoCo, ADCToPotFeedbacks);
    DDI_ADD_METHOD_COMMAND2(ddiLoPoMoCo, MotorVoltagesToDAC);
    DDI_ADD_METHOD_COMMAND2(ddiLoPoMoCo, CurrentLimitsToDAC);
  */
  
  DDI_ADD_METHOD_COMMAND1(ddiDMM16AT, GetInputAll);


//GSF
  // Only once per program, not per board
  dscInit( DSC_VERSION );
}

ddiDMM16AT::~ddiDMM16AT() {
  free(ADC_samples);
  dscFree();
}

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


void ddiDMM16AT::Configure(const char *filename) {
    
    cmnXMLPath xmlConfig;
    if (!filename) {
  CMN_LOG_CLASS(2) << "Warning, could not configure DMM16AT device" << std::endl;
  return;
    }
    
    struct stat st;
    if (stat(filename, &st) < 0) {
      CMN_LOG_CLASS(2) << "Invalid filename!! " << filename << std::endl;
      return;
    }
    

  
    xmlConfig.SetInputSource(filename);
    //char path[60];
    std::string context("/Config/Device[@Name=\"");
    context = context + GetName() + "\"]";

    CMN_LOG_CLASS(3) << "Configuring DMM16AT Board with \"" << filename << "\"" << std::endl;
    //read in the base address and axis factors
    xmlConfig.GetXMLValue(context.c_str(), "@BaseAddress", BaseAddress);
    //int SelectorSwitchPosition;
    //if (ret == false) {
  //    // try switch selector
  //    xmlConfig.GetXMLValue(context.c_str(), "@SelectorSwitchPosition", SelectorSwitchPosition);
  //    BaseAddress = 0x200 + SelectorSwitchPosition * 0x20;
    //}
    
  //xmlConfig.GetXMLValue(context.c_str(), "@StartChan", LowChannel);
    //xmlConfig.GetXMLValue(context.c_str(), "@EndChan", HighChannel);
//TEMP
  LowChannel = 0;
  HighChannel = 15;
    CMN_LOG_CLASS(3) << "BaseAddress: " << BaseAddress << std::endl;
    CMN_LOG_CLASS(3) << "Low Channel: " << LowChannel << " ,  High Channel: " << HighChannel << std::endl;

    //MaxAxis = EndAxis - StartAxis;
    //Board = new ddiLoPoMoCoLowLevelIO(BaseAddress); 
    //Board = new BoardIO(BaseAddress);
    //int version = Board->BoardVersion();
    // the latest version return 0xb0b || 0xS0 where S = ~(switch value)
    // version 1 boards return 0xa0a
    // some board with 10K50E also return 0xc0c
    //CMN_LOG_CLASS(3) << "Version: " << version << std::endl;
    //if (!( ( version & 0xFF0F )== 0xb0b || (version == 0xa0a) || (version == 0xc0c) ))
  //    CMN_LOG_CLASS(3) << "WARNING: Couldnt find a LoPoMoCo board at address (decimal) " << BaseAddress << std::endl;
    
    //unsigned short listEncoders[] = {1, 1, 1, 1};
    //Board->SetADInterruptNumber(0x05);
    //Board->SetTimerInterruptNumber(0x05);
    //Board->SetADInterrupt(false);
    //Board->SetTimerInterrupt(false);
    //Board->SetWDTPeriod(0x2FF);
    //Board->SetEncoderPreloadRegister(0x007FFFFF);
    //Board->PreLoadEncoders(listEncoders);
    //Board->DisableAllMotors();
    //Board->EnableAllMotors();
/*
    //read in the conversion factors
    for (unsigned int axis = 0; axis < 4; axis++) {
  sprintf(path, "Axis[%d]/Encoder/@FrequencyToRPS", axis+1);
  xmlConfig.GetXMLValue(context.c_str(), path, FrequencyToRPSRatio[axis]);
  // anton sprintf(path, "Axis[%d]/Encoder/@ShaftRevToEncoder", axis+1);
  // anton xmlConfig.GetXMLValue(context.c_str(), path, CountsToDeg[axis]);
  
  sprintf(path, "Axis[%d]/ADC/@CountsToMotorCurrents", axis+1);
  xmlConfig.GetXMLValue(context.c_str(), path, CountsToMotorCurrents[axis]);
  sprintf(path, "Axis[%d]/ADC/@CountsToPotFeedback", axis+1);
  xmlConfig.GetXMLValue(context.c_str(), path, CountsToPotFeedback[axis]);
  
  sprintf(path, "Axis[%d]/DAC/@MotorSpeedToCounts", axis+1);
  xmlConfig.GetXMLValue(context.c_str(), path, MotorSpeedToCounts[axis]);
  sprintf(path, "Axis[%d]/DAC/CurrentLimitToCounts/@PositiveSlope", axis+1);
  xmlConfig.GetXMLValue(context.c_str(), path, PositiveSlope[axis]);
  sprintf(path, "Axis[%d]/DAC/CurrentLimitToCounts/@PositiveIntercept", axis+1);
  xmlConfig.GetXMLValue(context.c_str(), path, PositiveIntercept[axis]);
  sprintf(path, "Axis[%d]/DAC/CurrentLimitToCounts/@NegativeSlope", axis+1);
  xmlConfig.GetXMLValue(context.c_str(), path, NegativeSlope[axis]);
  sprintf(path, "Axis[%d]/DAC/CurrentLimitToCounts/@NegativeIntercept", axis+1);
  xmlConfig.GetXMLValue(context.c_str(), path, NegativeIntercept[axis]);
  sprintf(path, "Axis[%d]/DAC/CurrentLimitToCounts/@VoltageToCounts", axis+1);
  xmlConfig.GetXMLValue(context.c_str(), path, VoltageToCounts[axis]);
    }
  */

  /////
  // Retrieve these settings from config file
  /////
  // Initialize board
  //dsccb.base_address = 0x300;
  dsccb.base_address = BaseAddress;
  dsccb.int_level = 3;
  dscInitBoard(DSC_DMM16AT, &dsccb, &dscb);

  memset(&dscadsettings, 0, sizeof(DSCADSETTINGS));

  // Initialize settings
  dscadsettings.range = RANGE_10;
  dscadsettings.polarity = BIPOLAR;
  dscadsettings.gain = GAIN_1;
  dscadsettings.load_cal = (BYTE)FALSE;
  dscadsettings.current_channel = 0;
  result = dscADSetSettings( dscb, &dscadsettings );

  memset(&dscadscan, 0, sizeof(DSCADSCAN));
  //dscadscan.low_channel = 0;
  //dscadscan.high_channel = 15;
  dscadscan.low_channel = LowChannel;
  dscadscan.high_channel = HighChannel;
  dscadscan.gain = dscadsettings.gain ;

  ADC_samples = (DSCSAMPLE*)malloc( sizeof(DSCSAMPLE) * 16 );

    CMN_LOG_CLASS(3) << "Configured DMM16AT" << std::endl;
}

/*

DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiLoPoMoCo, Enable)
    if (pdata->Data >= StartAxis && pdata->Data <= EndAxis) {
  CMN_LOG_CLASS(7) << "Enabling motor " << pdata->Data << std::endl;
  Board->EnableMotor(pdata->Data - StartAxis);
    } else {
  CMN_LOG_CLASS(5) << "Enabling motor " << pdata->Data
       << " failed since the index is out of range ["
       << StartAxis << ", " << EndAxis << "]" << std::endl;
  return ddiCommandBase::DEV_NOT_OK;
    }
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiLoPoMoCo, Disable)
    if (pdata->Data >= StartAxis && pdata->Data <= EndAxis) {
  CMN_LOG_CLASS(7) << "Disabling motor " << pdata->Data << std::endl;
  Board->DisableMotor(pdata->Data - StartAxis);
    } else {
  CMN_LOG_CLASS(5) << "Disabling motor " << pdata->Data
       << " failed since the index is out of range ["
       << StartAxis << ", " << EndAxis << "]" << std::endl;
  return ddiCommandBase::DEV_NOT_OK;
    }
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiLoPoMoCo, ResetEncoders)
    unsigned short listEncoders[] = {0, 0, 0, 0};
    if (pdata->Data >= StartAxis && pdata->Data <= EndAxis) {
  CMN_LOG_CLASS(7) << "Resetting encoder " << pdata->Data << std::endl;
  Board->SetEncoderIndices(false, 0x00, pdata->Data-StartAxis);
  listEncoders[pdata->Data-StartAxis] = 1;
  Board->SetEncoderPreloadRegister(0x007FFFFF);
  Board->PreLoadEncoders(listEncoders);
  //Board->SetEncoderIndices(true, MaxAxis, 0x00);
    } else {
  CMN_LOG_CLASS(5) << "Resetting encoder " << pdata->Data
       << " failed since the index is out of range ["
       << StartAxis << ", " << EndAxis << "]" << std::endl;
  return ddiCommandBase::DEV_NOT_OK;
    }
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND0(ddiLoPoMoCo, LatchEncoders)
    Board->LatchEncoders();
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND0

DDI_BEGIN_IMPLEMENT_COMMAND0(ddiLoPoMoCo, StartMotorCurrentConv)
    Board->StartConvCurrentFeedback();
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND0


DDI_BEGIN_IMPLEMENT_COMMAND0(ddiLoPoMoCo, StartPotFeedbackConv)
    Board->StartConvPotFeedback();
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND0


// for now the velocities need to be read before the encoder positions
// are read. This is because the FPGA has the same index register for
// all three.  So we cache the period and frequency, which is against
// the principals of not caching data in the hardware device
// driver. Hum, should we rethink this, or let this be an exception
// and allow such exception with the goodwill assumption that they
// would be documented where needed.
DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ddiLoPoMoCo, GetPositions)
    //Board->SetEncoderIndices(true, MaxAxis, 0x00);
    for (unsigned int axis = 0; axis <= MaxAxis; axis++) {
        //EncoderPeriods.Data[axis] = Board->GetEncoderPeriod();
        //EncoderFrequencies.Data[axis] = Board->GetEncoderFrequency();
        Board->SetEncoderIndices(false, MaxAxis, axis);
        pdata->Data[axis+StartAxis] = Board->GetEncoder() - 0x007FFFFF;
    }
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ddiLoPoMoCo, GetVelocities)
    for (unsigned int axis = 0; axis <= MaxAxis; axis++)
  pdata->Data[axis+StartAxis] = EncoderFrequencies.Data[axis];
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ddiLoPoMoCo, GetMotorCurrents)
    bool ADInterruptPending = false;
    Board->SetCurrentFeedbackMaxIndex(MaxAxis);
    ADInterruptPending = Board->PollADInterruptPending(5*MaxAxis);
    if (ADInterruptPending == true) {
  for (unsigned int axis = 0; axis <= MaxAxis; axis++)
      pdata->Data[axis+StartAxis] = Board->GetADFIFO();
    }
    return (ADInterruptPending?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ddiLoPoMoCo, GetPotFeedbacks)
    bool ADInterruptPending = false;
    Board->SetPotFeedbackMaxIndex(MaxAxis);
    ADInterruptPending = Board->PollADInterruptPending(5*MaxAxis);
    if (ADInterruptPending == true) {
  for (unsigned int axis = 0; axis <= MaxAxis; axis++)
      pdata->Data[axis+StartAxis] = Board->GetADFIFO();
    }
    return (ADInterruptPending?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiLoPoMoCo, SetMotorVoltages)
    //Board->SetMotorVoltageIndices(true, MaxAxis, 0x00);
    //cached the motor volatages because they are required for converting
    //current limits to dac counts
    for (unsigned int axis = 0; axis <= MaxAxis; axis++) {
        Board->SetMotorVoltageIndices(false, MaxAxis, axis);
        Board->SetMotorVoltage(pdata->Data[axis+StartAxis]);
        MotorVoltages.Data[axis] = pdata->Data[axis+StartAxis];
  //if (MotorVoltages.Data[axis] >= 4090) MotorVoltages.Data[axis] = 4090;
  //if (MotorVoltages.Data[axis] <= -4090) MotorVoltages.Data[axis] = -4090;
    }
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiLoPoMoCo, SetCurrentLimits)
    //Board->SetCurrentLimitIndices(true, MaxAxis, 0x00);
    for (unsigned int axis = 0; axis <= MaxAxis; axis++) {
        Board->SetCurrentLimitIndices(false, MaxAxis, axis);
        Board->SetCurrentLimit(pdata->Data[axis+StartAxis]);
    }
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1


DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiLoPoMoCo, SetPositions)
    // use auto increment and set one by one
    unsigned short listEncoders[] = {0, 0, 0, 0};
    //Board->SetEncoderIndices(true, MaxAxis, 0x00);
    for (unsigned int axis = 0; axis <= MaxAxis; axis++) {
        Board->SetEncoderIndices(false, MaxAxis, axis);
        listEncoders[axis] = 1;
        Board->SetEncoderPreloadRegister(pdata->Data[StartAxis + axis] + 0x007FFFFF);
        Board->PreLoadEncoders(listEncoders);
        listEncoders[axis] = 0;
    }
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1


DDI_BEGIN_IMPLEMENT_COMMAND0(ddiLoPoMoCo, LoadMotorVoltages)
    Board->LoadMotorVoltages();
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND0

DDI_BEGIN_IMPLEMENT_COMMAND0(ddiLoPoMoCo, LoadCurrentLimits)
    Board->LoadCurrentLimits();
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND0

DDI_BEGIN_IMPLEMENT_COMMAND0(ddiLoPoMoCo, LoadMotorVoltagesCurrentLimits)
    Board->LoadMotorVoltagesCurrentLimits();
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND0

DDI_BEGIN_IMPLEMENT_COMMAND2(ddiLoPoMoCo, FrequencyToRPS)
    for (unsigned int axis = 0; axis <= MaxAxis; axis++) {
  //1/DT; f_{count clk} = 1.04125;
  todata->Data[axis+StartAxis] = FrequencyToRPSRatio[axis]/((double)(fromdata->Data[axis+StartAxis]));
    }
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND2

DDI_BEGIN_IMPLEMENT_COMMAND2(ddiLoPoMoCo, ADCToMotorCurrents)
    for (unsigned int axis = 0; axis <= MaxAxis; axis++) {
  todata->Data[axis+StartAxis] = CountsToMotorCurrents[axis]*(double)(fromdata->Data[axis+StartAxis]);
    }
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND2

DDI_BEGIN_IMPLEMENT_COMMAND2(ddiLoPoMoCo, ADCToPotFeedbacks)
    for (unsigned int axis = 0; axis <= MaxAxis; axis++) {
  todata->Data[axis+StartAxis] = CountsToPotFeedback[axis]*(double)(fromdata->Data[axis+StartAxis]);
    }
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND2

DDI_BEGIN_IMPLEMENT_COMMAND2(ddiLoPoMoCo, MotorVoltagesToDAC)
    double dacCounts;
    for (unsigned int axis = 0; axis <= MaxAxis; axis++) {
  dacCounts = MotorSpeedToCounts[axis]*fromdata->Data[axis+StartAxis];
  if (dacCounts >= 4090) dacCounts = 4090;
  if (dacCounts <= -4090) dacCounts = -4090;
  todata->Data[axis+StartAxis] = (short)(dacCounts);
    }
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND2

DDI_BEGIN_IMPLEMENT_COMMAND2(ddiLoPoMoCo, CurrentLimitsToDAC)
    double dac2InVolts;
    for (unsigned int axis = 0; axis <= MaxAxis; axis++) {
  if (MotorVoltages.Data[axis] >=0 ) {
      dac2InVolts = PositiveSlope[axis]*fromdata->Data[axis+StartAxis] + PositiveIntercept[axis];
  } else {
      dac2InVolts = NegativeSlope[axis]*fromdata->Data[axis+StartAxis] + NegativeIntercept[axis];
  }
  todata->Data[axis+StartAxis] = (short)(VoltageToCounts[axis]*dac2InVolts);
    }
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND2

*/

//GSF
DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ddiDMM16AT, GetInputAll)
    
  result = dscADScan(dscb, &dscadscan, ADC_samples);

  int ctr=0;
    for(int i=0; i<16; i++){
        if ( (i>=LowChannel) && (i<=HighChannel) ){
        dscADCodeToVoltage(dscb, dscadsettings, dscadscan.sample_values[ctr], &ADC_voltage);
            ctr++;
        pdata->Data[i] = ADC_voltage;
        } else {
        pdata->Data[i] = 0;
        }
  }

    
    return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND2


//#endif // CISST_HAS_LOPOMOCO


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: ddiLoPoMoCo.cpp,v $
// Revision 1.20  2006/07/09 04:08:47  kapoor
// ddiDeviceInterface: ddiLoPoMoCo, fixes get/set encoder and set motor voltages
// and currents.
//
// Revision 1.19  2006/06/01 18:43:11  kapoor
// cisstDeviceInterface: Simplified the marco for the commands. Only
// one macro requires input and/or output types. The others just the
// class and method name.
//
// Revision 1.18  2006/05/14 03:21:14  kapoor
// cisstDeviceInterface: ddiLoPoMoCo, fixed typo in command name.
//
// Revision 1.17  2006/05/13 21:27:58  kapoor
// cisstDeviceInterface and LoPoMoCo: Warning that board is not found was
// being generated in error.
//
// Revision 1.16  2006/05/11 18:45:36  anton
// ddiMEI and ddiLoPoMoCo: Updated for new class registration.
//
// Revision 1.15  2006/05/09 03:29:47  kapoor
// cisstDeviceInterface: Added RTTI typeinfo of expected input/output to
// ddiCommand object. This allows checking for correct type in the mailbox
// command.
//
// Revision 1.14  2006/05/07 04:45:26  kapoor
// cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
// Revision 1.13  2006/05/05 05:09:38  kapoor
// cisstDeviceInterface: See checkin [1945] <--cool number.
//
// Revision 1.12  2006/05/05 03:35:08  kapoor
// cisstDeviceInterface: COMMAND_1 is replaced by 1_IN and 1_OUT to specify
// read and write operations. This macro only changes the "C" like API that
// is provided for the wrapping of device class, such that the output is
// the can be assigned directly.
//
// Revision 1.11  2006/05/03 05:35:03  kapoor
// Version using MACROS for python API and protected methods.
//
// Revision 1.10  2006/05/03 02:28:34  kapoor
// These compile. For easy tranfer to Anton. No gurantees on functionality.
//
// Revision 1.9  2006/05/02 20:35:22  anton
// Data objects for ddi and rts: Moved data definition to the device interface
// level and renamed the two classes of "vectors".  Updated LoPoMoCo to verify
// compilation.
//
// Revision 1.8  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.7  2005/05/28 03:21:31  kapoor
// ddiLoPoMoCo.cpp: Log a warning if board doesnt return expected version info.
//
// Revision 1.6  2005/04/16 21:36:36  kapoor
// Added check for valid filename
//
// Revision 1.5  2005/03/15 04:38:38  anton
// ddiLoPoMoCo: Added SetPositions (whatever).
//
// Revision 1.4  2005/03/15 02:07:32  anton
// ddiLoPoMoCo:  Bug correction to start potentiometer reading (Thanks to Ankur)
//
// Revision 1.3  2005/02/28 22:33:10  anton
// ddiLoPoMoCo:  Added LOG (see ticket #130) and use DDI_LOPOMOCO_NB_AXIS to
// set the lenght of data arrays.
//
// Revision 1.2  2005/01/12 04:57:39  kapoor
// Fixed bug in resetting encoder. See Ticket #117
//
// Revision 1.1  2004/05/27 17:09:25  anton
// Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
// The build process will have to be redone whenever will will add dynamic
// loading.
//
//
// Revision 1.3  2004/04/08 06:11:47  kapoor
// Added conditional compilation
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
