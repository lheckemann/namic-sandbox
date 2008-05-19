/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedStrideVectorIterator.h,v 1.5 2005/09/26 15:41:47 anton Exp $

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
  \brief Declaration of vctFixedStrideVectorConstIterator and vctFixedStrideVectorIterator
 */


#ifndef _vctFixedStrideVectorIterator_h
#define _vctFixedStrideVectorIterator_h

#include <iterator>
#include <cisstVector/vctContainerTraits.h>


/*!  Define an iterator over a memory array with stride.  The iterator
  is defined according to the STL specifications of
  random-access-iterarator.  It is almost identical to
  std::vector::iterator, except that the ++, --, +=, -=, +, -,
  operations all work in _stride increments, and these are specified
  in compile time.

  We first define a const iterator, which only allows to read the
  referenced object.  The non-const iterator hass all the
  functionality of a const_iterator, plus the mutability of the
  objects.  We therefore derive vctFixedStrideVectorIterator from
  vctFixedStrideVectorConstIterator (as done with std::list
  iterators).

  In the current version, we do not define operator=, and rely on
  explicit declarations of objects and the default operator= and copy
  constructor.

  \param _stride the stride between elements of the vector being
  iterated over.

  \param _elementType the type of the element that the iterator refers
  to.

  \sa vctFixedStrideVectorIterator
*/
template<class _elementType, int _stride>
class vctFixedStrideVectorConstIterator:
    public std::iterator<std::random_access_iterator_tag, _elementType>
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    
    /*! This of the iterator itself. */
    typedef vctFixedStrideVectorConstIterator<_elementType, _stride> ThisType;

    /*! Base type for this iterator, i.e. std::iterator. */
    typedef std::iterator<std::random_access_iterator_tag, _elementType> BaseType;

    /*! Type (i.e. category) of iterator,
      i.e. std::random_access_iterator_tag. */
    typedef typename BaseType::iterator_category iterator_category;

    /* Stride between the elements of a vector. */
    enum {STRIDE = _stride};

protected:
    /*! The data member points to the object being refered by this
      iterator.  It is not declared const, so we will be able to use
      it in the non-const iterator class.  But all the methods in
      vctFixedStrideVectorConstIterator are declared const to protect the data
      from writing.
    */
    value_type * DataPtr;

public:
    /*! Default constructor: create an uninitialized object. */
    vctFixedStrideVectorConstIterator()
    {}


    /*! Constructor taking a non-const element pointer.  Note that
      only read operations will be performed! */
    explicit vctFixedStrideVectorConstIterator(value_type * dataPtr)
        : DataPtr(dataPtr)
    {}


    /*! Constructor taking a const element pointer.  We need it in
     order to deal with const containers being refered by this
     iterator.  We have to perform const_cast to convert back to the
     stored non-const member pointer of this iterator.  However, the
     non-const iterator class completely shadows the constructors of
     the const-iterator base class, and so we don't have to worry
     about creating a non-const iterator over a const container.
    */
    explicit vctFixedStrideVectorConstIterator(const value_type * dataPtr)
        : DataPtr(const_cast<value_type *>(dataPtr))
    {}


    /*! Pre-increment. */
    ThisType & operator++() {
        DataPtr += STRIDE;
        return *this;
    }


    /*! Post increment. */
    ThisType operator++(int) {
        ThisType tmp(*this);
        ++(*this);
        return tmp;
    }


    /*! Pre-decrement. */
    ThisType & operator--() {
        DataPtr -= STRIDE;
        return *this;
    }


    /*! Post decrement. */
    ThisType operator--(int) {
        ThisType tmp(*this);
        --(*this);
        return tmp;
    }


    /*! Increase by given difference. */
    ThisType & operator+=(difference_type difference) {
        DataPtr += difference * STRIDE;
        return *this;
    }


    /*! Decrease by given difference. */
    ThisType & operator-=(difference_type difference) {
        DataPtr -= difference * STRIDE;
        return *this;
    }


    /*! Find difference between iterators.  This method does not test
      for reachability, and may return invalid values if the two
      iterators are not reachable.  It is the user's responsibility to
      ensure the correctness.
    */
    difference_type operator-(const ThisType & other) const {
        return (DataPtr - other.DataPtr) / STRIDE;
    }


    /*! Random access (return const reference). */
    const value_type & operator[](difference_type index) const {
        return *(DataPtr + index * STRIDE);
    }


    /*! Dereference (const) */
    const value_type & operator* () const {
        return *DataPtr;
    }


    /*! Order relation between iterators, required by STL.
     \note The STL manual states that "if j is reachable from i then i<j".
     This does not imply the converse: "if i<j then j is reachable from i".
     In the case here, the converse does not hold.
    */
    bool operator< (const ThisType & other) const {
        return ((*this) - other) < 0;
    }


    /*! Equality of iterators, required by STL */
    bool operator== (const ThisType & other) const {
        return DataPtr == other.DataPtr;
    }


    /*! Complementary operation to operator < */
    bool operator> (const ThisType & other) const {
        return other < *this;
    }


    /*! Complementary operation to operator == */
    bool operator != (const ThisType & other) const {
        return !( (*this) == other );
    }
};




/*!  The non-const iterator with a fixed stride.
 \sa vctFixedStrideVectorConstIterator
*/
template<class _elementType, int _stride>
class vctFixedStrideVectorIterator : public vctFixedStrideVectorConstIterator<_elementType, _stride>
{
public:
    /* documented in base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctFixedStrideVectorIterator<_elementType, _stride> ThisType;
    typedef vctFixedStrideVectorConstIterator<_elementType, _stride> BaseType;
    typedef typename BaseType::iterator_category iterator_category;

    /*! Default constructor: create an uninitialized object */
    vctFixedStrideVectorIterator()
        : BaseType() 
        {}


    /*! Constructor taking a non-const element pointer.  Read and
      write operations on the refered object are permitted.  Note that
      there is no constructor that takes a const element pointer.
    */
    explicit vctFixedStrideVectorIterator(value_type * dataPtr)
        : BaseType(dataPtr) 
        {}


    /*! Redefine operator++ to return vctFixedStrideVectorIterator instead of
      vctFixedStrideVectorConstIterator */
    ThisType & operator++() {
        this->DataPtr += this->STRIDE;
        return *this;
    }


    /*! Redefine operator++ to return vctFixedStrideVectorIterator instead of
      vctFixedStrideVectorConstIterator */
    ThisType operator++(int) {
        ThisType tmp(*this);
        this->DataPtr += this->STRIDE;
        return tmp;
    }


    /*! Redefine operator-- to return vctFixedStrideVectorIterator instead of
      vctFixedStrideVectorConstIterator */
    ThisType & operator--() {
        this->DataPtr -= this->STRIDE;
        return *this;
    }


    /*! Redefine operator-- to return vctFixedStrideVectorIterator instead of
      vctFixedStrideVectorConstIterator */
    ThisType operator--(int) {
        ThisType tmp(*this);
        this->DataPtr -= this->STRIDE;
        return tmp;
    }


    /*! Redefine operator+= to return vctFixedStrideVectorIterator instead of
      vctFixedStrideVectorConstIterator */
    ThisType & operator+=(difference_type difference) {
        this->DataPtr += difference * this->STRIDE;
        return *this;
    }


    /*! Redefine operator-= to return vctFixedStrideVectorIterator instead of
      vctFixedStrideVectorConstIterator */
    ThisType & operator-=(difference_type difference) {
        this->DataPtr -= difference * this->STRIDE;
        return *this;
    }


    /*! Add non-const version of operator [] */
    value_type & operator[](difference_type index) const {
        return *(this->DataPtr + index * this->STRIDE);
    }


    /*! Add non-const version of unary operator * */
    value_type & operator* () {
        return *(this->DataPtr);
    }
};



/*! const_iterator + difference_type required by STL */
template<class _elementType, int _stride>
vctFixedStrideVectorConstIterator<_elementType, _stride>
operator+(const vctFixedStrideVectorConstIterator<_elementType, _stride> & iterator,
          typename vctFixedStrideVectorConstIterator<_elementType, _stride>::difference_type difference)
{
    vctFixedStrideVectorConstIterator<_elementType, _stride> result(iterator);
    return result += difference;
}


/*! difference_type + const_iterator required by STL */
template<class _elementType, int _stride>
vctFixedStrideVectorConstIterator<_elementType, _stride> 
operator+(typename vctFixedStrideVectorConstIterator<_elementType, _stride>::difference_type difference,
          const vctFixedStrideVectorConstIterator<_elementType, _stride> & iterator)
{
    vctFixedStrideVectorConstIterator<_elementType, _stride> result(iterator);
    return result += difference;
}


/*! const_iterator - difference_type required by STL */
template<class _elementType, int _stride>
vctFixedStrideVectorConstIterator<_elementType, _stride> 
operator-(const vctFixedStrideVectorConstIterator<_elementType, _stride> & iterator,
          typename vctFixedStrideVectorConstIterator<_elementType,_stride>::difference_type difference)
{
    vctFixedStrideVectorConstIterator<_elementType, _stride> result(iterator);
    return result -= difference;
}


/*! iterator + difference_type required by STL.  Note that this method
  returns a non-const iterator, which is why it is declared separately
  from the other operator +.
*/
template<class _elementType, int _stride>
vctFixedStrideVectorIterator<_elementType, _stride>
operator+(const vctFixedStrideVectorIterator<_elementType, _stride> & iterator,
          typename vctFixedStrideVectorIterator<_elementType, _stride>::difference_type difference)
{
    vctFixedStrideVectorIterator<_elementType, _stride> result(iterator);
    return result += difference;
}


/*! difference_type + iterator required by STL */
template<class _elementType, int _stride>
vctFixedStrideVectorIterator<_elementType, _stride> 
operator+(typename vctFixedStrideVectorIterator<_elementType, _stride>::difference_type difference,
          const vctFixedStrideVectorIterator<_elementType, _stride> & iterator)
{
    vctFixedStrideVectorIterator<_elementType, _stride> result(iterator);
    return result += difference;
}


/*! iterator - difference_type required by STL */
template<class _elementType, int _stride>
vctFixedStrideVectorIterator<_elementType, _stride> 
operator-(const vctFixedStrideVectorIterator<_elementType, _stride> & iterator,
          typename vctFixedStrideVectorIterator<_elementType, _stride>::difference_type difference)
{
    vctFixedStrideVectorIterator<_elementType, _stride> result(iterator);
    return result -= difference;
}


#endif  // _vctFixedStrideVectorIterator_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedStrideVectorIterator.h,v $
// Revision 1.5  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.4  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.3  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.2  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
// Revision 1.1  2004/08/13 17:47:40  anton
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
// Revision 1.2  2004/07/27 20:08:06  anton
// cisstVector: Added some Doxygen documentation.  Minor code changes to
// keep the dynamic vectors in sync with the fixed size ones.
//
// Revision 1.1  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.3  2004/04/06 15:23:15  anton
// Doxygen clean-up
//
// Revision 1.2  2004/02/18 15:50:23  anton
// Removed comments related to vctIteratorArithmetics.h
//
// Revision 1.1  2003/11/20 18:18:52  anton
// Array iterators are now sequence iterators
//
//
// Revision 1.3  2003/10/20 17:10:08  anton
// [] operator is const
//
// Revision 1.2  2003/10/17 20:23:38  ofri
// vctArrayIterator.h:
// 1) Clarified some documentation
// 2) Fixed alignment and some line breaks for better consistency & debugging.
//
// Revision 1.1  2003/09/30 14:02:49  anton
// creation
//
//
// ****************************************************************************
