SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR}/Data)
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)


#######################
#######################
#
#  Define Executables
#
#######################
#######################

ADD_EXECUTABLE(itkPrincipalComponentsAnalysisTest1 itkPrincipalComponentsAnalysisTest1.cxx )
ADD_EXECUTABLE(itkPrincipalComponentsAnalysisTest2 itkPrincipalComponentsAnalysisTest2.cxx )
TARGET_LINK_LIBRARIES(itkPrincipalComponentsAnalysisTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)
TARGET_LINK_LIBRARIES(itkPrincipalComponentsAnalysisTest2 ITKQuadEdgeMesh ITKIO ITKNumerics)

###################
###################
#
#  Define Tests
#
###################
###################

ADD_TEST(itkPrincipalComponentsAnalysisTest1
  ${CXX_TEST_PATH}/itkPrincipalComponentsAnalysisTest1
  -c 5 -s 6.75
  ${TEST_DATA_ROOT}/PCATestSurface.vtk ${TEST_DATA_ROOT}/pcaOutput
  ${TEST_DATA_ROOT}/alpha0_*
  )

ADD_TEST(itkPrincipalComponentsAnalysisTest2
  ${CXX_TEST_PATH}/itkPrincipalComponentsAnalysisTest2
  -c 5
  ${TEST_DATA_ROOT}/scalarPcaOutput
  ${TEST_DATA_ROOT}/J0_*
  )

