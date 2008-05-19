/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnExportMacros.h,v 1.13 2006/02/04 05:18:09 pkaz Exp $

  Author(s):  Anton Deguet
  Created on: 2001-04-11

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
  \brief Macros for the creation of Visual C++ Dlls
 */

// clean
#undef CISST_EXPORT

// do it only for windows 32 OSs (95/98/me or NT/2000) and Microsoft compilers.
// Note: we cannot rely on cmnPortability here because this file is, in fact,
// included into cmnPortability.
#if WIN32 && _MSC_VER

#ifdef CISST_THIS_LIBRARY_AS_DLL
// warning regarding base class not exporting, i.e. all std::
#pragma warning (disable: 4275)
#define CISST_EXPORT _declspec(dllexport)
#elif CISST_DLL         // local and import headers
// warning regarding base class not exporting, i.e. all std::
#pragma warning (disable: 4275)
#define CISST_EXPORT _declspec(dllimport)
#endif

#endif // end windows dll stuff

// for all other operating systems and windows static libraries
#ifndef CISST_EXPORT
#define CISST_EXPORT
#endif


// Doxygen fake code, special values
#ifdef DOXYGEN

#undef CISST_EXPORT
/*!
  \brief Export a symbol for a Dll

   This macro is used only for the creation of Windows Dll with Visual
   C++.  On all other operating systems or compilers, this macro does
   nothing.

   With Windows and Microsoft VC++, whenever someone needs to create a
   Dll, the macro expands as _declspec(dllexport).  To specify that
   the file is used to create a Dll, the preprocessor must have
   \<library\>_EXPORTS defined.  This value is set automatically for
   each library by CMake if a DLL is to be compiled.

   Usage:
   Append macros to declare exported variables, functions and classes.

   - A global variable in a header file:
     \code
     extern CISST_EXPORT myType myVariable;
     \endcode

   - A global function in a header file:
     \code
     CISST_EXPORT myType functionName(..);
     \endcode

   - A class in a header file:
     \code
     class CISST_EXPORT className { ... };
     \endcode

   - For a templated class or method with inlined code only, do not use
     CISST_EXPORT

   - For a template class with no inlined code at all, use CISST_EXPORT
     for the class declaration

   - For a class mixing inlined and non inlined code, use CISST_EXPORT
     for the declarations of the non inlined code only 

   When one needs to use an existing Dll, the macro must expand as
   _declspec(dllimport).  To do so, the preprocessor must have
   CISST_DLL defined.  Again, using CMake, this is automatically set.


*/
#define CISST_EXPORT "depends on the OS and compiler"

#endif // Doxygen special values


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cmnExportMacros.h,v $
//  Revision 1.13  2006/02/04 05:18:09  pkaz
//  cmnExportMacros.h: fixed typos in doxygen comments.
//
//  Revision 1.12  2006/01/03 03:33:04  anton
//  cisstCommon Doxygen: Use \code \endcode instead of <pre></pre>, updated
//  cmnTypeTraits to avoid all specialization to appear, updated CISST_DEPRECATED.
//
//  Revision 1.11  2005/10/06 16:56:37  anton
//  Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
//  Revision 1.10  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.9  2005/05/19 19:29:00  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.8  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.7  2004/10/09 18:52:16  ofri
//  Some cleanup in cisstCommon header files.  gcc on cygwin prints a bunch of
//  warnings when it encounters #pragma warning .  Some files had that prama used
//  if WIN32 is defined, which is the case with cygwin/gcc.  I substituted that with
//  including cmnPortability and adding the warning disable that.
//
//  Revision 1.6  2004/06/14 19:19:44  anton
//  Added some documentation related to the use of CISST_EXPORT with templated
//  code, including classes with partially instantiated methods/partially
//  inlined (see ticket #26)
//
//  Revision 1.5  2003/10/03 19:09:10  anton
//  cleaned useless comments
//
//  Revision 1.4  2003/09/17 14:20:32  anton
//  changed from cmnEXPORT to CISST_EXPORT, added documentation
//
//  Revision 1.3  2003/08/14 14:55:31  anton
//  Cleanup older code from cis
//
//  Revision 1.2  2003/06/23 20:52:14  anton
//  filename changed
//
//
// ****************************************************************************

