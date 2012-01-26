/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id$

#include <sys/io.h>

#ifndef _ctfControlThread_h
#define _ctfControlThread_h

#include <cisstMultiTask.h>
#include <ctfGainData.h>

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


static float axis1 = 0.0;
static float axis2 = 0.0;
static float axis3 = 0.0;
static float axis4 = 0.0;

typedef mtsVector<ctfGainData> ctfGainVec;
CMN_DECLARE_SERVICES_INSTANTIATION(ctfGainVec);

class ctfControlThread: public mtsTaskPeriodic {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

protected:

    //******************** CONFIGURATION DATA (from XML) *********************
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
    mtsDoubleVec DegsPerCount;
    mtsDoubleVec OffsetInDeg;
    mtsDoubleVec CountsPerTurn;
    mtsDoubleVec GearRatio;
    mtsDoubleVec Pitch;
    mtsDoubleVec SlopePerPotVolt;
    mtsDoubleVec InterceptPotDeg;
    mtsBoolVec UsePotsToAdjust; // if true pot is used to adjust encoders for absolute reading
    mtsDoubleVec DefaultAdjustValues; // if pot is not used the adjust encoders to this value (in deg/mm)
    mtsBoolVec UsePotsToCheck; // if true pot is used to check with encoders
    mtsDoubleVec PotDifferenceLimit; // if pot is used as redundant check, encoder positions and pot positions (in deg/mm)
                                       // must match up to this limit
    mtsIntVec AmplifierNumber;
    mtsBoolVec SimulateAxis;
    ctfGainVec Gains;

    int UseErrorLimit;
    int SimulationOnly;

    //******************** STATE TABLE DATA *********************
    // data from the card and corresponding converted quantities
    mtsLongVec rawPositions;
    mtsShortVec rawVelocities;
    mtsShortVec rawCurrentFeedbacks;
    mtsShortVec rawPotFeedbacks;
    mtsShortVec outputVoltagesDAC;
    mtsShortVec outputCurrentsDAC;
    mtsDoubleVec positions;
    //GSF  
    mtsShortVec latchedindex;
    mtsDoubleVec velocities;
    mtsDoubleVec currentFeedbacks;
    mtsDoubleVec potFeedbacksVolts;
    mtsDoubleVec potFeedbacksDeg;
    mtsDoubleVec outputVoltagesVolts;
    mtsDoubleVec outputCurrentsmAmps;

    // for servo

    // this sets the mode of the controller.
    // 0 means just write input speed & current to output
    // 1 means servo to position (voltage)
    // 2 means servo to position (current)
    enum {
        MODE_DIRECT = 0,
        MODE_SERVO,
        MODE_SIMULATION
    };
    mtsIntVec ControllerMode;

    mtsDoubleVec commandPositions;
    mtsDoubleVec error;
    mtsDoubleVec derror0;
    // to provide a simple filter for derror
    mtsDoubleVec derrorFilter;
    mtsDoubleVec ierror;
    mtsBoolVec   overLimit;

    // these are temp place holders (not State Table)
    mtsDoubleVec preverror;
    mtsDoubleVec derror[CTF_CONTROL_DERROR_SIZE];
    mtsDoubleVec previerror;

    // this is used if we use this to directly write velocity from UI
    mtsDoubleVec CommandDirectVolts;

    //******************** REQUIRED METHODS *********************
    // things needed from the lower level devices
    // The structs here are optional and are only used for readability.
    
    struct DACstruct {
        mtsFunctionWrite SetOutputAll;
    } DAC;

    struct ADCstruct {
        mtsFunctionRead GetInputAll;
    } ADC;

    struct LoPoMoCoStruct {
        bool DeviceProvidesVelocities;
        mtsFunctionVoid LatchEncoders;
        mtsFunctionVoid StartMotorCurrentsConv;
        mtsFunctionVoid StartPotFeedbacksConv;
        mtsFunctionVoid LoadMotorVoltages;
        mtsFunctionVoid LoadMotorCurrents;

        mtsFunctionRead ReadPositions;
        //GSF
        mtsFunctionRead ReadLatchedIndex;
        mtsFunctionRead ReadPotFeedbacks;
        mtsFunctionRead ReadVelocities;
        mtsFunctionRead ReadMotorCurrents;
        mtsFunctionWrite WriteMotorVoltages;
        mtsFunctionWrite WriteMotorCurrents;
        mtsFunctionWrite SetEncoders;

        mtsFunctionWrite ResetEncoders;
        mtsFunctionWrite EnableAmplifier;
        mtsFunctionWrite DisableAmplifier;

        mtsFunctionQualifiedRead ConvertFrequencyToShaftDegPerS;
        mtsFunctionQualifiedRead ConvertMotorVoltagesToDAC;
        mtsFunctionQualifiedRead ConvertMotorCurrentsToDAC;
        mtsFunctionQualifiedRead ConvertADCToMotorCurrents;
        mtsFunctionQualifiedRead ConvertADCToPotFeedbacksVolts;
    } LoPoMoCo;

    //******************** PROVIDED METHODS *********************
    //things provided by this device
    void EncoderCountsToDegs(const mtsLongVec &encs, mtsDoubleVec &degs) const;
    void PotFeedbackToDegs(const mtsDoubleVec &pots, mtsDoubleVec &degs) const;

    void GetGain(const ctfGainData &fromdata, ctfGainData &todata) const;

    // Methods to get/set some class data. Note that these methods would not be
    // necessary if UsePotsToAdjust and GetDefaultAdjustValues were placed in
    // the StateTable.
    void GetUsePotsToAdjust(mtsBoolVec &usepots) const
    { usepots = UsePotsToAdjust; }
    void SetUsePotsToAdjust(const mtsBoolVec &usepots)
    { UsePotsToAdjust = usepots; }
    void GetDefaultAdjustValues(mtsDoubleVec &vals) const
    { vals = DefaultAdjustValues; }
    void SetDefaultAdjustValues(const mtsDoubleVec &vals)
    { DefaultAdjustValues = vals; }

    void ResetEncoders(const mtsBoolVec &mask);
    void AdjustPositions(const mtsBoolVec &mask);

    //gsf - logging
    void LogDataFile(void);

    void SetOutputCurrentsmAmps(const mtsDoubleVec &mAmps);
    void SetOutputVoltagesVolts(const mtsDoubleVec &volts);
    void SetGain(const ctfGainData &gains);

    // GSF - update data type to match ddiLoPoMoCo.h (mtsBoolVec)
    void Enable(const mtsShortVec &masks);
    void Disable(const mtsShortVec &masks);

    //******************** MISC INTERNAL DATA/METHODS *********************
    bool UsedPotsThisCycle;

    // these are to be used internally
    virtual void AdjustEncoders(const mtsBoolVec &);
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
    ctfControlThread(const std::string & taskName, double period);
    ~ctfControlThread();
    virtual void Run(void);
    virtual void Cleanup(void);
    virtual void Startup(void);
    virtual void Configure(const std::string & filename);
    virtual void PreProcessData(void);
    virtual void PostProcessData(void);

//    void SetPositions(double p1, double p2, double p3, double p4);

};


//void RetrieveAxisPositions();


CMN_DECLARE_SERVICES_INSTANTIATION(ctfControlThread);

#endif // _ctfControlThread_h
