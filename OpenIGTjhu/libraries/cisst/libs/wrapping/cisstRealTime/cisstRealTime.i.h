/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstRealTime.i.h,v 1.2 2006/05/05 21:50:47 kapoor Exp $

  Author(s):  Anton Deguet
  Created on:   2006-05-02

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

   For any wrapper using %import "cisstRealTime.i", the file
   cisstRealTime.i.h should be included in the %header %{ ... %} section
   of the interface file. */


#ifndef _cisstRealTime_i_h
#define _cisstRealTime_i_h


/* Put header files here */
#include "cisstCommon/cisstCommon.i.h"
#include "cisstVector/cisstVector.i.h"
#include "cisstDeviceInterface/cisstDeviceInterface.i.h"

#include "cisstRealTime/rtsTask.h"
#include "cisstRealTime/rtsCommand1Base.h"


#endif // _cisstRealTime_i_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cisstRealTime.i.h,v $
//  Revision 1.2  2006/05/05 21:50:47  kapoor
//  cisstDeviceInterface wrappers: Removed @ chars.
//
//  Revision 1.1  2006/05/05 19:42:21  kapoor
//  cisstRealTime: Added preliminary Python wrappers.
//
//
//
// ****************************************************************************
