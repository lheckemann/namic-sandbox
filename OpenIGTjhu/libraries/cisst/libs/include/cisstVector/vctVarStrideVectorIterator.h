/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctVarStrideVectorIterator.h,v 1.7 2005/09/26 15:41:47 anton Exp $
  
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
  \brief Declaration of vctVarStrideVectorConstIterator and vctVarStrideVectorIterator
 */


#ifndef _vctVarStrideVectorIterator_h
#define _vctVarStrideVectorIterator_h

#include <iterator>
#include <cisstVector/vctContainerTraits.h>


/*!
  Define an iterator over a memory array with stride.  The iterator is
  defined according to the STL specifications of
  random-access-iterarator.  It is almost identical to
  std::vector::iterator, except that the ++, --, +=, -=, +, -,
  operations all work in Stride increments.

  We first define a const iterator, which only allows to read the
  referenced object.  The non-const iterator hass all the
  functionality of a const_iterator, plus the mutability of the
  objects.  We therefore derive vctVarStrideVectorIterator from
  vctVarStrideVectorConstIterator (as done with std::list iterators).

  In the current version, we do not define operator=, and rely on
  explicit declarations of objects and the default operator= and copy
  constructor.

  \param _elementType the type of the element that the iterator refers to.

  \sa vctVarStrideVectorIterator
*/
template<class _elementType>
class vctVarStrideVectorConstIterator:
  public std::iterator<std::random_access_iterator_tag, _elementType>
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! The type of the iterator itself. */
    typedef vctVarStrideVectorConstIterator<_elementType> ThisType;

    /*! Base type for this iterator, i.e. std::iterator. */
    typedef std::iterator<std::random_access_iterator_tag, _elementType> BaseType;

    /*! Type (i.e. category) of iterator,
      i.e. std::random_access_iterator_tag. */
    typedef typename BaseType::iterator_category iterator_category;

protected:
    /*! The data member points to the object being refered by this
      iterator.  It is not declared const, so we will be able to use
      it in the non-const iterator class.  But all the methods in
      vctVarStrideVectorConstIterator are declared const to protect
      the data from writing.
    */
    value_type * DataPtr;

    /* Stride between the elements of a vector. */
    difference_type Stride;



public:
    /*! Default constructor: create an uninitialized object. */
    vctVarStrideVectorConstIterator():
        DataPtr(0),
        Stride(1)
        {}
    

    /*! Constructor taking a non-const element pointer.  Note that
      only read operations will be performed! */
    explicit vctVarStrideVectorConstIterator(value_type * dataPtr, difference_type stride = 1)
        : DataPtr(dataPtr), Stride(stride)
        {}
    
    
    /*! Constructor taking a const element pointer.  We need it in
      order to deal with const containers being refered by this
      iterator.  We have to perform const_cast to convert back to the
      stored non-const member pointer of this iterator.  However, the
      non-const iterator class completely shadows the constructors of
      the const-iterator base class, and so we don't have to worry
      about creating a non-const iterator over a const container.
    */
    explicit vctVarStrideVectorConstIterator(const value_type * dataPtr, difference_type stride = 1)
        : DataPtr(const_cast<value_type *>(dataPtr)), Stride(stride)
    {}


    /*! Pre-increment. */
    ThisType & operator++() {
        DataPtr += Stride;
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
        DataPtr -= Stride;
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
        DataPtr += difference * Stride;
        return *this;
    }


    /*! Decrease by given difference. */
    ThisType & operator-=(difference_type difference) {
        DataPtr -= difference * Stride;
        return *this;
    }


    /*! Find difference between iterators.  This method does not test
      for reachability, and may return invalid values if the two
      iterators are not reachable.  It is the user's responsibility to
      ensure the correctness.
    */
    difference_type operator-(const ThisType & other) const {
        return (DataPtr - other.DataPtr) / Stride;
    }


    /*! Random access (return const reference). */
    const value_type & operator[](difference_type index) const {
        return *(DataPtr + index * Stride);
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
 \sa vctVarStrideVectorConstIterator
*/
template<class _elementType>
class vctVarStrideVectorIterator : public vctVarStrideVectorConstIterator<_elementType>
{
public:
    /* documented in base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctVarStrideVectorIterator<_elementType> ThisType;
    typedef vctVarStrideVectorConstIterator<_elementType> BaseType;
    typedef typename BaseType::iterator_category iterator_category;

    /*! Default constructor: create an uninitialized object */
    vctVarStrideVectorIterator()
        : BaseType() 
        {}


    /*! Constructor taking a non-const element pointer.  Read and
      write operations on the refered object are permitted.  Note that
      there is no constructor that takes a const element pointer.
    */
    explicit vctVarStrideVectorIterator(value_type * dataPtr, difference_type stride = 1)
        : BaseType(dataPtr, stride) 
        {}


    /*! Redefine operator++ to return vctVarStrideVectorIterator instead of
      vctVarStrideVectorConstIterator */
    ThisType & operator++() {
        this->DataPtr += this->Stride;
        return *this;
    }


    /*! Redefine operator++ to return vctVarStrideVectorIterator instead of
      vctVarStrideVectorConstIterator */
    ThisType operator++(int) {
        ThisType tmp(*this);
        this->DataPtr += this->Stride;
        return tmp;
    }


    /*! Redefine operator-- to return vctVarStrideVectorIterator instead of
      vctVarStrideVectorConstIterator */
    ThisType & operator--() {
        this->DataPtr -= this->Stride;
        return *this;
    }


    /*! Redefine operator-- to return vctVarStrideVectorIterator instead of
      vctVarStrideVectorConstIterator */
    ThisType operator--(int) {
        ThisType tmp(*this);
        this->DataPtr -= this->Stride;
        return tmp;
    }


    /*! Redefine operator+= to return vctVarStrideVectorIterator instead of
      vctVarStrideVectorConstIterator */
    ThisType & operator+=(difference_type difference) {
        this->DataPtr += difference * this->Stride;
        return *this;
    }


    /*! Redefine operator-= to return vctVarStrideVectorIterator instead of
      vctVarStrideVectorConstIterator */
    ThisType & operator-=(difference_type difference) {
        this->DataPtr -= difference * this->Stride;
        return *this;
    }


    /*! Add non-const version of operator [] */
    value_type & operator[](difference_type index) const {
        return *(this->DataPtr + index * this->Stride);
    }


    /*! Add non-const version of unary operator * */
    value_type & operator* () {
        return *(this->DataPtr);
    }
};



/*! const_iterator + difference_type required by STL */
template<class _elementType>
vctVarStrideVectorConstIterator<_elementType>
operator+(const vctVarStrideVectorConstIterator<_elementType> & iterator,
          typename vctVarStrideVectorConstIterator<_elementType>::difference_type difference)
{
    vctVarStrideVectorConstIterator<_elementType> result(iterator);
    result += difference;
  return result;
}


/*! difference_type + const_iterator required by STL */
template<class _elementType>
vctVarStrideVectorConstIterator<_elementType> 
operator+(typename vctVarStrideVectorConstIterator<_elementType>::difference_type difference,
          const vctVarStrideVectorConstIterator<_elementType> & iterator)
{
    vctVarStrideVectorConstIterator<_elementType> result(iterator);
    return result += difference;
}


/*! const_iterator - difference_type required by STL */
template<class _elementType>
vctVarStrideVectorConstIterator<_elementType> 
operator-(const vctVarStrideVectorConstIterator<_elementType> & iterator,
          typename vctVarStrideVectorConstIterator<_elementType>::difference_type difference)
{
    vctVarStrideVectorConstIterator<_elementType> result(iterator);
    return result -= difference;
}


/*! iterator + difference_type required by STL.  Note that this method
  returns a non-const iterator, which is why it is declared separately
  from the other operator +.
*/
template<class _elementType>
vctVarStrideVectorIterator<_elementType>
operator+(const vctVarStrideVectorIterator<_elementType> & iterator,
          typename vctVarStrideVectorIterator<_elementType>::difference_type difference)
{
    vctVarStrideVectorIterator<_elementType> result(iterator);
    return result += difference;
}


/*! difference_type + iterator required by STL */
template<class _elementType>
vctVarStrideVectorIterator<_elementType> 
operator+(typename vctVarStrideVectorIterator<_elementType>::difference_type difference,
          const vctVarStrideVectorIterator<_elementType> & iterator)
{
    vctVarStrideVectorIterator<_elementType> result(iterator);
    return result += difference;
}


/*! iterator - difference_type required by STL */
template<class _elementType>
vctVarStrideVectorIterator<_elementType> 
operator-(const vctVarStrideVectorIterator<_elementType> & iterator,
          typename vctVarStrideVectorIterator<_elementType>::difference_type difference)
{
    vctVarStrideVectorIterator<_elementType> result(iterator);
    return result -= difference;
}


#endif  // _vctVarStrideVectorIterator_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctVarStrideVectorIterator.h,v $
// Revision 1.7  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.6  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.5  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.4  2004/11/17 18:24:52  anton
// vctVarStrideVectorIterator: operator++(int) should not return a reference.
//
// Revision 1.3  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
// Revision 1.2  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
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
// Revision 1.2  2004/08/04 21:08:58  anton
// rStrideSequenceIterator.h: Rewrote the constructors so that one can not
// create an iterator with a stride but no data pointer.
//
// Revision 1.1  2004/07/02 20:14:07  anton
// Code for VarStrideSequenceIterator.  Added the class as well as some code to
// try it.  An example is provided in examples/vectorTutorial/main.cpp.
//
//
// ****************************************************************************
