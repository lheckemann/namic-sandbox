#include "BRPtprMessages.h"
#include <iostream>
#include <stdlib.h>

// Turn constant into array!

// this will be sent through the pipe!
char *BRPStatusMessages[BRPTPRstatus_Number] = {
        "invalid!",
        BRPTPR_Idle,
        BRPTPR_Initializing,
        BRPTPR_Uncalibrated,
        BRPTPR_Ready,
        BRPTPR_Moving,
        BRPTPR_Manual,
        BRPTPR_Error,
        BRPTPR_EStop
};


// this will be sent through the pipe!
// we assume that this names are unique across the message types
std::string BRPCommands[BRPtprMessageCommandLastCommand] =
{ "invalid",
  
  BRPTPR_START_UP,
  BRPTPR_PLANNING,
  BRPTPR_CALIBRATION,
  BRPTPR_TARGETTING,
  BRPTPR_MANUAL,
  BRPTPR_EMERGENCY,

  BRPTPR_INITIALIZE,
  BRPTPR_ZFRAME,
  BRPTPR_TARGET,
  BRPTPR_HOME,

  BRPTPR_GET_POSITION,
  BRPTPR_GET_STATUS,

  BRPTPR_RESPONSE_POSITION,
  BRPTPR_RESPONSE_STATUS

};




/*
 // Constructor
BRPtprMessageStructType::BRPtprMessageStructType(BRPtprBufferType BufferType)
{
   this->Clean(BufferType);
}


/// are the magic numbers OK?
bool BRPtprMessageStructType::IsValid(void)
{
   if ( (this->StartMagic == BRPtprStartMagic) && (this->StopMagic = BRPtprStopMagic))
        return true;
   printf("Invalid buffer! Start: %d (%d) Stop: %d (%d) Command: %d\n",this->StartMagic, BRPtprStartMagic, this->StopMagic, BRPtprStopMagic, this->command);
   //printf("Invalid buffer! Command
   return false;
}


/// if "SendBuffer" initialize magic nr, otherwise clear
void BRPtprMessageStructType::Clean(BRPtprBufferType BufferType)
{
   if (BufferType==BRPtprSendBuffer) {
        this->StartMagic = BRPtprStartMagic;
    this->StopMagic = BRPtprStopMagic;
   } else {
    this->StartMagic = this->StopMagic = 0xFAFA;
   } 
}
*/
