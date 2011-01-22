//====================================================================
//
// MRI guided robot control system
//
// Copyright (C) 2003-2005 by The University of Tokyo,
// All Right Reserved.
//
//====================================================================
// $RCSfile: MrsvrThread.cpp,v $
// $Revision: 1.4 $ 
// $Author: junichi $
// $Date: 2006/01/20 03:15:48 $
//====================================================================


//====================================================================
// Description: 
//    Thread base class for MRI guided robot control system.
//====================================================================


#include <stdio.h>
#include "MrsvrThread.h"

// Constructor:
MrsvrThread::MrsvrThread()
{
  pthread_mutex_init(&mtxStatus, NULL);
  /*
  pthread_mutex_init(&mtxTriggerCnt, NULL);
  pthread_cond_init(&trigger, NULL);
  */
  resetTriggerCounter();
  runStatus   = MrsvrThread::STOP;
  triggerMode = MrsvrThread::NORMAL;
  fTrigger    = 0;
  queue = NULL;
  //initFifo(256);  // number of queue elements is 256
}


MrsvrThread::~MrsvrThread()
{
  pthread_mutex_lock(&mtxStatus);
  if (runStatus == MrsvrThread::RUN || runStatus == MrsvrThread::SLEEP) {
    pthread_cancel(thread);
  }
#ifdef DEBUG
  else {
    printf("MrsvrThread::~MrsvrThread(): ERROR: Cannot cancel thread.\n");
  }
#endif // DEBUG  
  pthread_mutex_unlock(&mtxStatus);
}


void* MrsvrThread::callProcess(void* ptr)
{
  MrsvrThread* pRT = reinterpret_cast<MrsvrThread*>(ptr);
  pRT->process();
  pthread_mutex_lock(&pRT->mtxStatus);
  pRT->runStatus = MrsvrThread::STOP;
  pthread_mutex_unlock(&pRT->mtxStatus);
  return (void*) 0;
}


void MrsvrThread::threadExit()
{
  if (this->runStatus == MrsvrThread::RUN) {
    pthread_mutex_lock(&mtxStatus);
    this->runStatus = MrsvrThread::STOP;
    pthread_mutex_unlock(&mtxStatus);
    pthread_mutex_destroy(&mtxTriggerCnt);
    pthread_cond_destroy(&trigger);

    pthread_exit(NULL);
  }
}


void MrsvrThread::run()
{
  fprintf(stderr, "run();\n");

  if (this->runStatus != MrsvrThread::RUN &&
      this->runStatus != MrsvrThread::WAIT) {
    fprintf(stderr, "pthread_mutex_lock(&mtxStatus);\n");
    pthread_mutex_lock(&mtxStatus);

    deleteFifo(); 
    initFifo(256);  // number of queue elements is 256

    fprintf(stderr, "pthread_cond_init(&trigger, NULL);\n");
    pthread_cond_init(&trigger, NULL);

    fprintf(stderr, "pthread_mutex_init(&mtxTriggerCnt, NULL);\n");
    pthread_mutex_init(&mtxTriggerCnt, NULL);

    this->runStatus = MrsvrThread::RUN;
    fprintf(stderr, "pthread_cond_signal(&trigger);\n");
    pthread_cond_signal(&trigger);

    fprintf(stderr, "pthread_create(&thread, NULL, callProcess, (void*) this);\n");

    pthread_create(&thread, NULL, callProcess, (void*) this);
    pthread_mutex_unlock(&mtxStatus);
    pthread_detach(thread);

    fprintf(stderr, "MrsvrThread::run(): the thread is starting ... %d\n", this->runStatus);
  }
}


void MrsvrThread::stop()
{
  if (runStatus == MrsvrThread::RUN || runStatus == MrsvrThread::WAIT) {
    pthread_mutex_lock(&mtxStatus);
    this->runStatus = MrsvrThread::STOP;
    pthread_cancel(thread);
    //pthread_mutex_unlock(&mtxTriggerCnt); <- not work since the mutex is ownd by other thread
    pthread_mutex_destroy(&mtxTriggerCnt);
    pthread_cond_destroy(&trigger);
    pthread_mutex_unlock(&mtxStatus);
    fprintf(stderr, "MrsvrThread::stop(): the thread is stopped");
#ifdef _DEBUG_THREAD
    cerr << "MrsvrThread::stop(): the thread is stopped" << endl;
#endif
  }
  //return runStatus;
}


void MrsvrThread::initFifo(int n)
{
  nQueue = n;
  queue = new void*[nQueue];
  pStart = pEnd = 0;
}


void MrsvrThread::deleteFifo()
{
  if (queue) {
    delete [] queue;
  }
}


int MrsvrThread::push(void* ptr)
{
  int tmp;

  queue[pEnd] = ptr;
  tmp = (pEnd + 1) % nQueue;
  if (tmp == pStart) {
    return 0;
  } else {
    pEnd = tmp;
    return 1;
  }
}


void* MrsvrThread::pull()
{
  void* r;
  if (pStart == pEnd) {
    return NULL;
  } else {
    r = queue[pStart];
    pStart = (pStart + 1) % nQueue;
    return r;
  }
}


int MrsvrThread::sleepThread()
{
  pthread_mutex_lock(&mtxStatus);
#if defined(__CYGWIN__)
  pthread_suspend(thread);
#elif defined(__LINUX__)
  pthread_kill(thread, SIGSTOP);
#else  
  pthread_kill(thread, SIGSTOP);
  //pthread_suspend_np(thread);
#endif // __CYGWIN__
  runStatus = MrsvrThread::SLEEP;
  pthread_mutex_unlock(&mtxStatus);
  return 0;
}


int MrsvrThread::resumeThread()
{
  pthread_mutex_lock(&mtxStatus);
#if defined(__CYGWIN__)
  pthread_continue(thread);
#elif defined(__LINUX__)
  pthread_kill(thread, SIGCONT);
#else
  pthread_kill(thread, SIGCONT);
  //pthread_resume_np(thread);
#endif // __CYGWIN__
  runStatus = MrsvrThread::RUN;
  pthread_mutex_lock(&mtxStatus);
  return 0;
}


void MrsvrThread::pullTrigger()
{
  if (triggerMode != MrsvrThread::COUNT_WITH_ARG) {
    pthread_mutex_lock(&mtxTriggerCnt);
    if (triggerMode == MrsvrThread::COUNT) {
      triggerCnt ++;
    }
    pthread_cond_signal(&trigger);
    pthread_mutex_unlock(&mtxTriggerCnt);
  }
}


int MrsvrThread::pullTrigger(void* ptr)
{
  int r = 0;
  if (triggerMode == MrsvrThread::COUNT_WITH_ARG) {
    pthread_mutex_lock(&mtxTriggerCnt);
    if (push(ptr)) {
      triggerCnt ++;
      r = 1;
    }
    pthread_cond_signal(&trigger);
    pthread_mutex_unlock(&mtxTriggerCnt);
  }
  return r;
}


void* MrsvrThread::waitForTrigger()
{
  void* r = (void*) NULL;
#ifdef DEBUG
  printf("MrsvrThread::waitForTrigger \n");
#endif //DEBUG

  if (fTrigger) {
    pthread_mutex_lock(&mtxTriggerCnt);
    if (triggerMode == NORMAL || triggerCnt == 0) {
      runStatus = MrsvrThread::WAIT;
      pthread_cond_wait(&trigger, &mtxTriggerCnt);
      runStatus = MrsvrThread::RUN;
    }
#ifdef DEBUG
    printf("MrsvrThread::waitForTrigger: trigger pulled. \n");
#endif //DEBUG
    if (triggerMode == MrsvrThread::COUNT && triggerCnt > 0) {
      triggerCnt --;
    } else if (triggerMode == MrsvrThread::COUNT_WITH_ARG 
               && triggerCnt > 0) {
      triggerCnt --;
      r = pull();
    } else {
    }
    pthread_mutex_unlock(&mtxTriggerCnt);
  }
  return r;
}




