//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrThread.h,v $
// $Revision: 1.3 $ 
// $Author: junichi $
// $Date: 2006/01/20 03:15:48 $
//====================================================================


//====================================================================
// Description: 
//    Thread base class for MRI guided robot control system.
//====================================================================


#ifndef _INC_MRSVR_THREAD
#define _INC_MRSVR_THREAD

#ifdef DEBUG
#define DEBUG_MRSVR_THREAD
#endif //DEBUG


#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <signal.h>

#include <unistd.h>
#include <string.h>

#include <pthread.h>

class MrsvrThread {

 public:
  // thread status
  enum {  
    RUN,
    STOP,
    SLEEP,
    WAIT,
  };

 protected:
  // Trigger Mode
  enum { 
    NORMAL,
    COUNT,
    COUNT_WITH_ARG
  };

 protected:
  pthread_t       thread;
  int             runStatus;
  pthread_mutex_t mtxStatus;     // Critical section in status changinging.
  int             fTrigger;      // Use trigger or not.
  pthread_cond_t  trigger;       // Condition variable to keep waiting.
  pthread_mutex_t mtxTriggerCnt; // Mutex for trigger.
  int             triggerMode;   // Trigger mode. See enum declaration above.
  int             triggerCnt;    // Trigger counter
  void*           triggerArg;    // Argument on triggering

  virtual void    process() = 0;
  static void*    callProcess(void*);
  void            threadExit();  // called from thread itself

  void*           waitForTrigger();
  inline void     resetTriggerCounter()   { triggerCnt = 0; };
  inline void     setTriggerMode(int mode){ triggerMode = mode; };
  inline void     enableTrigger()         { fTrigger = 1; };
  inline void     disableTrigger()        { fTrigger = 0; };

 private:  // FIFO used in Trigger Counting with Argument Mode
  void**          queue;
  int             nQueue;
  int             pStart, pEnd;
  void            deleteFifo();
  void            initFifo(int);
  int             push(void*);
  void*           pull();

 public:
  MrsvrThread();
  virtual ~MrsvrThread();

  int             sleepThread();
  int             resumeThread();
  void            pullTrigger();
  int             pullTrigger(void*);
  inline int      getStatus() { return runStatus;};
  virtual void    run();
  virtual void    stop();

};

#endif //_INC_MRSVR_THREAD
