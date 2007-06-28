PROJECT(ImageIOStreamingTest)

SET(TEST_DATA_ROOT     ${ImageIOStreamingTest_SOURCE_DIR}/Data/Input)
SET(TESTING_OUTPUT_DIR ${ImageIOStreamingTest_BINARY_DIR}/Temporary)

INCLUDE_DIRECTORIES( ${ImageIOStreamingTest_SOURCE_DIR} )

SET( ImageIOStreamingTest_SRC
ImageReadRegionOfInterestWrite.cxx)

ADD_EXECUTABLE( ImageIOStreamingTest ${ImageIOStreamingTest_SRC} )
TARGET_LINK_LIBRARIES( ImageIOStreamingTest ITKIOStreaming )

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


