//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrMrtsCon.h,v $
// $Revision: 1.1.1.1 $ 
// $Author: junichi $
// $Date: 2005/01/11 11:38:33 $
//====================================================================


//====================================================================
// Description: 
//    Message interface to communicate with MRTS.
//====================================================================


#ifndef _INC_MRSVR_MRTS_CON
#define _INC_MRSVR_MRTS_CON

#ifndef ENABLE_MRTS_CONNECTION
  #define ENABLE_MRTS_CONNECTION
#endif

#ifdef DEBUG
  #define DEBUG_MRSVR_MRTS_CON
#endif

#ifdef USE_ART
  #include <linux/art_task.h>
#endif

//#include   "MRTrackingClient.h"
#include   "MrsvrThread.h"
//#include "MrsvMessage.h"
#include   "shmKeys.h"
#include   "MrsvrRAS.h"


#define MAX_HOSTNAME             256
#define DEFAULT_MRTS_PORT        7776
#define DEFAULT_MRTS_HOST        "3.245.34.176"
#define DEFAULT_MRTS_INTERVAL    100000

          
class MrsvrMrtsCon : public MrsvrThread {

 private:
  static const char* statusStr[];
  enum {
    MRTS_DISCONNECTED,
    MRTS_CONNECTED,
    MRTS_RUN,
  };

 private:
  char mrtsHostname[MAX_HOSTNAME];
  int  mrtsPort;
  int  status;
  long interval;
  int  isRun;

 private:
  //MRTrackingClient*  mtc;
  //MsgTransform       msgTrans;
  int                getTrans();
  MrsvrRASWriter*    setPoint;  

 public:
  MrsvrMrtsCon();
  ~MrsvrMrtsCon();
  int         connect();
  int         disconnect();
  void        process();
  void        quitProcess();

  inline void setHostname(const char* name) 
    { strcpy(mrtsHostname, name); };
  inline void setPortNo(int no) { mrtsPort = no; };
  inline void setInterval(long intv) {interval = intv; };
  int         getStatus();
  const char* getStatusStr();

};


#endif // _INC_MRSVR_MRTS_CON


