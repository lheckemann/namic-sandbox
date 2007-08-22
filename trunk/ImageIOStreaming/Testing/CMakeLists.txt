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


IF(USE_YAFF)
IF(BrainWeb_DATA_ROOT)
  FIND_FILE(brainweb165a10f17YAFF_FOUND
    NAMES brainweb165a10f17.yaff
    PATHS ${BrainWeb_DATA_ROOT}
    DOC "YAFF surrogate header for the file brainweb165a10f17.mha"]
    )
ENDIF(BrainWeb_DATA_ROOT)
ENDIF(USE_YAFF)


ADD_EXECUTABLE( ImageIOStreamingTest ImageReadRegionOfInterestWrite.cxx)
TARGET_LINK_LIBRARIES( ImageIOStreamingTest ITKIOStreaming )

ADD_EXECUTABLE( ImageStreamedFilteringTest01 ImageStreamedFilteringTest01.cxx )
TARGET_LINK_LIBRARIES( ImageStreamedFilteringTest01 ITKIOStreaming )

ADD_EXECUTABLE( AnalyzeObjectMapTest AnalyzeObjectMapTest.cxx )
TARGET_LINK_LIBRARIES( AnalyzeObjectMapTest ITKIOStreaming )

ADD_EXECUTABLE( creatingImage creatingImage.cxx )
TARGET_LINK_LIBRARIES( creatingImage  ITKCommon ITKIOStreaming )


ADD_TEST( ImageIOStreamingNIFTITest01
   ${ImageIOStreamingTest_BINARY_DIR}/ImageIOStreamingTest
   ${TEST_DATA_ROOT}/tester.nii.gz
   ${TESTING_OUTPUT_DIR}/xx.nii.gz
   10 10 5 30 40 10)

ADD_TEST( ImageIOStreamingMETATest02
   ${ImageIOStreamingTest_BINARY_DIR}/ImageIOStreamingTest
   ${TEST_DATA_ROOT}/BrainProtonDensity3Slices.mhd
   ${TESTING_OUTPUT_DIR}/BrainProtonDensity3SlicesTestOutput02.mha
   20 30 1 30 40 1)

ADD_TEST( ImageIOStreamingAnalyzeObjectLabelMapTest01
   ${ImageIOStreamingTest_BINARY_DIR}/AnalyzeObjectMapTest
   ${TEST_DATA_ROOT}/test.obj
   ${TESTING_OUTPUT_DIR}/objectLabelTest.obj
   ${TEST_DATA_ROOT}/2dtest.nii
   ${TESTING_OUTPUT_DIR}/creatingObject.obj
   ${TESTING_OUTPUT_DIR}/circle.obj
   ${TESTING_OUTPUT_DIR}/blankImage.obj
   ${TESTING_OUTPUT_DIR}/OneDimensionImage.obj
   )

ADD_TEST( ImageIOStreamingAnalyzeObjectLabelMapCreateImageTest01
   ${ImageIOStreamingTest_BINARY_DIR}/creatingImage
   ${TESTING_OUTPUT_DIR}/2dtest.nii
   ${TESTING_OUTPUT_DIR}/newObject.obj
   )


IF(USE_YAFF)
ADD_TEST( ImageIOStreamingYAFFTest01
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

ENDIF(USE_YAFF)

IF(BrainWeb_DATA_ROOT)

ADD_TEST( ImageStreamedFilteringTest02
   ${ImageIOStreamingTest_BINARY_DIR}/ImageStreamedFilteringTest01
   ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha
   ${TESTING_OUTPUT_DIR}/ImageStreamedFilteringTest02Output.mha
   3
   )

IF(brainweb165a10f17YAFF_FOUND)

IF(USE_YAFF)
  ADD_TEST( ImageStreamedFilteringTest03
     ${ImageIOStreamingTest_BINARY_DIR}/ImageStreamedFilteringTest01
     ${BrainWeb_DATA_ROOT}/brainweb165a10f17.yaff
     ${TESTING_OUTPUT_DIR}/ImageStreamedFilteringTest03Output.mha
     3
     )

  ADD_TEST( ImageStreamedFilteringTest04
     ${ImageIOStreamingTest_BINARY_DIR}/ImageStreamedFilteringTest01
     ${BrainWeb_DATA_ROOT}/brainweb165a10f17.yaff
     ${TESTING_OUTPUT_DIR}/ImageStreamedFilteringTest04Output.mha
     6
     )
ENDIF(USE_YAFF)

ELSE(brainweb165a10f17YAFF_FOUND)
  MESSAGE("The file brainweb165a10f17.yaff was not found in your BrainWeb directory,
           you can copy it from the Testing/Data/Input directory.")
ENDIF(brainweb165a10f17YAFF_FOUND)

ENDIF(BrainWeb_DATA_ROOT)


