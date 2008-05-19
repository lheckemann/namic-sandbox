/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeVectorTypes.h,v 1.2 2005/12/23 21:27:31 anton Exp $
  
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
  \brief Typedef for fixed size vectors
*/


#ifndef _vctFixedSizeVectorTypes_h
#define _vctFixedSizeVectorTypes_h

#include <cisstVector/vctFixedSizeVector.h>

/*! Define a fixed size vector of size 1 containing doubles. */
//@{ 
typedef vctFixedSizeVector<double, 1> vctDouble1;
typedef vctFixedSizeVector<double, 1> vct1;
//@}
/*! Define a fixed size vector of size 2 containing doubles. */
//@{ 
typedef vctFixedSizeVector<double, 2> vctDouble2;
typedef vctFixedSizeVector<double, 2> vct2;
//@}
/*! Define a fixed size vector of size 3 containing doubles. */
//@{ 
typedef vctFixedSizeVector<double, 3> vctDouble3;
typedef vctFixedSizeVector<double, 3> vct3;
//@}
/*! Define a fixed size vector of size 4 containing doubles. */
//@{ 
typedef vctFixedSizeVector<double, 4> vctDouble4;
typedef vctFixedSizeVector<double, 4> vct4;
//@}
/*! Define a fixed size vector of size 5 containing doubles. */
//@{ 
typedef vctFixedSizeVector<double, 5> vctDouble5;
typedef vctFixedSizeVector<double, 5> vct5;
//@}
/*! Define a fixed size vector of size 6 containing doubles. */
//@{
 typedef vctFixedSizeVector<double, 6> vctDouble6;
typedef vctFixedSizeVector<double, 6> vct6;
//@}


/*! Define a fixed size vector of size 1 containing floats. */
typedef vctFixedSizeVector<float, 1> vctFloat1;
/*! Define a fixed size vector of size 2 containing floats. */
typedef vctFixedSizeVector<float, 2> vctFloat2;
/*! Define a fixed size vector of size 3 containing floats. */
typedef vctFixedSizeVector<float, 3> vctFloat3;
/*! Define a fixed size vector of size 4 containing floats. */
typedef vctFixedSizeVector<float, 4> vctFloat4;
/*! Define a fixed size vector of size 5 containing floats. */
typedef vctFixedSizeVector<float, 5> vctFloat5;
/*! Define a fixed size vector of size 6 containing floats. */
typedef vctFixedSizeVector<float, 6> vctFloat6;


/*! Define a fixed size vector of size 1 containing integers. */
typedef vctFixedSizeVector<int, 1> vctInt1;
/*! Define a fixed size vector of size 2 containing integers. */
typedef vctFixedSizeVector<int, 2> vctInt2;
/*! Define a fixed size vector of size 3 containing integers. */
typedef vctFixedSizeVector<int, 3> vctInt3;
/*! Define a fixed size vector of size 4 containing integers. */
typedef vctFixedSizeVector<int, 4> vctInt4;
/*! Define a fixed size vector of size 5 containing integers. */
typedef vctFixedSizeVector<int, 5> vctInt5;
/*! Define a fixed size vector of size 6 containing integers. */
typedef vctFixedSizeVector<int, 6> vctInt6;


/*! Define a fixed size vector of size 1 containing chars. */
typedef vctFixedSizeVector<char, 1> vctChar1;
/*! Define a fixed size vector of size 2 containing chars. */
typedef vctFixedSizeVector<char, 2> vctChar2;
/*! Define a fixed size vector of size 3 containing chars. */
typedef vctFixedSizeVector<char, 3> vctChar3;
/*! Define a fixed size vector of size 4 containing chars. */
typedef vctFixedSizeVector<char, 4> vctChar4;
/*! Define a fixed size vector of size 5 containing chars. */
typedef vctFixedSizeVector<char, 5> vctChar5;
/*! Define a fixed size vector of size 6 containing chars. */
typedef vctFixedSizeVector<char, 6> vctChar6;

#endif  // _vctFixedSizeVectorTypes_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedSizeVectorTypes.h,v $
// Revision 1.2  2005/12/23 21:27:31  anton
// cisstVector: Minor updates for Doxygen 1.4.5.
//
// Revision 1.1  2005/10/07 09:26:28  anton
// cisstVector: Split vctTypes in multiples files (see #60 and #160).
//
//
// ****************************************************************************

