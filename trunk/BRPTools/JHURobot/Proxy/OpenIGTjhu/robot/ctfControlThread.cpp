/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: ctfControlThread.cpp,v 1.11 2006/07/09 04:05:08 kapoor Exp $


#include <sys/io.h>
#include <cisstCommon/cmnXMLPath.h>
#include "ctfControlThread.h"


CMN_IMPLEMENT_SERVICES_TEMPLATED(ddiVecGain16);
CMN_IMPLEMENT_SERVICES(ctfControlThread);


ctfControlThread::ctfControlThread(const char* taskName, double period):
    rtsTask(taskName, period, false, 21000),
    SimulationOnly(1),
    Ticker(0),
    logStreamName(std::string("servo")+taskName+std::string(".csv")),
    logStream(logStreamName.c_str())
{
    rawPositions        = StateDataTable.NewElement<ddiVecLong16 >   ("Raw Positions");
    rawVelocities       = StateDataTable.NewElement<ddiVecShort16 >  ("Raw Velocities");
    rawCurrentFeedbacks = StateDataTable.NewElement<ddiVecShort16 >  ("Raw Current Feedbacks");
    rawPotFeedbacks     = StateDataTable.NewElement<ddiVecShort16 >  ("Raw Pot Feedbacks");
    outputVoltagesDAC   = StateDataTable.NewElement<ddiVecShort16 >  ("Raw Motor Voltages");
    outputCurrentsDAC   = StateDataTable.NewElement<ddiVecShort16 >  ("Raw Motor Currents");
    positions           = StateDataTable.NewElement<ddiVecDouble16 > ("Positions");
  //GSF - did this mess up the order?
    latchedindex        = StateDataTable.NewElement<ddiVecShort16 >  ("Latched Index Positions");
    velocities          = StateDataTable.NewElement<ddiVecDouble16 > ("Velocites");
    currentFeedbacks    = StateDataTable.NewElement<ddiVecDouble16 > ("Current Feedbacks");
    potFeedbacksVolts   = StateDataTable.NewElement<ddiVecDouble16 > ("Pot Feedback Volts");
    potFeedbacksDeg     = StateDataTable.NewElement<ddiVecDouble16 > ("Pot Feedback Deg");
    outputVoltagesVolts = StateDataTable.NewElement<ddiVecDouble16 > ("Motor Voltages");
    outputCurrentsmAmps = StateDataTable.NewElement<ddiVecDouble16 > ("Motor Currents");

    // for servo
    commandPositions = StateDataTable.NewElement<ddiVecDouble16 > ("Command Positions");
    error            = StateDataTable.NewElement<ddiVecDouble16 > ("Errors");
    derror0          = StateDataTable.NewElement<ddiVecDouble16 > ("D Errors");
    derrorFilter     = StateDataTable.NewElement<ddiVecDouble16 > ("D Errors Filtered");
    ierror           = StateDataTable.NewElement<ddiVecDouble16 > ("I Errors");
    overLimit        = StateDataTable.NewElement<ddiVecBool16 >   ("OverLimit");

    CMN_LOG(6) << "CURRENT STATE"
        << std::endl << rawPositions->Index        << "Raw Positions"
        << std::endl << rawVelocities->Index       << "Raw Velocities"
        << std::endl << rawCurrentFeedbacks->Index << "Raw Current Feedbacks"
        << std::endl << rawPotFeedbacks->Index     << "Raw Pot Feedbacks"
        << std::endl << outputVoltagesDAC->Index   << "Raw Motor Voltages"
        << std::endl << outputCurrentsDAC->Index   << "Raw Motor Currents"
        << std::endl << positions->Index           << "Positions"
        << std::endl << latchedindex->Index        << "Latched Index Positions"
        << std::endl << velocities->Index          << "Velocites"
        << std::endl << currentFeedbacks->Index    << "Current Feedbacks"
        << std::endl << potFeedbacksVolts->Index   << "Pot Feedback Volts"
        << std::endl << potFeedbacksDeg->Index     << "Pot Feedback Deg"
        << std::endl << outputVoltagesVolts->Index << "Motor Voltages"
        << std::endl << outputCurrentsmAmps->Index << "Motor Currents"
        << std::endl << commandPositions->Index    << "Command Positions"
        << std::endl << error->Index               << "Errors"
        << std::endl << derror0->Index             << "D Errors"
        << std::endl << ierror->Index              << "I Errors"
        << std::endl << overLimit->Index           << "OverLimit"
        << std::endl << std::endl;

    // To allow another task to read from state data table
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetRawPositions);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetRawVelocities);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetRawCurrentFeedbacks);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetRawPotFeedbacks);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetOutputVoltagesDAC);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetOutputCurrentsDAC);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetPositions);
    //GSF
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetLatchedIndex);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetVelocities);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetCurrentFeedbacks);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetPotFeedbacksVolts);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetPotFeedbacksDeg);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetOutputVoltagesVolts);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetOutputCurrentsmAmps);

    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetCommandPositions);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetErrors);
    DDI_ADD_METHOD_COMMAND2(ctfControlThread, GetGain);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetMode);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetUsePotsToAdjust);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetDefaultAdjustValue);
    DDI_ADD_METHOD_COMMAND1(ctfControlThread, GetOverLimit);

    RTS_ADD_METHOD_COMMAND1(ctfControlThread, ResetEncoders);
    RTS_ADD_METHOD_COMMAND1(ctfControlThread, AdjustPositions);
    //gsf - logging
    RTS_ADD_METHOD_COMMAND1(ctfControlThread, LogDataFile);

    RTS_ADD_METHOD_COMMAND1(ctfControlThread, SetOutputCurrentsmAmps);
    RTS_ADD_METHOD_COMMAND1(ctfControlThread, SetOutputVoltagesVolts);
    RTS_ADD_METHOD_COMMAND1(ctfControlThread, SetCommandPositions);
    RTS_ADD_METHOD_COMMAND1(ctfControlThread, SetGain);
    RTS_ADD_METHOD_COMMAND1(ctfControlThread, SetMode);
    RTS_ADD_METHOD_COMMAND1(ctfControlThread, SetUsePotsToAdjust);
    RTS_ADD_METHOD_COMMAND1(ctfControlThread, SetDefaultAdjustValues);
    RTS_ADD_METHOD_COMMAND1(ctfControlThread, Enable);
    RTS_ADD_METHOD_COMMAND1(ctfControlThread, Disable);

    DDI_ADD_METHOD_COMMAND2(ctfControlThread, EncoderCountsToDegs);
    DDI_ADD_METHOD_COMMAND2(ctfControlThread, PotFeedbackToDegs);

    ControllerMode.Data.SetAll((unsigned int)ctfControlThread::MODE_SIMULATION); //default for safety
    StateDataTable.Advance();
}

ctfControlThread::~ctfControlThread() {
}

RTS_BEGIN_IMPLEMENT_COMMAND1(ctfControlThread, ResetEncoders)
    bool result = true;
    if (Device) {
        ddiShort axisIndex;
        for (axisIndex.Data = 0; axisIndex.Data < CTF_CONTROL_MAX_AXIS; axisIndex.Data++) {
            if (pdata->Data.at(axisIndex.Data)) {
                result &= ResetEncodersCommand->Execute(axisIndex);
            }
        }
    }
    return (result?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

RTS_BEGIN_IMPLEMENT_COMMAND1(ctfControlThread, SetCommandPositions)
    return ((StateDataTable.Write(commandPositions->Index, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

// update gains for the i^th axis.
RTS_BEGIN_IMPLEMENT_COMMAND1(ctfControlThread, SetGain)
    Gains.Data[pdata->AxisNumber].PGain      = pdata->PGain;
    Gains.Data[pdata->AxisNumber].DGain      = pdata->DGain;
    Gains.Data[pdata->AxisNumber].IGain      = pdata->IGain;
    Gains.Data[pdata->AxisNumber].DACLimit   = pdata->DACLimit;
    Gains.Data[pdata->AxisNumber].ILimit     = pdata->ILimit;
    Gains.Data[pdata->AxisNumber].ErrorLimit = pdata->ErrorLimit;
    Gains.Data[pdata->AxisNumber].Offset     = pdata->Offset;
    return ddiCommandBase::DEV_OK;
}

// this sets the mode of the controller.
// 0 means just write input speed & current to output
// 1 means servo to position (voltage)
// 2 means servo to position (current)
RTS_BEGIN_IMPLEMENT_COMMAND1(ctfControlThread, SetMode)
    ControllerMode.Data.Assign(pdata->Data);
    CMN_LOG_CLASS(7) << "setting mode to" << ControllerMode.Data << std::endl;
    return ddiCommandBase::DEV_OK;
}

RTS_BEGIN_IMPLEMENT_COMMAND1(ctfControlThread, SetUsePotsToAdjust)
    UsePotsToAdjust.Data.Assign(pdata->Data);
    CMN_LOG_CLASS(7) << "setting use pots to" << UsePotsToAdjust.Data << std::endl;
    return ddiCommandBase::DEV_OK;
}

RTS_BEGIN_IMPLEMENT_COMMAND1(ctfControlThread, SetDefaultAdjustValues)
    DefaultAdjustValues.Data.Assign(pdata->Data);
    CMN_LOG_CLASS(7) << "setting use pots to" << UsePotsToAdjust.Data << std::endl;
    return ddiCommandBase::DEV_OK;
}

// this can be called in current or direct output modes
RTS_BEGIN_IMPLEMENT_COMMAND1(ctfControlThread, SetOutputVoltagesVolts)
    for (unsigned int axis = 0; axis < ControllerMode.Data.size(); axis++) {
        if (ControllerMode.Data.Element(axis) != MODE_SERVO) {
        CommandDirectVolts.Data.Element(axis) = pdata->Data.Element(axis);
        }
    }
    StateDataTable.Write(outputVoltagesVolts->Index, *pdata);
    return ddiCommandBase::DEV_OK;
}

// this can be called in position or direct output modes
RTS_BEGIN_IMPLEMENT_COMMAND1(ctfControlThread, SetOutputCurrentsmAmps)
    for (unsigned int axis = 0; axis < ControllerMode.Data.size(); axis++) {
        if (ControllerMode.Data.Element(axis) != MODE_SIMULATION) {
            outputCurrentsmAmps->Data.Element(axis) = pdata->Data.Element(axis);
        }
    }
    StateDataTable.Write(outputCurrentsmAmps->Index, *pdata);
    return ddiCommandBase::DEV_OK;
}

// the UI can call this method to adjust encoders based on pot reading
RTS_BEGIN_IMPLEMENT_COMMAND1(ctfControlThread, AdjustPositions)
    this->AdjustEncoders(*pdata);
    AdjustControllerAll();
    overLimit->Data.SetAll(false);
    StateDataTable.Write(overLimit->Index, *overLimit);
    //added oct 6th.
    StateDataTable.Advance();
    return ddiCommandBase::DEV_OK;
}

// gsf - logging
// the UI can call this method to log data to file
RTS_BEGIN_IMPLEMENT_COMMAND1(ctfControlThread, LogDataFile)
    CMN_LOG_CLASS(3) << "Received Log Data command - control thread" << std::endl;
    LogData();
    return ddiCommandBase::DEV_OK;
}

// UI to call these to enable / disable amps
RTS_BEGIN_IMPLEMENT_COMMAND1(ctfControlThread, Enable)
    ddiCommandBase::ReturnType ret = EnableAmplifierCommand->Execute(*pdata);
    return ret;
}

// UI to call these to enable / disable amps
RTS_BEGIN_IMPLEMENT_COMMAND1(ctfControlThread, Disable)
    ddiCommandBase::ReturnType ret = DisableAmplifierCommand->Execute(*pdata);
    return ret;
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetRawPositions)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(rawPositions->Index, now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetRawVelocities)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(rawVelocities->Index, now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetRawCurrentFeedbacks)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(rawCurrentFeedbacks->Index, now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetRawPotFeedbacks)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(rawPotFeedbacks->Index , now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetOutputVoltagesDAC)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(outputVoltagesDAC->Index, now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetOutputCurrentsDAC)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(outputCurrentsDAC->Index, now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetPositions)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(positions->Index, now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

//GSF
DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetLatchedIndex)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(latchedindex->Index, now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetVelocities)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(velocities->Index, now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetCurrentFeedbacks)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(currentFeedbacks->Index, now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetPotFeedbacksVolts)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(potFeedbacksVolts->Index, now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetPotFeedbacksDeg)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(potFeedbacksDeg->Index, now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetOutputVoltagesVolts)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(outputVoltagesVolts->Index , now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetOutputCurrentsmAmps)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(outputCurrentsmAmps->Index , now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetCommandPositions)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(commandPositions->Index , now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetErrors)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(error->Index, now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

DDI_BEGIN_IMPLEMENT_COMMAND2(ctfControlThread, GetGain)
    todata->AxisNumber = fromdata->AxisNumber;
    todata->PGain = Gains.Data[fromdata->AxisNumber].PGain;
    todata->DGain = Gains.Data[fromdata->AxisNumber].DGain;
    todata->IGain = Gains.Data[fromdata->AxisNumber].IGain;
    todata->DACLimit = Gains.Data[fromdata->AxisNumber].DACLimit;
    todata->ILimit = Gains.Data[fromdata->AxisNumber].ILimit;
    todata->ErrorLimit = Gains.Data[fromdata->AxisNumber].ErrorLimit;
    todata->Offset = Gains.Data[fromdata->AxisNumber].Offset;
    return ddiCommandBase::DEV_OK;
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetMode)
    pdata->Data.Assign(ControllerMode.Data);
    return ddiCommandBase::DEV_OK;
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetUsePotsToAdjust)
    pdata->Data.Assign(UsePotsToAdjust.Data);
    return ddiCommandBase::DEV_OK;
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetDefaultAdjustValue)
    pdata->Data.Assign(DefaultAdjustValues.Data);
    return ddiCommandBase::DEV_OK;
}

DDI_BEGIN_IMPLEMENT_COMMAND1_OUT(ctfControlThread, GetOverLimit)
    rtsTimeIndex now = StateDataTable.GetIndexReader();
    return ((StateDataTable.ReadFromReader(overLimit->Index, now, *pdata))?ddiCommandBase::DEV_OK:ddiCommandBase::DEV_NOT_OK);
}

// DegsPerCount here atcually represents counts per unit motion of the
// joint. If it is translatory joint it would mean counts per mm
// and rotary joint it would mean counts per deg.
// it is computed once during reading of configuration file using
// the formula:
// DegsPerCount = Pitch *  (1/GearRatio) * (360) * (1/CountsPerTurn)
// Pitch is in MM per Deg for translatory joints
// Pitch is 1 for rotary joints

// Here the input is raw input encoder counts and the output is joint
// position in deg (or mm) for rotary (resp translatory) joints.
DDI_BEGIN_IMPLEMENT_COMMAND2(ctfControlThread, EncoderCountsToDegs);
    for (unsigned int i = 0; i < fromdata->Data.size(); i++) {
        todata->Data(i) = (double)(fromdata->Data(i)) * DegsPerCount.Data(i) + OffsetInDeg.Data(i);
    }
    return ddiCommandBase::DEV_OK;
}

// the input to this function is voltage output read from the pot in Volts
// the output of this function is absolute position of the joint (in mm or in deg)
// depending on the type of joint.
//      y = m * x + c, where x is pot voltage, y is abs. position.
//      m, c must be calibrated and provided using the config file.
DDI_BEGIN_IMPLEMENT_COMMAND2(ctfControlThread, PotFeedbackToDegs);
    for (unsigned int i = 0; i < fromdata->Data.size(); i++) {
        todata->Data(i) = fromdata->Data(i) * SlopePerPotVolt.Data(i) + InterceptPotDeg.Data(i);
    }
    return ddiCommandBase::DEV_OK;;
}
///////////
// GSF - Modify this to zero based on index pulse
///////////
// MOD FOR SNAKE
// this method would adjust the encoders based on current pot readings.
void ctfControlThread::AdjustEncoders(ddiVecBool16 &axisToAdjust)
{
/*
    // step 1: read raw pot values
    // step 2: convert pot values to deg or mm
    // step 3: copy these to positions
    // step 4: compute raw positions
    // step 5: load these new values

    // step 1
    StartPotFeedbacksConvCommand->Execute();
    GetInputAllCommand->Execute(*potFeedbacksVolts);

    // step 2
    //ConvertADCToPotFeedbacksVolts->Execute(*rawPotFeedbacks, *potFeedbacksVolts);
    PotFeedbackToDegsProtected(*potFeedbacksVolts, *potFeedbacksDeg);

#if HACKS
    ///////////////////////////////////////////////////////////////////////////
    //hack for snake LARS (LARS pots are always zero, coz doesnt have pots)
    if (strcmp("SVSN", GetName()) == 0) {
        potFeedbacksDeg[0] = 0.;
        potFeedbacksDeg[1] = 0.;
        potFeedbacksDeg[2] = 0.;
        potFeedbacksDeg[3] = 0.;
        potFeedbacksDeg[4] = 0.;
        potFeedbacksDeg[5] = 0.;
        potFeedbacksDeg[6] = 0.;
        potFeedbacksDeg[7] = 0.;
#if GRIPPERSPECIAL
        potFeedbacksDeg[11] = GripperHome; // for gripper
#endif
        potFeedbacksDeg[12] = 0;
        potFeedbacksDeg[13] = 0;
        potFeedbacksDeg[14] = 0;
        potFeedbacksDeg[15] = 0;
    }

#endif

    //step 3
    UsedPotsThisCycle = true;
    for (unsigned int pp = 0; pp < rawPositions->Data.size(); pp++) {
        if (axisToAdjust.Data.Element(pp) == true) {
        if (UsePotsToAdjust.Data.at(pp)) {
            rawPositions->Data.at(pp) = (long)((potFeedbacksDeg->Data.at(pp) - OffsetInDeg.Data.at(pp))/DegsPerCount.Data.at(pp));
            positions->Data.at(pp) = potFeedbacksDeg->Data.at(pp);
        } else {
            rawPositions->Data.at(pp) = (long)(DefaultAdjustValues.Data.at(pp)*DegsPerCount.Data.at(pp) + OffsetInDeg.Data.at(pp));
            positions->Data.at(pp) = DefaultAdjustValues.Data.at(pp);
            rawPotFeedbacks->Data.at(pp) = 0;
            potFeedbacksVolts->Data.at(pp) = 0;
            potFeedbacksDeg->Data.at(pp) = 0;
        }
        }
    }
    CMN_LOG_CLASS(1) << "Base: got use pot value (T/F) " << UsePotsToAdjust << std::endl;
    CMN_LOG_CLASS(1) << "Base: got pot value (volts) " << *potFeedbacksVolts << std::endl;
    CMN_LOG_CLASS(1) << "Base: got pot value (deg) " << *potFeedbacksDeg << std::endl;
    CMN_LOG_CLASS(1) << "Base: got pot value (deg) " << *positions << std::endl;
    CMN_LOG_CLASS(1) << "Base: Setting encs to " << *rawPositions << std::endl;

    StateDataTable.Write(rawPotFeedbacks->Index, *rawPotFeedbacks);
    StateDataTable.Write(potFeedbacksVolts->Index, *potFeedbacksVolts);
    StateDataTable.Write(potFeedbacksDeg->Index,  *potFeedbacksDeg);
    StateDataTable.Write(positions->Index,  *positions);
    StateDataTable.Write(rawPositions->Index, *rawPositions);

    //step 5
    SetEncodersCommand->Execute(*rawPositions);
    ddiVecLong16 rawcheck;
    // Latch the encoders
    LatchEncodersCommand->Execute();
    // now datavalid field is not set, hence a zero gets copied in state
    ReadPositionsCommand->Execute(rawcheck);
    CMN_LOG_CLASS(1) << "Encs check to " << rawPositions->Data - rawcheck.Data << std::endl;
*/
//GSF - New function to zero encoders
/*
    for (unsigned int pp = 0; pp < rawPositions->Data.size(); pp++) {
            rawPositions->Data.at(pp) = 0;
            //positions->Data.at(pp) = 0;
            //rawPotFeedbacks->Data.at(pp) = 0;
            //potFeedbacksVolts->Data.at(pp) = 0;
        }
    
    SetEncodersCommand->Execute(*rawPositions);
*/
    ddiVecLong16  rawcheck;
    ddiVecShort16 latchcheck;
    ddiVecShort16 offsets;

 // Set offsets of index from zero position
    // Change to XML config file!!!
    offsets[0]=0;
//    offsets[1]=-2980; // for zero at top pos
    //offsets[1]=501;   // zero at full collapse (overlapping pins)
//    offsets[1]=0;
//    offsets[1]=3439;  // At vertical axis fully extended up
    offsets[1]=451; // Encoder value at index pulse -> encoder @ 3439 fully(reachable) up and 0 @ fully(unreacahble) down

    //offsets[2]=-57;   // zero at center
    //offsets[2]=0;
    offsets[2]=10;  // Encoder value at index pulse -> encoder @ 0 in center

    //offsets[3]=0;
    offsets[3]=-2400;   // Encoder value at index pulse -> encoder @ 0 when fuly retracted to stop

    offsets[4]=0;
    offsets[5]=0;
    offsets[6]=0;
    offsets[7]=0;
    offsets[8]=0;
    offsets[9]=0;
    offsets[11]=0;
    offsets[12]=0;
    offsets[13]=0;
    offsets[14]=0;
    offsets[15]=0;

    // Latch the encoders
    LatchEncodersCommand->Execute();
    
    // Read in raw encoder positions
    ReadPositionsCommand->Execute(rawcheck);
    CMN_LOG_CLASS(1) << "Raw Encoder Value: " << rawcheck.Data << std::endl;

    // Read in latched raw index positions
    ReadLatchedIndexCommand->Execute(latchcheck);    
    CMN_LOG_CLASS(1) << "Latched Index Value: " << latchcheck.Data << std::endl;

   // Adjust for offset to home encoder 
    for (unsigned int pp = 0; pp < rawPositions->Data.size(); pp++) {
            //rawPositions->Data.at(pp) = 0;    // set all to zero
            rawPositions->Data.at(pp) = rawcheck.Data.at(pp) - latchcheck.Data.at(pp) + offsets.Data.at(pp);    // set all based on latched index pulse
        //rawPositions->Data.at(pp) = offsets.Data.at(pp);  //set all based on offset from know position
        }
    
   // Set new raw encoder position 
    SetEncodersCommand->Execute(*rawPositions);



//GSF
//Test Digital IO
ddiBasicTypeDataObject<short> temp_axis;
temp_axis = 0;
//EnableAmplifierCommand->Execute(temp_axis);   //enable turn on/opens valve, releases brake
DisableAmplifierCommand->Execute(temp_axis);    //enable turn off/exhausts valve, activates brake
temp_axis = 1;
DisableAmplifierCommand->Execute(temp_axis);
temp_axis = 2;
DisableAmplifierCommand->Execute(temp_axis);
temp_axis = 3;
DisableAmplifierCommand->Execute(temp_axis);


}

// this method would adjust the command positions based on current positions reading
// it is important to have this, so that the motors dont jump when we suddenly turn
// on servo loop.

// you must modify this if you change your control algorithm. Typically all the inputs
// for your control algorithm must be set correctly based on current actual position
// before you can start the control loop. this would ensure that the output of control loop
// is zero the instant you start.
// before calling this ensure that latch enc , read enc is done so that position has the
// actual encoder position

void ctfControlThread::AdjustControllerAll()
{
    // these are set to zero
    ddiVecDouble16 dummyDouble;
    dummyDouble.Data.SetAll(0.0);
    StateDataTable.Write(error->Index, dummyDouble);
    StateDataTable.Write(ierror->Index, dummyDouble);
    StateDataTable.Write(derror0->Index, dummyDouble);
    StateDataTable.Write(commandPositions->Index, *positions);
}

void ctfControlThread::AdjustController(unsigned int axis)
{
    error->Data.Element(axis) = 0.0;
    ierror->Data.Element(axis) = 0.0;
    derror0->Data.Element(axis) = 0.0;
    commandPositions->Data.Element(axis) = positions->Data.Element(axis);
}


// this is the actual control method. most likely you will modify this to suit your needs.
// TIP: it is best to avoid having local variable inside this method unless they are small
/// such as int, long. vectors, arrays should be declared as members of TASK and used here.
// this is for efficiency
// this method takes time index, so that it could be used for getting data from history
// which might be useful if one has to write complex controllers
void ctfControlThread::ControllerMethodForPID(const unsigned int axis)
{
    //GSF - change to be added to 'axis' and subtracted from 'axis+8'
    double outputchange=0;

    // set a upper limit. not to blow the voltage too high
    if (ierror->Data[axis] >= Gains[axis].ILimit)
        ierror->Data[axis] = Gains[axis].ILimit;
    if (ierror->Data[axis] <= -Gains[axis].ILimit)
        ierror->Data[axis] = -Gains[axis].ILimit;
    //GSF
    //outputVoltagesVolts->Data[axis] += Gains[axis].IGain*ierror->Data[axis];
    outputchange += Gains[axis].IGain*ierror->Data[axis];
    overLimit->Data[axis] = false;
    if ((UsePotsToCheck.Data[axis] > 0) &&
            (fabs(positions->Data[axis]- potFeedbacksDeg->Data[axis]) >= PotDifferenceLimit.Data[axis])) overLimit->Data[axis] = true;
#if 0
    switch(UseErrorLimit) {
        case 2:
            if (fabs(error->Data[axis]) >= Gains[axis].ErrorLimit)
                OverLimit = true;
            break;
        case 0:
            // no error check
            break;
        case 1:
        default:
            if (error->Data[axis] >= Gains[axis].ErrorLimit)
                error->Data[axis] = Gains[axis].ErrorLimit;
            if (error->Data[axis] <= -Gains[axis].ErrorLimit)
                error->Data[axis] = -Gains[axis].ErrorLimit;
            break;
    }
#endif
    //GSF
    //outputVoltagesVolts->Data[axis] += Gains[axis].PGain*error->Data[axis];
    //outputVoltagesVolts->Data[axis] += Gains[axis].DGain*derrorFilter->Data[axis];
    //outputVoltagesVolts->Data[axis] = -1.0*outputVoltagesVolts->Data[axis];
    //outputVoltagesVolts->Data[axis] += Gains[axis].Offset;
    outputchange += Gains[axis].PGain*error->Data[axis];
    outputchange += Gains[axis].DGain*derrorFilter->Data[axis];
    //outputchange = -1.0*outputchange;
    outputchange += Gains[axis].Offset;

    // set a upper limit. not to blow the voltage too high
    if (outputchange >= Gains[axis].DACLimit)
        outputchange = Gains[axis].DACLimit;
    if (outputchange <= -Gains[axis].DACLimit)
        outputchange = -Gains[axis].DACLimit;

    //GSF - for differential output
    if (outputchange>0){
        outputVoltagesVolts->Data[axis] = outputchange;
    } else {
        outputVoltagesVolts->Data[axis+8] = -outputchange;
    }


    if (overLimit->Data[axis]) outputVoltagesVolts->Data[axis] = Gains[axis].Offset;
    //GSF
    if (overLimit->Data[axis+8]) outputVoltagesVolts->Data[axis+8] = Gains[axis+8].Offset;

    /*
    // set a upper limit. not to blow the voltage too high
    if (outputVoltagesVolts->Data[axis] >= Gains[axis].DACLimit)
        outputVoltagesVolts->Data[axis] = Gains[axis].DACLimit;
    if (outputVoltagesVolts->Data[axis] <= -Gains[axis].DACLimit)
        outputVoltagesVolts->Data[axis] = -Gains[axis].DACLimit;
    //GSF
    if (outputVoltagesVolts->Data[axis+8] >= Gains[axis+8].DACLimit)
        outputVoltagesVolts->Data[axis+8] = Gains[axis+8].DACLimit;
    if (outputVoltagesVolts->Data[axis+8] <= -Gains[axis+8].DACLimit)
        outputVoltagesVolts->Data[axis+8] = -Gains[axis+8].DACLimit;
    */

}

void ctfControlThread::ControllerMethodForSMC(const unsigned int axis)
{
    double outputforce; // control output force in N
    double mass = 0.044;    //sliding mass in kg
    double  S,X,u_s,u_eq;   // SMC params

    double lambda = double(Gains[axis].DGain);  //1
    double ks = double(Gains[axis].PGain);      //0.3
    double delta = double(Gains[axis].IGain);   //0.5

    // S is sliding surface
//  S = 2*Gains[axis].DGain*derrorFilter->Data[axis] + Gains[axis].DGain*Gains[axis].DGain*error->Data[axis];

    S = derrorFilter->Data[axis]*1000 + lambda*error->Data[axis];
    //         mm/s                                  mm
    //S = error->Data[axis];
    // smaller lambda -> more emphasis on speed error -> more damping


    if(fabs(S) < delta){
        // within boundary delta decrease force linearly to limit chattering, |X|<1
        X = S/delta;
    } else {
        if(S>0)
           // error beyond boundary delta, output X=+/-1
           X = 1;
        else
               X = -1;

    }

    // To get onto sliding surface
    u_s = ks * X * 10;  //needs ~3N - > ks=0.3

    // To maintain on sliding surface - FILL IN LATER
    u_eq = -mass*lambda*derrorFilter->Data[axis];
    //        kg                m/s
    
    outputforce = u_eq + u_s; // in Newtons
    

/*      

    //GSF - change to be added to 'axis' and subtracted from 'axis+8'
    double outputchange=0;

    // set a upper limit. not to blow the voltage too high
    if (ierror->Data[axis] >= Gains[axis].ILimit)
        ierror->Data[axis] = Gains[axis].ILimit;
    if (ierror->Data[axis] <= -Gains[axis].ILimit)
        ierror->Data[axis] = -Gains[axis].ILimit;
    //GSF
    //outputVoltagesVolts->Data[axis] += Gains[axis].IGain*ierror->Data[axis];
    outputchange += Gains[axis].IGain*ierror->Data[axis];
    overLimit->Data[axis] = false;
    if ((UsePotsToCheck.Data[axis] > 0) &&
            (fabs(positions->Data[axis]- potFeedbacksDeg->Data[axis]) >= PotDifferenceLimit.Data[axis])) overLimit->Data[axis] = true;
#if 0
    switch(UseErrorLimit) {
        case 2:
            if (fabs(error->Data[axis]) >= Gains[axis].ErrorLimit)
                OverLimit = true;
            break;
        case 0:
            // no error check
            break;
        case 1:
        default:
            if (error->Data[axis] >= Gains[axis].ErrorLimit)
                error->Data[axis] = Gains[axis].ErrorLimit;
            if (error->Data[axis] <= -Gains[axis].ErrorLimit)
                error->Data[axis] = -Gains[axis].ErrorLimit;
            break;
    }
#endif
    //GSF
    //outputVoltagesVolts->Data[axis] += Gains[axis].PGain*error->Data[axis];
    //outputVoltagesVolts->Data[axis] += Gains[axis].DGain*derrorFilter->Data[axis];
    //outputVoltagesVolts->Data[axis] = -1.0*outputVoltagesVolts->Data[axis];
    //outputVoltagesVolts->Data[axis] += Gains[axis].Offset;
    outputchange += Gains[axis].PGain*error->Data[axis];
    outputchange += Gains[axis].DGain*derrorFilter->Data[axis];
    //outputchange = -1.0*outputchange;
    outputchange += Gains[axis].Offset;

    // set a upper limit. not to blow the voltage too high
    if (outputchange >= Gains[axis].DACLimit)
        outputchange = Gains[axis].DACLimit;
    if (outputchange <= -Gains[axis].DACLimit)
        outputchange = -Gains[axis].DACLimit;
*/
    //GSF - for differential output
    if (outputforce>0){ //extend cylinder
        outputVoltagesVolts->Data[axis] = outputforce / 6.7877;     // Conver to Bar, 67.877mm^2
        outputVoltagesVolts->Data[axis+8] = 0;
    } else {        // retract cylinder
        outputVoltagesVolts->Data[axis] = 0;
        outputVoltagesVolts->Data[axis+8] = -outputforce / 5.9984;  // Conver to Bar, 59.984mm^2

    }

//GSF - 2-18-08
// Add feedforward breakaway pressure, shoul allow much lower gains
// Y = -0.076X + 12.446      R^2=0.995
//  Y = Voltage output before breakaway
//  X = vertical position in mm after zeroing
if(axis==1){
//  outputVoltagesVolts->Data[1] += ( positions->Data[1]*(-0.076) + 12.446 );
    outputVoltagesVolts->Data[1] += ( positions->Data[1]*(-0.076) + 12.2 );     // dropped gravity comp a bit
}


//GSF - 2-20-08
// Add scaled up force as further from zero position
// Apply up to 50% additional force at edge of travel(50mm from center) scaled linearly
if(axis==2){
    if(outputVoltagesVolts->Data[2]>0){
        outputVoltagesVolts->Data[2] += outputVoltagesVolts->Data[2] * (fabs(positions->Data[2])/50) * 0.5;
    }
    if(outputVoltagesVolts->Data[10]>0){
        outputVoltagesVolts->Data[10] += outputVoltagesVolts->Data[10] * (fabs(positions->Data[2])/50) * 0.5;
    }
}

/*
    if (overLimit->Data[axis]) outputVoltagesVolts->Data[axis] = Gains[axis].Offset;
    //GSF
    if (overLimit->Data[axis+8]) outputVoltagesVolts->Data[axis+8] = Gains[axis+8].Offset;
*/
    
    // set a upper limit. not to blow the voltage too high
    if (outputVoltagesVolts->Data[axis] >= Gains[axis].DACLimit)
        outputVoltagesVolts->Data[axis] = Gains[axis].DACLimit;
    if (outputVoltagesVolts->Data[axis] <= 0)
        outputVoltagesVolts->Data[axis] = 0;
    //GSF
    if (outputVoltagesVolts->Data[axis+8] >= Gains[axis+8].DACLimit)
        outputVoltagesVolts->Data[axis+8] = Gains[axis+8].DACLimit;
    if (outputVoltagesVolts->Data[axis+8] <= 0)
        outputVoltagesVolts->Data[axis+8] = 0;
    

}

void ctfControlThread::ControllerMethodForSimulation(const unsigned int axis)
{
    positions->Data.Element(axis) = commandPositions->Data.Element(axis);
    rawPositions->Data.Element(axis) = (long)(positions->Data.Element(axis)*DegsPerCount.Data.Element(axis) + OffsetInDeg.Data.Element(axis));
    error->Data.Element(axis) = 0.0;
    derror0->Data.Element(axis) = 0.0;
    ierror->Data.Element(axis) = 0.0;
    overLimit->Data.Element(axis) = false;
    outputVoltagesVolts->Data.Element(axis) = 0.0;
    outputCurrentsmAmps->Data.Element(axis) = 0.0;
}

// this method is used if we directly write motor voltage (hence speed) from the UI
void ctfControlThread::ControllerMethodForDirectVolt(const unsigned int axis)
{
    outputVoltagesVolts->Data[axis] = CommandDirectVolts.Data[axis];
    // set a upper limit. not to blow the voltage too high
    if (outputVoltagesVolts->Data[axis] >= Gains[axis].DACLimit)
        outputVoltagesVolts->Data[axis] = Gains[axis].DACLimit;
    if (outputVoltagesVolts->Data[axis] <= -Gains[axis].DACLimit)
        outputVoltagesVolts->Data[axis] = -Gains[axis].DACLimit;
}

void ctfControlThread::Startup(void)
{
    outputVoltagesVolts->Data.SetAll(0.0);
    ddiVecDouble16 pos;
    pos.Data.SetAll(0.0);
    StateDataTable.Write(commandPositions->Index, pos);
    overLimit->Data.SetAll(false);
    StateDataTable.Write(overLimit->Index, *overLimit);

    // Sent these initial values to the device
    ddiCommandBase::ReturnType ret1, ret2, ret3, ret4;
    if (Device) {
        outputVoltagesVolts->Data.SetAll(0);
        
        ret1 = ConvertMotorVoltagesToDACCommand->Execute(*outputVoltagesVolts, *outputVoltagesDAC);
        //ret2 = WriteMotorVoltagesCommand->Execute(*outputVoltagesDAC);
        //GSF
        ret2 = SetOutputAllCommand->Execute(*outputVoltagesVolts);
                
        LoadMotorVoltagesCommand->Execute();
        ret3 = ConvertMotorCurrentsToDACCommand->Execute(*outputCurrentsmAmps, *outputCurrentsDAC);
        ret4 = WriteMotorCurrentsCommand->Execute(*outputCurrentsDAC);
        CMN_LOG_CLASS(1) << "Setting currents to " << *outputCurrentsmAmps << std::endl;
        CMN_LOG_CLASS(1) << "Setting currents to " << *outputCurrentsDAC << std::endl;
        LoadMotorCurrentsCommand->Execute();


        //GSF - added to zero encoders at start of program
        // adjust encoders to pots values
        //this->AdjustEncoders(UsePotsToAdjust);

        for (unsigned int pp = 0; pp < rawPositions->Data.size(); pp++) {
            rawPositions->Data.at(pp) = 0;
        }
        SetEncodersCommand->Execute(*rawPositions);

    
       
        // make sure that output of control algorithm is zero for adjusted encoders
        AdjustControllerAll();

    //GSF
    // Disable Amps -> Turn off brake valves
    //DisableAmplifierCommand->Execute();
    }
}


void ctfControlThread::Run(void) {
    const rtsTimeIndex now = StateDataTable.GetIndexWriter();
    static rtsCommand1<ctfControlThread> dataMsg;

    UsedPotsThisCycle = false;

    // See is any message is waiting in the mail box and execute the
    // first one.
    if (DequeueCommand(&dataMsg, sizeof(rtsCommand1<ctfControlThread>))) {
    (this->*(dataMsg.Action))(dataMsg);
    }

#if MOTOR_CURRENT
    if (now.Ticks() % 2 == 0)
#endif
    {
        StartPotFeedbacksConvCommand->Execute();
    }
#if MOTOR_CURRENT
    else
    {
        if (UsedPotsThisCycle == false) StartMotorCurrentsConvCommand->Execute();
    }
#endif

    // Latch the encoders
    LatchEncodersCommand->Execute();

    // read in the positions and velocities
    // velocities must be read before positions
    if (DeviceProvidesVelocities) {
    ReadVelocitiesCommand->Execute(*rawVelocities);
    }
    // now datavalid field is not set, hence a zero gets copied in state
    ReadPositionsCommand->Execute(*rawPositions);
    //ConvertEncodersToShaftRevCommand->Execute(rawPositions, positions);
    EncoderCountsToDegsProtected(*rawPositions, *positions);

    //GSF - update positions based on kinematics
    // ADD to external file later

   // Vertical: (linear enc in mm -> vert motion in mm)
//GSF - TEMP LINEAR, COMMENT OUT BELOW
    positions->Data(1) = 2*sqrt(2*positions->Data(1)*68 - positions->Data(1)*positions->Data(1));  


    // Horizotal: (rotary enc in deg -> horiz motion in mm)
//    positions->Data(2) = 120 * sin(positions->Data(2));                     
    positions->Data(2) = 120 * sin( positions->Data(2) * (2*3.14159)/360 );                     
    
    if (DeviceProvidesVelocities) {
        ConvertFrequencyToShaftDegPerSCommand->Execute(*rawVelocities, *velocities);
    } else {
    // Compute your own, less accurate
    }

    // To read the potentiometers
#if MOTOR_CURRENT
    if (now.Ticks() % 2 == 0)
#endif
    {
        GetInputAllCommand->Execute(*potFeedbacksVolts);
        //ConvertADCToPotFeedbacksVolts->Execute(*rawPotFeedbacks, *potFeedbacksVolts);
        PotFeedbackToDegsProtected(*potFeedbacksVolts, *potFeedbacksDeg);
    }
#if MOTOR_CURRENT
    else
    {
        // to read currents. we read current and pots on alternate cycles,
        // since pots and currents cant be read at the same time
        if (UsedPotsThisCycle == false) {
            ReadMotorCurrentsCommand->Execute(*rawCurrentFeedbacks);
            ConvertADCToMotorCurrentsCommand->Execute(*rawCurrentFeedbacks, *currentFeedbacks);
        }
    }
#endif

    // process the data read from hardware before it is written on state table
    // and used in control algorithm. these can be used to perform controller
    // specific functions
    this->PreProcessData();

    rtsTimeIndex iter = now;
    --iter;
    //set command position for this tick
    StateDataTable.ReadFromWriter(commandPositions->Index, iter, *commandPositions);
    //get last error and ierror term
    StateDataTable.ReadFromWriter(error->Index, iter, preverror);
    StateDataTable.ReadFromWriter(ierror->Index, iter, previerror);
    //get last 3 derror terms
    error->Data.DifferenceOf(commandPositions->Data, positions->Data);
derror[0].Data.DifferenceOf(error->Data, preverror.Data);
    ierror->Data.SumOf(previerror.Data, error->Data);

    // filter the \dot e
    derrorFilter->Data.Assign(derror[0].Data);
    derrorFilter->Data.Add(derror[1].Data);
    derrorFilter->Data.Add(derror[2].Data);
    derrorFilter->Data.Add(derror[3].Data);
    derrorFilter->Data.Divide(4.0);
    derror0->Data.Assign(derror[0].Data);

    // this sets the mode of the controller.
    // 0 means just write input speed to output
    // 1 means servo to position
    outputVoltagesVolts->Data.SetAll(0);
    unsigned int axis;
    for (axis = 0; axis < ControllerMode.Data.size(); axis++) {
        if (SimulateAxis.Data.Element(axis)) ControllerMode.Data.Element(axis) = ctfControlThread::MODE_SIMULATION;
        if (ControllerMode.Data.Element(axis) == ctfControlThread::MODE_DIRECT) {
            // calling this would enusre that when we switch mode we dont jump
            // unexpectedly to some value specified in target position
            // see definition of AdjustController for more details
            AdjustController(axis);
            this->ControllerMethodForDirectVolt(axis);
        } else if (ControllerMode.Data.Element(axis) == ctfControlThread::MODE_SERVO ) {
            // this is a very simple point to point PID controller.
            // maintain a simple position servo NOTHING fancy. It works
            // so for most cases.
        //GSF - only run PID controller for first 8 axes, second 8 axes are 2nd channel for first 8
            if (axis<8){
//              this->ControllerMethodForPID(axis);
// GSF - New Controller
                this->ControllerMethodForSMC(axis);
            }
        } else if (ControllerMode.Data.Element(axis) == ctfControlThread::MODE_SIMULATION) {
            this->ControllerMethodForSimulation(axis);
        } else {
            /* bad mode do nothing */
        }
    }

    // Write data to table
    StateDataTable.Write(rawPositions->Index, *rawPositions);
    StateDataTable.Write(positions->Index, *positions);
    // StateDataTable.Write(VELOCITIES, velocities);
    StateDataTable.Write(rawCurrentFeedbacks->Index, *rawCurrentFeedbacks);
    StateDataTable.Write(currentFeedbacks->Index, *currentFeedbacks);
    StateDataTable.Write(rawPotFeedbacks->Index, *rawPotFeedbacks);
    StateDataTable.Write(potFeedbacksVolts->Index, *potFeedbacksVolts);
    StateDataTable.Write(potFeedbacksDeg->Index, *potFeedbacksDeg);

    StateDataTable.Write(error->Index, *error);
    StateDataTable.Write(derror0->Index, *derror0);
    StateDataTable.Write(derrorFilter->Index, *derrorFilter);
    StateDataTable.Write(ierror->Index, *ierror);
    StateDataTable.Write(overLimit->Index, *overLimit);

    // process the results of the controll algorithm before they
    // are written on to the device and the state data table.
    this->PostProcessData();

    // First write voltage because it is used by the current
    // conversion to determine which slope to use based on sign of
    // voltage.
    ConvertMotorVoltagesToDACCommand->Execute(*outputVoltagesVolts, *outputVoltagesDAC);
    
    //WriteMotorVoltagesCommand->Execute(*outputVoltagesDAC);
    //GSF
    SetOutputAllCommand->Execute(*outputVoltagesVolts);

    ConvertMotorCurrentsToDACCommand->Execute(*outputCurrentsmAmps, *outputCurrentsDAC);
    WriteMotorCurrentsCommand->Execute(*outputCurrentsDAC);
    LoadMotorVoltagesCommand->Execute();
    LoadMotorCurrentsCommand->Execute();

    // Since we just converted it, save it to the state data table
    StateDataTable.Write(outputVoltagesDAC->Index, *outputVoltagesDAC);
    StateDataTable.Write(outputCurrentsDAC->Index, *outputCurrentsDAC);
    // and also put the actual data
    StateDataTable.Write(outputVoltagesVolts->Index, *outputVoltagesVolts);
    StateDataTable.Write(outputCurrentsmAmps->Index, *outputCurrentsmAmps);

    // Advance state table
    StateDataTable.Advance();
    Ticker++;
}


// gsf-logging
void ctfControlThread::LogData()
{
  //ofstream datalogStream;

  CMN_LOG_CLASS(1) << "Logging data to file..." << std::endl;

  datalogStream.open("DataLog.csv");
//  datalogStream << "Writing this to a file.\n";

    if (datalogStream.is_open()) {
        CMN_LOG_CLASS(3) << "Writing to data log file Servo states... " << 

    StateDataTable.GetIndexWriter().Ticks() << std::endl;
        unsigned int numberOfDataToDump = 5;
        unsigned int dataToDump[] = {positions->Index, commandPositions->Index, error->Index, outputVoltagesVolts->Index, potFeedbacksVolts->Index};

        StateDataTable.CSVWrite(datalogStream, dataToDump, numberOfDataToDump);
        CMN_LOG_CLASS(3) << "done data log file Servo states..." << std::endl;

        datalogStream.close();
    }

  CMN_LOG_CLASS(1) << "...Done writing to file" << std::endl;
}

#include <fstream>
void ctfControlThread::Cleanup(void) {
    outputVoltagesDAC->Data.SetAll(0);
    //WriteMotorVoltagesCommand->Execute(*outputVoltagesDAC);
    //GSF
    outputVoltagesVolts->Data.SetAll(0);
    SetOutputAllCommand->Execute(*outputVoltagesVolts);

    LoadMotorVoltagesCommand->Execute();
#if GRIPPERSPECIAL
    if ((strcmp("SVSN", GetName()) == 0) && (GripperFile != NULL)) {
        CMN_LOG(1) << "Writing gripper value " << positions[11] << " to file" << std::endl;
        int ret_val = fprintf(GripperFile, "%lf\n", positions[11]);
        if (ret_val < 0) {
            CMN_LOG(1) << "***** Failed to save gripper value. Manual update of GripperHome.txt needed *****" << std::endl;
        }
        fclose(GripperFile);
    }
#endif

    // DATA LOGGING HERE
    if (logStream.is_open()) {
        CMN_LOG_CLASS(3) << "Writing to file Servo states... " << StateDataTable.GetIndexWriter().Ticks() << std::endl;
        unsigned int numberOfDataToDump = 4;
        unsigned int dataToDump[] = {positions->Index, commandPositions->Index, error->Index, outputVoltagesVolts->Index};
//        unsigned int dataToDump[] = {positions->Index, commandPositions->Index, error->Index, currentFeedbacks->Index};
            //potFeedbacksDeg->Index, currentFeedbacks->Index, derrorFilter->Index};
        StateDataTable.CSVWrite(logStream, dataToDump, numberOfDataToDump);
        CMN_LOG_CLASS(3) << "done file Servo states..." << std::endl;
        logStream.close();
    }
}


// Configures must be run in a bottom-up order.  eg a trajectory task
// needs to know somethings about what it is trying to control.
//
// A better example is that a servo task needs to know the hardaware
// before it can choose some pid values, maybe it could choose pid
// values intelligently.
void ctfControlThread::Configure(const char *filename) {
    cmnXMLPath xmlConfig;
    if (!filename) {
        CMN_LOG_CLASS(2) << "Warning: Could not configure Servo task, configuration file missing!"
            <<std::endl;
        return;
    }

    xmlConfig.SetInputSource(filename);
    char path[40];
    std::string context("/Config/Task[@Name=\"");
    context = context + GetName() + "\"]";

    CMN_LOG_CLASS(1) << "Configuring ctfControlThread" << std::endl;
    unsigned int axis; bool ret;
    xmlConfig.GetXMLValue(context.c_str(), "UseErrorLimit/@Value", UseErrorLimit);
    xmlConfig.GetXMLValue(context.c_str(), "SimulationOnly/@Value", SimulationOnly);

    int use_pots_to_adjust, use_pots_to_check, simulate_axis;
    for (axis = 0; axis < CTF_CONTROL_MAX_AXIS; axis++) {
        sprintf(path, "Axis[%d]/Amplifier/@Number", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, AmplifierNumber[axis]);
        sprintf(path, "Axis[%d]/Amplifier/@Simulate", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, simulate_axis);
        if (!ret) SimulateAxis[axis] = false;
        else SimulateAxis[axis] = (simulate_axis>0)?true:false;
        sprintf(path, "Axis[%d]/Encoder/@CountsPerTurn", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, CountsPerTurn[axis]);
        sprintf(path, "Axis[%d]/Encoder/@GearRatio", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, GearRatio[axis]);
        sprintf(path, "Axis[%d]/Encoder/@Pitch", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, Pitch[axis]);
        sprintf(path, "Axis[%d]/Encoder/@OffsetInDeg", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, OffsetInDeg[axis]);
        if (!ret) OffsetInDeg.Data.at(axis) = 0.0;
        sprintf(path, "Axis[%d]/Pot/@Slope", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, SlopePerPotVolt[axis]);
        if (!ret) SlopePerPotVolt.Data.at(axis) = 0.0;
        sprintf(path, "Axis[%d]/Pot/@Intercept", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, InterceptPotDeg[axis]);
        if (!ret) InterceptPotDeg.Data.at(axis) = 0.0;
        sprintf(path, "Axis[%d]/Pot/@UsePotsToAdjust", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, use_pots_to_adjust); // defaults to true
        if (!ret) UsePotsToAdjust[axis] = true;
        else UsePotsToAdjust[axis] = (use_pots_to_adjust>0)?true:false;
        sprintf(path, "Axis[%d]/Pot/@DefaultAdjustValues", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, DefaultAdjustValues[axis]);
        if (!ret) DefaultAdjustValues.Data.at(axis) = 0.0;
        sprintf(path, "Axis[%d]/Pot/@UsePotsToCheck", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, use_pots_to_check); // defaults to true
        if (!ret) UsePotsToCheck[axis] = true;
        else UsePotsToCheck[axis] = (use_pots_to_check>0)?true:false;
        sprintf(path, "Axis[%d]/Pot/@DifferenceLimit", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, PotDifferenceLimit[axis]);
        if (!ret) PotDifferenceLimit.Data.at(axis) = 0.0;
        sprintf(path, "Axis[%d]/MaxCurrent/@Value", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, outputCurrentsmAmps->Data[axis]);
        if (!ret) outputCurrentsmAmps->Data[axis] = 150;
        sprintf(path, "Axis[%d]/Filter/@PGain", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, Gains[axis].PGain);
        if (!ret) Gains[axis].PGain = 0;
        sprintf(path, "Axis[%d]/Filter/@DGain", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, Gains[axis].DGain);
        if (!ret) Gains[axis].DGain = 0;
        sprintf(path, "Axis[%d]/Filter/@IGain", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, Gains[axis].IGain);
        if (!ret) Gains[axis].IGain = 0;
        sprintf(path, "Axis[%d]/Filter/@ILimit", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, Gains[axis].ILimit);
        if (!ret) Gains[axis].ILimit = 1; // Default Sets the ILimit to 1V
        sprintf(path, "Axis[%d]/Filter/@DACLimit", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, Gains[axis].DACLimit);
        if (!ret) Gains[axis].DACLimit = 12.0;
        sprintf(path, "Axis[%d]/Filter/@OffsetVoltage", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, Gains[axis].Offset);
        if (!ret) Gains[axis].Offset = 0.0;
        sprintf(path, "Axis[%d]/Filter/@ErrorLimit", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, Gains[axis].ErrorLimit);
        if (!ret) Gains[axis].Offset = 1.0;
    }

    if (SimulationOnly > 0) {
        CMN_LOG_CLASS(1) << "**** WARNING **** " << std::endl
            << "The config file has been set to use simulation only"
            << std::endl;
        SimulateAxis.Data.SetAll(true);
        ControllerMode.Data.SetAll((unsigned int)ctfControlThread::MODE_SIMULATION); //default for safety
    } else {
        for (unsigned int pp =0; pp < SimulateAxis.Data.size(); pp++) {
            if (SimulateAxis.Data.Element(pp) == true) {
                ControllerMode.Data.Element(pp) = (unsigned int)ctfControlThread::MODE_SIMULATION;
            } else {
                ControllerMode.Data.Element(pp) = (unsigned int)ctfControlThread::MODE_DIRECT;
            }
        }
    }

    CMN_LOG_CLASS(1) << "Read ctfControlThread Configuration" << std::endl;
    // DegsPerCount here atcually represents counts per unit motion of the
    // joint. If it is translatory joint it would mean counts per mm
    // and rotary joint it would mean counts per deg.
    // it is computed once during reading of comfiguration file using
    // the formula
    // DegsPerCount = Pitch *  (1/GearRatio) * (360) * (1/CountsPerTurn)
    // Pitch is in MM per Deg for translatory joints
    // Pitch is 1 for rotary joints
    for (unsigned int dp = 0; dp < DegsPerCount.Data.size(); dp++) {
        DegsPerCount.Data.at(dp) = (360.0*Pitch.Data.at(dp))/(GearRatio.Data.at(dp)*CountsPerTurn.Data.at(dp));
    }

    if (Device) {
        //DAC
        SetOutputAllCommand = Device->GetMethodOneArg("SetOutputAll");

        //ADC
        GetInputAllCommand = Device->GetMethodOneArg("GetInputAll");

        //LoPoMoCo
        LatchEncodersCommand = Device->GetMethodZeroArg("LatchEncoders");
        StartMotorCurrentsConvCommand = Device->GetMethodZeroArg("StartMotorCurrentConv");
        StartPotFeedbacksConvCommand = Device->GetMethodZeroArg("StartPotFeedbackConv");
        LoadMotorVoltagesCommand = Device->GetMethodZeroArg("LoadMotorVoltages");
        LoadMotorCurrentsCommand = Device->GetMethodZeroArg("LoadCurrentLimits");

        ReadPositionsCommand = Device->GetMethodOneArg("GetPositions");

        //GSF
    ReadLatchedIndexCommand = Device->GetMethodOneArg("GetLatchedIndex");

        ReadPotFeedbacksCommand = Device->GetMethodOneArg("GetPotFeedbacks");
        ReadVelocitiesCommand = Device->GetMethodOneArg("GetVelocities");
        ReadMotorCurrentsCommand = Device->GetMethodOneArg("GetMotorCurrents");
        WriteMotorVoltagesCommand = Device->GetMethodOneArg("SetMotorVoltages");
        WriteMotorCurrentsCommand = Device->GetMethodOneArg("SetCurrentLimits");
        SetEncodersCommand = Device->GetMethodOneArg("SetPositions");

        ResetEncodersCommand = Device->GetMethodOneArg("ResetEncoders");

        EnableAmplifierCommand = Device->GetMethodOneArg("Enable");

        DisableAmplifierCommand = Device->GetMethodOneArg("Disable");

        ConvertFrequencyToShaftDegPerSCommand = Device->GetMethodTwoArg("FrequencyToRPS");
        ConvertADCToMotorCurrentsCommand = Device->GetMethodTwoArg("ADCToMotorCurrents");
        ConvertADCToPotFeedbacksVolts = Device->GetMethodTwoArg("ADCToPotFeedbacks");
        ConvertMotorVoltagesToDACCommand = Device->GetMethodTwoArg("MotorVoltagesToDAC");
        ConvertMotorCurrentsToDACCommand = Device->GetMethodTwoArg("CurrentLimitsToDAC");

    } else {
        //DAC
        SetOutputAllCommand = ddiDeviceInterface::NOPSCommand1;

        //ADC
        GetInputAllCommand = ddiDeviceInterface::NOPSCommand1;

        //LoPoMoCo
        LatchEncodersCommand = ddiDeviceInterface::NOPSCommand0;
        StartMotorCurrentsConvCommand = ddiDeviceInterface::NOPSCommand0;
        StartPotFeedbacksConvCommand = ddiDeviceInterface::NOPSCommand0;
        LoadMotorVoltagesCommand = ddiDeviceInterface::NOPSCommand0;
        LoadMotorCurrentsCommand = ddiDeviceInterface::NOPSCommand0;

        ReadPositionsCommand = ddiDeviceInterface::NOPSCommand1;
        //GSF
        ReadLatchedIndexCommand = ddiDeviceInterface::NOPSCommand1;
        ReadPotFeedbacksCommand = ddiDeviceInterface::NOPSCommand1;
        ReadVelocitiesCommand = ddiDeviceInterface::NOPSCommand1;
        ReadMotorCurrentsCommand = ddiDeviceInterface::NOPSCommand1;
        WriteMotorVoltagesCommand = ddiDeviceInterface::NOPSCommand1;
        WriteMotorCurrentsCommand = ddiDeviceInterface::NOPSCommand1;
        SetEncodersCommand = ddiDeviceInterface::NOPSCommand1;

        ResetEncodersCommand = ddiDeviceInterface::NOPSCommand1;
        EnableAmplifierCommand = ddiDeviceInterface::NOPSCommand1;
        DisableAmplifierCommand = ddiDeviceInterface::NOPSCommand1;

        ConvertFrequencyToShaftDegPerSCommand = ddiDeviceInterface::NOPSCommand2;
        ConvertADCToMotorCurrentsCommand = ddiDeviceInterface::NOPSCommand2;
        ConvertADCToPotFeedbacksVolts = ddiDeviceInterface::NOPSCommand2;
        ConvertMotorVoltagesToDACCommand = ddiDeviceInterface::NOPSCommand2;
        ConvertMotorCurrentsToDACCommand = ddiDeviceInterface::NOPSCommand2;
    }

    CMN_LOG(6)
        << *SetOutputAllCommand << std::endl
        << *GetInputAllCommand << std::endl
        << *LatchEncodersCommand << std::endl
        << *StartMotorCurrentsConvCommand << std::endl
        << *StartPotFeedbacksConvCommand << std::endl
        << *LoadMotorVoltagesCommand << std::endl
        << *LoadMotorCurrentsCommand << std::endl
        << *ReadPositionsCommand << std::endl
        << *ReadPotFeedbacksCommand << std::endl
        << *ReadVelocitiesCommand << std::endl
        << *ReadMotorCurrentsCommand << std::endl
        << *WriteMotorVoltagesCommand << std::endl
        << *WriteMotorCurrentsCommand << std::endl
        << *SetEncodersCommand << std::endl
        << *ResetEncodersCommand << std::endl
        << *EnableAmplifierCommand << std::endl
        << *DisableAmplifierCommand << std::endl
        << *ConvertFrequencyToShaftDegPerSCommand << std::endl
        << *ConvertMotorVoltagesToDACCommand << std::endl
        << *ConvertMotorCurrentsToDACCommand << std::endl
        << *ConvertADCToMotorCurrentsCommand << std::endl
        << *ConvertADCToPotFeedbacksVolts << std::endl
        << std::endl;

    CMN_LOG_CLASS(1) << "Completed ctfControlThread Configuration" << std::endl;

    if (Device) {
        outputVoltagesVolts->Data.SetAll(0);
        ConvertMotorVoltagesToDACCommand->Execute(*outputVoltagesVolts, *outputVoltagesDAC);
        
        //WriteMotorVoltagesCommand->Execute(*outputVoltagesDAC);
        //GSF
        SetOutputAllCommand->Execute(*outputVoltagesVolts); 

        LoadMotorVoltagesCommand->Execute();
        ConvertMotorCurrentsToDACCommand->Execute(*outputCurrentsmAmps, *outputCurrentsDAC);
        WriteMotorCurrentsCommand->Execute(*outputCurrentsDAC);
        CMN_LOG_CLASS(1) << "Setting currents to" << *outputCurrentsmAmps << std::endl;
        CMN_LOG_CLASS(1) << "Setting currents to" << *outputCurrentsDAC << std::endl;
        LoadMotorCurrentsCommand->Execute();
    }
}

void ctfControlThread::PreProcessData(void)
{
}

void ctfControlThread::PostProcessData(void)
{
}

// $Log: ctfControlThread.cpp,v $
// Revision 1.11  2006/07/09 04:05:08  kapoor
// cissstRobot: Flushing the current state of pre-robot libraries for safe keeping. Use at own risk.
// Adds New eye robot and original black steady hand (7 DoF) kinematics.
//
// Revision 1.10  2006/06/22 03:15:20  kapoor
// cisstRobot: Flushing the current state of pre-robot libraries for safe keeping.
// Use at own risk. Adds new 5 BAR + Snake robot
//
// Revision 1.9  2006/06/03 00:38:55  kapoor
// cisstRobot: Flushing the current state of pre-robot libraries for safe keeping.
// Use at own risk.
//
// Revision 1.8  2006/05/29 03:13:07  kapoor
// cisstMRC and networkRobot: Changes to the example. Still a work in progress.
//
// Revision 1.7  2006/05/27 03:46:29  kapoor
// cisstMRC: more code in examples, this should eventually make its way into
// the main library. For now it is checked in as examples.
//
// Revision 1.6  2006/05/14 04:12:43  kapoor
// cisstDeviceInterface Examples: Fixed Get/Set Gain data from IRE. Need
// wrapping for ddiGainData.
//
// Revision 1.5  2006/05/14 03:26:28  kapoor
// cisstDeviceInterface: Cleanup of controlThread and addition of IRE to this
// example. Also location of XML files is now in a more decent location.
//
// Revision 1.14  2006/05/02 02:49:58  kapoor
// Before major changes to include IRE.
//
// Revision 1.13  2006/04/06 20:40:59  kapoor
// Added a flag to not use pots for the stage 2.
//
// Revision 1.12  2006/03/27 01:39:18  kapoor
// Fixed the save value to file for gripper position.
//
// Revision 1.11  2006/03/17 19:52:29  kapoor
// Increased size of stored data.
//
// Revision 1.10  2005/12/24 18:06:02  kapoor
// Last working version before India trip.
//
// Revision 1.9  2005/12/22 15:07:01  kapoor
// mods to pots adjust.
//
// Revision 1.8  2005/12/21 23:24:01  kapoor
// Added current limit sensing.
//
// Revision 1.7  2005/12/19 03:23:10  kapoor
// increased the amount of data gathered.
//
// Revision 1.6  2005/12/15 05:45:22  kapoor
// Added flag to ignore pots in the Run method.
//
// Revision 1.5  2005/12/12 05:53:34  kapoor
// Fixed reset for LARS encs using pots. KAI's compensation.
//
// Revision 1.4  2005/12/05 07:32:10  kapoor
// Fixed ResetEncoder (The method is not used anywhere, but for correctness
//         it has been fixed)
//
// Revision 1.3  2005/11/07 00:44:13  kapoor
// Working version with VelocityMode=0 (sends command velocity directly)
//
// Revision 1.2  2005/10/23 18:07:21  kapoor
// Working version with both master and slave tested with angluar as well as
// positional velocity.
//
// Revision 1.1.1.1  2005/10/15 02:32:41  kapoor
// Working position MS
//
// Revision 1.4  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/08/29 00:25:18  kapoor
// controlThreadExample: Fixes (and also adds additional offsets) for setting
// encoders using pot feedback.
//
// Revision 1.2  2005/08/28 16:44:51  kapoor
// main.cpp
//
// Revision 1.1  2005/04/16 18:27:28  kapoor
// checking in controlthread code using FLTK for safe keeping
//
