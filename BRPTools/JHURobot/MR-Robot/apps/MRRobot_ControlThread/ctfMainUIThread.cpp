/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: ctfMainUIThread.cpp,v 1.10 2006/07/09 04:05:08 kapoor Exp $

#include <stdio.h> // 2011-05-23 Nathan
#include <sys/io.h>
#include <cisstCommon/cmnXMLPath.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include "ctfMainUIThread.h"
#ifdef MRRobot_HAS_IRE
#include <cisstInteractive/ireFramework.h>
#include "ireTask.h"
#endif
#include "ctfControlBase.h"

#define LL 121.0    // Link Length

CMN_IMPLEMENT_SERVICES(ctfMainUIThread);


using namespace std;

ctfMainUIThread::ctfMainUIThread(const std::string & taskName, double period):
    mtsTaskPeriodic(taskName, period, false, 5),
    Ticker(0),
    IRE(0),
    RobotControl(8) //Initialize # of joints
{
    cui = new ConsoleUI[CTF_NO_CUI];
    gPosUI = new GlobalPositionUI();        // Nathan 2009-06-13
#ifdef OLD_GUI
    sui = new Switcher(cui, gPosUI);
#else
    sui = new Switcher(cui);    
#endif //OLD_GUI
    desiredPosition.SetSize(CTF_NO_CUI);
    desiredDirectVolt.SetSize(CTF_NO_CUI);
    MaxInput.SetSize(CTF_NO_CUI);
    MaxDGain.SetSize(CTF_NO_CUI);
    MaxPGain.SetSize(CTF_NO_CUI);
    MaxIGain.SetSize(CTF_NO_CUI);
    MaxDirectVolt.SetSize(CTF_NO_CUI);
    MinInput.SetSize(CTF_NO_CUI);
    MinDGain.SetSize(CTF_NO_CUI);
    MinPGain.SetSize(CTF_NO_CUI);
    MinIGain.SetSize(CTF_NO_CUI);
    MinDirectVolt.SetSize(CTF_NO_CUI);
    
    ExitFlag = false;
#if STORE_POSITIONS
   startAveraging = false;
#endif

    // Start the OpenTracker threads
    OpenTracker.Initialize();
    CMN_LOG_CLASS_INIT_VERBOSE << "OpenTracker initialized" << endl;
    
    // Initialize the OpenTracker control object
    RobotControl.SetOpenTracker(&OpenTracker);
    //RobotControl.SetJointPositionsFlag = false;
    //RobotControl.SetVoltageOutputsFlag = false;
    CMN_LOG_CLASS_INIT_VERBOSE << "OpenTracker RobotControl initialized" << endl;

    // Add a required interface
    mtsInterfaceRequired *req = AddInterfaceRequired("Controller");
    if (req) {
        req->AddFunction("GetRawPositions", GetRawPositions);
        req->AddFunction("GetRawVelocities", GetRawVelocities);
        req->AddFunction("GetRawCurrentFeedbacks",GetRawCurrentFeedbacks);
        req->AddFunction("GetRawPotFeedbacks", GetRawPotFeedbacks);
        req->AddFunction("GetOutputVoltagesDAC", GetOutputVoltagesDAC);
        req->AddFunction("GetOutputCurrentsDAC", GetOutputCurrentsDAC);
        req->AddFunction("GetPositions", GetPositions);
        req->AddFunction("GetVelocities", GetVelocities);
        req->AddFunction("GetCurrentFeedbacks", GetCurrentFeedbacks);
        req->AddFunction("GetPotFeedbacksVolts", GetPotFeedbacksVolts);
        req->AddFunction("GetPotFeedbacksDeg", GetPotFeedbacksDeg);
        req->AddFunction("GetOutputVoltagesVolts", GetOutputVoltagesVolts);

        req->AddFunction("GetCommandPositions", GetCommandPositions);
        req->AddFunction("GetErrors", GetErrors);
        req->AddFunction("GetGain", GetGain);
        req->AddFunction("GetMode", GetMode);
        req->AddFunction("GetUsePotsToAdjust", GetUsePotsToAdjust);
        req->AddFunction("GetDefaultAdjustValues", GetDefaultAdjustValues);

        req->AddFunction("ResetEncoders", ResetEncoders);
        req->AddFunction("AdjustPositions", AdjustPositions);
    //gsf - logging
        req->AddFunction("LogDataFile", LogDataFile);

        req->AddFunction("SetOutputCurrentsmAmps", SetOutputCurrentsmAmps);
        req->AddFunction("SetOutputVoltagesVolts", SetOutputVoltagesVolts);
        req->AddFunction("SetCommandPositions", SetCommandPositions);
        req->AddFunction("SetGain", SetGain);
        req->AddFunction("SetMode", SetMode);
        req->AddFunction("SetUsePotsToAdjust", SetUsePotsToAdjust);
        req->AddFunction("SetDefaultAdjustValues", SetDefaultAdjustValues);
    }
}


ctfMainUIThread::~ctfMainUIThread() {
}


void ctfMainUIThread::Startup(void) {
//////////////////////////////////////////////////////////////////
//    mtsLongVec raw_positions;
//    mtsDoubleVec positions;
//    mtsDoubleVec errors;

//    GetRawPositions(raw_positions);
//    GetPositions(positions);
//    GetErrors(errors);
//////////////////////////////////////////////////////////////////
    Mode.Data = 0;
    if (sui) {
        sui->show(0, NULL);
        sui->DirectVolts->value(1);
//////////////
//        sui->EncoderCount_01->value(raw_positions[0]);    // by Nathan 2009-01-20
//      cui[i].EncoderCount->value(raw_positions[i]);
//        sui.EncoderAngle_01->value(positions[0]);     // by Nathan 2009-01-20
//        sui.Error_01->value(errors[0]);           // by Nathan 2009-01-20
//////////////
    }
    int i;
    if (cui) {
        for (i = 0; i < CTF_NO_CUI; i++) {
            cui[i].VoltInput->activate();
            cui[i].ZeroVolt->activate();
            cui[i].PositionInput->deactivate();
        }
    }
    ctfGainData gain;
    mtsDoubleVec positions(CTF_CONTROL_MAX_AXIS);

    // If Startup was called, we know that GetPositions
    // and GetGain were successfully bound to commands.
    GetPositions(positions);
    for (i = 0; i < CTF_NO_CUI; i++) {
        gain.AxisNumber = i;
        GetGain(gain, gain);
        if (cui) {
            cui[i].PGainSlider->value(gain.PGain);
            cui[i].DGainSlider->value(gain.DGain);
            cui[i].IGainSlider->value(gain.IGain);
            cui[i].PositionInput->value(positions[i]);
        }
    }
}

void ctfMainUIThread::Run(void) {
    // get from display
    int i;
    this->Ticker++;

    bool update_input_flag = false;
    mtsBoolVec adjustAll(CTF_CONTROL_MAX_AXIS);

    adjustAll.SetAll(true);

    //GSF
    // Disable Amps -> Turn off brake valves
    //DisableAmplifierCommand();

    ///////////////////////////////
    ///// NaviTrack Interface /////
    ///////////////////////////////

    // Check for New Command
    if (OpenTracker.IsThereNewCommand()) {
        //CMN_LOG_CLASS_INIT_VERBOSE << "NaviTrack: New command received" << endl;
        OpenTracker.ProcessNextCommand(&RobotControl);
    }
  
    // Check if new command joint positions    
    if( RobotControl.IsThereNewTargetJointPositions() ){ 
    
        //ctf_LoadData(RobotControl.LoadTargetJointPositions, desiredPosition, CTF_NO_CUI);
        RobotControl.LoadTargetJointPositions( desiredPosition.Pointer() );
        
        /*
        float _tmp[CTF_NO_CUI];
        RobotControl.LoadTargetJointPositions( _tmp); // Load new command joint positions
        for (i = 0; i < CTF_NO_CUI; i++) {
            desiredPosition[i] = _tmp[i];
        }*/
  
        SetCommandPositions(desiredPosition); // Set new command joint positions to StateTable
        CMN_LOG_CLASS_RUN_VERBOSE << "NaviTrack: Updated SetJointPositions: " << desiredPosition << std::endl;

        for (i = 0; i < CTF_NO_CUI; i++) {
            cui[i].PositionInput->value(desiredPosition[i]);
        }
  
    }

    
    // Check if new command voltage outputs
    if( RobotControl.IsThereNewTargetVoltageOutputs() ){ 
        float _tmp[CTF_NO_CUI];
        RobotControl.LoadTargetVoltageOutputs(_tmp); // Load new command voltage outputs
        for (i = 0; i < CTF_NO_CUI; i++) {
            desiredDirectVolt[i] = _tmp[i];
        } 
                
        SetOutputVoltagesVolts(desiredDirectVolt);
        // CMN_LOG_CLASS_INIT_VERBOSE << "NaviTrack: Updated SetVoltageOutputs" << endl;
    } 


    
    ///////////////////////////////
    /////      IRE Stuff      /////
    ///////////////////////////////
    
    if (sui->StartIREFlag) {
        sui->StartIREFlag = false;
#ifdef MRRobot_HAS_IRE
        CMN_LOG_CLASS_INIT_VERBOSE << "Starting IRE From ctfMainUIThread" << std::endl;
        IRE = new ireTask("IRE");
        // PKAZ: Call to ireLogger.ClearTextOutput() is a workaround.
        ((ireTask*)IRE)->StartupString.assign(
                                "import ireLogger;"
                                "ireLogger.ClearTextOutput();"
                                "from cisstMultiTaskPython import *;"
                                "from MRRobot_ControlThreadLibPython import *;"
                                "TaskManager = cmnObjectRegister.FindObject('TaskManager');"
                                "TaskManager.UpdateFromC();"
                                "TaskManager.BSVO.UpdateFromC();"
                                "servo = TaskManager.BSVO.MainInterface;"
                                "servo.UpdateFromC();");
        IRE->Create();
        // PKAZ: temp
        //IRE->Start();
#else
        CMN_LOG_CLASS_INIT_WARNING << "IRE not enabled!" << std::endl;
#endif
    }

    if (sui->AdjustEncodersFlag) {
        // call adjust encoders command
        AdjustPositions(adjustAll);
        sui->AdjustEncodersFlag = false;
    }

    //gsf - logging
    if (sui->LogDataFlag) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Received Log Data command" << std::endl;
        // call log data command
        LogDataFile();
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
        mtsIntVec controllerMode(CTF_CONTROL_MAX_AXIS);
        GetMode(controllerMode);
        for (i = 0; i < (int)controllerMode.size(); i++) {
            if (controllerMode.Element(i) != 2) {
                controllerMode.Element(i) = Mode.Data;
            }
        }
        SetMode(controllerMode);
        sui->DirectVoltsChanged = false;
    }

    if (Mode.Data == 1) {// we use PID, hence read values else ignore them
        for (i = 0; i < CTF_NO_CUI; i++) {
            desiredPosition[i] = cui[i].PositionInput->value();
            if ((bool) (cui[i].Update->value())) {
                CMN_LOG_CLASS_RUN_VERBOSE << "RUN: updated gains " << i << std::endl;
                gains.AxisNumber = i;
                GetGain(gains, gains);
                gains.PGain = cui[i].PGainSlider->value();
                gains.DGain = cui[i].DGainSlider->value();
                gains.IGain = cui[i].IGainSlider->value();
                SetGain(gains);
                cui[i].Update->value(0);
            }
            update_input_flag = update_input_flag || cui[i].InputChangedFlag;
        }
        if (update_input_flag) {
            CMN_LOG_CLASS_RUN_VERBOSE << "RUN: updated input" << std::endl;
            SetCommandPositions(desiredPosition);
            for (i = 0; i < CTF_NO_CUI; i++) cui[i].InputChangedFlag = false;
        }
    } else {
        for (i = 0; i < CTF_NO_CUI; i++) {
            desiredDirectVolt[i] = cui[i].VoltInput->value();
            update_input_flag = update_input_flag || cui[i].VoltChangedFlag;
        }
        if (update_input_flag) {
            CMN_LOG_CLASS_RUN_VERBOSE << "RUN: update direct volt" << std::endl;
            SetOutputVoltagesVolts(desiredDirectVolt);
            for (i = 0; i < CTF_NO_CUI; i++) cui[i].VoltChangedFlag = false;
        }
    }

    // show in display
    mtsLongVec raw_positions(CTF_CONTROL_MAX_AXIS);
    mtsDoubleVec raw_pot_feedback(CTF_CONTROL_MAX_AXIS);
    mtsDoubleVec positions(CTF_CONTROL_MAX_AXIS);
    mtsDoubleVec velocities(CTF_CONTROL_MAX_AXIS);
    mtsDoubleVec pot_feedback(CTF_CONTROL_MAX_AXIS);
    mtsDoubleVec output_voltages(CTF_CONTROL_MAX_AXIS);
    mtsDoubleVec cmd_pos(CTF_CONTROL_MAX_AXIS);
    mtsDoubleVec errors(CTF_CONTROL_MAX_AXIS);

    GetRawPositions(raw_positions);
    GetPositions(positions);
    GetPotFeedbacksVolts(raw_pot_feedback);
    GetPotFeedbacksDeg(pot_feedback);
    GetOutputVoltagesVolts(output_voltages);
    GetErrors(errors);
    GetCommandPositions(cmd_pos);
    for (i = 0; i < CTF_NO_CUI; i++) {
#ifdef OLD_GUI
        cui[i].EncoderCount->value(raw_positions[i]); // by Nathan 2009-01-20
#else
        sui->EncoderCount_02->value(raw_positions[1]);  // by Nathan 2009-01-21
        sui->EncoderCount_10->value(raw_positions[9]);  // by Nathan 2009-01-21
#endif
#ifdef OLD_GUI
        cui[i].EncoderAngle->value(positions[i]); // by Nathan 2009-01-20
#else
        sui->EncoderAngle_02->value(positions[1]);  // by Nathan 2009-01-21
        sui->EncoderAngle_10->value(positions[9]);  // by Nathan 2009-01-21
#endif
        cui[i].PotVolt->value(raw_pot_feedback[i]);
        cui[i].PotAngle->value(pot_feedback[i]);
        cui[i].OutputVoltage->value(output_voltages[i]);
#ifdef OLD_GUI
        cui[i].Error->value(errors[i]);       // by Nathan 2009-01-20
#else
        sui->Error_02->value(errors[1]);        // by Nathan 2009-01-21
        sui->Error_10->value(errors[9]);        // by Nathan 2009-01-21
#endif
//        globalPositions[0] = positions[0];
//        globalPositions[1] = positions[1];
//        globalPositions[2] = positions[2];
//        globalPositions[3] = positions[3];
        // Update NaviTrack Data
        //RobotControl.ActualRawEncoderPositions[i] = raw_positions[i];
       // RobotControl.ActualJointPositions[i] = positions[i];
       // RobotControl.RawSensorInputs[i] = raw_pot_feedback[i];
       // RobotControl.SensorValues[i] = pot_feedback[i];
       // RobotControl.ActualVoltageOutputs[i] = output_voltages[i];
        //CMN_LOG_CLASS_INIT_VERBOSE << RobotControl.ActualJointPositions[i] << "    ";
    }
    //CMN_LOG_CLASS_INIT_VERBOSE << endl;

    gPosUI->curPosX->value((positions[0] + positions[1]) / 2);                      // Nathan 2009-06-13
    gPosUI->curPosY->value(-103.9 + 30 + sqrt(pow(LL, 2) - (pow((positions[0] - positions[1]), 2) / 4)));   // Nathan 2009-06-13
    gPosUI->curPosZ->value(-6.35 + 39.10);
    gPosUI->tPosX->value((positions[0] + positions[1] +  errors[0] + errors[1]) / 2);           // Nathan 2009-06-13
    gPosUI->tPosY->value(-103.9 + 30 + sqrt(pow(LL, 2) - (pow((positions[0] - positions[1] + errors[0] - errors[1]), 2) / 4))); // Nathan 2009-06-13
    gPosUI->tPosZ->value(-6.35 + 39.10+ errors[4]);
    

    
    // Update NaviTrack Data

    //ctf_SaveData(raw_positions, RobotControl.SaveActualRawEncoderPositions, CTF_NO_CUI);
    RobotControl.SaveActualRawEncoderPositions( raw_positions.Pointer() );
    //CMN_LOG_CLASS_INIT_VERBOSE << " pos: " << positions[0] << " " << positions[1] << " " << positions[2] << "\n";
    //ctf_SaveData(positions, RobotControl.SaveActualJointPositions, CTF_NO_CUI);
    RobotControl.SaveActualJointPositions( positions.Pointer() );
    
    //ctf_SaveData(raw_pot_feedback, RobotControl.SaveActual2RawSensorInputs, CTF_NO_CUI);
    RobotControl.SaveActual2RawSensorInputs( raw_pot_feedback.Pointer() );
    //ctf_SaveData(pot_feedback, RobotControl.SaveActual2SensorValues, CTF_NO_CUI);
    RobotControl.SaveActual2SensorValues( pot_feedback.Pointer() );
    //ctf_SaveData(output_voltages, RobotControl.SaveActual2VoltageOutputs, CTF_NO_CUI);
    RobotControl.SaveActual2VoltageOutputs( output_voltages.Pointer() );
  
    
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
            CMN_LOG_CLASS_RUN_VERBOSE << alphaPot/48.0 << ", " << betaPot/48.0 << std::endl;
            startAveraging = false;
            sui->StorePositions->value(0);
        }
    }
#endif


    if (Fl::check() == 0) ExitFlag = true;
}

#include <fstream>
#include <cisstOSAbstraction/osaSleep.h>
void ctfMainUIThread::Cleanup(void) {
    mtsDoubleVec positions(CTF_CONTROL_MAX_AXIS);
    GetPositions(positions);
    FILE * pFile;
    double buffer[4];
    buffer[0] = positions[0];
    buffer[1] = positions[1];
    buffer[2] = positions[2];
    buffer[3] = positions[3];
    pFile = fopen("axisPositions.txt", "wb");
    fprintf(pFile, "%.2f %.2f %.2f %.2f", buffer[0], buffer[1], buffer[2], buffer[3]);
    fclose(pFile);

#ifdef MRRobot_HAS_IRE
    ///// IRE Stuff //////
    if (IRE) {
        CMN_LOG_CLASS_INIT_VERBOSE << "Ending IRE..." << std::endl;
        IRE->Kill();
    }
    double PeriodinMS = Period*1e-6;
    CMN_LOG_CLASS_INIT_VERBOSE << "Wait for IRE cleanup" << std::endl;
    long nloops = 0;
    if (IRE) {
        while (!IRE->IsTerminated()) {
            osaSleep(PeriodinMS*cmn_ms);
            if (nloops > 20) {
                CMN_LOG_CLASS_INIT_WARNING << "IRE Forced cleanup!" << std::endl;
                ireFramework::FinalizeShell();
                break; // force a cleanup, might be bad!
            }
            nloops++;
        }
    }
#endif
}

// Configures must be run in a bottom-up order.  eg a trajectory task
// needs to know something about what it is trying to control.
//
// A better example is that a servo task needs to know the hardaware
// before it can choose some pid values, maybe it could choose pid
// values intelligently.
void ctfMainUIThread::Configure(const std::string & filename) {
    cmnXMLPath xmlConfig;
    if (filename == "") {
        CMN_LOG_CLASS_INIT_WARNING << "Configure: could not configure MainUI task, configuration file missing!"
                                   <<std::endl;
        return;
    }

    xmlConfig.SetInputSource(filename);
    char path[40];
    std::string context("/Config/Task[@Name=\"");
    context = context + GetName() + "\"]";
    CMN_LOG_CLASS_INIT_VERBOSE << "Configuring ctfMainUIThread" << std::endl;
    unsigned int axis;
    for (axis = 0; axis < MaxInput.size(); axis++) {
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
    CMN_LOG_CLASS_INIT_VERBOSE << "Configuration of ctfMainUIThread almost done." << std::endl;
    // since we have  only CTF_NO_CUI axis for now
    for (axis = 0 ; axis < CTF_NO_CUI; axis++) {
        cui[axis].PositionInput->bounds(MinInput[axis], MaxInput[axis]);
        cui[axis].DGainSlider->bounds(MinDGain[axis], MaxDGain[axis]);
        cui[axis].PGainSlider->bounds(MinPGain[axis], MaxPGain[axis]);
        cui[axis].IGainSlider->bounds(MinIGain[axis], MaxIGain[axis]);
        cui[axis].VoltInput->bounds(MinDirectVolt[axis], MaxDirectVolt[axis]);
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Configuration of ctfMainUIThread done." << std::endl;
}
