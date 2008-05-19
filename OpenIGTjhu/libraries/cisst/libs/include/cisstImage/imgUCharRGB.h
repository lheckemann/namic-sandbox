/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: imgUCharRGB.h,v 1.1 2006/03/30 20:21:48 anton Exp $
  
  Author(s): Anton Deguet
  Created on: 2006-03-28

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
  \brief Types for unsigned char based RGB pixmap
*/


#ifndef _imgUCharRGB_h
#define _imgUCharRGB_h


#include <iostream>
#include <ios>


#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctDynamicMatrix.h>
#include <cisstVector/vctDynamicConstMatrixRef.h>

// Always include imgExport.h last
#include <cisstImage/imgExport.h>


namespace imgUCharRGB {

    typedef vctFixedSizeVector<unsigned char, 3> Pixel;
    typedef vctDynamicMatrix<Pixel> Pixmap;
    typedef vctDynamicMatrixRef<Pixel> PixmapRef;
    typedef vctDynamicConstMatrixRef<Pixel> ConstPixmapRef;


    /*! Predefined constants for pixel values. */
    //@{
    extern CISST_EXPORT const Pixel Black;
    extern CISST_EXPORT const Pixel White;
    extern CISST_EXPORT const Pixel Red;
    extern CISST_EXPORT const Pixel Green;
    extern CISST_EXPORT const Pixel Blue;
    //@}


    /*! Write to file in PPM format */
    template <class _matrixOwnerType>
    bool WritePPM(const vctDynamicConstMatrixBase<_matrixOwnerType, Pixel> & pixmap, const char * fileName) {
        
        typedef vctDynamicConstMatrixBase<_matrixOwnerType, Pixel> PixmapType;
        typedef typename PixmapType::const_iterator const_iterator;
        
        std::ofstream file(fileName, std::ios::out | std::ios::binary);
        if (!file) {
            CMN_LOG(1) << "imgWritePPM: Can't open file " << fileName << std::endl;
            return false;
        }
        
        file << "P6" << std::endl
             << pixmap.width() << " " << pixmap.height() << std::endl
             << "255" << std::endl;
        
        unsigned char *tempBuffer = new unsigned char [pixmap.width() * pixmap.height() * 3];
        const const_iterator end = pixmap.end();
        const_iterator iter = pixmap.begin();
        unsigned int bufferPosition;
        for (bufferPosition = 0; iter != end; ++iter) {
            tempBuffer[bufferPosition++] = (*iter).X();
            tempBuffer[bufferPosition++] = (*iter).Y();
            tempBuffer[bufferPosition++] = (*iter).Z();
        }
        
        file.write((char*)tempBuffer, pixmap.width() * pixmap.height() * 3);
        file.flush();
        file.close();
        delete[] tempBuffer;
        CMN_LOG(3) << "imgWritePPM: Successfully saved as " << fileName << std::endl;
        return true;
    };

}


#endif  // _imgUCharRGB_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: imgUCharRGB.h,v $
// Revision 1.1  2006/03/30 20:21:48  anton
// cisstImage: Added typedefs based on vctDynamicMatrix to handle pixmaps of
// RGB pixels (defined as vctFixedSizeVector) and a simple PPM export function.
// Also added a simple test program to check that the compiler doesn't pad our
// pixels and a small example program (see imageTutorial).
//
//
// ****************************************************************************

