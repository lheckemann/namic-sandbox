/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicMatrixRef.h,v 1.25 2006/01/26 19:05:21 anton Exp $
  
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on: 2004-07-01

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


#ifndef _vctDynamicMatrixRef_h
#define _vctDynamicMatrixRef_h


/*!
  \file
  \brief Declaration of vctDynamicMatrixRef
*/


#include<cisstVector/vctDynamicMatrixBase.h>
#include<cisstVector/vctDynamicConstMatrixRef.h>


/*!
  \ingroup cisstVector

  This is a concrete matrix, equivalent to vctFixedSizeMatrixRef:
  - Extends the set of operations of <code>vctDynamicConstMatrix</code> with non-const operations.
  - Initialized only with <code>value_type *</code>
*/
template<class _elementType>
class vctDynamicMatrixRef: public vctDynamicMatrixBase<vctDynamicMatrixRefOwner<_elementType>, _elementType>
{
public:
    /* documented in base class */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctDynamicMatrixRef<_elementType> ThisType;
    typedef vctDynamicMatrixRefOwner<_elementType> MatrixOwnerType;
    typedef vctDynamicMatrixBase<vctDynamicMatrixRefOwner<_elementType>, _elementType> BaseType;
    typedef typename MatrixOwnerType::iterator iterator;
    typedef typename MatrixOwnerType::const_iterator const_iterator;
    typedef typename MatrixOwnerType::reverse_iterator reverse_iterator;
    typedef typename MatrixOwnerType::const_reverse_iterator const_reverse_iterator;

    typedef typename BaseType::MatrixSizeType MatrixSizeType;
    
    vctDynamicMatrixRef() {
        SetRef(0, 0, 1, 1, 0);
    }

    vctDynamicMatrixRef(size_type rows, size_type cols, stride_type rowStride, stride_type colStride,
                        pointer dataPointer)
    {
        SetRef(rows, cols, rowStride, colStride, dataPointer);
    }

    vctDynamicMatrixRef(const MatrixSizeType & matrixSize, stride_type rowStride, stride_type colStride,
                        pointer dataPointer)
    {
        SetRef(matrixSize, rowStride, colStride, dataPointer);
    }

    /*! Construct a dynamic reference to a fixed-size matrix.
      \note the strides of this reference will be identical to the strides of
      the input matrix.  For a more advanced access, customize the parameters 
      yourself (see the other ctors).
    */
    template<unsigned int __rows, unsigned int __cols, int __rowStride, int __colStride, typename __dataPtrType>
    inline vctDynamicMatrixRef(vctFixedSizeMatrixBase<__rows, __cols, __rowStride, __colStride, _elementType, __dataPtrType> & otherMatrix)
    {
        SetRef(otherMatrix);
    }

    /*! Construct a dynamic reference to a fixed-size matrix.
      \param otherMatrix a fixed size matrix to which a reference is created.
      \param startRow row index of the position where the reference will start.
      \param startCol column index of the position where the reference will start.
      \param rows the number of rows being referenced
      \param cols the number of columns being referenced
      \note the strides of this reference will be identical to the strides of
      the input matrix.  For a more advanced access, customize the parameters 
      yourself (see the other ctors).
    */
    template<unsigned int __rows, unsigned int __cols, int __rowStride, int __colStride, typename __dataPtrType>
    inline vctDynamicMatrixRef(vctFixedSizeMatrixBase<__rows, __cols, __rowStride, __colStride, _elementType, __dataPtrType> & otherMatrix,
                               size_type startRow, size_type startCol, size_type rows, size_type cols)
    {
        SetRef(otherMatrix, startRow, startCol, rows, cols);
    }

    /*! Construct a dynamic reference to a dynamic matrix.
      \note the strides of this reference will be identical to the strides of
      the input matrix.  For a more advanced access, customize the parameters 
      yourself (see the other ctors).
    */
    template<class __ownerType>
    inline vctDynamicMatrixRef(vctDynamicMatrixBase<__ownerType, _elementType> & otherMatrix)
    {
        SetRef(otherMatrix);
    }

    /*! Construct a dynamic reference to a submatrix of a dynamic matrix.
      \param otherMatrix a fixed size matrix to which a reference is created.
      \param startRow row index of the position where the reference will start.
      \param startCol column index of the position where the reference will start.
      \param rows the number of rows being referenced
      \param cols the number of columns being referenced
      \note the strides of this reference will be identical to the strides of
      the input matrix.  For a more advanced access, customize the parameters 
      yourself (see the other ctors).
    */
    template<class __ownerType>
    inline vctDynamicMatrixRef(vctDynamicMatrixBase<__ownerType, _elementType> & otherMatrix,
                               size_type startRow, size_type startCol, size_type rows, size_type cols)
    {
        SetRef(otherMatrix, startRow, startCol, rows, cols);
    }

    /*! Set a dynamic reference to a memory location.  The user should
      specify all the parameters of the referenced memory, including a
      start memory address. */
    //@{
    void SetRef(size_type rows, size_type cols,
                stride_type rowStride, stride_type colStride,
                pointer dataPointer) {
        this->Matrix.SetRef(rows, cols, rowStride, colStride, dataPointer);
    }

    void SetRef(const MatrixSizeType & matrixSize,
                stride_type rowStride, stride_type colStride,
                pointer dataPointer) {
        this->Matrix.SetRef(matrixSize.Element(0), matrixSize.Element(1),
                            rowStride, colStride, dataPointer);
    }
    //@}


    /*! Set a dynamic reference to a fixed-size matrix.  The reference
      will have identical dimensions and strides as the input matrix.
    */
    template<unsigned int __rows, unsigned int __cols, int __rowStride, int __colStride, typename __dataPtrType>
    inline void SetRef(vctFixedSizeMatrixBase<__rows, __cols, __rowStride, __colStride, _elementType, __dataPtrType> & otherMatrix)
    {
        SetRef(otherMatrix.rows(), otherMatrix.cols(), otherMatrix.row_stride(), otherMatrix.col_stride(),
               otherMatrix.Pointer());
    }

    /*! Set a dynamic reference to a submatrix of a fixed-size matrix.
      \param otherMatrix a fixed size matrix to which a reference is created.
      \param startRow row index of the position where the reference will start.
      \param startCol column index of the position where the reference will start.
      \param rows the number of rows being referenced
      \param cols the number of columns being referenced
      \note the strides of this reference will be identical to the strides of
      the input matrix.  For a more advanced access, customize the parameters 
      yourself (see the SetRef methods).
    */
    template<unsigned int __rows, unsigned int __cols, int __rowStride, int __colStride, typename __dataPtrType>
    inline void SetRef(vctFixedSizeMatrixBase<__rows, __cols, __rowStride, __colStride, _elementType, __dataPtrType> & otherMatrix,
                       size_type startRow, size_type startCol, size_type rows, size_type cols)
    {
        CMN_ASSERT( (startRow + rows <= this->rows()) && (startCol + cols <= this->cols()) );
        SetRef(rows, cols, otherMatrix.row_stride(), otherMatrix.col_stride(), otherMatrix.Pointer(startRow, startCol));
    }

    /*! Set a dynamic reference to a dynamic matrix.  The reference
      will have identical dimensions and strides as the input matrix.
    */
    template<class __ownerType>
    inline void SetRef(vctDynamicMatrixBase<__ownerType, _elementType> & otherMatrix)
    {
        SetRef(otherMatrix.rows(), otherMatrix.cols(), otherMatrix.row_stride(), otherMatrix.col_stride(),
               otherMatrix.Pointer());
    }

    /*! Set a dynamic reference to a submatrix of a dynamic matrix.
      \param otherMatrix a fixed size matrix to which a reference is created.
      \param startRow row index of the position where the reference will start.
      \param startCol column index of the position where the reference will start.
      \param rows the number of rows being referenced
      \param cols the number of columns being referenced
      \note the strides of this reference will be identical to the strides of
      the input matrix.  For a more advanced access, customize the parameters 
      yourself (see the other SetRef methods).
    */
    //@{
    template<class __ownerType>
    inline void SetRef(vctDynamicMatrixBase<__ownerType, _elementType> & otherMatrix,
                       size_type startRow, size_type startCol, size_type rows, size_type cols)
    {
        CMN_ASSERT( (startRow + rows <= otherMatrix.rows()) && (startCol + cols <= otherMatrix.cols()) );
        SetRef(rows, cols, otherMatrix.row_stride(), otherMatrix.col_stride(),
               otherMatrix.Pointer(startRow, startCol));
    }

    template<class __ownerType>
    inline void SetRef(vctDynamicMatrixBase<__ownerType, _elementType> & otherMatrix,
                       const MatrixSizeType & start, const MatrixSizeType & matrixSize)
    {
        SetRef(otherMatrix,
               start.Element(0), start.Element(1), matrixSize.Element(0), matrixSize.Element(1));
    }
    //@}

    /*!
      \name Assignment operation into a matrix reference

      \param other The matrix to be copied.
    */
    //@{
  inline ThisType & operator=(const ThisType & other) {
    return reinterpret_cast<ThisType &>(this->Assign(other));
  }

  inline ThisType & operator=(const vctDynamicConstMatrixRef<_elementType> & other) {
    return reinterpret_cast<ThisType &>(this->Assign(other));
  }

    template<class __matrixOwnerType, typename __elementType>
  inline ThisType & operator=(const vctDynamicConstMatrixBase<__matrixOwnerType, __elementType> & other) {
    return reinterpret_cast<ThisType &>(this->Assign(other));
  }
    //@}

    /*! Assignement of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value) {
        this->SetAll(value);
        return *this;
    }

};


/*
  Methods declared and use for vctDynamicVectorBase.  Implemented here
  since these need the const-matrix base.
*/
#ifndef DOXYGEN
template<class _resultVectorOwnerType, class _matrixOwnerType, class _vectorOwnerType, class _elementType>
inline
void vctMultiplyMatrixVector(vctDynamicVectorBase<_resultVectorOwnerType, _elementType> & result,
                             const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix,
                             const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector) {
    const vctDynamicConstMatrixRef<_elementType> inputVectorRef(vector.size(), 1,
                                                                vector.stride(), 1,
                                                                vector.Pointer());
    vctDynamicMatrixRef<_elementType> resultRef(result.size(), 1,
                                                result.stride(), 1,
                                                result.Pointer());
    resultRef.ProductOf(matrix, inputVectorRef);
}


template<class _resultVectorOwnerType, class _vectorOwnerType, class _matrixOwnerType, class _elementType>
inline
void vctMultiplyVectorMatrix(vctDynamicVectorBase<_resultVectorOwnerType, _elementType> & result,
                             const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector,
                             const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix) {
    const vctDynamicConstMatrixRef<_elementType> inputVectorRef(1, vector.size() * vector.stride(),
                                                                1, vector.stride(),
                                                                vector.Pointer());
    vctDynamicMatrixRef<_elementType> resultRef(1, result.size() * result.stride(),
                                                1, result.stride(),
                                                result.Pointer());
    resultRef.ProductOf(inputVectorRef, matrix);
}
#endif // DOXYGEN

#endif // _vctDynamicMatrixRef_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicMatrixRef.h,v $
//  Revision 1.25  2006/01/26 19:05:21  anton
//  cisstVector: Generalized definition of MatrixSizeType and added documentation
//  (see #206 for motivation).
//
//  Revision 1.24  2006/01/09 20:52:22  anton
//  vctDynamicMatrixRef.h: Layout.
//
//  Revision 1.23  2005/12/23 21:27:31  anton
//  cisstVector: Minor updates for Doxygen 1.4.5.
//
//  Revision 1.22  2005/12/02 16:23:54  anton
//  cisstVector: Added assigment operator from scalar (see ticket #191).
//
//  Revision 1.21  2005/10/06 16:56:37  anton
//  Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
//  Revision 1.20  2005/09/27 18:01:25  anton
//  cisstVector: Use CMN_ASSERT instead of assert for vectors and matrices.
//
//  Revision 1.19  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.18  2005/07/19 15:24:32  anton
//  vctDynamicMatrixRef: All SetRef(vctFixedSizeMatrix) were using a const
//  reference for the input.  Removed the "const" keyword.
//
//  Revision 1.17  2005/06/16 03:38:29  anton
//  Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
//  VC 7.1 (.Net 2003).
//
//  Revision 1.16  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.15  2005/04/04 19:48:34  anton
//  cisstVector: In base types for dynamic containers, removed default value for
//  _elementType (was based on _ownerType::value_type) to -1- avoid issues with
//  SWIG and -2- force explicit declaration of _elementType whenever a base type
//  is used.
//
//  Revision 1.14  2005/01/04 00:01:29  ofri
//  vctDynamic[Const]MatrixRef: Updated the interfaces of constructors and SetRef()
//  following ticket #88.  Tests and example program compile and run successfully
//  on linux.
//
//  Revision 1.13  2004/11/18 20:57:30  ofri
//  RE: ticket #92.  Redefined all operator= I could find to return a ThisType &
//  instead of const ThisType &.  This included corresponding revision of the
//  Assign methods, and also definition from scratch of operator= for fixed and
//  dynamic matrix ref.
//
//  Revision 1.12  2004/11/11 20:35:46  anton
//  cisstVector: *: Added a vctContainerTraits to centralize the declarations
//  of size_type, difference_type, reference, const_reference, etc. *: All
//  iterators are now derived from std::iterator. *: Added some missing typedef
//  for iterators.
//
//  Revision 1.11  2004/11/01 07:03:15  anton
//  cisstVector: A couple of issues in vctDynamicMatrixRef.  The parameter matrix doesn't exist for SetRef.  Also, removed a couple of "const" (not sure of this).
//
//  Revision 1.10  2004/11/01 06:56:00  anton
//  cisstVector: Bugs in SetRef() for dynamic matrices.  Current code didn't
//  set the size (rows, cols) nor the starting point correctly.  I checked the
//  fixed size matrices which seems fine.
//
//  Revision 1.9  2004/10/26 15:20:52  ofri
//  Updating subsequence interfaces (ticket #76) and interfaces between fixed
//  and dynamic vectors/matrices (ticket #72).
//  *: vctFixedSize(Const)VectorBase::Get(Const)Subsequence now deprecated.
//  Replaced by Get(Const)Subvector with equal stride to the parent.
//  *: Added constructors for [Fixed|Dynamic][Vector|Matrix]Ref from the other
//  storage type.
//
//  Revision 1.8  2004/10/25 19:19:54  anton
//  cisstVector:  Created vctForwardDeclarations.h and removed forward
//  declarations of classes in all other files.  Added some constructors
//  for vector references to reference fixed size from dynamic and vice versa.
//
//  Revision 1.7  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.6  2004/10/22 18:07:06  anton
//  Doxygen: Made sure that all the classes are documented for Doxygen.
//
//  Revision 1.5  2004/10/14 19:02:02  ofri
//  vctDynamicMatrixRef: Added constructor and SetRef() from other fixed-
//  and dynamic-size matrix objects. See ticket #72.
//
//  Revision 1.4  2004/09/21 20:05:08  anton
//  vctDynamicMatrixRef: Code was an unmodified copy/paste.  Should work now.
//
//  Revision 1.3  2004/09/03 19:56:36  anton
//  Doxygen documentation.
//
//  Revision 1.2  2004/08/13 17:47:40  anton
//  cisstVector: Massive renaming to replace the word "sequence" by "vector"
//  (see ticket #50) as well as another more systematic naming for the engines.
//  The changes for the API are as follow:
//  *: vctFixedLengthSequenceBase -> vctFixedSizeVectorBase (and Const)
//  *: vctFixedLengthSequenceRef -> vctFixedSizeVectorRef (and Const)
//  *: vctDynamicSequenceBase -> vctDynamicVectorBase (and Const)
//  *: vctDynamicSequenceRef -> vctDynamicVectorRef (and Const)
//  *: vctDynamicSequenceRefOwner -> vctDynamicVectorRefOwner
//  *: vctFixedStrideSequenceIterator -> vctFixedStrideVectorIterator (and Const)
//  *: vctVarStrideSequenceIterator -> vctVarStrideVectorIterator (and Const)
//  *: vctSequenceRecursiveEngines -> vctFixedSizeVectorRecursiveEngines
//  *: vctSequenceLoopEngines -> vctDynamicVectorLoopEngines
//  *: vctMatrixLoopEngines -> vctFixedSizeMatrixLoopEngines
//  *: vctDynamicMatrixEngines -> vctDynamicMatrixLoopEngines
//  Also updated and corrected the documentation (latex, doxygen, figures) as
//  well as the tests and examples.
//
//  Revision 1.1  2004/08/04 21:11:10  anton
//  cisstVector: Added preliminary version of dynamic matrices.  Lots of work
//  is still required, this code is not ready to be used.
//
//
// ****************************************************************************
