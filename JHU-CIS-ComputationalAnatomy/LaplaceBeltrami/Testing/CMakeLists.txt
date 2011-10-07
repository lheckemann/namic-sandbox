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

ADD_EXECUTABLE(itkLaplaceBeltramiFilterTest1 itkLaplaceBeltramiFilterTest1.cxx )
ADD_EXECUTABLE(itkLaplaceBeltramiFilterTest2 itkLaplaceBeltramiFilterTest2.cxx )
TARGET_LINK_LIBRARIES(itkLaplaceBeltramiFilterTest1 ${ITK_LIBRARIES} )
TARGET_LINK_LIBRARIES(itkLaplaceBeltramiFilterTest2 ${ITK_LIBRARIES} )

###################
###################
#
#  Define Tests
#
###################
###################

ADD_TEST(itkLaplaceBeltramiFilterTest1
  ${CXX_TEST_PATH}/itkLaplaceBeltramiFilterTest1
  ${TEST_DATA_ROOT}/fv0.vtk
  ${TEMP}/fv0_surfaceHarmonic_0_Test1.vtk
  )

ADD_TEST(itkLaplaceBeltramiFilterTest2
  ${CXX_TEST_PATH}/itkLaplaceBeltramiFilterTest2
  -e 10
  ${TEST_DATA_ROOT}/fv0.vtk
  ${TEMP}/fv0_surfaceHarmonic_0_Test2.vtk
  )

ADD_TEST(itkLaplaceBeltramiFilterTest2Open
  ${CXX_TEST_PATH}/itkLaplaceBeltramiFilterTest2
  -e 10
  ${TEST_DATA_ROOT}/NewOpenSurface.vtk
  ${TEMP}/NewOpenSurfaceFirstHarm.vtk
  )



