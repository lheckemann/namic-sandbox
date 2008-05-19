/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctMatrixRotation3Base.h,v 1.15 2006/01/20 04:46:36 anton Exp $

  Author(s):  Anton Deguet
  Created on:  2005-08-19

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
  \brief Declaration of vctMatrixRotation3Base
 */


#ifndef _vctMatrixRotation3Base_h
#define _vctMatrixRotation3Base_h


#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctAxisAngleRotation3.h>
#include <cisstVector/vctRodriguezRotation3Base.h>
#include <cisstVector/vctQuaternionRotation3Base.h>
#include <cisstVector/vctExport.h>


#ifndef DOXYGEN
#ifndef SWIG

// helper functions for subtemplated methods of a templated class
template <class _matrixType, class _quaternionType>
const vctMatrixRotation3Base<_matrixType> &
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<_matrixType> & matrixRotation,
                              const vctQuaternionRotation3Base<_quaternionType> & quaternionRotation);

#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
template CISST_EXPORT const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > &
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
template CISST_EXPORT const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > &
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation);
template CISST_EXPORT const vctMatrixRotation3Base<vctDynamicMatrix<double> > &
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctDynamicVector<double> > & quaternionRotation);
template CISST_EXPORT const vctMatrixRotation3Base<vctDynamicMatrix<double> > &
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
#endif
#endif // SWIG
#endif // DOXYGEN


/*!
  \brief Define a rotation matrix for a space of dimension 3

  This class is templated by the type of container used to store the
  rotation matrix.  This class is an internal class, i.e. it is not
  intended for the end-user.  The class which should be used by most
  is vctMatRot3 (eventually vctMatrixRotation3<_elementType>).

  The main goal of this meta rotation matrix class is to ease the
  interface with Python.  For a native Python object, a matrix
  rotation can be defined based on a vctDynamicMatrix which is much
  easier to wrap than a vctFixedSizeMatrix.  For a C++ object accessed
  from Python, the rotation matrix will be defined using
  vctMatrixRotation3Base<vctDynamicMatrixRef<double> >, referring to
  the C++ vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> >.

  \param _containerType The type of the matrix.

  \sa vctFixedSizeMatrix
*/
template <class _containerType>
class vctMatrixRotation3Base: public _containerType
{
public:
    enum {ROWS = 3, COLS = 3};
    enum {DIMENSION = 3};
    typedef _containerType BaseType;
    typedef _containerType ContainerType;
    typedef vctMatrixRotation3Base<ContainerType> ThisType;

    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename ContainerType::value_type);

    /*! Traits used for all useful types and values related to the element type. */
    typedef cmnTypeTraits<value_type> TypeTraits;


protected:
    /*! Throw an exception unless this rotation is normalized. */
    inline void ThrowUnlessIsNormalized(void) const throw(std::runtime_error) {
        if (! IsNormalized()) {
            cmnThrow(std::runtime_error("vctMatrixRotation3Base: This rotation is not normalized"));
        }
    }

    /*!
      Throw an exception unless the input is normalized.
      \param input An object with \c IsNormalized method.
    */
    template <class _inputType>
    inline void ThrowUnlessIsNormalized(const _inputType & input) const throw(std::runtime_error) {
        if (! input.IsNormalized()) {
            cmnThrow(std::runtime_error("vctMatrixRotation3Base: Input is not normalized"));
        }
    }


public:

    vctMatrixRotation3Base():
        BaseType(Identity())
    {}

    /*! Constructor from 9 elements.  This constructor verifies that
      the matrix created is actually a valid rotation matrix.  If the
      input is not normalized, an exception will be thrown
      (std::runtime_error).  The parameters are given row first so
      that the code remains human readable:

      \code
      vctMatrixRotation3<double> matrix( 0.0, 1.0, 0.0,
                                        -1.0, 0.0, 0.0,
                                         0.0, 0.0, 1.0);
      \endcode
    */
    CISST_EXPORT vctMatrixRotation3Base(const value_type & element00, const value_type & element01, const value_type & element02,
                                        const value_type & element10, const value_type & element11, const value_type & element12,
                                        const value_type & element20, const value_type & element21, const value_type & element22)
        throw(std::runtime_error);


    /*! Const reference to the identity.  In this case, the identity matrix:
      <pre>
      1 0 0
      0 1 0
      0 0 1
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

    /*! Conversion from 9 elements. */
    inline const ThisType &
    From(const value_type & element00, const value_type & element01, const value_type & element02,
         const value_type & element10, const value_type & element11, const value_type & element12,
         const value_type & element20, const value_type & element21, const value_type & element22)
        throw(std::runtime_error)
    {
        this->FromRaw(element00, element01, element02,
                      element10, element11, element12,
                      element20, element21, element22);
        this->ThrowUnlessIsNormalized();
        return *this;
    }

    /*!
      Conversion from 3 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <int __stride1, class __dataPtrType1,
              int __stride2, class __dataPtrType2,
              int __stride3, class __dataPtrType3>
    inline const ThisType &
    From(const vctFixedSizeConstVectorBase<3, __stride1, value_type, __dataPtrType1>& v1,
         const vctFixedSizeConstVectorBase<3, __stride2, value_type, __dataPtrType2>& v2,
         const vctFixedSizeConstVectorBase<3, __stride3, value_type, __dataPtrType3>& v3,
         bool vectorsAreColumns = true)
        throw(std::runtime_error)
    {
        this->FromRaw(v1, v2, v3, vectorsAreColumns);
        this->ThrowUnlessIsNormalized();
        return *this;
    }


    /*!
      Conversion from 3 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2,
              class __vectorOwnerType3>
    inline const ThisType &
    From(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
         const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
         const vctDynamicConstVectorBase<__vectorOwnerType3, value_type>& v3,
         bool vectorsAreColumns = true)
        throw (std::runtime_error)
    {
        this->FromRaw(v1, v2, v3, vectorsAreColumns);
        this->ThrowUnlessIsNormalized();
        return *this;
    }

    /*! Conversion from an axis/angle rotation. */
    inline const ThisType &
    From(const vctAxisAngleRotation3<value_type> & axisAngleRotation)
        throw(std::runtime_error)
    {
        this->ThrowUnlessIsNormalized(axisAngleRotation);
        return this->FromRaw(axisAngleRotation);
    }

    /*! Conversion from a rotation quaternion. */
    template <class __containerType>
    inline const ThisType &
    From(const vctQuaternionRotation3Base<__containerType> & quaternionRotation)
        throw(std::runtime_error) 
    {
        this->ThrowUnlessIsNormalized(quaternionRotation);
        return this->FromRaw(quaternionRotation);
    }


    /*! Conversion from a Rodriguez rotation. */
    template <class __containerType>
    inline const ThisType &
    From(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation)
        throw(std::runtime_error)
    {
        this->ThrowUnlessIsNormalized(rodriguezRotation);
        return this->FromRaw(rodriguezRotation);
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

    /*! Conversion from 9 elements. */
    inline const ThisType &
    FromNormalized(const value_type & element00, const value_type & element01, const value_type & element02,
                   const value_type & element10, const value_type & element11, const value_type & element12,
                   const value_type & element20, const value_type & element21, const value_type & element22)
    {
        this->FromRaw(element00, element01, element02,
                      element10, element11, element12,
                      element20, element21, element22);
        this->NormalizedSelf();
        return *this;
    }

    /*!
      Conversion from 3 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <int __stride1, class __dataPtrType1,
              int __stride2, class __dataPtrType2,
              int __stride3, class __dataPtrType3>
    inline const ThisType &
    FromNormalized(const vctFixedSizeConstVectorBase<3, __stride1, value_type, __dataPtrType1>& v1,
                   const vctFixedSizeConstVectorBase<3, __stride2, value_type, __dataPtrType2>& v2,
                   const vctFixedSizeConstVectorBase<3, __stride3, value_type, __dataPtrType3>& v3,
                   bool vectorsAreColumns = true)
        throw(std::runtime_error)
    {
        this->FromRaw(v1, v2, v3, vectorsAreColumns);
        this->NormalizedSelf();
        return *this;
    }


    /*!
      Conversion from 3 dynamic vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2,
              class __vectorOwnerType3>
    inline const ThisType &
    FromNormalized(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
                   const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
                   const vctDynamicConstVectorBase<__vectorOwnerType3, value_type>& v3,
                   bool vectorsAreColumns = true)
    {
        this->FromRaw(v1, v2, v3, vectorsAreColumns);
        this->NormalizedSelf();
        return *this;
    }
    
    /*! Conversion from an axis/angle rotation. */
    inline const ThisType &
    FromNormalized(const vctAxisAngleRotation3<value_type> & axisAngleRotation)
    {
        return this->FromRaw(axisAngleRotation.Normalized());
    }

    /*!
      Conversion from a rotation quaternion.
      
      \param quaternionRotation A rotation quaternion 
    */
    template <class __containerType>
    inline const ThisType &
    FromNormalized(const vctQuaternionRotation3Base<__containerType> & quaternionRotation)
    {
        return this->FromRaw(quaternionRotation.Normalized());
    }


    /*! Conversion from a Rodriguez rotation. */
    template <class __containerType>
    inline const ThisType &
    FromNormalized(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation)
    {
        return this->FromRaw(rodriguezRotation.Normalized());
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


    /*! Conversion from 9 elements. */
    inline const ThisType &
    FromRaw(const value_type & element00, const value_type & element01, const value_type & element02,
            const value_type & element10, const value_type & element11, const value_type & element12,
            const value_type & element20, const value_type & element21, const value_type & element22)
    {
        this->Assign(element00, element01, element02,
                     element10, element11, element12,
                     element20, element21, element22);
        return *this;
    }


    /*!
      Conversion from 3 fixed size vectors.

      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <int __stride1, class __dataPtrType1,
              int __stride2, class __dataPtrType2,
              int __stride3, class __dataPtrType3>
    inline const ThisType &
    FromRaw(const vctFixedSizeConstVectorBase<3, __stride1, value_type, __dataPtrType1>& v1,
            const vctFixedSizeConstVectorBase<3, __stride2, value_type, __dataPtrType2>& v2,
            const vctFixedSizeConstVectorBase<3, __stride3, value_type, __dataPtrType3>& v3,
            bool vectorsAreColumns = true)
    {
        if (vectorsAreColumns) {
            this->Column(0).Assign(v1);
            this->Column(1).Assign(v2);
            this->Column(2).Assign(v3);
        } else {
            this->Row(0).Assign(v1);
            this->Row(1).Assign(v2);
            this->Row(2).Assign(v3);
        }
        return *this;
    }
    
    
    /*!
      Conversion from 3 dynamic vectors.
      
      By default the vectors represents the columns of the matrix. If
      the parameter vectorsAreColumns is set to false, the vectors
      provided will be used to set the matrix row by row. */
    template <class __vectorOwnerType1,
              class __vectorOwnerType2,
              class __vectorOwnerType3>
    inline const ThisType &
    FromRaw(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type>& v1,
            const vctDynamicConstVectorBase<__vectorOwnerType2, value_type>& v2,
            const vctDynamicConstVectorBase<__vectorOwnerType3, value_type>& v3,
            bool vectorsAreColumns = true)
        throw (std::runtime_error)
    {
        CMN_ASSERT(v1.size() == 3);
        CMN_ASSERT(v2.size() == 3);
        CMN_ASSERT(v3.size() == 3);
        if (vectorsAreColumns) {
            this->Column(0).Assign(v1);
            this->Column(1).Assign(v2);
            this->Column(2).Assign(v3);
        } else {
            this->Row(0).Assign(v1);
            this->Row(1).Assign(v2);
            this->Row(2).Assign(v3);
        }
        return *this;
    }


    /*! Conversion from an axis/angle rotation */
    CISST_EXPORT const ThisType &
    FromRaw(const vctAxisAngleRotation3<value_type> & axisAngleRotation);


    /*! Conversion from a quaternion rotation. */
    template <class __containerType>
    inline const ThisType &
    FromRaw(const vctQuaternionRotation3Base<__containerType> & quaternionRotation) {
        return vctMatrixRotation3BaseFromRaw(*this, quaternionRotation);
    }

    
    /*! Conversion from a Rodriguez rotation. */
    template <class __containerType>
    inline const ThisType &
    FromRaw(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation) {
        return this->FromRaw(vctAxisAngleRotation3<value_type>(rodriguezRotation, VCT_DO_NOT_NORMALIZE));
    }
    
    /*! A complementary form of assigning one matrix rotation to
      another.  The method is provided mostly for generic programming
      interfaces and for testing various operations on rotations */
    inline const ThisType &
    FromRaw(const ThisType & otherRotation) {
        return reinterpret_cast<const ThisType &>(this->Assign(otherRotation));
    }


    /*! Assign a 3x3 matrix to this rotation matrix.  This method does
      not substitute the Assign() method.  Assign() may perform type
      conversion, while From() only takes a matrix of the same element
      type.

      \note This method does not verify normalization.  It is
      introduced to allow using results of matrix operations and
      assign them to a rotation matrix.
    */
    template<int __rowStride, int __colStride, class __dataPtrType>
    inline const ThisType &
    FromRaw(const vctFixedSizeMatrixBase<3, 3, __rowStride, __colStride, value_type, __dataPtrType> & matrix) {
        this->Assign(matrix);
        return *this;
    }

    //@}


    /*! Normalizes this matrix.  This method converts the matrix to a
      quaternion, normalizes it and convert back to a matrix. */
    CISST_EXPORT const ThisType & NormalizedSelf(void);

    /*! Sets this rotation matrix as the normalized version of another one.    
      \param otherMatrix Matrix used to compute the normalized matrix. */
    inline ThisType & NormalizedOf(const ThisType & otherMatrix) {
        CMN_ASSERT(otherMatrix.Pointer() != this->Pointer());
        *this = otherMatrix;
        this->NormalizedSelf();
        return *this;
    }

    /*! Returns the normalized version of this rotation.  This method
      returns a copy of the normalized rotation and does not modify
      this rotation.  See also NormalizedSelf(). */
    inline ThisType
    Normalized(void) const {
        ThisType result(*this);
        result.NormalizedSelf();
        return result;
    }
    
    /*! Test if this matrix is normalized.  This methods checks that
      all the columns are normalized (within a margin of tolerance)
      and then checks that the three vectors are orthogonal to each
      other.

      \param tolerance Tolerance for the norm and scalar product tests.
    */
    inline bool IsNormalized(value_type tolerance = TypeTraits::Tolerance()) const {
        NormType columnNorm;
        columnNorm = this->Column(0).Norm();
        if (vctUnaryOperations<NormType>::AbsValue::Operate(columnNorm - 1) > tolerance)
            return false;
        columnNorm = this->Column(1).Norm();
        if (vctUnaryOperations<NormType>::AbsValue::Operate(columnNorm - 1) > tolerance)
            return false;
        columnNorm = this->Column(2).Norm();
        if (vctUnaryOperations<NormType>::AbsValue::Operate(columnNorm - 1) > tolerance)
            return false;
        value_type columnDot;
        columnDot = vctDotProduct(this->Column(0), this->Column(1));
        if (vctUnaryOperations<value_type>::AbsValue::Operate(columnDot) > tolerance)
            return false;
        columnDot = vctDotProduct(this->Column(0), this->Column(2));
        if (vctUnaryOperations<value_type>::AbsValue::Operate(columnDot) > tolerance)
            return false;
        columnDot = vctDotProduct(this->Column(1), this->Column(2));
        if (vctUnaryOperations<value_type>::AbsValue::Operate(columnDot) > tolerance)
            return false;
        return true;
    }

    
    /*! Inverse this rotation matrix.  This methods assumes that the
      matrix is normalized and sets this matrix as its transposed. */
    inline const ThisType &
    InverseSelf(void) {
        // could use the transpose operator but this seems more efficient
        value_type tmp;
        tmp = this->Element(0, 1); this->Element(0, 1) = this->Element(1, 0); this->Element(1, 0) = tmp;
        tmp = this->Element(0, 2); this->Element(0, 2) = this->Element(2, 0); this->Element(2, 0) = tmp;
        tmp = this->Element(1, 2); this->Element(1, 2) = this->Element(2, 1); this->Element(2, 1) = tmp;
        return *this;
    }
    
    
    /*! Set this rotation as the inverse of another one.  See also
        InverseSelf(). */
    inline const ThisType &
    InverseOf(const ThisType & otherRotation) {
        CMN_ASSERT(otherRotation.Pointer() != this->Pointer());
        this->TransposeOf(otherRotation);
        return *this;
    }


    /*! Create and return by copy the inverse of this matrix.  This
      method is not the most efficient since it requires a copy.  See
      also InverseSelf().  */
    inline ThisType
    Inverse(void) const {
        ThisType result;
        result.InverseOf(*this);
        return result;
    }
    

    /*! Apply the rotation to a vector of fixed size 3. The result is
      stored into a vector of size 3 provided by the caller and passed
      by reference.
      
      \param input The input vector
      \param output The output vector
    */
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline void
    ApplyTo(const vctFixedSizeConstVectorBase<3, __stride1, value_type, __dataPtrType1> & input,
            vctFixedSizeVectorBase<3, __stride2, value_type, __dataPtrType2> & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(*this, input);
    }


    /*! Apply the rotation to a vector of fixed size 3. The result is
      returned by copy.  This interface might be more convenient for
      some but one should note that it is less efficient since it
      requires a copy.

      \param input The input vector
      \return The output vector
    */
    template<int __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, 3>
    ApplyTo(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType> & input) const {
        vctFixedSizeVector<value_type, 3> result;
        this->ApplyTo(input, result);
        return result;
    }


    template<class __vectorOwnerType>
    inline vctFixedSizeVector<value_type, 3>
    ApplyTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input) const {
        vctFixedSizeVector<value_type, 3> result;
        this->ApplyTo(input, result);
        return result;
    }


    /*! Apply the rotation to another rotation.  The result is stored
      into a vctMatrixRotation3Base (ThisType) provided by the caller and
      passed by reference.

      \param input The input rotation
      \param output The output rotation
    */
    inline void
    ApplyTo(const ThisType & input, ThisType & output) const {
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
      It is assumed that both are of size 3.
    */
    template<class __vectorOwnerType1, class __vectorOwnerType2>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type> & input,
            vctDynamicVectorBase<__vectorOwnerType2, value_type> & output) const
        throw(std::runtime_error)
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == 3);
        CMN_ASSERT(output.size() == 3);
        output[0] = this->Element(0, 0) * input[0] + this->Element(0, 1) * input[1] + this->Element(0, 2) * input[2];
        output[1] = this->Element(1, 0) * input[0] + this->Element(1, 1) * input[1] + this->Element(1, 2) * input[2];
        output[2] = this->Element(2, 0) * input[0] + this->Element(2, 1) * input[1] + this->Element(2, 2) * input[2];
    }


    template<class __vectorOwnerType, int __stride, class __dataPtrType>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input,
            vctFixedSizeVectorBase<3, __stride, value_type, __dataPtrType> & output) const
        throw(std::runtime_error)
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == 3);
        output[0] = this->Element(0, 0) * input[0] + this->Element(0, 1) * input[1] + this->Element(0, 2) * input[2];
        output[1] = this->Element(1, 0) * input[0] + this->Element(1, 1) * input[1] + this->Element(1, 2) * input[2];
        output[2] = this->Element(2, 0) * input[0] + this->Element(2, 1) * input[1] + this->Element(2, 2) * input[2];
    }


    /*! Apply the inverse of the rotation to a vector of fixed size
      3. The result is stored into a vector of size 3 provided by the
      caller and passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline void
    ApplyInverseTo(const vctFixedSizeConstVectorBase<3, __stride1, value_type, __dataPtrType1> & input,
                   vctFixedSizeVectorBase<3, __stride2, value_type, __dataPtrType2> & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(this->TransposeRef(), input);
    }


    /*! Apply the the inverse of the rotation to a vector of fixed
      size 3. The result is returned by copy.  This interface might be
      more convenient for some but one should note that it is less
      efficient since it requires a copy.

      \param input The input vector
      \return The output vector
    */
    template<int __stride, class __dataPtrType>
    inline vctFixedSizeVector<value_type, 3>
    ApplyInverseTo(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType> & input) const {
        vctFixedSizeVector<value_type, 3> result;
        this->ApplyInverseTo(input, result);
        return result;
    }

    template<class __vectorOwnerType>
    inline vctFixedSizeVector<value_type, 3>
    ApplyInverseTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input) const {
        vctFixedSizeVector<value_type, 3> result;
        this->ApplyInverseTo(input, result);
        return result;
    }



    /*! Apply the inverse of the rotation to another rotation.  The
      result is stored into a vctMatrixRotation3Base (ThisType) provided
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
      reference by the caller.  It is assumed that both are of size 3.
    */
    template<class __vectorOwnerType1, class __vectorOwnerType2>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<__vectorOwnerType1, value_type> & input,
                   vctDynamicVectorBase<__vectorOwnerType2, value_type> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == 3);
        CMN_ASSERT(output.size() == 3);
        output[0] = this->Element(0, 0) * input[0] + this->Element(1, 0) * input[1] + this->Element(2, 0) * input[2];
        output[1] = this->Element(0, 1) * input[0] + this->Element(1, 1) * input[1] + this->Element(2, 1) * input[2];
        output[2] = this->Element(0, 2) * input[0] + this->Element(1, 2) * input[1] + this->Element(2, 2) * input[2];
    }

    template<class __vectorOwnerType, int __stride, class __dataPtrType>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input,
                   vctFixedSizeVectorBase<3, __stride, value_type, __dataPtrType> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == 3);
        output[0] = this->Element(0, 0) * input[0] + this->Element(1, 0) * input[1] + this->Element(2, 0) * input[2];
        output[1] = this->Element(0, 1) * input[0] + this->Element(1, 1) * input[1] + this->Element(2, 1) * input[2];
        output[2] = this->Element(0, 2) * input[0] + this->Element(1, 2) * input[1] + this->Element(2, 2) * input[2];
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
    inline vctFixedSizeVector<value_type, 3>
    operator * (const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType> & input) const
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


    /*!
      \name Deprecated methods
    */
    //@{

    /*! Convert from a unit vector and an angle. It is important to
      note that this method doesn't normalize the axis vector but
      asserts that it is normalized (in debug mode only). 

      \param axis A unit vector of size 3.
      \param angle The angle in radian

      \deprecated This method is deprecated.  Use From(vctAxisAngleRotation3) instead.
    */
    template<int __stride, class __dataPtrType>
    inline CISST_DEPRECATED const ThisType &
    From(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType> & axis,
         const value_type & angle) {
        return From(vctAxisAngleRotation3<value_type>(axis, angle));
    }

    template<int __stride, class __dataPtrType>
    inline CISST_DEPRECATED const ThisType & 
    From(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType> & rodriguezRotation) {
        const NormType axisLength = rodriguezRotation.Norm();
        const value_type axisTolerance = cmnTypeTraits<value_type>::Tolerance();
        const AngleType angle = (fabs(axisLength) < axisTolerance) ? 0.0 : axisLength;
        const vctFixedSizeVector<value_type,3> defaultAxis(value_type(1), value_type(0), value_type(0));
        const vctFixedSizeVector<value_type,3> axis(  (angle == 0.0) ? defaultAxis : (rodriguezRotation / (value_type) axisLength) );
        return this->From(axis, angle);
    }

    template <int __stride, class __dataPtrType>
    inline CISST_DEPRECATED void GetAxisAngle(vctFixedSizeVectorBase<3, __stride, value_type, __dataPtrType> & axis,
                                              value_type & angle) {
        angle = value_type(acos((this->Element(0, 0) + this->Element(1, 1) + this->Element(2, 2) - 1.0) / 2.0));
        value_type den = value_type(sqrt((this->Element(2, 1) - this->Element(1, 2)) * (this->Element(2, 1) - this->Element(1, 2)) +
           (this->Element(0, 2) - this->Element(2, 0)) * (this->Element(0, 2) - this->Element(2, 0)) +
           (this->Element(1, 0) - this->Element(0, 1)) * (this->Element(1, 0) - this->Element(0, 1))));
        axis.X() = (this->Element(2, 1) - this->Element(1, 2)) / den;
        axis.Y() = (this->Element(0, 2) - this->Element(2, 0)) / den;
        axis.Z() = (this->Element(1, 0) - this->Element(0, 1)) / den;
    }

    //@}

};


#endif  // _vctMatrixRotation3Base_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctMatrixRotation3Base.h,v $
// Revision 1.15  2006/01/20 04:46:36  anton
// cisstVector: Added methods AlmostEquivalent for all transformations as well
// as some missing Equal (and ==).  See ticket #204.
//
// Revision 1.14  2006/01/18 19:10:20  anton
// vctMatrixRotation3Base.h:  Fixed bug #203 in FromNormalized().
//
// Revision 1.13  2006/01/03 03:38:58  anton
// cisstVector Doxygen: Used \code \endcode instead of <pre> </pre>, added
// some missing <ul> </ul>, added links to VCT_NORMALIZE.
//
// Revision 1.12  2005/12/01 21:56:43  anton
// vctMatrixRotation3Base.h: Typos in documentation.
//
// Revision 1.11  2005/11/17 18:43:52  anton
// cisstVector transformations: Ongoing work re. ticket #110.
//
// Revision 1.10  2005/11/15 03:29:25  anton
// vctMatrixRotation3Base.h: In FromRaw(vctRodriguez), used correct ctor for
// temporary vctAxisAngle.
//
// Revision 1.9  2005/10/10 01:13:13  anton
// vctMatrixRotation3Base: Instantiations didn't match declaration (throw)
//
// Revision 1.8  2005/10/10 00:53:39  anton
// vctMatrixRotation3Base: Specialize the ctor from 9 elements based on the
// containerType.  This allows to use this ctor in Identity() and create a true
// static variable (see #179).
//
// Revision 1.7  2005/10/08 20:18:58  anton
// vctMatRot3Base and vctQuatRot3Base: Use assert, not throw, if size of dynamic
// vector if not 3 (see #180).  Updated vctQuatRot3Base to operate on dynamic
// vectors.
//
// Revision 1.6  2005/10/07 09:31:26  anton
// vctMatrixRotation3Base: Added a couple of ApplyTo(), throw exception if dynamic
// vectors don't have size = 3, CMN_ASSERT to verify that the input is different
// from the output (see #108)
//
// Revision 1.5  2005/10/07 03:32:35  anton
// vctMatrixRotation3: Major update to implement ticket #110 re. initial
// conditions for conversion methods (i.e. normalization of input).
//
// Revision 1.4  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.3  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/09/24 00:01:04  anton
// cisstVector: Use cmnThrow for all exceptions.
//
// Revision 1.1  2005/09/01 06:18:08  anton
// cisstVector transformations: Added a level of abstraction for all classes
// derived from fixed size containers (vctQuaternion, vctQuaternionRotation3,
// vctMatrixRotation3, vctRodriguezRotation3).  These classes are now derived
// from vctXyzBase<_containerType>.  This will ease the SWIG wrapping.
//
//
// ****************************************************************************

