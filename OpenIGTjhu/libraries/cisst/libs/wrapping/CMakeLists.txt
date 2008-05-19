#
# $Id: CMakeLists.txt,v 1.21 2005/09/06 13:41:09 anton Exp $
#
# CMakeLists for wrapping
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

PROJECT(cisstWrapping)

# Make sure that the SWIG generated files go with the libraries
SET(CMAKE_SWIG_FLAGS -outdir "${cisstWrapping_BINARY_DIR}")

# We define some new inline classes for the wrappers only
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Wrap each library
FOREACH(lib ${CISST_LIBRARIES})
  IF(BUILD_LIBS_${lib})
    IF(EXISTS ${cisstWrapping_SOURCE_DIR}/${lib})
      SUBDIRS(${lib})
    ENDIF(EXISTS ${cisstWrapping_SOURCE_DIR}/${lib})
  ENDIF(BUILD_LIBS_${lib})
ENDFOREACH(lib)


#
# $Log: CMakeLists.txt,v $
# Revision 1.21  2005/09/06 13:41:09  anton
# wrapping CMakeLists.txt: Test to wrap only compiled libraries.  See ticket
# #170 (to be closed).
#
# Revision 1.20  2005/09/06 02:08:15  anton
# CMake configuration: Created a main CMakeLists.txt for the libs, tests and
# examples.  A lot of code has been "factorized" in the ./CMakeLists.txt.
# Known problems: "doc" is not working yet. Dependencies on SWIG projects don't
# work for parallel builds. If a lib is selected and then unselected, the tests
# still appear ...
#
# Revision 1.19  2005/09/01 06:24:26  anton
# cisst wrapping:
# *: Reorganized files so that each library has its own CMakeLists.txt
# *: Added preliminary wrapping for some transformations.  This compiles, loads
# but is not tested.
#
# Revision 1.18  2005/08/19 05:29:14  anton
# wrapping CMakeLists: Added license.
#
# Revision 1.17  2005/06/17 23:04:46  anton
# cisst CMake code: Propagation of Python settings to tests and examples.
#
# Revision 1.16  2005/06/17 21:30:24  anton
# cisst CMake code:  Major update to ease the configuration of tests and
# examples.
#
# Revision 1.15  2005/06/15 15:06:20  anton
# Python wrapping: Split the cisstVector wrapping code in smaller files to
# increase readability.
#
# Revision 1.14  2005/05/04 13:25:46  anton
# TCL wrapping: Removed all the TCL wrapping code since it was only
# experimental, never worked and we don't have enough time to support it.
#
# Revision 1.13  2005/01/25 19:08:35  anton
# Python wrapping: Added -modern for SWIG (generates less code)
#
# Revision 1.12  2005/01/20 14:35:29  anton
# cisst Wrapping: Minor bug in CMakeLists.txt, missing " in COMMENT.
#
# Revision 1.11  2005/01/19 21:05:43  anton
# cisst wrapping:  Port to SWIG 1.3.24 (see ticket #122).  Mostly consists in
# removing the runtime specific options for the compilation.
#
# Revision 1.10  2005/01/18 22:17:08  anton
# Wrapping:  Updated CMakeLists.txt to correct issue raised in ticket #121.
# The files are not cleaned correctly, but this is a CMake bug.
#
# Revision 1.9  2005/01/07 20:01:06  anton
# Wrapping: Updated CMakelists.txt for take into account python_d (Windows)
# and force to compile the libraries as DLL (Windows, Linux seems to work
# fine with static libraries).
#
# Revision 1.8  2004/10/28 16:30:46  anton
# CMake:  Corrected a bug which prevented to clean all additional files.
#
# Revision 1.7  2004/10/28 04:49:37  anton
# wrapping: Added code for Tcl wrappers of cisstCommon and cisstVector.
# The %extend Get doesn't work yet (for embedding).
#
# Revision 1.6  2004/10/14 16:18:02  anton
# wrapping: changed the names of the modules from cisstXyz.i to
# cisstXyzPython.i to avoid dynamic loading issues with Windows/Python.
# Updated the __str__ method to use ToString().  For the register, used %extend
# to add the Get() method used in Python to get the right object.
#
# Revision 1.5  2004/10/08 22:53:29  anton
# Wrapping: I'm having a terrible time with ADD_CUSTOM_COMMAND between
# Linux and Windows but his version seems to work.
#
# Revision 1.4  2004/10/08 19:38:28  anton
# Wrapping: Error in install rule (* is not required)
#
# Revision 1.3  2004/10/07 18:29:27  anton
# SWIG Wrapping: Added a single SWIG runtime library used by all other SWIG
# Python modules.
#
# Revision 1.2  2004/08/31 14:08:31  anton
# Wrapping: After some discussions on the CMake mailing list, found a way to
# specify the output directory so that the .py files are directly generated
# in the same directory as the .so.
#
# Revision 1.1  2004/08/20 20:07:28  anton
# CMake Config: Update the config files to force the use of CMake 2.0.x or
# more (see #40) and cleaned-up the SWIG wrapping process thanks to the new
# CMake macros (see #55).
#
#
