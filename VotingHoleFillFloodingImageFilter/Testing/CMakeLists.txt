#
# Add tests here
#

ADD_EXECUTABLE(itkVotingBinaryHoleFillFloodingImageFilterTest1 
  itkVotingBinaryHoleFillFloodingImageFilterTest1.cxx)

TARGET_LINK_LIBRARIES(itkVotingBinaryHoleFillFloodingImageFilterTest1 ITKIO)

ADD_EXECUTABLE(itkVotingBinaryIterativeHoleFillingImageFilterTest1 
  itkVotingBinaryIterativeHoleFillingImageFilterTest1.cxx)

TARGET_LINK_LIBRARIES(itkVotingBinaryIterativeHoleFillingImageFilterTest1 ITKIO)

ADD_TEST(itkVotingTest1 
  ${EXECUTABLE_OUTPUT_PATH}/itkVotingBinaryHoleFillFloodingImageFilterTest1
  )

ADD_TEST(itkVotingTest2 
  ${EXECUTABLE_OUTPUT_PATH}/itkVotingBinaryIterativeHoleFillingImageFilterTest1
  )

