/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctAxisAngleRotation3.cpp,v 1.9 2006/03/27 13:52:36 kapoor Exp $
  
  Author(s):  Anton Deguet
  Created on:  2004-01-13

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


#include <cisstCommon/cmnConstants.h>
#include <cisstVector/vctAxisAngleRotation3.h>
#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctMatrixRotation3.h>

template<>
const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3<double>::Identity()
{
    static const vctAxisAngleRotation3<double> result(vctFixedSizeVector<double, DIMENSION>(1.0, 0.0, 0.0), 0.0);
    return result;
}


template<>
const vctAxisAngleRotation3<float> &
vctAxisAngleRotation3<float>::Identity()
{
    static const vctAxisAngleRotation3<float> result(vctFixedSizeVector<float, DIMENSION>(1.0f, 0.0f, 0.0f), 0.0);
    return result;
}


template <class _elementType, class _containerType>
const vctAxisAngleRotation3<_elementType> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<_elementType> & axisAngleRotation,
                             const vctQuaternionRotation3Base<_containerType> & quaternionRotation) {
    const _elementType r = quaternionRotation.R();
    axisAngleRotation.Angle() = acos(r) * 2.0;
    double oneMinusR2 = 1.0 - r * r;
    if (oneMinusR2 < cmnTypeTraits<_elementType>::Tolerance())
        oneMinusR2 = 0.0;
    _elementType sinAngle = _elementType(sqrt(oneMinusR2));
    if (vctUnaryOperations<_elementType>::AbsValue::Operate(sinAngle) > cmnTypeTraits<_elementType>::Tolerance()) {
        axisAngleRotation.Axis().X() = quaternionRotation.X() / sinAngle;
        axisAngleRotation.Axis().Y() = quaternionRotation.Y() / sinAngle;
        axisAngleRotation.Axis().Z() = quaternionRotation.Z() / sinAngle;
    } else {
        axisAngleRotation.Axis().X() = quaternionRotation.X();
        axisAngleRotation.Axis().Y() = quaternionRotation.Y();
        axisAngleRotation.Axis().Z() = quaternionRotation.Z();
    }
    return axisAngleRotation;
}


template <class _elementType, class _containerType>
const vctAxisAngleRotation3<_elementType> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<_elementType> & axisAngleRotation,
                             const vctMatrixRotation3Base<_containerType> & matrixRotation) {

    typedef vctAxisAngleRotation3<_elementType> AxisAngleRotationType;
    typedef typename AxisAngleRotationType::NormType NormType;
    typedef typename AxisAngleRotationType::AngleType AngleType;

    const NormType normTolerance = cmnTypeTraits<NormType>::Tolerance();
    const NormType trace = matrixRotation.Element(0, 0) + matrixRotation.Element(1, 1) + matrixRotation.Element(2, 2); // 2 * cos(angle) + 1
    const NormType xSin = matrixRotation.Element(2, 1) - matrixRotation.Element(1, 2); // 2 * x * sin(angle)
    const NormType ySin = matrixRotation.Element(0, 2) - matrixRotation.Element(2, 0); // 2 * y * sin(angle)
    const NormType zSin = matrixRotation.Element(1, 0) - matrixRotation.Element(0, 1); // 2 * z * sin(angle)
    const NormType normSquare = xSin * xSin + ySin * ySin + zSin * zSin; 
    NormType norm;
    if (normSquare < normTolerance) {
        norm = 0.0;
    } else {
        norm = sqrt(normSquare); // 2 * |sin(angle)|
    }
    
    // either 0 or PI
    if (norm == 0.0) {
        const NormType traceMinus3 = trace - NormType(3);
        // if the angle is 0, then cos(angle) = 1, and trace = 2*cos(angle) + 1 = 3
        if ( (traceMinus3 > -normTolerance) && (traceMinus3 < normTolerance) ) {
            axisAngleRotation.Angle() = _elementType(0);
            axisAngleRotation.Axis().Assign(_elementType(0), _elementType(0), _elementType(1));
            return axisAngleRotation;
        }
        // since norm is already 0, we are in the other case, i.e., angle-PI, but we just want
        // to assert that trace = -1
        CMN_ASSERT( (trace > (NormType(-1) - normTolerance)) && (trace < (NormType(-1) + normTolerance)) );
        // the diagonal is [k_x*k_x*v + c ,  k_y*k_y*v + c,  k_z*k_z*v + c]
        // c = -1 ;  v = (1 - c) = 2
        NormType xSquare = (matrixRotation.Element(0, 0) + 1) / 2;
        NormType ySquare = (matrixRotation.Element(1, 1) + 1) / 2;
        NormType zSquare = (matrixRotation.Element(2, 2) + 1) / 2;
        if (xSquare < normTolerance)
            xSquare = 0;
        if (ySquare < normTolerance)
            ySquare = 0;
        if (zSquare < normTolerance)
            zSquare = 0;
        NormType x = sqrt(xSquare);
        NormType y = sqrt(ySquare);
        NormType z = sqrt(zSquare);
        // we arbitrarily decide the k_x is positive, if it's zero then k_y is positive, and if both are zero, then k_z is positive
        if (x > 0) {
            if (matrixRotation.Element(1, 0) < 0) // Element(1,0) = k_x*k_y*v , where v=2, so we just need to check its sign
                y = -y;
            if (matrixRotation.Element(2, 0) < 0) // Element(2,0) = k_x*k_z*v
                z = -z;
        } else if (y > 0) {
            if (matrixRotation.Element(2, 1) < 0) // Element(2,1) = k_y*k_z*v
                z = -z;
        } else {
            z = 1.0; // x and y are zero, Z has to be one
        }
        axisAngleRotation.Axis().Assign(_elementType(x), _elementType(y), _elementType(z));
        axisAngleRotation.Angle() = _elementType(cmnPI);
        return axisAngleRotation;
    }
    
    const AngleType angle = atan2(norm / 2, (trace - 1) / 2);
    axisAngleRotation.Axis().Assign(_elementType(xSin), _elementType(ySin), _elementType(zSin));
    axisAngleRotation.Axis().NormalizedSelf();
    axisAngleRotation.Angle() = _elementType(angle);
    
    return axisAngleRotation;
}


template <class _elementType, class _containerType>
const vctAxisAngleRotation3<_elementType> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<_elementType> & axisAngleRotation,
                             const vctRodriguezRotation3Base<_containerType> & rodriguezRotation) {
    
    typedef vctAxisAngleRotation3<_elementType> AxisAngleRotationType;
    typedef typename AxisAngleRotationType::value_type value_type;
    typedef typename AxisAngleRotationType::NormType NormType;
    typedef typename AxisAngleRotationType::AngleType AngleType;

    const NormType axisLength = rodriguezRotation.Norm();
    const value_type axisTolerance = cmnTypeTraits<value_type>::Tolerance();
    const AngleType angle = (axisLength < axisTolerance) ? 0.0 : axisLength;
    axisAngleRotation.Angle() = angle;
    if (angle == 0.0) {
        axisAngleRotation.Axis().Assign(value_type(1), value_type(0), value_type(0));
    } else {
        axisAngleRotation.Axis().Assign(rodriguezRotation.X() / static_cast<value_type>(axisLength),
                                        rodriguezRotation.Y() / static_cast<value_type>(axisLength),
                                        rodriguezRotation.Z() / static_cast<value_type>(axisLength));
    }
    return axisAngleRotation;
}


// force the instantiation of the templated classes
template class vctAxisAngleRotation3<double>;
template class vctAxisAngleRotation3<float>;


// force instantiation of helper functions
template const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation);
template const vctAxisAngleRotation3<float> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<float> & axisAngleRotation,
                             const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation);

template const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > & matrixRotation);
template const vctAxisAngleRotation3<float> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<float> & axisAngleRotation,
                             const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > & matrixRotation);

template const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > & rodriguezRotation);
template const vctAxisAngleRotation3<float> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<float> & axisAngleRotation,
                             const vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > & rodriguezRotation);


// force instantiation for dynamic containers, this is used for Python wrappers
template const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctQuaternionRotation3Base<vctDynamicVector<double> > & quaternionRotation);

template const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation);

template const vctAxisAngleRotation3<double> &
vctAxisAngleRotation3FromRaw(vctAxisAngleRotation3<double> & axisAngleRotation,
                             const vctRodriguezRotation3Base<vctDynamicVector<double> > & rodriguezRotation);


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctAxisAngleRotation3.cpp,v $
// Revision 1.9  2006/03/27 13:52:36  kapoor
// vctAxisAngleRotation: changed plain assert to CMN_ASSERT, in method to convert
// rotation to axis angle. This method must be revisited and fixed as it does *NOT*
// handle all cases.
//
// Revision 1.8  2005/11/15 03:24:29  anton
// cisstVector transformations:  Use AngleType and NormType instead of
// value_type for local variables to increase numerical stability.
//
// Revision 1.7  2005/10/17 21:59:57  anton
// cisst: Introduced cmnPI and co.  Updated whole repository. See #131.
//
// Revision 1.6  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.5  2005/09/01 06:18:08  anton
// cisstVector transformations: Added a level of abstraction for all classes
// derived from fixed size containers (vctQuaternion, vctQuaternionRotation3,
// vctMatrixRotation3, vctRodriguezRotation3).  These classes are now derived
// from vctXyzBase<_containerType>.  This will ease the SWIG wrapping.
//
// Revision 1.4  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.3  2005/02/24 03:37:01  anton
// vctAxisAngleRotation3: New code for From(vctMatrixRotation3) based on
// algorithm developed by Ofri for vctRodriguezRotation3::From(vctMatrixRotation3).
//
// Revision 1.2  2005/02/08 22:09:15  ofri
// Following ticket #127, I added some safety checks in conversions to
// vctAxisAngleRotation3 and vctRodriguezRotation3 From other types.
// It is still necessary to go over all the evaluations of sqrt in our code and make
// sure that it doesn't take negative input.
//
// Revision 1.1  2005/02/04 16:56:54  anton
// cisstVector: Added classes vctAxisAngleRotation3 and vctRodriguezRotation3
// as planed in ticket #109.  Also updated vctMatrixRotation3 to use these new
// classes.  Other updates include vctRandom(rotation), new vctTypes and
// forward declarations.
//
//
// ****************************************************************************
