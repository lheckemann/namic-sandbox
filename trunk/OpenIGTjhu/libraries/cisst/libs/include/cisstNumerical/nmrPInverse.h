/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrPInverse.h,v 1.8 2006/01/28 05:59:25 anton Exp $

  Author(s): Ankur Kapoor
  Created on: 2005-10-18

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
Software be made publicly available under terms no more restrictive
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
  \brief Declaration of nmrPInverse
*/


#ifndef _nmrPInverse_h
#define _nmrPInverse_h

#include <cisstCommon/cmnThrow.h>
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstNumerical/nmrNetlib.h>
#include <cisstNumerical/nmrSVD.h>

// Always include last
#include <cisstNumerical/nmrExport.h>

/*!
  \ingroup cisstNumerical

  Algorithm P-Inverse: Moore-Penrose pseudo-inverse Calculates the
  Moore-Penrose pseudo-inverse of the M by N matrix A, and stores the
  result in PInverse.  The singular values of A are returned in S.
  The left singular vectors are returned in U, and the right singular
  vectors are returned in V.

  \f$ A^{+} = V * \Sigma^{+} * U^{T} \f$

  where \f$ \Sigma^{+} \f$ is a \f$ N \times M \f$ matrix which is
  zero except for its min(m,n) diagonal elements, U is a \f$ M \times
  M \f$ orthogonal matrix, and V is a \f$ N \times N \f$ orthogonal
  matrix.  The diagonal elements of \f$ \Sigma^{+} \f$ are the
  reciprocal ofc non-zero singular values of A; they are real and
  non-negative, andc are returned in descending order.  The first \f$
  \mbox{min}(m,n) \f$ columns of U and V are the left and right
  singular vectors of A.

  \note This routine returns \f$ V^{T} \f$, not \f$ V \f$.
  \note *On exit*, the content of *A is altered.*

  There are three ways to call this method to compute the pseudo-inverse of
  the matrix A.
  METHOD 1: User provides matrices A and A^{+}
     1) The User allocates memory for these matrices and
     vector.
     vctDynamicMatrix<double> A(5, 4);
     vctDynamicMatrix<double> PInverse(4, 5);
     ...
     2) The user calls the SVD routine
     nmrPInverse(A, PInverse);
     The PInverse method verifies that the size of the data objects matches
     the input, and allocates workspace memory, which is deallocated when
     the function ends.
     The PInverse function alters the contents of matrix A.
     For fixed size the function call is templated by size and row/column major, e.g.
     nmrPInverse<4, 3, VCT_COL_MAJOR>(A, PInverse);

  METHOD 2: Using a preallocated data object
     1) The user creates the input matrix
     vctDynamicMatrix<double> input(rows, cols , VCT_ROW_MAJOR);
     2) The user allocats a data object which could be of
     type nmrPInverseFixedSizeData, nmrPInverseDynamicData and nmrPInverseDynamicDataRef
     corresponding to fixed size, dynamic matrix or dynamic matrix reference.
     nmrPInverseDynamicData data(input);
     3) Call the nmrPInverse function
     nmrPInverse(input, data);
     The contents of input matrix is modified by this routine.
     The matrices U, Vt and vector S are available through the following methods
     std::cout << data.U() << data.S() << data.Vt() << std::endl;
     The pseudo-inverse is available through data.PInverse()

  METHOD 3: User provides matrix PInverse
     with workspace required by pseudo-inverse routine of LAPACK.
     1) User creates matrices and vector
     vctDynamicMatrix<double> A(5, 4);
     vctDynamicMatrix<double> PInverse(4, 5);
     2) User also needs to allocate memory for workspace. This method is particularly
     useful when the user is using more than one numerical methods from the library
     and is willing to share the workspace between them. In such as case, the user
     can allocate the a memory greater than the maximum required by different methods.
     To aid the user determine the minimum workspace required (and not spend time digging
     LAPACK documentation) the library provides helper function
     nmrPInverseDynamicData::WorkspaceSize(input)
     vctDynamicVector<double> Work(nmrPInverseDynamicData::WorkspaceSize(A));
     3) Call the SVD function
     nmrPInverse(A, PInverse, Work);
     or
     For fixed size the above two steps are replaced by
     nmrPInverseFixedSizeData<4, 3, VCT_COL_MAJOR>::TypeWork Work;
     nmrPInverse<4, 3, VCT_COL_MAJOR>(A, PInverse, Work);

     \note The PInverse functions make use of LAPACK routines.  To activate this
     code, set the CISST_HAS_CNETLIB flag to ON during the configuration
     with CMake.
     \note The general rule for numerical functions which depend on LAPACK is that 
     column-major matrices should be used everywhere, and that all
     matrices should be compact.
     \note For the specific case of PInverse, a valid result is also obtained if
     all the matrices are stored in row-major order.  This is an exeption to
     the general rule.  However, mixed-order is not tolerated.
 */

const double NMR_PINVERSE_EPS = 1.0842021724855e-19;
/*
   ****************************************************************************
                                  DYNAMIC SIZE
   ****************************************************************************
 */

/*! This is the class for the composite data container of PInverse.
 */
class nmrPInverseDynamicData {

public:
    /*! Type used for sizes within nmrSVDDynamicData.  This type
      is compatible with the cisstVector containers such as
      vctDynamicMatrix and vctDynamicVector (unsigned int).  To call
      the Fortran based routines, these values must be cast to
      #F_INTEGER. */
    typedef unsigned int size_type;

protected:
    /*!
      Memory allocated for Workspace matrices if needed
      This includes memory needed for SVD as well.
      Order of storage is m x m elements of U followed by
      n x n elements of Vt followed by min (m, n) elements of
      S, followed by memory for LAPACK workspace.
     */
    vctDynamicVector<double> WorkspaceMemory;
    /*!
      Memory allocated for PInverse if needed.
     */
    vctDynamicVector<double> OutputMemory;
    /*! References to workspace or return types, these point either
      to user allocated memory or our memory chunks if needed
     */
    //@{
    vctDynamicMatrixRef<double> PInverseReference;
    vctDynamicMatrixRef<double> UReference;
    vctDynamicMatrixRef<double> VtReference;
    vctDynamicVectorRef<double> SReference;
    vctDynamicVectorRef<double> WorkspaceReference;
    //@}

    /* Just store M, N, and StorageOrder which are needed
       to check if A matrix passed to solve method matches
       the allocated size. */
    size_type MMember;
    size_type NMember;
    bool StorageOrderMember;


    /*! Private method to set the data members MMember, NMember and
      StorageOrder.  This method must be called before
      AllocateOutputWorkspace, ThrowUnlessOutputSizeIsCorrect or
      ThrowUnlessWorkspaceSizeIsCorrect. */
    inline void SetDimension(size_type m, size_type n, bool storageOrder)
    {
        StorageOrderMember = storageOrder;
        MMember = m;
        NMember = n;
    }

    /*! Private method to allocate memory for the output and the
      workspace if needed.  This method assumes that the dimension m
      and n as well as the storage order are already set.  It is
      important to use this method in all the methods provided in the
      user API, even if all the memory is provided by the user since
      this method will ensure that the data
      (nmrPInverseDynamicData) does not keep any memory allocated.
      This is for the case where a single data is used first to
      allocate everything and, later on, used with user allocated
      memory (for either the workspace or the output).

      \note The method SetDimension must have been called before.
    */
    inline void AllocateOutputWorkspace(bool allocateOutput, bool allocateWorkspace)
    {
        // allocate output
        if (allocateOutput) {
            OutputMemory.SetSize(MMember * NMember);
            PInverseReference.SetRef(NMember, MMember,
                                     (StorageOrderMember) ? MMember : 1,
                                     (StorageOrderMember) ? 1 : NMember,
                                     OutputMemory.Pointer());
        } else {
            OutputMemory.SetSize(0);
        }
        // allocate workspace
        if (allocateWorkspace) {
            this->WorkspaceMemory.SetSize(WorkspaceSize(MMember, NMember));
            this->SetRefSVD(this->WorkspaceMemory);
        } else {
            this->WorkspaceMemory.SetSize(0);
        }
    }


    /*! Set all internal references used for the SVD problem.  This
      method requires a valid workspace but it doesn't check the size.
      It is the caller responsability to do so.

      \note The method SetDimension must have been called before.
    */
    template <class _vectorOwnerTypeWorkspace>
    inline void SetRefSVD(vctDynamicVectorBase<_vectorOwnerTypeWorkspace, double> & workspace)
    {
        const size_type minmn = (MMember < NMember) ? MMember : NMember;
        size_type current = 0;
        UReference.SetRef(MMember, MMember,
                          (StorageOrderMember) ? MMember : 1,
                          (StorageOrderMember) ? 1 : MMember,
                          workspace.Pointer(current));
        current += (MMember * MMember);
        VtReference.SetRef(NMember, NMember,
                           (StorageOrderMember) ? NMember : 1,
                           (StorageOrderMember) ? 1 : NMember,
                           workspace.Pointer(current));
        current += (NMember * NMember);
        SReference.SetRef(minmn,
                          workspace.Pointer(current),
                          1);
        current += minmn;
        WorkspaceReference.SetRef(nmrSVDDynamicData::WorkspaceSize(MMember, NMember),
                                  workspace.Pointer(current),
                                  1);
    }



    /*!  Verifies that the user provided reference for the output
      match the size of the data as set by SetDimension.  This
      method also checks that the storage order is consistent.

      \note The method SetDimension must have been called before.
    */
    template <typename _matrixOwnerTypePInverse>
    inline void ThrowUnlessOutputSizeIsCorrect(vctDynamicMatrixBase<_matrixOwnerTypePInverse, double> & pInverse) const
        throw(std::runtime_error)
    {
        // check sizes and storage order
        if ((MMember != pInverse.cols()) || (NMember != pInverse.rows())) {
            cmnThrow(std::runtime_error("nmrPInverseDynamicData: Size of matrix pInverse is incorrect."));
        }
        if (pInverse.StorageOrder() != StorageOrderMember) {
            cmnThrow(std::runtime_error("nmrPInverseDynamicData: Storage order of pInverse is incorrect."));
        }
    }

    /*! Verifies that the user provided references for the workspace
      match (or is greated than) the size of the data as set by
      SetDimension.  This method also checks that the workspace is
      compact.

      \note The method SetDimension must have been called before.
    */
    template <typename _vectorOwnerTypeWorkspace>
    inline void
    ThrowUnlessWorkspaceSizeIsCorrect(vctDynamicVectorBase<_vectorOwnerTypeWorkspace, double> & workspace) const
        throw(std::runtime_error)
    {
        const size_type lwork = nmrSVDDynamicData::WorkspaceSize(MMember, NMember);
        if (lwork > workspace.size()) {
            cmnThrow(std::runtime_error("nmrPInverseDynamicData: Workspace is too small."));
        }
        if (!workspace.IsCompact()) {
            cmnThrow(std::runtime_error("nmrPInverseDynamicData: Workspace must be compact."));
        }
    }


public:
    /*! Helper methods for user to set min working space required by LAPACK
      SVD routine plus memory needed for S, U & Vt matrices used to compute
      PInverse.
      \param m, n The size of matrix whose SVD needs to be computed
     */
    static inline size_type WorkspaceSize(size_type m, size_type n)
    {
        const size_type minmn = (m < n) ? m : n;
        const size_type maxmn = (m > n) ? m : n;
        const size_type lwork_1 = 3 * minmn + maxmn;
        const size_type lwork_2 = 5 * minmn;
        const size_type lwork = (lwork_1 > lwork_2) ? lwork_1 : lwork_2;
        //     u       vt      s       workspace
        return m * m + n * n + minmn + lwork;
    }

    /*! Helper method to determine the min working space required by LAPACK
      SVD routine plus memory needed for S, U & Vt matrices used to compute
      PInverse.
      \param A The matrix whose SVD needs to be computed
     */
    template <class _matrixOwnerTypeA>
    static inline size_type WorkspaceSize(vctDynamicMatrixBase<_matrixOwnerTypeA, double> &A)
    {
        return nmrPInverseDynamicData::WorkspaceSize(A.rows(), A.cols());
    }

#ifndef SWIG
#ifndef DOXYGEN
    /*! This class is not intended to be a top-level API.  It has been
      provided to avoid making the templated SVD function as a friend
      of this class, which turns out to be not so easy in
      .NET. Instead the Friend class provides a cumbersome way to get
      non-const references to the private data.
     */
    class Friend {
    private:
        nmrPInverseDynamicData & Data;
    public:
        Friend(nmrPInverseDynamicData &data): Data(data) {
        }
        inline vctDynamicVectorRef<double> &S(void) {
            return Data.SReference;
        }
        inline vctDynamicMatrixRef<double> &PInverse(void) {
            return Data.PInverseReference;
        }
        inline vctDynamicMatrixRef<double> &U(void) {
            return Data.UReference;
        }
        inline vctDynamicMatrixRef<double> &Vt(void) {
            return Data.VtReference;
        }
        inline vctDynamicVectorRef<double> &Workspace(void) {
            return Data.WorkspaceReference;
        }
        inline size_type M(void) {
            return Data.MMember;
        }
        inline size_type N(void) {
            return Data.NMember;
        }
        inline bool StorageOrder(void) {
            return Data.StorageOrderMember;
        }
    };
    friend class Friend;
#endif // DOXYGEN
#endif // SWIG
    
    /*! The default constuctor.
      For dynamic size, there are assigned default values,
      which MUST be changed by calling appropriate methods.
      (See nmrPInverseDynamicData::Allocate and
      nmrPInverseDynamicData::SetRef)
     */
    nmrPInverseDynamicData():
        MMember((size_type)0),
        NMember((size_type)0),
        StorageOrderMember((bool)VCT_COL_MAJOR)
    {
        AllocateOutputWorkspace(false, false);
    };
    

    /*! Constructor where user provides the input matrix to specify size,
      Memory allocation is done for output matrices and vectors as well as
      Workspace used by LAPACK. This case covers the scenario when user
      wants to make all system calls for memory allocation before entrying
      time critical code sections.
      \param A input matrix
    */
    template <class _matrixOwnerTypeA>
    nmrPInverseDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, double> &A)
    {
        this->Allocate(A.rows(), A.cols(), A.StorageOrder());
    }

    /*! Constructor where user provides the input matrix to specify size,
      Memory allocation is done for output matrices and vectors.
      This case covers the scenario when user wants to make all system
      calls for memory allocation before entrying time critical code sections
      and might be using more than one numerical method in the *same* thread,
      allowing her to share the workspace for LAPACK.
      \param A input matrix
      \output workspace workspace for SVD
    */
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeWorkspace>
    nmrPInverseDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, double> & A,
                               vctDynamicVectorBase<_vectorOwnerTypeWorkspace, double> & workspace)
    {
        this->SetRefWorkspace(A, workspace);
    }

    /*! Constructor where user provides the size and storage order of the input matrix,
      along with matrix AP and workspace.
      The data object now acts as a composite container to hold, pass and
      manipulate a convenitent storage for PInverse algorithm. Checks are made on the
      validity of the input and its consitency with the size of input matrix.
      \param m, n The size of input matrix
      \param storageOrder The storage order of input matrix
      \param pInverse The output matrices for PInverse
      \param workspace The workspace for LAPACK.
    */
    template <class _matrixOwnerTypeA,
              class _matrixOwnerTypePInverse,
              class _vectorOwnerTypeWorkspace>
    nmrPInverseDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, double> & A,
                               vctDynamicMatrixBase<_matrixOwnerTypePInverse, double> & pInverse,
                               vctDynamicVectorBase<_vectorOwnerTypeWorkspace, double> & workspace)
    {
        this->SetRef(pInverse, workspace);
    }
    /*! Constructor where user provides the size and storage order of the input matrix,
      along with matrix PInverse.
      The data object now acts as a composite container to hold, pass and
      manipulate a convenitent storage for SVD algorithm. Checks are made on thec
      validity of the input and its consitency with the size of input matrix.
      Memory allocation for workspace is done by the method.
      This case covers the scenario when user wants to make all system
      calls for memory allocation before entrying time critical code sections
      and might be using the PInverse matrix elsewhere in the *same* thread.
      \param m, n The size of input matrix
      \param storageOrder The storage order of input matrix
      \param pInverse The output matrix for PInverse
    */
    template <class _matrixOwnerTypeA, class _matrixOwnerTypePInverse>
    nmrPInverseDynamicData(vctDynamicMatrixBase<_matrixOwnerTypeA, double> & A,
                               vctDynamicMatrixBase<_matrixOwnerTypePInverse, double> & pInverse)
    {
        this->SetRefOutput(pInverse);
    }

    
    /*! This method allocates memory of output matrices and vector
      as well as the workspace.
      This method should be called before the nmrPInverseDynamicData
      object is passed on to nmrPInverse function, as the memory
      required for output matrices and workspace are allocated
      here or to reallocate memory previously allocated by constructor.
      Typically this method is called from a code segment
      where it is safe to allocate memory and use
      the data and workspace space later.
      \param A The matrix for which SVD needs to be computed, size MxN
    */
    template <class _matrixOwnerTypeA>
    inline void Allocate(vctDynamicMatrixBase<_matrixOwnerTypeA, double> &A)
    {
        this->SetDimension(A.rows(), A.cols(), A.StorageOrder());
        this->AllocateOutputWorkspace(true, true);
    }


    /*! This method allocates memory of output matrices and vector
      and uses the memory provided by user for workspace.
      Check is made to ensure that memory provided by user is sufficient
      for SVD routine of LAPACK.
      This method should be called before the nmrPInverseDynamicData
      object is passed on to nmrPInverse function, as the memory
      required for output matrices and workspace are allocated
      here or to reallocate memory previously allocated by constructor.
      This case covers the scenario when user wants to make all system
      calls for memory allocation before entrying time critical code sections
      and might be using more than one numerical method in the *same* thread,
      allowing her to share the workspace for LAPACK.
      Typically this method is called from a code segment
      where it is safe to allocate memory and use
      the data and workspace space later.
      \param A The matrix for which SVD needs to be computed, size MxN
      \param workspace The vector used for workspace by LAPACK.
    */
    template <class _matrixOwnerTypeA, class _vectorOwnerTypeWorkspace>
    inline void SetRefWorkspace(vctDynamicMatrixBase<_matrixOwnerTypeA, double> &A,
                                vctDynamicVectorBase<_vectorOwnerTypeWorkspace, double> &workspace)
    {
        this->SetDimension(A.rows(), A.cols(), A.StorageOrder());
        this->AllocateOutputWorkspace(true, false);
        // call helper method to set references for SVD components
        this->ThrowUnlessWorkspaceSizeIsCorrect(workspace);
        this->SetRefSVD(workspace);
    }
    
    /*! This method must be called before the data object
      is passed to nmrPInverse function.
      The user provides the input matrix to specify size,
      along with matrix PInverse and workspace.
      The data object now acts as a composite container to hold, pass and
      manipulate a convenitent storage for PInverse algorithm. Checks are made on the
      validity of the input and its consitency with the size of input matrix.
      \param A The input matrix
      \param pInverse The output matrix  for PInverse
      \param workspace The workspace for LAPACK.
    */
    template <class _matrixOwnerTypePInverse,
              class _vectorOwnerTypeWorkspace>
    void SetRef(vctDynamicMatrixBase<_matrixOwnerTypePInverse, double> &pInverse,
                vctDynamicVectorBase<_vectorOwnerTypeWorkspace, double> &workspace)
    {
        this->SetDimension(pInverse.cols(), pInverse.rows(), pInverse.StorageOrder());
        this->AllocateOutputWorkspace(false, false);
        // set reference on output
        this->ThrowUnlessOutputSizeIsCorrect(pInverse);
        this->PInverseReference.SetRef(pInverse);
        // set reference on workspace
        this->ThrowUnlessWorkspaceSizeIsCorrect(workspace);
        this->SetRefSVD(workspace);
    }

    /*! This method must be called before the data object
      is passed to nmrPInverse function.
      The user provides the input matrix to specify size,
      along with matrix PInverse.
      The data object now acts as a composite container to hold, pass and
      manipulate a convenitent storage for PInverse algorithm. Checks are made on thec
      validity of the input and its consitency with the size of input matrix.
      Memory allocation for workspace is done by the method.
      This case covers the scenario when user wants to make all system
      calls for memory allocation before entrying time critical code sections
      and might be using the PInverse matrix elsewhere in the *same* thread.
      \param A The input matrix
      \param pInverse The output matrix for PInverse
    */
    template <class _matrixOwnerTypePInverse>
    void SetRefOutput(vctDynamicMatrixBase<_matrixOwnerTypePInverse, double> &pInverse)
    {
        this->SetDimension(pInverse.cols(), pInverse.rows(), pInverse.StorageOrder());
        this->AllocateOutputWorkspace(false, true);
        this->ThrowUnlessOutputSizeIsCorrect(pInverse);
        this->PInverseReference.SetRef(pInverse);
    }


    
public:
    /*! In order to get access to U, V^t and S, after
      the have been computed by calling nmrPInverse function.
      use the following methods.
    */
    inline const vctDynamicVectorRef<double> &S(void) const {
        return SReference;
    }
    inline const vctDynamicMatrixRef<double> &U(void) const {
        return UReference;
    }
    inline const vctDynamicMatrixRef<double> &Vt(void) const {
        return VtReference;
    }
    inline const vctDynamicMatrixRef<double> &PInverse(void) const {
        return PInverseReference;
    }
};

/*! This function checks for valid input and
  calls the LAPACK function. The approach behind this defintion of the
  function is that the user creates a data object from a code
  wherein it is safe to do memory allocation. This data object
  is then passed on to this method along with the matrix whose
  PInverse is to be computed. The data object has members S, U, Vt 
  and PInverse, which can be accessed through calls to method
  get*() along with adequate workspace for LAPACK.
  This function modifies the contents of matrix A.
  For details about nature of the data matrices see text above.
  \param A A matrix of size MxN, of one of vctDynamicMatrix or vctDynamicMatrixRef
  \param data A data object of one of the types corresponding to
  input matrix
  
  \test nmrPInverseTest::TestDynamicColumnMajor
        nmrPInverseTest::TestDynamicRowMajor
        nmrPInverseTest::TestDynamicColumnMajorUserAlloc
        nmrPInverseTest::TestDynamicRowMajorUserAlloc
*/
template <class _matrixOwnerType>
inline F_INTEGER nmrPInverse(vctDynamicMatrixBase<_matrixOwnerType, double> &A, nmrPInverseDynamicData &data) throw (std::runtime_error)
{
    typedef unsigned int size_type;

    typename nmrPInverseDynamicData::Friend dataFriend(data);
    F_INTEGER ret_value;
    /* check that the size and storage order matches with Allocate() */
    if (A.StorageOrder() != dataFriend.StorageOrder()) {
        cmnThrow(std::runtime_error("nmrPInverse Solve: Storage order used for Allocate was different"));
    }
    if ((A.rows() != dataFriend.M()) || (A.cols() != dataFriend.N())) {
        cmnThrow(std::runtime_error("nmrPInverse Solve: Size used for Allocate was different"));
    }
    const size_type rows = A.rows();
    const size_type cols = A.cols();
    const size_type minmn = (rows < cols) ? rows : cols;
    
    ret_value = nmrSVD(A, dataFriend.U(), dataFriend.S(),
                       dataFriend.Vt(), dataFriend.Workspace());
    const double eps = NMR_PINVERSE_EPS * dataFriend.S().at(0);

    dataFriend.PInverse().SetAll(0);
    double singularValue;
    size_type irank, i, j;
    for (irank = 0; irank < minmn; irank++) {
        if ((singularValue = dataFriend.S().at(irank)) > eps) {
            for (j = 0; j < rows; j++) {
                for (i = 0; i < cols; i++) {
                    dataFriend.PInverse().at(i, j) = dataFriend.PInverse().at(i, j)
                        + dataFriend.Vt().at(irank, i) * dataFriend.U().at(j, irank) / singularValue;
                }
            }
        }
    }
    return ret_value;
}

/*! Basic version of PInverse where user provides the input
  matrix as well as storage for output and workspace needed
  by LAPACK.
  No mem allocation is done in this function, user allocates everything
  inlcuding workspace. The PInverse method verifies that the size
  of the data objects matchesc
  the input. See static methods in nmrPInverseDynamicData for helper
  functions that help determine the min workspace required.
  This function modifies the contents of matrix A.
  For sizes of other matrices see text above.
  \param A is a reference to a dynamic matrix of size MxN
  \param PInverse The output matrix for PInverse
  \param Workspace The workspace for LAPACK.

  \test nmrPInverseTest::TestDynamicColumnMajorUserAlloc
        nmrPInverseTest::TestDynamicRowMajorUserAlloc
 */
template <class _matrixOwnerTypeA, class _matrixOwnerTypePInverse, class _vectorOwnerTypeWorkspace>
inline F_INTEGER nmrPInverse(vctDynamicMatrixBase<_matrixOwnerTypeA, double> &A,
                             vctDynamicMatrixBase<_matrixOwnerTypePInverse, double> &PInverse, vctDynamicVectorBase<_vectorOwnerTypeWorkspace, double> &Workspace)
{
    nmrPInverseDynamicData svdData(A, PInverse, Workspace);
    F_INTEGER ret_value = nmrPInverse(A, svdData);
    return ret_value;
}

/*! Basic version of PInverse where user provides the input
  matrix as well as storage for output.
  The PInverse method verifies that the size
  of the data objects matches
  the input and allocates workspace memory, which is deallocated when
  the function ends.
  This function modifies the contents of matrix A.
  For sizes of other matrices see text above.
  \param A is a reference to a dynamic matrix of size MxN
  \param PInverse The output matrices and vector for PInverse

  \test nmrPInverseTest::TestDynamicColumnMajorUserAlloc
        nmrPInverseTest::TestDynamicRowMajorUserAlloc
 */
template <class _matrixOwnerTypeA, class _matrixOwnerTypePInverse>
inline F_INTEGER nmrPInverse(vctDynamicMatrixBase<_matrixOwnerTypeA, double> &A,
                             vctDynamicMatrixBase<_matrixOwnerTypePInverse, double> &PInverse)
{
    nmrPInverseDynamicData svdData(A, PInverse);
    F_INTEGER ret_value = nmrPInverse(A, svdData);
    return ret_value;
}

/*
   ****************************************************************************
                                  FIXED SIZE
   ****************************************************************************
 */

/*! This provides the data class for fixed size matrices
  and provides a easy to use template. That is
  nmrPInverseFixedSizeData<4, 3, VCT_COL_MAJOR> vs.
  nmrPInverseDataBase<vctFixedSizeMatrix<4, 3, VCT_COL_MAJOR>
  No extra workspace of allocation etc is required for fixed size.
 */
template <unsigned int _rows, unsigned int _cols, bool _storageOrder>
class nmrPInverseFixedSizeData
{
public:
    typedef unsigned int size_type;
#ifndef DOXYGEN
    enum {MIN_MN = (_rows < _cols) ? _rows : _cols};
    enum {LWORK_1 = 3 * MIN_MN + (_rows > _cols) ? _rows : _cols};
    enum {LWORK_2 = 5 * MIN_MN};
    enum {LWORK_3 = ((size_type)LWORK_1 > (size_type)LWORK_2) ? (size_type)LWORK_1 : (size_type)LWORK_2};
    enum {LWORK = _rows * _rows + _cols * _cols + (size_type)MIN_MN + (size_type)LWORK_3};
#endif // DOXYGEN

    typedef vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> MatrixTypeA;
    typedef vctFixedSizeMatrix<double, _cols, _rows, _storageOrder> MatrixTypePInverse;
    typedef vctFixedSizeVector<double, LWORK> VectorTypeWorkspace;
    typedef vctFixedSizeMatrixRef<double, _rows, _rows, _storageOrder ? _rows : 1, _storageOrder ? 1 : _rows> MatrixTypeU;
    typedef vctFixedSizeMatrixRef<double, _cols, _cols, _storageOrder ? _cols : 1, _storageOrder ? 1 : _cols> MatrixTypeVt;
    typedef vctFixedSizeVectorRef<double, MIN_MN, 1> VectorTypeS;
    typedef vctFixedSizeVectorRef<double, LWORK_3, 1> VectorTypeSVDWorkspace;
private:
    /*! Matrix for pseduoinverse */
    MatrixTypePInverse PInverseMember;

    /*! Workspace, including memory for output of SVD
      Order is U, Vt, S, workspace for SVD
    */
    VectorTypeWorkspace WorkspaceMember;

    /*! References to U, Vt, S and workspace in vector Workspace */
    MatrixTypeU UReference;
    MatrixTypeVt VtReference;
    VectorTypeS SReference;
    VectorTypeSVDWorkspace SVDWorkspaceReference;
public:
    
    /*! This class is not intended to be a top-level API.
      It has been provided to avoid making the tempalted
      PInverse function as a friend of this class, which turns
      out to be not so easy in .NET. Instead the Friend class
      provides a cumbersome way to get non-const references
      to the private data.
      Inorder to get non-const references the user has
      to first create a object of nmrPInverseFixedSizeData::Friend
      and then user get* method on that object. Our philosophy
      here is that this should be deterent for a general user
      and should ring alarm bells in a reasonable programmer.
    */
    class Friend {
    private:
        nmrPInverseFixedSizeData<_rows, _cols, _storageOrder> & Data;
    public:
        Friend(nmrPInverseFixedSizeData<_rows, _cols, _storageOrder> &data): Data(data) {
        }
        inline MatrixTypePInverse & PInverse(void) {
            return Data.PInverseMember;
        }
        inline VectorTypeS & S(void) {
            return Data.SReference;
        }
        inline MatrixTypeU & U(void) {
            return Data.UReference;
        }
        inline MatrixTypeVt & Vt(void) {
            return Data.VtReference;
        }
        inline VectorTypeWorkspace & Workspace(void) {
            return Data.WorkspaceMember;
        }
    };
    friend class Friend;
    
    /*we set the references too */
    nmrPInverseFixedSizeData():
        UReference(WorkspaceMember.Pointer(0)),
        VtReference(WorkspaceMember.Pointer(_rows * _rows)),
        SReference(WorkspaceMember.Pointer(_rows * _rows + _cols * _cols)),
        SVDWorkspaceReference(WorkspaceMember.Pointer(_rows * _rows + _cols * _cols + MIN_MN))
    {};
    
    /*! In order to get access to U, V^t and S, after
      the have been computed by calling nmrPInverse function.
      use the following methods.
    */
    inline const MatrixTypePInverse & PInverse(void) const {
        return PInverseMember;
    }
    inline const VectorTypeS & S(void) const {
        return SReference;
    }
    inline const MatrixTypeU & U(void) const {
        return UReference;
    }
    inline const MatrixTypeVt & Vt(void) const {
        return VtReference;
    }
};



/*! Basic version of svd where user provides the input
  matrix as well as storage for output and workspace needed
  by LAPACK.
  No mem allocation is done in this function, user allocates everything
  inlcuding workspace. The SVD method verifies that the size
  of the S and workspace vector objects match
  the input.
  This function modifies the contents of matrix A.
  For sizes of other matrices see text above.
  \param A is a reference to a dynamic matrix of size MxN
  \param U, S, Vt The output matrices and vector for SVD
  \param Work The workspace for LAPACK.
 
  \test nmrPInverseTest::TestFixedSizeColumnMajorMLeqN_T2
        nmrPInverseTest::TestFixedSizeRowMajorMLeqN_T2
        nmrPInverseTest::TestFixedSizeColumnMajorMGeqN_T2
        nmrPInverseTest::TestFixedSizeRowMajorMGeqN_T2
 */
template <unsigned int _rows, unsigned int _cols, unsigned int _work, bool _storageOrder, class _dataPtrType>
inline F_INTEGER nmrPInverse(vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> & A,
                             vctFixedSizeMatrix<double, _cols, _rows, _storageOrder> & pInverse,
                             vctFixedSizeVectorBase<_work, 1, double, _dataPtrType> & workspace)
{
    typedef unsigned int size_type;
    const size_type lwork = nmrPInverseFixedSizeData<_rows, _cols, _storageOrder>::LWORK;
    const size_type minmn = nmrPInverseFixedSizeData<_rows, _cols, _storageOrder>::MIN_MN;
    //Assert if requirement is greater than size provided!
    CMN_ASSERT(lwork <= _work);
    // split Work into submatrices and vectors
    // for this we simply use the nmrSVD with dynamic ref as parameters as we dont have
    // nmrSVD with fixedsizeref as parameters
    vctDynamicMatrixRef<double> ARef(A);
    vctDynamicMatrixRef<double> URef(_rows, _rows,
                                     (_storageOrder) ? _rows : 1,
                                     (_storageOrder) ? 1 : _rows,
                                     workspace.Pointer(0));
    vctDynamicMatrixRef<double> VtRef(_cols, _cols,
                                      (_storageOrder) ? _cols : 1,
                                      (_storageOrder) ? 1 : _cols,
                                      workspace.Pointer(_rows*_rows));
    vctDynamicVectorRef<double> SRef(minmn,
                                     workspace.Pointer(_rows * _rows + _cols * _cols));
    vctDynamicVectorRef<double> SVDWorkspaceRef(lwork,
                                                workspace.Pointer(_rows * _rows + _cols * _cols + minmn));
    F_INTEGER ret_value;
    ret_value = nmrSVD(ARef, URef, SRef, VtRef, SVDWorkspaceRef);
    const double eps = NMR_PINVERSE_EPS * SRef(0);
    pInverse.SetAll(0);
    double singularValue;
    size_type irank, i, j;
    for (irank = 0; irank < minmn; irank++) {
        if ((singularValue = SRef(irank)) > eps) {
            for (j = 0; j < _rows; j++) {
                for (i = 0; i < _cols; i++) {
                    pInverse(i, j) = pInverse(i, j)
                        + VtRef(irank, i) * URef(j, irank) / singularValue;
                }
            }
        }
    }

    return ret_value;
}

/*! Basic version of svd where user provides the input
  matrix as well as storage for output.
  The SVD method verifies that the size
  of the S vector object matches
  the input and allocates workspace memory, which is deallocated when
  the function ends.
  This function modifies the contents of matrix A.
  For sizes of other matrices see text above.
  \param A is a reference to a dynamic matrix of size MxN
  \param U, S, Vt The output matrices and vector for SVD

  \test nmrPInverseTest::TestFixedSizeColumnMajorMLeqN_T2
        nmrPInverseTest::TestFixedSizeRowMajorMLeqN_T2
        nmrPInverseTest::TestFixedSizeColumnMajorMGeqN_T2
        nmrPInverseTest::TestFixedSizeRowMajorMGeqN_T2
 */
template <unsigned int _rows, unsigned int _cols, bool _storageOrder>
inline F_INTEGER nmrPInverse(vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> &A,
                             vctFixedSizeMatrix<double, _cols, _rows, _storageOrder> &pInverse)
{
    typename nmrPInverseFixedSizeData<_rows, _cols, _storageOrder>::VectorTypeWorkspace workspace;
    F_INTEGER ret_value = nmrPInverse(A, pInverse, workspace);
    return ret_value;
}

/*! specialized version of svd for Fixed Size Matrix.
  since all error checking is done at compile time.
  The approach behind this defintion of the
  function is that the user creates a data object from a code
  wherein it is safe to do memory allocation. This data object
  is then passed on to this method along with the matrix whose
  SVD is to be computed. The data object has members S, U,
  and Vt, which can be accessed through calls to method
  get*() along with adequate workspace for LAPACK.
  This function modifies the contents of matrix A.
  For details about nature of the data matrices see text above.
  \param A A matrix of size MxN, of one of vctDynamicMatrix or vctDynamicMatrixRef
  \param data A data object of one of the types corresponding to
  input matrix

  \test nmrPInverseTest::TestFixedSizeColumnMajorMLeqN
        nmrPInverseTest::TestFixedSizeRowMajorMLeqN
        nmrPInverseTest::TestFixedSizeColumnMajorMGeqN
        nmrPInverseTest::TestFixedSizeRowMajorMGeqN
 */
template <unsigned int _rows, unsigned int _cols, bool _storageOrder>
inline F_INTEGER nmrPInverse(vctFixedSizeMatrix<double, _rows, _cols, _storageOrder> &A,
                             nmrPInverseFixedSizeData<_rows, _cols, _storageOrder> &data)
{
    typename nmrPInverseFixedSizeData<_rows, _cols, _storageOrder>::Friend dataFriend(data);
    /* all the checking is done by SVD */
    F_INTEGER ret_value = nmrPInverse(A, dataFriend.PInverse(), dataFriend.Workspace());
    return ret_value;
}


#endif


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: nmrPInverse.h,v $
// Revision 1.8  2006/01/28 05:59:25  anton
// cisstNumerical: Added nmrInverse with tests, doxygen documentation and
// example for numerical quickstart.  See #207.
//
// Revision 1.7  2006/01/27 00:59:49  anton
// cisstNumerical: Renamed "solution" to "data".  See #205.
//
// Revision 1.6  2006/01/18 03:30:12  anton
// cisstNumerical: Typo in workspace
//
// Revision 1.5  2006/01/10 04:06:12  anton
// nmrPInverse.h: Major update of the API.  This class now complies with the
// new designed developped with nmrSVD.  The documentation has not been updated!
//
// Revision 1.4  2006/01/05 22:11:16  anton
// nmrPInverse: Renamed data members and Get methods, defined a size_type and
// use it.  Renamed internal typedef to match new convention in nmrSVD.  Still
// have to re-organize methods and update documentation.
//
// Revision 1.3  2005/12/20 16:36:56  anton
// nmrPInverse: Update to reflect new nmrSVD API.
//
// Revision 1.2  2005/11/29 03:08:03  anton
// cisstNumerical: Systematic use of nmrNetlib.h and layout.
//
// ****************************************************************************
