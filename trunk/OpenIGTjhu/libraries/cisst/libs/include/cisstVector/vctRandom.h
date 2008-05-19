/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctRandom.h,v 1.14 2006/02/01 02:29:26 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2004-11-12

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


#ifndef _vctRandom_h
#define _vctRandom_h

#include <cisstCommon/cmnRandomSequence.h>
#include <cisstCommon/cmnPortability.h>

#include <cisstVector/vctForwardDeclarations.h>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicMatrix.h>

#include <cisstVector/vctExport.h>

/*!
  \file
  \brief Declarations of vctRandom functions.
  \ingroup cisstVector
*/

/*!  Define the global function vctRandom to initialize a vector or matrix object
  with random elements.  The function is overloaded to match the various vector
  and matrix types: fixed-size/dynamic and vector/matrix.  The function takes a
  range from which to choose random elements.  The size of the container is taken
  from the container itself.

  \note The function uses the global instance of cmnRandomSequence to extract random
  values.  As we have a vague plan to allow for multiple random sequence objects to
  coexist, these interfaces may need to be changed.
*/
//@{


template <unsigned int _size, int _stride, class _elementType, class _dataPtrType>
void vctRandom(vctFixedSizeVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
               const _elementType min,
               const _elementType max) {
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    const unsigned int size = vector.size();
    unsigned int index;
    for (index = 0; index < size; ++index) {
        randomSequence.ExtractRandomValue(min, max,
                                          vector[index]);    
    }
}


template <unsigned int _rows, unsigned int _cols, int _rowStride, 
          int _colStride, class _elementType, class _dataPtrType>
void vctRandom(vctFixedSizeMatrixBase<_rows, _cols, _rowStride, _colStride,
               _elementType, _dataPtrType> & matrix,
               const _elementType min,
               const _elementType max) {
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    const unsigned int rows = matrix.rows();
    const unsigned int cols = matrix.cols();
    unsigned int rowIndex, colIndex;
    for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
        for (colIndex = 0; colIndex < cols; ++colIndex) {
            randomSequence.ExtractRandomValue(min, max,
                                              matrix.Element(rowIndex, colIndex));    
        }
    }
}


template <class _vectorOwnerType, typename _elementType>
void vctRandom(vctDynamicVectorBase<_vectorOwnerType, _elementType> & vector,
               const typename vctDynamicVectorBase<_vectorOwnerType, _elementType>::value_type min,
               const typename vctDynamicVectorBase<_vectorOwnerType, _elementType>::value_type max)
{
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    const unsigned int size = vector.size();
    unsigned int index;
    for (index = 0; index < size; ++index) {
        randomSequence.ExtractRandomValue(min, max,
                                          vector[index]);    
    }
}


template <class _matrixOwnerType, typename _elementType>
void vctRandom(vctDynamicMatrixBase<_matrixOwnerType, _elementType> & matrix,
               const typename vctDynamicMatrixBase<_matrixOwnerType, _elementType>::value_type min,
               const typename vctDynamicMatrixBase<_matrixOwnerType, _elementType>::value_type max)
{
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    const unsigned int rows = matrix.rows();
    const unsigned int cols = matrix.cols();
    unsigned int rowIndex, colIndex;
    for (rowIndex = 0; rowIndex < rows; ++rowIndex) {
        for (colIndex = 0; colIndex < cols; ++colIndex) {
            randomSequence.ExtractRandomValue(min, max,
                                              matrix.Element(rowIndex, colIndex));    
        }
    }
}


template <class _containerType>
CISST_EXPORT void vctRandom(vctMatrixRotation3Base<_containerType> & matrixRotation);

#ifndef DOXYGEN
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
template CISST_EXPORT void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > &);
template CISST_EXPORT void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > &);
template CISST_EXPORT void vctRandom(vctMatrixRotation3Base<vctDynamicMatrix<double> > &);
#endif
#endif // DOXYGEN


template <class _containerType>
CISST_EXPORT void vctRandom(vctQuaternionRotation3Base<_containerType> & quaternionRotation);

#ifndef DOXYGEN
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
template CISST_EXPORT void vctRandom(vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > &);
template CISST_EXPORT void vctRandom(vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > &);
template CISST_EXPORT void vctRandom(vctQuaternionRotation3Base<vctDynamicVector<double> > &);
#endif
#endif // DOXYGEN


template <class _elementType>
CISST_EXPORT void vctRandom(vctAxisAngleRotation3<_elementType> & axisAngleRotation);

#ifndef DOXYGEN
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
template CISST_EXPORT void vctRandom(vctAxisAngleRotation3<double> &);
template CISST_EXPORT void vctRandom(vctAxisAngleRotation3<float> &);
#endif
#endif // DOXYGEN


template <class _containerType>
CISST_EXPORT void vctRandom(vctRodriguezRotation3Base<_containerType> & rodriguezRotation);

#ifndef DOXYGEN
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
template CISST_EXPORT void vctRandom(vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > &);
template CISST_EXPORT void vctRandom(vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > &);
template CISST_EXPORT void vctRandom(vctRodriguezRotation3Base<vctDynamicVector<double> > &);
#endif
#endif // DOXYGEN


template <class _containerType>
CISST_EXPORT void vctRandom(vctMatrixRotation2Base<_containerType> & matrixRotation);

#ifndef DOXYGEN
#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
template CISST_EXPORT void vctRandom(vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > &);
template CISST_EXPORT void vctRandom(vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > &);
template CISST_EXPORT void vctRandom(vctMatrixRotation2Base<vctDynamicMatrix<double> > &);
#endif
#endif // DOXYGEN

CISST_EXPORT void vctRandom(vctAngleRotation2 & angleRotation);
//@}


#endif  // _vctRandom_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctRandom.h,v $
// Revision 1.14  2006/02/01 02:29:26  anton
// vctRandom: Modified signature for dynamic containers to help the compiler
// cast the min and max bounds.
//
// Revision 1.13  2005/12/01 22:11:57  anton
// cisstVector: 2D transformations, added a base class for rotation matrix 2.
//
// Revision 1.12  2005/11/15 03:23:02  anton
// vctRandom: Added support for vctRodriguezRotation3 and vctAxisAngleRotation3.
//
// Revision 1.11  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.10  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.9  2005/09/01 06:18:08  anton
// cisstVector transformations: Added a level of abstraction for all classes
// derived from fixed size containers (vctQuaternion, vctQuaternionRotation3,
// vctMatrixRotation3, vctRodriguezRotation3).  These classes are now derived
// from vctXyzBase<_containerType>.  This will ease the SWIG wrapping.
//
// Revision 1.8  2005/07/22 18:54:10  ofri
// vctRandom.h: In response to ticket #156, moved the #include of the rotation
// and frame headers to the cpp file.  This seems to reduce the test compilation
// dependencies and time significantly.
//
// Revision 1.7  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.6  2005/04/08 16:39:26  anton
// vctRandom.h: Removed commented code.
//
// Revision 1.5  2005/02/24 01:51:50  anton
// cisstVector: Added classes vctAngleRotation2 and vctMatrixRotation2.  This
// code has not been tested.   The normalization and conversion methods have
// to be reviewed.
//
// Revision 1.4  2005/02/04 16:56:54  anton
// cisstVector: Added classes vctAxisAngleRotation3 and vctRodriguezRotation3
// as planed in ticket #109.  Also updated vctMatrixRotation3 to use these new
// classes.  Other updates include vctRandom(rotation), new vctTypes and
// forward declarations.
//
// Revision 1.3  2004/12/01 17:14:53  anton
// cisstVector:  Replace access to matrix elements by Element() instead of
// double [][] for increased performance.  See ticket #79.
//
// Revision 1.2  2004/11/18 18:48:46  ofri
// Documented function vctRandom
//
// Revision 1.1  2004/11/12 22:09:48  anton
// cisstVector: Added 4 functions to fill different containers with random
// values.  This is based on cmnRandomSequence.
//
//
// ****************************************************************************

