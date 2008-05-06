
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${SurfaceModelling_SOURCE_DIR})
SET(TEMP ${SurfaceModelling_BINARY_DIR}/Testing/Temporary)

ADD_EXECUTABLE(SurfaceModellingTest1 itkSurfaceModellingTest1.cxx )
TARGET_LINK_LIBRARIES(SurfaceModellingTest1 ITKIO ITKNumerics)

ADD_EXECUTABLE(QuadraticQuaternionPolynomialTest1 itkQuadraticQuaternionPolynomialTest1.cxx )
TARGET_LINK_LIBRARIES(QuadraticQuaternionPolynomialTest1 ITKIO ITKNumerics)

ADD_EXECUTABLE(QuaternionTest1 itkQuaternionTest1.cxx )
TARGET_LINK_LIBRARIES(QuaternionTest1 ITKIO ITKNumerics)


ADD_TEST(SurfaceModellingTest1 ${CXX_TEST_PATH}/SurfaceModellingTest1 )
ADD_TEST(VersorQuadraticInterpolatorTest1 ${CXX_TEST_PATH}/VersorQuadraticInterpolatorTest1) 
ADD_TEST(QuaternionTest1 ${CXX_TEST_PATH}/QuaternionTest1) 

