/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctMatrixRotation3Base.cpp,v 1.8 2005/11/17 22:48:50 anton Exp $

  Author(s):  Anton Deguet
  Created on:  2005-08-20

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


#include <cisstVector/vctMatrixRotation3Base.h>

template <>
vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> >::
vctMatrixRotation3Base(const double & element00, const double & element01, const double & element02,
                       const double & element10, const double & element11, const double & element12,
                       const double & element20, const double & element21, const double & element22)
    throw(std::runtime_error)
{
    this->From(element00, element01, element02,
               element10, element11, element12,
               element20, element21, element22);
}

template <>
const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > &
vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> >::Identity()
{
    static const ThisType result(1.0, 0.0, 0.0,
                                 0.0, 1.0, 0.0,
                                 0.0, 0.0, 1.0);
    return result;
}

template <>
vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> >::
vctMatrixRotation3Base(const float & element00, const float & element01, const float & element02,
                       const float & element10, const float & element11, const float & element12,
                       const float & element20, const float & element21, const float & element22)
    throw(std::runtime_error)
{
    this->From(element00, element01, element02,
               element10, element11, element12,
               element20, element21, element22);
}

template <>
const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > &
vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> >::Identity()
{
    static const ThisType result(1.0f, 0.0f, 0.0f,
                                 0.0f, 1.0f, 0.0f,
                                 0.0f, 0.0f, 1.0f);
    return result;
}



template <class _containerType>
const vctMatrixRotation3Base<_containerType> &
vctMatrixRotation3Base<_containerType>::FromRaw(const vctAxisAngleRotation3<typename _containerType::value_type> & axisAngleRotation) {

    typedef vctAxisAngleRotation3<value_type> AxisAngleType;

    const typename AxisAngleType::AngleType angle = axisAngleRotation.Angle();
    const typename AxisAngleType::AxisType axis = axisAngleRotation.Axis();
    
    const AngleType sinAngle = AngleType(sin(angle));
    const AngleType cosAngle = AngleType(cos(angle));
    const AngleType CompCosAngle = 1 - cosAngle;
    
    // first column
    this->Element(0, 0) = value_type(axis[0] * axis[0] * CompCosAngle + cosAngle);
    this->Element(1, 0) = value_type(axis[0] * axis[1] * CompCosAngle + axis[2] * sinAngle);
    this->Element(2, 0) = value_type(axis[0] * axis[2] * CompCosAngle - axis[1] * sinAngle);
    
    // second column
    this->Element(0, 1) = value_type(axis[1] * axis[0] * CompCosAngle - axis[2] * sinAngle);
    this->Element(1, 1) = value_type(axis[1] * axis[1] * CompCosAngle + cosAngle);
    this->Element(2, 1) = value_type(axis[1] * axis[2] * CompCosAngle + axis[0] * sinAngle);
    
    // third column
    this->Element(0, 2) = value_type(axis[2] * axis[0] * CompCosAngle + axis[1] * sinAngle);
    this->Element(1, 2) = value_type(axis[2] * axis[1] * CompCosAngle - axis[0] * sinAngle);
    this->Element(2, 2) = value_type(axis[2] * axis[2] * CompCosAngle + cosAngle);
    
    return *this;
}




template <class _matrixType, class _quaternionType>
const vctMatrixRotation3Base<_matrixType> &
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<_matrixType> & matrixRotation,
                              const vctQuaternionRotation3Base<_quaternionType> & quaternionRotation) {

    typedef typename _matrixType::value_type value_type;

    value_type xx = quaternionRotation.X() * quaternionRotation.X();
    value_type xy = quaternionRotation.X() * quaternionRotation.Y();
    value_type xz = quaternionRotation.X() * quaternionRotation.Z();
    value_type xr = quaternionRotation.X() * quaternionRotation.R();
    value_type yy = quaternionRotation.Y() * quaternionRotation.Y();
    value_type yz = quaternionRotation.Y() * quaternionRotation.Z();
    value_type yr = quaternionRotation.Y() * quaternionRotation.R();
    value_type zz = quaternionRotation.Z() * quaternionRotation.Z();
    value_type zr = quaternionRotation.Z() * quaternionRotation.R();
    matrixRotation.Assign(1 - 2 * (yy + zz), 2 * (xy - zr),     2 * (xz + yr),
                          2 * (xy + zr),     1 - 2 * (xx + zz), 2 * (yz - xr),
                          2 * (xz - yr),     2 * (yz + xr),     1 - 2 * (xx + yy));
    return matrixRotation;
}



// specialize for fixed size matrices
template <>
const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > &
vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> >::NormalizedSelf(void) {
    vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > quaternion;
    quaternion.FromRaw(*this);
    quaternion.NormalizedSelf();
    this->From(quaternion);
    return *this;
}

template <>
const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > &
vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> >::NormalizedSelf(void) {
    vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > quaternion;
    quaternion.FromRaw(*this);
    quaternion.NormalizedSelf();
    this->From(quaternion);
    return *this;
}


// force the instantiation of the templated classes
template class vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> >;
template class vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> >;


template <>
vctMatrixRotation3Base<vctDynamicMatrix<double> >::
vctMatrixRotation3Base(const double & element00, const double & element01, const double & element02,
                       const double & element10, const double & element11, const double & element12,
                       const double & element20, const double & element21, const double & element22)
    throw(std::runtime_error)
{
    this->SetSize(ROWS, COLS);
    this->From(element00, element01, element02,
               element10, element11, element12,
               element20, element21, element22);
}

template <>
const vctMatrixRotation3Base<vctDynamicMatrix<double> > &
vctMatrixRotation3Base<vctDynamicMatrix<double> >::Identity()
{
    static const ThisType result(1.0, 0.0, 0.0,
                                 0.0, 1.0, 0.0,
                                 0.0, 0.0, 1.0);
    return result;
}


template <>
const vctMatrixRotation3Base<vctDynamicMatrix<double> > &
vctMatrixRotation3Base<vctDynamicMatrix<double> >::NormalizedSelf(void) {

    vctQuaternionRotation3Base<vctDynamicVector<double> > quaternion;
    quaternion.FromRaw(*this);
    quaternion.NormalizedSelf();
    this->From(quaternion);
    return *this;
}


// force instantiation of helper functions
template const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > &
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
template const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > &
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation);


// force instantiation for dynamic containers, this is used for Python wrappers
template const vctMatrixRotation3Base<vctDynamicMatrix<double> > &
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctDynamicVector<double> > & quaternionRotation);

template const vctMatrixRotation3Base<vctDynamicMatrix<double> > &
vctMatrixRotation3BaseFromRaw(vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation,
                              const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);


template class vctMatrixRotation3Base<vctDynamicMatrix<double> >;


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctMatrixRotation3Base.cpp,v $
// Revision 1.8  2005/11/17 22:48:50  anton
// cisstVector transformations: Added required explicit casts to avoid warnings
// with Visual Studio compiler.  See checkin [1528] and ticket #192.
//
// Revision 1.7  2005/11/15 03:24:29  anton
// cisstVector transformations:  Use AngleType and NormType instead of
// value_type for local variables to increase numerical stability.
//
// Revision 1.6  2005/10/10 01:13:13  anton
// vctMatrixRotation3Base: Instantiations didn't match declaration (throw)
//
// Revision 1.5  2005/10/10 00:53:39  anton
// vctMatrixRotation3Base: Specialize the ctor from 9 elements based on the
// containerType.  This allows to use this ctor in Identity() and create a true
// static variable (see #179).
//
// Revision 1.4  2005/10/07 03:26:31  anton
// vctMatrixRotation3Base.cpp: Modified implementation of Identity() for dynamic
// matrices to avoid recusrsive call of default constructor.
//
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/09/07 03:29:48  anton
// vct{matrix,Quaternion}Rotation3Base.cpp: Changed order of instantiation for
// gcc 4.0 (worked with gcc 3.3 and 3.4).
//
// Revision 1.1  2005/09/01 06:18:08  anton
// cisstVector transformations: Added a level of abstraction for all classes
// derived from fixed size containers (vctQuaternion, vctQuaternionRotation3,
// vctMatrixRotation3, vctRodriguezRotation3).  These classes are now derived
// from vctXyzBase<_containerType>.  This will ease the SWIG wrapping.
//
//
// ****************************************************************************
