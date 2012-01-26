#include "BRPtprPipe.h"
#include <assert.h>
#include "BRPtprMessages.h"
#include "BRPplatform.h" // pthread, pipe

/// Constructor
BRPtprPipe::BRPtprPipe(void)
{
 this->lInitialized = false;
 this->lFromRobotThreadRunning = false;
 this->ToRobotPipe_Connected = false;
}


/// Destructor
BRPtprPipe::~BRPtprPipe(void)
{
        assert(lInitialized);
        StopFromRobotThread();
        if (ToRobotPipeHandle)
                JHU_PIPE_CLOSE(ToRobotPipeHandle);
                
        lInitialized = false;
}


/// Start listening/sending threads
bool BRPtprPipe::Initialize(void)
{
 assert(!lInitialized);

 // Create Thread to listen the pipe from the robot
 lFromRobotThreadRunning = true;

 // Start thread that listens the pipe from the robot
 int resStatus;
 CREATE_THREAD(hFromRobotThread,BRPtprPipe::FromRobotThread,(void *)this,resStatus);
  if (!resStatus) {
   printf("Thread creation failed, program stopped.\n");
        this->StopFromRobotThread();
        return false;
 }

 // create pipe to send messages to robot
 OpenToRobotPipe();
 
 lInitialized = true;
 return true;
}


/// Queue one command from Navigator software to Robot
bool BRPtprPipe::QueueCommand(igtlMessage *buff)
{
        assert(buff);
        assert(ToRobotPipeHandle);
        
        JHU_PIPE_STATUS ToRobotPipeStatus;

        if (!ToRobotPipe_Connected) {
                JHU_PIPE_CONNECT(ToRobotPipeHandle,ToRobotPipeStatus);
                if (!ToRobotPipeStatus) {
                        printf("Error during connection to pipe for sending commands to Robot\n");
                }
                ToRobotPipe_Connected = true;
        }

        JHU_PIPE_WRITE(buff, sizeof(igtlMessage),  ToRobotPipeHandle, ToRobotPipeStatus );
        if (!ToRobotPipeStatus) {
                printf("Pipe write error.\n");
                //JHU_PIPE_DISCONNECT(ToRobotPipeHandle);
                //OpenToRobotPipe();
        }

        return (bool)(ToRobotPipeStatus);
}


void BRPtprPipe::OpenToRobotPipe(void)
{
        JHU_PIPE_STATUS ToRobotPipeStatus;

        ToRobotPipe_Connected = false;
        JHU_OPEN_PIPE_WRITE(ToRobotCommandPipeFile, ToRobotCommandPipeFileWin,ToRobotPipeHandle,ToRobotPipeStatus);

        if (!ToRobotPipeStatus) {
                printf("Error opening pipe to robot\n");
        }
}


/// Thread proc - Receiving commands from the Robot Controller
void *BRPtprPipe::FromRobotThread(void *object)
{
    assert(object);
    BRPtprPipe *me = (BRPtprPipe *)object;
        if (!me->lFromRobotThreadRunning) {
                printf("BRPtprPipe::FromRobotThread not enabled to run. Already closed?\n");
        }
        
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE , NULL);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
        // debug_message: Thread number pthread_self() is running

        JHU_PIPE_STATUS FromRobotPipeStatusOK;
        JHU_PIPE_HANDLE FromRobotPipeHandle;

        // FromRobotPipeStatus = 0 (false) means ERROR
        JHU_OPEN_PIPE_READ(FromRobotStatusPipeFile, FromRobotStatusPipeFileWin,FromRobotPipeHandle,FromRobotPipeStatusOK);// called
        if (!FromRobotPipeStatusOK) {
                assert(false);
                return NULL;
        }

        igtlMessage buffer;
        buffer.Invalidate();

        // This blocks the pipe on Windows!
        JHU_PIPE_CONNECT(FromRobotPipeHandle, FromRobotPipeStatusOK);
        if (!FromRobotPipeStatusOK) {
                    printf("Could not connect to the pipe. \n");
                        assert(false);
                        return NULL;
                }
        
        while (me->lFromRobotThreadRunning) {
                pthread_testcancel();

                // Read
                JHU_PIPE_READ(&buffer, sizeof(igtlMessage),  FromRobotPipeHandle, FromRobotPipeStatusOK );
                if (!FromRobotPipeStatusOK) {
                        printf("Pipe connection lost? Waiting for new connection: \n");
                        JHU_PIPE_DISCONNECT(FromRobotPipeHandle);
                        JHU_PIPE_DISCONNECT(me->ToRobotPipeHandle);
                        JHU_OPEN_PIPE_READ(FromRobotStatusPipeFile, FromRobotStatusPipeFileWin,FromRobotPipeHandle,FromRobotPipeStatusOK);
                        JHU_PIPE_CONNECT(FromRobotPipeHandle, FromRobotPipeStatusOK);
                        if (FromRobotPipeStatusOK) {
                                printf("Pipe re-open successful!\n");
                        } else {
                                printf("Can't re-open pipe. Waiting 4 sec.\n");
                                sleep(4000);
                        }
                        continue;
                }

                pthread_testcancel();
                
                assert(buffer.IsValid());

                printf("Pipe packet received: %s body size: %d\n", buffer.get_header()->name, buffer.get_header()->body_size);

                if (!me->lFromRobotThreadRunning) break;
                if (me->network) {
                        me->network->SendPacket(buffer);
                }
        }

        JHU_PIPE_CLOSE(FromRobotPipeHandle);
        return NULL;
}


/// Stop the status sending thread
bool BRPtprPipe::StopFromRobotThread(void)
{
        //assert(lInitialized);
        //assert(lFromRobotThreadRunning);
        if (lFromRobotThreadRunning) {
                lFromRobotThreadRunning=false;
                return pthread_cancel(hFromRobotThread) == 0;
        }
        return true;
} 



