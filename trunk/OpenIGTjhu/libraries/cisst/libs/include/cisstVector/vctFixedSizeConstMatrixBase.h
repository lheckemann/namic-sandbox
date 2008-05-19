/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeConstMatrixBase.h,v 1.49 2006/07/03 17:31:56 dli Exp $
  
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
  \brief Declaration of vctFixedSizeConstMatrixBase
 */

#ifndef _vctFixedSizeConstMatrixBase_h
#define _vctFixedSizeConstMatrixBase_h


#include <cisstVector/vctFixedSizeVectorRef.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctFixedSizeMatrixTraits.h>
#include <cisstVector/vctFixedSizeMatrixLoopEngines.h>

#include <stdio.h>

/* Forward declarations */
#ifndef DOXYGEN
template<unsigned int _rows, unsigned int _cols,
         int _rowStride, int _colStride, class _dataPtrType,
         int __rowStride, int __colStride, class __dataPtrType,
         class _elementType,
         class _elementOperationType>
inline vctFixedSizeMatrix<bool, _rows, _cols>
vctFixedSizeMatrixElementwiseCompareMatrix(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride,
                                           _elementType, _dataPtrType> & matrix1,
                                           const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                                           _elementType, __dataPtrType> & matrix2);

template<unsigned int _rows, unsigned int _cols,
         int _rowStride, int _colStride, class _dataPtrType,
         class _elementType,
         class _elementOperationType>
inline vctFixedSizeMatrix<bool, _rows, _cols>
vctFixedSizeMatrixElementwiseCompareScalar(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride,
                                           _elementType, _dataPtrType> & matrix,
                                           const _elementType & scalar);
#endif // DOXYGEN

/*!  \brief A template for a fixed size matrix with fixed spacing in
  memory.

  The matrix can be defined over an existing memory block, or define
  its own memory block.  The class's methods follow the STL
  specifications for Random Access Container.  It uses
  vctFixedSizeMatrixTraits to define the types required by the STL
  interface.

  This class defines a read-only memory matrix.  The read/write matrix
  is derived from this class (compare with
  vctFixedStrideMatrixConstIterator).  Therefore, all the methods of
  this class are const.  The non-const matrix and methods are
  defined in vctFixedSizeMatrixBase.

  Matrix indexing is zero-based.

  The method provided for the compatibility with the STL containers
  start with a lower case.  Other methods start with a capitalilized
  letter (see CISST naming convention).

  \param _elementType the type of element in the matrix.

  \param _rows the number of rows that the matrix provides access to.

  \param _cols the number of columns that the matrix provides access
  to.

  \param _rowStride the spacing between the rows in the memory block.

  \param _colStride the spacing between the columns in the memory
  block.

  \param _dataPtrType the type of object that defines the matrix.
  This may be an element pointer of an acutal fixed-size C-style array
  type.

  \sa vctFixedStrideMatrixConstIterator vctFixedSizeMatrixTraits
*/
template<unsigned int _rows, unsigned int _cols, int _rowStride, 
         int _colStride, class _elementType, class _dataPtrType>
class vctFixedSizeConstMatrixBase
{
 public:
    /* Declare the container-defined typed required by STL, plus the
       types completed by our traits class */

    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! Type of the matrix itself. */
    typedef vctFixedSizeConstMatrixBase<_rows, _cols, 
        _rowStride, _colStride, _elementType, _dataPtrType> ThisType;

    /*! Traits used for all useful types related to a vctFixedSizeMatrix */
    typedef vctFixedSizeMatrixTraits<_elementType, _rows, _cols, 
        _rowStride, _colStride> MatrixTraits;
    
    /*! Iterator on the elements of the matrix. */
    typedef typename MatrixTraits::iterator iterator;

    /*! Const iterator on the elements of the matrix. */
    typedef typename MatrixTraits::const_iterator const_iterator;

    /*! Reverse iterator on the elements of the matrix. */
    typedef typename MatrixTraits::reverse_iterator reverse_iterator;

    /*! Const reverse iterator on the elements of the matrix. */
    typedef typename MatrixTraits::const_reverse_iterator const_reverse_iterator;

    /*! Define the dimensions of the matrix */
    enum {ROWS = MatrixTraits::ROWS, COLS = MatrixTraits::COLS,
          LENGTH = MatrixTraits::LENGTH};
    /*! Define the strides between rows and columns of the matrix */
    enum {ROWSTRIDE = MatrixTraits::ROWSTRIDE, 
          COLSTRIDE = MatrixTraits::COLSTRIDE};

    /*! Define properties of the main diagonal of the matrix */
    enum {DIAGONAL_LENGTH = (ROWS <= COLS) ? ROWS : COLS,
        DIAGONAL_STRIDE = ROWSTRIDE + COLSTRIDE};
    
    /*! The type indicating a row of this matrix accessed by (const) reference 
     */
    typedef vctFixedSizeConstVectorRef<_elementType, COLS, COLSTRIDE>
        ConstRowRefType;
    /*! The type indicating a row of this matrix accessed by (non-const) 
      reference */
    typedef vctFixedSizeVectorRef<_elementType, COLS, COLSTRIDE>
        RowRefType;
    /*! The type indicating a column of this matrix accessed by (const)
      reference */
    typedef vctFixedSizeConstVectorRef<_elementType, ROWS, ROWSTRIDE>
        ConstColumnRefType;
    /*! The type indicating a column of this matrix accessed by (non-const) 
      reference */
    typedef vctFixedSizeVectorRef<_elementType, ROWS, ROWSTRIDE>
        ColumnRefType;
    /*! The type indicating the main diagonal of the matrix accessed
      by (const) reference */
    typedef vctFixedSizeConstVectorRef<_elementType, DIAGONAL_LENGTH, DIAGONAL_STRIDE>
        ConstDiagonalRefType;
    /*! The type indicating the main diagonal of the matrix accessed
      by (non-const) reference */
    typedef  vctFixedSizeVectorRef<_elementType, DIAGONAL_LENGTH, DIAGONAL_STRIDE>
        DiagonalRefType;

    /*! The type of vector object required to store a copy of a row of this
      matrix */
    typedef vctFixedSizeVector<_elementType, COLS> RowValueType;
    /*! The type of vector object required to store a copy of a column of this
      matrix */
    typedef vctFixedSizeVector<_elementType, ROWS> ColumnValueType;

    /*! The type of const reference to this matrix. */
    typedef vctFixedSizeConstMatrixRef<_elementType, _rows, _cols, _rowStride, _colStride> ConstRefType;

    /*! The type of reference to this matrix. */
    typedef vctFixedSizeMatrixRef<_elementType, _rows, _cols, _rowStride, _colStride> RefType;

    
    /*! The type of object representing this matrix accessed in transposed
      order.  Access is by (const) reference */
    typedef vctFixedSizeConstMatrixRef<_elementType, _cols, _rows,
        _colStride, _rowStride> ConstRefTransposeType;
    
    /*! The type of object representing this matrix accessed in transposed
      order.  Access is by (non-const) reference */
    typedef vctFixedSizeMatrixRef<_elementType, _cols, _rows,
        _colStride, _rowStride> RefTransposeType;
    
    /*! The type of object required to store a transposed copy of this matrix */
    typedef vctFixedSizeMatrix<_elementType, _cols, _rows, VCT_DEFAULT_STORAGE> TransposeValueType;

    /*! The type of a matrix returned by value from operations on this object */
    typedef vctFixedSizeMatrix<_elementType, _rows, _cols, VCT_DEFAULT_STORAGE> MatrixValueType;
    
    /*! The type of a matrix of booleans returned from operations on this object, e.g.,
      ElementwiseEqual. */
    typedef vctFixedSizeMatrix<bool, _rows, _cols, VCT_DEFAULT_STORAGE> BoolMatrixValueType;


 protected:
    /*! A declaration of the matrix-defining member object */
    _dataPtrType Data;


    /*! Check the validity of an index. */
    inline void ThrowUnlessValidIndex(size_type index) const throw(std::out_of_range) {
        if (! ValidIndex(index)) {
            cmnThrow(std::out_of_range("vctFixedSizeMatrix: Invalid index"));
        }
    }
    
    
    /*! Check the validity of the row and column indices. */
    inline void ThrowUnlessValidIndex(size_type rowIndex, size_type colIndex) const throw(std::out_of_range) {
        if (! ValidIndex(rowIndex, colIndex)) {
            cmnThrow(std::out_of_range("vctFixedSizeMatrix: Invalid indices"));
        }
    }
    
    
 public:
    /*! Returns a const iterator on the first element (STL
      compatibility). */
    const_iterator begin() const {
        return const_iterator(Data, 0);
    }
    
    
    /*! Returns a const iterator on the past-the-last element (STL
      compatibility). */
    const_iterator end() const {
        return const_iterator(Data) + LENGTH;
    }
    
    
    /*! Returns a reverse const iterator on the last element (STL
      compatibility). */ 
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(Pointer(ROWS - 1, COLS - 1), 0);
    }
    
    
    /*! Returns a reverse const iterator on the element before first
      (STL compatibility). */ 
    const_reverse_iterator rend() const {
        return const_reverse_iterator(Data - ROWSTRIDE + 
                                      COLSTRIDE * (COLS - 1), 0);
    }
    
    /*! Returns the size of the matrix or vector (STL
      compatibility). */
    size_type size() const {
        return LENGTH;
    }
    
    /*! Return the number of matrix rows.  Using smallcase naming following STL */
    size_type rows() const {
      return ROWS;
    }

    /*! Return the number of matrix columns.  Using smallcase naming following STL */
    size_type cols() const {
      return COLS;
    }
    

    /*! Returns the maximum size of the matrix or vector (STL
      compatibility).  For a fixed size matrix or vector, same as
      the size(). */
    size_type max_size() const {
        return LENGTH;
    }
    
    
    /*! Returns the row stride. Not required by STL but provided for
      completeness. */
    difference_type row_stride() const {
        return ROWSTRIDE;
    }
    
    /*! Returns the column stride. Not required by STL but provided
      for completeness. */
    difference_type col_stride() const {
        return COLSTRIDE;
    }
    
    /*! Tell is the vector is empty (STL compatibility).  False unless
      SIZE is zero. */
    bool empty() const {
        return (LENGTH == 0);
    }
    
    /*! Reference a row of this matrix by index (const).
      \return a const reference to the element[index] */
    ConstRowRefType operator[](size_type index) const {
        return ConstRowRefType(Data + ROWSTRIDE * index);
    }

    
    /*! Returns a const pointer to an element of the container,
      specified by its index. Addition to the STL requirements.
    */
    const_pointer Pointer(size_type rowIndex, size_type colIndex) const {
        return Data + ROWSTRIDE * rowIndex + COLSTRIDE * colIndex;
    }

    /*! Returns a const pointer to the first element of the
      container. Addition to the STL requirements.
    */
    const_pointer Pointer(void) const {
        return Data;
    }

    /*! Returns true if the index is lesser or equal to the number of
      elements of the matrix. */
    inline bool ValidIndex(size_type index) const {
        return ((index >= 0)
                && (index < size()));
    }

    /*! Returns true if both rowIndex and colIndex are valid
      indices. */
    inline bool ValidIndex(size_type rowIndex, size_type colIndex) const {
        return ((rowIndex >= 0)
                && (rowIndex < rows())
                && (colIndex >= 0)
                && (colIndex < cols()));
    }
    
    /*! Returns true if rowIndex is a valid row index. */
    inline bool ValidRowIndex(size_type rowIndex) const {
        return ((rowIndex >= 0)
                && (rowIndex < rows()));
    }
    
    /*! Returns true if colIndex is a valid column index. */
    inline bool ValidColIndex(size_type colIndex) const {
        return ((colIndex >= 0)
                && (colIndex < cols()));
    }
    
    
    /*! Access an element by index (const).  Compare with std::vector::at().
      This method can be a handy substitute for the overloaded operator () when
      operator overloading is unavailable or inconvenient.
      \return a const reference to the index-th element (iterator order) */
    const_reference at(size_type index) const throw(std::out_of_range) {
        ThrowUnlessValidIndex(index);
        return (begin())[index];
    }

    
    /*! Access an element by index (const).  Compare with std::vector::at().
      This method can be a handy substitute for the overloaded operator () when
      operator overloading is unavailable or inconvenient.
      \return a const reference to the element at rowIndex, colIndex */
    const_reference at(size_type rowIndex, size_type colIndex) const throw(std::out_of_range) {
        ThrowUnlessValidIndex(rowIndex, colIndex);
        return *(Pointer(rowIndex, colIndex));
    }
    
#ifndef SWIG
    /*! Access an element by index (const).  See method at().
      \return a const reference to element[rowIndex, colIndex] */
    const_reference operator()(size_type rowIndex, size_type colIndex) const throw(std::out_of_range) {
        return at(rowIndex, colIndex);
    }
#endif


    /*! Access an element by indices (const). This method allows to
      access an element without any bounds checking.  It doesn't
      create any temporary row reference as a matrix[][] would do.

      \return a reference to the element at rowIndex, colIndex */
    const_reference Element(size_type rowIndex, size_type colIndex) const {
        return *(Pointer(rowIndex, colIndex));
    }


    ConstRowRefType Row(size_type index) const {
        return ConstRowRefType(Data + ROWSTRIDE * index);
    }
    
    ConstColumnRefType Column(size_type index) const {
        return ConstColumnRefType(Data + COLSTRIDE * index);
    }

    ConstDiagonalRefType Diagonal() const
    {
        return ConstDiagonalRefType(Data);
    }


    /*! \name Incremental operations returning a scalar.
      Compute a scalar from all the elements of the matrix. */
    //@{

    /*! Return the sum of the elements of the matrix.
      \return The sum of all the elements */
    inline value_type SumOfElements() const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    } 

    /*! Return the product of the elements of the matrix.
      \return The product of all the elements */
    inline value_type ProductOfElements() const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Multiplication,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the trace of the matrix, i.e. the sum of elements of
      its diagonal.
      \return The trace of the matrix.
    */
    inline value_type Trace(void) const {
        return this->Diagonal().SumOfElements();
    }

    /*! Return the square of the norm  of the matrix.
      \return The square of the norm */
    inline value_type NormSquare(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Square>::
            Run(*this);
    }

    /*! Return the norm of the matrix.
      \return The norm. */
    inline NormType Norm(void) const {
        return sqrt(NormType(NormSquare()));
    }

    /*! Return the L1 norm of the matrix, i.e. the sum of the absolute
      values of all the elements.
 
      \return The L1 norm. */
    inline value_type L1Norm(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the Linf norm of the matrix, i.e. the maximum of the absolute
      values of all the elements.
 
      \return The Linf norm. */
    inline value_type LinfNorm(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the maximum element of the matrix.
      \return The maximum element */
    inline value_type MaxElement(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the minimum element of the matrix.
      \return The minimum element */
    inline value_type MinElement(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Minimum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Compute the minimum AND maximum elements of the matrix.
      This method is more runtime-efficient than computing them
      separately.
      \param minElement reference to store the minimum element result.
      \param maxElement reference to store the maximum element result.
      \note If this matrix is empty (null pointer) the result is undefined.
    */
    inline void MinAndMaxElement(value_type & minElement, value_type & maxElement) const
    {
        vctFixedSizeMatrixLoopEngines::MinAndMax::Run((*this), minElement, maxElement);
    }

    /*! Return true if all the elements of this vector are strictly
      positive, false otherwise */
    inline bool IsPositive(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are
      non-negative, false otherwise */
    inline bool IsNonNegative(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are
      non-positive, false otherwise */
    inline bool IsNonPositive(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are strictly
      negative, false otherwise */
    inline bool IsNegative (void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are nonzero,
      false otherwise */
    inline bool All(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    /*! Return true if any element of this matrix is nonzero, false
      otherwise */
    inline bool Any(void) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::Or,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    //@}


    /*! \name Storage format. */
    //@{

    /*! Test if the matrix storage order is column major.

    \return true if the row stride is lesser or equal to the column
     stride. */
    inline bool IsColMajor(void) const {
        return (row_stride() <= col_stride());
    }

    /*! Test if the matrix storage order is row major.

    \return true if the column stride is lesser or equal to the row
     stride. */
    inline bool IsRowMajor(void) const {
        return (col_stride() <= row_stride());
    }
    
    /*! Test if the matrix is compact, i.e. a m by n matrix actually
      uses a contiguous block of memory or size m by n. */
    inline bool IsCompact(void) const {
        return (((row_stride() == 1) && (row_stride() == (int)rows()))
                || ((col_stride() == 1) && (col_stride() == (int)cols())));
    }

    /*! Test if the matrix is "Fortran" compatible, i.e. is compact
      and uses a column major storage order.

      \sa IsColMajor IsCompact
    */
    inline bool IsFortran(void) const {
        return (IsColMajor() && (row_stride() == 1) && (col_stride() == (int)rows()));
    }

    /*! Return the storage order, i.e. either #VCT_ROW_MAJOR or
      #VCT_COL_MAJOR. */
    inline bool StorageOrder(void) const {
        return this->IsRowMajor();
    }

    /*! Test if a matrix is square. */
    inline bool IsSquare(void) const {
        return (this->rows() == this->cols());
    }

    /*! Test if a matrix is square and of a given size. */
    inline bool IsSquare(size_type size) const {
        return ((this->rows() == size)
                && (this->cols() == size));
    }
    //@}


    /*! \name Elementwise comparisons between matrices.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between two matrices of the same size, containing
      the same type of elements.  The strides and the internal
      representation (_dataPtrType) can be different.  The comparaison
      (\f$ = \neq < \leq > \geq \f$) for Equal(), NotEqual(),
      Lesser(), LesserOrEqual(), Greater() or GreaterOrEqual() is
      performed elementwise between the two matrices.  A logical "and"
      is performed (except for NotEqual which uses a logical "or") to
      accumulate the elementwise results.  The only operators provided
      are "==" and "!=" since the semantic is not ambiguous.

      \return A boolean.
    */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline bool Equal(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                      value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline bool operator == (const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                             value_type, __dataPtrType> & otherMatrix) const {
        return Equal(otherMatrix);
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline bool AlmostEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
          value_type, __dataPtrType> & otherMatrix,
          value_type tolerance) const {
        return ((*this - otherMatrix).LinfNorm() <= tolerance);
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline bool AlmostEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
          value_type, __dataPtrType> & otherMatrix) const {
        return ((*this - otherMatrix).LinfNorm() <= cmnTypeTraits<_elementType>::Tolerance());
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline bool NotEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                         value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline bool operator != (const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                             value_type, __dataPtrType> & otherMatrix) const {
        return NotEqual(otherMatrix);
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline bool Lesser(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                       value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline bool LesserOrEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                              value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline bool Greater(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                        value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline bool GreaterOrEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                               value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Run(*this, otherMatrix);
    }

    //@}

    /*! \name Elementwise comparisons between matrices.
      Returns the matrix of comparison's results. */
    //@{
    /*! Comparison between two matrices of the same size, containing
      the same type of elements.  The strides and the internal
      representation (_dataPtrType) can be different.  The comparaison
      (\f$ = \neq < \leq > \geq \f$) for ElementwiseEqual(),
      ElementwiseNotEqual(), ElementwiseLesser(),
      ElementwiseLesserOrEqual(), ElementwiseGreater() or
      ElementwiseGreaterOrEqual() is performed elementwise between the
      two matrices and stored in a newly created matrix.  There is no
      operator provided since the semantic would be ambiguous.

      \return A matrix of booleans.
    */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline BoolMatrixValueType
    ElementwiseEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                     value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixElementwiseCompareMatrix<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            __rowStride, __colStride, __dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, otherMatrix);
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline BoolMatrixValueType
    ElementwiseNotEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                        value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixElementwiseCompareMatrix<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            __rowStride, __colStride, __dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, otherMatrix);
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline BoolMatrixValueType
    ElementwiseLesser(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                      value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixElementwiseCompareMatrix<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            __rowStride, __colStride, __dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, otherMatrix);
    }
    
    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline BoolMatrixValueType
    ElementwiseLesserOrEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                             value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixElementwiseCompareMatrix<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            __rowStride, __colStride, __dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, otherMatrix);
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline BoolMatrixValueType
    ElementwiseGreater(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                       value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixElementwiseCompareMatrix<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            __rowStride, __colStride, __dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, otherMatrix);
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline BoolMatrixValueType
    ElementwiseGreaterOrEqual(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
                              value_type, __dataPtrType> & otherMatrix) const {
        return vctFixedSizeMatrixElementwiseCompareMatrix<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            __rowStride, __colStride, __dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>(*this, otherMatrix);
    }
    //@}

    /*! \name Elementwise comparisons between a matrix and a scalar.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between a matrix and a scalar, containing the same
      type of elements.  The comparaison (\f$ = \neq < \leq > \geq
      \f$) for Equal(), NotEqual(), Lesser(), LesserOrEqual(),
      Greater() or GreaterOrEqual() is performed elementwise between
      the matrix and the scalar.  A logical "and" is performed (except
      for NotEqual which uses a logical "or") to accumulate the
      elementwise results..  The only operators provided are "==" and
      "!=" since the semantic is not ambiguous.

      \return A boolean.
    */
    inline bool Equal(const value_type & scalar) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool operator == (const value_type & scalar) const {
        return Equal(scalar);
    }

    /* documented above */
    inline bool NotEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool operator != (const value_type & scalar) const {
        return NotEqual(scalar);
    }

   /* documented above */
    inline bool Lesser(const value_type & scalar) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, scalar);
    }
    
    /* documented above */
    inline bool LesserOrEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool Greater(const value_type & scalar) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool GreaterOrEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Run(*this, scalar);
    }

    //@}


    /*! \name Elementwise comparisons between a matrix and a scalar.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between a matrix and a scalar, containing the same
      type of elements.  The comparaison (\f$ = \neq < \leq > \geq
      \f$) for ElementwiseEqual(), ElementwiseNotEqual(),
      ElementwiseLesser(), ElementwiseLesserOrEqual(),
      ElementwiseGreater() or ElementwiseGreaterOrEqual() is performed
      elementwise between the matrix and the scalar and stored in a
      newly created matrix.  There is no operator provided since the
      semantic would be ambiguous.

      \return A matrix of booleans.
    */
    inline BoolMatrixValueType ElementwiseEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixElementwiseCompareScalar<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, scalar);
    }

    /* documented above */
    inline BoolMatrixValueType ElementwiseNotEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixElementwiseCompareScalar<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, scalar);
    }

    /* documented above */
    inline BoolMatrixValueType ElementwiseLesser(const value_type & scalar) const {
        return vctFixedSizeMatrixElementwiseCompareScalar<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, scalar);
    }

    /* documented above */
    inline BoolMatrixValueType ElementwiseLesserOrEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixElementwiseCompareScalar<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, scalar);
    }

    /* documented above */
    inline BoolMatrixValueType ElementwiseGreater(const value_type & scalar) const {
        return vctFixedSizeMatrixElementwiseCompareScalar<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, scalar);
    }

    /* documented above */
    inline BoolMatrixValueType ElementwiseGreaterOrEqual(const value_type & scalar) const {
        return vctFixedSizeMatrixElementwiseCompareScalar<
            _rows, _cols,
            _rowStride, _colStride, _dataPtrType,
            value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>(*this, scalar);
    }

    //@}

    /*! \name Unary elementwise operations.
      Returns the result of matrix.op(). */
    //@{
    /*! Unary elementwise operations on a matrix.  For each element of
      the matrix "this", performs \f$ this[i] \leftarrow
      op(otherMatrix[i])\f$ where \f$op\f$ can calculate the absolute
      value (Abs) or the opposite (Negation).

      \return A new matrix.
    */
    inline MatrixValueType Abs(void) const;
    
    /* documented above */
    inline MatrixValueType Negation(void) const;

    /* documented above */
    inline MatrixValueType Floor(void) const;

    /* documented above */
    inline MatrixValueType Ceil(void) const;
    //@}


    /*! Return a transposed reference to this matrix.  The actual
      definition of this method follows the declaration of
      class vctFixedSizeConstMatrixRef.
    */
    ConstRefTransposeType TransposeRef(void) const;

    /*!  Return a transposed copy of this matrix.
    */
    TransposeValueType Transpose() const
    {
        return TransposeValueType(TransposeRef());
    }

    /*! Easy definition of a submatrix type

      This class declares a const submatrix type.
      To declare a submatrix object, here's an example.

      typedef vctFixedSizeMatrix<double, 4, 4> double4x4;
      double4x4 m;
      double4x4::ConstSubmatrix<3, 3>::Type subMatrix( m, 0, 0 );

      The submatrix strides with respect to the parent container are always 1.  The is, the
      memory strides between the elements of the parent matrix and submatrix are equal.
      For more sophisticated submatrices, the user has to write customized code.
    */
    template<unsigned int _subRows, unsigned int _subCols>
    class ConstSubmatrix
    {
    public:
        typedef vctFixedSizeConstMatrixRef<value_type, _subRows, _subCols, ROWSTRIDE, COLSTRIDE>
            Type;
    };

    /*! Return the identity matrix for the size and type of
      elements of this matrix class.  For example:
      \code
      vct4x4 m = vct4x4::Eye();
      vctInt3x3 m2 = vctInt3x3::Eye();
      typedef vctFixedSizeMatrix<float, 6, 6> MatrixType;
      MatrixType m3 = MatrixType::Eye();
      \endcode

      \note Eye is the spelling of "I".
    */
    static MatrixValueType Eye(void);

    /*! Return a string representation of the matrix elements */
    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }

    /*!  Print the matrix to a text stream */
    void ToStream(std::ostream & outputStream) const {
        const size_type myRows = rows();
        const size_type myCols = cols();
        // preserve the formatting flags as they were
        const int width = outputStream.width(12);
        const int precision = outputStream.precision(6);
        bool showpoint = ((outputStream.flags() & std::ios_base::showpoint) != 0);
        outputStream << std::setprecision(6) << std::showpoint;
        size_type indexRow, indexCol;
        for (indexRow = 0; indexRow < myRows; ++indexRow) {
            for (indexCol = 0; indexCol < myCols; ++indexCol) {
                outputStream << std::setw(12) << this->Element(indexRow, indexCol);
                if (indexCol < (myCols-1)) {
                    outputStream << " ";
                }
            }
            // end of line between rows, not at the end
            if (indexRow != (myRows - 1)) {
                outputStream << std::endl;
            }
        }
        // resume the formatting flags
        outputStream << std::setprecision(precision) << std::setw(width);
        if (!showpoint) {
            outputStream << std::noshowpoint;
        }
    }
};



/*! Return true if all the elements of the matrix are nonzero, false otherwise */
template<unsigned int _rows, unsigned int _cols, int _rowStride, 
         int _colStride, class _elementType, class _dataPtrType>
inline bool vctAll(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix) {
    return matrix.All();
}

/*! Return true if any element of the matrix is nonzero, false otherwise */
template<unsigned int _rows, unsigned int _cols, int _rowStride, 
         int _colStride, class _elementType, class _dataPtrType>
inline bool vctAny(const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix) {
    return matrix.Any();
}

/*! Stream out operator. */
template<unsigned int _rows, unsigned int _cols, int _rowStride, 
         int _colStride, class _elementType, class _dataPtrType>
std::ostream & operator << (std::ostream & output,
                            const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & matrix) {
    matrix.ToStream(output);
    return output;
}


#endif // _vctFixedSizeConstMatrixBase_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedSizeConstMatrixBase.h,v $
// Revision 1.49  2006/07/03 17:31:56  dli
// Added ProductOfElements function
//
// Revision 1.48  2006/01/26 20:45:22  anton
// cisstVector: Added Eye methods to compute identity for matrices.  See #210.
//
// Revision 1.47  2006/01/26 19:37:57  anton
// cisstVector: Added test methods IsSquare() and use it in nmrSVD and nmrLU.
// See ticket #209.
//
// Revision 1.46  2006/01/26 19:19:06  anton
// cisstVector: Added Trace() method to matrices.  See #208.
//
// Revision 1.45  2005/12/23 21:27:31  anton
// cisstVector: Minor updates for Doxygen 1.4.5.
//
// Revision 1.44  2005/12/13 00:04:57  ofri
// All cisstVector containers: Added methods
// 1) MinAndMaxElement  (at const base)
// 2) Floor, Ceil  (at const base -- implemented at container)
// 3) FloorSelf, CeilSelf, FloorOf, CeilOf  (at non-const base)
//
// Revision 1.43  2005/12/12 21:55:29  anton
// cisstVector matrices: Removed useless check in IsCompact() and added method
// StorageOrder() to retrieve the storage order (instead of using IsRowMajor()).
//
// Revision 1.42  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.41  2005/09/24 00:01:04  anton
// cisstVector: Use cmnThrow for all exceptions.
//
// Revision 1.40  2005/08/11 21:29:44  ofri
// cisstVector -- base classes ToStream(): Added code to preserve the previous
// state of the stream (precision, width, showpoint) and resume it before returning
// from the method.
//
// Revision 1.39  2005/08/04 15:31:57  anton
// cisstVector: Replaced RangeCheck() by ThrowUnlessValidIndex().  For dynamic
// matrices, Row() and Col() now check the index and throw using
// ThrowUnlessValid{Row,Col}Index().
//
// Revision 1.38  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.37  2005/04/28 03:55:23  anton
// cisstVector:  Added AlmostEqual() to all containers to compare with
// tolerance.
//
// Revision 1.36  2005/01/05 18:29:49  anton
// cisstVector: Added ValidIndex() methods for all containers (and tests) as
// requested in ticket #111.
//
// Revision 1.35  2004/12/01 17:14:53  anton
// cisstVector:  Replace access to matrix elements by Element() instead of
// double [][] for increased performance.  See ticket #79.
//
// Revision 1.34  2004/12/01 16:42:19  anton
// cisstVector: Modified the signature of Pointer methods for matrices.  We now
// have two signature, Pointer(void) and Pointer(row, col) with no default value
// to avoid the ambiguous Pointer(index).  See ticket #93.
//
// Revision 1.33  2004/11/30 23:36:05  anton
// cisstVector: Added the Element() method to access an element without range
// check.  See ticket #79.
//
// Revision 1.32  2004/11/30 20:13:01  anton
// vctFixedSizeConstMatrixBase: Added two types, RefType and ConstRefType to
// to ease the declaration of a reference to the whole matrix (simpler than
// using Submatrix).
//
// Revision 1.31  2004/11/29 17:31:28  anton
// cisstVector: Major corrections for matrices reverse iterators.  Correction
// of the - operator which now takes into account the current column position.
//
// Revision 1.30  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
// Revision 1.29  2004/11/08 18:10:17  anton
// cisstVector: Completed the use of VCT_STORAGE_ORDER, added related Doxygen
// documentation.
//
// Revision 1.28  2004/11/03 22:26:12  anton
// cisstVector:  Better support of storage order.  Added VCT_COL_MAJOR,
// VCT_ROW_MAJOR and VCT_DEFAULT_STORAGE as well as the methods IsColMajor(),
// IsRowMajor(), IsCompact() and IsFortran().
//
// Revision 1.27  2004/10/26 15:49:03  ofri
// vct[FixedSize|Dynamic](Const)MatrixBase classes: Added reference access to
// the main diagonal (ticket #76).  Fixed bug in the Row() and Column() methods
// of the dynamic matrix base classes : incorrect stride.
//
// Revision 1.26  2004/10/26 15:20:52  ofri
// Updating subsequence interfaces (ticket #76) and interfaces between fixed
// and dynamic vectors/matrices (ticket #72).
// *: vctFixedSize(Const)VectorBase::Get(Const)Subsequence now deprecated.
// Replaced by Get(Const)Subvector with equal stride to the parent.
// *: Added constructors for [Fixed|Dynamic][Vector|Matrix]Ref from the other
// storage type.
//
// Revision 1.25  2004/10/25 19:19:54  anton
// cisstVector:  Created vctForwardDeclarations.h and removed forward
// declarations of classes in all other files.  Added some constructors
// for vector references to reference fixed size from dynamic and vice versa.
//
// Revision 1.24  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.23  2004/10/22 18:07:06  anton
// Doxygen: Made sure that all the classes are documented for Doxygen.
//
// Revision 1.22  2004/10/19 22:02:05  anton
// cisstVector: Compilation error with .net 2000 and cleanup by adding the
// MatrixValueType.  See check-in [821] and ticket #75.
//
// Revision 1.21  2004/10/19 20:50:50  anton
// cisstVector: Preliminary support for different storage orders (i.e. row
// major by default, column major if specified).  This has been implemented
// for the vctFixedSizeMatrix as well as the vctDynamicMatrix.  We tested the
// code by simply changinf the default and all current tests passed.  See
// ticket #75 re. vctFortranMatrix.
//
// Revision 1.20  2004/10/14 20:42:23  ofri
// Added method vctFixedSizeConstMatrixBase::Transpose()
//
// Revision 1.19  2004/10/14 19:18:01  anton
// cisstVector, fixed size containers: Added operator() based on at() method
// as required per ticket #72.
//
// Revision 1.18  2004/10/08 19:38:58  anton
// cisstVector: Added a space between elements in ToStream methods.
//
// Revision 1.17  2004/09/24 20:20:42  anton
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
// Revision 1.16  2004/09/21 20:08:11  anton
// cisstVector: In mthods Norm(), use a cast to NormType(NormSquare) before
// calling sqrt.  Otherwise, .Net 2003 complains (.Net 7 worked).
//
// Revision 1.15  2004/08/17 13:58:31  ofri
// vctFixedSizeMatrixBase and ConstMatrixBase: Added templated inner classes for
// submatrix types (cf vctFixedSizeVectorBase).  Added documentation for methods
// ToString and ToStream.
//
// Revision 1.14  2004/08/16 19:22:53  anton
// cisstVector: Added bound checking for matrices as well as method "at"
// with two parameters (row and col indices).  See also ticket #13 and CVS
// commit [699]
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
// Revision 1.12  2004/08/11 19:21:32  anton
// cisstVector: the operator << and the methods ToString and ToStream have
// been improved.
//
// Revision 1.11  2004/08/04 21:05:37  anton
// vctFixedSizeConstMatrixBase.h: method cols_stride renamed col_stride to be
// consistant with row_stride, not rows_stride.
//
// Revision 1.10  2004/07/27 20:08:06  anton
// cisstVector: Added some Doxygen documentation.  Minor code changes to
// keep the dynamic vectors in sync with the fixed size ones.
//
// Revision 1.9  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.8  2004/03/19 15:58:50  ofri
// Documentation update
//
// Revision 1.7  2004/03/19 15:28:41  ofri
// Added the method at(index) to access sequence elements without operator
// overloading
//
// Revision 1.6  2004/02/16 19:40:52  anton
// Error in stream out operator, had a "std::cout<<" instead of output<<
//
// Revision 1.5  2004/01/23 18:17:18  anton
// Removed methods MultiplyVector and VectorMultiply, these are now declared as
// ProductOf(matrix, sequence) and ProductOf(sequence, matrix) in the sequence
// class.   Also removed the #include vctVectorAndMatrixOperations.h
//
// Revision 1.4  2004/01/15 15:25:42  anton
// - Corrected bug in != operator between matrices, incremental operation is Or, not And
// - Added operators != == < <= > >= between a matrix and a scalar
//
// Revision 1.3  2003/12/18 16:18:35  anton
// Added most methods and operators to be consistant with vctFixedLengthConstSequenceBase
//
// Revision 1.2  2003/11/26 15:56:51  ofri
// Fixed syntax error in vctFixedSizeConstMatrixBase::GetColumn
//
// Revision 1.1  2003/11/20 18:23:55  anton
// Splitted const and non const classes in two files.  Added Stream out operator
//
//
// Revision 1.2  2003/11/11 03:58:23  ofri
// 1) Changed the order of template arguments to vctFixedSizeMatrixBase
// classes
// 2) Defined the MatrixMatrixProduct method to use vctMatrixProductEngine
// instead of the ordinary recursive engines.
//
// Revision 1.1  2003/11/04 21:58:49  ofri
// Committing initial version of the code for fixed-size matrices.  Need to
// implement more operations and improve documentation, but the basic
// framework works.
//
// ****************************************************************************
