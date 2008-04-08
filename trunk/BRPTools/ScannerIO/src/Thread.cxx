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
#include "Thread.h"

#include "igtlConditionVariable.h"

using namespace std;

// Constructor:
Thread::Thread()
//: mtxStatus(), mtxTriggerCnt(), trigger(mtxTriggerCnt)
{
  //pthread_mutex_init(&mtxStatus, NULL);

  ResetTriggerCounter();
  this->runStatus   = Thread::STOP;
  this->triggerMode = Thread::NORMAL;
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


Thread::~Thread()
{
  //mtxStatus.acquire();
  this->MutexStatus->Lock();
  if (runStatus == Thread::RUN ||
      runStatus == Thread::SLEEP ||
      runStatus == Thread::RUN) {
    //ACE_Thread::cancel((ACE_thread_t)thread);
    if (this->ThreadID > 0)
      {
        this->m_Thread->TerminateThread(this->ThreadID);  /// Does it work???
        this->ThreadID = -1;
      }
  }
#ifdef _DEBUG_THREAD
  else {
    cerr << "Thread::~Thread(): ERROR: Cannot cancel thread." << endl;
  }
#endif // _DEBUG_THREAD
  //mtxStatus.release();
  this->MutexStatus->Unlock();

}


void* Thread::CallProcess(igtl::MultiThreader::ThreadInfo* vinfo)
{
  //Thread* pRT = reinterpret_cast<Thread*>(ptr);
  Thread* pRT = static_cast<Thread*>(vinfo->UserData);

  pRT->Process();
  pRT->runStatus = Thread::STOP;
  return (void*) 0;
}


void Thread::Exit()
{
  if (this->runStatus == Thread::RUN) {
    //mtxStatus.acquire();
    this->MutexStatus->Lock();
    this->runStatus = Thread::STOP;
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


int Thread::Run()
  // returns run status. (returns RUN if succeed)
{
  if (runStatus != Thread::RUN && runStatus != Thread::WAIT) {
    //mtxStatus.acquire();
    this->MutexStatus->Lock();
    
    // initialize trigger
    DeleteFifo();
    InitFifo(256);

    this->runStatus = Thread::RUN;
    /*
    ACE_Thread::spawn((ACE_THR_FUNC)CallProcess, 
                      (void*)this, 
                      THR_NEW_LWP | THR_JOINABLE,
                      &thread, &hthread);
    */
    this->ThreadID = this->m_Thread->SpawnThread((igtl::igtlThreadFunctionType)Thread::CallProcess, this);

#ifdef _DEBUG_THREAD
    cerr << "Thread::run(): the thread is detached." << endl;
#endif
    //mtxStatus.release();
    this->MutexStatus->Unlock();
    //trigger.signal();
    this->Trigger->Signal();

  }
  return runStatus;
}


int Thread::Stop()
  // returns run status. (returns STOP if succeed)
{
  if (runStatus == Thread::RUN||runStatus == Thread::WAIT) {

    //mtxStatus.acquire();    
    this->MutexStatus->Lock();
    this->runStatus = Thread::STOP;
    //ACE_Thread::cancel(thread);
    if (this->ThreadID >= 0)
      {
        this->m_Thread->TerminateThread(this->ThreadID);  /// Does it work???
        this->ThreadID = -1;
      }

    //mtxStatus.release();
    this->MutexStatus->Unlock();
    
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
  //mtxStatus.acquire();
  this->MutexStatus->Lock();
  //ACE_Thread::kill(thread, SIGSTOP);
  runStatus = Thread::SLEEP;
  //mtxStatus.release();
  this->MutexStatus->Unlock();
  return 0;
}

int Thread::Resume()
{
  //mtxStatus.acquire();
  this->MutexStatus->Lock();
  //ACE_Thread::kill(thread, SIGCONT);  
  runStatus = Thread::RUN;
  //mtxStatus.release();
  this->MutexStatus->Unlock();
  return 0;
}

void Thread::PullTrigger()
{
  if (triggerMode != Thread::COUNT_WITH_ARG &&
      (runStatus == Thread::WAIT || runStatus == Thread::RUN)) {
    //mtxTriggerCnt.acquire();
    this->MutexTriggerCnt->Lock();
    if (triggerMode == Thread::COUNT) {
      triggerCnt ++;
    }
    //trigger.signal();
    this->Trigger->Signal();
    //mtxTriggerCnt.release();
    this->MutexTriggerCnt->Unlock();
  }
}


int Thread::PullTrigger(void* ptr)
{
  int r = 0;
  if (triggerMode == Thread::COUNT_WITH_ARG &&
      (runStatus == Thread::WAIT || runStatus == Thread::RUN)) {
    //mtxTriggerCnt.acquire();
    cerr << "int Thread::PullTrigger(void* ptr): Signal" << endl;
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


void* Thread::WaitForTrigger()
{
  void* r = (void*) NULL;
#ifdef _DEBUG_THREAD
  cerr << "Thread::WaitForTrigger" << endl;
#endif // _DEBUG_THREAD

  if (fTrigger) {
    //mtxTriggerCnt.acquire();
    if (triggerMode == NORMAL || triggerCnt == 0) {
      runStatus = Thread::WAIT;
      //trigger.wait();
      cerr << "Thread::WaitForTrigger: Wait" << endl;
      this->SMutexTrigger->Lock();
      this->Trigger->Wait(this->SMutexTrigger);
      this->SMutexTrigger->Unlock();
      runStatus = Thread::RUN;
    }
    this->MutexTriggerCnt->Lock();
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
    //mtxTriggerCnt.release();
    this->MutexTriggerCnt->Unlock();
  }
  return r;
}
