/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: ctfMainUIThread.cpp,v 1.10 2006/07/09 04:05:08 kapoor Exp $


#include <sys/io.h>
#include <cisstCommon/cmnXMLPath.h>
#include <cisstInteractive/ireFramework.h>
#include "ctfMainUIThread.h"
#include "ireTask.h"
#include "ctfControlBase.h"

CMN_IMPLEMENT_SERVICES(ctfMainUIThread);


using namespace std;

ctfMainUIThread::ctfMainUIThread(const char* taskName, double period):
    rtsTask(taskName, period, false, 5),
    Ticker(0),
    IRE(0),
    RobotControl(8) //Initialize # of joints
{
    cui = new ConsoleUI[CTF_NO_CUI];
    sui = new Switcher(cui);
    ExitFlag = false;
    StateDataTable.Advance();
#if STORE_POSITIONS
    startAveraging = false;
#endif
    
    // Start the OpenTracker threads
    OpenTracker.Initialize();
    CMN_LOG(3) << "OpenTracker initialized" << endl;
    
    // Initialize the OpenTracker control object
    RobotControl.SetOpenTracker(&OpenTracker);
    //RobotControl.SetJointPositionsFlag = false;
    //RobotControl.SetVoltageOutputsFlag = false;
    CMN_LOG(3) << "OpenTracker RobotControl initialized" << endl;
}


ctfMainUIThread::~ctfMainUIThread() {
}


void ctfMainUIThread::Startup(void) {
    Mode.Data = 0;
    if (sui) {
        sui->show(0, NULL);
        sui->DirectVolts->value(1);
    }
    int i;
    if (cui) {
        for (i = 0; i < CTF_NO_CUI; i++) {
            cui[i].VoltInput->activate();
            cui[i].ZeroVolt->activate();
            cui[i].PositionInput->deactivate();
        }
    }
    ddiGainData gain;
    ddiVecDouble16 positions;
    if (Device) {
        GetPositions->Execute(positions);
        for (i = 0; i < CTF_NO_CUI; i++) {
            gain.AxisNumber = i;
            GetGain->Execute(gain, gain);
            if (cui) {
                cui[i].PGainSlider->value(gain.PGain);
                cui[i].DGainSlider->value(gain.DGain);
                cui[i].IGainSlider->value(gain.IGain);
                cui[i].PositionInput->value(positions[i]);
            }
        }
    }

;
}

void ctfMainUIThread::Run(void) {
    // get from display
    int i;
    const rtsTimeIndex now = StateDataTable.GetIndexWriter();

    bool update_input_flag = false;
    ddiVecBool16 adjustAll;
    adjustAll.Data.SetAll(true);

    //GSF
    // Disable Amps -> Turn off brake valves
    //DisableAmplifierCommand->Execute();


    ///////////////////////////////
    ///// NaviTrack Interface /////
    ///////////////////////////////

    // Check for New Command
    if (OpenTracker.IsThereNewCommand()) {
        //CMN_LOG(3) << "NaviTrack: New command received" << endl;
        OpenTracker.ProcessNextCommand(&RobotControl);
    }
  
    // Check if new command joint positions    
    if( RobotControl.IsThereNewTargetJointPositions() ){ 
    
        //ctf_LoadData(RobotControl.LoadTargetJointPositions, desiredPosition.Data, CTF_NO_CUI);
        RobotControl.LoadTargetJointPositions( desiredPosition.Data.Pointer() );
        
        /*
        float _tmp[CTF_NO_CUI];
        RobotControl.LoadTargetJointPositions( _tmp); // Load new command joint positions
        for (i = 0; i < CTF_NO_CUI; i++) {
            desiredPosition.Data[i] = _tmp[i];
        }*/
  
        SetCommandPositions->Execute(desiredPosition); // Set new command joint positions to StateDataTable   
        CMN_LOG(3) << "NaviTrack: Updated SetJointPositions: ";

        for (i = 0; i < CTF_NO_CUI; i++) {
            CMN_LOG(3) << desiredPosition.Data[i] << "   ";
            cui[i].PositionInput->value(desiredPosition.Data[i]);
        }
        CMN_LOG(3) << endl;
  
    }
    
    // Check if new command voltage outputs
    if( RobotControl.IsThereNewTargetVoltageOutputs() ){ 
        float _tmp[CTF_NO_CUI];
        RobotControl.LoadTargetVoltageOutputs(_tmp); // Load new command voltage outputs
        for (i = 0; i < CTF_NO_CUI; i++) {
            desiredDirectVolt.Data[i] = _tmp[i];
        } 
                
        SetOutputVoltagesVolts->Execute(desiredDirectVolt);
        // CMN_LOG(3) << "NaviTrack: Updated SetVoltageOutputs" << endl;
    } 


    
    ///////////////////////////////
    /////      IRE Stuff      /////
    ///////////////////////////////
    
    if (sui->StartIREFlag) {
        sui->StartIREFlag = false;
        CMN_LOG(1) << "Starting IRE From Sine 6" << std::endl;
        IRE = new ireTask("IRE");
//      ((ireTask*)IRE)->StartupString.assign("from controlThreadLibPython import *; objreg = cmnObjectRegister_Instance(); servo = objreg.FindObject(\"servo\")");
      ((ireTask*)IRE)->StartupString.assign("from cisstDeviceInterfacePython import *; from cisstRealTimePython import *; from MRRobot_ControlThreadLibPython import *; objreg = cmnObjectRegister_Instance(); servo = objreg.FindObject(\"servo\")");

        IRE->Create();
        IRE->Start();
    }

    if (sui->AdjustEncodersFlag) {
        // call adjust encoders command
        AdjustPositions->Execute(adjustAll);
        sui->AdjustEncodersFlag = false;
    }

    //gsf - logging
    if (sui->LogDataFlag) {
    CMN_LOG_CLASS(3) << "Received Log Data command" << std::endl;
        // call log data command
        LogDataFile->Execute(adjustAll);
        sui->LogDataFlag = false;
    }

    if (sui->DirectVoltsChanged) {
        // when the button is on we are in direct control mode so set the controller mode to 0
        // when the button is off we are in PID control mode so set the controller mode to 1
        if (sui->DirectVolts->value() == 0) {
            //button is off
            Mode.Data = 1;
            // disable direct volt control
            for (i = 0; i < CTF_NO_CUI; i++) {
                cui[i].VoltInput->deactivate();
                cui[i].ZeroVolt->deactivate();
                cui[i].PositionInput->activate();
            }
        } else {
            Mode.Data = 0;
            for (i = 0; i < CTF_NO_CUI; i++) {
                cui[i].VoltInput->activate();
                cui[i].ZeroVolt->activate();
                cui[i].PositionInput->deactivate();
            }
        }
        ddiVecInt16 controllerMode;
        GetMode->Execute(controllerMode);
        for (i = 0; i < (int)controllerMode.size(); i++) {
            if (controllerMode.Data.Element(i) != 2) {
                controllerMode.Data.Element(i) = Mode.Data;
            }
        }
        SetMode->Execute(controllerMode);
        sui->DirectVoltsChanged = false;
    }
    if (Mode.Data == 1) {// we use PID, hence read values else ignore them
        for (i = 0; i < CTF_NO_CUI; i++) {
            desiredPosition.Data[i] = cui[i].PositionInput->value();
            if ((bool) (cui[i].Update->value())) {
                CMN_LOG_CLASS(3) << "RUN: updated gains " << i << std::endl;
                gains.AxisNumber = i;
                GetGain->Execute(gains, gains);
                gains.PGain = cui[i].PGainSlider->value();
                gains.DGain = cui[i].DGainSlider->value();
                gains.IGain = cui[i].IGainSlider->value();
                SetGain->Execute(gains);
                cui[i].Update->value(0);
            }
            update_input_flag = update_input_flag || cui[i].InputChangedFlag;
        }
        if (update_input_flag) {
            CMN_LOG_CLASS(3) << "RUN: updated input" << std::endl;
            SetCommandPositions->Execute(desiredPosition);
            for (i = 0; i < CTF_NO_CUI; i++) cui[i].InputChangedFlag = false;
        }
    } else {
        for (i = 0; i < CTF_NO_CUI; i++) {
            desiredDirectVolt.Data[i] = cui[i].VoltInput->value();
            update_input_flag = update_input_flag || cui[i].VoltChangedFlag;
        }
        if (update_input_flag) {
            CMN_LOG_CLASS(3) << "RUN: update direct volt" << std::endl;
            SetOutputVoltagesVolts->Execute(desiredDirectVolt);
            for (i = 0; i < CTF_NO_CUI; i++) cui[i].VoltChangedFlag = false;
        }
    }

    // show in display
    ddiVecLong16 raw_positions;
    ddiVecDouble16 raw_pot_feedback;
    ddiVecDouble16 positions;
    ddiVecDouble16 velocities;
    ddiVecDouble16 pot_feedback;
    ddiVecDouble16 output_voltages;
    ddiVecDouble16 cmd_pos;
    ddiVecDouble16 errors;

    GetRawPositions->Execute(raw_positions);
    GetPositions->Execute(positions);
    GetPotFeedbacksVolts->Execute(raw_pot_feedback);
    GetPotFeedbacksDeg->Execute(pot_feedback);
    GetOutputVoltagesVolts->Execute(output_voltages);
    GetErrors->Execute(errors);
    GetCommandPositions->Execute(cmd_pos);
    for (i = 0; i < CTF_NO_CUI; i++) {
        cui[i].EncoderCount->value(raw_positions[i]);
        cui[i].EncoderAngle->value(positions[i]);
        cui[i].PotVolt->value(raw_pot_feedback[i]);
        cui[i].PotAngle->value(pot_feedback[i]);
        cui[i].OutputVoltage->value(output_voltages[i]);
        cui[i].Error->value(errors[i]);


        // Update NaviTrack Data
        //RobotControl.ActualRawEncoderPositions[i] = raw_positions[i];
       // RobotControl.ActualJointPositions[i] = positions[i];
       // RobotControl.RawSensorInputs[i] = raw_pot_feedback[i];
       // RobotControl.SensorValues[i] = pot_feedback[i];
       // RobotControl.ActualVoltageOutputs[i] = output_voltages[i];
        //CMN_LOG(3) << RobotControl.ActualJointPositions[i] << "    ";
    }
    //CMN_LOG(3) << endl;
    

    
    // Update NaviTrack Data

    //ctf_SaveData(raw_positions, RobotControl.SaveActualRawEncoderPositions, CTF_NO_CUI);
    RobotControl.SaveActualRawEncoderPositions( raw_positions.Data.Pointer() );
    //CMN_LOG(3) << " pos: " << positions[0] << " " << positions[1] << " " << positions[2] << "\n";
    //ctf_SaveData(positions, RobotControl.SaveActualJointPositions, CTF_NO_CUI);
    RobotControl.SaveActualJointPositions( positions.Data.Pointer() );
    
    //ctf_SaveData(raw_pot_feedback, RobotControl.SaveActual2RawSensorInputs, CTF_NO_CUI);
    RobotControl.SaveActual2RawSensorInputs( raw_pot_feedback.Data.Pointer() );
    //ctf_SaveData(pot_feedback, RobotControl.SaveActual2SensorValues, CTF_NO_CUI);
    RobotControl.SaveActual2SensorValues( pot_feedback.Data.Pointer() );
    //ctf_SaveData(output_voltages, RobotControl.SaveActual2VoltageOutputs, CTF_NO_CUI);
    RobotControl.SaveActual2VoltageOutputs( output_voltages.Data.Pointer() );
  


    
#if STORE_POSITIONS
    if (sui->StorePositionsChanged) {
        averagecounter=0;
        sui->StorePositionsChanged = false;
        alphaPot = 0;
        betaPot = 0;
        startAveraging = true;
    }
    if (startAveraging) {
        averagecounter++;
        alphaPot += pot_feedback[0];
        betaPot += pot_feedback[1];
        if (averagecounter == 48) {
            CMN_LOG(1) << alphaPot/48.0 << ", " << betaPot/48.0 << std::endl;
            startAveraging = false;
            sui->StorePositions->value(0);
        }
    }
#endif

    if (Fl::check() == 0) ExitFlag = true;
    StateDataTable.Advance();
}

#include <fstream>
#include <cisstOSAbstraction/osaTime.h>
void ctfMainUIThread::Cleanup(void) {

    ///// IRE Stuff //////
    if (IRE) {
        CMN_LOG(1) << "Ending IRE..." << std::endl;
        IRE->Kill();
    }
    double PeriodinMS = Period*1e-6;
    CMN_LOG(1) << "Wait for IRE cleanup" << std::endl;
    long nloops = 0;
    if (IRE) {
        while (!IRE->IsTerminated()) {
            osaTime::Sleep((long)(PeriodinMS));
            if (nloops > 20) {
                CMN_LOG(1) << "Forced cleanup!" << std::endl;
                ireFramework::FinalizeShell();
                break; // force a cleanup, might me bad!
            }
            nloops++;
        }
    }

}

// Configures must be run in a bottom-up order.  eg a trajectory task
// needs to know somethings about what it is trying to control.
//
// A better example is that a servo task needs to know the hardaware
// before it can choose some pid values, maybe it could choose pid
// values intelligently.
void ctfMainUIThread::Configure(const char *filename) {
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
    CMN_LOG_CLASS(3) << "Configuring ctfMainUIThread" << std::endl;
    unsigned int axis;
    for (axis = 0; axis < MaxInput.Data.size(); axis++) {
        sprintf(path, "Axis[%d]/MaxInput/@Value", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, MaxInput[axis]);
        sprintf(path, "Axis[%d]/MaxDGain/@Value", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, MaxDGain[axis]);
        sprintf(path, "Axis[%d]/MaxPGain/@Value", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, MaxPGain[axis]);
        sprintf(path, "Axis[%d]/MaxIGain/@Value", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, MaxIGain[axis]);
        sprintf(path, "Axis[%d]/MaxDirectVolt/@Value", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, MaxDirectVolt[axis]);
        sprintf(path, "Axis[%d]/MinInput/@Value", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, MinInput[axis]);
        sprintf(path, "Axis[%d]/MinDGain/@Value", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, MinDGain[axis]);
        sprintf(path, "Axis[%d]/MinPGain/@Value", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, MinPGain[axis]);
        sprintf(path, "Axis[%d]/MinIGain/@Value", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, MinIGain[axis]);
        sprintf(path, "Axis[%d]/MinDirectVolt/@Value", axis+1);
        xmlConfig.GetXMLValue(context.c_str(), path, MinDirectVolt[axis]);
    }
    // since we have  only CTF_NO_CUI axis for now
    for (axis = 0 ; axis < CTF_NO_CUI; axis++) {
        cui[axis].PositionInput->bounds(MinInput.Data[axis], MaxInput.Data[axis]);
        cui[axis].DGainSlider->bounds(MinDGain.Data[axis], MaxDGain.Data[axis]);
        cui[axis].PGainSlider->bounds(MinPGain.Data[axis], MaxPGain.Data[axis]);
        cui[axis].IGainSlider->bounds(MinIGain.Data[axis], MaxIGain.Data[axis]);
        cui[axis].VoltInput->bounds(MinDirectVolt.Data[axis], MaxDirectVolt.Data[axis]);
    }
    if (Device) {
        CMN_LOG_CLASS(3) << "Configuring ctfMainUIThread" << std::endl;

        GetRawPositions = Device->GetMethodOneArg("GetRawPositions");
        GetRawVelocities = Device->GetMethodOneArg("GetRawVelocities");
        GetRawCurrentFeedbacks = Device->GetMethodOneArg("GetRawCurrentFeedbacks");
        GetRawPotFeedbacks = Device->GetMethodOneArg("GetRawPotFeedbacks");
        GetOutputVoltagesDAC = Device->GetMethodOneArg("GetOutputVoltagesDAC");
        GetOutputCurrentsDAC = Device->GetMethodOneArg("GetOutputCurrentsDAC");
        GetPositions = Device->GetMethodOneArg("GetPositions");
        GetVelocities = Device->GetMethodOneArg("GetVelocities");
        GetCurrentFeedbacks = Device->GetMethodOneArg("GetCurrentFeedbacks");
        GetPotFeedbacksVolts = Device->GetMethodOneArg("GetPotFeedbacksVolts");
        GetPotFeedbacksDeg = Device->GetMethodOneArg("GetPotFeedbacksDeg");
        GetOutputVoltagesVolts = Device->GetMethodOneArg("GetOutputVoltagesVolts");
        GetOutputCurrentsmAmps = Device->GetMethodOneArg("GetOutputCurrentsmAmps");

        GetCommandPositions = Device->GetMethodOneArg("GetCommandPositions");
        GetErrors = Device->GetMethodOneArg("GetErrors");
        GetGain = Device->GetMethodTwoArg("GetGain");
        GetMode = Device->GetMethodOneArg("GetMode");
        GetUsePotsToAdjust = Device->GetMethodOneArg("GetUsePotsToAdjust");
        GetDefaultAdjustValue = Device->GetMethodOneArg("GetDefaultAdjustValue");

        ResetEncoders = Device->GetMethodOneArg("ResetEncoders");
        AdjustPositions = Device->GetMethodOneArg("AdjustPositions");
    //gsf - logging
        LogDataFile = Device->GetMethodOneArg("LogDataFile");

        SetOutputCurrentsmAmps = Device->GetMethodOneArg("SetOutputCurrentsmAmps");
        SetOutputVoltagesVolts = Device->GetMethodOneArg("SetOutputVoltagesVolts");
        SetCommandPositions = Device->GetMethodOneArg("SetCommandPositions");
        SetGain = Device->GetMethodOneArg("SetGain");
        SetMode = Device->GetMethodOneArg("SetMode");
        SetUsePotsToAdjust = Device->GetMethodOneArg("SetUsePotsToAdjust");
        SetDefaultAdjustValues = Device->GetMethodOneArg("SetDefaultAdjustValues");
    } else {
        GetRawPositions = ddiDeviceInterface::NOPSCommand1;
        GetRawVelocities = ddiDeviceInterface::NOPSCommand1;
        GetRawCurrentFeedbacks = ddiDeviceInterface::NOPSCommand1;
        GetRawPotFeedbacks = ddiDeviceInterface::NOPSCommand1;
        GetOutputVoltagesDAC = ddiDeviceInterface::NOPSCommand1;
        GetOutputCurrentsDAC = ddiDeviceInterface::NOPSCommand1;
        GetPositions = ddiDeviceInterface::NOPSCommand1;
        GetVelocities = ddiDeviceInterface::NOPSCommand1;
        GetCurrentFeedbacks = ddiDeviceInterface::NOPSCommand1;
        GetPotFeedbacksVolts = ddiDeviceInterface::NOPSCommand1;
        GetPotFeedbacksDeg = ddiDeviceInterface::NOPSCommand1;
        GetOutputVoltagesVolts = ddiDeviceInterface::NOPSCommand1;
        GetOutputCurrentsmAmps = ddiDeviceInterface::NOPSCommand1;

        GetCommandPositions = ddiDeviceInterface::NOPSCommand1;
        GetErrors = ddiDeviceInterface::NOPSCommand1;
        GetGain = ddiDeviceInterface::NOPSCommand2;
        GetMode = ddiDeviceInterface::NOPSCommand1;
        GetUsePotsToAdjust = ddiDeviceInterface::NOPSCommand1;
        GetDefaultAdjustValue = ddiDeviceInterface::NOPSCommand1;

        ResetEncoders = ddiDeviceInterface::NOPSCommand1;
        AdjustPositions = ddiDeviceInterface::NOPSCommand1;

    //gsf - logging
        LogDataFile = ddiDeviceInterface::NOPSCommand1;

        SetOutputCurrentsmAmps = ddiDeviceInterface::NOPSCommand1;
        SetOutputVoltagesVolts = ddiDeviceInterface::NOPSCommand1;
        SetCommandPositions = ddiDeviceInterface::NOPSCommand1;
        SetGain = ddiDeviceInterface::NOPSCommand1;
        SetMode = ddiDeviceInterface::NOPSCommand1;
        SetUsePotsToAdjust = ddiDeviceInterface::NOPSCommand1;
        SetDefaultAdjustValues = ddiDeviceInterface::NOPSCommand1;
    }

    CMN_LOG(1)
        << *GetRawPositions << std::endl
        << *GetRawVelocities << std::endl
        << *GetRawCurrentFeedbacks << std::endl
        << *GetRawPotFeedbacks << std::endl
        << *GetOutputVoltagesDAC << std::endl
        << *GetOutputCurrentsDAC << std::endl
        << *GetPositions << std::endl
        << *GetVelocities << std::endl
        << *GetCurrentFeedbacks << std::endl
        << *GetPotFeedbacksVolts << std::endl
        << *GetPotFeedbacksDeg << std::endl
        << *GetOutputVoltagesVolts << std::endl
        << *GetOutputCurrentsmAmps << std::endl
        << *GetCommandPositions << std::endl
        << *GetErrors << std::endl
        << *GetGain << std::endl
        << *GetMode << std::endl
        << *GetUsePotsToAdjust << std::endl
        << *GetDefaultAdjustValue << std::endl
        << *ResetEncoders << std::endl
        << *AdjustPositions << std::endl
        << *LogDataFile << std::endl
        << *SetOutputCurrentsmAmps << std::endl
        << *SetOutputVoltagesVolts << std::endl
        << *SetCommandPositions << std::endl
        << *SetGain << std::endl
        << *SetMode << std::endl
        << *SetUsePotsToAdjust << std::endl
        << *SetDefaultAdjustValues << std::endl
        << std::endl;
}

// $Log: ctfMainUIThread.cpp,v $
// Revision 1.10  2006/07/09 04:05:08  kapoor
// cissstRobot: Flushing the current state of pre-robot libraries for safe keeping. Use at own risk.
// Adds New eye robot and original black steady hand (7 DoF) kinematics.
//
// Revision 1.9  2006/06/22 03:15:21  kapoor
// cisstRobot: Flushing the current state of pre-robot libraries for safe keeping.
// Use at own risk. Adds new 5 BAR + Snake robot
//
// Revision 1.8  2006/06/03 00:38:55  kapoor
// cisstRobot: Flushing the current state of pre-robot libraries for safe keeping.
// Use at own risk.
//
// Revision 1.7  2006/05/29 03:13:07  kapoor
// cisstMRC and networkRobot: Changes to the example. Still a work in progress.
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
