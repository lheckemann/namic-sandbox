/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnPortability.h,v 1.32 2006/01/31 21:11:03 anton Exp $
  
  Author(s):  Anton Deguet
  Created on: 09/08/2003

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


#ifndef _cmnPortability_h
#define _cmnPortability_h

#include <string>

#include <cisstConfig.h>
#include <cisstCommon/cmnExport.h>

/*!
  \file
  \brief Portability across compilers and operating systems tools.
  \ingroup cisstCommon
*/

/*!
  Fallback value for both #CISST_OS and #CISST_COMPILER.
*/
#define CISST_UNDEFINED 0


/*! \name Testing the operating system */
//@{
/*!
  Value used to set the Operating System used.  To test if the
  operating system is this one, compare with CISST_OS:
  \code
  #if (CISST_OS == CISST_LINUX)
  ...
  #endif
  \endcode
*/
#define CISST_WINDOWS 1
#define CISST_LINUX 2
#define CISST_RTLINUX 3
#define CISST_IRIX 4
#define CISST_SOLARIS 5
#define CISST_LINUX_RTAI 6
#define CISST_CYGWIN 7
#define CISST_DARWIN 8
//@}

#ifndef DOXYGEN
extern CISST_EXPORT const std::string cmnOperatingSystemsStrings[];
#endif // DOXYGEN

/*! \name Testing the compiler */
//@{
/*!
  Value used to set the compiler used.  To test if the compiler is
  this one, compare with CISST_COMPILER:
  \code
  #if (CISST_COMPILER == CISST_GCC)
  ...
  #endif
  \endcode
*/
#define CISST_GCC 1
#define CISST_VCPP6 2
#define CISST_DOTNET7 3
#define CISST_SGI_CC 4
#define CISST_SUN_CC 5
#define CISST_INTEL_CC 6
#define CISST_DOTNET2003 7
//@}

// skip rest of tests for doxygen
#ifndef DOXYGEN

extern CISST_EXPORT const std::string cmnCompilersStrings[];

// First, see if it's gcc and then determine OS
// (Another compiler may determine the OS differently).
#ifdef __GNUC__  // see man gcc
  #define CISST_COMPILER CISST_GCC
  #ifdef linux // see cpp -dM and ctrl+d
    #define CISST_OS CISST_LINUX
  #endif // linux
  #if (CISST_HAS_LINUX_RTAI == ON) // overwrite if RTAI
    #undef CISST_OS
    #define CISST_OS CISST_LINUX_RTAI
  #endif // linux RTAI
  #ifdef sun
    #define CISST_OS CISST_SOLARIS
  #endif // sun
  #ifdef sgi
    #define CISST_OS CISST_IRIX
  #endif // sgi
  #ifdef __CYGWIN__
    #define CISST_OS CISST_CYGWIN
  #endif // __CYGWIN__
  #ifdef __APPLE__
    #define CISST_OS CISST_DARWIN
  #endif // __APPLE__
#else // __GNUC__
  #ifdef sgi
    #define CISST_OS CISST_IRIX
    #ifndef __GNUC__
       #define CISST_COMPILER CISST_SGI_CC
    #endif
  #endif // sgi
  #ifdef sun
    #define CISST_OS CISST_SOLARIS
    #ifndef __GNUC__
       #define CISST_COMPILER CISST_SUN_CC
    #endif
  #endif // sun
#endif // __GNUC__


// Microsoft compilers
#ifdef _WIN32  // see msdn.microsoft.com
// we require Windows 2000, XP or more
  #define _WIN32_WINNT 0x0500
  #define CISST_OS CISST_WINDOWS
  #if (_MSC_VER == 1200)
    #define CISST_COMPILER CISST_VCPP6
  #else
    #if (_MSC_VER == 1300)
      #define CISST_COMPILER CISST_DOTNET7
    #endif
    #if (_MSC_VER > 1300)
      #define CISST_COMPILER CISST_DOTNET2003
    #endif
  #endif
#endif // _WIN32 


// Intel compiler
#if (__INTEL_COMPILER)
  #define CISST_COMPILER CISST_INTEL_CC
  #if defined (__linux__) // linux
    #define CISST_OS CISST_LINUX
  #endif // linux
  #if (CISST_HAS_LINUX_RTAI == ON) // overwrite if RTAI
    #undef CISST_OS
    #define CISST_OS CISST_LINUX_RTAI
  #endif // linux RTAI
  #ifndef CISST_OS // windows at last
    #define CISST_OS CISST_WINDOWS
  #endif // windows
#endif // __INTEL_COMPILER


// Make sure that both OS and compilers are set
#ifndef CISST_COMPILER
  #define CISST_COMPILER CISST_UNDEFINED
#endif

#ifndef CISST_OS
  #define CISST_OS CISST_UNDEFINED
#endif

// We define unifying macros for the two Windows environments:
// Windows and Cygwin
#if (CISST_OS == CISST_WINDOWS) || (CISST_OS == CISST_CYGWIN)
//: CISST_OS_IS_WINDOWS is set to 1 for Windows and Cygwin,
// and set to zero otherwise
#  define CISST_OS_IS_WINDOWS 1
// Define WIN32 as 1 for any case (Cygwin is not entirely compatible
// otherwise).
#  ifndef WIN32
#    define WIN32 1
#  elif !WIN32
#    undef WIN32
#    define WIN32 1
#  endif
#else
//: CISST_OS_IS_WINDOWS is set to 1 for Windows and Cygwin,
// and set to zero otherwise
#  define CISST_OS_IS_WINDOWS 0
#endif

#endif // DOXYGEN end of doxygen section to skip


/*!
  \def CISST_DEPRECATED

  Define the macro CISST_DEPRECATED.  In gcc and .NET, a method or
  other interfaces (e.g., function, and sometimes class, type or
  variable) can be declared as deprecated by adding specific
  nonstandard qualifier in the declaration of the object.
  
  We have unified these qualifiers into a single macro.  Here's a usage example.
  
  \code
  void CISST_DEPRECATED f();
  \endcode
  
  After this declaration, when a programmer uses a call to f() inside
  any other block, a compiler warning is generated on those compilers
  which support deprecation qualifiers.
  
  The macro is defined as blank if the compiler does not support depracation.
  
  \note More work needs to be done on how to declare a deprecated class
*/
//@{
#if (CISST_COMPILER == CISST_GCC)
#define CISST_DEPRECATED __attribute__ ((deprecated))
#elif (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
#define CISST_COMPILER_IS_MSVC
#define CISST_DEPRECATED __declspec(deprecated)
#else
#define CISST_DEPRECATED
#endif
//@}

/*! Disable some specifically annoying warnings printed by Microsoft
  compilers:

  4290 C++ exception specification ignored ...
  4251 'identifier' : class 'type' needs to have dll-interface ...
  4786 'identifier' : identifier was truncated to 'number' characters ...
*/
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
#pragma warning(disable: 4290 4251 4786)
#endif

/*!  Discard of the Windows.h definition of macros min and max
*/
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003) || (CISST_OS == CISST_CYGWIN)
#define NOMINMAX
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#endif

/*! Allows the definition of M_PI, etc.  This is not very robust since it will
    not work if someone include math.h before us.
*/
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
#define _USE_MATH_DEFINES
#endif

#include <math.h>

/*!
  \brief Detect a NaN

  This macro is a wrapper for different compilers to simplify the use
  of the NaN macro.  With gcc and icc, is uses isnan() and for
  Microsoft compilers, _isnan().  In most cases, the test should look
  like:
  \code
  if (CMN_ISNAN(myValue)) {
      ... deal with the problem, i.e. not a number;
  }
  \endcode

  \param x The number to be tested.  
*/
#if (CISST_COMPILER == CISST_VCPP6) || (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
  #include <float.h>
  #define CMN_ISNAN(x) _isnan(x)
#else
  #if (CISST_OS == CISST_DARWIN)
    #ifndef isnan
      extern "C" int isnan (double);
    #endif
  #endif
  #define CMN_ISNAN(x) isnan(x)
#endif


/*!
  \brief Detect a finite number

  This macro is a wrapper for different compilers to simplify the use
  of the finite number macro.  With gcc and icc, is uses finite() and
  for Microsoft compilers, _finite().  In most cases, the test should
  look like:
  \code
  if (!CMN_ISFINITE(myValue)) {
      ... deal with the problem, i.e. infinite number;
  }
  \endcode

  \param x The number to be tested.  
*/
#if (CISST_COMPILER == CISST_VCPP6) || (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
#define CMN_ISFINITE(x) _finite(x)
#else
#define CMN_ISFINITE(x) finite(x)
#endif



/*!
  For whatever reason, there is a syntactic incompatiblity between .NET and gcc
  on the issue of member function template specialization.  .NET requires a strict
  declaration of the specialized member:
  
  template returnType foo::bar<templateArgs>(parameters);
  
  followed by the definition
  
  returnType foo::bar(parameters) { ... }
  
  Whereas gcc can deduce the template arguments by simply looking at the definition
  
  template<> returnType foo::bar(parameters) { ... }
  
  
  To provide specialization of template functions, we unify these two structures
  by mandating the explicit declaration preceded by the macro 
  CISST_DECLARE_TEMPLATE_FUNCTION_SPECIALIZATION , then writing the definition preceded
  by the macro CISST_DEFINE_TEMPLATE_FUNCTION_SPECIALIZATION. 
  
  For example:
  
  CISST_DECLARE_TEMPLATE_FUNCTION_SPECIALIZATION
  void foo::bar<templateArgs>(parameters);
  
  CISST_DEFINE_TEMPLATE_FUNCTION_SPECIALIZATION
  void foo::bar(parameters) { ... }
*/
#if (CISST_COMPILER == CISST_DOTNET7)
#define CISST_DECLARE_TEMPLATE_FUNCTION_SPECIALIZATION template
#define CISST_DEFINE_TEMPLATE_FUNCTION_SPECIALIZATION
#else
#define CISST_DECLARE_TEMPLATE_FUNCTION_SPECIALIZATION template<>
#define CISST_DEFINE_TEMPLATE_FUNCTION_SPECIALIZATION template<>
#endif

#endif // _cmnPortability_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cmnPortability.h,v $
//  Revision 1.32  2006/01/31 21:11:03  anton
//  cmnPortability.h: Added defined CISST_COMPILER_IS_MSVC.
//
//  Revision 1.31  2006/01/03 03:33:04  anton
//  cisstCommon Doxygen: Use \code \endcode instead of <pre></pre>, updated
//  cmnTypeTraits to avoid all specialization to appear, updated CISST_DEPRECATED.
//
//  Revision 1.30  2005/12/23 21:25:32  anton
//  cisstCommon: Minor updates for Doxygen 1.4.5.
//
//  Revision 1.29  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.28  2005/09/01 05:59:46  anton
//  cmnPortability.h: Typo in comments.
//
//  Revision 1.27  2005/05/19 19:29:00  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.26  2005/04/23 15:39:10  ofri
//  cmnPortability: Added cygwin to the list of environments that include window.h,
//  and the macro definition of min and max need to be overriden.
//
//  Revision 1.25  2005/02/24 02:40:35  anton
//  cmnPortability:  Added code to force the definition of M_PI with VC++ 7.1
//  (see ticket #131).
//
//  Revision 1.24  2005/02/03 21:14:02  ofri
//  cmnPortability: Extended the macro CISST_DEPRECATED to cover .NET compilers.
//  Added some documentation on the usage of the macro.
//
//  Revision 1.23  2004/12/28 05:30:22  anton
//  cisst: Added some code to compile under Darwin (i.e. Mac OS X).  Tested
//  with cisstCommon, cisstVector and cisstNumerical (with cnetlib).
//
//  Revision 1.22  2004/10/09 18:54:19  ofri
//  cmnPortability: documented warning suppression.  Added macro CISST_DEPRECATED
//  implemented for gcc.  Currently it's not in use, but one may want to try it.
//
//  Revision 1.21  2004/09/24 17:28:20  ofri
//  We defined unifying macros for the two Windows enviornments: Windows and Cygwin
//
//  Revision 1.20  2004/09/24 03:55:12  ofri
//  cmnPortability: Added identifiers for CYGWIN gcc compiler
//
//  Revision 1.19  2004/09/22 14:16:08  anton
//  cmnPortability.h: Removed a couple of ^M
//
//  Revision 1.18  2004/09/01 21:12:02  anton
//  cisstCommon: Major update of the class register and the logging system.
//  The class register is now a singleton (as in "Design Patterns") and doesn't
//  store any information related to the logging (i.e. global lod, multiplexer
//  for the output).  The data related to the log is now regrouped in the
//  singleton cmnLogger.  This code is still fairly experimental (but tested)
//  and the documentation is missing.  These changes should solve the tickets
//  #30, #38 and #46.
//
//  Revision 1.17  2004/08/12 20:08:02  anton
//  cmnPortability: Added some preliminary code to support .Net 2003
//
//  Revision 1.16  2004/08/10 13:58:37  ofri
//  Disabled "exception specification ignored" warning (4290) of .NET7 entirely
//  from cmnPortability.h as agreed in the last developers meeting.
//
//  Revision 1.15  2004/08/02 18:59:22  ofri
//  cmnCompatibility: Refering to [654], I disabled the Windows.h macros min and
//  max which interfere with the template functions std::min and std::max
//
//  Revision 1.14  2004/07/16 14:36:17  anton
//  cmnPortability: Removed CISST_MIN and CISST_MAX (were for VC++ 6)
//
//  Revision 1.13  2004/07/15 16:45:50  ofri
//  1) cmnPortability: Defined the macros CISST_DECLARE_TEMPLATE_FUNCTION_SPECIALIZATION
//  and CISST_DEFINE_TEMPLATE_FUNCTION_SPECIALIZATION to enable portable definitions
//  of template function specialization.
//  2) cmnRandomSequence: Implemented specialized versions of the generic methods
//  ExtractRandomValue and ExtractRandomValueArray for double, float, int.
//
//  Revision 1.12  2004/05/06 14:17:37  ofri
//  Definition of cmnPortability macros CISST_MAX and CISST_MIN for MSVC7
//  is now the standard std::max and std::min .  The previous definition resulted
//  from including the wrong header files.
//
//  Revision 1.11  2004/05/04 14:04:32  anton
//  Added _WIN32_WINNT
//
//  Revision 1.10  2004/02/18 22:10:40  anton
//  Added code to include cisstConfig.h and test for RTAI
//
//  Revision 1.9  2003/12/05 22:33:00  ofri
//  Added wrapper macros CISST_MIN and CISST_MAX to cmnPortability
//
//  Revision 1.8  2003/11/25 22:39:59  anton
//  Added #include <math.h> for isnan macro.  Updated documentation for icc
//
//  Revision 1.7  2003/11/25 22:31:51  anton
//  Added some tests to find the Intel compiler
//
//  Revision 1.6  2003/10/21 19:12:09  anton
//  Added CMN_ISFINITE, documentation
//
//  Revision 1.5  2003/10/21 17:08:23  anton
//  Added CMN_ISNAN.  Added documentation for CISST_xxx
//
//  Revision 1.4  2003/09/17 14:17:13  anton
//  added CISST_EXPORT for the extern strings
//
//  Revision 1.3  2003/09/10 15:57:49  ofri
//  Fixed typo in definition of CISST_COMPILER
//
//  Revision 1.2  2003/09/10 14:09:34  anton
//  added comments to reference man gcc, man cpp, etc
//
//  Revision 1.1  2003/09/09 18:54:08  anton
//  creation
//
//
// ****************************************************************************

