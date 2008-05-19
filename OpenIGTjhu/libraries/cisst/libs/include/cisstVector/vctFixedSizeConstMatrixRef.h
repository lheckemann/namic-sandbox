/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeConstMatrixRef.h,v 1.15 2005/09/27 18:01:25 anton Exp $

  Author(s):  Ofri Sadowsky
  Created on:  2003-11-04

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
  \brief Declaration of vctFixedSizeConstMatrixRef
 */

#ifndef _vctFixedSizeConstMatrixRef_h
#define _vctFixedSizeConstMatrixRef_h


#include <cisstVector/vctFixedSizeMatrixBase.h>


/*!  \brief An implementation of the ``abstract''
  vctFixedSizeConstMatrixBase.

  \ingroup cisstVector

  This implementations uses a pointer to the matrix beginning as the
  matrix defining data member.  An instantiation of this type can be
  used as a matrix reference with TransposeRef().

  Note that the class provides only const operations, except for
  assigning the matrix start, which does not affect the matrix.
  This is despite the fact that the stored pointer is non-const.
 
  See the base class (vctFixedSizeConstMatrixBase) for template
  parameter details.
*/
template<class _elementType, unsigned int _rows, unsigned int _cols,
         int _rowStride, int _colStride>
class vctFixedSizeConstMatrixRef : public vctFixedSizeConstMatrixBase
  <_rows, _cols,_rowStride, _colStride, _elementType,
   typename vctFixedSizeMatrixTraits<_elementType, _rows, _cols, _rowStride, _colStride>::pointer>
{
 public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    typedef vctFixedSizeMatrixTraits
        <_elementType, _rows, _cols, _rowStride, _colStride> MatrixTraits;

    typedef typename MatrixTraits::iterator iterator;
    typedef typename MatrixTraits::const_iterator const_iterator;
    typedef typename MatrixTraits::reverse_iterator reverse_iterator;
    typedef typename MatrixTraits::const_reverse_iterator const_reverse_iterator;
    
    typedef vctFixedSizeConstMatrixRef
        <value_type, _rows, _cols, _rowStride, _colStride> ThisType;
    typedef vctFixedSizeConstMatrixBase
        <_rows, _cols, _rowStride, _colStride, value_type, 
        typename MatrixTraits::pointer> BaseType;
    
    /*! Default constructor: create an uninitialized matrix */
    inline vctFixedSizeConstMatrixRef() 
        {}
    
    /*! Initialize the matrix with a (non-const) pointer */
    inline vctFixedSizeConstMatrixRef(pointer p) {
        SetRef(p);
    }

    /*! Convenience constructor to initialize a reference to a fixed-size matrix object.
      Since the argument is const, there is no worry about explicit instantiation.
    */
    template<unsigned int __rows, unsigned int __cols, class __dataPtrType>
    inline vctFixedSizeConstMatrixRef( 
        const vctFixedSizeConstMatrixBase<__rows, __cols, _rowStride, _colStride, _elementType, __dataPtrType>
        & matrix)
    {
        SetRef(matrix, 0, 0);
    }

    /*! Convenience constructor to initialize a reference to a fixed-size matrix object.
    */
    template<unsigned int __rows, unsigned int __cols, class __dataPtrType>
    inline vctFixedSizeConstMatrixRef( 
        const vctFixedSizeConstMatrixBase<__rows, __cols, _rowStride, _colStride, _elementType, __dataPtrType>
        & matrix, size_type startRow, size_type startCol )
    {
        SetRef(matrix, startRow, startCol);
    }

    /*! Convenience constructor to initialize a fixed-size reference to a dynamic matrix.
      The involves assertion that the sizes and strides match */
    template<class __matrixOwnerType>
    inline vctFixedSizeConstMatrixRef(
        const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
        size_type startRow, size_type startCol )
    {
        SetRef(matrix, startRow, startCol);
    }

    /*! Initialize the matrix with a const pointer.  This requires
      const_cast. */
    inline vctFixedSizeConstMatrixRef(const_pointer p) {
        SetRef(p);
    }
    
    /*! Assign the matrix start with a (non-const) pointer */
    inline void SetRef(pointer p) {
        this->Data = p;
    }
    
    /*! Assign the matrix start with a const pointer.  This requires const_cast. */
    inline void SetRef(const_pointer p) {
        this->Data = const_cast<pointer>(p);
    }

    /*! Convenience method to set a reference to a fixed-size matrix object.
      \param matrix the fixed size matrix to be referenced
      \param startRow the row index from which reference will start
      \param startCol the column index from which reference will start
      The strides of this matrix must be identical to the strides of the other
      matrix.
      \note This method asserts that the size of this matrix does not exceed the
      size of the input matrix (see #CMN_ASSERT).
    */
    template<unsigned int __rows, unsigned int __cols, class __dataPtrType>
    inline void SetRef( 
        const vctFixedSizeConstMatrixBase<__rows, __cols, _rowStride, _colStride, _elementType, __dataPtrType>
        & matrix, size_type startRow, size_type startCol )
    {
        CMN_ASSERT( (startRow + this->rows() <= matrix.rows()) && (startCol + this->cols() <= matrix.cols()) );
        SetRef(matrix.Pointer(startRow, startCol));
    }

    /*! Convenience method to set a reference to a dynamic matrix object.
      \param matrix the fixed size matrix to be referenced
      \param startRow the row index from which reference will start
      \param startCol the column index from which reference will start
      The strides of this matrix must be identical to the strides of the other
      matrix.
      \note This method asserts that the size of this matrix does not exceed the
      size of the input matrix (see CMN_ASSERT).
    */
    template<class __matrixOwnerType>
    inline void SetRef( const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
    size_type startRow, size_type startCol)
    {
        CMN_ASSERT( (this->row_stride() == matrix.stride()) && (this->col_stride() == matrix.col_stride()) );
        CMN_ASSERT( (startRow + this->rows() <= matrix.rows()) && (startCol + this->cols() <= matrix.cols()) );
        SetRef(matrix.Pointer(startRow, startCol));
    }

};


template<unsigned int _rows, unsigned int _cols, int _rowStride, 
         int _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeConstMatrixRef<_elementType, _cols, _rows, _colStride, _rowStride>
vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride,
                            _elementType, _dataPtrType>
::TransposeRef() const {
    return ConstRefTransposeType(Data);
}


template<unsigned int _rows, unsigned int _cols, int _rowStride, 
         int _colStride, class _elementType, class _dataPtrType>
typename vctFixedSizeMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::RefTransposeType
inline vctFixedSizeMatrixBase<_rows, _cols, _rowStride, _colStride,
                       _elementType, _dataPtrType>
::TransposeRef(void) {
    return RefTransposeType(this->Data);
}


#endif  // _vctFixedSizeConstMatrixRef_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedSizeConstMatrixRef.h,v $
// Revision 1.15  2005/09/27 18:01:25  anton
// cisstVector: Use CMN_ASSERT instead of assert for vectors and matrices.
//
// Revision 1.14  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.13  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.12  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.11  2005/05/10 21:58:33  ofri
// vctFixedSize[Const]MatrixRef: Added a constructor from a matrix without
// specific row and column.  Fixed a bug in SetRef, which did not use the
// row and column arguments.
//
// Revision 1.10  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
// Revision 1.9  2004/11/08 18:10:17  anton
// cisstVector: Completed the use of VCT_STORAGE_ORDER, added related Doxygen
// documentation.
//
// Revision 1.8  2004/10/26 15:20:52  ofri
// Updating subsequence interfaces (ticket #76) and interfaces between fixed
// and dynamic vectors/matrices (ticket #72).
// *: vctFixedSize(Const)VectorBase::Get(Const)Subsequence now deprecated.
// Replaced by Get(Const)Subvector with equal stride to the parent.
// *: Added constructors for [Fixed|Dynamic][Vector|Matrix]Ref from the other
// storage type.
//
// Revision 1.7  2004/10/25 19:19:54  anton
// cisstVector:  Created vctForwardDeclarations.h and removed forward
// declarations of classes in all other files.  Added some constructors
// for vector references to reference fixed size from dynamic and vice versa.
//
// Revision 1.6  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.5  2004/10/14 19:00:44  ofri
// vctFixedSizeConstMatrixRef and vctFixedSizeMatrixRef: Added constructor
// and SetRef() from other fixed- and dynamic-size matrix objects. See ticket #72.
// Eliminated initialization of non-const reference with const pointer.
// See ticket #73.
//
// Revision 1.4  2004/08/13 17:47:40  anton
// cisstVector: Massive renaming to replace the word "sequence" by "vector"
// (see ticket #50) as well as another more systematic naming for the engines.
// The changes for the API are as follow:
// *: vctFixedLengthSequenceBase -> vctFixedSizeVectorBase (and Const)
// *: vctFixedLengthSequenceRef -> vctFixedSizeVectorRef (and Const)
// *: vctDynamicSequenceBase -> vctDynamicVectorBase (and Const)
// *: vctDynamicSequenceRef -> vctDynamicVectorRef (and Const)
// *: vctDynamicSequenceRefOwner -> vctDynamicVectorRefOwner
// *: vctFixedStrideSequenceIterator -> vctFixedStrideVectorIterator (and Const)
// *: vctVarStrideSequenceIterator -> vctVarStrideVectorIterator (and Const)
// *: vctSequenceRecursiveEngines -> vctFixedSizeVectorRecursiveEngines
// *: vctSequenceLoopEngines -> vctDynamicVectorLoopEngines
// *: vctMatrixLoopEngines -> vctFixedSizeMatrixLoopEngines
// *: vctDynamicMatrixEngines -> vctDynamicMatrixLoopEngines
// Also updated and corrected the documentation (latex, doxygen, figures) as
// well as the tests and examples.
//
// Revision 1.3  2004/07/27 20:08:06  anton
// cisstVector: Added some Doxygen documentation.  Minor code changes to
// keep the dynamic vectors in sync with the fixed size ones.
//
// Revision 1.2  2004/03/18 15:43:59  ofri
// Declared method with the keyword inline
//
// Revision 1.1  2003/11/20 18:25:05  anton
// Splitted const and non const classes in two files.
//
//
// Revision 1.3  2003/11/14 22:04:10  anton
// dos2unix
//
// Revision 1.2  2003/11/11 03:53:49  ofri
// Changing order of template argument in references to vctFixedSizeMatrixBase
//
// Revision 1.1  2003/11/04 21:58:49  ofri
// Committing initial version of the code for fixed-size matrices.  Need to
// implement more operations and improve documentation, but the basic
// framework works.
//
// ****************************************************************************
