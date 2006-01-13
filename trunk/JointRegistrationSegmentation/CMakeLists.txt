PROJECT(JointRegistrationSegmentation)

# Put this first, so duplicate header are taken 
# first from this subdirectory instead of ITKs.
INCLUDE_DIRECTORIES( 
  ${JointRegistrationSegmentation_SOURCE_DIR}
  ${JointRegistrationSegmentation_SOURCE_DIR}/../ExpectationMaximizationFramework
  )


FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)




ADD_EXECUTABLE(ImageRegistrationTest
  ImageRegistrationTest.cxx 
  )

TARGET_LINK_LIBRARIES(ImageRegistrationTest 
      ITKStatistics ITKIO ITKCommon ITKNumerics)




ADD_EXECUTABLE(itkExpectationMaximizationImageClassificationTest
  itkExpectationMaximizationImageClassificationTest.cxx 
  )

TARGET_LINK_LIBRARIES(itkExpectationMaximizationImageClassificationTest 
      ITKStatistics ITKIO ITKCommon ITKNumerics)




ADD_EXECUTABLE(itkExpectationMaximizationImageClassificationWithPriorsTest
  itkExpectationMaximizationImageClassificationWithPriorsTest.cxx 
  )

TARGET_LINK_LIBRARIES(itkExpectationMaximizationImageClassificationWithPriorsTest 
       ITKStatistics ITKIO ITKCommon ITKNumerics)





ADD_EXECUTABLE(itkJointRegistrationSegmentationAlgorithmTest
  itkJointRegistrationSegmentationAlgorithmTest.cxx 
  )

TARGET_LINK_LIBRARIES(itkJointRegistrationSegmentationAlgorithmTest 
        ITKStatistics ITKIO ITKCommon ITKNumerics)




ADD_EXECUTABLE(itkMRIInhomogeneityEstimatorTest
itkMRIInhomogeneityEstimatorTest.cxx 
)

TARGET_LINK_LIBRARIES(itkMRIInhomogeneityEstimatorTest 
        ITKStatistics ITKIO ITKCommon ITKNumerics)


