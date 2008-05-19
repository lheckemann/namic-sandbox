/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctMatrixRotation2Base.cpp,v 1.1 2005/12/01 22:11:58 anton Exp $

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


#include <cisstVector/vctMatrixRotation2Base.h>


template <>
vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> >::
vctMatrixRotation2Base(const double & element00, const double & element01,
                       const double & element10, const double & element11)
    throw(std::runtime_error)
{
    this->From(element00, element01,
               element10, element11);
}

template <>
const vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > &
vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> >::Identity()
{
    static const ThisType result(1.0, 0.0,
                                 0.0, 1.0);
    return result;
}

template <>
vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> >::
vctMatrixRotation2Base(const float & element00, const float & element01,
                       const float & element10, const float & element11)
    throw(std::runtime_error)
{
    this->From(element00, element01,
               element10, element11);
}

template <>
const vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > &
vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> >::Identity()
{
    static const ThisType result(1.0f, 0.0f,
                                 0.0f, 1.0f);
    return result;
}



template <class _containerType>
const vctMatrixRotation2Base<_containerType> &
vctMatrixRotation2Base<_containerType>::FromRaw(const vctAngleRotation2 & angleRotation) {
    typedef typename _containerType::value_type value_type;
    const typename vctAngleRotation2::AngleType angle = angleRotation.Angle();
    const value_type sinAngle = value_type(sin(angle));
    const value_type cosAngle = value_type(cos(angle));
    // first column
    this->Element(0, 0) = cosAngle;
    this->Element(1, 0) = -sinAngle;
    // second column
    this->Element(0, 1) = sinAngle;
    this->Element(1, 1) = cosAngle;
    return *this;
}



// specialize for fixed size matrices
template <>
const vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > &
vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> >::NormalizedSelf(void) {
    this->FromRaw(vctAngleRotation2(*this, VCT_DO_NOT_NORMALIZE));
    return *this;
}

template <>
const vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > &
vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> >::NormalizedSelf(void) {
    this->FromRaw(vctAngleRotation2(*this, VCT_DO_NOT_NORMALIZE));
    return *this;
}


// force the instantiation of the templated classes
template class vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> >;
template class vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> >;


template <>
vctMatrixRotation2Base<vctDynamicMatrix<double> >::
vctMatrixRotation2Base(const double & element00, const double & element01,
                       const double & element10, const double & element11)
    throw(std::runtime_error)
{
    this->SetSize(ROWS, COLS);
    this->From(element00, element01,
               element10, element11);
}

template <>
const vctMatrixRotation2Base<vctDynamicMatrix<double> > &
vctMatrixRotation2Base<vctDynamicMatrix<double> >::Identity()
{
    static const ThisType result(1.0, 0.0,
                                 0.0, 1.0);
    return result;
}


template <>
const vctMatrixRotation2Base<vctDynamicMatrix<double> > &
vctMatrixRotation2Base<vctDynamicMatrix<double> >::NormalizedSelf(void)
{
    this->FromRaw(vctAngleRotation2(*this, VCT_DO_NOT_NORMALIZE));
    return *this;
}



// force instantiation for dynamic containers, this is used for Python wrappers
template class vctMatrixRotation2Base<vctDynamicMatrix<double> >;


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctMatrixRotation2Base.cpp,v $
// Revision 1.1  2005/12/01 22:11:58  anton
// cisstVector: 2D transformations, added a base class for rotation matrix 2.
//
//
// ****************************************************************************
