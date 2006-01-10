PROJECT(BayesianSegmentationModule)

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
ADD_EXECUTABLE(BayesianClassifier BayesianClassifier.cxx )
TARGET_LINK_LIBRARIES(BayesianClassifier ITKStatistics ITKIO ITKCommon)

# Test for the Bayesian Classifier Initialization
ADD_EXECUTABLE(BayesianClassifierInitializer BayesianClassifierInitializer.cxx)
TARGET_LINK_LIBRARIES(BayesianClassifierInitializer ITKStatistics ITKIO ITKCommon)

# Test for the wrapper class around the two 
ADD_EXECUTABLE(itkBayesianClassificationImageFilterTest
  itkBayesianClassificationImageFilterTest.cxx
  )
TARGET_LINK_LIBRARIES(itkBayesianClassificationImageFilterTest ITKStatistics ITKIO ITKCommon)

# test for Filter that generates a list sample with masking capabilities
ADD_EXECUTABLE(itkImageToListGeneratorTest
  itkImageToListGeneratorTest.cxx
  )
TARGET_LINK_LIBRARIES(itkImageToListGeneratorTest ITKStatistics ITKCommon)

