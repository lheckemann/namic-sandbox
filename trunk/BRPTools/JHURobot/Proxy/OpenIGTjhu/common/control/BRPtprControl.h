/** @file
  \brief Abstract class for controlling the robot - receives all the commands and sends the responses
  See ctfControl.cpp for usage
*/ 


#ifndef _BRPtprControl_h_
#define _BRPtprControl_h_


class BRPtprControl {

public:
 BRPtprControl(void);
 virtual ~BRPtprControl(void);
 
 virtual void WorkphaseSTART_UP(void) = 0;
 virtual void WorkphasePLANNING(void) = 0;
 virtual void WorkphaseCALIBRATION(void) = 0;
 virtual void WorkphaseTARGETING(void) = 0;
 virtual void WorkphaseMANUAL(void) = 0;
 virtual void WorkphaseEMERGENCY(void) = 0;

 virtual void ZFrame(float position[3], float orientation[4]) = 0;
 virtual void GoToCoordinates(float position[3], float orientation[4]) = 0;
 virtual void InitializeRobot(void) = 0;
 virtual void Home(void) = 0;

 virtual void GetActualCoordinates(void) = 0;
 virtual void GetStatus(void) = 0;
};


#endif // _BRPtprControl_h_

