/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaThreadBuddy.h,v 1.13 2006/04/28 21:36:47 pkaz Exp $

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
  \brief Declaration of osaThreadBuddy
*/

#ifndef _osaThreadBuddy_h
#define _osaThreadBuddy_h

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaThread.h>

#if (CISST_OS == CISST_LINUX_RTAI)
#include <rtai_lxrt.h>
#include <sys/mman.h>
// end of CISST_LINUX_RTAI

#elif (CISST_OS == CISST_WINDOWS)
#include <windows.h>
// end of CISST_WINDOWS

#else
#include <sys/time.h>
#endif // end of others

// Always include last
#include <cisstOSAbstraction/osaExport.h>

/*! \ingroup cisstOSAbstraction
  The OS dependent initilization that is
  to be executed for real-time support. This is called from the
  constructor of TaskManager, which is implemented as singleton. */
CISST_EXPORT void __os_init(void);

/*! \ingroup cisstOSAbstraction
  The OS dependent cleanup that is to be executed for real-time support.
  This is called from the distructor of TaskManager. */
CISST_EXPORT void __os_exit(void);


/*!  \brief Implementation of the Thread Proxy object in an OS
  independent manner.

  \ingroup cisstOSAbstraction

  RTAI provides "buddy" or proxy objects so that any thread created
  using pthread can be made real time. our rtsTask class uses this
  mechanism to provide real time abstract tasks that have a state and
  a mailbox. This class makes it possible to keep rtsTask class free
  from OS dependent stuff, which is moved here. This would also make
  it easy to provide Soft Real Time tasks in Vanila Linux flavor.
 */
class CISST_EXPORT osaThreadBuddy {
#if (CISST_OS == CISST_LINUX_RTAI)
    /*! A pointer to the thread buddy on RTAI. */
    RT_TASK *RTTask;
// end of CISST_LINUX_RTAI

#elif (CISST_OS == CISST_WINDOWS)
    /*! A pointer to void on all other operating systems */
    HANDLE WaitTimer;
    LARGE_INTEGER DueTime;
    bool IsSuspended;
    double TimerFrequency;
    char Name[6];
// end of CISST_WINDOWS

#else
    struct timeval DueTime;
    bool IsSuspended;
    char Name[6];
#endif // end of others
    /*! Thread period (if > 0) */
    double Period;

public:
    /*! Constructor. Does nothing */
#if (CISST_OS == CISST_LINUX_RTAI)
    osaThreadBuddy(){}
#elif (CISST_OS == CISST_WINDOWS)
    osaThreadBuddy():WaitTimer(NULL){}
#else
    osaThreadBuddy(){}
#endif

    /*! Destructor. */
    ~osaThreadBuddy();

    /*! Create a thread buddy with a name 'name' 
    
    \param name Name of thread buddy, the thread will also be refered
    using the same name.
    
    \param period The period of the thread
    
    \param stack_size The stack size allocated to the thread. A
      default value of 8Kb is used. */
    void Create(const char *name, double period, int stack_size = 1024*256);

    /*! Return true if thread is periodic.  Currently, returns true if
      the thread was created with a period > 0. */
    bool IsPeriodic() const { return Period > 0; }

    /*! Suspend the execution of the real time thread until the next
      period is reached. */
    void WaitForPeriod(void);

    /*! Suspend the execution of the real time thread for the
      remainder of the current period. */
    void WaitForRemainingPeriod(void);
    
    /*! Make a thread hard real time. Always returns false on
      non-real-time platforms. */
    void MakeHardRealTime(void);

    /*! Make a thread soft real time. Always returns true on
      non-real-time-platforms. */
    void MakeSoftRealTime(void);

    /*! Resume execution of the thread. */
    void Resume(void);

    /*! Suspend execution of the thread. */
    void Suspend(void);

    /*! Lock stack growth */
    void LockStack() {
#if (CISST_OS == CISST_LINUX_RTAI)
        mlockall( MCL_CURRENT | MCL_FUTURE );
#elif (CISST_OS == CISST_WINDOWS)

#else

#endif
    };

    /*! Unlock stack */
    void UnlockStack() {
#if (CISST_OS == CISST_LINUX_RTAI)
        munlockall();
#elif (CISST_OS == CISST_WINDOWS)

#else

#endif
    };
};


#endif // _osaThreadBuddy_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: osaThreadBuddy.h,v $
// Revision 1.13  2006/04/28 21:36:47  pkaz
// osaThreadBuddy and rtsTask:  fixed support for using the "main" thread in an rtsTask.
// Mailboxes did not work with the previous commit [1846].  The fix required support
// for non-periodic threads in osaThreadBuddy.
//
// Revision 1.12  2006/03/18 04:47:13  anton
// cisstOSAbstraction: Added license, replace tabs by spaces.
//
// Revision 1.11  2006/03/18 03:39:00  anton
// osaThreadBuddy: Conditional compilation updated so that all unix like use
// the same implementation (eg. Darwin works like Linux).
//
// Revision 1.10  2006/03/17 15:48:25  kapoor
// cisstOSAbstraction: Real code in threadbuddy for plain linux. See Ticket #218.
//
// Revision 1.9  2006/03/02 22:10:27  pkaz
// cisstOSAbstraction:  added dummy implementations of osaThreadBuddy and osaMailbox for other operating systems
// (currently, implementations only exist for RTAI and Windows -- see #218).
//
// Revision 1.8  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.7  2005/06/17 20:30:50  alamora
// cisstOSAbstraction and cisstRealTime: Added some required
// CISST_EXPORT to compile DLLs.
//
// Revision 1.6  2005/04/24 16:46:30  kapoor
// osaMailBox: Connect & Addmailbox return bool flag to indicate success/failure.
//
// Revision 1.5  2005/04/16 21:40:41  kapoor
// include/cisstOSAbstraction/osaThreadBuddy.h
//
// Revision 1.4  2005/03/17 23:17:33  kapoor
// Real-Time: CISST library ThreadBuddy bug (see ticket #135)
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
//
// Revision 1.3  2004/04/08 05:58:05  kapoor
// Added win32 implementation
//
// Revision 1.2  2004/03/24 20:47:56  kapoor
// Added lots of documentation.
//
// Revision 1.1  2004/03/16 18:05:11  kapoor
// Added osaThreadBuddy.
//
// ****************************************************************************
