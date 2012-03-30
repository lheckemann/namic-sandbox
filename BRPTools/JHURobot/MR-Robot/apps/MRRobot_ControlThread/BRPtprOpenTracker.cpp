
#include "BRPtprOpenTracker.h"
#include <iostream>

#include <assert.h>
#include <stdlib.h>

#include <cstring>  // added

#include "BRPtprMessages.h"
#include "BRPtprControl.h"


// Added on 22/03/2012 by Sohrab

#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlClientSocket.h"

// Local file
#include "igtlMessage.h"


// ----------------------------

#include <cisstCommon/cmnLogger.h>

#ifdef MRRobot_HAS_PROXY

#include "BRPplatform.h" // pthread, pipe


/// Constructor
BRPtprOpenTracker::BRPtprOpenTracker(void)
{
 this->lInitialized = false;

 this->lFromNavigatorThreadRunning = false;

 this->tempCounter=0;
 this->tempStat=BRPTPRstatus_Invalid;
 this->ToNavigatorPipe_Connected = false;

}


/// Destructor: Kills both threads
BRPtprOpenTracker::~BRPtprOpenTracker(void)
{
        assert(lInitialized);
        
        /// \todo Should we wait for thread termination, for safety? We're exiting anyways...
        StopFromNavigatorThread();
        if (ToNavigatorPipeHandle)
                JHU_PIPE_CLOSE(ToNavigatorPipeHandle);
        
        lInitialized = false;
}


/// Creates 1 thead and start it up - no commands accepted before Initialize
bool BRPtprOpenTracker::Initialize(void)
{
 assert(!lInitialized);

 // Create thread (command going to navigator)
 lFromNavigatorThreadRunning = true;
 int resStatus;

 CREATE_THREAD(hFromNavigatorThread,BRPtprOpenTracker::FromNavigatorThread,(void *)this,resStatus);

 if (!resStatus) {
         printf("Thread creation failed, program stopped.\n");
        this->StopFromNavigatorThread();
        return false;
 }

 // Create the pipe (status coming from robot)
 OpenToNavigatorPipe();
 
 lInitialized = true;
 return true;
}

/// True if there's a message in the queue
bool BRPtprOpenTracker::IsThereNewCommand(void)
{
 /*
 // Send positions on timer to Navigator (Slicer hack)
 if ( (tempStat==BRPTPRstatus_Manual) || (tempStat==BRPTPRstatus_Moving) ) {
        // Simulate "GetPosition" every 10th message
        this->tempCounter++;
        if (this->tempCounter>10) {
                 this->tempCounter=0;
                BRPtprMessageStructType buffer(BRPtprSendBuffer);
                buffer.command= COMM_BRPTPR_GET_POSITION;
                this->CommandMessagePool.WriteOneRecord(&buffer);
        }
 }*/

        return !FromNavigatorCommands.empty();
}

#else  // !MRRobot_HAS_PROXY

const unsigned int SlicerPort = 5678;

BRPtprOpenTracker::BRPtprOpenTracker(void) : SlicerSocket(0), SlicerClient(0)
{
 this->lInitialized = false;
}

BRPtprOpenTracker::~BRPtprOpenTracker(void) {
    if (SlicerSocket)
        SlicerSocket->CloseSocket();
    if (SlicerClient)
        SlicerClient->CloseSocket();
}
 
bool BRPtprOpenTracker::Initialize(void)
{
    CMN_LOG_RUN_VERBOSE << "Initialize: creating OpenIGTLink server on port " << SlicerPort << std::endl;
    SlicerSocket = igtl::ServerSocket::New();
    if (SlicerSocket->CreateServer(SlicerPort) == -1) {
        CMN_LOG_RUN_ERROR << "Initialize: could not create server on port " << SlicerPort << std::endl;
        return false;
    }
    this->lInitialized = true;
    return true;
}


bool BRPtprOpenTracker::IsThereNewCommand(void) {
  return true;
}

#endif

#include "igtl_util.h"

float jhu_get_float32(igtlMessage & msg, int index)
{
 long bit32=0; /// TODO Not Safe!
 memcpy((void*)(&bit32), (void *)(msg.body()+index), 4);
 if (igtl_is_little_endian()) {
   bit32 = BYTE_SWAP_INT32(bit32);
  }
 float f;
 memcpy((void*)(&f), (void*)(&bit32), sizeof(float));
 return f; 
}

bool BRPtprOpenTracker::SendZFrameToRobot(BRPtprControl *robotControl, igtlMessage & msg)
{
 float position[3];
 float orientation[4];
 
 position[0]=jhu_get_float32(msg, 0*4);
 position[1]=jhu_get_float32(msg, 1*4);
 position[2]=jhu_get_float32(msg, 2*4);
 
 orientation[0]=jhu_get_float32(msg, 3*4);
 orientation[1]=jhu_get_float32(msg, 4*4);
 orientation[2]=jhu_get_float32(msg, 5*4);
 orientation[3]=jhu_get_float32(msg, 6*4);

// ------------------
 std::cerr << "BRPtprOpenTracker::SendZFrameToRobot ("
           << position[0] << "," << position[1] << "," << position[2] << ")" 
           << " ("  << orientation[0] << "," << orientation[1] << "," << orientation[2] << "," << orientation[3] << ")" << std::endl;
// ------------------

 robotControl->ZFrame(position, orientation); 
 return true;
}


bool BRPtprOpenTracker::SendTargetToRobot(BRPtprControl *robotControl, igtlMessage & msg)
{
 float position[3];
 float orientation[4];
 
 position[0]=jhu_get_float32(msg, 0*4);
 position[1]=jhu_get_float32(msg, 1*4);
 position[2]=jhu_get_float32(msg, 2*4);
 
 orientation[0]=jhu_get_float32(msg, 3*4);
 orientation[1]=jhu_get_float32(msg, 4*4);
 orientation[2]=jhu_get_float32(msg, 5*4);
 orientation[3]=jhu_get_float32(msg, 6*4);

 std::cerr << "BRPtprOpenTracker::SendTargetToRobot ("
           << position[0] << "," << position[1] << "," << position[2] << ")" 
           << " ("  << orientation[0] << "," << orientation[1] << "," << orientation[2] << "," << orientation[3] << ")" << std::endl;

 robotControl->GoToCoordinates(position, orientation); 
 return true;
}


#ifdef MRRobot_HAS_PROXY

/// Processes the command from the queue
bool BRPtprOpenTracker::ProcessNextCommand(BRPtprControl *robotControl)
{
 assert(robotControl);

 // is there a command to process?
 if (FromNavigatorCommands.empty())
   return false;

 // get next command
 igtlMessage buff = FromNavigatorCommands.front();
 FromNavigatorCommands.pop_front();

 // it is valid?
 assert(buff.IsValid());
 if (!buff.IsValid())
         return false;
   
 // Get the command name
 char cmdstr[IGTL_HEADER_NAMESIZE+1]; // 20+1
 strncpy(cmdstr, buff.get_header()->name, IGTL_HEADER_NAMESIZE); // Max 20
 cmdstr[IGTL_HEADER_NAMESIZE]='\0'; // 21st is always 0

 // Find the corresponding number
 BRPtprMessageCommandType cmd=BRPtprInvalidCommand;

 BRPtprMessageCommandType i=COMM_BRPTPR_START_UP;
 while (i<BRPtprMessageCommandLastCommand) {
         assert(BRPCommands[i].length()<=12);
         if ( strcmp(cmdstr, BRPCommands[i].c_str()) == 0 ) {
                 cmd=i;
                 break;
         }
         i=(BRPtprMessageCommandType)((int)i+1);
 }
 
 // Process command received
 switch (cmd) {
   case COMM_BRPTPR_START_UP:                   robotControl->WorkphaseSTART_UP(); break;
   case COMM_BRPTPR_PLANNING:                   robotControl->WorkphasePLANNING(); break;
   case COMM_BRPTPR_CALIBRATION:                robotControl->WorkphaseCALIBRATION(); break;
   case COMM_BRPTPR_TARGETTING:         robotControl->WorkphaseTARGETING(); break;
   case COMM_BRPTPR_MANUAL:                     robotControl->WorkphaseMANUAL(); break;
   case COMM_BRPTPR_EMERGENCY:          robotControl->WorkphaseEMERGENCY(); break;

   case COMM_BRPTPR_INITIALIZE: robotControl->InitializeRobot(); break;
   case COMM_BRPTPR_ZFRAME:     SendZFrameToRobot(robotControl, buff);  break;
   case COMM_BRPTPR_TARGET:     SendTargetToRobot(robotControl, buff); break;
   case COMM_BRPTPR_HOME:       robotControl->Home(); break;

   case COMM_BRPTPR_GET_POSITION:       robotControl->GetActualCoordinates(); break;
   case COMM_BRPTPR_GET_STATUS: robotControl->GetStatus(); break;

   default: printf("Unknown command ID: [%d] Name: [%s]\n",cmd,cmdstr);
           assert(false);return false; // wrong command??
 }
 return true; 
}


/// Send a status to navigation software (through pipe)
bool BRPtprOpenTracker::QueueResponse(igtlMessage buff)
{
        assert(ToNavigatorPipeHandle);

        buff.encode_header();

        JHU_PIPE_STATUS ToNavigatorPipeStatus;
        JHU_PIPE_WRITE(&buff, sizeof(igtlMessage),  ToNavigatorPipeHandle, ToNavigatorPipeStatus );

        return (bool)(ToNavigatorPipeStatus);
}


/// Thread to watch commands from the navigator - save it to a queue (\see ProcessNextCommand)
void *BRPtprOpenTracker::FromNavigatorThread(void *object)
{
    assert(object);
    BRPtprOpenTracker *me=(BRPtprOpenTracker *)object;
        if (!me->lFromNavigatorThreadRunning) {
                printf("BRPtprOpenTracker::FromNavigatorThread not enabled to run. Already closed?\n");
        }
        
        // This thread can be cancelled anytime 
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE , NULL);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);


        //umask(0);
        //mknod(CommandPipeFile, S_IFIFO|0666, 0);
        //FILE *fp;
        //fp = fopen(CommandPipeFile, "r");

        JHU_PIPE_STATUS FromNavigatorPipeStatusSuccess;
        JHU_PIPE_HANDLE FromNavigatorPipeHandle;

        JHU_OPEN_PIPE_READ2(ToRobotCommandPipeFile, ToRobotCommandPipeFileWin,FromNavigatorPipeHandle,FromNavigatorPipeStatusSuccess);
        if (!FromNavigatorPipeStatusSuccess) {
                printf("BRPtprOpenTracker::FromNavigatorThread: could not open pipe for read (ToNavigator).\n\n Is the Proxy running? \n\n");
                return NULL;
        }

        igtlMessage buffer;
        buffer.Invalidate();

        while (me->lFromNavigatorThreadRunning) {
                pthread_testcancel();
/*
 int BytesRead; \
  BytesRead = fread(buffer, size, 1, ph); \
  ps = (BytesRead == 1); \
*/
        // read
        //buffer.Clean(BRPtprReceiveBuffer);
                //if (fread(&buffer, sizeof(BRPtprMessageStructType), 1, fp)!=1) {
                JHU_PIPE_READ(&buffer, sizeof(igtlMessage),  FromNavigatorPipeHandle, FromNavigatorPipeStatusSuccess );
                if (!FromNavigatorPipeStatusSuccess) {
                        printf("BRPtprOpenTracker::FromNavigatorThread: Pipe reset...\n");
                        sleep(4000);
                        //pthread_yield();
                        continue;
                }
                if (!me->lFromNavigatorThreadRunning) break;
                pthread_testcancel();
                assert(buffer.IsValid());

                // command arrived
                me->FromNavigatorCommands.push_back(buffer);


                /*
        // can we save it to the buffer?
                while (me->CommandMessagePool.GetMaxWriteNumber()<1) {
                   pthread_yield(); //can we use select(2) or poll(2)?
                   printf("BRPtprOpenTracker::CommandThread:  losing data?\n");
                        // we'll lose data! Not better to overwrite? - or it will be re-sent as no ACK?
                        // Delete (remove from the queue) the oldest item, making space for the new item?
                        // sigalert?
                }
        // write to pool!
                me->CommandMessagePool.WriteOneRecord(&buffer);
                */
        }
        
        //fclose(fp);
        JHU_PIPE_CLOSE(me->ToNavigatorPipeHandle);
        return NULL;
}
 
 
bool BRPtprOpenTracker::StopFromNavigatorThread(void)
{
        if (lFromNavigatorThreadRunning) {
                lFromNavigatorThreadRunning=false;
                printf("Stopping FromNavigatorThread\n");
                return pthread_cancel(hFromNavigatorThread) == 0;
        }
        return true;
}

void BRPtprOpenTracker::OpenToNavigatorPipe(void)
{

 pthread_setcancelstate(PTHREAD_CANCEL_ENABLE , NULL);
 pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
        // debug_message: Thread number pthread_self() is running

 JHU_PIPE_STATUS ToNavigatorPipeStatus;
 ToNavigatorPipe_Connected = false;
 JHU_OPEN_PIPE_WRITE2(FromRobotStatusPipeFile, FromRobotStatusPipeFileWin,ToNavigatorPipeHandle,ToNavigatorPipeStatus);

 if (!ToNavigatorPipeStatus) {
         printf("BRPtprOpenTracker::OpenToNavigatorPipe: could not open pipe for write (FromNavigator).\n\n Does the Proxy run? \n\n");
         exit(55);
 }
}

#else // !MRRobot_HAS_PROXY

union FloatUnion {
  char bytes[4];
  float data;

  float swap() {
    char tmp = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = tmp;
    tmp = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = tmp;
    return data;
  }
};

// This method checks the received commands and calls the appropriate function:
bool BRPtprOpenTracker::ProcessNextCommand(BRPtprControl *robotControl)
{
    static igtl::MessageHeader::Pointer headerMsg = 0;  // Should be in class
    if (!SlicerClient) {
        // Wait up to 10 msec for a connection
        SlicerClient = SlicerSocket->WaitForConnection(10);
        if (SlicerClient) {
            CMN_LOG_RUN_VERBOSE << "Received connection from Slicer!" << std::endl;
            // Create a message buffer to receive header
            headerMsg = igtl::MessageHeader::New();
        }
    }
    
    if (SlicerClient) {
        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the SlicerClient
        int rs = SlicerClient->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize(), 0);
        if (rs == headerMsg->GetPackSize())  {
           // Now, based on device type in header, receive and process and message body

           // Deserialize the header  
           headerMsg->Unpack(); 

           std::cerr << "Receiving a message: " << std::endl;
           std::cerr << "       Device Type: \"" << headerMsg->GetDeviceType() << "\"" << std::endl;
           std::cerr << "       Device Name: \"" << headerMsg->GetDeviceName() << "\"" << std::endl;

           if (strcmp(headerMsg->GetDeviceType(), "SET_Z_FRAME") == 0) {
               FloatUnion parms[7];
               std::cerr << "Processing Z_FRAME" << std::endl;
               rs = SlicerClient->Receive(parms, 7*sizeof(FloatUnion));
               if (rs == 7*sizeof(float)) {
                 float pos[3];
                 float orient[4];
                 pos[0] = parms[0].swap(); pos[1] = parms[1].swap(); pos[2] = parms[2].swap();
                 std::cerr << "Z_FRAME_Position = " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
                 orient[0] = parms[3].swap(); orient[1] = parms[4].swap(); orient[2] = parms[5].swap(); orient[3] = parms[6].swap();
                 std::cerr << "Z_FRAME_Orientation = " << orient[0] << ", " << orient[1] << ", " << orient[2] << ", " << orient[3] << std::endl;
                 robotControl->ZFrame(pos, orient); 
               }
               else
                   std::cerr << "Error: read " << rs << " bytes" << std::endl;
           }
           else if (strcmp(headerMsg->GetDeviceType(), "GET_COORDINA") == 0) {
             std::cerr << "Processing GET_COORDINA" << std::endl;
             robotControl->GetActualCoordinates();
           }
           else if (strcmp(headerMsg->GetDeviceType(), "MOVE_TO") == 0) {
               FloatUnion parms[7];
               std::cerr << "Processing MOVE_TO" << std::endl;
               rs = SlicerClient->Receive(parms, 7*sizeof(FloatUnion));
               if (rs == 7*sizeof(float)) {
                 float pos[3];
                 float orient[4];
                 pos[0] = parms[0].swap(); pos[1] = parms[1].swap(); pos[2] = parms[2].swap();
                 std::cerr << "TargetPosition = " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
                 orient[0] = parms[3].swap(); orient[1] = parms[4].swap(); orient[2] = parms[5].swap(); orient[3] = parms[6].swap();
                 std::cerr << "TargetOrientation = " << orient[0] << ", " << orient[1] << ", " << orient[2] << ", " << orient[3] << std::endl;
                 robotControl->GoToCoordinates(pos, orient); 
               }
               else
                   std::cerr << "Error: read " << rs << " bytes" << std::endl;
           }
           else if (strcmp(headerMsg->GetDeviceType(), "START_UP") == 0) {
             std::cerr << "Processing START_UP" << std::endl;
             robotControl->WorkphaseSTART_UP();
           }
           else if (strcmp(headerMsg->GetDeviceType(), "PLANNING") == 0) {
             std::cerr << "Processing PLANNING" << std::endl;
             robotControl->WorkphasePLANNING();
           }
           else if (strcmp(headerMsg->GetDeviceType(), "CALIBRATION") == 0) {
             std::cerr << "Processing CALIBRATION" << std::endl;
             robotControl->WorkphaseCALIBRATION();
           }
           else if (strcmp(headerMsg->GetDeviceType(), "TARGETING") == 0) {
             std::cerr << "Processing TARGETING" << std::endl;
             robotControl->WorkphaseTARGETING();
           }
           else if (strcmp(headerMsg->GetDeviceType(), "MANUAL") == 0) {
             std::cerr << "Processing MANUAL" << std::endl;
             robotControl->WorkphaseMANUAL();
           }
           else if (strcmp(headerMsg->GetDeviceType(), "EMERGENCY") == 0) {
             std::cerr << "Processing EMERGENCY" << std::endl;
             robotControl->WorkphaseEMERGENCY();
           }
           else if (strcmp(headerMsg->GetDeviceType(), "INITIALIZE") == 0) {
             std::cerr << "Processing INITIALIZE" << std::endl;
             robotControl->InitializeRobot();
           }
           else if (strcmp(headerMsg->GetDeviceType(), "HOME") == 0) {
             std::cerr << "Processing HOME" << std::endl;
             robotControl->Home();
           }
           else if (strcmp(headerMsg->GetDeviceType(), "GET_STATUS") == 0) {
             std::cerr << "Processing GET_STATUS" << std::endl;
             robotControl->GetStatus();
           }
           else
           {
               std::cerr << "Skipping message" << std::endl;
               SlicerClient->Skip(headerMsg->GetBodySizeToRead(), 0);
           } 
        }
        else if (rs > 0) {
           std::cerr << "Received message of size " << rs << ", header size = " << headerMsg->GetPackSize() << std::endl;
        }
    }
    return true;
}

/// Send a status to navigation software (through OpenIGTLink)
bool BRPtprOpenTracker::QueueResponse(igtlMessage buff)
{
    if (!SlicerClient->Send(buff.data(), buff.length())) {
        std::cerr << "Error sending position to robot" << std::endl;
        return false;
    }
    return true;
}

#endif

// ------- Queue messages to the Navigation Software ---------------

#include "igtl_util.h"

#ifdef _ANSI_ARGS_
#undef _ANSI_ARGS_
#endif
#ifdef PROTOTYPES
#undef PROTOTYPES
#endif

#define PROTOTYPES
#define _ANSI_ARGS_(c)  c
#include "crc32.h"

void add_Float32(igtlMessage & msg, float f, int index)
{
    long bit32=0; /// TODO Not Safe! 

        memcpy((void*)(&bit32), (void*)(&f), sizeof(float));

        if (igtl_is_little_endian()) {
                bit32 = BYTE_SWAP_INT32(bit32);
        }
        memcpy((void *)(msg.body()+index),&bit32,4);
}

void add_Int16(igtlMessage & msg, unsigned int bit16, int index)
{
        if (igtl_is_little_endian()) {
                bit16 = BYTE_SWAP_INT16(bit16);
        }
        memcpy((void *)(msg.body()+index),&bit16,2);
}

void add_Int64(igtlMessage & msg, long long bit64, int index)
{
        if (igtl_is_little_endian()) {
                bit64 = BYTE_SWAP_INT64(bit64);
        }
        memcpy((void *)(msg.body()+index),&bit64,8);
}

int add_Char(igtlMessage & msg, const char *ch, int index, int pad_to_length)
{
        int ch_len = strlen(ch);

        memcpy((void *)(msg.body()+index),ch,ch_len);

        if ( (pad_to_length>0) && (ch_len<pad_to_length) ) {
                memset((void *)(msg.body()+index+ch_len),0,pad_to_length-ch_len);
                ch_len+= pad_to_length-ch_len;
        }

        return index + ch_len;
}


void calc_crc(igtlMessage & msg)
{
  unsigned long crc = crc32(0L, Z_NULL, 0);
  crc = crc32(crc, (unsigned char*)( msg.body() ), msg.body_length() );
  msg.get_header()->crc = crc; 
}


/// Queue for sending the actual position and orientation - false if buffer full
bool BRPtprOpenTracker::QueueActualCoordinates(float pos[3],float orientation[4], float depth_vector[3])
{
        assert(lInitialized);
        igtlMessage msg;
        SetUpHeader(msg, COMM_BRPTPR_RESPONSE_POSITION);
    /*
        * Vector of three 32 bit floats: position X, Y, Z
    * Vector of three 32 bit floats: orientation
    * One 32 bit float: current insertion depth 
        */
        add_Float32(msg, pos[0],0*4);
        add_Float32(msg, pos[1],1*4);
        add_Float32(msg, pos[2],2*4);

        add_Float32(msg, orientation[0],3*4);
        add_Float32(msg, orientation[1],4*4);
        add_Float32(msg, orientation[2],5*4);
        //gsf added - changed to standard igtlink type pos(3),ori(4)
        add_Float32(msg, orientation[3],6*4);   

        //gsf commented, no depth, just actual position, on front of rob?
        //add_Float32(msg, depth_vector[0],IGTL_HEADER_SIZE+6*4); /// TODO why not 1&2 too?

        msg.body_length(28);
        

        calc_crc(msg);

//gsf temp
std::cerr << "BRPtprOpenTracker::QueueActualCoordinates (" << pos[0] << "," << pos[1] << "," << pos[2] << ")"
                << " (" << orientation[0] << "," << orientation[1] << "," << orientation[2] << "," << orientation[3] << ")"
                ")\n";

        return QueueResponse(msg);
        /* todo
        if (StatusMessagePool.GetMaxWriteNumber()<1)
                return false;
                
        BRPtprMessageStructType buff(BRPtprSendBuffer);
        buff.command = COMM_BRPTPR_RESPONSE_POSITION;
        FLOAT_COPY3(pos,buff.position);
    FLOAT_COPY4(orientation,buff.orientation);
        FLOAT_COPY3(depth_vector,buff.depth_vector);
    return StatusMessagePool.WriteOneRecord(&buff);
        return false;*/
}

bool BRPtprOpenTracker::QueueActualRobotStatus(BRPTPRstatusType RobotStatus, char *message)
{
        assert(lInitialized);
        igtlMessage msg;
        SetUpHeader(msg, COMM_BRPTPR_RESPONSE_STATUS);
        /*
                 Unsigned short (16bit) - Status code groups: 1-Ok, 2-Generic Error
                 64 bit integer - Sub code for the error (ex. 0x200 - file not found)
                 char[20] -     "Error", "Ok", "Starting up" - can be anything...
                 char[ BodySize - 30 ] - Optional (English) description (message)
        */

        unsigned int error_code = 2;
        switch (RobotStatus) {
                case BRPTPRstatus_Idle:
                case BRPTPRstatus_Initializing:
                case BRPTPRstatus_Uncalibrated:
                case BRPTPRstatus_Ready:
                case BRPTPRstatus_Moving:
                case BRPTPRstatus_Manual:
                                error_code = 1;
        }

        add_Int16(msg, error_code, 0);
        add_Int64(msg, RobotStatus, 2);
        add_Char(msg, BRPCommands[RobotStatus].c_str(),  8+2, 20);
        int end_p =  add_Char(msg, message, 8+2+20, 0);

        msg.body_length(end_p);

        calc_crc(msg);
        return QueueResponse(msg);
        /* todo
        if (StatusMessagePool.GetMaxWriteNumber()<1)
                return false;
                
        BRPtprMessageStructType buff(BRPtprSendBuffer);
        buff.command = COMM_BRPTPR_RESPONSE_STATUS;
        buff.status = RobotStatus;
    return StatusMessagePool.WriteOneRecord(&buff);
        return false;*/
}


void BRPtprOpenTracker::SetUpHeader(igtlMessage & msg,BRPtprMessageCommandType cmd)
{
        /// TODO verify cmd boundary
        std::memcpy(msg.get_header()->name, BRPCommands[cmd].c_str(), BRPCommands[cmd].length() );
        std::memcpy(msg.get_header()->device_name, "JHUbrpTP", 9 ); // max: 20 (IGTL_HEADER_DEVSIZE)
}

