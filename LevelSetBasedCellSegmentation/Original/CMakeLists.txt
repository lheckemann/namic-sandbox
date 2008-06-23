CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# This project is designed to be built outside the Insight source tree.
PROJECT(CellSegmentation)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(preprocess preprocess.cpp Functions.h)
TARGET_LINK_LIBRARIES(preprocess ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_EXECUTABLE(foregroundExtr foregroundExtr.cpp Functions.h)
TARGET_LINK_LIBRARIES(foregroundExtr ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_EXECUTABLE(GAC GACImageFilter.cxx Functions.h)
TARGET_LINK_LIBRARIES(GAC ITKBasicFilters ITKAlgorithms ITKStatistics ITKNumerics ITKCommon ITKIO)

ADD_EXECUTABLE(NPC NPointCorrelation.cxx)
TARGET_LINK_LIBRARIES(NPC ITKCommon ITKIO ITKNumerics ITKStatistics)


