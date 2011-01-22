//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrLog.h,v $
// $Revision: 1.1.1.1 $ 
// $Author: junichi $
// $Date: 2005/01/11 11:38:33 $
//====================================================================


//====================================================================
// Description:
//
//====================================================================


#ifndef _INC_MRSVR_LOG
#define _INC_MRSVR_LOG

#include "MrsvrSharedData.h"
#include "MrsvrDev.h"

#define NUM_LOG_BUFFER    4096

#define SIZE_LOG_BUFFER               40960
#define MAX_SINGLE_LOG_MESSAGE        1024

typedef struct {
  long      interval;      // logging interval in microsecond
  int       pos;
  float     position[NUM_ENCODERS][NUM_LOG_BUFFER];
  float     setPoint[NUM_ENCODERS][NUM_LOG_BUFFER];
  //float     velocity[NUM_ENCODERS][NUM_LOG_BUFFER];

  //for log text
  int       logTextIndex;
  int       logTextPage;
  char      logText[SIZE_LOG_BUFFER];

} MrsvrSharedLog;

#define SHARED_MEMORY_SIZE_LOG    sizeof(MrsvrSharedLog)


class MrsvrLog : public MrsvrSharedData {
  
 protected:
  MrsvrSharedLog*   sharedLog;

 public:
  MrsvrLog(key_t);
  ~MrsvrLog();
  
};


class MrsvrLogReader : public MrsvrLog {

private:


  int  lindex;  // 'logTextIndex' at last time the function is called.
  int  lpage;   // 'logTextPage' at last time the function is called.
  int  readPos;

public:
  inline int    getInterval() { return sharedLog->interval;        };
  inline int    clearCount()  { return (readPos = sharedLog->pos); };
  inline int    next() { // return 0 if all data have been read.
    if (sharedLog->pos != readPos) {
      readPos = (readPos+1)%NUM_LOG_BUFFER;
      return 1;
    } else {
      return 0;
    }
  };

  inline  float getPosition(int enc)
    { return sharedLog->position[enc][readPos]; };
  inline  float getSetPoint(int enc)
    { return sharedLog->setPoint[enc][readPos]; };
  int           getLatestLogText(char*, int);

  MrsvrLogReader(key_t);
  ~MrsvrLogReader();
};


class MrsvrLogWriter : public MrsvrLogReader {

public:
  inline void setInterval(long t) { sharedLog->interval = t; };
  inline void addPosition(int enc, float val) 
    { sharedLog->position[enc][sharedLog->pos] = val; };
  inline void addSetPoint(int enc, float val) 
    { sharedLog->setPoint[enc][sharedLog->pos] = val; };
  //inline void addVelocity(int enc, float val)
  //{ sharedLog->setPoint[enc][pos] = val; };
  inline void next() 
    { sharedLog->pos = (sharedLog->pos+1) % NUM_LOG_BUFFER; };

  int         addLogTextConst(const char*);
  int         addLogText(const char*, ...);

  MrsvrLogWriter(key_t);
  ~MrsvrLogWriter();
};



#endif //_INC_MRSVR_LOG
