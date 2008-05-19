#
# $Id: FindCisstNetlib.cmake,v 1.3 2005/11/30 16:38:50 anton Exp $
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
# Try to find CNetlib
# (code from www.netlib.org, ERC CISST customized version)
#
# The following values are defined
#
# CISSTNETLIB_INCLUDE_DIR     - include directories to use CISSTNETLIB
# CISSTNETLIB_LIBRARIES       - link against this to use CISSTNETLIB (fullpath)
#
# $Id: FindCisstNetlib.cmake,v 1.3 2005/11/30 16:38:50 anton Exp $
#

SET(CISSTNETLIB_SEARCH_PATH
  /usr
  /usr/local
  /home/erc/cisstNetlib
)

FIND_PATH(CISSTNETLIB_DIR
          include/cisstNetlib.h ${CISSTNETLIB_SEARCH_PATH})

IF(CISSTNETLIB_DIR) 
  SET(CISSTNETLIB_INCLUDE_DIR ${CISSTNETLIB_DIR}/include)
  FIND_LIBRARY(CISSTNETLIB_LIBRARIES cisstNetlib ${CISSTNETLIB_DIR}/lib ${CISSTNETLIB_DIR})
  FIND_LIBRARY(CISSTNETLIB_F95_LIBRARIES cisstNetlibF95 ${CISSTNETLIB_DIR}/lib ${CISSTNETLIB_DIR})
  IF(WIN32)
    FIND_LIBRARY(CISSTNETLIB_GCC_LIBRARIES cisstNetlibGCC ${CISSTNETLIB_DIR}/lib ${CISSTNETLIB_DIR})
  ENDIF(WIN32)
ENDIF(CISSTNETLIB_DIR)

#
# $Log: FindCisstNetlib.cmake,v $
# Revision 1.3  2005/11/30 16:38:50  anton
# CMakeLists: Added support for libcisstNetlibGCC on Windows.
#
# Revision 1.2  2005/11/29 03:00:39  anton
# FindCisstNetlib.cmake: *: cisstNetlib was spelled cisstnetlib *: rename
# lib Fortran 95 to find the one provided with cisstNetlib.
#
# Revision 1.1  2005/11/20 21:36:45  kapoor
# LAPACK: Added CISST_HAS_CISSTCNETLIB flag to cmake, which can be used to
# Compile code using the newer LAPACK3E. CLAPACK is still supported. See ticket #193.
#
#
#
