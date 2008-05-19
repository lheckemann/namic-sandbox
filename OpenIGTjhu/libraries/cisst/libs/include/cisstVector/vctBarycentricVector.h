/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctBarycentricVector.h,v 1.13 2005/09/26 15:41:47 anton Exp $
  
  Author(s):  Ofri Sadowsky
  Created on:  12/2/2003

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


#ifndef _vctBarycentricVector_h
#define _vctBarycentricVector_h

/*!
  \file
  \brief Defines vctBarycentricVector
*/


#include <cisstVector/vctFixedSizeVector.h>
#include <cisstCommon/cmnTypeTraits.h>

/*!
  \ingroup cisstVector

  class vctBarycentricVector is derived from vctFixedSizeVector and
  has the same basic functionality.  It contains a few additional
  functions meant to simplify dealing with vectors of barycentric
  coordinates.  For our general definition, a set of coordinates is
  barycentric if it sums to 1.  When a barycentric vector is used to
  parametrize a simplex geometrical object, all interior points have
  all-positive coordinates.  All member points (including the
  boundary) have non-negative coordinates.  A boundary point has one
  or more of its coordinates zero.  A vertex has one coordinate equal
  to 1, and the others zero.

  I have decided to derive this class from vctFixedSizeVector, which
  means that the operations can only be performed on locally owned
  memory (as opposed to a referenced vector).  The reason was that it
  would be too hard to regenerate this set of operations as a subclass
  for each vector type.  In addition, most of the operations here are
  const methods, and in the case of computing a new set of
  coordinates, they return it by value.  Therefore,
  vctBarycentricVector is safer to use than other library objects, but
  on the other hand the operations may be somewhat slower.

  I did not provide the full set of constructors as I have with
  vctFixedSizeVector, and therefore the use of a vctBarycentricVector
  should be more explicit in terms of type conversions than
  vctFixedSizeVector.

  Many operations on vctBarycentricVector require a tolerance
  argument, which is given with a default value.  The tolerance
  argument must be non-negative, or else the result is undefined. The
  current implementation does not test for it, but assumes it.  In
  most cases, we compare absolute values of numbers to the tolerance.
 */
template<class _elementType, unsigned int _size>
class vctBarycentricVector : public vctFixedSizeVector<_elementType, _size>
{
public:
    typedef vctFixedSizeVector<_elementType, _size> BaseType;
    typedef vctBarycentricVector<_elementType, _size> ThisType;

    typedef typename BaseType::value_type value_type;
    typedef class cmnTypeTraits<value_type> TypeTraits;

    /*! Default constructr -- call base class def. ctor */
    vctBarycentricVector()
        : BaseType()
    {}

    /*! copy constructor for vctBarycentricVector object */
    vctBarycentricVector(const ThisType & other)
        : BaseType( static_cast<const BaseType &>(other) )
    {}

    /*! copy constructor for vctFixedSizeVector object */
    vctBarycentricVector(const BaseType & other)
        : BaseType(other)
    {}

    /*! copy constructor from a general vector */
    template<int __stride, class __dataPtrType>
    vctBarycentricVector(const vctFixedSizeVectorBase<_size, __stride, _elementType, __dataPtrType> & other)
    : BaseType(other)
    {}

    vctBarycentricVector(_elementType value)
        : BaseType(value)
    {}

    vctBarycentricVector(_elementType element0, _elementType element1)
        : BaseType(element0, element1)
    {}

    vctBarycentricVector(_elementType element0, _elementType element1, _elementType element2)
        : BaseType(element0, element1, element2)
    {}

    vctBarycentricVector(_elementType element0, _elementType element1, _elementType element2,
        _elementType element3)
        : BaseType(element0, element1, element2, element3)
    {}

    /* This one cannot call BaseType constructor as we cannot identify the
     unknown arguments */
    vctBarycentricVector(_elementType element0, _elementType element1, _elementType element2,
        _elementType element3, _elementType element4, ...)
    {
        (*this)[0] = element0;
        (*this)[1] = element1;
        (*this)[2] = element2;
        (*this)[3] = element3;
        (*this)[4] = element4;
        va_list nextArg;
        va_start(nextArg, element4);
        for (unsigned int i = 5; i < _size; ++i) {
            (*this)[i] = va_arg(nextArg, value_type);
        }
        va_end(nextArg);
    }

    /*! return true iff the sum of elements of this vector is
      equal to 1 up to the given tolerance. See class documentation. */
    bool IsBarycentric(const _elementType tolerance = TypeTraits::Tolerance() ) const
    {
        const _elementType diff = this->SumOfElements() - 1;
        return ( (-tolerance <= diff) && (diff <= tolerance) );
    }

    /*! return true iff all the coordinates are greater than or
      equal to the given tolerance. See class documentation. */
    bool IsInterior(const _elementType tolerance = TypeTraits::Tolerance()) const
    {
        if (!IsBarycentric())
            return false;
        const bool result = ((*this).GreaterOrEqual(tolerance));
        return result;
    }

    /*! return true iff all the coodinates are greater than 
      -tolerance. See class documentation. */
    bool IsMember(const _elementType tolerance = TypeTraits::Tolerance()) const
    {
        if (!IsBarycentric())
            return false;
        const bool result = ((*this).GreaterOrEqual(-tolerance));
        return result;
    }

    /*! return true iff one of the coodinates is zero, up to
      the given tolerance.  This is not a membership test. */
    bool HasZero(const _elementType tolerance = TypeTraits::Tolerance()) const
    {
        return
            vctFixedSizeVectorRecursiveEngines<_size>::template
            SoViSi< typename vctBinaryOperations<bool>::Or, 
            typename vctBinaryOperations<bool, value_type, value_type>::Bound>::
            Unfold( (*this), tolerance );
    }

    /*! return (IsMember(tolerance) && HasZero(tolerance)); */
    bool IsBoundary(const _elementType tolerance = TypeTraits::Tolerance()) const
    {
        return (IsMember(tolerance) && HasZero(tolerance));
    }

    /*! return true iff a the vector is a member and a coordinate is equal to 
      1 up to a given tolerance */
    bool IsVertex(const _elementType tolerance = TypeTraits::Tolerance()) const
    {
        if (!IsMember(tolerance))
            return false;
        const ThisType diff((*this) - _elementType(1));
        return (diff.HasZero(tolerance));
    }

    /*! return a barycentric scaled version of this vector, that is,
      a vector whose sum of elements is 1.  If the sum of this vector
      is zero up to the given tolerance, return a zero vector (which is
      not barycentric).
     */
    ThisType ScaleToBarycentric(const _elementType tolerance = TypeTraits::Tolerance()) const
    {
        _elementType scale = this->SumOfElements();
        if ( (-tolerance <= scale) && (scale <= tolerance) )
            return ThisType(0);
        ThisType result(*this);
        result /= scale;
        return result;
    }

};


#endif // _vctBarycentricVector_h
// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctBarycentricVector.h,v $
// Revision 1.13  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.12  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.11  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.10  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.9  2004/09/24 20:20:42  anton
// cisstVector: Major update related to ticket #18 regarding order operators
// and methods.  The methods named Equal(), NotEqual(), Lesser(),
// LesserOrEqual(), etc.  return a boolean.  The same names with prefix
// Elementwise (e.g. ElementwiseEqual) return a vector or matrix of the tests
// performed for each element.  We only kept the operators == and != which
// return a boolean.  All other operators (>, >=, <, <=) have been removed.
// Also, in order to have the same API for all the containers, many methods
// have been added (vctAny, vctAll, etc... see tickets #52 and #61).  The
// doxygen comments have also been updated.
//
// Revision 1.8  2004/08/13 17:47:40  anton
// cisstVector: Massive renaming to replace the word "sequence" by "vector"
// (see ticket #50) as well as another more systematic naming for the engines.
// The changes for the API are as follow:
// *: vctFixedLengthSequenceBase -> vctFixedSizeVectorBase (and Const)
// *: vctFixedLengthSequenceRef -> vctFixedSizeVectorRef (and Const)
// *: vctDynamicSequenceBase -> vctDynamicVectorBase (and Const)
// *: vctDynamicSequenceRef -> vctDynamicVectorRef (and Const)
// *: vctDynamicSequenceRefOwner -> vctDynamicVectorRefOwner
// *: vctFixedStrideSequenceIterator -> vctFixedStrideVectorIterator (and Const)
// *: vctVarStrideSequenceIterator -> vctVarStrideVectorIterator (and Const)
// *: vctSequenceRecursiveEngines -> vctFixedSizeVectorRecursiveEngines
// *: vctSequenceLoopEngines -> vctDynamicVectorLoopEngines
// *: vctMatrixLoopEngines -> vctFixedSizeMatrixLoopEngines
// *: vctDynamicMatrixEngines -> vctDynamicMatrixLoopEngines
// Also updated and corrected the documentation (latex, doxygen, figures) as
// well as the tests and examples.
//
// Revision 1.7  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.6  2004/03/19 17:01:17  ofri
// Redefined local TypeTraits using keyword "class" instead of "typename".
// Defined local ElementVaArgPromotion type.
//
// Revision 1.5  2004/03/18 19:30:09  ofri
// Correct definition of local TypeTraits enables to use the tolerance from
// cmnTypeTraits
//
// Revision 1.4  2004/03/16 21:21:23  ofri
// Setting the default tolerance of operations on vctBarycentryVector to 1.0e-6.
// This is a temporary hack and needs to be resolved with cmnTypeTraits somehow
//
// Revision 1.3  2003/12/18 22:23:44  ofri
// Adapting vctBarycentricVector to the new order of template arguments
// for binary operations.
//
// Revision 1.2  2003/12/17 15:44:31  ofri
// Now using type inference in recursive engines (see recent changes in
// vctSequenceRecursiveEngines.h)
//
// Revision 1.1  2003/12/08 02:45:56  ofri
// Added classes vctBarycentricVector and vctConstants to cisstVector .
// This commit will be subject to further inspection.
//
//
// ****************************************************************************
