/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// ****************************************************************************
//
//  $Id: ddiMEI.cpp,v 1.15 2006/06/01 18:43:11 kapoor Exp $
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
#if CISST_HAS_MEI

extern "C"
{
  // idsp includes windows.h which includes msxml.h which conflicts with Xerces/xalan. So we must include
  // XMLPath last
#include <idsp.h>
}
#include <cisstDeviceInterface/ddiMEI.h>
#include <cisstVector.h>
#include <cisstCommon/cmnXMLPath.h>

CMN_IMPLEMENT_SERVICES(ddiMEI);

ddiMEI::ddiMEI():
    ddiDeviceInterface("MEI")
{

    DDI_ADD_METHOD_COMMAND1(ddiMEI, IsMoving);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, GetAccelerations);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, GetVelocities);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, GetError);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, GetCommand);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, GetPositions);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, GetGains);

    DDI_ADD_METHOD_COMMAND1(ddiMEI, Stop);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, Abort);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, StopAll);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, AbortAll);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, EnableAxes);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, DisableAxes);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, SetTargetPositions);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, SetMaxVelocities);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, SetMaxAccelerations);
    DDI_ADD_METHOD_COMMAND1(ddiMEI, SetGains);

    DDI_ADD_METHOD_COMMAND0(ddiMEI, LoadTargetPositions);
    DDI_ADD_METHOD_COMMAND0(ddiMEI, LoadTargetVelocities);

    DDI_ADD_METHOD_COMMAND2(ddiMEI, SIToEncoder);
    DDI_ADD_METHOD_COMMAND2(ddiMEI, EncoderToSI);
}


ddiMEI::~ddiMEI() {
}

#include <sys/types.h>
#include <sys/stat.h>

void ddiMEI::Configure(const char *filename) {
    cmnXMLPath xmlConfig;
    if (!filename) {
  CMN_LOG_CLASS(2) << "Warning, could not configure MEI device" << std::endl;
  return;
    }
#if WIN32
    struct _stat st;
    if (_stat(filename, &st) < 0) {
      CMN_LOG_CLASS(2) << "Invalid filename!! " << filename << std::endl;
      return;
    }
#endif
#if CISST_HAS_RTAI
    struct stat st;
    if (stat(filename, &st) < 0) {
      CMN_LOG_CLASS(2) << "Invalid filename!! " << filename << std::endl;
      return;
    }
#endif

    xmlConfig.SetInputSource(filename);
    char path[60];
    std::string context("/Config/Device[@Name=\"");
    context = context + GetName() + "\"]";

    CMN_LOG_CLASS(3) << "Configuring MEI with \"" << filename << "\"" << std::endl;
    //read in the base address and axis factors
    bool ret = xmlConfig.GetXMLValue(context.c_str(), "@BaseAddress", BaseAddress);
    xmlConfig.GetXMLValue(context.c_str(), "@StartAxis", StartAxis);
    xmlConfig.GetXMLValue(context.c_str(), "@EndAxis", EndAxis);
    CMN_LOG_CLASS(3) << "StartAxis: "  << StartAxis << "   EndAxis: " << EndAxis << std::endl
         << "BaseAddress: " << BaseAddress << std::endl;
    MaxAxis = EndAxis - StartAxis;
  if (MaxAxis+1 > 7) CMN_LOG_CLASS(1) << "Supports only 8 axis" << std::endl;

  int result = dsp_init(BaseAddress);
  char error_buffer[MAX_ERROR_LEN];
  if (result != DSP_OK)
  {
    error_msg(result, error_buffer);
    CMN_LOG_CLASS(1) << "MEI Init failed: [" <<  result << "] " << error_buffer << std::endl;
  }
  /*
  result = dsp_reset();
  if (result != DSP_OK) {
    error_msg(result, error_buffer);
    CMN_LOG_CLASS(1) << "MEI Reset failed: [" <<  result << "] " << error_buffer << std::endl;
  }
  */
  int16 axisMap[8];
  for (unsigned int i = 0; i <= MaxAxis; i++) {
    sprintf(path, "Axis[%d]/Amplifier/@Number", i+1);
    xmlConfig.GetXMLValue(context.c_str(), path, AxisNumber[i]);
    axisMap[i] = AxisNumber.Data[i]-1;
  }
  result = map_axes(MaxAxis+1, &axisMap[0]);
  if (result != DSP_OK) {
    error_msg(result, error_buffer);
    CMN_LOG_CLASS(1) << "MEI Map failed: [" <<  result << "] " << error_buffer << std::endl;
  }

  // stuff for individual axis now
  int16 filterCoeffs[COEFFICIENTS];
  for (unsigned int i = 0; i <= MaxAxis; i++) {
    result = set_amp_enable_level(i, 0); //set amp enable to low state
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI AmpLevel failed: [" <<  result << "] " << error_buffer << std::endl;
    }
    result = disable_amplifier(i);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI Disable failed: [" <<  result << "] " << error_buffer << std::endl;
    }
        double CountsPerTurn, GearRatio, Pitch;
        sprintf(path, "Axis[%d]/Encoder/@CountsPerTurn", i+1);
        xmlConfig.GetXMLValue(context.c_str(), path, CountsPerTurn);
        sprintf(path, "Axis[%d]/Encoder/@GearRatio", i+1);
        xmlConfig.GetXMLValue(context.c_str(), path, GearRatio);
        sprintf(path, "Axis[%d]/Encoder/@Pitch", i+1);
        xmlConfig.GetXMLValue(context.c_str(), path, Pitch);
        sprintf(path, "Axis[%d]/Encoder/@OffsetInSI", i+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, OffsetInSI[i]);
        if (!ret) OffsetInSI[i] = 0;
        /*
         * DegsPerCount here atcually represents counts per unit motion of the
         * joint. If it is translatory joint it would mean counts per mm
         * and rotary joint it would mean counts per deg.
         * it is computed once during reading of comfiguration file using
         * the formula
         * DegsPerCount = Pitch *  (1/GearRatio) * (360) * (1/CountsPerTurn)
         * Pitch is in MM per Deg for translatory joints
         * Pitch is 1 for rotary joints
         */
        DegsPerCount[i] = (Pitch * 360.0)/(GearRatio * CountsPerTurn);

    sprintf(path, "Axis[%d]/Filter/@PGain", i+1);
    ret = xmlConfig.GetXMLValue(context.c_str(), path, Gains[i].PGain);
    if (!ret) Gains[i].PGain = 0;
    sprintf(path, "Axis[%d]/Filter/@DGain", i+1);
    xmlConfig.GetXMLValue(context.c_str(), path, Gains[i].DGain);
    if (!ret) Gains[i].DGain = 0;
    sprintf(path, "Axis[%d]/Filter/@IGain", i+1);
    xmlConfig.GetXMLValue(context.c_str(), path, Gains[i].IGain);
    if (!ret) Gains[i].IGain = 0;
    sprintf(path, "Axis[%d]/Filter/@ILimit", i+1);
    xmlConfig.GetXMLValue(context.c_str(), path, Gains[i].ILimit);
    if (!ret) Gains[i].ILimit = 32767; // Default Sets the ILimit to 1V
    sprintf(path, "Axis[%d]/Filter/@DACLimit", i+1);
    xmlConfig.GetXMLValue(context.c_str(), path, Gains[i].DACLimit);
    if (!ret) Gains[i].DACLimit = 32767;
    sprintf(path, "Axis[%d]/Filter/@OffsetVoltage", i+1);
    xmlConfig.GetXMLValue(context.c_str(), path, Gains[i].Offset);
    if (!ret) Gains[i].Offset = 0.0;
    sprintf(path, "Axis[%d]/Filter/@OScale", i+1);
    xmlConfig.GetXMLValue(context.c_str(), path, Gains[i].OScale);
    if (!ret) Gains[i].OScale = 0;
    filterCoeffs[DF_P] = (int16)Gains[i].PGain;
    filterCoeffs[DF_I] = (int16)Gains[i].IGain;
    filterCoeffs[DF_D] = (int16)Gains[i].DGain;
    filterCoeffs[DF_ACCEL_FF] = (int16)Gains[i].AccelerationFF;
    filterCoeffs[DF_VEL_FF] = (int16)Gains[i].VelocityFF;
    filterCoeffs[DF_I_LIMIT] = (int16)Gains[i].ILimit;
    filterCoeffs[DF_OFFSET] = (int16)Gains[i].Offset;
    filterCoeffs[DF_DAC_LIMIT] = (int16)Gains[i].DACLimit;
    filterCoeffs[DF_SHIFT] = (int16)Gains[i].OScale;
    filterCoeffs[DF_FRICT_FF] = (int16)Gains[i].FrictionFF;
    result = set_filter(i, &filterCoeffs[0]);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI SetFilter failed: [" <<  result << "] " << error_buffer << std::endl;
    }
    /*
    result = set_unipolar(i, FALSE);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI Configure failed: [" <<  result << "] " << error_buffer << std::endl;
    }
    result = dsp_set_stepper(i, FALSE);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI Configure failed: [" <<  result << "] " << error_buffer << std::endl;
    }
    result = dsp_set_closed_loop(i, FALSE);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI Configure failed: [" <<  result << "] " << error_buffer << std::endl;
    }
    result = set_feedback(i, FB_ENCODER);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI Configure failed: [" <<  result << "] " << error_buffer << std::endl;
    }
    result = dsp_set_step_speed(i, DISABLE_STEP_OUTPUT);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI Configure failed: [" <<  result << "] " << error_buffer << std::endl;
    }
    result = set_home_index_config(i, HOME_ONLY);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI Configure failed: [" <<  result << "] " << error_buffer << std::endl;
    }
    result = set_integration(i, IM_STANDING);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI Configure failed: [" <<  result << "] " << error_buffer << std::endl;
    }
    */
    result = set_positive_limit(i, E_STOP_EVENT);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI Configure failed: [" <<  result << "] " << error_buffer << std::endl;
    }
    result = set_negative_limit(i, E_STOP_EVENT);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI Configure failed: [" <<  result << "] " << error_buffer << std::endl;
    }
    long loop;
    loop = 0;
    while (! motion_done(i) ) {
      loop++; 
      if (loop > 10000) {
        CMN_LOG_CLASS(1) << "Waited too long for motion" << std::endl;
        break;
      }
    }
    result = clear_status(i);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI Configure failed: [" <<  result << "] " << error_buffer << std::endl;
    }
    loop = 0;
    while (! motion_done(i) ) {
      loop++; 
      if (loop > 10000) {
        CMN_LOG_CLASS(1) << "Waited too long for motion" << std::endl;
        break;
      }
    }
    result = controller_run(i);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI Configure failed: [" <<  result << "] " << error_buffer << std::endl;
    }
    while (! motion_done(i) ) {
      loop++; 
      if (loop > 10000) {
        CMN_LOG_CLASS(1) << "Waited too long for motion" << std::endl;
        break;
      }
    }
  }
}




DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ddiMEI, IsMoving)
  for (unsigned int i = 0; i <= MaxAxis; i++) {
    if (in_motion(i) == TRUE) {
      pdata->Data[i] = true;
    } else {
      pdata->Data[i] = false;
    }
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ddiMEI, GetAccelerations)
  char error_buffer[MAX_ERROR_LEN];
  int result;
  for (unsigned int i = 0; i <= MaxAxis; i++) {
    result = get_accel(i, &(pdata->Data[i]));
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI GetAccelerations failed: [" <<  result << "] " << error_buffer << std::endl;
    }
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1


DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ddiMEI, GetVelocities)
  char error_buffer[MAX_ERROR_LEN];
  int result;
  for (unsigned int i = 0; i <= MaxAxis; i++) {
    result = get_velocity(i, &(pdata->Data[i]));
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI GetVelocities failed: [" <<  result << "] " << error_buffer << std::endl;
    }
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1


DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ddiMEI, GetError)
  char error_buffer[MAX_ERROR_LEN];
  int result;
  for (unsigned int i = 0; i <= MaxAxis; i++) {
    result = get_error(i, &(pdata->Data[i]));
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI GetError failed: [" <<  result << "] " << error_buffer << std::endl;
    }
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1


DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ddiMEI, GetCommand)
  char error_buffer[MAX_ERROR_LEN];
  int result;
  for (unsigned int i = 0; i <= MaxAxis; i++) {
    result = get_command(i, &(pdata->Data[i]));
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI GetCommand failed: [" <<  result << "] " << error_buffer << std::endl;
    }
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1


DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ddiMEI, GetPositions)
  char error_buffer[MAX_ERROR_LEN];
  int result;
  for (unsigned int i = 0; i <= MaxAxis; i++) {
    result = get_position(i, &(pdata->Data[i]));
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI SetFilter failed: [" <<  result << "] " << error_buffer << std::endl;
    }
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1


DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiMEI, Stop)
  char error_buffer[MAX_ERROR_LEN];
  int result = set_stop(pdata->Data);
  if (result != DSP_OK) {
    error_msg(result, error_buffer);
    CMN_LOG_CLASS(1) << "MEI Stop failed: [" <<  result << "] " << error_buffer << std::endl;
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiMEI, Abort)
  char error_buffer[MAX_ERROR_LEN];
  int result = controller_idle(pdata->Data);
  if (result != DSP_OK) {
    error_msg(result, error_buffer);
    CMN_LOG_CLASS(1) << "MEI Abort failed: [" <<  result << "] " << error_buffer << std::endl;
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiMEI, StopAll)
  int result;
  char error_buffer[MAX_ERROR_LEN];
  for (unsigned int i = 0; i <= MaxAxis; i++) {
    if (pdata->Data[i]) {
      result = set_stop(i);
      if (result != DSP_OK) {
        error_msg(result, error_buffer);
        CMN_LOG_CLASS(1) << "MEI Stop failed: [" <<  result << "] " << error_buffer << std::endl;
      }
    }
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiMEI, AbortAll)
  int result;
  char error_buffer[MAX_ERROR_LEN];
  for (unsigned int i = 0; i <= MaxAxis; i++) {
    if (pdata->Data[i]) {
      result = controller_idle(i);
      if (result != DSP_OK) {
        error_msg(result, error_buffer);
        CMN_LOG_CLASS(1) << "MEI Abort failed: [" <<  result << "] " << error_buffer << std::endl;
      }
    }
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1


DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiMEI, EnableAxes)
  int result;
  char error_buffer[MAX_ERROR_LEN];
  for (unsigned int i = 0; i <= MaxAxis; i++) {
    if (pdata->Data[i]) {
      result = enable_amplifier(i);
      if (result != DSP_OK) {
        error_msg(result, error_buffer);
        CMN_LOG_CLASS(1) << "MEI Enable axis failed: [" <<  result << "] " << error_buffer << std::endl;
      }
    }
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiMEI, DisableAxes)
  char error_buffer[MAX_ERROR_LEN];
  int result;
  for (unsigned int i = 0; i <= MaxAxis; i++) {
    if (pdata->Data[i]) {
      result = disable_amplifier(i);
      if (result != DSP_OK) {
        error_msg(result, error_buffer);
        CMN_LOG_CLASS(1) << "MEI Disable axis failed: [" <<  result << "] " << error_buffer << std::endl;
      }
    }
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiMEI, SetTargetPositions)
        TargetPositions = *pdata;
        return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiMEI, SetMaxVelocities)
        MaxVelocities = *pdata;
        return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiMEI, SetMaxAccelerations)
        MaxAcceleration = *pdata;
        return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

DDI_BEGIN_IMPLEMENT_COMMAND0(ddiMEI, LoadTargetPositions)
  int result;
  char error_buffer[MAX_ERROR_LEN];
  CMN_LOG_CLASS(3)
    << MaxAxis << std::endl
    << TargetPositions << std::endl
    << MaxVelocities << std::endl
    << MaxAcceleration << std::endl;

  for (unsigned int i = 0; i <= MaxAxis; i++) {
    result = start_move(i, TargetPositions.Data[i], MaxVelocities.Data[i], MaxAcceleration.Data[i]);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI PosMove axis failed: [" <<  result << "] " << error_buffer << std::endl;
    }
  }
  if (result != DSP_OK) {
    error_msg(result, error_buffer);
    CMN_LOG_CLASS(1) << "MEI PosMove axis failed: [" <<  result << "] " << error_buffer << std::endl;
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND0

DDI_BEGIN_IMPLEMENT_COMMAND0(ddiMEI, LoadTargetVelocities)
  int result;
  char error_buffer[MAX_ERROR_LEN];
  for (unsigned int i = 0; i <= MaxAxis; i++) {
    result = v_move(i, MaxVelocities.Data[i], MaxAcceleration.Data[i]);
    if (result != DSP_OK) {
      error_msg(result, error_buffer);
      CMN_LOG_CLASS(1) << "MEI VelMove axis failed: [" <<  result << "] " << error_buffer << std::endl;
    }
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND0

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ddiMEI, GetGains)
  int16 filterCoeffs[COEFFICIENTS];
  char error_buffer[MAX_ERROR_LEN];
  int result = set_filter(pdata->AxisNumber, &filterCoeffs[0]);
  if (result != DSP_OK) {
    error_msg(result, error_buffer);
    CMN_LOG_CLASS(1) << "MEI SetFilter failed: [" <<  result << "] " << error_buffer << std::endl;
  }
  pdata->PGain = filterCoeffs[DF_P];
  pdata->IGain = filterCoeffs[DF_I];
  pdata->DGain = filterCoeffs[DF_D];
  pdata->AccelerationFF = filterCoeffs[DF_ACCEL_FF];
  pdata->VelocityFF = filterCoeffs[DF_VEL_FF];
  pdata->ILimit = filterCoeffs[DF_I_LIMIT];
  pdata->Offset = filterCoeffs[DF_OFFSET];
  pdata->DACLimit = filterCoeffs[DF_DAC_LIMIT];
  pdata->OScale = filterCoeffs[DF_SHIFT];
  pdata->FrictionFF = filterCoeffs[DF_FRICT_FF];
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1


DDI_BEGIN_IMPLEMENT_COMMAND1_IN(ddiMEI, SetGains)
  int16 filterCoeffs[COEFFICIENTS];
  char error_buffer[MAX_ERROR_LEN];
  filterCoeffs[DF_P] = (int16)pdata->PGain;
  filterCoeffs[DF_I] = (int16)pdata->IGain;
  filterCoeffs[DF_D] = (int16)pdata->DGain;
  filterCoeffs[DF_ACCEL_FF] = (int16)pdata->AccelerationFF;
  filterCoeffs[DF_VEL_FF] = (int16)pdata->VelocityFF;
  filterCoeffs[DF_I_LIMIT] = (int16)pdata->ILimit;
  filterCoeffs[DF_OFFSET] = (int16)pdata->Offset;
  filterCoeffs[DF_DAC_LIMIT] = (int16)pdata->DACLimit;
  filterCoeffs[DF_SHIFT] = (int16)pdata->OScale;
  filterCoeffs[DF_FRICT_FF] = (int16)pdata->FrictionFF;
  int result = set_filter(pdata->AxisNumber, &filterCoeffs[0]);
  if (result != DSP_OK) {
    error_msg(result, error_buffer);
    CMN_LOG_CLASS(1) << "MEI SetFilter failed: [" <<  result << "] " << error_buffer << std::endl;
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND1

/* Protected methods that are executed for the corresponsding command */
DDI_BEGIN_IMPLEMENT_COMMAND2(ddiMEI, SIToEncoder)
  for (unsigned int i = 0; i <= MaxAxis; i++) {
        todata->Data(i) = (fromdata->Data(i)-OffsetInSI.Data(i))/DegsPerCount.Data(i);
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND2

DDI_BEGIN_IMPLEMENT_COMMAND2(ddiMEI, EncoderToSI)
  for (unsigned int i = 0; i <= MaxAxis; i++) {
        todata->Data(i) = fromdata->Data(i) * DegsPerCount.Data(i) + OffsetInSI.Data(i);
  }
  return ddiCommandBase::DEV_OK;
DDI_END_IMPLEMENT_COMMAND2

#endif // CISST_HAS_MEI


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: ddiMEI.cpp,v $
//  Revision 1.15  2006/06/01 18:43:11  kapoor
//  cisstDeviceInterface: Simplified the marco for the commands. Only
//  one macro requires input and/or output types. The others just the
//  class and method name.
//
//  Revision 1.14  2006/05/11 18:45:36  anton
//  ddiMEI and ddiLoPoMoCo: Updated for new class registration.
//
//  Revision 1.13  2006/05/09 03:29:47  kapoor
//  cisstDeviceInterface: Added RTTI typeinfo of expected input/output to
//  ddiCommand object. This allows checking for correct type in the mailbox
//  command.
//
//  Revision 1.12  2006/05/07 04:45:26  kapoor
//  cisstDeviceInterface and IRE: Fixed printing of dataobjects through python.
//
//  Revision 1.11  2006/05/05 05:09:38  kapoor
//  cisstDeviceInterface: See checkin [1945] <--cool number.
//
//  Revision 1.10  2006/05/05 03:35:08  kapoor
//  cisstDeviceInterface: COMMAND_1 is replaced by 1_IN and 1_OUT to specify
//  read and write operations. This macro only changes the "C" like API that
//  is provided for the wrapping of device class, such that the output is
//  the can be assigned directly.
//
//  Revision 1.9  2006/05/03 05:35:03  kapoor
//  Version using MACROS for python API and protected methods.
//
//  Revision 1.8  2006/03/07 20:13:23  kapoor
//  ddDeviceInterface: Fixed typos that gave compilation errors in previous commit of
//   ddiMEI.cpp
//
//  Revision 1.7  2006/03/04 19:07:32  kapoor
//  ddDeviceInterface: Split the Encoder to SI unit conversion ratio into three seperate values that are read from the xml-config files. This is the current standard for other servo loops as well, as it makes it easy to change these according to different robots.
//
//  Revision 1.6  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.5  2005/09/07 08:22:09  kapoor
//  ddiDeviceInterface: Minor fix in ATI DAQ interface. Added SI to Enc and Enc To SI conversion for MEI.
//
//  Revision 1.4  2005/07/29 20:02:02  pkaz
//  Update for RTAI
//
//  Revision 1.3  2005/07/01 16:05:11  kapoor
//  ddiMEI.cpp: Missing initalization of variable to zero.
//
//  Revision 1.2  2005/06/19 21:39:15  kapoor
//  ddiDeviceInterface: a working version of mei interface.
//
//  Revision 1.1  2004/05/27 17:09:25  anton
//  Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
//  The build process will have to be redone whenever will will add dynamic
//  loading.
//
//
// ****************************************************************************

