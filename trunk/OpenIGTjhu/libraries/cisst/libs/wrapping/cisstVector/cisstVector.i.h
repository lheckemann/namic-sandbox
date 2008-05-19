/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstVector.i.h,v 1.4 2006/04/17 18:08:49 anton Exp $

  Author(s):  Anton Deguet
  Created on: 2005-08-21

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


/* This file is to be used only for the generation of SWIG wrappers.
   It includes all the regular header files from the libraries as well
   as some header files created only for the wrapping process
   (e.g. vctDynamicMatrixRotation3.h).

   For any wrapper using %import "cisstVector.i", the file
   cisstVector.i.h should be included in the %header %{ ... %} section
   of the interface file. */


#ifndef _cisstVector_i_h
#define _cisstVector_i_h


/* Put header files here */
#include "cisstVector/vctFixedSizeVector.h"
#include "cisstVector/vctFixedSizeMatrix.h"

#include "cisstVector/vctDynamicVector.h"
#include "cisstVector/vctDynamicMatrix.h"

#include "cisstVector/vctRandom.h"

#include "cisstVector/vctQuaternionBase.h"
#include "cisstVector/vctDynamicQuaternion.h"
#include "cisstVector/vctDynamicQuaternionRotation3.h"

#include "cisstVector/vctDynamicMatrixRotation3.h"
#include "cisstVector/vctAxisAngleRotation3.h"

#include "cisstVector/vctFrameBase.h"

#include "cisstVector/vctFixedSizeVectorTypes.h"
#include "cisstVector/vctFixedSizeMatrixTypes.h"

#endif // _cisstVector_i_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cisstVector.i.h,v $
//  Revision 1.4  2006/04/17 18:08:49  anton
//  cisstVector.i.h: Preliminary support for vctFrameBase.
//
//  Revision 1.3  2005/10/07 09:35:28  anton
//  cisstVector wrappers: Start to add some flesh to vctMatRot3.
//
//  Revision 1.2  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.1  2005/09/01 06:24:26  anton
//  cisst wrapping:
//  *: Reorganized files so that each library has its own CMakeLists.txt
//  *: Added preliminary wrapping for some transformations.  This compiles, loads
//  but is not tested.
//
//
// ****************************************************************************
