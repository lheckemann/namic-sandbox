/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeMatrix.h,v 1.33 2006/01/26 20:45:22 anton Exp $

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
  \brief Declaration of vctFixedSizeMatrix
 */

#ifndef _vctFixedSizeMatrix_h
#define _vctFixedSizeMatrix_h

#include <cisstVector/vctFixedSizeConstMatrixRef.h>
#include <cisstVector/vctFixedSizeMatrixRef.h>


/*! \brief Implementation of a fixed-size matrix using template
  metaprogramming.
  
  \ingroup cisstVector

  The matrix type is stored as a contiguous array of a fixed size
  (stack allocation).  It provides methods for operations which are
  implemented using template metaprogramming.  See
  vctFixedSizeConstMatrixBase and vctFixedSizeMatrixBase for more
  implementation details.

  \param _elementType the type of an element in the matrix
  \param _rows the number of rows of the matrix
  \param _cols the number of columns of the matrix
  \param _rowMajor the storage order, either #VCT_ROW_MAJOR (default) or #VCT_COL_MAJOR.
*/

template<class _elementType, unsigned int _rows, unsigned int _cols, bool _rowMajor>
class vctFixedSizeMatrix : public vctFixedSizeMatrixBase
<_rows, _cols, _rowMajor?_cols:1, _rowMajor?1:_rows, _elementType, 
 typename vctFixedSizeMatrixTraits<_elementType, _rows, _cols, _rowMajor?_cols:1, _rowMajor?1:_rows>::array >
{
 public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /* no need to document, inherit doxygen documentation from vctFixedSizeVectorBase */
    typedef vctFixedSizeMatrixTraits<_elementType, _rows, _cols, _rowMajor?_cols:1, _rowMajor?1:_rows> MatrixTraits;

    /*! Type of the base class */
    typedef vctFixedSizeMatrixBase<_rows, _cols, _rowMajor?_cols:1, _rowMajor?1:_rows, _elementType, 
        typename vctFixedSizeMatrixTraits<_elementType, _rows, _cols, 
        _rowMajor?_cols:1, _rowMajor?1:_rows>::array >
        BaseType;
    typedef vctFixedSizeMatrix<_elementType, _rows, _cols> ThisType;

    typedef typename MatrixTraits::iterator iterator;
    typedef typename MatrixTraits::const_iterator const_iterator;
    typedef typename MatrixTraits::reverse_iterator reverse_iterator;
    typedef typename MatrixTraits::const_reverse_iterator const_reverse_iterator;

    typedef class cmnTypeTraits<value_type> TypeTraits;
    typedef typename TypeTraits::VaArgPromotion ElementVaArgPromotion;

    /*! Default constructor.  Do nothing. */
    vctFixedSizeMatrix()
    {}

    /*! Initialize all the elements to the given value.
      \param value the value used to set all the elements of the matrix
    */
    explicit inline vctFixedSizeMatrix(const value_type & value) {
        SetAll(value);
    }

    /*! Initialize the elements of the matrix with a set of given
      values.  The assignment is done by the order of the iterators,
      that is, row by row from the low column to the high column in
      each row.

      \note This constructor doesn't check that the correct number of
      elements have been provided.  Use with caution.

      \note Since this method relies on the standard \c va_arg, the
      parameters must be correctly casted.  E.g, \c 3.14 will be
      treated as a double while \c 3.14f will be treated as a float
      and \c 3 will be handled like an integer.
    */
    inline vctFixedSizeMatrix(const value_type element0, const value_type element1, ...)
    {
        CMN_ASSERT( this->size() > 1 );
        (*this).at(0) = element0;
        (*this).at(1) = element1;
        int elementIndex = 2;
        const int numElements = this->size();
        va_list nextArg;
        va_start(nextArg, element1);
        for (; elementIndex < numElements; ++elementIndex) {
            (*this).at(elementIndex) = value_type( va_arg(nextArg, ElementVaArgPromotion) );
        }
        va_end(nextArg);
    }
    
    /*! Initialize the elements of this matrix with values from
      another matrix.  The other matrix can include elements of any
      type, which will be converted using standard conversion to
      elements of this matrix.
    */
    template<class __elementType, int __rowStride, int __colStride, class __dataPtrType>
    explicit inline
    vctFixedSizeMatrix(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, __elementType, __dataPtrType> & matrix) {
        this->Assign(matrix);
    }

  template<int __rowStride, int __colStride, class __elementType, class __dataPtrType>
        inline ThisType & operator = (const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, __elementType,__dataPtrType> & other) {
        // Experimental static_cast replacing reinterpret_cast.  Return the reinterpret_cast if you encounter
        // compilation errors.
        return static_cast<ThisType &>(this->Assign(other));
  }
  
  inline ThisType & operator = (const ThisType & other) {
    return static_cast<ThisType &>(this->Assign(other));
  }
    
  template<int __rowStride, int __colStride>
        inline ThisType & operator = (const vctFixedSizeConstMatrixRef<value_type, _rows, _cols, __rowStride, __colStride> & other)
  {
    return static_cast<ThisType &>(this->Assign(other));
  }

  template<int __rowStride, int __colStride, class __elementType>
    inline ThisType & operator = (const vctFixedSizeConstMatrixRef<__elementType, _rows, _cols, __rowStride, __colStride> & other)
  {
    this->Assign(other);
    return *this;
  }

    /*! Assignement of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value) {
        this->SetAll(value);
        return *this;
    }

};




/*! \name Elementwise operations between fixed size matrices. */
//@{
/*! Operation between fixed size matrices (same size).
  \param matrix1 The first operand of the binary operation.
  \param matrix2 The second operand of the binary operation.
  \return The matrix result of \f$op(matrix1, matrix2)\f$. */
template<unsigned int _rows, unsigned int _cols,
         int _input1RowStride, int _input1ColStride, class _input1Data,
         int _input2RowStride, int _input2ColStride, class _input2Data, class _elementType>
    inline vctFixedSizeMatrix<_elementType, _rows, _cols>
    operator + (const vctFixedSizeConstMatrixBase<_rows, _cols, _input1RowStride, _input1ColStride, _elementType, _input1Data> & matrix1,
                const vctFixedSizeConstMatrixBase<_rows, _cols, _input2RowStride, _input2ColStride, _elementType, _input2Data> & matrix2) {
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.SumOf(matrix1, matrix2);
    return result; 
}

/* documented above */
template<unsigned int _rows, unsigned int _cols,
         int _input1RowStride, int _input1ColStride, class _input1Data,
         int _input2RowStride, int _input2ColStride, class _input2Data, class _elementType>
    inline vctFixedSizeMatrix<_elementType, _rows, _cols>
    operator - (const vctFixedSizeConstMatrixBase<_rows, _cols, _input1RowStride, _input1ColStride, _elementType, _input1Data> & matrix1,
                const vctFixedSizeConstMatrixBase<_rows, _cols, _input2RowStride, _input2ColStride, _elementType, _input2Data> & matrix2) {
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.DifferenceOf(matrix1, matrix2);
    return result; 
}
//@}


/*! \name Elementwise operations between a matrix and a scalar. */
//@{
/*! Operation between a matrix and a scalar.
 \param matrix The first operand of the binary operation.
 \param scalar The second operand of the binary operation.
 \return The matrix result of \f$op(matrix, scalar)\f$. */
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols> 
operator + (const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix,
            const _elementType & scalar)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.SumOf(matrix, scalar);
    return result;
}

/* documented above */
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols> 
operator - (const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix,
            const _elementType & scalar)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.DifferenceOf(matrix, scalar);
    return result;
}

/* documented above */
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols> 
operator * (const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix,
            const _elementType & scalar)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.ProductOf(matrix, scalar);
    return result;
}

/* documented above */
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols> 
operator / (const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix,
            const _elementType & scalar)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.RatioOf(matrix, scalar);
    return result;
}
//@}


/*! \name Elementwise operations between a scalar and a matrix. */
//@{
/*! Operation between a scalar and a matrix.
 \param scalar The first operand of the binary operation.
 \param matrix The second operand of the binary operation.
 \return The matrix result of \f$op(scalar, matrix)\f$. */
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols> 
operator + (const _elementType & scalar,
            const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.SumOf(scalar, matrix);
    return result;
}

/* documented above */
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols> 
operator - (const _elementType & scalar,
            const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.DifferenceOf(scalar, matrix);
    return result;
}

/* documented above */
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols> 
operator * (const _elementType & scalar,
            const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.ProductOf(scalar, matrix);
    return result;
}

/* documented above */
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols> 
operator / (const _elementType & scalar,
            const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.RatioOf(scalar, matrix);
    return result;
}
//@}



/*! \name Elementwise operations on a matrix. */
//@{
/*! Unary operation on a matrix.
  \param matrix The operand of the unary operation
  \return The matrix result of \f$op(matrix)\f$.
*/
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols> 
operator - (const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix)
{
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.NegationOf(matrix);
    return result;
}
//@}


template<unsigned int _rows, unsigned int _cols, unsigned int _input1Cols,
         int _input1RowStride, int _input1ColStride, class _input1DataPtrType, 
         int _input2RowStride, int _input2ColStride, class _input2DataPtrType,
         class _elementType>
inline vctFixedSizeMatrix<_elementType, _rows, _cols>
operator * (const vctFixedSizeConstMatrixBase<_rows, _input1Cols, _input1RowStride, _input1ColStride, _elementType, _input1DataPtrType> & input1Matrix,
            const vctFixedSizeConstMatrixBase<_input1Cols, _cols, _input2RowStride, _input2ColStride, _elementType, _input2DataPtrType> & input2Matrix) {
    vctFixedSizeMatrix<_elementType, _rows, _cols> result;
    result.ProductOf(input1Matrix, input2Matrix);
    return result; 
}


template<unsigned int _rows, unsigned int _cols,
         int _inputMatrixRowStride, int _inputMatrixColStride, class _inputMAtrixDataPtrType, 
         int _inputVectorStride, class _inputVectorDataPtrType,
         class _elementType>
inline vctFixedSizeVector<_elementType, _rows>
operator * (const vctFixedSizeConstMatrixBase<_rows, _cols, _inputMatrixRowStride, _inputMatrixColStride, _elementType, _inputMAtrixDataPtrType> & inputMatrix,
            const vctFixedSizeConstVectorBase<_cols, _inputVectorStride, _elementType, _inputVectorDataPtrType> & inputVector)
{
    vctFixedSizeVector<_elementType, _rows> result;
    result.ProductOf(inputMatrix, inputVector);
    return result; 
}


template<unsigned int _rows, unsigned int _cols,
         int _inputVectorStride, class _inputVectorDataPtrType,
         int _inputMatrixRowStride, int _inputMatrixColStride, class _inputMatrixDataPtrType, 
         class _elementType>
inline vctFixedSizeVector<_elementType, _cols>
operator * (const vctFixedSizeConstVectorBase<_rows, _inputVectorStride, _elementType, _inputVectorDataPtrType> & inputVector,
            const vctFixedSizeConstMatrixBase<_rows, _cols, _inputMatrixRowStride, _inputMatrixColStride, _elementType, _inputMatrixDataPtrType> & inputMatrix)
{
    vctFixedSizeVector<_elementType, _cols> result;
    result.ProductOf(inputVector, inputMatrix);
    return result; 
}


/*
  Methods declared previously and implemented here because they require vctFixedSizeVector
*/
#ifndef DOXYGEN

/* documented in class vctFixedSizeConstMatrixBase */
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
inline typename  vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::MatrixValueType 
vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::Abs(void) const {
    MatrixValueType result;
    vctFixedSizeMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<_elementType>::AbsValue>::
        Run(result, *this);
    return result;
}
    
/* documented in class vctFixedSizeConstMatrixBase */
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
inline typename  vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::MatrixValueType
vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::Negation(void) const {
    MatrixValueType result;
    vctFixedSizeMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<_elementType>::Negation>::
        Run(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstMatrixBase */
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
inline typename  vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::MatrixValueType
vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::Floor(void) const {
    MatrixValueType result;
    vctFixedSizeMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<_elementType>::Floor>::
        Run(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstMatrixBase */
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
inline typename  vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::MatrixValueType
vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::Ceil(void) const {
    MatrixValueType result;
    vctFixedSizeMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<_elementType>::Ceil>::
        Run(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstMatrixBase */
template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride, class _elementType, class _dataPtrType>
typename vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::MatrixValueType
vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>::Eye(void) {
    static MatrixValueType result(_elementType(0));
    result.Diagonal().SetAll(_elementType(1));
    return result;
}

/* documented in class vctFixedSizeConstMatrixBase */
template<unsigned int _rows, unsigned int _cols,
         int _rowStride, int _colStride, class _dataPtrType,
         int __rowStride, int __colStride, class __dataPtrType,
         class _elementType,
         class _elementOperationType>
inline vctFixedSizeMatrix<bool, _rows, _cols>
vctFixedSizeMatrixElementwiseCompareMatrix(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride,
                                           _elementType, _dataPtrType> & matrix1,
                                           const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                                           _elementType, __dataPtrType> & matrix2) {
    vctFixedSizeMatrix<bool, _rows, _cols> result;
    vctFixedSizeMatrixLoopEngines::
        MoMiMi<_elementOperationType>::Run(result, matrix1, matrix2);
    return result;
}

/* documented in class vctFixedSizeConstMatrixBase */
template<unsigned int _rows, unsigned int _cols,
         int _rowStride, int _colStride, class _dataPtrType,
         class _elementType,
         class _elementOperationType>
inline vctFixedSizeMatrix<bool, _rows, _cols>
vctFixedSizeMatrixElementwiseCompareScalar(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride,
                                           _elementType, _dataPtrType> & matrix,
                                           const _elementType & scalar) {
    vctFixedSizeMatrix<bool, _rows, _cols> result;
    vctFixedSizeMatrixLoopEngines::
        MoMiSi<_elementOperationType>::Run(result, matrix, scalar);
    return result;
}
#endif // DOXYGEN


#endif // _vctFixedSizeMatrix_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedSizeMatrix.h,v $
// Revision 1.33  2006/01/26 20:45:22  anton
// cisstVector: Added Eye methods to compute identity for matrices.  See #210.
//
// Revision 1.32  2005/12/23 21:27:31  anton
// cisstVector: Minor updates for Doxygen 1.4.5.
//
// Revision 1.31  2005/12/13 00:04:57  ofri
// All cisstVector containers: Added methods
// 1) MinAndMaxElement  (at const base)
// 2) Floor, Ceil  (at const base -- implemented at container)
// 3) FloorSelf, CeilSelf, FloorOf, CeilOf  (at non-const base)
//
// Revision 1.30  2005/12/02 16:23:54  anton
// cisstVector: Added assigment operator from scalar (see ticket #191).
//
// Revision 1.29  2005/09/27 18:01:25  anton
// cisstVector: Use CMN_ASSERT instead of assert for vectors and matrices.
//
// Revision 1.28  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.27  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.26  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.25  2004/12/01 16:38:41  anton
// vctFixedSizeMatrix: Renamed and re-ordered template parameters for operators
// * to increase readability (see #95).
//
// Revision 1.24  2004/11/30 21:36:32  anton
// vctFixedSize: Bug in operator vector * matrix, input vector size was
// set to matrix numbers of cols instead of rows.  The method ProductOf() was
// correct.  Cleaned-up layout to be able to read and compare both signatures.
//
// Revision 1.23  2004/11/18 20:57:30  ofri
// RE: ticket #92.  Redefined all operator= I could find to return a ThisType &
// instead of const ThisType &.  This included corresponding revision of the
// Assign methods, and also definition from scratch of operator= for fixed and
// dynamic matrix ref.
//
// Revision 1.22  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
// Revision 1.21  2004/11/08 18:10:17  anton
// cisstVector: Completed the use of VCT_STORAGE_ORDER, added related Doxygen
// documentation.
//
// Revision 1.20  2004/10/29 21:23:39  ofri
// vctFixedSizeMatrix: corrected error in operator * (matrix, vector) -- wrong
// definition of the size of the output;  added operator * (vector, matrix)
//
// Revision 1.19  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.18  2004/10/22 18:07:06  anton
// Doxygen: Made sure that all the classes are documented for Doxygen.
//
// Revision 1.17  2004/10/19 22:02:05  anton
// cisstVector: Compilation error with .net 2000 and cleanup by adding the
// MatrixValueType.  See check-in [821] and ticket #75.
//
// Revision 1.16  2004/10/19 20:50:50  anton
// cisstVector: Preliminary support for different storage orders (i.e. row
// major by default, column major if specified).  This has been implemented
// for the vctFixedSizeMatrix as well as the vctDynamicMatrix.  We tested the
// code by simply changinf the default and all current tests passed.  See
// ticket #75 re. vctFortranMatrix.
//
// Revision 1.15  2004/10/14 19:18:01  anton
// cisstVector, fixed size containers: Added operator() based on at() method
// as required per ticket #72.
//
// Revision 1.14  2004/09/24 20:20:42  anton
// cisstVector: Major update related to ticket #18 regarding order operators
// and methods.  The methods named Equal(), NotEqual(), Lesser(),
// LesserOrEqual(), etc.  return a boolean.  The same names with prefix
// Elementwise (e.g. ElementwiseEqual) return a vector or matrix of the tests
// performed for each element.  We only kept the operators == and != which
// return a boolean.  All other operators (>, >=, <, <=) have been removed.
// Also, in order to have the same API for all the containers, many methods
// have been added (vctAny, vctAll, etc... see tickets #52 and #61).  The
// doxygen comments have also been updated.
//
// Revision 1.13  2004/08/13 17:47:40  anton
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
// Revision 1.12  2004/07/27 20:08:06  anton
// cisstVector: Added some Doxygen documentation.  Minor code changes to
// keep the dynamic vectors in sync with the fixed size ones.
//
// Revision 1.11  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.10  2004/03/19 17:01:17  ofri
// Redefined local TypeTraits using keyword "class" instead of "typename".
// Defined local ElementVaArgPromotion type.
//
// Revision 1.9  2004/03/18 20:42:10  ofri
// Fixing va_arg bug in va_arg constructor: type promotion.
//
// Revision 1.8  2004/03/18 19:30:48  ofri
// Documented va_arg constructor
//
// Revision 1.7  2004/03/18 15:43:30  ofri
// 1) Added va_arg constructor for vctFixedSizeMatrix.
// 2) Replaced reinterpret_cast with the safer static_cast.  If it causes
// compilation errors, return the old reinterpret_cast.
//
// Revision 1.6  2004/01/15 15:24:00  anton
// Added multiplication of a matrix by vector using the * operator.  Minor documentation updates
//
// Revision 1.5  2003/12/19 21:46:03  anton
// Added * operator for the multiplication of two matrices
//
// Revision 1.4  2003/12/18 16:20:18  anton
// Added most methods and operators to be consistant with vctFixedSizeVector
//
// Revision 1.3  2003/11/20 18:21:04  anton
// Tabulations
//
// Revision 1.2  2003/11/11 03:47:21  ofri
// Changing order of template argument in references to vctFixedSizeMatrixBase
//
// Revision 1.1  2003/11/04 21:58:49  ofri
// Committing initial version of the code for fixed-size matrices.  Need to
// implement more operations and improve documentation, but the basic
// framework works.
//
// ****************************************************************************
