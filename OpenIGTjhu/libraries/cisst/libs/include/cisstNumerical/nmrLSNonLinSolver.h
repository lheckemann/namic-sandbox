/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLSNonLinSolver.h,v 1.6 2005/09/26 15:41:46 anton Exp $
  
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
  \brief Declaration of nmrLSNonLinSolver
*/


#ifndef _nmrLSNonLinSolver_h
#define _nmrLSNonLinSolver_h


#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicMatrix.h>

#include <cisstNumerical/nmrCallBack.h>

#include <cnetlib.h>

/*!
  \ingroup cisstNumerical

  Algorithm LSNonLin: Non Linear Least Squares by Levenberg Marquardt method
  The purpose of this is to minimize the sum of the squares of
       M nonlinear functions in N variables by a modification of the
       levenberg-marquardt algorithm. The user must provide a
       subroutine which calculates the functions. The jacobian is
       then calculated by a forward-difference approximation.

  Rather than compute the value f(x) (the sum of squares), LSNonLin requires 
  the user-defined function to compute the vector-valued function. 
       \f$ \textbf{F}(x) = [f_1(x), f_2(x), f_3(x)]^{t} \f$
  Then, in vector terms, you can restate this optimization problem as 
       \f$ \mbox{min} \frac{1}{2} \| \textbf{F}(x) \|_2^2 = \frac{1}{2} sum_i f_i(x)^2 \f$
  where \f$x\f$ is a vector and \f$\textbf{F}(x)\f$ is a function that 
  returns a vector value.

  The data members of this class are:

  - M: The number of nonlinear functions  M >= 0.
  - N: The number of variables.  N >= 0.
  - Tolerance: A nonnegative input variable. Termination occurs
           when the algorithm estimates either that the relative
           error in the sum of squares is at most Tolerance or that
           the relative error between X and the solution is at
           most Tolerance. (Default set to 1.0e-6).
  - IWork: An integer working vector of length N.
  - Work: A working vector of length M*N+5*N+M.
  - LWork: Length of working array.
  - Info: = info is an integer output variable. if the user has
            terminated execution, info is set to the (negative)
            value of Flag. see description of callBack.
      otherwise, info is set as follows.
   
            info = 0  improper input parameters.
   
            info = 1  algorithm estimates that the relative error
                      in the sum of squares is at most tol.
   
            info = 2  algorithm estimates that the relative error
                      between x and the solution is at most tol.
   
            info = 3  conditions for info = 1 and info = 2 both hold.
   
            info = 4  fvec is orthogonal to the columns of the
                      jacobian to machine precision.
   
            info = 5  number of calls to fcn has reached or
                      exceeded 200*(n+1).
   
            info = 6  tol is too small. no further reduction in
                      the sum of squares is possible.
   
            info = 7  tol is too small. no further improvement in
                      the approximate solution x is possible

  The input/output from this class is:
  - X: On entry, the inital estimate of solution vector.
       On exit, final estimate of solution vector.
  - RNorm: If used, the value of the squared 2-norm of the residual at 
       final value of x
  - callBack: Is object of type nmrCallBackLSNonLinSolver used to supply the user method.
    The user method which belongs to a user defined class 'Cfoo' has the 
  following definition
      int Cfoo::Mbar (vctDynamicVectorRef<double> &X,
          vctDynamicVectorRef<double> &F, long int &Flag);
  The solver calls this method when needed to obtain values for F for
  a given variable values X.
  the value of Flag should not be changed by Mbar unless
  the user wants to terminate execution of Solver.
  in this case set Flag to a negative integer.

  The following constructor can be used to contruct the nmrCallBackLSNonLinSolver object
  required to be passed to the Solve() method. This object needs to be created
  only once per given set of nonlinear functions to be minized.
    nmrCallBackLSNonLinSolver<nmrUNIQUE_IDENTIFIER_LINE, Cfoo> callBackObject(this, &Cfoo::Mbar);
  

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/

class nmrLSNonLinSolver {
  // we have this class so that we reserve memory only one
  // would help if svd of a same size matrix (or a matrix)
  // that doesnt change much is desired.

protected:
  long int M;
  long int N;
  double Tolerance;
  long int Info;
  long int Lwork;
  vctDynamicVector<long int> IWork;
  vctDynamicVector<double> Work;

public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrLSNonLinSolver(void)
    {
        Allocate(0, 0);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on M and N.  It relies on the
      method Allocate().  The next call to the Solve() method will
      check that the parameters match the dimension.

      \param m Number of nonlinear functions.
      \param n Number of variables
      This order will be used for the output as well.
    */
  nmrLSNonLinSolver(long int m, long int n)
    {
        Allocate(m, n);
    }

    
    /*! Constructor with memory allocation.  This constructor
       allocates the memory based on the actual input of the Solve()
       method.  It relies on the method Allocate().  The next call to
       the Solve() method will check that the parameters match the
       dimension and storage order. */
    //@{
  nmrLSNonLinSolver(vctDynamicVector<double> &X, vctDynamicVector<double> &F) {
        Allocate(X, F);
    }
    //@}


    /*! This method allocates the memory based on M and N.  The next
      call to the Solve() method will check that the parameters match
      the dimension.
      
      \param m Number of nonlinear functions
      \param n Number of variables
    */
  inline void Allocate(long int m, long int n) {
        M = m;
        N = n;
        Lwork = M * N + 5 * N + M;
        Tolerance = 1e-6;
        Work.SetSize(Lwork);
        IWork.SetSize(N);
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    //@{
    inline void Allocate(vctDynamicVector<double> &X, vctDynamicVector<double> &F) {
        Allocate(X.size(), F.size());
    }
    //@}


    /*! This computes the solves nonlinear least squares problem invloving
        M functions in N variables.
  On input, X contains the starting point,
  On output, X contains the final estimate, F contrains the final value
    */

    //@{
    template <int __instanceLine, class __elementType>
    inline void Solve(nmrCallBackFunctionF<__instanceLine, __elementType> &callBack, vctDynamicVector<double> &X,
                      vctDynamicVector<double> &F, double tolerance) throw (std::runtime_error) {
        if ((N != (int) X.size()) || (M != (int) F.size())) {
            cmnThrow(std::runtime_error("nmrLSNonLinSolver Solve: Size used for Allocate was different"));
        }
        Tolerance = tolerance;
        /* call the MINPACK C function */
        lmdif1_((U_fp)callBack.FunctionFlmdif, &M, &N,
                X.Pointer(), F.Pointer(), 
                &Tolerance, &Info,
                IWork.Pointer(), Work.Pointer(), &Lwork);
    }
    //@}
    
};


#endif // _nmrLSNonLinSolver_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrLSNonLinSolver.h,v $
//  Revision 1.6  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.5  2005/09/24 00:04:23  anton
//  cisstNumerical: Use cmnThrow for all exceptions, removed all #include of
//  cisstVector.h (use more specialized vctXyz.h).
//
//  Revision 1.4  2005/09/01 06:40:28  anton
//  nmrLSNonLinSolver: Removed #include <cisstVector.h> and use only required
//  header files from cisstVector.
//
//  Revision 1.3  2005/08/13 23:34:00  kapoor
//  Fixes for windows. Added NLP code.
//
//  Revision 1.2  2005/08/08 17:13:44  anton
//  Minor Doxygen typos.
//
//  Revision 1.1  2005/07/30 09:48:02  kapoor
//  Numerical: Added wrappers for MINPACK for Powells method and
//  Levenberg-Marquardt method (using analytic and approximate - BFGS jacobians)
//
//
//
// ****************************************************************************




