#
# $Id: CMakeLists.txt,v 1.4 2006/05/02 19:28:04 anton Exp $
#
# CMakeLists for cisstInteractive
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
SET(LIBRARY cisstInteractive)

# set dependencies
SET(DEPENDENCIES
    cisstCommon)

# all source files
SET(SOURCE_FILES
    ireFramework.cpp
    )

# all header files
SET(HEADER_FILES
    ireExport.h
    ireFramework.h
    )


# Link against these libraries
IF(WIN32)
  LINK_LIBRARIES(debug ${PYTHON_DEBUG_LIBRARY} optimized ${PYTHON_LIBRARY}) 
ELSE(WIN32)
  LINK_LIBRARIES(${PYTHON_LIBRARY}) 
ENDIF(WIN32)
  

CISST_ADD_LIBRARY_TO_PROJECT(cisstLibs)

# Call makeIrepy to compile the Python source files (*.py) and place
# the results (*.pyc files) in the destination directory.
ADD_CUSTOM_TARGET(cisstInteractive_COMPILE_IREPY ALL
                  ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/makeIrepy.py
                  ${CMAKE_CURRENT_SOURCE_DIR}/irepy  # src-dir
                  ${LIBRARY_OUTPUT_PATH}/irepy)   # dest-dir

#
# $Log: CMakeLists.txt,v $
# Revision 1.4  2006/05/02 19:28:04  anton
# Build tools: Major update in output directories.  Libraries and executables
# are now organized within "libs", "examples" and "tests" directories.  Paths
# can be set using the cisstvars scripts generated during the configuration
# (i.e. by CMake).  See ticket #215.
#
# Revision 1.3  2005/12/20 20:46:19  pkaz
# cisstInteractive:  added custom target to CMakeLists.txt to compile Python files into build directory (using makeIrepy.py)
#
# Revision 1.2  2005/09/06 02:08:15  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.1  2005/06/20 22:16:07  anton
# Interactive Research Environment: Moved all IRE code to cisstInteractive
#
#
