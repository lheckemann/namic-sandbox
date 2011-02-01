//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrLog.cpp,v $
// $Revision: 1.3 $ 
// $Author: junichi $
// $Date: 2005/11/15 11:05:33 $
//====================================================================


//====================================================================
// Description:
//
//====================================================================


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "MrsvrLog.h"


MrsvrLog::MrsvrLog(key_t shmkey) :
  MrsvrSharedData(shmkey, SHARED_MEMORY_SIZE_LOG) 
{
  sharedLog = (MrsvrSharedLog*)sharedMemory;
}


MrsvrLog::~MrsvrLog() {
}



MrsvrLogReader::MrsvrLogReader(key_t shmkey) :
  MrsvrLog(shmkey)
{
  clearCount();
  
  lindex = 0;
  lpage  = 0;
}


MrsvrLogReader::~MrsvrLogReader()
{
}


int MrsvrLogReader::getLatestLogText(char* dest, int n)
//
//  This function substitute latest log text in shared log buffer to 'dest'
//  and returns number of charactors which have copied.
//
//  dest  :  Destination to copy log text.
//  n     :  Maximum number of charactors. (including '\0')
//
{
  int nread;
  int lti = sharedLog->logTextIndex;
  int ltp = sharedLog->logTextPage;

  // decrement maximum number of charctors to include '\0'
  n --;

  // first, calcurate number of charactors, we have to copy
  nread = (ltp - lpage)*SIZE_LOG_BUFFER + lti - lindex;
  if (nread < 0) {
    lpage = ltp;
    lindex = lti;
    dest[0] = '\0';
    return -1;
  }
  if (nread > SIZE_LOG_BUFFER) {
    lpage = ltp - 1;
    lindex = lti;
    nread = SIZE_LOG_BUFFER;
  }
  if (nread > n) {
    nread = n;
  }

  if (nread + lindex > SIZE_LOG_BUFFER) {
    int nf = nread + lindex - SIZE_LOG_BUFFER;
    memcpy(dest, &(sharedLog->logText[lindex]), (size_t) (nf)*sizeof(char));
    memcpy(&dest[nf], sharedLog->logText, (nread-nf)*sizeof(char));
  } else {
    memcpy(dest, &(sharedLog->logText[lindex]), (size_t) nread*sizeof(char));
  }
  dest[nread] = '\0';
  lpage = ltp;
  lindex = lti;

  return nread;
}


MrsvrLogWriter::MrsvrLogWriter(key_t shmkey):
 MrsvrLogReader(shmkey)
{
  for (int j = 0; j < NUM_ENCODERS; j ++) {
    for (int i = 0; i < NUM_LOG_BUFFER; i ++) {
      sharedLog->position[j][i] = 0.0;
    }
  }
  sharedLog->interval = 1;
  sharedLog->pos = 0;

  sharedLog->logTextPage = 0;
  sharedLog->logTextIndex = 0;
  sharedLog->logText[0] = '\0';
}


MrsvrLogWriter::~MrsvrLogWriter()
{
  
}


int MrsvrLogWriter::addLogTextConst(const char* str)
{
  int length;
  int lti = sharedLog->logTextIndex;
  //  int ltp = sharedLog->logTextPage;

  length = strlen(str);

  if (length > MAX_SINGLE_LOG_MESSAGE) {
    length = MAX_SINGLE_LOG_MESSAGE;
  }
  if (length+lti > SIZE_LOG_BUFFER) {
    memcpy(&(sharedLog->logText[lti]), str, 
           SIZE_LOG_BUFFER-lti);
    memcpy(sharedLog->logText, &str[SIZE_LOG_BUFFER-lti],
           (length - SIZE_LOG_BUFFER+lti));
    sharedLog->logTextPage ++;
    sharedLog->logTextIndex = length + lti - SIZE_LOG_BUFFER;
  } else {
    memcpy(&(sharedLog->logText[lti]), str, length);
    sharedLog->logTextIndex += length;
  }
  return length;
}


int MrsvrLogWriter::addLogText(const char* format, ...)
{
  va_list args;
  char    buf[MAX_SINGLE_LOG_MESSAGE+1];
  int     r;

  va_start(args, format);
  vsnprintf(buf, MAX_SINGLE_LOG_MESSAGE+1, format, args);

  r = addLogTextConst(buf);
  va_end(args);

  return r;
}
