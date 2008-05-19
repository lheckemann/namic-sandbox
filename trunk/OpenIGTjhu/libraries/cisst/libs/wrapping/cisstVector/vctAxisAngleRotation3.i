/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctAxisAngleRotation3.i,v 1.3 2006/04/17 18:07:15 anton Exp $

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


#include "cisstVector/vctAxisAngleRotation3.h"


// instantiate and extend the end-user class
%include "cisstVector/vctAxisAngleRotation3.h"
%template(vctAxAnRot3) vctAxisAngleRotation3<double>;

%extend vctAxisAngleRotation3<double> {

    vctAxisAngleRotation3<double>(const vctDynamicConstVectorBase<vctDynamicVectorOwner<double> , double> & axis,
                                  double angle) throw(std::runtime_error) {
        vctAxisAngleRotation3<double> * result =
            new vctAxisAngleRotation3<double>(axis, angle);
        return result;
    }

    vctAxisAngleRotation3<double>(const vctDynamicConstVectorBase<vctDynamicVectorRefOwner<double> , double> & axis,
                                  double angle) throw(std::runtime_error) {
        vctAxisAngleRotation3<double> * result =
            new vctAxisAngleRotation3<double>(axis, angle);
        return result;
    }

    inline void From(const vctQuaternionRotation3Base<vctDynamicVector<double> > & quaternionRotation) {
        self->From(quaternionRotation);
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
    typedef vctAxisAngleRotation3<double> vctAxAnRot3;
%}

typedef vctAxisAngleRotation3<double> vctAxAnRot3;

%types(vctAxAnRot3 *);


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctAxisAngleRotation3.i,v $
//  Revision 1.3  2006/04/17 18:07:15  anton
//  vctAxisAngleRotation3.i: Added 2 ctors.  Still need to figure out a more
//  flexible implementation to support both vector and vector ref (SWIG macro?)
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
