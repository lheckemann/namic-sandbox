/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicConstVectorRef.h,v 1.16 2005/09/26 15:41:47 anton Exp $

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


#ifndef _vctDynamicConstVectorRef_h
#define _vctDynamicConstVectorRef_h

/*!
  \file
  \brief Declaration of vctDynamicConstVectorRef
*/


#include <cisstVector/vctDynamicConstVectorBase.h>
#include <cisstVector/vctDynamicVectorRefOwner.h>


/*! 
  \ingroup cisstVector
  
  This is a concrete vector, equivalent to vctFixedSizeConstVectorRef:
  - Does not own memory
  - Stride is a parameter
  - Does not allocate memory
  - Does not reallocate memory
  - Allows re-referencing and changing the size
      A single method (e.g. SetReference) for both
  - Only const operations
      Stores a non-const <code>value_type *</code> but only allows const access
  - Initialized with either <code>value_type *</code> or <code>const value_type *</code>
  - Optional:  Have a version with stride equals 1, and another version with stride as a parameter
*/
template<class _elementType>
class vctDynamicConstVectorRef :
    public vctDynamicConstVectorBase<vctDynamicVectorRefOwner<_elementType>, _elementType>
{
public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctDynamicConstVectorRef<value_type> ThisType;
    typedef vctDynamicVectorRefOwner<value_type> VectorOwnerType;
    typedef vctDynamicConstVectorBase<vctDynamicVectorRefOwner<value_type>, _elementType> BaseType;
    typedef typename BaseType::CopyType CopyType;
    typedef typename VectorOwnerType::iterator iterator;
    typedef typename VectorOwnerType::const_iterator const_iterator;
    typedef typename VectorOwnerType::reverse_iterator reverse_iterator;
    typedef typename VectorOwnerType::const_reverse_iterator const_reverse_iterator;
    
    /*! Default constructor initializes the vector reference to NULL */
    vctDynamicConstVectorRef()
    {}

    vctDynamicConstVectorRef(size_type size, pointer data, stride_type stride = 1) {
        SetRef(size, data, stride);
    }

    vctDynamicConstVectorRef(size_type size, const_pointer data, stride_type stride = 1) {
        SetRef(size, data, stride);
    }

    /*! Initialize a dynamic reference to a fixed-size vector.
      \note This constructor is non-explicit, and allows automatic conversion
      from a fixed-size vector to a dynamic vector representation.
      \note The size and stride values are taken from the fixed size vector.
    */
    template<unsigned int __size, int __stride, class __dataPtrType>
    vctDynamicConstVectorRef(const vctFixedSizeConstVectorBase<__size, __stride, _elementType, __dataPtrType> & otherVector)
    {
        SetRef(otherVector);
    }

    /*! Initialize a dynamic reference to specified subvector of a fixed-size vector.
      \note This constructor allows automatic conversion
      from a fixed-size vector to a dynamic vector representation.
      \note The stride values are taken from the fixed size vector, but the starting point and 
      length must be specified.
    */
    template<unsigned int __size, int __stride, class __dataPtrType>
    vctDynamicConstVectorRef(const vctFixedSizeConstVectorBase<__size, __stride, _elementType, __dataPtrType> & otherVector,
                             size_type startPosition, size_type length)
    {
        SetRef(otherVector, startPosition, length);
    }

    /*! Initialize a dynamic reference to a dynamic vector.
      \note the starting point, size, and stride, are taken from the other vector.
    */
    template<class __vectorOwnerType>
    vctDynamicConstVectorRef(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector)
    {
        SetRef(otherVector);
    }

    /*! Initialize a dynamic reference to a specified subvector of a dynamic vector.
      \note the stride is taken from the other vector, but the starting point and the
      length must be specified.
    */
    template<class __vectorOwnerType>
        vctDynamicConstVectorRef(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector,
        size_type startPosition, size_type length)
    {
        SetRef(otherVector, startPosition, length);
    }

    void SetRef(size_type size, const_pointer p, stride_type stride = 1) {
        this->Vector.SetRef(size, const_cast<pointer>(p), stride);
    }

    /*! Set a dynamic reference to a fixed-size vector
      \note the size and memory stride of this reference will be equal to the
      size and memory stride of the input vector.
    */
    template<unsigned int __size, int __stride, class __dataPtrType>
    void SetRef(const vctFixedSizeConstVectorBase<__size, __stride, _elementType, __dataPtrType> & otherVector)
    {
        SetRef(otherVector.size(), otherVector.Pointer(), otherVector.stride());
    }

    /*! Set a dynamic reference to a specified subvector of a fixed-size vector
      \note the memory stride of this reference will be equal to the
      memory stride of the input vector.
    */
    template<unsigned int __size, int __stride, class __dataPtrType>
    void SetRef(const vctFixedSizeConstVectorBase<__size, __stride, _elementType, __dataPtrType> & otherVector,
                size_type startPosition, size_type length) throw(std::out_of_range)
    {
        if (startPosition + length > otherVector.size()) {
            cmnThrow(std::out_of_range("vctDynamicConstVectorRef access out of range"));
        }
        SetRef(length, otherVector.Pointer(startPosition), otherVector.stride());
    }

    /*! Set a dynamic reference to a dynamic vector
      \note the size and memory stride of this reference will be equal to the
      size memory stride of the input vector.
    */
    template<class __vectorOwnerType>
    void SetRef(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector)
    {
        SetRef(otherVector.size(), otherVector.Pointer(), otherVector.stride());
    }

    /*! Set a dynamic reference to a specified subvector of dynamic vector
      \note the memory stride of this reference will be equal to the
      memory stride of the input vector.
    */
    template<class __vectorOwnerType>
    void SetRef(const vctDynamicConstVectorBase<__vectorOwnerType, _elementType> & otherVector,
                size_type startPosition, size_type length) throw(std::out_of_range)
    {
        if (startPosition + length > otherVector.size()) {
            cmnThrow(std::out_of_range("vctDynamicConstVectorRef access out of range"));
        }
        SetRef(length, otherVector.Pointer(startPosition), otherVector.stride());
    }

};


#endif // _vctDynamicConstVectorRef_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicConstVectorRef.h,v $
//  Revision 1.16  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.15  2005/09/24 00:01:04  anton
//  cisstVector: Use cmnThrow for all exceptions.
//
//  Revision 1.14  2005/07/20 18:24:57  anton
//  cisstVector: Changed vctDynamic{Const,}VectorRef to throw an exception
//  if the indices of the reference are out_of_range with respect to the
//  referenced vector.  This is safer than "assert" for Python slices.
//
//  Revision 1.13  2005/06/16 03:38:29  anton
//  Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
//  VC 7.1 (.Net 2003).
//
//  Revision 1.12  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.11  2005/04/04 19:48:34  anton
//  cisstVector: In base types for dynamic containers, removed default value for
//  _elementType (was based on _ownerType::value_type) to -1- avoid issues with
//  SWIG and -2- force explicit declaration of _elementType whenever a base type
//  is used.
//
//  Revision 1.10  2005/01/18 19:17:52  ofri
//  Removing the *explicit* qualifier from constructor definitions.
//  See ticket #123.
//
//  Revision 1.9  2005/01/06 18:54:00  anton
//  cisstVector: Introduction of type CopyType as requested in ticket #113.
//
//  Revision 1.8  2005/01/03 23:26:50  ofri
//  vctDynamic[Const]VectorRef: Updated the interfaces of constructors and SetRef()
//  following ticket #107.  Tests and example program compile and run successfully
//  on linux.
//
//  Revision 1.7  2004/11/11 20:35:46  anton
//  cisstVector: *: Added a vctContainerTraits to centralize the declarations
//  of size_type, difference_type, reference, const_reference, etc. *: All
//  iterators are now derived from std::iterator. *: Added some missing typedef
//  for iterators.
//
//  Revision 1.6  2004/10/26 15:20:52  ofri
//  Updating subsequence interfaces (ticket #76) and interfaces between fixed
//  and dynamic vectors/matrices (ticket #72).
//  *: vctFixedSize(Const)VectorBase::Get(Const)Subsequence now deprecated.
//  Replaced by Get(Const)Subvector with equal stride to the parent.
//  *: Added constructors for [Fixed|Dynamic][Vector|Matrix]Ref from the other
//  storage type.
//
//  Revision 1.5  2004/10/25 19:19:54  anton
//  cisstVector:  Created vctForwardDeclarations.h and removed forward
//  declarations of classes in all other files.  Added some constructors
//  for vector references to reference fixed size from dynamic and vice versa.
//
//  Revision 1.4  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.3  2004/10/14 19:23:27  ofri
//  vctDynamicConstVectorRef and vctDynamicVectorRef: Added constructors
//  from other vector types, and assignment operator from a dynamic vector.  See
//  ticket #72.
//
//  Revision 1.2  2004/08/13 21:40:40  anton
//  cisstVector: Added a bound check for all vectors (fixed size and dynamic)
//  as requested in ticket #13.  Added protect RangeCheck method to facilitate
//  (inspired from the STL distributed with gcc).  Added some tests in
//  vctGenericVectorTests.  Still have to do the equivalent for matrices.
//
//  Revision 1.1  2004/08/13 17:47:40  anton
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
//  Revision 1.2  2004/07/06 19:42:50  ofri
//  Giving the dynamic vector classes "standard look and feel" in terms of type
//  members, constructors, and indentations.  I made sure that the corresponding
//  example compiles and runs successfully.
//
//  Revision 1.1  2004/07/02 16:16:45  anton
//  Massive renaming in cisstVector for the addition of dynamic size vectors
//  and matrices.  The dynamic vectors are far from ready.
//
//
// ****************************************************************************
