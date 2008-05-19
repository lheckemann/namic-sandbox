/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctMatrixRotation3.cpp,v 1.7 2005/09/26 15:41:46 anton Exp $

  Author(s):  Anton Deguet
  Created on:  2004-01-12

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


#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctQuaternionRotation3.h>

// Force the instantiation of the templated classes
template class vctMatrixRotation3<double>;
template class vctMatrixRotation3<float>;


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctMatrixRotation3.cpp,v $
// Revision 1.7  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.6  2005/09/01 06:18:08  anton
// cisstVector transformations: Added a level of abstraction for all classes
// derived from fixed size containers (vctQuaternion, vctQuaternionRotation3,
// vctMatrixRotation3, vctRodriguezRotation3).  These classes are now derived
// from vctXyzBase<_containerType>.  This will ease the SWIG wrapping.
//
// Revision 1.5  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.4  2005/02/04 16:56:54  anton
// cisstVector: Added classes vctAxisAngleRotation3 and vctRodriguezRotation3
// as planed in ticket #109.  Also updated vctMatrixRotation3 to use these new
// classes.  Other updates include vctRandom(rotation), new vctTypes and
// forward declarations.
//
// Revision 1.3  2004/08/13 19:27:55  anton
// cisstVector:  For all the code related to rotations, added some assert to
// check that the input is valid (normalized quaternion, normalized axis,
// normalized matrix) for all the From method.  Added a
// vctQuaternionRotation.FromRaw(matrix) since this method is used to normalize
// the rotation matrices.  It's input doesn't have to be normalized.  As a side
// effect these additions, found a couple of IsNormalized methods which were
// not const.
//
// Revision 1.2  2004/02/27 02:44:57  ofri
// The Identity rotation and frame objects are now declared through a static
// method instead of a static object instance. This fixes a bug related to the
// order of static object instantiations.
//
// Revision 1.1  2004/02/06 15:40:20  anton
// Renamed vctRotationQuaternion3 to vctQuaternionRotation3
// Renamed vctRotationMatrix3 to vctMatrixRotation3
// Added Inverse and Normalized methods with consistant naming and signatures
// Added constructors from/to quaternion, matrix, axis/angle
//
//
// Revision 1.1  2004/01/15 15:21:10  anton
// Creation
//
//
// ****************************************************************************
