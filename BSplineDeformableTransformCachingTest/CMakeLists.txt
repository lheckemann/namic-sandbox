PROJECT(MattesMetricCachingTest)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(MattesMetricCachingTest MattesMetricCachingTest.cxx )

TARGET_LINK_LIBRARIES(MattesMetricCachingTest ITKIO ITKAlgorithms ITKNumerics)

ENABLE_TESTING()

SET(NUMBER_OF_CALLS 1000)

ADD_TEST( MattesWithCaching    MattesMetricCachingTest  ${NUMBER_OF_CALLS} 1 )

ADD_TEST( MattesWithoutCaching MattesMetricCachingTest  ${NUMBER_OF_CALLS} 0 )
