/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicConstMatrixBase.h,v 1.54 2006/07/03 17:31:56 dli Exp $
  
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2004-07-01

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


#ifndef _vctDynamicConstMatrixBase_h
#define _vctDynamicConstMatrixBase_h

/*!
  \file
  \brief Declaration of vctDynamicConstMatrixBase
*/

#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstVector/vctContainerTraits.h>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctDynamicVectorRef.h>
#include <cisstVector/vctDynamicConstVectorRef.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicMatrixLoopEngines.h>

#include <iostream>
#include <iomanip>


/* Forward declarations */
#ifndef DOXYGEN
template<class _matrixOwnerType, class __matrixOwnerType, class _elementType,
         class _elementOperationType>
inline vctReturnDynamicMatrix<bool>
vctDynamicMatrixElementwiseCompareMatrix(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix1,
                                         const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix2);

template<class _matrixOwnerType, class _elementType,
         class _elementOperationType>
inline vctReturnDynamicMatrix<bool>
vctDynamicMatrixElementwiseCompareScalar(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix,
                                         const _elementType & scalar);
#endif // DOXYGEN


/*! 
  This class is templated with the ``matrix owner type'', which may
  be a vctDynamicMatrixOwner or a vctMatrixRefOwner.  It provides
  const operations on the dynamic matrix, such as SumOfElements etc.
  
  Matrix indexing is zero-based.

  The method provided for the compatibility with the STL containers
  start with a lower case.  Other methods start with a capitalilized
  letter (see CISST naming convention).

  \param _matrixOwnerType the type of matrix owner.

  \param _elementType the type of elements of the matrix.
*/
template<class _matrixOwnerType, typename _elementType>
class vctDynamicConstMatrixBase
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! Type of the matrix itself. */
    typedef vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> ThisType;

    /*! Type of the data owner (dynamic array or pointer) */
    typedef _matrixOwnerType MatrixOwnerType;

    /*! Iterator on the elements of the matrix. */   
    typedef typename MatrixOwnerType::iterator iterator;

    /*! Const iterator on the elements of the matrix. */    
    typedef typename MatrixOwnerType::const_iterator const_iterator; 

    /*! Reverse iterator on the elements of the matrix. */   
    typedef typename MatrixOwnerType::reverse_iterator reverse_iterator;

    /*! Const reverse iterator on the elements of the matrix. */   
    typedef typename MatrixOwnerType::const_reverse_iterator const_reverse_iterator; 

    /*! Size of a matrix defined as a vector of two elements, the
      number of rows and the number of columns. */
    typedef vctFixedSizeVector<size_type, 2> MatrixSizeType;

    /*! The type indicating a row of this matrix accessed by (const)
      reference */
    typedef vctDynamicConstVectorRef<_elementType> ConstRowRefType;
    
    /*! The type indicating a row of this matrix accessed by
      (non-const) reference */
    typedef vctDynamicVectorRef<_elementType> RowRefType;

    /*! The type indicating a column of this matrix accessed by (const)
      reference */
    typedef vctDynamicConstVectorRef<_elementType> ConstColumnRefType;
    
    /*! The type indicating a column of this matrix accessed by
      (non-const) reference */
    typedef vctDynamicVectorRef<_elementType> ColumnRefType;

    /*! The type indicating the main diagonal of this matrix accessed
      by (const) reference */
    typedef vctDynamicConstVectorRef<_elementType> ConstDiagonalRefType;
    
    /*! The type indicating the main diagonal of this matrix accessed
      by (non-const) reference */
    typedef vctDynamicVectorRef<_elementType> DiagonalRefType;

    /*! The type of object representing this matrix accessed in transposed
      order.  Access is by (const) reference */
    typedef vctDynamicConstMatrixRef<_elementType> ConstRefTransposeType;

    /*! The type of object representing this matrix accessed in transposed
      order.  Access is by (non-const) reference */
    typedef vctDynamicConstMatrixRef<_elementType> RefTransposeType;

    /*! The type of object required to store a transposed copy of this matrix */
    typedef vctDynamicMatrix<_elementType> TransposeValueType;

    /*! The type of a matrix returned by value from operations on this object */
    typedef vctReturnDynamicMatrix<_elementType> MatrixValueType;

    typedef cmnTypeTraits<value_type> TypeTraits;

    /*! Define bool based on the container type to force some
      compilers (i.e. gcc 4.0) to delay the instantiation of the
      ElementWiseCompare methods. */
    typedef typename TypeTraits::BoolType BoolType;

    /*! The type of a matrix of booleans returned from operations on this object, e.g.,
      ElementwiseEqual. */
    typedef vctReturnDynamicMatrix<BoolType> BoolMatrixValueType;

    /*! The type of vector used to store const pointers on the rows or columns */
    typedef vctDynamicVector<const_pointer> ConstVectorPointerType;

    /*! The type of vector used to store pointers on the rows or columns */
    typedef vctDynamicVector<pointer> VectorPointerType;


protected:
    /*! Declaration of the matrix-defining member object */
    MatrixOwnerType Matrix;


    /*! Check the validity of an index. */
    inline void ThrowUnlessValidIndex(size_type index) const throw(std::out_of_range) {
        if (! ValidIndex(index)) {
            cmnThrow(std::out_of_range("vctDynamicMatrix: Invalid index"));
        }
    }
    

    /*! Check the validity of the row and column indices. */
    inline void ThrowUnlessValidIndex(size_type rowIndex, size_type colIndex) const throw(std::out_of_range) {
        if (! ValidIndex(rowIndex, colIndex)) {
            cmnThrow(std::out_of_range("vctDynamicMatrix: Invalid indices"));
        }
    }


    /*! Throw an exception unless the row index is valid */
    inline void ThrowUnlessValidRowIndex(size_type index) const throw(std::out_of_range) {
        if (! ValidRowIndex(index)) {
            cmnThrow(std::out_of_range("vctDynamicMatrix: Invalid row index"));
        }
    }

    /*! Throw an exception unless the column index is valid */
    inline void ThrowUnlessValidColIndex(size_type index) const throw(std::out_of_range) {
        if (! ValidColIndex(index)) {
            cmnThrow(std::out_of_range("vctDynamicMatrix: Invalid column index"));
        }
    }

public:
    /*! Returns a const iterator on the first element (STL
      compatibility). */
    const_iterator begin(void) const {
        return Matrix.begin(); 
    }

    /*! Returns a const iterator on the last element (STL
      compatibility). */
    const_iterator end(void) const {
        return Matrix.end(); 
    }

    /*! Returns a reverse const iterator on the last element (STL
      compatibility). */ 
    const_reverse_iterator rbegin(void) const {
        return Matrix.rbegin(); 
    }

    /*! Returns a reverse const iterator on the element before first
      (STL compatibility). */ 
    const_reverse_iterator rend(void) const {
        return Matrix.rend(); 
    }

    /*! Return the number of elements in the matrix.  This is not
      equivalent to the difference between the end and the beginning.
    */
    size_type size(void) const {
        return Matrix.size();
    }

    /*! Not required by STL but provided for completeness */
    size_type rows() const {
        return Matrix.rows();
    }

    /*! Not required by STL but provided for completeness */
    size_type cols() const {
        return Matrix.cols();
    }

    /*! Not required by STL but provided for completeness */
    size_type height() const {
        return Matrix.rows();
    }

    /*! Not required by STL but provided for completeness */
    size_type width() const {
        return Matrix.cols();
    }

    /*! Not required by STL but provided for completeness */
    difference_type row_stride() const {
        return Matrix.row_stride();
    }

    /*! Not required by STL but provided for completeness */
    difference_type col_stride() const {
        return Matrix.col_stride();
    }

    /*! Tell is the matrix is empty (STL compatibility).  False unless
      size is zero. */
    bool empty() const {
        return (size() == 0);
    }

    /*! Reference a row of this matrix by index (const).
      \return a const reference to the element[index] */
    ConstRowRefType operator[](size_type index) const {
        return ConstRowRefType(cols(), Pointer(index, 0), col_stride());
    }

    /*! Access an element by index (const).  Compare with
      std::matrix::at().  This method can be a handy substitute for
      the overloaded operator [] when operator overloading is
      unavailable or inconvenient.  \return a const reference to
      element[index] */
    const_reference at(size_type index) const throw(std::out_of_range) {
        ThrowUnlessValidIndex(index);
        return (begin())[index];
    }


    /*! Returns a const pointer to an element of the container,
      specified by its indices. Addition to the STL requirements.
    */
    const_pointer Pointer(index_type rowIndex, index_type colIndex) const {
        return Matrix.Pointer(rowIndex, colIndex);
    }

    /*! Returns a const pointer to the first element of the
      container. Addition to the STL requirements.
    */
    const_pointer Pointer(void) const {
        return Matrix.Pointer();
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
      \return a const reference to the element at rowIndex, colIndex */
    const_reference at(size_type rowIndex, size_type colIndex) const throw(std::out_of_range) {
        ThrowUnlessValidIndex(rowIndex, colIndex);
        return *(Pointer(rowIndex, colIndex));
    }

    /*! Overloaded operator () for simplified (const) element access with bounds checking */
    const_reference operator () (size_type rowIndex, size_type colIndex) const throw(std::out_of_range) {
        return this->at(rowIndex, colIndex);
    }


    /*! Access an element by indices (const). This method allows to
      access an element without any bounds checking.  It doesn't
      create any temporary row reference as a matrix[][] would do.

      \return a reference to the element at rowIndex, colIndex */
    const_reference Element(size_type rowIndex, size_type colIndex) const {
        return *(Pointer(rowIndex, colIndex));
    }


    ConstRowRefType Row(size_type index) const throw(std::out_of_range) {
        ThrowUnlessValidRowIndex(index);
        return ConstRowRefType(cols(), Pointer(index, 0), col_stride());
    }
    
    ConstColumnRefType Column(size_type index) const throw(std::out_of_range) {
        ThrowUnlessValidColIndex(index);
        return ConstColumnRefType(rows(), Pointer(0, index), row_stride());
    }

    ConstDiagonalRefType Diagonal() const {
        return ConstDiagonalRefType( std::min(rows(), cols()), Pointer(0, 0), row_stride() + col_stride() );
    }

    /*! Resize and fill a vector of const pointers on the rows of the
      matrix.  This method is provided to ease the interfacing with C
      libraries using matrices stored as value_type**. 

      To use this method, one must first create a dynamic vector of
      pointers, update it with the RowPointers method and then call
      the C function:

      \code
      vctDynamicMatrix<double> myMatrix = ...;
      vctDynamicVector<double *> rowPointers;
      myMatrix.RowPointers(rowPointers);
      c_function(rowPointers.Pointer());
      \endcode

      \note This method will throw an exception if the rows are not
      compact, i.e. if the column stride is not equal to 1.
    */ 
    ConstVectorPointerType & RowPointers(ConstVectorPointerType & rowPointers) const throw(std::runtime_error) {
        if (! this->col_stride() == 1) {
            cmnThrow(std::runtime_error("vctDynamicMatrix: RowPointers requires compact rows")); 
        }
        const unsigned int rows = this->rows();
        // resize the vector
        rowPointers.SetSize(rows);
        unsigned int index;
        for (index = 0; index < rows; ++index) {
            rowPointers[index] = this->Row(index).Pointer();
        }
        return rowPointers;
    }


    /*! \name Incremental operations returning a scalar.
      Compute a scalar from all the elements of the matrix. */
    //@{

    /*! Return the sum of the elements of the matrix.
      \return The sum of all the elements */
    inline value_type SumOfElements(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    } 

    /*! Return the product of the elements of the matrix.
      \return The product of all the elements */
    inline value_type ProductOfElements(void) const {
        return vctDynamicMatrixLoopEngines::
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
        return vctDynamicMatrixLoopEngines::
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
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the Linf norm of the matrix, i.e. the maximum of the absolute
      values of all the elements.
 
      \return The Linf norm. */
    inline value_type LinfNorm(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the maximum element of the matrix.
      \return The maximum element */
    inline value_type MaxElement(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the minimum element of the matrix.
      \return The minimum element */
    inline value_type MinElement(void) const {
        return vctDynamicMatrixLoopEngines::
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
        vctDynamicMatrixLoopEngines::MinAndMax::Run((*this), minElement, maxElement);
    }
    
    /*! Return true if all the elements of this matrix are strictly positive, 
      false otherwise */
    inline bool IsPositive(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are non-negative, 
      false otherwise */
    inline bool IsNonNegative(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are non-positive, 
      false otherwise */
    inline bool IsNonPositive(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are strictly negative, 
      false otherwise */
    inline bool IsNegative (void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this matrix are nonzero,
      false otherwise */
    inline bool All(void) const {
        return vctDynamicMatrixLoopEngines::
            SoMi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    /*! Return true if any element of this matrix is nonzero, false
      otherwise */
    inline bool Any(void) const {
        return vctDynamicMatrixLoopEngines::
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
        return Matrix.IsColMajor();
    }

    /*! Test if the matrix storage order is row major.

    \return true if the column stride is lesser or equal to the row
     stride. */
    inline bool IsRowMajor(void) const {
        return Matrix.IsRowMajor();
    }
    
    /*! Test if the matrix is compact, i.e. a m by n matrix actually
      uses a contiguous block of memory or size m by n. */
    inline bool IsCompact(void) const {
        return (((row_stride() == 1) && (col_stride() == (int)rows()))
                || ((col_stride() == 1) && (row_stride() == (int)cols())));
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
        return Matrix.StorageOrder();
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
      the same type of elements.  The strides can be different.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for Equal(),
      NotEqual(), Lesser(), LesserOrEqual(), Greater() or
      GreaterOrEqual() is performed elementwise between the two
      matrices.  A logical "and" is performed (except for NotEqual
      which uses a logical "or") to accumulate the elementwise
      results.  The only operators provided are "==" and "!=" since
      the semantic is not ambiguous.

      \return A boolean.
    */
    template <class __matrixOwnerType>
    inline bool Equal(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, otherMatrix);
    }
    
    /* documented above */
    template <class __matrixOwnerType>
    inline bool operator == (const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return Equal(otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool AlmostEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix,
          value_type tolerance) const {
        return ((*this - otherMatrix).LinfNorm() <= tolerance);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool AlmostEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return ((*this - otherMatrix).LinfNorm() <= TypeTraits::Tolerance());
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool NotEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, otherMatrix);
    }
    
    /* documented above */
    template <class __matrixOwnerType>
    inline bool operator != (const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return NotEqual(otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool Lesser(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool LesserOrEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, otherMatrix);
    }

    /* documented above */
    template <class __matrixOwnerType>
    inline bool Greater(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
            SoMiMi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, otherMatrix);
    }
    
    /* documented above */
    template <class __matrixOwnerType>
    inline bool GreaterOrEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixLoopEngines::
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
      representation (_matrixOwnerType) can be different.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for
      ElementwiseEqual(), ElementwiseNotEqual(), ElementwiseLesser(),
      ElementwiseLesserOrEqual(), ElementwiseGreater() or
      ElementwiseGreaterOrEqual() is performed elementwise between the
      two matrices and stored in a newly created matrix.  There is no
      operator provided since the semantic would be ambiguous.

      \return A matrix of booleans.
    */
    template<class __matrixOwnerType>
    inline BoolMatrixValueType
    ElementwiseEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, otherMatrix);
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline BoolMatrixValueType
    ElementwiseNotEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, otherMatrix);
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline BoolMatrixValueType
    ElementwiseLesser(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, otherMatrix);
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline BoolMatrixValueType
    ElementwiseLesserOrEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, otherMatrix);
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline BoolMatrixValueType
    ElementwiseGreater(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, otherMatrix);
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline BoolMatrixValueType
    ElementwiseGreaterOrEqual(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) const {
        return vctDynamicMatrixElementwiseCompareMatrix<_matrixOwnerType, __matrixOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>(*this, otherMatrix);
    }

    //@}


    /*! \name Elementwise comparisons between a matrix and a scalar.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between a matrix and a scalar.  The type of the
      elements of the matrix and the scalar must be the same.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for Equal(),
      NotEqual(), Lesser(), LesserOrEqual(), Greater() or
      GreaterOrEqual() is performed elementwise between the matrix and
      the scalar.  A logical "and" is performed (except for NotEqual
      which uses a logical "or") to accumulate the elementwise
      results..  The only operators provided are "==" and "!=" since
      the semantic is not ambiguous.

      \return A boolean.
    */
    inline bool Equal(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
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
        return vctDynamicMatrixLoopEngines::
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
        return vctDynamicMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool LesserOrEqual(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool Greater(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
            SoMiSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool GreaterOrEqual(const value_type & scalar) const {
        return vctDynamicMatrixLoopEngines::
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
    BoolMatrixValueType ElementwiseEqual(const value_type & scalar) const;

    /* documented above */
    BoolMatrixValueType ElementwiseNotEqual(const value_type & scalar) const;

    /* documented above */
    BoolMatrixValueType ElementwiseLesser(const value_type & scalar) const;

    /* documented above */
    BoolMatrixValueType ElementwiseLesserOrEqual(const value_type & scalar) const;

    /* documented above */
    BoolMatrixValueType ElementwiseGreater(const value_type & scalar) const;

    /* documented above */
    BoolMatrixValueType ElementwiseGreaterOrEqual(const value_type & scalar) const;

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

    /*! Return a transposed reference to this matrix.  This allows access to elements
      of this matrix in transposed order.
    */
    ConstRefTransposeType TransposeRef(void) const
    {
        return ConstRefTransposeType(cols(), rows(), col_stride(), row_stride(), Pointer());
    }

    /*! Return a transposed copy of this matrix */
    inline vctReturnDynamicMatrix<value_type> Transpose() const
    {
        vctDynamicMatrix<value_type> result( this->TransposeRef() );
        return vctReturnDynamicMatrix<value_type>(result);
    }


    /*! Return the identity matrix for the type of elements of this
      matrix class.  The size has to be specified by the caller.  For
      example:
      \code
      vctDoubleMat m = vctDoubleMat::Eye(12);
      typedef vctDynamicMatrix<float> MatrixType;
      MatrixType m2 = MatrixType::Eye(9);
      \endcode

      \note Eye is the spelling of "I".
    */
    static MatrixValueType Eye(size_type size);

    /*! Return a string representation of the matrix elements */
    std::string ToString(void) {
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

    /*! Define a ConstSubmatrix class for compatibility with the fixed size matrices.
      A const submatrix has the same stride as the parent container.

      Example:

      typedef vctDynamicMatrix<double> MatrixType;
      MatrixType M(6,6);
      MatrixType::ConstSubmatrix::Type topLeft(M, 0, 0);
      MatrixType::ConstSubmatrix::Type bottomRight(M, 3, 3);

      \note There is no straightforward way to define a fixed-size
      submatrix of a dynamic matrix, because the stride of the dynamic
      matrix is not known in compilation time.  A way to do it is:

      vctFixedSizeConstMatrixRef<double, 3, 3, 1, 6> topRight(M, 0, 3);
      vctFixedSizeConstMatrixRef<double, 3, 3, 1, 6> bottomLeft(M, 3, 0);
    */
#ifndef SWIG
    class ConstSubmatrix
    {
    public:
        typedef vctDynamicConstMatrixRef<value_type> Type;
    };
#endif // SWIG
};

#ifndef DOXYGEN
/* documented in class.  Implementation moved here for .Net 2003 */
template<class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::BoolMatrixValueType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::ElementwiseEqual(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::Equal>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template<class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::BoolMatrixValueType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::ElementwiseNotEqual(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::NotEqual>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template<class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::BoolMatrixValueType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::ElementwiseLesser(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::Lesser>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template<class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::BoolMatrixValueType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::ElementwiseLesserOrEqual(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::LesserOrEqual>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template<class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::BoolMatrixValueType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::ElementwiseGreater(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::Greater>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template<class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::BoolMatrixValueType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::ElementwiseGreaterOrEqual(const _elementType & scalar) const {
    return vctDynamicMatrixElementwiseCompareScalar<_matrixOwnerType, _elementType,
        typename vctBinaryOperations<bool, _elementType, _elementType>::GreaterOrEqual>(*this, scalar);
}
#endif // DOXYGEN

/*! Return true if all the elements of the matrix are nonzero, false otherwise */
template<class _matrixOwnerType, typename _elementType>
inline bool vctAll(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix) {
    return matrix.All();
}

/*! Return true if any element of the matrix is nonzero, false otherwise */
template<class _matrixOwnerType, typename _elementType>
inline bool vctAny(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix) {
    return matrix.Any();
}

/*! Stream out operator. */
template<class _matrixOwnerType, typename _elementType>
std::ostream & operator << (std::ostream & output,
                            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix) {
    matrix.ToStream(output);
    return output;
}


#endif // _vctDynamicConstMatrixBase_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicConstMatrixBase.h,v $
//  Revision 1.54  2006/07/03 17:31:56  dli
//  Added ProductOfElements function
//
//  Revision 1.53  2006/04/26 19:51:21  anton
//  vctDynamicMatrix: Correction for bug #221.  This should work well for matrices
//  using vctDynamicMatrixOwner.  For matrices using existing memory, i.e.
//  vctDynamicMatrixRefOwner, we need to redefine the specifications and add test
//  cases.
//
//  Revision 1.52  2006/03/30 19:48:21  anton
//  vctDynamicConstMatrixBase: Added height and width to make the matrices look
//  like a pixmap.
//
//  Revision 1.51  2006/01/26 20:45:22  anton
//  cisstVector: Added Eye methods to compute identity for matrices.  See #210.
//
//  Revision 1.50  2006/01/26 19:37:57  anton
//  cisstVector: Added test methods IsSquare() and use it in nmrSVD and nmrLU.
//  See ticket #209.
//
//  Revision 1.49  2006/01/26 19:19:06  anton
//  cisstVector: Added Trace() method to matrices.  See #208.
//
//  Revision 1.48  2006/01/26 19:05:21  anton
//  cisstVector: Generalized definition of MatrixSizeType and added documentation
//  (see #206 for motivation).
//
//  Revision 1.47  2006/01/25 03:17:16  anton
//  cisstVector and cisstNumerical: Test new type MatrixSizeType and test it
//  with nmrSVD (see ticket #206).
//
//  Revision 1.46  2006/01/03 03:38:58  anton
//  cisstVector Doxygen: Used \code \endcode instead of <pre> </pre>, added
//  some missing <ul> </ul>, added links to VCT_NORMALIZE.
//
//  Revision 1.45  2005/12/23 21:27:31  anton
//  cisstVector: Minor updates for Doxygen 1.4.5.
//
//  Revision 1.44  2005/12/13 00:04:56  ofri
//  All cisstVector containers: Added methods
//  1) MinAndMaxElement  (at const base)
//  2) Floor, Ceil  (at const base -- implemented at container)
//  3) FloorSelf, CeilSelf, FloorOf, CeilOf  (at non-const base)
//
//  Revision 1.43  2005/12/12 21:55:28  anton
//  cisstVector matrices: Removed useless check in IsCompact() and added method
//  StorageOrder() to retrieve the storage order (instead of using IsRowMajor()).
//
//  Revision 1.42  2005/10/06 16:56:37  anton
//  Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
//  Revision 1.41  2005/09/27 18:01:25  anton
//  cisstVector: Use CMN_ASSERT instead of assert for vectors and matrices.
//
//  Revision 1.40  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.39  2005/09/24 00:01:04  anton
//  cisstVector: Use cmnThrow for all exceptions.
//
//  Revision 1.38  2005/09/07 03:38:14  anton
//  cisstVector: Added BoolType to cmnTypeTraits in order to delay the
//  instantiation of the ElementWiseCompare methods for
//  vctDynamic{Vector,Matrix}ConstBase.  Credit goes to Ofri for this solution.
//  See #162.
//
//  Revision 1.37  2005/08/11 22:53:41  anton
//  vctDynamicMatrix: Safer implementation of RowPointers.  See #161.
//
//  Revision 1.36  2005/08/11 21:39:51  anton
//  vctDynamicMatrix: Added experimental RowPointers().  See ticket #161.
//
//  Revision 1.35  2005/08/11 21:29:44  ofri
//  cisstVector -- base classes ToStream(): Added code to preserve the previous
//  state of the stream (precision, width, showpoint) and resume it before returning
//  from the method.
//
//  Revision 1.34  2005/08/04 15:31:57  anton
//  cisstVector: Replaced RangeCheck() by ThrowUnlessValidIndex().  For dynamic
//  matrices, Row() and Col() now check the index and throw using
//  ThrowUnlessValid{Row,Col}Index().
//
//  Revision 1.33  2005/07/25 19:44:45  anton
//  vctDynamicConstMatrixBase: Bug in IsCompact().
//
//  Revision 1.32  2005/05/19 19:29:00  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.31  2005/04/28 03:55:23  anton
//  cisstVector:  Added AlmostEqual() to all containers to compare with
//  tolerance.
//
//  Revision 1.30  2005/04/04 19:48:34  anton
//  cisstVector: In base types for dynamic containers, removed default value for
//  _elementType (was based on _ownerType::value_type) to -1- avoid issues with
//  SWIG and -2- force explicit declaration of _elementType whenever a base type
//  is used.
//
//  Revision 1.29  2005/04/02 23:04:49  anton
//  cisstVector:  Added conditional compilation for SWIG in dynamic containers.
//
//  Revision 1.28  2005/01/06 23:36:00  anton
//  cisstVector Python: As mentioned in ticket #112, the introduction of the
//  class vctContainerTraits broke the Python SWIG wrapping.  Found a solution
//  for SWIG 1.3.22 (it is known that 1.3.24 DOES NOT WORK).
//
//  Revision 1.27  2005/01/05 18:29:49  anton
//  cisstVector: Added ValidIndex() methods for all containers (and tests) as
//  requested in ticket #111.
//
//  Revision 1.26  2004/12/01 21:51:48  anton
//  cisstVector: Use the NeutralElement() of the incremental operations for
//  the dynamic container engines (see #97).  Also rewrote the vector engines
//  to use a single index with [] instead of multiple interators.
//
//  Revision 1.25  2004/12/01 17:14:53  anton
//  cisstVector:  Replace access to matrix elements by Element() instead of
//  double [][] for increased performance.  See ticket #79.
//
//  Revision 1.24  2004/12/01 16:42:20  anton
//  cisstVector: Modified the signature of Pointer methods for matrices.  We now
//  have two signature, Pointer(void) and Pointer(row, col) with no default value
//  to avoid the ambiguous Pointer(index).  See ticket #93.
//
//  Revision 1.23  2004/11/30 23:36:05  anton
//  cisstVector: Added the Element() method to access an element without range
//  check.  See ticket #79.
//
//  Revision 1.22  2004/11/11 20:35:46  anton
//  cisstVector: *: Added a vctContainerTraits to centralize the declarations
//  of size_type, difference_type, reference, const_reference, etc. *: All
//  iterators are now derived from std::iterator. *: Added some missing typedef
//  for iterators.
//
//  Revision 1.21  2004/11/08 18:10:17  anton
//  cisstVector: Completed the use of VCT_STORAGE_ORDER, added related Doxygen
//  documentation.
//
//  Revision 1.20  2004/11/03 22:26:12  anton
//  cisstVector:  Better support of storage order.  Added VCT_COL_MAJOR,
//  VCT_ROW_MAJOR and VCT_DEFAULT_STORAGE as well as the methods IsColMajor(),
//  IsRowMajor(), IsCompact() and IsFortran().
//
//  Revision 1.19  2004/10/26 22:31:37  ofri
//  vctDynamic(Const)MatrixBase: Rolling back erroneous change from [848]
//
//  Revision 1.18  2004/10/26 17:14:18  ofri
//  Syntax error correction.
//
//  Revision 1.17  2004/10/26 15:49:03  ofri
//  vct[FixedSize|Dynamic](Const)MatrixBase classes: Added reference access to
//  the main diagonal (ticket #76).  Fixed bug in the Row() and Column() methods
//  of the dynamic matrix base classes : incorrect stride.
//
//  Revision 1.16  2004/10/26 15:20:52  ofri
//  Updating subsequence interfaces (ticket #76) and interfaces between fixed
//  and dynamic vectors/matrices (ticket #72).
//  *: vctFixedSize(Const)VectorBase::Get(Const)Subsequence now deprecated.
//  Replaced by Get(Const)Subvector with equal stride to the parent.
//  *: Added constructors for [Fixed|Dynamic][Vector|Matrix]Ref from the other
//  storage type.
//
//  Revision 1.15  2004/10/25 19:19:54  anton
//  cisstVector:  Created vctForwardDeclarations.h and removed forward
//  declarations of classes in all other files.  Added some constructors
//  for vector references to reference fixed size from dynamic and vice versa.
//
//  Revision 1.14  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.13  2004/10/21 19:27:47  anton
//  cisstVector: Added types BoolValueType and ValueType for
//  vctFixedSizeVector, vctDynamicVector, vctDynamicMatrix.  This solves #77.
//
//  Revision 1.12  2004/10/14 18:53:20  ofri
//  vctDynamicConstMatrixBase: Following ticket #72 added some documentation and:
//  1: operator() (row, col)
//  1: Transpose() method that returns a vctReturnDynamicMatrix object.
//
//  Revision 1.11  2004/10/08 19:38:58  anton
//  cisstVector: Added a space between elements in ToStream methods.
//
//  Revision 1.10  2004/10/07 21:20:29  ofri
//  Added method vctDynamicConstMatrixBase::TransposeRef() and related types.
//
//  Revision 1.9  2004/09/30 15:49:51  anton
//  cisstVector: solution for ticket #65 regarding a compilation error with
//  .Net 2003.  The issue remains mysterious but but a solution is to declare
//  the problematic methods in the class declaration and implement them right
//  after (in the same file).  The methods in question are the Elementwise
//  comparaison with a scalar (it works if comparing with a vector/matrix).
//
//  Revision 1.8  2004/09/24 20:20:42  anton
//  cisstVector: Major update related to ticket #18 regarding order operators
//  and methods.  The methods named Equal(), NotEqual(), Lesser(),
//  LesserOrEqual(), etc.  return a boolean.  The same names with prefix
//  Elementwise (e.g. ElementwiseEqual) return a vector or matrix of the tests
//  performed for each element.  We only kept the operators == and != which
//  return a boolean.  All other operators (>, >=, <, <=) have been removed.
//  Also, in order to have the same API for all the containers, many methods
//  have been added (vctAny, vctAll, etc... see tickets #52 and #61).  The
//  doxygen comments have also been updated.
//
//  Revision 1.7  2004/09/08 21:22:17  anton
//  vctDynamicMatrix: Added method ProductOf() as well as operator * for two
//  matrices.  Needed to implement Column() and Row() methods as well.
//  Corrected bugs in ToStream method.
//
//  Revision 1.6  2004/09/03 19:56:36  anton
//  Doxygen documentation.
//
//  Revision 1.5  2004/08/16 19:22:53  anton
//  cisstVector: Added bound checking for matrices as well as method "at"
//  with two parameters (row and col indices).  See also ticket #13 and CVS
//  commit [699]
//
//  Revision 1.4  2004/08/13 17:47:40  anton
//  cisstVector: Massive renaming to replace the word "sequence" by "vector"
//  (see ticket #50) as well as another more systematic naming for the engines.
//  The changes for the API are as follow:
//  *: vctFixedLengthSequenceBase -> vctFixedSizeVectorBase (and Const)
//  *: vctFixedLengthSequenceRef -> vctFixedSizeVectorRef (and Const)
//  *: vctDynamicSequenceBase -> vctDynamicVectorBase (and Const)
//  *: vctDynamicSequenceRef -> vctDynamicVectorRef (and Const)
//  *: vctDynamicSequenceRefOwner -> vctDynamicVectorRefOwner
//  *: vctFixedStrideSequenceIterator -> vctFixedStrideVectorIterator (and Const)
//  *: vctVarStrideSequenceIterator -> vctVarStrideVectorIterator (and Const)
//  *: vctSequenceRecursiveEngines -> vctFixedSizeVectorRecursiveEngines
//  *: vctSequenceLoopEngines -> vctDynamicVectorLoopEngines
//  *: vctMatrixLoopEngines -> vctFixedSizeMatrixLoopEngines
//  *: vctDynamicMatrixEngines -> vctDynamicMatrixLoopEngines
//  Also updated and corrected the documentation (latex, doxygen, figures) as
//  well as the tests and examples.
//
//  Revision 1.3  2004/08/12 20:23:52  anton
//  cisstVector: Attempt to solve an overloaded function conflict with .NET 2003
//  by casting the input parameter of sqrt to {quote: NormType}
//
//  Revision 1.2  2004/08/11 19:21:32  anton
//  cisstVector: the operator << and the methods ToString and ToStream have
//  been improved.
//
//  Revision 1.1  2004/08/04 21:11:10  anton
//  cisstVector: Added preliminary version of dynamic matrices.  Lots of work
//  is still required, this code is not ready to be used.
//
//
// ****************************************************************************
