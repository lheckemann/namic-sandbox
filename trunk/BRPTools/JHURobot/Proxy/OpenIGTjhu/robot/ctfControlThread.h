/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: ctfControlThread.h,v 1.9 2006/07/09 04:05:08 kapoor Exp $

#ifndef _ctfControlThread_h
#define _ctfControlThread_h

#include <cisstCommon/cmnDataObject.h>
#include <cisstRealTime/rtsTask.h>
#include <cisstRealTime/rtsCommand1.h>
#include <cisstDeviceInterface/ddiTypes.h>
/**
NOTE: IN CONTROL THREAD ALL VALUES ARE IN DEG AND MM IF NOTHING ELSE IS MENTIONED ABOUT THEM
**/
// this defines the history length used for computing the error dot term
#define CTF_CONTROL_DERROR_SIZE 4
#define CTF_CONTROL_MAX_AXIS 16
#define CTF_AXIS_IN_USE CTF_CONTROL_MAX_AXIS

#define MOTOR_CURRENT 1
#include <vector>
#include <iostream>

typedef ddiDynamicVectorDataObject<ddiGainData, CTF_CONTROL_MAX_AXIS> ddiVecGain16;
CMN_DECLARE_SERVICES_INSTANTIATION(ddiVecGain16);

class ctfControlThread: public rtsTask {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

protected:
    // these are the values read from the config file.
    // in control thread, all values are in mm and deg!

    // DegsPerCount here atcually represents counts per unit motion of the
    // joint. If it is translatory joint it would mean counts per mm
    // and rotary joint it would mean counts per deg.
    // it is computed once during reading of comfiguration file using
    // the formula
    // DegsPerCount = Pitch *  (1/GearRatio) * (1/360) * (1/CountsPerTurn)
    // Pitch is in MM per Deg for translatory joints
    // Pitch is 1 for rotary joints
    ddiVecDouble16 DegsPerCount;
    ddiVecDouble16 OffsetInDeg;
    ddiVecDouble16 CountsPerTurn;
    ddiVecDouble16 GearRatio;
    ddiVecDouble16 Pitch;
    ddiVecDouble16 SlopePerPotVolt;
    ddiVecDouble16 InterceptPotDeg;
    ddiVecBool16 UsePotsToAdjust; // if true pot is used to adjust encoders for absolute reading
    ddiVecDouble16 DefaultAdjustValues; // if pot is not used the adjust encoders to this value (in deg/mm)
    ddiVecBool16 UsePotsToCheck; // if true pot is used to check with encoders
    ddiVecDouble16 PotDifferenceLimit; // if pot is used as redundant check, encoder positions and pot positions (in deg/mm)
                                       // must match upto this limit
    ddiVecInt16 AmplifierNumber;
    ddiVecBool16 SimulateAxis;
    ddiDynamicVectorDataObject<ddiGainData, CTF_CONTROL_MAX_AXIS> Gains;

    // data from the card and corresponding converted quantities
    ddiVecLong16   *rawPositions;
    ddiVecShort16  *rawVelocities;
    ddiVecShort16  *rawCurrentFeedbacks;
    ddiVecShort16  *rawPotFeedbacks;
    ddiVecShort16  *outputVoltagesDAC;
    ddiVecShort16  *outputCurrentsDAC;
    ddiVecDouble16 *positions;
  //GSF  
    ddiVecShort16 *latchedindex;
    ddiVecDouble16 *velocities;
    ddiVecDouble16 *currentFeedbacks;
    ddiVecDouble16 *potFeedbacksVolts;
    ddiVecDouble16 *potFeedbacksDeg;
    ddiVecDouble16 *outputVoltagesVolts;
    ddiVecDouble16 *outputCurrentsmAmps;

    // for servo
    enum {
        MODE_DIRECT = 0,
        MODE_SERVO,
        MODE_SIMULATION
    };
    ddiVecInt16 ControllerMode;

    ddiVecDouble16 *commandPositions;
    ddiVecDouble16 *error;
    ddiVecDouble16 *derror0;
    ddiVecDouble16 *ierror;
    ddiVecBool16   *overLimit;
    // this is temp place holders
    ddiVecDouble16 desiredPositions;
    ddiVecDouble16 preverror;
    ddiVecDouble16 derror[4];
    // to provide a simple filter for derror
    ddiVecDouble16 *derrorFilter;
    ddiVecDouble16 previerror;

    // this is used if we use this to directly write velocity from UI
    ddiVecDouble16 CommandDirectVolts;

    bool UsedPotsThisCycle;
    int UseErrorLimit;
    int SimulationOnly;

    // things needed from the lower level device
    
    //DAC
    ddiCommand1Base* SetOutputAllCommand;

    //ADC
    ddiCommand1Base* GetInputAllCommand;

    //LoPoMoCo
    bool DeviceProvidesVelocities;
    ddiCommand0Base *LatchEncodersCommand;
    ddiCommand0Base *StartMotorCurrentsConvCommand;
    ddiCommand0Base *StartPotFeedbacksConvCommand;
    ddiCommand0Base *LoadMotorVoltagesCommand;
    ddiCommand0Base *LoadMotorCurrentsCommand;

    ddiCommand1Base *ReadPositionsCommand;
    //GSF
    ddiCommand1Base *ReadLatchedIndexCommand;
    ddiCommand1Base *ReadPotFeedbacksCommand;
    ddiCommand1Base *ReadVelocitiesCommand;
    ddiCommand1Base *ReadMotorCurrentsCommand;
    ddiCommand1Base *WriteMotorVoltagesCommand;
    ddiCommand1Base *WriteMotorCurrentsCommand;
    ddiCommand1Base *SetEncodersCommand;

    ddiCommand1Base* ResetEncodersCommand;
    ddiCommand1Base* EnableAmplifierCommand;
    ddiCommand1Base* DisableAmplifierCommand;

    ddiCommand2Base *ConvertFrequencyToShaftDegPerSCommand;
    ddiCommand2Base *ConvertMotorVoltagesToDACCommand;
    ddiCommand2Base *ConvertMotorCurrentsToDACCommand;
    ddiCommand2Base *ConvertADCToMotorCurrentsCommand;
    ddiCommand2Base *ConvertADCToPotFeedbacksVolts;

    //things provided by this device
    DDI_DECLARE_COMMAND2(EncoderCountsToDegs, ddiVecLong16, ddiVecDouble16);
    DDI_DECLARE_COMMAND2(PotFeedbackToDegs, ddiVecDouble16, ddiVecDouble16);

    // To allow another task to read from state data table
    DDI_DECLARE_COMMAND1_OUT(GetRawPositions, ddiVecLong16);
    DDI_DECLARE_COMMAND1_OUT(GetRawVelocities, ddiVecShort16);
    DDI_DECLARE_COMMAND1_OUT(GetRawCurrentFeedbacks, ddiVecShort16);
    DDI_DECLARE_COMMAND1_OUT(GetRawPotFeedbacks, ddiVecShort16);
    DDI_DECLARE_COMMAND1_OUT(GetOutputVoltagesDAC, ddiVecShort16);
    DDI_DECLARE_COMMAND1_OUT(GetOutputCurrentsDAC, ddiVecShort16);
    DDI_DECLARE_COMMAND1_OUT(GetPositions, ddiVecDouble16);
    //GSF
    DDI_DECLARE_COMMAND1_OUT(GetLatchedIndex, ddiVecShort16);
    DDI_DECLARE_COMMAND1_OUT(GetVelocities, ddiVecDouble16);
    DDI_DECLARE_COMMAND1_OUT(GetCurrentFeedbacks, ddiVecDouble16);
    DDI_DECLARE_COMMAND1_OUT(GetPotFeedbacksVolts, ddiVecDouble16);
    DDI_DECLARE_COMMAND1_OUT(GetPotFeedbacksDeg, ddiVecDouble16);
    DDI_DECLARE_COMMAND1_OUT(GetOutputVoltagesVolts, ddiVecDouble16);
    DDI_DECLARE_COMMAND1_OUT(GetOutputCurrentsmAmps, ddiVecDouble16);

    DDI_DECLARE_COMMAND1_OUT(GetCommandPositions, ddiVecDouble16);
    DDI_DECLARE_COMMAND1_OUT(GetErrors, ddiVecDouble16);
    DDI_DECLARE_COMMAND2(GetGain, ddiGainData, ddiGainData);
    DDI_DECLARE_COMMAND1_OUT(GetMode, ddiVecInt16);
    DDI_DECLARE_COMMAND1_OUT(GetUsePotsToAdjust, ddiVecBool16);
    DDI_DECLARE_COMMAND1_OUT(GetDefaultAdjustValue, ddiVecDouble16);
    DDI_DECLARE_COMMAND1_OUT(GetOverLimit, ddiVecBool16);

    RTS_DECLARE_COMMAND1(ResetEncoders, ddiVecBool16);
    RTS_DECLARE_COMMAND1(AdjustPositions, ddiVecBool16);

//gsf - logging
    RTS_DECLARE_COMMAND1(LogDataFile, ddiShort);

    RTS_DECLARE_COMMAND1(SetOutputCurrentsmAmps, ddiVecDouble16);
    RTS_DECLARE_COMMAND1(SetOutputVoltagesVolts, ddiVecDouble16);
    RTS_DECLARE_COMMAND1(SetCommandPositions, ddiVecDouble16);
    RTS_DECLARE_COMMAND1(SetGain, ddiGainData);
    RTS_DECLARE_COMMAND1(SetMode, ddiVecInt16);
    RTS_DECLARE_COMMAND1(SetUsePotsToAdjust, ddiVecBool16);
    RTS_DECLARE_COMMAND1(SetDefaultAdjustValues, ddiVecDouble16);

// GSF - updtae data type to match ddiLoPoMoCo.h
//    RTS_DECLARE_COMMAND1(Enable, ddiVecBool16);
//    RTS_DECLARE_COMMAND1(Disable, ddiVecBool16);

//    RTS_DECLARE_COMMAND1(Enable, ddiBasicTypeDataObject<short>);
//    RTS_DECLARE_COMMAND1(Disable, ddiBasicTypeDataObject<short>);

    RTS_DECLARE_COMMAND1(Enable, ddiShort);
    RTS_DECLARE_COMMAND1(Disable, ddiShort);

    // these are to be used internally
    virtual void AdjustEncoders(ddiVecBool16 &);
    virtual void AdjustController(unsigned int axis);
    virtual void AdjustControllerAll();
    virtual void ControllerMethodForPID(const unsigned int axis);
    virtual void ControllerMethodForSMC(const unsigned int axis);
    virtual void ControllerMethodForSimulation(const unsigned int axis);
    virtual void ControllerMethodForDirectVolt(const unsigned int axis);

//GSF-logging
    virtual void LogData();
    //std::string datalogStreamName;
    std::ofstream datalogStream;


    long Ticker;

    std::string logStreamName;
    std::ofstream logStream;
public:
    ctfControlThread(const char* taskName, double period);
    ~ctfControlThread();
    virtual void Run(void);
    virtual void Cleanup(void);
    virtual void Startup(void);
    virtual void Configure(const char *filename);
    virtual void PreProcessData(void);
    virtual void PostProcessData(void);
};
CMN_DECLARE_SERVICES_INSTANTIATION(ctfControlThread);


#endif // _ctfControlThread_h


// $Log: ctfControlThread.h,v $
// Revision 1.9  2006/07/09 04:05:08  kapoor
// cissstRobot: Flushing the current state of pre-robot libraries for safe keeping. Use at own risk.
// Adds New eye robot and original black steady hand (7 DoF) kinematics.
//
// Revision 1.8  2006/06/22 03:15:21  kapoor
// cisstRobot: Flushing the current state of pre-robot libraries for safe keeping.
// Use at own risk. Adds new 5 BAR + Snake robot
//
// Revision 1.7  2006/06/03 00:38:55  kapoor
// cisstRobot: Flushing the current state of pre-robot libraries for safe keeping.
// Use at own risk.
//
// Revision 1.6  2006/05/14 04:12:43  kapoor
// cisstDeviceInterface Examples: Fixed Get/Set Gain data from IRE. Need
// wrapping for ddiGainData.
//
// Revision 1.5  2006/05/14 03:26:28  kapoor
// cisstDeviceInterface: Cleanup of controlThread and addition of IRE to this
// example. Also location of XML files is now in a more decent location.
//
//
// ###########################################################################
// OLD LOG
// ###########################################################################
// Revision 1.7  2006/04/06 20:40:59  kapoor
// Added a flag to not use pots for the stage 2.
//
// Revision 1.6  2005/12/24 18:06:02  kapoor
// Last working version before India trip.
//
// Revision 1.5  2005/12/22 15:07:23  kapoor
// simple mods.
//
// Revision 1.4  2005/12/21 23:24:01  kapoor
// Added current limit sensing.
//
// Revision 1.3  2005/12/15 05:45:22  kapoor
// Added flag to ignore pots in the Run method.
//
// Revision 1.2  2005/11/07 00:44:13  kapoor
// Working version with VelocityMode=0 (sends command velocity directly)
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
