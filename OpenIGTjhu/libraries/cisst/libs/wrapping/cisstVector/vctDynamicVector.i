/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicVector.i,v 1.16 2006/06/28 23:48:34 anton Exp $

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


// Vector specifics
%ignore vctDynamicVectorElementwiseCompareVector;
%ignore vctDynamicVectorElementwiseCompareScalar;


// The following should be supported in their const versions and
// ignored otherwise.
%ignore *::X;
%ignore *::Y;
%ignore *::Z;
%ignore *::W;
%ignore *::XY;
%ignore *::XYZ;
%ignore *::XYZW;
%ignore *::XZ;
%ignore *::XW;
%ignore *::YZ;
%ignore *::YW;
%ignore *::ZW;
%ignore *::YZW;


// SWIG va_args is somewhat limited, a Python implementation is more reliable
%ignore *::Assign(value_type element0, value_type element1, ...);
// We don't really want to handle pointers on values in Python
%ignore *::Assign(const value_type * elements);


// Include what needs to be wrapped
%include "cisstVector/vctDynamicVectorOwner.h"
%include "cisstVector/vctDynamicVectorRefOwner.h"
%include "cisstVector/vctDynamicConstVectorBase.h"
%include "cisstVector/vctDynamicVectorBase.h"
%include "cisstVector/vctDynamicVector.h"
%include "cisstVector/vctDynamicConstVectorRef.h"
%include "cisstVector/vctDynamicVectorRef.h"


// Python modules we use for code extended in Python
%pythoncode{
    import types 
%}


/*
  Macro used to extend a const base with different owner types, no sub owner
 */
%define VCT_DYNAMIC_VECTOR_EXTEND_CONST_BASE_0(owner, elementType)

%extend vctDynamicConstVectorBase<owner, elementType> {

    // for scalar = v[1]
    inline elementType __getitem__(index_type index) const throw(std::out_of_range) {
        return (*self).at(index);
    }
    
    // for v = v2[4:6]
    inline vctDynamicConstVectorRef<elementType> __getslice__(index_type first, index_type last) const
        throw(std::out_of_range) {
        return vctDynamicConstVectorRef<elementType>((*self), first, (last - first + 1));
    }

    // for vector = scalar <op> vector
    inline vctDynamicVector<elementType> __radd__(elementType scalar) throw(std::runtime_error) {
        vctDynamicVector<elementType> result(self->size());
        result.SumOf(scalar, *self);
        return result;
    }
    inline vctDynamicVector<elementType> __rsub__(elementType scalar) throw(std::runtime_error) {
        vctDynamicVector<elementType> result(self->size());
        result.DifferenceOf(scalar, *self);
        return result;
    }
    inline vctDynamicVector<elementType> __rmul__(elementType scalar) throw(std::runtime_error) {
        vctDynamicVector<elementType> result(self->size());
        result.ProductOf(scalar, *self);
        return result;
    }
    inline vctDynamicVector<elementType> __rdiv__(elementType scalar) throw(std::runtime_error) {
        vctDynamicVector<elementType> result(self->size());
        result.RatioOf(scalar, *self);
        return result;
    }

    // for vector = vector <op> scalar
    inline vctDynamicVector<elementType> __add__(elementType scalar) throw(std::runtime_error) {
        vctDynamicVector<elementType> result(self->size());
        result.SumOf(*self, scalar);
        return result;
    }
    inline vctDynamicVector<elementType> __sub__(elementType scalar) throw(std::runtime_error) {
        vctDynamicVector<elementType> result(self->size());
        result.DifferenceOf(*self, scalar);
        return result;
    }
    inline vctDynamicVector<elementType> __mul__(elementType scalar) throw(std::runtime_error) {
        vctDynamicVector<elementType> result(self->size());
        result.ProductOf(*self, scalar);
        return result;
    }
    inline vctDynamicVector<elementType> __div__(elementType scalar) throw(std::runtime_error) {
        vctDynamicVector<elementType> result(self->size());
        result.RatioOf(*self, scalar);
        return result;
    }
}

%enddef




/*
  Macro used to extend a const base with different owner types, one sub owner
 */
%define VCT_DYNAMIC_VECTOR_EXTEND_CONST_BASE_1(owner, subOwner, elementType)

%extend vctDynamicConstVectorBase<owner, elementType> {

    // for vector = vector <op> vector
    inline vctDynamicVector<elementType> __add__(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        vctDynamicVector<elementType> result(self->size());
        result.SumOf(*self, other);
        return result;
    }
    inline vctDynamicVector<elementType> __sub__(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        vctDynamicVector<elementType> result(self->size());
        result.DifferenceOf(*self, other);
        return result;
    }
    inline vctDynamicVector<elementType> __mod__(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        vctDynamicVector<elementType> result(self->size());
        result.CrossProductOf(*self, other);
        return result;
    }
    inline elementType __mul__(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        elementType result;
        result = self->DotProduct(other);
        return result;
    }
    inline elementType DotProduct(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        elementType result;
        result = self->DotProduct(other);
        return result;
    }

    // for vector <comp> vector
    inline bool __eq__(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return ((*self) == other);
    }
    inline bool __ne__(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return ((*self) != other);
    }
    inline bool AlmostEqual(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->AlmostEqual(other));
    }
    inline bool Equal(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->Equal(other));
    }
    inline bool Greater(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->Greater(other));
    }
    inline bool Lesser(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->Lesser(other));
    }
    inline bool GreaterOrEqual(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->GreaterOrEqual(other));
    }
    inline bool LesserOrEqual(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->LesserOrEqual(other));
    }
    inline bool NotEqual(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        return (self->NotEqual(other));
    }
}

%enddef




/*
  Macro used to extend a base with different owner types with no sub owner
 */
%define VCT_DYNAMIC_VECTOR_EXTEND_BASE_0(owner, elementType)

%extend vctDynamicVectorBase<owner, elementType> {
    // for scalar = v[1]
    inline elementType __getitem__(index_type index) throw(std::out_of_range) {
        return (*self).at(index);
    }
    // for v[1] = 4
    inline void __setitem__(index_type index, elementType value) throw(std::out_of_range) {
        (*self).at(index) = value;
    }
    // for v = v2[4:6]
    inline vctDynamicVectorRef<elementType> __getslice__(index_type first, index_type last)
        throw(std::out_of_range) {
        return vctDynamicVectorRef<elementType>((*self), first, (last - first + 1));
    }
    // set random values
    inline void Random(elementType min, elementType max) {
        vctRandom(*self, min, max);
    }

    // We redefine Assign to pass to AssignVector is there is only one
    // parameter and it is not a numerical value.  Otherwise, perform
    // the assignment ourselves.
    %pythoncode {
        def Assign(self, *args):
            nbArgs = len(args)
            if (nbArgs == 1):
                arg0Type = type(args[0])
                if not ((arg0Type is types.IntType) or (arg0Type is types.FloatType)):
                    self.AssignVector(args[0])
                    return
            if nbArgs > self.size():
                raise RuntimeError, "Too many arguments compared to the vector size"
            for index in xrange(0, nbArgs):
                self[index] = args[index]
    }

}

%enddef



/*
  Macro used to extend a base with different owner types with one sub owner
 */
%define VCT_DYNAMIC_VECTOR_EXTEND_BASE_1(owner, subOwner, elementType)

%extend vctDynamicVectorBase<owner, elementType> {

    inline void Assign(const vctDynamicConstVectorBase<subOwner, elementType>& value)
        throw(std::runtime_error) {
        self->Assign(value);
    }

    // for v[1:3] = v2
    inline void __setslice__(index_type first, index_type last, const vctDynamicConstVectorBase<subOwner, elementType>& value)
        throw(std::out_of_range) {
        vctDynamicVectorRef<elementType> sliceReference(*self, first, (last - first + 1));
        sliceReference.Assign(value);
    }

    // for self.op(other) 
    inline void Add(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        self->Add(other);
    }
    inline void Subtract(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        self->Subtract(other);
    }
    inline void ElementwiseMultiply(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        self->ElementwiseMultiply(other);
    }
    inline void ElementwiseDivide(const vctDynamicConstVectorBase<subOwner, elementType>& other) throw(std::runtime_error) {
        self->ElementwiseDivide(other);
    }

    // methods for op(v, s)
    inline void SumOf(const vctDynamicConstVectorBase<subOwner, elementType>& v,
                      const elementType & s) throw(std::runtime_error) {
        self->SumOf(v, s);
    }
    inline void DifferenceOf(const vctDynamicConstVectorBase<subOwner, elementType>& v,
                             const elementType & s) throw(std::runtime_error) {
        self->DifferenceOf(v, s);
    }
    inline void ProductOf(const vctDynamicConstVectorBase<subOwner, elementType>& v,
                          const elementType & s) throw(std::runtime_error) {
        self->ProductOf(v, s);
    }
    inline void RatioOf(const vctDynamicConstVectorBase<subOwner, elementType>& v,
                        const elementType & s) throw(std::runtime_error) {
        self->RatioOf(v, s);
    }

    // methods for op(s, v)
    inline void SumOf(const elementType & s,
                      const vctDynamicConstVectorBase<subOwner, elementType>& v) throw(std::runtime_error) {
        self->SumOf(s, v);
    }
    inline void DifferenceOf(const elementType & s,
                             const vctDynamicConstVectorBase<subOwner, elementType>& v) throw(std::runtime_error) {
        self->DifferenceOf(s, v);
    }
    inline void ProductOf(const elementType & s,
                          const vctDynamicConstVectorBase<subOwner, elementType>& v) throw(std::runtime_error) {
        self->ProductOf(s, v);
    }
    inline void RatioOf(const elementType & s,
                        const vctDynamicConstVectorBase<subOwner, elementType>& v) throw(std::runtime_error) {
        self->RatioOf(s, v);
    }

    inline vctDynamicVector<elementType> __iadd__(const vctDynamicConstVectorBase<subOwner, elementType>& value) throw(std::runtime_error) {
        self->Add(value);
        return *self;
    }
    inline vctDynamicVector<elementType> __isub__(const vctDynamicConstVectorBase<subOwner, elementType>& value) throw(std::runtime_error) {
        self->Subtract(value);
        return *self;
    }
}
%enddef


/*
  Macro used to extend a base with different owner types with two sub owners
 */
%define VCT_DYNAMIC_VECTOR_EXTEND_BASE_2(owner, subOwner1, subOwner2, elementType)

%extend vctDynamicVectorBase<owner, elementType> {

    // methods for op(v, v)
    inline void SumOf(const vctDynamicConstVectorBase<subOwner1, elementType>& v1,
                      const vctDynamicConstVectorBase<subOwner2, elementType>& v2) throw(std::runtime_error) {
        self->SumOf(v1, v2);
    }
    inline void DifferenceOf(const vctDynamicConstVectorBase<subOwner1, elementType>& v1,
                             const vctDynamicConstVectorBase<subOwner2, elementType>& v2) throw(std::runtime_error) {
        self->DifferenceOf(v1, v2);
    }
    inline void ElementwiseProductOf(const vctDynamicConstVectorBase<subOwner1, elementType>& v1,
                                     const vctDynamicConstVectorBase<subOwner2, elementType>& v2) throw(std::runtime_error) {
        self->ElementwiseProductOf(v1, v2);
    }
    inline void ElementwiseRatioOf(const vctDynamicConstVectorBase<subOwner1, elementType>& v1,
                                   const vctDynamicConstVectorBase<subOwner2, elementType>& v2) throw(std::runtime_error) {
        self->ElementwiseRatioOf(v1, v2);
    }
    inline void CrossProductOf(const vctDynamicConstVectorBase<subOwner1, elementType>& v1,
                               const vctDynamicConstVectorBase<subOwner2, elementType>& v2) throw(std::runtime_error) {
        self->CrossProductOf(v1, v2);
    }
}
%enddef



/*
  Macro used to extend a base with different owner types with different owners for vector and matrix input
 */
%define VCT_DYNAMIC_VECTOR_EXTEND_BASE_11(owner, subOwnerMatrix, subOwnerVector, elementType)

%extend vctDynamicVectorBase<owner, elementType> {
    inline void ProductOf(const vctDynamicConstMatrixBase<subOwnerMatrix, elementType>& m,
                          const vctDynamicConstVectorBase<subOwnerVector, elementType>& v) throw(std::runtime_error) {
        self->ProductOf(m, v);
    }
    inline void ProductOf(const vctDynamicConstVectorBase<subOwnerVector, elementType>& v,
                          const vctDynamicConstMatrixBase<subOwnerMatrix, elementType>& m) throw(std::runtime_error) {
        self->ProductOf(v, m);
    }
}
%enddef




/*
  Macro to instantiate everything for a dynamic vector.  This is the
  main macro, which relies on the previously defined ones.
*/
%define VCT_DYNAMIC_VECTOR_INSTANTIATE(name, elementType)


%typemap(out) vctReturnDynamicVector<elementType> {
    vctDynamicVector<elementType> * resultptr;
    resultptr = new vctDynamicVector<elementType>((vctDynamicVector<elementType> &)(result));
    resultobj = SWIG_NewPointerObj((void *)(resultptr), SWIGTYPE_p_vctDynamicVectorT##elementType##_t, 1);
}


// Instantiate owners first
%template() vctDynamicVectorOwner<elementType>;
%template() vctDynamicVectorRefOwner<elementType>;

// Instantiate const base for different owners
%template(##name##ConstBase) vctDynamicConstVectorBase<vctDynamicVectorOwner<elementType>, elementType>;
%template(##name##RefConstBase) vctDynamicConstVectorBase<vctDynamicVectorRefOwner<elementType>, elementType>;

// Extend const base for different owner types
VCT_DYNAMIC_VECTOR_EXTEND_CONST_BASE_0(vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_CONST_BASE_0(vctDynamicVectorRefOwner<elementType>, elementType);

VCT_DYNAMIC_VECTOR_EXTEND_CONST_BASE_1(vctDynamicVectorOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_CONST_BASE_1(vctDynamicVectorOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_CONST_BASE_1(vctDynamicVectorRefOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_CONST_BASE_1(vctDynamicVectorRefOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType);

// Redefine Assign to handle va_list in Python
%rename(AssignVector) vctDynamicVectorBase<vctDynamicVectorOwner<elementType>, elementType>::Assign;
%rename(AssignVector) vctDynamicVectorBase<vctDynamicVectorRefOwner<elementType>, elementType>::Assign;

// Instantiate base for different owners
%template(##name##Base) vctDynamicVectorBase<vctDynamicVectorOwner<elementType>, elementType>;
%template(##name##RefBase) vctDynamicVectorBase<vctDynamicVectorRefOwner<elementType>, elementType>;

// Extend base for different owner types
VCT_DYNAMIC_VECTOR_EXTEND_BASE_0(vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_0(vctDynamicVectorRefOwner<elementType>, elementType);

VCT_DYNAMIC_VECTOR_EXTEND_BASE_1(vctDynamicVectorOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_1(vctDynamicVectorOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_1(vctDynamicVectorRefOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_1(vctDynamicVectorRefOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType);

VCT_DYNAMIC_VECTOR_EXTEND_BASE_2(vctDynamicVectorOwner<elementType>, vctDynamicVectorOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_2(vctDynamicVectorOwner<elementType>, vctDynamicVectorOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_2(vctDynamicVectorOwner<elementType>, vctDynamicVectorRefOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_2(vctDynamicVectorOwner<elementType>, vctDynamicVectorRefOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_2(vctDynamicVectorRefOwner<elementType>, vctDynamicVectorOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_2(vctDynamicVectorRefOwner<elementType>, vctDynamicVectorOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_2(vctDynamicVectorRefOwner<elementType>, vctDynamicVectorRefOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_2(vctDynamicVectorRefOwner<elementType>, vctDynamicVectorRefOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType);

VCT_DYNAMIC_VECTOR_EXTEND_BASE_11(vctDynamicVectorOwner<elementType>, vctDynamicMatrixOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_11(vctDynamicVectorOwner<elementType>, vctDynamicMatrixOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_11(vctDynamicVectorOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_11(vctDynamicVectorOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_11(vctDynamicVectorRefOwner<elementType>, vctDynamicMatrixOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_11(vctDynamicVectorRefOwner<elementType>, vctDynamicMatrixOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_11(vctDynamicVectorRefOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, vctDynamicVectorOwner<elementType>, elementType);
VCT_DYNAMIC_VECTOR_EXTEND_BASE_11(vctDynamicVectorRefOwner<elementType>, vctDynamicMatrixRefOwner<elementType>, vctDynamicVectorRefOwner<elementType>, elementType);

// Instantiate vector
%template(##name##) vctDynamicVector<elementType>;


// Instantiate vector ref and const ref
%template(##name##ConstRef) vctDynamicConstVectorRef<elementType>;
%template(##name##Ref) vctDynamicVectorRef<elementType>;


// Type addition for dynamic type checking
%{
    typedef vctDynamicVector<elementType> name;
    typedef vctDynamicConstVectorRef<elementType> ##name##ConstRef;
    typedef vctDynamicVectorRef<elementType> ##name##Ref;
%}

typedef vctDynamicVector<elementType> name;
typedef vctDynamicConstVectorRef<elementType> ##name##ConstRef;
typedef vctDynamicVectorRef<elementType> ##name##Ref;

%types(name *);
%types(##name##ConstRef *);
%types(##name##Ref *);


%enddef



// The very high level macros
VCT_DYNAMIC_VECTOR_INSTANTIATE(vctDoubleVec, double);
VCT_DYNAMIC_VECTOR_INSTANTIATE(vctIntVec, int);
VCT_DYNAMIC_VECTOR_INSTANTIATE(vctShortVec, short);
VCT_DYNAMIC_VECTOR_INSTANTIATE(vctLongVec, long);
VCT_DYNAMIC_VECTOR_INSTANTIATE(vctBoolVec, bool);


%pythoncode %{
# For type equivalence
vctVec = vctDoubleVec

# For all fixed size vectors
class vctFixedSizeVectorDouble(vctDoubleVec):
    def SetSize(self, *args):
        raise RuntimeError, "Can't SetSize for a fixed size vector"

    def resize(self, *args):
        raise RuntimeError, "Can't resize a fixed size vector"

class vctFixedSizeVectorInt(vctIntVec):
    def SetSize(self, *args):
        raise RuntimeError, "Can't SetSize for a fixed size vector"

    def resize(self, *args):
        raise RuntimeError, "Can't resize a fixed size vector"
%}


%define VCT_EMULATE_FIXED_SIZE_VECTOR(size)
%pythoncode %{
class vctDouble##size##(vctFixedSizeVectorDouble):
    def __init__(self, *args):
        vctDoubleVec.__init__(self, size)
        nbArgs = len(args)
        if nbArgs == 0:
            vctDoubleVec.SetAll(self, 0)
        elif nbArgs == 1:
            vctDoubleVec.SetAll(self, args[0])
        elif nbArgs == size:
            for i in xrange(0, size):
                vctDoubleVec.__setitem__(self, i, args[i])
        else:
            raise RuntimeError, str(self.__class__) + " requires either 0, 1 or size parameter(s)"

vct##size## = vctDouble##size##

class vctInt##size##(vctFixedSizeVectorInt):
    def __init__(self, *args):
        vctIntVec.__init__(self, size)
        nbArgs = len(args)
        if nbArgs == 0:
            vctIntVec.SetAll(self, 0)
        elif nbArgs == 1:
            vctIntVec.SetAll(self, args[0])
        elif nbArgs == size:
            for i in xrange(0, size):
                vctIntVec.__setitem__(self, i, args[i])
        else:
            raise RuntimeError, str(self.__class__) + " requires either 0, 1 or size parameter(s)"
%}
%enddef

VCT_EMULATE_FIXED_SIZE_VECTOR(2)
VCT_EMULATE_FIXED_SIZE_VECTOR(3)
VCT_EMULATE_FIXED_SIZE_VECTOR(4)
VCT_EMULATE_FIXED_SIZE_VECTOR(5)
VCT_EMULATE_FIXED_SIZE_VECTOR(6)


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicVector.i,v $
//  Revision 1.16  2006/06/28 23:48:34  anton
//  cisstVector wrappers: Added code and tests for dynamic vectors.
//
//  Revision 1.15  2006/06/13 01:13:33  anton
//  cisstVector wrapper: Updated wrappers for dynamic containers (bug in
//  matrix.Assign and used xrange for Assign loops).
//
//  Revision 1.14  2006/05/10 00:41:39  kapoor
//  cisstDeviceInterface: Added support for ddiVecShort and ddiVecLong.
//
//  Revision 1.13  2006/05/04 23:29:50  kapoor
//  cisst wrappers: Addition of bool type to ddiVec*
//
//  Revision 1.12  2006/04/17 18:03:25  anton
//  vctDynamicVector.i: Added __mod__ for operator %, i.e. cross product.
//
//  Revision 1.11  2006/03/16 03:37:45  anton
//  cisstVector wrappers: Added support for CrossProductOf(), DotProductOf() and
//  ProductOf(mat, vec)/ProductOf(vec, mat) with corresponding tests.
//
//  Revision 1.10  2006/03/14 22:31:07  anton
//  cisstVector wrappers: Corrected bug with Assign() method and va_list using
//  some Python code.  Added tests for vctDynamicVector.
//
//  Revision 1.9  2006/03/10 14:30:18  anton
//  cisstVector wrapping: Added throw exception for Assign method to handle size
//  mismatch.
//
//  Revision 1.8  2005/10/30 02:50:42  anton
//  vctDynamicVector.i: Error for exception message in __init__ in macro
//  VCT_EMULATE_FIXED_SIZE_VECTOR (was hard coded to print vctDouble3).
//
//  Revision 1.7  2005/10/28 21:44:53  anton
//  vctDynamicVector.i: Incorrect Python redefinition of SetSize and resize
//  for emulated fixed size vectors.
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
//  Revision 1.3  2005/07/20 18:24:57  anton
//  cisstVector: Changed vctDynamic{Const,}VectorRef to throw an exception
//  if the indices of the reference are out_of_range with respect to the
//  referenced vector.  This is safer than "assert" for Python slices.
//
//  Revision 1.2  2005/07/19 15:35:02  anton
//  vctDynamicVector.i: Minor clean-up.
//
//  Revision 1.1  2005/06/15 15:06:20  anton
//  Python wrapping: Split the cisstVector wrapping code in smaller files to
//  increase readability.
//
// ****************************************************************************
