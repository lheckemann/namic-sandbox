/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctAngleRotation2.cpp,v 1.4 2005/09/26 15:41:46 anton Exp $
  
  Author(s):  Anton Deguet
  Created on:  2005-02-23

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


#include <cisstVector/vctAngleRotation2.h>
#include <cisstVector/vctMatrixRotation2.h>


const vctAngleRotation2 &
vctAngleRotation2::Identity()
{
    static const vctAngleRotation2 result(0.0);
    return result;
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctAngleRotation2.cpp,v $
// Revision 1.4  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.2  2005/02/28 20:20:08  anton
// vctAngleRotation2: Moved definition of templated From(vctMatrixRotation2) to
// header file to avoid compilation issues with VCPP 7.0.  Also uses atan2.
//
// Revision 1.1  2005/02/24 01:51:50  anton
// cisstVector: Added classes vctAngleRotation2 and vctMatrixRotation2.  This
// code has not been tested.   The normalization and conversion methods have
// to be reviewed.
//
//
// ****************************************************************************
