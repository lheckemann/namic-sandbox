/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctTypes.cpp,v 1.10 2005/09/26 15:41:46 anton Exp $

  Author(s):  Anton Deguet
  Created on:  2003-09-16

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


#include <cisstVector/vctTypes.h>

// include header files of reference and dynamic types here, to ensure that
// they get preprocessed successfully
#include <cisstVector/vctFixedSizeConstVectorRef.h>
#include <cisstVector/vctFixedSizeVectorRef.h>
#include <cisstVector/vctFixedSizeConstMatrixRef.h>
#include <cisstVector/vctFixedSizeMatrixRef.h>
#include <cisstVector/vctDynamicConstVectorRef.h>
#include <cisstVector/vctDynamicVectorRef.h>
#include <cisstVector/vctDynamicConstMatrixRef.h>
#include <cisstVector/vctDynamicMatrixRef.h>

#if (CISST_COMPILER == CISST_DOTNET7)

template class vctFixedSizeVector<double, 1>;
template class vctFixedSizeVector<double, 2>;
template class vctFixedSizeVector<double, 3>;
template class vctFixedSizeVector<double, 4>;
template class vctFixedSizeVector<double, 5>;
template class vctFixedSizeVector<double, 6>;

template class vctFixedSizeVector<float, 1>;
template class vctFixedSizeVector<float, 2>;
template class vctFixedSizeVector<float, 3>;
template class vctFixedSizeVector<float, 4>;
template class vctFixedSizeVector<float, 5>;
template class vctFixedSizeVector<float, 6>;

template class vctFixedSizeVector<int, 1>;
template class vctFixedSizeVector<int, 2>;
template class vctFixedSizeVector<int, 3>;
template class vctFixedSizeVector<int, 4>;
template class vctFixedSizeVector<int, 5>;
template class vctFixedSizeVector<int, 6>;

template class vctFixedSizeVector<char, 1>;
template class vctFixedSizeVector<char, 2>;
template class vctFixedSizeVector<char, 3>;
template class vctFixedSizeVector<char, 4>;
template class vctFixedSizeVector<char, 5>;
template class vctFixedSizeVector<char, 6>;

template class vctBarycentricVector<double, 2>;
template class vctBarycentricVector<double, 3>;
template class vctBarycentricVector<double, 4>;


template class vctFixedSizeMatrix<double, 2, 2>;
template class vctFixedSizeMatrix<double, 2, 3>;
template class vctFixedSizeMatrix<double, 2, 4>;
template class vctFixedSizeMatrix<double, 3, 2>;
template class vctFixedSizeMatrix<double, 3, 3>;
template class vctFixedSizeMatrix<double, 3, 4>;
template class vctFixedSizeMatrix<double, 4, 2>;
template class vctFixedSizeMatrix<double, 4, 3>;
template class vctFixedSizeMatrix<double, 4, 4>;

template class vctFixedSizeMatrix<float, 2, 2>;
template class vctFixedSizeMatrix<float, 3, 3>;
template class vctFixedSizeMatrix<float, 4, 4>;

template class vctFixedSizeMatrix<int, 2, 2>;
template class vctFixedSizeMatrix<int, 3, 3>;
template class vctFixedSizeMatrix<int, 4, 4>;

template class vctFixedSizeMatrix<char, 2, 2>;
template class vctFixedSizeMatrix<char, 3, 3>;
template class vctFixedSizeMatrix<char, 4, 4>;


template class vctQuaternion<double>;
template class vctQuaternion<float>;


template class vctDynamicVector<float>;
template class vctDynamicVector<double>;
template class vctDynamicVector<int>;
template class vctDynamicVector<char>;


#endif


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctTypes.cpp,v $
// Revision 1.10  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.9  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.8  2004/10/14 18:50:12  ofri
// vctTypes.cpp: including header files of reference vector and matrix type to
// catch trivial syntax errors etc. during library build.
//
// Revision 1.7  2004/09/30 15:49:51  anton
// cisstVector: solution for ticket #65 regarding a compilation error with
// .Net 2003.  The issue remains mysterious but but a solution is to declare
// the problematic methods in the class declaration and implement them right
// after (in the same file).  The methods in question are the Elementwise
// comparaison with a scalar (it works if comparing with a vector/matrix).
//
// Revision 1.6  2004/07/13 20:39:01  ofri
// vctTypes: Added type definitions for non-square fixed-size matrices.
//
// Revision 1.5  2004/07/13 16:20:55  ofri
// Updating vctTypes: inclusion of cisstDynamicVector to provide code compilation
// and potentially DLL instantiations.
//
// Revision 1.4  2004/03/11 22:32:59  ofri
// Moved instantiation of rotation and frame CLASSES from vctTypes.cpp to
// the class cpp file
//
// Revision 1.3  2004/02/18 22:30:42  anton
// Added DIMENSION for all transformation to simplify the vctFrame template
//
// Revision 1.2  2004/02/18 18:01:08  anton
// Removed some classes instantiated more than once
//
// Revision 1.1  2004/02/18 15:58:13  anton
// Instantiation of classes moved from vctFixedSizeVector.cpp to vctTypes.cpp
//
//
// Log of vctFixedSizeVector.cpp
//
// Revision 1.2  2003/12/09 21:58:08  ofri
// Added instantiation of vctBarycentricVector
//
// Revision 1.1  2003/09/17 14:15:24  anton
// creation
//
//
// ****************************************************************************

