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

ADD_EXECUTABLE(BayesianSegmentationModule 
  BayesianSegmentation.cxx 
  )

TARGET_LINK_LIBRARIES(BayesianSegmentationModule ITKStatistics ITKIO ITKCommon ITKNumerics)


ADD_EXECUTABLE(itkBayesianClassifierImageFilterTest 
  itkBayesianClassifierImageFilterTest.cxx 
  )
TARGET_LINK_LIBRARIES(itkBayesianClassifierImageFilterTest ITKStatistics ITKIO ITKCommon ITKNumerics)


