/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnPath.h,v 1.8 2006/05/11 03:33:47 anton Exp $
  
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


/*!
  \file
  \brief Declaration of cmnPath
  \ingroup cisstCommon
*/

#ifndef _cmnPath_h
#define _cmnPath_h

#include <string>
#include <list>
#include <sstream>
#include <iostream>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnTokenizer.h>

// Always the last cisst include
#include <cisstCommon/cmnExport.h>


#if (CISST_OS == CISST_WINDOWS)
#  include <io.h>
#else
#  include <unistd.h>
#endif


/*!  \brief Search path to find a file.  This class contains a list of
  directories used to locate a file.

  \ingroup cisstCommon

  The directories can be added either at the head or the tail of the
  list.  This class doesn't check weither the directories in the path
  actually exist, mostly to allow a portable program (e.g. one program
  can add both "/usr/local/bin" and "C:\Program Files").

  As a convention, the separator between subdirectories should be a
  "/" (slash) and the separation between two directories in the path
  is ";" (semi-colon).  For example, "/bin;/usr/bin" will add both
  "/bin" and "/usr/bin".
*/
class CISST_EXPORT cmnPath: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

public:
    typedef std::list<std::string> PathType;
    typedef PathType::iterator iterator;
    typedef PathType::const_iterator const_iterator;

protected:
    PathType Path;
    cmnTokenizer Tokenizer;
    
    /*! Private method called by each constructor to configure the
      tokenizer. */
    void ConfigureTokenizer(void);
    
public:
    /*! Defines how to add a path to the search list. */
    enum {HEAD = true,
          TAIL = false
    };
    
    /*! Defines the mode to be used for a given file. */
#if (CISST_OS == CISST_WINDOWS)
    enum {EXIST = 00,
          WRITE = 02,
          READ = 04,
          EXECUTE = 04
    };
#else
    enum {READ = R_OK,
          WRITE = W_OK,
          EXECUTE = X_OK,
          EXIST = F_OK
    };
#endif

    cmnPath(void);

    /*! Create a search path from a string. */
    cmnPath(const std::string& path);

    /*! Destructor */
    virtual ~cmnPath(void) {}

    /*! Set the path from a string. */
    void Set(const std::string & path);

    /*! Add one or more directories to the path. */
    void Add(const std::string & path, bool head = HEAD);
    
    /*! Find the full name for a given file.
      \return The full path including the filename or an empty string.
    */
    std::string Find(const std::string & filename, short mode) const;

    /*! Remove the first occurence of a directory from the search list. */
    bool Remove(const std::string & directory);

    /*! Indicates if a given directory is in the search list. */
    bool Has(const std::string & directory) const;

    /*! Write the path to a stream. */
    void ToStream(std::ostream & outputStream) const;
};


// Add services instantiation
CMN_DECLARE_SERVICES_INSTANTIATION(cmnPath);


#endif // _cmnPath_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnPath.h,v $
// Revision 1.8  2006/05/11 03:33:47  anton
// cisstCommon: New implementation of class services and registration to
// support templated classes.  This code has not been extensively tested and
// needs further work.
//
// Revision 1.7  2005/10/06 16:56:37  anton
// Doxygen: Corrected errors and some warnings detected by Doxygen 1.4.3.
//
// Revision 1.6  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.5  2005/08/11 17:24:18  anton
// cmnGenericObject: Added ToString() and ToStream() methods for all classes
// derived from cmnGenericObject.  Updated cmnPath to reflect these changes.
// See also ticket #68.
//
// Revision 1.4  2005/07/06 02:38:07  anton
// cmnPath: Added some log messages and declared the tokenizer as a data
// member (see ticket #106).
//
// Revision 1.3  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.2  2005/04/26 13:16:35  anton
// cmnPath: Added code for MSVC7.1.  Tested with gcc (linux/darwin) as well.
//
// Revision 1.1  2005/04/26 03:28:14  anton
// cmnPath: Added preliminary version of this class (see ticket #106)
//
//
// ****************************************************************************
