/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrSVDRSSolver.h,v 1.9 2005/11/29 03:06:56 anton Exp $
  
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
  \brief Declaration of nmrSVDRSSolver
 */

#ifndef _nmrSVDRSSolver_h
#define _nmrSVDRSSolver_h


#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>


/*!
  \ingroup cisstNumerical

  Algorithm SVDRS: Singular Value Decomposition also treating Right
  Side Vector.

  The original version of this code was developed by Charles L. Lawson
  and Richard J. Hanson at Jet Propulsion Laboratory 1974 SEP 25, and
  published in the book "Solving Least Squares Problems",
  Prentice-Hall, 1974.

  This subroutine computes the singular value decomposition of the
  given \f$ M \times N \f$ matrix A, and optionally applies the
  transformations from the left to the \f$ Nb \f$ column vectors of
  the \f$ Mb \times Nb \f$ matrix B.  Either \f$ M > N \f$ or
  \f$ M < N \f$ is permitted.

  The singular value decomposition of A is of the form:

  \f$ A  =  U  S  V^{T} \f$

  where U is \f$ M \times M \f$ orthogonal, S is \f$ M \times N \f$
  diagonal with the diagonal terms nonnegative and ordered from large
  to small, and V is \f$ N \times N \f$ orthogonal.  Note that these
  matrices also satisfy:

  \f$ S = (U^{T})  A  V \f$

  The singular values, i.e. the diagonal terms of the matrix S, are
  returned in the matrix GetS().  If \f$ M < N \f$, positions \f$ M+1
  \f$ through N of S() will be set to zero.

  The product matrix  \f$ G = U^{T} B \f$ replaces the given matrix B
  in the matrix B(,).

  If the user wishes to obtain a minimum length least squares
  solution of the linear system:

  \f$ \| Ax - B \| \f$

  the solution X can be constructed, following use of this subroutine,
  by computing the sum for i = 1, ..., R of the outer products

  (Col i of V) * (1/S(i)) * (Row i of G)
  
  Here R denotes the pseudorank of A which the user may choose in the
  range 0 through \f$ \mbox{min}(M, N) \f$ based on the sizes of the
  singular values.

  The data members of this class are:

  - M: Number of rows of matrix A, B, G.
  - N: Number of columns of matrix A. No of rows and columns of matrix
        V.
  - Lda: First dimensioning parameter for A. \f$ Lda = \mbox{max}(M,N) \f$
  - Ldb: First dimensioning parameter for B. \f$ Ldb \geq M \f$
  - Nb: Number of columns in the matrices B and G.
  - A: On input contains the \f$ M \times N \f$ matrix A.
       On output contains the \f$ N \times N \f$ matrix V.
  - B: If \f$ Nb > 0 \f$ this array must contain a
       \f$ M \times Nb \f$ matrix on input and will contain the
       \f$ M \times Nb \f$ product matrix, \f$ G = U^{T}  B \f$ on output.
  - S: On return will contain the singular values of A, with the ordering
       \f$ S(1) > S(2) > ... > S(N1) > 0 \f$.
       If \f$ M < N \f$ the singular values indexed from \f$ M+1 \f$
       through N will be zero.
  - Work   Work space of total size at least \f$ 2N \f$.

  \note This code gives special treatment to rows and columns that are
  entirely zero.  This causes certain zero singular values to appear
  as exact zeros rather than as about MACHEPS times the largest
  singular value It similarly cleans up the associated columns of U
  and V.

  \note The input matrices of this class must use a column major
  storage order.  To do so, use #VCT_COL_MAJOR whenever you declare
  a matrix.  They must also be compact (see
  vctDynamicMatrix::IsFortran()).

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/
class nmrSVDRSSolver {

protected:
    long int M;                   /* No. of rows of A, B, G */
    long int N;                   /* No. of cols of A, No of rows and cols of V */
    long int Lda;                 /* >= max(m, n) */
    long int Ldb;                 /* >= m */
    long int Nb;                  /* No. of cols of B and G */
    vctDynamicMatrix<double> S;   /* Singular values, Dim(S)>n */
    vctDynamicMatrix<double> Work;/* Work space */

public:

    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrSVDRSSolver(void):
        M(0),
        N(0)
    {
        Allocate(M, N);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on M, N and Nb.  It relies on the
      method Allocate().  The next call to the Solve() method will
      check that the parameters match the dimension.
      
      \param m Number of rows of A
      \param n Number of columns of A
      \param nb Number of columns of B
    */
    nmrSVDRSSolver(long int m, long int n, long int nb = 1) {
        Allocate(m, n, nb);
    }

    
    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on the actual input of the Solve()
      method.  It relies on the method Allocate().  The next call to
      the Solve() method will check that the parameters match the
      dimension. */
    nmrSVDRSSolver(vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &B) {
        Allocate(A, B);
    }


    /*! This method allocates the memory based on M, N and Nb.  The
      next call to the Solve() method will check that the parameters
      match the dimension.
      
      \param m Number of rows of A
      \param n Number of columns of A
      \param nb Number of columns of B
    */
    inline void Allocate(long int m, long int n, long int nb = 1) {
        M = m;
        N = n;
        Lda = std::max(M, N);
        Ldb = M;
        Nb = nb;
        S.SetSize(N, 1, VCT_COL_MAJOR);
        Work.SetSize(2 * N, 1, VCT_COL_MAJOR);
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    inline void Allocate(vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &B) {
        Allocate(A.rows(), A.cols(), B.cols());
    }


    /*! This subroutine computes the singular value decomposition of the
      given \f$ M \times N \f$ matrix A, and optionally applies the
      transformations from the left to the \f$ Nb \f$ column vectors of
      the \f$ Mb \times Nb \f$ matrix B.  Either \f$ M > N \f$ or \f$ M <
      N \f$ is permitted.
      
      The singular value decomposition of A is of the form:
      
      \f$ A  =  U  S  V^{T} \f$
      
      \note This method verifies that the input parameters are using a
      column major storage order and that they are compact.  Both
      conditions are tested using vctDynamicMatrix::IsFortran().
      If the parameters don't meet all the requirements, an exception
      is thrown (std::runtime_error).
    */
    inline void Solve(vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &B)
        throw (std::runtime_error)
    {
        /* check that the size matches with Allocate() */
        if ((M != (int) A.rows())
            || (N != (int) A.cols())
            || (Nb != (int) B.cols())) {
            cmnThrow(std::runtime_error("nmrSVDRSSolver Solve: Sizes used for Allocate were different"));
        }
        
        /* check other dimensions */
        if (M != (int) B.rows()) {
            cmnThrow(std::runtime_error("nmrSVDRSSolver Solve: Sizes of parameters are incompatible"));
        }
        
        /* check that the matrices are Fortran like */
        if (! (A.IsFortran()
               && B.IsFortran())) {
            cmnThrow(std::runtime_error("nmrSVDRSSolver Solve: All parameters must be Fortran compatible"));
        }

        svdrs_(A.Pointer(), &Lda, &M, &N, B.Pointer(),
               &Ldb, &Nb, S.Pointer(), Work.Pointer());
    }
    

    inline const vctDynamicMatrix<double> & GetS(void) const {
        return S;
    }
};


#endif // _nmrSVDRSSolver_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrSVDRSSolver.h,v $
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
// Revision 1.3  2004/11/08 18:06:54  anton
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
