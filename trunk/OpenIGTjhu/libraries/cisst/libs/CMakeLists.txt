#
# $Id: CMakeLists.txt,v 1.31 2005/11/17 22:52:36 anton Exp $
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
PROJECT(cisstLibs)


# Interactive Research Environment meaningless without Python wrapping
VARIABLE_REQUIRES(BUILD_LIBS_cisstInteractive REQUIREMENT_MET CISST_HAS_SWIG_PYTHON)


# Build everything
FOREACH(lib ${CISST_LIBS})
  IF(BUILD_LIBS_${lib})
    SUBDIRS(code/${lib})
  ENDIF(BUILD_LIBS_${lib})
ENDFOREACH(lib)

# Build wrappers
IF(CISST_HAS_SWIG_PYTHON)
  SUBDIRS(wrapping)
ENDIF(CISST_HAS_SWIG_PYTHON)


#
# $Log: CMakeLists.txt,v $
# Revision 1.31  2005/11/17 22:52:36  anton
# CMakeLists.txt for Xerces and Xalan: Cleanup the existing solution.  Still
# pending a final decision re. how to package Xerces and Xalan.
#
# Revision 1.30  2005/09/06 02:08:14  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.29  2005/08/02 06:31:50  alamora
# TEMPORARY FIX
# Moved conditional XML linking from cisstCommon project to this file, to
# accomdate cisstTracker.  All projects will now dynamically link to Xalan
# and Xerces.
#
# Revision 1.28  2005/07/06 02:35:28  anton
# cisstConfig.h.in: Added variable CISST_BUILD_ROOT.
#
# Revision 1.27  2005/07/01 22:17:23  anton
# Main CMakeLists.txt: Apple also requires shared libraries when Python modules
# are generated.
#
# Revision 1.26  2005/06/21 15:30:51  anton
# Main CMakeLists for libs: Added option for cisstInteractive and check for
# dependencies.
#
# Revision 1.25  2005/06/17 23:04:46  anton
# cisst CMake code: Propagation of Python settings to tests and examples.
#
# Revision 1.24  2005/06/17 21:30:24  anton
# cisst CMake code:  Major update to ease the configuration of tests and
# examples.
#
# Revision 1.23  2005/05/19 19:29:00  anton
# cisst libs: Added the license to cisstCommon and cisstVector
#
# Revision 1.22  2005/05/04 13:25:46  anton
# TCL wrapping: Removed all the TCL wrapping code since it was only
# experimental, never worked and we don't have enough time to support it.
#
# Revision 1.21  2005/03/30 03:26:28  anton
# Libs CMakeLists.txt: Separate stable libs compiled by default and unstable
# which are not compiled by default.
#
# Revision 1.20  2004/11/01 17:44:48  anton
# cisstNumerical: Added some Solvers and the files to use our own cnetlib
# code added to the cisst CVS repository (module "cnetlib").
#
# Revision 1.19  2004/10/28 04:47:32  anton
# wrapping: Added choice to build Tcl wrappers with SWIG
#
# Revision 1.18  2004/10/27 14:40:35  anton
# CMake configuration: Added code to compile netlib and reorganized the way
# new header files are created/copied.
#
# Revision 1.17  2004/10/21 19:54:46  ofri
# Adding project cisstNumerical to the repository -- imported from Ofri.
# See ticket #78.
#
# Revision 1.16  2004/08/20 20:07:28  anton
# CMake Config: Update the config files to force the use of CMake 2.0.x or
# more (see #40) and cleaned-up the SWIG wrapping process thanks to the new
# CMake macros (see #55).
#
# Revision 1.15  2004/06/25 14:16:07  ofri
# Added a special .NET "project" to the main libraries CMakeLists.txt to ease
# editing of CMake scripts from visual studio
#
# Revision 1.14  2004/05/04 14:11:34  anton
# Added OSAbstraction, DeviceInterface, RealTime, Tracker
# If RTAI is found, export the include path
#
# Revision 1.13  2004/04/02 16:23:50  anton
# set BUILD_SHARED_LIBS
#
# Revision 1.12  2004/03/29 21:23:09  anton
# Added code for SWIG, all variables start with CISST_ for consistancy
#
# Revision 1.11  2004/02/18 22:12:53  anton
# Added option CISST_HAS_LINUX_RTAI
# Added code to generate cisstConfig.h and install it
# Added ${cisst_DINARY_DIR) in include path to find cisstConfig.h
# Removed wsock32 of libraries used to link since it is not used
#
# Revision 1.10  2004/02/18 15:48:24  anton
# Change order of libs in CISST_LIBS_COMPILED for the linker
#
# Revision 1.9  2004/01/22 21:55:54  anton
# cmake/cisstMacros.cmake
#
# Revision 1.8  2003/11/11 21:59:02  anton
# Added more warnings for gcc
#
# Revision 1.7  2003/10/15 16:08:26  anton
# Added specific directory for Doxygen and longer include path for build.cmake (for cisstCommon.h, etc.)
#
# Revision 1.6  2003/09/17 14:14:29  anton
# place libraries in ./bin, it helps the Windows stuff
#
# Revision 1.5  2003/09/09 18:55:34  anton
# Added cisstVector
#
# Revision 1.4  2003/08/06 14:01:05  anton
# INSTALL_FILES for cisstInstalled.cmake
#
# Revision 1.3  2003/07/31 18:18:19  anton
# - Added code to create a list of all compiled libraries
# - Added cmake configuration files for programs using the libraries
#
# Revision 1.2  2003/06/24 15:02:19  anton
# Added EXECUTABLE_OUTPUT_PATH
#
# Revision 1.1.1.1  2003/05/30 19:47:56  anton
# no message
#
#
