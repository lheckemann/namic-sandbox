/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctMatrixRotation2Base.h,v 1.4 2006/01/20 04:46:36 anton Exp $

  Author(s):  Anton Deguet
  Created on:  2005-12-01

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
  \File 
  \brief Declaration of vctMatrixRotation2Base
 */


#ifndef _vctMatrixRotation2Base_h
#define _vctMatrixRotation2Base_h


#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctAngleRotation2.h>
#include <cisstVector/vctExport.h>


/*!
  \brief Define a rotation matrix for a space of dimension 2

  This class is templated by the type of container used to store the
  rotation matrix.  This class is an internal class, i.e. it is not
  intended for the end-user.  The class which should be used by most
  is vctMatRot2 (eventually vctMatrixRotation2<_elementType>).

  The main goal of this meta rotation matrix class is to ease the
  interface with Python.  For a native Python object, a matrix
  rotation can be defined based on a vctDynamicMatrix which is much
  easier to wrap than a vctFixedSizeMatrix.  For a C++ object accessed
  from Python, the rotation matrix will be defined using
  vctMatrixRotation2Base<vctDynamicMatrixRef<double> >, referring to
  the C++ vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> >.

  \param _containerType The type of the matrix.

  \sa vctFixedSizeMatrix
*/
template <class _containerType>
class vctMatrixRotation2Base: public _containerType
{
public:

    enum {ROWS = 2, COLS = 2};
    enum {DIMENSION = 2};
    typedef _containerType BaseType;
    typedef _containerType ContainerType;
    typedef vctMatrixRotation2Base<ContainerType> ThisType;

    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename ContainerType::value_type);

    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<value_type> TypeTraits;


protected:
    /*! Throw an exception unless this rotation is normalized. */
    inline void ThrowUnlessIsNormalized(void) const throw(std::runtime_error) {
        if (! this->IsNormalized()) {
            cmnThrow(std::runtime_error("vctMatrixRotation2Base: This rotation is not normalized"));
        }
    }

    /*!
      Throw an exception unless the input is normalized.
      \param input An object with \c IsNormalized method.
    */
    template <class _inputType>
    inline void ThrowUnlessIsNormalized(const _inputType & input) const throw(std::runtime_error) {
        if (! input.IsNormalized()) {
            cmnThrow(std::runtime_error("vctMatrixRotation2Base: Input is not normalized"));
        }
    }


public:

    vctMatrixRotation2Base():
        BaseType(Identity())
    {}

    /*! Constructor from 4 elements.  This constructor verifies that
      the matrix created is actually a valid rotation matrix.  If the
      input is not normalized, an exception will be thrown
      (std::runtime_error).  The parameters are given row first so
      that the code remains human readable:

      \code
      vctMatrixRotation2<double> matrix( 0.0, 1.0,
                                        -1.0, 0.0);
      \endcode
    */
    CISST_EXPORT vctMatrixRotation2Base(const value_type & element00, const value_type & element01,
                                        const value_type & element10, const value_type & element11)
        throw(std::runtime_error);


    /*! Const reference to the identity.  In this case, the identity matrix:
      <pre>
      1 0
      0 1
      </pre>
    */
    static CISST_EXPORT const ThisType & Identity();


    /*!
      \name Conversion from normalized input.

      These methods will check that the input is normalized.  If the
      input is not normalized, an exception (\c std::runtime_error)
      will be thrown using cmnThrow().

      \note Since all exceptions are thrown using cmnThrow(), it is
      possible to configure these methods to use \c abort() if the
      normalization requirements are not met (see cmnThrow()).

    */
    //@{

    /*! Conversion from 4 elements. */
    inline const ThisType &
    From(const value_type & element00, const value_type & element01,
         const value_type & element10, const value_type & element11)
        throw(std::runtime_error)
    {
        this->FromRaw(element00, element01,
                      element10, element11);
        this->ThrowUnlessIsNormalized();
        return *this;
    }

    /*!
      Conversion from 2 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <int __stride1, class __dataPtrType1,
              int __stride2, class __dataPtrType2>
    inline const ThisType &
    From(const vctFixedSizeConstVectorBase<2, __stride1, value_type, __dataPtrType1>& v1,
         const vctFixedSizeConstVectorBase<2, __stride2, value_type, __dataPtrType2>& v2,
         bool vectorsAreColumns = true)
        throw(std::runtime_error)
    {
        this->FromRaw(v1, v2, vectorsAreColumns);
        this->ThrowUnlessIsNormalized();
        return *this;
    }

    /*!
      Conversion from 2 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2>
    inline const ThisType &
    From(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
         const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
         bool vectorsAreColumns = true)
        throw (std::runtime_error)
    {
        this->FromRaw(v1, v2, vectorsAreColumns);
        this->ThrowUnlessIsNormalized();
        return *this;
    }

    /*! Conversion from an angle rotation. */
    inline const ThisType &
    From(const vctAngleRotation2 & angleRotation)
        throw(std::runtime_error)
    {
        this->ThrowUnlessIsNormalized(angleRotation);
        return this->FromRaw(angleRotation);
    }
    //@}






    /*!
      \name Conversion and normalization.

      These method will accept any input and attempt to either
      normalize the input and then convert or convert and then
      normalize the quaternion itself.<br>The order depends on the
      type of input.
    */
    //@{

    /*! Conversion from 4 elements. */
    inline const ThisType &
    FromNormalized(const value_type & element00, const value_type & element01,
                   const value_type & element10, const value_type & element11)
    {
        this->FromRaw(element00, element01,
                      element10, element11);
        this->NormalizedSelf();
        return *this;
    }

    /*!
      Conversion from 2 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <int __stride1, class __dataPtrType1,
              int __stride2, class __dataPtrType2>
    inline const ThisType &
    FromNormalized(const vctFixedSizeConstVectorBase<2, __stride1, value_type, __dataPtrType1>& v1,
                   const vctFixedSizeConstVectorBase<2, __stride2, value_type, __dataPtrType2>& v2,
                   bool vectorsAreColumns = true)
        throw(std::runtime_error)
    {
        this->FromRaw(v1, v2, vectorsAreColumns);
        this->NormalizedSelf();
        return *this;
    }


    /*!
      Conversion from 2 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2>
    inline const ThisType &
    FromNormalized(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                   const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                   bool vectorsAreColumns = true)
    {
        this->FromRaw(v1, v2, vectorsAreColumns);
        this->NormalizedSelf();
        return *this;
    }
    
    /*! Conversion from an angle rotation. */
    inline const ThisType &
    FromNormalized(const vctAngleRotation2 & angleRotation)
    {
        return this->FromRaw(angleRotation.Normalized());
    }
    //@}






    /*!
      \name Conversion.

      These method don't check if the input is normalized nor try to
      normalize the results.  They should be used with caution since
      the resulting rotation (in this case a quaternion) might not be
      normalized.
    */
    //@{


    /*! Conversion from 4 elements. */
    inline const ThisType &
    FromRaw(const value_type & element00, const value_type & element01,
            const value_type & element10, const value_type & element11)
    {
        this->Assign(element00, element01,
                     element10, element11);
        return *this;
    }

    /*!
      Conversion from 2 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <int __stride1, class __dataPtrType1,
              int __stride2, class __dataPtrType2>
    inline const ThisType &
    FromRaw(const vctFixedSizeConstVectorBase<2, __stride1, value_type, __dataPtrType1>& v1,
            const vctFixedSizeConstVectorBase<2, __stride2, value_type, __dataPtrType2>& v2,
            bool vectorsAreColumns = true)
    {
        if (vectorsAreColumns) {
            this->Column(0).Assign(v1);
            this->Column(1).Assign(v2);
        } else {
            this->Row(0).Assign(v1);
            this->Row(1).Assign(v2);
        }
        return *this;
    }
    
    /*!
      Conversion from 2 dynamic vectors.
      
      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2>
    inline const ThisType &
    FromRaw(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
            const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
            bool vectorsAreColumns = true)
        throw (std::runtime_error)
    {
        CMN_ASSERT(v1.size() == 2);
        CMN_ASSERT(v2.size() == 2);
        if (vectorsAreColumns) {
            this->Column(0).Assign(v1);
            this->Column(1).Assign(v2);
        } else {
            this->Row(0).Assign(v1);
            this->Row(1).Assign(v2);
        }
        return *this;
    }

    /*! Conversion from an angle rotation */
    CISST_EXPORT const ThisType &
    FromRaw(const vctAngleRotation2 & angleRotation);

    /*! A complementary form of assigning one matrix rotation to
      another.  The method is provided mostly for generic programming
      interfaces and for testing various operations on rotations */
    inline const ThisType &
    FromRaw(const ThisType & otherRotation) {
        return reinterpret_cast<const ThisType &>(this->Assign(otherRotation));
    }

    /*! Assign a 2x2 matrix to this rotation matrix.  This method does
      not substitute the Assign() method.  Assign() may perform type
      conversion, while From() only takes a matrix of the same element
      type.

      \note This method does not verify normalization.  It is
      introduced to allow using results of matrix operations and
      assign them to a rotation matrix.
    */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType &
    FromRaw(const vctFixedSizeMatrixBase<2, 2, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        this->Assign(matrix);
        return *this;
    }

    //@}



    /*! Normalizes this matrix.  This method converts the matrix to an
      angle and convert back to a matrix. */
    CISST_EXPORT const ThisType & NormalizedSelf(void);

    /*! Sets this rotation matrix as the normalized version of another one.    
      \param otherMatrix Matrix used to compute the normalized matrix. */
    inline ThisType & NormalizedOf(const ThisType & otherMatrix) {
        *this = otherMatrix;
        this->NormalizedSelf();
        return *this;
    }

    /*! Returns the normalized version of this matrix.  This method
      returns a copy of the normalized version and does not modify
      this matrix. */
    ThisType Normalized(void) const {
        ThisType result(*this);
        result.NormalizedSelf();
        return result;
    }
    

    /*! Test if this matrix is normalized.  This methods checks that
      all the columns are normalized (within a margin of tolerance)
      and then checks that the two vectors are orthogonal to each
      other.

      \param tolerance Tolerance for the norm and scalar product tests.
    */
    inline bool IsNormalized(value_type tolerance = TypeTraits::Tolerance()) const
    {
        if ((vctUnaryOperations<value_type>::AbsValue::Operate(value_type(this->Column(0).Norm() - 1)) > tolerance)
            || (vctUnaryOperations<value_type>::AbsValue::Operate(value_type(this->Column(1).Norm() - 1)) > tolerance)
            || (vctUnaryOperations<value_type>::AbsValue::Operate(this->Column(0).DotProduct(this->Column(1))) > tolerance)) {
            return false; 
        } else {
            return true;
        }
    }

    
    /*! Inverse this rotation matrix.  This methods assumes that the
      matrix is normalized and sets this matrix as its transposed. */
    inline const ThisType & InverseSelf(void) {
        // could use the transpose operator but this seems more efficient
        value_type tmp;
        tmp = this->Element(0, 1);
        this->Element(0, 1) = this->Element(1, 0);
        this->Element(1, 0) = tmp;
        return *this;
    }


    /*! Set this rotation as the inverse of another one.  See also
      InverseSelf(). */
    inline const ThisType & InverseOf(const ThisType & otherRotation) {
        TransposeOf(otherRotation);
        return *this;
    }


    /*! Create and return by copy the inverse of this matrix.  This
      method is not the most efficient since it requires a copy.  See
      also InverseSelf().  */
    inline ThisType Inverse(void) const {
        ThisType result;
        result.InverseOf(*this);
        return result;
    }
    

    /*! Apply the rotation to a vector of fixed size 2. The result is
      stored into another vector of size 2 provided by the caller and
      passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline void
    ApplyTo(const vctFixedSizeConstVectorBase<2, __stride1, value_type, __dataPtrType1> & input,
            vctFixedSizeVectorBase<2, __stride2, value_type, __dataPtrType2> & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(*this, input);
    }


    /*! Apply the rotation to a vector of fixed size 2. The result is
      returned by copy.  This interface might be more convenient for
      some but one should note that it is less efficient since it
      requires a copy.

      \param input The input vector
      \return The output vector
    */
    template<int __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, 2>
    ApplyTo(const vctFixedSizeConstVectorBase<2, __stride, value_type, __dataPtrType> & input) const {
        vctFixedSizeVector<value_type, 2> result;
        this->ApplyTo(input, result);
        return result;
    }


    /*! Apply the rotation to another rotation.  The result is stored
      into a vctMatrixRotation2Base (ThisType) provided by the caller and
      passed by reference.
      
      \param input The input rotation
      \param output The output rotation
    */
    inline void ApplyTo(const ThisType & input, ThisType & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(*this, input);
    }


    /*! Apply the rotation to another rotation.  The result is
      returned by copy.  This interface might be more convenient for
      some but one should note that it is less efficient since it
      requires a copy.

      \param input The input rotation
      \return The output rotation
    */
    inline ThisType
    ApplyTo(const ThisType & input) const {
        CMN_ASSERT(input.Pointer() != this->Pointer());
        ThisType result;
        this->ApplyTo(input, result);
        return result;
    }

    
    /*! Apply the rotation to a dynamic vector.  The result is stored
      into another dynamic vector passed by reference by the caller.
      It is assumed that both are of size 2.
    */
    template<class _vectorOwnerType1, class _vectorOwnerType2>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
            vctDynamicVectorBase<_vectorOwnerType2, value_type> & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == 2);
        CMN_ASSERT(output.size() == 2);
        output[0] = this->Element(0, 0) * input[0] + this->Element(0, 1) * input[1];
        output[1] = this->Element(1, 0) * input[0] + this->Element(1, 1) * input[1];
    }


    /*! Apply the inverse of the rotation to a vector of fixed size
      2. The result is stored into a vector of size 2 provided by the
      caller and passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline void
    ApplyInverseTo(const vctFixedSizeConstVectorBase<2, __stride1, value_type, __dataPtrType1> & input,
                   vctFixedSizeVectorBase<2, __stride2, value_type, __dataPtrType2> & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(this->TransposeRef(), input);
    }


    /*! Apply the the inverse of the rotation to a vector of fixed
      size 2. The result is returned by copy.  This interface might be
      more convenient for some but one should note that it is less
      efficient since it requires a copy.

      \param input The input vector
      \return The output vector
    */
    template<int __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, 2>
    ApplyInverseTo(const vctFixedSizeConstVectorBase<2, __stride, value_type, __dataPtrType> & input) const {
        vctFixedSizeVector<value_type, 2> result;
        this->ApplyInverseTo(input, result);
        return result;
    }


    /*! Apply the inverse of the rotation to another rotation.  The
      result is stored into a vctMatrixRotation2Base (ThisType) provided
      by the caller and passed by reference.

      \param input The input rotation
      \param output The output rotation
    */
    inline void ApplyInverseTo(const ThisType & input, ThisType & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(this->TransposeRef(), input);
    }


    /*! Apply the inverse of the rotation to another rotation.  The
      result is returned by copy.  This interface might be more
      convenient for some but one should note that it is less
      efficient since it requires a copy.

      \param input The input rotation
      \return The output rotation
    */
    inline ThisType ApplyInverseTo(const ThisType & input) const {
        CMN_ASSERT(input.Pointer() != this->Pointer());
        ThisType result;
        this->ApplyInverseTo(input, result);
        return result;
    }


    /*! Apply the the inverse of the rotation to a dynamic vector.
      The result is stored into another dynamic vector passed by
      reference by the caller.  It is assumed that both are of size 2.
    */
    template<class _vectorOwnerType1, class _vectorOwnerType2>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
                   vctDynamicVectorBase<_vectorOwnerType2, value_type> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == 2);
        CMN_ASSERT(output.size() == 2);
        output[0] = this->Element(0, 0) * input[0] + this->Element(1, 0) * input[1];
        output[1] = this->Element(0, 1) * input[0] + this->Element(1, 1) * input[1];
    }

    /*! Multiply two rotation matrices and return the result as a rotation matrix.
      \return (*this) * other
      \note this function overrides and shadows the operator * defined for basic
      matrix type.  The latter returns a vctFixedSizeMatrix, while this version
      returns a specialized rotation matrix.
    */
    ThisType operator * (const ThisType & input) const
    {
        return this->ApplyTo(input);
    }

    /*! Override the * operator defined in the matrix base for matrix * vector.
      This prevents compiler confusion between the overloaded RotMat*RotMat and
      the base-class Mat*Vec
    */
    template<int __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, 2>
    operator * (const vctFixedSizeConstVectorBase<2, __stride, value_type, __dataPtrType> & input) const
    {
        return this->ApplyTo(input);
    }


    /*! Return true if this rotation is effectively equivalent to the
      other rotation, up to the given tolerance.  For a rotation
      matrix, this method is strictly the same as AlmostEqual.
      
      \sa AlmostEqual
    */
    inline bool AlmostEquivalent(const ThisType & other,
                                 value_type tolerance = TypeTraits::Tolerance()) const {
        return this->AlmostEqual(other, tolerance);
    }

};


#endif  // _vctMatrixRotation2Base_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctMatrixRotation2Base.h,v $
// Revision 1.4  2006/01/20 04:46:36  anton
// cisstVector: Added methods AlmostEquivalent for all transformations as well
// as some missing Equal (and ==).  See ticket #204.
//
// Revision 1.3  2006/01/03 03:38:58  anton
// cisstVector Doxygen: Used \code \endcode instead of <pre> </pre>, added
// some missing <ul> </ul>, added links to VCT_NORMALIZE.
//
// Revision 1.2  2005/12/02 02:40:23  anton
// vctRotationMatrix2: Update for gcc 4.0.
//
// Revision 1.1  2005/12/01 22:11:57  anton
// cisstVector: 2D transformations, added a base class for rotation matrix 2.
//
//
// ****************************************************************************

