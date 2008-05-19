/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnErrorCodeBase.h,v 1.4 2005/09/26 15:41:46 anton Exp $

  Author(s):  Ankur Kapoor
  Created on:  2003-06-2

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
  \brief Declaration of cmnErrorCodeBase 
 */


#ifndef _cmnErrorCodeBase_h
#define _cmnErrorCodeBase_h

#include <string>

class cmnErrorCodeBase {
 public:
    cmnErrorCodeBase (const char shortDescription[] = "UNDEFINED",
                      const char detailedDescription[] = "Undefined",
                      const char errorClass[] = "Base") :
        ShortDescription(shortDescription),
        DetailedDescription(detailedDescription),
        ErrorClass(errorClass) {};

    // compliance with CISST Formated I/O. Don't remember if it was ToString or Debug?
    inline std::string ToString (void) throw() {
        std::string toString = "ErrorClass: " + ErrorClass +
            ", ShortDescription: " + ShortDescription + 
            ", DetailedDescription: " + DetailedDescription;
      return toString;};
  private:
    std::string ShortDescription;
    std::string DetailedDescription;
    std::string ErrorClass;
};

#endif // _cmnErrorCodeBase_h

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnErrorCodeBase.h,v $
// Revision 1.4  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.3  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.2  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.1  2003/11/14 22:05:36  anton
// Added to repository for testing
//
//
// ****************************************************************************
