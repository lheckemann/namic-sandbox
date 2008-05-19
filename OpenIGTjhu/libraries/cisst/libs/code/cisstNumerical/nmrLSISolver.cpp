/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLSISolver.cpp,v 1.6 2005/09/26 15:41:46 anton Exp $
  
  Author(s):  Ankur Kapoor
  Created on:  2004-10-30

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
  \brief Declaration of nmrNNLSSolver
 */


#include <cisstNumerical/nmrLSISolver.h>


void nmrLSISolver::Solve(vctDynamicMatrix<double> &C, vctDynamicMatrix<double> &d,
                         vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &b)
    throw (std::runtime_error)
{
    /* check that the size matches with Allocate() */
    if ((Ma != (int) C.rows())
        || (Na != (int) C.cols())
        || (Mg != (int) A.rows())) {
        cmnThrow(std::runtime_error("nmrLSISolver Solve: Sizes used for Allocate were different"));
    }
    
    if ((C.cols() != A.cols())
        || (C.rows() != d.rows())
        || (A.rows() != b.rows())) {
        cmnThrow(std::runtime_error("nmrLSISolver Solve: Sizes of parameters are incompatible"));
    }
    
    /* check that the matrices are Fortran like */
    if (! (C.IsFortran()
           && d.IsFortran()
           && A.IsFortran()
           && b.IsFortran())) {
        cmnThrow(std::runtime_error("nmrLSISolver Solve: All parameters must be Fortran compatible"));
    }
    
    long int i, j, k;
    SVDRS.Solve(C, d);
    /* C is now replaced by V and d by U^{T}d */
    
    //Change this to ref
    vctDynamicMatrix<double> singularValues = SVDRS.GetS();
    long int nS = singularValues.rows();
    
    /***
        Compute V*S^{-1}U^{T}b
    ***/
    
    double VSiUtbval = 0;
    for (i = 0; i < Na; i++) {
        VSiUtbval = 0;
        for (j = 0; j < nS; j++) 
            VSiUtbval += C(i,j)*d(j,0)/singularValues(j, 0);
        VSiUtb(i, 0) = VSiUtbval;
    }
    
    /***
        Compute \tilde G = G*V*S^{-1} and \tilde h = h - \tilde G \tilde f_1
    ***/
    double htval;
    double Gtval;
    for (i = 0; i < Mg; i++) {
        htval = 0;
        for (j = 0; j < nS && j < Mg; j++) { 
            Gtval = 0;
            for (k = 0; k < nS; k++)
                Gtval += A(i, k)*C(k, j);
            GTilde(i, j) = Gtval/singularValues(j, 0);
            htval += A(i, j)*VSiUtb(j, 0);
        };
        HTilde(i, 0) = b(i, 0) - htval;
    }
    
    /***
        Compute min || z || s.t. \tilde G z \geq \tilde h
        where z = SV^{T}x - \tilde f
    **/
    LDP.Solve(GTilde, HTilde);
    Z = LDP.GetX();
    
    /***
        Z = S*V^{T}*X -  V*S^{-1}*U^{T}*b
        X = V*S^{-1}*Z + V*S^{-1}*U^{T}*b
    ***/
    double Xval = 0;
    for (i = 0; i < Na; i++) {
        Xval = 0;
        for (j = 0; j < nS; j++) {
            Xval += C(i, j)*Z(j, 0)/singularValues(j, 0);
        };
        X(i, 0) = Xval + VSiUtb(i, 0);
    }
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrLSISolver.cpp,v $
// Revision 1.6  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.5  2005/09/24 00:04:23  anton
// cisstNumerical: Use cmnThrow for all exceptions, removed all #include of
// cisstVector.h (use more specialized vctXyz.h).
//
// Revision 1.4  2005/07/27 21:04:27  anton
// cisstNumerical: Wrappers for netlib numerical methods now use exceptions
// instead of *assert* to check the parameters (size, storage order).
//
// Revision 1.3  2005/06/03 18:20:58  anton
// cisstNumerical: Added license.
//
// Revision 1.2  2004/11/03 22:21:25  anton
// cisstNumerical: Update the cnetlib front-end to separate allocation and
// solve. Uses VCT_COL_MAJOR and tests if the matrices are Fortran like.
//
// Revision 1.1  2004/11/01 17:44:48  anton
// cisstNumerical: Added some Solvers and the files to use our own cnetlib
// code added to the cisst CVS repository (module "cnetlib").
//
//
// ****************************************************************************
