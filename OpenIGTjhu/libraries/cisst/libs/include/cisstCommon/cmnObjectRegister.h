/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnObjectRegister.h,v 1.12 2006/06/06 22:34:24 ofri Exp $

  Author(s):  Andy LaMora, Anton Deguet
  Created on: 2004-05-11

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


#ifndef _cmnObjectRegister_h
#define _cmnObjectRegister_h

/*!
  \file
  \brief Defines cmnObjectRegister
*/

#include <map>
#include <iostream>
#include <sstream>
#include <string>


#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnExport.h>


/*!
  \brief Object Register

  \ingroup cisstCommon

  The class register allows to register some objects by name.  Since
  the object register is implemented as a singleton, it allows to
  retrieve an object from anywhere.

  This mechanism can be used to share an object between different
  threads.  Another example is the embedding of a scripting language
  such as Python.  In this case, the programmer can use the Register()
  method to register an object with a given name.  Within the Python
  shell, it is then possible to retrieve by name a "pointer" on the
  same object.

  The main restriction is that all the registered objects must be
  derived from cmnGenericObject.
 */
class CISST_EXPORT cmnObjectRegister {

    typedef std::map<std::string, cmnGenericObject *> ContainerType;
    typedef ContainerType::iterator iterator;

    /*! Map of class pointers registered. */
    ContainerType ObjectContainer;

public:
    typedef ContainerType::const_iterator const_iterator;

private:
    
    /*! Instance specific implementation of Register.
      \sa Register */
    bool RegisterInstance(const std::string & objectName,
                          cmnGenericObject * objectPointer);


    /*! Instance specific implementation of Remove.
      \sa Remove */
    bool RemoveInstance(const std::string & objectName);


    /*! Instance specific implementation of FindObject

      \param objectName The name to look up.
      \sa FindObject
    */
    cmnGenericObject* FindObjectInstance(const std::string & objectName) const;


    /*! Instance specific implementation of FindName

      \param objectPointer The object to look up.
      \sa FindName
    */
    std::string FindNameInstance(cmnGenericObject * objectPointer) const;


    /*! Instance specific implementation of ToStream.
      \sa ToStream */
    void ToStreamInstance(std::ostream & outputStream) const;

    /*! Instance specific implementation of begin.
      \sa begin */
    const_iterator beginInstance(void) const {
        return ObjectContainer.begin();
    }

    /*! Instance specific implementation of end.
      \sa end */
    const_iterator endInstance(void) const {
        return ObjectContainer.end();
    }

protected:
    /*! Constructor.  The only constructor must be private in order to
      ensure that the object register is a singleton. */
    cmnObjectRegister() {};
    
    virtual ~cmnObjectRegister() {};

 public:

    /*! The object register is instantiated as a singleton.  To access
      the unique instantiation, one needs to use this static method.
      The instantiated object register is created at the first call of
      this method since it is a static variable declared in this
      method's scope. 

      \return A pointer to the object register. */
    static cmnObjectRegister * Instance(void);


    /*!  The Register method registers an object pointer with a given
      name in the static register.
   
      \param objectName The name given to the soon registered object.

      \param objectPointer The pointer to the cmnGenericObject object
      (or any derived class).
    
      \return True if successful, false if the object has already been
      registered or the name has already been used.
    */
    static inline bool Register(const std::string & objectName,
                                cmnGenericObject * objectPointer) {
        return Instance()->RegisterInstance(objectName, objectPointer);
    }
    

    /*! The remove method allows to "un-register" a registered object.
      It doesn't affect the object itself.

      \param objectName The name given to the object when it was registered.
      
      \return True if the object was actually registered, false if the
      object was not registered. */
    static inline bool Remove(const std::string & objectName) {
        return Instance()->RemoveInstance(objectName);
    }


    /*! Get the object by name. Returns null if the object is
      not registered.
    
      \param objectName The name to look up.

      \return The pointer to the cmnGenericObject object
      corresponding to the className, or null if not registered.
    */
    static inline cmnGenericObject * FindObject(const std::string & objectName) {
        return Instance()->FindObjectInstance(objectName);
    }


    /*! Get the name of an object. Returns "undefined" if the object is
      not registered.
    
      \param objectPointer The object to look up.
    */
    static inline std::string FindName(cmnGenericObject * objectPointer) {
        return Instance()->FindNameInstance(objectPointer);
    }


    /*! Print the register content to an STL string and returns a copy of this
      string */
    static std::string ToString(void) {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }


    /*! Print the register content to a text stream */
    static inline void ToStream(std::ostream & outputStream) {
        return Instance()->ToStreamInstance(outputStream);
    }


   /*! Begin const iterator. */
    static const_iterator begin(void) {
        return Instance()->beginInstance();
    }


    /*! End const iterator. */
    static const_iterator end(void) {
        return Instance()->endInstance();
    }

};




#endif // _cmnObjectRegister_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: cmnObjectRegister.h,v $
//  Revision 1.12  2006/06/06 22:34:24  ofri
//  cmn[Class|Object]Register : Eliminated the unnecessary 'const' keyword in
//  definitions of ContainerType.  This done for better compliance with STL and
//  correction of compilation error in VC7
//
//  Revision 1.11  2006/05/10 18:41:23  anton
//  cmnObjectRegister.h: Use "const" std::string for the std::map indexing.
//
//  Revision 1.10  2006/03/10 14:24:00  anton
//  cmnObjectRegister: Added begin() and end() to provide a way to browse the
//  registered objects.  Renamed Iterator and ConstIterator to match STL conventions
//
//  Revision 1.9  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.8  2005/05/19 19:29:00  anton
//  cisst libs: Added the license to cisstCommon and cisstVector
//
//  Revision 1.7  2004/10/25 13:52:05  anton
//  Doxygen documentation:  Cleanup all the useless \ingroup.
//
//  Revision 1.6  2004/10/22 18:07:06  anton
//  Doxygen: Made sure that all the classes are documented for Doxygen.
//
//  Revision 1.5  2004/10/14 16:14:09  anton
//  cmnObjectRegister: Added a ToString() method.
//
//  Revision 1.4  2004/10/06 19:36:29  anton
//  cmnObjectRegister: Improved and tested version with icc, gcc, .net (2000/
//  2003).  I decided to greatly simplify the code not registering the object
//  register in the class register (it was twisted, complex and useless).
//
//  Revision 1.3  2004/10/05 22:01:22  anton
//  cisstCommon: Rewrote cmnObjectRegister.  This code is untested.
//
//  Revision 1.2  2004/05/11 18:55:24  alamora
//  no pertinent revisions
//
//  Revision 1.1  2004/04/11 15:18:45  alamora
//  Initial version added to cisst
//
//
// ****************************************************************************
