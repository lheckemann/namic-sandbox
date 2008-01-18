ADD_EXECUTABLE(MattesMetricCachingTest MattesMetricCachingTest.cxx )

# Win32 only
IF (WIN32)
  SET( MEMORY_LIBS Psapi.lib )
ELSE (WIN32)
  SET( MEMORY_LIBS  )
ENDIF (WIN32)

INCLUDE_DIRECTORIES(${MattesMetricCachingTest_SOURCE_DIR}/Source)

TARGET_LINK_LIBRARIES(MattesMetricCachingTest ITKIO ITKAlgorithms ITKNumerics ${MEMORY_LIBS})


SET(NUMBER_OF_CALLS 10)

SET( IMAGE_SIZE_X  50 )
SET( IMAGE_SIZE_Y  50 )
SET( IMAGE_SIZE_Z  50 )

SET( IMAGE_SIZE ${IMAGE_SIZE_X} ${IMAGE_SIZE_Y} ${IMAGE_SIZE_Z} )

#ADD_TEST( MattesWithCaching    MattesMetricCachingTest  ${IMAGE_SIZE} ${NUMBER_OF_CALLS} 1 )
#ADD_TEST( MattesWithoutCaching MattesMetricCachingTest  ${IMAGE_SIZE} ${NUMBER_OF_CALLS} 0 )

SET( SIZE_LIST 50 100 150 204 )

FOREACH( SIZE ${SIZE_LIST} )
  SET( IMAGE_SIZE ${SIZE} ${SIZE} ${SIZE} )
  MESSAGE("Adding test of size ${IMAGE_SIZE}")
  ADD_TEST( MattesWithCaching${SIZE} MattesMetricCachingTest  ${IMAGE_SIZE} ${NUMBER_OF_CALLS} 1 )
  ADD_TEST( MattesWithoutCaching${SIZE} MattesMetricCachingTest  ${IMAGE_SIZE} ${NUMBER_OF_CALLS} 0 )
ENDFOREACH( SIZE ${SIZE_LIST} )

