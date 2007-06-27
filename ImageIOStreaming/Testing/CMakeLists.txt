PROJECT(ImageIOStreamingTest)

INCLUDE_DIRECTORIES( ${ImageIOStreamingTest_SOURCE_DIR} )

SET( ImageIOStreamingTest_SRC
ImageReadRegionOfInterestWrite.cxx)

ADD_EXECUTABLE( ImageIOStreamingTest ${ImageIOStreamingTest_SRC} )
TARGET_LINK_LIBRARIES( ImageIOStreamingTest ITKIOStreaming )

ADD_TEST( ImageIOStreamingTest01 ${ImageIOStreamingTest_BINARY_DIR}/ImageIOStreamingTest ${ImageIOStreamingTest_SOURCE_DIR}/BrainT1Slice.png ${ImageIOStreamingTest_BINARY_DIR}/xx.png 30 40 80 90)

