/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: mrmlWin32Header.h,v $
  Language:  C++
  Date:      $Date: 2005/02/08 03:16:28 $
  Version:   $Revision: 1.29 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See mrmlCopyright.txt or http://www.mrml.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __mrmlWIN32Header_h
#define __mrmlWIN32Header_h

#include "mrmlConfigure.h"

// add in the Windows variants

#if defined(__CYGWIN__)
#ifndef WIN32
#define WIN32 1
#endif
#ifndef _WIN32
#define _WIN32 1
#endif
#endif

/** Disable some common warnings in MS VC++ */
#if defined(_MSC_VER)

// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning ( disable : 4244 )

// 'identifier' : truncation from 'type1' to 'type2'
#pragma warning ( disable : 4305 )

// 'conversion' : truncation of constant value
#pragma warning ( disable : 4309 )

// decorated name length exceeded, name was truncated
#pragma warning ( disable : 4503 )

// 'identifier' : identifier was truncated to 'number' characters in the
// debug information
#pragma warning ( disable : 4786 )

// 'type' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning ( disable : 4800 )

// 'identifier' : class 'type' needs to have dll-interface to be used by
// clients of class 'type2'
#pragma warning ( disable : 4251 )


// non dll-interface class 'type' used as base for dll-interface class 'type2'
#pragma warning ( disable : 4275 )

// C++ exception specification ignored except to indicate a 
// function is not __declspec(nothrow)
#pragma warning ( disable : 4290 )

// 'type' : inconsistent dll linkage.  dllexport assumed.
#pragma warning ( disable : 4273 )


// typename keyword in default template arguments is not accepted by
// MSVC.  This macro should only be used in such places.
# if !defined(CABLE_CONFIGURATION) && (_MSC_VER < 1310)
#  define mrml_TYPENAME
# else
#  define mrml_TYPENAME typename
# endif
#else
# define mrml_TYPENAME typename
#endif

// mrml_EXPORT can not be used
#define mrml_EXPORT

#if (defined(_WIN32) || defined(WIN32)) && !defined(MRMLSTATIC) 
# ifdef mrmlCommon_EXPORTS
#  define mrmlCommon_EXPORT __declspec(dllexport)
# else
#  define mrmlCommon_EXPORT __declspec(dllimport)
# endif  /* mrmlCommon_EXPORT */
#else
/* unix needs nothing */
#define mrmlCommon_EXPORT 
#endif


#endif
