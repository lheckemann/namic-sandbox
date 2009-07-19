#
# Add tests here
#

ADD_EXECUTABLE(itkBinaryThresholdImageFilterTest1 
  itkBinaryThresholdImageFilterTest1.cxx)

TARGET_LINK_LIBRARIES(itkBinaryThresholdImageFilterTest1 ITKIO)

ADD_EXECUTABLE(itkVotingBinaryHoleFillFloodingImageFilterTest1 
  itkVotingBinaryHoleFillFloodingImageFilterTest1.cxx)

TARGET_LINK_LIBRARIES(itkVotingBinaryHoleFillFloodingImageFilterTest1 ITKIO)

ADD_EXECUTABLE(itkVotingBinaryIterativeHoleFillingImageFilterTest1 
  itkVotingBinaryIterativeHoleFillingImageFilterTest1.cxx)

TARGET_LINK_LIBRARIES(itkVotingBinaryIterativeHoleFillingImageFilterTest1 ITKIO)

ADD_EXECUTABLE(itkEndorseFrontPropagationImageFilterTest1 
  itkEndorseFrontPropagationImageFilterTest1.cxx)

TARGET_LINK_LIBRARIES(itkEndorseFrontPropagationImageFilterTest1 ITKIO)

ADD_EXECUTABLE(itkFrontPropagationLabelImageFilterTest1
  itkFrontPropagationLabelImageFilterTest1.cxx)

TARGET_LINK_LIBRARIES(itkFrontPropagationLabelImageFilterTest1 ITKIO)



ADD_TEST(itkThresholdTest1
  ${EXECUTABLE_OUTPUT_PATH}/itkBinaryThresholdImageFilterTest1
  ${PROJECT_SOURCE_DIR}/Data/BrainProtonDensitySlice.png
  ${TEMP}/BrainProtonDensitySliceThresholded1.png
  180 255
  0 255
  )

ADD_TEST(itkVotingTest1 
  ${EXECUTABLE_OUTPUT_PATH}/itkVotingBinaryIterativeHoleFillingImageFilterTest1
  ${TEMP}/BrainProtonDensitySliceThresholded1.png
  ${TEMP}/BrainProtonDensitySliceThresholded1Filled1.png
  2 2
  100
  )

ADD_TEST(itkVotingTest2 
  ${EXECUTABLE_OUTPUT_PATH}/itkVotingBinaryHoleFillFloodingImageFilterTest1
  ${TEMP}/BrainProtonDensitySliceThresholded1.png
  ${TEMP}/BrainProtonDensitySliceThresholded1Filled2.png
  2 2 
  1
  100
  )

ADD_TEST(itkEndorseTest1 
  ${EXECUTABLE_OUTPUT_PATH}/itkEndorseFrontPropagationImageFilterTest1
  ${TEMP}/BrainProtonDensitySliceThresholded1.png
  ${PROJECT_SOURCE_DIR}/Data/BrainProtonDensitySlice.png
  ${TEMP}/BrainProtonDensitySliceThresholded1Endorsed1.png
  2 2 
  120.0
  100
  )


SET_TESTS_PROPERTIES( itkVotingTest1 PROPERTIES DEPENDS itkThresholdTest1)
SET_TESTS_PROPERTIES( itkVotingTest2 PROPERTIES DEPENDS itkThresholdTest1)
SET_TESTS_PROPERTIES( itkVotingTest2 PROPERTIES DEPENDS itkVotingTest1)
SET_TESTS_PROPERTIES( itkEndorseTest1 PROPERTIES DEPENDS itkThresholdTest1)

ADD_TEST(itkFrontPropagationLabelImageFilterTest1 
  ${EXECUTABLE_OUTPUT_PATH}/itkFrontPropagationLabelImageFilterTest1
  ${TEMP}/BrainProtonDensitySliceThresholded1.png
  ${PROJECT_SOURCE_DIR}/Data/BrainProtonDensitySlice.png
  ${TEMP}/BrainProtonDensitySliceThresholded1Endorsed1.png
  2 2 
  120.0
  100
  )


