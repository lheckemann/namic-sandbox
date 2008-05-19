/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaThread.h,v 1.7 2006/03/18 04:47:13 anton Exp $

  Author(s): Ankur Kapoor
  Created on: 2004-04-30

--- begin cisst license - do not edit ---

CISST Software License Agreement(c)

Copyright 2005 Johns Hopkins University (JHU) All Rights Reserved.

This software ("CISST Software") is provided by The Johns Hopkins
University on behalf of the copyright holders and
contributors. Permission is hereby granted, without payment, to copy,
modify, display and distribute this software and its documentation, if
any, for research purposes only, provided that (1) the above copyright
notice and the following four paragraphs appear on all copies of this
software, and (2) that source code to any modifications to this
software be made publicly available under terms no more restrictive
than those in this License Agreement. Use of this software constitutes
acceptance of these terms and conditions.

The CISST Software has not been reviewed or approved by the Food and
Drug Administration, and is for non-clinical, IRB-approved Research
Use Only.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY
EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of osaThread
  \ingroup cisstOSAbstraction
 */

#ifndef _osaThread_h
#define _osaThread_h

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaThreadAdapter.h>
#include <cisstOSAbstraction/osaExport.h>

#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
#include <pthread.h>
#endif // CISST_LINUX_RTAI || CISST_LINUX

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#define SCHED_FIFO 0 /*! No Scheduling Policy available in windows */
#endif

/*!
  \brief PriorityType and SchedulingPolicyType.

  For now these are just typedefs to integers. I would like to have a
  common return value for all OS. Maybe an enum, but the problem is
  that the granularity in any linux flavor is much much larger than
  windows.
*/
typedef int PriorityType;


/*!
  \brief SchedulingPolicyType.
  For now SchedulingPolicy is typedef'ed to int
  */
typedef int SchedulingPolicyType;


/*!
  \brief Define a thread object
 
  \ingroup cisstOSAbstraction
  
  The thread class -- ``just-a-wrapper-class'' to make the code look
  cleaner.  For now, wraps functions such as CreateThread,
  DeleteThread, GetThreadPriority, SetThreadPriority and so on. It
  might be a good idea to add functionality such as being able to
  refer to a thread by name, have periodic threads (looking into it)
  etc.  Peter in his mail had suggested we associate some state to the
  task, I like the idea, since I was anyway thinking adding more info
  to the thread than what the OS primitive provides, leading to an
  "intelligent" thread object with memory (noun form of memorize).
  Maybe this doesnt remain a just-a-wrapper-class after all. In that
  case I suggest calling it osaTask class, to distinguish it from a
  Thread since we a moving away from a thread primitive.  This also
  brings us to the point where we should decide - should we be adding
  inter-task communication primitives write here in osaTask class? My
  answer is NO. That might make things more complex. Which brings up
  another idea, to seperate out osaTask (osaLoop) and osaThread.
  osaTask would have a state, communcation channel(s) etc, everything
  needed to setup some sort of periodic control loop (servo,
  trajectory, data gathering etc). This would make things easier for
  people in case people want just an abstraction for threads and dont
  want periodicity, state etc to keep the overheads to a minimum in
  case there are any.
 */
class CISST_EXPORT osaThread {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
    /*! Pointer to OS dependent thread. */
    pthread_t *Thread;
    
    /*! The OS dependent Mutex. Not used current, but plans are to use
      it to syncronize starting of threads under Manager's control */
    pthread_mutex_t Mutex;

    /*! The OS dependent Condition variable. Not used current, but
      plans are to use it to syncronize starting of threads under
      Manager's control */
    pthread_cond_t Condition;
#endif // CISST_LINUX_RTAI || CISST_LINUX

#if (CISST_OS == CISST_WINDOWS)
    HANDLE Thread;
    HANDLE WaitTimer;
#endif // CISST_WINDOWS

    /*! The name associated with the thread. */
    char Name[6];


    /*! The OS *indepdent* Scheduling Policy for the thread. For now
       it is typedef'ed to int */
    SchedulingPolicyType Policy;


    /*! The OS *indepdent* Priority for the thread. For now it is
       typedef'ed to int */
    PriorityType Priority;


public:

    /*! Default constructor.  Does nothing. */
    osaThread() {};


    /*! Default distructor.  Does nothing. */
    ~osaThread() {};


    /*! Creates a new thread. */
    template<class _entryType, class _userDataType>
    void Create(_entryType* obj, void * (_entryType::*threadStart)(_userDataType),
                _userDataType userData = _userDataType(),
                const char *name = NULL, int priority = 0, int policy = SCHED_FIFO) {
#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
        typedef osaHeapCallBack<_entryType, _userDataType, void *, void *> adapter_t;
        unsigned int i;
        pthread_attr_t new_attr;
        Thread = new pthread_t;
        pthread_mutex_init(&Mutex, NULL);
        pthread_cond_init(&Condition, NULL);
        pthread_attr_init(&new_attr);
        /*
          pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);
          pthread_attr_setschedpolicy(&new_attr, policy);
          new_param.sched_priority = priority;
          pthread_attr_setschedparam(&new_attr, &new_param);
        */
        pthread_create(Thread, &new_attr, adapter_t::CallbackAndDestroy, (void*)adapter_t::Create(obj, threadStart, userData));
        if (name) {
            for (i=0; i<6; i++) Name[i] = name[i];
        }
        Priority = priority;
        Policy = policy;
#endif // CISST_LINUX_RTAI || CISST_LINUX

#if (CISST_OS == CISST_WINDOWS)
        typedef osaHeapCallBack<_entryType, _userDataType, void *, LPVOID> adapter_t;
        DWORD dwDummy;
        unsigned int i = 0;
        Thread = CreateThread(
                              NULL,                            //default security attributes
                              0,                                //use default stack size
                              adapter_t::CallbackAndDestroy,    //thread 'start routine'
                              (LPVOID) adapter_t::Create( obj, threadStart, userData ),        //argument to 'start routine'
                              0,                                //default creation flags
                              &dwDummy                        //dummy argument for thread id - we dont care
                              );
        if (name) {
            for (i=0; i<6 && name[i]!='\0'; i++) {
                Name[i]=name[i];
            }
        } else {
            for (i=0; i<6; i++) {
                Name[i] = 'X';
            }
        }
        Priority = priority;
        Policy = policy;
#endif // CISST_WINDOWS
    }


    /*! Deletes a thread.  Deletes (removes from memory) everything
      associated with a thread, e.g. stack, local data, static
      variables everything.
    */
    void Delete(void);


    /*! Yield the current task.  Stops the current task and takes it
      at the end of the list of ready tasks having its same
      priority. The scheduler makes the next ready task of the same
      priority active.
    */
    void Yield2(void);


    /*! Delay/suspend execution for a while.  Suspends execution of
      the caller task for a time of delay internal count units. During
      this time the CPU is used by other tasks.
     
      \param time The amount of time the task is suspended (unit to be
             fixed later).
    */
    void Sleep(int time);


    /*! Wait for the thread to exit. */
    void Wait(void);

    
    /*! Get the OS *independent* priority of the thread. */
    PriorityType GetPriority(void);


    /*! Set the OS *independent* priority of the thread. */
    void SetPriority(PriorityType priority);
    

    /*! Get the OS *independent* scheduling policy of the thread. Will not
     be available on all platforms or will return a constant. */
    SchedulingPolicyType GetSchedulingPolicy(void);

    /*! Set the OS *independent* scheduling policy of the thread. Will not
     be available on all platforms. */
    void SetSchedulingPolicy(SchedulingPolicyType policy);
};
/*
#if (CISST_OS == CISST_LINUX_RTAI)
template <class O, class U>
void osaThread::Create(O* obj, void * (O::*threadStart)(U), U userData, const char *name, int priority, int policy) {
    typedef osaHeapCallBack<O, U, void *, void *> adapter_t;
    int i;
    pthread_attr_t new_attr;
    pThread = new pthread_t;
    pthread_mutex_init(&Mutex, NULL);
    pthread_cond_init(&Condition, NULL);
    pthread_attr_init(&new_attr);
///
    pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setschedpolicy(&new_attr, policy);
    new_param.sched_priority = priority;
    pthread_attr_setschedparam(&new_attr, &new_param);
///
    pthread_create(pThread, &new_attr, adapter_t::CallbackAndDestroy, (void*)adapter_t::Create(obj, threadStart, userData));
    if (name) {for(i=0;i<6;i++) Name[i]=name[i];}
    Priority = priority;
    Policy = policy;
}
#endif // CISST_LINUX_RTAI

#if (CISST_OS == CISST_WINDOWS)
template<class _entryType, class _userDataType>
void osaThread::Create(_entryType* obj, void * (_entryType::*threadStart)(_userDataType), _userDataType userData, const char *name, int priority, int policy) {
    typedef osaHeapCallBack<_entryType, _userDataType, DWORD, LPVOID> adapter_t;
    DWORD dwDummy;
    pThread = CreateThread(
        NULL,                            //default security attributes
        0,                                //use default stack size
        adapter_t::CallbackAndDestroy,    //thread 'start routine'
        (LPVOID) adapter_t::Create( obj, ThreadStart, UserData ),        //argument to 'start routine'
        0,                                //default creation flags
        &dwDummy                        //dummy argument for thread id - we dont care
        );
    if (name) {for(i=0;i<6;i++) Name[i]=name[i];}
    Priority = priority;
    Policy = policy;
}
#endif // CISST_WINDOWS
*/


#endif // _osaThread_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: osaThread.h,v $
// Revision 1.7  2006/03/18 04:47:13  anton
// cisstOSAbstraction: Added license, replace tabs by spaces.
//
// Revision 1.6  2005/11/24 02:32:38  anton
// cisstOSAbstraction: Added compilation flags for Darwin (see #194).
//
// Revision 1.5  2005/10/07 02:25:44  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.4  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/01/10 19:21:53  anton
// cisstOSAbastraction: Added osaExport.h and required CISST_EXPORT to compile
// as a DLL (for windows).
//
// Revision 1.2  2004/05/28 21:50:21  anton
// Made modifications so that cisstOSAbstraction can compile on Linux with
// gcc and icc.  ThreadBuddy, Mailboxes and Semaphores are still missing.
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
// Revision 1.6  2004/04/08 05:59:03  kapoor
// Added win32 implementation
//
// Revision 1.5  2004/03/24 20:47:56  kapoor
// Added lots of documentation.
//
// Revision 1.4  2004/03/05 17:40:39  kapoor
// Removed MakePeriodic, IsPeriodic, MakeNonPeriodic methods. This functionality is now provided by rtsTask.
//
// Revision 1.3  2004/03/05 05:04:20  kapoor
// Minor changes. Removed commented out code.
//
// Revision 1.2  2004/03/04 21:26:14  kapoor
// more changes.
//
// Revision 1.1.1.1  2004/03/02 04:05:03  kapoor
// Importing source into local tree
//
// ****************************************************************************
