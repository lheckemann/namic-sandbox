
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

# Add path to find itkTestingMacros.h
INCLUDE_DIRECTORIES(
  ${SBIA_HARDI_SOURCE_DIR}/Testing
  )

ADD_EXECUTABLE(itkRealSymSphericalHarmonicBasisTest itkRealSymSphericalHarmonicBasisTest.cxx ${VNL_TEMPLATE_SOURCES} ${SpecialFunctions_SOURCES} )
TARGET_LINK_LIBRARIES(itkRealSymSphericalHarmonicBasisTest ITKNumerics ITKCommon)
ADD_TEST(itkRealSymSphericalHarmonicBasisTest ${CXX_TEST_PATH}/itkRealSymSphericalHarmonicBasisTest )

ADD_EXECUTABLE(itkSymRealSphericalHarmonicRepTest1 itkSymRealSphericalHarmonicRepTest1.cxx ${VNL_TEMPLATE_SOURCES} ${SpecialFunctions_SOURCES} )
TARGET_LINK_LIBRARIES(itkSymRealSphericalHarmonicRepTest1  ITKCommon )
ADD_TEST(itkSymRealSphericalHarmonicRepTest1 ${CXX_TEST_PATH}/itkSymRealSphericalHarmonicRepTest1 )

ADD_EXECUTABLE(itkVariableLengthVectorCastImageFilterTest1 itkVariableLengthVectorCastImageFilterTest1.cxx ${VNL_TEMPLATE_SOURCES} ${SpecialFunctions_SOURCES} )
TARGET_LINK_LIBRARIES(itkVariableLengthVectorCastImageFilterTest1 ITKNumerics ITKCommon)
ADD_TEST(itkVariableLengthVectorCastImageFilterTest1 ${CXX_TEST_PATH}/itkVariableLengthVectorCastImageFilterTest1 )

ADD_EXECUTABLE(SymRshRepReadWriteTest SymRshRepReadWriteTest.cxx ${VNL_TEMPLATE_SOURCES} ${SpecialFunctions_SOURCES} )
TARGET_LINK_LIBRARIES(SymRshRepReadWriteTest ITKNumerics ITKCommon ITKIO)
ADD_TEST(SymRshRepReadWriteTest ${CXX_TEST_PATH}/SymRshRepReadWriteTest )

ADD_EXECUTABLE(itkSymRshReconImageFilterTest1 itkSymRshReconImageFilterTest1.cxx ${SpecialFunctions_SOURCES} )
TARGET_LINK_LIBRARIES(itkSymRshReconImageFilterTest1  ITKCommon )
ADD_TEST(itkSymRshReconImageFilterTest ${CXX_TEST_PATH}/itkSymRshReconImageFilterTest1 )

ADD_EXECUTABLE(itkOdfReconImageFilterTest1 itkOdfReconImageFilterTest1.cxx ${SpecialFunctions_SOURCES} )
TARGET_LINK_LIBRARIES(itkOdfReconImageFilterTest1  ITKCommon )
ADD_TEST(itkOdfReconImageFilterTest ${CXX_TEST_PATH}/itkOdfReconImageFilterTest1 )

ADD_EXECUTABLE(itkFiberImpulseResponseImageCalculatorTest itkFiberImpulseResponseImageCalculatorTest.cxx ${SpecialFunctions_SOURCES} )
TARGET_LINK_LIBRARIES(itkFiberImpulseResponseImageCalculatorTest  ITKCommon )
ADD_TEST(itkFiberImpulseResponseImageCalculatorTest ${CXX_TEST_PATH}/itkFiberImpulseResponseImageCalculatorTest )

ADD_EXECUTABLE(itkFodCsdReconImageFilterTest1 itkFodCsdReconImageFilterTest1.cxx ${SpecialFunctions_SOURCES} )
TARGET_LINK_LIBRARIES(itkFodCsdReconImageFilterTest1  ITKCommon )
ADD_TEST(itkFodCsdReconImageFilterTest ${CXX_TEST_PATH}/itkFodCsdReconImageFilterTest1 )

ADD_EXECUTABLE(SymRSHPowerImageFilterTest1 SymRSHPowerImageFilterTest1.cxx ${SpecialFunctions_SOURCES} )
TARGET_LINK_LIBRARIES(SymRSHPowerImageFilterTest1  ITKCommon )
ADD_TEST(SymRSHPowerImageFilterTest ${CXX_TEST_PATH}/SymRSHPowerImageFilterTest1 )

ADD_EXECUTABLE(itkReplaceSpecialFunctionsTest itkReplaceSpecialFunctionsTest.cxx ${SpecialFunctions_SOURCES} )
TARGET_LINK_LIBRARIES(itkReplaceSpecialFunctionsTest  ITKCommon )
ADD_TEST(itkReplaceSpecialFunctionsTest ${CXX_TEST_PATH}/itkReplaceSpecialFunctionsTest )

SET(TEST_DATA_ROOT ${PROJECT_SOURCE_DIR}/Data )
SET(TEMP_DATA_DIR  ${PROJECT_BINARY_DIR}/Testing/Temporary)

#run regression tests
if (BUILD_EXECS)

  #RSH computations
  ADD_TEST( computeRshCoeffsRegTest ${CXX_TEST_PATH}/computeRshCoeffs
    -d ${TEST_DATA_ROOT}/template_dwi.hdr
    -m ${TEST_DATA_ROOT}/template_dwi_mask.hdr
    -r ${TEST_DATA_ROOT}/template_dwi.bvec
    -l 8
    -p rshOut
    -o ${TEMP_DATA_DIR}
   )
   #should do regression here!
   
  #ODF computations
  ADD_TEST( odfReconRegTest ${CXX_TEST_PATH}/odfRecon
    -d ${TEST_DATA_ROOT}/template_dwi.hdr
    -m ${TEST_DATA_ROOT}/template_dwi_mask.hdr
    -r ${TEST_DATA_ROOT}/template_dwi.bvec
    -l 8
    -p odfOut
    -o ${TEMP_DATA_DIR}
   )

  #FOD computations
  ADD_TEST( calcFirResponseRegTest ${CXX_TEST_PATH}/calcFirResponse
    -d ${TEST_DATA_ROOT}/template_dwi.hdr
    -m ${TEST_DATA_ROOT}/template_dwi_mask.hdr
    -r ${TEST_DATA_ROOT}/template_dwi.bvec
    -b ${TEST_DATA_ROOT}/template_dwi.bval
    -l 8
    -p firResp
    -o ${TEMP_DATA_DIR}
  )
  
  ADD_TEST( fodReconRegTest ${CXX_TEST_PATH}/fodRecon
    -d ${TEST_DATA_ROOT}/template_dwi.hdr
    -m ${TEST_DATA_ROOT}/template_dwi_mask.hdr
    -r ${TEST_DATA_ROOT}/template_dwi.bvec
    -R ${TEMP_DATA_DIR}/firResp.txt
    -l 8
    -p fodOut
    -o ${TEMP_DATA_DIR}
   )
   #should do regression here!

endif(BUILD_EXECS)

