/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnThrow.h,v 1.4 2005/09/28 20:28:50 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2005-09-22

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
  \brief Declaration of the template function cmnThrow
 */


#ifndef _cmnThrow_h
#define _cmnThrow_h

#include <cisstCommon/cmnLogger.h>

#include <stdexcept>
#include <cstdlib>


/*!
  \ingroup cisstCommon

  Throw an exception.  This templated function should be used to throw
  any exception.

  One of the advantages of this function over the default \c throw is
  that if the exception is of type \c std::exception, the message is
  logged (using the \c what method).<br> This function attempts to
  dynamically cast the exception.  If the cast succeeds, the cmnThrow
  will log the message using #CMN_LOG (level of detail 1).  If the
  cast fails, cmnThrow will log a less informative message anyway.<br>
  Once the message has been logged, cmnThrow simply uses \c throw to
  throw the exception.

  Using this function systematically within the cisst packages also
  allows some system wide configuration:

  \li In some special cases such as real-time programming, exceptions
  can be somewhat impractical.  If the variable \c
  CMN_THROW_DOES_ABORT is defined at compilation time, cmnThrow
  doesn't throw an exception but uses the \c abort function.  This is
  a very special case and the vast majority of users should not use
  this option.<br>Using the CMake advanced mode, it is possible to
  define CMN_THROW_DOES_ABORT for the whole cisst package.  It is
  important to note that this option will break many of the cisst
  package tests programs (tests based on \c try and \c catch).

  \li This function might be used later on to provide a nice debug
  breakpoint.  Indeed, cmnThrow is called before the exception is
  actually throw and the stack unwinding.

  \sa #CMN_ASSERT

  \note The type of exception thrown would ideally derived from \c
  std::exception but this is not a requirement.
 */
template <class _exceptionType>
inline void cmnThrow(const _exceptionType & except) throw(_exceptionType) {
    // try to create an std::exception pointer
    const std::exception * stdExcept = dynamic_cast<const std::exception *>(&except);
    if (stdExcept) {
        CMN_LOG(1) << "cmnThrow with std::exception ("
                   << stdExcept->what()
                   << ")"
                   << std::endl;
    } else {
        CMN_LOG(1) << "cmnThrow with non std::exception"
                   << std::endl;
    }
#ifdef CMN_THROW_DOES_ABORT
    CMN_LOG(1) << "cmnThrow is configured to abort() (CMN_THROW_DOES_ABORT defined)"
               << std::endl;
    std::abort();
#else
    throw except;
#endif // CMN_THROW_DOES_ABORT
}


#endif // _cmnThrow_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnThrow.h,v $
// Revision 1.4  2005/09/28 20:28:50  anton
// libs documentation: Corrected Doxygen links.
//
// Revision 1.3  2005/09/26 21:23:30  anton
// cisstCommon: Updated CMN_ASSERT and cmnThrow.  Variables are not defined in
// cisstConfig.h anymore (user could not override).  CMN_ASSERT can now be set
// to throw an exception (user can set preference with CMake).
//
// Revision 1.2  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.1  2005/09/23 23:55:25  anton
// cisstCommon: Added inline templated function cmnThrow.
//
//
// ****************************************************************************
