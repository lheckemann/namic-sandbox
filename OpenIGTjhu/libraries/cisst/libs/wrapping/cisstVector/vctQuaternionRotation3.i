/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctQuaternionRotation3.i,v 1.3 2005/10/08 20:13:30 anton Exp $

  Author(s):  Anton Deguet
  Created on: 2005-08-19

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


#include "cisstVector/vctQuaternionRotation3Base.h"


// macro to extend using different types of vectors
%define VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_1(owner, subOwner)
%extend vctQuaternionRotation3Base<owner > {

    inline vctDouble3
        ApplyTo(const vctDynamicConstVectorBase<subOwner, value_type>& input)
        throw (std::runtime_error) 
        {
            return self->ApplyTo(input);
        }

    inline vctDouble3
        ApplyInverseTo(const vctDynamicConstVectorBase<subOwner, value_type>& input)
        throw (std::runtime_error) 
        {
            return self->ApplyInverseTo(input);
        }

    inline vctDouble3
        __mul__(const vctDynamicConstVectorBase<subOwner, value_type>& input)
        throw (std::runtime_error) 
        {
            return self->ApplyTo(input);
        }
}
%enddef



// macro to extend using different types of vectors
%define VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_2(owner, subOwner1, subOwner2)    
%extend vctQuaternionRotation3Base<owner > {

    inline void ApplyTo(const vctDynamicConstVectorBase<subOwner1, value_type>& input,
                        vctDynamicVectorBase<subOwner2, value_type> & output)
        throw (std::runtime_error) 
        {
            self->ApplyTo(input, output);
        }

    inline void ApplyInverseTo(const vctDynamicConstVectorBase<subOwner1, value_type>& input,
                               vctDynamicVectorBase<subOwner2, value_type> & output)
        throw (std::runtime_error) 
        {
            self->ApplyInverseTo(input, output);
        }
}
%enddef


// instantiate the templated base class
%include "cisstVector/vctQuaternionRotation3Base.h"
%template(vctQuatRot3Base) vctQuaternionRotation3Base<vctDynamicVector<double> >;

VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_1(vctDynamicVector<double>,
                                             vctDynamicVectorOwner<double>);
VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_1(vctDynamicVector<double>,
                                             vctDynamicVectorOwner<double>);

VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_2(vctDynamicVector<double>,
                                             vctDynamicVectorOwner<double>, vctDynamicVectorOwner<double>);
VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_2(vctDynamicVector<double>,
                                             vctDynamicVectorOwner<double>, vctDynamicVectorRefOwner<double>);
VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_2(vctDynamicVector<double>,
                                             vctDynamicVectorRefOwner<double>, vctDynamicVectorOwner<double>);
VCT_QUATERNION_ROTATION_BASE_EXTEND_VECTOR_2(vctDynamicVector<double>,
                                             vctDynamicVectorRefOwner<double>, vctDynamicVectorRefOwner<double>);


// instantiate and extend the end-user class
%include "cisstVector/vctDynamicQuaternionRotation3.h"
%template(vctQuatRot3) vctDynamicQuaternionRotation3<double>;

%extend vctDynamicQuaternionRotation3<double> {

    inline void Random(void) {
        vctRandom(*self);
    }

    inline void From(const vctAxisAngleRotation3<double> & axisAngleRotation) {
        self->From(axisAngleRotation);
    }
    
    inline void From(const vctRodriguezRotation3Base<vctDynamicVector<double> > & rodriguezRotation) {
        self->From(rodriguezRotation);
    }

    inline void From(const vctMatrixRotation3Base<vctDynamicMatrix<double> > & matrixRotation) {
        self->From(matrixRotation);
    }
}


// type declarations for SWIG
%{
    typedef vctQuaternionRotation3Base<vctDynamicVector<double> > vctQuatRot3Base;
    typedef vctDynamicQuaternionRotation3<double> vctQuatRot3;
%}

typedef vctQuaternionRotation3Base<vctDynamicVector<double> > vctQuatRot3Base;
typedef vctDynamicQuaternionRotation3<double> vctQuatRot3;

%types(vctQuatRot3Base *);
%types(vctQuatRot3 *);


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctQuaternionRotation3.i,v $
//  Revision 1.3  2005/10/08 20:13:30  anton
//  cisstVector wrapping: Updated vctQuatRot3 to match features of vctMatRot3.
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
