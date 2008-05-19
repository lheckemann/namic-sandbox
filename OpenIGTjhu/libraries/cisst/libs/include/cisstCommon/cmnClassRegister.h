/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnClassRegister.h,v 1.29 2006/07/13 22:07:14 pkaz Exp $

  Author(s):  Alvin Liem, Anton Deguet
  Created on: 2002-08-01

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
  \brief Class register definitions and log macros.
*/

#ifndef _cmnClassRegister_h
#define _cmnClassRegister_h

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <typeinfo>
#include <fstream>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnExport.h>


/*!  This is the main macro used to register a class. Every class
  which needs to be registered should include this macro within its
  declaration AND be derived from cmnGenericObject.  It must be used
  in conjunction with #CMN_IMPLEMENT_SERVICES.
  
  To declare a registered class without dynamic creation, the file
  myClass.h should have:

  \code
    class myClass: public cmnGenericObject {
       #CMN_DECLARE_SERVICES(myClass, CMN_NO_DYNAMIC_CREATION);
     public:
       ....
     };
  \endcode

  To implement the registration (required step), the file myClass.cpp
  must have:

  \code
    #CMN_IMPLEMENT_SERVICES(myClass, 3);
  \endcode

  The second parameter (in our example: 3) is the default level of
  detail to filter the log for this given class.  For more details
  regarding the logging in cisst, see #CMN_LOG_CLASS and cmnLogger.

  \param className The name to associate with this class.  This MUST
  be the name of the class itself.
  
  \param hasDynamicCreation Set this parameter to
  #CMN_DYNAMIC_CREATION to enable dynamic creation and
  #CMN_NO_DYNAMIC_CREATION to disable dynamic creation.  Dynamic
  creation requires a public default constructor.
 */
#define CMN_DECLARE_SERVICES_INTERNAL(hasDynamicCreation, lod, export) \
    public: \
      enum {HAS_DYNAMIC_CREATION = hasDynamicCreation}; \
      enum {INITIAL_LOD = lod}; \
      export static cmnClassServicesBase * ClassServices(void); \
      virtual cmnClassServicesBase * const Services(void) const \
      { \
          return this->ClassServices(); \
      } \
    private: \
      static cmnClassServicesBase * ClassServicesPointer;

#define CMN_DECLARE_SERVICES(hasDynamicCreation, lod) \
        CMN_DECLARE_SERVICES_INTERNAL(hasDynamicCreation, lod, )

#define CMN_DECLARE_SERVICES_EXPORT(hasDynamicCreation, lod) \
        CMN_DECLARE_SERVICES_INTERNAL(hasDynamicCreation, lod, CISST_EXPORT)


#define CMN_DECLARE_SERVICES_INSTANTIATION(className) \
template<> CISST_EXPORT \
cmnClassServicesBase * cmnClassServicesInstantiate<className>(void);

   
/*!  This is the main macro used to implement the registration of a
  class.  It must be used in conjunction with #CMN_DECLARE_SERVICES.
  See #CMN_DECLARE_SERVICES for futher details.
  
  \param className See #CMN_DECLARE_SERVICES.  It is very important to
  match the name provided in #CMN_DECLARE_SERVICES.

  \param lod The default Level of Detail used for this class.
*/
#define CMN_IMPLEMENT_SERVICES(className) \
cmnClassServicesBase * className::ClassServices(void) \
{ \
    static cmnClassServicesBase * classServices = cmnClassServicesInstantiate<className>(); \
    return classServices; \
} \
cmnClassServicesBase * className::ClassServicesPointer = className::ClassServices(); \
template<> \
cmnClassServicesBase * cmnClassServicesInstantiate<className>(void) \
{ \
    static cmnClassServices<className::HAS_DYNAMIC_CREATION, className> classServices(#className, \
                                                                                      &typeid(className), \
                                                                                      className::INITIAL_LOD); \
    return static_cast<cmnClassServicesBase *>(&classServices); \
}

#define CMN_IMPLEMENT_SERVICES_TEMPLATED(className) \
template<> \
cmnClassServicesBase * className::ClassServices(void) \
{ \
    static cmnClassServicesBase * classServices = cmnClassServicesInstantiate<className>(); \
    return classServices; \
} \
template<> \
cmnClassServicesBase * className::ClassServicesPointer = className::ClassServices(); \
template<> \
cmnClassServicesBase * const className::Services(void) const \
{ \
   return this->ClassServices(); \
} \
template<> \
cmnClassServicesBase * cmnClassServicesInstantiate<className>(void) \
{ \
    static cmnClassServices<className::HAS_DYNAMIC_CREATION, className> classServices(#className, \
                                                                                      &typeid(className), \
                                                                                      className::INITIAL_LOD); \
    return static_cast<cmnClassServicesBase *>(&classServices); \
}


/* forward declarations */
class cmnClassServicesBase;
class cmnGenericObject;


/*!
  \brief Main register for classes.

  \ingroup cisstCommon
 
  This class handles the registration of classes.  The registration
  process allows to retrieve by name some information about a specific
  class from a centralized point.  The current version of the class
  register allows to:

  - Create dynamically an object based on a name provided as a string.
    This feature is required for the deserialization process (from a
    buffer and a name, create a new object of a given type defined by
    a string).

  - Modify the logging level of detail for a given class.  See
    cmnLogger for more details regarding the logging in cisst.

  Since the dynamic creation requires a base class, we have introduced
  the cmnGenericObject class in cisst.  One can only register classes
  derived from cmnGenericObject.  The registration requires the use of
  #CMN_DECLARE_SERVICES and #CMN_IMPLEMENT_SERVICES.
 
  Here are the details of the implementation. The macro
  #CMN_DECLARE_SERVICES defines a method called Services() for each
  registered class. This method returns a pointer on a static data
  member of type cmnClassServices. This cmnClassServices object is a
  unique identifier for that class. While the reasons for writing a
  method which returns a pointer on a static data member might seem
  confusing, they are actually quite simple. This method is used to
  ensure that the data member is initialized the first time it is
  used. This is done to avoid problems related to the order of
  instantiation of the static data members.
 
  When the Services() method defined in the #CMN_IMPLEMENT_SERVICES
  macro is called, the cmnClassServices object registers itself with
  the static class register. This allows us to keep a single list of
  all the classes that exist in a particular program. The register is
  implemented as a vector of pointers to the static cmnClassServices
  objects that exist throughout the program. It should be noted that
  the cmnClassRegister::FindClassServices() method performs a linear
  search on the vector and should be used sparingly.

  It is important to note that all class to be registered will be
  registered by the time the <code>main()</code> function is called.
  Since we rely on the creation of global static objects for the
  registration and the order of instantiation of these objects before
  <code>main()</code> is unknown, class might be register or not
  before the <code>main()</code> execution.
 */
class CISST_EXPORT cmnClassRegister {
 public:
    /*! Type used to define the Log Level of Detail */
    typedef cmnLogger::LoDType LoDType;

    /*! Simple typedefs to ease the declaration of iterators */
    typedef std::map<std::string, cmnClassServicesBase*> ServicesContainerType;
    typedef ServicesContainerType::value_type EntryType;

    typedef ServicesContainerType::const_iterator const_iterator;
    typedef ServicesContainerType::const_reverse_iterator const_reverse_iterator;

 private:
    typedef ServicesContainerType::iterator iterator;
    typedef ServicesContainerType::reverse_iterator reverse_iterator;

    /*! List of class services registered. */
    ServicesContainerType ServicesContainer;

 public:
    /*! Instance specific implementation of FindClassServices.

      \param className The name to look up.
      \sa FindClassServices
    */
    cmnClassServicesBase* FindClassServicesInstance(const std::string & className);

    cmnClassServicesBase* FindClassServicesInstance(const std::type_info & typeInfo);

    /*! Instance specific implementation of Register.
      \sa Register */
    const std::string *
    RegisterInstance(cmnClassServicesBase * classServicesPointer,
                     const std::string & className);

    /*! Instance specific implementation of ToString.
      \sa ToString */
    std::string ToStringInstance(void) const;

    /*! Instance specific implementation of begin.
      \sa begin */
    const_iterator beginInstance(void) const {
        return ServicesContainer.begin();
    }

    /*! Instance specific implementation of end.
      \sa end */
    const_iterator endInstance(void) const {
        return ServicesContainer.end();
    }

protected:
    /*! Constructor.  The only constructor must be private in order to
      ensure that the class register is a singleton. */
    cmnClassRegister() {};

 public:

    /*! The class register is instantiated as a singleton.  To access
      the unique instantiation, one needs to use this static method.
      The instantiated class register is created at the first call of
      this method since it is a static variable declared in this
      method's scope. 

      \return A pointer to the class register. */
    static cmnClassRegister* Instance(void);


    /*!  The Register method registers a class pointer in the static
      register. It MUST NOT be used directly.  It is used by the
      #CMN_DECLARE_SERVICES macro.
   
      \param classServicesPointer The pointer to the cmnClassServices
      object.
    
      \return bool True if successful, false if the class has not been
      registered (e.g. one can not register twice).  This might happen
      if a programmer doesn't give the right string name for the class
      to be registered with the macros #CMN_DECLARE_SERVICES and
      #CMN_IMPLEMENT_SERVICES.
    */
    static inline const std::string *
    Register(cmnClassServicesBase *classServicesPointer,
             const std::string & className)
    {
        return Instance()->RegisterInstance(classServicesPointer,
                                            className);
    }


    /*! The SetLoD method allows the user to specify the lod for a
      specific class. It checks to see if the class is registered. If
      so, it updates the cmnClassServices object directly. Otherwise,
      it log a warning message.
    
      \param className The name of the class.
      \param lod The level of detail.
    
      \return bool True if the class is registered.
    */
    static bool SetLoD(const std::string& className, LoDType lod);


    /*! Get the class services by name. Returns null if the class is
      not registered.
    
      \param className The name to look up.

      \return The pointer to the cmnClassServicesBase object
      corresponding to the className, or null if not registered.
    */
    static inline cmnClassServicesBase* FindClassServices(const std::string& className) {
        return Instance()->FindClassServicesInstance(className);
    }

    static inline cmnClassServicesBase* FindClassServices(const std::type_info& typeInfo) {
        return Instance()->FindClassServicesInstance(typeInfo);
    }
    

    /*! Dynamic creation of objects.

    \param className The name of the class of the object to be created.
    
    \return A pointer on cmnGenericObject, NULL if the register
    doesn't have this class registered. */
    static cmnGenericObject* Create(const std::string& className);


    /*! Print the register content to an STL string and returns a copy
      of this string. */
    static std::string ToString(void);


    /*! Print the register content to a text stream. */
    void ToStream(std::ostream & outputStream) const;

 
    /*! Begin const iterator. */
    static const_iterator begin(void) {
        return Instance()->beginInstance();
    }


    /*! End const iterator. */
    static const_iterator end(void) {
        return Instance()->endInstance();
    }

}; 


#include <cisstCommon/cmnClassServices.h>


#endif // _cmnClassRegister_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnClassRegister.h,v $
// Revision 1.29  2006/07/13 22:07:14  pkaz
// cmnClassRegister.h: Added CMN_DECLARE_SERVICES_EXPORT.
//
// Revision 1.28  2006/06/06 22:34:24  ofri
// cmn[Class|Object]Register : Eliminated the unnecessary 'const' keyword in
// definitions of ContainerType.  This done for better compliance with STL and
// correction of compilation error in VC7
//
// Revision 1.27  2006/05/11 03:33:47  anton
// cisstCommon: New implementation of class services and registration to
// support templated classes.  This code has not been extensively tested and
// needs further work.
//
// Revision 1.26  2006/05/10 18:40:25  anton
// cisstCommon class register: Updated to use a std::map to store class services
// and added std::type_info to the class services for a more reliable type
// identification of templated classes.
//
// Revision 1.25  2006/03/10 14:22:22  anton
// cmnClassRegister: Removed List() method which returned a copy of the
// internal container.  Replaced by iterators.
//
// Revision 1.24  2006/03/07 04:27:51  anton
// cmnClassRegister: rename iterator types to match STL convention,
// added begin() and end(), still have to remove List() method.
//
// Revision 1.23  2006/01/03 03:33:04  anton
// cisstCommon Doxygen: Use \code \endcode instead of <pre></pre>, updated
// cmnTypeTraits to avoid all specialization to appear, updated CISST_DEPRECATED.
//
// Revision 1.22  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.21  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.20  2005/07/06 02:32:57  anton
// cmnGenericObject: virtual method Services is now const and returns a const
// pointer.
//
// Revision 1.19  2005/06/06 00:32:25  anton
// cmnClassRegister: Added ToString() method.
//
// Revision 1.18  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.17  2005/04/06 21:57:04  alamora
// New methods List and ListInstance return list of Register contents as
// a ServicesContainer
//
// Revision 1.16  2005/03/02 21:04:52  anton
// cisstCommon: Updated the registration code with optional dynamic creation
// to a version which compiles with gcc, icc and VCPP 7 (thanks to Ofri).
// Updated documentation.  See ticket #132.
//
// Revision 1.15  2005/03/02 03:52:42  anton
// cmnClassRegister: Experimental code to turn dynamic creation on or off
// based on a template parameter.  Works with VC++ 7.1.
//
// Revision 1.14  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.13  2004/10/05 22:01:22  anton
// cisstCommon: Rewrote cmnObjectRegister.  This code is untested.
//
// Revision 1.12  2004/09/03 19:32:03  anton
// cisstCommon: Documentation of the new log and class registration system,
// see check-in [718]
//
// Revision 1.11  2004/09/01 21:12:02  anton
// cisstCommon: Major update of the class register and the logging system.
// The class register is now a singleton (as in "Design Patterns") and doesn't
// store any information related to the logging (i.e. global lod, multiplexer
// for the output).  The data related to the log is now regrouped in the
// singleton cmnLogger.  This code is still fairly experimental (but tested)
// and the documentation is missing.  These changes should solve the tickets
// #30, #38 and #46.
//
// Revision 1.10  2004/08/19 21:00:41  anton
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
// Revision 1.9  2004/05/27 15:07:43  anton
// Corrected the very bad macros introduced by Ankur :-) At least the tests
// compile now!
//
// Revision 1.8  2004/05/11 16:34:43  kapoor
// Checked in **PREMATURE** dynamic object creation code. NO flames PLEASE
//
// Revision 1.7  2003/10/15 16:25:00  anton
// use CISST_COMPILER == CISST_GCC for CMN_LOG_HACK (still required for gcc 3.2)
//
// Revision 1.6  2003/10/15 16:09:07  anton
// renamed macros to match CMN_, not cmn
//
// Revision 1.5  2003/09/17 14:20:07  anton
// changed from cmnEXPORT to CISST_EXPORT
//
// Revision 1.4  2003/09/09 18:53:46  anton
// changed from double quotes to <> for all #include
//
// Revision 1.3  2003/07/31 18:15:33  anton
// Minor updates in documentation
//
// Revision 1.2  2003/06/23 18:50:13  anton
// remove tabs
//
// Revision 1.1.1.1  2003/05/30 19:47:58  anton
// no message
//
//
// ****************************************************************************
