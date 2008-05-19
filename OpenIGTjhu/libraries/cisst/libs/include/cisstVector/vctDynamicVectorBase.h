/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicVectorBase.h,v 1.39 2006/05/21 11:27:06 ofri Exp $
  
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


#ifndef _vctDynamicVectorBase_h 
#define _vctDynamicVectorBase_h

/*!
  \file
  \brief Declaration of vctDynamicVectorBase
*/


#include <cstdarg>

#include<cisstVector/vctDynamicConstVectorBase.h>
#include<cisstVector/vctDynamicConstVectorRef.h>
#include<cisstVector/vctStoreBackUnaryOperations.h>
#include<cisstVector/vctStoreBackBinaryOperations.h>


#ifndef DOXYGEN
// forward declaration of auxiliary function to multiply matrix * vector
template<class _resultVectorOwnerType, class _matrixOwnerType, class _vectorOwnerType, class _elementType>
inline
void vctMultiplyMatrixVector(vctDynamicVectorBase<_resultVectorOwnerType, _elementType> & result,
                             const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix,
                             const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector);

// forward declaration of auxiliary function to multiply matrix * vector
template<class _resultVectorOwnerType, class _vectorOwnerType, class _matrixOwnerType, class _elementType>
inline
void vctMultiplyVectorMatrix(vctDynamicVectorBase<_resultVectorOwnerType, _elementType> & result,
                             const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector,
                             const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix);
#endif // DOXYGEN

/*!
  This class provides all the const methods inherited from
  vctConstVectorBase, and extends them with non-const methods, such
  as SumOf.

  \sa vctDynamicConstVectorBase

*/
template<class _vectorOwnerType, typename _elementType>
class vctDynamicVectorBase : public vctDynamicConstVectorBase<_vectorOwnerType, _elementType>
{
public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    typedef vctDynamicVectorBase ThisType;
    /*! Type of the base class. */
    typedef vctDynamicConstVectorBase<_vectorOwnerType, _elementType> BaseType;
    
    typedef _vectorOwnerType VectorOwnerType;

    typedef typename BaseType::iterator iterator; 
    typedef typename BaseType::reverse_iterator reverse_iterator; 
    typedef typename BaseType::const_iterator const_iterator; 
    typedef typename BaseType::const_reverse_iterator const_reverse_iterator; 

    typedef typename BaseType::CopyType CopyType; 
    typedef typename BaseType::VectorValueType VectorValueType; 


    /*! Type traits for the elements of the vector. */
    typedef class cmnTypeTraits<value_type> TypeTraits;
    typedef typename TypeTraits::VaArgPromotion ElementVaArgPromotion;


    /*! Returns an iterator on the first element (STL
      compatibility). */
    iterator begin(void) {
        return this->Vector.begin(); 
    }

    /*! Returns an iterator on the last element (STL
      compatibility). */
    iterator end(void) {
        return this->Vector.end(); 
    }

    /*! Returns a reverse iterator on the last element (STL
      compatibility). */ 
    reverse_iterator rbegin(void) {
        return this->Vector.rbegin(); 
    }

    /*! Returns a reverse iterator on the element before first
      (STL compatibility). */ 
    reverse_iterator rend(void) {
        return this->Vector.rend(); 
    }

    /* documented in base class */
    const_iterator begin(void) const {
        return this->Vector.begin(); 
    }

    /* documented in base class */
    const_iterator end(void) const {
        return this->Vector.end(); 
    }

    /* documented in base class */
    const_reverse_iterator rbegin(void) const {
        return this->Vector.rbegin(); 
    }

    /* documented in base class */
    const_reverse_iterator rend(void) const {
        return this->Vector.rend(); 
    }
    
    /*! Access an element by index.
      \return a reference to the element[index] */    
    reference operator[](int index) {
        return *Pointer(index);
    }

    /* documented in base class */
    const_reference operator[](int index) const {
        return *Pointer(index);
    }
    
    /*! Addition to the STL requirements.  Return a pointer to an element of the
      container, specified by its index.
    */
    pointer Pointer(int index = 0) {
        return this->Vector.Pointer(index);
    }

    /* documented in base class */
    const_pointer Pointer(int index = 0) const {
        return this->Vector.Pointer(index);
    }

    /*! Access an element by index (non-const).  Compare with
      std::vector::at.  This method can be a handy substitute for the
      overloaded operator [] when operator overloading is unavailable
      or inconvenient.

      \return a non-const reference to element[index] */
    reference at(size_type index) throw(std::out_of_range) {
        this->ThrowUnlessValidIndex(index);
        return *(Pointer(index));
    }

    /* documented in base class */
    const_reference at(size_type index) const throw(std::out_of_range) {
        this->ThrowUnlessValidIndex(index);
        return *(Pointer(index));
    }

    /*! Overloaded operator () for simplified (non-const) element access with bounds checking */
    inline reference operator() (size_type index) throw(std::out_of_range) {
        return this->at(index);
    }

    /*! Overloaded operator () for simplified (const) element access with bounds checking */
    inline const_reference operator() (size_type index) const throw(std::out_of_range) {
        return this->at(index);
    }


    /*! Access an element by index (non const). This method allows
      to access an element without any bounds checking.

      \return a reference to the element at index */
    reference Element(size_type index) {
        return *(Pointer(index));
    }

    /* documented in base class */
    const_reference Element(size_type index) const {
        return *(Pointer(index));
    }

    
    /*! Assign the given value to all the elements.
      \param value the value used to set all the elements of the vector
      \return The value used to set all the elements
    */
    inline value_type SetAll(const value_type value) {
        vctDynamicVectorLoopEngines::
            VioSi<typename vctStoreBackBinaryOperations<value_type>::SecondOperand>::
            Run(*this, value);
        return value;
    }


    /*!
      \name Assignment operation between vectors of different types.
      
      \param other The vector to be copied.
    */
    //@{
    template<class __vectorOwnerType, typename __elementType>
    inline ThisType & Assign(const vctDynamicConstVectorBase<__vectorOwnerType, __elementType> & other) {
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type,
            typename __vectorOwnerType::value_type>::Identity>::
            Run(*this, other);
    return *this;
    }

    template<class __vectorOwnerType, typename __elementType>
    inline ThisType & operator = (const vctDynamicConstVectorBase<__vectorOwnerType, __elementType> & other) {
        return this->Assign(other);
    }

    template<unsigned int __size, int __stride, class __elementType, class __dataPtrType>
    inline ThisType & Assign(const vctFixedSizeConstVectorBase<__size, __stride, __elementType, __dataPtrType>
                             & other) {
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type, __elementType>::Identity>::
            Run(*this, other);
        return *this;
    }
    //@}

    /*! Assign to this vector values from a C array given as a
      pointer to value_type.  The purpose of this method is to simplify
      the syntax by not necessitating the creation of an explicit vector
      for the given array.  However, we only provide this method for
      an array of value_type.  For arrays of other types a vector
      still needs to be declared.

      This method assumes that the input array has the necessary
      number of elements.

      \return a reference to this object.
    */
    inline ThisType & Assign(const value_type * elements)
    {
        vctDynamicVectorLoopEngines::
            VoVi< typename vctUnaryOperations<value_type>::Identity >::
            Run(*this, vctDynamicConstVectorRef<value_type>(this->size(), elements));
        return *this;
    }

    /*! Assign to this vector using stdarg macros and variable number
      of arguments.  This operation assumes that all the arguments are
      of type value_type, and that their number is equal to the size
      of the vector.  The user may need to explicitly cast the
      parameters to value_type to avoid runtime bugs and errors.  We
      have not checked if stdarg macros can use reference types
      (probably not), so unlike the other constructors, this
      constructor takes all arguments by value.

      \note This method does not assert that the size is correct,
      as there is no way to know how many arguments were passed.
    */

    inline ThisType & Assign(value_type element0, value_type element1, ...) throw(std::runtime_error) {
        const unsigned int size = this->size();
        if (size < 2) {
            cmnThrow(std::runtime_error("vctDynamicVector: Assign from va_list requires size >= 2"));
        }
        this->at(0) = element0;
        this->at(1) = element1;
        va_list nextArg;
        va_start(nextArg, element1);
        for (unsigned int i = 2; i < size; ++i) {
            this->at(i) = value_type( va_arg(nextArg, ElementVaArgPromotion) );
        }
        va_end(nextArg);
        return *this;
    }


    /*! Concatenate a single element at the end of a shorter-by-1
      vector to obtain a vector of my size.  The concatenation result
      is stored in this vector.  The function is useful, e.g., when
      embedding vectors from R^n into R^{n+1} or into homogeneous
      space.
    */
    template<class __vectorOwnerType, class __elementType>
    inline ThisType & ConcatenationOf(const vctDynamicConstVectorBase<__vectorOwnerType, __elementType> & otherVector,
                                      __elementType lastElement) {
  vctDynamicVectorRef<value_type> firstElements(*this, 0, otherVector.size() - 1);
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type, __elementType>::Identity>::
            Run(firstElements, otherVector);
        (*this)[otherVector.size()] = value_type(lastElement);
        return *this;
    }


    /*! \name Size dependant methods.
      
      The following methods are size dependant, i.e. don't necessarily
      mean anything for all sizes of vector.  For example, using the
      Z() method on a vector of size 2 shouldn't be allowed.
      Therefore, we are using #CMN_ASSERT to check that the size is
      valid */
    
    //@{

    /*! Returns the first element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 1. */
    value_type & X(void) {
        CMN_ASSERT(this->size() > 0);
        return *(Pointer(0));
    }

    // we have to redeclare it here, shadowed
    const value_type & X(void) const {
        return BaseType::X();
    }


    /*! Returns the second element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 2. */
    value_type & Y(void) {
        CMN_ASSERT(this->size() > 1);
        return *(Pointer(1));
    }

    // we have to redeclare it here, shadowed
    const value_type & Y(void) const {
        return BaseType::Y();
    }


    /*! Returns the third element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 3. */
    value_type & Z(void) {
        CMN_ASSERT(this->size() > 2);
        return *(Pointer(2));
    }

    // we have to redeclare it here, shadowed
    const value_type & Z(void) const {
        return BaseType::Z();
    }

    /*! Returns the fourth element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 4. */
    value_type & W(void) {
        CMN_ASSERT(this->size() > 3);
        return *(Pointer(3));
    }

    // we have to redeclare it here, shadowed
    const value_type & W(void) const {
        return BaseType::W();
    }

    /*! Return a (non-const) vector reference for the first two elements of this
      vector.  May be used when switching from homogeneous coordinates to normal.
      (shadowed from base class).
    */
    vctDynamicVectorRef<_elementType> XY(void) {
        CMN_ASSERT(this->size() > 1);
        return vctDynamicVectorRef<_elementType>(2, Pointer(0), this->stride());
    }

    /*! Return a (non-const) vector reference for the first and third elements of this
      vector. */
    vctDynamicVectorRef<_elementType> XZ(void) {
        CMN_ASSERT(this->size() > 2);
        return vctDynamicVectorRef<_elementType>(2, Pointer(0), 2 * this->stride());
    }

    /*! Return a (non-const) vector reference for the first and fourth elements of this
      vector. */
    vctDynamicVectorRef<_elementType> XW(void) {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicVectorRef<_elementType>(2, Pointer(0), 3 * this->stride());
    }

    /*! Return a (non-const) vector reference for the two elements of this
      vector beginning on the second. */
    vctDynamicVectorRef<_elementType> YZ(void) {
        CMN_ASSERT(this->size() > 2);
        return vctDynamicVectorRef<_elementType>(2, Pointer(1), this->stride());
    }

    /*! Return a (non-const) vector reference for the two elements of this
      vector beginning on the second. */
    vctDynamicVectorRef<_elementType> YW(void) {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicVectorRef<_elementType>(2, Pointer(1), 2 * this->stride());
    }

    /*! Return a (non-const) vector reference for the two elements of this
      vector beginning on the third. */
    vctDynamicVectorRef<_elementType> ZW(void) {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicVectorRef<_elementType>(2, Pointer(2), this->stride());
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

    /*! Return a (non-const) vector reference for the first three
      elements of this vector.  May be used when switching from
      homogeneous coordinates to normal, or when refering to a
      subvector of a row or a column inside a small matrix.
    */
    vctDynamicVectorRef<_elementType> XYZ(void) {
        CMN_ASSERT(this->size() > 2);
        return vctDynamicVectorRef<_elementType>(3, Pointer(0), this->stride());
    }

    /*! Return a (non-const) vector reference for the second, third and fourth elements
      of this vector. */
    vctDynamicVectorRef<_elementType> YZW(void) {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicVectorRef<_elementType>(3, Pointer(1),  this->stride());
    }

    /*! Return a (const) vector reference for the first three elements
      of this vector.  May be used when switching from homogeneous
      coordinates to normal, or when refering to a subvector of a row
      or a column inside a small matrix.  (shadowed from base class).
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

    /*! Return a (non-const) vector reference for the first four
      elements of this vector.  May be used when switching from
      homogeneous coordinates to normal, or when refering to a
      subvector of a row or a column inside a small matrix.
    */
    vctDynamicVectorRef<_elementType> XYZW(void) {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicVectorRef<_elementType>(4, Pointer(0), this->stride());
    }

    /*! Return a (const) vector reference for the first four elements
      of this vector.  May be used when switching from homogeneous
      coordinates to normal, or when refering to a subvector of a row
      or a column inside a small matrix.  (shadowed from base class).
    */
    vctDynamicConstVectorRef<_elementType> XYZW(void) const {
        CMN_ASSERT(this->size() > 3);
        return vctDynamicConstVectorRef<_elementType>(4, Pointer(0), this->stride());
    }

    /*!  Cross Product of two vectors.  This method uses #CMN_ASSERT
      to check that the size of the vector is 3, and can only be
      performed on arguments vectors of size 3.
    */
    template<class __vectorOwnerType1, class __vectorOwnerType2>
    inline void CrossProductOf(
            const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & inputVector1,
            const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & inputVector2) 
    {
        CMN_ASSERT(this->size() == 3);
        CMN_ASSERT(inputVector1.size() == 3);
        CMN_ASSERT(inputVector2.size() == 3);
        (*this)[0] = inputVector1[1] *  inputVector2[2] - inputVector1[2] * inputVector2[1];
        (*this)[1] = inputVector1[2] *  inputVector2[0] - inputVector1[0] * inputVector2[2];
        (*this)[2] = inputVector1[0] *  inputVector2[1] - inputVector1[1] * inputVector2[0];    
    }

    //@}



    /*! \name Binary elementwise operations between two vectors.
      Store the result of op(vector1, vector2) to a third vector. */
    //@{
    /*! Binary elementwise operations between two vectors.  For each
      element of the vectors, performs \f$ this[i] \leftarrow
      op(vector1[i], vector2[i])\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ElementwiseProductOf), a division (ElementwiseRatioOf), a
      minimum (ElementwiseMinOf) or a maximum (ElementwiseMaxOf).

      \param vector1 The first operand of the binary operation

      \param vector2 The second operand of the binary operation
      
      \return The vector "this" modified.
    */
    template<class __vectorOwnerType1, class __vectorOwnerType2>
    inline const ThisType & SumOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & vector1,
                                  const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector2) {
        vctDynamicVectorLoopEngines::
            VoViVi< typename vctBinaryOperations<value_type>::Addition >
            ::Run(*this, vector1, vector2);
        return *this;
    }    
    
    /* documented above */
    template<class __vectorOwnerType1, class __vectorOwnerType2>
    inline const ThisType & DifferenceOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & vector1,
                                         const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector2) {
        vctDynamicVectorLoopEngines::
            VoViVi< typename vctBinaryOperations<value_type>::Subtraction >
            ::Run(*this, vector1, vector2);
        return *this;
    }
    
    /* documented above */
    template<class __vectorOwnerType1, class __vectorOwnerType2>
    inline const ThisType & ElementwiseProductOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & vector1,
                                                 const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector2) {
        vctDynamicVectorLoopEngines::
            VoViVi< typename vctBinaryOperations<value_type>::Multiplication >
            ::Run(*this, vector1, vector2);
        return *this;
    }
    
    /* documented above */
    template<class __vectorOwnerType1, class __vectorOwnerType2>
    inline const ThisType & ElementwiseRatioOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & vector1,
                                               const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector2) {
        vctDynamicVectorLoopEngines::
            VoViVi< typename vctBinaryOperations<value_type>::Division >
            ::Run(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template<class __vectorOwnerType1, class __vectorOwnerType2>
    inline const ThisType & ElementwiseMinOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & vector1,
                                             const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector2) {
        vctDynamicVectorLoopEngines::
            VoViVi< typename vctBinaryOperations<value_type>::Minimum >
            ::Run(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template<class __vectorOwnerType1, class __vectorOwnerType2>
    inline const ThisType & ElementwiseMaxOf(const vctDynamicConstVectorBase<__vectorOwnerType1, _elementType> & vector1,
                                             const vctDynamicConstVectorBase<__vectorOwnerType2, _elementType> & vector2) {
        vctDynamicVectorLoopEngines::
            VoViVi< typename vctBinaryOperations<value_type>::Maximum >
            ::Run(*this, vector1, vector2);
        return *this;
    }
    //@}



    /*! \name Binary elementwise operations between two vectors.
      Store the result of op(this, otherVector) back to this vector. */
    //@{
    /*! Store back binary elementwise operations between two
      vectors.  For each element of the vectors, performs \f$
      this[i] \leftarrow op(this[i], otherVector[i])\f$ where
      \f$op\f$ is either an addition (Add), a subtraction
      (Subtraction), a multiplication (ElementwiseMultiply) a division
      (ElementwiseDivide), a minimization (ElementwiseMin) or a
      maximisation (ElementwiseMax).
      
      \param otherVector The second operand of the binary operation
      (this[i] is the first operand)
      
      \return The vector "this" modified.
    */
    template<class __vectorOwnerType>
    inline ThisType & Add(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVi<typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template<class __vectorOwnerType>
    inline ThisType & Subtract(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Run(*this, otherVector);
        return *this;
    }
    
    /* documented above */
    template<class __vectorOwnerType>
    inline ThisType & ElementwiseMultiply(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Run(*this, otherVector);
        return *this;
    }
    
    /* documented above */
    template<class __vectorOwnerType>
    inline ThisType & ElementwiseDivide(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Run(*this, otherVector);
        return *this;
    }
    
    /* documented above */
    template<class __vectorOwnerType>
    inline ThisType & ElementwiseMin(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template<class __vectorOwnerType>
    inline ThisType & ElementwiseMax(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template<class __vectorOwnerType>
    inline const ThisType & operator += (const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        return this->Add(otherVector);
    }
    
    /* documented above */
    template<class __vectorOwnerType>
    inline const ThisType & operator -= (const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        return this->Subtract(otherVector);
    }
    //@}


    /*! \name Binary elementwise operations between two vectors.
      Operate on both elements and store values in both. */
    //@{
    /*! Swap the elements of both vectors with each other.
    */
    template<class __vectorOwnerType>
    inline ThisType & SwapElementsWith(vctDynamicVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VioVio<typename vctStoreBackBinaryOperations<value_type>::Swap >::
            Run(*this, otherVector);
        return *this;
    }
    //@}
   
    /*! \name Binary elementwise operations a vector and a scalar.
      Store the result of op(vector, scalar) to a third vector. */
    //@{
    /*! Binary elementwise operations between a vector and a scalar.
      For each element of the vector "this", performs \f$ this[i]
      \leftarrow op(vector[i], scalar)\f$ where \f$op\f$ is either
      an addition (SumOf), a subtraction (DifferenceOf), a
      multiplication (ProductOf), a division (RatioOf), a minimum
      (MinOf) or a maximum (MaxOf).

      \param vector The first operand of the binary operation.
      \param scalar The second operand of the binary operation.
      
      \return The vector "this" modified.
    */
    template<class __vectorOwnerType>
    inline const ThisType & SumOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector, 
                                  const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VoViSi< typename vctBinaryOperations<value_type>::Addition >::
            Run(*this, vector, scalar);
        return *this;
    }
    
    /* documented above */
    template<class __vectorOwnerType>
    inline const ThisType & DifferenceOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector,
                                         const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VoViSi< typename vctBinaryOperations<value_type>::Subtraction >::
            Run(*this, vector, scalar);
        return *this;
    }
    
    /* documented above */    
    template<class __vectorOwnerType>
    inline const ThisType & ProductOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector,
                                      const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VoViSi< typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, vector, scalar);
        return *this;
    }
    
    /* documented above */
    template<class __vectorOwnerType>
    inline const ThisType & RatioOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector,
                                    const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VoViSi< typename vctBinaryOperations<value_type>::Division >::
            Run(*this, vector, scalar);
        return *this;
    }

    /* documented above */
    template<class __vectorOwnerType>
    inline const ThisType & MinOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector,
                                  const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VoViSi<typename vctBinaryOperations<value_type>::Minimum>::
            Run(*this, vector, scalar);
        return *this;
    }

    /* documented above */
    template<class __vectorOwnerType>
    inline const ThisType & MaxOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector,
                                  const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VoViSi< typename vctBinaryOperations<value_type>::Maximum >::
            Run(*this, vector, scalar);
        return *this;
    }
    //@}
    


    /*! \name Binary elementwise operations a scalar and a vector.
      Store the result of op(scalar, vector) to a third vector. */
    //@{
    /*! Binary elementwise operations between a scalar and a vector.
      For each element of the vector "this", performs \f$ this[i]
      \leftarrow op(scalar, vector[i])\f$ where \f$op\f$ is either an
      addition (SumOf), a subtraction (DifferenceOf), a multiplication
      (ProductOf) or a division (RatioOf).

      \param scalar The first operand of the binary operation.
      \param vector The second operand of the binary operation.
      
      \return The vector "this" modified.
    */
    template<class __vectorOwnerType>
    inline const ThisType & SumOf(const value_type scalar,
                                  const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoSiVi< typename vctBinaryOperations<value_type>::Addition >::
            Run(*this, scalar, vector);
        return *this;
    }
    
    /* documented above */
    template<class __vectorOwnerType>
    inline const ThisType & DifferenceOf(const value_type scalar,
                                         const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoSiVi< typename vctBinaryOperations<value_type>::Subtraction >::
            Run(*this, scalar, vector);
        return *this;
    }
    
    /* documented above */
    template<class __vectorOwnerType>
    inline const ThisType & ProductOf(const value_type scalar,
                                      const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoSiVi< typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar, vector);
        return *this;
    }
    
    /* documented above */
    template<class __vectorOwnerType>
    inline const ThisType & RatioOf(const value_type scalar,
                                    const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoSiVi< typename vctBinaryOperations<value_type>::Division >::
            Run(*this, scalar, vector);
        return *this;
    }

    /* documented above */
    template<class __vectorOwnerType>
    inline const ThisType & MinOf(const value_type scalar,
                                  const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoSiVi< typename vctBinaryOperations<value_type>::Minimum >::
            Run(*this, scalar, vector);
        return *this;
    }

    /* documented above */
    template<class __vectorOwnerType>
    inline const ThisType & MaxOf(const value_type scalar,
                                  const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & vector) {
        vctDynamicVectorLoopEngines::
            VoSiVi< typename vctBinaryOperations<value_type>::Maximum >::
            Run(*this, scalar, vector);
        return *this;
    }
    //@}



    /*! \name Binary elementwise operations between a vector and a scalar.
      Store the result of op(this, scalar) back to this vector. */
    //@{
    /*! Store back binary elementwise operations between a vector
      and a scalar.  For each element of the vector "this", performs
      \f$ this[i] \leftarrow op(this[i], scalar)\f$ where \f$op\f$ is
      either an addition (Add), a subtraction (Subtract), a
      multiplication (Multiply), a division (Divide), a minimum (Min)
      or a maximum (Max).

      \param scalar The second operand of the binary operation
      (this[i] is the first operand.
      
      \return The vector "this" modified.
    */
    inline ThisType & Add(const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Run(*this, scalar);
        return *this;
    }
    
    /* documented above */
    inline ThisType & Subtract(const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Run(*this, scalar);
        return *this;
    }
    
    /* documented above */
    inline ThisType & Multiply(const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar);
        return *this;
    }
    
    /* documented above */
    inline ThisType & Divide(const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Run(*this, scalar);
        return *this;
    }
    
    /* documented above */
    inline ThisType & Min(const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Run(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Max(const value_type scalar) {
        vctDynamicVectorLoopEngines::
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
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

    template<class __vectorOwnerType>
    inline ThisType & MultAdd(const value_type scalar,
    const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector)
    {
        vctDynamicVectorLoopEngines::
            VioSiVi<
            typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Run(*this, scalar, otherVector);
        return *this;
    }



    /*! \name Binary elementwise operations between a vector and a
      matrix.  Store the result of op() to a third vector (this
      vector). */
    //@{

    /*!
      Product of a matrix and a vector.
      
      \param inputMatrix The first operand of the binary operation
      
      \param inputVector The second operand of the binary operation
      
      \return The vector "this" modified.
    */
    template<class __matrixOwnerType, class __vectorOwnerType>
    inline const ThisType & ProductOf(const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & inputMatrix,
                                      const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & inputVector) {
        vctMultiplyMatrixVector(*this, inputMatrix, inputVector);
        return *this;
    }


    /*!
      Product of a vector and a matrix.
      
      \param inputVector The first operand of the binary operation
      
      \param inputMatrix The second operand of the binary operation
      
      \return The vector "this" modified.
    */
    template<class __vectorOwnerType, class __matrixOwnerType>
    inline const ThisType & ProductOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & inputVector,
                                      const vctDynamicConstMatrixBase<__matrixOwnerType, _elementType> & inputMatrix) {
        vctMultiplyVectorMatrix(*this, inputVector, inputMatrix);
        return *this;
    }

    //@}


    /*! Define a Subvector class for compatibility with the fixed size vectors.
      A subvector has the same stride as the parent container.

      Example:

      typedef vctDynamicVector<double> VectorType;
      VectorType v(9);
      VectorType::Subvector::Type first4(v, 0, 4);
      VectorType::Subvector::Type last5(v, 4, 5);

      \note There is no straightforward way to define a fixed-size
      subvector of a dynamic vector, because the stride of the dynamic
      vector is not known in compilation time.  A way to do it is:

      vctFixedSizeVectorRef<double, 3 /( add stride here if necessary )/> firstThree(v, 0);
      vctFixedSizeVectorRef<double, 3 /( add stride here if necessary )/> lastThree(v, 6);
    */
#ifndef SWIG
    class Subvector
    {
    public:
        typedef vctDynamicVectorRef<value_type> Type;
    };
#endif

    /*! \name Unary elementwise operations.
      Store the result of op(vector) to another vector. */
    //@{
    /*! Unary elementwise operations on a vector.  For each element of
      the vector "this", performs \f$ this[i] \leftarrow
      op(otherVector[i])\f$ where \f$op\f$ can calculate the absolute
      value (AbsOf), the opposite (NegationOf) or the normalized
      version (NormalizedOf).

      \param otherVector The operand of the unary operation.
      
      \return The vector "this" modified.
    */
    template<class __vectorOwnerType>
    inline ThisType & AbsOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type>::AbsValue>::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template<class __vectorOwnerType>
    inline ThisType & NegationOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type>::Negation>::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template<class __vectorOwnerType>
    inline ThisType & FloorOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type>::Floor>::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template<class __vectorOwnerType>
    inline ThisType & CeilOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) {
        vctDynamicVectorLoopEngines::
            VoVi<typename vctUnaryOperations<value_type>::Ceil>::
            Run(*this, otherVector);
        return *this;
    }

    /* documented above */
    template<class __vectorOwnerType>
    inline ThisType & NormalizedOf(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector) throw(std::runtime_error) {
        *this = otherVector;
        this->NormalizedSelf();
        return *this;
    }
    //@}

    /*! \name Store back unary elementwise operations.
      Store the result of op(this) to this vector. */
    //@{
    /*! Unary elementwise operations on a vector.  For each element of
      the vector "this", performs \f$ this[i] \leftarrow
      op(this[i])\f$ where \f$op\f$ can calculate the absolute value
      (AbsSelf), the opposite (NegationSelf) or the normalized version
      (NormalizedSelf).
      
      \return The vector "this" modified.
    */
    inline const ThisType & AbsSelf(void) {
        vctDynamicVectorLoopEngines::
            Vio<typename vctStoreBackUnaryOperations<value_type>::MakeAbs>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline const ThisType & NegationSelf(void) {
        vctDynamicVectorLoopEngines::
            Vio<typename vctStoreBackUnaryOperations<value_type>::MakeNegation>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline const ThisType & FloorSelf(void) {
        vctDynamicVectorLoopEngines::
            Vio<typename vctStoreBackUnaryOperations<value_type>::MakeFloor>::
            Run(*this);
        return *this;
    }

    /* documented above */
    inline const ThisType & CeilSelf(void) {
        vctDynamicVectorLoopEngines::
            Vio<typename vctStoreBackUnaryOperations<value_type>::MakeCeil>::
            Run(*this);
        return *this;
    }

    inline const ThisType & NormalizedSelf(void) throw(std::runtime_error) {
        value_type norm = value_type(this->Norm());
        if (norm >= TypeTraits::Tolerance()) {
            this->Divide(norm);
        } else {
            cmnThrow(std::runtime_error("Division by quasi zero detected in vctDynamicVector NormalizedSelf()"));
        }
        return *this;
    }
    //@}


};


#endif // _vctDynamicVectorBase_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicVectorBase.h,v $
//  Revision 1.39  2006/05/21 11:27:06  ofri
//  vct[FixedSize|Dynamic] vectors and matrices: implmented a new operation
//  MultAdd, of the form CioSiCi.
//
//  Revision 1.38  2006/03/18 03:26:02  anton
//  vctDynamicVectorBase: Corrected message for exception send by Assign(...)
//
//  Revision 1.37  2006/02/12 18:32:38  ofri
//  vctDynaic[Const]VectorBase : Extended named subvector methods YZ(), YZW(),
//  XZ(), XW(), ZW() etc.
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
//  Revision 1.34  2005/10/06 16:56:37  anton
//  Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
//  Revision 1.33  2005/09/27 18:01:25  anton
//  cisstVector: Use CMN_ASSERT instead of assert for vectors and matrices.
//
//  Revision 1.32  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.31  2005/09/24 00:01:04  anton
//  cisstVector: Use cmnThrow for all exceptions.
//
//  Revision 1.30  2005/09/01 06:05:00  anton
//  vctDynamicVectorBase:  Added method ConcatenationOf() to replicate
//  method from vctFixedSizeVectorBase.
//
//  Revision 1.29  2005/08/11 21:36:22  anton
//  vctDynamicVectorBase.h: Put back the #include <assert.h> removed in
//  previous revision.
//
//  Revision 1.28  2005/08/11 20:35:39  anton
//  vctDynamicVector: Added Assign and ctor from va_list (see ticket #64, also
//  has flaws listed in tickets #136 and #150).
//
//  Revision 1.27  2005/08/04 15:31:57  anton
//  cisstVector: Replaced RangeCheck() by ThrowUnlessValidIndex().  For dynamic
//  matrices, Row() and Col() now check the index and throw using
//  ThrowUnlessValid{Row,Col}Index().
//
//  Revision 1.26  2005/07/19 15:25:36  anton
//  vctDynamicVectorBase: Moved the const Pointer() method below the Pointer()
//  method so that one can easily find it.
//
//  Revision 1.25  2005/06/16 03:38:29  anton
//  Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
//  VC 7.1 (.Net 2003).
//
//  Revision 1.24  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.23  2005/04/04 19:48:34  anton
//  cisstVector: In base types for dynamic containers, removed default value for
//  _elementType (was based on _ownerType::value_type) to -1- avoid issues with
//  SWIG and -2- force explicit declaration of _elementType whenever a base type
//  is used.
//
//  Revision 1.22  2005/03/31 16:25:46  anton
//  vctDynamicVectorBase: In Assign(value_type *), create a vctDynamicConstVectorRef
//  to provide the method size() to the engine.
//
//  Revision 1.21  2005/03/31 15:51:02  anton
//  cisstVector: Found a couple of _size, replaced by size().
//
//  Revision 1.20  2005/01/21 22:31:20  ofri
//  Added method vctDynamicVectorBase::Assign(vctFixedSizeConstVectorBase) --
//  long time needed...  Compilation and existing tests run successfully on linux.
//
//  Revision 1.19  2005/01/11 22:42:18  anton
//  cisstVector:  Added normalization methods to the vectors.  Removed useless
//  code from vctQuaternion and added some tests.  See ticket #110.
//
//  Revision 1.18  2005/01/06 18:54:00  anton
//  cisstVector: Introduction of type CopyType as requested in ticket #113.
//
//  Revision 1.17  2004/12/10 21:59:07  ofri
//  vct[FixedSize|Dynamic](Const)VectorBase: Added methods XY(), XYZ(), XYZW()
//  to access subvectors easily.  These are useful when handling barycentric
//  coordinates.
//
//  Revision 1.16  2004/12/07 04:12:26  ofri
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
//  Revision 1.15  2004/11/30 23:36:05  anton
//  cisstVector: Added the Element() method to access an element without range
//  check.  See ticket #79.
//
//  Revision 1.14  2004/11/18 20:57:30  ofri
//  RE: ticket #92.  Redefined all operator= I could find to return a ThisType &
//  instead of const ThisType &.  This included corresponding revision of the
//  Assign methods, and also definition from scratch of operator= for fixed and
//  dynamic matrix ref.
//
//  Revision 1.13  2004/11/11 20:35:46  anton
//  cisstVector: *: Added a vctContainerTraits to centralize the declarations
//  of size_type, difference_type, reference, const_reference, etc. *: All
//  iterators are now derived from std::iterator. *: Added some missing typedef
//  for iterators.
//
//  Revision 1.12  2004/11/01 16:57:06  ofri
//  Added the method W() to access the fourth element of a vector
//
//  Revision 1.11  2004/10/26 15:20:52  ofri
//  Updating subsequence interfaces (ticket #76) and interfaces between fixed
//  and dynamic vectors/matrices (ticket #72).
//  *: vctFixedSize(Const)VectorBase::Get(Const)Subsequence now deprecated.
//  Replaced by Get(Const)Subvector with equal stride to the parent.
//  *: Added constructors for [Fixed|Dynamic][Vector|Matrix]Ref from the other
//  storage type.
//
//  Revision 1.10  2004/10/25 19:19:54  anton
//  cisstVector:  Created vctForwardDeclarations.h and removed forward
//  declarations of classes in all other files.  Added some constructors
//  for vector references to reference fixed size from dynamic and vice versa.
//
//  Revision 1.9  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.8  2004/10/22 18:07:06  anton
//  Doxygen: Made sure that all the classes are documented for Doxygen.
//
//  Revision 1.7  2004/10/14 18:57:15  ofri
//  vctDynamicConstVectorBase and vctDynamicVectorBase: Added operator ().
//  See ticket #72.
//
//  Revision 1.6  2004/10/07 21:10:34  ofri
//  Added method vctDynamicVectorBase::Assign(_elementType *)
//
//  Revision 1.5  2004/10/07 19:45:46  anton
//  cisstVector: Added % operator and tested CrossProduct for dynamic vectors.
//  This will close ticket #23.
//
//  Revision 1.4  2004/09/24 20:20:42  anton
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
//  Revision 1.3  2004/09/21 20:10:12  anton
//  cisstVector:  for dynamic vectors, added size dependant methods (X(), Y(),
//  Z() and CrossProductOf()) as well as ProductOf(matrix, vector) and
//  ProductOf(vector, matrix).
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
//  Revision 1.12  2004/07/27 20:08:06  anton
//  cisstVector: Added some Doxygen documentation.  Minor code changes to
//  keep the dynamic vectors in sync with the fixed size ones.
//
//  Revision 1.11  2004/07/23 18:02:35  anton
//  vctDynamic vector and vectors:
//  *: Added comparison operators (==, !=, <=, etc) for sequences and scalars
//  *: Added Abs, Negation, AbsOf, NegationOf, AbsSelf, NegationSelf methods
//  *: Added Add, Subtract, Multiply, Divide for scalars as well as
//     +=, -=, /=, *= for vector and scalar
//  *: Added global operators +, -, *, / for vector/vector, vector/scalar,
//     scalar/vector (whenever it was pertinent)
//  *: Added loop engined {quote: Vio} and {quote: SoViSi}
//
//  Revision 1.10  2004/07/16 20:10:14  anton
//  vctDynamicSequence: Added VoSiVi and VoViSi engines and methods.
//
//  Revision 1.9  2004/07/16 14:38:05  anton
//  vctDynamicSequenceBase:  Added = operator and methods based on VioVi and
//  VoViVi engines.
//
//  Revision 1.8  2004/07/15 17:57:29  anton
//  cisstVector engines using input scalars rely on copies, not reference
//  anymore since the scalar used could be modified by the operation itself
//  (see ticket #51).  We decided to use a const copy even if this might be
//  at a slight efficiency cost.  This modification has an impact on all engines
//  (loop/recursive, sequence/matrix) and we decided to also modify the user API
//  (e.g. sequence.RatioOf(const sequence &, const scalar)) so that the internal
//  behavior can be deduced from the signature on the method.
//
//  Revision 1.7  2004/07/13 16:25:39  ofri
//  Updating vctDynamicSequenceBase: 1) now templated by _elementType
//  as well as _sequenceOwnerType to enforce type compatibility during compilation.
//  2) Function declarations now enforce type compatibility
//
//  Revision 1.6  2004/07/13 01:50:19  ofri
//  vctDynamicSequenceBase: created method Add. *NOT TESTED YET*.
//
//  Revision 1.5  2004/07/12 20:31:30  anton
//  vctDynamicSequenceBase:
//  #1: Incorrect definition of most iterators (bad copy/paste)
//  #2: Added one Assign method and SetAll
//
//  Revision 1.4  2004/07/06 19:42:50  ofri
//  Giving the dynamic vector classes "standard look and feel" in terms of type
//  members, constructors, and indentations.  I made sure that the corresponding
//  example compiles and runs successfully.
//
//  Revision 1.3  2004/07/06 17:47:11  anton
//  Dynamic Sequences:
//  *: Compilation error re. const and const_iterator
//  *: Added some iterator methods to sequences
//  *: Compilation errors re. the order (size, stride, data) in constructor.  The order is now (size, data, stride = 1) everywhere to be consistant
//  *: The method Resize is now resize (STL spelling)
//
//  Revision 1.2  2004/07/02 20:14:07  anton
//  Code for VarStrideSequenceIterator.  Added the class as well as some code to
//  try it.  An example is provided in examples/vectorTutorial/main.cpp.
//
//  Revision 1.1  2004/07/02 16:16:45  anton
//  Massive renaming in cisstVector for the addition of dynamic size vectors
//  and matrices.  The dynamic vectors are far from ready.
//
//
// ****************************************************************************
