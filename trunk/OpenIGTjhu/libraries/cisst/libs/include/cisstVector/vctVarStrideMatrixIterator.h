/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctVarStrideMatrixIterator.h,v 1.11 2005/12/20 17:25:26 anton Exp $
  
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on:  2004-07-02

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
  \brief Declaration of vctVarStrideMatrixConstIterator and vctVarStrideMatrixIterator
 */


#ifndef _vctVarStrideMatrixIterator_h
#define _vctVarStrideMatrixIterator_h

#include <iterator>
#include <cisstVector/vctContainerTraits.h>


/*!
  \param _elementType the type of the element that the iterator refers to.

  \sa vctVarStrideMatrixIterator
*/
template<class _elementType>
class vctVarStrideMatrixConstIterator:
    public std::iterator<std::random_access_iterator_tag, _elementType>
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    
    /*! The type of the iterator itself. */
    typedef vctVarStrideMatrixConstIterator<_elementType> ThisType;
    
    /*! Base type for this iterator, i.e. std::iterator. */
    typedef std::iterator<std::random_access_iterator_tag, _elementType> BaseType;

    /*! Type (i.e. category) of iterator,
      i.e. std::random_access_iterator_tag. */
    typedef typename BaseType::iterator_category iterator_category;

protected:
    /*! The data member points to the object being refered by this
      iterator.  It is not declared const, so we will be able to use
      it in the non-const iterator class.  But all the methods in
      vctVarStrideMatrixConstIterator are declared const to protect
      the data from writing.
    */
    value_type * DataPtr;

    /* Strides between the elements of a matrix. */
    difference_type ColumnStride;
    int NumColumns;
    difference_type RowStride;
    int CurrentColumn;

    /*! An auxiliary method to wrap the column index of the iterator.
      If the current column exceeds the number of columns in the matrix,
      more one row down and wrap to the corresponding column starting
      from the left
    */
    inline void WrapToRight() {
        if (CurrentColumn >= NumColumns) {
            DataPtr += RowStride - (ColumnStride * NumColumns);
            CurrentColumn -= NumColumns;
        }
    }


    /*! An auxiliary method to wrap the column index of the iterator.
      If the current column is negative, move one row up, and wrap to the
      corresponding column starting from the right
    */
    inline void WrapToLeft() {
        if (CurrentColumn < 0) {
            DataPtr -= RowStride - (ColumnStride * NumColumns);
            CurrentColumn += NumColumns;
        }
    }


public:
    /*! Default constructor: create an uninitialized object. */
    vctVarStrideMatrixConstIterator():
        DataPtr(0),
        ColumnStride(1),
        NumColumns(0),
        RowStride(1),
        CurrentColumn(0)
        {}


    /*! Constructor taking a non-const element pointer.  Note that
      only read operations will be performed! */
    vctVarStrideMatrixConstIterator(value_type * dataPtr, difference_type columnStride, 
        int numColumns, difference_type rowStride, int initialColumn = 0):
        DataPtr(dataPtr),
        ColumnStride(columnStride),
        NumColumns(numColumns),
        RowStride(rowStride),
        CurrentColumn(initialColumn)
        {}
    
    
    /*! Constructor taking a const element pointer.  We need it in
      order to deal with const containers being refered by this
      iterator.  We have to perform const_cast to convert back to the
      stored non-const member pointer of this iterator.  However, the
      non-const iterator class completely shadows the constructors of
      the const-iterator base class, and so we don't have to worry
      about creating a non-const iterator over a const container.
    */
    vctVarStrideMatrixConstIterator(const value_type * dataPtr, difference_type columnStride,
        int numColumns, difference_type rowStride, int initialColumn = 0):
        DataPtr(const_cast<value_type *>(dataPtr)),
        ColumnStride(columnStride),
        NumColumns(numColumns),
        RowStride(rowStride),
        CurrentColumn(initialColumn)
    {}


    /*! Pre-increment. */
    ThisType & operator++() {
        DataPtr += ColumnStride;
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
        DataPtr -= ColumnStride;
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
        DataPtr += (difference / NumColumns) * RowStride + (difference % NumColumns) * ColumnStride;
        CurrentColumn += difference % NumColumns;
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
        DataPtr -= (difference / NumColumns) * RowStride + (difference % NumColumns) * ColumnStride;
        CurrentColumn -= difference % NumColumns;
        if (difference >= 0)
            WrapToLeft();
        else
            WrapToRight();
        return *this;
    }


    /*! Subtraction between iterators returns the number of increments needed
      for the second operand to reach the first operand, if it is reachable.

      The number of increments is found by the following equations:
      DataPtr - (difference / NumColumns) * RowStride + (difference % NumColumns) * ColumnStride == other.DataPtr
      DataPtr - other.DataPtr == (difference / NumColumns) * RowStride + (difference % NumColumns) * ColumnStride 
      
      if (RowStride >= NumColumns * ColumnStride) {
      (DataPtr - other.DataPtr) / RowStride == row_diff == (difference / NumColumns)
      DataPtr - other.DataPtr - row_diff * RowStride == (difference % NumColumns) * ColumnStride
      (DataPtr - other.DataPtr - row_diff * RowStride) / ColumnStride == col_diff == (difference % NumColumns)
      difference == row_diff * NumColumns + col_diff
      }
      otherwise switch the roles of rows and columns.

      \note this operation assumes reachability and does not test for it.
    */
    difference_type operator-(const ThisType & other) const {
        const value_type * beginThisRow = DataPtr - CurrentColumn * ColumnStride;
        const value_type * beginThatRow = other.DataPtr - other.CurrentColumn * ColumnStride;
        const difference_type rowDiff = (beginThisRow - beginThatRow) / RowStride;
        const difference_type colDiff = CurrentColumn - other.CurrentColumn;
        const difference_type result = rowDiff * NumColumns + colDiff;
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




/*!  The non-const iterator with a fixed stride.
 \sa vctVarStrideMatrixConstIterator
*/
template<class _elementType>
class vctVarStrideMatrixIterator : public vctVarStrideMatrixConstIterator<_elementType>
{
public:
    /* documented in base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctVarStrideMatrixIterator<_elementType> ThisType;
    typedef vctVarStrideMatrixConstIterator<_elementType> BaseType;
    typedef typename BaseType::iterator_category iterator_category;

    /*! Default constructor: create an uninitialized object */
    vctVarStrideMatrixIterator():
        BaseType()
        {}

    /*! Constructor taking a non-const element pointer.  Read and
      write operations on the refered object are permitted.  Note that
      there is no constructor that takes a const element pointer.
    */
    vctVarStrideMatrixIterator(value_type * dataPtr, difference_type columnStride,
        int numColumns, difference_type rowStride, int initialColumn = 0):
        BaseType(dataPtr, columnStride, numColumns, rowStride, initialColumn)
        {}


    /*! Redefine operator++ to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator */
    ThisType & operator++() {
        this->DataPtr += this->ColumnStride;
        ++(this->CurrentColumn);
        this->WrapToRight();
        return *this;
    }


    /*! Redefine operator++ to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator */
    ThisType operator++(int) {
        ThisType tmp(*this);
        ++(*this);
        return tmp;
    }


    /*! Redefine operator-- to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator */
    ThisType & operator--() {
        this->DataPtr -= this->ColumnStride;
        --(this->CurrentColumn);
        this->WrapToLeft();
        return *this;
    }


    /*! Redefine operator-- to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator */
    ThisType operator--(int) {
        ThisType tmp(*this);
        --(*this);
        return tmp;
    }


    /*! Redefine operator+= to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator.
      \note If the number of columns of the matrix is zero then the iterator
      is returned unchanged.
    */
    ThisType & operator+=(difference_type difference) {
        if (this->NumColumns == 0)
            return *this;
        this->DataPtr += (difference / this->NumColumns) * this->RowStride + (difference % this->NumColumns) * this->ColumnStride;
        this->CurrentColumn += difference % this->NumColumns;
        if (difference >= 0)
            this->WrapToRight();
        else
            this->WrapToLeft();
        return *this;
    }


    /*! Redefine operator-= to return vctVarStrideMatrixIterator instead of
      vctVarStrideMatrixConstIterator
      \note If the number of columns of the matrix is zero then the iterator
      is returned unchanged.
    */
    ThisType & operator-=(difference_type difference) {
        if (this->NumColumns == 0)
            return *this;
        this->DataPtr -= (difference / this->NumColumns) * this->RowStride + (difference % this->NumColumns) * this->ColumnStride;
        this->CurrentColumn -= difference % this->NumColumns;
        if (difference >= 0)
            this->WrapToLeft();
        else
            this->WrapToRight();
        return *this;
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



/*! const_iterator + difference_type required by STL */
template<class _elementType>
vctVarStrideMatrixConstIterator<_elementType>
operator+(const vctVarStrideMatrixConstIterator<_elementType> & iterator,
          typename vctVarStrideMatrixConstIterator<_elementType>::difference_type difference)
{
    vctVarStrideMatrixConstIterator<_elementType> result(iterator);
    return result += difference;
}


/*! difference_type + const_iterator required by STL */
template<class _elementType>
vctVarStrideMatrixConstIterator<_elementType> 
operator+(typename vctVarStrideMatrixConstIterator<_elementType>::difference_type difference,
          const vctVarStrideMatrixConstIterator<_elementType> & iterator)
{
    vctVarStrideMatrixConstIterator<_elementType> result(iterator);
    return result += difference;
}


/*! const_iterator - difference_type required by STL */
template<class _elementType>
vctVarStrideMatrixConstIterator<_elementType> 
operator-(const vctVarStrideMatrixConstIterator<_elementType> & iterator,
          typename vctVarStrideMatrixConstIterator<_elementType>::difference_type difference)
{
    vctVarStrideMatrixConstIterator<_elementType> result(iterator);
    return result -= difference;
}


/*! iterator + difference_type required by STL.  Note that this method
  returns a non-const iterator, which is why it is declared separately
  from the other operator +.
*/
template<class _elementType>
vctVarStrideMatrixIterator<_elementType>
operator+(const vctVarStrideMatrixIterator<_elementType> & iterator,
          typename vctVarStrideMatrixIterator<_elementType>::difference_type difference)
{
    vctVarStrideMatrixIterator<_elementType> result(iterator);
    return result += difference;
}


/*! difference_type + iterator required by STL */
template<class _elementType>
vctVarStrideMatrixIterator<_elementType> 
operator+(typename vctVarStrideMatrixIterator<_elementType>::difference_type difference,
          const vctVarStrideMatrixIterator<_elementType> & iterator)
{
    vctVarStrideMatrixIterator<_elementType> result(iterator);
    return result += difference;
}


/*! iterator - difference_type required by STL */
template<class _elementType>
vctVarStrideMatrixIterator<_elementType> 
operator-(const vctVarStrideMatrixIterator<_elementType> & iterator,
          typename vctVarStrideMatrixIterator<_elementType>::difference_type difference)
{
    vctVarStrideMatrixIterator<_elementType> result(iterator);
    return result -= difference;
}


#endif  // _vctVarStrideMatrixIterator_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctVarStrideMatrixIterator.h,v $
// Revision 1.11  2005/12/20 17:25:26  anton
// vctVarStrideMatrixIterator: Added required this-> for gcc 4.0
//
// Revision 1.10  2005/12/16 22:32:55  ofri
// Matrix iterators (fixed and var stride): In operators += and -= added test for
// numColumns == 0 to avoid division by zero in zero-columns matrices.  Hopefully.
// the compiler will not generate an error when an iterator is defined for a
// fixed-size zero-column matrix.
//
// Revision 1.9  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.8  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.7  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.6  2004/11/29 17:31:28  anton
// cisstVector: Major corrections for matrices reverse iterators.  Correction
// of the - operator which now takes into account the current column position.
//
// Revision 1.5  2004/11/12 22:10:48  anton
// vctVarStrideMatrixIterator: Couple of typos which prevented compilation.
//
// Revision 1.4  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
// Revision 1.3  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.2  2004/08/05 21:10:36  ofri
// vctFixedStrideMatrixIterator: ctor argument initialColumn changed to signed int.
// vctVarStrideMatrixIterator bug fix: ctor now initializes CurrentColumn
//
// Revision 1.1  2004/08/04 21:11:10  anton
// cisstVector: Added preliminary version of dynamic matrices.  Lots of work
// is still required, this code is not ready to be used.
//
//
// ****************************************************************************
