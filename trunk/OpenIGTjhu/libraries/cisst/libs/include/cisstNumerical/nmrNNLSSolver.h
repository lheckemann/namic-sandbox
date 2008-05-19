/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrNNLSSolver.h,v 1.9 2005/11/29 03:06:56 anton Exp $
  
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

#ifndef _nmrNNLSSolver_h
#define _nmrNNLSSolver_h

#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>

/*!
  \ingroup cisstNumerical

  Algorithm NNLS: Non Negative Least Squares

  The original version of this code was developed by Charles L. Lawson
  and Richard J. Hanson at Jet Propulsion Laboratory 1973 JUN 15, and
  published in the book "Solves Least Squares Problems",
  Prentice-Hall, 1974.

  Given a \f$ M \times N\f$ matrix A, and a \f$ M \times 1 \f$ vector B,
  compute a \f$ N \times 1 \f$ vector X, that solves the least squares
  problem:

  \f$ \mbox{min} \; 1 / 2 \| AX - B \| \; \mbox{subject to} \; X \geq 0\f$
     
  The data members of this class are:
  - M, N: Dimension of input matrix.
  - MDA: Is the first dimension parameter for the matrix A.
  - A: On entry contains the \f$ M \times N \f$ matrix A.
       On exit the matrix product of \f$ QA \f$, where Q is a \f$ M \times N \f$ orthogonal matrix generated implicitly by this subroutine.
  - B: On entry contains the \f$ M \times 1 \f$ matrix B>
       On exit the matrix product if \f$ QB \f$
  - X: On exit X contains the solution vector>
  - rnorm: Contains the Euclidean norm of the residual vector.
  - W: A \f$ N \times 1 \f$ working array. On exit W constrains the dual solution vector>
  - ZZ: A \f$ M \times 1 \f$ working array.
  - Index: A working array of integers of size atleast \f$ N \times 1 \f$

  \note The input matrices of this class must use a column major
  storage order.  To do so, use #VCT_COL_MAJOR whenever you declare a
  matrix.  They must also be compact (see
  vctDynamicMatrix::IsFortran()).

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/
class nmrNNLSSolver {

protected:
    long int M;
    long int N;
    long int Mda;
    long int Mode;
    double RNorm;
    vctDynamicMatrix<double> X;
    vctDynamicMatrix<double> W;
    vctDynamicMatrix<double> Zz;
    vctDynamicMatrix<long int> Index;

public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrNNLSSolver(void):
        M(0),
        N(0)
    {
        Allocate(M, N);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on M and N.  It relies on the
      method Allocate().  The next call to the Solve() method will
      check that the parameters match the dimension.

      \param m Number of rows of C
      \param n Number of columns of C
    */
    nmrNNLSSolver(long int m, long int n) {
        Allocate(m, n);
    }


    /*! Constructor with memory allocation.  This constructor
       allocates the memory based on the actual input of the Solve()
       method.  It relies on the method Allocate().  The next call to
       the Solve() method will check that the parameters match the
       dimension. */
    nmrNNLSSolver(vctDynamicMatrix<double> &C, vctDynamicMatrix<double> &d) {
        Allocate(C, d);
    }
    

    /*! This method allocates the memory based on M and N.  The next
      call to the Solve() method will check that the parameters match
      the dimension.
      
      \param m Number of rows of C
      \param n Number of columns of C
    */
    inline void Allocate(long int m, long int n) {
        M = m;
        N = n;
        Mda = M;
        X.SetSize(N, 1, VCT_COL_MAJOR);
        W.SetSize(N,1, VCT_COL_MAJOR);
        Zz.SetSize(M,1, VCT_COL_MAJOR);
        Index.SetSize(N, 1, VCT_COL_MAJOR);
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    inline void Allocate(vctDynamicMatrix<double> &C, vctDynamicMatrix<double> &d) {
        Allocate(C.rows(), C.cols());
    }
    

    /*! Given a \f$ M \times N\f$ matrix A, and a \f$ M \times 1 \f$ vector B,
      compute a \f$ N \times 1 \f$ vector X, that solves the least squares
      problem:

      \f$ \mbox{min} \; 1 / 2 \| AX - B \| \; \mbox{subject to} \; X \geq 0\f$

      \note This method verifies that the input parameters are using a
      column major storage order and that they are compact.  Both
      conditions are tested using vctDynamicMatrix::IsFortran().
      If the parameters don't meet all the requirements, an exception
      is thrown (std::runtime_error).
    */
    inline void Solve(vctDynamicMatrix<double> &C, vctDynamicMatrix<double> d) 
        throw (std::runtime_error)
    {
        /* check that the size matches with Allocate() */
        if ((M != (int) C.rows())
            || (N != (int) C.cols())) {
            cmnThrow(std::runtime_error("nmrNNLSSolver Solve: Sizes used for Allocate were different"));
        }
        
        /* check other dimensions */
        if (C.rows() != d.rows()) {
            cmnThrow(std::runtime_error("nmrNNLSSolver Solve: Sizes of parameters are incompatible"));
        }
        
        /* check that the matrices are Fortran like */
        if (! (C.IsFortran()
               && d.IsFortran())) {
            cmnThrow(std::runtime_error("nmrNNLSSolver Solve: All parameters must be Fortran compatible"));
        }
        
        nnls_(C.Pointer(), &Mda, &M, &N, d.Pointer(), X.Pointer(), &RNorm,
              W.Pointer(), Zz.Pointer(), Index.Pointer(), &Mode);
        //error handling??
    }

    
    /*! Get X.  This method must be used after Solve(). */
    inline const vctDynamicMatrix<double> &GetX(void) const {
        return X;
    }
    

    /* Get W.  This method must be used after Solve(). */
    inline const vctDynamicMatrix<double> &GetDual(void) const {
        return W;
    }
    

    /* Get RNorm.  This method must be used after Solve(). */
    inline double GetRNorm(void) const {
        return RNorm;
    }
};


#endif // _nmrNNLSSolver_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrNNLSSolver.h,v $
// Revision 1.9  2005/11/29 03:06:56  anton
// cisstNumerical:  Systematic use of nmrNetlib.h and, modified all "min" and "max"
// to use STL ones.
//
// Revision 1.8  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.7  2005/09/24 00:04:23  anton
// cisstNumerical: Use cmnThrow for all exceptions, removed all #include of
// cisstVector.h (use more specialized vctXyz.h).
//
// Revision 1.6  2005/07/27 21:04:27  anton
// cisstNumerical: Wrappers for netlib numerical methods now use exceptions
// instead of *assert* to check the parameters (size, storage order).
//
// Revision 1.5  2005/06/03 18:20:58  anton
// cisstNumerical: Added license.
//
// Revision 1.4  2005/01/10 19:19:55  anton
// cisstNumerical: Updates to allow compilation as a Dll.
//
// Revision 1.3  2004/11/08 18:06:53  anton
// cisstNumerical: Major Doxygen update based on Ankur's notes and Ofri's
// comments in the code.
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
