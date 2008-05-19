/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnObjectRegister.cpp,v 1.9 2006/03/10 14:24:00 anton Exp $

  Author(s):  Anton Deguet
  Created on: 2004-10-05

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


#include <cisstCommon/cmnObjectRegister.h>


cmnObjectRegister* cmnObjectRegister::Instance(void) {
    // create a static variable
    static cmnObjectRegister instance;
    return &instance;
}


bool cmnObjectRegister::RegisterInstance(const std::string & objectName,
                                         cmnGenericObject * objectPointer) {
    // try to find this object first
    iterator what = ObjectContainer.find(objectName);
    if (what == ObjectContainer.end()) {
        // verify that the pointer itself is not registered
        const iterator end = ObjectContainer.end();
        iterator iter = ObjectContainer.begin();
        iterator found = end;
        while ((iter != end) && (found == end)) {
            if (iter->second == objectPointer) {
                found = iter;
            }
            iter++;
        }
        if (found != end) {
            // pointer already registered
            CMN_LOG(1) << "class cmnObjectRegister: Registration failed.  There is already a registered object with the address: "
                       << objectPointer
                       << " (name: " << found->first << ")" << std::endl;
            return false;
        } else {
            // actually register
            ObjectContainer[objectName] = objectPointer;
            return true;
        }
    } else {
        // name already used
        CMN_LOG(1) << "class cmnObjectRegister: Registration failed.  There is already a registered object with the name: "
                   << objectName << std::endl;
        return false;
    }
    return false;
}


bool cmnObjectRegister::RemoveInstance(const std::string & objectName) {
    if (ObjectContainer.erase(objectName) == 0) {
        CMN_LOG(1) << "class cmnObjectRegister: " << objectName
                   << " can not be removed from the register since it is not registered" 
                   << std::endl;
        return false;
    }
    return true;
}


cmnGenericObject* cmnObjectRegister::FindObjectInstance(const std::string& objectName) const {
    cmnGenericObject * result = NULL;
    const_iterator what = ObjectContainer.find(objectName);
    if (what != ObjectContainer.end()) {
        result = what->second;
    }
    return result;
}


std::string cmnObjectRegister::FindNameInstance(cmnGenericObject * objectPointer) const {
    const const_iterator end = ObjectContainer.end();
    const_iterator iter = ObjectContainer.begin();
    const_iterator found = end;
    while ((iter != end) && (found == end)) {
        if (iter->second == objectPointer) {
            return iter->first;
        }
        iter++;
    }
    return "undefined";
}


void cmnObjectRegister::ToStreamInstance(std::ostream & outputStream) const {
    const_iterator iterator;
    const const_iterator end = ObjectContainer.end();
    for (iterator = ObjectContainer.begin(); iterator != end; iterator++) {
        outputStream << " " << iterator->first
                     << " (" << ((iterator->second)->Services())->GetName() << ")";
    }
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnObjectRegister.cpp,v $
// Revision 1.9  2006/03/10 14:24:00  anton
// cmnObjectRegister: Added begin() and end() to provide a way to browse the
// registered objects.  Renamed Iterator and ConstIterator to match STL conventions
//
// Revision 1.8  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.7  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.6  2005/02/03 19:06:55  anton
// cmnObjectRegister.cpp: Removed commented line.
//
// Revision 1.5  2005/01/25 18:06:27  alamora
// ToStream no longer prints "Registered objects:"
// The IRE expects ToStream output in this format:
// var1 (var1_type) var2 (var2_type)
// Update the IRE when changes are made to ToStream
//
// Revision 1.4  2004/10/14 16:14:55  anton
// cmnClassRegister and cmnObjectRegister: Removed useless std::endl in
// ToStream() methods.
//
// Revision 1.3  2004/10/06 19:36:29  anton
// cmnObjectRegister: Improved and tested version with icc, gcc, .net (2000/
// 2003).  I decided to greatly simplify the code not registering the object
// register in the class register (it was twisted, complex and useless).
//
// Revision 1.2  2004/10/05 22:01:22  anton
// cisstCommon: Rewrote cmnObjectRegister.  This code is untested.
//
//
// ****************************************************************************

