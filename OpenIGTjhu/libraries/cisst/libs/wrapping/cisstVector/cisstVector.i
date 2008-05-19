/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstVector.i,v 1.5 2006/04/17 18:05:08 anton Exp $

  Author(s):  Anton Deguet
  Created on: 2004-03-29

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


%module cisstVectorPython

%include <std_string.i>
%include <std_except.i>

%import "cisstCommon/cisstCommon.i"

%header %{
    // Put header files here
    #include "cisstVector/cisstVector.i.h"
%}

%ignore *::operator[]; // We define __setitem__ and __getitem__
%ignore *::operator=; // Just to avoid warnings

// We don't use iterators in Python, so far.
%ignore *::begin;
%ignore *::end;
%ignore *::rbegin;
%ignore *::rend;
%ignore *::at;

// Generate parameter documentation for IRE
%feature("autodoc", "1");

// General approach for Python print
%rename(__str__) ToString;
%ignore *::ToStream;

// The traits are used everywhere
%include "cisstVector/vctContainerTraits.h"

// Define some basic flags
%include "cisstVector/vctForwardDeclarations.h" 

// Include per type of container
%include "cisstVector/vctDynamicVector.i"
%include "cisstVector/vctFixedSizeVector.i"

%include "cisstVector/vctDynamicMatrix.i"
%include "cisstVector/vctFixedSizeMatrix.i"

%include "cisstVector/vctQuaternion.i"
%include "cisstVector/vctQuaternionRotation3.i"
%include "cisstVector/vctMatrixRotation3.i"
%include "cisstVector/vctAxisAngleRotation3.i"

// %include "cisstVector/vctFrame.i"


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cisstVector.i,v $
//  Revision 1.5  2006/04/17 18:05:08  anton
//  cisstVector.i: Added support for vctForwardDeclaration to define VCT_COL_MAJOR,
//  VCT_ROW_MAJOR, etc ...
//
//  Revision 1.4  2006/03/14 22:31:07  anton
//  cisstVector wrappers: Corrected bug with Assign() method and va_list using
//  some Python code.  Added tests for vctDynamicVector.
//
//  Revision 1.3  2006/02/11 18:06:20  anton
//  wrapping: Added autodoc for IRE in context documentation.
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
//  Revision 1.9  2005/08/19 05:30:44  anton
//  Wrapping: Main interface files have only the required #include, i.e. don't
//  include the SWIG generated cisstXyz.h.  Also added license.
//
//  Revision 1.8  2005/08/15 06:00:00  anton
//  cisstVector wrapping: Reorganized files per library.
//
//  Revision 1.7  2005/07/19 15:37:25  anton
//  cisstVector.i: Added support for fixed size matrices.
//
//  Revision 1.6  2005/06/15 15:06:20  anton
//  Python wrapping: Split the cisstVector wrapping code in smaller files to
//  increase readability.
//
// ****************************************************************************
