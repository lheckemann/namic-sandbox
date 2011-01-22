//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrSharedData.h,v $
// $Revision: 1.1.1.1 $ 
// $Author: junichi $
// $Date: 2005/01/11 11:38:33 $
//====================================================================


//====================================================================
// Description: 
//    MrsvrSharedData class provides the data sharing between 
//    differrent proccesses on the same host.
//====================================================================


#ifndef _INC_MRSVR_SHARED_DATA
#define _INC_MRSVR_SHARED_DATA

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#ifdef DEBUG
#define DEBUG_MRSVR_SHARED_DATA
#endif

class MrsvrSharedData {
 protected:
  int              shmID;          // shared memory id
  void*            sharedMemory;   // head address of shared memory area
  
 public:
  MrsvrSharedData(key_t, int);
  ~MrsvrSharedData();
};


#endif
