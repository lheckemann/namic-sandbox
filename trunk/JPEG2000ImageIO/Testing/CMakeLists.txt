SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR}/Data)
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)

ADD_EXECUTABLE(ImageReadRegionOfInterestWrite ImageReadRegionOfInterestWrite.cxx)
TARGET_LINK_LIBRARIES(ImageReadRegionOfInterestWrite JPEG2000ImageIO)

ADD_EXECUTABLE(itkJPEG2000ImageIOTest01 itkJPEG2000ImageIOTest01.cxx)
TARGET_LINK_LIBRARIES(itkJPEG2000ImageIOTest01 JPEG2000ImageIO)


ADD_TEST(RegionTest01  ${CXX_TEST_PATH}/ImageReadRegionOfInterestWrite ${TEST_DATA_ROOT}/Input/Bretagne1.j2k )

ADD_TEST(JpegTest01  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest01 ${TEST_DATA_ROOT}/Input/Bretagne1.j2k ${TEMP}/Bretagne1_01.mha )
ADD_TEST(JpegTest02  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest01 ${TEST_DATA_ROOT}/Input/Cevennes2.jp2 ${TEMP}/Bretagne1_02.mha )

