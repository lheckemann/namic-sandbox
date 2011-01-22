//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrRAS.h,v $
// $Revision: 1.1.1.1 $ 
// $Author: junichi $
// $Date: 2005/01/11 11:38:33 $
//====================================================================


//====================================================================
// Description:
//    Class to represent locator position and orientation in RAS 
//    coodinate. 
//====================================================================


#ifndef _INC_MRSVR_RAS
#define _INC_MRSVR_RAS

#ifdef DEBUG
#define DEBUG_MRSVR_RAS
#endif  //DEBUG

#include "MrsvrSharedData.h"

typedef struct {
  float             nr;
  float             na;
  float             ns;
  float             tr;
  float             ta;
  float             ts;
  float             pr;
  float             pa;
  float             ps;
  unsigned int      upCnt;
  int               lock;
} MrsvrRASPos;


// Size of shared memory area for the parameters.
#define SHARED_MEMORY_SIZE_RAS   (int)(sizeof(MrsvrRASPos))

class MrsvrRAS : public MrsvrSharedData {

 protected:
  // position parameters
  MrsvrRASPos*    pos;

 public:
  MrsvrRAS(key_t);
  ~MrsvrRAS();

};


class MrsvrRASReader : public MrsvrRAS {
  
 private:
  unsigned int prevCnt;

 public:

  inline float  getNR()     { return pos->nr; };
  inline float  getNA()     { return pos->na; };
  inline float  getNS()     { return pos->ns; };
  inline float  getTR()     { return pos->tr; };
  inline float  getTA()     { return pos->ta; };
  inline float  getTS()     { return pos->ts; };
  inline float  getPR()     { return pos->pr; };
  inline float  getPA()     { return pos->pa; };
  inline float  getPS()     { return pos->ps; };
  inline float* getMatrix() { return (float*)sharedMemory; };
  inline int    isUpdate() { 
    if (pos->upCnt > prevCnt || pos->upCnt == 0) {
      prevCnt = pos->upCnt;
      return 1;
    } else {
      return 0;
    }
  }
        
 public:
  MrsvrRASReader(key_t);
  ~MrsvrRASReader();

};



class MrsvrRASWriter : public MrsvrRASReader {

 public:
  inline void setNR(float r) { pos->nr = r; };
  inline void setNA(float a) { pos->na = a; };
  inline void setNS(float s) { pos->ns = s; };
  inline void setTR(float r) { pos->tr = r; };
  inline void setTA(float a) { pos->ta = a; };
  inline void setTS(float s) { pos->ts = s; };
  inline void setPR(float r) { pos->pr = r; };
  inline void setPA(float a) { pos->pa = a; };
  inline void setPS(float s) { pos->ps = s; };
  inline void setUpdateFlag() { pos->upCnt ++; };
  //inline void setMatrix() { eturn (int*)sharedMemory; };
  
 public:
  MrsvrRASWriter(key_t);
  ~MrsvrRASWriter();

};


#endif // _INC_MRSVR_RAS
