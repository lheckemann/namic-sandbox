//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrSharedData.cpp,v $
// $Revision: 1.1.1.1 $ 
// $Author: junichi $
// $Date: 2005/01/11 11:38:33 $
//====================================================================


//====================================================================
// Description:  
//    MrsvrSharedData class provides the data sharing between 
//    differrent proccesses on the same host.
//====================================================================


#include "MrsvrSharedData.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

MrsvrSharedData::MrsvrSharedData(key_t shmkey, int shmsize)
{
  int   perm;    // access permission flag

  perm = 0644 | IPC_CREAT;

  shmID = shmget((key_t)shmkey, shmsize, perm );
  if (shmID == -1) {
#ifdef DEBUG_MRSVR_SHARED_DATA    
    perror("shmget");    
    printf("MrsvrSharedData: shmget() failed.\n");
#endif
    return;
  }

  // attach shared memory to address space of the process.
  sharedMemory = shmat(shmID, (void*)0, 0);
  if (sharedMemory == (void*)-1) {
#ifdef DEBUG_MRSVR_SHARED_DATA    
    perror("shmat");
    printf("MrsvrSharedData: shmat() failed.\n");
#endif
    return;
  }

}


MrsvrSharedData::~MrsvrSharedData()
{
  // detach the shared memory
  if (shmdt((char*)sharedMemory) == -1) {
#ifdef DEBUG_MRSVR_SHARED_DATA    
    printf("MrsvrSharedData: shmdt() failed.\n");
#endif
    return;
  }
  if (shmctl(shmID, IPC_RMID, 0) == -1) {
#ifdef DEBUG_MRSVR_SHARED_DATA    
    printf("MrsvrSharedData: shmctl(IPC_RMID) failed.\n");
#endif
    return;
  }
}

