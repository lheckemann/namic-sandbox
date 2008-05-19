/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeMatrix.i,v 1.4 2005/09/26 15:41:47 anton Exp $

  Author(s):  Anton Deguet
  Created on: 2004-07-18

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
%typemap(out) vctDouble2x2 & {
    vctDynamicMatrixRef<double>* resultRef = new vctDynamicMatrixRef<double>(*result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrixRef<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleMatRef");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultRef), typeInfo, SWIG_POINTER_DISOWN);
    }
}

%typemap(out) vctInt2x2 & {
    vctDynamicMatrixRef<int>* resultRef = new vctDynamicMatrixRef<int>(*result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrixRef<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntMatRef");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultRef), typeInfo, SWIG_POINTER_DISOWN);
    }
}


// Typemaps(out) for copies
%typemap(out) vctDouble2x2 {
    vctDynamicMatrix<double>* resultCopy = new vctDynamicMatrix<double>(result.rows(), result.cols());
    resultCopy->Assign(result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrix<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleMat");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultCopy), typeInfo, SWIG_POINTER_DISOWN);
    }
}

%typemap(out) vctInt2x2 {
    vctDynamicMatrix<int>* resultCopy = new vctDynamicMatrix<int>(result.rows(), result.cols());
    resultCopy->Assign(result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrix<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntMat");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultCopy), typeInfo, SWIG_POINTER_DISOWN);
    }
}




// Typemaps(in) for references
%typemap(in) vctDouble2x2 & {
    vctDynamicMatrix<double>* resultPointer = (vctDynamicMatrix<double>*) NULL;
    // convert the input result to a dynamic matrix
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrix<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleMat");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic matrix
    if (($1->rows() != resultPointer->rows()) || ($1->cols() != resultPointer->cols())) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    $1 = reinterpret_cast<$1_ltype>(resultPointer->Pointer());
}


%typemap(in) vctInt2x2 & {
    vctDynamicMatrix<int>* resultPointer = (vctDynamicMatrix<int>*) NULL;
    // convert the input result to a dynamic matrix
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrix<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntMat");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic matrix
    if (($1->rows() != resultPointer->rows()) || ($1->cols() != resultPointer->cols())) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    $1 = reinterpret_cast<$1_ltype>(resultPointer->Pointer());
}


// Typemaps(in) for copies
%typemap(in) vctDouble2x2 {
    vctDynamicMatrix<double>* resultPointer = (vctDynamicMatrix<double>*) NULL;
    // convert the input result to a dynamic matrix
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrix<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleMat");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic matrix
    if (($1.rows() != resultPointer->rows()) || ($1.cols() != resultPointer->cols())) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    // copy to temporary matrix
    $1.Assign(resultPointer->Pointer());
}

%typemap(in) vctInt2x2 {
    vctDynamicMatrix<int>* resultPointer = (vctDynamicMatrix<int>*) NULL;
    // convert the input result to a dynamic matrix
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrix<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntMat");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic matrix
    if (($1.rows() != resultPointer->rows()) || ($1.cols() != resultPointer->cols())) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    // copy to temporary matrix
    $1.Assign(resultPointer->Pointer());
}



// Copy of typemaps
%apply vctDouble2x2 & {vctDouble3x3 &, vctDouble4x4 &, vctDouble5x5 &, vctDouble6x6 &};
%apply vctDouble2x2 {vctDouble3x3, vctDouble4x4, vctDouble5x5, vctDouble6x6};

%apply vctInt2x2 & {vctInt3x3 &, vctInt4x4 &, vctInt5x5 &, vctInt6x6 &};
%apply vctInt2x2 {vctInt3x3, vctInt4x4, vctInt5x5, vctInt6x6};




// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctFixedSizeMatrix.i,v $
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
//  Revision 1.2  2005/07/22 20:07:42  anton
//  Fixed size containers: Added typemaps for reference inputs.
//
//  Revision 1.1  2005/07/19 15:36:53  anton
//  vctFixedSizeMatrix.i: Initial typemaps to support fixed size matrices with
//  SWIG.
//
// ****************************************************************************
