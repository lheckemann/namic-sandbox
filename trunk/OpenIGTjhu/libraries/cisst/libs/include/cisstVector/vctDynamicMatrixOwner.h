/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctDynamicMatrixOwner.h,v 1.14 2006/04/26 19:51:21 anton Exp $
  
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


#ifndef _vctDynamicMatrixOwner_h
#define _vctDynamicMatrixOwner_h


/*!
  \file
  \brief Declaration of vctDynamicMatrixOwner
*/


#include <cisstVector/vctVarStrideMatrixIterator.h>


/*!
  This templated class owns a dynamically allocated array, but does
  not provide any other operations */
template<class _elementType>
class vctDynamicMatrixOwner
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /* iterators are container specific */
    typedef vctVarStrideMatrixConstIterator<value_type> const_iterator;
    typedef vctVarStrideMatrixConstIterator<value_type> const_reverse_iterator;
    typedef vctVarStrideMatrixIterator<value_type> iterator;
    typedef vctVarStrideMatrixIterator<value_type> reverse_iterator;

    vctDynamicMatrixOwner():
        Rows(0),
        Cols(0),
        RowMajor(VCT_DEFAULT_STORAGE),
        Data(0)
        {}
    
    vctDynamicMatrixOwner(size_type rows, size_type cols, bool rowMajor = VCT_DEFAULT_STORAGE):
        Rows(rows),
        Cols(cols),
        RowMajor(rowMajor),
        Data(new value_type[rows * cols])
    {}
    
    ~vctDynamicMatrixOwner() {
        Disown();
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
        return RowMajor ? Cols : 1;
    }

    difference_type col_stride(void) const {
        return RowMajor ? 1 : Rows;
    }

    pointer Pointer(index_type rowIndex, index_type colIndex) {
        return Data + rowIndex * row_stride() + colIndex * col_stride();
    }

    pointer Pointer(void) {
        return Data;
    }
    
    const_pointer Pointer(index_type rowIndex, index_type colIndex) const {
        return Data + rowIndex * row_stride() + colIndex * col_stride();
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

    /*!  Non-preserving resize operation.  This method discards of all
      the current data of the dynamic array and allocates new space in
      the requested size.

      \note If the storage order and the sizes (both rows and columns)
      are unchanged, this method does nothing.
     */
    void SetSize(size_type rows, size_type cols, bool rowMajor) {
        if ((rows == Rows) && (cols == Cols) && (rowMajor == RowMajor)) return;
        Disown();
        Own(rows, cols, rowMajor, new value_type[rows * cols]);
    }

    /*! Data preserving resize operation.  This method allocates a
      buffer of the input size and copies all possible old buffer
      elements to the new buffer.  If the new buffer is larger than
      the old, the tail elements are initialized with a default
      constructor.

      \note If the storage order and the sizes (both rows and columns)
      are unchanged, this method does nothing.

      \note This method doesn't allow to change the storage order of
      the elements (i.e. stays either row or column major).
    */
    void resize(size_type rows, size_type cols) {
        if ((rows == Rows) && (cols == Cols)) return;
        // new allocated memory
        value_type * newData = new value_type[rows * cols];
        const size_type minRows = std::min(rows, Rows);
        const size_type minCols = std::min(cols, Cols);
        const difference_type new_row_stride = RowMajor ? cols : 1;
        const difference_type new_col_stride = RowMajor ? 1 : rows;
        size_type r, c;
        for (r = 0; r < minRows; ++r)
            for (c = 0; c < minCols; ++c)
                newData[r * new_row_stride + c * new_col_stride] = Data[r * row_stride() + c * col_stride()];

        value_type * oldData = Own(rows, cols, RowMajor, newData);
        delete[] oldData;
    }

    /*! Release the currently owned data pointer from being owned.
      Reset this owner's data pointer and size to zero.  Return the
      old data pointer without freeing memory.
     */
    value_type * Release() {
        value_type * oldData = Data;
        Data = 0;
        Rows = 0;
        Cols = 0;
        RowMajor = VCT_DEFAULT_STORAGE;
        return oldData;
    }

    /*! Have this owner take ownership of a new data pointer. Return
      the old data pointer without freeing memory.
 
      \note This method returns a pointer to the previously owned
      memory block but doesn't tell if the old block was row or column
      major nor the size of the block.
    */
    value_type * Own(size_type rows, size_type cols, bool rowMajor, value_type * data) {
        value_type * oldData = Data;
        Rows = rows;
        Cols = cols;
        RowMajor = rowMajor;
        Data = data;
        return oldData;
    }

    /*! Free the memory allocated for the data pointer.  Reset data
      pointer and size to zero.
    */
    void Disown(void) {
        delete[] Data;
        Rows = 0;
        Cols = 0;
        RowMajor = VCT_DEFAULT_STORAGE;
        Data = 0;
    }

    inline bool IsColMajor(void) const {
        return !RowMajor;
    }
    
    inline bool IsRowMajor(void) const {
        return RowMajor;
    }

    inline bool StorageOrder(void) const {
        return RowMajor;
    }

protected:
    size_type Rows;
    size_type Cols;
    bool RowMajor;
    value_type* Data;
};


#endif // _vctDynamicMatrixOwner_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctDynamicMatrixOwner.h,v $
//  Revision 1.14  2006/04/26 19:51:21  anton
//  vctDynamicMatrix: Correction for bug #221.  This should work well for matrices
//  using vctDynamicMatrixOwner.  For matrices using existing memory, i.e.
//  vctDynamicMatrixRefOwner, we need to redefine the specifications and add test
//  cases.
//
//  Revision 1.13  2005/09/26 15:41:47  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.12  2005/07/28 20:54:13  anton
//  vctDynamic{Vector,Matrix}Owner: Modified SetSize() and resize() to do
//  nothing if the size of container is unchanged.
//
//  Revision 1.11  2005/05/19 19:29:01  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.10  2005/02/09 15:30:06  ofri
//  Resovling .NET compiler warning in vctDynamicMatrixOwner::resize -- signed type
//  mismatch.
//
//  Revision 1.9  2004/12/01 16:42:20  anton
//  cisstVector: Modified the signature of Pointer methods for matrices.  We now
//  have two signature, Pointer(void) and Pointer(row, col) with no default value
//  to avoid the ambiguous Pointer(index).  See ticket #93.
//
//  Revision 1.8  2004/11/29 17:31:28  anton
//  cisstVector: Major corrections for matrices reverse iterators.  Correction
//  of the - operator which now takes into account the current column position.
//
//  Revision 1.7  2004/11/11 20:35:46  anton
//  cisstVector: *: Added a vctContainerTraits to centralize the declarations
//  of size_type, difference_type, reference, const_reference, etc. *: All
//  iterators are now derived from std::iterator. *: Added some missing typedef
//  for iterators.
//
//  Revision 1.6  2004/11/03 22:26:12  anton
//  cisstVector:  Better support of storage order.  Added VCT_COL_MAJOR,
//  VCT_ROW_MAJOR and VCT_DEFAULT_STORAGE as well as the methods IsColMajor(),
//  IsRowMajor(), IsCompact() and IsFortran().
//
//  Revision 1.5  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.4  2004/10/19 20:50:50  anton
//  cisstVector: Preliminary support for different storage orders (i.e. row
//  major by default, column major if specified).  This has been implemented
//  for the vctFixedSizeMatrix as well as the vctDynamicMatrix.  We tested the
//  code by simply changinf the default and all current tests passed.  See
//  ticket #75 re. vctFortranMatrix.
//
//  Revision 1.3  2004/09/09 14:03:32  ofri
//  vctDynamicMatrixOwner: Bug fix in method resize().  See #57
//
//  Revision 1.2  2004/08/16 19:19:49  anton
//  cisstVector: Replaced a couple of "value_type *" by typedefed "pointer"
//  and "const value_type *" by "const_pointer".
//
//  Revision 1.1  2004/08/04 21:11:10  anton
//  cisstVector: Added preliminary version of dynamic matrices.  Lots of work
//  is still required, this code is not ready to be used.
//
//
// ****************************************************************************
