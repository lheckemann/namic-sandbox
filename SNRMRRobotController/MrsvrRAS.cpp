//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrRAS.cpp,v $
// $Revision: 1.1.1.1 $ 
// $Author: junichi $
// $Date: 2005/01/11 11:38:33 $
//====================================================================

//====================================================================
// Description:
//
//====================================================================


#include "MrsvrRAS.h"

MrsvrRAS::MrsvrRAS(key_t shmkey) :
  MrsvrSharedData(shmkey, SHARED_MEMORY_SIZE_RAS) 
{
  pos  = (MrsvrRASPos*)sharedMemory;
}


MrsvrRAS::~MrsvrRAS()
{
}


MrsvrRASReader::MrsvrRASReader(key_t shmkey) :
  MrsvrRAS(shmkey)
{
  
}


MrsvrRASReader::~MrsvrRASReader()
{
}


MrsvrRASWriter::MrsvrRASWriter(key_t shmkey) :
  MrsvrRASReader(shmkey)
{
  setNR(0.0);
  setNA(0.0);
  setNS(0.0);
  setTR(0.0);
  setTA(0.0);
  setTS(0.0);
  setPR(0.0);
  setPA(0.0);
  setPS(0.0);
  pos->upCnt = 0;
}



MrsvrRASWriter::~MrsvrRASWriter()
{
}


