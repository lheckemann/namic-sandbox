# This project is designed to be built outside the Insight source tree.
PROJECT(blockmatching)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)


INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  /home/blloyd/Kitware/Apps/cvsClasses/
  /home/blloyd/Kitware/Apps/MyClasses/
)


ADD_EXECUTABLE(blockmatching itkDetAndDetOverTraceNminusOneTensorFilterTest.cxx)

TARGET_LINK_LIBRARIES(blockmatching
    ITKCommon
    ITKIO
    ITKBasicFilters
    ITKNumerics )
