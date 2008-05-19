/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctQuaternionBase.h,v 1.6 2005/10/12 18:57:08 pkaz Exp $

  Author(s):  Anton Deguet
  Created on:  2005-08-18

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
  \brief Declaration of vctQuaternionBase
 */


#ifndef _vctQuaternionBase_h
#define _vctQuaternionBase_h


#include <cisstCommon/cmnTypeTraits.h>
#include <cisstVector/vctContainerTraits.h>

// always the last file to include
#include <cisstVector/vctExport.h>


/*!
  \brief Define a quaternion container.

  It is important to note that the class vctQuaternion is for
  any quaternion, i.e. it does not necessary represent a unit
  quaternion.

  \param _baseType The base class used to contain the 4 elements.

  \sa vctFixedSizeVector, vctDynamicVector
*/
template <class _containerType>
class vctQuaternionBase: public _containerType
{
 public:
    enum {SIZE = 4};
    typedef _containerType BaseType;
    typedef _containerType ContainerType;
    typedef vctQuaternionBase<_containerType> ThisType;

    /* no need to document, inherit doxygen documentation from base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(typename ContainerType::value_type);
    typedef cmnTypeTraits<value_type> TypeTraits;

    /*! Default constructor.  This constructor has to be instantiated
        be type of container.  Methods which create an object of
        ThisType will rely on the default constructor for memory
        allocation (if needed). */
    CISST_EXPORT vctQuaternionBase(void);

    /* Methods X, Y, Z are inherited, add methods R() */

    /*! Returns the last element of the quaternion, i.e. the real
      part.  This method is const.  The methods X(), Y() and Z() are
      inherited from the base class. */
    const_reference R(void) const {
        return *(this->Pointer(3));
    }
    
    /*! Access the last element of the quaternion, i.e. the real part.
      This method is not const. The methods X(), Y() and Z() are
      inherited from the base class. */
    reference R(void) {
        return *(this->Pointer(3));
    }
    

    /*! Sets this quaternion as the conjugate of another one.
      \param otherQuaternion Quaternion used to compute the conjugate. */
    inline const ThisType & ConjugateOf(const ThisType & otherQuaternion) {
        this->X() = - otherQuaternion.X();
        this->Y() = - otherQuaternion.Y();
        this->Z() = - otherQuaternion.Z();
        this->R() = otherQuaternion.R();
        return *this;
    }


    /*! Replaces this quaternion by its conjugate. */
    inline const ThisType & ConjugateSelf(void) {
        this->X() = - this->X();
        this->Y() = - this->Y();
        this->Z() = - this->Z();
        return *this;
    }


    /*! Returns the conjugate of this quaternion.  This method returns
      a copy of the conjugate and does not modify this quaternion. */
    inline ThisType Conjugate(void) const {
        ThisType result;
        result.ConjugateOf(*this);
        return result;
    }


    /*! Set this quaternion as the product of two other ones.
      \param quat1 Left operand 
      \param quat2 Right operand 
      \note Quaternion product in non-commutative.
    */
    inline const ThisType & ProductOf(const ThisType & quat1,
                                      const ThisType & quat2) {
        this->X() = quat1.R() * quat2.X() +  quat1.X() * quat2.R() +  quat1.Y() * quat2.Z() -  quat1.Z() * quat2.Y();
        this->Y() = quat1.R() * quat2.Y() -  quat1.X() * quat2.Z() +  quat1.Y() * quat2.R() +  quat1.Z() * quat2.X();
        this->Z() = quat1.R() * quat2.Z() +  quat1.X() * quat2.Y() -  quat1.Y() * quat2.X() +  quat1.Z() * quat2.R();
        this->R() = quat1.R() * quat2.R() -  quat1.X() * quat2.X() -  quat1.Y() * quat2.Y() -  quat1.Z() * quat2.Z();
        return *this;
    }

    /*! Compute the quotient quat1 / quat2.  The reciprocal of
      a quaternion q is  conj(q) / norm(q) */
    inline const ThisType & QuotientOf(const ThisType & quat1,
                                       const ThisType & quat2) {
        const value_type quat2Norm = value_type(quat2.Norm());
        BaseType quat2I;
        quat2I.Assign(-quat2.X() / quat2Norm, -quat2.Y() / quat2Norm, -quat2.Z() / quat2Norm, quat2.R() / quat2Norm);
        this->X() = quat1.R() * quat2I.X() +  quat1.X() * quat2.R() +  quat1.Y() * quat2I.Z() -  quat1.Z() * quat2I.Y();
        this->Y() = quat1.R() * quat2I.Y() -  quat1.X() * quat2I.Z() +  quat1.Y() * quat2.R() +  quat1.Z() * quat2I.X();
        this->Z() = quat1.R() * quat2I.Z() +  quat1.X() * quat2I.Y() -  quat1.Y() * quat2I.X() +  quat1.Z() * quat2.R();
        this->R() = quat1.R() * quat2.R() -  quat1.X() * quat2I.X() -  quat1.Y() * quat2I.Y() -  quat1.Z() * quat2I.Z();
        return *this;
    }
    
    /*! Post-multiply this quaternion by another, i.e., this = this * other.
      \note Quaternion product is non-commutative.
    */
    inline ThisType & PostMultiply(const ThisType & other) {
        ThisType result;
        result = ProductOf(*this, other);
        *this = result;
        return *this;
    }

    /*! Pre-multiply this quaternion by another, i.e., this = other * this.
      \note Quaternion product is non-commutative.
    */
    inline ThisType & PreMultiply(const ThisType & other) {
        ThisType result;
        result = ProductOf(other, *this);
        *this = result;
        return *this;
    }
    
    /*! Divide this quaternion by another, i.e., this = this / other. */
    inline ThisType & Divide(const ThisType & other) {
        ThisType result;
        result.QuotientOf(*this, other);
        *this = result;
        return *this;
    }
    
    /*! Divide this quaternion by a scalar: equal to elementwise division.
      The method re-implements vector elementwise division, which is otherwise
      shadowed by the Divide(ThisType) method.
    */
    inline ThisType & Divide(const value_type s) {
        (static_cast<BaseType *>(this))->Divide(s);
        return *this;
    }

};


/*! Product of two quaternions.  This operator relies on the method
  vctQuaternion::ProductOf.

  \param quat1 Left operand
  \param quat2 Right operand
*/
template <class _containerType>
inline vctQuaternionBase<_containerType>
operator * (const vctQuaternionBase<_containerType> & quat1,
            const vctQuaternionBase<_containerType> & quat2) {
    vctQuaternionBase<_containerType> result;
    result.ProductOf(quat1, quat2);
    return result;
}


/*! Quotient of two quaternions.  This operator relies on the method
  vctQuaternion::QuotientOf.

  \param quat1 Left operand
  \param quat2 Right operand
*/
template <class _containerType>
inline vctQuaternionBase<_containerType>
operator / (const vctQuaternionBase<_containerType> & quat1,
            const vctQuaternionBase<_containerType> & quat2) {
    vctQuaternionBase<_containerType> result;
    result.QuotientOf(quat1, quat2);
    return result;
}


#endif  // _vctQuaternionBase_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctQuaternionBase.h,v $
// Revision 1.6  2005/10/12 18:57:08  pkaz
// vctQuaternionBase.h:  added include of vctExport.h
//
// Revision 1.5  2005/10/08 20:16:24  anton
// vctQuaternionBase: Sepcialized default ctor to perform required memory
// allocation if derived from dynamic vector.
//
// Revision 1.4  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.3  2005/09/28 20:28:50  anton
// libs documentation: Corrected Doxygen links.
//
// Revision 1.2  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.1  2005/09/01 06:18:08  anton
// cisstVector transformations: Added a level of abstraction for all classes
// derived from fixed size containers (vctQuaternion, vctQuaternionRotation3,
// vctMatrixRotation3, vctRodriguezRotation3).  These classes are now derived
// from vctXyzBase<_containerType>.  This will ease the SWIG wrapping.
//
//
// ****************************************************************************

