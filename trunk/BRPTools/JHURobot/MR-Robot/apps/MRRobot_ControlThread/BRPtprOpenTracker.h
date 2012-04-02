/** @file
  \brief Receives commands, sends status through named pipe (FIFO) - Linux implementation (Csaba Csoma)
  See ctfControl.cpp for usage
*/ 

// Robot Control


#ifndef _BRPtprOpenTracker_h_
#define _BRPtprOpenTracker_h_

#ifdef MRRobot_HAS_PROXY
#include "BRPtprRingBuffer.h"
#endif
#include "BRPtprMessages.h"
#include "igtlMessage.h"
#include "igtl_header.h"

#ifdef MRRobot_HAS_PROXY
#include "BRPplatform.h" // pthread, pipe
#else
#include "igtlServerSocket.h"
#endif


class BRPtprControl;


class BRPtprOpenTracker {

#ifndef MRRobot_HAS_PROXY
 // SlicerSocket is the server socket that Slicer initially connects to
 igtl::ServerSocket::Pointer SlicerSocket;
 // SlicerClient is the client socket that is created by SlicerSocket->WaitForConnection
 igtl::ClientSocket::Pointer SlicerClient;
#endif

public:
 BRPtprOpenTracker(void);
 ~BRPtprOpenTracker(void);
 
 bool Initialize(void);

 bool IsThereNewCommand(void);

 bool ProcessNextCommand(BRPtprControl *robotControl);

 // Outgoing messages -> to the navigation software
 bool QueueActualCoordinates(float pos[3], float orientation[4], float depth_vector[3]);
 bool QueueActualRobotStatus(BRPTPRstatusType RobotStatus, const char *message);

 // temp function!
 void SetRobotStatus(BRPTPRstatusType stat) {tempStat=stat;}

protected:

 bool SendZFrameToRobot(BRPtprControl *robotControl, igtlMessage & buff);
 bool SendTargetToRobot(BRPtprControl *robotControl, igtlMessage & buff);


 void SetUpHeader(igtlMessage & msg,BRPtprMessageCommandType cmd);

 bool lInitialized;
 BRPTPRstatusType tempStat;

 bool QueueResponse(igtlMessage buff);

#ifdef MRRobot_HAS_PROXY

 int tempCounter;
 
 static void *FromNavigatorThread(void *object);
 bool StopFromNavigatorThread(void);

 bool lFromNavigatorThreadRunning;
 pthread_t hFromNavigatorThread;

 void OpenToNavigatorPipe(void);
 JHU_PIPE_HANDLE ToNavigatorPipeHandle;
 bool ToNavigatorPipe_Connected;

 igtlMessage_queue FromNavigatorCommands;
#endif

};

#endif // _BRPtprOpenTracker_h_

