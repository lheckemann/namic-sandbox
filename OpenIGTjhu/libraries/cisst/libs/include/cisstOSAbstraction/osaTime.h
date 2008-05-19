/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaTime.h,v 1.7 2006/03/18 04:47:13 anton Exp $

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
  \brief Declaration of osaTime
 */

#ifndef _osaTime_h
#define _osaTime_h
#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>

#if (CISST_OS == CISST_LINUX_RTAI)
#include <rtai_lxrt.h>
#include <unistd.h>
#endif // CISST_LINUX_RTAI

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
#include <sys/time.h>
#include <unistd.h>
#endif // CISST_LINUX || CISST_DARWIN

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#endif // CISST_WINDOWS

/*!
  \brief Define a Time object

  \ingroup cisstOSAbstraction
 
  This is a wrapper class for implementing time on different operating
  systems. The functionality is hold and return time in a uniform
  format across platforms. The sole important method getCurrentTime
  would return the most accurate available time on the OS.
*/
namespace osaTime {
#if (CISST_OS == CISST_LINUX_RTAI) || defined DOXYGEN
    /*! Return the most accurate time available on the system. In nanoseconds.
      \return A double representing time
     */
    inline double GetCurrentTime() {
        return rt_get_time_ns();
    }

    /*! Excute a not so accurate wait or sleep. ****DO NOT****
      use this inside of rtsTask methods, might lead to stack overflows.
      This is a very coarse functionality for genral purpose. No time 
      gurantees are made on any OS. 
      \param time Time in milliseconds to sleep */
    inline void Sleep(long time) {
        usleep(time * 1000);
    }
#endif // CISST_LINUX_RTAI

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN)
    inline double GetCurrentTime() {
        struct timeval currentTime;
        gettimeofday(&currentTime, NULL);
        return ((double) currentTime.tv_sec) * 1000.0 + ((double)currentTime.tv_usec) / 1000.0;
    }

    inline void Sleep(long time) {
        usleep(time * 1000);
    }

#endif // CISST_LINUX || CISST_DARWIN

    
#if (CISST_OS == CISST_WINDOWS)
    inline double GetCurrentTime() {
        LARGE_INTEGER liTimerFrequency, liTimeNow;
        double timerFrequency, time;
        if (QueryPerformanceCounter(&liTimeNow) ==0 )    {
            CMN_LOG(1) << "No performance counter available" << std::endl;
        }
        if (QueryPerformanceFrequency(&liTimerFrequency) ==    0) {
            CMN_LOG(1) << "No performance counter available" << std::endl;
        }
        timerFrequency = (double)liTimerFrequency.QuadPart/(double)(1000000000); //counts per nanosec
        time = (double)liTimeNow.QuadPart/timerFrequency;
        return time;
    }

    inline void Sleep(long time) {
        //Sleep(time);
        HANDLE WaitTimer;
        LARGE_INTEGER dueTime;
        time *= -10*1000;
        dueTime.QuadPart = time; //dueTime is in 100ns
        WaitTimer = CreateWaitableTimer(NULL, true, "osaTime");
        SetWaitableTimer(WaitTimer, &dueTime, 0, NULL, NULL, 0);
        WaitForSingleObject(WaitTimer, INFINITE);
        CloseHandle(WaitTimer);
    }
    
#endif // CISST_WINDOWS

}


#endif // _osaTime_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: osaTime.h,v $
// Revision 1.7  2006/03/18 04:47:13  anton
// cisstOSAbstraction: Added license, replace tabs by spaces.
//
// Revision 1.6  2005/12/01 03:38:37  anton
// osaTime: Added support for Darwin (allows to compile current tests for
// real-time and device interface on Mac).
//
// Revision 1.5  2005/11/29 14:34:40  anton
// osaTime.h: Use CMN_LOG instead of std::cout, minor typo in error message.
//
// Revision 1.4  2005/10/07 02:25:44  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.3  2005/09/26 15:41:47  anton
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
// Revision 1.5  2004/04/08 05:57:31  kapoor
// period is now double. added win32 implementation.
//
// Revision 1.4  2004/03/24 20:47:56  kapoor
// Added lots of documentation.
//
// Revision 1.3  2004/03/22 00:09:13  kapoor
// removed the class osaTime and added namespace osaTime which has the function
// GetCurrentTime.
//
// Revision 1.2  2004/03/16 17:44:28  kapoor
// Removed ^M.
//
// Revision 1.1.1.1  2004/03/02 04:05:03  kapoor
// Importing source into local tree
//
// ****************************************************************************
