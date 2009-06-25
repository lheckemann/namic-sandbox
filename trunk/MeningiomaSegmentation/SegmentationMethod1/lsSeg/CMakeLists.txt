CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

SET(projName "itkTest")
PROJECT(${projName})
# INCLUDE_REGULAR_EXPRESSION("^.*$")

################################
# include ITK, to avoid setting it everytime, do:
# set the environment variable ITK_DIR to the dir containing ITKConfig.cmake in .bashrc
FIND_PACKAGE (ITK)
IF (ITK_FOUND)
   INCLUDE(${USE_ITK_FILE})
ENDIF(ITK_FOUND) 


SET(itkLibs
ITKCommon
ITKNumerics
ITKBasicFilters
ITKIO
)

SET(Libraries
${itkLibs}
)



set(cexx "featureMap")
ADD_EXECUTABLE(${cexx} ${cexx}.cxx)
TARGET_LINK_LIBRARIES(${cexx} ${Libraries})

set(cexx "gacSeg3")
ADD_EXECUTABLE(${cexx} ${cexx}.cxx)
TARGET_LINK_LIBRARIES(${cexx} ${Libraries})
