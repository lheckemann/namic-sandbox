/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctRandom.cpp,v 1.9 2005/12/01 22:11:58 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2005-01-31

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

#include <cisstVector/vctRandom.h>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctAxisAngleRotation3.h>
#include <cisstVector/vctRodriguezRotation3.h>

#include <cisstVector/vctMatrixRotation2.h>
#include <cisstVector/vctAngleRotation2.h>


template <class _containerType>
void vctRandom(vctMatrixRotation3Base<_containerType> & matrixRotation) {
    typedef typename _containerType::value_type value_type;
    vctQuaternionRotation3<value_type> quaternionRotation;
    vctRandom(quaternionRotation);
    matrixRotation.From(quaternionRotation);
}

template void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrix<double, 3, 3> > &);
template void vctRandom(vctMatrixRotation3Base<vctFixedSizeMatrix<float, 3, 3> > &);
template void vctRandom(vctMatrixRotation3Base<vctDynamicMatrix<double> > &);



template <class _containerType>
void vctRandom(vctQuaternionRotation3Base<_containerType> & quaternionRotation) {
    typedef typename _containerType::value_type value_type;
    vctRandom(quaternionRotation, (value_type) -1.0, (value_type) 1.0);
    quaternionRotation.NormalizedSelf();
}

template void vctRandom(vctQuaternionRotation3Base<vctFixedSizeVector<double, 4> > &);
template void vctRandom(vctQuaternionRotation3Base<vctFixedSizeVector<float, 4> > &);
template void vctRandom(vctQuaternionRotation3Base<vctDynamicVector<double> > &);




template <class _elementType>
void vctRandom(vctAxisAngleRotation3<_elementType> & axisAngleRotation) {
    typedef _elementType value_type;
    vctQuaternionRotation3<value_type> quaternionRotation;
    vctRandom(quaternionRotation);
    axisAngleRotation.FromRaw(quaternionRotation);
}

template void vctRandom(vctAxisAngleRotation3<double> &);
template void vctRandom(vctAxisAngleRotation3<float> &);



template <class _containerType>
void vctRandom(vctRodriguezRotation3Base<_containerType> & rodriguezRotation) {
    typedef typename _containerType::value_type value_type;
    vctAxisAngleRotation3<value_type> axisAngleRotation;
    vctRandom(axisAngleRotation);
    rodriguezRotation.FromRaw(axisAngleRotation);
}

template void vctRandom(vctRodriguezRotation3Base<vctFixedSizeVector<double, 3> > &);
template void vctRandom(vctRodriguezRotation3Base<vctFixedSizeVector<float, 3> > &);
template void vctRandom(vctRodriguezRotation3Base<vctDynamicVector<double> > &);



template <class _containerType>
void vctRandom(vctMatrixRotation2Base<_containerType> & matrixRotation) {
    vctAngleRotation2 angleRotation;
    vctRandom(angleRotation);
    matrixRotation.From(angleRotation);
}

template void vctRandom(vctMatrixRotation2Base<vctFixedSizeMatrix<double, 2, 2> > &);
template void vctRandom(vctMatrixRotation2Base<vctFixedSizeMatrix<float, 2, 2> > &);
template void vctRandom(vctMatrixRotation2Base<vctDynamicMatrix<double> > &);


void vctRandom(vctAngleRotation2 & angleRotation) {
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValue(0.0, 2 * cmnPI,
                                      angleRotation.Angle());
}



// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctRandom.cpp,v $
// Revision 1.9  2005/12/01 22:11:58  anton
// cisstVector: 2D transformations, added a base class for rotation matrix 2.
//
// Revision 1.8  2005/11/15 03:23:02  anton
// vctRandom: Added support for vctRodriguezRotation3 and vctAxisAngleRotation3.
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
// Revision 1.4  2005/07/22 18:54:49  ofri
// vctRandom.cpp: In response to ticket #156, moved the #include of the rotation
// and frame headers to the cpp file.  This seems to reduce the test compilation
// dependencies and time significantly.
//
// Revision 1.3  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.2  2005/02/24 01:51:50  anton
// cisstVector: Added classes vctAngleRotation2 and vctMatrixRotation2.  This
// code has not been tested.   The normalization and conversion methods have
// to be reviewed.
//
// Revision 1.1  2005/02/04 16:56:54  anton
// cisstVector: Added classes vctAxisAngleRotation3 and vctRodriguezRotation3
// as planed in ticket #109.  Also updated vctMatrixRotation3 to use these new
// classes.  Other updates include vctRandom(rotation), new vctTypes and
// forward declarations.
//
//
// ****************************************************************************
