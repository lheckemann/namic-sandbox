# Testing directory

ADD_EXECUTABLE(MattesMetricCachingTest MattesMetricCachingTest.cxx itkExperimentTimeProbesCollector.cxx )
ADD_EXECUTABLE(MattesMetricPDFJacobianFlatteningTest MattesMetricPDFJacobianFlatteningTest.cxx itkExperimentTimeProbesCollector.cxx )

# Win32 only
IF (WIN32)
  SET( MEMORY_LIBS MemoryUsage Psapi.lib )
ELSE (WIN32)
  SET( MEMORY_LIBS MemoryUsage )
ENDIF (WIN32)

INCLUDE_DIRECTORIES(${MattesMetricCachingTest_SOURCE_DIR}/Source)

TARGET_LINK_LIBRARIES(MattesMetricCachingTest ITKIO ITKAlgorithms ITKNumerics ${MEMORY_LIBS})
TARGET_LINK_LIBRARIES(MattesMetricPDFJacobianFlatteningTest ITKIO ITKAlgorithms ITKNumerics ${MEMORY_LIBS})

ADD_EXECUTABLE(MemoryUsageExample MemoryUsageExample.cxx)
TARGET_LINK_LIBRARIES(MemoryUsageExample ITKIO ITKAlgorithms ITKNumerics ${MEMORY_LIBS})

ADD_EXECUTABLE(MachineInformationExample MachineInformationExample.cxx MachineInformation.cxx)
TARGET_LINK_LIBRARIES(MachineInformationExample itksys)

ADD_EXECUTABLE(MultiThreadedMIMetricTest MultiThreadedMIMetricTest.cxx itkExperimentTimeProbesCollector.cxx itkExperimentMemoryProbesCollector.cxx)
TARGET_LINK_LIBRARIES(MultiThreadedMIMetricTest ITKIO ITKAlgorithms ITKNumerics ${MEMORY_LIBS})

IF(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  ADD_EXECUTABLE(TestParseSmaps TestParseSmaps.cxx)
ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )

#ADD_EXECUTABLE(TestParseSmaps2 TestParseSmaps2.cxx)

# Set in top-level CMakeLists.txt
#SET(NUMBER_OF_CALLS 10)
#SET( SIZE_LIST 50 100 150 204 )

FOREACH( SIZE ${SIZE_LIST} )
  SET( IMAGE_SIZE ${SIZE} ${SIZE} ${SIZE} )
  ADD_TEST( MattesWithCaching${SIZE} MattesMetricCachingTest  ${IMAGE_SIZE} ${NUMBER_OF_CALLS} 1 )
  ADD_TEST( MattesWithoutCaching${SIZE} MattesMetricCachingTest  ${IMAGE_SIZE} ${NUMBER_OF_CALLS} 0 )
  ADD_TEST( MattesWithPDFJacobian${SIZE} MattesMetricPDFJacobianFlatteningTest  ${IMAGE_SIZE} ${NUMBER_OF_CALLS} 1 )
  ADD_TEST( MattesWithoutPDFJacobian${SIZE} MattesMetricPDFJacobianFlatteningTest  ${IMAGE_SIZE} ${NUMBER_OF_CALLS} 0 )
ENDFOREACH( SIZE ${SIZE_LIST} )

