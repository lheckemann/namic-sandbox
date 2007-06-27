PROJECT(ImageIOStreamingTest)

INCLUDE_DIRECTORIES( ${ImageIOStreamingTest_SOURCE_DIR} )

SET( ImageIOStreamingTest_SRC
ImageReadRegionOfInterestWrite.cxx)

ADD_EXECUTABLE( ImageIOStreamingTest ${ImageIOStreamingTest_SRC} )
TARGET_LINK_LIBRARIES( ImageIOStreamingTest ITKIO )

ADD_TEST( ImageIOStreamingTest01 ImageIOStreamingTest )

