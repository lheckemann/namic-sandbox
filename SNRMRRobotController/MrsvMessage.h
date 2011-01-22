//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003 by The University of Tokyo, All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvMessage.h,v $
// $Revision: 1.1.1.1 $ 
// $Author: junichi $
// $Date: 2005/01/11 11:38:33 $
//====================================================================


//====================================================================
// Description: 
//    Definition of IDs and structures for messaging between 3D Slicer 
//    module and robot control system.
//====================================================================


#ifndef _INC_MRSV_MESSAGE
#define _INC_MRSV_MESSAGE



#define MESSAGE_BUFFER_SIZE      256

#define MESSAGE_MAX_INFO_STR     64

#define MRSV_START_STRING        "MRSV_SERVER:1.0"

typedef short msgType;

#define MSG_CHANGE_MODE          ((msgType)0)
#define MSG_SEND_COMMAND         ((msgType)1)
#define MSG_SEND_POSITION        ((msgType)2)
#define MSG_SEND_SYSINFO         ((msgType)3)
#define MSG_REQUEST_POSITION     ((msgType)4)
#define MSG_NUM                  ((msgType)5)

#define MSG_RETURN_SUCCESS       ((msgType)5)
#define MSG_RETURN_ERROR         ((msgType)6)
#define MSG_RETURN_DENY          ((msgType)7)


typedef struct {
  short modeNumber;
} MrsvMsgMode;

typedef struct {
  short command;
  short target;
  int value;
} MrsvMsgCommand;

typedef struct {
  int nx;
  int ny;
  int nz;
  int tx;
  int ty;
  int tz;
  int px;
  int py;
  int pz;
} MrsvMsgPosition;

typedef struct {
  char software[MESSAGE_MAX_INFO_STR];
  char os[MESSAGE_MAX_INFO_STR];
} MrsvMsgSysInfo;



#endif // _INC_MSV_MESSAGE
