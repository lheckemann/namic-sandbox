PROJECT (ExploreNormalizedMutualInformation)

FIND_PACKAGE(ITK)
IF (NOT ITK_FOUND)
  MESSAGE(FATAL_ERROR "Please set ITK_DIR.")
ENDIF(NOT ITK_FOUND)
INCLUDE(${USE_ITK_FILE})


INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_EXECUTABLE (ExploreNormalizedMutualInformation ExploreNormalizedMutualInformation.cxx )

TARGET_LINK_LIBRARIES(ExploreNormalizedMutualInformation
  ITKIO
  ITKBasicFilters
  ITKNumerics
  ITKCommon
)


