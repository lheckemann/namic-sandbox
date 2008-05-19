/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctQuaternionRotation3Base.h,v 1.8 2006/01/20 04:46:36 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2005-08-24

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
  \brief Declaration of vctQuaternionRotation3Base
 */


#ifndef _vctQuaternionRotation3Base_h
#define _vctQuaternionRotation3Base_h


#include <cisstVector/vctQuaternion.h>
#include <cisstVector/vctAxisAngleRotation3.h>
#include <cisstVector/vctRodriguezRotation3.h>
#include <cisstVector/vctMatrixRotation3Base.h>
#include <cisstVector/vctFixedSizeMatrix.h>

#include <cisstVector/vctExport.h>


#ifndef DOXYGEN
#ifndef SWIG

// helper functions for subtemplated methods of a templated class
template <class _quaternionType, class _matrixType>
const vctQuaternionRotation3Base<_quaternionType> &
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<_quaternionType> & quaternionRotation,
                                  const vctMatrixRotation3Base<_matrixType> & matrixRotation);

#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
template CISST_EXPORT const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > &
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > & matrixRotation);
template CISST_EXPORT const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > &
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > & matrixRotation);
template CISST_EXPORT const vctQuaternionRotation3Base<vctDynamicVector<double> > &
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctDynamicVector<double> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation);
#endif
#endif // SWIG
#endif // DOXYGEN


/*!
  \brief Define a rotation quaternion for a space of dimension 3

  This class is templated by the element type.  It is derived from
  vctQuaternionBase.  This class is templated by the type of container
  used to store the quaternion.  It is an "inner" class of the
  library, i.e. a regular user should use either the predefined type
  ::vctQuatRot3 or the class vctQuaternionRotation3.

  \param _containerType The type of container used for the quaternion.

  \sa vctQuaternionRotation3, vctQuaternionBase
*/
template <class _containerType>
class vctQuaternionRotation3Base: public vctQuaternionBase<_containerType>
{
public:
    enum {SIZE = 4};
    enum {DIMENSION = 3};
    
    /*! Type of base class. */
    typedef vctQuaternionBase<_containerType> BaseType;

    /*! Type of container.  This must be a vector of 4 elements
      (either fixed size or dynamic).  In any case, this class should
      only be used by the library programmers. */
    typedef _containerType ContainerType;
    typedef vctQuaternionRotation3Base<_containerType> ThisType;

    /* no need to document, inherit doxygen documentation from vctFixedSizeVectorBase */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename ContainerType::value_type);
    typedef cmnTypeTraits<value_type> TypeTraits;


protected:
    /*! Throw an exception unless this rotation is normalized. */
    inline void ThrowUnlessIsNormalized(void) const throw(std::runtime_error) {
        if (! this->IsNormalized()) {
            cmnThrow(std::runtime_error("vctQuaternionRotation3Base: This rotation is not normalized"));
        }
    }

    /*!
      Throw an exception unless the input is normalized.
      \param input An object with \c IsNormalized method.
    */
    template <class _inputType>
    inline void ThrowUnlessIsNormalized(const _inputType & input) const throw(std::runtime_error) {
        if (! input.IsNormalized()) {
            cmnThrow(std::runtime_error("vctQuaternionRotation3Base: Input is not normalized"));
        }
    }

public:

    /*! Default constructor. Sets the rotation quaternion to identity. */
    inline vctQuaternionRotation3Base():
        BaseType(Identity())
    {}

    /*! Const reference to the identity.  In this case, <tt>(0, 0, 0,
        1)</tt>*/
    static CISST_EXPORT const ThisType & Identity();


    /*! Constructor from 4 elements.  This constructor asserts in
      debug mode only that the quaternion created is actually a valid
      rotation quaternion (i.e. normalized).  This constructor must
      but used with precautions.
    */
    inline vctQuaternionRotation3Base(value_type x, value_type y, value_type z,
                                      value_type r) {
        this->Assign(x, y, z, r);
    }




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

    /*! Conversion from 4 numbers.  This method actually performs an
      assignement and then check that the result is normalized. */
    inline const ThisType &
    From(value_type x, value_type y, value_type z, value_type r) throw(std::runtime_error) {
        this->Assign(x, y, z, r);
        this->ThrowUnlessIsNormalized(*this);
        return *this;
    }

    /*! Conversion from axis/angle. */
    inline const ThisType &
    From(const vctAxisAngleRotation3<value_type> axisAngleRotation) throw(std::runtime_error) {
        this->ThrowUnlessIsNormalized(axisAngleRotation);
        return FromRaw(axisAngleRotation);
    }

    /*! Conversion from a Rodriguez rotation. */
    template <class __containerType>
    inline const ThisType & 
    From(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation) throw(std::runtime_error) {
        this->ThrowUnlessIsNormalized(rodriguezRotation);
        return this->FromRaw(rodriguezRotation);
    }

    /*! Conversion from a rotation matrix. */
    template <class __containerType>
    inline const ThisType &
    From(const vctMatrixRotation3Base<__containerType> & matrixRotation) throw(std::runtime_error) {
        this->ThrowUnlessIsNormalized(matrixRotation);
        return FromRaw(matrixRotation);
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
    
    /*! Conversion from 4 numbers.  This method actually performs an
      assignement and then normalize the quaternion (\c this). */
    inline const ThisType &
    FromNormalized(value_type x, value_type y, value_type z, value_type r) {
        this->Assign(x, y, z, r);
        this->NormalizedSelf();
        return *this;
    }

    /*! Conversion from axis/angle. */
    inline const ThisType &
    FromNormalized(const vctAxisAngleRotation3<value_type> axisAngleRotation) {
        return FromRaw(axisAngleRotation.Normalized());
    }

    /*! Conversion from a Rodriguez rotation. */
    template <class __containerType>
    inline const ThisType & 
    FromNormalized(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation) {
        return this->FromRaw(rodriguezRotation.Normalized());
    }

    /*!
      Conversion from a rotation matrix.
    
      This method could normalize the input first, but the rotation
      matrix normalization relies on -1- a conversion to quaternion,
      -2- normalization and -3- a conversion back to rotation matrix.
      Therefore it converts to quaternion and then normalizes the
      resulting quaternion (\c this).
    */
    template <class __containerType>
    inline const ThisType &
    FromNormalized(const vctMatrixRotation3Base<__containerType> & matrixRotation) {
        this->FromRaw(matrixRotation);
        this->NormalizedSelf();
        return *this;
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
    
    inline const ThisType &
    FromRaw(value_type x, value_type y, value_type z, value_type r) {
        this->Assign(x, y, z, r);
        return *this;
    }

    inline const ThisType &
    FromRaw(const vctAxisAngleRotation3<value_type> axisAngleRotation) {
        typedef vctAxisAngleRotation3<value_type> AxisAngleType;
        typedef typename AxisAngleType::AxisType AxisType;

        const AngleType angle = axisAngleRotation.Angle();
        const AxisType axis = axisAngleRotation.Axis();

        const AngleType halfAngle = angle * 0.5;
        const value_type s = (value_type) sin(halfAngle);
        const value_type c = (value_type) cos(halfAngle);
        
        this->Assign(s * axis[0], s * axis[1], s * axis[2], c); 
        return *this;
    }

    /*! Conversion from a Rodriguez rotation. */
    template <class __containerType>
    inline const ThisType & 
    FromRaw(const vctRodriguezRotation3Base<__containerType> & rodriguezRotation) {
        return this->FromRaw(vctAxisAngleRotation3<value_type>(rodriguezRotation, VCT_DO_NOT_NORMALIZE));
    }

    /*! Conversion from a rotation matrix.  The algorithm is based on
      http://www.j3d.org/matrix_faq/matrfaq_latest.html.  This method
      is important since we use it to convert a rotation matrix to a
      quaternion to normalize the matrix.  The method From can not be
      used since it asserts that the input matrix is already
      normalized.

      \param matrixRotation A rotation matrix 
    */
    template <class __containerType>
    inline const ThisType & FromRaw(const vctMatrixRotation3Base<__containerType> & matrixRotation) {
        return vctQuaternionRotation3BaseFromRaw(*this, matrixRotation);
    }
    //@}




    /*! A complementary form of assigning one quaternion rotation to
      another.  The method is provided mostly for generic programming
      interfaces, and for testing various operations on rotations */
    inline const ThisType & From(const ThisType & otherRotation) {
        return reinterpret_cast<const ThisType &>(this->Assign(otherRotation));
    }
    

    /*! Sets this rotation quaternion as the normalized version of another one.    
      \param otherQuaternion rotation quaternion used to compute the normalized quaternion. */
    inline ThisType & NormalizedOf(const ThisType & otherQuaternion) {
        CMN_ASSERT(otherQuaternion.Pointer() != this->Pointer());
        this->Assign(otherQuaternion);
        this->NormalizedSelf();
        return *this;
    }


    /*! Normalizes this quaternion rotation. */
    inline const ThisType & NormalizedSelf(void) {
        BaseType::NormalizedSelf();
        return *this;
    }


    /*! Returns the normalized version of this quaternion rotation.
      This method returns a copy of the normalized version and does
      not modify this quaternion. */
    inline ThisType
    Normalized(void) const {
        ThisType result(*this);
        result.NormalizedSelf();
        return result;
    }


    /*! Inverse this rotation quaternion.  This methods assumes that
      the quaternion is normalized and sets this unit quaternion as
      its transposed. */
    inline const ThisType & InverseSelf(void) {
        this->ConjugateSelf();
        return *this;
    }


    /*! Set this rotation as the inverse of another one. */
    inline const ThisType & InverseOf(const ThisType & otherRotation) {
        ConjugateOf(otherRotation);
        return *this;
    }


    /*! Create and return by copy the inverse of this matrix.  This
      method is not the most efficient since it requires a copy. */
    inline ThisType Inverse(void) const {
        ThisType result;
        result.InverseOf(*this);
        return result;
    }


    /*! Apply the rotation to a vector of fixed size 3. The result is stored into
      a vector of size 3 provided by the caller and passed by reference.

      \param input The input vector
      \param output The output vector
    */
    template<int __stride1, class __dataPtrType1, int __stride2, class __dataPtrType2>
    inline void
    ApplyTo(const vctFixedSizeConstVectorBase<3, __stride1, value_type, __dataPtrType1> & input,
            vctFixedSizeVectorBase<3, __stride2, value_type, __dataPtrType2> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        // could build a vector with a 0 as real component, create the
        // conjugate and then perfom two quaternion products but his
        // seems more efficient.
        value_type qR, qX, qY, qZ;
        const value_type x = this->X();
        const value_type y = this->Y();
        const value_type z = this->Z();
        const value_type r = this->R();

        // compute the product of quaternions q_input = [in, 0] and q
        // conjugate.  Since q_input[3] = 0, suppress terms with it.
        qR = - input.X() * x - input.Y() * y - input.Z() * z;
        qX = - input.X() * r + input.Y() * z - input.Z() * y;
        qY = - input.Y() * r + input.Z() * x - input.X() * z;
        qZ = - input.Z() * r + input.X() * y - input.Y() * x;
        
        // multiply q by (qX, qY, qY, qR).  For out quaternion,
        // element 4 (qR) is not interesting since we only want the
        // vector.
        output.X() = - r * qX - x * qR - y * qZ + z * qY;
        output.Y() = - r * qY - y * qR - z * qX + x * qZ;
        output.Z() = - r * qZ - z * qR - x * qY + y * qX;        
    }


    template<class __vectorOwnerType, int __stride, class __dataPtrType>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input,
            vctFixedSizeVectorBase<3, __stride, value_type, __dataPtrType> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == 3);
        // could build a vector with a 0 as real component, create the
        // conjugate and then perfom two quaternion products but his
        // seems more efficient.
        value_type qR, qX, qY, qZ;
        const value_type x = this->X();
        const value_type y = this->Y();
        const value_type z = this->Z();
        const value_type r = this->R();

        // compute the product of quaternions q_input = [in, 0] and q
        // conjugate.  Since q_input[3] = 0, suppress terms with it.
        qR = - input.X() * x - input.Y() * y - input.Z() * z;
        qX = - input.X() * r + input.Y() * z - input.Z() * y;
        qY = - input.Y() * r + input.Z() * x - input.X() * z;
        qZ = - input.Z() * r + input.X() * y - input.Y() * x;
        
        // multiply q by (qX, qY, qY, qR).  For out quaternion,
        // element 4 (qR) is not interesting since we only want the
        // vector.
        output.X() = - r * qX - x * qR - y * qZ + z * qY;
        output.Y() = - r * qY - y * qR - z * qX + x * qZ;
        output.Z() = - r * qZ - z * qR - x * qY + y * qX;        
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
    ApplyTo(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType> & input) const
    {
        vctFixedSizeVector<value_type, 3> result;
        this->ApplyTo(input, result);
        return result;
    }

    template<class __vectorOwnerType>
    inline vctFixedSizeVector<value_type, 3>
    ApplyTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input) const
    {
        CMN_ASSERT(input.size() == 3);
        vctFixedSizeVector<value_type, 3> result;
        this->ApplyTo(input, result);
        return result;
    }



    /*! Apply the rotation to another rotation.  The result is stored
      into a vctQuaternionRotation3Base (ThisType) provided by the caller and
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
    inline ThisType ApplyTo(const ThisType & input) const {
        ThisType result;
        this->ApplyTo(input, result);
        return result;
    }


    /*! Apply the rotation to a dynamic vector.  The result is stored
      into another dynamic vector passed by reference by the caller.
      It is assumed that both are of size 3.
    */
    template<class _vectorOwnerType1, class _vectorOwnerType2>
    inline void
    ApplyTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
            vctDynamicVectorBase<_vectorOwnerType2, value_type> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == 3);
        CMN_ASSERT(output.size() == 3);
        // see code for fixed size input/output
        value_type qR, qX, qY, qZ;
        const value_type x = this->X();
        const value_type y = this->Y();
        const value_type z = this->Z();
        const value_type r = this->R();

        qR = - input.X() * x - input.Y() * y - input.Z() * z;
        qX = - input.X() * r + input.Y() * z - input.Z() * y;
        qY = - input.Y() * r + input.Z() * x - input.X() * z;
        qZ = - input.Z() * r + input.X() * y - input.Y() * x;
        
        output.X() = - r * qX - x * qR - y * qZ + z * qY;
        output.Y() = - r * qY - y * qR - z * qX + x * qZ;
        output.Z() = - r * qZ - z * qR - x * qY + y * qX;        
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
                   vctFixedSizeVectorBase<3, __stride2, value_type, __dataPtrType2> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        // could build a vector with a 0 as real component, create the
        // conjugate and then perfom two quaternion products but his
        // seems more efficient.
        value_type qR, qX, qY, qZ;
        const value_type x = this->X();
        const value_type y = this->Y();
        const value_type z = this->Z();
        const value_type r = this->R();
        
        // compute the product of quaternions q_input = [in, 0] and q
        // conjugate.  Since q_input[3] = 0, suppress terms with it.
        qR =   input.X() * x + input.Y() * y + input.Z() * z;
        qX = - input.X() * r - input.Y() * z + input.Z() * y;
        qY = - input.Y() * r - input.Z() * x + input.X() * z;
        qZ = - input.Z() * r - input.X() * y + input.Y() * x;
        
        // multiply q by (qX, qY, qY, qR).  For out quaternion,
        // element 4 (qR) is not interesting since we only want the
        // vector.
        output.X() = - r * qX + x * qR + y * qZ - z * qY;
        output.Y() = - r * qY + y * qR + z * qX - x * qZ;
        output.Z() = - r * qZ + z * qR + x * qY - y * qX;        
    }


    template<class __vectorOwnerType, int __stride, class __dataPtrType>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input,
                   vctFixedSizeVectorBase<3, __stride, value_type, __dataPtrType> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == 3);
        // could build a vector with a 0 as real component, create the
        // conjugate and then perfom two quaternion products but his
        // seems more efficient.
        value_type qR, qX, qY, qZ;
        const value_type x = this->X();
        const value_type y = this->Y();
        const value_type z = this->Z();
        const value_type r = this->R();
        
        // compute the product of quaternions q_input = [in, 0] and q
        // conjugate.  Since q_input[3] = 0, suppress terms with it.
        qR =   input.X() * x + input.Y() * y + input.Z() * z;
        qX = - input.X() * r - input.Y() * z + input.Z() * y;
        qY = - input.Y() * r - input.Z() * x + input.X() * z;
        qZ = - input.Z() * r - input.X() * y + input.Y() * x;
        
        // multiply q by (qX, qY, qY, qR).  For out quaternion,
        // element 4 (qR) is not interesting since we only want the
        // vector.
        output.X() = - r * qX + x * qR + y * qZ - z * qY;
        output.Y() = - r * qY + y * qR + z * qX - x * qZ;
        output.Z() = - r * qZ + z * qR + x * qY - y * qX;        
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
    ApplyInverseTo(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType> & input) const
    {
        vctFixedSizeVector<value_type, 3> result;
        this->ApplyInverseTo(input, result);
        return result;
    }
    
    template<class __vectorOwnerType>
    inline vctFixedSizeVector<value_type, 3>
    ApplyInverseTo(const vctDynamicConstVectorBase<__vectorOwnerType, value_type> & input) const
    {
        CMN_ASSERT(input.size() == 3);
        vctFixedSizeVector<value_type, 3> result;
        this->ApplyInverseTo(input, result);
        return result;
    }


    /*! Apply the inverse of the rotation to another rotation.  The
      result is stored into a vctQuaternionRotation3Base (ThisType) provided
      by the caller and passed by reference.
      
      \param input The input rotation
      \param output The output rotation
    */
    inline void ApplyInverseTo(const ThisType & input, ThisType & output) const {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        output.ProductOf(this->Conjugate(), input);
    }
    

    /*! Apply the inverse of the rotation to another rotation.  The
      result is returned by copy.  This interface might be more
      convenient for some but one should note that it is less
      efficient since it requires a copy.
      
      \param input The input rotation
      \return The output rotation
    */
    inline ThisType ApplyInverseTo(const ThisType & input) const {
        ThisType result;
        this->ApplyInverseTo(input, result);
        return result;
    }
    

    /*! Apply the the inverse of the rotation to a dynamic vector.
      The result is stored into another dynamic vector passed by
      reference by the caller.  It is assumed that both are of size 3.
    */
    template<class _vectorOwnerType1, class _vectorOwnerType2>
    inline void
    ApplyInverseTo(const vctDynamicConstVectorBase<_vectorOwnerType1, value_type> & input,
                   vctDynamicVectorBase<_vectorOwnerType2, value_type> & output) const
    {
        CMN_ASSERT(input.Pointer() != output.Pointer());
        CMN_ASSERT(input.size() == 3);
        CMN_ASSERT(output.size() == 3);
        // see code for fixed size input/output
        value_type qR, qX, qY, qZ;
        const value_type x = this->X();
        const value_type y = this->Y();
        const value_type z = this->Z();
        const value_type r = this->R();
        
        qR =   input.X() * x + input.Y() * y + input.Z() * z;
        qX = - input.X() * r - input.Y() * z + input.Z() * y;
        qY = - input.Y() * r - input.Z() * x + input.X() * z;
        qZ = - input.Z() * r - input.X() * y + input.Y() * x;
        
        output.X() = - r * qX + x * qR + y * qZ - z * qY;
        output.Y() = - r * qY + y * qR + z * qX - x * qZ;
        output.Z() = - r * qZ + z * qR + x * qY - y * qX;        
    }


    /*! Return true if this quaternion rotation is equivalent to the
      other quaternion rotation, up to the given tolerance.
      Quaternion rotation may be effectively equivalent if one is
      elementwise equal to the other, or if one is the negation of the
      other.

      The tolerance factor is used to compare each of the elements of the
      difference vector.

      \sa AlmostEqual
    */
    inline bool AlmostEquivalent(const ThisType & other,
                                 value_type tolerance = TypeTraits::Tolerance()) const {
        ThisType differenceVector;
        differenceVector.DifferenceOf(*this, other);
        differenceVector.AbsSelf();
        if (differenceVector.Lesser(tolerance)) {
            return true;
        }
        differenceVector.SumOf(*this, other);
        differenceVector.AbsSelf();
        if (differenceVector.Lesser(tolerance)) {
            return true;
        }
        return false;
    }


    /*! Multiply two rotation quaternions and return the result as a normalized rotation
      quaternion.
      \return (*this) * other
      \note this function overrides and shadows the operator * defined for basic
      quaternion type.  The latter returns a vctQuaternion, while this version
      returns a specialized vctQuaternionRotation3Base
    */
    ThisType operator * (const ThisType & input) const {
        CMN_ASSERT(input.Pointer() != this->Pointer());
        return this->ApplyTo(input);
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

      \note This method is deprecated.  Use
      From(vctAxisAngleRotation3) instead.
   
    */
    template <int __stride, class __dataPtrType>
    inline CISST_DEPRECATED const ThisType &
    From(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType> & axis,
         const AngleType & angle) {
        assert(vctUnaryOperations<value_type>::AbsValue::Operate(value_type(axis.Norm() - 1))
               < cmnTypeTraits<value_type>::Tolerance());
        const double halfAngle = angle * 0.5;
        const value_type s = value_type(sin(halfAngle));
        const value_type c = value_type(cos(halfAngle));
        this->ConcatenationOf(s * axis, c); 
        return *this;
    }

    /*! Convert from ``Rodriguez rotation'' to rotation
      quaternion.  A Rodriguez rotation is a 3-element vector whose
      direction is the rotation axis, and magnitude is the rotation
      angle in radians.

      \param rodriguezRotation the Rodriguez rotation

      \note For reasons of numerical stability, if the magnitude of
      the Rodriguez vector is less than
      cmnTypeTraits<value_type>::Tolerance(), it is regarded as zero.

      \note This method is deprecated.  Use
      From(vctRodriguezRotation3Base) instead.
    */
    template<int __stride, class __dataPtrType>
    inline CISST_DEPRECATED const ThisType & 
    From(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType> & rodriguezRotation) {
  const value_type axisLength = rodriguezRotation.Norm();
  const value_type axisTolerance = cmnTypeTraits<value_type>::Tolerance();
        const double angle = (fabs(axisLength) < axisTolerance) ? 0.0 : axisLength;
        const vctFixedSizeVector<value_type,3> defaultAxis(value_type(1), value_type(0), value_type(0));
        const vctFixedSizeVector<value_type,3> axis(  (angle == value_type(0)) ? defaultAxis : (rodriguezRotation / axisLength) );
        return this->From(axis, angle);
    }


    /*! Convert to an axis and angle representation.  It is important
      to note that this method doesn't check if the rotation
      quaternion is normalized or not.

      \param axis The axis of the rotation
      
      \param angle The angle around the axis to match the rotation

      \note This method is deprecated.  Use vctAxisAngleRotation3.From() instead.
    */
    template <int __stride, class __dataPtrType>
    inline CISST_DEPRECATED
    void GetAxisAngle(vctFixedSizeVectorBase<3, __stride, value_type, __dataPtrType> & axis,
                      value_type & angle) {
        angle = value_type(acos(this->R()) * 2);
        value_type sinAngle = value_type(sqrt(1.0 - this->R() * this->R()));
        if (vctUnaryOperations<value_type>::AbsValue::Operate(sinAngle) > cmnTypeTraits<value_type>::Tolerance()) {
            axis.X() = this->X() / sinAngle;
            axis.Y() = this->Y() / sinAngle;
            axis.Z() = this->Z() / sinAngle;
        } else {
            axis.X() = this->X();
            axis.Y() = this->Y();
            axis.Z() = this->Z();
        }
    }    
    //@}
};



template<class _containerType, class _elementType, int _stride, class _dataPtrType>
inline vctFixedSizeVector<_elementType, 3>
operator * (const vctQuaternionRotation3Base<_containerType> & rotationQuaternion,
            const vctFixedSizeConstVectorBase<3, _stride, _elementType, _dataPtrType> & vector) {
    vctFixedSizeVector<_elementType, 3> result;
    rotationQuaternion.ApplyTo(vector, result);
    return result;
}


#endif  // _vctQuaternionRotation3Base_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctQuaternionRotation3Base.h,v $
// Revision 1.8  2006/01/20 04:46:36  anton
// cisstVector: Added methods AlmostEquivalent for all transformations as well
// as some missing Equal (and ==).  See ticket #204.
//
// Revision 1.7  2005/11/15 03:32:34  anton
// vctQuaternionRotation3Base:
// *: FromRaw(vctRodriguez) use correct ctor for temporary vctAxisAngle
// *: Redefined Normalized method so that returned value is a
// vctQuaternionRotation, not a base type (i.e. vctQuaternion)
//
// Revision 1.6  2005/10/08 20:18:58  anton
// vctMatRot3Base and vctQuatRot3Base: Use assert, not throw, if size of dynamic
// vector if not 3 (see #180).  Updated vctQuatRot3Base to operate on dynamic
// vectors.
//
// Revision 1.5  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.4  2005/09/28 20:31:06  anton
// vctQuaternionRotation: Major update to implement ticket #110 re. initial
// conditions for conversion methods (i.e. normalization of input).
//
// Revision 1.3  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/09/05 22:54:22  ofri
// Added constructor for vctRodriguezRotation3Base, following ticket #169.  Plus a
// few cosmetic changes, such as replacing const_reference parameters with
// value_type
//
// Revision 1.1  2005/09/01 06:44:47  anton
// vctQuaternionRotation3Base: Omited new file in check-in [1381].
//
//
// ****************************************************************************

