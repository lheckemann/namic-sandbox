PROJECT(BayesianSegmentationModule)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)


INCLUDE_DIRECTORIES( 
  ${BayesianSegmentationModule_SOURCE_DIR}
  )

# Test for the Bayesian Classifier
ADD_EXECUTABLE(itkBayesianClassifierImageFilterTest 
  itkBayesianClassifierImageFilterTest.cxx 
  )
TARGET_LINK_LIBRARIES(itkBayesianClassifierImageFilterTest ITKStatistics ITKIO ITKCommon ITKNumerics)

# Test for the Bayesian Classifier Initialization
ADD_EXECUTABLE(itkBayesianClassifierInitializationImageFilterTest
  itkBayesianClassifierInitializationImageFilterTest.cxx
  )
TARGET_LINK_LIBRARIES(itkBayesianClassifierInitializationImageFilterTest ITKStatistics ITKIO ITKCommon ITKNumerics)


