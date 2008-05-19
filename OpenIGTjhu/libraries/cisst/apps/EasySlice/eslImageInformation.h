/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

// $Id: eslImageInformation.h,v 1.3 2005/09/26 15:41:46 anton Exp $


#ifndef _eslImageInformation_h
#define _eslImageInformation_h


#include <string>
#include <sstream>


class eslImageInformation {
public:
    std::string filename;
    double dx, dy;
    double x, y, z;
    double nbRows, nbCols;
    double FovWidth, FovHeight;
    double thick;
    double tilt;

    inline std::string ToString(void) const {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }
    
    inline void ToStream(std::ostream & outputStream) const {
        outputStream << "File:           " << filename << std::endl
                     << "Rows & columns: " << nbRows << " x " << nbCols << std::endl
                     << "Size of pixels: " << dx << " " << dy << std::endl
                     << "Field of view:  " << FovWidth << " x " << FovHeight << std::endl
                     << "Thickness:      " << thick << std::endl
                     << "Gantry tilt:    " << tilt << std::endl
                     << "Image Patient:  " << x << " " << y << " " << z;
    }

    
};


/*! Stream out operator. */
inline std::ostream & operator << (std::ostream & output,
                                   const eslImageInformation & imageInfo) {
    imageInfo.ToStream(output);
    return output;
}


#endif


// $Log: eslImageInformation.h,v $
// Revision 1.3  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/08/04 15:15:50  anton
// imported into cvstrac
//
// Revision 1.1.1.1  2005/07/29 21:30:45  anton
// Creation
//
