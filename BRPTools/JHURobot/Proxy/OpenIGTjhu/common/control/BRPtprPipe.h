#ifndef _BRPtprPipe_h_
#define _BRPtprPipe_h_

// Pipe for the proxy
// First thread: get messages from pipe, send it to Slicer

//#include "BRPtprRingBuffer.h"
//#include "BRPtprMessagesProc.h"
#include "igtlMessage.h"
#include "igtlNetwork.h"

#include "BRPplatform.h"


class BRPtprPipe {
public:
        BRPtprPipe(void);
        ~BRPtprPipe(void);

        bool Initialize(void);

        bool QueueCommand(igtlMessage *buff);
        //bool ThereIsPosition(void);
        //bool GetNextPosition(igtlMessage *buff);

        void SetNetwork(igtlNetwork *n) { network=n; }

protected:
        bool lInitialized;

        igtlNetwork *network;

        void OpenToRobotPipe(void);
        JHU_PIPE_HANDLE ToRobotPipeHandle;   // FILE *
        bool ToRobotPipe_Connected;
        

        //static void *CommandThread(void *object);
        //bool StopCommandThread(void);
 
        static void *FromRobotThread(void *object);
        bool StopFromRobotThread(void);
        
        bool lFromRobotThreadRunning;
        pthread_t hFromRobotThread;
 
        //bool lStatusThreadRunning;
        //pthread_t hStatusThread;

        //igtlMessage_queue CommandMessagePool;
        //igtlMessage_queue FromRobotMessagePool;

};

#endif // _BRPtprPipe_h_  




