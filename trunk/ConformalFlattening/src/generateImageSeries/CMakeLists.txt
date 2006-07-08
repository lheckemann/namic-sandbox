PROJECT(generateImageSeries)

# Include this directory before the ITK_DIR. If we are going to make changes
# to the bayesian filters, we want the file in the sandbox to be picked up 
# prior to the one in ITK.
INCLUDE_DIRECTORIES( 
  ${generateImageSeries_SOURCE_DIR}
  )

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(generateImageSeries generateImageSeries.cxx )
TARGET_LINK_LIBRARIES(generateImageSeries ITKIO ITKCommon)

ADD_EXECUTABLE(generateImageFromSeries generateImageFromSeries.cxx )
TARGET_LINK_LIBRARIES(generateImageFromSeries ITKIO ITKCommon)
