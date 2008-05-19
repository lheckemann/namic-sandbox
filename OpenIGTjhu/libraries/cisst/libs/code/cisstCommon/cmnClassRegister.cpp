/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnClassRegister.cpp,v 1.14 2006/05/10 18:40:25 anton Exp $

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


#include <fstream>

#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnClassServices.h>


cmnClassRegister* cmnClassRegister::Instance(void) {
    // create a static variable
    static cmnClassRegister instance;
    return &instance;
}


const std::string * cmnClassRegister::RegisterInstance(cmnClassServicesBase* classServicesPointer,
                                                       const std::string & className)
{
    cmnClassServicesBase * existingServicesPointer = FindClassServicesInstance(className);
    
    // check if this class is already registered
    if (existingServicesPointer != NULL) {
        CMN_LOG(1) << "Class cmnClassRegister: The class " << className
                   << " is already registered.  You should not have this problem, this is a bug!" << std::endl;
    } else {
        std::pair<iterator, bool> insertionResult;
        EntryType newEntry(className, classServicesPointer);
        insertionResult = ServicesContainer.insert(newEntry);
        if (insertionResult.second) {
            CMN_LOG(2) << "Class cmnClassRegister: The class " << className
                       << " has been registered with Log LoD " << classServicesPointer->GetLoD() << std::endl;
            return &((*insertionResult.first).first);
        } else {
            CMN_LOG(1) << "Class cmnClassRegister: The class " << className
                       << " can not be inserted.  You should not have this problem, this is a bug!" << std::endl;            
        }
    }
    return 0;
}


std::string cmnClassRegister::ToStringInstance(void) const {
    std::stringstream outputStream;
    ToStream(outputStream);
    return outputStream.str();
}


bool cmnClassRegister::SetLoD(const std::string& name, LoDType lod) {
    // see if class is registered.  If so, copy preferences (lod)
    cmnClassServicesBase* classServicesPointer = FindClassServices(name);
    if (classServicesPointer != NULL) {
        classServicesPointer->SetLoD(lod);
        CMN_LOG(2)<< "Class cmnClassRegister::SetLoD(): The class " << classServicesPointer->GetName()
                  << " log LoD has been set to " << classServicesPointer->GetLoD() << std::endl;
    } else {
        // we need to warn the programmer
    CMN_LOG(1) << "Class cmnClassRegister::SetLoD(): The class " << name
                   << " is not registered (yet?) " << std::endl;
    }
    return false;
}



cmnClassServicesBase * cmnClassRegister::FindClassServicesInstance(const std::string & className) {
    const_iterator iterator;
    const const_iterator end = ServicesContainer.end();
    cmnClassServicesBase * result = NULL;
    iterator = ServicesContainer.find(className);
    if (iterator != end) {
            result = iterator->second;
            CMN_LOG(7) << "Class cmnClassRegister::FindClassServicesInstance(): Found class info for "
                       << className << std::endl; 
    } else {
        CMN_LOG(6) << "Class cmnClassRegister::FindClassServicesInstance(): Couldn't find class info for "
                   << className << std::endl; 
    }
    return result;
}


cmnClassServicesBase * cmnClassRegister::FindClassServicesInstance(const std::type_info & typeInfo) {
    const_iterator iterator;
    const const_iterator end = ServicesContainer.end();
    cmnClassServicesBase * result = NULL;
    iterator = ServicesContainer.begin();
    while ((iterator != end) && (result == NULL)) {
        if ((iterator->second)->TypeInfoPointer() == &typeInfo) {
            result = iterator->second;
            CMN_LOG(7) << "Class cmnClassRegister::FindClassServicesInstance(): Found class info for the given type_info"
                       << std::endl;
        }
        iterator++;
    }
    return result;
}


cmnGenericObject * cmnClassRegister::Create(const std::string& className) {
    cmnClassServicesBase * classServicesPointer = FindClassServices(className);
    if (classServicesPointer) {
        return (classServicesPointer->Create());
    }
    return NULL;
}


std::string cmnClassRegister::ToString(void) {
    return Instance()->ToStringInstance();
}


void cmnClassRegister::ToStream(std::ostream & outputStream) const {
    const_iterator iterator;
    const const_iterator end = ServicesContainer.end();
    outputStream << "Registered classes:";
    for (iterator = ServicesContainer.begin(); iterator != end; iterator++) {
        outputStream << " " << iterator->first;
    }
}    


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnClassRegister.cpp,v $
// Revision 1.14  2006/05/10 18:40:25  anton
// cisstCommon class register: Updated to use a std::map to store class services
// and added std::type_info to the class services for a more reliable type
// identification of templated classes.
//
// Revision 1.13  2006/03/07 04:28:07  anton
// cmnClassRegister: rename iterator types to match STL convention.
//
// Revision 1.12  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.11  2005/06/06 00:32:25  anton
// cmnClassRegister: Added ToString() method.
//
// Revision 1.10  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.9  2005/04/04 20:19:49  kapoor
// cmnClassRegister: Bug in SetLoD, trying to use NULL pointer.
//
// Revision 1.8  2004/10/14 16:14:55  anton
// cmnClassRegister and cmnObjectRegister: Removed useless std::endl in
// ToStream() methods.
//
// Revision 1.7  2004/09/01 21:12:02  anton
// cisstCommon: Major update of the class register and the logging system.
// The class register is now a singleton (as in "Design Patterns") and doesn't
// store any information related to the logging (i.e. global lod, multiplexer
// for the output).  The data related to the log is now regrouped in the
// singleton cmnLogger.  This code is still fairly experimental (but tested)
// and the documentation is missing.  These changes should solve the tickets
// #30, #38 and #46.
//
// Revision 1.6  2004/08/19 21:00:41  anton
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
// Revision 1.5  2004/05/11 16:34:43  kapoor
// Checked in **PREMATURE** dynamic object creation code. NO flames PLEASE
//
// Revision 1.4  2003/10/15 16:09:55  anton
// renamed macros to match CMN_, not cmn
//
// Revision 1.3  2003/09/09 18:50:14  anton
// changed from double quotes to <> for all #include
//
// Revision 1.2  2003/06/23 20:53:51  anton
// removed tabs
//
// Revision 1.1.1.1  2003/05/30 19:47:56  anton
// no message
//
//
// ****************************************************************************


