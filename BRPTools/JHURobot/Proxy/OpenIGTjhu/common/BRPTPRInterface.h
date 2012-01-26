// BRPTPRInterface.h version 2.0: February 25, 2007 4:09pm
 
#ifndef _BRPTPRInterface_h_
#define _BRPTPRInterface_h_
 
// Workflow values: "workphase" attribute
#define BRPTPR_START_UP "START_UP"
#define BRPTPR_PLANNING "PLANNING"
#define BRPTPR_CALIBRATION "CALIBRATION"
#define BRPTPR_TARGETTING "TARGETING"
#define BRPTPR_MANUAL "MANUAL"
#define BRPTPR_EMERGENCY "EMERGENCY"
 
// Command values: "command" attribute
#define BRPTPR_INITIALIZE "INITIALIZE"
#define BRPTPR_ZFRAME "SET_Z_FRAME"
#define BRPTPR_TARGET "MOVE_TO"
#define BRPTPR_HOME "POSITION"

// Query, same as command
#define BRPTPR_GET_POSITION "GET_COORDINA"
#define BRPTPR_GET_STATUS "GET_STATUS"
 

// Response
#define BRPTPR_RESPONSE_POSITION "COORDINATES"
#define BRPTPR_RESPONSE_STATUS "STATUS"



// Status values: "status" attribute
#define BRPTPR_Idle "Idle"
#define BRPTPR_Initializing "Initializing"
#define BRPTPR_Uncalibrated "Uncalibrated"
#define BRPTPR_Ready "Ready"
#define BRPTPR_Moving "Moving"
#define BRPTPR_Manual "Manual"
#define BRPTPR_Error "Error"
#define BRPTPR_EStop "EStop"
 
#endif // _BRPTPRInterface_h_
