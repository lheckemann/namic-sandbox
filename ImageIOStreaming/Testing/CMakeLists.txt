PROJECT(ImageIOStreamingTest)

SET(TEST_DATA_ROOT     ${ImageIOStreamingTest_SOURCE_DIR}/Data/Input)
SET(TESTING_OUTPUT_DIR ${ImageIOStreamingTest_BINARY_DIR}/Temporary)

INCLUDE_DIRECTORIES( ${ImageIOStreamingTest_SOURCE_DIR} )

#
#
#  Find the BrainWeb image datasets to use them as input
#
FIND_PATH(BrainWeb_DATA_ROOT 
  NAMES brainweb165a10f17.mha  
  DOC "http://public.kitware.com/pub/itk/Data/BrainWeb/BrainPart1.tgz")


ADD_EXECUTABLE( ImageIOStreamingTest ImageReadRegionOfInterestWrite.cxx)
TARGET_LINK_LIBRARIES( ImageIOStreamingTest ITKIOStreaming )

ADD_EXECUTABLE( ImageStreamedFilteringTest01 ImageStreamedFilteringTest01.cxx )
TARGET_LINK_LIBRARIES( ImageStreamedFilteringTest01 ITKIOStreaming )

ADD_TEST( ImageIOStreamingTest01 
   ${ImageIOStreamingTest_BINARY_DIR}/ImageIOStreamingTest 
   ${TEST_DATA_ROOT}/tester.nii.gz 
   ${TESTING_OUTPUT_DIR}/xx.nii.gz 
   10 10 5 30 40 10)

ADD_TEST( ImageIOStreamingTest02 
   ${ImageIOStreamingTest_BINARY_DIR}/ImageIOStreamingTest 
   ${TEST_DATA_ROOT}/BrainProtonDensity3Slices.mhd
   ${TESTING_OUTPUT_DIR}/BrainProtonDensity3SlicesTestOutput02.mha
   20 30 1 30 40 1)

ADD_TEST( ImageIOStreamingTest03 
   ${ImageIOStreamingTest_BINARY_DIR}/ImageIOStreamingTest 
   ${TEST_DATA_ROOT}/BrainProtonDensity3Slices.yaff
   ${TESTING_OUTPUT_DIR}/BrainProtonDensity3SlicesTestOutput03.mha
   20 30 1 30 40 1)

ADD_TEST( ImageStreamedFilteringTest01 
   ${ImageIOStreamingTest_BINARY_DIR}/ImageStreamedFilteringTest01 
   ${TEST_DATA_ROOT}/BrainProtonDensity3Slices.yaff
   ${TESTING_OUTPUT_DIR}/BrainProtonDensity3SlicesTestOutput04.mha
   3
   )

IF(BrainWeb_DATA_ROOT)
ADD_TEST( ImageStreamedFilteringTest02 
   ${ImageIOStreamingTest_BINARY_DIR}/ImageStreamedFilteringTest01 
   ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha
   ${TESTING_OUTPUT_DIR}/BrainProtonDensity3SlicesTestOutput04.mha
   3
   )

ENDIF(BrainWeb_DATA_ROOT)


