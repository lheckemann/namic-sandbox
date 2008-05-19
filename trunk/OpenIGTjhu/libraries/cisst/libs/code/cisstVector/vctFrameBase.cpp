/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFrameBase.cpp,v 1.3 2005/09/26 15:41:46 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2004-02-12

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


#include <cisstVector/vctFrameBase.h>
#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctQuaternionRotation3.h>

template<>
const vctFrameBase<vctMatrixRotation3<double> > &
vctFrameBase<vctMatrixRotation3<double> >::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}

template<>
const vctFrameBase<vctMatrixRotation3<float> > &
vctFrameBase<vctMatrixRotation3<float> >::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}

template<>
const vctFrameBase<vctQuaternionRotation3<double> > &
vctFrameBase<vctQuaternionRotation3<double> >::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}

template<>
const vctFrameBase<vctQuaternionRotation3<float> > &
vctFrameBase<vctQuaternionRotation3<float> >::Identity()
{
  static const TranslationType zeroTranslation(0);
  static const RotationType identityRotation;
  static const ThisType result(identityRotation, zeroTranslation);
  return result;
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFrameBase.cpp,v $
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.1  2004/10/21 20:14:47  anton
// cisstVector: Added missing file in check-in [829]
//
// Revision 1.6  2004/10/14 19:34:52  anton
// cisstVector: For the frames, constructors are now based on (rotation,
// translation) instead of (translation, rotation) as requested in #72.
//
// Revision 1.5  2004/04/02 16:22:20  anton
// Removed explicit instantiation of the classes.  The class is templated and
// the method Identity is the only one that needs to be instantiated.  The
// specialization for floats and doubles does it.
//
// Revision 1.4  2004/03/11 22:32:59  ofri
// Moved instantiation of rotation and frame CLASSES from vctTypes.cpp to
// the class cpp file
//
// Revision 1.3  2004/02/27 02:44:57  ofri
// The Identity rotation and frame objects are now declared through a static
// method instead of a static object instance. This fixes a bug related to the
// order of static object instantiations.
//
// Revision 1.2  2004/02/18 22:30:42  anton
// Added DIMENSION for all transformation to simplify the vctFrame template
//
// Revision 1.1  2004/02/18 16:00:38  anton
// Creation
//
//
// ****************************************************************************
