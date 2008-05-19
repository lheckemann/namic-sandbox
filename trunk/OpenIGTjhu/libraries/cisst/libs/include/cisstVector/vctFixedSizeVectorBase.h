/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeVectorBase.h,v 1.34 2006/05/21 11:27:06 ofri Exp $
  
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
  \brief Declaration of vctFixedSizeVectorBase
 */

#ifndef _vctFixedSizeVectorBase_h
#define _vctFixedSizeVectorBase_h


#include <cstdarg>

#include <cisstVector/vctFixedSizeConstVectorBase.h>


#ifndef DOXYGEN
// forward declaration of auxiliary function to multiply matrix*vector
template<unsigned int _resultSize, int _resultStride, class _resultElementType, class _resultDataPtrType,
unsigned int _matrixCols, int _matrixRowStride, int _matrixColStride, class _matrixDataPtrType,
int _vectorStride, class _vectorDataPtrType>
inline void MultiplyMatrixVector(
    vctFixedSizeVectorBase<_resultSize, _resultStride, _resultElementType, _resultDataPtrType> & result,
    const vctFixedSizeConstMatrixBase<_resultSize, _matrixCols, _matrixRowStride, _matrixColStride, 
    _resultElementType, _matrixDataPtrType> & matrix,
    const vctFixedSizeConstVectorBase<_matrixCols, _vectorStride, _resultElementType, _vectorDataPtrType> & vector);

// forward declaration of auxiliary function to multiply vector*matrix
template<unsigned int _resultSize, int _resultStride, class _resultElementType, class _resultDataPtrType,
unsigned int _vectorSize, int _vectorStride, class _vectorDataPtrType,
int _matrixRowStride, int _matrixColStride, class _matrixDataPtrType >
inline void MultiplyVectorMatrix(
    vctFixedSizeVectorBase<_resultSize, _resultStride, _resultElementType, _resultDataPtrType> & result,
    const vctFixedSizeConstVectorBase<_vectorSize, _vectorStride, _resultElementType, _vectorDataPtrType> & vector,
    const vctFixedSizeConstMatrixBase<_vectorSize, _resultSize, _matrixRowStride, _matrixColStride, 
    _resultElementType, _matrixDataPtrType> & matrix);
#endif // DOXYGEN


/*! 
  \brief A template for a fixed length vector with fixed spacing in
  memory.

  This class defines a vector with read/write operations.  It
  extends vctFixedSizeConstVectorBase with non-const methods.  See
  the base class for more documentation.

  \sa vctFixedSizeConstVectorBase
*/
template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
class vctFixedSizeVectorBase : public vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType>
{
 public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! Type traits for the vector. Based on type of elements, size,
      stride and data representation, it defines array, pointer,
      etc. (see vctFixedSizeVectorTraits). */
    typedef vctFixedSizeVectorTraits<_elementType, _size, _stride> VectorTraits;
    /* documented in the base class */
    typedef vctFixedSizeVectorBase<_size, _stride, _elementType, _dataPtrType> ThisType;
    /*! Type of the base class. */
    typedef vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> BaseType;
    typedef typename BaseType::CopyType CopyType;
    typedef cmnTypeTraits<value_type> TypeTraits;

    /* Declare the container-defined typed required by STL, plus the
       types completed by our traits class */
    typedef typename VectorTraits::iterator iterator;
    typedef typename VectorTraits::const_iterator const_iterator;
    typedef typename VectorTraits::reverse_iterator reverse_iterator;
    typedef typename VectorTraits::const_reverse_iterator const_reverse_iterator;

    /*! type of overlay row matrix over this sequence */
    //@{
    typedef typename BaseType::RowConstMatrixRefType RowConstMatrixRefType;
    typedef typename BaseType::RowMatrixRefType RowMatrixRefType;
    //@}
    /*! type of overlay column matrix over this sequence */
    //@{
    typedef typename BaseType::ColConstMatrixRefType ColConstMatrixRefType;
    typedef typename BaseType::ColMatrixRefType ColMatrixRefType;
    //@}

    enum {SIZE = BaseType::SIZE};
    enum {STRIDE = BaseType::STRIDE};
    enum {SIZEMINUSONE = SIZE - 1};

    /*! Returns an iterator on the first element (STL
      compatibility). */
    inline iterator begin() {
        return iterator(this->Data);
    }

    
    /* documented in base class */
    inline const_iterator begin() const {
        return BaseType::begin();
    }
    
    
    /*! Returns an iterator on the last element (STL
      compatibility). */
    iterator end() {
        return iterator(this->Data + STRIDE * SIZE);
    }


    /* documented in base class */
    inline const_iterator end() const {
        return BaseType::end();
    }


    /*! Returns a reverse iterator on the last element (STL
      compatibility). */ 
    reverse_iterator rbegin() {
        return reverse_iterator(this->Data + STRIDE * (SIZE - 1));
    }

    
    /* documented in base class */
    const_reverse_iterator rbegin() const {
        return BaseType::rbegin();
    }

    
    /*! Returns a reverse iterator on the element before first
      (STL compatibility). */ 
    reverse_iterator rend() {
        return reverse_iterator(this->Data - STRIDE);
    }


    /* documented in base class */
    const_reverse_iterator rend() const {
        return BaseType::rend();
    }
    

    /*! Access an element by index.
      \return a reference to the element[index] */    
    reference operator[](size_type index) {
        return *(Pointer(index));
    }

    
    /* documented in base class */
    const_reference operator[](size_type index) const {
        return BaseType::operator[](index);
    }

    /*! Access an element by index (const).  Compare with
      std::vector::at.  This method can be a handy substitute for the
      overloaded operator [] when operator overloading is unavailable
      or inconvenient.
      
      \return a non-const reference to element[index] */
    reference at(size_type index) throw(std::out_of_range) {
        this->ThrowUnlessValidIndex(index);
        return *(Pointer(index));
    }

    /*! Access an element by index (const).  Compare with
      std::vector::at.  This method can be a handy substitute for the
      overloaded operator [] when operator overloading is unavailable
      or inconvenient.

      \return a const reference to element[index] */
    const_reference at(size_type index) const throw(std::out_of_range) {
        this->ThrowUnlessValidIndex(index);
        return *(Pointer(index));
    }

#ifndef SWIG
    /*! Access an element by index (const).  See method at().
      \return a non-const reference to element[index] */
    reference operator()(size_type index) throw(std::out_of_range) {
        return at(index);
    }

    /*! Access an element by index (const).  See method at().
      \return a const reference to element[index] */
    const_reference operator()(size_type index) const throw(std::out_of_range) {
        return at(index);
    }
#endif


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


    /*! Addition to the STL requirements.  Return a pointer to an element of the
      container, specified by its index.
    */
    pointer Pointer(size_type index = 0) {
        return this->Data + STRIDE * index;
    }


    /* documented in base class */
    const_pointer Pointer(size_type index = 0) const {
        return BaseType::Pointer(index);
    }


    /*! Assign the given value to all the elements.
      \param value the value used to set all the elements of the vector
      \return The value used to set all the elements
    */
    inline value_type SetAll(const value_type & value) {
        return vctFixedSizeVectorRecursiveEngines<_size>::template
            SoVoSi<typename vctBinaryOperations<value_type>::SecondOperand>::
            Unfold(*this, value);
    }

    /*!
      \name Assignment operation between vectors of different types.
      
      \param other The vector to be copied.
    */
    //@{
    template<int __stride, class __elementType, class __dataPtrType>
    inline ThisType & Assign(const vctFixedSizeConstVectorBase<_size, __stride, __elementType, __dataPtrType> & other) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoVi< typename vctUnaryOperations<value_type, __elementType>::Identity >::
            Unfold(*this, other);
        return *this;
    }

    template<int __stride, class __elementType, class __dataPtrType>
    inline ThisType & operator = (const vctFixedSizeConstVectorBase<_size, __stride, __elementType, __dataPtrType> & other) {
        return this->Assign(other);
    }
    //@}

    /*! The following Assign() methods provide a convenient interface for assigning a
      list of values to a vector without a need for type conversion.  They precede the
      use of var-arg, which in turn is incapable of enforcing argument type on the
      arguments following the ellipsis.  This simpler interface is defined for a
      selected collection of vector sizes, namely 1 to 4.  For larger sizes, the
      va_arg interface takes over.

      Each of these implementations checks match between the number of arguments
      and the size of the target vector object.  It throws a std::runtime_error
      if there is a size mismatch.
    */
    //@{
    inline ThisType & Assign(const value_type element0) throw(std::runtime_error)
    {
        if (this->size() != 1) {
            cmnThrow(std::runtime_error("Mismatch between number of arguments assigned (1) and vector size"));
        }
        (*this)[0] = element0;
        return *this;
    }

    inline ThisType & Assign(const value_type element0, const value_type element1) throw(std::runtime_error)
    {
        if (this->size() != 2) {
            cmnThrow(std::runtime_error("Mismatch between number of arguments assigned (2) and vector size"));
        }
        (*this)[0] = element0;
        (*this)[1] = element1;
        return *this;
    }

    inline ThisType & Assign(const value_type element0, const value_type element1,
        const value_type element2) throw(std::runtime_error)
    {
        if (this->size() != 3) {
            cmnThrow(std::runtime_error("Mismatch between number of arguments assigned (3) and vector size"));
        }
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
        return *this;
    }

    inline ThisType & Assign(const value_type element0, const value_type element1,
        const value_type element2, const value_type element3) throw(std::runtime_error)
    {
        if (this->size() != 4) {
            cmnThrow(std::runtime_error("Mismatch between number of arguments assigned (4) and vector size"));
        }
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
        (*this)[3] = element3;
        return *this;
    }
    //@}

    /*! Assign to this vector a set of values provided as independent
      arguments, by using cstdarg macros, that is, an unspecified
      number of arguments, greater than 4.  This function is not using
      a recursive engine, as it may be hard and not worthwhile to use
      a recursive engine with the va_arg ``iterator''.  This operation
      assumes that all the arguments are of type value_type, and that
      their number is equal to the size of the vector.  The arguments
      are passed <em>by value</em>.  The user may need to explicitly
      cast the parameters to value_type to avoid runtime bugs and
      errors.

      \return a reference to this vector.
    */
    inline ThisType & Assign(const value_type element0, const value_type element1, 
        const value_type element2, const value_type element3, const value_type element4, ...)
    {
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
        (*this)[3] = element3;
        (*this)[4] = element4;
        va_list nextArg;
        va_start(nextArg, element4);
        for (unsigned int i = 5; i < _size; ++i) {
            (*this)[i] = static_cast<value_type>(va_arg(nextArg, typename TypeTraits::VaArgPromotion));
        }
        va_end(nextArg);
        return *this;
    }


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
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoVi< typename vctUnaryOperations<value_type>::Identity >::
            Unfold(*this, elements);
        return *this;
    }


    /*! Concatenate a single element at the end of a shorter-by-1 vector to obtain a vector
      of my size.  The concatenation result is stored in this vector.  The function is useful, e.g.,
      when embedding vectors from R^n into R^{n+1} or into homogeneous space.
    */
    template<int __stride, class __elementTypeVector, class __dataPtrType, class __elementType>
    inline ThisType & ConcatenationOf(const vctFixedSizeConstVectorBase<SIZEMINUSONE, __stride, __elementTypeVector, __dataPtrType> & other,
              __elementType last) {
        // recursive copy for the first size-1 elements
        vctFixedSizeVectorRecursiveEngines<SIZEMINUSONE>::template
            VoVi<typename vctUnaryOperations<value_type, __elementTypeVector>::Identity>::
            Unfold(*this, other);
        // cast and assign last element
        (*this)[SIZEMINUSONE] = value_type(last);
        return *this;
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
    value_type & X(void) {
        CMN_ASSERT(_size > 0);
        return *(Pointer(0));
    }

    // we have to redeclare it here, shadowed
    const value_type & X(void) const {
        return BaseType::X();
    }


    /*! Returns the second element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 2. */
    value_type & Y(void) {
        CMN_ASSERT(_size > 1);
        return *(Pointer(1));
    }

    // we have to redeclare it here, shadowed
    const value_type & Y(void) const {
        return BaseType::Y();
    }


    /*! Returns the third element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 3. */
    value_type & Z(void) {
        CMN_ASSERT(_size > 2);
        return *(Pointer(2));
    }

    // we have to redeclare it here, shadowed
    const value_type & Z(void) const {
        return BaseType::Z();
    }

    /*! Returns the fourth element of the vector.  This method uses
      #CMN_ASSERT to check that the size is at least 4. */
    value_type & W(void) {
        CMN_ASSERT(_size > 3);
        return *(Pointer(3));
    }

    // we have to redeclare it here, shadowed
    const value_type & W(void) const {
        return BaseType::W();
    }


    /*! Return a (non-const) vector reference for the first two elements of this
      vector.  May be used when switching from homogeneous coordinates to normal */
    vctFixedSizeVectorRef<_elementType, 2, _stride> XY(void) {
        CMN_ASSERT(_size > 1);
        return vctFixedSizeVectorRef<_elementType, 2, _stride>(Pointer(0));
    }

    /*! Return a (non-const) vector reference for the first and third elements of this
      vector. */
    vctFixedSizeVectorRef<_elementType, 2, 2 * _stride> XZ(void) {
        CMN_ASSERT(_size > 2);
        return vctFixedSizeVectorRef<_elementType, 2, 2 * _stride>(Pointer(0));
    }

    /*! Return a (non-const) vector reference for the first and fourth elements of this
      vector. */
    vctFixedSizeVectorRef<_elementType, 2, 3 * _stride> XW(void) {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeVectorRef<_elementType, 2, 3 * _stride>(Pointer(0));
    }

    /*! Return a (non-const) vector reference for the two elements of this
      vector beginning on the second. */
    vctFixedSizeVectorRef<_elementType, 2, _stride> YZ(void) {
        CMN_ASSERT(_size > 2);
        return vctFixedSizeVectorRef<_elementType, 2, _stride>(Pointer(1));
    }

    /*! Return a (non-const) vector reference for the second and fourth elements
      of this vector. */
    vctFixedSizeVectorRef<_elementType, 2, 2 * _stride> YW(void) {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeVectorRef<_elementType, 2, 2 * _stride>(Pointer(1));
    }

    /*! Return a (non-const) vector reference for the two elements of this
      vector beginning on the third. */
    vctFixedSizeVectorRef<_elementType, 2, _stride> ZW(void) {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeVectorRef<_elementType, 2, _stride>(Pointer(2));
    }

    /*! Return a (const) vector reference for the first two elements of this
      vector.  May be used when switching from homogeneous coordinates to normal */
    vctFixedSizeConstVectorRef<_elementType, 2, _stride> XY(void) const {
        return BaseType::XY();
    }

    /*! Return a (const) vector reference for the first and third elements of this
      vector. */
    vctFixedSizeConstVectorRef<_elementType, 2, 2 * _stride> XZ(void) const {
        return BaseType::XZ();
    }

    /*! Return a (const) vector reference for the first and fourth elements of this
      vector. */
    vctFixedSizeConstVectorRef<_elementType, 2, 3 * _stride> XW(void) const {
        return BaseType::XW();
    }

    /*! Return a (const) vector reference for the two elements of this
      vector beginning on the second. */
    vctFixedSizeConstVectorRef<_elementType, 2, _stride> YZ(void) const {
        return BaseType::YZ();
    }

    /*! Return a (const) vector reference for the second and fourth elements
      of this vector. */
    vctFixedSizeConstVectorRef<_elementType, 2, 2 * _stride> YW(void) const {
        return BaseType::YW();
    }

    /*! Return a (const) vector reference for the two elements of this
      vector beginning on the third. */
    vctFixedSizeConstVectorRef<_elementType, 2, _stride> ZW(void) const {
        return BaseType::ZW();
    }

    /*! Return a (non-const) vector reference for the first three elements of this
      vector.  May be used when switching from homogeneous coordinates to normal,
      or when refering to a subvector of a row or a column inside a small matrix.
    */
    vctFixedSizeVectorRef<_elementType, 3, _stride> XYZ(void) {
        CMN_ASSERT(_size > 2);
        return vctFixedSizeVectorRef<_elementType, 3, _stride>(Pointer(0));
    }


    /*! Return a (non-const) vector reference for the second, third and fourth elements
      of this vector. */
    vctFixedSizeVectorRef<_elementType, 3, _stride> YZW(void) {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeVectorRef<_elementType, 3, _stride>(Pointer(1));
    }

    /*! Return a (const) vector reference for the first three elements of this
      vector.  May be used when switching from homogeneous coordinates to normal,
      or when refering to a subvector of a row or a column inside a small matrix.
      (shadowed from base class).
    */
    vctFixedSizeConstVectorRef<_elementType, 3, _stride> XYZ(void) const {
        CMN_ASSERT(_size > 2);
        return vctFixedSizeConstVectorRef<_elementType, 3, _stride>(Pointer(0));
    }

    /*! Return a (const) vector reference for the second, third and fourth elements
      of this vector. */
    vctFixedSizeConstVectorRef<_elementType, 3, _stride> YZW(void) const {
        return BaseType::YZW();
    }

    /*! Return a (non-const) vector reference for the first four elements of this
      vector.  May be used when switching from homogeneous coordinates to normal,
      or when refering to a subvector of a row or a column inside a small matrix.
    */
    vctFixedSizeVectorRef<_elementType, 4, _stride> XYZW(void) {
        CMN_ASSERT(_size > 3);
        return vctFixedSizeVectorRef<_elementType, 4, _stride>(Pointer(0));
    }

    /*! Return a (const) vector reference for the first four elements of this
      vector.  May be used when switching from homogeneous coordinates to normal,
      or when refering to a subvector of a row or a column inside a small matrix.
      (shadowed from base class).
    */
    vctFixedSizeConstVectorRef<_elementType, 4, _stride> XYZW(void) const {
        return BaseType::XYZW();
    }
    //@}

    /*! Create an overlay matrix (MatrixRef) on top of this vector and
      return it.
    */
    //@{
    RowConstMatrixRefType AsRowMatrix() const
    {
        return BaseType::AsRowMatrix();
    }

    RowMatrixRefType AsRowMatrix()
    {
        return RowMatrixRefType(Pointer());
    }

    ColConstMatrixRefType AsColMatrix() const
    {
        return BaseType::AsColMatrix();
    }

    ColMatrixRefType AsColMatrix()
    {
        return ColMatrixRefType(Pointer());
    }
    //@}


    /*! Cross Product of two vectors.  This method uses #CMN_ASSERT to
      check that the size of the vector is 3, and can only be
      performed on arguments vectors of size 3.
    */
    template<int __stride1, class __dataPtr1Type, int __stride2, class __dataPtr2Type>
        inline void CrossProductOf(
            const vctFixedSizeConstVectorBase<3, __stride1, _elementType, __dataPtr1Type> & inputVector1,
            const vctFixedSizeConstVectorBase<3, __stride2, _elementType, __dataPtr2Type> & inputVector2) 
    {
        CMN_ASSERT(SIZE == 3);
        (*this)[0] = inputVector1[1] *  inputVector2[2] - inputVector1[2] * inputVector2[1];
        (*this)[1] = inputVector1[2] *  inputVector2[0] - inputVector1[0] * inputVector2[2];
        (*this)[2] = inputVector1[0] *  inputVector2[1] - inputVector1[1] * inputVector2[0];    
    }


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
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline const ThisType & SumOf(const vctFixedSizeConstVectorBase<_size, __stride1, value_type, __dataPtrType1> & vector1, 
                                  const vctFixedSizeConstVectorBase<_size, __stride2, value_type, __dataPtrType2> & vector2) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViVi< typename vctBinaryOperations<value_type>::Addition >
            ::Unfold(*this, vector1, vector2);
        return *this;
    }
    
    
    /* documented above */
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline const ThisType & DifferenceOf(const vctFixedSizeConstVectorBase<_size, __stride1, value_type, __dataPtrType1> & vector1, 
                                         const vctFixedSizeConstVectorBase<_size, __stride2, value_type, __dataPtrType2> & vector2) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViVi< typename vctBinaryOperations<value_type>::Subtraction >
            ::Unfold(*this, vector1, vector2);
        return *this;
    }
    
    /* documented above */
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline const ThisType & ElementwiseProductOf(const vctFixedSizeConstVectorBase<_size, __stride1, value_type, __dataPtrType1> & vector1, 
                                                 const vctFixedSizeConstVectorBase<_size, __stride2, value_type, __dataPtrType2> & vector2) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViVi< typename vctBinaryOperations<value_type>::Multiplication >
            ::Unfold(*this, vector1, vector2);
        return *this;
    }
    
    /* documented above */
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline const ThisType & ElementwiseRatioOf(const vctFixedSizeConstVectorBase<_size, __stride1, value_type, __dataPtrType1> & vector1, 
                                               const vctFixedSizeConstVectorBase<_size, __stride2, value_type, __dataPtrType2> & vector2) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViVi< typename vctBinaryOperations<value_type>::Division >
            ::Unfold(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline const ThisType & ElementwiseMinOf(const vctFixedSizeConstVectorBase<_size, __stride1, value_type, __dataPtrType1> & vector1, 
                                             const vctFixedSizeConstVectorBase<_size, __stride2, value_type, __dataPtrType2> & vector2) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViVi< typename vctBinaryOperations<value_type>::Minimum >
            ::Unfold(*this, vector1, vector2);
        return *this;
    }

    /* documented above */
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline const ThisType & ElementwiseMaxOf(const vctFixedSizeConstVectorBase<_size, __stride1, value_type, __dataPtrType1> & vector1, 
                                             const vctFixedSizeConstVectorBase<_size, __stride2, value_type, __dataPtrType2> & vector2) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViVi< typename vctBinaryOperations<value_type>::Maximum >
            ::Unfold(*this, vector1, vector2);
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
    template<int __stride, class __dataPtrType>
    inline ThisType & Add(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Unfold(*this, otherVector);
        return *this;
    }
    
    /* documented above */
    template<int __stride, class __dataPtrType>
    inline ThisType & Subtract(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Unfold(*this, otherVector);
        return *this;
    }
    
    /* documented above */
    template<int __stride, class __dataPtrType>
    inline ThisType & ElementwiseMultiply(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Unfold(*this, otherVector);
        return *this;
    }
    
    /* documented above */
    template<int __stride, class __dataPtrType>
    inline ThisType & ElementwiseDivide(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Unfold(*this, otherVector);
        return *this;
    }
    
    /* documented above */
    template<int __stride, class __dataPtrType>
    inline ThisType & ElementwiseMin(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline ThisType & ElementwiseMax(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline const ThisType & operator += (const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        return this->Add(otherVector);
    }
    
    /* documented above */
    template<int __stride, class __dataPtrType>
    inline const ThisType & operator -= (const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        return this->Subtract(otherVector);
    }
    //@}


    /*! \name Binary elementwise operations between two vectors.
      Operate on both elements and store values in both. */
    //@{
    /*! Swap the elements of both vectors with each other.
    */
    template<int __stride, class __dataPtrType>
    inline ThisType & SwapElementsWith(vctFixedSizeVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioVio< typename vctStoreBackBinaryOperations<value_type>::Swap >::
            Unfold(*this, otherVector);
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
    template<int __stride, class __dataPtrType>
    inline const ThisType & SumOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector, 
                                const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViSi< typename vctBinaryOperations<value_type>::Addition >::
            Unfold(*this, vector, scalar);
        return *this;
    }
    
    /* documented above */
    template<int __stride, class __dataPtrType>
    inline const ThisType & DifferenceOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector,
                                         const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViSi< typename vctBinaryOperations<value_type>::Subtraction >::
            Unfold(*this, vector, scalar);
        return *this;
    }
    
    /* documented above */    
    template<int __stride, class __dataPtrType>
    inline const ThisType & ProductOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector,
                                      const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViSi< typename vctBinaryOperations<value_type>::Multiplication >::
            Unfold(*this, vector, scalar);
        return *this;
    }
    
    /* documented above */
    template<int __stride, class __dataPtrType>
    inline const ThisType & RatioOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector,
                                    const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViSi< typename vctBinaryOperations<value_type>::Division >::
            Unfold(*this, vector, scalar);
        return *this;
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline const ThisType & MinOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector,
                                  const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViSi<typename vctBinaryOperations<value_type>::Minimum>::
            Unfold(*this, vector, scalar);
        return *this;
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline const ThisType & MaxOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector,
                                  const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoViSi< typename vctBinaryOperations<value_type>::Maximum >::
            Unfold(*this, vector, scalar);
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
      (ProductOf), a division (RatioOf), a minimum (MinOf) or a
      maximum (MaxOf).

      \param scalar The first operand of the binary operation.
      \param vector The second operand of the binary operation.
      
      \return The vector "this" modified.
    */
    template<int __stride, class __dataPtrType>
    inline const ThisType & SumOf(const value_type scalar,
                                  const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoSiVi< typename vctBinaryOperations<value_type>::Addition >::
            Unfold(*this, scalar, vector);
        return *this;
    }
    
    /* documented above */
    template<int __stride, class __dataPtrType>
    inline const ThisType & DifferenceOf(const value_type scalar,
                                         const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoSiVi< typename vctBinaryOperations<value_type>::Subtraction >::
            Unfold(*this, scalar, vector);
        return *this;
    }
    
    /* documented above */
    template<int __stride, class __dataPtrType>
    inline const ThisType & ProductOf(const value_type scalar,
                                      const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoSiVi< typename vctBinaryOperations<value_type>::Multiplication >::
            Unfold(*this, scalar, vector);
        return *this;
    }
    
    /* documented above */
    template<int __stride, class __dataPtrType>
    inline const ThisType & RatioOf(const value_type scalar,
                                    const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoSiVi< typename vctBinaryOperations<value_type>::Division >::
            Unfold(*this, scalar, vector);
        return *this;
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline const ThisType & MinOf(const value_type scalar,
                                  const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoSiVi< typename vctBinaryOperations<value_type>::Minimum >::
            Unfold(*this, scalar, vector);
        return *this;
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline const ThisType & MaxOf(const value_type scalar,
                                  const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & vector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoSiVi< typename vctBinaryOperations<value_type>::Maximum >::
            Unfold(*this, scalar, vector);
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
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Addition >::
            Unfold(*this, scalar);
      return *this;
    }

    /* documented above */
    inline ThisType & Subtract(const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Subtraction >::
            Unfold(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Multiply(const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Multiplication >::
            Unfold(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Divide(const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Division >::
            Unfold(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Min(const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Minimum >::
            Unfold(*this, scalar);
        return *this;
    }

    /* documented above */
    inline ThisType & Max(const value_type scalar) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSi< typename vctStoreBackBinaryOperations<value_type>::Maximum >::
            Unfold(*this, scalar);
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


    template<int __stride, class __dataPtrType>
    inline ThisType & MultAdd(const value_type scalar,
    const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector)
    {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VioSiVi<
            typename vctStoreBackBinaryOperations<value_type>::Addition,
            typename vctBinaryOperations<value_type>::Multiplication >::
            Unfold(*this, scalar, otherVector);
        return *this;
    }

    /*! \name Binary elementwise operations between a vector and a matrix.
      Store the result of op() to a third vector. */
    //@{
    /*!
      Product of a matrix and a vector.
      
      \param inputMatrix The first operand of the binary operation
      
      \param inputVector The second operand of the binary operation
      
      \return The vector "this" modified.
    */
    template<unsigned int __matrixCols, int __matrixRowStride, int __matrixColStride, class __matrixDataPtrType,
             int __vectorStride, class __vectorDataPtrType>
    inline const ThisType & ProductOf(const vctFixedSizeConstMatrixBase<_size, __matrixCols, __matrixRowStride, __matrixColStride, _elementType, __matrixDataPtrType> & inputMatrix,
                                      const vctFixedSizeConstVectorBase<__matrixCols, __vectorStride, _elementType, __vectorDataPtrType> & inputVector)
    {
        MultiplyMatrixVector(*this, inputMatrix, inputVector);
        return *this;
    }

    template<unsigned int __vectorSize, int __vectorStride, class __vectorDataPtrType,
             int __matrixRowStride, int __matrixColStride, class __matrixDataPtrType>
    inline const ThisType & ProductOf(const vctFixedSizeConstVectorBase<__vectorSize,  __vectorStride, _elementType, __vectorDataPtrType> & inputVector,
                                      const vctFixedSizeConstMatrixBase<__vectorSize, _size, __matrixRowStride, __matrixColStride, _elementType, __matrixDataPtrType> & inputMatrix )
    {
        MultiplyVectorMatrix(*this, inputVector, inputMatrix);
        return *this;
    }
    //@}



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
    template<int __stride, class __dataPtrType>
    inline ThisType & AbsOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoVi< typename vctUnaryOperations<value_type>::AbsValue >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline ThisType & NegationOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoVi< typename vctUnaryOperations<value_type>::Negation >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline ThisType & FloorOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoVi< typename vctUnaryOperations<value_type>::Floor >::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline ThisType & CeilOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            VoVi< typename vctUnaryOperations<value_type>::Ceil>::
            Unfold(*this, otherVector);
        return *this;
    }

    /* documented above */
    template<int __stride, class __dataPtrType>
    inline const ThisType & NormalizedOf(const vctFixedSizeConstVectorBase<_size, __stride, value_type, __dataPtrType> & otherVector) throw(std::runtime_error) {
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
        vctFixedSizeVectorRecursiveEngines<_size>::template
            Vio< typename vctStoreBackUnaryOperations<value_type>::MakeAbs >::
            Unfold(*this);
        return *this;
    }

    /* documented above */
    inline const ThisType & NegationSelf(void) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            Vio< typename vctStoreBackUnaryOperations<value_type>::MakeNegation >::
            Unfold(*this);
        return *this;
    }

    /* documented above */
    inline const ThisType & FloorSelf(void) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            Vio< typename vctStoreBackUnaryOperations<value_type>::MakeFloor >::
            Unfold(*this);
        return *this;
    }

    /* documented above */
    inline const ThisType & CeilSelf(void) {
        vctFixedSizeVectorRecursiveEngines<_size>::template
            Vio< typename vctStoreBackUnaryOperations<value_type>::MakeCeil >::
            Unfold(*this);
        return *this;
    }

    /* documented above */
    inline const ThisType & NormalizedSelf(void) throw(std::runtime_error) {
        value_type norm = value_type(this->Norm());
        if (norm >= TypeTraits::Tolerance()) {
            this->Divide(norm);
        } else {
            cmnThrow(std::runtime_error("Division by quasi zero detected in vctFixedSizeVector NormalizedSelf()"));
        }
        return *this;
    }
    //@}


#if 0   // eliminating definition of GetSubsequence and GetConstSubsequence methods
    /*! Initialize a subsequence of the container, starting at a
      specified position.  The size and the stride of the subsequence
      are given by the type of the result object.

      A subsequence obtained this way may be non-const.  Use
      GetConstSubsequence to obtain a const subsequence.
    */
    template<class _subsequenceType>
    void GetSubsequence(size_type position, _subsequenceType & result) {
        CMN_ASSERT( (_subsequenceType::STRIDE % ThisType::STRIDE) == 0 );
        CMN_ASSERT( position + 
                    ((_subsequenceType::SIZE-1) * (_subsequenceType::STRIDE / ThisType::STRIDE)) 
                    <= (ThisType::SIZE-1) );
        result.SetRef( Pointer(position) );
    }

    template<class _subsequenceType>
    void GetConstSubsequence(size_type position, _subsequenceType & result) {
        BaseType::GetConstSubsequence(position, result);
    }
#endif  // eliminate definition

    /*! ``Syntactic sugar'' to simplify the creation of a reference to a subvector.
      This class declares a non-const subvector type.
      To declare a subvector object, here's an example.

      typedef vctFixedSizeVector<double, 9> Vector9;
      Vector9 v;
      Vector9::Subvector<3>::Type first3(v, 0);  // first 3 elements of v
      Vector9::Subvector<3>::Type mid3(v, 0);  // middle 3 elements of v
      Vector9::Subvector<3>::Type last3(v, 6);  // last 3 elements of v

      \param _subSize the size of the subvector

      \note the stride of the subvector with respect to its parent container
      is always 1.  That is, the memory strides of the subvector and the
      parent container are equal.  For more sophisticated subsequences,
      the user has to write cusomized code.
    */
    template<unsigned int _subSize>
    class Subvector {
    public:
        typedef vctFixedSizeVectorRef<value_type, _subSize, STRIDE> Type;
    };

};


#endif  // _vctFixedSizeVectorBase_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedSizeVectorBase.h,v $
// Revision 1.34  2006/05/21 11:27:06  ofri
// vct[FixedSize|Dynamic] vectors and matrices: implmented a new operation
// MultAdd, of the form CioSiCi.
//
// Revision 1.33  2005/12/23 21:27:31  anton
// cisstVector: Minor updates for Doxygen 1.4.5.
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
// Revision 1.26  2005/08/13 03:57:34  pkaz
// vctFixedSizeVectorBase.h:  added this-> for gcc 3.4/4.0.
//
// Revision 1.25  2005/08/04 15:31:57  anton
// cisstVector: Replaced RangeCheck() by ThrowUnlessValidIndex().  For dynamic
// matrices, Row() and Col() now check the index and throw using
// ThrowUnlessValid{Row,Col}Index().
//
// Revision 1.24  2005/07/22 19:12:06  ofri
// vctFixedSizeVectorBase: Fixing typo introduced in check-in [1254] which was
// detected in running the tests.
//
// Revision 1.23  2005/07/22 15:34:22  ofri
// vctFixedSizeVectorBase: Following ticket #150, I added versions of the
// Assign() method with 1 to 4 named arguments, and revised the va_arg
// version to have 5 or more arguments.
//
// Revision 1.22  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.21  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.20  2005/01/11 22:42:18  anton
// cisstVector:  Added normalization methods to the vectors.  Removed useless
// code from vctQuaternion and added some tests.  See ticket #110.
//
// Revision 1.19  2005/01/06 18:54:00  anton
// cisstVector: Introduction of type CopyType as requested in ticket #113.
//
// Revision 1.18  2004/12/10 21:59:07  ofri
// vct[FixedSize|Dynamic](Const)VectorBase: Added methods XY(), XYZ(), XYZW()
// to access subvectors easily.  These are useful when handling barycentric
// coordinates.
//
// Revision 1.17  2004/12/07 04:12:26  ofri
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
// Revision 1.16  2004/11/30 23:36:05  anton
// cisstVector: Added the Element() method to access an element without range
// check.  See ticket #79.
//
// Revision 1.15  2004/11/30 21:36:32  anton
// vctFixedSize: Bug in operator vector * matrix, input vector size was
// set to matrix numbers of cols instead of rows.  The method ProductOf() was
// correct.  Cleaned-up layout to be able to read and compare both signatures.
//
// Revision 1.14  2004/11/18 20:57:30  ofri
// RE: ticket #92.  Redefined all operator= I could find to return a ThisType &
// instead of const ThisType &.  This included corresponding revision of the
// Assign methods, and also definition from scratch of operator= for fixed and
// dynamic matrix ref.
//
// Revision 1.13  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
// Revision 1.12  2004/11/01 16:57:06  ofri
// Added the method W() to access the fourth element of a vector
//
// Revision 1.11  2004/10/26 15:20:52  ofri
// Updating subsequence interfaces (ticket #76) and interfaces between fixed
// and dynamic vectors/matrices (ticket #72).
// *: vctFixedSize(Const)VectorBase::Get(Const)Subsequence now deprecated.
// Replaced by Get(Const)Subvector with equal stride to the parent.
// *: Added constructors for [Fixed|Dynamic][Vector|Matrix]Ref from the other
// storage type.
//
// Revision 1.10  2004/10/25 19:19:54  anton
// cisstVector:  Created vctForwardDeclarations.h and removed forward
// declarations of classes in all other files.  Added some constructors
// for vector references to reference fixed size from dynamic and vice versa.
//
// Revision 1.9  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.8  2004/10/22 18:07:06  anton
// Doxygen: Made sure that all the classes are documented for Doxygen.
//
// Revision 1.7  2004/10/14 19:18:01  anton
// cisstVector, fixed size containers: Added operator() based on at() method
// as required per ticket #72.
//
// Revision 1.6  2004/10/08 22:09:18  ofri
// cisstVector: Added methods AsRowMatrix() and AsColMatrix() to the fixed size
// base (see #69).  This compiles successfully on gcc and .net7.  Not tested yet.
// This code is committed for continued work in the coming few days.
//
// Revision 1.5  2004/09/24 20:20:42  anton
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
// Revision 1.4  2004/09/08 14:13:22  anton
// cisstVector and SWIG: Introduced SIZEMINUSONE as an enum to allow SWIG to
// parse the template parameters specified as expressions (_size - 1).  Also
// wrap ConcatenationOf method for fixed size vector.
//
// Revision 1.3  2004/08/14 23:07:35  ofri
// Added default value for the _subStride template argument in
// vctFixedSizeVecorBase::Subsequence
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
// Revision 1.34  2004/08/10 14:01:59  ofri
// 1) Defined concatenation operator & which takes to tempalated sequences
// and returns a fixed size vector object (vctFixedSizeVector.h)
// 2) Moved the definition of subsequence types (template class SubsequenceRef)
// from vctFixedSizeVector to the sequence base header files with appropriate
// forward declarations
//
// Revision 1.33  2004/07/27 20:08:06  anton
// cisstVector: Added some Doxygen documentation.  Minor code changes to
// keep the dynamic vectors in sync with the fixed size ones.
//
// Revision 1.32  2004/07/16 14:41:01  anton
// vctFixed vector and matrix:  Operators +=, -=, *= and /= now return a const
// reference (was non const) to avoid hugly syntax like v += v2 /= v3
//
// Revision 1.31  2004/07/15 17:57:29  anton
// cisstVector engines using input scalars rely on copies, not reference
// anymore since the scalar used could be modified by the operation itself
// (see ticket #51).  We decided to use a const copy even if this might be
// at a slight efficiency cost.  This modification has an impact on all engines
// (loop/recursive, sequence/matrix) and we decided to also modify the user API
// (e.g. sequence.RatioOf(const sequence &, const scalar)) so that the internal
// behavior can be deduced from the signature on the method.
//
// Revision 1.30  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.29  2004/04/06 15:23:15  anton
// Doxygen clean-up
//
// Revision 1.28  2004/04/01 18:18:14  anton
// ElementWise is now Elementwise (w is lower case)
// More systematic implementation of ElementwiseMinWith (+ documentation)
//
// Revision 1.27  2004/03/29 21:19:09  anton
// #ifdef for SWIG
//
// Revision 1.26  2004/03/19 15:27:57  ofri
// Added the method at(index) to access sequence elements without operator
// overloading
//
// Revision 1.25  2004/03/16 20:44:39  ofri
// Added some documentation and methods: ElementwiseMinOf(v,v),
// ElementwiseMaxOf(v,v), ElementwiseMinWith(v), ElementwiseMaxWith(v)
//
// Revision 1.24  2004/02/11 18:38:57  anton
// Added ConcatenationOf
//
// Revision 1.23  2004/02/06 15:35:34  anton
// cmnVaType is now cmnTypeTraits
// Added X(), Y() and Z() const methods, shadowed by non const ones
// Abs and Negation methods now have consistant names and signatures
//
// Revision 1.22  2004/01/23 18:20:01  anton
// Changed the template parameters names of ProductOf(matrix, sequence) and
// ProductOf(sequence, matrix) to something more explicit.
// For helper function MultiplyVectorMatrix, changed order of parameters to
// vector then matrix (an unfortunate copy/paste was matrix, sequence).
//
// Revision 1.21  2004/01/15 15:26:29  anton
// va_arg now relies on cmnVaTypes to resolve some issues with the promotion of types (float)
//
// Revision 1.20  2003/12/24 19:41:34  ofri
// Removed conditional compilation switch.
//
// Revision 1.19  2003/12/24 19:40:00  ofri
// Added operation vctFixedLenghSequenceBase::ProductOf(vector, matrix)
// and auxiliary function MultiplyVectorMatrix
//
// Revision 1.18  2003/12/21 22:55:57  ofri
// Externalized the implementation of
// vctFixedLengthSequenceBase::ProductOf(matrix,vector)
// to the global function MatrixVectorProduct(result, matrix, vector).  This solves
// the problem of double templates in .NET.  The test programs compile and run
// successfully on .NET, gcc, icc.
// The code is committed with a conditional compilation switch to be removed
// when this change becomes final.
//
// Revision 1.17  2003/12/19 21:45:24  anton
// Added sequence.ProductOf(matrix, sequence)
//
// Revision 1.16  2003/12/18 16:17:00  anton
// Changed Sum to SumOf (idem for difference, ratio, product).  Changed MakeNegation to Negate.  For product and ratios between vectors, ProductOf is now ElementWiseProductOf
//
// Revision 1.15  2003/12/16 16:37:44  anton
// The returned type is not infered from the binary incremental operation for all recursive engines SoXxxx
//
// Revision 1.14  2003/12/03 22:52:09  ofri
// 1) Renamed method v3.CrossProduct(v1, v2) to v3.CrossProductOf(v1,v2).
// 2) Templated it over the data ptr of v1 and v2
// 3) Assertion and size forcing to 3
//
// Revision 1.13  2003/11/25 21:37:04  ofri
// Added constructors for vctFixedSizeVector and assignment methods
// to vctFixedLengthSequenceBase, using variable number of arguments
// and value_type pointer
//
// Revision 1.12  2003/11/20 18:16:36  anton
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


