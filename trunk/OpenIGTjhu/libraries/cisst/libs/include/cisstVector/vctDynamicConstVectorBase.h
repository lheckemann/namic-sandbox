/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicConstVectorBase.h,v 1.40 2006/07/03 17:31:56 dli Exp $
  
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


#ifndef _vctDynamicConstVectorBase_h
#define _vctDynamicConstVectorBase_h

/*!
  \file
  \brief Declaration of vctDynamicConstVectorBase
*/

#include <iostream>
#include <iomanip>
#include <sstream>

#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstVector/vctContainerTraits.h>
#include <cisstVector/vctDynamicVectorLoopEngines.h>
#include <cisstVector/vctUnaryOperations.h>
#include <cisstVector/vctBinaryOperations.h>
#include <cisstVector/vctForwardDeclarations.h>


/* Forward declarations */
#ifndef DOXYGEN
template<class _vectorOwnerType, class __vectorOwnerType, class _elementType,
         class _elementOperationType>
vctReturnDynamicVector<bool>
vctDynamicVectorElementwiseCompareVector(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector1,
                                         const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector2);

template<class _vectorOwnerType, class _elementType,
         class _elementOperationType>
vctReturnDynamicVector<bool>
vctDynamicVectorElementwiseCompareScalar(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector,
                                         const _elementType & scalar);
#endif // DOXYGEN


/*! 
  This class is templated with the ``vector owner type'', which may
  be a vctDynamicVectorOwner or a vctVectorRefOwner.  It provides
  const operations on the dynamic vector, such as SumOfElements etc.
  
  Vector indexing is zero-based.

  The method provided for the compatibility with the STL containers
  start with a lower case.  Other methods start with a capitalilized
  letter (see CISST naming convention).

  \param _vectorOwnerType the type of vector owner.

  \param _elementType the type of elements of the vector.
*/
template<class _vectorOwnerType, typename _elementType>
class vctDynamicConstVectorBase
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! Type of the vector itself. */
    typedef vctDynamicConstVectorBase<_vectorOwnerType, _elementType> ThisType;

    /*! Type of the data owner (dynamic array or pointer) */
    typedef _vectorOwnerType VectorOwnerType;

    /*! Iterator on the elements of the vector. */   
    typedef typename VectorOwnerType::iterator iterator;

    /*! Const iterator on the elements of the vector. */    
    typedef typename VectorOwnerType::const_iterator const_iterator; 

    /*! Reverse iterator on the elements of the vector. */   
    typedef typename VectorOwnerType::reverse_iterator reverse_iterator;

    /*! Const reverse iterator on the elements of the vector. */   
    typedef typename VectorOwnerType::const_reverse_iterator const_reverse_iterator; 

    /*! The type used to create a copy. */
    typedef vctDynamicVector<_elementType> CopyType;

    /*! The type of a vector returned by value from operations on this object */
    typedef vctReturnDynamicVector<_elementType> VectorValueType;

    typedef cmnTypeTraits<value_type> TypeTraits;

    /*! Define bool based on the container type to force some
      compilers (i.e. gcc 4.0) to delay the instantiation of the
      ElementWiseCompare methods. */
    typedef typename TypeTraits::BoolType BoolType;

    /*! The type of a vector of booleans returned from operations on this object, e.g.,
      ElementwiseEqual. */
    typedef vctReturnDynamicVector<BoolType> BoolVectorValueType;


protected:
    /*! Declaration of the vector-defining member object */
    VectorOwnerType Vector;


    /*! Check the validity of an index. */
    inline void ThrowUnlessValidIndex(size_type index) const throw(std::out_of_range) {
        if (! ValidIndex(index)) {
            cmnThrow(std::out_of_range("vctDynamicVector: Invalid index"));
        }
    }

public:
    /*! Returns a const iterator on the first element (STL
      compatibility). */
    const_iterator begin(void) const {
        return Vector.begin(); 
    }

    /*! Returns a const iterator on the last element (STL
      compatibility). */
    const_iterator end(void) const {
        return Vector.end(); 
    }

    /*! Returns a reverse const iterator on the last element (STL
      compatibility). */ 
    const_reverse_iterator rbegin(void) const {
        return Vector.rbegin(); 
    }

    /*! Returns a reverse const iterator on the element before first
      (STL compatibility). */ 
    const_reverse_iterator rend(void) const {
        return Vector.rend(); 
    }

    /*! Return the number of elements in the vector.  This is not
      equivalent to the difference between the end and the beginning.
    */
    size_type size(void) const {
        return Vector.size();
    }

    /*! Not required by STL but provided for completeness */
    difference_type stride() const {
        return Vector.stride();
    }

    /*! Tell is the vector is empty (STL compatibility).  False unless
      size is zero. */
    bool empty() const {
        return (size() == 0);
    }

    /*! Access an element by index (const).
      \return a const reference to the element[index] */
    const_reference operator[](index_type index) const {
        return *Pointer(index);
    }

    /*! Access an element by index (const).  Compare with
      std::vector::at().  This method can be a handy substitute for
      the overloaded operator [] when operator overloading is
      unavailable or inconvenient.  \return a const reference to
      element[index] */
    const_reference at(size_type index) const throw(std::out_of_range) {
        ThrowUnlessValidIndex(index);
        return *(Pointer(index));
    }

    /*! Overloaded operator () for simplified (const) element access with bounds checking */
    inline const_reference operator() (size_type index) const throw(std::out_of_range) {
        return this->at(index);
    }

    
    /*! Access an element by index (const). This method allows
      to access an element without any bounds checking.

      \return a reference to the element at index */
    const_reference Element(size_type index) const {
        return *(Pointer(index));
    }


    /*! Returns a const pointer to an element of the container,
      specified by its index. Addition to the STL requirements.
    */
    const_pointer Pointer(index_type index = 0) const {
        return Vector.Pointer(index);
    }


    /*! Returns true if this index is valid. */
    inline bool ValidIndex(size_type index) const {
        return ((index >= 0)
                && (index < size()));
    }


    /*! \name Size dependant methods.
      
    The following methods are size dependant, i.e. don't necessarily
    mean anything for all sizes of vector.  For example, using the Z()
    method on a vector of size 2 shouldn't be allowed.  Therefore, we
    are using #CMN_ASSERT to check that the size is valid. */
    
    //@{

    /*! Returns the first element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 1. */
    const value_type & X(void) const {
        CMN_ASSERT(this->size() > 0);
        return *(Pointer(0));
    }


    /*! Returns the second element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 2. */
    const value_type & Y(void) const {
        CMN_ASSERT(this->size() > 1);
        return *(Pointer(1));
    }


    /*! Returns the third element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 3. */
    const value_type & Z(void) const {
        CMN_ASSERT(this->size() > 2);
        return *(Pointer(2));
    }

    /*! Returns the fourth element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 4. */
    const value_type & W(void) const {
        CMN_ASSERT(this->size() > 3);
        return *(Pointer(3));
    }

    /*! Return a (const) vector reference for the first two elements of this
      vector.  May be used when switching from homogeneous coordinates to normal.
      (shadowed from base class).
    */
    vctDynamicConstVectorRef<_elementType> XY(void) const {
        CMN_ASSERT(this->size() > 1);
        return vctDynamicConstVectorRef<_elementType>(2, Pointer(0), this->stride());
    }

    /*! Return a (const) vector reference for the first and third elements of this
      vector. */
    vctDynamicConstVectorRef<_elementType> XZ(void) const {
        CMN_ASSERT(this->size() > 2);
        return vctDynamicConstVectorRef<_elementType>(2, Pointer(0), 2 * this->stride());
    }

    /*! Return a (const) vector reference for the first and fourth elements of this
      vector. */
    vctDynamicConstVectorRef<_elementType> XW(void) const {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicConstVectorRef<_elementType>(2, Pointer(0), 3 * this->stride());
    }

    /*! Return a (const) vector reference for the two elements of this
      vector beginning on the second. */
    vctDynamicConstVectorRef<_elementType> YZ(void) const {
        CMN_ASSERT(this->size() > 2);
        return vctDynamicConstVectorRef<_elementType>(2, Pointer(1), this->stride());
    }

    /*! Return a (const) vector reference for the two elements of this
      vector beginning on the second. */
    vctDynamicConstVectorRef<_elementType> YW(void) const {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicConstVectorRef<_elementType>(2, Pointer(1), 2 * this->stride());
    }

    /*! Return a (const) vector reference for the two elements of this
      vector beginning on the third. */
    vctDynamicConstVectorRef<_elementType> ZW(void) const {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicConstVectorRef<_elementType>(2, Pointer(2), this->stride());
    }

    /*! Return a (const) vector reference for the first three elements of this
      vector.  May be used when switching from homogeneous coordinates to normal,
      or when refering to a subvector of a row or a column inside a small matrix.
      (shadowed from base class).
    */
    vctDynamicConstVectorRef<_elementType> XYZ(void) const {
        CMN_ASSERT(this->size() > 2);
        return vctDynamicConstVectorRef<_elementType>(3, Pointer(0), this->stride());
    }

    /*! Return a (const) vector reference for the second, third and fourth elements
      of this vector. */
    vctDynamicConstVectorRef<_elementType> YZW(void) const {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicConstVectorRef<_elementType>(3, Pointer(1),  this->stride());
    }

    /*! Return a (const) vector reference for the first four elements of this
      vector.  May be used when switching from homogeneous coordinates to normal,
      or when refering to a subvector of a row or a column inside a small matrix.
      (shadowed from base class).
    */
    vctDynamicConstVectorRef<_elementType> XYZW(void) const {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicConstVectorRef<_elementType>(4, Pointer(0), this->stride());
    }

    //@}



    /*! \name Incremental operations returning a scalar.
      Compute a scalar from all the elements of the vector. */
    //@{

    /*! Return the sum of the elements of the vector.
      \return The sum of all the elements */
    inline value_type SumOfElements(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    } 

    /*! Return the product of the elements of the vector.
      \return The product of all the elements */
    inline value_type ProductOfElements(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Multiplication,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the square of the norm  of the vector.
      \return The square of the norm */
    inline value_type NormSquare(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Square>::
            Run(*this);
    }

    /*! Return the norm of the vector.
      \return The norm. */
    inline NormType Norm(void) const {
        return sqrt(NormType(NormSquare()));
    }

    /*! Test if this vector is normalized.  This method will use the default
      tolerance defined in cmnTypeTraits as the maximum difference between the
      norm and 1 to consider the vector normalized.
    */
    inline bool IsNormalized(void) const {
        if (vctUnaryOperations<_elementType>::AbsValue::Operate(_elementType(Norm() - 1)) < TypeTraits::Tolerance()) {
            return true;
        } else {
            return false;
        }
    }

    /*! Test if this vector is normalized.  This method uses the
      tolerance provided by the user.

      \param tolerance The maximum difference between the norm and 1
      to consider the vector normalized.

      \note This method has been overloaded instead of using a default
      value for the tolerance parameter to allow the use of
      vctDynamicVector with types for which don't have a tolerance
      defined via cmnTypeTraits.  Otherwise, some compilers will try
      to expand the signature and will not be able to set the default
      tolerance.
    */
    inline bool IsNormalized(_elementType tolerance) const {
        if (vctUnaryOperations<_elementType>::AbsValue::Operate(_elementType(Norm() - 1)) < tolerance) {
            return true;
        } else {
            return false;
        }
    }

    /*! Return the L1 norm of the vector, i.e. the sum of the absolute
      values of all the elements.
 
      \return The L1 norm. */
    inline value_type L1Norm(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the Linf norm of the vector, i.e. the maximum of the absolute
      values of all the elements.
 
      \return The Linf norm. */
    inline value_type LinfNorm(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this);
    }

    /*! Return the maximum element of the vector.
      \return The maximum element */
    inline value_type MaxElement(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Return the minimum element of the vector.
      \return The minimum element */
    inline value_type MinElement(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<value_type>::Minimum,
            typename vctUnaryOperations<value_type>::Identity>::
            Run(*this);
    }

    /*! Compute the minimum AND maximum elements of the vector.
      This method is more runtime-efficient than computing them
      separately.
      \param minElement reference to store the minimum element result.
      \param maxElement reference to store the maximum element result.
      \note If this vector is empty (null pointer) the result is undefined.
    */
    inline void MinAndMaxElement(value_type & minElement, value_type & maxElement) const
    {
        vctDynamicVectorLoopEngines::MinAndMax::Run((*this), minElement, maxElement);
    }

    /*! Return true if all the elements of this vector are strictly positive, 
      false otherwise */
    inline bool IsPositive(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are non-negative, 
      false otherwise */
    inline bool IsNonNegative(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are non-positive, 
      false otherwise */
    inline bool IsNonPositive(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonPositive>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are strictly negative, 
      false otherwise */
    inline bool IsNegative (void) const {
        return vctDynamicVectorLoopEngines::
            SoVi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNegative>::
            Run(*this);
    }

    /*! Return true if all the elements of this vector are nonzero,
      false otherwise */
    inline bool All(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    /*! Return true if any element of this vector is nonzero, false
      otherwise */
    inline bool Any(void) const {
        return vctDynamicVectorLoopEngines::
            SoVi< typename vctBinaryOperations<bool>::Or,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Run(*this);
    }

    //@}


    /*! \name Storage format. */
    //@{

    /*! Test if the vector is compact, i.e. a vector of length l
      actually uses a contiguous block of memory or size l.  A compact
      vector has a stride equal to 1. */
    inline bool IsCompact(void) const {
        return (this->stride() == 1);
    }

    /*! Test if the vector is "Fortran" compatible, i.e. is compact.
      \sa IsCompact
    */
    inline bool IsFortran(void) const {
        return this->IsCompact();
    }
    //@}


    /*! Dot product with another vector <em>of the same type and size</em>
     
    \param otherVector second operand of the dot product ("this" is the first operand)
    \return The dot product of this and otherVector.
    */
    template <class __vectorOwnerType>
    inline value_type DotProduct(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication>::
            Run(*this, otherVector);
    }


    /*! \name Elementwise comparisons between vectors.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between two vectors of the same size, containing
      the same type of elements.  The stride can be different.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for Equal(),
      NotEqual(), Lesser(), LesserOrEqual(), Greater() or
      GreaterOrEqual() is performed elementwise between the two
      vectors.  A logical "and" is performed (except for NotEqual
      which uses a logical "or") to accumulate the elementwise
      results.  The only operators provided are "==" and "!=" since
      the semantic is not ambiguous.

      \return A boolean.
    */
    template <class __vectorOwnerType>
    inline bool Equal(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool operator == (const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return Equal(otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool AlmostEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector,
          value_type tolerance) const {
        return ((*this - otherVector).LinfNorm() <= tolerance);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool AlmostEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return ((*this - otherVector).LinfNorm() <= TypeTraits::Tolerance());
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool NotEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool operator != (const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return NotEqual(otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool Lesser(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool LesserOrEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, otherVector);
    }

    /* documented above */
    template <class __vectorOwnerType>
    inline bool Greater(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, otherVector);
    }
    
    /* documented above */
    template <class __vectorOwnerType>
    inline bool GreaterOrEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorLoopEngines::
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Run(*this, otherVector);
    }
    //@}


    /*! \name Elementwise comparisons between vectors.
      Returns the vector of comparison's results. */
    //@{
    /*! Comparison between two vectors of the same size, containing
      the same type of elements.  The strides and the internal
      representation (_vectorOwnerType) can be different.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for
      ElementwiseEqual(), ElementwiseNotEqual(), ElementwiseLesser(),
      ElementwiseLesserOrEqual(), ElementwiseGreater() or
      ElementwiseGreaterOrEqual() is performed elementwise between the
      two vectors and stored in a newly created vector.  There is no
      operator provided since the semantic would be ambiguous.
      
      \return A vector of booleans.
    */
    template<class __vectorOwnerType>
    BoolVectorValueType
    ElementwiseEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorElementwiseCompareVector<
            _vectorOwnerType, __vectorOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, otherVector);
    }


    /* documented above */
    template<class __vectorOwnerType>
    BoolVectorValueType
    ElementwiseNotEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorElementwiseCompareVector<
            _vectorOwnerType, __vectorOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, otherVector);
    }
    
    /* documented above */
    template<class __vectorOwnerType>
    BoolVectorValueType
    ElementwiseLesser(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorElementwiseCompareVector<
            _vectorOwnerType, __vectorOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, otherVector);
    }

    /* documented above */
    template<class __vectorOwnerType>
    BoolVectorValueType
    ElementwiseLesserOrEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorElementwiseCompareVector<
            _vectorOwnerType, __vectorOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, otherVector);
    }

    /* documented above */
    template<class __vectorOwnerType>
    BoolVectorValueType
    ElementwiseGreater(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorElementwiseCompareVector<
            _vectorOwnerType, __vectorOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, otherVector);
    }

    /* documented above */
    template<class __vectorOwnerType>
    BoolVectorValueType
    ElementwiseGreaterOrEqual(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) const {
        return vctDynamicVectorElementwiseCompareVector<
            _vectorOwnerType, __vectorOwnerType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>(*this, otherVector);
    }
    //@}


    /*! \name Elementwise comparisons between a vector and a scalar.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between a vector and a scalar.  The type of the
      elements of the vector and the scalar must be the same.  The
      comparaison (\f$ = \neq < \leq > \geq \f$) for Equal(),
      NotEqual(), Lesser(), LesserOrEqual(), Greater() or
      GreaterOrEqual() is performed elementwise between the vector and
      the scalar.  A logical "and" is performed (except for NotEqual
      which uses a logical "or") to accumulate the elementwise
      results..  The only operators provided are "==" and "!=" since
      the semantic is not ambiguous.

      \return A boolean.
    */
    inline bool Equal(const value_type & scalar) const {
        return vctDynamicVectorLoopEngines::
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Run(*this, scalar);
    }
    
    /* documented above */
    inline bool operator == (const value_type & scalar) const {
        return Equal(scalar);
    }

    /* documented above */
    inline bool NotEqual(const value_type & scalar) const {
        return vctDynamicVectorLoopEngines::
            SoViSi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool operator != (const value_type & scalar) const {
        return NotEqual(scalar);
    }

    /* documented above */
    inline bool Lesser(const value_type & scalar) const {
        return vctDynamicVectorLoopEngines::
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool LesserOrEqual(const value_type & scalar) const {
        return vctDynamicVectorLoopEngines::
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool Greater(const value_type & scalar) const {
        return vctDynamicVectorLoopEngines::
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Run(*this, scalar);
    }

    /* documented above */
    inline bool GreaterOrEqual(const value_type & scalar) const {
        return vctDynamicVectorLoopEngines::
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Run(*this, scalar);
    }
    //@}

    /*! \name Elementwise comparisons between a vector and a scalar.
      Returns the vector of comparison's results. */
    //@{
    /*! Comparison between a vector and a scalar, containing the same
      type of elements.  The comparaison (\f$ = \neq < \leq > \geq
      \f$) for ElementwiseEqual(), ElementwiseNotEqual(),
      ElementwiseLesser(), ElementwiseLesserOrEqual(),
      ElementwiseGreater() or ElementwiseGreaterOrEqual() is performed
      elementwise between the vector and the scalar and stored in a
      newly created vector.  There is no operator provided since the
      semantic would be ambiguous.
      
      \return A vector of booleans.
    */
    BoolVectorValueType ElementwiseEqual(const value_type & scalar) const;
    
    /* documented above */
    BoolVectorValueType ElementwiseNotEqual(const value_type & scalar) const;
    
    /* documented above */
    BoolVectorValueType ElementwiseLesser(const value_type & scalar) const;

    /* documented above */
    BoolVectorValueType ElementwiseLesserOrEqual(const value_type & scalar) const;

    /* documented above */
    BoolVectorValueType ElementwiseGreater(const value_type & scalar) const;

    /* documented above */
    BoolVectorValueType ElementwiseGreaterOrEqual(const value_type & scalar) const;
    //@}

    /*! Define a ConstSubvector class for compatibility with the fixed size vectors.
      A const subvector has the same stride as the parent container.

      Example:

      typedef vctDynamicVector<double> VectorType;
      VectorType v(9);
      VectorType::ConstSubvector::Type first4(v, 0, 4);
      VectorType::ConstSubvector::Type last5(v, 4, 5);

      \note There is no straightforward way to define a fixed-size
      subvector of a dynamic vector, because the stride of the dynamic
      vector is not known in compilation time.  A way to do it is:

      vctFixedSizeConstVectorRef<double, 3 /( add stride here if necessary )/> firstThree(v, 0);
      vctFixedSizeConstVectorRef<double, 3 /( add stride here if necessary )/> lastThree(v, 6);
    */
#ifndef SWIG
    class ConstSubvector
    {
    public:
        typedef vctDynamicConstVectorRef<value_type> Type;
    };
#endif // SWIG

    /*! \name Unary elementwise operations.
      Returns the result of vector.op(). */
    //@{
    /*! Unary elementwise operations on a vector.  For each element of
      the vector "this", performs \f$ this[i] \leftarrow
      op(otherVector[i])\f$ where \f$op\f$ can calculate the absolute
      value (Abs), the opposite (Negation) or the normalized version
      (Normalized).
      
      \return A new vector.
    */
    inline VectorValueType Abs(void) const;

    /* documented above */
    inline VectorValueType Negation(void) const;

    /* documented above */
    inline VectorValueType Floor(void) const;

    /* documented above */
    inline VectorValueType Ceil(void) const;

    /* documented above */
    inline VectorValueType Normalized(void) const throw(std::runtime_error);
    //@}


    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
    
    void ToStream(std::ostream & outputStream) const
    {
        size_type index;
        const size_type mySize = size();
        // preserve the formatting flags as they were
        const int width = outputStream.width(12);
        const int precision = outputStream.precision(6);
        bool showpoint = ((outputStream.flags() & std::ios_base::showpoint) != 0);
        outputStream << std::setprecision(6) << std::showpoint;
        for (index = 0; index < mySize; ++index) {
            outputStream << std::setw(12) << (*this)[index]; 
            if (index < (mySize-1)) {
                outputStream << " "; 
            }
        }
        // resume the formatting flags
        outputStream << std::setprecision(precision) << std::setw(width);
        if (!showpoint) {
            outputStream << std::noshowpoint;
        }
    }
};



/* documented in class.  Implementation moved here for .Net 2003 */
template<class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::BoolVectorValueType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::ElementwiseEqual(const value_type & scalar) const {
    return vctDynamicVectorElementwiseCompareScalar<_vectorOwnerType,  value_type,
        typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template<class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::BoolVectorValueType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::ElementwiseNotEqual(const value_type & scalar) const {
    return vctDynamicVectorElementwiseCompareScalar<_vectorOwnerType,  value_type,
        typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template<class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::BoolVectorValueType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::ElementwiseLesser(const value_type & scalar) const {
    return vctDynamicVectorElementwiseCompareScalar<_vectorOwnerType,  value_type,
        typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template<class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::BoolVectorValueType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::ElementwiseLesserOrEqual(const value_type & scalar) const {
    return vctDynamicVectorElementwiseCompareScalar<_vectorOwnerType,  value_type,
        typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template<class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::BoolVectorValueType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::ElementwiseGreater(const value_type & scalar) const {
    return vctDynamicVectorElementwiseCompareScalar<_vectorOwnerType,  value_type,
        typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, scalar);
}

/* documented in class.  Implementation moved here for .Net 2003 */
template<class _vectorOwnerType, class _elementType>
inline typename vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::BoolVectorValueType
vctDynamicConstVectorBase<_vectorOwnerType, _elementType>::ElementwiseGreaterOrEqual(const value_type & scalar) const {
    return vctDynamicVectorElementwiseCompareScalar<_vectorOwnerType,  value_type,
        typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>(*this, scalar);
}


/*!
  Dot product of two vectors <em>of the same type and size</em>.
  
  \param vector1 first operand of the dot product.
  \param vector2 second operand of the dot product.
  \return The dot product of vector1 and vector2.
*/
template <class _vector1OwnerType, class _vector2OwnerType, class _elementType>
inline _elementType vctDotProduct(const vctDynamicConstVectorBase<_vector1OwnerType, _elementType> & vector1,
                                  const vctDynamicConstVectorBase<_vector2OwnerType, _elementType> & vector2) {
    return vector1.DotProduct(vector2);
}

/*!
  Dot product of two vectors <em>of the same type and size</em>.
  
  \param vector1 first operand of the dot product.
  \param vector2 second operand of the dot product.
  \return The dot product of vector1 and vector2.
*/
template <class _vector1OwnerType, class _vector2OwnerType, class _elementType>
inline _elementType operator * (const vctDynamicConstVectorBase<_vector1OwnerType, _elementType> & vector1,
                                const vctDynamicConstVectorBase<_vector2OwnerType, _elementType> & vector2) {
    return vector1.DotProduct(vector2);
}

/*! Return true if all the elements of the vector are nonzero, false otherwise */
template<class _vectorOwnerType, typename _elementType>
inline bool vctAll(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector) {
    return vector.All();
}

/*! Return true if any element of the vector is nonzero, false otherwise */
template<class _vectorOwnerType, typename _elementType>
inline bool vctAny(const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector) {
    return vector.Any();
}

/*! Stream out operator. */
template<class _vectorOwnerType, typename _elementType>
std::ostream & operator << (std::ostream & output,
                            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector) {
    vector.ToStream(output);
    return output;
}

#ifndef DOXYGEN
template<unsigned int __size, class _vectorOwnerType, class __elementType>
std::ostream & operator << (std::ostream & output,
                            const vctDynamicConstVectorBase< _vectorOwnerType, vctFixedSizeVector<__elementType, __size> > & vector)
{
    int numElements = vector.size();
    int counter;
    for (counter = 0; counter < numElements; ++counter) {
        output << "[ ";
        vector[counter].ToStream(output);
        output << "]\n";
    }
    return output;
}
#endif // DOXYGEN


#endif // _vctDynamicConstVectorBase_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicConstVectorBase.h,v $
//  Revision 1.40  2006/07/03 17:31:56  dli
//  Added ProductOfElements function
//
//  Revision 1.39  2006/02/12 18:32:38  ofri
//  vctDynaic[Const]VectorBase : Extended named subvector methods YZ(), YZW(),
//  XZ(), XW(), ZW() etc.
//
//  Revision 1.38  2005/12/23 21:27:31  anton
//  cisstVector: Minor updates for Doxygen 1.4.5.
//
//  Revision 1.37  2005/12/21 21:43:42  anton
//  cisstVector: Added IsCompact() for vectors.  IsFortran() has been added as
//  well to match the API of matrices.  Most Fortran based routines should then
//  use IsFortran() on both vectors and matrices.  Routines which can be used
//  with any storage order should at least use IsCompact() on the vectors and
//  matrices.
//
//  Revision 1.36  2005/12/13 00:05:05  ofri
//  All cisstVector containers: Added methods
//  1) MinAndMaxElement  (at const base)
//  2) Floor, Ceil  (at const base -- implemented at container)
//  3) FloorSelf, CeilSelf, FloorOf, CeilOf  (at non-const base)
//
//  Revision 1.35  2005/10/06 16:56:37  anton
//  Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
//  Revision 1.34  2005/09/27 18:01:25  anton
//  cisstVector: Use CMN_ASSERT instead of assert for vectors and matrices.
//
//  Revision 1.33  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.32  2005/09/24 00:01:04  anton
//  cisstVector: Use cmnThrow for all exceptions.
//
//  Revision 1.31  2005/09/07 03:38:14  anton
//  cisstVector: Added BoolType to cmnTypeTraits in order to delay the
//  instantiation of the ElementWiseCompare methods for
//  vctDynamic{Vector,Matrix}ConstBase.  Credit goes to Ofri for this solution.
//  See #162.
//
//  Revision 1.30  2005/08/11 21:29:44  ofri
//  cisstVector -- base classes ToStream(): Added code to preserve the previous
//  state of the stream (precision, width, showpoint) and resume it before returning
//  from the method.
//
//  Revision 1.29  2005/08/04 15:31:57  anton
//  cisstVector: Replaced RangeCheck() by ThrowUnlessValidIndex().  For dynamic
//  matrices, Row() and Col() now check the index and throw using
//  ThrowUnlessValid{Row,Col}Index().
//
//  Revision 1.28  2005/06/16 03:38:29  anton
//  Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
//  VC 7.1 (.Net 2003).
//
//  Revision 1.27  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.26  2005/04/28 03:55:23  anton
//  cisstVector:  Added AlmostEqual() to all containers to compare with
//  tolerance.
//
//  Revision 1.25  2005/04/04 19:48:34  anton
//  cisstVector: In base types for dynamic containers, removed default value for
//  _elementType (was based on _ownerType::value_type) to -1- avoid issues with
//  SWIG and -2- force explicit declaration of _elementType whenever a base type
//  is used.
//
//  Revision 1.24  2005/03/31 15:51:02  anton
//  cisstVector: Found a couple of _size, replaced by size().
//
//  Revision 1.23  2005/03/01 21:53:41  kapoor
//  vctDynamicConstVectorBase: Created two IsNormalized() methods to avoid
//  a signature with cmnTypeTraits::Tolerance since this tolerance is only
//  defined for basic types and we want to allow the use of
//  vctDynamicVector<vctDouble3> (for example :-))
//
//  Revision 1.22  2005/01/11 22:42:18  anton
//  cisstVector:  Added normalization methods to the vectors.  Removed useless
//  code from vctQuaternion and added some tests.  See ticket #110.
//
//  Revision 1.21  2005/01/06 18:54:00  anton
//  cisstVector: Introduction of type CopyType as requested in ticket #113.
//
//  Revision 1.20  2005/01/05 18:29:49  anton
//  cisstVector: Added ValidIndex() methods for all containers (and tests) as
//  requested in ticket #111.
//
//  Revision 1.19  2004/12/10 21:59:07  ofri
//  vct[FixedSize|Dynamic](Const)VectorBase: Added methods XY(), XYZ(), XYZW()
//  to access subvectors easily.  These are useful when handling barycentric
//  coordinates.
//
//  Revision 1.18  2004/12/01 21:51:48  anton
//  cisstVector: Use the NeutralElement() of the incremental operations for
//  the dynamic container engines (see #97).  Also rewrote the vector engines
//  to use a single index with [] instead of multiple interators.
//
//  Revision 1.17  2004/11/30 23:36:05  anton
//  cisstVector: Added the Element() method to access an element without range
//  check.  See ticket #79.
//
//  Revision 1.16  2004/11/11 20:35:46  anton
//  cisstVector: *: Added a vctContainerTraits to centralize the declarations
//  of size_type, difference_type, reference, const_reference, etc. *: All
//  iterators are now derived from std::iterator. *: Added some missing typedef
//  for iterators.
//
//  Revision 1.15  2004/11/01 16:57:06  ofri
//  Added the method W() to access the fourth element of a vector
//
//  Revision 1.14  2004/10/26 15:20:52  ofri
//  Updating subsequence interfaces (ticket #76) and interfaces between fixed
//  and dynamic vectors/matrices (ticket #72).
//  *: vctFixedSize(Const)VectorBase::Get(Const)Subsequence now deprecated.
//  Replaced by Get(Const)Subvector with equal stride to the parent.
//  *: Added constructors for [Fixed|Dynamic][Vector|Matrix]Ref from the other
//  storage type.
//
//  Revision 1.13  2004/10/25 19:19:54  anton
//  cisstVector:  Created vctForwardDeclarations.h and removed forward
//  declarations of classes in all other files.  Added some constructors
//  for vector references to reference fixed size from dynamic and vice versa.
//
//  Revision 1.12  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.11  2004/10/21 19:27:47  anton
//  cisstVector: Added types BoolValueType and ValueType for
//  vctFixedSizeVector, vctDynamicVector, vctDynamicMatrix.  This solves #77.
//
//  Revision 1.10  2004/10/14 21:24:43  ofri
//  vctDynamicConstVectorBase: Added operator << (ostream) for the special case
//  of a dynamic container for fixed-size vectors (see ticket #72).  I decided that
//  for now I will not deal with a dynamic container of vctFixedSizeConstVectorBase,
//  as I don't think we're likely to have something other than a vctFixedSizeVector.
//  As for the API, I tried to implement it in the form of ToStream() and had some
//  difficulties.  I prefered to have this implementation for now.
//
//  Revision 1.9  2004/10/14 18:57:15  ofri
//  vctDynamicConstVectorBase and vctDynamicVectorBase: Added operator ().
//  See ticket #72.
//
//  Revision 1.8  2004/10/08 19:38:58  anton
//  cisstVector: Added a space between elements in ToStream methods.
//
//  Revision 1.7  2004/09/30 15:49:51  anton
//  cisstVector: solution for ticket #65 regarding a compilation error with
//  .Net 2003.  The issue remains mysterious but but a solution is to declare
//  the problematic methods in the class declaration and implement them right
//  after (in the same file).  The methods in question are the Elementwise
//  comparaison with a scalar (it works if comparing with a vector/matrix).
//
//  Revision 1.6  2004/09/28 18:32:36  ofri
//  Moved the definitions of elementwise comparison operations between dynamic
//  vector and scalar from vctDynamicConstVectorBase to vctDynamicVector.
//  Changed the return type from vctDynamicVector to vctReturnDynamicVector.
//  The result gets compiled successfully on gcc, .net7, .net2003.  Tests executed
//  successfully on gcc.  See ticket #65.
//
//  Revision 1.5  2004/09/24 20:20:42  anton
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
//  Revision 1.4  2004/09/21 20:10:12  anton
//  cisstVector:  for dynamic vectors, added size dependant methods (X(), Y(),
//  Z() and CrossProductOf()) as well as ProductOf(matrix, vector) and
//  ProductOf(vector, matrix).
//
//  Revision 1.3  2004/09/03 19:56:36  anton
//  Doxygen documentation.
//
//  Revision 1.2  2004/08/13 21:40:40  anton
//  cisstVector: Added a bound check for all vectors (fixed size and dynamic)
//  as requested in ticket #13.  Added protect RangeCheck method to facilitate
//  (inspired from the STL distributed with gcc).  Added some tests in
//  vctGenericVectorTests.  Still have to do the equivalent for matrices.
//
//  Revision 1.1  2004/08/13 17:47:40  anton
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
//  Revision 1.13  2004/08/12 20:23:52  anton
//  cisstVector: Attempt to solve an overloaded function conflict with .NET 2003
//  by casting the input parameter of sqrt to {quote: NormType}
//
//  Revision 1.12  2004/08/11 19:21:32  anton
//  cisstVector: the operator << and the methods ToString and ToStream have
//  been improved.
//
//  Revision 1.11  2004/08/04 21:02:30  anton
//  cisstVector: Methods {quote: ToString} now relies on the stream out
//  operator (<<).
//
//  Revision 1.10  2004/07/27 20:08:06  anton
//  cisstVector: Added some Doxygen documentation.  Minor code changes to
//  keep the dynamic vectors in sync with the fixed size ones.
//
//  Revision 1.9  2004/07/23 19:28:32  anton
//  vctDynamicVector: Added the dot product function and operator as well
//  as the tests in vctGenericSequenceTest
//
//  Revision 1.8  2004/07/23 18:02:35  anton
//  vctDynamic vector and sequences:
//  *: Added comparison operators (==, !=, <=, etc) for sequences and scalars
//  *: Added Abs, Negation, AbsOf, NegationOf, AbsSelf, NegationSelf methods
//  *: Added Add, Subtract, Multiply, Divide for scalars as well as
//     +=, -=, /=, *= for vector and scalar
//  *: Added global operators +, -, *, / for vector/vector, vector/scalar,
//     scalar/vector (whenever it was pertinent)
//  *: Added loop engined {quote: Vio} and {quote: SoViSi}
//
//  Revision 1.7  2004/07/13 16:24:24  ofri
//  Updating vctDynamicConstSequenceBase: 1) now templated by _elementType
//  as well as _sequenceOwnerType to enforce type compatibility during compilation.
//  2) Substituted elaborate test for HasInfinity with a unified function call on
//  cmnTypeTraits.  3) Function declarations now enforce type compatibility
//
//  Revision 1.6  2004/07/12 20:29:49  anton
//  vctDynamicConstSequenceBase: Added methods relying on engines SoVi & SoViVi
//
//  Revision 1.5  2004/07/06 19:42:50  ofri
//  Giving the dynamic vector classes "standard look and feel" in terms of type
//  members, constructors, and indentations.  I made sure that the corresponding
//  example compiles and runs successfully.
//
//  Revision 1.4  2004/07/06 17:47:11  anton
//  Dynamic Sequences:
//  *: Compilation error re. const and const_iterator
//  *: Added some iterator methods to sequences
//  *: Compilation errors re. the order (size, stride, data) in constructor.  The order is now (size, data, stride = 1) everywhere to be consistant
//  *: The method Resize is now resize (STL spelling)
//
//  Revision 1.3  2004/07/02 20:14:07  anton
//  Code for VarStrideSequenceIterator.  Added the class as well as some code to
//  try it.  An example is provided in examples/vectorTutorial/main.cpp.
//
//  Revision 1.2  2004/07/02 18:18:31  anton
//  Introduced an index_type which might have to be moved to some kind of traits
//
//  Revision 1.1  2004/07/02 16:16:45  anton
//  Massive renaming in cisstVector for the addition of dynamic size vectors
//  and matrices.  The dynamic vectors are far from ready.
//
//
// ****************************************************************************
