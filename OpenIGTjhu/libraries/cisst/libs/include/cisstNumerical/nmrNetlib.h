/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrNetlib.h,v 1.4 2006/01/11 20:53:17 anton Exp $

  Author(s): Anton Deguet
  Created on: 2005-11-24

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
*/


#ifndef _nmrNetlib_h
#define _nmrNetlib_h

#include <cisstConfig.h>

// sanity check
#if CISST_HAS_CNETLIB && CISST_HAS_CISSTNETLIB
#warning "Can't have both cnetlib and cisstNetlib"
#endif

// load correct header file
#if CISST_HAS_CNETLIB
#define CISST_HAS_NETLIB
#include <cnetlib.h>
// f2c.h defines these as macros
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#elif CISST_HAS_CISSTNETLIB
#define CISST_HAS_NETLIB
#include <cisstNetlib.h>
#endif


/*!
  \var typedef long int F_INTEGER

  Type used for sizes in Fortran routines.  This type differs from the
  default size_type in cisst (unsigned int) therefore an explicit cast
  should be used whenever a Fortran routine is called. */ 
typedef long int F_INTEGER;


#endif // _nmrNetlib_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrNetlib.h,v $
//  Revision 1.4  2006/01/11 20:53:17  anton
//  cisstNumerical Code: Introduced CISST_HAS_NETLIB to indicate that any
//  netlib distribution is available (either cnetlib or cisstNetlib).
//
//  Revision 1.3  2005/12/23 21:32:39  anton
//  nmrNetlib.h: Added documentation for F_INTEGER.
//
//  Revision 1.2  2005/12/20 16:35:27  anton
//  nmrNetlib: Introduced F_INTEGER for long int
//
//  Revision 1.1  2005/11/29 02:56:30  anton
//  cisstNumerical: Added nmrNetlib to centralize #if for CISST_HAS_CNETLIB
//  versus CISST_HAS_CISSTNETLIB.  Also, #undef min and max if defined in
//  f2c.h.
//
//
// ****************************************************************************
