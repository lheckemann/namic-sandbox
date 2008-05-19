/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnXMLPath.cpp,v 1.15 2006/05/13 21:23:45 kapoor Exp $
  
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


#include <cisstCommon/cmnXMLPath.h>

#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
#include <xalanc/XalanSourceTree/XalanSourceTreeInit.hpp>

// required to register the class.  default level of detail is 1 as
// mentioned in header file
CMN_IMPLEMENT_SERVICES(cmnXMLPath);
CMN_IMPLEMENT_SERVICES(ParseErrorHandler);

using namespace XERCES_CPP_NAMESPACE;
using namespace XALAN_CPP_NAMESPACE;


// ***************************************************************************
// Begin with definitions of the error handling class.
void ParseErrorHandler::startElement(const XMLCh* const name,
                                     AttributeList& attributes)
{
    char* message = XMLString::transcode(name);
  CMN_LOG_CLASS(3) << "Element: "<< message << std::endl;
    XMLString::release(&message);
}

void ParseErrorHandler::warning(const SAXParseException& exc)
{
  warningCount++;
  char* message = XMLString::transcode(exc.getMessage());
    CMN_LOG_CLASS(2) << "Warning: " << message
                     << std::endl << "Line: " << exc.getLineNumber()
                     << std::endl <<  "Col: " << exc.getColumnNumber()
                     << std::endl;
}
void ParseErrorHandler::error(const SAXParseException &exc)
{
  errorCount++;
    char* message = XMLString::transcode(exc.getMessage());
    CMN_LOG_CLASS(1) << "Non-fatal Error: " << message
                     << std::endl << "Line: " << exc.getLineNumber()
                     << std::endl <<  "Col: " << exc.getColumnNumber()
                     << std::endl;
}
void ParseErrorHandler::fatalError(const SAXParseException& exc)
{
  fatalCount++;
    char* message = XMLString::transcode(exc.getMessage());
    CMN_LOG_CLASS(1) << "Fatal Error: " << message
                     << std::endl << "Line: " << exc.getLineNumber()
                     << std::endl <<  "Col: " << exc.getColumnNumber()
                     << std::endl;
}


// ***************************************************************************
// Begin definition of cmnXMLPath

cmnXMLPath::cmnXMLPath()
{
    try  {
        XMLPlatformUtils::Initialize();
        XPathEvaluator::initialize();
        // Initialize the XalanSourceTree subsystem...
        XalanSourceTreeInit    theSourceTreeInit;
        DOMSupport = new XalanSourceTreeDOMSupport();
        // We'll use these to parse the XML file.
        Liaison = new XalanSourceTreeParserLiaison(*DOMSupport);
        // Hook the two together...
        DOMSupport->setParserLiaison(Liaison);
        // Create an error handler...
        errorHandler =  new ParseErrorHandler();
  }
    catch (...)  {
        CMN_LOG_CLASS(1) << "Exception caught while creating cmnXMLPath." << std::endl;
    }
}



cmnXMLPath::~cmnXMLPath()
{
  // Delete the error handler object (since we used "new")
  delete errorHandler;
#if 0
    // in newer versions these should be called only one.
    XPathEvaluator::terminate();
    XMLPlatformUtils::Terminate();
#endif
}



void cmnXMLPath::SetInputSource(const char *filename)
{
    const XalanDOMString fileName(filename);
    
    // Create an input source that represents a local file...
    const LocalFileInputSource inputSource(fileName.c_str());
    // Parse the document...
    Document = Liaison->parseXMLStream(inputSource);
    if (Document == 0) {
        CMN_LOG_CLASS(1) << "An error occured while parsing \"" << filename << "\"" << std::endl;
    }
    CMN_ASSERT(Document != 0);
    PrefixResolver = new XalanDocumentPrefixResolver(Document);
}


void cmnXMLPath::SetInputSourceWithValidation(const char *filename)
{
    const XalanDOMString fileName(filename);
  CMN_LOG_CLASS(1) << "Ive been called with: " << filename << std::endl;
    
    // Create an input source that represents a local file...
    const LocalFileInputSource inputSource(fileName.c_str());
  // Turn on validation...
  Liaison->setUseValidation(true);
    
  // Set error handler (supresses validation exceptions)
  Liaison->setErrorHandler(errorHandler);
    
  // Parse the document...
  try {
    Document = Liaison->parseXMLStream(inputSource);
  } catch (...) {
    Document = 0;
  }
  CMN_LOG_CLASS(1) << "well, looks like I'm ok so far..." << std::endl;
  // Stay consistant with this class's violent-death-right-now method
  if ((errorHandler->getErrorCount()>0) || (errorHandler->getFatalCount() > 0))
    Document = 0;
    
  if (Document == 0) {
    CMN_LOG_CLASS(1) << "An error occured while parsing \"" << filename << "\"" << std::endl;
    }
  
  CMN_ASSERT(Document != 0);
    PrefixResolver = new XalanDocumentPrefixResolver(Document);
  CMN_LOG_CLASS(1) << "Done with validation"<< std::endl;
}


void cmnXMLPath::PrintValue(std::ostream &out, const char *context, const char *XPath)
{
    CharVectorType str;
    if (GetXMLValue(context, XPath, str) == false) {
        CMN_LOG_CLASS(2) << "Warning -- No nodes matched the location path \"" << XPath
                         << "\"" << std::endl;
    } else {
        out << &(str.front()) << std::endl;
    }
}


bool cmnXMLPath::GetXMLValue(const char *context, const char *XPath, std::string &storage)
{
    CharVectorType tmpStorage;
    if (GetXMLValue(context, XPath, tmpStorage)) {
        storage = &(tmpStorage.front());
        return true;
    }
    return false;
}

#include <xalanc/XPath/XNodeSetBase.hpp>
bool cmnXMLPath::GetXMLValue(const char *context, const char *XPath, CharVectorType  &storage)
{
    bool result = false;
    try  {
        XPathEvaluator  evaluator;
        // OK, let's find the context node...
        XalanNode* const contextNode = evaluator.selectSingleNode(*DOMSupport,
                                                                  Document,
                                                                  XalanDOMString(context).c_str(),
                                                                  *PrefixResolver);
        
        if (contextNode == 0) {
            CMN_LOG_CLASS(1) << "Can't find context \"" << context << "\"" << std::endl;
            return false;
        } else {
            // OK, let's evaluate the expression...
            XObjectPtr resultValue(evaluator.evaluate(*DOMSupport,
                                                      contextNode,
                                                      XalanDOMString(XPath).c_str(),
                                                      *PrefixResolver));
            
            CMN_ASSERT(resultValue.null() == false);
            CMN_LOG_CLASS(10) << "Retrieved type: " << resultValue->getType() << std::endl;
            if (resultValue->getType() == XObject::eTypeNodeSet) {
                int length = ((XNodeSetBase*)(resultValue.get()))->getLength();
                CMN_LOG_CLASS(10) << "Retrieved # of nodes: " << length << std::endl;
                if (length != 1) {
                    CMN_LOG_CLASS(3) << "Xpath " << XPath << " not found" << std::endl;
                    return false;
                } else {
                    TranscodeToLocalCodePage(resultValue->str(), storage, true);
                    CMN_LOG_CLASS(3) << "Retrieved \"" << storage << "\" for \"" << XPath
                        << "\" in context \"" << context << "\"" << std::endl;
                    return true;
                }
            }
            return false;
        }
    }
    catch (...) {
        CMN_LOG_CLASS(1) << "Exception caught while retrieving  \""
            << XPath << "\" in context \"" << context << "\""
            << std::endl;
        return false;
    }
    return result;
}


bool cmnXMLPath::GetXMLValue(const char *context, const char *XPath, double &value)
{
    bool result = false;
    try {
        XPathEvaluator  evaluator;
        // OK, let's find the context node...
        XalanNode* const contextNode = evaluator.selectSingleNode(*DOMSupport,
                Document,
                XalanDOMString(context).c_str(),
                *PrefixResolver);

        if (contextNode == 0) {
            CMN_LOG_CLASS(1) << "Can't find context \"" << context << "\"" << std::endl;
            return false;
        } else {
            // OK, let's evaluate the expression...
            XObjectPtr resultValue(evaluator.evaluate(*DOMSupport,
                        contextNode,
                        XalanDOMString(XPath).c_str(),
                        *PrefixResolver));

            CMN_ASSERT(resultValue.null() == false);
            CMN_LOG_CLASS(10) << "Retrieved type: " << resultValue->getType() << std::endl;
            if (resultValue->getType() == XObject::eTypeNodeSet) {
                int length = ((XNodeSetBase*)(resultValue.get()))->getLength();
                CMN_LOG_CLASS(10) << "Retrieved # of nodes: " << length << std::endl;
                if (length != 1) {
                    value = 0;
                    CMN_LOG_CLASS(3) << "Xpath " << XPath << " not found" << std::endl;
                    return false;
                } else {
                    value = resultValue->num();
                    CMN_LOG_CLASS(3) << "Retrieved \"" << value << "\" for \"" << XPath
                        << "\" in context \"" << context << "\"" << std::endl;
                    return true;
                }
            }
            return false;

        }
    }
    catch (...) {
        CMN_LOG_CLASS(1) << "Exception caught while retrieving \""
                         << XPath << "\" in context \"" << context << "\""
                         << std::endl;
        return false;
    }
    return result;
}



bool cmnXMLPath::GetXMLValue(const char *context, const char *XPath, int &value)
{
    //! Ah, a simple way to get an int
    double tmp;
    bool result = GetXMLValue(context, XPath, tmp);
    value = (int) tmp;
    return result;
}



bool cmnXMLPath::GetXMLValue(const char *context, const char *XPath, bool &value)
{
    bool result = false;
    try  {
        XPathEvaluator  evaluator;
        // OK, let's find the context node...
        XalanNode* const contextNode = evaluator.selectSingleNode(*DOMSupport,
                Document,
                XalanDOMString(context).c_str(),
                *PrefixResolver);

        if (contextNode == 0) {
            CMN_LOG_CLASS(1) << "Can't find context \"" << context << "\"" << std::endl;
            return false;
        } else {
            // OK, let's evaluate the expression...
            XObjectPtr resultValue(evaluator.evaluate(*DOMSupport,
                        contextNode,
                        XalanDOMString(XPath).c_str(),
                        *PrefixResolver));

            CMN_ASSERT(resultValue.null() == false);
            CMN_LOG_CLASS(10) << "Retrieved type: " << resultValue->getType() << std::endl;
            if (resultValue->getType() == XObject::eTypeNodeSet) {
                int length = ((XNodeSetBase*)(resultValue.get()))->getLength();
                CMN_LOG_CLASS(10) << "Retrieved # of nodes: " << length << std::endl;
                if (length != 1) {
                    CMN_LOG_CLASS(3) << "Xpath " << XPath << " not found" << std::endl;
                    value = false;
                    return false;
                } else {
                    value = resultValue->boolean();
                    CMN_LOG_CLASS(3) << "Retrieved \"" << value << "\" for \"" << XPath
                        << "\" in context \"" << context << "\"" << std::endl;
                    return true;
                }
            }

        }
    }
    catch (...) {
        CMN_LOG_CLASS(1) << "Exception caught while retrieving \""
                         << XPath << "\" in context \"" << context << "\""
                         << std::endl;
        return false;
    }
    return result;
}

bool cmnXMLPath::GetXMLValue(const char *context, const char *XPath, vctDouble3 &value)
{
  std::string storage;
  CharVectorType tmpStorage;
    
  if (GetXMLValue(context, XPath, tmpStorage)) {
    storage = &(tmpStorage.front());
    // If storage is empty, return vct3(0,0,0)
    if (storage.length() == 0) {
      value.Assign(0,0,0);
      return true;
    }
    // Else storage should contain 3 space delimited doubles
    // Note: we are NOT checking this; parser will ensure type integrity
    cmnTokenizer tokenizer;
    tokenizer.Parse(storage.c_str());
    const char* const* toks = tokenizer.GetTokensArray();
        
    std::string s("");
    std::stringstream os;
    unsigned int i;
    for (i = 0; i < value.size() ; i++) {
      s.assign(toks[i]);
      os << std::setprecision(12) << std::setw(24) << std::showpoint << s << std::endl; 
      os >> value[i];
    }
        
    return true;
    }
    
    return false;
}

bool cmnXMLPath::GetXMLValue(const char *context, const char *XPath, vctQuat &value)
{
  std::string storage;
  CharVectorType tmpStorage;
    
  if (GetXMLValue(context, XPath, tmpStorage)) {
    storage = &(tmpStorage.front());
    // If storage is empty, return vctQuat(0,0,0,0)
    if (storage.length() == 0) {
      value.Assign(0,0,0,0);
      return true;
    }
    // Else storage should contain 4 space delimited doubles
    // Note: we are NOT checking this; parser will ensure type integrity
    cmnTokenizer tokenizer;
    tokenizer.Parse(storage.c_str());
    const char* const* toks = tokenizer.GetTokensArray();
        
    std::string s("");
    std::stringstream os;
    unsigned int i;
    for (i = 0; i < value.size() ; i++) {
      s.assign(toks[i]);
      os << std::setprecision(12) << std::setw(24) << std::showpoint << s << std::endl; 
      os >> value[i];
    }
        
    CMN_LOG_CLASS(1) << "value: " << value << std::endl;
    return true;
    }
    
    return false;
}

bool cmnXMLPath::PrintErrors() {
  std::stringstream errorStr;
  errorStr << "fatal: " << errorHandler->getErrorCount() << std::endl;
  errorStr << "warnings: " << errorHandler->getWarningCount() << std::endl;
  errorStr << "errors: " << errorHandler->getErrorCount() << std::endl;
  errorStr << "err msg: " << errorHandler->getErrorMsg() << std::endl;
  std::cout << errorStr.str() << std::endl;
  CMN_LOG_CLASS(1) << errorStr.str() << std::endl;
  return true;
}



// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cmnXMLPath.cpp,v $
// Revision 1.15  2006/05/13 21:23:45  kapoor
// cisstCommon: cmnXPath destructor calls terminate. This API had been changed
// I guess and terminate should be called once per program? Not sure, should
// look further at a later time.
//
// Revision 1.14  2006/05/11 18:44:27  anton
// cmnXMLPath: Update for new class registration.
//
// Revision 1.13  2006/03/17 03:29:20  kapoor
// Fixed proper handling of missing keywords in XML file. See ticket #219.
//
// Revision 1.12  2005/11/09 21:46:49  anton
// cmnXMLPath: Moved using namespace to code file and used explicit name spaces
// where needed in header files.  Commented #define WIN32_LEAN_AND_MEAN which
// doesn't seem required anymore are well as a list of redundant methods (all
// methods added for SWIG).  If the problem persists with the SWIG generated
// wrappers we will have to figure out a solution in the wrapping process.
//
// Revision 1.11  2005/09/26 15:41:46  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.10  2005/08/04 04:19:34  alamora
// cisstCommon/cmnXMLPath: added methods for use with IRE only
//
// Revision 1.9  2005/08/01 22:01:51  alamora
// Major update.  Added method for Schema validation.  Added ParseErrorHandler
// for handling callbacks from the parser (thus supressing parse exceptions).
// Added GetXMLValue() overloads for vct3 and vctQuat
//
// Revision 1.8  2005/06/19 21:38:01  kapoor
// cmnXMLPath: std::vector<char> changed to CharVectorType to be compatible with newer version of XALAN.
//
// Revision 1.7  2005/05/19 19:29:01  anton
// cisst libs: Added the license to cisstCommon and cisstVector
//
// Revision 1.6  2005/02/28 22:38:14  anton
// cmnXMLPath: More log messages.
//
// Revision 1.5  2005/02/11 22:49:30  anton
// *** empty log message ***
//
// Revision 1.4  2005/02/11 19:52:01  anton
// cmnXMLPath: Now derives from cmnGenericObject and uses the log to display
// error messages.
//
// Revision 1.3  2004/10/30 00:51:40  kapoor
// Added method to get an integer.
//
// Revision 1.2  2004/06/25 14:47:22  ofri
// Removed conditional compilation usage from cmnXMLPath files, as this
// functionality is now in the CMake scripts (see #45, [610]).  Also moved
// header #include s from cmnXMLPath.h to .cpp (reduces dependencies)
// THIS IS A TEMPORARY FIX UNTIL XML POLICY IS DECIDED
//
// Revision 1.1  2004/05/27 15:02:11  anton
// Attempt to have an XML portable reader based on XMLPath.  This needs to be
// seriously reviewed.
//
// Revision 1.1  2004/04/08 06:14:40  kapoor
// Added a simple API for parsing XML
//
// ****************************************************************************
