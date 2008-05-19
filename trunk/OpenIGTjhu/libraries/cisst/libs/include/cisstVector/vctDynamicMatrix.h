/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicMatrix.h,v 1.28 2006/01/26 20:45:22 anton Exp $
  
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


#ifndef _vctDynamicMatrix_h
#define _vctDynamicMatrix_h


/*!
  \file
  \brief Declaration of vctDynamicMatrix
*/


#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctDynamicMatrixBase.h>
#include <cisstVector/vctDynamicMatrixRef.h>
#include <cisstVector/vctDynamicMatrixOwner.h>


/*!
  \ingroup cisstVector

  \brief A matrix object of dynamic size.

  This class defines a matrix object of dynamic size with memory
  allocation.

  The algebraic operations are mostly inherited from the base classes
  vctDynamicMatrixBase and vctDynamicConstMatrixBase.  Here, we will
  briefly describe the specific properties of vctDynamicMatrix, with a
  few usage examples.

  <ol>
  <li> The class is templated by its element type, that is, the matrix
  element.  Normally, the element should be an arithmetic type, that
  is, support all the standard arithmetic operations: +, -, *, /, =,
  ==, <, >, <=, >=, ...
  <li> The class uses dynamically allocated memory, and, more
  importantly, \em owns the memory.  That is, a vctDynamicMatrix
  object automatically frees the allocated memory it owns when it is
  destroyed.
  <li>To allocate the memory, use one of the following operations.
  \code
  // define a typical element type
  typedef double ElementType;

  // the matrixRows and matrixCols variables can be set to any value at
  // any time before creating the matrix.
  unsigned int matrixRows = 12;
  unsigned int matrixCols = 9;
  
  // constructor allocation
  vctDynamicMatrix<ElementType> m1(matrixRows, matrixCols);
  
  // Create an empty matrix and later allocate memory.
  vctDynamicMatrix<ElementType> m2;
  m2.SetSize(matrixRows, matrixCols);

  // Create a dynamic matrix of some size and then change it.
  // This operation does not preserve any elements in the resized
  // matrix
  vctDynamicMatrix<Elements> m3(3 * matrixRows, 3 * matrixCols);
  m3.SetSize(2 * matrixRows, 2 * matrixCols);
  
  // resize a matrix and keep as many elements as possible.
  m3.resize(matrixRows, matrixCols);
  
  // Store an algebraic result to a new matrix.  In this case,
  // memory is allocated by the algebraic operation, and then
  // attached to the matrix object.
  vctDynamicMatrix<double> m4 = m3 - m2;
  \endcode
  <li> The default storage order is row first.  This can be modified
  using the different constructors as well as the method SetSize with
  the flags #VCT_ROW_MAJOR or #VCT_COL_MAJOR.
  \code
  // 12 by 7 matrix stored column first
  vctDynamicMatrix<double> m1(12, 7, VCT_COL_MAJOR);
  // a similar matrix filled with zeroes
  vctDynamicMatrix<double> m1(12, 7, 0.0, VCT_COL_MAJOR);
  // resize the matrix and change its storage order
  m1.SetSize(5, 7, VCT_ROW_MAJOR);
  \endcode
  <li> Matrix assignment can be facilitated through the Assign method
  (defined in the base class) or as follows.
  \code
  // Initialize all elements to the same value
  vctDynamicMatrix<ElementType> v5(matrixRows, matrixCols, 2.0);
  
  // Initialize the elements by specific values.  NOTE: All the
  // arguments MUST be of type ElementType
  vctDynamicMatrix<ElementType> matrix(2, 4);
  matrix.Assign(7.0, 1.0, 2.0, 3.0,
                4.0, 5.0, 6.0, 7.0); // correct
  matrix.Assign(7, 1, 2, 3,
                4, 5, 6, 7); // WRONG, missing dot
  
  // Assign one matrix to another.
  vctDynamicMatrix<int> matrixInt;
  matrixInt.Assign(matrix);
  matrixInt = matrix; // same operation
  \endcode
  </ol>

  A few more notes.
  <ul>
  <li> The elements lie in contiguous increasing memory addresses.
  <li> There is no direct way of ``disowning'' a vctDynamicMatrix,
  that is taking ownership of its memory.
  <li> When a function returns an allocating dynamic matrix object, it
  is better to return a vctReturnDynamicMatrix, which is a helper
  class designed for transfering the ownership of the allocated memory
  without reallocation.
  </ul>

  \param _elementType the type of an element in the matrix

  \sa vctDynamicMatrixBase vctDynamicConstMatrixBase
*/
template<class _elementType>
class vctDynamicMatrix : public vctDynamicMatrixBase<vctDynamicMatrixOwner<_elementType>, _elementType>
{

    friend class vctReturnDynamicMatrix<_elementType>;

public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    typedef vctDynamicMatrixBase<vctDynamicMatrixOwner<_elementType>, _elementType> BaseType;
    typedef vctDynamicMatrix<_elementType> ThisType;

    /* documented in base class */
    typedef typename BaseType::MatrixSizeType MatrixSizeType;

    /*! Default constructor. Initialize an empty matrix. */
    vctDynamicMatrix()
        // The default initialization of vctDynamicMatrixOwner is empty.
    {}


    /* Constructor: Create a matrix of the specified size.  Elements
       initialized with default constructor.  The storage order can be
       either #VCT_ROW_MAJOR or #VCT_COL_MAJOR. */
    //@{
    vctDynamicMatrix(size_type rows, size_type cols, bool storageOrder = VCT_DEFAULT_STORAGE) {
        SetSize(rows, cols, storageOrder);
    }

    vctDynamicMatrix(const MatrixSizeType & matrixSize, bool storageOrder = VCT_DEFAULT_STORAGE) {
        SetSize(matrixSize, storageOrder);
    }
    //@}

    /*! Constructor: Create a matrix of the specified size and assign all
      elements a specific value. The storage order can be
       either #VCT_ROW_MAJOR or #VCT_COL_MAJOR.*/
    vctDynamicMatrix(size_type rows, size_type cols, value_type value, bool storageOrder = VCT_DEFAULT_STORAGE) {
        SetSize(rows, cols, storageOrder);
        SetAll(value);
    }

    /*! Special copy constructor: Take ownership of the data of a
      temporary matrix object of type vctReturnDynamicMatrix.  Disown
      the other matrix.
    */
    vctDynamicMatrix(const vctReturnDynamicMatrix<value_type> & otherMatrix);

    
    /*! Copy constructor: Allocate memory to store a copy of the other
      matrix, and copy the elements of the other matrix to this
      matrix.
    */
    vctDynamicMatrix(const ThisType & otherMatrix) {
        SetSize(otherMatrix.rows(), otherMatrix.cols(), otherMatrix.StorageOrder());
        this->Assign(otherMatrix);
    }
    
    /*! Copy constructor: Allocate memory and copy all the elements
      from the other matrix. The storage order can be either
      #VCT_ROW_MAJOR or #VCT_COL_MAJOR.*/
    template<class _otherMatrixOwnerType>
    vctDynamicMatrix(const vctDynamicConstMatrixBase<_otherMatrixOwnerType, _elementType> & otherMatrix, bool storageOrder = VCT_DEFAULT_STORAGE) {
        SetSize(otherMatrix.rows(), otherMatrix.cols(), storageOrder);
        this->Assign(otherMatrix);
    }


    /*!  Assignment from a dynamic matrix to a matrix.  The
      operation discards the old memory allocated for this matrix, and
      allocates new memory the size of the input matrix.  Then the
      elements of the input matrix are copied into this matrix.

      \todo This assumes a row major storage.  Needs more work.
    */
    template <class __matrixOwnerType, typename __elementType>
    ThisType & operator=(const vctDynamicConstMatrixBase<__matrixOwnerType, __elementType> & otherMatrix) {
        SetSize(otherMatrix.rows(), otherMatrix.cols());
        this->Assign(otherMatrix);
    return *this;
    }


    /*!  Assignment from a dynamic matrix to this matrix.  The
      operation discards the old memory allocated for this matrix, and
      allocates new memory the size of the input matrix.  Then the
      elements of the input matrix are copied into this matrix.
    */
    ThisType & operator=(const ThisType& otherMatrix) {
        SetSize(otherMatrix.rows(), otherMatrix.cols(), otherMatrix.StorageOrder());
        this->Assign(otherMatrix);
    return *this;
    }

    /*!  Assignement from a transitional vctReturnDynamicMatrix to a
      vctDynamicMatrix variable.  This specialized operation does not
      perform any element copy.  Instead it transfers ownership of the
      data from the other matrix to this matrix, and disowns the other
      matrix.  The right hand side operand must be a temporary object
      returned, e.g., from a function or overloaded operator.

      \todo This operator needs some revisions.
    */
    ThisType & operator=(const vctReturnDynamicMatrix<value_type> & otherMatrix);

    /*! Assignement of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value) {
        this->SetAll(value);
        return *this;
    }


    /*! Non-destructive size change.  Change the size to the specified
      size, and copy as many elements as possible from the former
      matrix.

      \note This method doesn't allow to change the storage order.
    */
    void resize(size_type rows, size_type cols) {
        this->Matrix.resize(rows, cols);
    }

    /*! DESTRUCTIVE size change.  Change the size to the specified
      size.  Discard of all the old values. The storage order can be
      either #VCT_ROW_MAJOR or #VCT_COL_MAJOR.  If the storage order
      is not specified, it is not modified. */
    //@{
    void SetSize(size_type rows, size_type cols, bool storageOrder) {
        this->Matrix.SetSize(rows, cols, storageOrder);
    }

    void SetSize(const MatrixSizeType & matrixSize, bool storageOrder) {
        this->Matrix.SetSize(matrixSize.Element(0), matrixSize.Element(1), storageOrder);
    }

    void SetSize(size_type rows, size_type cols) {
        this->Matrix.SetSize(rows, cols, this->StorageOrder());
    }

    void SetSize(const MatrixSizeType & matrixSize) {
        this->Matrix.SetSize(matrixSize.Element(0), matrixSize.Element(1), this->StorageOrder());
    }
    //@}
};


/*!
  Class vctReturnDynamicMatrix is specialized to store a temporary
  matrix object by transfering ownership.  An object of this class has
  all the methods inherited from vctDynamicMatrix, but can only be
  constructed in one way -- taking the ownership from another
  vctDynamicMatrix (or vctReturnDynamicMatrix) object.  In turn, when
  an assignment occurs from a vctReturnDynamicMatrix to a
  vctDynamicMatrix (or likewise construction), the lhs
  vctDynamicMatrix strips this object from the ownership of the data.

  Use this class only when you want to return a newly created dynamic
  matrix from a function, without going through memory allocation and
  deallocation.  Never use it on an object that is going to remain
  in scope after constructing the vctReturnDynamicMatrix.
*/
template<class _elementType>
class vctReturnDynamicMatrix : public vctDynamicMatrix<_elementType> {
public:
    /*! Base type of vctReturnDynamicMatrix. */
    typedef vctDynamicMatrix<_elementType> BaseType;
    explicit vctReturnDynamicMatrix(const BaseType & other) {
        BaseType & nonConstOther = const_cast<BaseType &>(other);
        // if we don't save it in a variable, it will be destroyed in the Release operation
        const unsigned int rows = other.rows();
        const unsigned int cols = other.cols();
        const bool storageOrder = other.StorageOrder();
        this->Matrix.Own(rows, cols, storageOrder, nonConstOther.Matrix.Release());
    }
};


// implementation of the special copy constuctor of vctDynamicMatrix
template<class _elementType>
vctDynamicMatrix<_elementType>::vctDynamicMatrix(const vctReturnDynamicMatrix<_elementType> & other) {
    vctReturnDynamicMatrix<_elementType> & nonConstOther =
        const_cast< vctReturnDynamicMatrix<_elementType> & >(other);
    // if we don't save it in a variable, it will be destroyed in the Release operation
    const unsigned int rows = other.rows();
    const unsigned int cols = other.cols();
    const bool storageOrder = other.StorageOrder();
    this->Matrix.Own(rows, cols, storageOrder, nonConstOther.Matrix.Release());
}


// implementation of the special assignment operator from vctReturnDynamicMatrix to vctDynamicMatrix
template<class _elementType>
vctDynamicMatrix<_elementType> &
vctDynamicMatrix<_elementType>::operator =(const vctReturnDynamicMatrix<_elementType> & other) {
    vctReturnDynamicMatrix<_elementType> & nonConstOther =
        const_cast< vctReturnDynamicMatrix<_elementType> & >(other);
    // if we don't save it in a variable, it will be destroyed in the Release operation
    const unsigned int rows = other.rows();
    const unsigned int cols = other.cols();
    const bool storageOrder = other.StorageOrder();
    this->Matrix.Disown();
    this->Matrix.Own(rows, cols, storageOrder, nonConstOther.Matrix.Release());
    return *this;
}


/*! \name Elementwise operations between dynamic matrices. */
//@{
/*! Operation between dynamic matrices (same size).
  \param inputMatrix1 The first operand of the binary operation.
  \param inputMatrix2 The second operand of the binary operation.
  \return The matrix result of \f$op(matrix1, matrix2)\f$.
*/
template<class _matrixOwnerType1, class _matrixOwnerType2, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator + (const vctDynamicConstMatrixBase<_matrixOwnerType1, _elementType> & inputMatrix1,
            const vctDynamicConstMatrixBase<_matrixOwnerType2, _elementType> & inputMatrix2) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix1);
    resultStorage.Add(inputMatrix2);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template<class _matrixOwnerType1, class _matrixOwnerType2, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator - (const vctDynamicConstMatrixBase<_matrixOwnerType1, _elementType> & inputMatrix1,
            const vctDynamicConstMatrixBase<_matrixOwnerType2, _elementType> & inputMatrix2) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix1);
    resultStorage.Subtract(inputMatrix2);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}
//@}



/*! \name Elementwise operations between a dynamic matrix and a
  scalar. */
//@{
/*! Operation between a dynamic matrix and a scalar.
  \param inputMatrix The first operand of the binary operation.
  \param inputScalar The second operand of the binary operation.
  \return The matrix result of \f$op(matrix, scalar)\f$.
*/
template<class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator + (const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix);
    resultStorage.Add(inputScalar);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template<class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator - (const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix);
    resultStorage.Subtract(inputScalar);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template<class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator * (const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix);
    resultStorage.Multiply(inputScalar);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template<class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator / (const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix,
            const _elementType & inputScalar) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix);
    resultStorage.Divide(inputScalar);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}
//@}



/*! \name Elementwise operations between a scalar and a dynamic
  matrix. */
//@{
/*! Operation between a scalar and a dynamic matrix.
  \param inputScalar The first operand of the binary operation.
  \param inputMatrix The second operand of the binary operation.
  \return The matrix result of \f$op(scalar, matrix)\f$.
*/
template<class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator + (const _elementType & inputScalar,
            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix.rows(), inputMatrix.cols());
    resultStorage.SumOf(inputScalar, inputMatrix);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template<class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator - (const _elementType & inputScalar,
            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix.rows(), inputMatrix.cols());
    resultStorage.DifferenceOf(inputScalar, inputMatrix);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template<class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator * (const _elementType & inputScalar,
            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix.rows(), inputMatrix.cols());
    resultStorage.ProductOf(inputScalar, inputMatrix);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented above */
template<class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator / (const _elementType & inputScalar,
            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix.rows(), inputMatrix.cols());
    resultStorage.RatioOf(inputScalar, inputMatrix);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}
//@}


/*! \name Elementwise operations on a dynamic matrix. */
//@{
/*! Unary operation on a dynamic matrix.
  \param inputMatrix The operand of the unary operation
  \return The matrix result of \f$op(matrix)\f$.
*/
template<class _matrixOwnerType, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator - (const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix.rows(), inputMatrix.cols());
    resultStorage.NegationOf(inputMatrix);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}
//@}



#ifndef DOXYGEN
template<class _matrixOwnerType1, class _matrixOwnerType2, class _elementType>
vctReturnDynamicMatrix<_elementType>
operator * (const vctDynamicConstMatrixBase<_matrixOwnerType1, _elementType> & inputMatrix1,
            const vctDynamicConstMatrixBase<_matrixOwnerType2, _elementType> & inputMatrix2) {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(inputMatrix1.rows(), inputMatrix2.cols());
    resultStorage.ProductOf(inputMatrix1, inputMatrix2);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}


template<class _matrixOwnerType, class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator * (const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix,
            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputMatrix.rows());
    resultStorage.ProductOf(inputMatrix, inputVector);
    return vctReturnDynamicVector<value_type>(resultStorage);
}


template<class _vectorOwnerType, class _matrixOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
operator * (const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & inputVector,
            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & inputMatrix) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputMatrix.cols());
    resultStorage.ProductOf(inputVector, inputMatrix);
    return vctReturnDynamicVector<value_type>(resultStorage);
}



/*
  Methods declared previously and implemented here because they require vctReturnDynamicMatrix
*/


/* documented in class vctDynamicConstMatrixBase */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::MatrixValueType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::Abs(void) const {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(this->rows(), this->cols());
    vctDynamicMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<value_type>::AbsValue>::
        Run(resultStorage, *this);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented in class vctDynamicConstMatrixBase */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::MatrixValueType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::Negation(void) const {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(this->rows(), this->cols());
    vctDynamicMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<value_type>::Negation>::
        Run(resultStorage, *this);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented in class vctDynamicConstMatrixBase */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::MatrixValueType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::Floor(void) const {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(this->rows(), this->cols());
    vctDynamicMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<value_type>::Floor>::
        Run(resultStorage, *this);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented in class vctDynamicConstMatrixBase */
template <class _matrixOwnerType, class _elementType>
inline typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::MatrixValueType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::Ceil(void) const {
    typedef _elementType value_type;
    vctDynamicMatrix<value_type> resultStorage(this->rows(), this->cols());
    vctDynamicMatrixLoopEngines::
        MoMi<typename vctUnaryOperations<value_type>::Ceil>::
        Run(resultStorage, *this);
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* documented in class vctDynamicConstMatrixBase */
template <class _matrixOwnerType, class _elementType>
typename vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::MatrixValueType
vctDynamicConstMatrixBase<_matrixOwnerType, _elementType>::Eye(size_type size) {
    vctDynamicMatrix<value_type> resultStorage(size, size);
    resultStorage.SetAll(_elementType(0));
    resultStorage.Diagonal().SetAll(_elementType(1));
    return vctReturnDynamicMatrix<value_type>(resultStorage);
}

/* Documented in class vctDynamicConstMatrixBase */
template<class _matrixOwnerType, class __matrixOwnerType, class _elementType,
         class _elementOperationType>
inline vctReturnDynamicMatrix<bool>
vctDynamicMatrixElementwiseCompareMatrix(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix1,
                                         const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix2) {
    vctDynamicMatrix<bool> result(matrix1.rows(), matrix1.cols());
    vctDynamicMatrixLoopEngines::
        MoMiMi<_elementOperationType>::Run(result, matrix1, matrix2);
    return vctReturnDynamicMatrix<bool>(result);
}

/* documented in class vctDynamicConstMatrixBase */
template<class _matrixOwnerType, class _elementType,
         class _elementOperationType>
inline vctReturnDynamicMatrix<bool>
vctDynamicMatrixElementwiseCompareScalar(const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix,
                                         const _elementType & scalar) {
    vctDynamicMatrix<bool> result(matrix.rows(), matrix.cols());
    vctDynamicMatrixLoopEngines::
        MoMiSi<_elementOperationType>::Run(result, matrix, scalar);
    return vctReturnDynamicMatrix<bool>(result);
}

#endif // DOXYGEN


#endif // _vctDynamicMatrix_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicMatrix.h,v $
//  Revision 1.28  2006/01/26 20:45:22  anton
//  cisstVector: Added Eye methods to compute identity for matrices.  See #210.
//
//  Revision 1.27  2006/01/26 19:05:21  anton
//  cisstVector: Generalized definition of MatrixSizeType and added documentation
//  (see #206 for motivation).
//
//  Revision 1.26  2006/01/25 03:17:16  anton
//  cisstVector and cisstNumerical: Test new type MatrixSizeType and test it
//  with nmrSVD (see ticket #206).
//
//  Revision 1.25  2006/01/18 16:40:02  anton
//  vctDynamicMatrix.h: Added SetSize(rows, cols) with no implicit change of
//  storage order.  Also replaced IsRowMajor() by more explicit and readable
//  StorageOrder().
//
//  Revision 1.24  2006/01/03 03:37:17  anton
//  vctDynamic{Vector,Matrix}.h: Added Doxygen class documentation based on Ofri's
//  input.
//
//  Revision 1.23  2005/12/23 21:27:31  anton
//  cisstVector: Minor updates for Doxygen 1.4.5.
//
//  Revision 1.22  2005/12/13 00:04:56  ofri
//  All cisstVector containers: Added methods
//  1) MinAndMaxElement  (at const base)
//  2) Floor, Ceil  (at const base -- implemented at container)
//  3) FloorSelf, CeilSelf, FloorOf, CeilOf  (at non-const base)
//
//  Revision 1.21  2005/12/02 16:23:54  anton
//  cisstVector: Added assigment operator from scalar (see ticket #191).
//
//  Revision 1.20  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.19  2005/09/12 15:02:19  ofri
//  vctDynamicMatrix : Added definition of BaseType (which was missing)
//
//  Revision 1.18  2005/06/16 03:38:29  anton
//  Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
//  VC 7.1 (.Net 2003).
//
//  Revision 1.17  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.16  2005/04/04 19:48:34  anton
//  cisstVector: In base types for dynamic containers, removed default value for
//  _elementType (was based on _ownerType::value_type) to -1- avoid issues with
//  SWIG and -2- force explicit declaration of _elementType whenever a base type
//  is used.
//
//  Revision 1.15  2004/11/18 20:57:30  ofri
//  RE: ticket #92.  Redefined all operator= I could find to return a ThisType &
//  instead of const ThisType &.  This included corresponding revision of the
//  Assign methods, and also definition from scratch of operator= for fixed and
//  dynamic matrix ref.
//
//  Revision 1.14  2004/11/08 18:10:17  anton
//  cisstVector: Completed the use of VCT_STORAGE_ORDER, added related Doxygen
//  documentation.
//
//  Revision 1.13  2004/11/03 22:26:12  anton
//  cisstVector:  Better support of storage order.  Added VCT_COL_MAJOR,
//  VCT_ROW_MAJOR and VCT_DEFAULT_STORAGE as well as the methods IsColMajor(),
//  IsRowMajor(), IsCompact() and IsFortran().
//
//  Revision 1.12  2004/10/25 19:19:54  anton
//  cisstVector:  Created vctForwardDeclarations.h and removed forward
//  declarations of classes in all other files.  Added some constructors
//  for vector references to reference fixed size from dynamic and vice versa.
//
//  Revision 1.11  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.10  2004/10/21 19:27:47  anton
//  cisstVector: Added types BoolValueType and ValueType for
//  vctFixedSizeVector, vctDynamicVector, vctDynamicMatrix.  This solves #77.
//
//  Revision 1.9  2004/10/19 20:50:50  anton
//  cisstVector: Preliminary support for different storage orders (i.e. row
//  major by default, column major if specified).  This has been implemented
//  for the vctFixedSizeMatrix as well as the vctDynamicMatrix.  We tested the
//  code by simply changinf the default and all current tests passed.  See
//  ticket #75 re. vctFortranMatrix.
//
//  Revision 1.8  2004/10/07 21:21:33  ofri
//  vctDynamicMatrix: added constructor with value for all elements
//
//  Revision 1.7  2004/09/30 15:49:51  anton
//  cisstVector: solution for ticket #65 regarding a compilation error with
//  .Net 2003.  The issue remains mysterious but but a solution is to declare
//  the problematic methods in the class declaration and implement them right
//  after (in the same file).  The methods in question are the Elementwise
//  comparaison with a scalar (it works if comparing with a vector/matrix).
//
//  Revision 1.6  2004/09/24 20:20:42  anton
//  cisstVector: Major update related to ticket #18 regarding order operators
//  and methods.  The methods named Equal(), NotEqual(), Lesser(),
//  LesserOrEqual(), etc.  return a boolean.  The same names with prefix
//  Elementwise (e.g. ElementwiseEqual) return a vector or matrix of the tests
//  performed for each element.  We only kept the operators == and != which
//  return a boolean.  All other operators (>, >=, <, <=) have been removed.
//  Also, in order to have the same API for all the containers, many methods
//  have been added (vctAny, vctAll, etc... see tickets #52 and #61).  The
//  doxygen comments have also been updated.
//
//  Revision 1.5  2004/09/21 20:02:57  anton
//  vctDynamicMatrix: Added operators for matrix * vector and vector * matrix
//
//  Revision 1.4  2004/09/08 21:22:17  anton
//  vctDynamicMatrix: Added method ProductOf() as well as operator * for two
//  matrices.  Needed to implement Column() and Row() methods as well.
//  Corrected bugs in ToStream method.
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
