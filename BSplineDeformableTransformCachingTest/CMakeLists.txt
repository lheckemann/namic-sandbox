PROJECT(MattesMetricCachingTest)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(MattesMetricCachingTest MattesMetricCachingTest.cxx )

TARGET_LINK_LIBRARIES(MattesMetricCachingTest ITKIO ITKAlgorithms ITKNumerics)

FIND_FILE(INPUT_TEST_IMAGE inputImage.mha ~/data)

ENABLE_TESTING()

ADD_TEST( MattesWithCaching    MattesMetricCachingTest ${INPUT_TEST_IMAGE} 1000 1 )

ADD_TEST( MattesWithoutCaching MattesMetricCachingTest ${INPUT_TEST_IMAGE} 1000 0 )
