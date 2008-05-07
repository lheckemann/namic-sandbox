
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${VectorImageRegistration_SOURCE_DIR})
SET(TEMP ${VectorImageRegistration_BINARY_DIR}/Testing/Temporary)

SET(BASIC_SOURCES
  ../Source/itkNumericTraitsFixedArrayPixel.cxx
  )

ADD_EXECUTABLE(VectorImageRegistrationTest1 VectorImageRegistrationTest1.cxx ${BASIC_SOURCES})
TARGET_LINK_LIBRARIES(VectorImageRegistrationTest1 ITKIO ITKNumerics)

ADD_TEST(VectorImageRegistrationTest1 ${CXX_TEST_PATH}/VectorImageRegistrationTest1 )

