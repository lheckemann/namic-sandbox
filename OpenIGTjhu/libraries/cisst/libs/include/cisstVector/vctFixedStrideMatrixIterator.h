/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedStrideMatrixIterator.h,v 1.13 2005/12/16 22:32:55 ofri Exp $

  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on:  2003-09-30

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
  \brief Declaration of vctFixedStrideMatrixConstIterator and vctFixedStrideMatrixIterator
*/


#ifndef _vctFixedStrideMatrixIterator_h
#define _vctFixedStrideMatrixIterator_h


#include <iterator>

#include <cisstCommon/cmnAssert.h>
#include <cisstVector/vctContainerTraits.h>


/*!
  \brief Matrix iterator.

  The matrix iterators are parametrized by the type of matrix element,
  the stride (pointer increment) in row direction and column
  direction, and the number of matrix columns spanned by the iterator.
  The iterator traverses the matrix elements in row-first order, with
  respect to its matrix, and typically possesses the matrix' strides.
  Note that the iterator does not use the number of rows, and does not
  keep track of the current row index in the matrix.  This was done in
  order to keep the iterator's memory size as compact as possible.  It
  does, however, keep track of the current column in the matrix, which
  is required in order to be able to wrap to the next row when the end
  of the column is reached.

  The matrix iterators conform to the STL specification for a Random
  Access Iterator.  This means that one can access all the elements of
  a matrix sequentially by subscripting or incrementing the iterator.
  However, in many cases it is faster to access the elements by
  row-column indices than by the iterator, since the iterator needs to
  do the bookkeeping of column indices.

  The matrix iterators are defined hierarchically, with the
  vctFixedStrideMatrixConstIterator being immutable, and
  vctFixedStrideMatrixIterator derived from it and mutable.

  \note The comparison operator == compares both the pointer and the
  column index maintained by the iterator.  This was the only way we
  could ensure that the end() method on the matrix classes
  (vctConstFixedSizeMatrixBase etc.)  will evaluate correctly and be
  comparable with an iterator that's incremented.  However, the
  ordering of iterators relies only on the values of the pointers.
  Recall that iterator ordering has reachability issues, and see the
  next note.

  \note There are reachability issues with our iterators system,
  though they conform with the STL specifications.  Generally, STL
  does not require reachability between any pair of iterators, except
  iterators that refer to the same container object.  In our case, the
  iterator may span a vector of memory locations in an arbitrary
  order, depending on the strides of both rows and columns.  In other
  words, if we trace the memory locations pointed by an iterator which
  is incremented, they may be incremented or decremented depending on
  the current column index and the strides. This means that we cannot
  rely on memory order to order the iterators, and furthermore, we
  cannot assume reachability in the general case.  Reachability is
  guaranteed only for iterators that were obtained from the same
  matrix object and have the same directionality (forward or
  reverse). The correctness of operations like ordering iterators
  depends on reachability.  Therefore, use iterator ordering with
  care, and do not try to mix iterators from different containers into
  the same expression.
*/
template<class _elementType, int _columnStride, unsigned int _numColumns, int _rowStride>
class vctFixedStrideMatrixConstIterator:
    public std::iterator<std::random_access_iterator_tag, _elementType>
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! The type of the iterator itself. */
    typedef vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> ThisType;

    /*! Base type for this iterator, i.e. std::iterator. */
    typedef std::iterator<std::random_access_iterator_tag, _elementType> BaseType;

    /*! Type (i.e. category) of iterator,
      i.e. std::random_access_iterator_tag. */
    typedef typename BaseType::iterator_category iterator_category;

    enum {COL_STRIDE = _columnStride, ROW_STRIDE = _rowStride};
    enum {NUM_COLUMNS = _numColumns};

protected:
    value_type * DataPtr;
    int CurrentColumn;

    enum {ROW_FIRST_INDEX = (ROW_STRIDE >= NUM_COLUMNS * COL_STRIDE)};

    /*! An auxiliary method to wrap the column index of the iterator.
      If the current column exceeds the number of columns in the matrix,
      more one row down and wrap to the corresponding column starting
      from the left
    */
    inline void WrapToRight() {
        if (CurrentColumn >= NUM_COLUMNS) {
            DataPtr += ROW_STRIDE - (COL_STRIDE * NUM_COLUMNS);
            CurrentColumn -= NUM_COLUMNS;
        }
    }


    /*! An auxiliary method to wrap the column index of the iterator.
      If the current column is negative, move one row up, and wrap to the
      corresponding column starting from the right
    */
    inline void WrapToLeft() {
        if (CurrentColumn < 0) {
            DataPtr -= ROW_STRIDE - (COL_STRIDE * NUM_COLUMNS);
            CurrentColumn += NUM_COLUMNS;
        }
    }


public:
    /*! Default constructor: create an uninitialized object */
    vctFixedStrideMatrixConstIterator() {
        CMN_ASSERT(ROW_STRIDE * COL_STRIDE > 0);
    }


    /*! Constructor taking a non-const element pointer.  Note that
      only read operations will be performed! The initial column
      position is assumed to be zero by default, but may be chosen
      otherwise (within the proper range).

      \param dataPtr data pointer
      \param initialColumn column index where the iterator is initialized
    */
    explicit vctFixedStrideMatrixConstIterator(value_type * dataPtr, 
        int initialColumn = 0)
        : DataPtr(dataPtr)
        , CurrentColumn(initialColumn) {
        CMN_ASSERT(ROW_STRIDE * COL_STRIDE > 0);
        CMN_ASSERT( initialColumn < NUM_COLUMNS );
    }


    /*! Constructor taking a const element pointer.  We need it in
      order to deal with const containers being refered by this
      iterator.  We have to perform const_cast to convert back to the
      stored non-const member pointer of this iterator.  However, the
      non-const iterator class completely shadows the constructors of
      the const-iterator base class, and so we don't have to worry
      about creating a non-const iterator over a const container.

      \param dataPtr const data pointer
      \param initialColumn column index where the iterator is initialized
    */
    explicit vctFixedStrideMatrixConstIterator(const value_type * dataPtr,
        int initialColumn = 0)
        : DataPtr(const_cast<value_type *>(dataPtr))
        , CurrentColumn(initialColumn) {
        CMN_ASSERT(ROW_STRIDE * COL_STRIDE > 0);
        CMN_ASSERT( initialColumn < NUM_COLUMNS );
    }


    /*! Pre-increment. */
    ThisType & operator++() {
        DataPtr += COL_STRIDE;
        ++CurrentColumn;
        WrapToRight();
        return *this;
    }


    /*! Post-increment. */
    ThisType operator++(int) {
        ThisType tmp(*this);
        ++(*this);
        return tmp;
    }


    /*! Pre-decrement. */
    ThisType & operator--() {
        DataPtr -= COL_STRIDE;
        --CurrentColumn;
        WrapToLeft();
        return *this;
    }


    /*! Post-decrement. */
    ThisType operator--(int) {
        ThisType tmp(*this);
        --(*this);
        return tmp;
    }


    /*! Increment by offset.
      \param difference offset to increment by
    */
    ThisType & operator +=(difference_type difference) {
        DataPtr += (NUM_COLUMNS == 0) ? 0
            : (difference / NUM_COLUMNS) * ROW_STRIDE + (difference % NUM_COLUMNS) * COL_STRIDE;
        CurrentColumn += (NUM_COLUMNS == 0) ? 0 : difference % NUM_COLUMNS;
        if (difference >= 0)
            WrapToRight();
        else
            WrapToLeft();
        return *this;
    }


    /*! Decrement by offset.
     \param difference offset to decrement by
    */
    ThisType & operator -=(difference_type difference) {
        DataPtr -= (NUM_COLUMNS == 0) ? 0
            : (difference / NUM_COLUMNS) * ROW_STRIDE + (difference % NUM_COLUMNS) * COL_STRIDE;
        CurrentColumn -= (NUM_COLUMNS == 0) ? 0 : difference % NUM_COLUMNS;
        if (difference >= 0)
            WrapToLeft();
        else
            WrapToRight();
        return *this;
    }


    /*! Subtraction between iterators returns the number of increments needed
      for the second operand to reach the first operand, if it is reachable.

      The number of increments is found by the following equations:
      DataPtr - (difference / NUM_COLUMNS) * ROW_STRIDE + (difference % NUM_COLUMNS) * COL_STRIDE == other.DataPtr
      DataPtr - other.DataPtr == (difference / NUM_COLUMNS) * ROW_STRIDE + (difference % NUM_COLUMNS) * COL_STRIDE 
      
      if (ROW_STRIDE >= NUM_COLUMNS * COL_STRIDE) {
      (DataPtr - other.DataPtr) / ROW_STRIDE == row_diff == (difference / NUM_COLUMNS)
      DataPtr - other.DataPtr - row_diff * ROW_STRIDE == (difference % NUM_COLUMNS) * COL_STRIDE
      (DataPtr - other.DataPtr - row_diff * ROW_STRIDE) / COL_STRIDE == col_diff == (difference % NUM_COLUMNS)
      difference == row_diff * NUM_COLUMNS + col_diff
      }
      otherwise switch the roles of rows and columns.

      \note this operation assumes reachability and does not test for it.
    */
    difference_type operator-(const ThisType & other) const {
        const value_type * beginThisRow = DataPtr - CurrentColumn * COL_STRIDE;
        const value_type * beginThatRow = other.DataPtr - other.CurrentColumn * COL_STRIDE;
        const difference_type rowDiff = (beginThisRow - beginThatRow) / ROW_STRIDE;
        const difference_type colDiff = CurrentColumn - other.CurrentColumn;
        const difference_type result = rowDiff * NUM_COLUMNS + colDiff;
        return result;
    }


    /*! Random access (return const reference). */
    const value_type & operator[](difference_type index) const {
        ThisType ptrCalc(*this);
        ptrCalc += index;
        return *ptrCalc;
    }


    /*! Dereference (const). */
    const value_type & operator* () const {
        return *DataPtr;
    }


    /*! Order relation between iterators, required by STL.

      \note The STL manual states that "if j is reachable from i then
      i<j".  This does not imply the converse: "if i<j then j is
      reachable from i".  In the case here, the converse does not
      hold.
    */
    bool operator< (const ThisType & other) const {
        return ((*this) - other) < 0;
    }


    /*! Equality of iterators, required by STL. */
    bool operator== (const ThisType & other) const {
        return (DataPtr == other.DataPtr) && (CurrentColumn == other.CurrentColumn);
    }


    /*! Complementary operation to operator <. */
    bool operator> (const ThisType & other) const {
        return other < *this;
    }


    /*! Complementary operation to operator ==. */
    bool operator != (const ThisType & other) const {
        return !( (*this) == other );
    }
};



/*! const_iterator + difference_type required by STL */
template<class _elementType, int _columnStride, unsigned int _numColumns, int _rowStride>
vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns,_rowStride>
operator+(const vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> & iterator,
          typename vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride>::difference_type difference)
{
    vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> result(iterator);
    return result += difference;
}

/*! difference_type + const_iterator required by STL */
template<class _elementType, int _columnStride, unsigned int _numColumns, int _rowStride>
vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride>
operator+(typename vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride>::difference_type difference,
          const vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> & iterator)
{
    vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> result(iterator);
    return result += difference;
}

/*! const_iterator - difference_type required by STL */
template<class _elementType, int _columnStride, unsigned int _numColumns, int _rowStride>
vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride>
operator-(const vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> & iterator,
          typename vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns,_rowStride>::difference_type difference)
{
    vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns,_rowStride> result(iterator);
    return result -= difference;
}


/*! \sa vctFixedStrideMatrixConstIterator */
template<class _elementType, int _columnStride, unsigned int _numColumns, int _rowStride>
class vctFixedStrideMatrixIterator:
    public vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride>
{
public:
    /* documented in base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride> ThisType;
    typedef vctFixedStrideMatrixConstIterator<_elementType, _columnStride, _numColumns, _rowStride> BaseType;
    typedef typename BaseType::iterator_category iterator_category;
    
    /*! Default constructor: create an uninitialized object */
    vctFixedStrideMatrixIterator()
        : BaseType() 
    {}


    /*! Constructor taking a non-const element pointer.  Read and
      write operations on the refered object are permitted.  Note that
      there is no constructor that takes a const element pointer.
    */
    explicit vctFixedStrideMatrixIterator(value_type * dataPtr,
        int initialColumn = 0)
        : BaseType(dataPtr, initialColumn) 
    {}


    /*! Redefine operator++ to return vctFixedStrideMatrixIterator instead of
    vctFixedStrideMatrixConstIterator */
    ThisType & operator++() {
        return reinterpret_cast<ThisType &>(BaseType::operator++());
    }


    /*! Redefine operator++ to return vctFixedStrideMatrixIterator instead of
    vctFixedStrideMatrixConstIterator */
    ThisType operator++(int) {
        ThisType tmp(*this);
        ++(*this);
        return tmp;
    }


    /*! Redefine operator-- to return vctFixedStrideMatrixIterator instead of
    vctFixedStrideMatrixConstIterator */
    ThisType & operator--() {
        return reinterpret_cast<ThisType &>(BaseType::operator--());
    }


    /*! Redefine operator-- to return vctFixedStrideMatrixIterator instead of
    vctFixedStrideMatrixConstIterator */
    ThisType operator--(int) {
        ThisType tmp(*this);
        --(*this);
        return tmp;
    }


    /*! Redefine operator+= to return vctArrayIterator instead of
    vctArrayConstIterator */
    ThisType & operator+=(difference_type difference) {
        return reinterpret_cast<ThisType &>(BaseType::operator+=(difference));
    }


    /*! Redefine operator-= to return vctArrayIterator instead of
    vctArrayConstIterator */
    ThisType & operator-=(difference_type difference) {
        return reinterpret_cast<ThisType &>(BaseType::operator-=(difference));
    }


    /*! Add non-const version of operator [] */
    value_type & operator[](difference_type index) const {
        ThisType ptrCalc(*this);
        ptrCalc += index;
        return *ptrCalc;
    }


    /*! Add non-const version of unary operator * */
    value_type & operator* () {
        return *(this->DataPtr);
    }

};


/*! iterator + difference_type required by STL */
template<class _elementType, int _columnStride, unsigned int _numColumns, int _rowStride>
vctFixedStrideMatrixIterator<_elementType, _columnStride,_numColumns, _rowStride>
operator+(const vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride> & iterator,
          typename vctFixedStrideMatrixIterator<_elementType,_columnStride, _numColumns, _rowStride>::difference_type difference)
{
    vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns,_rowStride> result(iterator);
    return result += difference;
}

/*! difference_type + iterator required by STL */
template<class _elementType, int _columnStride, unsigned int _numColumns, int _rowStride>
vctFixedStrideMatrixIterator< _elementType, _columnStride, _numColumns, _rowStride>
operator+(typename vctFixedStrideMatrixIterator< _elementType, _columnStride, _numColumns, _rowStride>::difference_type difference,
          const vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride> & iterator)
{
    vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride> result(iterator);
    return result += difference;
}

/*! iterator - difference_type required by STL */
template<class _elementType, int _columnStride, unsigned int _numColumns, int _rowStride>
vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride>
operator-(const vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride> & iterator,
          typename vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns, _rowStride>::difference_type difference)
{
    vctFixedStrideMatrixIterator<_elementType, _columnStride, _numColumns,_rowStride> result(iterator);
    return result -= difference;
}

#endif


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedStrideMatrixIterator.h,v $
// Revision 1.13  2005/12/16 22:32:55  ofri
// Matrix iterators (fixed and var stride): In operators += and -= added test for
// numColumns == 0 to avoid division by zero in zero-columns matrices.  Hopefully.
// the compiler will not generate an error when an iterator is defined for a
// fixed-size zero-column matrix.
//
// Revision 1.12  2005/09/27 18:01:25  anton
// cisstVector: Use CMN_ASSERT instead of assert for vectors and matrices.
//
// Revision 1.11  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.10  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.9  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.8  2004/11/30 20:11:34  anton
// vctFixedStrideMatrixIterator: Bug in matrix iterator -- which called the
// base type ++ incorrectly.  Discovered via new test cases.
//
// Revision 1.7  2004/11/29 17:31:28  anton
// cisstVector: Major corrections for matrices reverse iterators.  Correction
// of the - operator which now takes into account the current column position.
//
// Revision 1.6  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
// Revision 1.5  2004/10/22 18:07:06  anton
// Doxygen: Made sure that all the classes are documented for Doxygen.
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
// Revision 1.3  2004/08/05 21:10:36  ofri
// vctFixedStrideMatrixIterator: ctor argument initialColumn changed to signed int.
// vctVarStrideMatrixIterator bug fix: ctor now initializes CurrentColumn
//
// Revision 1.2  2004/07/27 20:08:06  anton
// cisstVector: Added some Doxygen documentation.  Minor code changes to
// keep the dynamic vectors in sync with the fixed size ones.
//
// Revision 1.1  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.5  2004/02/06 15:32:16  anton
// operator++ was returning an iterator by reference, now returns a copy
//
// Revision 1.4  2003/10/20 16:22:06  ofri
// vctFixedStrideMatrixIterator updates:
// 1) Added class-level documentation
// 2) Revised constructor to take unsigned int column index and to assert it is
// within range
// 3) Changed operator== to consider column index to facilitate matrix end()
// operation.
//
// Revision 1.3  2003/10/17 20:41:28  ofri
// vctFixedStrideMatrixIterator.h: Fixed syntax errors, updated variable names, fixed
// alignment, added class vctFixedStrideMatrixIterator (non-const) and global operators +
// and -.
// The file now goes through gcc compilation without errors.  Still needs to be
// tested thoroughly using the generic iterator tests.
//
// Revision 1.2  2003/10/02 14:27:37  anton
// Added doxygen documentation
//
// Revision 1.1  2003/09/30 14:02:49  anton
// creation
//
//
// ****************************************************************************
