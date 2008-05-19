/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctTypes.h,v 1.19 2005/12/23 21:27:31 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2003-09-12

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
  \brief Typedef for cisstVector
*/


#ifndef _vctTypes_h
#define _vctTypes_h


#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctFixedSizeMatrixTypes.h>
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctDynamicMatrixTypes.h>
#include <cisstVector/vctTransformationTypes.h>


#endif  // _vctTypes_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctTypes.h,v $
// Revision 1.19  2005/12/23 21:27:31  anton
// cisstVector: Minor updates for Doxygen 1.4.5.
//
// Revision 1.18  2005/10/07 09:28:02  anton
// cisstVector: Updated vctTypes to include types by categories (see #60 and #160)
//
// Revision 1.17  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.16  2005/09/23 17:21:23  ofri
// vctTypes.h: Added typedefs for fixed-size matrices of size 1xN and Mx1 with
// 1 <= M,N <= 4
//
// Revision 1.15  2005/07/19 15:29:56  anton
// vctTypes: Added more defined types for matrices.
//
// Revision 1.14  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.13  2005/04/20 19:41:13  anton
// vctTypes: The default frame using float (vctFloatFrm3) was based on a
// quaternion.  It is instantiated using a rotation matrix.
//
// Revision 1.12  2005/02/24 01:51:50  anton
// cisstVector: Added classes vctAngleRotation2 and vctMatrixRotation2.  This
// code has not been tested.   The normalization and conversion methods have
// to be reviewed.
//
// Revision 1.11  2005/02/16 23:04:58  ofri
// vctTypes.h: Added typedef definitions for vctFloatMxN (fixed size
// float matrices)
//
// Revision 1.10  2005/02/04 16:56:54  anton
// cisstVector: Added classes vctAxisAngleRotation3 and vctRodriguezRotation3
// as planed in ticket #109.  Also updated vctMatrixRotation3 to use these new
// classes.  Other updates include vctRandom(rotation), new vctTypes and
// forward declarations.
//
// Revision 1.9  2004/10/21 20:12:41  anton
// cisstVector: Renaming of vctFrame and related typedefs as specified in
// ticket #72.
//
// Revision 1.8  2004/07/13 20:38:21  ofri
// vctTypes: Added type definitions for non-square fixed-size matrices.
//
// Revision 1.7  2004/07/13 16:20:46  ofri
// Updating vctTypes: inclusion of cisstDynamicVector to provide code compilation
// and potentially DLL instantiations.
//
// Revision 1.6  2004/07/12 20:25:48  anton
// cisstVector: Removed redundant #include statements.  These didn't look that
// great on the nice Doxygen graphs.
//
// Revision 1.5  2004/04/06 15:23:15  anton
// Doxygen clean-up
//
// Revision 1.4  2004/03/16 21:17:26  ofri
// Added typedef's for barycentric vectors
//
// Revision 1.3  2004/02/18 22:30:42  anton
// Added DIMENSION for all transformation to simplify the vctFrame template
//
// Revision 1.2  2004/02/18 15:49:37  anton
// Added types related to transformations
//
// Revision 1.1  2003/12/17 15:30:59  anton
// Replaces vctDouble3 etc..
//
//
// ****************************************************************************

