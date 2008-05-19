/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeVectorRef.h,v 1.14 2005/12/02 16:23:54 anton Exp $
  
  Author(s):  Ofri Sadowsky, Anton Deguet
  Created on:  2003-09-30

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
  \brief Declaration of vctFixedSizeVectorRef
 */


#ifndef _vctFixedSizeVectorRef_h
#define _vctFixedSizeVectorRef_h


#include <cisstVector/vctFixedSizeConstVectorRef.h>


/*!  \brief An implementation of the ``abstract''
  vctFixedSizeVectorBase.

  \ingroup cisstVector

  This implementations uses a pointer to the vector beginning as the
  vector defining data member.  An instantiation of this type can be
  used as a subsequence with GetSubsequence().
 
  See the base class (vctFixedSizeVectorBase) for template
  parameter details.

  \sa vctFixedSizeConstVectorRef
*/
template<class _elementType, unsigned int _size, int _stride>
class vctFixedSizeVectorRef : public vctFixedSizeVectorBase<
    _size, _stride, _elementType, 
    typename vctFixedSizeVectorTraits<_elementType, _size, _stride>::pointer >
{
 public:
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);
    typedef vctFixedSizeVectorTraits<_elementType, _size, _stride> VectorTraits;
    typedef vctFixedSizeVectorRef<value_type, _size, _stride> ThisType;
    typedef vctFixedSizeVectorBase<_size, _stride, value_type, pointer> BaseType;
    typedef typename BaseType::CopyType CopyType;


    /*! Default constructor: create an uninitialized vector */
    vctFixedSizeVectorRef() {}
    

    /*! Initialize the vector with a (non-const) pointer */
    vctFixedSizeVectorRef(pointer p) {
        SetRef(p);
    }

    
    /*! Initialize a fixed size reference to a fixed-size vector.
      \note This constructor is explicit.
      \note The stride values are taken from the fixed size vector.
    */
    template<unsigned int __size, class __dataPtrType>
    explicit vctFixedSizeVectorRef(vctFixedSizeVectorBase<__size, _stride, _elementType, __dataPtrType> & otherVector,
                                   size_type startPosition = 0)
    {
        SetRef(otherVector, startPosition);
    }

    /*! Initialize a fixed size reference to a dynamic vector.
      \note This constructor is declared as explicit, since it is atypical.
    */
    template<class __vectorOwnerType>
    explicit vctFixedSizeVectorRef(vctDynamicVectorBase<__vectorOwnerType, _elementType> & otherVector,
                                   size_type startPosition = 0)
    {
        SetRef(otherVector, startPosition);
    }


    /*! Assign the vector start with a (non-const) pointer */
    void SetRef(pointer p) {
        this->Data = p;
    }

    /*! Set a fixed size reference to a fixed-size vector.
      \note the stride of the input vector must be identical to the stride of this
      vector.
      \note this vector must be contained in the input vector, that is, startPos+_size <= __size.
    */
    template<unsigned int __size, class __dataPtrType>
    void SetRef(vctFixedSizeVectorBase<__size, _stride, _elementType, __dataPtrType> & otherVector,
                size_type startPosition = 0)
    {
        CMN_ASSERT( startPosition + this->size() <= otherVector.size() );
        SetRef(otherVector.Pointer(startPosition));
    }

    /*! Set a fixed size reference to a dynamic vector.
      \note the stride of the input vector must be identical to the stride of this
      vector.
      \note this vector must be contained in the input vector, that is, startPos+_size <= __size.
    */
    template<class __vectorOwnerType>
    void SetRef(vctDynamicVectorBase<__vectorOwnerType, _elementType> & otherVector,
                size_type startPosition = 0)
    {
        CMN_ASSERT(this->stride() == otherVector.stride());
        CMN_ASSERT( startPosition + this->size() <= otherVector.size() );
        SetRef(otherVector.Pointer(startPosition));
    }


    /*!
      Assignment operation between vectors of different types

      \param other The vector to be copied.
    */
    //@{
  inline CISST_DEPRECATED ThisType & operator=(const ThisType & other) {
    return reinterpret_cast<ThisType &>(this->Assign(other));
  }

  template<int __stride>
  inline ThisType & operator=(const vctFixedSizeConstVectorRef<value_type, _size, __stride> & other) {
    return reinterpret_cast<ThisType &>(this->Assign(other));
  }

  template<int __stride, class __elementType, class __dataPtrType>
  inline ThisType & operator=(const vctFixedSizeConstVectorBase<_size, __stride, __elementType, __dataPtrType> & other) {
    return reinterpret_cast<ThisType &>(this->Assign(other));
  }
    //@}

    /*! Assignement of a scalar to all elements.  See also SetAll. */
    inline ThisType & operator = (const value_type & value) {
        this->SetAll(value);
        return *this;
    }

};

#endif  // _vctFixedSizeVectorRef_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedSizeVectorRef.h,v $
// Revision 1.14  2005/12/02 16:23:54  anton
// cisstVector: Added assigment operator from scalar (see ticket #191).
//
// Revision 1.13  2005/09/27 18:01:25  anton
// cisstVector: Use CMN_ASSERT instead of assert for vectors and matrices.
//
// Revision 1.12  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.11  2005/07/19 15:31:56  anton
// vctFixedSizeVectorRef: Marked operator = from same type as deprecated.  See
// check-in #1236 for vctDynamicVectorRef.
//
// Revision 1.10  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.9  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.8  2005/02/08 21:30:26  ofri
// vctFixedSize(Const)VectorRef: Corrected syntax error in SetRef(dynamic) :
// replaced = with ==.
//
// Revision 1.7  2005/01/06 18:54:00  anton
// cisstVector: Introduction of type CopyType as requested in ticket #113.
//
// Revision 1.6  2004/11/18 20:57:30  ofri
// RE: ticket #92.  Redefined all operator= I could find to return a ThisType &
// instead of const ThisType &.  This included corresponding revision of the
// Assign methods, and also definition from scratch of operator= for fixed and
// dynamic matrix ref.
//
// Revision 1.5  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
// Revision 1.4  2004/10/26 15:20:52  ofri
// Updating subsequence interfaces (ticket #76) and interfaces between fixed
// and dynamic vectors/matrices (ticket #72).
// *: vctFixedSize(Const)VectorBase::Get(Const)Subsequence now deprecated.
// Replaced by Get(Const)Subvector with equal stride to the parent.
// *: Added constructors for [Fixed|Dynamic][Vector|Matrix]Ref from the other
// storage type.
//
// Revision 1.3  2004/10/25 19:19:54  anton
// cisstVector:  Created vctForwardDeclarations.h and removed forward
// declarations of classes in all other files.  Added some constructors
// for vector references to reference fixed size from dynamic and vice versa.
//
// Revision 1.2  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.1  2004/08/13 17:47:40  anton
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
// Revision 1.5  2004/08/11 20:38:28  ofri
// Removed redundant subsequence definitions from vctFixedLengthSequenceRef
// and vctFixedLengthConstSequenceRef, as they are now defined in the base
// types.
// Added excluded code in vctFixedLengthConstSequenceBase to conditionally
// evaulate a compilation error.  Pending decision before including.
//
// Revision 1.4  2004/07/27 20:08:06  anton
// cisstVector: Added some Doxygen documentation.  Minor code changes to
// keep the dynamic vectors in sync with the fixed size ones.
//
// Revision 1.3  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.2  2004/04/06 15:23:15  anton
// Doxygen clean-up
//
// Revision 1.1  2003/11/20 18:31:04  anton
// Splitted const and non const classes in two files.  Renamed FixedLengthSequence to FixedLengthSequenceRef
//
//
// Revision 1.8  2003/11/11 22:04:50  anton
// STRIDE undeclared, uses _stride (idem for size).  Added assignement operator from vctFixedLengthConstSequenceBase
//
// Revision 1.7  2003/11/10 21:40:43  anton
// Added operator =
//
// Revision 1.6  2003/11/03 21:31:31  ofri
// Changed the order of template parameters when refering to
// vctFixedLengthSequenceBase (_elementType now comes third instead of first).
// This change of order does not apply to the definition of vctFixedLengthSequence
// itself.
//
// Revision 1.5  2003/10/20 18:35:19  anton
// Changed FixedSizeVectorOperations and FixedSizeVectorTraits to FixedLengthSequence Operations and Traits
//
// Revision 1.4  2003/10/20 17:07:15  anton
// Removed commented code
//
// Revision 1.3  2003/10/03 19:15:51  anton
// Updated doxygen documentation
//
// Revision 1.2  2003/09/30 18:47:16  anton
// Renamed template parameters __stride to _subStride
//
// Revision 1.1  2003/09/30 14:02:49  anton
// creation
//
//
// ****************************************************************************




