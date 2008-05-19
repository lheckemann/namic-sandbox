#
# $Id: FindCNetlib.cmake,v 1.3 2005/09/06 13:58:53 anton Exp $
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
# CNETLIB_INCLUDE_DIR     - include directories to use CNETLIB
# CNETLIB_LIBRARIES       - link against this to use CNETLIB (fullpath)
#
# $Id: FindCNetlib.cmake,v 1.3 2005/09/06 13:58:53 anton Exp $
#

SET(CNETLIB_SEARCH_PATH
  /usr
  /usr/local
  /home/erc/cnetlib
)

FIND_PATH(CNETLIB_DIR
          include/cnetlib.h ${CNETLIB_SEARCH_PATH})

IF(CNETLIB_DIR) 
  SET(CNETLIB_INCLUDE_DIR ${CNETLIB_DIR}/include)
  FIND_LIBRARY(CNETLIB_LIBRARIES cnetlib ${CNETLIB_DIR}/lib)
ENDIF(CNETLIB_DIR)

#
# $Log: FindCNetlib.cmake,v $
# Revision 1.3  2005/09/06 13:58:53  anton
# cmake files: Added license.
#
# Revision 1.2  2005/06/17 21:30:24  anton
# cisst CMake code:  Major update to ease the configuration of tests and
# examples.
#
# Revision 1.1  2004/11/01 17:44:48  anton
# cisstNumerical: Added some Solvers and the files to use our own cnetlib
# code added to the cisst CVS repository (module "cnetlib").
#
#
