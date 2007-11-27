PROJECT(Exercise36)

FIND_PACKAGE(ITK REQUIRED)
IF( ITK_FOUND )
  INCLUDE (${USE_ITK_FILE})
ENDIF( ITK_FOUND )


INCLUDE_DIRECTORIES( 
  ${Exercise36_SOURCE_DIR} 
  )

ADD_EXECUTABLE( ConvertImagesToVectorImage ConvertImagesToVectorImage.cxx )
TARGET_LINK_LIBRARIES( ConvertImagesToVectorImage ITKIO ITKCommon )

ADD_EXECUTABLE( ImageKmeansModelEstimator ImageKmeansModelEstimator.cxx )
TARGET_LINK_LIBRARIES( ImageKmeansModelEstimator ITKStatistics ITKIO ITKCommon )

ADD_EXECUTABLE( ImageKmeansModelClassifier ImageKmeansModelClassifier.cxx )
TARGET_LINK_LIBRARIES( ImageKmeansModelClassifier ITKStatistics ITKIO ITKCommon )

ADD_EXECUTABLE( ImageMarkovRandomField ImageMarkovRandomField.cxx )
TARGET_LINK_LIBRARIES( ImageMarkovRandomField ITKStatistics ITKIO ITKCommon )

ADD_EXECUTABLE( ExpectationMaximizationMixtureModelEstimator ExpectationMaximizationMixtureModelEstimator.cxx )
TARGET_LINK_LIBRARIES( ExpectationMaximizationMixtureModelEstimator ITKStatistics ITKIO ITKCommon )

