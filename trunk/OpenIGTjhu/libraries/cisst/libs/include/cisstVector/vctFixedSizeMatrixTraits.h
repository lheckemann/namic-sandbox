/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeMatrixTraits.h,v 1.7 2005/09/26 15:41:47 anton Exp $

  Author(s):  Ofri Sadowsky
  Created on:  2003-11-04

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
  \brief Declaration of vctFixedSizeMatrixTraits
 */


#ifndef _vctFixedSizeMatrixTraits_h
#define _vctFixedSizeMatrixTraits_h


#include <cisstVector/vctFixedStrideMatrixIterator.h>


/*!  \brief Define common container related types based on the
  properties of a fixed size container.

  The types are declared according to the STL requirements for the
  types declared by a container object.  This class is used as a trait
  to declare the actual containers.

  In addition to the STL required types, we declare a few more types for
  completeness.

*/
template<class _elementType, unsigned int _rows, unsigned int _cols,
         int _rowStride, int _colStride>
class vctFixedSizeMatrixTraits
{
public:
    /* define most types from vctContainerTraits */
    VCT_CONTAINER_TRAITS_TYPEDEFS(_elementType);

    /*! Iterator, follows the STL naming convention. */
    typedef vctFixedStrideMatrixIterator<_elementType, _colStride, _cols, _rowStride> iterator;

    /*! Const iterator, follows the STL naming convention. */
    typedef vctFixedStrideMatrixConstIterator<_elementType, _colStride, _cols, _rowStride> const_iterator;

    /*! Reverse iterator, follows the STL naming convention. */
    typedef vctFixedStrideMatrixIterator<_elementType, -_colStride, _cols, -_rowStride> reverse_iterator;

    /*! Const reverse iterator, follows the STL naming convention. */
    typedef vctFixedStrideMatrixConstIterator<_elementType, -_colStride, _cols, -_rowStride> const_reverse_iterator;

    /*! Define the dimensions of the matrix: number of rows, number of
      columns, and the total length of the matrix, which is their product
    */
    enum {ROWS = _rows, COLS = _cols, LENGTH = ROWS*COLS};
    /*! Declared for completeness. */
    typedef value_type array[LENGTH];
    /*! Declared for completeness. */
    typedef const value_type const_array[LENGTH];

    /*! Declared to enable inference of the stride of the container. */
    enum {ROWSTRIDE = _rowStride, COLSTRIDE = _colStride};
};

#endif


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedSizeMatrixTraits.h,v $
// Revision 1.7  2005/09/26 15:41:47  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.6  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.5  2004/11/11 20:35:46  anton
// cisstVector: *: Added a vctContainerTraits to centralize the declarations
// of size_type, difference_type, reference, const_reference, etc. *: All
// iterators are now derived from std::iterator. *: Added some missing typedef
// for iterators.
//
// Revision 1.4  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.3  2004/09/03 19:56:36  anton
// Doxygen documentation.
//
// Revision 1.2  2004/07/02 16:16:45  anton
// Massive renaming in cisstVector for the addition of dynamic size vectors
// and matrices.  The dynamic vectors are far from ready.
//
// Revision 1.1  2003/11/04 21:58:49  ofri
// Committing initial version of the code for fixed-size matrices.  Need to
// implement more operations and improve documentation, but the basic
// framework works.
//
// ****************************************************************************
