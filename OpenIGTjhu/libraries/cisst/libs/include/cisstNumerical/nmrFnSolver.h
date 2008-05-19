/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrFnSolver.h,v 1.4 2005/09/26 15:41:46 anton Exp $
  
  Author(s):  Ankur Kapoor
  Created on: 2004-10-26

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
  \brief Declaration of nmrFnSolver
*/


#ifndef _nmrFnSolver_h
#define _nmrFnSolver_h

#include <cisstVector/vctDynamicMatrix.h>

#include <cnetlib.h>

/*!
  \ingroup cisstNumerical

  Algorithm Fn: Solve nonlinear set of equations using Powell's method
  The purpose of this class is to find a zero of a system of
  N nonlinear functions in N variables by a modification
  of the powell hybrid method. the user must provide a
  subroutine which calculates the functions and the jacobian.

  The data members of this class are:

  - N: The number of variables and also the equations.  N >= 0.
  - Tolerance: A nonnegative input variable. Termination occurs
           when the algorithm estimates either that the relative
           error in the sum of squares is at most Tolerance or that
           the relative error between X and the solution is at
           most Tolerance. (Default set to 1.0e-6).
  - Work: A working vector of length N*(N+13)/2
  - LWork: Length of working array.
  - Info: = info is an integer output variable. if the user has
            terminated execution, info is set to the (negative)
            value of Flag. see description of FunctionPointer.
      otherwise, info is set as follows.

          info = 0   improper input parameters.
 
          info = 1   algorithm estimates that the relative error
                     between x and the solution is at most tol.
 
          info = 2   number of calls to user function has
                     reached 200*(n+1).
 
          info = 3   tol is too small. no further improvement in
                     the approximate solution x is possible.
 
          info = 4   iteration is not making good progress.

  The input/output from this class is:
  - X: On entry, the inital estimate of solution vector.
       On exit, final estimate of solution vector.
  - callBack: Is object of type nmrCallBackFnSolver used to supply the user method.
    The user method which belongs to a user defined class 'Cfoo' has the 
  following definition
      int Cfoo::Mbar (vctDynamicVectorRef<double> &X,
          vctDynamicVectorRef<double> &F, long int &Flag);
  The solver calls this method when needed to obtain values for F for
  a given variable values X.
  the value of Flag should not be changed by Mbar unless
  the user wants to terminate execution of Solver.
  in this case set Flag to a negative integer.

  The following constructor can be used to contruct the nmrCallBackFnSolver object
  required to be passed to the Solve() method. This object needs to be created
  only once per given set of nonlinear functions to be minized.
    nmrCallBackFnSolver<nmrUNIQUE_IDENTIFIER_LINE, Cfoo> callBackObject(this, &Cfoo::Mbar);
  

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/
class nmrFnSolver {
  // we have this class so that we reserve memory only one
  // would help if svd of a same size matrix (or a matrix)
  // that doesnt change much is desired.

protected:
  long int N;
  double Tolerance;
  long int Info;
  long int Lwork;
  vctDynamicVector<double> Work;

public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrFnSolver(void)
    {
        Allocate(0);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on N.  It relies on the
      method Allocate().  The next call to the Solve() method will
      check that the parameters match the dimension.

      \param n Number of variables
      This order will be used for the output as well.
    */
  nmrFnSolver(long int n)
    {
        Allocate(n);
    }

    
    /*! Constructor with memory allocation.  This constructor
       allocates the memory based on the actual input of the Solve()
       method.  It relies on the method Allocate().  The next call to
       the Solve() method will check that the parameters match the
       dimension and storage order. */
    //@{
  nmrFnSolver(vctDynamicVector<double> &X) {
        Allocate(X);
    }
    //@}


    /*! This method allocates the memory based on N.  The next
      call to the Solve() method will check that the parameters match
      the dimension.
      
      \param n Number of variables
    */
  inline void Allocate(long int n) {
        N = n;
        Lwork = N*(3*3*3*N+13)/2;
        Tolerance = 1e-6;
        Work.SetSize(Lwork);
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    //@{
    inline void Allocate(vctDynamicVector<double> &X) {
        Allocate(X.size());
    }
    //@}


    /*! This computes the solves nonlinear equations problem invloving
        N functions in N variables.
  On input, X contains the starting point,
  On output, X contains the final estimate
  F contains the final values
    */

    //@{
    template <int __instanceLine, class __elementType>
    inline void Solve(nmrCallBackFunctionF<__instanceLine, __elementType> &callBack, vctDynamicVector<double> &X,
                      vctDynamicVector<double> &F, double tolerance) throw (std::runtime_error) {
        if ((N != (int) X.size()) || (N != (int) F.size())) {
            cmnThrow(std::runtime_error("nmrFnSolver Solve: Size used for Allocate was different"));
        }
        Tolerance = tolerance;
        /* call the MINPACK C function */
        hybrd1_((U_fp)callBack.FunctionFhybrd, &N,
                X.Pointer(), F.Pointer(), 
                &Tolerance, &Info,
                Work.Pointer(), &Lwork);
    }
    //@}
    
};

#endif // _nmrFnSolver_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrFnSolver.h,v $
//  Revision 1.4  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.3  2005/09/24 00:04:23  anton
//  cisstNumerical: Use cmnThrow for all exceptions, removed all #include of
//  cisstVector.h (use more specialized vctXyz.h).
//
//  Revision 1.2  2005/08/13 23:34:00  kapoor
//  Fixes for windows. Added NLP code.
//
//  Revision 1.1  2005/07/30 09:48:02  kapoor
//  Numerical: Added wrappers for MINPACK for Powells method and
//  Levenberg-Marquardt method (using analytic and approximate - BFGS jacobians)
//
//
//
// ****************************************************************************




