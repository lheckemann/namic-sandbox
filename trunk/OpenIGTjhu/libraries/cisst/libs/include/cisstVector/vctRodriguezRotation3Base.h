/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctRodriguezRotation3Base.h,v 1.12 2006/01/20 04:46:36 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2005-08-25

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
  \brief Declaration of vctRodriguezRotation3Base
 */


#ifndef _vctRodriguezRotation3Base_h
#define _vctRodriguezRotation3Base_h

#include <cisstCommon/cmnConstants.h>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctForwardDeclarations.h>

#include <cisstVector/vctExport.h>


#ifndef DOXYGEN
#ifndef SWIG

// helper functions for subtemplated methods of a templated class
template <class _rodriguezType, class _matrixType>
const vctRodriguezRotation3Base<_rodriguezType> &
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<_rodriguezType> & rodriguezRotation,
                                 const vctMatrixRotation3Base<_matrixType> & matrixRotation);

template <class _rodriguezType, class _matrixType>
const vctRodriguezRotation3Base<_rodriguezType> &
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<_rodriguezType> & rodriguezRotation,
                                 const vctQuaternionRotation3Base<_matrixType> & quaternionRotation);

#if (CISST_COMPILER == CISST_DOTNET7) || (CISST_COMPILER == CISST_DOTNET2003)
template CISST_EXPORT const vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > &
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > & rodriguezRotation,
                                 const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
template CISST_EXPORT const vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > &
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > & rodriguezRotation,
                                 const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation);
template CISST_EXPORT const vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > &
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > & rodriguezRotation,
                                 const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > & matrixRotation);
template CISST_EXPORT const vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > &
vctRodriguezRotation3BaseFromRaw(vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > & rodriguezRotation,
                                 const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > & matrixRotation);
#endif
#endif // SWIG
#endif // DOXYGEN


/*!  \brief Define a rotation based on the rodriguez representation
  for a space of dimension 3.

  This representation is based on a
  vector.  The direction of the vector determines the axis of rotation
  and its norm defines the amplitude of the rotation.

  This class is templated by the element type.

  \param _elementType The type of elements.

  \sa vctQuaternion
*/
template <class _containerType>
class vctRodriguezRotation3Base: public _containerType
{
public:
    enum {DIMENSION = 3};
    typedef _containerType BaseType;
    typedef _containerType ContainerType;
    typedef vctRodriguezRotation3Base<ContainerType> ThisType;

    /* no need to document, inherit doxygen documentation from vctFixedSizeVectorBase */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename ContainerType::value_type);
    typedef cmnTypeTraits<value_type> TypeTraits;


protected:
    /*! Throw an exception unless this rotation is normalized. */
    inline void ThrowUnlessIsNormalized(void) const throw(std::runtime_error) {
        if (! IsNormalized()) {
            cmnThrow(std::runtime_error("vctRodriguezRotation3Base: This rotation is not normalized"));
        }
    }

    /*!
      Throw an exception unless the input is normalized.
      \param input An object with \c IsNormalized method.
    */
    template <class _inputType>
    inline void ThrowUnlessIsNormalized(const _inputType & input) const throw(std::runtime_error) {
        if (! input.IsNormalized()) {
            cmnThrow(std::runtime_error("vctRodriguezRotation3Base: Input is not normalized"));
        }
    }

public:

    /*! Default constructor. Sets the rotation to (0, 0, 0). */
    inline vctRodriguezRotation3Base():
        BaseType(Identity())
    {}

    /*! Constructor from a 3D vector.
    \param axis A vector of size 3.
    */
    template<int __stride, class __dataPtrType>
    inline vctRodriguezRotation3Base(const vctFixedSizeConstVectorBase<DIMENSION, __stride, value_type, __dataPtrType> & axis):
        BaseType(axis)
    {}

    /*! Constructor from three elements: x, y, and z */
    inline vctRodriguezRotation3Base(value_type x, value_type y, value_type z) {
        this->Assign(x, y, z);
    }
    
    /*! Const reference to the identity.  In this case, a null
        vector. */
    static CISST_EXPORT const ThisType & Identity();

    template <int __stride, class __dataPtrType>
    inline const ThisType & From(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType>& vector)
        throw(std::runtime_error)
    {
        FromRaw(vector);
        // Always true - ThrowUnlessIsNormalized();
        return (*this);
    }


    template <class __vectorOwnerType>
    inline const ThisType & From(const vctDynamicConstVectorBase<__vectorOwnerType, value_type>& vector)
        throw(std::runtime_error)
    {
        FromRaw(vector);
        // Always true - ThrowUnlessIsNormalized();
        return (*this);
    }


    template <class __containerType>
    inline const ThisType & From(const vctQuaternionRotation3Base<__containerType> & quaternionRotation)
        throw(std::runtime_error)
    {
        ThrowUnlessIsNormalized(quaternionRotation);
        return FromRaw(quaternionRotation);
    }

    
    template <class __containerType>
    inline const ThisType & From(const vctMatrixRotation3Base<__containerType> & matrixRotation) {
        ThrowUnlessIsNormalized(matrixRotation);
        return FromRaw(matrixRotation);
    }


    inline const ThisType & From(const vctAxisAngleRotation3<value_type> & axisAngleRotation) {
        ThrowUnlessIsNormalized(axisAngleRotation);
        return FromRaw(axisAngleRotation);
    }



    template <int __stride, class __dataPtrType>
    inline const ThisType & FromNormalized(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType>& vector)
    {
        FromRaw(vector);
        return NormalizedSelf();
    }


    // might throw because of size
    template <class __vectorOwnerType>
    inline const ThisType & FromNormalized(const vctDynamicConstVectorBase<__vectorOwnerType, value_type>& vector)
        throw(std::runtime_error)
    {
        FromRaw(vector);
        return NormalizedSelf();
    }


    template <class __containerType>
    inline const ThisType & FromNormalized(const vctQuaternionRotation3Base<__containerType> & quaternionRotation)
    {
        return FromRaw(quaternionRotation.Normalized());
    }

    
    template <class __containerType>
    inline const ThisType & FromNormalized(const vctMatrixRotation3Base<__containerType> & matrixRotation) {
        return FromRaw(matrixRotation.Normalized());
    }


    inline const ThisType & FromNormalized(const vctAxisAngleRotation3<value_type> & axisAngleRotation) {
        return FromRaw(axisAngleRotation.Normalized());
    }





    template <int __stride, class __dataPtrType>
    inline const ThisType & FromRaw(const vctFixedSizeConstVectorBase<3, __stride, value_type, __dataPtrType>& vector)
    {
        this->Assign(vector);
        return *this;
    }


    template <class __vectorOwnerType>
    inline const ThisType & FromRaw(const vctDynamicConstVectorBase<__vectorOwnerType, value_type>& vector)
    {
        if (vector.size() != 3) {
            cmnThrow(std::runtime_error("vctRodriguezRotation3Base: From requires a vector of size 3"));
        }
        this-Assign(vector);
        return *this;
    }


    template <class __containerType>
    inline const ThisType & FromRaw(const vctQuaternionRotation3Base<__containerType> & quaternionRotation) {
        return vctRodriguezRotation3BaseFromRaw(*this, quaternionRotation);
    }

    
    template <class __containerType>
    inline const ThisType & FromRaw(const vctMatrixRotation3Base<__containerType> & matrixRotation) {
        return vctRodriguezRotation3BaseFromRaw(*this, matrixRotation);
    }


    inline const ThisType & FromRaw(const vctAxisAngleRotation3<value_type> & axisAngleRotation) {
        this->Assign(axisAngleRotation.Axis());
        this->Multiply(value_type(axisAngleRotation.Angle()));
        return *this;
    }


    inline const ThisType & InverseSelf(void) {
        this->NegationSelf();
        return *this;
    }

    inline const ThisType & InverseOf(const ThisType & otherRotation) {
        this->NegationOf(otherRotation);
        return *this;
    }

    inline ThisType Inverse(void) const {
        ThisType result;
        result.NegationOf(*this);
        return result;
    }

    /*! Norm lesser than 2 * PI. */
    inline const ThisType & NormalizedSelf(void) {
        const NormType norm = this->Norm();
        if (norm > (2.0 * cmnPI)) {
            const NormType remainder = fmod(norm, (2.0 * cmnPI));
            // const NormType quotient = (norm - remainder) / (2.0 * cmnPI);
            this->Multiply(value_type(remainder / norm));
        }
        return *this;
    }

    /*! See NormalizedSelf */
    inline const ThisType & NormalizedOf(const ThisType & otherRotation) {
        this->Assign(otherRotation);
        NormalizedSelf();
        return *this;
    }

    /*! See NormalizedSelf */
    inline ThisType Normalized(void) const {
        ThisType result(*this);
        result.NormalizedSelf();
        return result;
    }

    /*! Test if this rotation is normalized.  This methods always
      return "true" since any angle is considered valid.  This method
      is provided mostly for API completion.

      \param tolerance Tolerance.
    */
    inline bool IsNormalized(value_type tolerance = TypeTraits::Tolerance()) const {
        return true;
    }


    /*! Return true if this rotation is equivalent to the other
      rotation, up to the given tolerance.  Rotations may be
      effectively equivalent if their unit axis are almost equal and
      the angles are equal modulo 2 PI.

      The tolerance factor is used to compare each of the elements of
      the difference vector.
    */
    inline bool AlmostEquivalent(const ThisType & other,
                                 value_type tolerance = TypeTraits::Tolerance()) const {
        const AngleType angleThis = this->Norm();
        const AngleType angleOther = other.Norm();
        const AngleType dotProduct = AngleType(this->DotProduct(other));
        // two quasi null rotations
        if ((angleThis <= tolerance) && (angleOther <= tolerance)) {
            return true;
        }
        // one rotation is almost null
        if ((angleThis <= tolerance) || (angleOther <= tolerance)) {
            return false;
        }
        // both rotations are non null
        const AngleType cosAngle = dotProduct / (angleThis * angleOther);
        const AngleType absCosAngle = cosAngle > value_type(0.0) ? cosAngle : -cosAngle;
        // axis don't seem aligned
        if ((absCosAngle <= (AngleType(1.0) - tolerance))
            || (absCosAngle >= (AngleType(1.0) + tolerance))) {
            return false;
        }
        // axis are aligned
        const AngleType angleDifference = angleThis - cosAngle * angleOther;
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

};

#endif  // _vctRodriguezRotation3Base_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctRodriguezRotation3Base.h,v $
// Revision 1.12  2006/01/20 04:46:36  anton
// cisstVector: Added methods AlmostEquivalent for all transformations as well
// as some missing Equal (and ==).  See ticket #204.
//
// Revision 1.11  2005/12/07 23:16:41  anton
// Transformations: All IsNormalized() tests now consider that any angle is
// valid.  These don't break the conversion methods and any range seems
// arbitrary.  This is related to ticket #110.
//
// Revision 1.10  2005/12/05 22:46:30  anton
// cisstVector: Correct major bug in Rodriguez normalization and tests.
//
// Revision 1.9  2005/12/02 03:05:43  anton
// cisstVector: Update on AlmostEqual() methods for axis angle and Rodriguez.
//
// Revision 1.8  2005/12/01 21:02:47  ofri
// vctRodriguezRotation3Base: Added return value to some methods, following
// ticket #196
//
// Revision 1.7  2005/11/17 22:48:50  anton
// cisstVector transformations: Added required explicit casts to avoid warnings
// with Visual Studio compiler.  See checkin [1528] and ticket #192.
//
// Revision 1.6  2005/11/15 03:37:50  anton
// vctAxisAngle and vctRodriguez: Major updates re. ticket #110. Work in progress.
//
// Revision 1.5  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.4  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/09/24 00:01:04  anton
// cisstVector: Use cmnThrow for all exceptions.
//
// Revision 1.2  2005/09/05 22:54:22  ofri
// Added constructor for vctRodriguezRotation3Base, following ticket #169.  Plus a
// few cosmetic changes, such as replacing const_reference parameters with
// value_type
//
// Revision 1.1  2005/09/01 06:18:08  anton
// cisstVector transformations: Added a level of abstraction for all classes
// derived from fixed size containers (vctQuaternion, vctQuaternionRotation3,
// vctMatrixRotation3, vctRodriguezRotation3).  These classes are now derived
// from vctXyzBase<_containerType>.  This will ease the SWIG wrapping.
//
//
// ****************************************************************************

