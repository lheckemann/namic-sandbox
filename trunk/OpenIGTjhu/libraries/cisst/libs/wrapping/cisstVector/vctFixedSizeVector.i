/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeVector.i,v 1.5 2006/06/22 03:10:11 kapoor Exp $

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


// Typemaps(out) for references
%typemap(out) vctDouble2 & {
    vctDynamicVectorRef<double>* resultRef = new vctDynamicVectorRef<double>(*result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVectorRef<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleVecRef");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultRef), typeInfo, SWIG_POINTER_DISOWN);
    }
}

%typemap(out) vctInt2 & {
    vctDynamicVectorRef<int>* resultRef = new vctDynamicVectorRef<int>(*result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVectorRef<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntVecRef");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultRef), typeInfo, SWIG_POINTER_DISOWN);
    }
}


// Typemaps(out) for copies
%typemap(out) vctDouble2 {
    vctDynamicVector<double>* resultCopy = new vctDynamicVector<double>(result.size());
    resultCopy->Assign(result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVector<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleVec");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultCopy), typeInfo, SWIG_POINTER_DISOWN);
    }
}

%typemap(out) vctInt2 {
    vctDynamicVector<int>* resultCopy = new vctDynamicVector<int>(result.size());
    resultCopy->Assign(result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVector<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntVec");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultCopy), typeInfo, SWIG_POINTER_DISOWN);
    }
}





// Typemaps(in) for references
%typemap(in) vctDouble2 & {
    vctDynamicVector<double>* resultPointer = (vctDynamicVector<double>*) NULL;
    // convert the input result to a dynamic vector
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVector<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleVec");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic vector
    if ($1->size() != resultPointer->size()) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    // check the stride of the dynamic vector
    if ($1->stride() != 1) {
        PyErr_SetString(PyExc_RuntimeError, "Stride is not 1");
        SWIG_fail;
    }
    $1 = reinterpret_cast<$1_ltype>(resultPointer->Pointer());
}


%typemap(in) vctInt2 & {
    vctDynamicVector<int>* resultPointer = (vctDynamicVector<int>*) NULL;
    // convert the input result to a dynamic vector
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVector<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntVec");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic vector
    if ($1->size() != resultPointer->size()) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    // check the stride of the dynamic vector
    if ($1->stride() != 1) {
        PyErr_SetString(PyExc_RuntimeError, "Stride is not 1");
        SWIG_fail;
    }
    $1 = reinterpret_cast<$1_ltype>(resultPointer->Pointer());
}




// Typemaps(in) for copies
%typemap(in) vctDouble2 {
    vctDynamicVector<double>* resultPointer = (vctDynamicVector<double>*) NULL;
    // convert the input result to a dynamic vector
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVector<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleVec");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic vector
    if ($1.size() != resultPointer->size()) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    // copy to temporary vector
    $1.Assign(resultPointer->Pointer());
}

%typemap(in) vctInt2 {
    vctDynamicVector<int>* resultPointer = (vctDynamicVector<int>*) NULL;
    // convert the input result to a dynamic vector
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVector<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntVec");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic vector
    if ($1.size() != resultPointer->size()) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    // copy to temporary vector
    $1.Assign(resultPointer->Pointer());
}



// Copy of typemaps
%apply vctDouble2 & {vctDouble3 &, vctDouble4 &, vctDouble5 &, vctDouble6 &, vctDouble7 &, vctDouble8 &, vctDouble9 &, vctDouble10 &};
%apply vctDouble2 {vctDouble3, vctDouble4, vctDouble5, vctDouble6, vctDouble7, vctDouble8, vctDouble9, vctDouble10};

%apply vctInt2 & {vctInt3 &, vctInt4 &, vctInt5 &, vctInt6 &, vctInt7 &, vctInt8 &, vctInt9 &, vctInt10 &};
%apply vctInt2 {vctInt3, vctInt4, vctInt5, vctInt6, vctInt7, vctInt8, vctInt9, vctInt10};




// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctFixedSizeVector.i,v $
//  Revision 1.5  2006/06/22 03:10:11  kapoor
//  cisstVector: Wrappers for fixed size vector of size 7-10.
//
//  Revision 1.4  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.3  2005/09/01 06:24:26  anton
//  cisst wrapping:
//  *: Reorganized files so that each library has its own CMakeLists.txt
//  *: Added preliminary wrapping for some transformations.  This compiles, loads
//  but is not tested.
//
//  Revision 1.2  2005/08/19 05:32:54  anton
//  SWIG interface files:  Added the license.
//
//  Revision 1.1  2005/08/15 06:00:00  anton
//  cisstVector wrapping: Reorganized files per library.
//
//  Revision 1.5  2005/07/22 20:07:42  anton
//  Fixed size containers: Added typemaps for reference inputs.
//
//  Revision 1.4  2005/07/01 03:30:37  anton
//  vctFixedSizeVector SWIG interface: Use SWIG function to query exact type
//  name instead of guessing the mangled names.
//
//  Revision 1.3  2005/06/23 21:40:18  anton
//  vctFixedSizeVector.i: Working version of in/out typemaps
//
//  Revision 1.2  2005/06/23 15:14:17  anton
//  vctFixedSizeVector.i:  Work in progress.  Now handles references and copies
//  form C++ to Python.
//
//  Revision 1.1  2005/06/15 15:06:20  anton
//  Python wrapping: Split the cisstVector wrapping code in smaller files to
//  increase readability.
//
// ****************************************************************************
