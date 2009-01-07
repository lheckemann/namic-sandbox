CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT(ModularLevelSets)

SET(ModularLevelSetsSrcs
  ModularLevelSets.cxx
)

SET(ModularLevelSetsLibraries
  ITKCommon
  ITKIO
  ITKBasicFilters
  ITKAlgorithms
)

INCLUDE(${CMAKE_ROOT}/Modules/FindITK.cmake)

IF(USE_ITK_FILE)
  INCLUDE(${USE_ITK_FILE})
ENDIF(USE_ITK_FILE)

ADD_EXECUTABLE(ModularLevelSets ${ModularLevelSetsSrcs}) 
TARGET_LINK_LIBRARIES(ModularLevelSets ${ModularLevelSetsLibraries})

