/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicVector.h,v 1.37 2006/01/03 03:37:17 anton Exp $

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


#ifndef _vctDynamicVector_h
#define _vctDynamicVector_h


/*!
  \file
  \brief Declaration of vctDynamicVector
*/


#include <cisstVector/vctDynamicVectorBase.h>
#include <cisstVector/vctDynamicVectorOwner.h>


/*!
  \ingroup cisstVector

  \brief A vector object of dynamic size.

  This class defines a vector object of dynamic size with memory
  allocation.  Note that unlike the STL vector class (std::vector),
  this class is not meant to be used as a dynamic container, but in
  vector algebra.

  The algebraic operations are mostly inherited from the base classes
  vctDynamicVectorBase and vctDynamicConstVectorBase.  Here, we will
  briefly describe the specific properties of vctDynamicVector, with a
  few usage examples.

  <ol>
  <li> The class is templated by its element type, that is, the vector
  element.  Normally, the element should be an arithmetic type, that
  is, support all the standard arithmetic operations: +, -, *, /, =,
  ==, <, >, <=, >=, ...
  <li> The class uses dynamically allocated memory, and, more
  importantly, \em owns the memory.  That is, a vctDynamicVector object
  automatically frees the allocated memory it owns when it is
  destroyed.
  <li>To allocate the memory, use one of the following operations.
  \code
  // define a typical element type
  typedef double ElementType;

  // the vectorSize variable can be set to any value at any time
  // before creating the vector.
  unsigned int vectorSize = 12;
  
  // constructor allocation
  vctDynamicVector<ElementType> v1(vectorSize);
  
  // Create an empty vector and later allocate memory.
  vctDynamicVector<ElementType> v2;
  v2.SetSize(vectorSize);

  // Create a dynamic vector of some size and then change it.
  // This operation does not preserve any elements in the resized
  // vector
  vctDynamicVector<Elements> v3(3 * vectorSize);
  v3.SetSize(2 * vectorSize);
  
  // resize a vector and keep as many elements as possible.
  v3.resize(vectorSize);
  
  // Store an algebraic result to a new vector.  In this case,
  // memory is allocated by the algebraic operation, and then
  // attached to the vector object.
  vctDynamicVector<double> v4 = v3 - v2;
  \endcode
  <li> Vector assignment can be facilitated through the Assign method
  (defined in the base class) or as follows.
  \code
  // Initialize all elements to the same value
  vctDynamicVector<ElementType> v5(vectorSize, 2.0);
  
  // Initialize the elements by specific value.  NOTE: All the
  // arguments MUST be of type ElementType
  vctDynamicVector<ElementType> sequence(7,
                                         1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0);
  
  // Assign one vector to another.  This operation reallocates
  // space for the target vector.  Note that the right-hand-side
  // object can be any ``dynamic'' vector of any element type,
  // not just a vctDynamicVector<ElementType>
  vctDynamicMatrix<int> someNumbers(numRows, numCols);
  v1 = someNumbers.Column(2);
  \endcode
  </ol>

  A few more notes.
  <ul>
  <li> The stride is always 1, that is, the elements lie in contiguous
  increasing memory addresses.
  <li> There is no direct way of ``disowning'' a vctDynamicVector,
  that is taking ownership of its memory.
  <li> When a function returns an allocating dynamic vector object, it
  is better to return a vctReturnDynamicVector, which is a helper
  class designed for transfering the ownership of the allocated memory
  without reallocation.
  </ul>

  \param _elementType the type of an element in the vector

  \sa vctDynamicVectorBase vctDynamicConstVectorBase
*/
template<class _elementType>
class vctDynamicVector : public vctDynamicVectorBase<vctDynamicVectorOwner<_elementType>, _elementType>
{

    friend class vctReturnDynamicVector<_elementType>;

public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctDynamicVector<_elementType> ThisType;
    typedef vctDynamicVectorBase<vctDynamicVectorOwner<_elementType>, _elementType> BaseType;
    typedef typename BaseType::CopyType CopyType; 
    typedef typename BaseType::TypeTraits TypeTraits; 
    typedef typename BaseType::ElementVaArgPromotion ElementVaArgPromotion; 

    /*! Default constructor. Initialize an empty vector. */
    vctDynamicVector()
        // The default initialization of vctDynamicVectorOwner is empty.
    {}


    /*! Constructor: Create a vector of the specified size.  Elements
       initialized with default constructor. */
    explicit vctDynamicVector(size_type size) {
        SetSize(size);
    }

    /*! Constructor: Create a vector of the specified size and assign all
      elements a specific value. */
    vctDynamicVector(size_type size, value_type value) {
        SetSize(size);
        SetAll(value);
    }

    /*! Constructor for any size greater than or equal to 2, using
      stdarg macros and variable number of arguments.  This operation
      assumes that all the arguments are of type value_type, and that
      their number is equal to the size of the vector.  The user may
      need to explicitly cast the parameters to value_type to avoid
      runtime bugs and errors.  We have not checked if stdarg macros
      can use reference types (probably not), so unlike the other
      constructors, this constructor takes all arguments by value.

      \note This constructor does not assert that the size is correct,
      as there is no way to know how many arguments were passed.
    */
    vctDynamicVector(size_type size, value_type element0, value_type element1, ...) throw(std::runtime_error) {
        if (size < 2) {
            cmnThrow(std::runtime_error("vctDynamicVector: Constructor from va_list requires size >= 2"));
        }
        SetSize(size);
        this->at(0) = element0;
        this->at(1) = element1;
        va_list nextArg;
        va_start(nextArg, element1);
        for (unsigned int i = 2; i < size; ++i) {
            this->at(i) = value_type( va_arg(nextArg, ElementVaArgPromotion) );
        }
        va_end(nextArg);
    }

    /*! Constructor: Create a vector of the specified size and assign the
      elements values from the memory block pointed to */
    vctDynamicVector(size_type size, value_type * values) {
        SetSize(size);
        Assign(values);
    }

    /*! Special copy constructor: Take ownership of the data of a
      temporary vector object of type vctReturnDynamicVector.  Disown
      the other vector.
    */
    vctDynamicVector(const vctReturnDynamicVector<value_type> & otherVector);

    
    /*! Copy constructor: Allocate memory to store a copy of the other
      vector, and copy the elements of the other vector to this
      vector.
    */
    vctDynamicVector(const ThisType & otherVector) {
        SetSize(otherVector.size());
        this->Assign(otherVector);
    }
    
    /*! Copy constructor: Allocate memory and copy all the elements
      from the other vector. */
    template<class _otherVectorOwnerType>
    vctDynamicVector(const vctDynamicConstVectorBase<_otherVectorOwnerType, _elementType> & otherVector) {
        SetSize(otherVector.size());
        this->Assign(otherVector);
    }


    /*!  Assignment from a dynamic vector to a vector.  The
      operation discards the old memory allocated for this vector, and
      allocates new memory the size of the input vector.  Then the
      elements of the input vector are copied into this vector.
    */
    template <class __vectorOwnerType, typename __elementType>
    ThisType & operator=(const vctDynamicConstVectorBase<__vectorOwnerType, __elementType> & otherVector) {
        SetSize(otherVector.size());
        this->Assign(otherVector);
    return *this;
    }


    /*!  Assignment from a dynamic vector to this vector.  The
      operation discards the old memory allocated for this vector, and
      allocates new memory the size of the input vector.  Then the
      elements of the input vector are copied into this vector.
    */
    ThisType & operator=(const ThisType& other) {
        SetSize(other.size());
        this->Assign(other);
    return *this;
    }

    /*!  Assignement from a transitional vctReturnDynamicVector to a
      vctDynamicVector variable.  This specialized operation does not
      perform any element copy.  Instead it transfers ownership of the
      data from the other vector to this vector, and disowns the other
      vector.  The right hand side operand must be a temporary object
      returned, e.g., from a function or overloaded operator.
    */
    ThisType & operator=(const vctReturnDynamicVector<value_type> & other);

    /*! Assignement of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value) {
        this->SetAll(value);
        return *this;
    }

    /*! Non-destructive size change.  Change the size to the specified
      size, and copy as many elements as possible from the former
      vector.
    */
    void resize(size_type size) {
        this->Vector.resize(size);
    }

    /*! DESTRUCTIVE size change.  Change the size to the specified
      size.  Discard of all the old values. */
    void SetSize(size_type size) {
        this->Vector.SetSize(size);
    }
};


/*!
  \ingroup cisstVector
  
  Class vctReturnDynamicVector is specialized to store a temporary
  vector object by transfering ownership.  An object of this class has
  all the methods inherited from vctDynamicVector, but can only be
  constructed in one way -- taking the ownership from another
  vctDynamicVector (or vctReturnDynamicVector) object.  In turn, when
  an assignment occurs from a vctReturnDynamicVector to a
  vctDynamicVector (or likewise construction), the lhs
  vctDynamicVector strips this object from the ownership of the data.

  Use this class only when you want to return a newly created dynamic
  vector from a function, without going through memory allocation and
  deallocation.  Never use it on an object that is going to remain
  in scope after constructing the vctReturnDynamicVector.
*/
template<class _elementType>
class vctReturnDynamicVector : public vctDynamicVector<_elementType> {
public:
    /*! Base type of vctReturnDynamicVector. */
    typedef vctDynamicVector<_elementType> BaseType;
    explicit vctReturnDynamicVector(const BaseType & other) {
        BaseType & nonConstOther = const_cast<BaseType &>(other);
        // if we don't save it in a variable, it will be destroyed in the Release operation
        const unsigned int size = other.size();
        this->Vector.Own(size, nonConstOther.Vector.Release());
    }
};


// implementation of the special copy constuctor of vctDynamicVector
template<class _elementType>
vctDynamicVector<_elementType>::vctDynamicVector(const vctReturnDynamicVector<_elementType> & other) {
    vctReturnDynamicVector<_elementType> & nonConstOther =
        const_cast< vctReturnDynamicVector<_elementType> & >(other);
    // if we don't save it in a variable, it will be destroyed in the Release operation
    const unsigned int size = other.size();
    this->Vector.Own(size, nonConstOther.Vector.Release());
}


// implementation of the special assignment operator from vctReturnDynamicVector to vctDynamicVector
template<class _elementType>
vctDynamicVector<_elementType> &
vctDynamicVector<_elementType>::operator =(const vctReturnDynamicVector<_elementType> & other) {
    vctReturnDynamicVector<_elementType> & nonConstOther =
        const_cast< vctReturnDynamicVector<_elementType> & >(other);
    // if we don't save it in a variable, it will be destroyed in the Release operation
    const unsigned int size = other.size();
    this->Vector.Disown();
    this->Vector.Own(size, nonConstOther.Vector.Release());
    return *this;
}


/*! \name Elementwise operations between dynamic vectors. */
//@{
/*! Operation between dynamic vectors (same size).
  \param inputVector1 The first operand of the binary operation.
  \param inputVector2 The second operand of the binary operation.
  \return The vector result of \f$op(vector1, vector2)\f$.
*/
template<class _vectorOwnerType1, class _vectorOwnerType2, class _elementType>
vctReturnDynamicVector<_elementType>
operator + (const vctDynamicConstVectorBase<_vectorOwnerType1, _elementType> & inputVector1,
            const vctDynamicConstVectorBase<_vectorOwnerType2, _elementType> & inputVector2) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector1);
    resultStorage.Add(inputVector2);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template<class _vectorOwnerType1, class _vectorOwnerType2, class _elementType>
vctReturnDynamicVector<_elementType>
operator - (const vctDynamicConstVectorBase<_vectorOwnerType1, _elementType> & inputVector1,
            const vctDynamicConstVectorBase<_vectorOwnerType2, _elementType> & inputVector2) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector1);
    resultStorage.Subtract(inputVector2);
    return vctReturnDynamicVector<value_type>(resultStorage);
}
//@}


/*! \name Cross product between dynamic vectors. */
/*! The cross product between two dynamic vectors will assert that
  both input vectors are of size 3.

  \param inputVector1 left operand of the cross product

  \param inputVector2 right operand of the cross product

  \return The cross product as a vctReturnDynamicVector
*/
//@{
template<class _vectorOwnerType1, class _vectorOwnerType2, class _elementType>
vctReturnDynamicVector<_elementType>
operator % (const vctDynamicConstVectorBase<_vectorOwnerType1, _elementType> & inputVector1,
            const vctDynamicConstVectorBase<_vectorOwnerType2, _elementType> & inputVector2) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(3);
    resultStorage.CrossProductOf(inputVector1, inputVector2);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

template<class _vectorOwnerType1, class _vectorOwnerType2, class _elementType>
vctReturnDynamicVector<_elementType>
vctCrossProduct(const vctDynamicConstVectorBase<_vectorOwnerType1, _elementType> & inputVector1,
                const vctDynamicConstVectorBase<_vectorOwnerType2, _elementType> & inputVector2) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(3);
    resultStorage.CrossProductOf(inputVector1, inputVector2);
    return vctReturnDynamicVector<value_type>(resultStorage);
}
//@}



/*! \name Elementwise operations between a dynamic vector and a
  scalar. */
//@{
/*! Operation between a dynamic vector and a scalar.
  \param inputVector The first operand of the binary operation.
  \param inputScalar The second operand of the binary operation.
  \return The vector result of \f$op(vector, scalar)\f$.
*/
template<class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator + (const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector);
    resultStorage.Add(inputScalar);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template<class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator - (const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector);
    resultStorage.Subtract(inputScalar);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template<class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator * (const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector);
    resultStorage.Multiply(inputScalar);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template<class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator / (const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector);
    resultStorage.Divide(inputScalar);
    return vctReturnDynamicVector<value_type>(resultStorage);
}
//@}



/*! \name Elementwise operations between a scalar and a dynamic
  vector. */
//@{
/*! Operation between a scalar and a dynamic vector.
  \param inputScalar The first operand of the binary operation.
  \param inputVector The second operand of the binary operation.
  \return The vector result of \f$op(scalar, vector)\f$.
*/
template<class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator + (const _elementType & inputScalar,
            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector.size());
    resultStorage.SumOf(inputScalar, inputVector);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template<class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator - (const _elementType & inputScalar,
            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector.size());
    resultStorage.DifferenceOf(inputScalar, inputVector);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template<class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator * (const _elementType & inputScalar,
            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector.size());
    resultStorage.ProductOf(inputScalar, inputVector);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented above */
template<class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator / (const _elementType & inputScalar,
            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector.size());
    resultStorage.RatioOf(inputScalar, inputVector);
    return vctReturnDynamicVector<value_type>(resultStorage);
}
//@}


/*! \name Elementwise operations on a dynamic vector. */
//@{
/*! Unary operation on a dynamic vector.
  \param inputVector The operand of the unary operation
  \return The vector result of \f$op(vector)\f$.
*/
template<class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator - (const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector.size());
    resultStorage.NegationOf(inputVector);
    return vctReturnDynamicVector<value_type>(resultStorage);
}
//@}



/*
  Methods declared previously and implemented here because they require vctReturnDynamicVector
*/

#ifndef DOXYGEN
/* documented in class vctDynamicConstVectorBase */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::VectorValueType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::Abs(void) const {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(this->size());
    vctDynamicVectorLoopEngines::
        VoVi<typename vctUnaryOperations<value_type>::AbsValue>::
        Run(resultStorage, *this);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented in class vctDynamicConstVectorBase */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::VectorValueType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::Negation(void) const {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(this->size());
    vctDynamicVectorLoopEngines::
        VoVi<typename vctUnaryOperations<value_type>::Negation>::
        Run(resultStorage, *this);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented in class vctDynamicConstVectorBase */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::VectorValueType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::Floor(void) const {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(this->size());
    vctDynamicVectorLoopEngines::
        VoVi<typename vctUnaryOperations<value_type>::Floor>::
        Run(resultStorage, *this);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented in class vctDynamicConstVectorBase */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::VectorValueType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::Ceil(void) const {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(this->size());
    vctDynamicVectorLoopEngines::
        VoVi<typename vctUnaryOperations<value_type>::Ceil>::
        Run(resultStorage, *this);
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented in class vctDynamicConstVectorBase */
template <class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::VectorValueType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::Normalized(void) const throw(std::runtime_error) {
    vctDynamicVector<value_type> resultStorage(*this);
    resultStorage.NormalizedSelf();
    return vctReturnDynamicVector<value_type>(resultStorage);
}

/* documented in class vctDynamicConstVectorBase */
template<class _vectorOwnerType, class __vectorOwnerType, class _elementType,
         class _elementOperationType>
inline vctReturnDynamicVector<bool>
vctDynamicVectorElementwiseCompareVector(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector1,
                                         const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector2) {
    vctDynamicVector<bool> result(vector1.size());
    vctDynamicVectorLoopEngines::
        VoViVi<_elementOperationType>::Run(result, vector1, vector2);
    return vctReturnDynamicVector<bool>(result);
}

/* documented in class vctDynamicConstVectorBase */
template<class _vectorOwnerType, class _elementType, class _elementOperationType>
inline vctReturnDynamicVector<bool>
vctDynamicVectorElementwiseCompareScalar(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector,
                                         const _elementType & scalar) {
    vctDynamicVector<bool> result(vector.size());
    vctDynamicVectorLoopEngines::
        VoViSi<_elementOperationType>::Run(result, vector, scalar);
    return vctReturnDynamicVector<bool>(result);
}

#endif // DOXYGEN

#endif // _vctDynamicVector_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicVector.h,v $
//  Revision 1.37  2006/01/03 03:37:17  anton
//  vctDynamic{Vector,Matrix}.h: Added Doxygen class documentation based on Ofri's
//  input.
//
//  Revision 1.36  2005/12/23 21:27:31  anton
//  cisstVector: Minor updates for Doxygen 1.4.5.
//
//  Revision 1.35  2005/12/13 00:04:57  ofri
//  All cisstVector containers: Added methods
//  1) MinAndMaxElement  (at const base)
//  2) Floor, Ceil  (at const base -- implemented at container)
//  3) FloorSelf, CeilSelf, FloorOf, CeilOf  (at non-const base)
//
//  Revision 1.34  2005/12/02 16:23:54  anton
//  cisstVector: Added assigment operator from scalar (see ticket #191).
//
//  Revision 1.33  2005/11/15 22:28:34  anton
//  vctDynamicVector: Constructor from one scalar is explicit to prevent the
//  compiler from defining the operator=(scalar).  See ticket #191.
//
//  Revision 1.32  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.31  2005/09/24 00:01:04  anton
//  cisstVector: Use cmnThrow for all exceptions.
//
//  Revision 1.30  2005/08/11 20:35:39  anton
//  vctDynamicVector: Added Assign and ctor from va_list (see ticket #64, also
//  has flaws listed in tickets #136 and #150).
//
//  Revision 1.29  2005/06/16 03:38:29  anton
//  Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
//  VC 7.1 (.Net 2003).
//
//  Revision 1.28  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.27  2005/04/29 14:31:20  ofri
//  vctDynamicVector: Added constructor that takes a C-style size and
//  "value_type *", and copies elements from the pointed array to the new nector.
//  Compare with vctFixedSizeVector(__elementType *)
//
//  Revision 1.26  2005/04/04 19:48:34  anton
//  cisstVector: In base types for dynamic containers, removed default value for
//  _elementType (was based on _ownerType::value_type) to -1- avoid issues with
//  SWIG and -2- force explicit declaration of _elementType whenever a base type
//  is used.
//
//  Revision 1.25  2005/02/03 19:11:35  anton
//  cisstVector: Added global function vctCrossProduct() as requested in
//  ticket #126.
//
//  Revision 1.24  2005/01/11 22:42:18  anton
//  cisstVector:  Added normalization methods to the vectors.  Removed useless
//  code from vctQuaternion and added some tests.  See ticket #110.
//
//  Revision 1.23  2005/01/06 18:54:00  anton
//  cisstVector: Introduction of type CopyType as requested in ticket #113.
//
//  Revision 1.22  2004/11/18 20:57:30  ofri
//  RE: ticket #92.  Redefined all operator= I could find to return a ThisType &
//  instead of const ThisType &.  This included corresponding revision of the
//  Assign methods, and also definition from scratch of operator= for fixed and
//  dynamic matrix ref.
//
//  Revision 1.21  2004/10/25 19:19:54  anton
//  cisstVector:  Created vctForwardDeclarations.h and removed forward
//  declarations of classes in all other files.  Added some constructors
//  for vector references to reference fixed size from dynamic and vice versa.
//
//  Revision 1.20  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.19  2004/10/21 19:27:47  anton
//  cisstVector: Added types BoolValueType and ValueType for
//  vctFixedSizeVector, vctDynamicVector, vctDynamicMatrix.  This solves #77.
//
//  Revision 1.18  2004/10/07 21:15:42  ofri
//  vctDynamicVector: added constructor with value for all elements
//
//  Revision 1.17  2004/10/07 19:45:46  anton
//  cisstVector: Added % operator and tested CrossProduct for dynamic vectors.
//  This will close ticket #23.
//
//  Revision 1.16  2004/09/30 15:49:51  anton
//  cisstVector: solution for ticket #65 regarding a compilation error with
//  .Net 2003.  The issue remains mysterious but but a solution is to declare
//  the problematic methods in the class declaration and implement them right
//  after (in the same file).  The methods in question are the Elementwise
//  comparaison with a scalar (it works if comparing with a vector/matrix).
//
//  Revision 1.15  2004/09/28 18:32:36  ofri
//  Moved the definitions of elementwise comparison operations between dynamic
//  vector and scalar from vctDynamicConstVectorBase to vctDynamicVector.
//  Changed the return type from vctDynamicVector to vctReturnDynamicVector.
//  The result gets compiled successfully on gcc, .net7, .net2003.  Tests executed
//  successfully on gcc.  See ticket #65.
//
//  Revision 1.14  2004/09/24 20:20:42  anton
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
//  Revision 1.13  2004/09/03 19:56:36  anton
//  Doxygen documentation.
//
//  Revision 1.12  2004/08/13 17:47:40  anton
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
//  Revision 1.11  2004/08/04 21:03:55  anton
//  cisstVector: Minor Doxygen corrections for vctDynamicVector.h
//
//  Revision 1.10  2004/07/27 20:08:06  anton
//  cisstVector: Added some Doxygen documentation.  Minor code changes to
//  keep the dynamic vectors in sync with the fixed size ones.
//
//  Revision 1.9  2004/07/23 18:02:35  anton
//  vctDynamic vector and vectors:
//  *: Added comparison operators (==, !=, <=, etc) for sequences and scalars
//  *: Added Abs, Negation, AbsOf, NegationOf, AbsSelf, NegationSelf methods
//  *: Added Add, Subtract, Multiply, Divide for scalars as well as
//     +=, -=, /=, *= for vector and scalar
//  *: Added global operators +, -, *, / for vector/vector, vector/scalar,
//     scalar/vector (whenever it was pertinent)
//  *: Added loop engined {quote: Vio} and {quote: SoViSi}
//
//  Revision 1.8  2004/07/23 12:57:42  ofri
//  Fixing bugs related to assignment and construction of vctDynamicVector from
//  vctReturnDynamicVector.  Added some documentation.
//
//  Revision 1.7  2004/07/16 14:39:10  anton
//  vctDynamicVector:  Added a couple of = operators.
//
//  Revision 1.6  2004/07/13 16:27:22  ofri
//  Updating vctDynamicVector:  1) Compilation error correction (non-const cast)
//  2)  Defined operator+(vector, vector).  *not tested*.
//
//  Revision 1.5  2004/07/13 01:52:32  ofri
//  vctDynamicVector: Added documentation.  Added copy constructors.
//  Added class vctReturnDynamicVector -- *tentative version*.
//
//  Revision 1.4  2004/07/12 20:33:27  anton
//  vctDynamicVector: Added default constructor which sets the size to zero
//
//  Revision 1.3  2004/07/06 19:42:50  ofri
//  Giving the dynamic vector classes "standard look and feel" in terms of type
//  members, constructors, and indentations.  I made sure that the corresponding
//  example compiles and runs successfully.
//
//  Revision 1.2  2004/07/06 17:47:11  anton
//  Dynamic Sequences:
//  *: Compilation error re. const and const_iterator
//  *: Added some iterator methods to sequences
//  *: Compilation errors re. the order (size, stride, data) in constructor.  The order is now (size, data, stride = 1) everywhere to be consistant
//  *: The method Resize is now resize (STL spelling)
//
//  Revision 1.1  2004/07/02 16:16:45  anton
//  Massive renaming in cisstVector for the addition of dynamic size vectors
//  and matrices.  The dynamic vectors are far from ready.
//
//
// ****************************************************************************
