PROJECT(ThresholdFilter)

# Include this directory before the ITK_DIR. If we are going to make changes
# to the bayesian filters, we want the file in the sandbox to be picked up 
# prior to the one in ITK.
INCLUDE_DIRECTORIES( 
  ${BayesianSegmentationModule_SOURCE_DIR}
  )

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)

# Test for the Bayesian Classifier
ADD_EXECUTABLE(ThresholdFilter createMask.cxx )
TARGET_LINK_LIBRARIES(ThresholdFilter ITKStatistics ITKIO ITKCommon)
