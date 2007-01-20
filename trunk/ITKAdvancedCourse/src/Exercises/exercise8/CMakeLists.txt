# This project is designed to be built outside the Insight source tree.
PROJECT(Exercise8)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(filter8       filter.cxx       )
ADD_EXECUTABLE(filter8Answer filterAnswer.cxx )

TARGET_LINK_LIBRARIES(filter8       ITKCommon ITKIO)
TARGET_LINK_LIBRARIES(filter8Answer ITKCommon ITKIO)
