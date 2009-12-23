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

ADD_EXECUTABLE(computeRshCoeffs computeRshCoeffs.cxx ${SpecialFunctions_SOURCES})
TARGET_LINK_LIBRARIES(computeRshCoeffs ITKIO ITKNumerics)

ADD_EXECUTABLE(odfRecon odfRecon.cxx ${SpecialFunctions_SOURCES})
TARGET_LINK_LIBRARIES(odfRecon ITKIO ITKNumerics)

ADD_EXECUTABLE(calcFirResponse calcFirResponse.cxx ${SpecialFunctions_SOURCES})
TARGET_LINK_LIBRARIES(calcFirResponse ITKIO ITKNumerics)

ADD_EXECUTABLE(fodRecon fodRecon.cxx ${SpecialFunctions_SOURCES})
TARGET_LINK_LIBRARIES( fodRecon ITKIO ITKNumerics)

ADD_EXECUTABLE(computeRshPowers computeRshPowers.cxx ${SpecialFunctions_SOURCES})
TARGET_LINK_LIBRARIES(computeRshPowers ITKIO ITKNumerics)

INSTALL(TARGETS
  computeRshCoeffs
  odfRecon
  calcFirResponse
  fodRecon
  computeRshPowers
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
