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

ADD_EXECUTABLE(itkJPEG2000ImageIOTest04 itkJPEG2000ImageIOTest04.cxx)
TARGET_LINK_LIBRARIES(itkJPEG2000ImageIOTest04 JPEG2000ImageIO)

ADD_TEST(RegionTest00
  ${CXX_TEST_PATH}/ImageReadRegionOfInterestWrite
  ${TEST_DATA_ROOT}/Input/Bretagne1.j2k
  ${TEMP}/Bretagne1_RegionTest01.tif
  160 120
  320 240
  )


IF(CLIF2007_DATA_ROOT)

  FUNCTION(MOSAIC_TEST TESTID INITIAL_X INITIAL_Y TILE_SIZE_X TILE_SIZE_Y)
#     SET(TILE_SIZE_X 256)
#     SET(TILE_SIZE_Y 256)
    ADD_TEST(MosaicTest${TESTID}
      ${CXX_TEST_PATH}/ImageReadRegionOfInterestWrite
      ${CLIF2007_DATA_ROOT}/mosaic_im100010_05.jp2
      ${TEMP}/mosaic_im100010_05_tile_${TESTID}.png
      ${INITIAL_X}   ${INITIAL_Y}
      ${TILE_SIZE_X} ${TILE_SIZE_Y}
      )
  ENDFUNCTION(MOSAIC_TEST)

  MOSAIC_TEST( 00    0    0  256 256 )
  MOSAIC_TEST( 01    0  256  256 256 )
  MOSAIC_TEST( 10  256    0  256 256 )
  MOSAIC_TEST( 11  256  256  256 256 )
  MOSAIC_TEST( 02    0  512  256 256 )
  MOSAIC_TEST( 20  512    0  256 256 )
  MOSAIC_TEST( 22  512  512  256 256 )
  MOSAIC_TEST( 21  512  256  256 256 )
  MOSAIC_TEST( 12  256  512  256 256 )
  MOSAIC_TEST( 03    0    0  2048 2048 )
  MOSAIC_TEST( 30  128  128  2048 2048 )

ENDIF(CLIF2007_DATA_ROOT)


ADD_TEST(JpegTest00  ${CXX_TEST_PATH}/itkJPEG2000ImageIOFactoryTest01 )
ADD_TEST(JpegTest01  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest00 )

# tests for reading 8-bit single component data
ADD_TEST(JpegTest02  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest01 ${TEST_DATA_ROOT}/Input/Bretagne0.j2k ${TEMP}/Bretagne0_01.png )
ADD_TEST(JpegTest03  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest01 ${TEST_DATA_ROOT}/Input/file4.jp2 ${TEMP}/file4.png )

# tests for writing 8-bit single component data
ADD_TEST(JpegTest04  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest01 ${TEST_DATA_ROOT}/Input/Bretagne0.bmp ${TEMP}/Bretagne0_02.j2k )

# tests for reading 16-bit single component data
ADD_TEST(JpegTest05  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest02 ${TEST_DATA_ROOT}/Input/test16bit.j2k ${TEMP}/test16bit_01.png )

# tests for writing 16-bit single component data
ADD_TEST(JpegTest06  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest02 ${TEST_DATA_ROOT}/Input/test16bit.png ${TEMP}/test16bit_02.j2k )

# tests for reading 3 component data with 8bits per component
ADD_TEST(JpegTest07  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest03 ${TEST_DATA_ROOT}/Input/Bretagne1.j2k ${TEMP}/Bretagne1_01.png )
ADD_TEST(JpegTest08  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest03 ${TEST_DATA_ROOT}/Input/Cevennes2.jp2 ${TEMP}/Cevennes2_01.png )

# tests for writing 3 component data with 8bits per component
ADD_TEST(JpegTest09  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest03 ${TEST_DATA_ROOT}/Input/Bretagne1.tif ${TEMP}/Bretagne1_02.j2k )

# tests for writing 3 component data with 8bits per component and specified tile size
ADD_TEST(JpegTest10  ${CXX_TEST_PATH}/itkJPEG2000ImageIOTest04 ${TEST_DATA_ROOT}/Input/Bretagne2.bmp ${TEMP}/Bretagne2_01.j2k 640 480 )
