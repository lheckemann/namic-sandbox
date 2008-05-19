/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnPath.cpp,v 1.6 2006/05/11 03:33:47 anton Exp $

  Author(s):  Anton Deguet
  Created on: 2005-04-18

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


#include <cisstCommon/cmnPath.h>


CMN_IMPLEMENT_SERVICES(cmnPath);


cmnPath::cmnPath() {
    ConfigureTokenizer();
    Path.clear();
}


cmnPath::cmnPath(const std::string & path) {
    ConfigureTokenizer();
    Set(path);
}


void cmnPath::ConfigureTokenizer(void) {
    Tokenizer.SetDelimiters(";");
    Tokenizer.SetEscapeMarkers("");
}


void cmnPath::Set(const std::string & path) {
    Path.clear();
    Add(path);
}


void cmnPath::Add(const std::string & path, bool head) {
    Tokenizer.Parse(path);
    CMN_LOG_CLASS(3) << "Adding the following \""
                     << path << "\" at the "
                     << (head ? "beginning" : "end") << std::endl;
    const char* const* tokens = Tokenizer.GetTokensArray(); 
    iterator position = head ? Path.begin() : Path.end();
    while (*tokens != NULL) {
        Path.insert(position, std::string(*tokens));
        // std::cout << *tokens << std::endl;
        *tokens ++;
    }
    CMN_LOG_CLASS(3) << "Path is now set to: " << ToString() << std::endl;
}
    

std::string cmnPath::Find(const std::string & filename, short mode) const {
    std::string fullName("");
    const_iterator iter = Path.begin();
    const const_iterator end = Path.end(); 
    while (iter != end) {
        fullName = (*iter) + "/" + filename;
        if (access(fullName.c_str(), mode) == 0) {
            break;
        }
        ++iter;
    }
    if (iter == end) {
        CMN_LOG_CLASS(3) << "Couldn't find file \"" << filename << "\" in path " << ToString() << std::endl;
        return "";
    }
    CMN_LOG_CLASS(3) << "Found \"" << fullName << "\" in path " << ToString() << std::endl;
    return fullName;
}


bool cmnPath::Remove(const std::string & directory) {
    iterator iter;
    iter = std::find(Path.begin(), Path.end(), directory);
    if (iter != Path.end()) {
        Path.erase(iter);
        CMN_LOG_CLASS(3) << "Directory " << directory << " found and removed from current path. "
                         << "Path is now set to: " << ToString() << std::endl;
        return true;
    }
    CMN_LOG_CLASS(3) << "Directory " << directory << " not found in current path." << std::endl;
    return false;  
}


bool cmnPath::Has(const std::string & directory) const {
    const_iterator iter;
    iter = std::find(Path.begin(), Path.end(), directory);
    if (iter != Path.end()) {
        return true;
    }
    return false;
}


void cmnPath::ToStream(std::ostream & outputStream) const {
    const unsigned int size = Path.size();
    unsigned int index;
    const_iterator iter = Path.begin();
    for (index = 0; index < size; index++) {
        outputStream << *iter;
        if (index < (size - 1)) {
            outputStream << ";";
        }
        iter++;
    }
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnPath.cpp,v $
// Revision 1.6  2006/05/11 03:33:47  anton
// cisstCommon: New implementation of class services and registration to
// support templated classes.  This code has not been extensively tested and
// needs further work.
//
// Revision 1.5  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.4  2005/08/11 17:24:18  anton
// cmnGenericObject: Added ToString() and ToStream() methods for all classes
// derived from cmnGenericObject.  Updated cmnPath to reflect these changes.
// See also ticket #68.
//
// Revision 1.3  2005/07/06 02:38:07  anton
// cmnPath: Added some log messages and declared the tokenizer as a data
// member (see ticket #106).
//
// Revision 1.2  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.1  2005/04/26 03:28:14  anton
// cmnPath: Added preliminary version of this class (see ticket #106)
//
//
// ****************************************************************************


