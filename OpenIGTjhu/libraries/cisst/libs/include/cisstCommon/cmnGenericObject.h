/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnGenericObject.h,v 1.8 2005/09/26 15:41:46 anton Exp $

  Author(s):  Anton Deguet
  Created on:  2004-08-18

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
  \brief Defines cmnGenericObject
*/

#ifndef _cmnGenericObject_h
#define _cmnGenericObject_h


#include <string>
#include <iostream>


#include <cisstCommon/cmnExport.h>


class cmnClassServicesBase;

/*!
  \brief Base class for high level objects.
  
  \ingroup cisstCommon
  
  \sa cmnClassRegister cmnClassServiceBase
*/
class CISST_EXPORT cmnGenericObject {
public:

    virtual ~cmnGenericObject(void) {};

    /*! Pure virtual method to access the class services.  The derived
      classes should always declare and implement this method using
      the macros #CMN_DECLARE_SERVICES and #CMN_IMPLEMENT_SERVICES.
      In NO WAY, a user should declare this method otherwise.
      
      \return A pointer on the class services.  This points to the
      unique instance of cmnClassServiceBase for a given class.

      \sa cmnClassRegister cmnClassServiceBase
    */  
    virtual cmnClassServicesBase* const Services(void) const = 0;

    
    /*! Formatted IO to a string.  This method relies on ToStream
      which should be overloaded for each class. */ 
    std::string ToString(void) const;

  
    /*! The default ToStream method returns the name of the class.
      This method must be overloaded to provide a useful message. */
    virtual void ToStream(std::ostream & outputStream) const;

};


/*! Stream out operator. */
inline
std::ostream & operator << (std::ostream & output,
                            const cmnGenericObject & object) {
    object.ToStream(output);
    return output;
}



#endif // _cmnGenericObject_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnGenericObject.h,v $
// Revision 1.8  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.7  2005/08/14 00:51:26  anton
// cisstCommon: Port to gcc-4.0 (use template<> for template specialization).
//
// Revision 1.6  2005/08/11 17:24:18  anton
// cmnGenericObject: Added ToString() and ToStream() methods for all classes
// derived from cmnGenericObject.  Updated cmnPath to reflect these changes.
// See also ticket #68.
//
// Revision 1.5  2005/07/06 02:32:57  anton
// cmnGenericObject: virtual method Services is now const and returns a const
// pointer.
//
// Revision 1.4  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.3  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.2  2004/09/03 19:32:03  anton
// cisstCommon: Documentation of the new log and class registration system,
// see check-in [718]
//
// Revision 1.1  2004/08/19 21:00:41  anton
// cisstCommon: Major update of the class register and class services.  This
// follows the numerous meetings regarding the registration of classes, the
// dynamic creation and the runtime type information.  Please see the LaTeX
// documents "Base class and base tools".  Other minor changes include:
// *: Two macros have been introduced, CMN_DECLARE_SERVICES and
// CMN_IMPLEMENT_SERVICES.  This solves the issue #31.
// *: Removed the cumbersome registered/not registered status which was
// introduced to allow users to set preferences before the class was
// registered.  The new design forces the registration before "main()".  This
// solves the issue #29.
// *: LoD is now logLoD which tells a bit more about this Level Of Detail.
// *: Test programs have been updated too.
//
//
// ****************************************************************************

