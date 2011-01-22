//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrMessageServer.h,v $
// $Revision: 1.2 $ 
// $Author: junichi $
// $Date: 2006/01/20 03:15:48 $
//====================================================================


//====================================================================
// Description: 
//    Message interface for MRI guided robot control system.
//====================================================================


#ifndef _INC_MRSVR_MESSAGE_SERVER
#define _INC_MRSVR_MESSAGE_SERVER

#define DEFAULT_SVR_PORT     10000


#ifdef DEBUG
  #define DEBUG_MRSVR_MESSAGE_SERVER
  #define DUMP_MSG_POSITION(mp) {\
      printf("  nx  = %d\n", mp->nx);\
      printf("  ny  = %d\n", mp->ny);\
      printf("  nz  = %d\n", mp->nz);\
      printf("  tx  = %d\n", mp->tx);\
      printf("  ty  = %d\n", mp->ty);\
      printf("  tz  = %d\n", mp->tz);\
      printf("  px  = %d\n", mp->px);\
      printf("  py  = %d\n", mp->py);\
      printf("  pz  = %d\n", mp->pz);}

#endif //DEBUG

#define NUM_CLIENT_WAIT   5
#define BUFFER_SIZE       256

// Function permission masks
#define PERMIT_NONE       0x00
#define PERMIT_OTHERS     0x01
#define PERMIT_MASTER     0x10

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <netdb.h>

#include "MrsvrThread.h"
#include "MrsvMessage.h"
#include "MrsvrStatus.h"

/*
#include "MrsvrLogicalPosition.h"
#include "MrsvrPhysicalPosition.h"
*/

#include "shmKeys.h"
#include "MrsvrRAS.h"

#include "sockutil.h"
        
  
class MrsvrMessageServer : public MrsvrThread {
  
 private:
  static const char* svrStatusStr[];
  static const char* robotModeStr[];
  pthread_mutex_t    mtxCommand;

 public:
  enum {
    SVR_STOP,
    SVR_WAIT,
    SVR_CONNECTED,
    NUM_SVR_STATUS,
  };

  enum {
    TARGET_ACCEPTED,
    TARGET_OUT_OF_RANGE,
  };

 private:
  int                port;
  int                sv;
  int                serverSockFD;
  int                masterSockFD;
  // Save socket file descriptor for master client. 
  // Only master client can send control command to this module.
  struct sockaddr_in serverAddr;
  int                serverLen;

  char               remoteOS[MESSAGE_MAX_INFO_STR];
  char               remoteSoftware[MESSAGE_MAX_INFO_STR];
  struct hostent*    peer_host; 
  struct sockaddr_in peer_sin;
  time_t             startTime;

  // Function permission table. (See permissioin masks defiend above.)
  short permissions[MSG_NUM];

  Sbfd               *masterBufferedFD;

 private:
  MrsvMsgMode*           msgMode;
  MrsvMsgCommand*        msgCmd;
  MrsvMsgPosition*       sndMsgPos;
  MrsvMsgPosition*       rcvMsgPos;

  // position parameters shared with main controller process
  /*
  MrsvrLogicalPosition*  currentLogPos;
  MrsvrPhysicalPosition* currentPhysPos;
  MrsvrLogicalPosition*  targetLogPos;
  MrsvrPhysicalPosition* targetPhysPos;
  */

  MrsvrRASWriter*    currentPos;
  MrsvrRASWriter*    setPoint;
  MrsvrStatusReader* robotStatus;  // Status reader for the MRI robot.

  unsigned char      rcvBuf[MESSAGE_BUFFER_SIZE];
  int                onRcvMsgMaster(Sbfd* bfd);
  int                onRcvMsg(int);
  msgType            sendMsgType(int, msgType);
  msgType            sendMsgPosition(int, MrsvMsgPosition*);

  bool               fSetTarget;
  double             targetPosition[3];
  //bool               fStop;
  int                nextRobotMode;   // default -1;

 public:
  MrsvrMessageServer(int);
  ~MrsvrMessageServer();
  void        init();
  void        process();
  inline void setPortNo(int no) { port = no; };
  // Note: a new port number is used after restarting the main thread.

  void        stop();

  int         getSvrStatus();
  const char* getSvrStatusStr();
  long        getConnectionTime();
  const char* getRemoteOS();
  const char* getRemoteSoftware();
  const char* getRemoteHost(); 
  const char* getRemoteIP();
  int         getRemotePort();

  bool        getTarget(double*);
  //bool        getStop();
  bool        getMode(int*);

 private:
  int         setTarget(double*);
  //void        setStop();
  int         setMode(const char*);
  void        getRobotStatus(int* mode, int* outrange, int* lock);


  

};

#endif // _INC_MRSVR_MESSAGE_SERVER
