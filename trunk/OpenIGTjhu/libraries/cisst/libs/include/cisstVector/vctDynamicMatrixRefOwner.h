/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicMatrixRefOwner.h,v 1.9 2006/04/26 19:51:21 anton Exp $
  
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


#ifndef _vctDynamicMatrixRefOwner_h
#define _vctDynamicMatrixRefOwner_h

/*!
  \file
  \brief Declaration of vctDynamicMatrixRefOwner
*/


#include <cisstVector/vctVarStrideMatrixIterator.h>

/*! 
  This templated class stores a pointer, a size, and a stride, and
  allows element access, but does not provide any other operations,
  and does not own the data */
template<class _elementType>
class vctDynamicMatrixRefOwner
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /* iterators are container specific */    
    typedef vctVarStrideMatrixConstIterator<value_type> const_iterator;
    typedef vctVarStrideMatrixConstIterator<value_type> const_reverse_iterator;
    typedef vctVarStrideMatrixIterator<value_type> iterator;
    typedef vctVarStrideMatrixIterator<value_type> reverse_iterator;


    vctDynamicMatrixRefOwner():
        Rows(0),
        Cols(0),
        RowStride(1),
        ColStride(1),
        Data(0)
    {}

    vctDynamicMatrixRefOwner(size_type rows, size_type cols,
                             stride_type rowStride, stride_type colStride,
                             pointer data):
        Rows(rows),
        Cols(cols),
        RowStride(rowStride),
        ColStride(colStride),
        Data(data)
    {}

    void SetRef(size_type rows, size_type cols,
                stride_type rowStride, stride_type colStride,
                pointer data) {
        Rows = rows;
        Cols = cols;
        RowStride = rowStride;
        ColStride = colStride;
        Data = data;
    }

    size_type size(void) const {
        return Rows * Cols;
    }

    size_type rows(void) const {
        return Rows;
    }

    size_type cols(void) const {
        return Cols;
    }

    difference_type row_stride(void) const {
        return RowStride;
    }

    difference_type col_stride(void) const {
        return ColStride;
    }

    pointer Pointer(index_type rowIndex, index_type colIndex) {
        return Data +  rowIndex * row_stride() + colIndex * col_stride();
    }

    pointer Pointer(void) {
        return Data;
    }
    
    const_pointer Pointer(index_type rowIndex, index_type colIndex) const {
        return Data +  rowIndex * row_stride() + colIndex * col_stride();
    }

    const_pointer Pointer(void) const {
        return Data;
    }
    
    const_iterator begin(void) const {
        return const_iterator(Data, col_stride(), cols(), row_stride());
    }

    const_iterator end(void) const {
        return const_iterator(Data, col_stride(), cols(), row_stride())
            + rows() * cols();
    }

    iterator begin(void) {
        return iterator(Data, col_stride(), cols(), row_stride());
    }

    iterator end(void) {
        return iterator(Data, col_stride(), cols(), row_stride())
            + rows() * cols();
    }

    const_reverse_iterator rbegin(void) const {
        return const_reverse_iterator(Data + row_stride() * (rows() - 1) + col_stride() * (cols() - 1),
                                      -col_stride(), cols(), -row_stride());
    }

    const_reverse_iterator rend(void) const {
        return const_reverse_iterator(Data - row_stride() + col_stride() * (cols() - 1),
                                      -col_stride(), cols(), -row_stride());
    }

    reverse_iterator rbegin(void) {
        return reverse_iterator(Data + row_stride() * (rows() - 1) + col_stride() * (cols() - 1),
                                -col_stride(), cols(), -row_stride());
    }

    reverse_iterator rend(void) {
        return reverse_iterator(Data - row_stride() + col_stride() * (cols() - 1),
                                -col_stride(), cols(), -row_stride());
    }

    inline bool IsColMajor(void) const {
        return (this->row_stride() <= this->col_stride());
    }
    
    inline bool IsRowMajor(void) const {
        return (this->col_stride() <= this->row_stride());
    }

    inline bool StorageOrder(void) const {
        return this->IsRowMajor();
    }

 protected:
    size_type Rows;
    size_type Cols;
    stride_type RowStride;
    stride_type ColStride;
    value_type* Data;
};


#endif // _vctDynamicMatrixRefOwner_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicMatrixRefOwner.h,v $
//  Revision 1.9  2006/04/26 19:51:21  anton
//  vctDynamicMatrix: Correction for bug #221.  This should work well for matrices
//  using vctDynamicMatrixOwner.  For matrices using existing memory, i.e.
//  vctDynamicMatrixRefOwner, we need to redefine the specifications and add test
//  cases.
//
//  Revision 1.8  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.7  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.6  2004/12/01 16:42:20  anton
//  cisstVector: Modified the signature of Pointer methods for matrices.  We now
//  have two signature, Pointer(void) and Pointer(row, col) with no default value
//  to avoid the ambiguous Pointer(index).  See ticket #93.
//
//  Revision 1.5  2004/11/29 17:31:28  anton
//  cisstVector: Major corrections for matrices reverse iterators.  Correction
//  of the - operator which now takes into account the current column position.
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
//  Revision 1.2  2004/08/16 19:28:30  anton
//  cisstVector: Added preliminary code for vctDynamicMatrixRefOwner and
//  vctDynamicConstMatrixRef.
//
//  Revision 1.1  2004/08/04 21:11:10  anton
//  cisstVector: Added preliminary version of dynamic matrices.  Lots of work
//  is still required, this code is not ready to be used.
//
//
// ****************************************************************************
