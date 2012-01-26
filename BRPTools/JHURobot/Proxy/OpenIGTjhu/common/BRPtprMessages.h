/** @file
  \brief Header for IGT Slave and RobotControl modules - pipe commands structure and file location

  IGT Master <-> Network <-> IGT Slave <-----pipe----> Robot Controller
  It is important to run NTP in both sides! (TimeStamp is in local time)

  http://www.na-mic.org/Wiki/index.php/OpenIGTLink/Protocol/JHUBRP
*/ 

#ifndef _BRPtprMessages_h_
#define _BRPtprMessages_h_

#include <string>
#include "BRPTPRInterface.h"

#define FLOAT_COPY3(src, dst) \
{\
        (dst)[0]=(src)[0]; \
        (dst)[1]=(src)[1]; \
        (dst)[2]=(src)[2]; \
}

#define FLOAT_COPY4(src, dst) \
{\
        (dst)[0]=(src)[0]; \
        (dst)[1]=(src)[1]; \
        (dst)[2]=(src)[2]; \
        (dst)[3]=(src)[3]; \
}


/// Named Pipe: Commands going to the robot
#define ToRobotCommandPipeFile "/tmp/BRPtprOTcommand"
#define ToRobotCommandPipeFileWin "\\\\.\\pipe\\BRPtprOTcommand"

/// Named Pipe: Status coming from the robot
#define FromRobotStatusPipeFile  "/tmp/BRPtprOTstatus2"
#define FromRobotStatusPipeFileWin  "\\\\.\\pipe\\BRPtprOTstatus2"

/// The maximum number of messages in the message queue - use igtlMessage_queue instead!
//#define BRPtprMessagePoolSize 30

/// \brief Each message begins/ends with this signature to detect boundary errors
/// Since it's IPC (one machine, 2 processes), no need for CRC check
// #define BRPtprStartMagic (0xCB23)
// #define BRPtprStopMagic  (0x86FE)


/// Message type: Commands and Queries from navigation software to robot
/// If you modify this, you will need to modify BRPCommands in the .cpp too!
typedef enum {
  BRPtprInvalidCommand = 0,
  COMM_BRPTPR_START_UP,
  COMM_BRPTPR_PLANNING,
  COMM_BRPTPR_CALIBRATION,
  COMM_BRPTPR_TARGETTING,
  COMM_BRPTPR_MANUAL,
  COMM_BRPTPR_EMERGENCY,

  COMM_BRPTPR_INITIALIZE,
  COMM_BRPTPR_ZFRAME,
  COMM_BRPTPR_TARGET,
  COMM_BRPTPR_HOME,

  COMM_BRPTPR_GET_POSITION,
  COMM_BRPTPR_GET_STATUS,

  COMM_BRPTPR_RESPONSE_POSITION,
  COMM_BRPTPR_RESPONSE_STATUS,

 BRPtprMessageCommandLastCommand /* used to get the size only */
} BRPtprMessageCommandType;


/// Status type: Robot status
/// If you modify this, you will need to modify BRPStatusMessages in the .cpp too!
typedef enum {
        BRPTPRstatus_Invalid = 0,
        BRPTPRstatus_Idle,
        BRPTPRstatus_Initializing,
        BRPTPRstatus_Uncalibrated,
        BRPTPRstatus_Ready,
        BRPTPRstatus_Moving,
        BRPTPRstatus_Manual,
        BRPTPRstatus_Error,
        BRPTPRstatus_EStop,

BRPTPRstatus_Number
} BRPTPRstatusType;


/// Possible workphases (this is just "ready"/"error"/etc, so this is NOT the description)
extern char *BRPStatusMessages[];

/// Commands in text format. Size = BRPtprMessageCommandLastCommand.
extern std::string BRPCommands[];
/*
Use igtlMessage instead!

/// Buffer type for BRPtprMessageStructType
typedef enum {
        BRPtprSendBuffer=true,
        BRPtprReceiveBuffer=false
} BRPtprBufferType;


/// Structure: One message (both direction) - virtual functions can't be used (it will segfault) !
class BRPtprMessageStructType {
public:
 BRPtprMessageStructType(BRPtprBufferType BufferType);

 bool IsValid(void); 
 void Clean(BRPtprBufferType BufferType);

 unsigned int StartMagic;
 BRPtprMessageCommandType command;
 BRPTPRstatusType status;
 float position[3];
 float orientation[4];
 float depth_vector[3];
 unsigned int StopMagic;
};
*/

#endif // _BRPtprMessages_h_

