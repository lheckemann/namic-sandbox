
ENABLE_TESTING()

# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(testOdfImFilter testOdfImFilter.cxx)
TARGET_LINK_LIBRARIES(testOdfImFilter ITKIO)
ADD_TEST(testOdfImFilter ${CXX_TEST_PATH}/testOdfImFilter )

ADD_EXECUTABLE(testOrientationDistributionFunction testOrientationDistributionFunction.cxx)
TARGET_LINK_LIBRARIES(testOrientationDistributionFunction ITKIO)
ADD_TEST(testOrientationDistributionFunction ${CXX_TEST_PATH}/testOrientationDistributionFunction )

ADD_EXECUTABLE(testOdfReadWrite testOdfReadWrite.cxx)
TARGET_LINK_LIBRARIES(testOdfReadWrite ITKIO)
ADD_TEST(testOdfReadWrite ${CXX_TEST_PATH}/testOdfReadWrite )

