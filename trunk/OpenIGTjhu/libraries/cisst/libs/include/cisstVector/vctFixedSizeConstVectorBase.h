/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeConstVectorBase.h,v 1.35 2006/07/03 17:31:56 dli Exp $

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
  \brief Declaration of vctFixedSizeConstVectorBase
 */

#ifndef _vctFixedSizeConstVectorBase_h
#define _vctFixedSizeConstVectorBase_h

#include <iostream>
#include <iomanip>
#include <sstream>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstCommon/cmnTypeTraits.h>

#include <cisstVector/vctFixedSizeVectorTraits.h>
#include <cisstVector/vctBinaryOperations.h>
#include <cisstVector/vctUnaryOperations.h>
#include <cisstVector/vctStoreBackBinaryOperations.h>
#include <cisstVector/vctStoreBackUnaryOperations.h>
#include <cisstVector/vctFixedSizeVectorRecursiveEngines.h>
#include <cisstVector/vctForwardDeclarations.h>

/* Forward declarations */
#ifndef DOXYGEN
template<unsigned int _size,
         int _stride, class _dataPtrType,
         int __stride, class __dataPtrType,
         class _elementType,
         class _elementOperationType>
inline vctFixedSizeVector<bool, _size>
vctFixedSizeVectorElementwiseCompareVector(const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector1,
                                           const vctFixedSizeConstVectorBase<_size, __stride, _elementType, __dataPtrType> & vector2);

template<unsigned int _size,
         int _stride, class _dataPtrType,
         class _elementType,
         class _elementOperationType>
inline vctFixedSizeVector<bool, _size>
vctFixedSizeVectorElementwiseCompareScalar(const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector,
                                           const _elementType & scalar);
#endif // DOXYGEN


/*!
  \brief A template for a fixed length vector with fixed spacing in
  memory.

  The vector can be defined over an existing memory block, or define
  its own memory block.  The class's methods follow the STL
  specifications for Random Access Container.  It uses
  vctFixedSizeVectorTraits to define the types required by the STL
  interface.

  This class defines a read-only memory vector.  The read/write
  vector is derived from this class (compare with
  vctFixedStrideVectorConstIterator).  Therefore, all the methods of
  this class are const.  The non-const vector and methods are
  defined in vctFixedSizeVectorBase.

  Vector indexing is zero-based.

  The method provided for the compatibility with the STL containers
  start with a lower case.  Other methods start with a capitalilized
  letter (see CISST naming convention).

  \param _elementType the type of element in the vector.

  \param _size the number of elements that the vector provides
  access to.

  \param _stride the spacing between the elements in the memory block.
  _stride=1 means that the elements are next to each other.  _stride=2
  means they are separated by one, and so on.

  \param _dataPtrType the type of object that defines the vector.
  This may be an element pointer of an acutal fixed-size C-style array
  type.

  \sa vctFixedStrideVectorConstIterator vctFixedSizeVectorTraits
*/
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
class vctFixedSizeConstVectorBase
{
 public:
    /* Declare the container-defined typed required by STL, plus the
      types completed by our traits class */

    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    
    /*! Type of the vector itself. */
    typedef vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> ThisType;
    
    /*! Traits used for all useful types related to a vctFixedSizeVector. */
    typedef vctFixedSizeVectorTraits<_elementType, _size, _stride> VectorTraits;
    
    /*! Iterator on the elements of the vector. */   
    typedef typename VectorTraits::iterator iterator;
    
    /*! Const iterator on the elements of the vector. */   
    typedef typename VectorTraits::const_iterator const_iterator;
    
    /*! Reverse iterator on the elements of the vector. */   
    typedef typename VectorTraits::reverse_iterator reverse_iterator;
    
    /*! Const reverse iterator on the elements of the vector. */   
    typedef typename VectorTraits::const_reverse_iterator const_reverse_iterator;
    
    /*! type of overlay row matrix over this sequence */
    //@{
    typedef vctFixedSizeConstMatrixRef<_elementType, 1, _size, _stride*_size,_stride> RowConstMatrixRefType;
    typedef vctFixedSizeMatrixRef<_elementType, 1, _size, _stride*_size, _stride> RowMatrixRefType;
    //@}
    /*! type of overlay column matrix over this sequence */
    //@{
    typedef vctFixedSizeConstMatrixRef<_elementType, _size, 1, _stride, _stride*_size> ColConstMatrixRefType;
    typedef vctFixedSizeMatrixRef<_elementType, _size, 1, _stride, _stride*_size> ColMatrixRefType;
    //@}
    
    /* Size of the vector. */
    enum {SIZE = VectorTraits::SIZE};
    
    /* Stride between the elements of a sub vector. */
    enum {STRIDE = VectorTraits::STRIDE};
    
    /*! The type of a vector returned by value from operations on this object */
    typedef vctFixedSizeVector<_elementType, _size> VectorValueType;
    
    /*! The type used to create a copy. */
    typedef vctFixedSizeVector<_elementType, _size> CopyType;
    
    /*! The type of a vector of booleans returned from operations on this object, e.g.,
      ElementwiseEqual. */
    typedef vctFixedSizeVector<bool, _size> BoolVectorValueType;
    
    
 protected:
    /*! A declaration of the vector-defining member object */
    _dataPtrType Data;


    /*! Check the validity of an index. */
    inline void ThrowUnlessValidIndex(size_type index) const throw(std::out_of_range) {
        if (! ValidIndex(index)) {
            cmnThrow(std::out_of_range("vctFixedSizeVector: Invalid index"));
        }
    }
    
 public:
    /*! Returns a const iterator on the first element (STL
      compatibility). */
    const_iterator begin() const {
        return const_iterator(Data);
    }
    

    /*! Returns a const iterator on the last element (STL
      compatibility). */
    const_iterator end() const {
        return const_iterator(Data + STRIDE * SIZE);
    }
    
    
    /*! Returns a reverse const iterator on the last element (STL
      compatibility). */ 
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(Data + STRIDE * (SIZE - 1));
    }
    

    /*! Returns a reverse const iterator on the element before first
      (STL compatibility). */ 
    const_reverse_iterator rend() const {
        return const_reverse_iterator(Data - STRIDE);
    }
    

    /*! Returns the size of the vector (STL
      compatibility). */
    size_type size() const {
        return SIZE;
    }
    

    /*! Returns the maximum size of the vector (STL compatibility).
      For a fixed size vector, same as the size(). */
    size_type max_size() const {
        return SIZE;
    }
    

    /*! Not required by STL but provided for completeness */
    difference_type stride() const {
        return STRIDE;
    }
    
    
    /*! Tell is the vector is empty (STL compatibility).  False unless
      SIZE is zero. */
    bool empty() const {
        return (SIZE == 0);
    }
    

    /*! Access an element by index (const).
      \return a const reference to the element[index] */
    const_reference operator[](size_type index) const {
        return *(Pointer(index));
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


#ifndef SWIG
    /*! Access an element by index (const).  See method at().
      \return a const reference to element[index] */
    const_reference operator()(size_type index) const throw(std::out_of_range) {
        return at(index);
    }
#endif


    /*! Access an element by index (const). This method allows
      to access an element without any bounds checking.

      \return a reference to the element at index */
    const_reference Element(size_type index) const {
        return *(Pointer(index));
    }

    
    /*! Returns a const pointer to an element of the container,
      specified by its index. Addition to the STL requirements.
    */
    const_pointer Pointer(size_type index = 0) const {
        return Data + STRIDE * index;
    }


    /*! Returns true if this index is valid. */
    inline bool ValidIndex(size_type index) const {
        return ((index >= 0)
                && (index < size()));
    }


    /*! \name Size dependant methods.
      
      The following methods are size dependant, i.e. don't necessarily
      mean anything for all sizes of vector.  For example, using the
      Z() method on a vector of size 2 shouldn't be allowed.  It would
      have been possible to check these errors at compilation time,
      but this would require an overhead of code and therefore
      execution time which is not worth it.  Therefore, we are using
      and #CMN_ASSERT() to check that the template parameter _size is
      valid
      
      \note Using #CMN_ASSERT on a template parameter still allows the
      compiler to perform some optimization, which would be harder if
      #CMN_ASSERT was testing a method paramater. */
    
    //@{

    /*! Returns the first element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 1. */
    const value_type & X(void) const {
        CMN_ASSERT(_size > 0);
        return *(Pointer(0));
    }


    /*! Returns the second element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 2. */
    const value_type & Y(void) const {
        CMN_ASSERT(_size > 1);
        return *(Pointer(1));
    }


    /*! Returns the third element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 3. */
    const value_type & Z(void) const {
        CMN_ASSERT(_size > 2);
        return *(Pointer(2));
    }

    /*! Returns the fourth element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 4. */
    const value_type & W(void) const {
        CMN_ASSERT(_size > 3);
        return *(Pointer(3));
    }

    /*! Return a (const) vector reference for the first two elements of this
      vector.  May be used when switching from homogeneous coordinates to normal */
    vctFixedSizeConstVectorRef<_elementType, 2, _stride> XY(void) const {
        CMN_ASSERT(_size > 1);
        return vctFixedSizeConstVectorRef<_elementType, 2, _stride>(Pointer(0));
    }

    /*! Return a (const) vector reference for the first and third elements of this
      vector. */
    vctFixedSizeConstVectorRef<_elementType, 2, 2 * _stride> XZ(void) const {
        CMN_ASSERT(_size > 2);
        return vctFixedSizeConstVectorRef<_elementType, 2, 2 * _stride>(Pointer(0));
    }

    /*! Return a (const) vector reference for the first and fourth elements of this
      vector. */
    vctFixedSizeConstVectorRef<_elementType, 2, 3 * _stride> XW(void) const {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeConstVectorRef<_elementType, 2, 3 * _stride>(Pointer(0));
    }

    /*! Return a (const) vector reference for the two elements of this
      vector beginning on the second. */
    vctFixedSizeConstVectorRef<_elementType, 2, _stride> YZ(void) const {
        CMN_ASSERT(_size > 2);
        return vctFixedSizeConstVectorRef<_elementType, 2, _stride>(Pointer(1));
    }

    /*! Return a (const) vector reference for the two elements of this
      vector beginning on the second. */
    vctFixedSizeConstVectorRef<_elementType, 2, 2 * _stride> YW(void) const {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeConstVectorRef<_elementType, 2, 2 * _stride>(Pointer(1));
    }

    /*! Return a (const) vector reference for the two elements of this
      vector beginning on the third. */
    vctFixedSizeConstVectorRef<_elementType, 2, _stride> ZW(void) const {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeConstVectorRef<_elementType, 2, _stride>(Pointer(2));
    }

    /*! Return a (const) vector reference for the first three elements of this
      vector.  May be used when switching from homogeneous coordinates to normal,
      or when refering to a subvector of a row or a column inside a small matrix.
    */
    vctFixedSizeConstVectorRef<_elementType, 3, _stride> XYZ(void) const {
        CMN_ASSERT(_size > 2);
        return vctFixedSizeConstVectorRef<_elementType, 3, _stride>(Pointer(0));
    }

    /*! Return a (const) vector reference for the second, third and fourth elements
      of this vector. */
    vctFixedSizeConstVectorRef<_elementType, 3, _stride> YZW(void) const {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeConstVectorRef<_elementType, 3, _stride>(Pointer(1));
    }

    /*! Return a (const) vector reference for the first four elements of this
      vector.  May be used when switching from homogeneous coordinates to normal,
      or when refering to a subvector of a row or a column inside a small matrix.
    */
    vctFixedSizeConstVectorRef<_elementType, 4, _stride> XYZW(void) const {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeConstVectorRef<_elementType, 4, _stride>(Pointer(0));
    }

    //@}

    /*! Create an overlay matrix (MatrixRef) on top of this vector and
      return it.
    */
    //@{
    RowConstMatrixRefType AsRowMatrix() const
    {
        return RowConstMatrixRefType(Pointer());
    }

    ColConstMatrixRefType AsColMatrix() const
    {
        return ColConstMatrixRefType(Pointer());
    }
    //@}

    /*! \name Incremental operations returning a scalar.
      Compute a scalar from all the elements of the vector. */
    //@{

    /*! Return the sum of the elements of the vector.
      \return The sum of all the elements */
    inline value_type SumOfElements() const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template 
            SoVi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Identity>::
            Unfold(*this);
    } 


    /*! Return the product of the elements of the vector.
      \return The product of all the elements */
    inline value_type ProductOfElements() const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template 
            SoVi<typename vctBinaryOperations<value_type>::Multiplication,
            typename vctUnaryOperations<value_type>::Identity>::
            Unfold(*this);
    }


    /*! Return the square of the norm  of the vector.
      \return The square of the norm */
    inline value_type NormSquare(void) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoVi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::Square>::
            Unfold(*this);
    }

    /*! Return the norm of the vector.
      \return The norm. */
    inline NormType Norm(void) const {
        return sqrt(NormType(NormSquare()));
    }

    /*! Test if this vector is normalized.
      \param tolerance The maximum difference between the norm and 1
      to consider the vector normalized.
    */
    inline bool IsNormalized(_elementType tolerance = cmnTypeTraits<_elementType>::Tolerance()) const {
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
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoVi<typename vctBinaryOperations<value_type>::Addition,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Unfold(*this);
    }

    /*! Return the Linf norm of the vector, i.e. the maximum of the absolute
      values of all the elements.
 
      \return The Linf norm. */
    inline value_type LinfNorm(void) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoVi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::AbsValue>::
            Unfold(*this);
    }

    /*! Return the maximum element of the vector.
      \return The maximum element */
    inline value_type MaxElement(void) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoVi<typename vctBinaryOperations<value_type>::Maximum,
            typename vctUnaryOperations<value_type>::Identity>::
            Unfold(*this);
    }

    /*! Return the minimum element of the vector.
      \return The minimum element */
    inline value_type MinElement(void) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoVi<typename vctBinaryOperations<value_type>::Minimum,
            typename vctUnaryOperations<value_type>::Identity>::
            Unfold(*this);
    }
    
    /*! Compute the minimum AND maximum elements of the vector.
      This method is more runtime-efficient than computing them
      separately.
      \param minElement reference to store the minimum element result.
      \param maxElement reference to store the maximum element result.
    */
    inline void MinAndMaxElement(value_type & minElement, value_type & maxElement) const
    {
        vctFixedSizeVectorRecursiveEngines<_size>::MinAndMax::
            Unfold((*this), minElement, maxElement);
    }
    
    /*! Return true if all the elements of this vector are strictly
      positive, false otherwise */
    inline bool IsPositive(void) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoVi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsPositive>::
            Unfold(*this);
    }

    /*! Return true if all the elements of this vector are
      non-negative, false otherwise */
    inline bool IsNonNegative(void) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoVi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonNegative>::
            Unfold(*this);
    }

    /*! Return true if all the elements of this vector are
      non-positive, false otherwise */
    inline bool IsNonPositive(void) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoVi<typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonPositive>::
            Unfold(*this);
    }

    /*! Return true if all the elements of this vector are strictly
      negative, false otherwise */
    inline bool IsNegative (void) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoVi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNegative>::
            Unfold(*this);
    }


    /*! Return true if all the elements of this vector are nonzero,
      false otherwise */
    inline bool All(void) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoVi< typename vctBinaryOperations<bool>::And,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Unfold(*this);
    }

    /*! Return true if any element of this vector is nonzero, false
      otherwise */
    inline bool Any(void) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoVi< typename vctBinaryOperations<bool>::Or,
            typename vctUnaryOperations<bool, value_type>::IsNonzero>::
            Unfold(*this);
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
    template<int __stride, class __dataPtrType>
    inline value_type DotProduct(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
      SoViVi<typename vctBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication>::
            Unfold(*this, otherVector);
    }


    /*! \name Elementwise comparisons between vectors.
      Returns the result of the comparison. */
    //@{
    /*! Comparison between two vectors of the same size, containing
      the same type of elements.  The stride and the internal
      representation (_dataPtrType) can be different.  The comparaison
      (\f$ = \neq < \leq > \geq \f$) for Equal(), NotEqual(),
      Lesser(), LesserOrEqual(), Greater() or GreaterOrEqual() is
      performed elementwise between the two vectors.  A logical "and"
      is performed (except for NotEqual which uses a logical "or") to
      accumulate the elementwise results.  The only operators
      provided are "==" and "!=" since the semantic is not ambiguous.

      \return A boolean.
    */
    template<int __stride, class __dataPtrType>
    inline bool Equal(const vctFixedSizeConstVectorBase<_size, __stride,
                      value_type, __dataPtrType> & otherVector) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Unfold(*this, otherVector);
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline bool operator == (const vctFixedSizeConstVectorBase<_size, __stride,
                             value_type, __dataPtrType> & otherVector) const {
        return Equal(otherVector);
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline bool AlmostEqual(const vctFixedSizeConstVectorBase<_size, __stride,
          value_type, __dataPtrType> & otherVector,
          value_type tolerance) const {
        return ((*this - otherVector).LinfNorm() <= tolerance);
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline bool AlmostEqual(const vctFixedSizeConstVectorBase<_size, __stride,
          value_type, __dataPtrType> & otherVector) const {
        return ((*this - otherVector).LinfNorm() <= cmnTypeTraits<_elementType>::Tolerance());
    }
    
    /* documented above */
    template<int __stride, class __dataPtrType>
    inline bool NotEqual(const vctFixedSizeConstVectorBase<_size, __stride,
                         value_type, __dataPtrType> & otherVector) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViVi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Unfold(*this, otherVector);
    }
    
    /* documented above */
    template<int __stride, class __dataPtrType>
    inline bool operator != (const vctFixedSizeConstVectorBase<_size, __stride,
                             value_type, __dataPtrType> & otherVector) const {
        return NotEqual(otherVector);
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline bool Lesser(const vctFixedSizeConstVectorBase<_size, __stride,
                       value_type, __dataPtrType> & otherVector) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Unfold(*this, otherVector);
    }
    
    /* documented above */
    template<int __stride, class __dataPtrType>
    inline bool LesserOrEqual(const vctFixedSizeConstVectorBase<_size, __stride,
                              value_type, __dataPtrType> & otherVector) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Unfold(*this, otherVector);
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline bool Greater(const vctFixedSizeConstVectorBase<_size, __stride,
                        value_type, __dataPtrType> & otherVector) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Unfold(*this, otherVector);
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline bool GreaterOrEqual(const vctFixedSizeConstVectorBase<_size, __stride,
                               value_type, __dataPtrType> & otherVector) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViVi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Unfold(*this, otherVector);
    }
    //@}

    /*! \name Elementwise comparisons between vectors.
      Returns the vector of comparison's results. */
    //@{
    /*! Comparison between two vectors of the same size, containing
      the same type of elements.  The strides and the internal
      representation (_dataPtrType) can be different.  The comparaison
      (\f$ = \neq < \leq > \geq \f$) for ElementwiseEqual(),
      ElementwiseNotEqual(), ElementwiseLesser(),
      ElementwiseLesserOrEqual(), ElementwiseGreater() or
      ElementwiseGreaterOrEqual() is performed elementwise between the
      two vectors and stored in a newly created vector.  There is no
      operator provided since the semantic would be ambiguous.

      \return A vector of booleans.
    */
    template<int __stride, class __dataPtrType>
    inline BoolVectorValueType
    ElementwiseEqual(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) const {
        return vctFixedSizeVectorElementwiseCompareVector<
            _size, _stride, _dataPtrType, __stride, __dataPtrType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, otherVector);
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline BoolVectorValueType
    ElementwiseNotEqual(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) const {
        return vctFixedSizeVectorElementwiseCompareVector<
            _size, _stride, _dataPtrType, __stride, __dataPtrType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, otherVector);
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline BoolVectorValueType
    ElementwiseLesser(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) const {
        return vctFixedSizeVectorElementwiseCompareVector<
            _size, _stride, _dataPtrType, __stride, __dataPtrType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, otherVector);
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline BoolVectorValueType
    ElementwiseLesserOrEqual(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) const {
        return vctFixedSizeVectorElementwiseCompareVector<
            _size, _stride, _dataPtrType, __stride, __dataPtrType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, otherVector);
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline BoolVectorValueType
    ElementwiseGreater(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) const {
        return vctFixedSizeVectorElementwiseCompareVector<
            _size, _stride, _dataPtrType, __stride, __dataPtrType, value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, otherVector);
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline BoolVectorValueType
    ElementwiseGreaterOrEqual(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) const {
        return vctFixedSizeVectorElementwiseCompareVector<
            _size, _stride, _dataPtrType, __stride, __dataPtrType, value_type,
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
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>::
            Unfold(*this, scalar);
    }
    
    /* documented above */
    inline bool operator == (const value_type & scalar) const {
        return Equal(scalar);
    }
    
    /* documented above */
    inline bool NotEqual(const value_type & scalar) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViSi<typename vctBinaryOperations<bool>::Or,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>::
            Unfold(*this, scalar);
    }
    
    /* documented above */
    inline bool operator != (const value_type & scalar) const {
        return NotEqual(scalar);
    }

    /* documented above */
    inline bool Lesser(const value_type & scalar) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>::
            Unfold(*this, scalar);
    }
    
    /* documented above */
    inline bool LesserOrEqual(const value_type & scalar) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>::
            Unfold(*this, scalar);
    }

    /* documented above */
    inline bool Greater(const value_type & scalar) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>::
            Unfold(*this, scalar);
    }
    
    /* documented above */
    inline bool GreaterOrEqual(const value_type & scalar) const {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViSi<typename vctBinaryOperations<bool>::And,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>::
            Unfold(*this, scalar);
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
    inline BoolVectorValueType ElementwiseEqual(const value_type & scalar) const {
        return vctFixedSizeVectorElementwiseCompareScalar<_size, _stride, _dataPtrType,  value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Equal>(*this, scalar);
    }

    /* documented above */
    inline BoolVectorValueType ElementwiseNotEqual(const value_type & scalar) const {
        return vctFixedSizeVectorElementwiseCompareScalar<_size, _stride, _dataPtrType,  value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::NotEqual>(*this, scalar);
    }

    /* documented above */
    inline BoolVectorValueType ElementwiseLesser(const value_type & scalar) const {
        return vctFixedSizeVectorElementwiseCompareScalar<_size, _stride, _dataPtrType,  value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Lesser>(*this, scalar);
    }

    /* documented above */
    inline BoolVectorValueType ElementwiseLesserOrEqual(const value_type & scalar) const {
        return vctFixedSizeVectorElementwiseCompareScalar<_size, _stride, _dataPtrType,  value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::LesserOrEqual>(*this, scalar);
    }

    /* documented above */
    inline BoolVectorValueType ElementwiseGreater(const value_type & scalar) const {
        return vctFixedSizeVectorElementwiseCompareScalar<_size, _stride, _dataPtrType,  value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::Greater>(*this, scalar);
    }

    /* documented above */
    inline BoolVectorValueType ElementwiseGreaterOrEqual(const value_type & scalar) const {
        return vctFixedSizeVectorElementwiseCompareScalar<_size, _stride, _dataPtrType,  value_type,
            typename vctBinaryOperations<bool, value_type, value_type>::GreaterOrEqual>(*this, scalar);
    }

    //@}


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


#if 0   // eliminating definition of GetConstSubsequence method
    /*! Initialize a subsequence of the container, starting at a
      specified position.  The size and the stride of the subsequence
      are given by the type of the result object.

      This method is tightly coupled with the type of the result.
      Generally the result object should have a SIZE and a STRIDE
      member enums, which means generally that the result object is
      derived from vctFixedSizeConstVectorBase.  Practically, the
      type of the result object depends on the type of this object,
      and the stride of the result should be a multiple of the stride
      of this object
    */
    template<class _subsequenceType>
    void GetConstSubsequence(size_type position, _subsequenceType & result) const {
        assert( (_subsequenceType::STRIDE % ThisType::STRIDE) == 0 );
        assert( position + 
                ((_subsequenceType::SIZE-1) * (_subsequenceType::STRIDE / ThisType::STRIDE)) 
                <= (ThisType::SIZE-1) );
        assert( position + 
                ((_subsequenceType::SIZE-1) * (_subsequenceType::STRIDE / ThisType::STRIDE)) 
                >= 0 );
        result.SetRef( Pointer(position) );
    }
#endif  // eliminate definition

#if 0  // eliminating definition of GetConstSubsequence method
    /*!  This class can be called ``syntactic sugar''.  It helps the user
     calculate automatically the length of a subsequence, if it can be done
     during compilation.  To do this calculation, use the SUB_SIZE enum
     member as the size argument of the subsequence:

     // Assume Vector1Type is derived from vctFixedSizeConstVectorBase
     enum {SUBSEQUENCE_START = 2, SUBSEQUENCE_STRIDE = 3};
     typedef Vector1Type::SubsequenceTraits<SUBSEQUENCE_START, SUBSEQUENCE_STRIDE>
         SubsequenceTraits;
     vctFixedSizeConstVector< Vector1Type::value_type, SubsequenceTraits::SUB_SIZE,
         SubsequenceTraits::SUB_STRIDE > Subsequence1Type;
     // Now initialize a subsequence object
     Subsequence1Type subsequence;
     vector.GetConstSubsequence(SUBSEQUENCE_START, subsequence);
        
     \param _subPosition the starting index (zero-based) of the subsequence
     inside its containing vector.
     \param _subStride the stride of the subsequence inside its containing
     vector

     \note The position and the stride must be given relative to the 
     immediately containing vector.  That is, regardless of the underlying
     memory structure.  If the containing vector is itself a subsequence of
     a third vector, the stride and the position will be scaled by the code,
     and the user need not know about any super-container except for the 
     immediate one.
    
    */
#ifndef SWIG // SWIG 1.3.21 doesn't handle sub-classes
    template<size_type _subPosition, int _subStride>
    class SubsequenceTraits {
        public:
        enum {
            SUB_STRIDE = _subStride,
            SUB_POSITION = _subPosition,
            NUM_ELEMENTS = (SUB_STRIDE > 0) ? (SIZE - SUB_POSITION) : SUB_POSITION,
            ABS_SUB_STRIDE = (SUB_STRIDE > 0) ? SUB_STRIDE : -SUB_STRIDE,
            SUB_SIZE = (NUM_ELEMENTS / SUB_STRIDE) + ((NUM_ELEMENTS % SUB_STRIDE) != 0)
        };
    };
#endif // SWIG
#endif  // eliminate definition

    /*! ``Syntactic sugar'' to simplify the creation of a reference to a subvector.
      This class declares a const subvector type.
      To declare a subvector object, here's an example.

      typedef vctFixedSizeVector<double, 9> Vector9;
      Vector9 v;
      Vector9::ConstSubvector<3>::Type first3(v, 0);  // first 3 elements of v
      Vector9::ConstSubvector<3>::Type mid3(v, 0);  // middle 3 elements of v
      Vector9::ConstSubvector<3>::Type last3(v, 6);  // last 3 elements of v

      \param _subSize the size of the subvector

      \note the stride of the subvector with respect to its parent container
      is always 1.  That is, the memory strides of the subvector and the
      parent container are equal.  For more sophisticated subsequences,
      the user has to write customized code.
    */
    template<unsigned int _subSize>
    class ConstSubvector {
    public:
        typedef vctFixedSizeConstVectorRef<value_type, _subSize, STRIDE> Type;
    };

    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
    
    void ToStream(std::ostream & outputStream) const {
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



/*!
  Dot product of two vectors <em>of the same type and size</em>.
  
  \param vector1 first operand of the dot product.
  \param vector2 second operand of the dot product.
  \return The dot product of vector1 and vector2.
*/
template<unsigned int _size, int _vector1Stride, class _vector1Data, int _vector2Stride, class _vector2Data, class _elementType>
inline _elementType
vctDotProduct(const vctFixedSizeConstVectorBase<_size, _vector1Stride, _elementType, _vector1Data> & vector1,
              const vctFixedSizeConstVectorBase<_size, _vector2Stride, _elementType, _vector2Data> & vector2)
{
    return vector1.DotProduct(vector2);
}

/*!
  Dot product of two vectors <em>of the same type and size</em>.
  
  \param vector1 first operand of the dot product.
  \param vector2 second operand of the dot product.
  \return The dot product of vector1 and vector2.
*/
template<unsigned int _size, int _vector1Stride, class _vector1Data, int _vector2Stride, class _vector2Data, class _elementType>
inline _elementType
operator * (const vctFixedSizeConstVectorBase<_size, _vector1Stride, _elementType, _vector1Data> & vector1,
            const vctFixedSizeConstVectorBase<_size, _vector2Stride, _elementType, _vector2Data> & vector2) {
    return vector1.DotProduct(vector2);
}

/*! Return true if all the elements of the vector are nonzero, false otherwise */
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline bool vctAll(const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector) {
    return vector.All();
}

/*! Return true if any element of the vector is nonzero, false otherwise */
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
inline bool vctAny(const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector) {
    return vector.Any();
}

/*! Stream out operator. */
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
std::ostream & operator << (std::ostream & output,
                            const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector) {
    vector.ToStream(output);
    return output;
}

#endif  // _vctFixedSizeConstVectorBase_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedSizeConstVectorBase.h,v $
// Revision 1.35  2006/07/03 17:31:56  dli
// Added ProductOfElements function
//
// Revision 1.34  2005/12/23 21:27:31  anton
// cisstVector: Minor updates for Doxygen 1.4.5.
//
// Revision 1.33  2005/12/21 21:43:42  anton
// cisstVector: Added IsCompact() for vectors.  IsFortran() has been added as
// well to match the API of matrices.  Most Fortran based routines should then
// use IsFortran() on both vectors and matrices.  Routines which can be used
// with any storage order should at least use IsCompact() on the vectors and
// matrices.
//
// Revision 1.32  2005/12/13 00:04:57  ofri
// All cisstVector containers: Added methods
// 1) MinAndMaxElement  (at const base)
// 2) Floor, Ceil  (at const base -- implemented at container)
// 3) FloorSelf, CeilSelf, FloorOf, CeilOf  (at non-const base)
//
// Revision 1.31  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.30  2005/09/27 18:01:25  anton
// cisstVector: Use CMN_ASSERT instead of assert for vectors and matrices.
//
// Revision 1.29  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.28  2005/09/24 00:01:04  anton
// cisstVector: Use cmnThrow for all exceptions.
//
// Revision 1.27  2005/09/23 17:24:08  ofri
// vctFixedSize[|Const]VectorBase: Added slice methods for subvectors XZ, XW, YW,
// ZW, YZW in const and non-const versions
//
// Revision 1.26  2005/08/11 21:29:44  ofri
// cisstVector -- base classes ToStream(): Added code to preserve the previous
// state of the stream (precision, width, showpoint) and resume it before returning
// from the method.
//
// Revision 1.25  2005/08/04 15:31:57  anton
// cisstVector: Replaced RangeCheck() by ThrowUnlessValidIndex().  For dynamic
// matrices, Row() and Col() now check the index and throw using
// ThrowUnlessValid{Row,Col}Index().
//
// Revision 1.24  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.23  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.22  2005/04/28 03:55:23  anton
// cisstVector:  Added AlmostEqual() to all containers to compare with
// tolerance.
//
// Revision 1.21  2005/01/12 15:50:13  anton
// vctFixedSizeVector: Removed definition and use of TypeTraits for SWIG.
//
// Revision 1.20  2005/01/11 22:42:18  anton
// cisstVector:  Added normalization methods to the vectors.  Removed useless
// code from vctQuaternion and added some tests.  See ticket #110.
//
// Revision 1.19  2005/01/06 18:54:00  anton
// cisstVector: Introduction of type CopyType as requested in ticket #113.
//
// Revision 1.18  2005/01/05 18:29:49  anton
// cisstVector: Added ValidIndex() methods for all containers (and tests) as
// requested in ticket #111.
//
// Revision 1.17  2004/12/10 21:59:07  ofri
// vct[FixedSize|Dynamic](Const)VectorBase: Added methods XY(), XYZ(), XYZW()
// to access subvectors easily.  These are useful when handling barycentric
// coordinates.
//
// Revision 1.16  2004/11/30 23:36:05  anton
// cisstVector: Added the Element() method to access an element without range
// check.  See ticket #79.
//
// Revision 1.15  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
// Revision 1.14  2004/11/01 16:57:06  ofri
// Added the method W() to access the fourth element of a vector
//
// Revision 1.13  2004/10/26 15:20:52  ofri
// Updating subsequence interfaces (ticket #76) and interfaces between fixed
// and dynamic vectors/matrices (ticket #72).
// *: vctFixedSize(Const)VectorBase::Get(Const)Subsequence now deprecated.
// Replaced by Get(Const)Subvector with equal stride to the parent.
// *: Added constructors for [Fixed|Dynamic][Vector|Matrix]Ref from the other
// storage type.
//
// Revision 1.12  2004/10/25 19:19:54  anton
// cisstVector:  Created vctForwardDeclarations.h and removed forward
// declarations of classes in all other files.  Added some constructors
// for vector references to reference fixed size from dynamic and vice versa.
//
// Revision 1.11  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.10  2004/10/22 18:07:06  anton
// Doxygen: Made sure that all the classes are documented for Doxygen.
//
// Revision 1.9  2004/10/21 19:27:47  anton
// cisstVector: Added types BoolValueType and ValueType for
// vctFixedSizeVector, vctDynamicVector, vctDynamicMatrix.  This solves #77.
//
// Revision 1.8  2004/10/14 19:18:01  anton
// cisstVector, fixed size containers: Added operator() based on at() method
// as required per ticket #72.
//
// Revision 1.7  2004/10/09 20:54:08  ofri
// Fixing bug in type definition of vctFixedSizeConstVectorBase::ColConstMatrixRefType
// etc : put matrix strides in the correct order.
//
// Revision 1.6  2004/10/08 22:09:18  ofri
// cisstVector: Added methods AsRowMatrix() and AsColMatrix() to the fixed size
// base (see #69).  This compiles successfully on gcc and .net7.  Not tested yet.
// This code is committed for continued work in the coming few days.
//
// Revision 1.5  2004/10/08 19:38:58  anton
// cisstVector: Added a space between elements in ToStream methods.
//
// Revision 1.4  2004/09/24 20:20:42  anton
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
// Revision 1.3  2004/09/21 20:08:11  anton
// cisstVector: In mthods Norm(), use a cast to NormType(NormSquare) before
// calling sqrt.  Otherwise, .Net 2003 complains (.Net 7 worked).
//
// Revision 1.2  2004/08/13 21:40:40  anton
// cisstVector: Added a bound check for all vectors (fixed size and dynamic)
// as requested in ticket #13.  Added protect RangeCheck method to facilitate
// (inspired from the STL distributed with gcc).  Added some tests in
// vctGenericVectorTests.  Still have to do the equivalent for matrices.
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
// Revision 1.17  2004/08/12 20:24:58  anton
// cisstVector: Removed useless #include <math.h> which is included by
// cmnPortability.  This allows a better control over the system includes.
//
// Revision 1.16  2004/08/11 20:42:26  ofri
// Added excluded code in vctFixedLengthConstSequenceBase to conditionally
// evaulate a compilation error.  Pending decision before including.
//
// Revision 1.15  2004/08/11 19:21:32  anton
// cisstVector: the operator << and the methods ToString and ToStream have
// been improved.
//
// Revision 1.14  2004/08/10 14:01:59  ofri
// 1) Defined concatenation operator & which takes to tempalated sequences
// and returns a fixed size vector object (vctFixedSizeVector.h)
// 2) Moved the definition of subsequence types (template class SubsequenceRef)
// from vctFixedSizeVector to the sequence base header files with appropriate
// forward declarations
//
// Revision 1.13  2004/08/04 21:02:30  anton
// cisstVector: Methods {quote: ToString} now relies on the stream out
// operator (<<).
//
// Revision 1.12  2004/07/27 20:08:06  anton
// cisstVector: Added some Doxygen documentation.  Minor code changes to
// keep the dynamic vectors in sync with the fixed size ones.
//
// Revision 1.11  2004/07/19 20:46:50  ofri
// vctFixedLengthConstSequenceBase: Added global functions vctAll and vctAny to
// provide (almost) uniform functional interface with other languages (Matlab, Cg)
//
// Revision 1.10  2004/07/14 16:20:26  ofri
// Added class vctUnaryOperations::IsNonzero and methods
// vctFixedLengthConstSequenceBase::All, vctFixedLengthConstSequenceBase::Any
//
// Revision 1.9  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.8  2004/03/29 21:17:18  anton
// #ifdef for SWIG and added a primitive ToString method for SWIG
//
// Revision 1.7  2004/03/19 15:27:57  ofri
// Added the method at(index) to access sequence elements without operator
// overloading
//
// Revision 1.6  2003/12/18 16:14:50  anton
// Changed the order of the template parameters for vctBinaryOperations so that the output is first (consistant with the rest of code)
//
// Revision 1.5  2003/12/17 17:09:09  ofri
// Fixed the order of template arguments in sign determining methods (e.g.,
// IsPositive)
//
// Revision 1.4  2003/12/16 16:37:22  anton
// The returned type is now infered from the binary incremental operation for all recursive engines SoXxxx
//
// Revision 1.3  2003/12/03 15:55:32  ofri
// 1) Fixed the bug that caused warning 4800 in .NET (wrong parameter to
// recursive engine So*).
// 2) Added sign determination operations: IsPositive, IsNonNegative,
// IsNonPositive, IsNegative.
//
// Revision 1.2  2003/11/25 20:31:55  ofri
// Added push and pop of warning state to suppress warning 4800 on MSVC7
//
// Revision 1.1  2003/11/20 18:16:36  anton
// Splitted const and non const classes in two files.  Added X, Y and Z methods
//
//
// Revision 1.11  2003/11/13 19:15:17  anton
// Added operators == != < <= > >= for sequence/sequence and sequence/scalar
//
// Revision 1.10  2003/11/11 22:03:38  anton
// Tabulations
//
// Revision 1.9  2003/11/03 21:29:54  ofri
// A new definition for vctFixedLengthSequenceBase (and ConstSequenceBase):
// 1) The order of template arguments was changed so that the numeric arguments
// come first, and the type arguments come last.  That is, the _elementType
// argument is now third instead of first.  This change was applied to all sets of
// template arguments, including function headers.
// 2) We now call vctSequenceRecursiveEngines directly instead of using the additional
// layer vctFixedLengthSequenceOperations.  This layer was completely removed
// from the sequence class.  In addition, the oprations are called directly for
// the sequence objects, not using iterators.
//
// These changes were tested by the time of submission on gcc using the cisstVector
// test library, and passed all tests successfully.  Not tested yet on
// msvc / others
//
// Revision 1.8  2003/10/21 15:05:41  anton
// Added #include<math.h> for Microsoft compilers
//
// Revision 1.7  2003/10/20 20:28:14  anton
// Changed vector to sequence in documentation.  Added * as dot product and vctDotProduct
//
// Revision 1.6  2003/10/20 18:35:19  anton
// Changed FixedSizeVectorOperations and FixedSizeVectorTraits to FixedLengthSequence Operations and Traits
//
// Revision 1.5  2003/10/20 17:15:48  anton
// Added all operations/methods from vctFixedSizeVector.h
//
// Revision 1.4  2003/10/13 13:38:55  ofri
// Added documentation for vctFixedLengthConstSequenceBase::SubsequenceTraits
//
// Revision 1.3  2003/10/03 19:15:57  anton
// Updated doxygen documentation
//
// Revision 1.2  2003/09/30 18:46:58  anton
// Renamed template parameters __stride to _subStride
//
// Revision 1.1  2003/09/30 14:02:49  anton
// creation
//
//
// ****************************************************************************


