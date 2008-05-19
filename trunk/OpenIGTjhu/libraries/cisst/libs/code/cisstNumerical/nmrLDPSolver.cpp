/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLDPSolver.cpp,v 1.6 2005/09/26 15:41:46 anton Exp $
  
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


#include <cisstNumerical/nmrLDPSolver.h>

void nmrLDPSolver::Solve(vctDynamicMatrix<double> &G, vctDynamicMatrix<double> &h)
    throw (std::runtime_error) 
{
    /* check that the size matches with Allocate() */
    if ((M != (int) G.rows())
        || (N != (int) G.cols())) {
        cmnThrow(std::runtime_error("nmrLDPSolver Solve: Sizes used for Allocate were different"));
    }

    if (G.rows() != h.rows()) {
        cmnThrow(std::runtime_error("nmrLDPSolver Solve: Sizes of parameters are incompatible"));
    }
    
    /* check that the matrices are Fortran like */
    if (! (G.IsFortran()
           && h.IsFortran())) {
        cmnThrow(std::runtime_error("nmrLDPSolver Solve: All parameters must be Fortran compatible"));
    }
    
    int i, j;
    for (j = 0; j < M; j++) { 
        for (i = 0; i < N; i++) {
            E(i,j) = G(j,i);
        }
        E(N,j) = h(j,0);
    }
    CopyE = E;
    
    long int mda = N+1;
    long int m_nnls = N+1;
    long int n_nnls = M;
    long int mode;
    double rnorm;
    
    F.SetAll(0.0);
    F(N, 0) = 1.0;
    nnls_(E.Pointer(), &mda, &m_nnls, &n_nnls, F.Pointer(), U.Pointer(), &rnorm,
          W.Pointer(), Zz.Pointer(), Index.Pointer(), &mode);
    if (mode != 1) {
        return;
    }
    for (i = 0; i <= N;i++) {
        R(i,0) = (i==N)?-1.0:0.0;
        for (j = 0; j < M; j++)
            R(i,0) += CopyE(i,j)*U(j,0);          
    }
    double one_plus_fac = 1 + R(N,0);
    double fac =  R(N,0);
    
    if (diff_(&one_plus_fac, &fac) <= 0) return;
    for (int k = 0; k < N; k++) { 
        X(k,0) = -R(k,0) / R(N,0);
    }
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrLDPSolver.cpp,v $
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
