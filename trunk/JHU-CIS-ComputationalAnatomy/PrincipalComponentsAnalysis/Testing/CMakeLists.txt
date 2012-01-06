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

ADD_EXECUTABLE(itkScalarPCATest itkScalarPCATest.cxx )
ADD_EXECUTABLE(itkVectorKernelPCATest itkVectorKernelPCATest.cxx )
TARGET_LINK_LIBRARIES(itkScalarPCATest ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(itkVectorKernelPCATest ${ITK_LIBRARIES})

###################
###################
#
#  Define Tests
#
###################
###################

ADD_TEST(itkVectorKernelPCATest
  ${CXX_TEST_PATH}/itkVectorKernelPCATest
  -c 5 -s 6.75
  ${TEST_DATA_ROOT}/PCATestSurface.vtk ${TEMP}/VectorKernelPCAOutput
  ${TEST_DATA_ROOT}/alpha0_01
  ${TEST_DATA_ROOT}/alpha0_02
  ${TEST_DATA_ROOT}/alpha0_03
  ${TEST_DATA_ROOT}/alpha0_04
  ${TEST_DATA_ROOT}/alpha0_05
  ${TEST_DATA_ROOT}/alpha0_06
  ${TEST_DATA_ROOT}/alpha0_07
  ${TEST_DATA_ROOT}/alpha0_08
  ${TEST_DATA_ROOT}/alpha0_09
  ${TEST_DATA_ROOT}/alpha0_10
  ${TEST_DATA_ROOT}/alpha0_11
  ${TEST_DATA_ROOT}/alpha0_12
  ${TEST_DATA_ROOT}/alpha0_13
  ${TEST_DATA_ROOT}/alpha0_14
  ${TEST_DATA_ROOT}/alpha0_15
  ${TEST_DATA_ROOT}/alpha0_16
  ${TEST_DATA_ROOT}/alpha0_17
  ${TEST_DATA_ROOT}/alpha0_18
  ${TEST_DATA_ROOT}/alpha0_19
  ${TEST_DATA_ROOT}/alpha0_20
  ${TEST_DATA_ROOT}/alpha0_21
  ${TEST_DATA_ROOT}/alpha0_22
  ${TEST_DATA_ROOT}/alpha0_23
  ${TEST_DATA_ROOT}/alpha0_24
  ${TEST_DATA_ROOT}/alpha0_25
  ${TEST_DATA_ROOT}/alpha0_26
  ${TEST_DATA_ROOT}/alpha0_27
  ${TEST_DATA_ROOT}/alpha0_28
  ${TEST_DATA_ROOT}/alpha0_29
  ${TEST_DATA_ROOT}/alpha0_30
  ${TEST_DATA_ROOT}/alpha0_31
  ${TEST_DATA_ROOT}/alpha0_32
  ${TEST_DATA_ROOT}/alpha0_33
  ${TEST_DATA_ROOT}/alpha0_34
  ${TEST_DATA_ROOT}/alpha0_35
  ${TEST_DATA_ROOT}/alpha0_36
  ${TEST_DATA_ROOT}/alpha0_37
  ${TEST_DATA_ROOT}/alpha0_38
  ${TEST_DATA_ROOT}/alpha0_39
  ${TEST_DATA_ROOT}/alpha0_40
  )

ADD_TEST(itkScalarPCATest
  ${CXX_TEST_PATH}/itkScalarPCATest
  -c 5
  ${TEMP}/ScalarPcaOutput
  ${TEST_DATA_ROOT}/J0_01
  ${TEST_DATA_ROOT}/J0_02
  ${TEST_DATA_ROOT}/J0_03
  ${TEST_DATA_ROOT}/J0_04
  ${TEST_DATA_ROOT}/J0_05
  ${TEST_DATA_ROOT}/J0_06
  ${TEST_DATA_ROOT}/J0_07
  ${TEST_DATA_ROOT}/J0_08
  ${TEST_DATA_ROOT}/J0_09
  ${TEST_DATA_ROOT}/J0_10
  ${TEST_DATA_ROOT}/J0_11
  ${TEST_DATA_ROOT}/J0_12
  ${TEST_DATA_ROOT}/J0_13
  ${TEST_DATA_ROOT}/J0_14
  ${TEST_DATA_ROOT}/J0_15
  ${TEST_DATA_ROOT}/J0_16
  ${TEST_DATA_ROOT}/J0_17
  ${TEST_DATA_ROOT}/J0_18
  ${TEST_DATA_ROOT}/J0_19
  ${TEST_DATA_ROOT}/J0_20
  ${TEST_DATA_ROOT}/J0_21
  ${TEST_DATA_ROOT}/J0_22
  ${TEST_DATA_ROOT}/J0_23
  ${TEST_DATA_ROOT}/J0_24
  ${TEST_DATA_ROOT}/J0_25
  ${TEST_DATA_ROOT}/J0_26
  ${TEST_DATA_ROOT}/J0_27
  ${TEST_DATA_ROOT}/J0_28
  ${TEST_DATA_ROOT}/J0_29
  ${TEST_DATA_ROOT}/J0_30
  ${TEST_DATA_ROOT}/J0_31
  ${TEST_DATA_ROOT}/J0_32
  ${TEST_DATA_ROOT}/J0_33
  ${TEST_DATA_ROOT}/J0_34
  ${TEST_DATA_ROOT}/J0_35
  ${TEST_DATA_ROOT}/J0_36
  ${TEST_DATA_ROOT}/J0_37
  ${TEST_DATA_ROOT}/J0_38
  ${TEST_DATA_ROOT}/J0_39
  ${TEST_DATA_ROOT}/J0_40
  )

