#include "ctfControl.h"
//#include "BRPTPRInterface.h"
#include <assert.h>
//#include <stdlib.h>
//#include <algorithm>
#include <cmath>


//GSF
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctTypes.h>

// Links Length
// 2010-10-04 Link length updated by Nathan
#define L1 119.0        // Link Length for axis 1
#define L2 119.4        // Link Length for axis 2
#define L3 120.0        // Link Length for axis 3
#define L4 120.1        // Link Length for axis 4

#define a1 30.8 // see robot workspace
#define a2 30.0 // see robot workspace
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
//    for(int i=0; i < 8; i++){
 //       JntPos[i] = 0.0;
  //  }

}


/// Destructor
ctfControl::~ctfControl(void)
{
}


// Incoming commands from the navigation software ----------------------------------


/// standby (boot the robot)
void ctfControl::WorkphaseSTART_UP(void)
{
        CMN_LOG_RUN_VERBOSE << "ctfControl::WorkphaseSTART_UP: booting the robot, wait..." << std::endl;

        // change status & send response
        ctfControlBase::WorkphaseSTART_UP();
}


/// standby
void ctfControl::WorkphasePLANNING(void)
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { CMN_LOG_RUN_WARNING << "Emergency stop!\n"; return; }
        CMN_LOG_RUN_VERBOSE << "ctfControl::WorkphasePLANNING\n";

        // change status & send response
        ctfControlBase::WorkphasePLANNING();
}


/// Calibrate the robot!
void ctfControl::WorkphaseCALIBRATION(void)
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { CMN_LOG_RUN_WARNING << "Emergency stop!\n"; return; }
        CMN_LOG_RUN_VERBOSE << "ctfControl::WorkphaseCALIBRATION\n";

        /// \todo add calibration here

        // change status to "ready" & send response
        ctfControlBase::WorkphaseCALIBRATION();
}


/// standby
void ctfControl::WorkphaseTARGETING(void)
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { CMN_LOG_RUN_WARNING << "Emergency stop!\n"; return; }
        CMN_LOG_RUN_VERBOSE << "ctfControl::WorkphaseTARGETING\n";

        // send response
        ctfControlBase::WorkphaseTARGETING();
}


/// standby
void ctfControl::WorkphaseMANUAL(void)
{
        CMN_LOG_RUN_VERBOSE << "ctfControl::WorkphaseMANUAL\n";

        // change status & send response
        ctfControlBase::WorkphaseMANUAL();
}


/// something really bad happened - flush the air!
void ctfControl::WorkphaseEMERGENCY(void)
{
        this->mCurrentStatus = BRPTPRstatus_EStop;
        CMN_LOG_RUN_VERBOSE << "ctfControl::WorkphaseEMERGENCY        -------- EMERGENCY STOP!\n";

        /// \todo flush the air!

        // change status & send response
        ctfControlBase::WorkphaseEMERGENCY();
}


/// Command: Go to this global RAS position
void ctfControl::GoToCoordinates(float position[3], float orientation[4])
{
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { CMN_LOG_RUN_WARNING << "Emergency stop!\n"; return; }
        CMN_LOG_RUN_WARNING << "ctfControl::GoToCoordinates (" << position[0]
                            << "," << position[1] << "," << position[2] << ") \n" ;

        // Store the desired coordinates
        //FLOAT_COPY3(position,this->TargetRobotPosition);
        //FLOAT_COPY4(orientation,this->TargetRobotOrientation);

        // New Command Joint Positions (in mm) Based on GoToCoordinates Command
        float JntPos[8];    // Nathan 2011-04-27
   
        // Robot configuration to solve for: From robot origin -> robot tip

        vctMatRot3 R_Rob;
        
        // Command Input Robot Tip position in RAS coords
        vct3 P_RAS;
        P_RAS.Element(0) = position[0];
        P_RAS.Element(1) = position[1];
        P_RAS.Element(2) = position[2];
        CMN_LOG_RUN_DEBUG <<  "ctfControl::P_RAS     - " << P_RAS << "\n";      

        // Command Input needle orientation in RAS coords
        vctQuatRot3 Q_RAS;
        Q_RAS.X() = orientation[0];
        Q_RAS.Y() = orientation[1]; 
        Q_RAS.Z() = orientation[2];
        Q_RAS.R() = orientation[3];
    
    theta = 2 * acos(orientation[3]); // Reza 11/07/2011 : theta is the rotation value 
    
        CMN_LOG_RUN_DEBUG <<  "ctfControl::Q_RAS     - " << Q_RAS << "\n";      
        CMN_LOG_RUN_DEBUG <<  "                Norm  - " << 1.0 - Q_RAS.Norm() << std::endl;
        
    // 2010-06-01 Nathan, 4DOF
        vctMatRot3 R_RAS;
        R_RAS.FromRaw(Q_RAS);
        CMN_LOG_RUN_DEBUG << "ctfControl::R_RAS      - " << R_RAS << "\n";
    
    vctDouble3x3 R_RAS_Inv;
        R_RAS_Inv = R_RAS.Inverse();

        //  robot coordinate origin in Z-frame Coordinate 
        // MOVE TO HEADER OR XML FILE 
        vct3 P_Offset;
        P_Offset.Element(0) = P_Offset_X;
        P_Offset.Element(1) = P_Offset_Y;       // z-frame origin at center of z-frame
        P_Offset.Element(2) = P_Offset_Z;       // front edge defined as front surface of needle guide post
        CMN_LOG_RUN_DEBUG <<  "ctfControl::P_Offset  - " << P_Offset << "\n";

        // Received Z-Frame origin in RAS coords.       
        vct3 P_Z;
        P_Z.Element(0) = ZFramePosition[0];
        P_Z.Element(1) = ZFramePosition[1];
        P_Z.Element(2) = ZFramePosition[2];
//      P_Z.Element(0) = 0;
//      P_Z.Element(1) = 0;
//      P_Z.Element(2) = 0;
        CMN_LOG_RUN_DEBUG <<  "ctfControl::P_Z       - " << P_Z << "\n";        

        // Received Z-Frame orientation as quaternion
        vctQuatRot3 Q_Z;
        Q_Z.X() = ZFrameOrientation[0];
        Q_Z.Y() = ZFrameOrientation[1];
        Q_Z.Z() = ZFrameOrientation[2];
        Q_Z.R() = ZFrameOrientation[3];
        //Q_Z.X() = 0;
        //Q_Z.Y() = 0;
        //Q_Z.Z() = 0;
        //Q_Z.R() = 1;
        CMN_LOG_RUN_DEBUG <<  "ctfControl::Q_Z       - " <<Q_Z << "\n";
        CMN_LOG_RUN_DEBUG <<  "                Norm  - " << 1.0 - Q_Z.Norm() << std::endl;
                
    // 2010-06-01 Nathan, 4DOF
        // Z-Frame Rotation as 3x3 rotation matrix
        vctMatRot3 R_Z;
        R_Z.FromRaw(Q_Z);
        CMN_LOG_RUN_DEBUG <<  "ctfControl::R_Z       - " << R_Z << "\n";

        vctDouble3x3 R_Z_Inv;
        R_Z_Inv = R_Z.Inverse();
        CMN_LOG_RUN_DEBUG <<  "ctfControl::R_Z_Inv   - " << R_Z_Inv << "\n";


/////////////////////////////////////////////////////////////////////////
    vct3 P_Rob;
    P_Rob = R_Z_Inv * (P_RAS - P_Z) - P_Offset; // Target Position in Robot coordinate (Offset applied)
        
    CMN_LOG_RUN_DEBUG <<  "ctfControl::P_Rob   - " << P_Rob << "\n";
    
    // Reza Updated 1/26/2012OB
    vct3 u_RAS;
    vct3 u_Rob;
    
    u_RAS.Element(0) = R_RAS[0][2];
    u_RAS.Element(1) = R_RAS[1][2];  // needle orientation in Robot coordinate
    u_RAS.Element(2) = R_RAS[2][2];
    
    CMN_LOG_RUN_DEBUG <<  "ctfControl::u_RAS   - " << u_RAS << "\n";

    //CMN_LOG_RUN_DEBUG <<  "ctfControl::d   - " << d << "\n";

    //u_RAS.Element(0) = orientation[0] / d;
    //u_RAS.Element(1) = orientation[1] / d;  // needle orientation in Robot coordinate
    //u_RAS.Element(2) = orientation[2] / d;
    


    u_Rob = R_Z_Inv * u_RAS; // Z-frame is assumed to be perfectly aligned with Rob coordinate system
    
    CMN_LOG_RUN_DEBUG <<  "ctfControl::u_Rob   - " << u_Rob << "\n";
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
        // define R_Rob_to_f and Tr_Rob_to_f as the rotation and translation matrices from "robot coordinate" to front triangle coordinate system (due to manufacturing inaccuracies)
        
        // T1=[1.0000   -0.0010    0.0055   0.58
   //      0.0010    1.0000   -0.0009         0
   //      -0.0055    0.0009    1.000138.575 -19.40
   //       0             0         0    1.0000]
    
    // upded by Reza on 29/1/2011
    vctMatRot3  R_Rob_to_f;
    
    R_Rob_to_f[0][0] = 1;
    R_Rob_to_f[0][1] = -0.0010;
    R_Rob_to_f[0][2] = 0.0055;
    R_Rob_to_f[1][0] = 0.0010 ;
    R_Rob_to_f[1][1] = 1;
    R_Rob_to_f[1][2] = -0.0009;
    R_Rob_to_f[2][0] = -0.0055;
    R_Rob_to_f[2][1] = 0.0009;
    R_Rob_to_f[2][2] = 1;

    //R_Rob_to_f[0][0] = 1;
    //R_Rob_to_f[0][1] = 0;
    //R_Rob_to_f[0][2] = 0;
    //R_Rob_to_f[1][0] = 0;
    //R_Rob_to_f[1][1] = 1;
    //R_Rob_to_f[1][2] = 0;
    //R_Rob_to_f[2][0] = 0;
    //R_Rob_to_f[2][1] = 0;
    //R_Rob_to_f[2][2] = 1;
    
    CMN_LOG_RUN_DEBUG <<  "ctfControl::R_Rob_to_f   - " << R_Rob_to_f << "\n";


    vctDouble3x3 R_Rob_to_f_Inv;
    R_Rob_to_f_Inv = R_Rob_to_f.Inverse();
    
    vct3  Tr_Rob_to_f;
    
    Tr_Rob_to_f.Element(0) = 0.58;
    Tr_Rob_to_f.Element(1) = 0;
    Tr_Rob_to_f.Element(2) = -19.40;

    //Tr_Rob_to_f.Element(0) = 0;
    //Tr_Rob_to_f.Element(1) = 0;
    //Tr_Rob_to_f.Element(2) = -18.5;
    
    CMN_LOG_RUN_DEBUG <<   "ctfControl::Tr_Rob_to_f   - " << Tr_Rob_to_f << "\n";
        
// define R_Rob_to_b and Tr_Rob_to_b as the rotation and translation matrices from "robot coordinate" to back triangle coordinate system (due to manufacturing inaccuracies)
        
        // T2=[1.0000   -0.0010    0.0052    2.14
    //     0.0011    1.0000   -0.0009         0
    //     -0.0052    0.0009    1.0000  297.82
    //         0        0         0       1.0000]
    
    vctMatRot3  R_Rob_to_b;
    
    R_Rob_to_b[0][0] = 1;
    R_Rob_to_b[0][1] = -0.0010;
    R_Rob_to_b[0][2] = 0.0052;
    R_Rob_to_b[1][0] = 0.0011 ;
    R_Rob_to_b[1][1] = 1;
    R_Rob_to_b[1][2] = -0.0009;
    R_Rob_to_b[2][0] = -0.0052;
    R_Rob_to_b[2][1] = 0.0009;
    R_Rob_to_b[2][2] = 1;

    //R_Rob_to_b[0][0] = 1;
    //R_Rob_to_b[0][1] = 0;
    //R_Rob_to_b[0][2] = 0;
    //R_Rob_to_b[1][0] = 0;
    //R_Rob_to_b[1][1] = 1;
    //R_Rob_to_b[1][2] = 0;
    //R_Rob_to_b[2][0] = 0;
    //R_Rob_to_b[2][1] = 0;
    //R_Rob_to_b[2][2] = 1;
    
    CMN_LOG_RUN_DEBUG <<  "ctfControl::R_Rob_to_b   - " << R_Rob_to_b << "\n";

    vctDouble3x3 R_Rob_to_b_Inv;
    R_Rob_to_b_Inv = R_Rob_to_b.Inverse();

    
    vct3  Tr_Rob_to_b;
    
    Tr_Rob_to_b.Element(0) = 2.14;
    Tr_Rob_to_b.Element(1) = 0;
    Tr_Rob_to_b.Element(2) = 297.82;

    //Tr_Rob_to_b.Element(0) = 0;
    //Tr_Rob_to_b.Element(1) = 0;
    //Tr_Rob_to_b.Element(2) = 300.5;
    
    CMN_LOG_RUN_DEBUG <<  "ctfControl::Tr_Rob_to_b   - " << Tr_Rob_to_b << "\n";
    
    // upded by Reza on 29/1/2011
    // Define P_f and u_f as correspondants to P_Rob and u_Rob
    
    vct3 P_f;
    P_f = R_Rob_to_f * P_Rob + Tr_Rob_to_f;
   
    CMN_LOG_RUN_DEBUG <<  "ctfControl::P_f   - " << P_f << "\n";

    vct3  u_f;
    u_f = R_Rob_to_f * u_Rob;

    CMN_LOG_RUN_DEBUG <<  "ctfControl::u_f   - " << u_f << "\n";
    
    // Define P_b and u_b as correspondants to P_Rob and u_Rob 
    vct3 P_b;
    P_b = R_Rob_to_b * P_Rob + Tr_Rob_to_b;
    
    CMN_LOG_RUN_DEBUG <<  "ctfControl::P_b   - " << P_b << "\n";

    vct3  u_b;
    u_b = R_Rob_to_b * u_Rob;
    
    CMN_LOG_RUN_DEBUG <<  "ctfControl::u_b   - " << u_b << "\n";
        // needle trajectory intersection with fornt and back triangle
        
    vct3 tri_f;
        
        tri_f.Element(0) = P_f.Element(0)-(u_f.Element(0) / u_f.Element(2)) * P_f.Element(2);   // x for front triangle
        tri_f.Element(1) = P_f.Element(1)-(u_f.Element(1) / u_f.Element(2)) * P_f.Element(2);   // y for front triangle
        tri_f.Element(2) = P_f.Element(2)-(u_f.Element(2) / u_f.Element(2)) * P_f.Element(2); // Z for front triangle
        CMN_LOG_RUN_DEBUG << "tri_f.Element(0)  - " << tri_f.Element(0) << "\n";
        CMN_LOG_RUN_DEBUG << "tri_f.Element(1)  - " << tri_f.Element(1) << "\n";
        CMN_LOG_RUN_DEBUG << "tri_f.Element(2)  - " << tri_f.Element(2) << "\n";

    vct3 tri_b;
    
        tri_b.Element(0) = P_b.Element(0)-(u_b.Element(0) / u_b.Element(2)) * P_b.Element(2);   // x for back triangle
        tri_b.Element(1) = P_b.Element(1)-(u_b.Element(1) / u_b.Element(2)) * P_b.Element(2);   // y for back triangle
        tri_b.Element(2) = P_b.Element(2)-(u_b.Element(2) / u_b.Element(2)) * P_b.Element(2);   // Z for back triangle
        CMN_LOG_RUN_DEBUG << "tri_b.Element(0)  - " << tri_b.Element(0) << "\n";
        CMN_LOG_RUN_DEBUG << "tri_b.Element(1)  - " << tri_b.Element(1) << "\n";
        CMN_LOG_RUN_DEBUG << "tri_b.Element(2)  - " << tri_b.Element(2) << "\n";


        //DON'T NEED THIS
        // Calculated/planned needle tip offset from start position
        vct3 Needle_Offset;
        Needle_Offset.Element(0) = 0;                                   // (x)
        Needle_Offset.Element(1) = 0;                                   // (y)
        Needle_Offset.Element(2) = P_Rob.Element(2);    // (z, + along needle axis I->S)

        //ActualJointPositions[4] =  P_Rob.Element(2);

/*
        CMN_LOG_INIT_VERBOSE <<  "ctfControl::Needle_Offset - " << Needle_Offset << "\n";

        // Apply needle offset (add calculated needle depth to z-offset & recalculate x,y robot command positions)
        // Updated offset from Z-frame origin to robot origin in Z-frame coords
        
        // Recalculated with needle depth applied as offset in z-direction
        P_Rob = P_Rob + (R_Z_Inv*R_RAS)*Needle_Offset;
*/

/*
        // Define corresponding joint motions
        JntPos[0] = 0;                  // n/a
        JntPos[1] = P_Rob.Element(1);   // vertical (y)
        JntPos[2] = P_Rob.Element(0);   // horizontal (x)
        JntPos[3] = P_Rob.Element(2);   // insertion depth from front of needle guide (z)
        JntPos[4] = 0;
        JntPos[5] = 0;
        JntPos[6] = 0;
        JntPos[7] = 0;
*/

// HERE Inverse Kinematics138.575

        // x: P_Rob.Element(0)
        // y: P_Rob.Element(1)
        // L: 124.06
        /*
        JntPos[0] = float(P_Rob.Element(0) + sqrt(pow(LL, 2) - pow((P_Rob.Element(1) - 30), 2)));
        JntPos[1] = float(P_Rob.Element(0) - sqrt(pow(LL, 2) - pow((P_Rob.Element(1) - 30), 2)));
        JntPos[2] = float(P_Rob.Element(0) + sqrt(pow(LL, 2) - pow((P_Rob.Element(1) - 30), 2)));
        JntPos[3] = float(P_Rob.Element(0) - sqrt(pow(LL, 2) - pow((P_Rob.Element(1) - 30), 2)));
        JntPos[4] = float(P_Rob.Element(2));    // insertion depth from front of needle guide (z)
        JntPos[5] = 0;
        JntPos[6] = 0;
        JntPos[7] = 0;
        */
    
    // upded by Reza on 29/1/2011
        JntPos[0] = float(tri_f.Element(0) + sqrt(pow(L1, 2) - pow((tri_f.Element(1) - 30.8), 2)));
        JntPos[1] = float(tri_f.Element(0) - sqrt(pow(L2, 2) - pow((tri_f.Element(1) - 30.8), 2)));
        JntPos[2] = float(tri_b.Element(0) + sqrt(pow(L3, 2) - pow((tri_b.Element(1) - 30), 2)));
        JntPos[3] = float(tri_b.Element(0) - sqrt(pow(L4, 2) - pow((tri_b.Element(1) - 30), 2)));
    //JntPos[4] = float(sqrt(pow(JntPos[0]-P_Rob.Element(0),2) + pow(JntPos[1]-P_Rob.Element(1),2) + pow(z1-P_Rob.Element(2),2))) - Needle_Offset_Z;
        JntPos[4] = float(sqrt(pow(tri_f.Element(0) - P_f.Element(0),2) + pow(tri_f.Element(1) - P_f.Element(1),2) + pow(tri_f.Element(2) - P_f.Element(2),2)) - Needle_Offset_Z);  // 
        JntPos[5] = 0;
        JntPos[6] = 0;
        JntPos[7] = 0;

        NeedleDepth = JntPos[4];
        
        for(int j=0; j<8; j++)
                GJointPos[j] = JntPos[j];

        CMN_LOG_RUN_DEBUG <<  "ctfControl::tri_f.Z   - " << tri_f << "\n";
        CMN_LOG_RUN_DEBUG <<  "Calculated Robot Command Position (F_Rorig->F_Rtip): " << "\n";
        CMN_LOG_RUN_DEBUG <<  "ctfControl::P_Rob     - " << P_Rob << "\n";
        CMN_LOG_RUN_DEBUG <<  "ctfControl::CmdJoints - "
                          << "Ax0: " << JntPos[0] << " , "
                          << "Ax1: " << JntPos[1] << " , "
                          << "Ax2: " << JntPos[2] << " , "
                          << "Ax3: " << JntPos[3] << " , "
                          << "Ax4: " << JntPos[4] << "\n";

        // Set Robot Joint Positions
        this->SetJointPositions(JntPos);
        
        // store coord, change status & send response
        ctfControlBase::GoToCoordinates(position, orientation);
}


/// Command: set this Z-Frame for translations (store it and flip the flag)
/*
void ctfControl::ZFrame(float position[3], float orientation[4])
{
        CMN_LOG_INIT_VERBOSE << "ctfControl::ZFrame\n";

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
        // Robot UPDATED FRORWARD Kinematics after Calibration
        ///////////////////////////


        // Actual Robot Tip position in RAS coords to solve for
        vct3 P_RAS;

        // Robot tip wrt robot origin
        vct3 P_Rob;

// HERE Forward Kinematics:     updated by Reza on 29/01/2011
    // 1- having actual joint positions for each axis, find tri_f and tri_b in their own coordinate systems using updated forward kinematics
    // 2- transform them to Rob coordinate system (tri_f_Rob and tri_b_Rob )
    // 3- find u_Rob (needle orientationunit as a unit vector)
    // 4- find P_Rob by extending the line defined by tri_f_Rob and tri_b_Rob (how much? ans: Needle_Offset.Element(2) + ActualJointPositions[4])
    // 5- transform P_Rob to RAS coordinate (P_RAS)
    // 6- transform u_Rob to RAS (u_RAS)
    // 7- find the quaternion corresponding to the u_RAS found in 6
  
    
    
    // 1- find tri_f and tri_b in their own coordinate systems :
    double zeta1_f = pow(L1, 2);
    double zeta2_f = pow(L2, 2);
    double zeta3_f = ActualJointPositions[0] - ActualJointPositions[1];

    double khi_f = zeta1_f - pow(((pow(zeta3_f, 2) + zeta1_f-zeta2_f) / (2 * zeta3_f)), 2);

    double y1 = a1 + sqrt(khi_f);
    double x1 = (ActualJointPositions[0] + ActualJointPositions[1] + sqrt(zeta2_f - khi_f) - sqrt(zeta1_f - khi_f)) / 2;

    vct3 tri_f;
        
        tri_f.Element(0) = x1;  // x for front triangle in its own coordinate
        tri_f.Element(1) = y1;  // y for front triangle in its own coordinate
    tri_f.Element(2) = 0;       // z for front triangle in its own coordinate
 
    double zeta1_b = pow(L3, 2);
    double zeta2_b = pow(L4, 2);
    double zeta3_b = ActualJointPositions[2] - ActualJointPositions[3];

    double khi_b = zeta1_b - pow(((pow(zeta3_b, 2) + zeta1_b - zeta2_b) / (2 * zeta3_b)), 2);

    double y2 = a2 + sqrt(khi_b);
    double x2 = (ActualJointPositions[2] + ActualJointPositions[3] + sqrt(zeta2_b - khi_b) - sqrt(zeta1_b - khi_b)) / 2;

    vct3 tri_b;
        
        tri_b.Element(0) = x2;  // x for front triangle in its own coordinate
        tri_b.Element(1) = y2;  // y for front triangle in its own coordinate
    tri_b.Element(2) = 0;       // z for front triangle in its own coordinate   
    
    // 2- transform them to Rob coordinate system (tri_f_Rob and tri_b_Rob )
    // front
    vct3 tri_f_Rob;
    vctMatRot3 R_Rob_to_f;    
    R_Rob_to_f[0][0] = 1;
    R_Rob_to_f[0][1] = -0.0010;
    R_Rob_to_f[0][2] = 0.0055;
    R_Rob_to_f[1][0] = 0.0010 ;
    R_Rob_to_f[1][1] = 1;
    R_Rob_to_f[1][2] = -0.0009;
    R_Rob_to_f[2][0] = -0.0055;
    R_Rob_to_f[2][1] = 0.0009;
    R_Rob_to_f[2][2] = 1;   
    
    vctDouble3x3 R_Rob_to_f_Inv;
    R_Rob_to_f_Inv = R_Rob_to_f.Inverse();
    
    vct3  Tr_Rob_to_f;
    Tr_Rob_to_f.Element(0) = 0.58;
    Tr_Rob_to_f.Element(1) = 0;
    Tr_Rob_to_f.Element(2) = -19.40;
    
    tri_f_Rob = R_Rob_to_f_Inv * tri_f - Tr_Rob_to_f;
    
    // back
    vct3 tri_b_Rob;
    vctMatRot3  R_Rob_to_b;  
    R_Rob_to_b[0][0] = 1;
    R_Rob_to_b[0][1] = -0.0010;
    R_Rob_to_b[0][2] = 0.0052;
    R_Rob_to_b[1][0] = 0.0011;
    R_Rob_to_b[1][1] = 1;
    R_Rob_to_b[1][2] = -0.0009;
    R_Rob_to_b[2][0] = -0.0052;
    R_Rob_to_b[2][1] = 0.0009;
    R_Rob_to_b[2][2] = 1;
    
    vctDouble3x3 R_Rob_to_b_Inv;
    R_Rob_to_b_Inv = R_Rob_to_b.Inverse();
    
    vct3  Tr_Rob_to_b;
    Tr_Rob_to_b.Element(0) = 2.14;
    Tr_Rob_to_b.Element(1) = 0;
    Tr_Rob_to_b.Element(2) = 297.82;
    
    tri_b_Rob = R_Rob_to_b_Inv * tri_b - Tr_Rob_to_b;  
    
    
    // 3- find u_Rob (needle orientationunit as a unit vector)
    vct3 u_Rob;
    u_Rob = (tri_f_Rob - tri_b_Rob) / sqrt(pow(tri_f_Rob.Element(0) - tri_b_Rob[0], 2) + pow(tri_f_Rob.Element(1) - tri_b_Rob[1], 2) + pow(tri_f_Rob.Element(2) - tri_b_Rob[2], 2));

//gsf temp
std::cerr << "u_Rob (" << u_Rob[0] << "," << u_Rob[1] << "," << u_Rob[2] << ")\n";
    
    // 4- find P_Rob by extending the line defined by tri_f_Rob and tri_b_Rob (how much? ans: Needle_Offset.Element(2) + ActualJointPositions[4])    
    //vct3 P_Rob;
    //P_Rob = tri_f_Rob + (Needle_Offset_Z + ActualJointPositions[4]) * u_Rob;
 P_Rob = tri_f_Rob + (Needle_Offset_Z + NeedleDepth) * u_Rob;

//gsf temp
std::cerr << "P_Rob (" << P_Rob[0] << "," << P_Rob[1] << "," << P_Rob[2] << ")"
                << " NeedleDepth (" << NeedleDepth  << ")\n";
    
    // 5- transform P_Rob to RAS coordinate (P_RAS)
        // Calculate Robot tip position in the patient/scanner RAS coordinate system
        // To Send coordinate in RAS coords (Incorporate Z-frame coords & offsets)
    vctQuatRot3 Q_Z;
        Q_Z.X() = ZFrameOrientation[0];
        Q_Z.Y() = ZFrameOrientation[1];
        Q_Z.Z() = ZFrameOrientation[2];
        Q_Z.R() = ZFrameOrientation[3];
    
    vctMatRot3 R_Z;
    R_Z.FromRaw(Q_Z);
    
    vct3 P_Offset;
        P_Offset.Element(0) = P_Offset_X;       // (x)
        P_Offset.Element(1) = P_Offset_Y;       // (y)
        P_Offset.Element(2) = P_Offset_Z;       // (z)
    
    vct3 P_Z;
        P_Z.Element(0) = ZFramePosition[0];             // (x in RAS)
        P_Z.Element(1) = ZFramePosition[1];             // (y in RAS)
        P_Z.Element(2) = ZFramePosition[2];             // (z in RAS)
    
        P_RAS = R_Z * (P_Rob + P_Offset) + P_Z; // (P_Rob + P_Offset) = P_Rob_in_Z_Frame

    
    // 6- transform u_Rob to RAS (u_RAS)
    vct3 u_RAS;
    u_RAS = R_Z * u_Rob;  // = S unit vector as well
 
    
    // 7- find the corresponding quaternion value out of u_RAS
   // 7-1 we know that finding the quaternion (i.e. rotation) out of the unit vector has infinite solution.
    //    to make our quaternion specific, we find the one corresponding with the theta we found in inverse kinematics
    //    and we know it does not affect the needle orientation while being as close to the desired quaternion
    
    vct3 u_y;
    u_y.Element(0) = 0.0;
    u_y.Element(1) = 1.0;
    u_y.Element(2) = 0.0;

    vct3 u_x;
    u_x = u_y % u_RAS;
    u_y = u_RAS % u_x;

    vctMatRot3 R_RAS;
    R_RAS[0][0] = u_x.Element(0);
    R_RAS[1][0] = u_x.Element(1);
    R_RAS[2][0] = u_x.Element(2);
    R_RAS[0][1] = u_y.Element(0);
    R_RAS[1][1] = u_y.Element(1);
    R_RAS[2][1] = u_y.Element(2);
    R_RAS[0][2] = u_RAS.Element(0);
    R_RAS[1][2] = u_RAS.Element(1);
    R_RAS[2][2] = u_RAS.Element(2);

    vctQuatRot3 Q_RAS;
    Q_RAS.FromRaw(R_RAS);

    double q[4]; 
    //q[0] = u_RAS.Element(0)* sin(theta/2);
    //q[1] = u_RAS.Element(1)* sin(theta/2);
    //q[2] = u_RAS.Element(2)* sin(theta/2);
    //q[3] = cos(theta/2);

    q[0] = Q_RAS.X();
    q[1] = Q_RAS.Y();
    q[2] = Q_RAS.Z();
    q[3] = Q_RAS.W();

    // Fixed needle tip offsets from Robot origin at front surface of needle guide
        // MOVE TO HEADER OR XML FILE 
        vct3 Needle_Offset;

        // JT edit -- commented out
        //Needle_Offset.Element(0) = Needle_Offset_X;           // (x)
        //Needle_Offset.Element(1) = Needle_Offset_Y;           // (y)
        //Needle_Offset.Element(2) = Needle_Offset_Z;           // (z, + along needle axis I->S)
        //CMN_LOG_INIT_VERBOSE <<  "ctfControl::P_Offset  - " << P_Offset << "\n";

        // Calculated robot tip location in RAS coords
        ActualRobotPosition[0] = P_RAS.Element(0);      // x, horizontal
        ActualRobotPosition[1] = P_RAS.Element(1);      // y, vertical
        ActualRobotPosition[2] = P_RAS.Element(2);      // z, axial (to front edge of needle guide)
        //CMN_LOG_INIT_VERBOSE <<  "Calculated Actual Robot Position (F_RAS->F_tip): " << "\n";
        //CMN_LOG_INIT_VERBOSE <<  "ctfControl::P_RAS     - " << P_RAS << "\n";

        ActualRobotOrientation[0] = q[0];
        ActualRobotOrientation[1] = q[1];
        ActualRobotOrientation[2] = q[2];
        ActualRobotOrientation[3] = q[3];
        //CMN_LOG_INIT_VERBOSE <<  "ctfControl::Q_RAS     - " << Q_RAS << "\n";

        // Needle tip offset from robot tip in robot end effector coordinates
        // Needle axis defined as Rz column (z-axis) of R_RAS
        /*
        ActualDeptVector[0] = Needle_Offset.Element(0);         // No motion in this direction (x), offset currently defined as 0
        ActualDeptVector[1] = Needle_Offset.Element(1);         // No motion in this direction (y), offset currently defined as 0
        ActualDeptVector[2] = Needle_Offset.Element(2) + ActualJointPositions[4];       // z axis - needle insertion depth from front surface of robot end effector
        */
        //CMN_LOG_INIT_VERBOSE <<  "ctfControl::Depth     - " << ActualDeptVector[0] << "," << ActualDeptVector[1] << "," << ActualDeptVector[2] << "\n";


        /*
        CMN_LOG_INIT_VERBOSE << "ctfControl::GetActualCoordinates (" << ActualRobotPosition[0] << "," << ActualRobotPosition[1] << "," << ActualRobotPosition[2] << ")"
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
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { CMN_LOG_INIT_VERBOSE << "Emergency stop!\n"; return; }
        CMN_LOG_INIT_VERBOSE << "ctfControl::SetJointPositions " << pos[0] << "," << pos[1] << "," << pos[2] << "\n";
        // copy pos to TargetJointPositions
        TargetJointPositions.assign(pos,pos+nTotalJointNr);
        this->IsTargetJointPositionsChanged=true; // flip the flag: we received a new target!
}*/


/// Query: send me the number of robot joints
/*
void ctfControl::GetNumberOfJoints(void)
{
        CMN_LOG_INIT_VERBOSE << "ctfControl::GetNumberOfJoints\n";
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
        if (this->mCurrentStatus == BRPTPRstatus_EStop) { CMN_LOG_RUN_WARNING << "Emergency stop!\n"; return; }
        CMN_LOG_RUN_WARNING << "ctfControl::LockRobot not implemented! \n";
}


/// Command: loose the brakes
void ctfControl::UnlockRobot(void)
{
        CMN_LOG_RUN_WARNING << "ctfControl::UnlockRobot not implemented! \n";
}


/// Command: home
void ctfControl::Home(void)
{
        CMN_LOG_RUN_WARNING << "ctfControl::Home not implemented! \n";
}


/// Command: Initialize
void ctfControl::InitializeRobot(void)
{
        CMN_LOG_RUN_WARNING << "ctfControl::InitializeRobot not implemented! \n";
}

/// Query: what is the status?
/*void ctfControl::GetStatus(void)
{
        CMN_LOG_INIT_VERBOSE << "ctfControl::GetStatus\n";
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
//CMN_LOG_INIT_VERBOSE << "Received Joint pos: " << pos[0] << "   " <<pos[1] << "   " << pos[2] << "\n";
        ActualJointPositions.assign(pos, pos+nTotalJointNr);
//temp
//CMN_LOG_INIT_VERBOSE << "Received Joint pos: " << ActualJointPositions[0] << "   " <<ActualJointPositions[1] << "   " << ActualJointPositions[2] << "\n";

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

