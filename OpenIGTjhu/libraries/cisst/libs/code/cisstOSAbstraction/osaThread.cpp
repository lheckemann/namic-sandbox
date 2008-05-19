/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaThread.cpp,v 1.5 2006/03/18 04:47:13 anton Exp $

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


#include <cisstOSAbstraction/osaThread.h>


#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
void osaThread::Delete(void) {
    pthread_cancel (*Thread);
    // wait for thread to terminate
    pthread_join(*Thread, NULL);
}
#endif // CISST_LINUX_RTAI || CISST_LINUX

#if (CISST_OS == CISST_WINDOWS)
void osaThread::Delete(void) {
    TerminateThread(Thread, 0);
    // wait for thread to terminate
    WaitForSingleObject(Thread, INFINITE);
}
#endif // CISST_WINDOWS



#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX)
void osaThread::Yield2(void) {
#if (CISST_COMPILER == CISST_GCC)
    pthread_yield();
#endif // CISST_GCC
}
#endif // CISST_LINUX_RTAI || CISST_LINUX

#if (CISST_OS == CISST_WINDOWS)
void osaThread::Yield2(void) {
    SwitchToThread();
}
#endif // CISST_WINDOWS



#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
void osaThread::Sleep(int time) {
    struct timespec timeval;
    timeval.tv_sec = 0;
    timeval.tv_nsec = time;
    pthread_mutex_lock(&Mutex);
    pthread_cond_timedwait(&Condition, &Mutex, &timeval);
    pthread_mutex_unlock(&Mutex);
}
#endif // CISST_LINUX_RTAI || CISST_LINUX

#if (CISST_OS == CISST_WINDOWS)
void osaThread::Sleep(int time) {
    // Sleep(time);
    LARGE_INTEGER dueTime;
    dueTime.QuadPart = -time/100; //dueTime is in 100ns
    WaitTimer = CreateWaitableTimer(NULL, true, Name);
    SetWaitableTimer(WaitTimer, &dueTime, 0, NULL, NULL, 0);
    WaitForSingleObject(WaitTimer, INFINITE);
    CloseHandle(WaitTimer);
}
#endif // CISST_WINDOWS



#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
PriorityType osaThread::GetPriority(void) {
    return Priority;
}
#endif // CISST_LINUX_RTAI || CISST_LINUX

#if (CISST_OS == CISST_WINDOWS)
PriorityType osaThread::GetPriority(void) {
    return Priority;
}
#endif // CISST_WINDOWS



#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
void osaThread::SetPriority(PriorityType priority) {
    struct sched_param param;
    param.sched_priority = priority;
    pthread_setschedparam(*Thread, Policy, &param);
    Priority = priority;
}
#endif // CISST_LINUX_RTAI || CISST_LINUX

#if (CISST_OS == CISST_WINDOWS)
void osaThread::SetPriority(PriorityType priority) {
}
#endif // CISST_WINDOWS



#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
SchedulingPolicyType osaThread::GetSchedulingPolicy(void) {
    return Policy;
}
#endif // CISST_LINUX_RTAI || CISST_LINUX

#if (CISST_OS == CISST_WINDOWS)
SchedulingPolicyType osaThread::GetSchedulingPolicy(void) {
    return Policy;
}
#endif // CISST_WINDOWS



#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
void osaThread::SetSchedulingPolicy(SchedulingPolicyType policy) {
    struct sched_param param;
    param.sched_priority = Priority;
    pthread_setschedparam(*Thread, policy, &param);
    Policy = policy;
}
#endif // CISST_LINUX_RTAI || CISST_LINUX

#if (CISST_OS == CISST_WINDOWS)
void osaThread::SetSchedulingPolicy(SchedulingPolicyType policy) {
}
#endif // CISST_WINDOWS



#if (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
void osaThread::Wait(void) {
    pthread_join(*Thread, NULL);
}
#endif // CISST_LINUX_RTAI || CISST_LINUX

#if (CISST_OS == CISST_WINDOWS)
void osaThread::Wait(void) {
    WaitForSingleObject(Thread, INFINITE);
}
#endif // CISST_WINDOWS


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: osaThread.cpp,v $
// Revision 1.5  2006/03/18 04:47:13  anton
// cisstOSAbstraction: Added license, replace tabs by spaces.
//
// Revision 1.4  2005/11/24 02:32:38  anton
// cisstOSAbstraction: Added compilation flags for Darwin (see #194).
//
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2004/05/28 21:50:21  anton
// Made modifications so that cisstOSAbstraction can compile on Linux with
// gcc and icc.  ThreadBuddy, Mailboxes and Semaphores are still missing.
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.2  2004/04/08 06:07:54  kapoor
// Added win32 implementation
//
// Revision 1.1  2004/03/04 21:27:31  kapoor
// made created templated instead of the whole class.
//
// ****************************************************************************
