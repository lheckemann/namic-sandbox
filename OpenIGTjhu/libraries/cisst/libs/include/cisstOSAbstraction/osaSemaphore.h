/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: osaSemaphore.h,v 1.5 2006/03/18 04:47:13 anton Exp $

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
  \brief Declaration of osaSemaphore
  \ingroup cisstOSAbstraction
 */

#ifndef _osaSemaphore_h
#define _osaSemaphore_h

#include <cisstCommon/cmnPortability.h>
#include <cisstOSAbstraction/osaExport.h>

#if (CISST_OS == CISST_LINUX_RTAI)
#include <rtai_sem.h>
#include <rtai_types.h>
#endif // CISST_LINUX_RTAI

/*!
  \brief Define a Semaphore object

  \ingroup cisstOSAbstraction
 
  The semaphore class for mutual exclusion -- just a wrapper class so
  that the interface looks cleaner. Any OS worth a name should provide
  semaphores, so this one is easy wrap.
 */

class CISST_EXPORT osaSemaphore {
  /*! Pointer to OS dependent semaphore. */
#if (CISST_OS == CISST_LINUX_RTAI)
  SEM *Semaphore;
#else
  void *Semaphore;
#endif // CISST_LINUX_RTAI

public:
  /* Enum type representing a timeout period of infinity and no wait. */
  enum TimeoutType {
    WAITFOREVER = -1, /*! The calling function will be blocked till
                            the function returns */
    NOWAIT = 0     /*! The calling function will not wait for
                        the function */
  };
    
  /*! Enum type for return values of a lock operation. */
  enum ReturnType {
    LOCKFAILED, /*! The lock operation failed because the semaphore couldn;t be obtained */
    SUCCESS,    /*! The lock operation was successful */
    TIMEDOUT  /*! The lock operation timedout after waiting for specified time */
  };  
    
  /*! Default constructor.  Does nothing. */
  osaSemaphore(const char* name, int value = 1);
    
  /*! Default distructor.  Does nothing. */
  ~osaSemaphore();
    
  /*! Semaphore lock operation.  If, prior to the operation, the
      value of the semaphore is zero, the semaphore lock operation
      shall cause the calling thread to be blocked and added to the
      set of threads awaiting the semaphore; otherwise, the value
      shall be decremented.
    */
  void Lock(void);

  /*! Semaphore unlock operation.  If, prior to the operation, there
    are any threads in the set of threads awaiting the semaphore,
    then some thread from that set shall be removed from the set and
    becomes unblocked; otherwise, the semaphore value shall be
    incremented.
      
      \return true if successful, false otherwise.
    */
  void Unlock(void);

  /*! Semaphore lock operation with timeout
   
    \param timeout If timeout == osaWAITFOREVER then the calling
  thread is blocked (This is same as calling Lock().  timeout ==
  osaNOWAIT then the function return after checking if semaphore can
  be locked or not. If a lock can be obtained then the value of
  semaphore is decremented.  timeout > 0 then the function waits for
  the specified amount of time (units to be specified later) before
  returning. If a lock can be obtained in that time the value of
  semaphore is decremented.
   
    \return An enumerated type representing if a lock was obtained, or
    operation timedout or lock failed
    */
  ReturnType TryLock(int timeout);

  /*! Change the value of a semaphore.  This function is not defined
   in the POSIX 1003.13 Realtime Standard. Maybe we should have it
   at all, doesnt make sense to me to be able to change semaphore
   value other than using lock and unlock, but might be useful in
   testing.
   */
  void Set(void);

  /*! Return some identifier associtated with the semaphore
      \return Integer identifier associated with the semaphore
    */
  int Get(void);
};


#endif // _osaSemaphore_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: osaSemaphore.h,v $
// Revision 1.5  2006/03/18 04:47:13  anton
// cisstOSAbstraction: Added license, replace tabs by spaces.
//
// Revision 1.4  2005/10/07 02:25:44  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.3  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/01/10 19:21:53  anton
// cisstOSAbastraction: Added osaExport.h and required CISST_EXPORT to compile
// as a DLL (for windows).
//
// Revision 1.1  2004/05/05 20:56:50  anton
// Added missing file of cisstOSAbstraction from Ankur
//
//
// Revision 1.3  2004/03/24 20:47:56  kapoor
// Added lots of documentation.
//
// Revision 1.2  2004/03/16 17:44:52  kapoor
// Removed ^M.
//
// Revision 1.1.1.1  2004/03/02 04:05:03  kapoor
// Importing source into local tree
//
// ****************************************************************************
