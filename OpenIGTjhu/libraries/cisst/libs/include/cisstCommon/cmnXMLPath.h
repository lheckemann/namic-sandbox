/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnXMLPath.h,v 1.16 2006/05/11 18:44:27 anton Exp $
  
  Author(s):  Ankur Kapoor
  Created on: 2004-04-30

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
  \brief Defines XMLPath 
*/

#ifndef _cmnXMLPath_h
#define _cmnXMLPath_h

#include <xalanc/Include/PlatformDefinitions.hpp>
#if defined(XALAN_CLASSIC_IOSTREAMS)
#error CISST code requires the usage of new iostream header files
#endif

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnTokenizer.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnAssert.h>

#include <cisstVector/vctTypes.h>

#include <cassert>
#include <vector>
#include <string>

#include <iostream>

#include <xalanc/XPath/XObject.hpp>
#include <xalanc/XPath/XStringBase.hpp>
#include <xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp>

#include <xalanc/XalanSourceTree/XalanSourceTreeParserLiaison.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#include <cisstCommon/cmnExport.h>


// At some point, windows.h is going to be included (OS abstraction).
// This instructs the compiler to include only the bare minimum of
// Windows header files.  Specifically, it prevents namespace 
// collisions between MsXml and Xerces/Xalan.
// #define WIN32_LEAN_AND_MEAN

/*!
  \ingroup cisstCommon

  This class provides a simple but somewhat powerful XPath implementation
  for parsing and validating XML.  Validation is handed via XML Schemas.
  XPath query expressions are passed to a processor method (GetXMLValue),
  along with a context and typed output param reference, which processes 
  the query and returns results as the output parameter type.
  Error checking is limited:  
  ** SetInputSource() tries to catch as many exceptions as possible,
  and does no error checking whatsoever.  Data may be missing and
  misformated. Generates a GENERAL PROTECTION FAULT (via assertions) 
  on error.
  ** SetInputSourceWithValidation() reports parsing error/warning 
  events through the ParseErrorHandler object.  Data is verified
  against an XML Schema named in the XML source; no other data
  checking is necessary (if the schema is complete).  Also 
  generates a GPF, until a new error policy is decided.
  This is very easy to use - set the input XML file to parse using
  SetInputSource, and Get any attribute value of a tag using
  GetXMLValue, which accepts a context and a XPath
  As an example consider the following config file
  <code> <config>
    <device name="mei">
      <axis name="x" axis-on-amp="1" logical-axis-no="1">
      <filter pgain="1000" ipgain="100" dgain="200" />
      <encoder out-units="mm" multiplier="0.123" />
    </axis>
    ...
    <axis name="rx" axis-on-amp="5" logical-axis-no="4">
       <filter pgain="100" ipgain="10" dgain="50"  />
       <encoder out-units="rad" multiplier="0.564" />
    </axis>
  </device>
  <task name="forceservo">
    <provides name="startservo" argtype="bool" />
    <provides name="stopservo" argtype="bool" />
    ...
    <state name="velocity" elementtype="std::vector[double]" />
    <state name="force" elementtype="cmnFNTNmData" />
    ...
  </task>
  </config>
  </code>
  Now the value of pgain for axis can be read by passing /config/device as context
  and axis/@name="x"/filter/@pgain
  Yes that simple!
  */



class CISST_EXPORT ParseErrorHandler : public XERCES_CPP_NAMESPACE::HandlerBase {
    /*! In Xerces, one is expected to provide error handlers.  Error
      handlers are derived from the base class HandlerBase, and
      implement callback methods used by Xerces when it encounters
      errors, warnings, or messages.  This is one such implementation.
      If no error handler is provided, Xerces will instead throw very
      explicit, and hard-to-catch, exceptions.  Currently configured
      only for use with XML Schema Validation errors.  This class
      resides outside cmnXMLPath in order to simplify script wrapping;
      logically, it may be better placed within. */
  CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

private:
  int errorCount;
  int fatalCount;
  int warningCount;
  std::string errorString;
  std::string fatalString;
  std::string warningString;
    
public:
  /*! Construct/Destructor */
  ParseErrorHandler():
        errorCount(0), 
        fatalCount(0), 
        warningCount(0), 
        errorString(""),
        fatalString(""),
        warningString("") {};
    
  ~ParseErrorHandler() { };
    
  /*! Triggered when an element is added  */
    void startElement(const XMLCh* const, XERCES_CPP_NAMESPACE::AttributeList&);
    
  /*! Triggered upon a warning of any kind  */
    void warning(const XERCES_CPP_NAMESPACE::SAXParseException&);
  /*! Get count */
  inline int getWarningCount() {
    return warningCount;
  }
  inline std::string getWarningMsg() {
    return warningString;
  }
    
  /*! Triggered upon error of any kind  */
  void error(const XERCES_CPP_NAMESPACE::SAXParseException&);
  /*! Get count */
  inline int getErrorCount() {
    return errorCount;
  }
  inline std::string getErrorMsg() {
    return errorString;
  }
  /*! Triggered upon fatal errors ONLY */
  void fatalError(const XERCES_CPP_NAMESPACE::SAXParseException&);
  /*! Get count */
  inline int getFatalCount() {
    return fatalCount;
  }
  inline std::string getFatalString() {
    return fatalString;
  }
};


CMN_DECLARE_SERVICES_INSTANTIATION(ParseErrorHandler);


class CISST_EXPORT cmnXMLPath: public cmnGenericObject {
    /*! Register this class with a default level of detail 1.  Levels
      of details are 1 for errors, 2 for warnings and 3 for very
      verbose.  */
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    
    /*! Xalan DOM Support structure */
    XALAN_CPP_NAMESPACE::XalanSourceTreeDOMSupport *DOMSupport;
    
    /*! Xalan tree parser */
    XALAN_CPP_NAMESPACE::XalanSourceTreeParserLiaison *Liaison;
    
    /*! Xerces DOM document */
    XALAN_CPP_NAMESPACE::XalanDocument *Document;
    
    /*! Document Prefix resolver */
    XALAN_CPP_NAMESPACE::XalanDocumentPrefixResolver *PrefixResolver;
    
  /* Xerces error handler */
  ParseErrorHandler* errorHandler;
    
public:
    /*! Constructor */
    cmnXMLPath();
    
    /*! Destructor */
    virtual ~cmnXMLPath();
  
    /*! Set the input source file2 */
    void SetInputSource(const char *filename);
  
  /*! Set the input source file, and validate it: expects a linked schema
      (see http://www.w3.org/TR/xmlschema-0/#schemaLocation)  */
  void cmnXMLPath::SetInputSourceWithValidation(const char *filename);
    
    /*! For debugging. Print the attribute value as a string on stream */
    void PrintValue(std::ostream &out, const char *context, const char *XPath);
    
    /*! Get the XPath result and cast it as FixedSizeVector(3) */
  bool GetXMLValue(const char * context, const char *XPath, vctDouble3 &value);
  
  /*! Get the XPath result and cast it as Quaternion */
  bool GetXMLValue(const char * context, const char *XPath, vctQuat &value);
    
  /*! Get the XPath result and cast it as string */
    bool GetXMLValue(const char * context, const char * XPath, bool &value);
    
    /*! Get the XPath result and cast it as string */
    bool GetXMLValue(const char * context, const char * XPath, int &value);
    
    /*! Get the XPath result and cast it as string */
    bool GetXMLValue(const char * context, const char * XPath, double &value);
    
    /*! Get the XPath result and cast it as string */
    bool GetXMLValue(const char * context, const char * XPath, XALAN_CPP_NAMESPACE::CharVectorType &storage);
    
    /*! Get the XPath result and cast it as string */
    bool GetXMLValue(const char * context, const char * XPath, std::string &storage);
    
  /*! SWIG sometimes has trouble with overloaded functions with reference params.  
      These inline helper methods are to assist xml config from the IRE. */
  inline
  bool Vec3RefGetXMLValue(const char* context, const char *XPath, vctDouble3& v) {
    return GetXMLValue(context, XPath, v);
  };

#if 0    
  inline
  vctDouble3 Vec3GetXMLValue(const char* c, const char* x, char* retval ) {
    vctDouble3 v;
    if (!GetXMLValue( c, x, v )) 
      strncpy(retval,"False", 5);
    else
      strncpy(retval, "True", 4);
    return v;
  };
  
  inline
  std::string CharGetXMLValue(const char* c, const char* x, char* retval ) {
    std::string st;
    if (!GetXMLValue( c, x, st )) 
      strncpy(retval,"False", 5);
    else
      strncpy(retval, "True", 4);
    return st;
  };
    
    inline
  double DblGetXMLValue(const char* c, const char* x, char* retval) {
    double result=0;
    if (!GetXMLValue( c, x, result ))
      strncpy(retval,"False", 5);
    else
      strncpy(retval, "True", 4);
    return result;
  }

    inline
  bool BoolGetXMLValue(const char* c, const char* x, char* retval) {
    bool result=false;
    if (!GetXMLValue( c, x, result ))
      strncpy(retval,"False", 5);
    else
      strncpy(retval, "True", 4);
    return result;
  }
#endif

  bool PrintErrors();
};


CMN_DECLARE_SERVICES_INSTANTIATION(cmnXMLPath);


#endif // _cmnXMLPath_h

 
// ****************************************************************************
//                              Change History                   
// ****************************************************************************
//
// $Log: cmnXMLPath.h,v $
// Revision 1.16  2006/05/11 18:44:27  anton
// cmnXMLPath: Update for new class registration.
//
// Revision 1.15  2005/11/09 21:46:49  anton
// cmnXMLPath: Moved using namespace to code file and used explicit name spaces
// where needed in header files.  Commented #define WIN32_LEAN_AND_MEAN which
// doesn't seem required anymore are well as a list of redundant methods (all
// methods added for SWIG).  If the problem persists with the SWIG generated
// wrappers we will have to figure out a solution in the wrapping process.
//
// Revision 1.14  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.13  2005/08/10 04:55:06  kapoor
// XMLPath: re-ordered to match declaration order
//
// Revision 1.12  2005/08/04 04:19:18  alamora
// cisstCommon/cmnXMLPath: added methods for use with IRE only
//
// Revision 1.11  2005/08/01 22:02:12  alamora
// Major update.  Added method for Schema validation.  Added ParseErrorHandler
// for handling callbacks from the parser (thus supressing parse exceptions).
// Added GetXMLValue() overloads for vct3 and vctQuat
//
// Revision 1.10  2005/07/31 20:48:07  alamora
// Added CISST_EXPORT for shared libs
//
// Revision 1.9  2005/06/19 21:46:36  kapoor
// cmnXMLPath: std::vector<char> changed to CharVectorType to be compatible with newer version of XALAN.
//
// Revision 1.8  2005/05/19 19:29:00  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.7  2005/03/03 18:36:54  anton
// All libraries: Updated calls to CMN_DECLARE_SERVICES to use CMN_{NO,}_DYNAMIC_CREATION
// (see ticket #132 for details).
//
// Revision 1.6  2005/02/11 22:49:30  anton
// *** empty log message ***
//
// Revision 1.5  2005/02/11 19:52:01  anton
// cmnXMLPath: Now derives from cmnGenericObject and uses the log to display
// error messages.
//
// Revision 1.4  2004/10/30 00:56:31  kapoor
// Added method to read an integer.
//
// Revision 1.3  2004/10/25 13:52:05  anton
// Doxygen documentation:  Cleanup all the useless \ingroup.
//
// Revision 1.2  2004/06/25 14:47:12  ofri
// Removed conditional compilation usage from cmnXMLPath files, as this
// functionality is now in the CMake scripts (see #45, [610]).  Also moved
// header #include s from cmnXMLPath.h to .cpp (reduces dependencies)
// THIS IS A TEMPORARY FIX UNTIL XML POLICY IS DECIDED
//
// Revision 1.1  2004/05/27 15:02:11  anton
// Attempt to have an XML portable reader based on XMLPath.  This needs to be
// seriously reviewed.
//
//
// Revision 1.1  2004/04/08 06:14:40  kapoor
// Added a simple API for parsing XML
//
// ****************************************************************************
