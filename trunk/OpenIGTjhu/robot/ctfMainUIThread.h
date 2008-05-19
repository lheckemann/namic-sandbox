/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: ctfMainUIThread.h,v 1.7 2006/07/09 04:05:08 kapoor Exp $

#ifndef _ctfMainUIThread_h
#define _ctfMainUIThread_h

#include <cisstCommon/cmnDataObject.h>
#include <cisstRealTime/rtsTask.h>
#include <cisstRealTime/rtsCommand1Base.h>
#include <vector>

#include "Switcher.h"
#include "Console.h"

#include "BRPtprOpenTracker.h"
#include "ctfControl.h"

#define CTF_CONTROL_DERROR_SIZE 4
#define CTF_CONTROL_MAX_AXIS 16
#define CTF_NO_CUI CTF_CONTROL_MAX_AXIS
#define STORE_POSITIONS 1
class ctfMainUIThread: public rtsTask {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

protected:
    ddiVecDouble16 desiredPosition;
    ddiVecDouble16 desiredDirectVolt;
    ddiInt Mode;
    ddiGainData gains;
    bool ExitFlag;

    long Ticker;
    rtsTask *IRE;

    ConsoleUI *cui;
    Switcher *sui;
    // max values for GUI
    ddiVecDouble16 MaxInput;
    ddiVecDouble16 MaxDGain;
    ddiVecDouble16 MaxPGain;
    ddiVecDouble16 MaxIGain;
    ddiVecDouble16 MaxDirectVolt;
    ddiVecDouble16 MinInput;
    ddiVecDouble16 MinDGain;
    ddiVecDouble16 MinPGain;
    ddiVecDouble16 MinIGain;
    ddiVecDouble16 MinDirectVolt;

    ddiCommand1Base *GetRawPositions;
    ddiCommand1Base *GetRawVelocities;
    ddiCommand1Base *GetRawCurrentFeedbacks;
    ddiCommand1Base *GetRawPotFeedbacks;
    ddiCommand1Base *GetOutputVoltagesDAC;
    ddiCommand1Base *GetOutputCurrentsDAC;
    ddiCommand1Base *GetPositions;
    ddiCommand1Base *GetVelocities;
    ddiCommand1Base *GetCurrentFeedbacks;
    ddiCommand1Base *GetPotFeedbacksVolts;
    ddiCommand1Base *GetPotFeedbacksDeg;
    ddiCommand1Base *GetOutputVoltagesVolts;
    ddiCommand1Base *GetOutputCurrentsmAmps;

    ddiCommand1Base *GetCommandPositions;
    ddiCommand1Base *GetErrors;
    ddiCommand2Base *GetGain;
    ddiCommand1Base *GetMode;
    ddiCommand1Base *GetUsePotsToAdjust;
    ddiCommand1Base *GetDefaultAdjustValue;

    ddiCommand1Base *ResetEncoders;
    ddiCommand1Base *AdjustPositions;

//gsf - logging
    ddiCommand1Base *LogDataFile;

    ddiCommand1Base *SetOutputCurrentsmAmps;
    ddiCommand1Base *SetOutputVoltagesVolts;
    ddiCommand1Base *SetCommandPositions;
    ddiCommand1Base *SetGain;
    ddiCommand1Base *SetMode;
    ddiCommand1Base *SetUsePotsToAdjust;
    ddiCommand1Base *SetDefaultAdjustValues;

#if STORE_POSITIONS
    double alphaPot, betaPot;
    int averagecounter;
    bool startAveraging;
#endif
   BRPtprOpenTracker OpenTracker;
   ctfControl RobotControl;
 public:

    ctfMainUIThread(const char* taskName, double period);
    ~ctfMainUIThread();
    virtual void Run(void);
    virtual void Cleanup(void);
    virtual void Startup(void);
    virtual void Configure(const char *filename);
    bool GetExitFlag() {return ExitFlag;}
};
CMN_DECLARE_SERVICES_INSTANTIATION(ctfMainUIThread);


#endif // _ctfMainUIThread_h


// $Log: ctfMainUIThread.h,v $
// Revision 1.7  2006/07/09 04:05:08  kapoor
// cissstRobot: Flushing the current state of pre-robot libraries for safe keeping. Use at own risk.
// Adds New eye robot and original black steady hand (7 DoF) kinematics.
//
// Revision 1.6  2006/05/14 04:12:43  kapoor
// cisstDeviceInterface Examples: Fixed Get/Set Gain data from IRE. Need
// wrapping for ddiGainData.
//
// Revision 1.5  2006/05/14 03:26:28  kapoor
// cisstDeviceInterface: Cleanup of controlThread and addition of IRE to this
// example. Also location of XML files is now in a more decent location.
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
