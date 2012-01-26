#include "ctfControl.h"
//#include "BRPTPRInterface.h"
#include <assert.h>
//#include <stdlib.h>
//#include <algorithm>


//GSF
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctTypes.h>


//CMN_IMPLEMENT_SERVICES_TEMPLATED(ddiVecGain16);
//CMN_IMPLEMENT_SERVICES(ctfControl);


// Object initialization -------------------------------------

#define ctfControl_Initialize_array(a,x) { a.reserve(x); a.resize(x); std::fill(a.begin(), a.end(), 0.0); }


/// Constructor
ctfControl::ctfControl(int nRobotJointNumbers): ctfControlBase(nRobotJointNumbers)
{
//TEMP: For Testing
        //float position[3] = {25,125,0};
        //float orientation[4] = {0,0,0,1};
        //GoToCoordinates(position, orientation);

        //GetActualCoordinates();

}


/// Destructor
ctfControl::~ctfControl(void)
{
}


// Incoming commands from the navigation software ----------------------------------


/// standby (boot the robot)
void ctfControl::WorkphaseSTART_UP(void)
{
        CMN_LOG(3) << "ctfControl::WorkphaseSTART_UP: booting the robot, wait..." << std::endl;

        // change status & send response
        ctfControlBase::WorkphaseSTART_UP();
}


/// standby
void ctfControl::WorkphasePLANNING(void)
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { CMN_LOG(3) << "Emergency stop!\n"; return; }
        CMN_LOG(3) << "ctfControl::WorkphasePLANNING\n";

        // change status & send response
        ctfControlBase::WorkphasePLANNING();
}


/// Calibrate the robot!
void ctfControl::WorkphaseCALIBRATION(void)
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { CMN_LOG(3) << "Emergency stop!\n"; return; }
        CMN_LOG(3) << "ctfControl::WorkphaseCALIBRATION\n";

        /// \todo add calibration here

        // change status to "ready" & send response
        ctfControlBase::WorkphasePLANNING();
}


/// standby
void ctfControl::WorkphaseTARGETING(void)
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { CMN_LOG(3) << "Emergency stop!\n"; return; }
        CMN_LOG(3) << "ctfControl::WorkphaseTARGETING\n";

        // send response
        ctfControlBase::WorkphaseTARGETING();
}


/// standby
void ctfControl::WorkphaseMANUAL(void)
{
        CMN_LOG(3) << "ctfControl::WorkphaseTARGETING\n";

        // change status & send response
        ctfControlBase::WorkphaseMANUAL();
}


/// something really bad happened - flush the air!
void ctfControl::WorkphaseEMERGENCY(void)
{
        this->mCurrentStatus = BRPTPRstatus_EStop;
        CMN_LOG(3) << "ctfControl::WorkphaseEMERGENCY        -------- EMERGENCY STOP!\n";

        /// \todo flush the air!

        // change status & send response
        ctfControlBase::WorkphaseMANUAL();
}


/// Command: Go to this global RAS position
void ctfControl::GoToCoordinates(float position[3], float orientation[4])
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { CMN_LOG(3) << "Emergency stop!\n"; return; }
        CMN_LOG(3) << "ctfControl::GoToCoordinates (" << position[0] << "," << position[1] << "," << position[2] << ") \n" ;

        // Store the desired coordinates
        //FLOAT_COPY3(position,this->TargetRobotPosition);
        //FLOAT_COPY4(orientation,this->TargetRobotOrientation);

        // New Command Joint Positions (in mm) Based on GoToCoordinates Command
        float JntPos[8];

        // Robot configuration to solve for: From robot origin -> robot tip
        vct3 P_Rob;
        
        // Command Input Robot Tip position in RAS coords
        vct3 P_RAS;
        P_RAS.Element(0) = position[0];
        P_RAS.Element(1) = position[1];
        P_RAS.Element(2) = position[2];
        CMN_LOG(3) <<  "ctfControl::P_RAS     - " << P_RAS << "\n";     


        // Command Input Robot Tip orientation in RAS coords
        vctQuatRot3 Q_RAS;
        Q_RAS.X() = orientation[0];
        Q_RAS.Y() = orientation[1];
        Q_RAS.Z() = orientation[2];
        Q_RAS.R() = orientation[3];
        CMN_LOG(3) <<  "ctfControl::Q_RAS     - " << Q_RAS << "\n";     

        // Offset from Z-frame origin to robot origin in Z-frame coords
        // MOVE TO HEADER OR XML FILE 
        vct3 P_Offset;
        P_Offset.Element(0) = P_Offset_X;
        P_Offset.Element(1) = P_Offset_Y;       // z-frame origin at center of z-frame
        P_Offset.Element(2) = P_Offset_Z;       // front edge defined as front surface of needle guide post
        CMN_LOG(3) <<  "ctfControl::P_Offset  - " << P_Offset << "\n";


        // Received Z-Frame position in RAS coords.     
        vct3 P_Z;
        P_Z.Element(0) = ZFramePosition[0];
        P_Z.Element(1) = ZFramePosition[1];
        P_Z.Element(2) = ZFramePosition[2];
//      P_Z.Element(0) = 0;
//      P_Z.Element(1) = 0;
//      P_Z.Element(2) = 0;
        CMN_LOG(3) <<  "ctfControl::P_Z       - " << P_Z << "\n";       


        // Received Z-Frame Rotation as quaternion
        vctQuatRot3 Q_Z;
        //Q_Z.X() = ZFrameOrientation[0];
        //Q_Z.Y() = ZFrameOrientation[1];
        //Q_Z.Z() = ZFrameOrientation[2];
        //Q_Z.R() = ZFrameOrientation[3];
        Q_Z.X() = 0;
        Q_Z.Y() = 0;
        Q_Z.Z() = 0;
        Q_Z.R() = 1;
        CMN_LOG(3) <<  "ctfControl::Q_Z       - " <<Q_Z << "\n";


        // Z-Frame Rotation as 3x3 rotation matrix
        vctMatRot3 R_Z(Q_Z);
        CMN_LOG(3) <<  "ctfControl::R_Z       - " << R_Z << "\n";

        vctDouble3x3 R_Z_Inv;
        R_Z_Inv = R_Z.Inverse();
        CMN_LOG(3) <<  "ctfControl::R_Z_Inv   - " << R_Z_Inv << "\n";


        ///////////////////////////
        // Robot Kinematics - move to seprate file later
        ///////////////////////////


/*
        // To Send joint positions directly
        JntPos[0] = 0;                          // n/a
        JntPos[1] = position[1];        // vertical (y)
        JntPos[2] = position[0];        // horizontal (x)
        JntPos[3] = position[1];    // needle insertion (z)
        JntPos[4] = 0;
        JntPos[5] = 0;
        JntPos[6] = 0;
        JntPos[7] = 0;
*/


        // To Send coordinate in RAS coords (Incorporate Z-frame coords & offsets)

        P_Rob = R_Z_Inv*(P_RAS - P_Z) - P_Offset;

/*
        //DON'T NEED THIS
        // Calculated/planned needle tip offset from start position
        vct3 Needle_Offset;
        Needle_Offset.Element(0) = 0;                                   // (x)
        Needle_Offset.Element(1) = 0;                                   // (y)
        Needle_Offset.Element(2) = P_Rob.Element(2);    // (z, + along needle axis I->S)

        CMN_LOG(3) <<  "ctfControl::Needle_Offset - " << Needle_Offset << "\n";

        // Apply needle offset (add calculated needle depth to z-offset & recalculate x,y robot command positions)
        // Updated offset from Z-frame origin to robot origin in Z-frame coords
        
        // Recalculated with needle depth applied as offset in z-direction
        P_Rob = P_Rob + (R_Z_Inv*R_RAS)*Needle_Offset;
*/


        // Define corresponding joint motions
        JntPos[0] = 0;                  // n/a
        JntPos[1] = P_Rob.Element(1);   // vertical (y)
        JntPos[2] = P_Rob.Element(0);   // horizontal (x)
        JntPos[3] = P_Rob.Element(2);   // insertion depth from front of needle guide (z)
        JntPos[4] = 0;
        JntPos[5] = 0;
        JntPos[6] = 0;
        JntPos[7] = 0;
        
        CMN_LOG(3) <<  "Calculated Robot Command Position (F_Rorig->F_Rtip): " << "\n";
        CMN_LOG(3) <<  "ctfControl::P_Rob     - " << P_Rob << "\n";
        CMN_LOG(3) <<  "ctfControl::CmdJoints - " << "Ax0: " << JntPos[0] << " , " << "Ax1: " << JntPos[1] << " , " << "Ax2: " << JntPos[2] << " , " << "Ax3: " << JntPos[3] << "\n";

        // Set Robot Joint Positions
        this->SetJointPositions(JntPos);
        
        // store coord, change status & send response
        ctfControlBase::GoToCoordinates(position, orientation);
}


/// Command: set this Z-Frame for translations (store it and flip the flag)
/*
void ctfControl::ZFrame(float position[3], float orientation[4])
{
        CMN_LOG(3) << "ctfControl::ZFrame\n";

        // store, change status & send response
        ctfControlBase::ZFrame(position, float orientation);
}
*/




/// Query: send me the actual robot coordinates
void ctfControl::GetActualCoordinates(void)
{
        
        assert(this->lRobotCoordinatesInitialized);
        assert(this->OpenTracker);
        assert(this->lActualJointPositions);

        
        ///////////////////////////
        // Robot Kinematics - move to separate file later
        ///////////////////////////


        // Actual Robot Tip position in RAS coords to solve for
        vct3 P_RAS;

        // Robot tip wrt robot origin
        vct3 P_Rob;
        P_Rob.Element(0) = ActualJointPositions[2];     // x, horizontal
        P_Rob.Element(1) = ActualJointPositions[1];     // y, vertical
        P_Rob.Element(2) = 0.0;                                                 // z, axial (insertion separate)
        //CMN_LOG(3) <<  "Actual Current Robot Joint Positions: " << "\n";
        //CMN_LOG(3) <<  "ctfControl::ActJointPos - " << "Ax0: " << ActualJointPositions[0] << " , " << "Ax1: " << ActualJointPositions[1] << " , " << "Ax2: " << ActualJointPositions[2] << " , " << "Ax3: " << ActualJointPositions[3] << "\n";
        //CMN_LOG(3) <<  "ctfControl::P_Rob       - " << P_Rob << "\n"; 

        // Offset from Z-frame origin to robot origin in Z-frame coords (assuming Z-frame is orientated on robot so that it's orientation is close to Identity in standard pose)
        // MOVE TO HEADER OR XML FILE 
        vct3 P_Offset;
        P_Offset.Element(0) = P_Offset_X;       // (x)
        P_Offset.Element(1) = P_Offset_Y;       // (y)
        P_Offset.Element(2) = P_Offset_Z;       // (z)
        //CMN_LOG(3) <<  "ctfControl::P_Offset  - " << P_Offset << "\n";        

        // Fixed needle tip offsets from Roboit origin at front surface of needle guide
        // MOVE TO HEADER OR XML FILE 
        vct3 Needle_Offset;
        Needle_Offset.Element(0) = Needle_Offset_X;             // (x)
        Needle_Offset.Element(1) = Needle_Offset_Y;             // (y)
        Needle_Offset.Element(2) = Needle_Offset_Z;             // (z, + along needle axis I->S)
        //CMN_LOG(3) <<  "ctfControl::P_Offset  - " << P_Offset << "\n";

        // Received Z-Frame position in RAS coords.     
        vct3 P_Z;
        P_Z.Element(0) = ZFramePosition[0];             // (x in RAS)
        P_Z.Element(1) = ZFramePosition[1];             // (y in RAS)
        P_Z.Element(2) = ZFramePosition[2];             // (z in RAS)
//      P_Z.Element(0) = 0;
//      P_Z.Element(1) = 0;
//      P_Z.Element(2) = 0;
        //CMN_LOG(3) <<  "ctfControl::P_Z       - " << P_Z << "\n";     

        // Received Z-Frame Rotation as quaternion
        // w.r.t. RAS orientation. Should be clode to R=I -> q=[0,0,0,1] if z-frame attached to robot in standard pose in scanner
        vctQuatRot3 Q_Z;
//      Q_Z.X() = ZFrameOrientation[0];
//      Q_Z.Y() = ZFrameOrientation[1];
//      Q_Z.Z() = ZFrameOrientation[2];
//      Q_Z.R() = ZFrameOrientation[3];
        Q_Z.X() = 0;
        Q_Z.Y() = 0;
        Q_Z.Z() = 0;
        Q_Z.R() = 1;    
        //CMN_LOG(3) <<  "ctfControl::Q_Z       - " <<Q_Z << "\n";      


        // Z-Frame Rotation as 3x3 rotation matrix
        // Should be close to I if robot in standard pose
        vctMatRot3 R_Z(Q_Z);
        //CMN_LOG(3) <<  "ctfControl::R_Z       - " << R_Z << "\n";

        // Take inverse of R_Z
        vctDouble3x3 R_Z_Inv;
        R_Z_Inv = R_Z.Inverse();
        //CMN_LOG(3) <<  "ctfControl::R_Z_Inv   - " << R_Z_Inv << "\n";


        // Calculate Robot tip position in the patient/scanner RAS coordinate system
        // To Send coordinate in RAS coords (Incorporate Z-frame coords & offsets)
        P_RAS = R_Z*P_Rob + R_Z*P_Offset + P_Z; 

        // Robot Tip orientation in RAS coords
        // Q=[0,0,1,0] & [0,0,0,1] generates needle trajectory from I->S in slicer (Z-axis of R_RAS is needle axis)
        // Should be same as Q_Z as long as robot motion aligned with attached Z-frame axes
        vctQuatRot3 Q_RAS;
        Q_RAS.X() = Q_Z.X();
        Q_RAS.Y() = Q_Z.Y();
        Q_RAS.Z() = Q_Z.Z();
        Q_RAS.R() = Q_Z.R();

        // Calculated robot tip location in RAS coords
        ActualRobotPosition[0] = P_RAS.Element(0);              // x, horizontal
        ActualRobotPosition[1] = P_RAS.Element(1);      // y, vertical
        ActualRobotPosition[2] = P_RAS.Element(2);              // z, axial (to front edge of needle guide)
        //CMN_LOG(3) <<  "Calculated Actual Robot Position (F_RAS->F_tip): " << "\n";
        //CMN_LOG(3) <<  "ctfControl::P_RAS     - " << P_RAS << "\n";

        ActualRobotOrientation[0] = Q_RAS.X();
        ActualRobotOrientation[1] = Q_RAS.Y();
        ActualRobotOrientation[2] = Q_RAS.Z();
        ActualRobotOrientation[3] = Q_RAS.R();
        //CMN_LOG(3) <<  "ctfControl::Q_RAS     - " << Q_RAS << "\n";

        // Needle tip offset from robot tip in robot end effector coordinates
        // Needle axis defined as Rz column (z-axis) of R_RAS
        ActualDeptVector[0] = Needle_Offset.Element(0);         // No motion in this direction (x), offset currently defined as 0
        ActualDeptVector[1] = Needle_Offset.Element(1);         // No motion in this direction (y), offset currently defined as 0
        ActualDeptVector[2] = Needle_Offset.Element(2) + ActualJointPositions[3];       // z axis - needle insertion depth from front surface of robot end effector
        //CMN_LOG(3) <<  "ctfControl::Depth     - " << ActualDeptVector[0] << "," << ActualDeptVector[1] << "," << ActualDeptVector[2] << "\n";


        /*
        CMN_LOG(3) << "ctfControl::GetActualCoordinates (" << ActualRobotPosition[0] << "," << ActualRobotPosition[1] << "," << ActualRobotPosition[2] << ")"
                << " (" << ActualRobotOrientation[0] << "," << ActualRobotOrientation[1] << "," << ActualRobotOrientation[2] << "," << ActualRobotOrientation[3] << ")"
                << " (" << ActualDeptVector[0] << "," << ActualDeptVector[1] << "," << ActualDeptVector[2] << ")\n";
                
        */

        // Send ActualRobotPosition, ActualRobotOrientation, ActualDeptVector to Navigation Software
        ctfControlBase::GetActualCoordinates();
}


/// Command: go to this local (robot) position - nTotalJointNr long array
/*
void ctfControl::SetJointPositions(float pos[])
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { CMN_LOG(3) << "Emergency stop!\n"; return; }
        CMN_LOG(3) << "ctfControl::SetJointPositions " << pos[0] << "," << pos[1] << "," << pos[2] << "\n";
        // copy pos to TargetJointPositions
        TargetJointPositions.assign(pos,pos+nTotalJointNr);
        this->IsTargetJointPositionsChanged=true; // flip the flag: we received a new target!
}*/


/// Query: send me the number of robot joints
/*
void ctfControl::GetNumberOfJoints(void)
{
        CMN_LOG(3) << "ctfControl::GetNumberOfJoints\n";
        assert(this->OpenTracker);
        assert(false); //this->OpenTracker->QueueNumberOfJoints( this->nTotalJointNr );
}*/


/// Query: send me the actual joint positions - nTotalJointNr long array
/*void ctfControl::GetActualJointPositions(void)
{
        assert(this->lActualJointPositions);
        assert(this->OpenTracker);
        assert(false); //this->OpenTracker->QueueActualJointPositions(this->nTotalJointNr, &ActualJointPositions[0]);
}*/


/// Command: engage all the brakes
void ctfControl::LockRobot(void)
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { CMN_LOG(3) << "Emergency stop!\n"; return; }
        CMN_LOG(3) << "ctfControl::LockRobot not implemented! \n";
}


/// Command: loose the brakes
void ctfControl::UnlockRobot(void)
{
        CMN_LOG(3) << "ctfControl::UnlockRobot not implemented! \n";
}


/// Command: home
void ctfControl::Home(void)
{
        CMN_LOG(3) << "ctfControl::Home not implemented! \n";
}


/// Command: Initialize
void ctfControl::InitializeRobot(void)
{
        CMN_LOG(3) << "ctfControl::InitializeRobot not implemented! \n";
}

/// Query: what is the status?
/*void ctfControl::GetStatus(void)
{
        CMN_LOG(3) << "ctfControl::GetStatus\n";
        assert(this->OpenTracker);
        this->OpenTracker->QueueActualRobotStatus(this->mCurrentStatus,"Message not yet implemented");
}*/



 // Internal functions to set/get the robot status -------------------------------

/// stores "p" and "o" into ActualRobotPositions/ActualRobotOrientation/ActualDeptVector (global coord system)
/*void ctfControl::SaveActualRobotPosition(const float p[3], const float o[4], const float d[3])
{
        FLOAT_COPY3(p, this->ActualRobotPosition);
        FLOAT_COPY4(o, this->ActualRobotOrientation);
        FLOAT_COPY3(d, this->ActualDeptVector);
        lRobotCoordinatesInitialized = true; // don't send it to the Nav Soft before initialized!
}*/


/// stores "pos" into ActualJointPositions variable (local coord system)
/*void ctfControl::SaveActualJointPositions(const float pos[])
{
//CMN_LOG(3) << "Received Joint pos: " << pos[0] << "   " <<pos[1] << "   " << pos[2] << "\n";
        ActualJointPositions.assign(pos, pos+nTotalJointNr);
//temp
//CMN_LOG(3) << "Received Joint pos: " << ActualJointPositions[0] << "   " <<ActualJointPositions[1] << "   " << ActualJointPositions[2] << "\n";

        lActualJointPositions=true; // don't give to the Nav Software before it is initialized!
        // it can be logged too...
}*/


/// stores "pos" into ActualRawEncoderPositions variable (local coord system)
/*void ctfControl::SaveActualRawEncoderPositions(const float pos[])
{
        ActualRawEncoderPositions.assign(pos, pos+nTotalJointNr);
}*/


/// 2x: stores "valve" into Actual2VoltageOutputs variable (local coord system)
/*void ctfControl::SaveActual2VoltageOutputs(const float valve[])
{
        Actual2VoltageOutputs.assign(valve, valve+nTotalValveNr); // nTotalValveNr, not nTotalJointNr!
}*/


/// 2x: stores "valve" into Actual2RawSensorInputs variable (local coord system)
/*void ctfControl::SaveActual2RawSensorInputs(const float valve[])
{
        Actual2RawSensorInputs.assign(valve, valve+nTotalValveNr); // nTotalValveNr, not nTotalJointNr!
}*/


/// 2x: stores "valve" into Actual2SensorValues variable (local coord system)
/*void ctfControl::SaveActual2SensorValues(const float valve[])
{
        Actual2SensorValues.assign(valve, valve+nTotalValveNr); // nTotalValveNr, not nTotalJointNr!
}*/
 

/// did TargetJointPositions change since last time?
/*bool ctfControl::IsThereNewTargetJointPositions(void)
{
        return this->IsTargetJointPositionsChanged;
}*/


/// copies TargetJointPositions into "pos" and flips the flag
/*void ctfControl::LoadTargetJointPositions(float pos[])
{
        assert(this->IsTargetJointPositionsChanged);
        //TargetJointPositions.assign(pos, pos+nTotalJointNr);
        std::copy(TargetJointPositions.begin(), TargetJointPositions.end(), pos);
}*/

/* /// copies TargetJointPositions into "pos" and flips the flag
void ctfControl::LoadTargetJointPositions(double_it start, double_it end)
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
/*bool ctfControl::IsThereNewTargetVoltageOutputs(void)
{
        return this->IsTargetVoltageOutputsChanged;
}*/


/// copies TargetVoltageOutputs into "pos" and flips the flag
/*void ctfControl::LoadTargetVoltageOutputs(float pos[])
{
        std::copy(TargetVoltageOutputs.begin(), TargetVoltageOutputs.end(), pos);
}*/


/// did TargetRobotCoordinates change since last time?
/*bool ctfControl::IsThereNewTargetRobotCoordinates(void)
{
        return this->lTargetRobotCoordinates;
}*/

/// copies TargetRobotPosition/TargetRobotOrientation into "p/o" and flips the flag
/*void ctfControl::LoadTargetRobotCoordinates(float p[], float o[])
{
        FLOAT_COPY3(this->TargetRobotPosition, p);
        FLOAT_COPY4(this->TargetRobotOrientation, o);
        lTargetRobotCoordinates = false; // target processed
}*/

