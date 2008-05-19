#
# $Id: FindRTAI.cmake,v 1.3 2005/09/06 13:58:53 anton Exp $
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
# try to find RTAI on UNIX systems.
#
# The following values are defined
#
# RTAI_INCLUDE_DIR     - include directories to use RTAI
# RTAI_LIBRARIES       - link against this to use RTAI (fullpath)
#
# $Id: FindRTAI.cmake,v 1.3 2005/09/06 13:58:53 anton Exp $
#

IF (UNIX)
  SET(RTAI_SEARCH_PATH
    /usr
    /usr/local
    /usr/realtime
  )

  FIND_PATH(RTAI_DIR
            include/rtai.h ${RTAI_SEARCH_PATH})

  IF(RTAI_DIR) 
    SET(RTAI_INCLUDE_DIR ${RTAI_DIR}/include)
    FIND_LIBRARY(RTAI_LIBRARIES lxrt ${RTAI_DIR}/lib)
  ENDIF(RTAI_DIR)

ENDIF (UNIX)

#
# $Log: FindRTAI.cmake,v $
# Revision 1.3  2005/09/06 13:58:53  anton
# cmake files: Added license.
#
# Revision 1.2  2005/06/17 21:30:24  anton
# cisst CMake code:  Major update to ease the configuration of tests and
# examples.
#
# Revision 1.1  2004/02/18 22:13:23  anton
# Creation
#
#
