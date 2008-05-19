/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctAxisAngleRotation3.h,v 1.16 2006/01/20 04:46:36 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2005-01-13

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
  \brief Declaration of vctAxisAngleRotation3
 */


#ifndef _vctAxisAngleRotation3_h
#define _vctAxisAngleRotation3_h

#include <cisstCommon/cmnConstants.h>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctQuaternionRotation3Base.h>
#include <cisstVector/vctMatrixRotation3Base.h>
#include <cisstVector/vctRodriguezRotation3Base.h>
#include <cisstVector/vctExport.h>


#ifndef DOXYGEN
#ifndef SWIG

template <class _elementType, class _containerType>
const vctAxisAngleRotation3<_elementType> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<_elementType> & axisAngleRotation,
                             const vctQuaternionRotation3Base<_containerType> & quaternionRotation);

template <class _elementType, class _containerType>
const vctAxisAngleRotation3<_elementType> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<_elementType> & axisAngleRotation,
                             const vctMatrixRotation3Base<_containerType> & matrixRotation);

template <class _elementType, class _containerType>
const vctAxisAngleRotation3<_elementType> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<_elementType> & axisAngleRotation,
                             const vctRodriguezRotation3Base<_containerType> & rodriguezRotation);

#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
template CISST_EXPORT const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
template CISST_EXPORT const vctAxisAngleRotation3<float> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<float> & axisAngleRotation,
                             const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation);
template CISST_EXPORT const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > & matrixRotation);
template CISST_EXPORT const vctAxisAngleRotation3<float> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<float> & axisAngleRotation,
                             const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > & matrixRotation);
template CISST_EXPORT const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > & rodriguezRotation);
template CISST_EXPORT const vctAxisAngleRotation3<float> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<float> & axisAngleRotation,
                             const vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > & rodriguezRotation);
template CISST_EXPORT const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctQuaternionRotation3Base<vctDynamicVector<double> > & quaternionRotation);
template CISST_EXPORT const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation);
template CISST_EXPORT const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctRodriguezRotation3Base<vctDynamicVector<double> > & rodriguezRotation);
#endif
#endif // SWIG
#endif // DOXYGEN


/*!  \brief Define a rotation based on an axis and an angle for a
  space of dimension 3

  \ingroup cisstVector

  This class is templated by the element type.  It contains a fixed
  size vector and an angle.

  \param _elementType The type of elements.

  \sa vctQuaternion
*/
template <class _elementType>
class vctAxisAngleRotation3
{
public:
    /* no need to document, inherit doxygen documentation from vctFixedSizeVectorBase */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    enum {DIMENSION = 3};
    typedef vctAxisAngleRotation3<value_type> ThisType;
    typedef vctFixedSizeVector<value_type, DIMENSION> AxisType;
    typedef cmnTypeTraits<_elementType> TypeTraits;

protected:
    AxisType AxisMember;
    AngleType AngleMember;

    
    /*! Throw an exception unless this rotation is normalized. */
    inline void ThrowUnlessIsNormalized(void) const throw(std::runtime_error) {
        if (! IsNormalized()) {
            cmnThrow(std::runtime_error("vctAxisAngleRotation3: This rotation is not normalized"));
        }
    }

    /*!
      Throw an exception unless the input is normalized.
      \param input An object with \c IsNormalized method.
    */
    template <class _inputType>
    inline void ThrowUnlessIsNormalized(const _inputType & input) const throw(std::runtime_error) {
        if (! input.IsNormalized()) {
            cmnThrow(std::runtime_error("vctAxisAngleRotation3: Input is not normalized"));
        }
    }

public:
    
    /*! Default constructor. Sets the axis to (1, 0, 0) and the angle to zero. */
    inline vctAxisAngleRotation3():
        AxisMember(1.0, 0.0, 0.0),
        AngleMember(0.0)
    {}
    

    /*! Assignement method.  This method doesn't perform any test on the input. */
    inline ThisType & Assign(const ThisType & other) {
        AxisMember.Assign(other.Axis());
        AngleMember = other.Angle();
        return *this;
    }
    

    /*!
      \name Constructors with normalization test.
      
      These constructors will check that the input is valid,
      i.e. normalized.  If the input is not normalized, an exception
      (of type \c std::runtime_error) will be thrown.  Each
      constructor uses the corresponding From() method based on the
      input type.

      \note See the cmnThrow() function if an \c abort is better than
      an exception for your application.
    */
    //@{


    /*! Constructor from an axis and an angle.
      \param axis A unit vector of size 3.
      \param angle The angle in radian
    */
    template<int __stride, class __dataPtrType>
    inline vctAxisAngleRotation3(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & axis,
                                 const AngleType angle)
        throw(std::runtime_error)
    {
        From(axis, angle);
    }
    
    /*! Constructor from an axis and an angle.
      \param axis A unit vector of size 3.
      \param angle The angle in radian
    */
    template <class _vectorOwnerType>
    inline vctAxisAngleRotation3(const vctDynamicConstVectorBase<_vectorOwnerType, value_type> & axis,
                                 AngleType angle)
        throw(std::runtime_error)
    {
        From(axis, angle);
    }
    
    /*! Constructor from a vctQuaternionRotation3. */
    template <class _containerType>
    inline vctAxisAngleRotation3(const vctQuaternionRotation3Base<_containerType> & quaternionRotation)
        throw(std::runtime_error)
    {
        From(quaternionRotation);
    }
    
    /*! Constructor from a vctMatrixRotation3. */
    template <class _containerType>
    inline vctAxisAngleRotation3(const vctMatrixRotation3Base<_containerType> & matrixRotation)
        throw(std::runtime_error)
    {
        From(matrixRotation);
    }
    
    /*! Constructor from a vctRodriguezRotation3. */
    template <class _containerType>
    inline vctAxisAngleRotation3(const vctRodriguezRotation3Base<_containerType> & rodriguezRotation)
        throw(std::runtime_error)
    {
        From(rodriguezRotation);
    }

    //@}


    /*!
      \name Constructors without normalization test
      
      These constructors will either assume that the input is
      normalized or normalize the input (a copy of it, if required)
      based on the last parameter provided:
      <ul>

        <li>If the normalization flag is set to #VCT_DO_NOT_NORMALIZE,
        the input is considered already normalized and the constructor
        will not perform any sanity check.  This can lead to numerical
        instabilities which have to be handled by the caller.

        <li>If the normalization flag is set to #VCT_NORMALIZE, the
        input will be normalized.  This option should be used whenever
        it is important to obtain a result as "normalized" as
        possible.

      </ul>
    */
    //@{

    /*! Constructor from an axis and an angle.
      \param axis A vector of size 3.
      \param angle The angle in radian
      \param normalizeInput Normalize the input or convert as is (#VCT_NORMALIZE or #VCT_DO_NOT_NORMALIZE)
    */
    template<int __stride, class __dataPtrType>
    inline vctAxisAngleRotation3(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & axis,
                                 const AngleType angle,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(axis, angle);
        } else {
            FromRaw(axis, angle);
        }
    }

    /*! Constructor from an axis and an angle.
      \param axis A vector of size 3.
      \param angle The angle in radian
      \param normalizeInput Normalize the input or convert as is (#VCT_NORMALIZE or #VCT_DO_NOT_NORMALIZE)
    */
    template <class _vectorOwnerType>
    inline vctAxisAngleRotation3(const vctDynamicConstVectorBase<_vectorOwnerType, value_type> & axis,
                                 AngleType angle,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(axis, angle);
        } else {
            FromRaw(axis, angle);
        }
    }
    
    /*! Constructor from a vctQuaternionRotation3. */
    template <class _containerType>
    inline vctAxisAngleRotation3(const vctQuaternionRotation3Base<_containerType> & quaternionRotation,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(quaternionRotation);
        } else {
            FromRaw(quaternionRotation);
        }
    }
    
    /*! Constructor from a vctMatrixRotation3. */
    template <class _containerType>
    inline vctAxisAngleRotation3(const vctMatrixRotation3Base<_containerType> & matrixRotation,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(matrixRotation);
        } else {
            FromRaw(matrixRotation);
        }
    }
    
    /*! Constructor from a vctRodriguezRotation3. */
    template <class _containerType>
    inline vctAxisAngleRotation3(const vctRodriguezRotation3Base<_containerType> & rodriguezRotation,
                                 bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(rodriguezRotation);
        } else {
            FromRaw(rodriguezRotation);
        }
    }
    
    //@}

    

    /*! Const reference to the identity.  In this case, the axis is
        set to <tt>(1, 0, 0)</tt> and the angle is set to zero. */
    static CISST_EXPORT const ThisType & Identity();

    /*! Const reference to the rotation axis data member. */
    inline const AxisType & Axis(void) const {
        return AxisMember;
    }
    
    /*! Reference to the rotation axis data member. */
    inline AxisType & Axis(void) {
        return AxisMember;
    }
    
    /*! Const reference to the rotation angle data member (in radians). */
    inline const AngleType & Angle(void) const {
        return AngleMember;
    }
    
    /*! Reference to the rotation angle data member (in radians). */
    inline AngleType & Angle(void) {
        return AngleMember;
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

    /*! Conversion from an angle and an axis. */
    template<int __stride, class __dataPtrType>
    inline const ThisType & From(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & axis,
                                 const AngleType angle)
        throw(std::runtime_error)
    {
        FromRaw(axis, angle);
        ThrowUnlessIsNormalized();
        return *this;
    }

    /*! Conversion from an angle and an axis. */
    template <class _vectorOwnerType>
    inline const ThisType & From(const vctDynamicConstVectorBase<_vectorOwnerType, value_type> & axis,
                                 AngleType angle)
        throw(std::runtime_error)
    {
        AxisMember[0] = axis[0];
        AxisMember[1] = axis[1];
        AxisMember[2] = axis[2];
        AngleMember = angle;
        ThrowUnlessIsNormalized();
        return *this;
    }

    /*! Conversion from a vctQuaternionRotation3. */
    template <class _containerType>
    inline const ThisType & From(const vctQuaternionRotation3Base<_containerType> & quaternionRotation)
        throw(std::runtime_error)
    {
        ThrowUnlessIsNormalized(quaternionRotation);
        return FromRaw(quaternionRotation);
    }
    
    /*! Conversion from a vctMatrixRotation3. */
    template <class _containerType>
    inline const ThisType & From(const vctMatrixRotation3Base<_containerType> & matrixRotation)
        throw(std::runtime_error)
    {
        ThrowUnlessIsNormalized(matrixRotation);
        return FromRaw(matrixRotation);
    }

    /*! Conversion from a vctRodriguezRotation3. */
    template <class _containerType>
    inline const ThisType & From(const vctRodriguezRotation3Base<_containerType> & rodriguezRotation)
        throw(std::runtime_error)
    {
        ThrowUnlessIsNormalized(rodriguezRotation);
        return FromRaw(rodriguezRotation);
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

    /*! Conversion from an axis and an angle. */
    template<int __stride, class __dataPtrType>
    inline const ThisType & FromNormalized(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & axis,
                                           const AngleType angle)
    {
        FromRaw(axis, angle);
        NormalizedSelf();
        return *this;
    }

    /*! Conversion from an axis and an angle. */
    template <class _containerType>
    inline const ThisType & FromNormalized(const vctQuaternionRotation3Base<_containerType> & quaternionRotation) {
        return FromRaw(quaternionRotation.Normalized());
    }

    /*! Conversion from a vctMatrixRotation3. */
    template <class _containerType>
    inline const ThisType & FromNormalized(const vctMatrixRotation3Base<_containerType> & matrixRotation) {
        return FromRaw(matrixRotation.Normalized());
    }
    
    /*! Conversion from a vctRodriguezRotation3. */
    template <class _containerType>
    inline const ThisType & FromNormalized(const vctRodriguezRotation3Base<_containerType> & rodriguezRotation) {
        return FromRaw(rodriguezRotation.Normalized());
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

    /*! Conversion from an axis and an angle. */
    template<int __stride, class __dataPtrType>
    inline const ThisType & FromRaw(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & axis,
                                    const AngleType angle)
    {
        AxisMember.Assign(axis);
        AngleMember = angle;
        return *this;
    }

    /*! Conversion from a vctQuaternionRotation3. */
    template <class _containerType>
    inline const ThisType & FromRaw(const vctQuaternionRotation3Base<_containerType> & quaternionRotation) {
        return vctAxisAngleRotation3FromRaw(*this, quaternionRotation);
    }

    /*! Conversion from a vctMatrixRotation3. */
    template <class _containerType>
    inline const ThisType & FromRaw(const vctMatrixRotation3Base<_containerType> & matrixRotation) {
        return vctAxisAngleRotation3FromRaw(*this, matrixRotation);
    }
    
    /*! Conversion from a vctRodriguezRotation3. */
    template <class _containerType>
    inline const ThisType & FromRaw(const vctRodriguezRotation3Base<_containerType> & rodriguezRotation) {
        return vctAxisAngleRotation3FromRaw(*this, rodriguezRotation);
    }
    
    //@}


    /*! Inverse this axis angle rotation.  This methods assumes that
        the axis is normalized and the angle is betweem 0 and 2 * PI.
        The axis is not changed but the angle is set to 2 * PI minus
        the previous value. */
    inline const ThisType & InverseSelf(void) {
        AngleMember = 2.0 * cmnPI - AngleMember;
        return *this;
    }

    /*! Set this rotation as the inverse of another one.  See also
        InverseSelf(). */
    inline const ThisType & InverseOf(const ThisType & otherRotation) {
        AxisMember.Assign(otherRotation.Axis());
        AngleMember = 2.0 * cmnPI - otherRotation.Angle();
        return *this;
    }

    /*! Create and return by copy the inverse of this rotation.  This
      method is not the most efficient since it requires a copy.  See
      also InverseSelf(). */
    inline ThisType Inverse(void) const {
        ThisType result;
        result.Angle() = 2 * cmnPI - this->Angle();
        return result;
    }

    /*! Normalizes this rotation.  This method first normalizes the
        axis and then modifies the angle so that it's value is between
        0 and 2 * PI. */
    inline const ThisType & NormalizedSelf(void) {
        AxisMember.NormalizedSelf();
        AngleMember = fmod(AngleMember, 2.0 * cmnPI);
        if (AngleMember < 0.0) {
            AngleMember += (2.0 * cmnPI);
        }
        return *this;
    }

    /*!
      Sets this rotation as the normalized version of another one.    

      \param otherRotation Axis angle rotation used to compute the
      normalized rotation. */
    inline const ThisType & NormalizedOf(const ThisType & otherRotation) {
        AxisMember.Assign(otherRotation.Axis());
        AngleMember = otherRotation.Angle();
        NormalizedSelf();
        return *this;
    }

    /*! Returns the normalized version of this rotation.  This method
      returns a copy of the normalized rotation and does not modify
      this rotation.   See also NormalizedSelf(). */
    inline ThisType Normalized(void) const {
        ThisType result(*this);
        result.NormalizedSelf();
        return result;
    }


    /*! Test if this rotation is normalized.  This methods checks that
      the axis is normalized (with tolerance).  Any angle is considered valid.

      \param tolerance Tolerance for the norm test.
    */
    inline bool IsNormalized(value_type tolerance = TypeTraits::Tolerance()) const {
        return AxisMember.IsNormalized(tolerance);
    }
    

    /*! Return true if this rotation is exactly equal to the other
      rotation, without any tolerance error.  Rotations may be
      effectively equal if one is elementwise equal to the other.

      \sa AlmostEqual
    */
    //@{
    inline bool Equal(const ThisType & other) const {
        return ((this->Angle() == other.Angle()) && (this->Axis().Equal(other.Axis())));
    }

    inline bool operator==(const ThisType & other) const {
        return this->Equal(other);
    }
    //@}


    /*! Return true if this rotation is effectively equal to the other
      rotation, up to the given tolerance.  Rotations may be
      effectively equal if one is elementwise equal to the other.

      The tolerance factor is used to compare each of the elements of
      the difference vector.

      \sa AlmostEquivalent
    */
    inline bool AlmostEqual(const ThisType & other,
                            value_type tolerance = TypeTraits::Tolerance()) const {
        const AngleType angleDiff = this->Angle() - other.Angle();
        const AngleType angle = (angleDiff < 0) ? -angleDiff : angleDiff;

        return ((angle <= tolerance) && (this->Axis().AlmostEqual(other.Axis(), tolerance)));
    }


    /*! Return true if this rotation is effectively equavilent to the
      other rotation, up to the given tolerance.  Rotation may be
      effectively equivalent if their axis are aligned and their
      angles are equal, modulus 2 PI (take into account the direction
      of the axis).

      The tolerance factor is used to compare each of the elements of
      the difference vector.

      \sa AlmostEqual
    */
    inline bool AlmostEquivalent(const ThisType & other,
                                 value_type tolerance = TypeTraits::Tolerance()) const {
        const AngleType angleThis = this->Angle();
        const AngleType angleOther = other.Angle();
        // two quasi null rotations
        if ((angleThis <= tolerance) && (angleOther <= tolerance)) {
            return true;
        }
        // one rotation is almost null
        if ((angleThis <= tolerance) || (angleOther <= tolerance)) {
            return false;
        }

        // both rotations are non null
        const AngleType dotProduct = AngleType(this->Axis().Normalized().DotProduct(other.Axis().Normalized()));
        const AngleType absDotProduct = dotProduct > value_type(0.0) ? dotProduct : -dotProduct;
        // axis don't seem aligned
        if ((absDotProduct <= (AngleType(1.0) - tolerance))
            || (absDotProduct >= (AngleType(1.0) + tolerance))) {
            return false;
        }
        // axis are aligned
        const AngleType angleDifference = angleThis - dotProduct * angleOther;
        AngleType angleRemain = fmod(angleDifference, 2.0 * cmnPI);
        if (angleRemain > cmnPI) {
            angleRemain -= (2.0 * cmnPI);
        } else if (angleRemain < -cmnPI) {
            angleRemain += (2.0 *cmnPI);
        }
        if ((angleRemain > -tolerance) && (angleRemain < tolerance)) {
            return true;
        }
        return false;
    }


    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
    

    void ToStream(std::ostream & outputStream) const {
        outputStream << "axis: "
                     << std::endl
                     << this->Axis()
                     << std::endl
                     << "angle: "
                     << std::endl
                     << this->Angle();
    }
};


/*! Stream out operator. */
template<class _elementType>
std::ostream & operator << (std::ostream & output,
                            const vctAxisAngleRotation3<_elementType> & axisAngle) {
    axisAngle.ToStream(output);
    return output;
}


#endif  // _vctAxisAngleRotation3_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctAxisAngleRotation3.h,v $
// Revision 1.16  2006/01/20 04:46:36  anton
// cisstVector: Added methods AlmostEquivalent for all transformations as well
// as some missing Equal (and ==).  See ticket #204.
//
// Revision 1.15  2006/01/03 03:38:58  anton
// cisstVector Doxygen: Used \code \endcode instead of <pre> </pre>, added
// some missing <ul> </ul>, added links to VCT_NORMALIZE.
//
// Revision 1.14  2005/12/23 21:27:31  anton
// cisstVector: Minor updates for Doxygen 1.4.5.
//
// Revision 1.13  2005/12/07 23:16:41  anton
// Transformations: All IsNormalized() tests now consider that any angle is
// valid.  These don't break the conversion methods and any range seems
// arbitrary.  This is related to ticket #110.
//
// Revision 1.12  2005/12/06 02:51:46  anton
// vctAxisAngleRotation3: More stable AlmostEqual().
//
// Revision 1.11  2005/12/02 03:05:43  anton
// cisstVector: Update on AlmostEqual() methods for axis angle and Rodriguez.
//
// Revision 1.10  2005/12/01 21:58:58  anton
// vctAxisAngleRotation3.h: Errors in inversion (used -angle, not 2PI - angle),
// normalization (normalized the axis, not the angle) and IsNormalized() used
// default tolerance (not user specified).
//
// Revision 1.9  2005/11/17 18:43:52  anton
// cisstVector transformations: Ongoing work re. ticket #110.
//
// Revision 1.8  2005/11/15 03:37:50  anton
// vctAxisAngle and vctRodriguez: Major updates re. ticket #110. Work in progress.
//
// Revision 1.7  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.6  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.5  2005/09/01 06:18:08  anton
// cisstVector transformations: Added a level of abstraction for all classes
// derived from fixed size containers (vctQuaternion, vctQuaternionRotation3,
// vctMatrixRotation3, vctRodriguezRotation3).  These classes are now derived
// from vctXyzBase<_containerType>.  This will ease the SWIG wrapping.
//
// Revision 1.4  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.3  2005/02/22 23:00:16  anton
// vctAxisAngleRotation3 and vctRodriguezRotation3: Added IsNormalized() for
// syntactic completion, i.e. required for templated code such as tests.
//
// Revision 1.2  2005/02/08 22:11:46  ofri
// vctAxisAngleRotation3: converted norm and angle types to the canonical AngleType
// (following ticket #128)
//
// Revision 1.1  2005/02/04 16:56:54  anton
// cisstVector: Added classes vctAxisAngleRotation3 and vctRodriguezRotation3
// as planed in ticket #109.  Also updated vctMatrixRotation3 to use these new
// classes.  Other updates include vctRandom(rotation), new vctTypes and
// forward declarations.
//
//
// ****************************************************************************

