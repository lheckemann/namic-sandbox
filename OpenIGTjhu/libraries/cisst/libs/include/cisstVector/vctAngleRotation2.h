/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctAngleRotation2.h,v 1.10 2006/01/03 03:38:58 anton Exp $
  
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
  \brief Declaration of vctAngleRotation2
 */


#ifndef _vctAngleRotation2_h
#define _vctAngleRotation2_h


#include <cisstCommon/cmnConstants.h>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctExport.h>


/*!  \brief Define a rotation based on an angle for a
  space of dimension 2

  \ingroup cisstVector

  This class is not templated by the element type.  It contains an
  angle of type AngleType.

  \sa vctMatrixRotation2Base vctMatrixRotation2
*/
class vctAngleRotation2
{
 public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(double);
    enum {DIMENSION = 2};
    typedef vctAngleRotation2 ThisType;
    typedef cmnTypeTraits<AngleType> TypeTraits;

 protected:
    AngleType AngleMember;


    /*! Throw an exception unless this rotation is normalized. */
    inline void ThrowUnlessIsNormalized(void) const throw(std::runtime_error) {
        if (! IsNormalized()) {
            cmnThrow(std::runtime_error("vctAngleRotation2: This rotation is not normalized"));
        }
    }

    /*!
      Throw an exception unless the input is normalized.
      \param input An object with \c IsNormalized method.
    */
    template <class _inputType>
    inline void ThrowUnlessIsNormalized(const _inputType & input) const throw(std::runtime_error) {
        if (! input.IsNormalized()) {
            cmnThrow(std::runtime_error("vctAngleRotation2: Input is not normalized"));
        }
    }


 public:

    /*! Default constructor. Sets the angle to zero. */
    inline vctAngleRotation2():
        AngleMember(0.0)
    {}
    
    
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
    /*! Constructor from an angle (in radians). */
    inline explicit
    vctAngleRotation2(const AngleType angle)
        throw(std::runtime_error)
    {
        From(angle);
    }
    
    /*! Constructor from a rotation matrix. */
    template <class _containerType>
    inline vctAngleRotation2(const vctMatrixRotation2Base<_containerType> & matrixRotation)
        throw(std::runtime_error)
    {
        From(matrixRotation);
    }
    //@}


    /*!
      \name Constructors without normalization test
      
      These constructors will either assume that the input is
      normalized or normalize the input (a copy of it, if required)
      based on the last parameter provided.

      <ul>

      <li>If the normalization flag is set to #VCT_DO_NOT_NORMALIZE,
      the input is considered already normalized and the constructor
      will not perform any sanity check.  This can lead to numerical
      instabilities which have to be handled by the caller.

      <li>If the normalization flag is set to #VCT_NORMALIZE, the
      input will be normalized.  This option should be used whenever
      it is important to obtain a result as "normalized" as possible.

      </ul>
    */
    //@{
    /*! Constructor from an angle (in radians). */
    inline vctAngleRotation2(const AngleType angle,
                             bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(angle);
        } else {
            FromRaw(angle);
        }
    }
    
    /*! Constructor from a rotation matrix. */
    template <class _containerType>
    inline vctAngleRotation2(const vctMatrixRotation2Base<_containerType> & matrixRotation,
                             bool normalizeInput)
    {
        if (normalizeInput) {
            FromNormalized(matrixRotation);
        } else {
            FromRaw(matrixRotation);
        }
    }


    /*! Const reference to the identity.  In this case, the angle is
        set to zero. */
    static CISST_EXPORT const ThisType & Identity();


    inline const AngleType & Angle(void) const {
        return this->AngleMember;
    }

    inline AngleType & Angle(void) {
        return this->AngleMember;
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

    /*! Conversion from an angle (in radians). */
    inline const ThisType &
    From(const AngleType angle)
        throw(std::runtime_error)
    {
        this->AngleMember = angle;
        // this->ThrowUnlessIsNormalized();
        return *this;
    }
    
    /*! Conversion from a rotation matrix. */
    template <class _containerType>
    inline const ThisType &
    From(const vctMatrixRotation2Base<_containerType> & matrixRotation)
        throw(std::runtime_error)
    {
        this->ThrowUnlessIsNormalized(matrixRotation);
        this->AngleMember = atan2(matrixRotation.Element(0, 1), matrixRotation.Element(0, 0));
        return *this;
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

    /*! Conversion from an angle (in radians). */
    inline const ThisType &
    FromNormalized(const AngleType angle)
    {
        this->AngleMember = angle;
        this->NormalizedSelf();
        return *this;
    }
    
    /*! Conversion from a rotation matrix. */
    template <class _containerType>
    inline const ThisType &
    FromNormalized(const vctMatrixRotation2Base<_containerType> & matrixRotation) {
        return FromRaw(matrixRotation.Normalized());
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
    
    /*! Conversion from an angle (in radians). */ 
    inline const ThisType &
    FromRaw(const AngleType angle)
    {
        this->AngleMember = angle;
        return *this;
    }

    /*! Conversion from a rotation matrix. */
    template <class _containerType>
    inline const ThisType &
    FromRaw(const vctMatrixRotation2Base<_containerType> & matrixRotation) {
        AngleMember = atan2(matrixRotation.Element(0, 1), matrixRotation.Element(0, 0));
        return *this;
    }

    //@}


    /*! Inverse this angle rotation.  This methods assumes that the
        angle is betweem 0 and 2 * PI.  The angle is set to 2 * PI
        minus the previous value. */
    
    inline const ThisType & InverseSelf(void) {
        AngleMember = 2 * cmnPI - AngleMember;
        return *this;
    }

    /*! Set this rotation as the inverse of another one.  See also
        InverseSelf(). */
    inline const ThisType & InverseOf(const ThisType & otherRotation) {
        AngleMember = 2 * cmnPI - otherRotation.Angle();
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


    /*! Normalizes the angle.  This method ensures that the angle is
      between 0 and 2 * PI. */
    inline const ThisType & NormalizedSelf(void) {
        AngleMember = fmod(AngleMember, 2.0 * cmnPI);
        if (AngleMember < 0.0) {
            AngleMember += (2.0 * cmnPI);
        }
        return *this;
    }

    /*!
      Sets this rotation as the normalized version of another one.    

      \param otherRotation Angle rotation used to compute the
      normalized rotation. */
    inline const ThisType & NormalizedOf(const ThisType & otherRotation) {
        AngleMember = otherRotation.Angle();
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

    /*! Test if this rotation is normalized.  This methods always
      return "true" since any angle is considered valid.  This method
      is provided mostly for API completion.

      \param tolerance Tolerance.
    */
    inline bool IsNormalized(AngleType tolerance = TypeTraits::Tolerance()) const {
        return true;
    }


    std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
    
    inline void ToStream(std::ostream & outputStream) const {
        outputStream << "Angle: "
                     << std::endl
                     << this->Angle();
    }
};


/*! Stream out operator. */
inline
std::ostream & operator << (std::ostream & output,
                            const vctAngleRotation2 & angleRotation) {
    angleRotation.ToStream(output);
    return output;
}


#endif  // _vctAngleRotation2_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctAngleRotation2.h,v $
// Revision 1.10  2006/01/03 03:38:58  anton
// cisstVector Doxygen: Used \code \endcode instead of <pre> </pre>, added
// some missing <ul> </ul>, added links to VCT_NORMALIZE.
//
// Revision 1.9  2005/12/08 02:22:23  anton
// vctAngleRotation2.h: Remove #include for vctMatrixRotation2Base.h, circular
// inclusion.
//
// Revision 1.8  2005/12/07 23:16:41  anton
// Transformations: All IsNormalized() tests now consider that any angle is
// valid.  These don't break the conversion methods and any range seems
// arbitrary.  This is related to ticket #110.
//
// Revision 1.7  2005/12/06 16:39:58  anton
// vctAngleRotation2.h: Constructor from angle is now "explicit" and From()
// method uses vctMatrixRotation2Base instead of vctMatrixRotation2.
//
// Revision 1.6  2005/12/01 22:11:57  anton
// cisstVector: 2D transformations, added a base class for rotation matrix 2.
//
// Revision 1.5  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.4  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.2  2005/02/28 20:20:08  anton
// vctAngleRotation2: Moved definition of templated From(vctMatrixRotation2) to
// header file to avoid compilation issues with VCPP 7.0.  Also uses atan2.
//
// Revision 1.1  2005/02/24 01:51:50  anton
// cisstVector: Added classes vctAngleRotation2 and vctMatrixRotation2.  This
// code has not been tested.   The normalization and conversion methods have
// to be reviewed.
//
//
// ****************************************************************************

