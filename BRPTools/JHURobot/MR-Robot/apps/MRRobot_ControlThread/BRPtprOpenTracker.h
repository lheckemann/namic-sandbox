/** @file
  \brief Receives commands, sends status through named pipe (FIFO) - Linux implementation (Csaba Csoma)
  See ctfControl.cpp for usage
*/ 

// Robot Control


#ifndef _BRPtprOpenTracker_h_
#define _BRPtprOpenTracker_h_

//#include "BRPtprRingBuffer.h"
#include "BRPtprMessages.h"
#include "igtlMessage.h"
#include "igtl_header.h"

#include "BRPplatform.h" // pthread, pipe


class BRPtprControl;


class BRPtprOpenTracker {
public:
 BRPtprOpenTracker(void);
 ~BRPtprOpenTracker(void);
 
 bool Initialize(void);

 bool IsThereNewCommand(void);

 bool ProcessNextCommand(BRPtprControl *robotControl);
 
 bool SendZFrameToRobot(BRPtprControl *robotControl, igtlMessage & buff);
 bool SendTargetToRobot(BRPtprControl *robotControl, igtlMessage & buff);

 // Outgoing messages -> to the navigation software
 bool QueueActualCoordinates(float pos[3],float orientation[4], float depth_vector[3]);
 bool QueueActualRobotStatus(BRPTPRstatusType RobotStatus, char *message);
 //bool QueueActualCoordinates(float pos[3],float orientation[4]);
 //bool QueueNumberOfJoints( int JointNumber );
 //bool QueueActualJointPositions(int nr, float jpos[]);
 //bool QueueStatus(char *status, char *message, bool lDone, bool lEmergency);

 // temp function!
 void SetRobotStatus(BRPTPRstatusType stat) {tempStat=stat;}

protected:

 bool QueueResponse(igtlMessage buff);
 void SetUpHeader(igtlMessage & msg,BRPtprMessageCommandType cmd);

 int tempCounter;
 BRPTPRstatusType tempStat;
 
 bool lInitialized;

 static void *FromNavigatorThread(void *object);
 bool StopFromNavigatorThread(void);

 bool lFromNavigatorThreadRunning;
 pthread_t hFromNavigatorThread;

 void OpenToNavigatorPipe(void);
 JHU_PIPE_HANDLE ToNavigatorPipeHandle;
 bool ToNavigatorPipe_Connected;

 igtlMessage_queue FromNavigatorCommands;

 /*
 static void *CommandThread(void *object);
 bool StopCommandThread(void);
 
 static void *StatusThread(void *object);
 bool StopStatusThread(void);
 */



 /*
 bool lCommandThreadRunning;
 pthread_t hCommandThread;
 
 bool lStatusThreadRunning;
 pthread_t hStatusThread;
*/
// BRPtprRingBuffer CommandMessagePool;
 //BRPtprRingBuffer StatusMessagePool;
};

#endif // _BRPtprOpenTracker_h_

