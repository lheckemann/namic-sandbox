// Version 1.0: Jul 17, 2007 12:08PM

#ifndef _BRPtprStatus_h_
#define _BRPtprStatus_h_

typedef enum { 
  BRPtprStatus_Invalid = 0,
  
  BRPtprStatus_Initializing,
  BRPtprStatus_Ready,
  BRPtprStatus_Moving,
  BRPtprStatus_Manual,
  BRPtprStatus_Error,
  
  BRPtprStatus_Size
 } BRPtprStatusType; // should be the same structure as BRPtprStatusMessage in BRPtprStatus.cpp

extern char *BRPtprStatusMessage[];


#endif // _BRPtprStatus_h_
