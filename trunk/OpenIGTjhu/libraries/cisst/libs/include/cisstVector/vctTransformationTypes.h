/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctTransformationTypes.h,v 1.2 2005/12/23 21:27:31 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2003-09-12

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
  \brief Typedef for different transformations
*/


#ifndef _vctTransformationTypes_h
#define _vctTransformationTypes_h


#include <cisstVector/vctBarycentricVector.h>
#include <cisstVector/vctQuaternion.h>
#include <cisstVector/vctAxisAngleRotation3.h>
#include <cisstVector/vctRodriguezRotation3.h>
#include <cisstVector/vctQuaternionRotation3.h>
#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctAngleRotation2.h>
#include <cisstVector/vctMatrixRotation2.h>
#include <cisstVector/vctFrameBase.h>


/*! Define a vector of barycentric coordinates of 1 double. */
typedef vctBarycentricVector<double, 1> vctBarycentric1;
/*! Define a vector of barycentric coordinates of 2 doubles. */
typedef vctBarycentricVector<double, 2> vctBarycentric2;
/*! Define a vector of barycentric coordinates of 3 doubles. */
typedef vctBarycentricVector<double, 3> vctBarycentric3;
/*! Define a vector of barycentric coordinates of 4 doubles. */
typedef vctBarycentricVector<double, 4> vctBarycentric4;


/*! Define a quaternion of doubles. */
//@{
typedef vctQuaternion<double> vctDoubleQuat;
typedef vctQuaternion<double> vctQuat;
//@}
/*! Define a quaternion of floats. */
typedef vctQuaternion<float> vctFloatQuat;


/*! Define a rotation in dimension 3 using a unit quaternion of
  doubles. */
//@{
typedef vctQuaternionRotation3<double> vctDoubleQuatRot3;
typedef vctQuaternionRotation3<double> vctQuatRot3;
//@}
/*! Define a rotation in dimension 3 using a unit quaternion of
  floats. */
//@{
typedef vctQuaternionRotation3<float> vctFloatQuatRot3;
//@}


/*! Define a rotation in dimension 3 using an axis of doubles and an
  angle (of type NormType). */
//@{
typedef vctAxisAngleRotation3<double> vctDoubleAxAnRot3;
typedef vctAxisAngleRotation3<double> vctAxAnRot3;
//@}
/*! Define a rotation in dimension 3 using an axis of doubles and an
  angle (of type NormType). */ 
//@{
typedef vctAxisAngleRotation3<float> vctFloatAxAnRot3;
//@}


/*! Define a rotation in dimension 3 using a single vector a.k.a
  Rodriguez representation. */
//@{
typedef vctRodriguezRotation3<double> vctDoubleRodRot3;
typedef vctRodriguezRotation3<double> vctRodRot3;
//@}
/*! Define a rotation in dimension 3 using  a single vector a.k.a
  Rodriguez representation. */
//@{
typedef vctRodriguezRotation3<float> vctFloatRodRot3;
//@}


/*! Define a rotation in dimension 3 using a 3 by 3 matrix of
  doubles. */
//@{
typedef vctMatrixRotation3<double> vctDoubleMatRot3;
typedef vctMatrixRotation3<double> vctMatRot3;
typedef vctMatrixRotation3<double> vctDoubleRot3;
typedef vctMatrixRotation3<double> vctRot3;
//@}
/*! Define a rotation in dimension 3 using a 3 by 3 matrix of
  float. */
//@{
typedef vctMatrixRotation3<float> vctFloatMatRot3;
typedef vctMatrixRotation3<float> vctFloatRot3;
//@}

/*! Define a transformation in dimension 3 using a unit quaternion of
  doubles for its rotation. */
//@{
typedef vctFrameBase<vctQuaternionRotation3<double> > vctDoubleQuatFrm3;
typedef vctFrameBase<vctQuaternionRotation3<double> > vctQuatFrm3;
//@}

/*! Define a transformation in dimension 3 using a unit quaternion of
  floats for its rotation. */
//@{
typedef vctFrameBase<vctQuaternionRotation3<float> > vctFloatQuatFrm3;
//@}

/*! Define a transformation in dimension 3 using a 3 by 3 matrix of
  doubles for its rotation. */
//@{
typedef vctFrameBase<vctMatrixRotation3<double> > vctDoubleMatFrm3;
typedef vctFrameBase<vctMatrixRotation3<double> > vctMatFrm3;
typedef vctFrameBase<vctMatrixRotation3<double> > vctDoubleFrm3;
typedef vctFrameBase<vctMatrixRotation3<double> > vctFrm3;
//@}

/*! Define a transformation in dimension 3 using a 3 by 3 matrix of
  floats for its rotation. */
//@{
typedef vctFrameBase<vctMatrixRotation3<float> > vctFloatMatFrm3;
typedef vctFrameBase<vctMatrixRotation3<float> > vctFloatFrm3;
//@}


/*! Define a rotation in dimension 2 using an
  angle (of type AngleType). */
//@{
typedef vctAngleRotation2 vctDoubleAnRot2;
typedef vctAngleRotation2 vctAnRot2;
//@}
/*! Define a rotation in dimension 2 using an
  angle (of type AngleType). */ 
//@{
typedef vctAngleRotation2 vctFloatAnRot3;
//@}


/*! Define a rotation in dimension 2 using a 2 by 2 matrix of
  doubles. */
//@{
typedef vctMatrixRotation2<double> vctDoubleMatRot2;
typedef vctMatrixRotation2<double> vctMatRot2;
typedef vctMatrixRotation2<double> vctDoubleRot2;
typedef vctMatrixRotation2<double> vctRot2;
//@}
/*! Define a rotation in dimension 2 using a 2 by 2 matrix of
  float. */
//@{
typedef vctMatrixRotation2<float> vctFloatMatRot2;
typedef vctMatrixRotation2<float> vctFloatRot2;
//@}


#endif  // _vctTransformationTypes_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctTransformationTypes.h,v $
// Revision 1.2  2005/12/23 21:27:31  anton
// cisstVector: Minor updates for Doxygen 1.4.5.
//
// Revision 1.1  2005/10/07 09:26:28  anton
// cisstVector: Split vctTypes in multiples files (see #60 and #160).
//
//
// ****************************************************************************

