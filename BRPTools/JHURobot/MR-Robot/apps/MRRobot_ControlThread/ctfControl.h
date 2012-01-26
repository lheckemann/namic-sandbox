/** @file
  \brief Redefined BRPtprControl for this specific robot - extends the control logic with computation
*/ 

#ifndef _ctfControl_h_
#define _ctfControl_h_

#include <vector>
#include "BRPtprControl.h"
#include "BRPtprOpenTracker.h"
#include "BRPtprMessages.h"
#include "ctfControlBase.h"

//Defined Offsets
#define P_Offset_X  0
#define P_Offset_Y -113 // Nathan 06-10-2010#define P_Offset_Y 74, -103.9   -113.3  -113
#define P_Offset_Z  2           // Nathan 06-12-2009 #define P_Offset_Z 55.65, 6.35

#define Needle_Offset_X 0
//#define Needle_Offset_Y 101.657       // Nathan 06-12-2009 #define Needle_Offset_Y 0
#define Needle_Offset_Y 0               // Nathan 06-12-2009 #define Needle_Offset_Y 0

// If biopsy, -14.5 (-25.5mm + 11mm)
// If brachy, -25.5
#define Needle_Offset_Z -25.5           // Nathan 06-12-2009 #define Needle_Offset_Z 61.2, -39.10

static float GJointPos[8];


class ctfControl: public ctfControlBase
{
public:
 // Object initialization02
 ctfControl(int nRobotJointNumbers);
 ~ctfControl(void);
 
 // Incoming commands from the navigation software
 virtual void WorkphaseSTART_UP(void);
 virtual void WorkphasePLANNING(void);
 virtual void WorkphaseCALIBRATION(void);
 virtual void WorkphaseTARGETING(void);
 virtual void WorkphaseMANUAL(void);
 virtual void WorkphaseEMERGENCY(void); 

 virtual void GetActualCoordinates(void);
 virtual void GoToCoordinates(float position[3], float orientation[4]);
 virtual void LockRobot(void);
 virtual void UnlockRobot(void);
 
 virtual void Home(void);
 virtual void InitializeRobot(void);

// void ResetJointPositions(){
//      printf("Now the position of each axis will be reset!\n");
//      for(int i = 0; i < 8; i++)
//              printf("GJoint %d value %.3f\n", i, GJointPos[i]);      
//      this->SetJointPositions(GJointPos);
//      printf("Reset Completed!\n");
// }

 double theta;       // theta is the angle of rotation corresponding to the quaternion Q_RAS
 double NeedleDepth;


};




#endif // _ctfControl_h_

