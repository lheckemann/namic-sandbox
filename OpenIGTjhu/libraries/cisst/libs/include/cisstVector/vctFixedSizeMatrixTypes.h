/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeMatrixTypes.h,v 1.2 2005/12/23 21:27:31 anton Exp $
  
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
  \brief Typedef for fixed size matrices
*/


#ifndef _vctFixedSizeMatrixTypes_h
#define _vctFixedSizeMatrixTypes_h

#include <cisstVector/vctFixedSizeMatrix.h>

/*! Specialization of vctFixedSizeMatrix for different types of
  elements and sizes. */
//@{
typedef vctFixedSizeMatrix<double, 1, 1> vctDouble1x1;
typedef vctFixedSizeMatrix<double, 1, 1> vct1x1;
typedef vctFixedSizeMatrix<double, 1, 2> vctDouble1x2;
typedef vctFixedSizeMatrix<double, 1, 2> vct1x2;
typedef vctFixedSizeMatrix<double, 1, 3> vctDouble1x3;
typedef vctFixedSizeMatrix<double, 1, 3> vct1x3;
typedef vctFixedSizeMatrix<double, 1, 4> vctDouble1x4;
typedef vctFixedSizeMatrix<double, 1, 4> vct1x4;

typedef vctFixedSizeMatrix<double, 2, 1> vctDouble2x1;
typedef vctFixedSizeMatrix<double, 2, 1> vct2x1;
typedef vctFixedSizeMatrix<double, 2, 2> vctDouble2x2;
typedef vctFixedSizeMatrix<double, 2, 2> vct2x2;
typedef vctFixedSizeMatrix<double, 2, 3> vctDouble2x3;
typedef vctFixedSizeMatrix<double, 2, 3> vct2x3;
typedef vctFixedSizeMatrix<double, 2, 4> vctDouble2x4;
typedef vctFixedSizeMatrix<double, 2, 4> vct2x4;

typedef vctFixedSizeMatrix<double, 3, 1> vctDouble3x1;
typedef vctFixedSizeMatrix<double, 3, 1> vct3x1;
typedef vctFixedSizeMatrix<double, 3, 2> vctDouble3x2;
typedef vctFixedSizeMatrix<double, 3, 2> vct3x2;
typedef vctFixedSizeMatrix<double, 3, 3> vctDouble3x3;
typedef vctFixedSizeMatrix<double, 3, 3> vct3x3;
typedef vctFixedSizeMatrix<double, 3, 4> vctDouble3x4;
typedef vctFixedSizeMatrix<double, 3, 4> vct3x4;

typedef vctFixedSizeMatrix<double, 4, 1> vctDouble4x1;
typedef vctFixedSizeMatrix<double, 4, 1> vct4x1;
typedef vctFixedSizeMatrix<double, 4, 2> vctDouble4x2;
typedef vctFixedSizeMatrix<double, 4, 2> vct4x2;
typedef vctFixedSizeMatrix<double, 4, 3> vctDouble4x3;
typedef vctFixedSizeMatrix<double, 4, 3> vct4x3;
typedef vctFixedSizeMatrix<double, 4, 4> vctDouble4x4;
typedef vctFixedSizeMatrix<double, 4, 4> vct4x4;


typedef vctFixedSizeMatrix<float, 1, 1> vctFloat1x1;
typedef vctFixedSizeMatrix<float, 1, 2> vctFloat1x2;
typedef vctFixedSizeMatrix<float, 1, 3> vctFloat1x3;
typedef vctFixedSizeMatrix<float, 1, 4> vctFloat1x4;

typedef vctFixedSizeMatrix<float, 2, 1> vctFloat2x1;
typedef vctFixedSizeMatrix<float, 2, 2> vctFloat2x2;
typedef vctFixedSizeMatrix<float, 2, 3> vctFloat2x3;
typedef vctFixedSizeMatrix<float, 2, 4> vctFloat2x4;

typedef vctFixedSizeMatrix<float, 3, 1> vctFloat3x1;
typedef vctFixedSizeMatrix<float, 3, 2> vctFloat3x2;
typedef vctFixedSizeMatrix<float, 3, 3> vctFloat3x3;
typedef vctFixedSizeMatrix<float, 3, 4> vctFloat3x4;

typedef vctFixedSizeMatrix<float, 4, 1> vctFloat4x1;
typedef vctFixedSizeMatrix<float, 4, 2> vctFloat4x2;
typedef vctFixedSizeMatrix<float, 4, 3> vctFloat4x3;
typedef vctFixedSizeMatrix<float, 4, 4> vctFloat4x4;


typedef vctFixedSizeMatrix<int, 1, 1> vctInt1x1;
typedef vctFixedSizeMatrix<int, 1, 2> vctInt1x2;
typedef vctFixedSizeMatrix<int, 1, 3> vctInt1x3;
typedef vctFixedSizeMatrix<int, 1, 4> vctInt1x4;

typedef vctFixedSizeMatrix<int, 2, 1> vctInt2x1;
typedef vctFixedSizeMatrix<int, 2, 2> vctInt2x2;
typedef vctFixedSizeMatrix<int, 2, 3> vctInt2x3;
typedef vctFixedSizeMatrix<int, 2, 4> vctInt2x4;

typedef vctFixedSizeMatrix<int, 3, 1> vctInt3x1;
typedef vctFixedSizeMatrix<int, 3, 2> vctInt3x2;
typedef vctFixedSizeMatrix<int, 3, 3> vctInt3x3;
typedef vctFixedSizeMatrix<int, 3, 4> vctInt3x4;

typedef vctFixedSizeMatrix<int, 4, 1> vctInt4x1;
typedef vctFixedSizeMatrix<int, 4, 2> vctInt4x2;
typedef vctFixedSizeMatrix<int, 4, 3> vctInt4x3;
typedef vctFixedSizeMatrix<int, 4, 4> vctInt4x4;

typedef vctFixedSizeMatrix<char, 1, 1> vctChar1x1;
typedef vctFixedSizeMatrix<char, 2, 2> vctChar2x2;
typedef vctFixedSizeMatrix<char, 3, 3> vctChar3x3;
typedef vctFixedSizeMatrix<char, 4, 4> vctChar4x4;
//@}

#endif  // _vctFixedSizeMatrixTypes_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedSizeMatrixTypes.h,v $
// Revision 1.2  2005/12/23 21:27:31  anton
// cisstVector: Minor updates for Doxygen 1.4.5.
//
// Revision 1.1  2005/10/07 09:26:28  anton
// cisstVector: Split vctTypes in multiples files (see #60 and #160).
//
//
// ****************************************************************************

