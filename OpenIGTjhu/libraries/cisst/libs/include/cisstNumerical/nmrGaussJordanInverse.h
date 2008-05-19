/*
  $Id: nmrGaussJordanInverse.h,v 1.3 2006/01/31 21:12:47 anton Exp $
  
  Author(s):  Ofri Sadowsky
  Created on: 2005-09-23

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

#ifndef _nmrGaussJordanInverse_h
#define _nmrGaussJordanInverse_h

/*!
  \file
*/

#include <cisstCommon/cmnPortability.h>
#include <cisstVector/vctForwardDeclarations.h>

// Always the last file to include!
#include <cisstNumerical/nmrExport.h>

/*!
  \name Gauss Jordan Inverse for fixed size matrices

  The function nmrGaussJordanInverseNxN (N = 2, 3, 4) computes the
  inverse of a NxN matrix using Gauss-Jordan elimination.

  The function is instantiated for matrices of double and float in
  either storage order.  When compiled in release mode, this function
  is at least 2.5 times faster than a parallel function in C-LAPACK
  based on LU decomposition (dgetrf, dgetri), with an equvalent
  precision.  However, the speed advantage is counted here only
  in RELEASE mode, whereas in debug mode these functions are
  significantly slower.

  The functions have been optimized to run for specific and typically
  occuring fixed-size cases, namely 2x2, 3x3 and 4x4.  However, if all
  you need is to solve one linear equation, it is more efficient to have
  a direct solver than to compute the inverse first.

  In terms of numerical stability, the Gauss-Jordan method should be
  fairly stable for the specific matrix sizes in question, but it
  only uses row pivoting, and therefore other methods that include
  also column pivoting should be more stable.

  \param A the matrix whose inverse is computed, passed by value.
  \param nonsingular (output) set to true if A is nonsingular, and
  to false if A is singular.
  \param Ainv (output) set to the inverse of A if A is nonsingular
  \param singularityTolerance a tolerance value for determining when
  a near-zero is encountered on the diagonal, which indicates that
  A is singular.

  \note For the 2x2 case, it may be more efficient and not less
  stable to use Kramer's rule.
*/

//@{
/*! Gauss Jordan Inverse for a 2 by 2 matrix. */
template<class _elementType, bool _rowMajorIn, bool _rowMajorOut>
CISST_EXPORT
void nmrGaussJordanInverse2x2(
    vctFixedSizeMatrix<_elementType, 2, 2, _rowMajorIn> A,
    bool & nonsingular,
    vctFixedSizeMatrix<_elementType, 2, 2, _rowMajorOut> & Ainv,
    const _elementType singularityTolerance);

/*! Gauss Jordan Inverse for a 3 by 3 matrix. */
template<class _elementType, bool _rowMajorIn, bool _rowMajorOut>
CISST_EXPORT
void nmrGaussJordanInverse3x3(
    vctFixedSizeMatrix<_elementType, 3, 3, _rowMajorIn> A,
    bool & nonsingular,
    vctFixedSizeMatrix<_elementType, 3, 3, _rowMajorOut> & Ainv,
    const _elementType singularityTolerance);

/*! Gauss Jordan Inverse for a 4 by 4 matrix. */
template<class _elementType, bool _rowMajorIn, bool _rowMajorOut>
CISST_EXPORT
void nmrGaussJordanInverse4x4(
    vctFixedSizeMatrix<_elementType, 4, 4, _rowMajorIn> A,
    bool & nonsingular,
    vctFixedSizeMatrix<_elementType, 4, 4, _rowMajorOut> & Ainv,
    const _elementType singularityTolerance);
//@}


#ifndef DOXYGEN
#ifdef CISST_COMPILER_IS_MSVC
// --- double 2x2 ---
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<double, 2, 2, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 2, 2, VCT_ROW_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<double, 2, 2, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 2, 2, VCT_COL_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<double, 2, 2, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 2, 2, VCT_ROW_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<double, 2, 2, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 2, 2, VCT_COL_MAJOR> &,
                         const double);
// --- float 2x2 ---
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<float, 2, 2, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 2, 2, VCT_ROW_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<float, 2, 2, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 2, 2, VCT_COL_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<float, 2, 2, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 2, 2, VCT_ROW_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse2x2(vctFixedSizeMatrix<float, 2, 2, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 2, 2, VCT_COL_MAJOR> &,
                         const float);
// --- double 3x3 ---
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 3, 3, VCT_ROW_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 3, 3, VCT_COL_MAJOR> &,
                         const double);
// --- float 3x3 ---
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 3, 3, VCT_ROW_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse3x3(vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 3, 3, VCT_COL_MAJOR> &,
                         const float);
// --- double 4x4 ---
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<double, 4, 4, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 4, 4, VCT_ROW_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<double, 4, 4, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 4, 4, VCT_ROW_MAJOR> &,
                         const double);
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<double, 4, 4, VCT_COL_MAJOR> &,
                         const double);
// --- float 4x4 ---
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<float, 4, 4, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 4, 4, VCT_ROW_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<float, 4, 4, VCT_ROW_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 4, 4, VCT_ROW_MAJOR> &,
                         const float);
template CISST_EXPORT void
nmrGaussJordanInverse4x4(vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR>, bool &,
                         vctFixedSizeMatrix<float, 4, 4, VCT_COL_MAJOR> &,
                         const float);
#endif // CISST_COMPILER_IS_MSVC
#endif // DOXYGEN

#endif  // _nmrGaussJordanInverse_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrGaussJordanInverse.h,v $
//  Revision 1.3  2006/01/31 21:12:47  anton
//  cisstNumerical: Added flags for deprecated symbols to avoid warnings when
//  the deprecated classes are not really used.
//
//  Revision 1.2  2006/01/05 16:44:26  anton
//  nmrGaussJordanInverse.h: Explicit declaration of instantiated functions for
//  Microsoft compilers and minor Doxygen update.
//
//  Revision 1.1  2006/01/04 22:50:47  ofri
//  Added functions nmrGaussJordanInverseNxN for N=2, 3, 4.  Tests were successful.
//
//
// ****************************************************************************


