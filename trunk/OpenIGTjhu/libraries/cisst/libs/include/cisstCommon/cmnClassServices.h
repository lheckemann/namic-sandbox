/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnClassServices.h,v 1.10 2006/05/10 18:40:25 anton Exp $

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
  \brief Defines cmnClassServices
*/

#ifndef _cmnClassServices_h
#define _cmnClassServices_h

#include <cisstCommon/cmnClassServicesBase.h>


/*! \name Possible values used in combination with #CMN_DECLARE_SERVICES.
 */
//@{
/* This parameter allows to either register a class without dynamic
  creation (just for logging, usually a "processing" class) or
  register the class with dynamic creation (the log is available as
  well).  Dynamic creation can be used to create an object of a given
  type based on its name provided as a string. */
#define CMN_DYNAMIC_CREATION true
#define CMN_NO_DYNAMIC_CREATION false
//@}


/*! This class is a helper for cmnClassServices.  Its goal is to
  specialize the method Create() based on the _hasDynamicCreation
  parameter to either create a new object of type _class (which
  requires a public default constructor) or do nothing and return
  NULL.  The later means that dynamic creation is disabled.
*/
template <bool _hasDynamicCreation>
class cmnConditionalObjectFactory; 


/*! Specialization of cmnConditionalObjectFactory with disabled
  dynamic creation.
*/
template<>
class cmnConditionalObjectFactory<CMN_NO_DYNAMIC_CREATION>
{
public:
    template <class _class>
    class ClassSpecialization {
    public:
        inline static cmnGenericObject* Create(void) {
            /* may be throw an exception instead */
            return NULL;
        }
    };
};


/*! Specialization of cmnConditionalObjectFactory with enabled dynamic
  creation.
*/
template<>
class cmnConditionalObjectFactory<CMN_DYNAMIC_CREATION>
{
public:
    template <class _class>
    class ClassSpecialization {
    public:
        inline static cmnGenericObject* Create(void) {
            return new _class;
        }
    };
};



/*!
  \brief Class services

  \sa cmnClassRegister cmnClassServicesBase
 */
template <bool _hasDynamicCreation, class _class>
class cmnClassServices: public cmnClassServicesBase {
 public:
    /*! Type of the base class, i.e. cmnClassServicesBase. */
    typedef cmnClassServicesBase BaseType;

    /* documented in base class */
    typedef BaseType::LoDType LoDType;

    /*!  Constructor. Sets the name of the class and the Level of Detail
      setting for the class.
      
      \param name The name to be associated with the class.
      \param lod The Log Level of Detail setting to be used with this class.
    */
    cmnClassServices(const std::string& className, const std::type_info * typeInfo, LoDType lod = 5):
        BaseType(className, typeInfo, lod)
    {}

    /* documented in base class */
    virtual cmnGenericObject* Create(void) {
        typedef cmnConditionalObjectFactory<_hasDynamicCreation> FactoryType; 
        typedef typename FactoryType::template ClassSpecialization<_class> CreatorType;
        return CreatorType::Create();
    }
};


#endif // _cmnClassServices_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnClassServices.h,v $
// Revision 1.10  2006/05/10 18:40:25  anton
// cisstCommon class register: Updated to use a std::map to store class services
// and added std::type_info to the class services for a more reliable type
// identification of templated classes.
//
// Revision 1.9  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.8  2005/06/16 03:38:29  anton
// Port to gcc 3.4 (common, vector and numerical) tested with gcc 3.3 and MS
// VC 7.1 (.Net 2003).
//
// Revision 1.7  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.6  2005/03/02 21:04:52  anton
// cisstCommon: Updated the registration code with optional dynamic creation
// to a version which compiles with gcc, icc and VCPP 7 (thanks to Ofri).
// Updated documentation.  See ticket #132.
//
// Revision 1.5  2005/03/02 03:52:42  anton
// cmnClassRegister: Experimental code to turn dynamic creation on or off
// based on a template parameter.  Works with VC++ 7.1.
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

