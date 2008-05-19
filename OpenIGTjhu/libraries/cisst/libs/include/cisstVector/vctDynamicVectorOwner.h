/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicVectorOwner.h,v 1.15 2005/09/26 15:41:47 anton Exp $
  
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


#ifndef _vctDynamicVectorOwner_h
#define _vctDynamicVectorOwner_h


/*!
  \file
  \brief Declaration of vctDynamicVectorOwner
*/


#include <cisstVector/vctFixedStrideVectorIterator.h>


/*!
  This templated class owns a dynamically allocated array, but does
  not provide any other operations */
template<class _elementType>
class vctDynamicVectorOwner
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /* iterators are container specific */    
    enum { DEFAULT_STRIDE = 1 };
#ifndef SWIG
    typedef vctFixedStrideVectorConstIterator<value_type, DEFAULT_STRIDE> const_iterator;
    typedef vctFixedStrideVectorConstIterator<value_type, -DEFAULT_STRIDE> const_reverse_iterator;
    typedef vctFixedStrideVectorIterator<value_type, DEFAULT_STRIDE> iterator;
    typedef vctFixedStrideVectorIterator<value_type, -DEFAULT_STRIDE> reverse_iterator;
#endif // SWIG

    vctDynamicVectorOwner()
        : Size(0)
        , Data(0)
    {}
    
    vctDynamicVectorOwner(size_type size)
        : Size(size)
        , Data(new value_type[size])
    {}
    
    ~vctDynamicVectorOwner() {
        Disown();
    }
    
    size_type size(void) const {
        return Size;
    }

    int stride() const
    {
        return DEFAULT_STRIDE;
    }
    
    pointer Pointer(index_type index = 0) {
        return Data + index;
    }
    
    const_pointer Pointer(index_type index = 0) const {
        return Data + index;
    }
    
    const_iterator begin(void) const {
        return const_iterator(Data);
    }

    const_iterator end(void) const {
        return const_iterator(Data + Size);
    }

    iterator begin(void) {
        return iterator(Data);
    }

    iterator end(void) {
        return iterator(Data + Size);
    }

    const_reverse_iterator rbegin(void) const {
        return const_reverse_iterator(Data + Size - 1);
    }

    const_reverse_iterator rend(void) const {
        return const_reverse_iterator(Data - 1);
    }

    reverse_iterator rbegin(void) {
        return reverse_iterator(Data + Size-1);
    }

    reverse_iterator rend(void) {
        return reverse_iterator(Data - 1);
    }

    /*!  Non-preserving resize operation.  This method discards of all
      the current data of the dynamic array and allocates new space in
      the requested size.

      \note If the size is unchanged, this method does nothing.
     */
    void SetSize(size_type size) {
        if (size == Size) return;
        Disown();
        Own(size, new value_type[size]);
    }

    /*! Data preserving resize operation.  This method allocates a
      buffer of the input size and copies all possible old buffer
      elements to the new buffer.  If the new buffer is larger than
      the old, the tail elements are initialized with a default
      constructor.

      \note If the size is unchanged, this method does nothing.
    */
    void resize(size_type size) {
        if (size == Size) return;
        // new allocated memory
        value_type * newData = new value_type[size];
        const size_type minSize = std::min(Size, size);
        std::copy(Data, Data + minSize, newData);
        value_type * oldData = Own(size, newData);
        delete[] oldData;
    }

    /*! Release the currently owned data pointer from being owned.
      Reset this owner's data pointer and size to zero.  Return the
      old data pointer without freeing memory.
     */
    value_type * Release()
    {
        value_type * oldData = Data;
        Data = 0;
        Size = 0;
        return oldData;
    }

    /*! Have this owner take ownership of a new data pointer. Return
      the old data pointer without freeing memory.
    */
    value_type * Own(size_type size, value_type * data) {
        value_type * oldData = Data;
        Size = size;
        Data = data;
        return oldData;
    }

    /*! Free the memory allocated for the data pointer.  Reset data
      pointer and size to zero.
    */
    void Disown(void) {
        delete[] Data;
        Size = 0;
        Data = 0;
    }
    

protected:
    size_type Size;
    value_type* Data;
};


#endif // _vctDynamicVectorOwner_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicVectorOwner.h,v $
//  Revision 1.15  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.14  2005/07/28 20:54:13  anton
//  vctDynamic{Vector,Matrix}Owner: Modified SetSize() and resize() to do
//  nothing if the size of container is unchanged.
//
//  Revision 1.13  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.12  2005/04/02 23:04:49  anton
//  cisstVector:  Added conditional compilation for SWIG in dynamic containers.
//
//  Revision 1.11  2004/11/29 17:31:28  anton
//  cisstVector: Major corrections for matrices reverse iterators.  Correction
//  of the - operator which now takes into account the current column position.
//
//  Revision 1.10  2004/11/11 20:35:46  anton
//  cisstVector: *: Added a vctContainerTraits to centralize the declarations
//  of size_type, difference_type, reference, const_reference, etc. *: All
//  iterators are now derived from std::iterator. *: Added some missing typedef
//  for iterators.
//
//  Revision 1.9  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.8  2004/08/16 19:19:49  anton
//  cisstVector: Replaced a couple of "value_type *" by typedefed "pointer"
//  and "const value_type *" by "const_pointer".
//
//  Revision 1.7  2004/08/13 17:47:40  anton
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
//  Revision 1.6  2004/07/06 19:42:50  ofri
//  Giving the dynamic vector classes "standard look and feel" in terms of type
//  members, constructors, and indentations.  I made sure that the corresponding
//  example compiles and runs successfully.
//
//  Revision 1.5  2004/07/06 17:47:11  anton
//  Dynamic Sequences:
//  *: Compilation error re. const and const_iterator
//  *: Added some iterator methods to sequences
//  *: Compilation errors re. the order (size, stride, data) in constructor.  The order is now (size, data, stride = 1) everywhere to be consistant
//  *: The method Resize is now resize (STL spelling)
//
//  Revision 1.4  2004/07/06 00:53:16  ofri
//  Added reverse iterators to vctDynamicVectorOwner and vctDynamicSequenceRefOwner.
//  Fixed some bugs in vctDynamicVectorOwner.
//
//  Revision 1.3  2004/07/02 20:11:58  anton
//  vctDynamicVectorOwner::SetSize was not setting the data member Size correctly.
//
//  Revision 1.2  2004/07/02 18:18:31  anton
//  Introduced an index_type which might have to be moved to some kind of traits
//
//  Revision 1.1  2004/07/02 16:16:45  anton
//  Massive renaming in cisstVector for the addition of dynamic size vectors
//  and matrices.  The dynamic vectors are far from ready.
//
//
// ****************************************************************************
