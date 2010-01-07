########################################################################
#
#    Date:    $Date$
#    Version: $Revision$
#    Author:  $Author$ 
#    ID:      $Id: CMakeLists.txt 172 2009-09-29 18:39:02Z bloyl@UPHS.PENNHEALTH.PRV $
#
#    File Description
#        Blah blah blah
#
#    Copyright (c) Blah Blah Blah
#    
#    This software is distributed WITHOUT ANY WARRANTY; without even
#    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#    PURPOSE.  See the above copyright notices for more information.
########################################################################
ADD_SUBDIRECTORY(Source)


INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/VotingHoleFillFloodingImageFilter_Source )

IF(BUILD_EXECS)
  ADD_SUBDIRECTORY(Exec)
ENDIF(BUILD_EXECS)

IF(BUILD_SLICER)
  ADD_SUBDIRECTORY(Slicer)
ENDIF(BUILD_SLICER)
