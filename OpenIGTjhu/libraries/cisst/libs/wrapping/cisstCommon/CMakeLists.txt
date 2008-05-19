#
# $Id: CMakeLists.txt,v 1.4 2006/05/24 14:23:52 anton Exp $
#
# CMakeLists for cisstCommon wrapping
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
#

PROJECT(cisstCommonWrapping)


IF(CISST_HAS_SWIG_PYTHON)
  SET(cisstCommon_PYTHON_INTERFACE ${cisstCommonWrapping_SOURCE_DIR}/cisstCommonPython.i)

  IF(EXISTS ${cisstCommon_PYTHON_INTERFACE})
    # We are using C++ code
    SET_SOURCE_FILES_PROPERTIES(${cisstCommon_PYTHON_INTERFACE} PROPERTIES CPLUSPLUS ON)
    # Make sure the runtime code is not included
    SET_SOURCE_FILES_PROPERTIES(${cisstCommon_PYTHON_INTERFACE}
                                PROPERTIES SWIG_FLAGS "-v;-modern;-fcompact;-fvirtual")
    SWIG_ADD_MODULE(cisstCommonPython python ${cisstCommon_PYTHON_INTERFACE})
    IF(WIN32)
      SWIG_LINK_LIBRARIES(cisstCommonPython cisstCommon
                          debug ${PYTHON_DEBUG_LIBRARY}
                          optimized ${PYTHON_LIBRARY})
    ELSE(WIN32)
      SWIG_LINK_LIBRARIES(cisstCommonPython cisstCommon ${PYTHON_LIBRARIES})
    ENDIF(WIN32)
    # Copy the .py file generated to wherever the libraries are
    ADD_CUSTOM_COMMAND(TARGET _cisstCommonPython
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND}
                       ARGS -E copy
                               ${cisstWrapping_BINARY_DIR}/cisstCommonPython.py
                               ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/cisstCommonPython.py)
  ENDIF(EXISTS ${cisstCommon_PYTHON_INTERFACE})
ENDIF(CISST_HAS_SWIG_PYTHON)




IF(CISST_HAS_SWIG_JAVA)
  SET(cisstCommon_JAVA_INTERFACE ${cisstCommonWrapping_SOURCE_DIR}/cisstCommonJava.i)

  IF(EXISTS ${cisstCommon_JAVA_INTERFACE})
    # We are using C++ code
    SET_SOURCE_FILES_PROPERTIES(${cisstCommon_JAVA_INTERFACE} PROPERTIES CPLUSPLUS ON)
    # Make sure the runtime code is not included
    SET_SOURCE_FILES_PROPERTIES(${cisstCommon_JAVA_INTERFACE}
                                PROPERTIES SWIG_FLAGS "-v")
    SWIG_ADD_MODULE(cisstCommonJava java ${cisstCommon_JAVA_INTERFACE})
    SWIG_LINK_LIBRARIES(cisstCommonJava cisstCommon)
  ENDIF(EXISTS ${cisstCommon_JAVA_INTERFACE})
ENDIF(CISST_HAS_SWIG_JAVA)


#
# $Log: CMakeLists.txt,v $
# Revision 1.4  2006/05/24 14:23:52  anton
# cisstCommon wrappers: Attempt to use SWIG to generate Java wrappers.  Work
# in progress, currently untested and partial wrapping only.
#
# Revision 1.3  2006/05/02 19:28:04  anton
# Build tools: Major update in output directories.  Libraries and executables
# are now organized within "libs", "examples" and "tests" directories.  Paths
# can be set using the cisstvars scripts generated during the configuration
# (i.e. by CMake).  See ticket #215.
#
# Revision 1.2  2006/03/10 14:34:37  anton
# Misc CMakeLists.txt:  New flags for SWIG to reduce size of code and avoid
# Visual C++ warnings.
#
# Revision 1.1  2005/09/01 06:24:26  anton
# cisst wrapping:
# *: Reorganized files so that each library has its own CMakeLists.txt
# *: Added preliminary wrapping for some transformations.  This compiles, loads
# but is not tested.
#
#
