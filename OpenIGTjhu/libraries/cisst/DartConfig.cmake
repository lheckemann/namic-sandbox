#
# $Id: DartConfig.cmake,v 1.5 2006/02/11 20:17:45 anton Exp $
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


# Set a long and somewhat informative build name for Dart
IF(CMAKE_COMPILER_IS_GNUCXX)
  EXEC_PROGRAM(${CMAKE_CXX_COMPILER} ARGS -dumpversion
               OUTPUT_VARIABLE CISST_GNUCXX_VERSION)
  SET(BUILDNAME_TMP ${CMAKE_SYSTEM_NAME}-g++-${CISST_GNUCXX_VERSION}-${CMAKE_BUILD_TYPE}-)
ELSE(CMAKE_COMPILER_IS_GNUCXX)
  SET(BUILDNAME_TMP ${CMAKE_SYSTEM_NAME}-${CMAKE_CXX_COMPILER}-${CMAKE_BUILD_TYPE}-)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# Tells which libraries have been compiled and assumed tests have been as well
IF(BUILD_LIBS_cisstCommon)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}Co)
ENDIF(BUILD_LIBS_cisstCommon)

IF(BUILD_LIBS_cisstVector)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}Ve)
ENDIF(BUILD_LIBS_cisstVector)

IF(BUILD_LIBS_cisstNumerical)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}Nu)
ENDIF(BUILD_LIBS_cisstNumerical)

IF(BUILD_LIBS_cisstInteractive)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}In)
ENDIF(BUILD_LIBS_cisstInteractive)

IF(BUILD_LIBS_cisstOSAbstraction)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}Os)
ENDIF(BUILD_LIBS_cisstOSAbstraction)

IF(BUILD_LIBS_cisstDeviceInterface)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}De)
ENDIF(BUILD_LIBS_cisstDeviceInterface)

IF(BUILD_LIBS_cisstRealTime)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}Re)
ENDIF(BUILD_LIBS_cisstRealTime)

IF(BUILD_LIBS_cisstTracker)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}Tr)
ENDIF(BUILD_LIBS_cisstTracker)

# Tells if Python was used
IF(CISST_HAS_SWIG_PYTHON)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}-Py)
ENDIF(CISST_HAS_SWIG_PYTHON)

# Tells which version of netlib was used
IF(CISST_HAS_CNETLIB)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}-CNe)
ENDIF(CISST_HAS_CNETLIB)

IF(CISST_HAS_CISSTNETLIB)
  SET(BUILDNAME_TMP ${BUILDNAME_TMP}-CiNe)
ENDIF(CISST_HAS_CISSTNETLIB)


# Save to cache
SET(BUILDNAME ${BUILDNAME_TMP}
    CACHE STRING "Full name of build configuration." FORCE)
MARK_AS_ADVANCED(BUILDNAME)


# Options for Dart2
SET(DROP_METHOD "xmlrpc")
SET(DROP_SITE "http://www.cisst.org:8081")
SET(DROP_LOCATION "cisst")
SET(COMPRESS_SUBMISSION ON)

# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
SET (NIGHTLY_START_TIME "01:00:00 EDT")


#
# $Log: DartConfig.cmake,v $
# Revision 1.5  2006/02/11 20:17:45  anton
# DartConfig.cmake: Dart is now on cisst.org (was lacuna.cs.jhu.edu).
#
# Revision 1.4  2005/12/07 23:12:58  anton
# DartConfig.cmake: Create a build name which tells which libs have been
# compiled, if Python wrappers have been generated and which netlib is used.
#
# Revision 1.3  2005/11/12 22:48:21  anton
# DartConfig.cmake: Removed processor from build name (name was getting too
# long).
#
# Revision 1.2  2005/11/11 19:27:24  anton
# DartConfig.cmake: Added longer more explicit build name for Dart2.
#
# Revision 1.1  2005/09/06 01:41:44  anton
# DartConfig: Moved to root directory.
#
# Revision 1.1  2005/06/21 17:32:09  anton
# Tests and Dart: Created the recommended DartConfig.cmake and moved settings
# from main CMakeLists.txt
#
#

