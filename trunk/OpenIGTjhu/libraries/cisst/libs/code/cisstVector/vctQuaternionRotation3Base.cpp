/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctQuaternionRotation3Base.cpp,v 1.5 2005/11/17 22:48:50 anton Exp $
  
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


#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctQuaternionRotation3Base.h>
#include <cisstVector/vctMatrixRotation3Base.h>

template<>
const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > &
vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> >::Identity()
{
    static const ThisType result(0.0, 0.0, 0.0, 1.0);
    return result;
}

template<>
const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > &
vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> >::Identity()
{
    static const ThisType result(0.0f, 0.0f, 0.0f, 1.0f);
    return result;
}


template <class _quaternionType, class _matrixType>
const vctQuaternionRotation3Base<_quaternionType> &
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<_quaternionType> & quaternionRotation,
                                  const vctMatrixRotation3Base<_matrixType> & matrixRotation)
{
    typedef typename _quaternionType::value_type value_type;
    typedef typename _quaternionType::TypeTraits TypeTraits;
    typedef typename _quaternionType::NormType NormType;

    NormType trace, traceInverse;
    trace = NormType(1.0) + matrixRotation.Element(0, 0) + matrixRotation.Element(1, 1) + matrixRotation.Element(2, 2);
    if (vctUnaryOperations<NormType>::AbsValue::Operate(trace) > cmnTypeTraits<NormType>::Tolerance()) {
        trace = NormType(sqrt(trace) * 2.0);
        traceInverse = NormType(1.0) / trace;
        quaternionRotation.X() = value_type((matrixRotation.Element(2, 1) - matrixRotation.Element(1, 2)) * traceInverse);
        quaternionRotation.Y() = value_type((matrixRotation.Element(0, 2) - matrixRotation.Element(2, 0)) * traceInverse);
        quaternionRotation.Z() = value_type((matrixRotation.Element(1, 0) - matrixRotation.Element(0, 1)) * traceInverse);
        quaternionRotation.R() = value_type(0.25 * trace);
    } else 
        if (matrixRotation.Element(0, 0) > matrixRotation.Element(1, 1) && matrixRotation.Element(0, 0) > matrixRotation.Element(2, 2))  {
            trace  = NormType(sqrt(1.0 + matrixRotation.Element(0, 0) - matrixRotation.Element(1, 1) - matrixRotation.Element(2, 2)) * 2.0);
            traceInverse = NormType(1.0) / trace;
            quaternionRotation.X() = value_type(0.25 * trace);
            quaternionRotation.Y() = value_type((matrixRotation.Element(0, 1) + matrixRotation.Element(1, 0)) * traceInverse);
            quaternionRotation.Z() = value_type((matrixRotation.Element(2, 0) + matrixRotation.Element(0, 2)) * traceInverse);
            quaternionRotation.R() = value_type((matrixRotation.Element(1, 2) - matrixRotation.Element(2, 1)) * traceInverse);
        } else if (matrixRotation.Element(1, 1) > matrixRotation.Element(2, 2)) {
            trace  = NormType(sqrt(1.0 + matrixRotation.Element(1, 1) - matrixRotation.Element(0, 0) - matrixRotation.Element(2, 2)) * 2.0);
            traceInverse = NormType(1.0) / trace;
            quaternionRotation.X() = value_type((matrixRotation.Element(0, 1) + matrixRotation.Element(1, 0)) * traceInverse);
            quaternionRotation.Y() = value_type(0.25 * trace);
            quaternionRotation.Z() = value_type((matrixRotation.Element(1, 2) + matrixRotation.Element(2, 1)) * traceInverse);
            quaternionRotation.R() = value_type((matrixRotation.Element(2, 0) - matrixRotation.Element(0, 2)) * traceInverse);
        } else {
            trace  = NormType(sqrt(1.0 + matrixRotation.Element(2, 2) - matrixRotation.Element(0, 0) - matrixRotation.Element(1, 1)) * 2.0);
            traceInverse = NormType(1.0) / trace;
            quaternionRotation.X() = value_type((matrixRotation.Element(2, 0) + matrixRotation.Element(0, 2)) * traceInverse);
            quaternionRotation.Y() = value_type((matrixRotation.Element(1, 2) + matrixRotation.Element(2, 1)) * traceInverse);
            quaternionRotation.Z() = value_type(0.25 * trace);
            quaternionRotation.R() = value_type((matrixRotation.Element(0, 1) - matrixRotation.Element(1, 0)) * traceInverse);
        }
    return quaternionRotation;
}


// force the instantiation of the templated classes
template class vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> >;
template class vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> >;

// force instantiation of helper functions
template const vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > &
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > & matrixRotation);
template const vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > &
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > & matrixRotation);

// force instantiation for dynamic containers, this is used for Python wrappers
template <>
const vctQuaternionRotation3Base<vctDynamicVector<double> > &
vctQuaternionRotation3Base<vctDynamicVector<double> >::Identity()
{
    static ContainerType result(SIZE, 0.0, 0.0, 0.0, 1.0);
    return static_cast<ThisType &>(result);
}

template const vctQuaternionRotation3Base<vctDynamicVector<double> > &
vctQuaternionRotation3BaseFromRaw(vctQuaternionRotation3Base<vctDynamicVector<double> > & quaternionRotation,
                                  const vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation);

template class vctQuaternionRotation3Base<vctDynamicVector<double> >;


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctQuaternionRotation3Base.cpp,v $
// Revision 1.5  2005/11/17 22:48:50  anton
// cisstVector transformations: Added required explicit casts to avoid warnings
// with Visual Studio compiler.  See checkin [1528] and ticket #192.
//
// Revision 1.4  2005/11/15 03:24:29  anton
// cisstVector transformations:  Use AngleType and NormType instead of
// value_type for local variables to increase numerical stability.
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
