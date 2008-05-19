#
# $Id: CMakeLists.txt,v 1.9 2006/01/05 23:19:49 anton Exp $
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
PROJECT(cisstDoc)


# Check for basic tools
FIND_PACKAGE(UnixCommands)
FIND_PACKAGE(LATEX)


# Create simple script to install multiple files
CONFIGURE_FILE(${cisstDoc_SOURCE_DIR}/install-files.sh.in
               ${cisstDoc_BINARY_DIR}/install-files.sh
               @ONLY@)

# List of directories
SET(CISST_DOCS
    latex
    doxygen
    other-formats
    release-notes)
# Build by default
FOREACH(doc ${CISST_DOCS})
  OPTION(BUILD_DOC_${doc} "Build ${doc}." ON)
ENDFOREACH(doc)

# Include CMakeLists.txt if necessary
FOREACH(doc ${CISST_DOCS})
  IF(BUILD_DOC_${doc})
    SUBDIRS(${doc})
  ENDIF(BUILD_DOC_${doc})
ENDFOREACH(doc)


# Install index file
INSTALL_FILES(/share/cisst/doc
              ".html"
              ${CMAKE_CURRENT_SOURCE_DIR}/index.html)

INSTALL_FILES(/share/cisst/doc
              ".png"
              ${CMAKE_CURRENT_SOURCE_DIR}/link.png
              ${CMAKE_CURRENT_SOURCE_DIR}/ps.png
              ${CMAKE_CURRENT_SOURCE_DIR}/pdf.png)

INSTALL_FILES(/share/cisst/doc
              ".gif"
              ${CMAKE_CURRENT_SOURCE_DIR}/envelope.gif)


#
# $Log: CMakeLists.txt,v $
# Revision 1.9  2006/01/05 23:19:49  anton
# Release notes:  Removed change logs which were not maintained and replaced
# by release notes.  Release notes are organized per release with an extra
# file for ongoing modifications (cisst-cvs.txt).  At release time, this file
# is used to create the new release notes and must be emptied.
#
# Revision 1.8  2005/09/09 15:09:38  anton
# Doc CMakeLists: Copy icons in the install tree.
#
# Revision 1.7  2005/09/07 07:08:58  anton
# Doc: Updated all CMakeLists work with ./CMakeLists.txt, added license,
# corrected couple of issues re. CMake 2.0+.
#
# Revision 1.6  2004/01/22 22:00:17  anton
# Added options to compile or not each subdirectory (latex, doxygen, etc)
#
# Revision 1.5  2003/10/08 13:34:56  anton
# INSTALL_PREFIX now shows correctly in UI
#
# Revision 1.4  2003/09/12 20:03:37  anton
# add install of install-files.sh, moved FindLATEX and FindUnixCommands up to factorize
#
# Revision 1.3  2003/08/06 13:52:59  anton
# Removed the useless * in the INSTALL_FILES command.  Added the INCLUDE of cisstXXX.cmake
#
# Revision 1.2  2003/08/01 17:50:06  anton
# Created subdir for other format
#
# Revision 1.1  2003/06/02 22:10:18  anton
# Initial version.  Compile latex files and install index.html.
#
#
