#
# $Id: CMakeLists.txt,v 1.17 2006/05/24 14:23:52 anton Exp $
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


# Backward compatibility
CMAKE_MINIMUM_REQUIRED(VERSION 2.0)
PROJECT(cisst)


# Custom CMake functions
INCLUDE(${cisst_SOURCE_DIR}/libs/cmake/cisstMacros.cmake)


# Output directories.
SET(LIBRARY_OUTPUT_PATH ${cisst_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${cisst_BINARY_DIR}/bin)

# Where to find afterwards
SET(cisst_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
SET(cisst_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")


# Options
OPTION(BUILD_TESTS "Build tests." OFF)
OPTION(BUILD_EXAMPLES "Build examples." OFF)

IF(UNIX)
  OPTION(BUILD_DOC "Build the documentation." OFF)
ENDIF(UNIX)

# Build static or dynamic libraries
OPTION(CISST_BUILD_SHARED_LIBS "Build cisst with shared libraries." OFF)
IF(CISST_BUILD_SHARED_LIBS)
  SET(IS_SHARED SHARED)
  SET(BUILD_SHARED_LIBS SHARED)
  IF(WIN32)
    ADD_DEFINITIONS(-DCISST_DLL)
  ENDIF(WIN32)
ELSE(CISST_BUILD_SHARED_LIBS)
  SET(IS_SHARED STATIC)
ENDIF(CISST_BUILD_SHARED_LIBS)


# Wrapping with swig for Python and/or Java
OPTION(CISST_HAS_SWIG_PYTHON "Build the Python wrappers for cisst with Swig." OFF)
OPTION(CISST_HAS_SWIG_JAVA "Build the Java wrappers for cisst with Swig." OFF)

IF(CISST_HAS_SWIG_PYTHON OR CISST_HAS_SWIG_JAVA)
  # Find Swig
  FIND_PACKAGE(SWIG REQUIRED)
  INCLUDE(${SWIG_USE_FILE})

  # Check that the main libraries are compiled as DLL for Windows
  IF(WIN32 OR APPLE)
    VARIABLE_REQUIRES(CISST_HAS_SWIG_PYTHON REQUIREMENT_MET CISST_BUILD_SHARED_LIBS)
  ENDIF(WIN32 OR APPLE)

  # Define the include directory for interface files
  SET(CISST_SWIG_INCLUDE_DIR ${cisst_SOURCE_DIR}/libs/wrapping)

  # Force to use exceptions
  SET(CMN_THROW_DOES_ABORT OFF CACHE BOOL "cmnThrow uses abort() instead of throw" FORCE)
  SET(CMN_ASSERT_THROWS_EXCEPTION ON CACHE BOOL "CMN_ASSERT throws an exception instead of abort" FORCE)

ENDIF(CISST_HAS_SWIG_PYTHON OR CISST_HAS_SWIG_JAVA)

IF(CISST_HAS_SWIG_PYTHON)
  # Find Python
  FIND_PACKAGE(PythonLibs REQUIRED)
  FIND_PACKAGE(PythonInterp REQUIRED)
  INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
ENDIF(CISST_HAS_SWIG_PYTHON)

IF(CISST_HAS_SWIG_JAVA)
  # Find JNI (Java Native Interface)
  FIND_PACKAGE(JNI REQUIRED)
  INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH})
ENDIF(CISST_HAS_SWIG_JAVA)



# TEMPORARY!!!  We must still decide on an XML policy.
# Conditional inclusion of cmnXMLPath in the entire CISST project.
# Note: to use trackers or robots, users are going to need XML anyway.
# cmnXMLPath depends on external libraries, which the user may or 
# may not have.  The user should specify path definitions to these
# external libraries if USE_cmnXMLPath is ON.
#
# This is a temporary fix to simplify building the libraries until
# an XML policy is decided on.
SET(USE_cmnXMLPath OFF CACHE BOOL "Include cmnXMLPath in the version (requires setting additional paths)")

IF(USE_cmnXMLPath)
  SET(XERCES_HEADER_ROOT "" CACHE PATH "Root of the Xerces library header files tree")
  SET(XERCES_LIB_ROOT "" CACHE PATH "Root of the Xerces library build tree")
  
  SET(XALAN_HEADER_ROOT "" CACHE PATH "Root of the Xalan library header files tree")
  SET(XALAN_LIB_ROOT "" CACHE PATH "Root of the Xalan library build tree")

  INCLUDE_DIRECTORIES(${XERCES_HEADER_ROOT} ${XALAN_HEADER_ROOT})
  LINK_DIRECTORIES(${XERCES_LIB_ROOT}/ ${XALAN_LIB_ROOT}/)
  
  IF(WIN32)
    SET(XERCES_LIBRARY "Xerces-C_2" CACHE STRING "Name of the Xerces library file")
    SET(XALAN_LIBRARY "Xalan-C_1" CACHE STRING "Name of the Xalan library file")
  ELSE(WIN32)
    SET(XERCES_LIBRARY "xerces-c" CACHE STRING "Name of the Xerces library file")
    SET(XALAN_LIBRARY "xalan-c" CACHE STRING "Name of the Xalan library file")
  ENDIF(WIN32)

  IF(WIN32)
    LINK_LIBRARIES(
        optimized ${XERCES_LIBRARY} optimized ${XALAN_LIBRARY}
        debug  "${XERCES_LIBRARY}D" debug "${XALAN_LIBRARY}D"
    )
  ELSE(WIN32)
    LINK_LIBRARIES(
        ${XERCES_LIBRARY} ${XALAN_LIBRARY}
    )
  ENDIF(WIN32)
ENDIF(USE_cmnXMLPath)
# End of cmnXMLPath section


# User options related to dependencies, eg: external libraries
# Check for C NETLIB
OPTION(CISST_HAS_CNETLIB "Use the netlib.org sets of numerical methods in C (provided on cisst.org)." OFF)
IF(CISST_HAS_CNETLIB)
  INCLUDE(${cisst_SOURCE_DIR}/libs/cmake/FindCNetlib.cmake)
  INCLUDE_DIRECTORIES(${CNETLIB_INCLUDE_DIR})
ENDIF(CISST_HAS_CNETLIB)

# Check for Fortran NETLIB
OPTION(CISST_HAS_CISSTNETLIB "Use the netlib.org sets of numerical methods in Fortran (provided on cisst.org)." OFF)
IF(CISST_HAS_CISSTNETLIB)
  INCLUDE(${cisst_SOURCE_DIR}/libs/cmake/FindCisstNetlib.cmake)
  INCLUDE_DIRECTORIES(${CISSTNETLIB_INCLUDE_DIR})
ENDIF(CISST_HAS_CISSTNETLIB)

# Variable to indicate that netlib is available, either version
IF(CISST_HAS_CNETLIB OR CISST_HAS_CISSTNETLIB)
  SET(CISST_HAS_NETLIB ON CACHE BOOL "cisst has a version of netlib (Computed with CISST_HAS_CNETLIB OR CISST_HAS_CISSTNETLIB)" FORCE)
ELSE(CISST_HAS_CNETLIB OR CISST_HAS_CISSTNETLIB)
  SET(CISST_HAS_NETLIB OFF CACHE BOOL "cisst has a version of netlib (Computed with CISST_HAS_CNETLIB OR CISST_HAS_CISSTNETLIB)" FORCE)
ENDIF(CISST_HAS_CNETLIB OR CISST_HAS_CISSTNETLIB)
MARK_AS_ADVANCED(CISST_HAS_NETLIB)


# Make sure not both version of NETLIB have been selected
IF(CISST_HAS_CNETLIB AND CISST_HAS_CISSTNETLIB)
  MESSAGE(FATAL_ERROR "You must choose either CISST_HAS_CNETLIB or CISST_HAS_CISSTNETLIB.")
ENDIF(CISST_HAS_CNETLIB AND CISST_HAS_CISSTNETLIB)

# Check for RTAI
OPTION(CISST_HAS_LINUX_RTAI "Use the RTAI real time extensions for Linux." OFF)
IF(CISST_HAS_LINUX_RTAI)
  INCLUDE(${cisst_SOURCE_DIR}/libs/cmake/FindRTAI.cmake)
  INCLUDE_DIRECTORIES(${RTAI_INCLUDE_DIR})
ENDIF(CISST_HAS_LINUX_RTAI)


# Determine if cmnThrow uses "throw" or "abort"
OPTION(CMN_THROW_DOES_ABORT "cmnThrow uses abort() instead of throw" OFF)
MARK_AS_ADVANCED(CMN_THROW_DOES_ABORT)
IF(CMN_THROW_DOES_ABORT)
  ADD_DEFINITIONS(-DCMN_THROW_DOES_ABORT)
ENDIF(CMN_THROW_DOES_ABORT)


# Determine if CMN_ASSERT should be disabled
OPTION(CMN_ASSERT_DISABLED "CMN_ASSERT is disabled, i.e. does not assert (for release mode)" OFF)
MARK_AS_ADVANCED(CMN_ASSERT_DISABLED)
IF(CMN_ASSERT_DISABLED)
  ADD_DEFINITIONS(-DCMN_ASSERT_DISABLED)
ENDIF(CMN_ASSERT_DISABLED)

# Determine if CMN_ASSERT should throw an exception instead of abort
OPTION(CMN_ASSERT_THROWS_EXCEPTION "CMN_ASSERT throws an exception instead of abort" OFF)
MARK_AS_ADVANCED(CMN_ASSERT_THROWS_EXCEPTION)
IF(CMN_ASSERT_THROWS_EXCEPTION)
  ADD_DEFINITIONS(-DCMN_ASSERT_THROWS_EXCEPTION)
ENDIF(CMN_ASSERT_THROWS_EXCEPTION)


# Put "." in the rpath of all CISST shared libraries.
LINK_DIRECTORIES(. ${LIBRARY_OUTPUT_PATH})

# Determine if we want to use rpath, no by default
IF(UNIX)
  OPTION(CMAKE_SKIP_RPATH "Use rpath to hard code the path to dynamic libraries." ON)
  MARK_AS_ADVANCED(FORCE CMAKE_SKIP_RPATH)
ENDIF(UNIX)


# Add include directory
SET(CISST_INCLUDE_DIR
    ${cisst_SOURCE_DIR}/libs/include
    ${cisst_BINARY_DIR}/libs/include)
INCLUDE_DIRECTORIES(${CISST_INCLUDE_DIR})


# To have more warnings with gcc
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


# Find native threads
FIND_PACKAGE(Threads REQUIRED)
LINK_LIBRARIES(${CMAKE_THREAD_LIBS_INIT})


# Which libraries to build
SET(CISST_LIBS_STABLE
    cisstCommon
    cisstVector
    )

SET(CISST_LIBS_UNSTABLE
    cisstNumerical
    cisstImage
    cisstInteractive
    cisstOSAbstraction
    cisstDeviceInterface
    cisstRealTime
    cisstTracker
    )

SET(CISST_LIBS
    ${CISST_LIBS_STABLE}
    ${CISST_LIBS_UNSTABLE})

# Build by default
FOREACH(lib ${CISST_LIBS_STABLE})
  OPTION(BUILD_LIBS_${lib} "Build ${lib}." ON)
ENDFOREACH(lib)

FOREACH(lib ${CISST_LIBS_UNSTABLE})
  OPTION(BUILD_LIBS_${lib} "Build ${lib}." OFF)
ENDFOREACH(lib)

# Finally, create lib targets
SUBDIRS(libs)

# List of libraries to be built 
FOREACH(lib ${CISST_LIBS})
  IF(BUILD_LIBS_${lib})
    SET(CISST_LIBRARIES ${lib} ${CISST_LIBRARIES})
  ENDIF(BUILD_LIBS_${lib})
ENDFOREACH(lib)

# Contional builds
IF(BUILD_TESTS)
  # Settings for CTest
  ENABLE_TESTING()
  MARK_AS_ADVANCED(BUILD_TESTING)
  INCLUDE(Dart)
  # cisst Tests
  SUBDIRS(tests)
ENDIF(BUILD_TESTS)

IF(BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF(BUILD_EXAMPLES)

IF(BUILD_DOC)
  SUBDIRS(doc)
ENDIF(BUILD_DOC)


# Create misc. config files
SET(CISST_BUILD_INCLUDE_DIRECTORIES ${CISST_INCLUDE_DIR})
SET(CISST_BUILD_DOXYGEN_DIRECTORY ${cisst_SOURCE_DIR}/libs/include)
SET(CISST_BUILD_LINK_DIRECTORIES ${cisst_LIBRARY_PATH})
SET(CISST_BUILD_CISSTNETLIB_LIBRARIES ${CISSTNETLIB_LIBRARIES} ${CISSTNETLIB_F95_LIBRARIES})
IF(NOT UNIX)
    SET(CISST_BUILD_CISSTNETLIB_LIBRARIES ${CISST_BUILD_CISSTNETLIB_LIBRARIES} ${CISSTNETLIB_GCC_LIBRARIES})
ENDIF(NOT UNIX)
CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/cmake/cisstBuild.cmake.in
               ${cisst_BINARY_DIR}/cisstBuild.cmake
               @ONLY)

SET(CISST_INSTALLED_INCLUDE_DIRECTORIES ${CMAKE_INSTALL_PREFIX}/include)
SET(CISST_INSTALLED_DOXYGEN_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include)
SET(CISST_INSTALLED_LINK_DIRECTORIES ${CMAKE_INSTALL_PREFIX}/lib)
CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/cmake/cisstInstalled.cmake.in
               ${cisst_BINARY_DIR}/cisstInstalled.cmake
               @ONLY)
INSTALL_FILES(/share/cisst/cmake
              ".cmake"
              ${cisst_BINARY_DIR}/cisstInstalled.cmake)


# Configuration file
SET(CISST_BINARY_DIR ${cisst_BINARY_DIR})
CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/include/cisstConfig.h.in
               ${cisst_BINARY_DIR}/libs/include/cisstConfig.h
               @ONLY)
INSTALL_FILES(/include
               ".h"
               ${cisst_BINARY_DIR}/libs/include/cisstConfig.h)


# Environment variables for Windows
IF(WIN32)
  # script to set cisst environment variables
  CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/cisstvars.bat.in
                 ${cisst_BINARY_DIR}/cisstvars.bat)
  # script to set Microsoft compiler environment variables
  GET_FILENAME_COMPONENT(BUILD_TOOL_DIRECTORY ${CMAKE_MAKE_PROGRAM} PATH)
  FIND_PROGRAM(VSVARS32_BAT
               NAMES vsvars32.bat
               PATHS ${BUILD_TOOL_DIRECTORY}/../../Common7/Tools)
  MARK_AS_ADVANCED(VSVARS32_BAT)
  CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/cisstvsvars.bat.in
                 ${cisst_BINARY_DIR}/cisstvsvars.bat)

  # If Visual Studio
  IF(CMAKE_GENERATOR MATCHES "Visual Studio*" )
    FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
      CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/visual-studio.bat.in
                     ${cisst_BINARY_DIR}/visual-studio-${config}.bat)
    ENDFOREACH(config)
  ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio*" )

ENDIF(WIN32)

# Environment variables for Unix
IF(UNIX)
  # script to set cisst environment variables
  CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/cisstvars.csh.in
                 ${cisst_BINARY_DIR}/cisstvars.csh
                 @ONLY)
  # script to set cisst environment variables
  CONFIGURE_FILE(${cisst_SOURCE_DIR}/libs/utils/cisstvars.sh.in
                 ${cisst_BINARY_DIR}/cisstvars.sh
                 @ONLY)
ENDIF(UNIX)

#
# $Log: CMakeLists.txt,v $
# Revision 1.17  2006/05/24 14:23:52  anton
# cisstCommon wrappers: Attempt to use SWIG to generate Java wrappers.  Work
# in progress, currently untested and partial wrapping only.
#
# Revision 1.16  2006/05/02 19:28:04  anton
# Build tools: Major update in output directories.  Libraries and executables
# are now organized within "libs", "examples" and "tests" directories.  Paths
# can be set using the cisstvars scripts generated during the configuration
# (i.e. by CMake).  See ticket #215.
#
# Revision 1.15  2006/03/30 20:21:48  anton
# cisstImage: Added typedefs based on vctDynamicMatrix to handle pixmaps of
# RGB pixels (defined as vctFixedSizeVector) and a simple PPM export function.
# Also added a simple test program to check that the compiler doesn't pad our
# pixels and a small example program (see imageTutorial).
#
# Revision 1.14  2006/02/13 22:20:39  ofri
# main CMakeLists and cisstBuild export: include all the cisstNetlib libraries
# into one variable CISSTNETLIB_LIBRARIES in the EXPORTED variables.
#
# Revision 1.13  2006/01/11 20:49:17  anton
# cisstNumerical CMake: Added a variable to indicate that either netlib is
# available and factor most of the CMake code (see CISST_HAS_NETLIB).
#
# Revision 1.12  2005/12/21 21:03:17  anton
# CMake: Moved find package _PythonInterp_ to top level so that PYTHON_EXECUTABLE
# is defined in all sub directories.
#
# Revision 1.11  2005/11/30 14:54:52  anton
# CMakeLists: Use CMake thread functions to use native threads.
#
# Revision 1.10  2005/11/29 02:58:53  anton
# Main CMakeLists.txt: Message with fatal error if both cnetlib and
# cisstNetlib are selected at the same time.
#
# Revision 1.9  2005/11/21 23:27:09  kapoor
# Extra newline in file causes problems for Cmake.
#
# Revision 1.8  2005/11/21 16:19:16  kapoor
# LAPACK: Added CISST_HAS_CISSTCNETLIB flag to cmake, which can be used to
# Compile code using the newer LAPACK3E. CLAPACK is still supported. See ticket #193.
#
# Revision 1.7  2005/11/17 22:52:36  anton
# CMakeLists.txt for Xerces and Xalan: Cleanup the existing solution.  Still
# pending a final decision re. how to package Xerces and Xalan.
#
# Revision 1.6  2005/09/26 21:23:30  anton
# cisstCommon: Updated CMN_ASSERT and cmnThrow.  Variables are not defined in
# cisstConfig.h anymore (user could not override).  CMN_ASSERT can now be set
# to throw an exception (user can set preference with CMake).
#
# Revision 1.5  2005/09/23 23:57:36  anton
# cmnThrow: Added CMake option to force an abort() instead of throw.
#
# Revision 1.4  2005/09/07 08:04:36  anton
# Main CMakeLists.txt: BUILD_LIBS_cisstNumerical is now OFF by default to
# reflect the build instructions.
#
# Revision 1.3  2005/09/07 07:08:58  anton
# Doc: Updated all CMakeLists work with ./CMakeLists.txt, added license,
# corrected couple of issues re. CMake 2.0+.
#
# Revision 1.2  2005/09/06 02:08:14  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.1  2005/09/01 16:16:47  anton
# Main CMakeLists.txt: Test addition to CVS repository.
#
#
