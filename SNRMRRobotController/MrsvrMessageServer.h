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
#include <sys/errno.h>

#include "MrsvrThread.h"
#include "MrsvMessage.h"
#include "MrsvrStatus.h"

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlServerSocket.h"
#include "igtlSocket.h"

/*
#include "MrsvrLogicalPosition.h"
#include "MrsvrPhysicalPosition.h"
*/

#include "shmKeys.h"
#include "MrsvrRAS.h"

class MrsvrMessageServer : public MrsvrThread {
  
 public:
  typedef float Matrix4x4[4][4];

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
  int                connectionStatus;
  int                fRunServer;

  char               remoteOS[MESSAGE_MAX_INFO_STR];
  char               remoteSoftware[MESSAGE_MAX_INFO_STR];
  time_t             startTime;

  // Function permission table. (See permissioin masks defiend above.)
  short permissions[MSG_NUM];

 private:
  
  MrsvMsgMode*           msgMode;
  MrsvMsgCommand*        msgCmd;
  MrsvMsgPosition*       sndMsgPos;
  MrsvMsgPosition*       rcvMsgPos;

  igtl::Socket::Pointer  socket;

  MrsvrRASWriter*    currentPos;
  MrsvrRASWriter*    setPoint;
  MrsvrStatusReader* robotStatus;  // Status reader for the MRI robot.

  unsigned char      rcvBuf[MESSAGE_BUFFER_SIZE];
  int                onRcvMsgMaster(igtl::Socket::Pointer& socket, igtl::MessageHeader::Pointer& header);
  msgType            sendMsgType(int, msgType);
  msgType            sendMsgPosition(int, MrsvMsgPosition*);

  bool               fSetTargetMatrix;
  bool               fSetCalibrationMatrix;

  Matrix4x4          targetMatrix;
  Matrix4x4          calibrationMatrix;
  //double             targetPosition[3];
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

  bool        getTargetMatrix(Matrix4x4& matrix);
  bool        getCalibrationMatrix(Matrix4x4& matrix);
  //bool        getStop();
  bool        getMode(int*);

  igtl::Socket* getSocket() { return this->socket; };
  int         sendCurrentPosition(igtl::Matrix4x4& current);

 private:
  int         setTargetMatrix(igtl::Matrix4x4& matrix);
  int         setCalibrationMatrix(igtl::Matrix4x4& matrix);
  //void        setStop();
  int         setMode(const char*);
  void        getRobotStatus(int* mode, int* outrange, int* lock);
 

};

#endif // _INC_MRSVR_MESSAGE_SERVER
