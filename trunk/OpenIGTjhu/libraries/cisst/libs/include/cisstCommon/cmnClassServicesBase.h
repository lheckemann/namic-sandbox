/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnClassServicesBase.h,v 1.8 2006/05/11 03:33:47 anton Exp $

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
  \brief Defines the cmnClassServicesBase
*/

#ifndef _cmnClassServicesBase_h
#define _cmnClassServicesBase_h


#include <string>
#include <typeinfo>

#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnExport.h>

/*!
  \brief Base class for class services
  
  \sa cmnClassServices
*/
class CISST_EXPORT cmnClassServicesBase {
public:
    /*! Type used to define the logging level of detail. */
    typedef cmnLogger::LoDType LoDType;

    /*!  Constructor. Sets the name of the class and the Level of Detail
      setting for the class.
      
      \param name The name to be associated with the class.
      \param lod The Log Level of Detail setting to be used with this class.
    */
    cmnClassServicesBase(const std::string & className, const std::type_info * typeInfo, LoDType lod = 5):
        TypeInfoMember(typeInfo),
        LoDMember(lod)
    {
        NameMember = cmnClassRegister::Register(this, className);
    }
    
    
    /*! Virtual destructor.  Does nothing. */
    virtual ~cmnClassServicesBase() {}


    /*! Create a new empty object of the same type as represented by
      this object.  This can be used whenever an object needs to be
      dynamically created (e.g. deserialization, object factory).
      This method uses the C++ "new" operator and the programmers
      should remember to use a matching "delete" if needed.
      
      \return a pointer to the newly created object or null if
      object cannot be created.
    */
    virtual cmnGenericObject* Create(void) = 0;   
    

    /*! Get the name associated with the class.
    
      \return The name of the class as a string.
    */
    inline const std::string & GetName(void) const {
        return (*NameMember);
    }

    inline const std::type_info * TypeInfoPointer(void) const {
        return TypeInfoMember;
    }


    /*! Get the log Level of Detail associated with the class.  This
      is the level used to filter the log messages.
    
      \return The log Level of Detail.
    */
    inline const LoDType & GetLoD(void) const {
        return LoDMember;
    }


    /*! Change the Level of Detail setting for the class.
      
    \param lod The log Level of Detail setting.
    */
    inline void SetLoD(LoDType newLoD) {
        LoDMember = newLoD;
    }


private:
    /*! The name of the class. */ 
    const std::string * NameMember;

    const std::type_info * TypeInfoMember;

    /*! The log Level of Detail. */
    LoDType LoDMember;
};



template <class _class>
cmnClassServicesBase * cmnClassServicesInstantiate(void);

 
#endif // _cmnClassServicesBase_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnClassServicesBase.h,v $
// Revision 1.8  2006/05/11 03:33:47  anton
// cisstCommon: New implementation of class services and registration to
// support templated classes.  This code has not been extensively tested and
// needs further work.
//
// Revision 1.7  2006/05/10 18:40:25  anton
// cisstCommon class register: Updated to use a std::map to store class services
// and added std::type_info to the class services for a more reliable type
// identification of templated classes.
//
// Revision 1.6  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.5  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.4  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.3  2004/09/03 19:32:03  anton
// cisstCommon: Documentation of the new log and class registration system,
// see check-in [718]
//
// Revision 1.2  2004/09/01 21:12:02  anton
// cisstCommon: Major update of the class register and the logging system.
// The class register is now a singleton (as in "Design Patterns") and doesn't
// store any information related to the logging (i.e. global lod, multiplexer
// for the output).  The data related to the log is now regrouped in the
// singleton cmnLogger.  This code is still fairly experimental (but tested)
// and the documentation is missing.  These changes should solve the tickets
// #30, #38 and #46.
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

