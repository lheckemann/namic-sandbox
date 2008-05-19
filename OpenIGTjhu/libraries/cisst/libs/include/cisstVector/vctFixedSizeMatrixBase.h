/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeMatrixBase.h,v 1.53 2006/05/21 11:27:06 ofri Exp $

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
  \brief Declaration of vctFixedSizeMatrixBase
 */

#ifndef _vctFixedSizeMatrixBase_h
#define _vctFixedSizeMatrixBase_h

#include <cstdarg>
#include <cisstVector/vctFixedSizeConstMatrixBase.h>
#include <cisstCommon/cmnTypeTraits.h>

/*! 
  \brief A template for a fixed size matrix with fixed spacings in
  memory.

  This class defines a matrix with read/write operations.  It
  extends vctFixedSizeConstMatrixBase with non-const methods.  See
  the base class for more documentation.
  
  \sa vctFixedSizeConstMatrixBase
*/
template<unsigned int _rows, unsigned int _cols, int _rowStride, 
         int _colStride, class _elementType, class _dataPtrType>
class vctFixedSizeMatrixBase : public vctFixedSizeConstMatrixBase
<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType>
{
 public:
    /* Declare the container-defined typed required by STL, plus the
       types completed by our traits class */
    
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /* documented in base class */
    typedef vctFixedSizeMatrixBase<_rows, _cols, 
        _rowStride, _colStride, _elementType, _dataPtrType> ThisType;
    
    /*! Type of the base class. */
    typedef vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, 
        _elementType, _dataPtrType> BaseType;
    
    typedef vctFixedSizeMatrixTraits<_elementType, _rows, _cols, 
        _rowStride, _colStride> MatrixTraits;

    typedef typename MatrixTraits::iterator iterator;
    typedef typename MatrixTraits::const_iterator const_iterator;
    typedef typename MatrixTraits::reverse_iterator reverse_iterator;
    typedef typename MatrixTraits::const_reverse_iterator const_reverse_iterator;
    
    typedef typename BaseType::RowRefType RowRefType;
    typedef typename BaseType::ColumnRefType ColumnRefType;
    typedef typename BaseType::ConstRowRefType ConstRowRefType;
    typedef typename BaseType::ConstColumnRefType ConstColumnRefType;
    typedef typename BaseType::DiagonalRefType DiagonalRefType;
    typedef typename BaseType::ConstDiagonalRefType ConstDiagonalRefType;
    typedef typename BaseType::RefTransposeType RefTransposeType;
    typedef typename BaseType::ConstRefTransposeType ConstRefTransposeType;
    typedef typename BaseType::RowValueType RowValueType;
    typedef typename BaseType::ColumnValueType ColumnValueType;
    
    /*! Define the dimensions of the matrix */
    enum {ROWS = MatrixTraits::ROWS, COLS = MatrixTraits::COLS,
          LENGTH = MatrixTraits::LENGTH};
    /*! Define the strides between rows and columns of the matrix */
    enum {ROWSTRIDE = MatrixTraits::ROWSTRIDE, 
          COLSTRIDE = MatrixTraits::COLSTRIDE};
    
 public:
    /*! Returns an iterator on the first element (STL compatibility). */
    inline iterator begin() {
        return iterator(this->Data, 0);
    }
    
    /* documented in base class */
    inline const_iterator begin() const {
        return const_iterator(this->Data, 0);
    }
    
    /*! Returns an iterator on the last element (STL compatibility). */
    inline iterator end() {
        return iterator(this->Data) + LENGTH;
    }
    
    /* documented in base class */
    inline const_iterator end() const {
        return const_iterator(this->Data) + LENGTH;
    }
    
    /*! Returns a reverse iterator on the last element (STL compatibility). */ 
    reverse_iterator rbegin() {
        return reverse_iterator(Pointer(ROWS - 1, COLS - 1), 0);
    }
    
    /* documented in base class */
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(Pointer(ROWS - 1, COLS - 1), 0);
    }

    /*! Returns a reverse iterator on the element before first 
      (STL compatibility). */ 
    reverse_iterator rend() {
        return reverse_iterator(this->Data - ROWSTRIDE + 
                                COLSTRIDE * (COLS - 1), 0);
    }

    /* documented in base class */
    const_reverse_iterator rend() const {
        return const_reverse_iterator(this->Data - ROWSTRIDE + 
                                      COLSTRIDE * (COLS - 1), 0);
    }


    /*! Reference a row of this matrix by index (non-const).
      \return a const reference to the element[index] */
    RowRefType operator[](size_type index) {
        return RowRefType(this->Data + ROWSTRIDE * index);
    }

    /*! Reference a row of this matrix by index (const).
      \return a const reference to the element[index] */
    ConstRowRefType operator[](size_type index) const {
        return ConstRowRefType(this->Data + ROWSTRIDE * index);
    }


    /*! Returns a non const pointer to an element of the container,
      specified by its index. Addition to the STL requirements.
    */
    pointer Pointer(size_type rowIndex, size_type colIndex) {
        return this->Data + ROWSTRIDE * rowIndex + COLSTRIDE * colIndex;
    }

    /*! Returns a non const pointer to the first element of the
      container. Addition to the STL requirements.
    */
    pointer Pointer(void) {
        return this->Data;
    }

    /* documented in base class */ 
    const_pointer Pointer(size_type rowIndex, size_type colIndex) const {
        return BaseType::Pointer(rowIndex, colIndex);
    }

    /* documented in base class */
    const_pointer Pointer(void) const {
        return this->Data;
    }


    /*! Access an element by index (non-const).  Compare with std::vector::at().
      This method can be a handy substitute for the overloaded operator () when
      operator overloading is unavailable or inconvenient.
      \return a reference to the index-th element (iterator order) */
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

#ifndef SWIG
    /*! Access an element by index.  See method at().
      \return a const reference to element[rowIndex, colIndex] */
    reference operator()(size_type rowIndex, size_type colIndex) throw(std::out_of_range) {
        return at(rowIndex, colIndex);
    }

    /* documented in base class */
    const_reference operator()(size_type rowIndex, size_type colIndex) const throw(std::out_of_range) {
        return at(rowIndex, colIndex);
    }
#endif


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


    /*! \name Row, column, and main diagonal references. */
    //@{
    /*! Create a row reference. */  
    RowRefType Row(size_type index) {
        return RowRefType(this->Data + ROWSTRIDE * index);
    }

    /*! Create a column reference. */  
    ColumnRefType Column(size_type index) {
        return ColumnRefType(this->Data + COLSTRIDE * index);
    }

    /* Create a reference to the main diagonal */
    DiagonalRefType Diagonal() {
        return DiagonalRefType(this->Data);
    }

    /*! Create a const row reference. */  
    ConstRowRefType Row(size_type index) const {
        return ConstRowRefType(this->Data + ROWSTRIDE * index);
    }

    /*! Create a const column reference. */  
    ConstColumnRefType Column(size_type index) const {
        return ConstColumnRefType(this->Data + COLSTRIDE * index);
    }

    /* Create a const reference to the main diagonal */
    ConstDiagonalRefType Diagonal() const {
        return ConstDiagonalRefType(this->Data);
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

      \note The current implementation does not validate that the input permuted indexes
      is an actual permutation of the numbers 0..(ROWS-1).  Nor does it assure that the input
      permutation array has the right size.  Both are the caller's responsibility.

      \note Do not use this method for an in-place permutation of the input matrix.
    */
    template<int __rowStride, int __colStride, class __dataPtrType>
    void RowPermutationOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
        _elementType, __dataPtrType> & inputMatrix, const unsigned int permutedRowIndexes[])
    {
        unsigned int thisRowIndex;
        for (thisRowIndex = 0; thisRowIndex < ROWS; ++thisRowIndex) {
            Row(thisRowIndex).Assign( inputMatrix.Row(permutedRowIndexes[thisRowIndex]) );
        }
    }

    /*! Assign a permutation of the rows of the input matrix to the rows of this matrix.
      Both matrices must have the same size.
      \param inputMatrix the input matrix for the permutation.
      \param permutedRowIndexes an array of row indices.  The assignment performed is:
      this->Row( permutedRowIndexes[i] ) <-- inputMatrix.Row(i).

      \note The current implementation does not validate that the input permuted indexes
      is an actual permutation of the numbers 0..(ROWS-1).  Nor does it assure that the input
      permutation array has the right size.  Both are the caller's responsibility.

      \note Do not use this method for an in-place permutation of the input matrix.
    */
    template<int __rowStride, int __colStride, class __dataPtrType>
    void RowInversePermutationOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
        _elementType, __dataPtrType> & inputMatrix, const unsigned int permutedRowIndexes[])
    {
        unsigned int thisRowIndex;
        for (thisRowIndex = 0; thisRowIndex < ROWS; ++thisRowIndex) {
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
    template<int __rowStride, int __colStride, class __dataPtrType>
    void ColumnPermutationOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
        _elementType, __dataPtrType> & inputMatrix, const unsigned int permutedColumnIndexes[])
    {
        unsigned int thisColumnIndex;
        for (thisColumnIndex = 0; thisColumnIndex < COLS; ++thisColumnIndex) {
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
    template<int __rowStride, int __colStride, class __dataPtrType>
    void ColumnInversePermutationOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride,
        _elementType, __dataPtrType> & inputMatrix, const unsigned int permutedColumnIndexes[])
    {
        unsigned int thisColumnIndex;
        for (thisColumnIndex = 0; thisColumnIndex < COLS; ++thisColumnIndex) {
            Column(permutedColumnIndexes[thisColumnIndex]).Assign( inputMatrix.Column(thisColumnIndex) );
        }
    }
    //@}


    /*! Assign the given value to all the elements.
      \param value the value used to set all the elements of the matrix
      \return The value used to set all the elements
    */
    inline value_type SetAll(const value_type value) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::SecondOperand>::
            Run(*this, value);
        return value;
    }


    /*! Assignment operation between matrices of different types
    */
    template<int __rowStride, int __colStride, class __elementType, class __dataPtrType>
    inline ThisType & Assign(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, __elementType, __dataPtrType> & other) {
        vctFixedSizeMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type, __elementType>::Identity>::
            Run(*this, other);
        return *this;  
    }


    /*! Assign to this matrix a set of values provided as independent
      arguments, by using cstdarg macros, that is, an unspecified
      number of arguments.  This function is not using a recursive
      engine, as it may be hard and not worthwhile to use a recursive
      engine with the va_arg ``iterator''.  This operation assumes
      that all the arguments are of type value_type, and that their
      number is equal to the size of the matrix.  The arguments are
      passed by \em value.  The user may need to explicitly cast
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
        if (inputIsRowMajor)
        {
            // row-major input means that the row-stride is equal to the number of elements
            // in a row, i.e, COLS, and the column-stride is 1.
            const vctFixedSizeConstMatrixRef<value_type, ROWS, COLS, COLS, 1> tmpRef(elements);
            this->Assign(tmpRef);
        }
        else
        {
            // column-major input means that the column-stride is equal to the number of
            // elements in a column, i.e., ROWS, and the row-stride is 1.
            const vctFixedSizeConstMatrixRef<value_type, ROWS, COLS, 1, ROWS> tmpRef(elements);
            this->Assign(tmpRef);
        }
        return *this;
    }



    /*! Return a transposed reference to this matrix.  The actual
      definition of this method follows the declaration of
      class vctFixedSizeMatrixRef.
    */
    RefTransposeType TransposeRef(void);

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

      \param input1Matrix The first operand of the binary operation

      \param input2Matrix The second operand of the binary operation
      
      \return The vector "this" modified.
    */
    template<int __input1RowStride, int __input1ColStride, class __input1DataPtrType,
             int __input2RowStride, int __input2ColStride, class __input2DataPtrType>
    const ThisType & SumOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __input1RowStride, __input1ColStride,
                           value_type, __input1DataPtrType> & input1Matrix,
                           const vctFixedSizeConstMatrixBase<_rows, _cols, __input2RowStride, __input2ColStride,
                           value_type, __input2DataPtrType> & input2Matrix) {
        vctFixedSizeMatrixLoopEngines::MoMiMi<typename vctBinaryOperations<value_type>::Addition>
            ::Run(*this, input1Matrix, input2Matrix);
        return *this;
    }
    
    /* documented above */
    template<int __input1RowStride, int __input1ColStride, class __input1DataPtrType,
             int __input2RowStride, int __input2ColStride, class __input2DataPtrType>
    const ThisType & DifferenceOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __input1RowStride, __input1ColStride,
                                  value_type, __input1DataPtrType> & input1Matrix,
                                  const vctFixedSizeConstMatrixBase<_rows, _cols, __input2RowStride, __input2ColStride,
                                  value_type, __input2DataPtrType> & input2Matrix) {
        vctFixedSizeMatrixLoopEngines::MoMiMi<typename vctBinaryOperations<value_type>::Subtraction>
            ::Run(*this, input1Matrix, input2Matrix);
        return *this;
    }

    /* documented above */
    template<int __input1RowStride, int __input1ColStride, class __input1DataPtrType,
             int __input2RowStride, int __input2ColStride, class __input2DataPtrType>
    const ThisType & ElementwiseProductOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __input1RowStride, __input1ColStride,
                                          value_type, __input1DataPtrType> & input1Matrix,
                                          const vctFixedSizeConstMatrixBase<_rows, _cols, __input2RowStride, __input2ColStride,
                                          value_type, __input2DataPtrType> & input2Matrix) {
        vctFixedSizeMatrixLoopEngines::MoMiMi<typename vctBinaryOperations<value_type>::Multiplication>
            ::Run(*this, input1Matrix, input2Matrix);
        return *this;
    }    
    
    /* documented above */
    template<int __input1RowStride, int __input1ColStride, class __input1DataPtrType,
             int __input2RowStride, int __input2ColStride, class __input2DataPtrType>
    const ThisType & ElementwiseRatioOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __input1RowStride, __input1ColStride,
                                        value_type, __input1DataPtrType> & input1Matrix,
                                        const vctFixedSizeConstMatrixBase<_rows, _cols, __input2RowStride, __input2ColStride,
                                        value_type, __input2DataPtrType> & input2Matrix) {
        vctFixedSizeMatrixLoopEngines::MoMiMi<typename vctBinaryOperations<value_type>::Division>
            ::Run(*this, input1Matrix, input2Matrix);
        return *this;
    }

    /* documented above */
    template<int __input1RowStride, int __input1ColStride, class __input1DataPtrType,
             int __input2RowStride, int __input2ColStride, class __input2DataPtrType>
    const ThisType & ElementwiseMinOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __input1RowStride, __input1ColStride,
                                      value_type, __input1DataPtrType> & input1Matrix,
                                      const vctFixedSizeConstMatrixBase<_rows, _cols, __input2RowStride, __input2ColStride,
                                      value_type, __input2DataPtrType> & input2Matrix) {
        vctFixedSizeMatrixLoopEngines::MoMiMi<typename vctBinaryOperations<value_type>::Minimum>
            ::Run(*this, input1Matrix, input2Matrix);
        return *this;
    }

    /* documented above */
    template<int __input1RowStride, int __input1ColStride, class __input1DataPtrType,
             int __input2RowStride, int __input2ColStride, class __input2DataPtrType>
    const ThisType & ElementwiseMaxOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __input1RowStride, __input1ColStride,
                                      value_type, __input1DataPtrType> & input1Matrix,
                                      const vctFixedSizeConstMatrixBase<_rows, _cols, __input2RowStride, __input2ColStride,
                                      value_type, __input2DataPtrType> & input2Matrix) {
        vctFixedSizeMatrixLoopEngines::MoMiMi<typename vctBinaryOperations<value_type>::Maximum>
            ::Run(*this, input1Matrix, input2Matrix);
        return *this;
    }
    //@}


    
    /*! \name Binary elementwise operations between two matrices.
      Store the result of op(this, otherMatrix) back to this matrix. */
    //@{
    /*! Store back binary elementwise operations between two matrices.
      For each element of the matrices, performs \f$ this[i]
      \leftarrow op(this[i], otherMatrix[i])\f$ where \f$op\f$ is
      either an addition (Add), a subtraction (Subtraction), a
      multiplication (ElementwiseMultiply) or a division
      (ElementwiseDivide), a minimization (ElementwiseMin) or a
      maximisation (ElementwiseMax).
      
      \param otherMatrix The second operand of the binary operation
      (this[i] is the first operand)
      
      \return The vector "this" modified.
    */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline ThisType & Add(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Addition>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline ThisType & Subtract(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Subtraction>::
            Run(*this, otherMatrix);
        return *this;
    }
    
    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline ThisType & ElementwiseMultiply(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Multiplication>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline ThisType & ElementwiseDivide(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Division>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline ThisType & ElementwiseMin(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Minimum>::
            Run(*this, otherMatrix);
        return *this;
    }
    
    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline ThisType & ElementwiseMax(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MioMi<typename vctStoreBackBinaryOperations<value_type>::Maximum>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & operator += (const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        return this->Add(otherMatrix);
    }
    
    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & operator -= (const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        return this->Subtract(otherMatrix);
    }
    //@}
    

    
    /*! \name Binary elementwise operations a matrix and a scalar.
      Store the result of op(matrix, scalar) to a third matrix. */
    //@{
    /*! Binary elementwise operations between a matrix and a scalar.
      For each element of the matrix "this", performs \f$ this[i]
      \leftarrow op(matrix[i], scalar)\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ProductOf), a division (RatioOf), a minimum (MinOf) or a
      maximum (MaxOf).

      \param matrix The first operand of the binary operation.
      \param scalar The second operand of the binary operation.
      
      \return The matrix "this" modified.
    */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & SumOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix, 
                                  const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Addition>::
            Run(*this, matrix, scalar);
        return *this;
    }
    
    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & DifferenceOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix, 
                                         const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Subtraction>::
            Run(*this, matrix, scalar);
        return *this;
    }
    
    /* documented above */    
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & ProductOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix, 
                                      const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Multiplication>::
            Run(*this, matrix, scalar);
        return *this;
    }
    
    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & RatioOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix, 
                                    const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Division>::
            Run(*this, matrix, scalar);
        return *this;
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & MinOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix, 
                                  const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Minimum>::
            Run(*this, matrix, scalar);
        return *this;
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & MaxOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix, 
                                  const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MoMiSi<typename vctBinaryOperations<value_type>::Maximum>::
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
      (ProductOf), a division (RatioOf), a minimum (MinOf) or a
      maximum (MaxOf).

      \param scalar The first operand of the binary operation.
      \param matrix The second operand of the binary operation.
      
      \return The matrix "this" modified.
    */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & SumOf(const value_type scalar,
                                  const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        vctFixedSizeMatrixLoopEngines::
            MoSiMi<typename vctBinaryOperations<value_type>::Addition>::
            Run(*this, scalar, matrix);
        return *this;
    }
    
    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & DifferenceOf(const value_type scalar,
                                         const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        vctFixedSizeMatrixLoopEngines::
            MoSiMi<typename vctBinaryOperations<value_type>::Subtraction>::
            Run(*this, scalar, matrix);
        return *this;
    }
    
    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & ProductOf(const value_type scalar,
                                      const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        vctFixedSizeMatrixLoopEngines::
            MoSiMi<typename vctBinaryOperations<value_type>::Multiplication>::
            Run(*this, scalar, matrix);
        return *this;
    }
    
    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & RatioOf(const value_type scalar,
                                    const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        vctFixedSizeMatrixLoopEngines::
            MoSiMi<typename vctBinaryOperations<value_type>::Division>::
            Run(*this, scalar, matrix);
        return *this;
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & MinOf(const value_type scalar,
                                  const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        vctFixedSizeMatrixLoopEngines::
            MoSiMi<typename vctBinaryOperations<value_type>::Minimum>::
            Run(*this, scalar, matrix);
        return *this;
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & MaxOf(const value_type scalar,
                                  const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        vctFixedSizeMatrixLoopEngines::
            MoSiMi<typename vctBinaryOperations<value_type>::Maximum>::
            Run(*this, scalar, matrix);
        return *this;
    }
    //@}

    /*! \name Binary elementwise operations between a matrix and a scalar.
      Store the result of op(this, scalar) back to this matrix. */
    //@{
    /*! Store back binary elementwise operations between a matrix and
      a scalar.  For each element of the matrix "this", performs \f$
      this[i] \leftarrow op(this[i], scalar)\f$ where \f$op\f$ is
      either an addition (Add), a subtraction (Subtract), a
      multiplication (Multiply), a division (Divide), a minimum (Min)
      or a maximum (Max).

      \param scalar The second operand of the binary operation
      (this[i] is the first operand.
      
      \return The matrix "this" modified.
    */
    inline ThisType & Add(const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::Addition>::
            Run(*this, scalar);
      return *this;
    }

    /* documented above */
    inline ThisType & Subtract(const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::Subtraction>::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Multiply(const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::Multiplication>::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Divide(const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::Division>::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Min(const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::Minimum>::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Max(const value_type scalar) {
        vctFixedSizeMatrixLoopEngines::
            MioSi<typename vctStoreBackBinaryOperations<value_type>::Maximum>::
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


    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & MultAdd(const value_type scalar,
    const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix)
    {
        vctFixedSizeMatrixLoopEngines::
            MioSiMi<
            typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar,  otherMatrix);
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
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & AbsOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & NegationOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::Negation>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & FloorOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::Floor>::
            Run(*this, otherMatrix);
        return *this;
    }

    /* documented above */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & CeilOf(const vctFixedSizeConstMatrixBase<_rows, _cols, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        vctFixedSizeMatrixLoopEngines::
            MoMi<typename vctUnaryOperations<value_type>::Ceil>::
            Run(*this, otherMatrix);
        return *this;
    }
    //@}



    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType & TransposeOf(const vctFixedSizeConstMatrixBase<_cols, _rows, __rowStride, __colStride, value_type, __dataPtrType> & otherMatrix) {
        Assign(otherMatrix.TransposeRef());
        return *this;
    }



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
        vctFixedSizeMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeAbs>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline const ThisType & NegationSelf(void) {
        vctFixedSizeMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeNegation>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline const ThisType & FloorSelf(void) {
        vctFixedSizeMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeFloor>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline const ThisType & CeilSelf(void) {
        vctFixedSizeMatrixLoopEngines::
            Mio<typename vctStoreBackUnaryOperations<value_type>::MakeCeil>::
            Run(*this);
        return *this;
    }
    //@}


    /*! Product of two matrices.  The template parameters insure that
      the size of the matrices match.
      
    \param input1Matrix The left operand of the binary operation.

    \param input2Matrix The right operand of the binary operation.
    
    \return The matrix "this" modified. */
    template<unsigned int __input1Cols, int __input1RowStride, int __input1ColStride, class __input1DataPtrType, 
        int __input2RowStride, int __input2ColStride, class __input2DataPtrType>
        void ProductOf(const vctFixedSizeConstMatrixBase<_rows, __input1Cols, __input1RowStride, __input1ColStride,
                       _elementType, __input1DataPtrType> & input1Matrix,
                       const vctFixedSizeConstMatrixBase<__input1Cols, _cols, __input2RowStride, __input2ColStride,
                       _elementType, __input2DataPtrType> & input2Matrix) {
        typedef vctFixedSizeConstMatrixBase<_rows, __input1Cols, __input1RowStride, __input1ColStride,
            _elementType, __input1DataPtrType> Input1MatrixType;
        typedef vctFixedSizeConstMatrixBase<__input1Cols, _cols, __input2RowStride, __input2ColStride,
            _elementType, __input2DataPtrType> Input2MatrixType;
        typedef typename Input1MatrixType::ConstRowRefType Input1RowRefType;
        typedef typename Input2MatrixType::ConstColumnRefType Input2ColumnRefType;
        vctFixedSizeMatrixLoopEngines::
            Product<typename vctBinaryOperations<value_type, Input1RowRefType, Input2ColumnRefType>::DotProduct>::
            Run((*this), input1Matrix, input2Matrix);   
    }

    /*! Compute the outer product of two vectors and store the result to
      this matrix.  The outer product (v1*v2)[i,j] = v1[i] * v2[j] */
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
        void OuterProductOf(const vctFixedSizeConstVectorBase<_rows, __stride1, _elementType, __dataPtrType1> & columnVector,
        const vctFixedSizeConstVectorBase<_cols, __stride2, _elementType, __dataPtrType2> & rowVector)
    {
        typedef vctFixedSizeConstVectorBase<_rows, __stride1, _elementType, __dataPtrType1> InputColumnType;
        typedef vctFixedSizeConstVectorBase<_cols, __stride2, _elementType, __dataPtrType2> InputRowType;
        typedef typename InputColumnType::ColConstMatrixRefType ColMatrixType;
        typedef typename InputRowType::RowConstMatrixRefType RowMatrixType;
        const ColMatrixType colMatrix = columnVector.AsColMatrix();
        const RowMatrixType rowMatrix = rowVector.AsRowMatrix();
        this->ProductOf(colMatrix, rowMatrix);
    }


    /*! Easy definition of a submatrix type

      This class declares a non-const submatrix type.
      To declare a submatrix object, here's an example.

      typedef vctFixedSizeMatrix<double, 4, 4> double4x4;
      double4x4 m;
      double4x4::ConstSubmatrix<3, 3>::Type subMatrix( m, 0, 0 );

      The submatrix strides with respect to the parent container are always 1.  The is, the
      memory strides between the elements of the parent matrix and submatrix are equal.
      For more sophisticated submatrices, the user has to write customized code.
    */
    template<unsigned int _subRows, unsigned int _subCols>
    class Submatrix
    {
    public:
        typedef vctFixedSizeMatrixRef<value_type, _subRows, _subCols, ROWSTRIDE, COLSTRIDE>
            Type;
    };
        
};


#endif // _vctFixedSizeMatrixBase_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedSizeMatrixBase.h,v $
// Revision 1.53  2006/05/21 11:27:06  ofri
// vct[FixedSize|Dynamic] vectors and matrices: implmented a new operation
// MultAdd, of the form CioSiCi.
//
// Revision 1.52  2006/01/03 03:38:58  anton
// cisstVector Doxygen: Used \code \endcode instead of <pre> </pre>, added
// some missing <ul> </ul>, added links to VCT_NORMALIZE.
//
// Revision 1.51  2005/12/13 00:04:57  ofri
// All cisstVector containers: Added methods
// 1) MinAndMaxElement  (at const base)
// 2) Floor, Ceil  (at const base -- implemented at container)
// 3) FloorSelf, CeilSelf, FloorOf, CeilOf  (at non-const base)
//
// Revision 1.50  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.49  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.48  2005/09/01 06:03:14  anton
// vctDynamicMatrixBase: Added method TransposeOf() and updated documentation
// for vctFixedSizeMatrixBase::TransposeOf().
//
// Revision 1.47  2005/08/04 15:31:57  anton
// cisstVector: Replaced RangeCheck() by ThrowUnlessValidIndex().  For dynamic
// matrices, Row() and Col() now check the index and throw using
// ThrowUnlessValid{Row,Col}Index().
//
// Revision 1.46  2005/07/22 19:10:02  ofri
// vct[FixedSize|Dynamic]MatrixBase: Changed the documentation and semantics
// of the Assign(const value_type *) following ticket #154.  The interface now
// requires packed arrays.
//
// Revision 1.45  2005/07/19 15:29:24  anton
// vctFixedSizeMatrixBase: Added method Assign(const value_type * elements).
//
// Revision 1.44  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.43  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.42  2004/12/17 15:21:56  ofri
// Fixing syntaxt error in vct[FixedSize|Dynamic]MatrixBase::ExchangeColumns
// (see ticket #99)
//
// Revision 1.41  2004/12/07 04:12:26  ofri
// Following ticket #99:
// 1) Added engine VioVio in vctFixedSizeVectorRecursiveEngines and in
// vctDynamicVectorLoopEngines
// 2) Added operation Swap in vctStoreBackBinaryOperations
// 3) Added operation SwapElementsWith in vctFixedSizeVectorBase and
// vctDynamicVectorBase
// 4) Rewrote the methods EchangeRows and ExchangeColumns in
// vctFixedSizeMatrixBase; added them to vctDynamicMatrixBase.
// 5) Same for row/column permutation methods.
// I am committing the files after compiling the library successfully and running
// the tests successfully on Cygwin.
//
// Revision 1.40  2004/12/01 16:42:19  anton
// cisstVector: Modified the signature of Pointer methods for matrices.  We now
// have two signature, Pointer(void) and Pointer(row, col) with no default value
// to avoid the ambiguous Pointer(index).  See ticket #93.
//
// Revision 1.39  2004/11/30 23:36:05  anton
// cisstVector: Added the Element() method to access an element without range
// check.  See ticket #79.
//
// Revision 1.38  2004/11/29 17:31:28  anton
// cisstVector: Major corrections for matrices reverse iterators.  Correction
// of the - operator which now takes into account the current column position.
//
// Revision 1.37  2004/11/18 20:57:30  ofri
// RE: ticket #92.  Redefined all operator= I could find to return a ThisType &
// instead of const ThisType &.  This included corresponding revision of the
// Assign methods, and also definition from scratch of operator= for fixed and
// dynamic matrix ref.
//
// Revision 1.36  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
// Revision 1.35  2004/11/03 22:26:12  anton
// cisstVector:  Better support of storage order.  Added VCT_COL_MAJOR,
// VCT_ROW_MAJOR and VCT_DEFAULT_STORAGE as well as the methods IsColMajor(),
// IsRowMajor(), IsCompact() and IsFortran().
//
// Revision 1.34  2004/10/26 15:49:03  ofri
// vct[FixedSize|Dynamic](Const)MatrixBase classes: Added reference access to
// the main diagonal (ticket #76).  Fixed bug in the Row() and Column() methods
// of the dynamic matrix base classes : incorrect stride.
//
// Revision 1.33  2004/10/26 15:20:52  ofri
// Updating subsequence interfaces (ticket #76) and interfaces between fixed
// and dynamic vectors/matrices (ticket #72).
// *: vctFixedSize(Const)VectorBase::Get(Const)Subsequence now deprecated.
// Replaced by Get(Const)Subvector with equal stride to the parent.
// *: Added constructors for [Fixed|Dynamic][Vector|Matrix]Ref from the other
// storage type.
//
// Revision 1.32  2004/10/25 15:48:52  ofri
// Added method vctFixedSizeMatrixBase::OuterProductOf(vector, vector). See
// ticket #69.
//
// Revision 1.31  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.30  2004/10/22 18:07:06  anton
// Doxygen: Made sure that all the classes are documented for Doxygen.
//
// Revision 1.29  2004/10/14 19:18:01  anton
// cisstVector, fixed size containers: Added operator() based on at() method
// as required per ticket #72.
//
// Revision 1.28  2004/10/07 21:09:01  ofri
// vctFixedSizeMatrixBase: Logical reordering of typedefs
//
// Revision 1.27  2004/09/24 20:20:42  anton
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
// Revision 1.26  2004/09/21 20:01:36  anton
// cisstVector documentation: Added Doxygen doc for ProductOf(matrix, matrix)
//
// Revision 1.25  2004/08/17 13:58:31  ofri
// vctFixedSizeMatrixBase and ConstMatrixBase: Added templated inner classes for
// submatrix types (cf vctFixedSizeVectorBase).  Added documentation for methods
// ToString and ToStream.
//
// Revision 1.24  2004/08/16 19:22:53  anton
// cisstVector: Added bound checking for matrices as well as method "at"
// with two parameters (row and col indices).  See also ticket #13 and CVS
// commit [699]
//
// Revision 1.23  2004/08/13 17:47:40  anton
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
// Revision 1.22  2004/08/03 21:02:44  ofri
// class vctFixedSizeMatrixBase: added methods RowInversePermutationOf and
// ColumnInversePermutationOf as complementary to [Row/Column]PermutationOf
//
// Revision 1.21  2004/07/27 20:08:06  anton
// cisstVector: Added some Doxygen documentation.  Minor code changes to
// keep the dynamic vectors in sync with the fixed size ones.
//
// Revision 1.20  2004/07/19 19:22:42  ofri
// vctFixedSizeMatrixBase: Assed methods RowPermutationOf() and ColumnPermutationOf()
//
// Revision 1.19  2004/07/16 14:41:01  anton
// vctFixed vector and matrix:  Operators +=, -=, *= and /= now return a const
// reference (was non const) to avoid hugly syntax like v += v2 /= v3
//
// Revision 1.18  2004/07/15 17:57:29  anton
// cisstVector engines using input scalars rely on copies, not reference
// anymore since the scalar used could be modified by the operation itself
// (see ticket #51).  We decided to use a const copy even if this might be
// at a slight efficiency cost.  This modification has an impact on all engines
// (loop/recursive, sequence/matrix) and we decided to also modify the user API
// (e.g. sequence.RatioOf(const sequence &, const scalar)) so that the internal
// behavior can be deduced from the signature on the method.
//
// Revision 1.17  2004/07/13 20:39:42  ofri
// vctFixedSizeMatrixBase: Added methods ExchangeRows and ExchangeColumns
//
// Revision 1.16  2004/07/12 20:27:57  anton
// cisstVector: Removed engine SoMoSi used by vctFixedSizeMatrix::SetAll()
// which now relies on MioSi
//
// Revision 1.15  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.14  2004/04/01 18:16:32  anton
// Removed const on methods returning a reverse_iterator (not const_reverse_iterator)
// For const Row method, returned type is now ConstRowRefType
// ElementWise is now Elementwise (w is lower case)
//
// Revision 1.13  2004/03/19 15:28:41  ofri
// Added the method at(index) to access sequence elements without operator
// overloading
//
// Revision 1.12  2004/03/16 20:46:15  ofri
// Added const version of methods from base class which were shadowed:
// Pointer, operator (), Row, Column
//
// Revision 1.11  2004/02/11 15:26:40  anton
// Added TransposeRef const method since it was shadowed by the non const one
//
// Revision 1.10  2004/02/06 15:33:55  anton
// cmnVaTypes is now cmnTypeTraits
// All methods Abs, Negation now have a consistant name and signature
//
// Revision 1.9  2004/01/16 22:54:17  anton
// Added TransposeOf method
//
// Revision 1.8  2004/01/15 15:23:11  anton
// Added Assign(element0, ...) based on va_arg
//
// Revision 1.7  2003/12/19 21:48:24  anton
// Method MatrixMatrixProduct renamed to ProductOf and it uses vctMatrixLoopEngines::Product
//
// Revision 1.6  2003/12/18 16:19:39  anton
// Added most methods and operators to be consistant with vctFixedLengthSequenceBase
//
// Revision 1.5  2003/12/09 20:48:06  ofri
// Fixed type name bug in vctFixedSizeMatrixBase::Column(unsigned int)
//
// Revision 1.4  2003/11/27 03:45:18  ofri
// Added tentatively to vctFixedSizeMatrixBase:
// 1) Elementwise operation engine MoMiMi
// 2) Methods SumOf(matrix, matrix) and DifferenceOf(matrix, matrix) using
// the MoMiMi engine
//
// Revision 1.3  2003/11/20 18:24:04  anton
// Splitted const and non const classes in two files.
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
