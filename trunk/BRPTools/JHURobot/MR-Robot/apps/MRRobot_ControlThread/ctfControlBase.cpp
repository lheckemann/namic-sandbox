#include "ctfControlBase.h"
#include "BRPTPRInterface.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>  // cout


// Object initialization -------------------------------------

#define ctfControl_Initialize_array(a,x) { a.reserve(x); a.resize(x); std::fill(a.begin(), a.end(), 0.0); }


/// Constructor
ctfControlBase::ctfControlBase(int nRobotJointNumbers)
{
        this->OpenTracker = NULL;

        this->nTotalJointNr = nRobotJointNumbers;
        this->nTotalValveNr = 2*nRobotJointNumbers;
        ctfControl_Initialize_array(this->ActualJointPositions, this->nTotalJointNr);
        ctfControl_Initialize_array(this->ActualRawEncoderPositions, this->nTotalJointNr);
        ctfControl_Initialize_array(this->Actual2VoltageOutputs, this->nTotalValveNr);
        ctfControl_Initialize_array(this->Actual2RawSensorInputs, this->nTotalValveNr);
        ctfControl_Initialize_array(this->Actual2SensorValues, this->nTotalValveNr);

        /// \todo this should be false!
        this->lRobotCoordinatesInitialized = true;
        this->ActualRobotPosition[0] = this->ActualRobotPosition[1] = this->ActualRobotPosition[2] = 0.0;
        this->ActualRobotOrientation[0] = 1.0;this->ActualRobotOrientation[1] = 0.0;
        this->ActualRobotOrientation[2] = 0.0;this->ActualRobotOrientation[3] = 0.0;
        this->ActualDeptVector[0] = this->ActualDeptVector[1] = this->ActualDeptVector[2] = 0.0;

        /// \todo this should be false!
        this->lZFrameInitialized = true;
        this->ZFramePosition[0] = this->ZFramePosition[1] = this->ZFramePosition[2] = 0.0;
        this->ZFrameOrientation[0] = 1.0;this->ZFrameOrientation[1] = 0.0;
        this->ZFrameOrientation[2] = 0.0;this->ZFrameOrientation[3] = 0.0;

        // nothing yet initialized
        this->mCurrentStatus = BRPTPRstatus_Idle;

        /// \todo this should be false!
        this->lActualJointPositions=true;
        std::fill(ActualJointPositions.begin(), ActualJointPositions.end(), 0.0);
 
        this->IsTargetJointPositionsChanged=false;
        this->IsTargetVoltageOutputsChanged=false;
        this->lTargetRobotCoordinates=false;    
}


/// Destructor
ctfControlBase::~ctfControlBase(void)
{
        this->OpenTracker = NULL;
}


/// OpenTracke object - we can send responses here
void ctfControlBase::SetOpenTracker(BRPtprOpenTracker *ot)
{
        this->OpenTracker = ot;
}


// Incoming commands from the navigation software ----------------------------------


/// Workphase: START_UP - boot the robot & send status
void ctfControlBase::WorkphaseSTART_UP(void)
{
        DEBUG_MESSAGE << "ctfControlBase::WorkphaseSTART_UP" << std::endl;

        // response - Uncalibrated
        this->mCurrentStatus = BRPTPRstatus_Uncalibrated;
        this->OpenTracker->SetRobotStatus(mCurrentStatus);

        this->GetStatus(); 
}


/// Workphase: PLANNING - standby & send status
void ctfControlBase::WorkphasePLANNING(void)
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { DEBUG_MESSAGE << "Emergency stop!\n"; return;}
        DEBUG_MESSAGE << "ctfControlBase::WorkphasePLANNING\n";

        // Uncalibrated if it was Idle, otherwise do nothing - just send status
        if (this->mCurrentStatus < BRPTPRstatus_Uncalibrated) {
                this->mCurrentStatus = BRPTPRstatus_Uncalibrated;
        }
        this->OpenTracker->SetRobotStatus(mCurrentStatus);

        // response
        this->GetStatus(); 
}


/// Workphase: CALIBRATION - calibrate robot & send status
void ctfControlBase::WorkphaseCALIBRATION(void)
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { DEBUG_MESSAGE << "Emergency stop!\n"; return;}
        DEBUG_MESSAGE << "ctfControlBase::WorkphaseCALIBRATION\n";

        // idle->uncalibrated. Will not be ready until ZFrame arrives!
        if (this->mCurrentStatus < BRPTPRstatus_Uncalibrated) {
                this->mCurrentStatus = BRPTPRstatus_Uncalibrated;
        }
        this->OpenTracker->SetRobotStatus(mCurrentStatus);

        // response
        this->GetStatus(); 
}


/// Workphase: TARGETING - standby
void ctfControlBase::WorkphaseTARGETING(void)
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { DEBUG_MESSAGE << "Emergency stop!\n"; return;}
        DEBUG_MESSAGE << "ctfControlBase::WorkphaseTARGETING\n";
//Nathan 2009-06-25
//      assert(this->mCurrentStatus >= BRPTPRstatus_Ready);
        if (this->mCurrentStatus == BRPTPRstatus_Manual)
                this->mCurrentStatus = BRPTPRstatus_Ready;

        // response
        this->GetStatus(); 
}


/// Workphase: MANUAL control of the robot - unlock some brakes
void ctfControlBase::WorkphaseMANUAL(void)
{
        DEBUG_MESSAGE << "ctfControlBase::WorkphaseMANUAL\n";

        assert(mCurrentStatus > BRPTPRstatus_Uncalibrated);
        this->mCurrentStatus = BRPTPRstatus_Manual;
        this->OpenTracker->SetRobotStatus(mCurrentStatus);
        
        this->GetStatus(); 
}


/// Workphase: EMERGENCY - something really bad happened
void ctfControlBase::WorkphaseEMERGENCY(void)
{
        mCurrentStatus = BRPTPRstatus_EStop;
        this->OpenTracker->SetRobotStatus(mCurrentStatus);
        DEBUG_MESSAGE << "ctfControlBase::WorkphaseEMERGENCY\n";
}



/// Command: Go to this global RAS position and set status to Moving
void ctfControlBase::GoToCoordinates(float position[3], float orientation[4])
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { DEBUG_MESSAGE << "Emergency stop!\n"; return;}
        DEBUG_MESSAGE << "ctfControlBase::GoToCoordinates (" << position[0] << "," << position[1] << "," << position[2] << ") \n" ;
        
        FLOAT_COPY3(position,this->TargetRobotPosition);
        FLOAT_COPY4(orientation,this->TargetRobotOrientation);

        mCurrentStatus = BRPTPRstatus_Moving;
        this->OpenTracker->SetRobotStatus(mCurrentStatus);

        lTargetRobotCoordinates = true; // there's a new target to be proces
}


/// Command: set this Z-Frame for translations (store it and flip the flag) - now we are ready to target!
void ctfControlBase::ZFrame(float position[3], float orientation[4])
{
        FLOAT_COPY3(position,this->ZFramePosition);
        FLOAT_COPY4(orientation,this->ZFrameOrientation);
        DEBUG_MESSAGE << "ctfControlBase::ZFrame " << this->ZFramePosition << " x " << this->ZFrameOrientation << "\n";

        this->lZFrameInitialized = true;

        // uncalibrated -> ready
        if (this->mCurrentStatus<BRPTPRstatus_Ready) {
                this->mCurrentStatus = BRPTPRstatus_Ready;
        }
        this->OpenTracker->SetRobotStatus(mCurrentStatus);
}


/// Query: send me the actual (current) robot coordinates
void ctfControlBase::GetActualCoordinates(void)
{
        DEBUG_MESSAGE << "ctfControlBase::GetActualCoordinates\n";
        assert(this->lRobotCoordinatesInitialized);
        assert(this->OpenTracker);
        assert(this->lActualJointPositions);

        /*
        DEBUG_MESSAGE << "ctfControlBase::GetActualCoordinates (" << ActualRobotPosition[0] << "," << ActualRobotPosition[1] << "," << ActualRobotPosition[2] << ")"
                << " (" << ActualRobotOrientation[0] << "," << ActualRobotOrientation[1] << "," << ActualRobotOrientation[2] << "," << ActualRobotOrientation[3] << ")"
                << " (" << ActualDeptVector[0] << "," << ActualDeptVector[1] << "," << ActualDeptVector[2] << ")\n";
                
        */
        this->OpenTracker->QueueActualCoordinates(ActualRobotPosition, ActualRobotOrientation, ActualDeptVector);
}


/// Command: go to this local (robot) position - nTotalJointNr long array
void ctfControlBase::SetJointPositions(float pos[])
{
//      printf("This function is SetJointPositions\n");
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { DEBUG_MESSAGE << "Emergency stop!\n"; return;}
        DEBUG_MESSAGE << "ctfControlBase::SetJointPositions " << pos[0] << ", " << pos[1] << ", " << pos[2] << ", " << pos[3] << "\n";

        // copy pos to TargetJointPositions
        TargetJointPositions.assign(pos,pos+nTotalJointNr);
        this->IsTargetJointPositionsChanged=true; // flip the flag: we received a new target!
}


/// Query: send me the number of robot joints
void ctfControlBase::GetNumberOfJoints(void)
{
        DEBUG_MESSAGE << "ctfControlBase::GetNumberOfJoints\n";
        assert(this->OpenTracker);
        assert(false); //this->OpenTracker->QueueNumberOfJoints( this->nTotalJointNr );
}


/// Query: send me the actual joint positions - nTotalJointNr long array
void ctfControlBase::GetActualJointPositions(void)
{
        assert(this->lActualJointPositions);
        assert(this->OpenTracker);
        assert(false); //this->OpenTracker->QueueActualJointPositions(this->nTotalJointNr, &ActualJointPositions[0]);
}


/// Command: engage all the brakes
void ctfControlBase::LockRobot(void)
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { DEBUG_MESSAGE << "Emergency stop!\n"; return;}
        DEBUG_MESSAGE << "ctfControlBase::LockRobot not implemented! \n";
}


/// Command: loose the brakes
void ctfControlBase::UnlockRobot(void)
{
        DEBUG_MESSAGE << "ctfControlBase::UnlockRobot not implemented! \n";
}


/// Query: what is the status?
void ctfControlBase::GetStatus(void)
{
        DEBUG_MESSAGE << "ctfControlBase::GetStatus\n";
        assert(this->OpenTracker);
        this->OpenTracker->QueueActualRobotStatus(this->mCurrentStatus,"Message not yet implemented");
}



 // Internal functions to set/get the robot status -------------------------------

/// stores "p" and "o" into ActualRobotPositions/ActualRobotOrientation/ActualDeptVector (global coord system)
void ctfControlBase::SaveActualRobotPosition(const float p[3], const float o[4], const float d[3])
{
        FLOAT_COPY3(p, this->ActualRobotPosition);
        FLOAT_COPY4(o, this->ActualRobotOrientation);
        FLOAT_COPY3(d, this->ActualDeptVector);
        lRobotCoordinatesInitialized = true; // don't send it to the Nav Soft before initialized!
}


/// stores "pos" into ActualJointPositions variable (local coord system)
void ctfControlBase::SaveActualJointPositions(const float pos[])
{
//DEBUG_MESSAGE << "Received Joint pos: " << pos[0] << "   " <<pos[1] << "   " << pos[2] << "\n";
        ActualJointPositions.assign(pos, pos+nTotalJointNr);
//temp
//DEBUG_MESSAGE << "Received Joint pos: " << ActualJointPositions[0] << "   " <<ActualJointPositions[1] << "   " << ActualJointPositions[2] << "\n";

        lActualJointPositions=true; // don't give to the Nav Software before it is initialized!
        // it can be logged too...
}

void ctfControlBase::SaveActualJointPositions(const double * pos)
{
        ActualJointPositions.assign(pos, pos+nTotalJointNr);
        lActualJointPositions=true; // don't give to the Nav Software before it is initialized!
}




/// stores "pos" into ActualRawEncoderPositions variable (local coord system)
void ctfControlBase::SaveActualRawEncoderPositions(const float pos[])
{
        ActualRawEncoderPositions.assign(pos, pos+nTotalJointNr);
}

void ctfControlBase::SaveActualRawEncoderPositions(const long int * pos)
{
        ActualRawEncoderPositions.assign(pos, pos+nTotalJointNr);
}

/// 2x: stores "valve" into Actual2VoltageOutputs variable (local coord system)
void ctfControlBase::SaveActual2VoltageOutputs(const float valve[])
{
        Actual2VoltageOutputs.assign(valve, valve+nTotalValveNr); // nTotalValveNr, not nTotalJointNr!
}

void ctfControlBase::SaveActual2VoltageOutputs(const double * valve)
{
        Actual2VoltageOutputs.assign(valve, valve+nTotalValveNr); // nTotalValveNr, not nTotalJointNr!
}


/// 2x: stores "valve" into Actual2RawSensorInputs variable (local coord system)
void ctfControlBase::SaveActual2RawSensorInputs(const float valve[])
{
        Actual2RawSensorInputs.assign(valve, valve+nTotalValveNr); // nTotalValveNr, not nTotalJointNr!
}

void ctfControlBase::SaveActual2RawSensorInputs(const double * valve)
{
        Actual2RawSensorInputs.assign(valve, valve+nTotalValveNr); // nTotalValveNr, not nTotalJointNr!
}


/// 2x: stores "valve" into Actual2SensorValues variable (local coord system)
void ctfControlBase::SaveActual2SensorValues(const float valve[])
{
        Actual2SensorValues.assign(valve, valve+nTotalValveNr); // nTotalValveNr, not nTotalJointNr!
}

void ctfControlBase::SaveActual2SensorValues(const double * valve)
{
        Actual2SensorValues.assign(valve, valve+nTotalValveNr); // nTotalValveNr, not nTotalJointNr!
}
 

/// did TargetJointPositions change since last time?
bool ctfControlBase::IsThereNewTargetJointPositions(void)
{
        return this->IsTargetJointPositionsChanged;
}


/// copies TargetJointPositions into "pos" and flips the flag
void ctfControlBase::LoadTargetJointPositions(float pos[])
{
        assert(this->IsTargetJointPositionsChanged);
        //TargetJointPositions.assign(pos, pos+nTotalJointNr);
        std::copy(TargetJointPositions.begin(), TargetJointPositions.end(), pos);
        this->IsTargetJointPositionsChanged=false;
}

/// copies TargetJointPositions into "pos" and flips the flag
void ctfControlBase::LoadTargetJointPositions(double * pos)
{
        assert(this->IsTargetJointPositionsChanged);
        //TargetJointPositions.assign(pos, pos+nTotalJointNr);
        std::copy(TargetJointPositions.begin(), TargetJointPositions.end(), pos);
        this->IsTargetJointPositionsChanged=false;
}

/* /// copies TargetJointPositions into "pos" and flips the flag
void ctfControlBase::LoadTargetJointPositions(double_it start, double_it end)
{
        assert(this->IsTargetJointPositionsChanged);

        for (int i=0;i<nTotalJointNr;i++) {
                assert(start!=end);
                *start=TargetJointPositions[i];
                start++;
        }

        this->IsTargetJointPositionsChanged=false;
}*/


/// did TargetVoltageOutputs change since last time?
bool ctfControlBase::IsThereNewTargetVoltageOutputs(void)
{
        return this->IsTargetVoltageOutputsChanged;
}


/// copies TargetVoltageOutputs into "pos" and flips the flag
void ctfControlBase::LoadTargetVoltageOutputs(float pos[])
{
        assert(this->IsTargetVoltageOutputsChanged);
        std::copy(TargetVoltageOutputs.begin(), TargetVoltageOutputs.end(), pos);
        this->IsTargetVoltageOutputsChanged=false;
}


/// did TargetRobotCoordinates change since last time?
bool ctfControlBase::IsThereNewTargetRobotCoordinates(void)
{
        return this->lTargetRobotCoordinates;
}

/// copies TargetRobotPosition/TargetRobotOrientation into "p/o" and flips the flag
void ctfControlBase::LoadTargetRobotCoordinates(float p[], float o[])
{
        FLOAT_COPY3(this->TargetRobotPosition, p);
        FLOAT_COPY4(this->TargetRobotOrientation, o);
        lTargetRobotCoordinates = false; // target processed
}

void ctfControlBase::InitializeRobot(void)
{
        printf("InitializeRobot not yet implemented!\n");
        assert(false);
}

void ctfControlBase::Home(void)
{
        printf("Home not yet implemented!\n");
        assert(false);
}


