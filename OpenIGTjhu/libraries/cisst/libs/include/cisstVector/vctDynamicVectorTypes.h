/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicVectorTypes.h,v 1.3 2005/12/23 21:27:31 anton Exp $
  
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
  \brief Typedef for dynamic vectors
*/


#ifndef _vctDynamicVectorTypes_h
#define _vctDynamicVectorTypes_h


#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicConstVectorRef.h>
#include <cisstVector/vctDynamicVectorRef.h>

//@{
/*! Define some types of dynamic vectors */
typedef vctDynamicVector<double> vctDoubleVec;
typedef vctDynamicVector<double> vctVec;
typedef vctDynamicVector<float> vctFloatVec;
typedef vctDynamicVector<int> vctIntVec;
//@}

#endif  // _vctDynamicVectorTypes_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctDynamicVectorTypes.h,v $
// Revision 1.3  2005/12/23 21:27:31  anton
// cisstVector: Minor updates for Doxygen 1.4.5.
//
// Revision 1.2  2005/12/01 21:22:09  ofri
// vctDynamicVectorTypes.h : Added include to the Dynamic[Const]VectorRef classes.
//
// Revision 1.1  2005/10/07 09:26:28  anton
// cisstVector: Split vctTypes in multiples files (see #60 and #160).
//
//
// ****************************************************************************

