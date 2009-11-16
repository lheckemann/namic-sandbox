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
TARGET_LINK_LIBRARIES(itkLaplaceBeltramiFilterTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)
TARGET_LINK_LIBRARIES(itkLaplaceBeltramiFilterTest2 ITKQuadEdgeMesh ITKIO ITKNumerics)

###################
###################
#
#  Define Tests
#
###################
###################

ADD_TEST(itkLaplaceBeltramiFilterTest1
  ${CXX_TEST_PATH}/itkLaplaceBeltramiFilterTest1
  )

ADD_TEST(itkLaplaceBeltramiFilterTest2
  ${CXX_TEST_PATH}/itkLaplaceBeltramiFilterTest2
  )
