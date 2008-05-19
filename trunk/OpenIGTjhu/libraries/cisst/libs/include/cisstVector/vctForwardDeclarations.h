/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctForwardDeclarations.h,v 1.15 2005/12/28 15:48:15 anton Exp $

  Author(s):  Anton Deguet
  Created on:  2004-10-25

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


#ifndef _vctForwardDeclarations_h
#define _vctForwardDeclarations_h

/*!
  \file
  \brief Forward declarations and \#define for cisstVector
  \ingroup cisstVector
*/

/*!
  \name Definition of the storage order used for the matrices.

  The default for cisstVector is row major (#VCT_ROW_MAJOR) which is
  the default convention for C/C++ code.  In order to support some
  numerical libraries in Fortran (or Fortran compatible), cisstVector
  also support column major order (#VCT_COL_MAJOR). */
//@{

/*! Value to use for a row major storage order. */
const bool VCT_ROW_MAJOR = true;

/*! Value to use for a column major storage order. */
const bool VCT_COL_MAJOR = false;

/*! Default storage order used in cisstVector. */
const bool VCT_DEFAULT_STORAGE = VCT_ROW_MAJOR;

/*! More friendly value to use for Fortran storage. Equivalent to #VCT_COL_MAJOR. */
const bool VCT_FORTRAN_ORDER = VCT_COL_MAJOR;

//@}


/*! Value used to force the normalization of the input for rotation
  constructors.  See classes vctMatrixRotation3Base,
  vctQuaternionRotation3Base, vctAxisAngleRotation3,
  vctRodriguezRotation3Base, etc. */
const bool VCT_NORMALIZE = true;

/*! Value used to NOT force the normalization of the input for
  rotation constructors.  Not only the input will not be normalized,
  but the constructor or method will not check if the input is
  normalized.  See classes vctMatrixRotation3Base,
  vctQuaternionRotation3Base, vctAxisAngleRotation3,
  vctRodriguezRotation3Base, etc. */
const bool VCT_DO_NOT_NORMALIZE = false;


// fixed size vectors
template <unsigned int _size, int _stride, class _elementType, class _dataPtrType>
class vctFixedSizeConstVectorBase;

template <unsigned int _size, int _stride, class _elementType, class _dataPtrType>
class vctFixedSizeVectorBase;

template <class _elementType, unsigned int _size, int _stride>
class vctFixedSizeConstVectorRef;

template <class _elementType, unsigned int _size, int _stride>
class vctFixedSizeVectorRef;

template <class _elementType, unsigned int _size>
class vctFixedSizeVector;


// fixed size matrices
template <unsigned int _rows, unsigned int _cols, int _rowStride, 
          int _colStride, class _elementType, class _dataPtrType>
class vctFixedSizeConstMatrixBase;

template <unsigned int _rows, unsigned int _cols, int _rowStride, 
          int _colStride, class _elementType, class _dataPtrType>
class vctFixedSizeMatrixBase;

template <class _elementType, unsigned int _rows, unsigned int _cols,
          int _rowStride, int _colStride>
class vctFixedSizeConstMatrixRef;

template <class _elementType, unsigned int _rows, unsigned int _cols,
          int _rowStride, int _colStride>
class vctFixedSizeMatrixRef;

template <class _elementType, unsigned int _rows, unsigned int _cols,
          bool _rowMajor = VCT_ROW_MAJOR>
class vctFixedSizeMatrix;


// dynamic vectors
template <class _vectorOwnerType, class _elementType>
class vctDynamicConstVectorBase;

template <class _vectorOwnerType, class _elementType>
class vctDynamicVectorBase;

template <class _elementType>
class vctDynamicConstVectorRef;

template <class _elementType>
class vctDynamicVectorRef;

template <class _elementType>
class vctDynamicVector;

template <class _elementType>
class vctReturnDynamicVector;


// dynamic matrices
template <class _matrixOwnerType, class _elementType>
class vctDynamicConstMatrixBase;

template <class _matrixOwnerType, class _elementType>
class vctDynamicMatrixBase;

template <class _elementType>
class vctDynamicConstMatrixRef;

template <class _elementType>
class vctDynamicMatrixRef;

template <class _elementType>
class vctDynamicMatrix;

template <class _elementType>
class vctReturnDynamicMatrix;


// transformations
template <class _containerType>
class vctMatrixRotation3Base;
template <class _elementType>
class vctMatrixRotation3;

template <class _containerType>
class vctQuaternionBase;
template <class _elementType>
class vctQuaternion;

template <class _containerType>
class vctQuaternionRotation3Base;
template <class _elementType>
class vctQuaternionRotation3;

template <class _elementType>
class vctAxisAngleRotation3;

template <class _containerType>
class vctRodriguezRotation3Base;
template <class _elementType>
class vctRodriguezRotation3;

template <class _containerType>
class vctMatrixRotation2Base;
template <class _elementType>
class vctMatrixRotation2;

class vctAngleRotation2;

template <class _rotationType>
class vctFrameBase;


#endif  // _vctForwardDeclarations_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctForwardDeclarations.h,v $
// Revision 1.15  2005/12/28 15:48:15  anton
// vctForwardDeclarations.h: Use VCT_ROW_MAJOR instead of true.
//
// Revision 1.14  2005/12/08 22:18:42  anton
// vctForwardDeclarations.h: Define VCT_ROW_MAJOR and others constants related
// to storage order as "const bool" instead of #define.
//
// Revision 1.13  2005/12/07 23:14:03  anton
// vctForwardDeclarations.h: Converted VCT_NORMALIZE and VCT_DO_NOT_NORMALIZE
// to const bool.
//
// Revision 1.12  2005/12/01 22:11:57  anton
// cisstVector: 2D transformations, added a base class for rotation matrix 2.
//
// Revision 1.11  2005/11/15 03:26:47  anton
// vctForwardDeclarations: Added VCT_DO_NOT_NORMALIZE
//
// Revision 1.10  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.9  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.8  2005/09/01 06:18:08  anton
// cisstVector transformations: Added a level of abstraction for all classes
// derived from fixed size containers (vctQuaternion, vctQuaternionRotation3,
// vctMatrixRotation3, vctRodriguezRotation3).  These classes are now derived
// from vctXyzBase<_containerType>.  This will ease the SWIG wrapping.
//
// Revision 1.7  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.6  2005/02/24 01:51:50  anton
// cisstVector: Added classes vctAngleRotation2 and vctMatrixRotation2.  This
// code has not been tested.   The normalization and conversion methods have
// to be reviewed.
//
// Revision 1.5  2005/02/04 16:56:54  anton
// cisstVector: Added classes vctAxisAngleRotation3 and vctRodriguezRotation3
// as planed in ticket #109.  Also updated vctMatrixRotation3 to use these new
// classes.  Other updates include vctRandom(rotation), new vctTypes and
// forward declarations.
//
// Revision 1.4  2005/02/03 19:08:10  anton
// vctForwardDeclarations: Added VCT_FORTRAN_ORDER and VCT_NORMALIZE.
//
// Revision 1.3  2004/11/08 18:10:17  anton
// cisstVector: Completed the use of VCT_STORAGE_ORDER, added related Doxygen
// documentation.
//
// Revision 1.2  2004/11/03 22:26:12  anton
// cisstVector:  Better support of storage order.  Added VCT_COL_MAJOR,
// VCT_ROW_MAJOR and VCT_DEFAULT_STORAGE as well as the methods IsColMajor(),
// IsRowMajor(), IsCompact() and IsFortran().
//
// Revision 1.1  2004/10/25 19:19:54  anton
// cisstVector:  Created vctForwardDeclarations.h and removed forward
// declarations of classes in all other files.  Added some constructors
// for vector references to reference fixed size from dynamic and vice versa.
//
//
// ****************************************************************************

