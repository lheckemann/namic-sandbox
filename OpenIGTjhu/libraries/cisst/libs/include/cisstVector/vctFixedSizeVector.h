/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeVector.h,v 1.51 2005/12/23 21:27:31 anton Exp $

  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on:  8/18/2003

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
  \brief Declaration of vctFixedSizeVector
 */

#ifndef _vctFixedSizeVector_h
#define _vctFixedSizeVector_h


#include <cisstVector/vctFixedSizeVectorRef.h>



/*! \brief Implementation of a fixed-size vector using template
  metaprogramming.

  \ingroup cisstVector

  The vector type is stored as a contiguous array of a fixed size
  (stack allocation).  It provides methods for operations which are
  implemented using template metaprogramming.  See
  vctFixedSizeConstVectorBase and vctFixedSizeVectorBase for
  more implementation details.

  \param _elementType the type of an element in the vector
  \param _size the size of the vector

  \note We do not provide a constructor from one <code>value_type *</code>.
  You can use vctFixedSizeVectorBase::Assign(const value_type * ) for
  that purpose.  The reason is we did not want to have too many functions
  in the first place.
*/
template<class _elementType, unsigned int _size>
class vctFixedSizeVector : public vctFixedSizeVectorBase<
    _size, 1, _elementType,
    typename vctFixedSizeVectorTraits<_elementType, _size, 1>::array 
    >
{
 public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! Type traits for the vector. Based on type of elements, size
       and stride, it defines array, pointer, etc. (see
       vctFixedSizeVectorTraits). */
    typedef vctFixedSizeVectorTraits<_elementType, _size, 1> VectorTraits;
    /* no need to document, inherit doxygen documentation from
       vctFixedSizeVectorBase */
    typedef vctFixedSizeVector<value_type, _size> ThisType;
    typedef vctFixedSizeVectorBase<_size, 1, _elementType, typename VectorTraits::array> BaseType;
    typedef typename BaseType::CopyType CopyType;
    /*! Type traits for the elements of the vector. */
    typedef class cmnTypeTraits<value_type> TypeTraits;
    typedef typename TypeTraits::VaArgPromotion ElementVaArgPromotion;

    enum {SIZEMINUSONE = _size - 1};

 public:
    /*! Default constructor.  Do nothing. */
    inline vctFixedSizeVector()
        {}

    /*! Initialize all the elements to the given value.
      \param value the value used to set all the elements of the vector
    */
    explicit inline vctFixedSizeVector(const value_type & value) {
        SetAll(value);
    }
    
    /*! Initialize the elements of this vector with values from another
      vector.  The other vector can include elements of any type,
      which will be converted using standard conversion to elements of this
      vector.
    */
    template<class __elementType, int __stride, class __dataPtrType>
    explicit inline
    vctFixedSizeVector(const vctFixedSizeConstVectorBase<_size, __stride, __elementType, __dataPtrType> & vector) {
        this->Assign(vector);
    }

    //@{
    /*! initializing a vector from a vector-ref of the same element type does
      not need to be declared "explicit".
    */
    template<int __stride>
    inline
    vctFixedSizeVector(const vctFixedSizeConstVectorRef<_elementType, _size, __stride> & vector) {
        this->Assign(vector);
    }

    template<int __stride>
    inline
    vctFixedSizeVector(const vctFixedSizeVectorRef<_elementType, _size, __stride> & vector) {
        this->Assign(vector);
    }
    //@}

    /*! Initialize the elements of this vector by appending one value
      after the elements from another vector.  The other vector has
      to have the same element type as this one.
    */
    template<int __stride, class __dataPtrType>
    inline
    vctFixedSizeVector(const vctFixedSizeConstVectorBase<SIZEMINUSONE, __stride, _elementType, __dataPtrType> & vector, value_type lastElement) {
        // vctFixedSizeVector(const vctFixedSizeConstVectorBase<_size-1, __stride, _elementType, __dataPtrType> & vector, value_type lastElement) {
        this->ConcatenationOf(vector, lastElement);
    }

    /*! Initialize this vector from a conventional C array by copying
      the elements (assuming input stride == 1).
    */
    template<class __elementType>
    explicit inline
    vctFixedSizeVector(const __elementType * elements)
    {
        this->Assign(elements);
    }


    /*! \name Size dependant constructors.
      
      The following methods are size dependant, i.e. don't necessarily
      mean anything for all sizes of vector.  For example, using the
      constructor vctFixedSizeVector(value, value, value) on a vector
      of size 2 shouldn't be allowed.  It would have been possible to
      check these errors at compilation time, but this would require
      an overhead of code and therefore execution time which is not
      worth it.  Therefore, we are using #CMN_ASSERT to check that
      the template parameter _size is valid
      
      \note Using #CMN_ASSERT on a template parameter still allows the
      compiler to perform some optimization, which would be harder if
      assert was testing a method paramater.

      \note The constructor with one argument is reserved for a
      SetAll semantics.
      */
    
    //@{

    /*! Constructor for a vector of size 2. */
    inline vctFixedSizeVector(const value_type & element0, const value_type & element1) {
        CMN_ASSERT(_size == 2);
        (*this)[0] = element0;
        (*this)[1] = element1;
    }

    /*! Constructor for a vector of size 3. */
    inline vctFixedSizeVector(const value_type & element0, const value_type & element1,
                              const value_type & element2) {
        CMN_ASSERT(_size == 3);
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
    }

    /*! Constructor for a vector of size 4. */
    inline vctFixedSizeVector(const value_type & element0, const value_type & element1,
                              const value_type & element2, const value_type & element3) {
        CMN_ASSERT(_size == 4);
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
        (*this)[3] = element3;
    }

    /*! Constructor for any size greater than 4, using stdarg macros
      and variable number of arguments.  This operation assumes that
      all the arguments are of type value_type, and that their number
      is equal to the size of the vector.  The user may need to
      explicitly cast the parameters to value_type to avoid runtime
      bugs and errors.  We have not checked if stdarg macros can use
      reference types (probably not), so unlike the other
      constructors, this constructor takes all arguments by value.

      \note This constructor does not assert that the size is correct,
      as there is no way to know how many arguments were passed.
    */
    inline vctFixedSizeVector(const value_type element0, const value_type element1,
        const value_type element2, const value_type element3, const value_type element4, ...)
    {
        CMN_ASSERT(_size > 4);
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
        (*this)[3] = element3;
        (*this)[4] = element4;
        va_list nextArg;
        va_start(nextArg, element4);
        for (unsigned int i = 5; i < _size; ++i) {
            (*this).at(i) = value_type( va_arg(nextArg, ElementVaArgPromotion) );
        }
        va_end(nextArg);
    }

    //@}


    /*!
      Assignment operation between vectors of different types.

      \param other The vector to be copied.
    */
    //@{
  inline ThisType & operator = (const ThisType & other) {
    return reinterpret_cast<ThisType &>(this->Assign(other));
  }

  template<int __stride, class __elementType, class __dataPtrType>
    inline ThisType & operator = (const vctFixedSizeConstVectorBase<_size, __stride, __elementType,__dataPtrType> & other) {
        return reinterpret_cast<ThisType &>(this->Assign(other));
  }
  
  template<int __stride>
    inline ThisType & operator = (const vctFixedSizeConstVectorRef<value_type, _size, __stride> & other) {
    return reinterpret_cast<ThisType &>(this->Assign(other));
  }

  template<int __stride, class __elementType>
    inline ThisType & operator = (const vctFixedSizeConstVectorRef<__elementType, _size, __stride> & other) {
    this->Assign(other);
    return *this;
  }
    //@}

    /*! Assignement of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value) {
        this->SetAll(value);
        return *this;
    }

};


/*! \name Elementwise operations between fixed size vectors. */
//@{
/*! Operation between fixed size vectors (same size).
  \param vector1 The first operand of the binary operation.
  \param vector2 The second operand of the binary operation.
  \return The vector result of \f$op(vector1, vector2)\f$. */
template<unsigned int _size, int _vector1Stride, class _vector1Data, int _vector2Stride, class _vector2Data, class _elementType>
inline vctFixedSizeVector<_elementType, _size>
operator + (const vctFixedSizeConstVectorBase<_size, _vector1Stride, _elementType, _vector1Data> & vector1,
            const vctFixedSizeConstVectorBase<_size, _vector2Stride, _elementType, _vector2Data> & vector2)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.SumOf(vector1, vector2);
    return result; 
}

/* documented above */
template<unsigned int _size, int _vector1Stride, class _vector1Data, int _vector2Stride, class _vector2Data, class _elementType>
inline vctFixedSizeVector<_elementType, _size>
operator - (const vctFixedSizeConstVectorBase<_size, _vector1Stride, _elementType, _vector1Data> & vector1,
            const vctFixedSizeConstVectorBase<_size, _vector2Stride, _elementType, _vector2Data> & vector2)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.DifferenceOf(vector1, vector2);
    return result;
}

//@}


/*! Cross product between fixed size vectors (size 3).
  \param vector1 The first operand of the cross product.
  \param vector2 The second operand of the cross product.
  \return The cross product. */
template<int _vector1Stride, class _vector1Data, int _vector2Stride, class _vector2Data, class _elementType>
inline vctFixedSizeVector<_elementType, 3>
vctCrossProduct(const vctFixedSizeConstVectorBase<3, _vector1Stride, _elementType, _vector1Data> & vector1,
                const vctFixedSizeConstVectorBase<3, _vector2Stride, _elementType, _vector2Data> & vector2)
{
    vctFixedSizeVector<_elementType, 3> result;
    result.CrossProductOf(vector1, vector2);
    return result; 
}


/*! Cross product between fixed size vectors (size 3).
  \param vector1 The first operand of the cross product.
  \param vector2 The second operand of the cross product.
  \return The cross product. */
template<int _vector1Stride, class _vector1Data, int _vector2Stride, class _vector2Data, class _elementType>
inline vctFixedSizeVector<_elementType, 3>
operator % (const vctFixedSizeConstVectorBase<3, _vector1Stride, _elementType, _vector1Data> & vector1,
            const vctFixedSizeConstVectorBase<3, _vector2Stride, _elementType, _vector2Data> & vector2)
{
    vctFixedSizeVector<_elementType, 3> result;
    result.CrossProductOf(vector1, vector2);
    return result; 
}


/*! \name Elementwise operations between a fixed size vector and a scalar. */
//@{
/*! Operation between a fixed size vector and a scalar.
 \param vector The first operand of the binary operation.
 \param scalar The second operand of the binary operation.
 \return The vector result of \f$op(vector, scalar)\f$. */
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size> 
operator + (const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
            const _elementType & scalar)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.SumOf(vector, scalar);
    return result;
}

/* documented above */
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size> 
operator - (const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
            const _elementType & scalar)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.DifferenceOf(vector, scalar);
    return result;
}

/* documented above */
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size> 
operator * (const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
            const _elementType & scalar)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.ProductOf(vector, scalar);
    return result;
}

/* documented above */
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size> 
operator / (const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
            const _elementType & scalar)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.RatioOf(vector, scalar);
    return result;
}
//@}


/*! \name Elementwise operations between a scalar and a fixed size vector. */
//@{
/*! Operation between a scalar and a fixed size vector.
 \param scalar The first operand of the binary operation.
 \param vector The second operand of the binary operation.
 \return The vector result of \f$op(scalar, vector)\f$. */
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size> 
operator + (const _elementType & scalar,
            const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector)

{
    vctFixedSizeVector<_elementType, _size> result;
    result.SumOf(scalar, vector);
    return result;
}

/* documented above */
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size> 
operator - (const _elementType & scalar,
            const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.DifferenceOf(scalar, vector);
    return result;
}

/* documented above */
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size> 
operator * (const _elementType & scalar,
            const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.ProductOf(scalar, vector);
    return result;
}

/* documented above */
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size> 
operator / (const _elementType & scalar,
            const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.RatioOf(scalar, vector);
    return result;
}
//@}


/*! \name Elementwise operations on a fixed size vector. */
//@{
/*! Unary operation on a fixed size vector.
  \param vector The operand of the unary operation
  \return The vector result of \f$op(vector)\f$.
*/
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline vctFixedSizeVector<_elementType, _size> 
operator - (const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector)
{
    vctFixedSizeVector<_elementType, _size> result;
    result.NegationOf(vector);
    return result;
}
//@}


/*! Concatenation operator between vectors.  The operands may be any two fixed-size
  vectors.  The result is a vector of a size equal to the sum of sizes, that 
  contains the concatenated elements.*/
#ifndef SWIG // SWIG 1.3.22 doesn't like _size1 + _size2 for the template "instantiation" of vector
template<unsigned int _size1, int _stride1, class _elementType, class _dataPtr1,
unsigned int _size2, int _stride2, class _dataPtr2>
inline vctFixedSizeVector<_elementType, _size1 + _size2>
operator & (const vctFixedSizeConstVectorBase<_size1, _stride1, _elementType, _dataPtr1> & vector1,
            const vctFixedSizeConstVectorBase<_size2, _stride2, _elementType, _dataPtr2> & vector2)
{
    vctFixedSizeVector<_elementType, _size1+_size2> result;
    vctFixedSizeVectorRef<_elementType, _size1, 1> subvector1(result.Pointer(0));
    vctFixedSizeVectorRef<_elementType, _size2, 1> subvector2(result.Pointer(_size1));
    subvector1.Assign(vector1);
    subvector2.Assign(vector2);

    return result;
}
#endif


/*
  Methods declared previously and implemented here because they require vctFixedSizeVector
*/

#ifndef DOXYGEN

/* documented in class vctFixedSizeConstVectorBase */
template <unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline typename vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::VectorValueType 
vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::Abs(void) const {
    vctFixedSizeVector<_elementType, _size> result;
    vctFixedSizeVectorRecursiveEngines<_size>::template
        VoVi<typename vctUnaryOperations<_elementType>::AbsValue>::
        Unfold(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstVectorBase */
template <unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline typename vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::VectorValueType 
vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::Negation(void) const {
    vctFixedSizeVector<_elementType, _size> result;
    vctFixedSizeVectorRecursiveEngines<_size>::template
        VoVi<typename vctUnaryOperations<_elementType>::Negation>::
        Unfold(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstVectorBase */
template <unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline typename vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::VectorValueType 
vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::Floor(void) const {
    vctFixedSizeVector<_elementType, _size> result;
    vctFixedSizeVectorRecursiveEngines<_size>::template
        VoVi<typename vctUnaryOperations<_elementType>::Floor>::
        Unfold(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstVectorBase */
template <unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline typename vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::VectorValueType 
vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::Ceil(void) const {
    vctFixedSizeVector<_elementType, _size> result;
    vctFixedSizeVectorRecursiveEngines<_size>::template
        VoVi<typename vctUnaryOperations<_elementType>::Ceil>::
        Unfold(result, *this);
    return result;
}

/* documented in class vctFixedSizeConstVectorBase */
template <unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline typename vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::VectorValueType 
vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>::Normalized(void) const throw(std::runtime_error) {
    vctFixedSizeVector<_elementType, _size> result(*this);
    result.NormalizedSelf();
    return result;
}

/* documented in class vctFixedSizeConstVectorBase */
template<unsigned int _size,
         int _stride, class _dataPtrType,
         int __stride, class __dataPtrType,
         class _elementType,
         class _elementOperationType>
inline vctFixedSizeVector<bool, _size>
vctFixedSizeVectorElementwiseCompareVector(const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector1,
                                           const vctFixedSizeConstVectorBase<_size, __stride, _elementType, __dataPtrType> & vector2) {
    vctFixedSizeVector<bool, _size> result;
    vctFixedSizeVectorRecursiveEngines<_size>::template
        VoViVi<_elementOperationType>::Unfold(result, vector1, vector2);
    return result;
}

/* documented in class vctFixedSizeConstVectorBase */
template<unsigned int _size,
         int _stride, class _dataPtrType,
         class _elementType,
         class _elementOperationType>
inline vctFixedSizeVector<bool, _size>
vctFixedSizeVectorElementwiseCompareScalar(const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
                                           const _elementType & scalar) {
    vctFixedSizeVector<bool, _size> result;
    vctFixedSizeVectorRecursiveEngines<_size>::template
        VoViSi<_elementOperationType>::Unfold(result, vector, scalar);
    return result;
}

#endif // DOXYGEN

#endif // _vctFixedSizeVector_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedSizeVector.h,v $
// Revision 1.51  2005/12/23 21:27:31  anton
// cisstVector: Minor updates for Doxygen 1.4.5.
//
// Revision 1.50  2005/12/13 00:04:08  ofri
// All cisstVector containers: Added methods
// 1) MinAndMaxElement  (at const base)
// 2) Floor, Ceil  (at const base -- implemented at container)
// 3) FloorSelf, CeilSelf, FloorOf, CeilOf  (at non-const base)
//
// Revision 1.49  2005/12/02 16:23:54  anton
// cisstVector: Added assigment operator from scalar (see ticket #191).
//
// Revision 1.48  2005/09/27 18:01:25  anton
// cisstVector: Use CMN_ASSERT instead of assert for vectors and matrices.
//
// Revision 1.47  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.46  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.45  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.44  2005/02/03 19:11:35  anton
// cisstVector: Added global function vctCrossProduct() as requested in
// ticket #126.
//
// Revision 1.43  2005/01/11 22:42:18  anton
// cisstVector:  Added normalization methods to the vectors.  Removed useless
// code from vctQuaternion and added some tests.  See ticket #110.
//
// Revision 1.42  2005/01/06 18:54:00  anton
// cisstVector: Introduction of type CopyType as requested in ticket #113.
//
// Revision 1.41  2004/11/18 20:57:30  ofri
// RE: ticket #92.  Redefined all operator= I could find to return a ThisType &
// instead of const ThisType &.  This included corresponding revision of the
// Assign methods, and also definition from scratch of operator= for fixed and
// dynamic matrix ref.
//
// Revision 1.40  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
// Revision 1.39  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.38  2004/10/21 19:27:47  anton
// cisstVector: Added types BoolValueType and ValueType for
// vctFixedSizeVector, vctDynamicVector, vctDynamicMatrix.  This solves #77.
//
// Revision 1.37  2004/10/14 19:18:01  anton
// cisstVector, fixed size containers: Added operator() based on at() method
// as required per ticket #72.
//
// Revision 1.36  2004/09/24 20:20:42  anton
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
// Revision 1.35  2004/09/08 14:13:22  anton
// cisstVector and SWIG: Introduced SIZEMINUSONE as an enum to allow SWIG to
// parse the template parameters specified as expressions (_size - 1).  Also
// wrap ConcatenationOf method for fixed size vector.
//
// Revision 1.34  2004/08/20 20:10:19  anton
// cisstVector: Conditional compilation for SWIG (doesn't deal with operations
// on template parameters).
//
// Revision 1.33  2004/08/13 18:40:54  anton
// cisstVector: added % for the cross product and some basic tests for the
// cross product.  See ticket #23.
//
// Revision 1.32  2004/08/13 17:47:40  anton
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
// Revision 1.31  2004/08/10 14:01:59  ofri
// 1) Defined concatenation operator & which takes to tempalated sequences
// and returns a fixed size vector object (vctFixedSizeVector.h)
// 2) Moved the definition of subsequence types (template class SubsequenceRef)
// from vctFixedSizeVector to the sequence base header files with appropriate
// forward declarations
//
// Revision 1.30  2004/07/27 20:08:06  anton
// cisstVector: Added some Doxygen documentation.  Minor code changes to
// keep the dynamic vectors in sync with the fixed size ones.
//
// Revision 1.29  2004/07/13 22:02:23  ofri
// vctFixedSizeVector: Added non-explicit constructors from sequence-ref with
// same element type.
//
// Revision 1.28  2004/07/12 20:25:48  anton
// cisstVector: Removed redundant #include statements.  These didn't look that
// great on the nice Doxygen graphs.
//
// Revision 1.27  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.26  2004/04/06 15:23:15  anton
// Doxygen clean-up
//
// Revision 1.25  2004/03/29 21:16:41  anton
// #ifdef for SWIG
//
// Revision 1.24  2004/03/19 17:01:17  ofri
// Redefined local TypeTraits using keyword "class" instead of "typename".
// Defined local ElementVaArgPromotion type.
//
// Revision 1.23  2004/03/18 20:41:39  ofri
// Fixing va_arg bug in va_arg constructor: type promotion.
//
// Revision 1.22  2004/03/18 15:40:23  ofri
// Added append constructor: vector<size>(vector<size-1>,value)
//
// Revision 1.21  2004/02/18 15:54:26  anton
// vctFixedSizeVector is pure template, it doesn't require DLL export.  Removed CISST_EXPORT
//
// Revision 1.20  2003/12/24 19:38:44  ofri
// Added constructor to vctFixedSizeVector from a conventional C array
// (pointer)
//
// Revision 1.19  2003/12/18 16:21:41  anton
// Changed Sum to SumOf (idem for difference, ratio, product)
//
// Revision 1.18  2003/11/25 21:37:04  ofri
// Added constructors for vctFixedSizeVector and assignment methods
// to vctFixedLengthSequenceBase, using variable number of arguments
// and value_type pointer
//
// Revision 1.17  2003/11/20 18:27:05  anton
// Added size dependant constructors
//
// Revision 1.16  2003/11/13 19:16:06  anton
// Uncommented operator = to asign from SequenceBase
//
// Revision 1.15  2003/11/11 22:02:34  anton
// Moved Abs() method to base class but returns a vector so implementation is here.   Method Abs(other) is now AbsOf(other).  Idem for Negation()
//
// Revision 1.14  2003/11/10 21:40:06  anton
// Removed #include<iostream> and cout
//
// Revision 1.13  2003/11/04 21:57:00  ofri
// Added return statement to operator= to solve compilation error.
// Note: the operator= need more work as they do nothing in the current version.
//
// Revision 1.12  2003/11/03 21:39:53  ofri
// Changes to vctFixedSizeVector .  These changes were tested succesfully with gcc
// on linux using the cisstVector test library.  However, the version submitted is
// a trial version which needs to be refined.
// 1) Changed the order of template arguments in references to
// vctFixedLengthSequenceBase.
// 2) Generalized the definition of some of the operator global functions to use
// a SequenceBase instead of FixedSizeVector.
// 3) Experimenting with the definitions of operator= . This includes some debug
// messages, which should be removed at a later stage.
//
// Revision 1.11  2003/10/20 20:25:20  anton
// Removed operators * and / between vectors since they are ambiguous
//
// Revision 1.10  2003/10/20 19:40:05  anton
// Tabulations
//
// Revision 1.9  2003/10/20 18:35:19  anton
// Changed FixedSizeVectorOperations and FixedSizeVectorTraits to FixedLengthSequence Operations and Traits
//
// Revision 1.8  2003/10/20 17:21:11  anton
// Moved most methods to vctFixedLengthSequenceBase, added comments for GetSubsequence
//
// Revision 1.7  2003/10/03 19:14:40  anton
// Removed methods [] defined in base class. Cleaned doxygen documentation
//
// Revision 1.6  2003/10/02 14:29:46  anton
// Uncomment all references to SetAll
//
// Revision 1.5  2003/09/30 18:46:25  anton
// removed protected Data since it is now inherited from base class.  Renamed template parameters __stride to _subStride
//
// Revision 1.4  2003/09/30 14:01:28  anton
// now using vctFixedLengthSequence as base class
//
// Revision 1.3  2003/09/17 17:41:56  anton
// added an _ to prefix all the template arguments
//
// Revision 1.2  2003/09/17 14:22:14  anton
// added code for Windows Dll, ie CISST_EXPORT
//
// Revision 1.1  2003/09/09 18:51:46  anton
// creation
//
//
// ****************************************************************************

