/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicMatrix.i,v 1.11 2006/07/10 21:55:00 anton Exp $

  Author(s):  Anton Deguet
  Created on: 2004-03-29

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


// Matrix specifics
%ignore vctDynamicMatrixElementwiseCompareMatrix;
%ignore vctDynamicMatrixElementwiseCompareScalar;
%ignore vctMultiplyMatrixVector;
%ignore vctMultiplyVectorMatrix;


// SWIG va_args is somewhat limited, a Python implementation is more reliable
%ignore *::Assign(value_type element0, value_type element1, ...);
// We don't really want to handle pointers on values in Python
%ignore *::Assign(const value_type * elements);


// Include what needs to be wrapped
%include "cisstVector/vctDynamicMatrixOwner.h"
%include "cisstVector/vctDynamicMatrixRefOwner.h"
%include "cisstVector/vctDynamicConstMatrixBase.h"
%include "cisstVector/vctDynamicMatrixBase.h"
%include "cisstVector/vctDynamicMatrix.h"
%include "cisstVector/vctDynamicConstMatrixRef.h"
%include "cisstVector/vctDynamicMatrixRef.h"



/*
  Macro used to extend a const base with different owner types, no sub owner
 */
%define VCT_DYNAMIC_MATRIX_EXTEND_CONST_BASE_0(owner, elementType)

%extend vctDynamicConstMatrixBase<owner, elementType> {
    // for row = m[i]
    inline vctDynamicConstVectorRef<elementType> __getitem__(index_type index) const throw(std::out_of_range) {
        if (!(*self).ValidRowIndex(index)) {
            throw std::out_of_range("vctDynamicMatrix access out of range");
        }
        return (*self).Row(index);
    }
    // for matrix = scalar <op> matrix
    inline vctDynamicMatrix<elementType> __radd__(elementType scalar) throw(std::runtime_error) {
        vctDynamicMatrix<elementType> result(self->rows(), self->cols());
        result.SumOf(scalar, *self);
        return result;
    }
    inline vctDynamicMatrix<elementType> __rsub__(elementType scalar) throw(std::runtime_error) {
        vctDynamicMatrix<elementType> result(self->rows(), self->cols());
        result.DifferenceOf(scalar, *self);
        return result;
    }
    inline vctDynamicMatrix<elementType> __rmul__(elementType scalar) throw(std::runtime_error) {
        vctDynamicMatrix<elementType> result(self->rows(), self->cols());
        result.ProductOf(scalar, *self);
        return result;
    }
    inline vctDynamicMatrix<elementType> __rdiv__(elementType scalar) throw(std::runtime_error) {
        vctDynamicMatrix<elementType> result(self->rows(), self->cols());
        result.RatioOf(scalar, *self);
        return result;
    }

    // for matrix = matrix <op> scalar
    inline vctDynamicMatrix<elementType> __add__(elementType scalar) throw(std::runtime_error) {
        vctDynamicMatrix<elementType> result(self->rows(), self->cols());
        result.SumOf(*self, scalar);
        return result;
    }
    inline vctDynamicMatrix<elementType> __sub__(elementType scalar) throw(std::runtime_error) {
        vctDynamicMatrix<elementType> result(self->rows(), self->cols());
        result.DifferenceOf(*self, scalar);
        return result;
    }
    inline vctDynamicMatrix<elementType> __mul__(elementType scalar) throw(std::runtime_error) {
        vctDynamicMatrix<elementType> result(self->rows(), self->cols());
        result.ProductOf(*self, scalar);
        return result;
    }
    inline vctDynamicMatrix<elementType> __div__(elementType scalar) throw(std::runtime_error) {
        vctDynamicMatrix<elementType> result(self->rows(), self->cols());
        result.RatioOf(*self, scalar);
        return result;
    }
}

%enddef




/*
  Macro used to extend a const base with different owner types, one sub owner
 */
%define VCT_DYNAMIC_MATRIX_EXTEND_CONST_BASE_1(owner, subOwner, elementType)

%extend vctDynamicConstMatrixBase<owner, elementType> {

    // for matrix = matrix <op> matrix
    inline vctDynamicMatrix<elementType> __add__(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        vctDynamicMatrix<elementType> result(self->rows(), self->cols());
        result.SumOf(*self, other);
        return result;
    }
    inline vctDynamicMatrix<elementType> __sub__(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        vctDynamicMatrix<elementType> result(self->rows(), self->cols());
        result.DifferenceOf(*self, other);
        return result;
    }
    inline vctDynamicMatrix<elementType> __mul__(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        vctDynamicMatrix<elementType> result(self->rows(), other.cols());
        result.ProductOf(*self, other);
        return result;
    }

    // for matrix <comp> matrix
    inline bool __eq__(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return ((*self) == other);
    }
    inline bool __ne__(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return ((*self) != other);
    }
    inline bool AlmostEqual(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->AlmostEqual(other));
    }
    inline bool Equal(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->Equal(other));
    }
    inline bool Greater(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->Greater(other));
    }
    inline bool Lesser(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->Lesser(other));
    }
    inline bool GreaterOrEqual(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->GreaterOrEqual(other));
    }
    inline bool LesserOrEqual(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->LesserOrEqual(other));
    }
    inline bool NotEqual(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->NotEqual(other));
    }
}

%enddef



/*
  Macro used to extend a const base with different owner types with one sub owner for vector
 */
%define VCT_DYNAMIC_MATRIX_EXTEND_CONST_BASE_VECTOR_1(owner, subOwner, elementType)

%extend vctDynamicConstMatrixBase<owner, elementType> {
    // matrix * vector
    inline vctDynamicVector<elementType> __mul__(const vctDynamicConstVectorBase<subOwner, elementType>& vector) throw(std::runtime_error) {
        vctDynamicVector<elementType> result((*self).rows());
        result.ProductOf(*self, vector);
        return result;   
    }

    // vector * matrix
    inline vctDynamicVector<elementType> __rmul__(const vctDynamicConstVectorBase<subOwner, elementType>& vector) throw(std::runtime_error) {
        vctDynamicVector<elementType> result((*self).cols());
        result.ProductOf(vector, *self);
        return result;   
    }
}
%enddef



/*
  Macro used to extend a base with different owner types with no sub owner
 */
%define VCT_DYNAMIC_MATRIX_EXTEND_BASE_0(owner, elementType)

%extend vctDynamicMatrixBase<owner, elementType> {
    // for row = m[i]
    inline vctDynamicVectorRef<elementType> __getitem__(index_type index) throw(std::out_of_range) {
        if (!(*self).ValidRowIndex(index)) {
            throw std::out_of_range("vctDynamicMatrix access out of range");
        }
        return vctDynamicVectorRef<elementType>((*self).Row(index));
    }
    // set random values
    inline void Random(elementType min, elementType max) {
        vctRandom(*self, min, max);
    }

    // We redefine Assign to pass to AssignMatrix if there is only one
    // parameter and it is not a numerical value.  Otherwise, perform
    // the assignment ourselves.
    %pythoncode {
        def Assign(self, *args):
            nbArgs = len(args)
            if (nbArgs == 1):
                arg0Type = type(args[0])
                if not ((arg0Type is types.IntType) or (arg0Type is types.FloatType)):
                    self.AssignMatrix(args[0])
                    return
            if nbArgs > self.size():
                raise RuntimeError, "Too many arguments compared to the matrix size"
            row = 0
            col = 0
            for index in xrange(0, nbArgs):
                self[row][col] = args[index]
                row += 1
                if row >= self.rows():
                    row = 0
                    col += 1
            }
}
%enddef



/*
  Macro used to extend a base with different owner types with one sub owner
 */
%define VCT_DYNAMIC_MATRIX_EXTEND_BASE_1(owner, subOwner, elementType)

%extend vctDynamicMatrixBase<owner, elementType> {

    inline void Assign(const vctDynamicConstMatrixBase<subOwner, elementType>& value)
        throw(std::runtime_error) {
        self->Assign(value);
    }

    // for self.op(other) 
    inline void Add(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        self->Add(other);
    }
    inline void Subtract(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        self->Subtract(other);
    }
    inline void ElementwiseMultiply(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        self->ElementwiseMultiply(other);
    }
    inline void ElementwiseDivide(const vctDynamicConstMatrixBase<subOwner, elementType>& other) throw(std::runtime_error) {
        self->ElementwiseDivide(other);
    }

    // methods for op(v, s)
    inline void SumOf(const vctDynamicConstMatrixBase<subOwner, elementType>& v,
                      const elementType & s) throw(std::runtime_error) {
        self->SumOf(v, s);
    }
    inline void DifferenceOf(const vctDynamicConstMatrixBase<subOwner, elementType>& v,
                             const elementType & s) throw(std::runtime_error) {
        self->DifferenceOf(v, s);
    }
    inline void ProductOf(const vctDynamicConstMatrixBase<subOwner, elementType>& v,
                          const elementType & s) throw(std::runtime_error) {
        self->ProductOf(v, s);
    }
    inline void RatioOf(const vctDynamicConstMatrixBase<subOwner, elementType>& v,
                        const elementType & s) throw(std::runtime_error) {
        self->RatioOf(v, s);
    }

    // methods for op(s, v)
    inline void SumOf(const elementType & s,
                      const vctDynamicConstMatrixBase<subOwner, elementType>& v) throw(std::runtime_error) {
        self->SumOf(s, v);
    }
    inline void DifferenceOf(const elementType & s,
                             const vctDynamicConstMatrixBase<subOwner, elementType>& v) throw(std::runtime_error) {
        self->DifferenceOf(s, v);
    }
    inline void ProductOf(const elementType & s,
                          const vctDynamicConstMatrixBase<subOwner, elementType>& v) throw(std::runtime_error) {
        self->ProductOf(s, v);
    }
    inline void RatioOf(const elementType & s,
                        const vctDynamicConstMatrixBase<subOwner, elementType>& v) throw(std::runtime_error) {
        self->RatioOf(s, v);
    }


    inline vctDynamicMatrix<elementType> __iadd__(const vctDynamicConstMatrixBase<subOwner, elementType>& value) throw(std::runtime_error) {
        self->Add(value);
        return *self;
    }
    inline vctDynamicMatrix<elementType> __isub__(const vctDynamicConstMatrixBase<subOwner, elementType>& value) throw(std::runtime_error) {
        self->Subtract(value);
        return *self;
    }
}
%enddef




/*
  Macro used to extend a base with different owner types with one sub owner for vector
 */
%define VCT_DYNAMIC_MATRIX_EXTEND_BASE_VECTOR_1(owner, subOwner, elementType)

%extend vctDynamicMatrixBase<owner, elementType> {
    // for m[i] = rowVector
    inline void __setitem__(index_type index, const vctDynamicConstVectorBase<subOwner, elementType>& value) throw(std::out_of_range) {
        if (!(*self).ValidRowIndex(index)) {
            throw std::out_of_range("vctDynamicMatrix access out of range");
        }
        (*self).Row(index) = value;
    }
}
%enddef




/*
  Macro used to extend a base with different owner types with two sub owners
 */
%define VCT_DYNAMIC_MATRIX_EXTEND_BASE_2(owner, subOwner1, subOwner2, elementType)

%extend vctDynamicMatrixBase<owner, elementType> {

    // methods for op(v, v)
    inline void SumOf(const vctDynamicConstMatrixBase<subOwner1, elementType>& m1,
                      const vctDynamicConstMatrixBase<subOwner2, elementType>& m2) throw(std::runtime_error) {
        self->SumOf(m1, m2);
    }
    inline void DifferenceOf(const vctDynamicConstMatrixBase<subOwner1, elementType>& m1,
                             const vctDynamicConstMatrixBase<subOwner2, elementType>& m2) throw(std::runtime_error) {
        self->DifferenceOf(m1, m2);
    }
    inline void ProductOf(const vctDynamicConstMatrixBase<subOwner1, elementType>& m1,
			  const vctDynamicConstMatrixBase<subOwner2, elementType>& m2) throw(std::runtime_error) {
        self->ProductOf(m1, m2);
    }
    inline void ElementwiseProductOf(const vctDynamicConstMatrixBase<subOwner1, elementType>& m1,
                                     const vctDynamicConstMatrixBase<subOwner2, elementType>& m2) throw(std::runtime_error) {
        self->ElementwiseProductOf(m1, m2);
    }
    inline void ElementwiseRatioOf(const vctDynamicConstMatrixBase<subOwner1, elementType>& m1,
                                   const vctDynamicConstMatrixBase<subOwner2, elementType>& m2) throw(std::runtime_error) {
        self->ElementwiseRatioOf(m1, m2);
    }
}
%enddef


/*
  Macro used to extend a Ref with different owner types with one sub owner
 */

%define VCT_DYNAMIC_MATRIX_EXTEND_REF_1(subOwner, elementType)

%extend vctDynamicMatrixRef<elementType> {

    vctDynamicMatrixRef<elementType>(vctDynamicMatrixBase<subOwner, elementType> & otherMatrix)
    {
        vctDynamicMatrixRef<elementType> * result =
            new vctDynamicMatrixRef<elementType>(otherMatrix);
        return result;
    }

    vctDynamicMatrixRef<elementType>(vctDynamicMatrixBase<subOwner, elementType> & otherMatrix,
                                     size_type startRow, size_type startCol, size_type rows,
                                     size_type cols) {
        vctDynamicMatrixRef<elementType> * result =
            new vctDynamicMatrixRef<elementType>(otherMatrix, startRow, startCol, rows, cols);
        return result;
    }

    inline void SetRef(vctDynamicMatrixBase<subOwner, elementType> & otherMatrix,
                       size_type startRow, size_type startCol, size_type rows,
                       size_type cols) {
        self->SetRef(otherMatrix, startRow, startCol, rows, cols);
    }
}
%enddef


/*
  Macro to instantiate everything for a dynamic matrix.  This is the
  main macro, which relies on the previously defined ones.
*/
%define VCT_DYNAMIC_MATRIX_INSTANTIATE(name, elementType)


%typemap(out) vctReturnDynamicMatrix<elementType> {
    vctDynamicMatrix<elementType> * resultptr;
    resultptr = new vctDynamicMatrix<elementType>((vctDynamicMatrix<elementType> &)(result));
    resultobj = SWIG_NewPointerObj((void *)(resultptr), SWIGTYPE_p_vctDynamicMatrixT##elementType##_t, 1);
}


// Instantiate owners first
%template() vctDynamicMatrixOwner<elementType>;
%template() vctDynamicMatrixRefOwner<elementType>;

// Instantiate const base for different owners
%template(##name##ConstBase) vctDynamicConstMatrixBase<vctDynamicMatrixOwner<elementType>, elementType>;
%template(##name##RefConstBase) vctDynamicConstMatrixBase<vctDynamicMatrixRefOwner<elementType>, elementType>;

// Extend const base for different owner types
VCT_DYNAMIC_MATRIX_EXTEND_CONST_BASE_0(vctDynamicMatrixOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_CONST_BASE_0(vctDynamicMatrixRefOwner<elementType>, elementType);

VCT_DYNAMIC_MATRIX_EXTEND_CONST_BASE_1(vctDynamicMatrixOwner<elementType>, vctDynamicMatrixOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_CONST_BASE_1(vctDynamicMatrixOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_CONST_BASE_1(vctDynamicMatrixRefOwner<elementType>, vctDynamicMatrixOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_CONST_BASE_1(vctDynamicMatrixRefOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, elementType);

VCT_DYNAMIC_MATRIX_EXTEND_CONST_BASE_VECTOR_1(vctDynamicMatrixOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType)
VCT_DYNAMIC_MATRIX_EXTEND_CONST_BASE_VECTOR_1(vctDynamicMatrixOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType)
VCT_DYNAMIC_MATRIX_EXTEND_CONST_BASE_VECTOR_1(vctDynamicMatrixRefOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType)
VCT_DYNAMIC_MATRIX_EXTEND_CONST_BASE_VECTOR_1(vctDynamicMatrixRefOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType)


// Redefine Assign to handle va_list in Python
%rename(AssignMatrix) vctDynamicMatrixBase<vctDynamicMatrixOwner<elementType>, elementType>::Assign;
%rename(AssignMatrix) vctDynamicMatrixBase<vctDynamicMatrixRefOwner<elementType>, elementType>::Assign;


// Instantiate base for different owners
%template(##name##Base) vctDynamicMatrixBase<vctDynamicMatrixOwner<elementType>, elementType>;
%template(##name##RefBase) vctDynamicMatrixBase<vctDynamicMatrixRefOwner<elementType>, elementType>;

// Extend base for different owner types
VCT_DYNAMIC_MATRIX_EXTEND_BASE_0(vctDynamicMatrixOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_BASE_0(vctDynamicMatrixRefOwner<elementType>, elementType);

VCT_DYNAMIC_MATRIX_EXTEND_BASE_1(vctDynamicMatrixOwner<elementType>, vctDynamicMatrixOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_BASE_1(vctDynamicMatrixOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_BASE_1(vctDynamicMatrixRefOwner<elementType>, vctDynamicMatrixOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_BASE_1(vctDynamicMatrixRefOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, elementType);

VCT_DYNAMIC_MATRIX_EXTEND_BASE_VECTOR_1(vctDynamicMatrixOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType)
VCT_DYNAMIC_MATRIX_EXTEND_BASE_VECTOR_1(vctDynamicMatrixOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType)
VCT_DYNAMIC_MATRIX_EXTEND_BASE_VECTOR_1(vctDynamicMatrixRefOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType)
VCT_DYNAMIC_MATRIX_EXTEND_BASE_VECTOR_1(vctDynamicMatrixRefOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType)

VCT_DYNAMIC_MATRIX_EXTEND_BASE_2(vctDynamicMatrixOwner<elementType>, vctDynamicMatrixOwner<elementType>, vctDynamicMatrixOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_BASE_2(vctDynamicMatrixOwner<elementType>, vctDynamicMatrixOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_BASE_2(vctDynamicMatrixOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, vctDynamicMatrixOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_BASE_2(vctDynamicMatrixOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_BASE_2(vctDynamicMatrixRefOwner<elementType>, vctDynamicMatrixOwner<elementType>, vctDynamicMatrixOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_BASE_2(vctDynamicMatrixRefOwner<elementType>, vctDynamicMatrixOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_BASE_2(vctDynamicMatrixRefOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, vctDynamicMatrixOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_BASE_2(vctDynamicMatrixRefOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, elementType);


// Instantiate matrix
%template(##name##) vctDynamicMatrix<elementType>;


// Instantiate matrix ref and const ref
%template(##name##ConstRef) vctDynamicConstMatrixRef<elementType>;
%template(##name##Ref) vctDynamicMatrixRef<elementType>;

// Extend matrix ref for different owner types :
VCT_DYNAMIC_MATRIX_EXTEND_REF_1(vctDynamicMatrixOwner<elementType>, elementType);
VCT_DYNAMIC_MATRIX_EXTEND_REF_1(vctDynamicMatrixRefOwner<elementType>, elementType);


// Type addition for dynamic type checking
%{
    typedef vctDynamicMatrix<elementType> name;
    typedef vctDynamicConstMatrixRef<elementType> ##name##ConstRef;
    typedef vctDynamicMatrixRef<elementType> ##name##Ref;
%}

typedef vctDynamicMatrix<elementType> name;
typedef vctDynamicConstMatrixRef<elementType> ##name##ConstRef;
typedef vctDynamicMatrixRef<elementType> ##name##Ref;

%types(name *);
%types(##name##ConstRef *);
%types(##name##Ref *);


%enddef


VCT_DYNAMIC_MATRIX_INSTANTIATE(vctDoubleMat, double);
VCT_DYNAMIC_MATRIX_INSTANTIATE(vctIntMat, int);
VCT_DYNAMIC_MATRIX_INSTANTIATE(vctShortMat, short);
VCT_DYNAMIC_MATRIX_INSTANTIATE(vctLongMat, long);



%pythoncode %{
# For type equivalence
vctMat = vctDoubleMat

# For all fixed size matrices
class vctFixedSizeMatrixDouble(vctDoubleMat):
    def SetSize(self, *args):
        raise RuntimeError, "Can't SetSize for a fixed size matrix"

    def resize(self, *args):
        raise RuntimeError, "Can't resize a fixed size matrix"

class vctFixedSizeMatrixInt(vctIntMat):
    def SetSize(self, *args):
        raise RuntimeError, "Can't SetSize for a fixed size matrix"

    def resize(self, *args):
        raise RuntimeError, "Can't resize a fixed size matrix"
%}

%define VCT_EMULATE_FIXED_SIZE_MATRIX(rows, cols)
%pythoncode %{
class vctDoubleMat##rows##cols##(vctFixedSizeMatrixDouble):
    def __init__(self, *args):
        vctDoubleMat.__init__(self, rows, cols)
        nbArgs = len(args)
        if nbArgs == 0:
            vctDoubleMat.SetAll(self, 0)
        elif nbArgs == 1:
            vctDoubleMat.SetAll(self, args[0])
        elif nbArgs == (rows * cols):
            for i in xrange(0, rows):
                vctDoubleMat.__setitem__(self, i, args[i])
        else:
            raise RuntimeError, str(self.__class__) + " requires either 0, 1 or size parameter(s)"

vct##rows##cols## = vctDoubleMat##rows##cols##

class vctIntMat##rows##cols##(vctFixedSizeMatrixInt):
    def __init__(self, *args):
        vctIntMat.__init__(self, rows, cols)
        nbArgs = len(args)
        if nbArgs == 0:
            vctIntMat.SetAll(self, 0)
        elif nbArgs == 1:
            vctIntMat.SetAll(self, args[0])
        elif nbArgs == (rows * cols):
            for i in xrange(0, rows):
                vctIntMat.__setitem__(self, i, args[i])
        else:
            raise RuntimeError, str(self.__class__) + " requires either 0, 1 or size parameter(s)"
%}
%enddef

VCT_EMULATE_FIXED_SIZE_MATRIX(2, 2)
VCT_EMULATE_FIXED_SIZE_MATRIX(3, 3)
VCT_EMULATE_FIXED_SIZE_MATRIX(4, 4)


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicMatrix.i,v $
//  Revision 1.11  2006/07/10 21:55:00  anton
//  vctDynamicMatrix.i: Fix for Assign() and added methods to compare matrices.
//
//  Revision 1.10  2006/06/13 01:13:33  anton
//  cisstVector wrapper: Updated wrappers for dynamic containers (bug in
//  matrix.Assign and used xrange for Assign loops).
//
//  Revision 1.9  2006/06/06 14:52:13  anton
//  vctDynamicMatrix.i: Mai update, compile and pass current tests.  Needs more
//  test cases.
//
//  Revision 1.8  2006/04/17 18:04:06  anton
//  vctDynamicMatrix.i: Added ctor and SetRef() for matrix ref.  Work in progress
//
//  Revision 1.7  2006/03/10 14:30:18  anton
//  cisstVector wrapping: Added throw exception for Assign method to handle size
//  mismatch.
//
//  Revision 1.6  2005/10/28 16:06:50  anton
//  cisstVector wrapping: Introduced extra type vctVec and vctMat as well as
//  emulation for vct2, vctDouble2 and vctInt2 (for sizes 2, 3, 4, 5 and 6).
//
//  Revision 1.5  2005/10/07 09:34:14  anton
//  cisstVector wrappers: Added AlmostEqual() for dynamic vector/matrix
//
//  Revision 1.4  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.3  2005/09/13 15:46:58  anton
//  cisstVector wrapping: Corrected bug for matrix * matrix.  Removed operators
//  __iadd__(scalar) since they were implemented twice.  Updated dynamic vectors
//  to maintain same macros/structure.
//
//  Revision 1.2  2005/08/19 05:32:54  anton
//  SWIG interface files:  Added the license.
//
//  Revision 1.1  2005/08/15 06:00:00  anton
//  cisstVector wrapping: Reorganized files per library.
//
//  Revision 1.5  2005/08/15 05:50:59  anton
//  vctDynamicMatrix.i: Added ProductOf(matrix, matrix), corrected size of
//  output for __mul__(matrix, matrix).
//
//  Revision 1.4  2005/07/19 19:21:00  anton
//  vctDynamicMatrix.i: Added operators for matrix * vector and vector * matrix.
//
//  Revision 1.3  2005/07/19 15:36:03  anton
//  vctDynamicMatrix.i: Added and tested support to get row, column and
//  diagonal access.
//
//  Revision 1.2  2005/07/14 21:42:20  anton
//  vctDynamicMatrix.i: Used macros similar to those used for vctDynamicVector.
//
//  Revision 1.1  2005/06/15 15:06:20  anton
//  Python wrapping: Split the cisstVector wrapping code in smaller files to
//  increase readability.
//
// ****************************************************************************
