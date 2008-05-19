/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicVectorRefOwner.h,v 1.7 2005/09/26 15:41:47 anton Exp $
  
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


#ifndef _vctDynamicVectorRefOwner_h
#define _vctDynamicVectorRefOwner_h

/*!
  \file
  \brief Declaration of vctDynamicVectorRefOwner
*/


#include <cisstVector/vctVarStrideVectorIterator.h>

/*! 
  \ingroup cisstVector

  This templated class stores a pointer, a size, and a stride, and
  allows element access, but does not provide any other operations,
  and does not own the data */
template<class _elementType>
class vctDynamicVectorRefOwner
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /* iterators are container specific */    
    typedef vctVarStrideVectorConstIterator<value_type> const_iterator;
    typedef vctVarStrideVectorIterator<value_type> iterator;
    typedef vctVarStrideVectorConstIterator<value_type> const_reverse_iterator;
    typedef vctVarStrideVectorIterator<value_type> reverse_iterator;


    vctDynamicVectorRefOwner()
        : Size(0)
        , Stride(1)
        , Data(0)
    {}

    vctDynamicVectorRefOwner(size_type size, value_type * data, stride_type stride = 1)
        : Size(size)
        , Stride(stride)
        , Data(data)
    {}

    void SetRef(size_type size, value_type * data, stride_type stride = 1)
    {
        Size = size;
        Stride = stride;
        Data = data;
    }

    size_type size(void) const {
        return Size;
    }

    stride_type stride(void) const {
        return Stride;
    }
    
    pointer Pointer(int index = 0) {
        return Data + Stride * index;
    }

    const_pointer Pointer(int index = 0) const {
        return Data + Stride * index;
    }
    
    const_iterator begin(void) const {
        return const_iterator(Data, Stride);
    }
     
    const_iterator end(void) const {
        return const_iterator(Data + Size * Stride, Stride);
    }

    iterator begin(void) {
        return iterator(Data, Stride);
    }
     
    iterator end(void) {
        return iterator(Data + Size * Stride, Stride);
    }

    const_reverse_iterator rbegin(void) const {
      return const_reverse_iterator(Data + (Size-1) * Stride, -Stride);
    }
     
    const_reverse_iterator rend(void) const {
      return const_reverse_iterator(Data - Stride, -Stride);
    }

    reverse_iterator rbegin(void) {
      return reverse_iterator(Data + (Size-1) * Stride, -Stride);
    }
     
    reverse_iterator rend(void) {
      return reverse_iterator(Data - Stride, -Stride);
    }

 protected:
    size_type Size;
    int Stride;
    value_type* Data;
};


#endif // _vctDynamicVectorRefOwner_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicVectorRefOwner.h,v $
//  Revision 1.7  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.6  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.5  2005/02/03 20:25:44  ofri
//  Added method vctDynamicVectorOwner::stride(), which, apparently, was absent
//  so far.
//
//  Revision 1.4  2004/11/11 20:35:46  anton
//  cisstVector: *: Added a vctContainerTraits to centralize the declarations
//  of size_type, difference_type, reference, const_reference, etc. *: All
//  iterators are now derived from std::iterator. *: Added some missing typedef
//  for iterators.
//
//  Revision 1.3  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.2  2004/08/16 19:19:49  anton
//  cisstVector: Replaced a couple of "value_type *" by typedefed "pointer"
//  and "const value_type *" by "const_pointer".
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
//  Revision 1.5  2004/07/06 19:42:50  ofri
//  Giving the dynamic vector classes "standard look and feel" in terms of type
//  members, constructors, and indentations.  I made sure that the corresponding
//  example compiles and runs successfully.
//
//  Revision 1.4  2004/07/06 17:47:11  anton
//  Dynamic Sequences:
//  *: Compilation error re. const and const_iterator
//  *: Added some iterator methods to sequences
//  *: Compilation errors re. the order (size, stride, data) in constructor.  The order is now (size, data, stride = 1) everywhere to be consistant
//  *: The method Resize is now resize (STL spelling)
//
//  Revision 1.3  2004/07/06 00:53:16  ofri
//  Added reverse iterators to vctDynamicVectorOwner and vctDynamicSequenceRefOwner.
//  Fixed some bugs in vctDynamicVectorOwner.
//
//  Revision 1.2  2004/07/02 20:14:07  anton
//  Code for VarStrideVectorIterator.  Added the class as well as some code to
//  try it.  An example is provided in examples/vectorTutorial/main.cpp.
//
//  Revision 1.1  2004/07/02 16:16:45  anton
//  Massive renaming in cisstVector for the addition of dynamic size vectors
//  and matrices.  The dynamic vectors are far from ready.
//
//
// ****************************************************************************
