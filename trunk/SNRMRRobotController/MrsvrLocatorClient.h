//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrLocatorClient.h,v $
// $Revision: 1.5 $ 
// $Author: junichi $
// $Date: 2006/01/20 03:15:47 $
//====================================================================


//====================================================================
// Description: 
//    This module is a message interface for locator server.
//====================================================================


#ifndef _INC_MRSVR_LOCATOR_CLIENT
#define _INC_MRSVR_LOCATOR_CLIENT

#ifndef ENABLE_LOCATOR_CLIENT
  #define ENABLE_LOCATOR_CLIENT
#endif

#ifdef DEBUG
  #define _DEBUG_MRSVR_LOCATOR_CLIENT
#endif

#ifdef USE_ART
  #include <linux/art_task.h>
#endif

//#include   "MRTrackingClient.h"
#include   "MrsvrThread.h"
//#include "MrsvMessage.h"
#include   "shmKeys.h"
#include   "MrsvrRAS.h"

#include   "locinfop.h"

#include <stdio.h>


#define MAX_HOSTNAME              256
#define DEFAULT_LSERVER_PORT      7777
#define DEFAULT_LSERVER_HOST      "3.245.34.176"
#define DEFAULT_LSERVER_INTERVAL  10000   // (us)
#define DEFAULT_LSERVER_INTERVAL_MS (DEFAULT_LSERVER_INTERVAL/1000)

#define DEFAULT_TIMEOUT           5000   // (ms)
#define DEFAULT_POS_TIMEOUT       500    // (ms)

          
class MrsvrLocatorClient : public MrsvrThread {

//------------------------- Public constants -------------------------//
public:
  enum {
    SUCCESS,
    FAILURE,
  };
  enum {
    LOCATOR_DISCONNECTED,
    LOCATOR_CONNECTED,
  };
  enum {
    LOCATOR_SENSOR_OK,
    LOCATOR_SENSOR_BLOCKED,
  };

//------------------------ Private constants ------------------------//
private:
  //  static const char* statusStr[];

//------------------------ Private variables -------------------------//
private:
  struct sockaddr_in clientAddr;
  struct hostent*    serverHostent;
  struct in_addr*    serverIpAddr;
  int                clientLen;
  //  char   serverHostname[];
  int                serverPort;
  int                serverFd;

  short*             signalPoolRe;
  short*             signalPoolIm;
  int                signalRes;
  int                nlines;

  unsigned int       loopInterval;   //(ms)

  MrsvrThread*       eventOnSignalReceive;

  pthread_mutex_t    mtxLatestPos;
  float              latestPos[9];
  int                locStatus;

  FILE               *locfp;

  
//-------------------- Constructor & destructor ----------------------//
public:
  MrsvrLocatorClient();
  MrsvrLocatorClient(const char*, int);
  ~MrsvrLocatorClient();

//------------------------ Private functions -------------------------//  
private:
  int                getPos(int timeout=DEFAULT_POS_TIMEOUT);
  size_t             readn(void*, size_t);               
  size_t             readn(void*, size_t, int); // with timeout
  size_t             writen(const void*, size_t);
  int                sendMsg(LIPMSG);
  int                getMsg();
  int                getMsg(int);

  int                sendSize();
  int                sendResetCounter();

  int                closeConnection();

//------------------------- Public functions -------------------------//
public:
  void               setServer(const char*, int);
  void               setServer(const wchar_t*, int);
  int                connectToServer();
  int                disconnect();
  void               process();
  int                getLatestPos(float pos[9]);

  
  inline void        setInterval(unsigned int intv) {   /* in us */
    loopInterval = intv;
  };
  inline void        setIntervalMs(unsigned int intv) { /* in ms */
    loopInterval = intv*1000;
  };
  inline int         getStatus() {
    return (serverFd < 0)?LOCATOR_DISCONNECTED:LOCATOR_CONNECTED;
  };

  inline void        registerEventOnSignal(MrsvrThread& th) {
    eventOnSignalReceive = &th;
  };
  inline void        unregisterEventOnSignal() {
    eventOnSignalReceive = 0;
  };

  inline void        setSignalPool(short* spr, short* spi, 
                                   int sigres, int nl) {
    signalPoolRe = spr;
    signalPoolIm = spi;
    signalRes    = sigres;
    nlines       = nl;
  };


};


#endif // _INC_MRSVR_LOCATOR_CLIENT


