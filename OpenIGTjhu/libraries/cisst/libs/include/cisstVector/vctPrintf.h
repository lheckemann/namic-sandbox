/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id: vctPrintf.h,v 1.2 2006/02/16 16:36:16 anton Exp $

Author(s):  Ofri Sadowsky
Created on: 2006-02-15

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

#ifndef _vctPrintf_h
#define _vctPrintf_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnPrintf.h>
#include <cisstVector/vctFixedSizeConstVectorBase.h>
#include <cisstVector/vctFixedSizeConstMatrixBase.h>
#include <cisstVector/vctDynamicConstVectorBase.h>
#include <cisstVector/vctDynamicConstMatrixBase.h>

template<unsigned int _size, int _stride, class _elementType, class _dataPtrType>
bool cmnTypePrintf(cmnPrintfParser & parser, 
                   const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & vector)
{
    if (!parser.MatchOutputWithFormatChar(vector[0]) ) {
        return false;
    }

    unsigned int counter;
    for (counter = 0; counter < _size; ++counter) {
        cmnTypePrintf(parser, vector[counter]);
        if (counter != (_size-1))
            parser.RawOutput(" ");
    }

    return true;
}

template<class _vectorOwnerType, class _elementType>
bool cmnTypePrintf(cmnPrintfParser & parser, 
                   const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & vector)
{
    const unsigned int vectorSize = vector.size();
    if (vectorSize < 1)
        return true;

    if (!parser.MatchOutputWithFormatChar(vector[0]) ) {
        return false;
    }

    unsigned int counter;
    for (counter = 0; counter < vectorSize; ++counter) {
        const _elementType element = vector[counter];
        cmnTypePrintf(parser, element);
        if (counter != (vectorSize-1))
            parser.RawOutput(" ");
    }

    return true;
}


template<unsigned int _rows, unsigned int _cols, int _rowStride, int _colStride,
class _elementType, class _dataPtrType>
    inline bool cmnTypePrintf(cmnPrintfParser & parser, 
    const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride,
    _elementType, _dataPtrType> & matrix)
{
    if (!parser.MatchOutputWithFormatChar(matrix.Element(0,0)) ) {
        return false;
    }

    unsigned int rowCounter, colCounter;
    for (rowCounter = 0; rowCounter < _rows; ++rowCounter) {
        for (colCounter = 0; colCounter < _cols; ++colCounter) {
            cmnTypePrintf(parser, matrix.Element(rowCounter, colCounter));
            if (colCounter != (_cols-1))
                parser.RawOutput(" ");
        }
        if (rowCounter != (_rows - 1))
            parser.RawOutput("\n");
    }

    return true;
}


template<class _matrixOwnerType, class _elementType>
bool cmnTypePrintf(cmnPrintfParser & parser, 
                   const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & matrix)
{
    const unsigned int rows = matrix.rows();
    const unsigned int cols = matrix.cols();

    if (matrix.size() < 1)
        return true;

    if (!parser.MatchOutputWithFormatChar(matrix.Element(0,0)) ) {
        return false;
    }

    unsigned int rowCounter, colCounter;
    for (rowCounter = 0; rowCounter < rows; ++rowCounter) {
        for (colCounter = 0; colCounter < cols; ++colCounter) {
            cmnTypePrintf(parser, matrix.Element(rowCounter, colCounter));
            if (colCounter != (cols - 1))
                parser.RawOutput(" ");
        }
        if (rowCounter != (rows - 1))
            parser.RawOutput("\n");
    }

    return true;
}


#endif  // _vctPrintf_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: vctPrintf.h,v $
//  Revision 1.2  2006/02/16 16:36:16  anton
//  vctPrintf.h: Replaced _rows and _cols by appropriate values for dynamic
//  matrices cmnTypePrintf.
//
//  Revision 1.1  2006/02/16 00:21:00  ofri
//  Added formatted output files cmnPrintf and vctPrintf in the respective
//  directories (code, include)
//
//
// ****************************************************************************
