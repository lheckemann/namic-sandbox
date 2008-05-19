#
# $Id: CMakeLists.txt,v 1.26 2006/02/16 00:25:16 ofri Exp $
#
# CMakeLists for cisstCommon
#
# --- begin cisst license - do not edit ---
# 
# CISST Software License Agreement(c)
# 
# Copyright 2005 Johns Hopkins University (JHU) All Rights Reserved.
# 
# This software ("CISST Software") is provided by The Johns Hopkins
# University on behalf of the copyright holders and
# contributors. Permission is hereby granted, without payment, to copy,
# modify, display and distribute this software and its documentation, if
# any, for research purposes only, provided that (1) the above copyright
# notice and the following four paragraphs appear on all copies of this
# software, and (2) that source code to any modifications to this
# software be made publicly available under terms no more restrictive
# than those in this License Agreement. Use of this software constitutes
# acceptance of these terms and conditions.
# 
# The CISST Software has not been reviewed or approved by the Food and
# Drug Administration, and is for non-clinical, IRB-approved Research
# Use Only.
# 
# IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO
# ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
# EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
# 
# THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY
# EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE, AND NON-INFRINGEMENT.
# 
# THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
# 
# --- end cisst license ---

# set the library name
SET(LIBRARY cisstCommon)

# all source files
SET(SOURCE_FILES
    cmnClassRegister.cpp
    cmnDataObject.cpp
    cmnGenericObject.cpp
    cmnLogger.cpp
    cmnObjectRegister.cpp
    cmnOutputMultiplexer.cpp
    cmnPortability.cpp
    cmnRandomSequence.cpp
    cmnPath.cpp
    cmnPrintf.cpp
    cmnTokenizer.cpp
    cmnTypeTraits.cpp
    )

# all header files
SET(HEADER_FILES
    cmnAssert.h
    cmnCallbackStreambuf.h
    cmnClassRegister.h
    cmnClassServicesBase.h
    cmnClassServices.h
    cmnConstants.h
    cmnDataObject.h
    cmnErrorCodeBase.h
    cmnErrorCode.h
    cmnExceptionBase.h
    cmnExport.h
    cmnExportMacros.h
    cmnGenericObject.h
    cmnLogger.h
    cmnLODMultiplexerStreambuf.h
    cmnLODOutputMultiplexer.h
    cmnMultiplexerStreambuf.h
    cmnMultiplexerStreambufProxy.h
    cmnObjectRegister.h
    cmnOutputMultiplexer.h
    cmnPortability.h
    cmnPrintf.h
    cmnRandomSequence.h
    cmnPath.h
    cmnThrow.h
    cmnTokenizer.h
    cmnTypeTraits.h
    )

# Conditional inclusion of cmnXMLPath in the cisstCommon project.
IF(USE_cmnXMLPath)
  SET(SOURCE_FILES ${SOURCE_FILES} cmnXMLPath.cpp)
  SET(HEADER_FILES ${HEADER_FILES} cmnXMLPath.h)
ENDIF(USE_cmnXMLPath)

CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)


#
# $Log: CMakeLists.txt,v $
# Revision 1.26  2006/02/16 00:25:16  ofri
# Modified CMakeLists files to include cmnPrintf and vctPrintf
#
# Revision 1.25  2006/01/14 08:43:40  pkaz
# cisstCommon:  added cmnCallbackStreambuf.h
#
# Revision 1.24  2005/11/30 15:28:01  anton
# cmnConstant.cpp: Removed file since to symbol was instantiated.
#
# Revision 1.23  2005/11/17 22:52:36  anton
# CMakeLists.txt for Xerces and Xalan: Cleanup the existing solution.  Still
# pending a final decision re. how to package Xerces and Xalan.
#
# Revision 1.22  2005/10/17 21:59:57  anton
# cisst: Introduced cmnPI and co.  Updated whole repository. See #131.
#
# Revision 1.21  2005/09/23 23:55:25  anton
# cisstCommon: Added inline templated function cmnThrow.
#
# Revision 1.20  2005/09/06 02:08:14  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.19  2005/08/11 17:24:18  anton
# cmnGenericObject: Added ToString() and ToStream() methods for all classes
# derived from cmnGenericObject.  Updated cmnPath to reflect these changes.
# See also ticket #68.
#
# Revision 1.18  2005/08/04 04:21:59  alamora
# cisstCommon/cmnXMLPath: Xalan/Xerces configuration has been moved
# up to the main CISST CMakeLists.txt.  Commented out in this version until an
# XML policy is decided.
#
# Revision 1.17  2005/07/01 04:00:03  anton
# cisstCommon: Removed macro CMN_LOG_HACK and class cmnOutputInhibitor as
# specified in tickets #67 and #146.
#
# Revision 1.16  2005/06/20 22:16:07  anton
# Interactive Research Environment: Moved all IRE code to cisstInteractive
#
# Revision 1.15  2005/05/19 19:29:01  anton
# cisst libs: Added the license to cisstCommon and cisstVector
#
# Revision 1.14  2005/04/26 03:28:14  anton
# cmnPath: Added preliminary version of this class (see ticket #106)
#
# Revision 1.13  2005/04/22 23:50:18  ofri
# Added class cmnTokenizer to the repository.  It can be use to break strings
# (commands, paths, etc.) into tokens.
#
# Revision 1.12  2005/03/30 03:25:36  anton
# cisstCommon CMakeLists.txt: Set IRE off by default and is available
# only if SWIG wrapping is on.
#
# Revision 1.11  2005/03/08 20:14:31  alamora
# Modified to include option for building cmnIRE.  cmnIRE requires Python.
# Seeks Advanced Variables already used by the Python SWIG option.
#
# Revision 1.10  2004/10/05 22:01:22  anton
# cisstCommon: Rewrote cmnObjectRegister.  This code is untested.
#
# Revision 1.9  2004/09/01 21:12:02  anton
# cisstCommon: Major update of the class register and the logging system.
# The class register is now a singleton (as in "Design Patterns") and doesn't
# store any information related to the logging (i.e. global lod, multiplexer
# for the output).  The data related to the log is now regrouped in the
# singleton cmnLogger.  This code is still fairly experimental (but tested)
# and the documentation is missing.  These changes should solve the tickets
# #30, #38 and #46.
#
# Revision 1.8  2004/08/20 14:53:01  anton
# cisstCommon: Forgot to add cmnClassServicesBase.cpp to the CVS repository.
#
# Revision 1.7  2004/08/19 21:00:41  anton
# cisstCommon: Major update of the class register and class services.  This
# follows the numerous meetings regarding the registration of classes, the
# dynamic creation and the runtime type information.  Please see the LaTeX
# documents "Base class and base tools".  Other minor changes include:
# *: Two macros have been introduced, CMN_DECLARE_SERVICES and
# CMN_IMPLEMENT_SERVICES.  This solves the issue #31.
# *: Removed the cumbersome registered/not registered status which was
# introduced to allow users to set preferences before the class was
# registered.  The new design forces the registration before "main()".  This
# solves the issue #29.
# *: LoD is now logLoD which tells a bit more about this Level Of Detail.
# *: Test programs have been updated too.
#
# Revision 1.6  2004/06/25 14:30:22  ofri
# Conditional inclusion of cmnXMLPath in the cisstCommon project, and adding
# user-defined paths to external XML libraries to enable building cmnXMLPath
# on multiple platforms.  The XML paths are exported through cisstBuild.cmake
# See ticket #45.  THIS IS A TEMPORARY FIX UNTIL THE XML POLICY IS DECIDED.
#
# Revision 1.5  2004/05/27 15:02:11  anton
# Attempt to have an XML portable reader based on XMLPath.  This needs to be
# seriously reviewed.
#
# Revision 1.4  2004/05/04 14:07:06  anton
# Added cmnDataObject
#
# Revision 1.3  2004/02/06 15:37:07  anton
# code/cisstVector/CMakeLists.txt
#
# Revision 1.2  2004/01/30 18:57:27  ofri
# Use macro CISST_ADD_LIBRARY_TO_PROJECT instead of CISST_ADD_LIBRARY
#
# Revision 1.1  2004/01/22 21:55:42  anton
# We are now using recursive CMakeLists.txt instead of a single root CMakeLists which
# used a macro to create all the libraries.  This should avoid useless recompilations
# with .Net and allow more flexibility for each library.
#
#
# Revision 1.5  2004/01/15 15:20:13  anton
# Added cmnVaTypes
#
# Revision 1.4  2003/11/14 22:03:43  anton
# Added preliminary version of error/exception classes
#
# Revision 1.3  2003/09/09 18:55:15  anton
# Added cmnPortability
#
# Revision 1.2  2003/08/28 17:14:49  ofri
# Added class cmnRandomSequence to the cisstCommon library,
#
# Revision 1.1.1.1  2003/05/30 19:47:59  anton
# no message
#
#
