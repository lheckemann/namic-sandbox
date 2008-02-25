/*=========================================================================

  Program:   Imaging Scanner Interface
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Brigham and Women's Hospital. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <stdio.h>
#include "thread.h"

using namespace std;

// Constructor:
Thread::Thread()
  : mtxStatus(), mtxTriggerCnt(), trigger(mtxTriggerCnt)
{
  //pthread_mutex_init(&mtxStatus, NULL);

  ResetTriggerCounter();
  runStatus   = Thread::STOP;
  triggerMode = Thread::NORMAL;
  fTrigger    = 0;

  queue = NULL;
  //InitFifo(256);  // -> move to run()
}


Thread::~Thread()
{
  //pthread_mutex_lock(&mtxStatus);
  mtxStatus.acquire();
  if (runStatus == Thread::RUN ||
      runStatus == Thread::SLEEP ||
      runStatus == Thread::RUN) {
    //pthread_cancel(thread);
    ACE_Thread::cancel((ACE_thread_t)thread);
  }
#ifdef _DEBUG_THREAD
  else {
    cerr << "Thread::~Thread(): ERROR: Cannot cancel thread." << endl;
  }
#endif // _DEBUG_THREAD
  //pthread_mutex_unlock(&mtxStatus);
  mtxStatus.release();
}


void* Thread::CallProcess(void* ptr)
{
  Thread* pRT = reinterpret_cast<Thread*>(ptr);
  pRT->Process();
  pRT->runStatus = Thread::STOP;
  return (void*) 0;
}


void Thread::Exit()
{
  if (this->runStatus == Thread::RUN) {
    //pthread_mutex_lock(&mtxStatus);
    mtxStatus.acquire();
    this->runStatus = Thread::STOP;
    //pthread_mutex_unlock(&mtxStatus);
    mtxStatus.release();

    //pthread_mutex_destroy(&mtxTriggerCnt);
    //pthread_cond_destroy(&trigger);

    //pthread_exit(NULL);
    ACE_Thread::exit(NULL);
  }
}


int Thread::Run()
  // returns run status. (returns RUN if succeed)
{
  if (runStatus != Thread::RUN && runStatus != Thread::WAIT) {
    //pthread_mutex_lock(&mtxStatus);
    mtxStatus.acquire();

    // initialize trigger
    DeleteFifo();
    InitFifo(256);

    //pthread_cond_init(&trigger, NULL);
    //pthread_mutex_init(&mtxTriggerCnt, NULL);
    //mtxTriggerCnt = new ACE_Thread_Mutex;
    //trigger = new ACE_Thread_Condition<ACE_Thread_Mutex*>(mtxTriggerCnt);

    this->runStatus = Thread::RUN;
    //pthread_create(&thread, NULL, CallProcess, (void*) this);
    //pthread_detach(thread);
    ACE_Thread::spawn((ACE_THR_FUNC)CallProcess, 
                      (void*)this, 
                      THR_NEW_LWP | THR_JOINABLE,
                      &thread, &hthread);

#ifdef _DEBUG_THREAD
    cerr << "Thread::run(): the thread is detached." << endl;
#endif
    //pthread_mutex_unlock(&mtxStatus);
    mtxStatus.release();
    //pthread_cond_signal(&trigger);
    trigger.signal();

  }
  return runStatus;
}


int Thread::Stop()
  // returns run status. (returns STOP if succeed)
{
  if (runStatus == Thread::RUN||runStatus == Thread::WAIT) {
    //pthread_mutex_lock(&mtxStatus);
    mtxStatus.acquire();    
    this->runStatus = Thread::STOP;
    //pthread_cancel(thread);
    ACE_Thread::cancel(thread);

    //pthread_mutex_unlock(&mtxTriggerCnt); <- not work since the mutex is ownd by other thread

    //pthread_mutex_destroy(&mtxTriggerCnt);
    //pthread_cond_destroy(&trigger);
    //pthread_mutex_unlock(&mtxStatus);
    mtxStatus.release();
    
#ifdef _DEBUG_THREAD
    cerr << "Thread::stop(): the thread is stopped" << endl;
#endif


  }
  return runStatus;
}


void Thread::InitFifo(int n)
{
  nQueue = n;
  queue = new void*[nQueue];
  pStart = pEnd = 0;
}

void Thread::DeleteFifo()
{
  if (queue) {
    delete [] queue;
  }
}


int Thread::Push(void* ptr)
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


void* Thread::Pull()
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




int Thread::Sleep()
{
  //pthread_mutex_lock(&mtxStatus);
  mtxStatus.acquire();
  //pthread_kill(thread, SIGSTOP);
  ACE_Thread::kill(thread, SIGSTOP);
  runStatus = Thread::SLEEP;
  //pthread_mutex_unlock(&mtxStatus);
  mtxStatus.release();
  return 0;
}


int Thread::Resume()
{
  //pthread_mutex_lock(&mtxStatus);
  mtxStatus.acquire();
  //pthread_kill(thread, SIGCONT);
  ACE_Thread::kill(thread, SIGCONT);  
  runStatus = Thread::RUN;
  //pthread_mutex_lock(&mtxStatus);
  mtxStatus.release();
  return 0;
}


void Thread::PullTrigger()
{
  if (triggerMode != Thread::COUNT_WITH_ARG &&
      (runStatus == Thread::WAIT || runStatus == Thread::RUN)) {
    //pthread_mutex_lock(&mtxTriggerCnt);
    mtxTriggerCnt.acquire();
    if (triggerMode == Thread::COUNT) {
      triggerCnt ++;
    }
    //pthread_cond_signal(&trigger);
    trigger.signal();
    //pthread_mutex_unlock(&mtxTriggerCnt);
    mtxTriggerCnt.release();
  }
}


int Thread::PullTrigger(void* ptr)
{
  int r = 0;
  if (triggerMode == Thread::COUNT_WITH_ARG &&
      (runStatus == Thread::WAIT || runStatus == Thread::RUN)) {
    //pthread_mutex_lock(&mtxTriggerCnt);
    mtxTriggerCnt.acquire();
    if (Push(ptr)) {
      triggerCnt ++;
      r = 1;
    }
    //pthread_cond_signal(&trigger);
    trigger.signal();
    //pthread_mutex_unlock(&mtxTriggerCnt);
    mtxTriggerCnt.release();
  }
  return r;
}


void* Thread::WaitForTrigger()
{
  void* r = (void*) NULL;
#ifdef _DEBUG_THREAD
  cerr << "Thread::WaitForTrigger" << endl;
#endif // _DEBUG_THREAD

  if (fTrigger) {
    //pthread_mutex_lock(&mtxTriggerCnt);
    mtxTriggerCnt.acquire();
    if (triggerMode == NORMAL || triggerCnt == 0) {
      runStatus = Thread::WAIT;
      //pthread_cond_wait(&trigger, &mtxTriggerCnt);
      trigger.wait();
      runStatus = Thread::RUN;
    }
#ifdef _DEBUG_THREAD
    cerr << "Thread::WaitForTrigger: trigger pulled." << endl;
#endif // _DEBUG_THREAD
    if (triggerMode == Thread::COUNT && triggerCnt > 0) {
      triggerCnt --;
    } else if (triggerMode == Thread::COUNT_WITH_ARG 
               && triggerCnt > 0) {
      triggerCnt --;
      r = Pull();
    } else {
    }
    //pthread_mutex_unlock(&mtxTriggerCnt);
    mtxTriggerCnt.release();
  }
  return r;
}
