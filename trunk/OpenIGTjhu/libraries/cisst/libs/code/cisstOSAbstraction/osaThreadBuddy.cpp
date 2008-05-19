/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaThreadBuddy.cpp,v 1.11 2006/04/28 21:36:47 pkaz Exp $

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


#include <cisstOSAbstraction/osaThreadBuddy.h>
#include <cisstCommon/cmnLogger.h>

#if (CISST_OS == CISST_LINUX_RTAI)
#include <sys/mman.h> // for mlockall
#include <rtai_lxrt.h> // for RTAI specific calls
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
const char __lock_filepath[] = "/var/lock/subsys/rtai";
const char __lock_filename[] = "/var/lock/subsys/rtai/rtai.lock";
char __lock_file[256];
#endif


#if (CISST_OS == CISST_LINUX_RTAI)
void __os_init(void) {
    struct stat st, st_1;
    stat(__lock_filename, &st);
    if (st.st_nlink > 1) {
        std::cerr << "__os_init Info: Another real-time program is already running" << std::endl;
    } 

    // no prog runs, no harm to start again!
    rt_set_oneshot_mode();
    start_rt_timer(nano2count(1*1000*1000)); //the period is ignored, so it doesnt matter

    // obtain a lock
    sprintf(__lock_file, "%s.%d", __lock_filename, getpid());
    link(__lock_filename, __lock_file);
    stat(__lock_filename, &st_1);
    if (st_1.st_nlink < st.st_nlink) {
        std::cerr << " __os_init Error: Couldnt create a link to lock" << std::endl;
    }
    rt_linux_use_fpu(1);
}

void __os_exit(void) {
    // should check if more task (started by other processes are there)
    struct stat st;
    // unlink lock file
    unlink(__lock_file);
    stat(__lock_filename, &st);
    if (st.st_nlink > 1) {
        std::cerr << "__os_exit Info: Another real-time program is running, we don't stop the timer." << std::endl;
    } else {
        stop_rt_timer();
        return;
    }
}

#elif (CISST_OS == CISST_WINDOWS)
void __os_init(void) {
    //nothing is to be done now. later when we have our own dispatcher we might want to add
    //some code here
}

void __os_exit(void) {
    //nothing is to be done now. later when we have our own dispatcher we might want to add
    //some code here
}

#else // default unix
void __os_init(void) {}

void __os_exit(void) {}

#endif



#include <iostream>
#if (CISST_OS == CISST_LINUX_RTAI)
osaThreadBuddy::~osaThreadBuddy() {
    if (RTTask) {
        rt_task_delete(RTTask);
    }
}

#elif (CISST_OS == CISST_WINDOWS)
osaThreadBuddy::~osaThreadBuddy() {
    if (WaitTimer !=NULL) {
        CloseHandle(WaitTimer);
        WaitTimer = NULL;
    }
}

#else // default unix
osaThreadBuddy::~osaThreadBuddy() {}

#endif // CISST_WINDOWS



#if (CISST_OS == CISST_LINUX_RTAI)
void osaThreadBuddy::Create(const char *name, double period, int stack_size) {
    Period = period;
    RTTask = rt_task_init_schmod(nam2num(name), 0, stack_size, 0, SCHED_FIFO, 0xF);
    if (!RTTask) {
        CMN_LOG(1) << "OOPS!!! Couldn't create Proxy object" << std::endl;
        exit(1);
    }
    if (rt_task_use_fpu(RTTask, 1) < 0) {
        CMN_LOG(1) << "OOPS!!! FPU couldnt be allocated" << std::endl;
    }
    // maybe this should be just before we make code real-time
    // causes failure of memory intensive calls such as onenGL
    // so should be done with soft real time.
    // or maybe should have something like "really-soft-real-time"
    mlockall(MCL_CURRENT | MCL_FUTURE);
    if (IsPeriodic())
        rt_task_make_periodic_relative_ns(RTTask, 0, (unsigned long)period);
}

#elif (CISST_OS == CISST_WINDOWS)
void osaThreadBuddy::Create(const char *name, double period, int stack_size) {
    Period = period;
    DueTime.QuadPart = 0UL;
    IsSuspended = false;
    for (unsigned int i = 0; i < 6; i++) Name[i] = name[i];
    if (IsPeriodic()) {
        WaitTimer = CreateWaitableTimer(NULL, true, Name);
        if (WaitTimer == NULL) {
            CMN_LOG(1) << "OOPS! Couldnt create a waitable time" << std::endl;
        }
    }
}

#else // default unix
void osaThreadBuddy::Create(const char *name, double period, int stack_size) {
    Period = period;
    DueTime.tv_sec = 0;
    DueTime.tv_usec = 0;
    IsSuspended = false;
    for (unsigned int i = 0; i < 6; i++) Name[i] = name[i];
}

#endif



#if (CISST_OS == CISST_LINUX_RTAI)
void osaThreadBuddy::WaitForPeriod(void) {
    if (IsPeriodic())
        rt_task_wait_period();
}

#elif (CISST_OS == CISST_WINDOWS)
void osaThreadBuddy::WaitForPeriod(void) {
    if (IsPeriodic()) {
        // this might change in future when we introduce our own
        // scheduler/dispatcher but even then our sleep might be better
        // that using Sleep() provided by windows;
        LARGE_INTEGER dueTime;
        LONGLONG waitTime = (LONGLONG)(-Period/100);
        dueTime.QuadPart = waitTime; //dueTime is in 100ns
        SetWaitableTimer(WaitTimer, &dueTime, 0, NULL, NULL, 0);
        WaitForSingleObject(WaitTimer, INFINITE);
    }
}

#else // default unix
#include <sys/select.h>
void osaThreadBuddy::WaitForPeriod(void) {
    if (IsPeriodic()) {
        struct timespec timeout;
        timeout.tv_sec = 0;
        timeout.tv_nsec = (long)(Period);
        pselect(0, NULL, NULL, NULL, &timeout, NULL);
    }
}

#endif



#if (CISST_OS == CISST_LINUX_RTAI)
void osaThreadBuddy::WaitForRemainingPeriod(void) {
    if (IsPeriodic())
        rt_task_wait_period();
}

#elif (CISST_OS == CISST_WINDOWS)
void osaThreadBuddy::WaitForRemainingPeriod(void) {
    if (!IsPeriodic())
        return;
    // in a periodic loop at the end we want to wait only for the
    // portion of time that is left of the period. now if someone else
    // was generating the time event, then we wait for that
    // event. but for now //we are keeping our own time, so we must
    // wait for remaining portion of the time.
    LARGE_INTEGER liTimerFrequency, liTimeRemaining, liTimeNow;
    double elapsedTime, timeRemaining;
    do {
#if 1
        if (DueTime.QuadPart == 0) {
            // this is the first time it is being called
#if 0
            SYSTEMTIME sysnow;
            FILETIME now;
            GetSystemTime(&sysnow);
            SystemTimeToFileTime(&sysnow, &now);
            ULARGE_INTEGER ulnow;
            ulnow.HighPart = now.dwHighDateTime;
            ulnow.LowPart = now.dwLowDateTime;
            DueTime.QuadPart = ulnow.QuadPart + Period;
#endif
            if (QueryPerformanceCounter(&DueTime) ==0 ) {
                CMN_LOG(1) << "No performace counter available" << std::endl;
            }
            if (QueryPerformanceFrequency(&liTimerFrequency) == 0) {
                CMN_LOG(1) << "No performace counter available" << std::endl;
            }
            TimerFrequency = (double)liTimerFrequency.QuadPart/(double)(1000000000);
        }
        QueryPerformanceCounter(&liTimeNow);
        elapsedTime = (double)(liTimeNow.QuadPart - DueTime.QuadPart);
        elapsedTime /= TimerFrequency; //this is in nano seconds
        timeRemaining = Period - elapsedTime; //this is in nano seconds
        liTimeRemaining.QuadPart = (LONGLONG)(-timeRemaining/100);

        SetWaitableTimer(WaitTimer, &liTimeRemaining, 0, NULL, NULL, 0);//dueTime is in 100ns
        WaitForSingleObject(WaitTimer, INFINITE);
        QueryPerformanceCounter(&DueTime);
#endif

#if 0
    LARGE_INTEGER dueTime;
    LONGLONG waitTime = (LONGLONG)(-Period/100);
    dueTime.QuadPart = waitTime; //dueTime is in 100ns
    SetWaitableTimer(WaitTimer, &dueTime, 0, NULL, NULL, 0);
    WaitForSingleObject(WaitTimer, INFINITE);
#endif
    } while (IsSuspended);
}

#else // default unix
#include <sys/time.h>
#include <unistd.h>

void osaThreadBuddy::WaitForRemainingPeriod(void) {
    if (!IsPeriodic())
        return;
    double elapsedTime, timeRemaining;
    struct timeval timeNow, timeLater;
    struct timespec timeSleep;
    do {
        if (DueTime.tv_usec == 0 && DueTime.tv_sec == 0) {
            // this is the first time this is being called;
            gettimeofday(&DueTime, NULL);
        }
        gettimeofday(&timeNow, NULL);
        elapsedTime = (double)(1000*1000*(timeNow.tv_sec-DueTime.tv_sec)+(timeNow.tv_usec-DueTime.tv_usec)); // in usec
        timeRemaining = Period - elapsedTime*1000;
        timeSleep.tv_sec = 0;
        timeSleep.tv_nsec = (long)timeRemaining;
        pselect(0, NULL, NULL, NULL, &timeSleep, NULL);
        gettimeofday(&timeLater, NULL);
        DueTime.tv_sec = timeLater.tv_sec;
        DueTime.tv_usec = timeLater.tv_usec;
    } while (IsSuspended);
}

#endif



#if (CISST_OS == CISST_LINUX_RTAI)
void osaThreadBuddy::MakeHardRealTime(void) {
    rt_make_hard_real_time();
}
#else
void osaThreadBuddy::MakeHardRealTime(void) {
    //a NOP on all other os
}
#endif



#if (CISST_OS == CISST_LINUX_RTAI)
void osaThreadBuddy::MakeSoftRealTime(void) {
    rt_make_soft_real_time();
}

#else
void osaThreadBuddy::MakeSoftRealTime(void) {
    //a NOP on all other os
}
#endif



#if (CISST_OS == CISST_LINUX_RTAI)
void osaThreadBuddy::Resume(void) {
    rt_task_resume(RTTask);
}

#elif (CISST_OS == CISST_WINDOWS)
void osaThreadBuddy::Resume(void) {
    IsSuspended = false;
}

#else
void osaThreadBuddy::Resume(void) {
    IsSuspended = false;
}

#endif



#if (CISST_OS == CISST_LINUX_RTAI)
void osaThreadBuddy::Suspend(void) {
    rt_task_suspend(RTTask);
}

#elif (CISST_OS == CISST_WINDOWS)
void osaThreadBuddy::Suspend(void) {
    IsSuspended = true;
}

#else 
void osaThreadBuddy::Suspend(void) {
    IsSuspended = true;
}

#endif


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: osaThreadBuddy.cpp,v $
// Revision 1.11  2006/04/28 21:36:47  pkaz
// osaThreadBuddy and rtsTask:  fixed support for using the "main" thread in an rtsTask.
// Mailboxes did not work with the previous commit [1846].  The fix required support
// for non-periodic threads in osaThreadBuddy.
//
// Revision 1.10  2006/03/18 04:47:13  anton
// cisstOSAbstraction: Added license, replace tabs by spaces.
//
// Revision 1.9  2006/03/18 03:39:00  anton
// osaThreadBuddy: Conditional compilation updated so that all unix like use
// the same implementation (eg. Darwin works like Linux).
//
// Revision 1.8  2006/03/17 15:48:25  kapoor
// cisstOSAbstraction: Real code in threadbuddy for plain linux. See Ticket #218.
//
// Revision 1.7  2006/03/17 03:39:58  kapoor
// rtsTask: RTAI requires the program /thread to tell the scheduler if FPU will
// be used in the process/thread. Not setting this could lead to the FPU stack
// corruption.
//
// Revision 1.6  2006/03/02 22:10:27  pkaz
// cisstOSAbstraction:  added dummy implementations of osaThreadBuddy and osaMailbox for other operating systems
// (currently, implementations only exist for RTAI and Windows -- see #218).
//
// Revision 1.5  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.4  2005/05/03 15:37:02  anton
// cisstOSAbstraction: Cleanup some log (replaced printf, cout by CMN_LOG)
//
// Revision 1.3  2005/04/24 16:43:35  kapoor
// rtsTask: For RTAI, added lock file to disallow stopping of rtai if
// a process is still running. This is a workaround, because I was hoping
// that rtai would handle it.
//
// Revision 1.2  2005/03/17 23:17:27  kapoor
// Real-Time: CISST library ThreadBuddy bug (see ticket #135)
//
// Revision 1.1  2004/05/04 14:12:11  anton
// Imported code from Ankur
//
//
// Revision 1.2  2004/04/08 06:07:37  kapoor
// Added win32 implementation
//
// Revision 1.1  2004/03/16 18:05:08  kapoor
// Added osaThreadBuddy.
//
// ****************************************************************************
