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


#include <cisstCommon/cmnDataObject.h>
#include <cisstRealTime/rtsTask.h>
#include <cisstRealTime/rtsCommand1.h>
#include <cisstDeviceInterface/ddiTypes.h>

//Defined Offsets
#define P_Offset_X 0
#define P_Offset_Y 74
#define P_Offset_Z 55.65

#define Needle_Offset_X 0
#define Needle_Offset_Y 0
#define Needle_Offset_Z 61.2

class ctfControl: public ctfControlBase
{
public:
 // Object initialization
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
};

#endif // _ctfControl_h_

