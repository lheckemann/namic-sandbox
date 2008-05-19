/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnConstants.h,v 1.4 2006/06/23 17:52:07 ofri Exp $
  
  Author(s):  Anton Deguet
  Created on: 2005-10-17

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
  \brief Declaration of various constants
  \ingroup cisstCommon
*/

#ifndef _cmnConstants_h
#define _cmnConstants_h


#include <cisstCommon/cmnExport.h>


/*! PI */
const double cmnPI = 3.1415926535897932384626433832795029;

/*! PI / 2 */
const double cmnPI_2 = 1.5707963267948966192313216916397514;

/*! PI / 4 */
const double cmnPI_4 = 0.7853981633974483096156608458198757;

/*! PI / 180 : convert degrees to radians */
const double cmnPI_180 = cmnPI / 180.0;


#endif // _cmnConstants_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnConstants.h,v $
// Revision 1.4  2006/06/23 17:52:07  ofri
// cmnConstants.h : Added constant cmnPI_180 to convert degrees to radians.
//
// Revision 1.3  2006/01/25 22:16:53  anton
// cmnConstants.h: Removed confusing cmn2_PI following 2006-01-25 meeting.
//
// Revision 1.2  2005/10/24 20:42:24  anton
// cmnConstants: Moved "instantiation" to header file using explicit values.
// This should help the compiler to optimize some code (see #131).
//
// Revision 1.1  2005/10/17 23:50:02  anton
// cmnConstants: Added first version with cmnPI, cmn2_PI, cmnPI_2 and cmnPI_4
// (see ticket #113).
//
//
// ****************************************************************************
