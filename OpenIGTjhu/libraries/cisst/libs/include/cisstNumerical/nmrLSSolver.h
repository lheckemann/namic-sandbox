/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrLSSolver.h,v 1.6 2005/11/29 03:06:56 anton Exp $
  
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
  \brief Declaration of nmrLSSolver
*/


#ifndef _nmrLSSolver_h
#define _nmrLSSolver_h


#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>

/*!
  \ingroup cisstNumerical

  Algorithm LS: Least Squares by QR or LQ decomposition
   This solves overdetermined or underdetermined real linear systems
   involving an M-by-N matrix A, or its transpose, using a QR or LQ
   factorization of A.  It is assumed that A has full rank.
 
   The following options are provided:
 
   1. If m >= n:  find the least squares solution of
      an overdetermined system, i.e., solve the least squares problem
                  \f$ \mbox {minimize} \| B - A*X \| \f$
 
   2. If m < n:  find the minimum norm solution of
      an underdetermined system \f$ A * X = B \f$

  The data members of this class are:

  - M: The number of rows of the input matrix A.  \f$ M >= 0 \f$.
  - N: The number of columns of the input matrix A.  \f$ N >= 0 \f$.
  - NRHS: The number of right hand sides, i.e., the number of
             columns of the matrices B and X. \f$ NRHS >=0 \f$.
  - Lda: The leading dimension of the array A.  \f$ Lda \geq \mbox{max}(1,M) \f$.
  - Ldb: The leading dimension of the array B.  \f$ Ldb \geq \mbox{max}(1,M,N). \f$.
  - Lwork: The dimension of the matrix Work. 
            \f$ Lwork \geq \mbox{max}( 1, MN + max( MN, NRHS ) )\f$.
            For optimal performance,
            \f$ Lwork \geq \mbox{max}( 1, MN + max( MN, NRHS )*NB )\f$.
            where \f$ MN = \mbox{min}(M,N) \f$ and \f$ NB \f$ is the optimum block size.
  - Info: = 0: successful exit
          < 0: argument had an illegal value
  - Work: Working matrix of dimenstion \f$ Lwork \times 1 \f$.

  The input/output from this class is:
  - A: On entry, the \f$ M \times N \f$ matrix A.
       On exit,
            if M >= N, A is overwritten by details of its QR factorization
            if M <  N, A is overwritten by details of its LQ factorization
  - B: On entry, the matrix B of right hand side vectors, stored
             columnwise; B is M-by-NRHS 
       On exit, B is overwritten by the solution vectors, stored
             columnwise:
             if m >= n, rows 1 to n of B contain the least
             squares solution vectors; the residual sum of squares for the
             solution in each column is given by the sum of squares of
             elements N+1 to M in that column;
             if m < n, rows 1 to N of B contain the
             minimum norm solution vectors;

  \note The input matrix must be compact (see
  vctDynamicMatrix::IsCompact() or vctFixedSizeMatrix::IsCompact()).

  \note This code relies on the ERC CISST cnetlib library.  Since
  cnetlib is optional, make sure that CISST_HAS_CNETLIB has been
  turned ON during the configuration with CMake.
*/
class nmrLSSolver {
    // we have this class so that we reserve memory only one would
    // help if svd of a same size matrix (or a matrix) that doesnt
    // change much is desired.

protected:
    long int M;
    long int N;
    long int NRHS;
    long int Lda;
    long int Ldb;
    long int Lwork;
    char Trans;
    vctDynamicMatrix<double> Work;
    long int Info;
    bool StorageOrder;

public:
    /*! Default constructor.  This constructor doesn't allocate any
      memory.  If you use this constructor, you will need to use one
      of the Allocate() methods before you can use the Solve
      method.  */
    nmrLSSolver(void):
        M(0),
        N(0),
        NRHS(0),
        StorageOrder(VCT_COL_MAJOR)
    {
        Allocate(M, N, NRHS, StorageOrder);
    }


    /*! Constructor with memory allocation.  This constructor
      allocates the memory based on M and N.  It relies on the
      method Allocate().  The next call to the Solve() method will
      check that the parameters match the dimension.

      \param m Number of rows of A
      \param n Number of columns of A
      \param nrhs Number of columns of B
      \param storageOrder Storage order used for the input matrix.
      This order will be used for the output as well.
    */
    nmrLSSolver(long int m, long int n, long int nrhs, bool storageOrder) {
        Allocate(m, n, nrhs, storageOrder);
    }

    
    /*! Constructor with memory allocation.  This constructor
       allocates the memory based on the actual input of the Solve()
       method.  It relies on the method Allocate().  The next call to
       the Solve() method will check that the parameters match the
       dimension and storage order. */
    //@{
    nmrLSSolver(vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &B) {
        Allocate(A, B);
    }
    //@}


    /*! This method allocates the memory based on M and N.  The next
      call to the Solve() method will check that the parameters match
      the dimension.
      
      \param m Number of rows of A
      \param n Number of columns of A
      \param nrhs Number of columns of B
      \param storageOrder Storage order used for all the matrices
    */
    inline void Allocate(long int m, long int n, long int nrhs, bool storageOrder) {
        const long int one = 1;
        StorageOrder = storageOrder;
        M = m;
        N = n;
        NRHS = nrhs;
        Lda = std::max(one, M);
        Ldb = std::max(one, std::max(M, N));
        long int MN = std::min(M, N);
        Lwork = std::max (one, MN + std::max (MN, NRHS));
        Trans = 'N';
        Work.SetSize(Lwork, 1, StorageOrder);
    }


    /*! Allocate memory to solve this problem.  This method provides a
      convenient way to extract the required sizes from the input
      containers.  The next call to the Solve() method will check that
      the parameters match the dimension. */
    //@{
    inline void Allocate(vctDynamicMatrix<double> &A, vctDynamicMatrix<double> &B) {
        Allocate(A.rows(), A.cols(), B.cols(), A.IsRowMajor());
    }
    //@}


    /*! This computes the solves overdetermined or underdetermined real linear systems
        involving an M-by-N matrix A, using the right hand side M-by-NRHS matrix B.
    */

    //@{
    template <class _matrixOwnerType>
    inline void Solve(vctDynamicMatrixBase<_matrixOwnerType, double> &A, vctDynamicMatrixBase<_matrixOwnerType, double> &B) throw (std::runtime_error) {
        /* check that the size and storage order matches with Allocate() */
        if (A.IsRowMajor() != StorageOrder) {
            cmnThrow(std::runtime_error("nmrLSSolver Solve: Storage order used for Allocate was different"));
        }
        if (B.IsRowMajor() != StorageOrder) {
            cmnThrow(std::runtime_error("nmrLSSolver Solve: Storage order used for Allocate was different"));
        }

        /* check sizes based on storage order, there is a more compact
           expression for this test but I find this easier to read and
           debug (Anton) */
        if (A.IsColMajor()) {
            if ((M != (int) A.rows()) || (N != (int) A.cols())) {
                cmnThrow(std::runtime_error("nmrLSSolver Solve: Size used for Allocate was different"));
            }
        } 
        if (B.IsColMajor()) {
            if ((M != (int) B.rows()) || (NRHS != (int) B.cols())) {
                cmnThrow(std::runtime_error("nmrLSSolver Solve: Size used for Allocate was different"));
            }
        } 
        
        /* check that the matrices are Fortran like */
        if (! A.IsCompact()) {
            cmnThrow(std::runtime_error("nmrLSSolver Solve: Requires a compact matrix"));
        }
        if (! B.IsCompact()) {
            cmnThrow(std::runtime_error("nmrLSSolver Solve: Requires a compact matrix"));
        }

        /* call the LAPACK C function */
        dgels_(&Trans, &M, &N, &NRHS,
               A.Pointer(), &Lda, 
               B.Pointer(), &Ldb,
               Work.Pointer(), &Lwork, &Info);
    }
    //@}
  
};


#endif // _nmrLSSolver_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrLSSolver.h,v $
//  Revision 1.6  2005/11/29 03:06:56  anton
//  cisstNumerical:  Systematic use of nmrNetlib.h and, modified all "min" and "max"
//  to use STL ones.
//
//  Revision 1.5  2005/10/06 16:56:37  anton
//  Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
//  Revision 1.4  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.3  2005/09/24 00:04:23  anton
//  cisstNumerical: Use cmnThrow for all exceptions, removed all #include of
//  cisstVector.h (use more specialized vctXyz.h).
//
//  Revision 1.2  2005/07/29 19:04:27  anton
//  cisstNumerical: Updated doc for Doxygen and replaced tabs by spaces.
//
//  Revision 1.1  2005/07/29 00:09:53  kapoor
//  cisstNumerical: Added wrappers for Least-Squares (LS) and pseudo-inverse.
//
//
// ****************************************************************************




