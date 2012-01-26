/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: ctfMainUIThread.h,v 1.7 2006/07/09 04:05:08 kapoor Exp $

#ifndef _ctfMainUIThread_h
#define _ctfMainUIThread_h

#include <cisstMultiTask.h>
#include "ctfGainData.h"
#include <vector>

#ifdef OLD_GUI
#include "Switcher-Old.h"
#include "Console-Old.h"
#else
#include "Switcher.h"
#include "Console.h"
#endif

#include "GlobalPosition.h"     // Nathan 2009-06-13
#include "BRPtprOpenTracker.h"
#include "ctfControl.h"

#define CTF_CONTROL_DERROR_SIZE 4
#define CTF_CONTROL_MAX_AXIS 16
#define CTF_NO_CUI CTF_CONTROL_MAX_AXIS
#define STORE_POSITIONS 1

//static float globalPositions[8];

class ctfMainUIThread: public mtsTaskPeriodic {  // PK: maybe should be mtsTaskContinuous?

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

protected:
    mtsDoubleVec desiredPosition;
    mtsDoubleVec desiredDirectVolt;
    mtsInt Mode;
    ctfGainData gains;
    bool ExitFlag;

    long Ticker;
    mtsTask *IRE;

    ConsoleUI *cui;
    Switcher *sui;
    GlobalPositionUI *gPosUI;       // Nathan 2009-06-13
    
    // max values for GUI
    mtsDoubleVec MaxInput;
    mtsDoubleVec MaxDGain;
    mtsDoubleVec MaxPGain;
    mtsDoubleVec MaxIGain;
    mtsDoubleVec MaxDirectVolt;
    mtsDoubleVec MinInput;
    mtsDoubleVec MinDGain;
    mtsDoubleVec MinPGain;
    mtsDoubleVec MinIGain;
    mtsDoubleVec MinDirectVolt;

    mtsFunctionRead GetRawPositions;
    mtsFunctionRead GetRawVelocities;
    mtsFunctionRead GetRawCurrentFeedbacks;
    mtsFunctionRead GetRawPotFeedbacks;
    mtsFunctionRead GetOutputVoltagesDAC;
    mtsFunctionRead GetOutputCurrentsDAC;
    mtsFunctionRead GetPositions;
    mtsFunctionRead GetVelocities;
    mtsFunctionRead GetCurrentFeedbacks;
    mtsFunctionRead GetPotFeedbacksVolts;
    mtsFunctionRead GetPotFeedbacksDeg;
    mtsFunctionRead GetOutputVoltagesVolts;
    mtsFunctionRead GetOutputCurrentsmAmps;

    mtsFunctionRead GetCommandPositions;
    mtsFunctionRead GetErrors;
    mtsFunctionQualifiedRead GetGain;
    mtsFunctionRead GetMode;
    mtsFunctionRead GetUsePotsToAdjust;
    mtsFunctionRead GetDefaultAdjustValues;

    mtsFunctionWrite ResetEncoders;
    mtsFunctionWrite AdjustPositions;

//gsf - logging
    mtsFunctionVoid LogDataFile;

    mtsFunctionWrite SetOutputCurrentsmAmps;
    mtsFunctionWrite SetOutputVoltagesVolts;
    mtsFunctionWrite SetCommandPositions;
    mtsFunctionWrite SetGain;
    mtsFunctionWrite SetMode;
    mtsFunctionWrite SetUsePotsToAdjust;
    mtsFunctionWrite SetDefaultAdjustValues;

#if STORE_POSITIONS
    double alphaPot, betaPot;
    int averagecounter;
    bool startAveraging;
#endif
   BRPtprOpenTracker OpenTracker;
   ctfControl RobotControl;
 public:

    ctfMainUIThread(const std::string & taskName, double period);
    ~ctfMainUIThread();
    virtual void Run(void);
    virtual void Cleanup(void);
    virtual void Startup(void);
    virtual void Configure(const std::string & filename);
    bool GetExitFlag() {return ExitFlag;}

//    void SavePositions(){
//        printf("Now save the axis positions on the file.\n");
//  FILE *pFile;
//        pFile = fopen("saveAxisPositions.txt", "w");
//        fprintf(pFile, "%.2f %.2f %.2f %.2f", globalPositions[0], globalPositions[1], globalPositions[2], globalPositions[3]);
//        fclose(pFile);
//  printf("Save Completed!\n");
//    }

};
CMN_DECLARE_SERVICES_INSTANTIATION(ctfMainUIThread);


#endif // _ctfMainUIThread_h
