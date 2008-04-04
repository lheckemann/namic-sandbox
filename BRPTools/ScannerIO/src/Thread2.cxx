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
#include "Thread2.h"

#include "igtlConditionVariable.h"

using namespace std;

// Constructor:
Thread2::Thread2()
//: mtxStatus(), mtxTriggerCnt(), trigger(mtxTriggerCnt)
{
  //pthread_mutex_init(&mtxStatus, NULL);

  ResetTriggerCounter();
  this->runStatus   = Thread2::STOP;
  this->triggerMode = Thread2::NORMAL;
  this->fTrigger    = 0;
  this->queue = NULL;

  this->ThreadID = -1;
  this->m_Thread = igtl::MultiThreader::New();
  this->MutexStatus = igtl::MutexLock::New();
  this->MutexTriggerCnt = igtl::MutexLock::New();
  this->SMutexTrigger = igtl::SimpleMutexLock::New();
  this->Trigger = igtl::ConditionVariable::New();

  //InitFifo(256);  // -> move to run()
}


Thread2::~Thread2()
{
  //mtxStatus.acquire();
  this->MutexStatus->Lock();
  if (runStatus == Thread2::RUN ||
      runStatus == Thread2::SLEEP ||
      runStatus == Thread2::RUN) {
    //ACE_Thread::cancel((ACE_thread_t)thread);
    if (this->ThreadID > 0)
      {
        this->m_Thread->TerminateThread(this->ThreadID);  /// Does it work???
        this->ThreadID = -1;
      }
  }
#ifdef _DEBUG_THREAD
  else {
    cerr << "Thread2::~Thread2(): ERROR: Cannot cancel thread." << endl;
  }
#endif // _DEBUG_THREAD
  //mtxStatus.release();
  this->MutexStatus->Unlock();

}


void* Thread2::CallProcess(void* ptr)
{
  //Thread2* pRT = reinterpret_cast<Thread2*>(ptr);
  igtl::MultiThreader::ThreadInfo* vinfo = 
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  Thread2* pRT = static_cast<Thread2*>(vinfo->UserData);

  pRT->Process();
  pRT->runStatus = Thread2::STOP;
  return (void*) 0;
}


void Thread2::Exit()
{
  if (this->runStatus == Thread2::RUN) {
    //mtxStatus.acquire();
    this->MutexStatus->Lock();
    this->runStatus = Thread2::STOP;
    //mtxStatus.release();
    this->MutexStatus->Unlock();

    //ACE_Thread::exit(NULL);
    if (this->ThreadID >= 0)
      {
        this->m_Thread->TerminateThread(this->ThreadID);  /// Does it work???
        this->ThreadID = -1;
      }
    
  }
}


int Thread2::Run()
  // returns run status. (returns RUN if succeed)
{
  if (runStatus != Thread2::RUN && runStatus != Thread2::WAIT) {
    //mtxStatus.acquire();
    this->MutexStatus->Lock();
    
    // initialize trigger
    DeleteFifo();
    InitFifo(256);

    this->runStatus = Thread2::RUN;
    /*
    ACE_Thread::spawn((ACE_THR_FUNC)CallProcess, 
                      (void*)this, 
                      THR_NEW_LWP | THR_JOINABLE,
                      &thread, &hthread);
    */
    this->ThreadID = this->m_Thread->SpawnThread(Thread2::CallProcess, this);

#ifdef _DEBUG_THREAD
    cerr << "Thread2::run(): the thread is detached." << endl;
#endif
    //mtxStatus.release();
    this->MutexStatus->Unlock();
    //trigger.signal();
    this->Trigger->Signal();

  }
  return runStatus;
}


int Thread2::Stop()
  // returns run status. (returns STOP if succeed)
{
  if (runStatus == Thread2::RUN||runStatus == Thread2::WAIT) {

    //mtxStatus.acquire();    
    this->MutexStatus->Lock();
    this->runStatus = Thread2::STOP;
    //ACE_Thread::cancel(thread);
    if (this->ThreadID >= 0)
      {
        this->m_Thread->TerminateThread(this->ThreadID);  /// Does it work???
        this->ThreadID = -1;
      }

    //mtxStatus.release();
    this->MutexStatus->Unlock();
    
#ifdef _DEBUG_THREAD
    cerr << "Thread2::stop(): the thread is stopped" << endl;
#endif


  }
  return runStatus;
}


void Thread2::InitFifo(int n)
{
  nQueue = n;
  queue = new void*[nQueue];
  pStart = pEnd = 0;
}

void Thread2::DeleteFifo()
{
  if (queue) {
    delete [] queue;
  }
}


int Thread2::Push(void* ptr)
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


void* Thread2::Pull()
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


int Thread2::Sleep()
{
  //mtxStatus.acquire();
  this->MutexStatus->Lock();
  //ACE_Thread::kill(thread, SIGSTOP);
  runStatus = Thread2::SLEEP;
  //mtxStatus.release();
  this->MutexStatus->Unlock();
  return 0;
}

int Thread2::Resume()
{
  //mtxStatus.acquire();
  this->MutexStatus->Lock();
  //ACE_Thread::kill(thread, SIGCONT);  
  runStatus = Thread2::RUN;
  //mtxStatus.release();
  this->MutexStatus->Unlock();
  return 0;
}

void Thread2::PullTrigger()
{
  if (triggerMode != Thread2::COUNT_WITH_ARG &&
      (runStatus == Thread2::WAIT || runStatus == Thread2::RUN)) {
    //mtxTriggerCnt.acquire();
    this->MutexTriggerCnt->Lock();
    if (triggerMode == Thread2::COUNT) {
      triggerCnt ++;
    }
    //trigger.signal();
    this->Trigger->Signal();
    //mtxTriggerCnt.release();
    this->MutexTriggerCnt->Unlock();
  }
}


int Thread2::PullTrigger(void* ptr)
{
  int r = 0;
  if (triggerMode == Thread2::COUNT_WITH_ARG &&
      (runStatus == Thread2::WAIT || runStatus == Thread2::RUN)) {
    //mtxTriggerCnt.acquire();
    cerr << "int Thread2::PullTrigger(void* ptr): Signal" << endl;
    this->MutexTriggerCnt->Lock();
    if (Push(ptr)) {
      triggerCnt ++;
      r = 1;
    }
    //trigger.signal();
    this->Trigger->Signal();
    //mtxTriggerCnt.release();
    this->MutexTriggerCnt->Unlock();
  }
  return r;
}


void* Thread2::WaitForTrigger()
{
  void* r = (void*) NULL;
#ifdef _DEBUG_THREAD
  cerr << "Thread2::WaitForTrigger" << endl;
#endif // _DEBUG_THREAD

  if (fTrigger) {
    //mtxTriggerCnt.acquire();
    if (triggerMode == NORMAL || triggerCnt == 0) {
      runStatus = Thread2::WAIT;
      //trigger.wait();
      cerr << "Thread2::WaitForTrigger: Wait" << endl;
      this->SMutexTrigger->Lock();
      this->Trigger->Wait(this->SMutexTrigger);
      this->SMutexTrigger->Unlock();
      runStatus = Thread2::RUN;
    }
    this->MutexTriggerCnt->Lock();
#ifdef _DEBUG_THREAD
    cerr << "Thread2::WaitForTrigger: trigger pulled." << endl;
#endif // _DEBUG_THREAD
    if (triggerMode == Thread2::COUNT && triggerCnt > 0) {
      triggerCnt --;
    } else if (triggerMode == Thread2::COUNT_WITH_ARG 
               && triggerCnt > 0) {
      triggerCnt --;
      r = Pull();
    } else {
    }
    //mtxTriggerCnt.release();
    this->MutexTriggerCnt->Unlock();
  }
  return r;
}
