/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDeterminant.h,v 1.7 2005/09/26 15:41:47 anton Exp $
  
  Author(s):  Ofri Sadowsky
  Created on:  2004-04-16

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


#ifndef _vctDeterminant_h
#define _vctDeterminant_h

#include <cisstVector/vctFixedSizeConstMatrixBase.h>
/*!
  \file
  \brief Defines vctDeterminant
*/

/*!
  \brief Determinant function for fixed size matrices
  
  Compute the determinant of a fixed size square matrix.  This
  templated class is currently specialized for matrices of size 1 by
  1, 2 by 2 or 3 by 3.

  \param _size The size of the square matrix
*/
template<unsigned int _size>
class vctDeterminant
{
public:
    enum {SIZE = _size};
    /*!
      Actually compute the determinant of the matrix.

      \param matrix A fixed size square matrix
    */
    template<int _rowStride, int _colStride, class _elementType, class _dataPtrType>
    static _elementType Compute(const vctFixedSizeConstMatrixBase<_size, _size, _rowStride, _colStride, _elementType, _dataPtrType> & matrix);
};


#ifndef DOXYGEN

template<>
class vctDeterminant<1>
{
public:
    enum {SIZE = 1};
    template<int _rowStride, int _colStride, class _elementType, class _dataPtrType>
    static _elementType Compute(const vctFixedSizeConstMatrixBase<SIZE, SIZE, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
    {
        return matrix.Element(0, 0);
    }
};


template<>
class vctDeterminant<2>
{
public:
    enum {SIZE = 2};
    template<int _rowStride, int _colStride, class _elementType, class _dataPtrType>
    static _elementType Compute(const vctFixedSizeConstMatrixBase<SIZE, SIZE, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
    {
        return matrix.Element(0, 0) * matrix.Element(1, 1) - matrix.Element(0, 1) * matrix.Element(1, 0);
    }
};


template<>
class vctDeterminant<3>
{
public:
    enum {SIZE = 3};
    template<int _rowStride, int _colStride, class _elementType, class _dataPtrType>
    static _elementType Compute(const vctFixedSizeConstMatrixBase<SIZE, SIZE, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
    {
        return
            matrix.Element(0, 0) * ( matrix.Element(1, 1) * matrix.Element(2, 2) - matrix.Element(1, 2) * matrix.Element(2, 1) ) -
            matrix.Element(0, 1) * ( matrix.Element(1, 0) * matrix.Element(2, 2) - matrix.Element(1, 2) * matrix.Element(2, 0) ) +
            matrix.Element(0, 2) * ( matrix.Element(1, 0) * matrix.Element(2, 1) - matrix.Element(1, 1) * matrix.Element(2, 0) );
    }
};


#endif // DOXYGEN

#endif  // _vctDeterminant_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctDeterminant.h,v $
// Revision 1.7  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.6  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.5  2004/12/01 17:14:53  anton
// cisstVector:  Replace access to matrix elements by Element() instead of
// double [][] for increased performance.  See ticket #79.
//
// Revision 1.4  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.3  2004/09/03 19:53:52  anton
// vctDeterminant: Documentation.
//
// Revision 1.2  2004/04/20 21:24:04  ofri
// Attempting to fix compilation error on gcc
//
// Revision 1.1  2004/04/20 21:17:51  ofri
// class vctDeterminant added to the repository after being tested on .NET
//
//
// ****************************************************************************
