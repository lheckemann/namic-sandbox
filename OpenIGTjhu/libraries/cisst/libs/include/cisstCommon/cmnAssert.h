/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnAssert.h,v 1.9 2005/12/23 21:25:32 anton Exp $

  Author(s):  Ankur Kapoor
  Created on: 2003-06-25

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
  \brief Assert macros definitions.
*/

#ifndef _cmnAssert_h
#define _cmnAssert_h

#include <assert.h>
#include <cstdlib>
#include <sstream>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstCommon/cmnLogger.h>


#ifndef DOXYGEN

// No __FUNCTION__ for g++ version < 2.6 __FUNCDNAME__ Valid only
// within a function and returns the decorated name of the enclosing
// function (as a string). __FUNCDNAME__ is not expanded if you use
// the /EP or /P compiler option.  __FUNCSIG__ Valid only within a
// function and returns the signature of the enclosing function (as a
// string). __FUNCSIG__ is not expanded if you use the /EP or /P
// compiler option.  __FUNCTION__ Valid only within a function and
// returns the undecorated name of the enclosing function (as a
// string). __FUNCTION__ is not expanded if you use the /EP or /P
// compiler option.


// Visual C++
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
    #ifdef __FUNCSIG__
        #define CMN_PRETTY_FUNCTION  __FUNCSIG__
    #else
        #warning "With Visual Studio, you need /EP or /P to have __FUNCSIG__"
    #endif

// GNU CC and Intel CC
#elif (CISST_COMPILER == CISST_GCC) || (CISST_COMPILER == CISST_INTEL_CC)
    #define CMN_PRETTY_FUNCTION __PRETTY_FUNCTION__

// Otherwise
#else
  #warning "Visual C++, GNU C++ and Intel CC are supported so far"
#endif

// Set a default value
#ifndef CMN_PRETTY_FUNCTION
    #define CMN_PRETTY_FUNCTION ""
#endif

#endif // DOXYGEN


/*!
  \ingroup cisstCommon

  Assert a condition.  This macro should be used whenever one needs to
  assert a condition.

  This macro has two main advantages over the system \c assert:

  \li The error message is log using #CMN_LOG (level of detail 1).

  \li CMN_ASSERT behavior can be modified using the defined variables
  CMN_ASSERT_DISABLED and CMN_ASSERT_THROWS_EXCEPTION.  The first
  variable allows to not compile the assertion.  It is similar to the
  \c NDEBUG for the standard \c assert .<br>The second variable allows
  to throw an exception (of type \c std::logic_error) instead of using
  the system \c abort.  This can be convenient if the libraries are
  wrapped for an interpreted language such as Python.  In this case,
  an \c abort() has the annoying effect of aborting the interpreter
  itself.<br>Both these variables can be modified using the CMake
  advanced mode.

  \note When compiling on linux make sure ulimit -c is unlimited!
  Otherwise, no core file will be generated with \c abort() .  By
  default, Redhat and Debian are set to prevent any core dump.

  \note On windows \c abort() uses stderr in console apps, message box
  API with OK button for release builds and message box API with
  "Abort, Ignore, Retry" for debug builds.

  \sa cmnThrow
*/
#ifdef CMN_ASSERT_DISABLED
    #define CMN_ASSERT(expr)
#else // CMN_ASSERT_DISABLED

#ifdef CMN_ASSERT_THROWS_EXCEPTION

#define CMN_ASSERT(expr) \
  if (!(expr)) { \
        std::stringstream messageBuffer; \
        messageBuffer << __FILE__ << ": Assertion '" << #expr \
                << "' failed in: " << CMN_PRETTY_FUNCTION \
                << ", line #" << __LINE__; \
    cmnThrow(std::logic_error(messageBuffer.str())); \
  }

#else // CMN_ASSERT_THROWS_EXCEPTION

#define CMN_ASSERT(expr) \
  if (!(expr)) { \
        std::stringstream messageBuffer; \
        messageBuffer << __FILE__ << ": Assertion '" << #expr \
                << "' failed in: " << CMN_PRETTY_FUNCTION \
                << ", line #" << __LINE__; \
        std::cerr << messageBuffer.str() << std::endl; \
    CMN_LOG(1) << messageBuffer.str() << std::endl; \
    abort(); \
  }

#endif // CMN_ASSERT_THROWS_EXCEPTION

#endif // CMN_ASSERT_DISABLED


#endif // _cmnAssert_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnAssert.h,v $
// Revision 1.9  2005/12/23 21:25:32  anton
// cisstCommon: Minor updates for Doxygen 1.4.5.
//
// Revision 1.8  2005/09/26 21:23:30  anton
// cisstCommon: Updated CMN_ASSERT and cmnThrow.  Variables are not defined in
// cisstConfig.h anymore (user could not override).  CMN_ASSERT can now be set
// to throw an exception (user can set preference with CMake).
//
// Revision 1.7  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.6  2005/09/23 23:58:41  anton
// cmnAssert: Added support for intel CC which supports pretty function.
//
// Revision 1.5  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.4  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.3  2004/10/07 20:13:15  anton
// cmnAssert: Added conditional compilation flag for .net 2003.  Examples were
// not compiling with .net 2003.
//
// Revision 1.2  2004/01/30 18:45:39  ofri
// Added a #else case to the definition of CMN_ASSERT to allow it to compile
// without the assertion flag set.
//
// Revision 1.1  2003/11/14 22:05:36  anton
// Added to repository for testing
//
//
// ****************************************************************************

