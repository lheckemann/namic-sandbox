/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicMatrixBase.h,v 1.42 2006/05/21 11:27:06 ofri Exp $
  
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


#ifndef _vctDynamicMatrixBase_h 
#define _vctDynamicMatrixBase_h

/*!
  \file
  \brief Declaration of vctDynamicMatrixBase
*/


#include <cstdarg>
#include<cisstVector/vctDynamicConstMatrixBase.h>
#include<cisstVector/vctStoreBackUnaryOperations.h>
#include<cisstVector/vctStoreBackBinaryOperations.h>


/*!
  This class provides all the const methods inherited from
  vctConstMatrixBase, and extends them with non-const methods, such
  as SumOf.

  \sa vctDynamicConstMatrixBase

*/
template<class _matrixOwnerType, typename _elementType>
class vctDynamicMatrixBase : public vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    typedef vctDynamicMatrixBase ThisType;
    /*! Type of the base class. */
    typedef vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> BaseType;
    
    typedef _matrixOwnerType MatrixOwnerType;

    typedef typename BaseType::iterator iterator; 
    typedef typename BaseType::reverse_iterator reverse_iterator; 
    typedef typename BaseType::const_iterator const_iterator; 
    typedef typename BaseType::const_reverse_iterator const_reverse_iterator; 

    typedef typename BaseType::MatrixSizeType MatrixSizeType;

    typedef typename BaseType::ConstRowRefType ConstRowRefType;
    typedef typename BaseType::RowRefType RowRefType;

    typedef typename BaseType::ConstColumnRefType ConstColumnRefType;
    typedef typename BaseType::ColumnRefType ColumnRefType;

    typedef typename BaseType::ConstDiagonalRefType ConstDiagonalRefType;
    typedef typename BaseType::DiagonalRefType DiagonalRefType;

    typedef typename BaseType::ConstRefTransposeType ConstRefTransposeType;
    typedef typename BaseType::RefTransposeType RefTransposeType;

    typedef typename BaseType::ConstVectorPointerType ConstVectorPointerType;
    typedef typename BaseType::VectorPointerType VectorPointerType;


    /*! Returns an iterator on the first element (STL
      compatibility). */
    iterator begin(void) {
        return this->Matrix.begin(); 
    }

    /*! Returns an iterator on the last element (STL
      compatibility). */
    iterator end(void) {
        return this->Matrix.end(); 
    }

    /*! Returns a reverse iterator on the last element (STL
      compatibility). */ 
    reverse_iterator rbegin(void) {
        return this->Matrix.rbegin(); 
    }

    /*! Returns a reverse iterator on the element before first
      (STL compatibility). */ 
    reverse_iterator rend(void) {
        return this->Matrix.rend(); 
    }

    /* documented in base class */
    const_iterator begin(void) const {
        return this->Matrix.begin(); 
    }

    /* documented in base class */
    const_iterator end(void) const {
        return this->Matrix.end(); 
    }

    /* documented in base class */
    const_reverse_iterator rbegin(void) const {
        return this->Matrix.rbegin(); 
    }

    /* documented in base class */
    const_reverse_iterator rend(void) const {
        return this->Matrix.rend(); 
    }
    
    /*! Reference a row of this matrix by index.
      \return a reference to the element[index] */
    RowRefType operator[](size_type index) {
        return RowRefType(this->cols(), Pointer(index, 0), this->col_stride());
    }

    /* documented in base class */
    ConstRowRefType operator[](size_type index) const {
        return ConstRowRefType(this->cols(), Pointer(index, 0), this->col_stride());
    }

    
    /*! Return a non const pointer to an element of the container,
      specified by its indices. Addition to the STL requirements.
    */
    pointer Pointer(size_type rowIndex, size_type colIndex) {
        return this->Matrix.Pointer(rowIndex, colIndex);
    }

    /*! Returns a non const pointer to the first element of the
      container. Addition to the STL requirements.
    */
    pointer Pointer(void) {
        return this->Matrix.Pointer();
    }

    /* documented in base class */
    const_pointer Pointer(size_type rowIndex, size_type colIndex) const {
        return BaseType::Pointer(rowIndex, colIndex);
    }

    /* documented in base class */
    const_pointer Pointer(void) const {
        return BaseType::Pointer();
    }


    /*! Access an element by index (const).  Compare with
      std::vector::at.  This method can be a handy substitute for the
      overloaded operator [] when operator overloading is unavailable
      or inconvenient.

      \return a non-const reference to element[index] */
    reference at(size_type index) throw(std::out_of_range) {
        this->ThrowUnlessValidIndex(index);
        return (begin())[index];
    }

    
    /* documented in base class */
    const_reference at(size_type index) const throw(std::out_of_range) {
        this->ThrowUnlessValidIndex(index);
        return (begin())[index];
    }


    /*! Access an element by index.  Compare with std::vector::at().
      This method can be a handy substitute for the overloaded operator () when
      operator overloading is unavailable or inconvenient.
      \return a reference to the element at rowIndex, colIndex */
    reference at(size_type rowIndex, size_type colIndex) throw(std::out_of_range) {
        this->ThrowUnlessValidIndex(rowIndex, colIndex);
        return *(Pointer(rowIndex, colIndex));
    }

    /* documented in base class */
    const_reference at(size_type rowIndex, size_type colIndex) const throw(std::out_of_range) {
        this->ThrowUnlessValidIndex(rowIndex, colIndex);
        return *(Pointer(rowIndex, colIndex));
    }


    /*! Overloaded operator () for simplified (const) element access with bounds checking */
    const_reference operator () (size_type rowIndex, size_type colIndex) const throw(std::out_of_range) {
        return this->at(rowIndex, colIndex);
    }
    /*! Overloaded operator () for simplified (non const) element access with bounds checking */
    reference operator () (size_type rowIndex, size_type colIndex) throw(std::out_of_range) {
        return this->at(rowIndex, colIndex);
    }


    /*! Access an element by indices (non const). This method allows
      to access an element without any bounds checking.  It doesn't
      create any temporary row reference as a matrix[][] would do.

      \return a reference to the element at rowIndex, colIndex */
    reference Element(size_type rowIndex, size_type colIndex) {
        return *(Pointer(rowIndex, colIndex));
    }

    /* documented in base class */
    const_reference Element(size_type rowIndex, size_type colIndex) const {
        return *(Pointer(rowIndex, colIndex));
    }


    /*! \name Row and column references. */
    //@{
    /*! Create a row reference. */  
    RowRefType Row(size_type index) throw(std::out_of_range) {
        this->ThrowUnlessValidRowIndex(index);
        return RowRefType(this->cols(), Pointer(index, 0), this->col_stride());
    }
    
    /*! Create a column reference. */  
    ColumnRefType Column(size_type index) throw(std::out_of_range) {
        this->ThrowUnlessValidColIndex(index);
        return ColumnRefType(this->rows(), Pointer(0, index), this->row_stride());
    }

    /*! Create a non-const reference to the main diagonal of this matrix */
    DiagonalRefType Diagonal() {
        return DiagonalRefType( std::min(this->rows(), this->cols()), Pointer(0, 0), this->row_stride() + this->col_stride() );
    }

    /*! Resize and fill a vector of pointers on the rows of the
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
    VectorPointerType & RowPointers(VectorPointerType & rowPointers) throw(std::runtime_error) {
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


    /*! Create a const row reference. */  
    ConstRowRefType Row(size_type index) const throw(std::out_of_range) {
        this->ThrowUnlessValidRowIndex(index);
        return ConstRowRefType(this->cols(), Pointer(index, 0), this->col_stride());
    }
    
    /*! Create a const column reference. */  
    ConstColumnRefType Column(size_type index) const throw(std::out_of_range) {
        this->ThrowUnlessValidColIndex(index);
        return ConstColumnRefType(this->rows(), Pointer(0, index), this->row_stride());
    }

    /*! Create a const reference to the main diagonal of this matrix */
    ConstDiagonalRefType Diagonal() const {
        return ConstDiagonalRefType( std::min(this->rows(), this->cols()), Pointer(0, 0), this->row_stride() + this->col_stride() );
    }

    /* documented in base class */
    ConstVectorPointerType RowPointers(ConstVectorPointerType & rowPointers) const throw(std::runtime_error) {
        return BaseType::RowPointers(rowPointers);
    }

    //@}
    
    //@{ Methods to change the order of rows and columns of a matrix
    /*! Exchange two rows of the matrix */
    void ExchangeRows(const size_type row1Index, const size_type row2Index) {
        RowRefType row1( Row(row1Index) );
        RowRefType row2( Row(row2Index) );
        row1.SwapElementsWith(row2);
    }

    /*! Exchange two colums of the matrix */
    void ExchangeColumns(const size_type col1Index, const size_type col2Index) {
        ColumnRefType col1( Column(col1Index) );
        ColumnRefType col2( Column(col2Index) );
        col1.SwapElementsWith(col2);
    }

    /*! Assign a permutation of the rows of the input matrix to the rows of this matrix.
      Both matrices must have the same size.
      \param inputMatrix the input matrix for the permutation.
      \param permutedRowIndexes an array of row indices.  The assignment performed is:
      this->Row(i) <-- inputMatrix.Row( permutedRowIndexes[i] ).

      \note The current implementation does not validate that the input permuted
      indexes is an actual permutation of the numbers 0..(ROWS-1).  Nor does it
      assure that the input permutation array has the right size.  Both are the
      caller's responsibility.

      \note Do not use this method for an in-place permutation of the input matrix.
    */
    template<class __matrixOwnerType>
    void RowPermutationOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & inputMatrix,
    const unsigned int permutedRowIndexes[])
    {
        const size_type numRows = this->rows();
        size_type thisRowIndex;
        for (thisRowIndex = 0; thisRowIndex < numRows; ++thisRowIndex) {
            Row(thisRowIndex).Assign( inputMatrix.Row(permutedRowIndexes[thisRowIndex]) );
        }
    }

    /*! Assign a permutation of the rows of the input matrix to the rows of this matrix.
      Both matrices must have the same size.
      \param inputMatrix the input matrix for the permutation.
      \param permutedRowIndexes an array of row indices.  The assignment performed is:
      this->Row( permutedRowIndexes[i] ) <-- inputMatrix.Row(i).

      \note The current implementation does not validate that the input permuted
      indexes is an actual permutation of the numbers 0..(ROWS-1).  Nor does it
      assure that the input permutation array has the right size.  Both are the
      caller's responsibility.

      \note Do not use this method for an in-place permutation of the input matrix.
    */
    template<class __matrixOwnerType>
    void RowInversePermutationOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & inputMatrix,
    const unsigned int permutedRowIndexes[])
    {
        const size_type numRows = this->rows();
        size_type thisRowIndex;
        for (thisRowIndex = 0; thisRowIndex < numRows; ++thisRowIndex) {
            Row(permutedRowIndexes[thisRowIndex]).Assign( inputMatrix.Row(thisRowIndex) );
        }
    }

    /*! Assign a permutation of the columns of the input matrix to the column of this matrix.
      Both matrices must have the same size.
      \param inputMatrix the input matrix for the permutation.
      \param permutedColumnIndexes an array of column indices.  The assignment performed is:
      this->Column(i) <-- inputMatrix.Column( permutedColumnIndexes[i] ).

      \note The current implementation does not validate that the input permuted indexes
      is an actual permutation of the numbers 0..(COLS-1).  Nor does it assure that the input
      permutation array has the right size.  Both are the caller's responsibility.

      \note Do not use this method for an in-place permutation of the input matrix.
    */
    template<class __matrixOwnerType>
    void ColumnPermutationOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & inputMatrix,
    const unsigned int permutedColumnIndexes[])
    {
        const size_type numCols = this->cols();
        size_type thisColumnIndex;
        for (thisColumnIndex = 0; thisColumnIndex < numCols; ++thisColumnIndex) {
            Column(thisColumnIndex).Assign( inputMatrix.Column(permutedColumnIndexes[thisColumnIndex]) );
        }
    }

    /*! Assign a permutation of the columns of the input matrix to the column of this matrix.
      Both matrices must have the same size.
      \param inputMatrix the input matrix for the permutation.
      \param permutedColumnIndexes an array of column indices.  The assignment performed is:
      this->Column( permutedColumnIndexes[i] ) <-- inputMatrix.Column(i).

      \note The current implementation does not validate that the input permuted indexes
      is an actual permutation of the numbers 0..(COLS-1).  Nor does it assure that the input
      permutation array has the right size.  Both are the caller's responsibility.

      \note Do not use this method for an in-place permutation of the input matrix.
    */
    template<class __matrixOwnerType>
    void ColumnInversePermutationOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & inputMatrix,
    const unsigned int permutedColumnIndexes[])
    {
        const size_type numCols = this->cols();
        size_type thisColumnIndex;
        for (thisColumnIndex = 0; thisColumnIndex < numCols; ++thisColumnIndex) {
            Column(permutedColumnIndexes[thisColumnIndex]).Assign( inputMatrix.Column(thisColumnIndex) );
        }
    }
    //@}

    /*! Assign the given value to all the elements.
      \param value the value used to set all the elements of the matrix
      \return The value used to set all the elements
    */
    inline value_type SetAll(const value_type value) {
        vctDynamicMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::SecondOperand>::
            Run(*this, value);
        return value;
    }


    /*!
      \name Assignment operation between matrices of different types.
      
      \param other The matrix to be copied.
    */
    //@{
    template<class __matrixOwnerType, typename __elementType>
    inline ThisType & Assign(const vctDynamicConstMatrixBase<__matrixOwnerType, __elementType> & other) {
        vctDynamicMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type,
            typename __matrixOwnerType::value_type>::Identity>::
            Run(*this, other);
    return *this;
    }

    template<class __matrixOwnerType, typename __elementType>
    inline ThisType & operator = (const vctDynamicConstMatrixBase<__matrixOwnerType, __elementType> & other) {
        return this->Assign(other);
    }

    template<unsigned int __rows, unsigned int __cols, int __rowStride, int __colStride, class __elementType, class __dataPtrType>
    inline ThisType & Assign(const vctFixedSizeConstMatrixBase<__rows, __cols, __rowStride, __colStride, __elementType, __dataPtrType>
                             & other) {
        vctDynamicMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type, __elementType>::Identity>::
            Run(*this, other);
        return *this;
    }
    //@}

    /*! Assign to this matrix values from a C array given as a
      pointer to value_type.  The purpose of this method is to simplify
      the syntax by not necessitating the creation of an explicit matrix
      for the given array.  However, we only provide this method for
      an array of value_type.  For arrays of other types a matrix
      still needs to be declared.

      \param elements a pointer to a C array of elements to be assigned to
      this matrix.
      \param inputIsRowMajor a flag indicating the storage order of the elements
      in the input array.

      \note For lack of better knowledge, this method assumes that the input
      array is _packed_, that is, that all the elements are stored in a
      contiguous memory block with no gaps.  The only existing options now relate
      to the storage order of the input elements.  If the input is not packed,
      you should create a MatrixRef explicitly, with stride specifications,
      or use other tricks.

      \return a reference to this object.
    */
    inline ThisType & Assign(const value_type * elements, bool inputIsRowMajor = true)
    {
        // The row stride is the difference between two rows.  That is, in row-major
        // storage it is equal to the number of columns, and in column-major it is
        // equal to 1.
        const difference_type inputRowStride = (inputIsRowMajor) ? this->cols() : 1;
        const difference_type inputColStride = (inputIsRowMajor) ? 1 : this->rows();
        const vctDynamicConstMatrixRef<value_type> tmpRef( this->rows(), this->cols(),
            inputRowStride, inputColStride, elements );
        this->Assign(tmpRef);
        return *this;
    }


    /*! Assign to this matrix a set of values provided as independent
      arguments, by using cstdarg macros, that is, an unspecified
      number of arguments.  This operation assumes
      that all the arguments are of type value_type, and that their
      number is equal to the size of the matrix.  The arguments are
      passed <em>by value</em>.  The user may need to explicitly cast
      the parameters to value_type to avoid runtime bugs and errors.
      The order of the paramaters is row first which allows to keep
      the code pretty intuitive:
      \code
      matrix.Assign( 0.0, 1.0,
                    -1.0, 0.0);
      \endcode

      \return a reference to this matrix.
    */
    inline ThisType & Assign(const value_type element0, ...)
    {
        iterator iter = begin();
        (*iter) = element0;
        ++iter;
        va_list nextArg;
        va_start(nextArg, element0);
        for (; iter != end(); ++iter) {
            (*iter) = static_cast<value_type>(va_arg(nextArg, typename cmnTypeTraits<value_type>::VaArgPromotion));
        }
        va_end(nextArg);
        return *this;
    }



    /*! Return a transposed reference to this matrix.
    */
    RefTransposeType TransposeRef(void)
    {
        return RefTransposeType(this->cols(), this->rows(), this->col_stride(), this->row_stride(), Pointer());
    }

    /* documented in base class */
    ConstRefTransposeType TransposeRef(void) const {
        return BaseType::TransposeRef();
    }

    /*! \name Binary elementwise operations between two matrices.
      Store the result of op(matrix1, matrix2) to a third matrix. */
    //@{
    /*! Binary elementwise operations between two matrices.  For each
      element of the matrices, performs \f$ this[i] \leftarrow
      op(matrix1[i], matrix2[i])\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ElementwiseProductOf), a division (ElementwiseRatioOf), a
      minimum (ElementwiseMinOf) or a maximum (ElementwiseMaxOf).

      \param matrix1 The first operand of the binary operation

      \param matrix2 The second operand of the binary operation
      
      \return The matrix "this" modified.
    */
    template<class __matrixOwnerType1, class __matrixOwnerType2>
    inline const ThisType & SumOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                                  const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        vctDynamicMatrixLoopEngines::
            MoMiMi< typename vctBinaryOperations<value_type>::Addition >
            ::Run(*this, matrix1, matrix2);
        return *this;
    }    
    
    /* documented above */
    template<class __matrixOwnerType1, class __matrixOwnerType2>
    inline const ThisType & DifferenceOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                                         const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        vctDynamicMatrixLoopEngines::
            MoMiMi< typename vctBinaryOperations<value_type>::Subtraction >
            ::Run(*this, matrix1, matrix2);
        return *this;
    }
    
    /* documented above */
    template<class __matrixOwnerType1, class __matrixOwnerType2>
    inline const ThisType & ElementwiseProductOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                                                 const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        vctDynamicMatrixLoopEngines::
            MoMiMi< typename vctBinaryOperations<value_type>::Multiplication >
            ::Run(*this, matrix1, matrix2);
        return *this;
    }
    
    /* documented above */
    template<class __matrixOwnerType1, class __matrixOwnerType2>
    inline const ThisType & ElementwiseRatioOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                                               const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        vctDynamicMatrixLoopEngines::
            MoMiMi< typename vctBinaryOperations<value_type>::Division >
            ::Run(*this, matrix1, matrix2);
        return *this;
    }

    /* documented above */
    template<class __matrixOwnerType1, class __matrixOwnerType2>
    inline const ThisType & ElementwiseMinOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                                             const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        vctDynamicMatrixLoopEngines::
            MoMiMi< typename vctBinaryOperations<value_type>::Minimum >
            ::Run(*this, matrix1, matrix2);
        return *this;
    }

    /* documented above */
    template<class __matrixOwnerType1, class __matrixOwnerType2>
    inline const ThisType & ElementwiseMaxOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                                             const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        vctDynamicMatrixLoopEngines::
            MoMiMi< typename vctBinaryOperations<value_type>::Maximum >
            ::Run(*this, matrix1, matrix2);
        return *this;
    }
    //@}



    /*! \name Binary elementwise operations between two matrices.
      Store the result of op(this, otherMatrix) back to this matrix. */
    //@{
    /*! Store back binary elementwise operations between two
      matrices.  For each element of the matrices, performs \f$
      this[i] \leftarrow op(this[i], otherMatrix[i])\f$ where
      \f$op\f$ is either an addition (Add), a subtraction
      (Subtraction), a multiplication (ElementwiseMultiply) a division
      (ElementwiseDivide), a minimization (ElementwiseMin) or a
      maximisation (ElementwiseMax).
      
      \param otherMatrix The second operand of the binary operation
      (this[i] is the first operand)
      
      \return The matrix "this" modified.
    */
    template<class __matrixOwnerType>
    inline ThisType & Add(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline ThisType & Subtract(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MioMi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Run(*this, otherMatrix);
        return *this;
    }
    
    /* documented above */
    template<class __matrixOwnerType>
    inline ThisType & ElementwiseMultiply(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MioMi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Run(*this, otherMatrix);
        return *this;
    }
    
    /* documented above */
    template<class __matrixOwnerType>
    inline ThisType & ElementwiseDivide(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MioMi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Run(*this, otherMatrix);
        return *this;
    }
    
    /* documented above */
    template<class __matrixOwnerType>
    inline ThisType & ElementwiseMin(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MioMi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline ThisType & ElementwiseMax(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MioMi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline const ThisType & operator += (const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        return this->Add(otherMatrix);
    }
    
    /* documented above */
    template<class __matrixOwnerType>
    inline const ThisType & operator -= (const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        return this->Subtract(otherMatrix);
    }
    //@}

   
    /*! \name Binary elementwise operations a matrix and a scalar.
      Store the result of op(matrix, scalar) to a third matrix. */
    //@{
    /*! Binary elementwise operations between a matrix and a scalar.
      For each element of the matrix "this", performs \f$ this[i]
      \leftarrow op(matrix[i], scalar)\f$ where \f$op\f$ is either
      an addition (SumOf), a subtraction (DifferenceOf), a
      multiplication (ProductOf), a division (RatioOf), a minimum
      (MinOf) or a maximum (MaxOf).

      \param matrix The first operand of the binary operation.
      \param scalar The second operand of the binary operation.
      
      \return The matrix "this" modified.
    */
    template<class __matrixOwnerType>
    inline const ThisType & SumOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix, 
                                  const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MoMiSi< typename vctBinaryOperations<value_type>::Addition >::
            Run(*this, matrix, scalar);
        return *this;
    }
    
    /* documented above */
    template<class __matrixOwnerType>
    inline const ThisType & DifferenceOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
                                         const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MoMiSi< typename vctBinaryOperations<value_type>::Subtraction >::
            Run(*this, matrix, scalar);
        return *this;
    }
    
    /* documented above */    
    template<class __matrixOwnerType>
    inline const ThisType & ProductOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
                                      const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MoMiSi< typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, matrix, scalar);
        return *this;
    }
    
    /* documented above */
    template<class __matrixOwnerType>
    inline const ThisType & RatioOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
                                    const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MoMiSi< typename vctBinaryOperations<value_type>::Division >::
            Run(*this, matrix, scalar);
        return *this;
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline const ThisType & MinOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
                                  const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Minimum>::
            Run(*this, matrix, scalar);
        return *this;
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline const ThisType & MaxOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix,
                                  const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MoMiSi< typename vctBinaryOperations<value_type>::Maximum >::
            Run(*this, matrix, scalar);
        return *this;
    }
    //@}
    


    /*! \name Binary elementwise operations a scalar and a matrix.
      Store the result of op(scalar, matrix) to a third matrix. */
    //@{
    /*! Binary elementwise operations between a scalar and a matrix.
      For each element of the matrix "this", performs \f$ this[i]
      \leftarrow op(scalar, matrix[i])\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ProductOf) or a division (RatioOf).

      \param scalar The first operand of the binary operation.
      \param matrix The second operand of the binary operation.
      
      \return The matrix "this" modified.
    */
    template<class __matrixOwnerType>
    inline const ThisType & SumOf(const value_type scalar,
                                  const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix) {
        vctDynamicMatrixLoopEngines::
            MoSiMi< typename vctBinaryOperations<value_type>::Addition >::
            Run(*this, scalar, matrix);
        return *this;
    }
    
    /* documented above */
    template<class __matrixOwnerType>
    inline const ThisType & DifferenceOf(const value_type scalar,
                                         const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix) {
        vctDynamicMatrixLoopEngines::
            MoSiMi< typename vctBinaryOperations<value_type>::Subtraction >::
            Run(*this, scalar, matrix);
        return *this;
    }
    
    /* documented above */
    template<class __matrixOwnerType>
    inline const ThisType & ProductOf(const value_type scalar,
                                      const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix) {
        vctDynamicMatrixLoopEngines::
            MoSiMi< typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar, matrix);
        return *this;
    }
    
    /* documented above */
    template<class __matrixOwnerType>
    inline const ThisType & RatioOf(const value_type scalar,
                                    const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix) {
        vctDynamicMatrixLoopEngines::
            MoSiMi< typename vctBinaryOperations<value_type>::Division >::
            Run(*this, scalar, matrix);
        return *this;
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline const ThisType & MinOf(const value_type scalar,
                                  const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix) {
        vctDynamicMatrixLoopEngines::
            MoSiMi< typename vctBinaryOperations<value_type>::Minimum >::
            Run(*this, scalar, matrix);
        return *this;
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline const ThisType & MaxOf(const value_type scalar,
                                  const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & matrix) {
        vctDynamicMatrixLoopEngines::
            MoSiMi< typename vctBinaryOperations<value_type>::Maximum >::
            Run(*this, scalar, matrix);
        return *this;
    }
    //@}



    /*! \name Binary elementwise operations between a matrix and a scalar.
      Store the result of op(this, scalar) back to this matrix. */
    //@{
    /*! Store back binary elementwise operations between a matrix
      and a scalar.  For each element of the matrix "this", performs
      \f$ this[i] \leftarrow op(this[i], scalar)\f$ where \f$op\f$ is
      either an addition (Add), a subtraction (Subtract), a
      multiplication (Multiply), a division (Divide), a minimum (Min)
      or a maximum (Max).

      \param scalar The second operand of the binary operation
      (this[i] is the first operand.
      
      \return The matrix "this" modified.
    */
    inline ThisType & Add(const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MioSi< typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Run(*this, scalar);
        return *this;
    }
    
    /* documented above */
    inline ThisType & Subtract(const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MioSi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Run(*this, scalar);
        return *this;
    }
    
    /* documented above */
    inline ThisType & Multiply(const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MioSi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar);
        return *this;
    }
    
    /* documented above */
    inline ThisType & Divide(const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MioSi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Run(*this, scalar);
        return *this;
    }
    
    /* documented above */
    inline ThisType & Min(const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MioSi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Max(const value_type scalar) {
        vctDynamicMatrixLoopEngines::
            MioSi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline const ThisType & operator += (const value_type scalar) {
        return this->Add(scalar);
    }
    
    /* documented above */
    inline const ThisType & operator -= (const value_type scalar) {
        return this->Subtract(scalar);
    }

    /* documented above */
    inline const ThisType & operator *= (const value_type scalar) {
        return this->Multiply(scalar);
    }

    /* documented above */
    inline const ThisType & operator /= (const value_type scalar) {
        return this->Divide(scalar);
    }
    //@}


    template<class __matrixOwnerType>
    inline ThisType & MultAdd(const value_type scalar,
    const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix)
    {
        vctDynamicMatrixLoopEngines::
            MioSiMi<
            typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar, otherMatrix);
        return *this;
    }


    /*! \name Unary elementwise operations.
      Store the result of op(matrix) to another matrix. */
    //@{
    /*! Unary elementwise operations on a matrix.  For each element of
      the matrix "this", performs \f$ this[i] \leftarrow
      op(otherMatrix[i])\f$ where \f$op\f$ can calculate the absolute
      value (AbsOf), the opposite (NegationOf) or the transpose
      (TransposeOf).

      \param otherMatrix The operand of the unary operation.
      
      \return The matrix "this" modified.
    */
    template<class __matrixOwnerType>
    inline ThisType & AbsOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline ThisType & NegationOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::Negation>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline ThisType & FloorOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::Floor>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template<class __matrixOwnerType>
    inline ThisType & CeilOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        vctDynamicMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::Ceil>::
            Run(*this, otherMatrix);
        return *this;
    }

    template<class __matrixOwnerType>
    inline ThisType & TransposeOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & otherMatrix) {
        Assign(otherMatrix.TransposeRef());
        return *this;
    }

    //@}

    /*! \name Store back unary elementwise operations.
      Store the result of op(this) to this matrix. */
    //@{
    /*! Unary elementwise operations on a matrix.  For each element of
      the matrix "this", performs \f$ this[i] \leftarrow
      op(this[i])\f$ where \f$op\f$ can calculate the absolute
      value (AbsSelf) or the opposite (NegationSelf).
      
      \return The matrix "this" modified.
    */
    inline const ThisType & AbsSelf(void) {
        vctDynamicMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeAbs>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline const ThisType & NegationSelf(void) {
        vctDynamicMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeNegation>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline const ThisType & FloorSelf(void) {
        vctDynamicMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeFloor>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline const ThisType & CeilSelf(void) {
        vctDynamicMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeCeil>::
            Run(*this);
        return *this;
    }
    //@}


    /*! Product of two matrices.  If the sizes of the matrices don't
      match, an exception is thrown.
      
    \param matrix1 The left operand of the binary operation.

    \param matrix2 The right operand of the binary operation.
    
    \return The matrix "this" modified. */
    template<class __matrixOwnerType1, class __matrixOwnerType2>
    void ProductOf(const vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> & matrix1,
                   const vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> & matrix2) {
        typedef vctDynamicConstMatrixBase<__matrixOwnerType1, _elementType> Input1MatrixType;
        typedef vctDynamicConstMatrixBase<__matrixOwnerType2, _elementType> Input2MatrixType;
        typedef typename Input1MatrixType::ConstRowRefType Input1RowRefType;
        typedef typename Input2MatrixType::ConstColumnRefType Input2ColumnRefType;
        vctDynamicMatrixLoopEngines::
            Product<typename vctBinaryOperations<value_type, Input1RowRefType, Input2ColumnRefType>::DotProduct>::
            Run((*this), matrix1, matrix2);   
    }


    /*! Compute the outer product of two vectors and store the result to
      this matrix.  The outer product (v1*v2)[i,j] = v1[i] * v2[j] */
    template<class __vectorOwnerType1, class __vectorOwnerType2>
    void OuterProductOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & colVector,
                        const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & rowVector)
    {
        vctDynamicConstMatrixRef<_elementType> colMatrix, rowMatrix;
        colMatrix.SetRef(colVector.size(), 1, colVector.stride(), 1, colVector.Pointer(0));
        rowMatrix.SetRef(1, rowVector.size(), 1, rowVector.stride(), rowVector.Pointer(0));
        this->ProductOf(colMatrix, rowMatrix);
    }



    /*! Define a Submatrix class for compatibility with the fixed size matrices.
      A submatrix has the same stride as the parent container.

      Example:

      typedef vctDynamicMatrix<double> MatrixType;
      MatrixType M(6,6);
      MatrixType::Submatrix::Type topLeft(M, 0, 0, 3, 3);
      MatrixType::Submatrix::Type bottomRight(M, 3, 3); // implicitely everything left

      \note There is no straightforward way to define a fixed-size
      submatrix of a dynamic matrix, because the stride of the dynamic
      matrix is not known in compilation time.  A way to do it is:

      vctFixedSizeMatrixRef<double, 3, 3, 1, 6> topRight(M, 0, 3);
      vctFixedSizeMatrixRef<double, 3, 3, 1, 6> bottomLeft(M, 3, 0);
    */
#ifndef SWIG
    class Submatrix
    {
    public:
        typedef vctDynamicMatrixRef<value_type> Type;
    };
#endif // SWIG

};


#endif // _vctDynamicMatrixBase_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicMatrixBase.h,v $
//  Revision 1.42  2006/05/21 11:27:06  ofri
//  vct[FixedSize|Dynamic] vectors and matrices: implmented a new operation
//  MultAdd, of the form CioSiCi.
//
//  Revision 1.41  2006/01/26 19:05:21  anton
//  cisstVector: Generalized definition of MatrixSizeType and added documentation
//  (see #206 for motivation).
//
//  Revision 1.40  2006/01/03 03:38:58  anton
//  cisstVector Doxygen: Used \code \endcode instead of <pre> </pre>, added
//  some missing <ul> </ul>, added links to VCT_NORMALIZE.
//
//  Revision 1.39  2005/12/13 00:04:56  ofri
//  All cisstVector containers: Added methods
//  1) MinAndMaxElement  (at const base)
//  2) Floor, Ceil  (at const base -- implemented at container)
//  3) FloorSelf, CeilSelf, FloorOf, CeilOf  (at non-const base)
//
//  Revision 1.38  2005/10/06 16:56:37  anton
//  Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
//  Revision 1.37  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.36  2005/09/24 00:01:04  anton
//  cisstVector: Use cmnThrow for all exceptions.
//
//  Revision 1.35  2005/09/01 06:03:14  anton
//  vctDynamicMatrixBase: Added method TransposeOf() and updated documentation
//  for vctFixedSizeMatrixBase::TransposeOf().
//
//  Revision 1.34  2005/08/14 00:54:00  anton
//  vctDynamicMatrixBase: Added required this-> for gcc 3.4 and 4.0
//
//  Revision 1.33  2005/08/12 12:52:46  anton
//  vctDynamicMatrixBase: Obvious typos in RowPointers() methods.
//
//  Revision 1.32  2005/08/11 22:53:41  anton
//  vctDynamicMatrix: Safer implementation of RowPointers.  See #161.
//
//  Revision 1.31  2005/08/11 21:39:51  anton
//  vctDynamicMatrix: Added experimental RowPointers().  See ticket #161.
//
//  Revision 1.30  2005/08/04 15:31:57  anton
//  cisstVector: Replaced RangeCheck() by ThrowUnlessValidIndex().  For dynamic
//  matrices, Row() and Col() now check the index and throw using
//  ThrowUnlessValid{Row,Col}Index().
//
//  Revision 1.29  2005/07/28 20:19:33  anton
//  vctDynamicMatrixBase: Added OuterProductOf().  Code based on check-in [844]
//  following ticket #69.
//
//  Revision 1.28  2005/07/22 19:10:02  ofri
//  vct[FixedSize|Dynamic]MatrixBase: Changed the documentation and semantics
//  of the Assign(const value_type *) following ticket #154.  The interface now
//  requires packed arrays.
//
//  Revision 1.27  2005/07/19 15:22:57  anton
//  vctDynamicMatrixBase: Added Assign(vctFixedSizeConstMatrixBase).
//
//  Revision 1.26  2005/06/16 03:38:29  anton
//  Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
//  VC 7.1 (.Net 2003).
//
//  Revision 1.25  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.24  2005/04/04 19:48:34  anton
//  cisstVector: In base types for dynamic containers, removed default value for
//  _elementType (was based on _ownerType::value_type) to -1- avoid issues with
//  SWIG and -2- force explicit declaration of _elementType whenever a base type
//  is used.
//
//  Revision 1.23  2005/04/02 23:04:49  anton
//  cisstVector:  Added conditional compilation for SWIG in dynamic containers.
//
//  Revision 1.22  2004/12/17 15:21:56  ofri
//  Fixing syntaxt error in vct[FixedSize|Dynamic]MatrixBase::ExchangeColumns
//  (see ticket #99)
//
//  Revision 1.21  2004/12/07 04:12:26  ofri
//  Following ticket #99:
//  1) Added engine VioVio in vctFixedSizeVectorRecursiveEngines and in
//  vctDynamicVectorLoopEngines
//  2) Added operation Swap in vctStoreBackBinaryOperations
//  3) Added operation SwapElementsWith in vctFixedSizeVectorBase and
//  vctDynamicVectorBase
//  4) Rewrote the methods EchangeRows and ExchangeColumns in
//  vctFixedSizeMatrixBase; added them to vctDynamicMatrixBase.
//  5) Same for row/column permutation methods.
//  I am committing the files after compiling the library successfully and running
//  the tests successfully on Cygwin.
//
//  Revision 1.20  2004/12/01 16:42:20  anton
//  cisstVector: Modified the signature of Pointer methods for matrices.  We now
//  have two signature, Pointer(void) and Pointer(row, col) with no default value
//  to avoid the ambiguous Pointer(index).  See ticket #93.
//
//  Revision 1.19  2004/11/30 23:36:05  anton
//  cisstVector: Added the Element() method to access an element without range
//  check.  See ticket #79.
//
//  Revision 1.18  2004/11/29 17:31:28  anton
//  cisstVector: Major corrections for matrices reverse iterators.  Correction
//  of the - operator which now takes into account the current column position.
//
//  Revision 1.17  2004/11/18 20:57:30  ofri
//  RE: ticket #92.  Redefined all operator= I could find to return a ThisType &
//  instead of const ThisType &.  This included corresponding revision of the
//  Assign methods, and also definition from scratch of operator= for fixed and
//  dynamic matrix ref.
//
//  Revision 1.16  2004/11/11 20:35:46  anton
//  cisstVector: *: Added a vctContainerTraits to centralize the declarations
//  of size_type, difference_type, reference, const_reference, etc. *: All
//  iterators are now derived from std::iterator. *: Added some missing typedef
//  for iterators.
//
//  Revision 1.15  2004/11/03 22:26:12  anton
//  cisstVector:  Better support of storage order.  Added VCT_COL_MAJOR,
//  VCT_ROW_MAJOR and VCT_DEFAULT_STORAGE as well as the methods IsColMajor(),
//  IsRowMajor(), IsCompact() and IsFortran().
//
//  Revision 1.14  2004/11/01 06:53:51  anton
//  vctDynamicMatrixBase.h: Added Assign() from va_list.  See ticket #64 (not
//  solved yet!).
//
//  Revision 1.13  2004/10/26 22:31:37  ofri
//  vctDynamic(Const)MatrixBase: Rolling back erroneous change from [848]
//
//  Revision 1.12  2004/10/26 15:49:03  ofri
//  vct[FixedSize|Dynamic](Const)MatrixBase classes: Added reference access to
//  the main diagonal (ticket #76).  Fixed bug in the Row() and Column() methods
//  of the dynamic matrix base classes : incorrect stride.
//
//  Revision 1.11  2004/10/26 15:20:52  ofri
//  Updating subsequence interfaces (ticket #76) and interfaces between fixed
//  and dynamic vectors/matrices (ticket #72).
//  *: vctFixedSize(Const)VectorBase::Get(Const)Subsequence now deprecated.
//  Replaced by Get(Const)Subvector with equal stride to the parent.
//  *: Added constructors for [Fixed|Dynamic][Vector|Matrix]Ref from the other
//  storage type.
//
//  Revision 1.10  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.9  2004/10/22 18:07:06  anton
//  Doxygen: Made sure that all the classes are documented for Doxygen.
//
//  Revision 1.8  2004/10/14 19:09:13  ofri
//  vctDynamicConstMatrixBase: Following ticket #72 added operator() (row, col)
//
//  Revision 1.7  2004/10/07 21:12:12  ofri
//  Added methods vctDynamicMatrixBase::Assign(_elementType *),
//  vctDynamicMatrixBase::TransposeRef() [const and non-const]
//
//  Revision 1.6  2004/09/24 20:20:42  anton
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
//  Revision 1.5  2004/09/21 20:01:36  anton
//  cisstVector documentation: Added Doxygen doc for ProductOf(matrix, matrix)
//
//  Revision 1.4  2004/09/08 21:22:17  anton
//  vctDynamicMatrix: Added method ProductOf() as well as operator * for two
//  matrices.  Needed to implement Column() and Row() methods as well.
//  Corrected bugs in ToStream method.
//
//  Revision 1.3  2004/08/16 19:22:53  anton
//  cisstVector: Added bound checking for matrices as well as method "at"
//  with two parameters (row and col indices).  See also ticket #13 and CVS
//  commit [699]
//
//  Revision 1.2  2004/08/13 17:47:40  anton
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
//  Revision 1.1  2004/08/04 21:11:10  anton
//  cisstVector: Added preliminary version of dynamic matrices.  Lots of work
//  is still required, this code is not ready to be used.
//
//
// ****************************************************************************
