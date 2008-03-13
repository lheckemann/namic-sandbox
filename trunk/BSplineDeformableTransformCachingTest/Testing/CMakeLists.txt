# Testing directory

ADD_EXECUTABLE(MattesMetricCachingTest MattesMetricCachingTest.cxx itkExperimentTimeProbesCollector.cxx itkExperimentMemoryProbesCollector.cxx)
ADD_EXECUTABLE(MattesMetricPDFJacobianFlatteningTest MattesMetricPDFJacobianFlatteningTest.cxx itkExperimentTimeProbesCollector.cxx )

# Win32 only
IF (WIN32)
  SET( MEMORY_LIBS MemoryUsage Psapi.lib )
ELSE (WIN32)
  SET( MEMORY_LIBS MemoryUsage )
ENDIF (WIN32)

SET(TEMP ${MattesMetricCachingTest_BINARY_DIR}/Testing/Temporary)
SET(BASELINE ${ITK_SOURCE_DIR}/Testing/Data/Baseline/Registration)

SET(REGISTRATION_EXAMPLES ${EXECUTABLE_OUTPUT_PATH}/RegistrationExamplesTests)

INCLUDE_DIRECTORIES(${MattesMetricCachingTest_SOURCE_DIR}/Source)

TARGET_LINK_LIBRARIES(MattesMetricCachingTest ITKIO ITKAlgorithms ITKNumerics ${MEMORY_LIBS})
TARGET_LINK_LIBRARIES(MattesMetricPDFJacobianFlatteningTest ITKIO ITKAlgorithms ITKNumerics ${MEMORY_LIBS})

ADD_EXECUTABLE(MemoryUsageExample MemoryUsageExample.cxx)
TARGET_LINK_LIBRARIES(MemoryUsageExample ITKIO ITKAlgorithms ITKNumerics ${MEMORY_LIBS})

ADD_EXECUTABLE(MachineInformationExample MachineInformationExample.cxx MachineInformation.cxx)
TARGET_LINK_LIBRARIES(MachineInformationExample itksys)

ADD_EXECUTABLE(MultiThreadedMIMetricTest MultiThreadedMIMetricTest.cxx itkExperimentTimeProbesCollector.cxx itkExperimentMemoryProbesCollector.cxx)
TARGET_LINK_LIBRARIES(MultiThreadedMIMetricTest ITKIO ITKAlgorithms ITKNumerics ${MEMORY_LIBS})

ADD_EXECUTABLE(RegistrationExamplesTests RegistrationExamplesTests.cxx )
TARGET_LINK_LIBRARIES(RegistrationExamplesTests ITKStatistics ITKIO ITKNumerics ${MEMORY_LIBS})

ADD_EXECUTABLE(ImageRegistration13 ImageRegistration13.cxx )
TARGET_LINK_LIBRARIES(ImageRegistration13 ITKStatistics ITKIO ITKNumerics ${MEMORY_LIBS})

ADD_EXECUTABLE(MultiResImageRegistration2 MultiResImageRegistration2.cxx )
TARGET_LINK_LIBRARIES(MultiResImageRegistration2 ITKStatistics ITKIO ITKNumerics ${MEMORY_LIBS})

ADD_TEST(ImageRegistration13TestNAMICSandbox ${REGISTRATION_EXAMPLES}
  --compare ${BASELINE}/ImageRegistration13Test.png
            ${TEMP}/ImageRegistration13TestNAMICSandbox.png
  ImageRegistration13TestNAMICSandbox
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceBorder20.png
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceR10X13Y17.png
        ${TEMP}/ImageRegistration13TestNAMICSandbox.png
)

ADD_TEST(ImageRegistration13TestITKCVS ${REGISTRATION_EXAMPLES}
  --compare ${BASELINE}/ImageRegistration13Test.png
            ${TEMP}/ImageRegistration13TestITKCVS.png
  ImageRegistration13TestITKCVS
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceBorder20.png
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceR10X13Y17.png
        ${TEMP}/ImageRegistration13TestITKCVS.png
)


ADD_TEST(ImageRegistration13TestNoPDFJacobian ${REGISTRATION_EXAMPLES}
  --compare ${BASELINE}/ImageRegistration13Test.png
            ${TEMP}/ImageRegistration13TestNoPDFJacobian.png
  ImageRegistration13TestNoPDFJacobian
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceBorder20.png
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceR10X13Y17.png
        ${TEMP}/ImageRegistration13TestNoPDFJacobian.png
)

ADD_TEST(ImageRegistration13TestNoPDFJacobianB ${REGISTRATION_EXAMPLES}
  ImageRegistration13TestNoPDFJacobian
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceBorder20.png
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceR10X13Y17.png
        ${TEMP}/ImageRegistration13TestNoPDFJacobian.png
)

ADD_TEST(ImageRegistration13TestNoCaching ${REGISTRATION_EXAMPLES}
  --compare ${BASELINE}/ImageRegistration13Test.png
            ${TEMP}/ImageRegistration13TestNoCaching.png
  ImageRegistration13TestNoCaching
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceBorder20.png
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceR10X13Y17.png
        ${TEMP}/ImageRegistration13TestNoCaching.png
)

ADD_TEST(MultiRes2Test ${REGISTRATION_EXAMPLES}
  --compare ${BASELINE}/MultiResImageRegistration2Test.png
            ${TEMP}/MultiResImageRegistration2Test.png
  --compareNumberOfPixelsTolerance 100
  MultiResImageRegistration2Test
        ${ITK_SOURCE_DIR}/Examples/Data/BrainT1SliceBorder20.png
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceShifted13x17y.png
        ${TEMP}/MultiResImageRegistration2Test.png  0
)

ADD_TEST(MultiRes2TestNoPDFJacobian ${REGISTRATION_EXAMPLES}
  --compare ${BASELINE}/MultiResImageRegistration2Test.png
            ${TEMP}/MultiResImageRegistration2TestNoPDFJacobian.png
  --compareNumberOfPixelsTolerance 100
  MultiResImageRegistration2TestNoPDFJacobian
        ${ITK_SOURCE_DIR}/Examples/Data/BrainT1SliceBorder20.png
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceShifted13x17y.png
        ${TEMP}/MultiResImageRegistration2TestNoPDFJacobian.png  0
)

ADD_TEST(MultiRes2TestNoPDFJacobianB ${REGISTRATION_EXAMPLES}
  MultiResImageRegistration2TestNoPDFJacobian
        ${ITK_SOURCE_DIR}/Examples/Data/BrainT1SliceBorder20.png
        ${ITK_SOURCE_DIR}/Examples/Data/BrainProtonDensitySliceShifted13x17y.png
        ${TEMP}/MultiResImageRegistration2TestNoPDFJacobian.png  0
)


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

