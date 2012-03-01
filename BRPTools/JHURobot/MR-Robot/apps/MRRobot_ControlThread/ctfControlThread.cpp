/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id$


#include <sys/io.h>
#include <cisstCommon/cmnXMLPath.h>
#include "ctfControlThread.h"
#include <math.h> // for gravity compensation, by Nathan
#define NO_AdjustEncoder 1

// 2010-10-04 Link length updated by Nathan
#define L1 118.67   // Link Length for axis 1
#define L2 119.09   // Link Length for axis 2
#define L3 119.70   // Link Length for axis 3
#define L4 119.80   // Link Length for axis 4

CMN_IMPLEMENT_SERVICES(ctfGainData);
CMN_IMPLEMENT_SERVICES_TEMPLATED(ctfGainVec);
CMN_IMPLEMENT_SERVICES(ctfControlThread);


//ctfControlThread* pAxis1 = (ctfControlThread*)NULL;




//void RetrieveAxisPositions()
//{
//    ctfControlThread* pControl = (ctfControlThread*) pAxis1;
//    //printf("axis1: %f", axis1);
//    //pControl->positions(0) = axis1;
//    //pControl->SetPositions((double)axis1, (double)axis2, (double)axis3, (double)axis4);
//    printf("Completed!\n");
//}


ctfControlThread::ctfControlThread(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 21000),
    SimulationOnly(1),
    Ticker(0),
    logStreamName(std::string("servo")+taskName+std::string(".csv")),
    logStream(logStreamName.c_str())
{
//axis1 = 0.0;
//axis2 = 0.0;
//axis3 = 0.0;
//axis4 = 0.0;
FILE *rFile;
rFile = fopen("axisPositions.txt", "r");
fscanf(rFile, "%f %f %f %f", &axis1, &axis2, &axis3, &axis4);
printf("Now the position of each axis will be set to the values from axisPositions.txt\n");
printf("axis1: %f\n", axis1);
printf("axis2: %f\n", axis2);
printf("axis3: %f\n", axis3);
printf("axis4: %f\n", axis4);

    // This was not set in the old code, so it probably defaulted
    // to false.
    LoPoMoCo.DeviceProvidesVelocities = false;

    // Set size of dynamic vectors
    // PK: This should be reduced from 16
    DegsPerCount.SetSize(CTF_CONTROL_MAX_AXIS);
    OffsetInDeg.SetSize(CTF_CONTROL_MAX_AXIS);
    CountsPerTurn.SetSize(CTF_CONTROL_MAX_AXIS);
    GearRatio.SetSize(CTF_CONTROL_MAX_AXIS);
    Pitch.SetSize(CTF_CONTROL_MAX_AXIS);
    SlopePerPotVolt.SetSize(CTF_CONTROL_MAX_AXIS);
    InterceptPotDeg.SetSize(CTF_CONTROL_MAX_AXIS);
    UsePotsToAdjust.SetSize(CTF_CONTROL_MAX_AXIS);
    DefaultAdjustValues.SetSize(CTF_CONTROL_MAX_AXIS);
    UsePotsToCheck.SetSize(CTF_CONTROL_MAX_AXIS);
    PotDifferenceLimit.SetSize(CTF_CONTROL_MAX_AXIS);
    AmplifierNumber.SetSize(CTF_CONTROL_MAX_AXIS);
    SimulateAxis.SetSize(CTF_CONTROL_MAX_AXIS);
    Gains.SetSize(CTF_CONTROL_MAX_AXIS);

    // Set size of state data and add to state table
    rawPositions.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(rawPositions, "Raw Positions");
    rawVelocities.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(rawVelocities, "Raw Velocities");
    rawCurrentFeedbacks.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(rawCurrentFeedbacks, "Raw Current Feedbacks");
    rawPotFeedbacks.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(rawPotFeedbacks, "Raw Pot Feedbacks");
    outputVoltagesDAC.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(outputVoltagesDAC, "Raw Motor Voltages");
    outputCurrentsDAC.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(outputCurrentsDAC, "Raw Motor Currents");
    positions.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(positions, "Positions");
    //GSF
    latchedindex.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(latchedindex, "Latched Index Positions");
    velocities.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(velocities, "Velocites");
    currentFeedbacks.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(currentFeedbacks, "Current Feedbacks");
    potFeedbacksVolts.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(potFeedbacksVolts, "Pot Feedback Volts");
    potFeedbacksDeg.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(potFeedbacksDeg, "Pot Feedback Deg");
    outputVoltagesVolts.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(outputVoltagesVolts, "Motor Voltages");
    outputCurrentsmAmps.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(outputCurrentsmAmps, "Motor Currents");
    // for servo
    ControllerMode.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(ControllerMode, "Controller Mode");
    commandPositions.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(commandPositions, "Command Positions");
    error.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(error, "Errors");
    derror0.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(derror0, "D Errors");
    derrorFilter.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(derrorFilter, "D Errors Filtered");
    ierror.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(ierror, "I Errors");
    overLimit.SetSize(CTF_CONTROL_MAX_AXIS);
    StateTable.AddData(overLimit, "OverLimit");

    // Set size of "temp place holders" variables
    preverror.SetSize(CTF_CONTROL_MAX_AXIS);
    unsigned int i;
    for (i = 0; i < CTF_CONTROL_DERROR_SIZE; i++)
        derror[i].SetSize(CTF_CONTROL_MAX_AXIS);
    previerror.SetSize(CTF_CONTROL_MAX_AXIS);
    CommandDirectVolts.SetSize(CTF_CONTROL_MAX_AXIS);

    // Set up required interfaces (DAC, ADC, Servo)
    mtsInterfaceRequired *required = AddInterfaceRequired("DAC");
    if (required)
        required->AddFunction("SetOutputAll", DAC.SetOutputAll);
    required = AddInterfaceRequired("ADC");
    if (required)
        required->AddFunction("GetInputAll", ADC.GetInputAll);
    required = AddInterfaceRequired("LoPoMoCo");
    if (required) {
        required->AddFunction("LatchEncoders", LoPoMoCo.LatchEncoders);
        required->AddFunction("StartMotorCurrentConv", LoPoMoCo.StartMotorCurrentsConv);
        required->AddFunction("StartPotFeedbackConv", LoPoMoCo.StartPotFeedbacksConv);
        required->AddFunction("LoadMotorVoltages", LoPoMoCo.LoadMotorVoltages);
        required->AddFunction("LoadCurrentLimits", LoPoMoCo.LoadMotorCurrents);

        required->AddFunction("GetPositions", LoPoMoCo.ReadPositions);

        //GSF
        required->AddFunction("GetLatchedIndex", LoPoMoCo.ReadLatchedIndex);

        required->AddFunction("GetPotFeedbacks", LoPoMoCo.ReadPotFeedbacks);
        required->AddFunction("GetVelocities", LoPoMoCo.ReadVelocities);
        required->AddFunction("GetMotorCurrents", LoPoMoCo.ReadMotorCurrents);
        required->AddFunction("SetMotorVoltages", LoPoMoCo.WriteMotorVoltages);
        required->AddFunction("SetCurrentLimits", LoPoMoCo.WriteMotorCurrents);
        required->AddFunction("SetPositions", LoPoMoCo.SetEncoders);

        required->AddFunction("ResetEncoders", LoPoMoCo.ResetEncoders);

        required->AddFunction("Enable", LoPoMoCo.EnableAmplifier);
        required->AddFunction("Disable", LoPoMoCo.DisableAmplifier);

        required->AddFunction("FrequencyToRPS", LoPoMoCo.ConvertFrequencyToShaftDegPerS);
        required->AddFunction("ADCToMotorCurrents", LoPoMoCo.ConvertADCToMotorCurrents);
        required->AddFunction("ADCToPotFeedbacks", LoPoMoCo.ConvertADCToPotFeedbacksVolts);
        required->AddFunction("MotorVoltagesToDAC", LoPoMoCo.ConvertMotorVoltagesToDAC);
        required->AddFunction("CurrentLimitsToDAC", LoPoMoCo.ConvertMotorCurrentsToDAC);
    }

    // Set up provided interface
    mtsInterfaceProvided *provided = AddInterfaceProvided("MainInterface");
    if (provided) {
        provided->AddCommandRead(&ctfControlThread::GetUsePotsToAdjust, this,
                                 "GetUsePotsToAdjust", UsePotsToAdjust);
        provided->AddCommandWrite(&ctfControlThread::SetUsePotsToAdjust, this,
                                  "SetUsePotsToAdjust", UsePotsToAdjust);
        provided->AddCommandRead(&ctfControlThread::GetDefaultAdjustValues, this,
                                 "GetDefaultAdjustValues", DefaultAdjustValues);
        provided->AddCommandWrite(&ctfControlThread::SetDefaultAdjustValues, this,
                                 "SetDefaultAdjustValues", DefaultAdjustValues);

        provided->AddCommandReadState(StateTable, rawPositions, "GetRawPositions");
        provided->AddCommandReadState(StateTable, rawVelocities, "GetRawVelocities");
        provided->AddCommandReadState(StateTable, rawCurrentFeedbacks, "GetRawCurrentFeedbacks");
        provided->AddCommandReadState(StateTable, rawPotFeedbacks, "GetRawPotFeedbacks");
        provided->AddCommandReadState(StateTable, outputVoltagesDAC, "GetOutputVoltagesDAC");
        provided->AddCommandReadState(StateTable, outputCurrentsDAC, "GetOutputCurrentsDAC");
        provided->AddCommandReadState(StateTable, positions, "GetPositions");
        //GSF
        provided->AddCommandReadState(StateTable, latchedindex, "GetLatchedIndex");
        provided->AddCommandReadState(StateTable, velocities, "GetVelocities");
        provided->AddCommandReadState(StateTable, currentFeedbacks, "GetCurrentFeedbacks");
        provided->AddCommandReadState(StateTable, potFeedbacksVolts, "GetPotFeedbacksVolts");
        provided->AddCommandReadState(StateTable, potFeedbacksDeg, "GetPotFeedbacksDeg");
        provided->AddCommandReadState(StateTable, outputVoltagesVolts, "GetOutputVoltagesVolts");
        provided->AddCommandReadState(StateTable, outputCurrentsmAmps, "GetOutputCurrentsmAmps");
        // for servo
        provided->AddCommandReadState(StateTable, ControllerMode, "GetMode");
        provided->AddCommandWriteState(StateTable, ControllerMode, "SetMode");
        provided->AddCommandReadState(StateTable, commandPositions, "GetCommandPositions");
        provided->AddCommandReadState(StateTable, error, "GetErrors");

        // Qualified read
        provided->AddCommandQualifiedRead(&ctfControlThread::GetGain, this,
                                          "GetGain", ctfGainData(), ctfGainData());

        // Write commands
        provided->AddCommandWrite(&ctfControlThread::ResetEncoders, this,
                                  "ResetEncoders", mtsBoolVec(CTF_CONTROL_MAX_AXIS));
        provided->AddCommandWrite(&ctfControlThread::AdjustPositions, this,
                                  "AdjustPositions", mtsBoolVec(CTF_CONTROL_MAX_AXIS));

        //gsf - logging
        provided->AddCommandVoid(&ctfControlThread::LogDataFile, this,
                                  "LogDataFile");

        provided->AddCommandWrite(&ctfControlThread::SetOutputCurrentsmAmps, this,
                                  "SetOutputCurrentsmAmps", outputCurrentsmAmps);
        provided->AddCommandWrite(&ctfControlThread::SetOutputVoltagesVolts, this,
                                  "SetOutputVoltagesVolts", outputVoltagesVolts);
        provided->AddCommandWriteState(StateTable, commandPositions, "SetCommandPositions");
        provided->AddCommandWrite(&ctfControlThread::SetGain, this,
                                  "SetGain", ctfGainData());
        provided->AddCommandWrite(&ctfControlThread::Enable, this,
                                  "Enable", mtsShortVec(CTF_CONTROL_MAX_AXIS));
        provided->AddCommandWrite(&ctfControlThread::Disable, this,
                                  "Disable", mtsShortVec(CTF_CONTROL_MAX_AXIS));

        provided->AddCommandQualifiedRead(&ctfControlThread::EncoderCountsToDegs, this,
                                          "EncoderCountsToDegs", rawPositions, positions);
        provided->AddCommandQualifiedRead(&ctfControlThread::PotFeedbackToDegs, this,
                                          "PotFeedbackToDegs", potFeedbacksVolts, potFeedbacksDeg);
    }
    ControllerMode.SetAll((unsigned int)ctfControlThread::MODE_SIMULATION); //default for safety

//    pAxis1 = this;


}

ctfControlThread::~ctfControlThread()
{
}

void ctfControlThread::ResetEncoders(const mtsBoolVec &mask)
{
    bool result = true;
    mtsShort axisIndex;
    for (axisIndex.Data = 0; axisIndex.Data < CTF_CONTROL_MAX_AXIS; axisIndex.Data++) {
        if (mask.at(axisIndex.Data)) {
            result &= LoPoMoCo.ResetEncoders(axisIndex);
        }
    }
}

// update gains for the i^th axis.
void ctfControlThread::SetGain(const ctfGainData &gain)
{
    Gains[gain.AxisNumber].PGain      = gain.PGain;
    Gains[gain.AxisNumber].DGain      = gain.DGain;
    Gains[gain.AxisNumber].IGain      = gain.IGain;
    Gains[gain.AxisNumber].DACLimit   = gain.DACLimit;
    Gains[gain.AxisNumber].ILimit     = gain.ILimit;
    Gains[gain.AxisNumber].ErrorLimit = gain.ErrorLimit;
    Gains[gain.AxisNumber].Offset     = gain.Offset;
}

// this can be called in current or direct output modes
void ctfControlThread::SetOutputVoltagesVolts(const mtsDoubleVec &volts)
{
    for (unsigned int axis = 0; axis < ControllerMode.size(); axis++) {
        if (ControllerMode.Element(axis) != MODE_SERVO) {
            CommandDirectVolts.Element(axis) = volts.Element(axis);
        }
    }
    outputVoltagesVolts = volts;
}

// this can be called in position or direct output modes
void ctfControlThread::SetOutputCurrentsmAmps(const mtsDoubleVec &mAmps)
{
    for (unsigned int axis = 0; axis < ControllerMode.size(); axis++) {
        if (ControllerMode.Element(axis) != MODE_SIMULATION) {
            outputCurrentsmAmps.Element(axis) = mAmps.Element(axis);
        }
    }
    outputCurrentsmAmps = mAmps;
}

// the UI can call this method to adjust encoders based on pot reading
void ctfControlThread::AdjustPositions(const mtsBoolVec &masks)
{
    this->AdjustEncoders(masks);
    AdjustControllerAll();
    overLimit.SetAll(false);
    //added oct 6th.
    //removed feb 21, 2009 (PK)
    //StateDataTable.Advance();
}

// gsf - logging
// the UI can call this method to log data to file
void ctfControlThread::LogDataFile(void)
{
    CMN_LOG_CLASS_INIT_VERBOSE << "Received Log Data command - control thread" << std::endl;
    LogData();
}

// UI to call these to enable / disable amps
void ctfControlThread::Enable(const mtsShortVec &masks)
{
    LoPoMoCo.EnableAmplifier(masks);
}

// UI to call these to enable / disable amps
void ctfControlThread::Disable(const mtsShortVec &masks)
{
    LoPoMoCo.DisableAmplifier(masks);
}

// Only using fromdata.AxisNumber -- could be replaced by mtsUInt
void ctfControlThread::GetGain(const ctfGainData &fromdata, ctfGainData &todata) const
{
    todata.AxisNumber = fromdata.AxisNumber;
    todata.PGain = Gains[fromdata.AxisNumber].PGain;
    todata.DGain = Gains[fromdata.AxisNumber].DGain;
    todata.IGain = Gains[fromdata.AxisNumber].IGain;
    todata.DACLimit = Gains[fromdata.AxisNumber].DACLimit;
    todata.ILimit = Gains[fromdata.AxisNumber].ILimit;
    todata.ErrorLimit = Gains[fromdata.AxisNumber].ErrorLimit;
    todata.Offset = Gains[fromdata.AxisNumber].Offset;
}

// DegsPerCount here actually represents counts per unit motion of the
// joint. If it is translatory joint it would mean counts per mm
// and rotary joint it would mean counts per deg.
// it is computed once during reading of configuration file using
// the formula:
// DegsPerCount = Pitch *  (1/GearRatio) * (360) * (1/CountsPerTurn)
// Pitch is in MM per Deg for translatory joints
// Pitch is 1 for rotary joints

// Here the input is raw input encoder counts and the output is joint
// position in deg (or mm) for rotary (resp translatory) joints.
void ctfControlThread::EncoderCountsToDegs(const mtsLongVec &encs, mtsDoubleVec &degs) const
{
    for (unsigned int i = 0; i < encs.size(); i++) {
        degs(i) = (double)(encs(i)) * DegsPerCount(i) + OffsetInDeg(i);
    }
}

// the input to this function is voltage output read from the pot in Volts
// the output of this function is absolute position of the joint (in mm or in deg)
// depending on the type of joint.
//      y = m * x + c, where x is pot voltage, y is abs. position.
//      m, c must be calibrated and provided using the config file.
void ctfControlThread::PotFeedbackToDegs(const mtsDoubleVec &pots, mtsDoubleVec &degs) const
{
    for (unsigned int i = 0; i < pots.size(); i++) {
        degs(i) = pots(i) * SlopePerPotVolt(i) + InterceptPotDeg(i);
    }
}

#ifdef NO_AdjustEncoder
///////////
// GSF - Modify this to zero based on index pulse
///////////
// MOD FOR SNAKE
// this method would adjust the encoders based on current pot readings.
void ctfControlThread::AdjustEncoders(const mtsBoolVec &axisToAdjust)
{
/*
    // step 1: read raw pot values
    // step 2: convert pot values to deg or mm
    // step 3: copy these to positions
    // step 4: compute raw positions
    // step 5: load these new values

    // step 1
    LoPoMoCo.StartPotFeedbacksConv();
    ADC.GetInputAll(potFeedbacksVolts);

    // step 2
    //LoPoMoCo.ConvertADCToPotFeedbacksVolts(rawPotFeedbacks, potFeedbacksVolts);
    LoPoMoCo.PotFeedbackToDegs(potFeedbacksVolts, potFeedbacksDeg);

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
    for (unsigned int pp = 0; pp < rawPositions.size(); pp++) {
        if (axisToAdjust.Element(pp) == true) {
        if (UsePotsToAdjust.at(pp)) {
            rawPositions.at(pp) = (long)((potFeedbacksDeg.at(pp) - OffsetInDeg.at(pp))/DegsPerCount.at(pp));
            positions.at(pp) = potFeedbacksDeg.at(pp);
        } else {
            rawPositions.at(pp) = (long)(DefaultAdjustValues.at(pp)*DegsPerCount.at(pp) + OffsetInDeg.at(pp));
            positions.at(pp) = DefaultAdjustValues.at(pp);
            rawPotFeedbacks.at(pp) = 0;
            potFeedbacksVolts.at(pp) = 0;
            potFeedbacksDeg.at(pp) = 0;
        }
        }
    }
    CMN_LOG_CLASS_INIT_ERROR << "Base: got use pot value (T/F) " << UsePotsToAdjust << std::endl;
    CMN_LOG_CLASS_INIT_ERROR << "Base: got pot value (volts) " << *potFeedbacksVolts << std::endl;
    CMN_LOG_CLASS_INIT_ERROR << "Base: got pot value (deg) " << *potFeedbacksDeg << std::endl;
    CMN_LOG_CLASS_INIT_ERROR << "Base: got pot value (deg) " << *positions << std::endl;
    CMN_LOG_CLASS_INIT_ERROR << "Base: Setting encs to " << *rawPositions << std::endl;

    //step 5
    SetEncoders(rawPositions);
    mtsLongVec rawcheck(CTF_CONTROL_MAX_AXIS);
    // Latch the encoders
    LatchEncoders();
    // now datavalid field is not set, hence a zero gets copied in state
    ReadPositions(rawcheck);
    CMN_LOG_CLASS_INIT_ERROR << "Encs check to " << rawPositions - rawcheck << std::endl;
*/
//GSF - New function to zero encoders
/*
    for (unsigned int pp = 0; pp < rawPositions.size(); pp++) {
            rawPositions.at(pp) = 0;
            //positions.at(pp) = 0;
            //rawPotFeedbacks.at(pp) = 0;
            //potFeedbacksVolts.at(pp) = 0;
        }
    
    SetEncoders(rawPositions);
*/
    mtsLongVec rawcheck(CTF_CONTROL_MAX_AXIS);
    mtsShortVec latchcheck(CTF_CONTROL_MAX_AXIS);
    mtsShortVec offsets(CTF_CONTROL_MAX_AXIS);

 // Set offsets of index from zero position
    // Change to XML config file!!!

/* Nathan 2009-06-15
    offsets[0]=0;
//    offsets[1]=-2980; // for zero at top pos
    //offsets[1]=501;   // zero at full collapse (overlapping pins)
//    offsets[1]=0;
//    offsets[1]=3439;  // At vertical axis fully extended up
//    offsets[1]=110; // Encoder value at index pulse -> encoder @ 3439 fully(reachable) up and 0 @ fully(unreacahble) down
    offsets[1] = 0; // Nathan 2009-05-04
    
    //offsets[2]=-57;   // zero at center
    //offsets[2]=0;
//    offsets[2]=10;    // Encoder value at index pulse -> encoder @ 0 in center
    offsets[2]=62;  // 2008-11-11 by nathan


    //offsets[3]=0;
    offsets[3]=-2400;   // Encoder value at index pulse -> encoder @ 0 when fuly retracted to stop
*/

// Nathan 2009-06-15
    offsets[0] = -97; //-97.425;
    offsets[1] = 97;  //97.425;
    offsets[2] = -97; //-97.425;
    offsets[3] = 97; //97.425;
//
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
    LoPoMoCo.LatchEncoders();
    
    // Read in raw encoder positions
    LoPoMoCo.ReadPositions(rawcheck);
    CMN_LOG_CLASS_RUN_VERBOSE << "Raw Encoder Value: " << rawcheck << std::endl;

    // Read in latched raw index positions
    LoPoMoCo.ReadLatchedIndex(latchcheck);    
    CMN_LOG_CLASS_RUN_VERBOSE << "Latched Index Value: " << latchcheck << std::endl;

    mtsLongVec tempEnc(CTF_CONTROL_MAX_AXIS);
   // Adjust for offset to home encoder 
    for (unsigned int pp = 0; pp < tempEnc.size(); pp++) {
            //tempEnc.at(pp) = 0; // set all to zero
            tempEnc.at(pp) = rawcheck.at(pp) - latchcheck.at(pp) + offsets.at(pp); // set all based on latched index pulse
        //tempEnc.at(pp) = offsets.at(pp);   //set all based on offset from known position
        }
    
   // Set new raw encoder position 
    LoPoMoCo.SetEncoders(tempEnc);



//GSF
//Test Digital IO
mtsShort temp_axis;
temp_axis = 0;
//LoPoMoCo.EnableAmplifier(temp_axis); //enable turn on/opens valve, releases brake
LoPoMoCo.DisableAmplifier(temp_axis);  //enable turn off/exhausts valve, activates brake
temp_axis = 1;
LoPoMoCo.DisableAmplifier(temp_axis);
temp_axis = 2;
LoPoMoCo.DisableAmplifier(temp_axis);
temp_axis = 3;
LoPoMoCo.DisableAmplifier(temp_axis);
}

#else

void ctfControlThread::AdjustEncoders(const mtsBoolVec &axisToAdjust)
{
    mtsLongVec rawcheck(CTF_CONTROL_MAX_AXIS);
    mtsShortVec latchcheck(CTF_CONTROL_MAX_AXIS);
    mtsShortVec offsets(CTF_CONTROL_MAX_AXIS);

 
// Nathan 2009-06-15
    offsets[0] = -97; //-97.425;
    offsets[1] = 97;  //97.425;
    offsets[2] = -97; //-97.425;
    offsets[3] = 97; //97.425;
//
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
    LoPoMoCo.LatchEncoders();
    
    mtsLongVec tempEnc(CTF_CONTROL_MAX_AXIS);
   // Adjust for offset to home encoder 
    for (unsigned int pp = 0; pp < tempEnc.size(); pp++) {
            tempEnc.at(pp) = offsets.at(pp); // set all based on latched index pulse
    }
    
   // Set new raw encoder position 
    LoPoMoCo.SetEncoders(tempEnc);
}
#endif

// this method would adjust the command positions based on current positions reading
// it is important to have this, so that the motors don't jump when we suddenly turn
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
    error.SetAll(0.0);
    ierror.SetAll(0.0);
    derror0.SetAll(0.0);
    commandPositions = positions;
}

void ctfControlThread::AdjustController(unsigned int axis)
{
    error.Element(axis) = 0.0;
    ierror.Element(axis) = 0.0;
    derror0.Element(axis) = 0.0;
    commandPositions.Element(axis) = positions.Element(axis);
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
    if (ierror[axis] >= Gains[axis].ILimit)
        ierror[axis] = Gains[axis].ILimit;
    if (ierror[axis] <= -Gains[axis].ILimit)
        ierror[axis] = -Gains[axis].ILimit;
    //GSF
    //outputVoltagesVolts[axis] += Gains[axis].IGain*ierror[axis];
    outputchange += Gains[axis].IGain*ierror[axis];
    overLimit[axis] = false;
    if ((UsePotsToCheck[axis] > 0) &&
            (fabs(positions[axis]- potFeedbacksDeg[axis]) >= PotDifferenceLimit[axis])) overLimit[axis] = true;
#if 0
    switch(UseErrorLimit) {
        case 2:
            if (fabs(error[axis]) >= Gains[axis].ErrorLimit)
                OverLimit = true;
            break;
        case 0:
            // no error check
            break;
        case 1:
        default:
            if (error[axis] >= Gains[axis].ErrorLimit)
                error[axis] = Gains[axis].ErrorLimit;
            if (error[axis] <= -Gains[axis].ErrorLimit)
                error[axis] = -Gains[axis].ErrorLimit;
            break;
    }
#endif
    //GSF
    //outputVoltagesVolts[axis] += Gains[axis].PGain*error[axis];
    //outputVoltagesVolts[axis] += Gains[axis].DGain*derrorFilter[axis];
    //outputVoltagesVolts[axis] = -1.0*outputVoltagesVolts[axis];
    //outputVoltagesVolts[axis] += Gains[axis].Offset;
    outputchange += Gains[axis].PGain*error[axis];
    outputchange += Gains[axis].DGain*derrorFilter[axis];
    //outputchange = -1.0*outputchange;
    outputchange += Gains[axis].Offset;

    // set a upper limit. not to blow the voltage too high
    if (outputchange >= Gains[axis].DACLimit)
        outputchange = Gains[axis].DACLimit;
    if (outputchange <= -Gains[axis].DACLimit)
        outputchange = -Gains[axis].DACLimit;

    //GSF - for differential output
    if (outputchange>0){
        if (overLimit[axis])
            outputVoltagesVolts[axis] = Gains[axis].Offset;           
        else
            outputVoltagesVolts[axis] = outputchange;
    } else {
        if (overLimit[axis]) 
            outputVoltagesVolts[axis+8] = Gains[axis+8].Offset;
        else
            outputVoltagesVolts[axis+8] = -outputchange;
    }

    /*
    // set a upper limit. not to blow the voltage too high
    if (outputVoltagesVolts[axis] >= Gains[axis].DACLimit)
        outputVoltagesVolts[axis] = Gains[axis].DACLimit;
    if (outputVoltagesVolts[axis] <= -Gains[axis].DACLimit)
        outputVoltagesVolts[axis] = -Gains[axis].DACLimit;
    //GSF
    if (outputVoltagesVolts[axis+8] >= Gains[axis+8].DACLimit)
        outputVoltagesVolts[axis+8] = Gains[axis+8].DACLimit;
    if (outputVoltagesVolts[axis+8] <= -Gains[axis+8].DACLimit)
        outputVoltagesVolts[axis+8] = -Gains[axis+8].DACLimit;
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
    S = 2*Gains[axis].DGain*derrorFilter[axis] + Gains[axis].DGain*Gains[axis].DGain*error[axis];

/*
    //TEMP
    // NOTE: Can have different sliding modes as approaches target
    if(fabs(error[axis])<10){
        lambda = lambda * error[axis]/10;
    }
*/
    //S = derrorFilter[axis] + lambda*error[axis];
    //          mm/ms                       mm
//  S = lambda*derrorFilter[axis] + error[axis];


    //TEMP
        //S = error[axis];
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

    //TEMP
    //u_s = 0;

    // To maintain on sliding surface
    //u_eq = -mass*lambda*derrorFilter[axis];
    u_eq = mass*lambda*derrorFilter[axis]*1000;    // sign seems right now
    //        kg                mm/ms=m/s
    // NOTE: Can add friction terms and external load here

        //TEMP
    //u_eq = 0;
    
    outputforce = u_eq + u_s; // in Newtons
    

/*      

    //GSF - change to be added to 'axis' and subtracted from 'axis+8'
    double outputchange=0;

    // set a upper limit. not to blow the voltage too high
    if (ierror[axis] >= Gains[axis].ILimit)
        ierror[axis] = Gains[axis].ILimit;
    if (ierror[axis] <= -Gains[axis].ILimit)
        ierror[axis] = -Gains[axis].ILimit;
    //GSF
    //outputVoltagesVolts[axis] += Gains[axis].IGain*ierror[axis];
    outputchange += Gains[axis].IGain*ierror[axis];
    overLimit[axis] = false;
    if ((UsePotsToCheck[axis] > 0) &&
            (fabs(positions[axis]- potFeedbacksDeg[axis]) >= PotDifferenceLimit[axis])) overLimit[axis] = true;
#if 0
    switch(UseErrorLimit) {
        case 2:
            if (fabs(error[axis]) >= Gains[axis].ErrorLimit)
                OverLimit = true;
            break;
        case 0:
            // no error check
            break;
        case 1:
        default:
            if (error[axis] >= Gains[axis].ErrorLimit)
                error[axis] = Gains[axis].ErrorLimit;
            if (error[axis] <= -Gains[axis].ErrorLimit)
                error[axis] = -Gains[axis].ErrorLimit;
            break;
    }
#endif
    //GSF
    //outputVoltagesVolts[axis] += Gains[axis].PGain*error[axis];
    //outputVoltagesVolts[axis] += Gains[axis].DGain*derrorFilter[axis];
    //outputVoltagesVolts[axis] = -1.0*outputVoltagesVolts[axis];
    //outputVoltagesVolts[axis] += Gains[axis].Offset;
    outputchange += Gains[axis].PGain*error[axis];
    outputchange += Gains[axis].DGain*derrorFilter[axis];
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
        outputVoltagesVolts[axis] = outputforce / 6.7877;  // Conver to Bar, 67.877mm^2
        outputVoltagesVolts[axis+8] = 0;
    } else {        // retract cylinder
        outputVoltagesVolts[axis] = 0;
        outputVoltagesVolts[axis+8] = -outputforce / 5.9984;   // Conver to Bar, 59.984mm^2

    }

// GSF TEMP - REMOVE GRAVITY COMP
    
//GSF - 2-18-08
// Add feedforward breakaway pressure, should allow much lower gains
// Y = -0.076X + 12.446      R^2=0.995
//  Y = Voltage output before breakaway
//  X = vertical position in mm after zeroing
if(axis==1){
//    outputVoltagesVolts[1] += ( positions[1]*(-0.076) + 12.446 );  // 2009-05-04
//  outputVoltagesVolts[1] += ( positions[1]*(-0.076) + 12.2 );       // dropped gravity comp a bit
//  outputVoltagesVolts[1] += ( positions[1]*(-0.076) + 13.4 );       // dropped gravity comp a bit

    // 2008-11-12 by Nathan
//  outputVoltagesVolts[1] += ( 77.611 * exp(-0.035 * positions[1]) );

    // 2008-11-28 by Nathan Cho
//  outputVoltagesVolts[1] += ( 28.202 * exp(-0.021 * positions[1]) );
//    printf("outputVoltagesVolts = %f\n",outputVoltagesVolts[1]);

}


//GSF - 2-20-08
// Add scaled up force as further from zero position
// Apply up to 50% additional force at edge of travel(50mm from center) scaled linearly
/* Nathan 2009-06-20
if(axis==2){
    if(outputVoltagesVolts[2]>0){
        outputVoltagesVolts[2] += outputVoltagesVolts[2] * (fabs(positions[2])/50) * 0.5;
    }
    if(outputVoltagesVolts[10]>0){
        outputVoltagesVolts[10] += outputVoltagesVolts[10] * (fabs(positions[2])/50) * 0.5;
    }
}
*/

if(axis==2){
}
/*
    if (overLimit[axis]) outputVoltagesVolts[axis] = Gains[axis].Offset;
    //GSF
    if (overLimit[axis+8]) outputVoltagesVolts[axis+8] = Gains[axis+8].Offset;
*/
  

  //TEMP
    // set a upper limit. not to blow the voltage too high
    if (outputVoltagesVolts[axis] >= 6.5)//Gains[axis].DACLimit)
        outputVoltagesVolts[axis] = 6.5;//Gains[axis].DACLimit;
    if (outputVoltagesVolts[axis] <= 0)
        outputVoltagesVolts[axis] = 0;
    //GSF
    if (outputVoltagesVolts[axis+8] >= 6.5)//Gains[axis+8].DACLimit)
        outputVoltagesVolts[axis+8] = 6.5;//Gains[axis+8].DACLimit;
    if (outputVoltagesVolts[axis+8] <= 0)
        outputVoltagesVolts[axis+8] = 0;
    

}

void ctfControlThread::ControllerMethodForSimulation(const unsigned int axis)
{
    positions.Element(axis) = commandPositions.Element(axis);
    rawPositions.Element(axis) = (long)(positions.Element(axis)*DegsPerCount.Element(axis) + OffsetInDeg.Element(axis));
    error.Element(axis) = 0.0;
    derror0.Element(axis) = 0.0;
    ierror.Element(axis) = 0.0;
    overLimit.Element(axis) = false;
    outputVoltagesVolts.Element(axis) = 0.0;
    outputCurrentsmAmps.Element(axis) = 0.0;
}

// this method is used if we directly write motor voltage (hence speed) from the UI
void ctfControlThread::ControllerMethodForDirectVolt(const unsigned int axis)
{
    outputVoltagesVolts[axis] = CommandDirectVolts[axis];
    // set a upper limit. not to blow the voltage too high
    if (outputVoltagesVolts[axis] >= Gains[axis].DACLimit)
        outputVoltagesVolts[axis] = Gains[axis].DACLimit;
    if (outputVoltagesVolts[axis] <= -Gains[axis].DACLimit)
        outputVoltagesVolts[axis] = -Gains[axis].DACLimit;
}

void ctfControlThread::Startup(void)
{
    outputVoltagesVolts.SetAll(0.0);
    commandPositions.SetAll(0.0);
    overLimit.SetAll(false);

    // Sent these initial values to the device.
    mtsExecutionResult ret1, ret2, ret3, ret4;

    ret1 = LoPoMoCo.ConvertMotorVoltagesToDAC(outputVoltagesVolts, outputVoltagesDAC);
    //ret2 = LoPoMoCo.WriteMotorVoltages(outputVoltagesDAC);
    //GSF
    ret2 = DAC.SetOutputAll(outputVoltagesVolts);
                
    LoPoMoCo.LoadMotorVoltages();
    ret3 = LoPoMoCo.ConvertMotorCurrentsToDAC(outputCurrentsmAmps, outputCurrentsDAC);
    ret4 = LoPoMoCo.WriteMotorCurrents(outputCurrentsDAC);
    CMN_LOG_CLASS_RUN_VERBOSE << "Setting currents to " << outputCurrentsmAmps << std::endl
                              << "Setting currents to " << outputCurrentsDAC << std::endl;
    LoPoMoCo.LoadMotorCurrents();


    //GSF - added to zero encoders at start of program
    // adjust encoders to pots values
    //this->AdjustEncoders(UsePotsToAdjust);

    for (unsigned int pp = 0; pp < rawPositions.size(); pp++) {
        rawPositions.at(pp) = 0;
    }
    LoPoMoCo.SetEncoders(rawPositions);
       
    // make sure that output of control algorithm is zero for adjusted encoders
    AdjustControllerAll();

    //GSF
    // Disable Amps -> Turn off brake valves
    //LoPoMoCo.DisableAmplifier();
}


void ctfControlThread::Run(void) {

    

    UsedPotsThisCycle = false;
    mtsStateIndex::TimeTicksType Ticks = StateTable.GetIndexWriter().Ticks();

    // See if any message is waiting in the mail box and execute the
    // first one.
    // PK: this executes all commands
    ProcessQueuedCommands();

#if MOTOR_CURRENT
    if (Ticks % 2 == 0)
#endif
    {
        LoPoMoCo.StartPotFeedbacksConv();
    }
#if MOTOR_CURRENT
    else
    {
        if (UsedPotsThisCycle == false) LoPoMoCo.StartMotorCurrentsConv();
    }
#endif

    // Latch the encoders
    LoPoMoCo.LatchEncoders();

    // read in the positions and velocities
    // velocities must be read before positions
    if (LoPoMoCo.DeviceProvidesVelocities) {
        LoPoMoCo.ReadVelocities(rawVelocities);
    }
    // now datavalid field is not set, hence a zero gets copied in state
    LoPoMoCo.ReadPositions(rawPositions);

    //LoPoMoCo.ConvertEncodersToShaftRev(rawPositions, positions);
    EncoderCountsToDegs(rawPositions, positions);

    //GSF - update positions based on kinematics
    // ADD to external file later

   // Vertical: (linear enc in mm -> vert motion in mm)
//GSF - TEMP LINEAR, COMMENT OUT BELOW
//    printf("before position(1) = %f\n", positions(1));
//    positions(1) = (2*sqrt(2*positions(1)*68 - positions(1)*positions(1))) + 27;  
//      positions(1) = (2*sqrt(2*positions(1)*68 - positions(1)*positions(1)));  // Nathan Cho 2009-05-04
//    printf("after position(1) = %f\n", positions(1));

// 2008-11-24 by Nathan
//    positions(1) = (-1.2763*pow(10,-12)*positions(1)*positions(1)*positions(1)*positions(1)) + (1.35962*pow(10,-8)*positions(1)*positions(1)*positions(1)) + (-5.72148*pow(10,-5)*positions(1)*positions(1)) + (0.13006*positions(1)) + 6.97088;

//    positions(1) = (-0.0000000000012763*(positions(1)*positions(1)*positions(1)*positions(1))) + (0.0000000135962*(positions(1)*positions(1)*positions(1))) + (-0.0000572148*(positions(1)*positions(1))) + (0.13006*positions(1)) + 6.97088;


    // Horizotal: (rotary enc in deg -> horiz motion in mm)
//    positions(2) = 120 * sin(positions(2));                     
//    positions(2) = 120 * sin( positions(2) * (2*3.14159)/360 );                     

/* Nathan 2009-06-15
    // 2008-11-26 by Nathan
    positions(2) = 130 * sin( positions(2) * (2*3.14159)/360 ); // 130 = 2 * 65(link length)
*/

    // positions(0): front left encoder
    // positions(1): front right encoder
    // positions(2): rear left encoder
    // positions(3): rear right encoder
//    positions(0) =  90.5 - positions(0);  // Axis 1 +
//    positions(1) = -90.5 - positions(1);  // Axis 2 -
//    positions(2) =  90.5 - positions(2);  // Axis 3 -
//    positions(3) = -90.5 - positions(3);  // Axis 4 +

    positions(0) =  axis1 - positions(0);   // Axis 1 +
    positions(1) =  axis2 - positions(1);   // Axis 2 -
    positions(2) =  axis3 - positions(2);   // Axis 3 -
    positions(3) =  axis4 - positions(3);   // Axis 4 +

//    positions(4) = positions(4);      // needle


    
    if (LoPoMoCo.DeviceProvidesVelocities) {
        LoPoMoCo.ConvertFrequencyToShaftDegPerS(rawVelocities, velocities);
    } else {
    // Compute your own, less accurate
    }

    // To read the potentiometers
#if MOTOR_CURRENT
    if (Ticks % 2 == 0)
#endif
    {
        ADC.GetInputAll(potFeedbacksVolts);
        //LoPoMoCo.ConvertADCToPotFeedbacksVolts(rawPotFeedbacks, potFeedbacksVolts);
        PotFeedbackToDegs(potFeedbacksVolts, potFeedbacksDeg);
    }
#if MOTOR_CURRENT
    else
    {
        // to read currents. we read current and pots on alternate cycles,
        // since pots and currents cant be read at the same time
        if (UsedPotsThisCycle == false) {
            LoPoMoCo.ReadMotorCurrents(rawCurrentFeedbacks);
            LoPoMoCo.ConvertADCToMotorCurrents(rawCurrentFeedbacks, currentFeedbacks);
        }
    }
#endif

    // process the data read from hardware before it is written on state table
    // and used in control algorithm. these can be used to perform controller
    // specific functions
    this->PreProcessData();

    //get last error and ierror term
    preverror = error;
    previerror = ierror;
    //get last 3 derror terms
    error.DifferenceOf(commandPositions, positions);
    derror[0].DifferenceOf(error, preverror);    
    // derror in mm/ms, based on desired velocity of 0
    ierror.SumOf(previerror, error);

    // filter the \dot e
    derrorFilter.Assign(derror[0]);
    derrorFilter.Add(derror[1]);
    derrorFilter.Add(derror[2]);
    derrorFilter.Add(derror[3]);
    derrorFilter.Divide(4.0);
    derror0.Assign(derror[0]);

    // this sets the mode of the controller.
    // 0 means just write input speed to output
    // 1 means servo to position
    outputVoltagesVolts.SetAll(0);
    unsigned int axis;
    for (axis = 0; axis < ControllerMode.size(); axis++) {
        if (SimulateAxis.Element(axis)) ControllerMode.Element(axis) = ctfControlThread::MODE_SIMULATION;
        if (ControllerMode.Element(axis) == ctfControlThread::MODE_DIRECT) {
            // calling this would enusre that when we switch mode we dont jump
            // unexpectedly to some value specified in target position
            // see definition of AdjustController for more details
            AdjustController(axis);
            this->ControllerMethodForDirectVolt(axis);
        } else if (ControllerMode.Element(axis) == ctfControlThread::MODE_SERVO ) {
            // this is a very simple point to point PID controller.
            // maintain a simple position servo NOTHING fancy. It works
            // so for most cases.
        //GSF - only run PID controller for first 8 axes, second 8 axes are 2nd channel for first 8
            if (axis<8){
              this->ControllerMethodForPID(axis);
// GSF - New Controller
//                this->ControllerMethodForSMC(axis); // Nathan Cho 2009-05-04
            }
        } else if (ControllerMode.Element(axis) == ctfControlThread::MODE_SIMULATION) {
            this->ControllerMethodForSimulation(axis);
        } else {
            /* bad mode do nothing */
        }
    }

    // process the results of the controll algorithm before they
    // are written on to the device and the state data table.
    this->PostProcessData();

    // First write voltage because it is used by the current
    // conversion to determine which slope to use based on sign of
    // voltage.
    LoPoMoCo.ConvertMotorVoltagesToDAC(outputVoltagesVolts, outputVoltagesDAC);
    
    //LoPoMoCo.WriteMotorVoltages(outputVoltagesDAC);
    //GSF
    DAC.SetOutputAll(outputVoltagesVolts);

    LoPoMoCo.ConvertMotorCurrentsToDAC(outputCurrentsmAmps, outputCurrentsDAC);
    LoPoMoCo.WriteMotorCurrents(outputCurrentsDAC);
    LoPoMoCo.LoadMotorVoltages();
    LoPoMoCo.LoadMotorCurrents();

    Ticker++;
}


// gsf-logging
void ctfControlThread::LogData()
{
    CMN_LOG_CLASS_RUN_VERBOSE << "Logging data to file..." << std::endl;

    datalogStream.open("DataLog.csv");

    if (datalogStream.is_open()) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Writing to data log file Servo states... "
                                  << StateTable.GetIndexWriter().Ticks() << std::endl;
        unsigned int numberOfDataToDump = 5;
        mtsGenericObject *dataToDump[] = {&positions, &commandPositions, &error, &outputVoltagesVolts, &potFeedbacksVolts};

        datalogStream << "need to replace with mtsCollector state, see example 3 in multiTaskTutorial" << std::endl;
        // StateTable.CSVWrite(datalogStream, dataToDump, numberOfDataToDump);
        CMN_LOG_CLASS_RUN_VERBOSE << "done data log file Servo states..." << std::endl;

        datalogStream.close();
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "...Done writing to file" << std::endl;
}

#include <fstream>
void ctfControlThread::Cleanup(void) {
    outputVoltagesDAC.SetAll(0);
    //LoPoMoCo.WriteMotorVoltages(outputVoltagesDAC);
    //GSF
    outputVoltagesVolts.SetAll(0);
    DAC.SetOutputAll(outputVoltagesVolts);

    LoPoMoCo.LoadMotorVoltages();
#if GRIPPERSPECIAL
    if ((strcmp("SVSN", GetName()) == 0) && (GripperFile != NULL)) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Writing gripper value " << positions[11] << " to file" << std::endl;
        int ret_val = fprintf(GripperFile, "%lf\n", positions[11]);
        if (ret_val < 0) {
            CMN_LOG_CLASS_RUN_ERROR << "***** Failed to save gripper value. Manual update of GripperHome.txt needed *****" << std::endl;
        }
        fclose(GripperFile);
    }
#endif

    // DATA LOGGING HERE
    if (logStream.is_open()) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Writing to file Servo states... " << StateTable.GetIndexWriter().Ticks() << std::endl;
        unsigned int numberOfDataToDump = 4;
        mtsGenericObject *dataToDump[] = {&positions, &commandPositions, &error, &outputVoltagesVolts};
//        unsigned int dataToDump[] = {positions, commandPositions, error, currentFeedbacks};
            //potFeedbacksDeg, currentFeedbacks, derrorFilter};
        logStream << "need to replace with mtsCollector state, see example 3 in multiTaskTutorial" << std::endl;
        // StateTable.CSVWrite(logStream, dataToDump, numberOfDataToDump);
        CMN_LOG_CLASS_RUN_VERBOSE << "done file Servo states..." << std::endl;
        logStream.close();
    }
}


// Configures must be run in a bottom-up order.  eg a trajectory task
// needs to know somethings about what it is trying to control.
//
// A better example is that a servo task needs to know the hardware
// before it can choose some pid values, maybe it could choose pid
// values intelligently.
void ctfControlThread::Configure(const std::string & filename) {
    cmnXMLPath xmlConfig;
    if (filename == "") {
        CMN_LOG_CLASS_INIT_ERROR << "Warning: Could not configure Servo task, configuration file missing!"
                                 << std::endl;
        return;
    }

    xmlConfig.SetInputSource(filename);
    char path[40];
    std::string context("/Config/Task[@Name=\"");
    context = context + GetName() + "\"]";

    CMN_LOG_CLASS_INIT_VERBOSE << "Configuring ctfControlThread" << std::endl;
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
        if (!ret) OffsetInDeg.at(axis) = 0.0;
        sprintf(path, "Axis[%d]/Pot/@Slope", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, SlopePerPotVolt[axis]);
        if (!ret) SlopePerPotVolt.at(axis) = 0.0;
        sprintf(path, "Axis[%d]/Pot/@Intercept", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, InterceptPotDeg[axis]);
        if (!ret) InterceptPotDeg.at(axis) = 0.0;
        sprintf(path, "Axis[%d]/Pot/@UsePotsToAdjust", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, use_pots_to_adjust); // defaults to true
        if (!ret) UsePotsToAdjust[axis] = true;
        else UsePotsToAdjust[axis] = (use_pots_to_adjust>0)?true:false;
        sprintf(path, "Axis[%d]/Pot/@DefaultAdjustValues", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, DefaultAdjustValues[axis]);
        if (!ret) DefaultAdjustValues.at(axis) = 0.0;
        sprintf(path, "Axis[%d]/Pot/@UsePotsToCheck", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, use_pots_to_check); // defaults to true
        if (!ret) UsePotsToCheck[axis] = true;
        else UsePotsToCheck[axis] = (use_pots_to_check>0)?true:false;
        sprintf(path, "Axis[%d]/Pot/@DifferenceLimit", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, PotDifferenceLimit[axis]);
        if (!ret) PotDifferenceLimit.at(axis) = 0.0;
        sprintf(path, "Axis[%d]/MaxCurrent/@Value", axis+1);
        ret = xmlConfig.GetXMLValue(context.c_str(), path, outputCurrentsmAmps[axis]);
        if (!ret) outputCurrentsmAmps[axis] = 150;
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
        CMN_LOG_CLASS_INIT_WARNING << "**** WARNING **** " << std::endl
                                   << "The config file " << filename << " has been set to use simulation only"
                                   << std::endl;
        SimulateAxis.SetAll(true);
        ControllerMode.SetAll((unsigned int)ctfControlThread::MODE_SIMULATION); //default for safety
    } else {
        for (unsigned int pp =0; pp < SimulateAxis.size(); pp++) {
            if (SimulateAxis.Element(pp) == true) {
                ControllerMode.Element(pp) = (unsigned int)ctfControlThread::MODE_SIMULATION;
            } else {
                ControllerMode.Element(pp) = (unsigned int)ctfControlThread::MODE_DIRECT;
            }
        }
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Read ctfControlThread Configuration" << std::endl;
    // DegsPerCount here atcually represents counts per unit motion of the
    // joint. If it is translatory joint it would mean counts per mm
    // and rotary joint it would mean counts per deg.
    // it is computed once during reading of comfiguration file using
    // the formula
    // DegsPerCount = Pitch *  (1/GearRatio) * (360) * (1/CountsPerTurn)
    // Pitch is in MM per Deg for translatory joints
    // Pitch is 1 for rotary joints
    for (unsigned int dp = 0; dp < DegsPerCount.size(); dp++) {
        DegsPerCount.at(dp) = (360.0*Pitch.at(dp))/(GearRatio.at(dp)*CountsPerTurn.at(dp));
    }

    CMN_LOG_CLASS_INIT_VERBOSE << "Completed ctfControlThread Configuration" << std::endl;
}

void ctfControlThread::PreProcessData(void)
{
}

void ctfControlThread::PostProcessData(void)
{
}

//void ctfControlThread::SetPositions(double p1, double p2, double p3, double p4)
//{
//    positions(0) = p1;
//    positions(1) = p2;
//    positions(2) = p3;
//    positions(3) = p4;
//}
