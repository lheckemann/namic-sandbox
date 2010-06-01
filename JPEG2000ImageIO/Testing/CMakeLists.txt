SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR}/Data)
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)

ADD_EXECUTABLE(ImageReadRegionOfInterestWrite ImageReadRegionOfInterestWrite.cxx)
TARGET_LINK_LIBRARIES(ImageReadRegionOfInterestWrite JPEG2000ImageIO)

ADD_EXECUTABLE(itkJPEG2000ImageIOFactoryTest01 itkJPEG2000ImageIOFactoryTest01.cxx)
TARGET_LINK_LIBRARIES(itkJPEG2000ImageIOFactoryTest01 JPEG2000ImageIO)

ADD_EXECUTABLE(itkJPEG2000ImageIOTest00 itkJPEG2000ImageIOTest00.cxx)
TARGET_LINK_LIBRARIES(itkJPEG2000ImageIOTest00 JPEG2000ImageIO)

ADD_EXECUTABLE(itkJPEG2000ImageIOTest01 itkJPEG2000ImageIOTest01.cxx)
TARGET_LINK_LIBRARIES(itkJPEG2000ImageIOTest01 JPEG2000ImageIO)

ADD_EXECUTABLE(itkJPEG2000ImageIOTest02 itkJPEG2000ImageIOTest02.cxx)
TARGET_LINK_LIBRARIES(itkJPEG2000ImageIOTest02 JPEG2000ImageIO)

ADD_EXECUTABLE(itkJPEG2000ImageIOTest03 itkJPEG2000ImageIOTest03.cxx)
TARGET_LINK_LIBRARIES(itkJPEG2000ImageIOTest03 JPEG2000ImageIO)

ADD_TEST(RegionTest01  ${CXX_TEST_PATH}/ImageReadRegionOfInterestWrite ${TEST_DATA_ROOT}/Input/Bretagne1.j2k )

ADD_TEST(JpegTest00  ${CXX_TEST_PATH}/itkJPEG2000ImageIOFactoryTest01 )
ADD_TEST(JpegTest01  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest00 )

# tests for reading 8-bit single component data
ADD_TEST(JpegTest02  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest01 ${TEST_DATA_ROOT}/Input/Bretagne0.j2k ${TEMP}/Bretagne0_01.png )
ADD_TEST(JpegTest03  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest01 ${TEST_DATA_ROOT}/Input/file4.jp2 ${TEMP}/file4.png )

# tests for writing 8-bit single component data
ADD_TEST(JpegTest04  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest01 ${TEST_DATA_ROOT}/Input/Bretagne0.bmp ${TEMP}/Bretagne0_02.j2k )

# tests for reading 16-bit single component data
ADD_TEST(JpegTest05  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest02 ${TEST_DATA_ROOT}/Input/Bretagne2.j2k ${TEMP}/Bretagne2_04.png )

# tests for writing 16-bit single component data
ADD_TEST(JpegTest06  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest02 ${TEST_DATA_ROOT}/Input/Bretagne2.jpeg ${TEMP}/Bretagne2_05.j2k )

# tests for reading 3 component data with 8bits per component
ADD_TEST(JpegTest07  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest03 ${TEST_DATA_ROOT}/Input/Bretagne1.j2k ${TEMP}/Bretagne1_01.png )
ADD_TEST(JpegTest08  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest03 ${TEST_DATA_ROOT}/Input/Cevennes2.jp2 ${TEMP}/Cevennes2_01.png )

# tests for writing 3 component data with 8bits per component
ADD_TEST(JpegTest09  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest03 ${TEST_DATA_ROOT}/Input/Bretagne1.bmp ${TEMP}/Bretagne1_02.j2k )
