#
# $Id: FindMEI.cmake,v 1.3 2005/09/06 13:58:53 anton Exp $
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
# try to find MEI on Windows systems.
# (Motion Engineering Inc.)
#
# The following values are defined
#
# MEI_INCLUDE_DIR     - include directories to use MEI
# MEI_LIBRARY         - link against this to use MEI
#
# $Id: FindMEI.cmake,v 1.3 2005/09/06 13:58:53 anton Exp $
#

IF(WIN32)

  # Find the MEI Library
  FIND_LIBRARY(MEI_LIBRARY NAMES medvc60f PATHS
    "C:/MEI/2509/Libs/"
    ${MEI_LIBRARY}/*)

  # Find the MEI include directory 
  FIND_PATH(MEI_INCLUDE_DIR Idsp.h
    "C:/MEI/2509/Sources/"
    ${MEI_LIBRARY_PATH}/Sources/)

ENDIF(WIN32)

IF(CISST_HAS_LINUX_RTAI)

  # Find the MEI Library
  FIND_LIBRARY(MEI_LIBRARY NAMES mpd PATHS
    "/usr/local/lib/"
    ${MEI_LIBRARY}/*)

  # Find the MEI include directory 
  FIND_PATH(MEI_INCLUDE_DIR idsp.h
    "/usr/local/include/mei/"
    ${MEI_LIBRARY_PATH}/)
ENDIF(CISST_HAS_LINUX_RTAI)

#
# $Log: FindMEI.cmake,v $
# Revision 1.3  2005/09/06 13:58:53  anton
# cmake files: Added license.
#
# Revision 1.2  2005/07/30 06:15:02  kapoor
# DeviceInterface: (MEI) Added roboust cmake thingies for MEI.
#
# Revision 1.1  2004/05/27 17:09:25  anton
# Incorporated code from Ankur for the ATI, MEI, STG and LoPoMoCo devices.
# The build process will have to be redone whenever will will add dynamic
# loading.
#
#
