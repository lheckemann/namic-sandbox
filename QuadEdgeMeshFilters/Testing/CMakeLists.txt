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


ADD_EXECUTABLE(itkShiftScalarsQuadEdgeMeshFilterTest itkShiftScalarsQuadEdgeMeshFilterTest.cxx )
TARGET_LINK_LIBRARIES(itkShiftScalarsQuadEdgeMeshFilterTest ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkWindowScalarsQuadEdgeMeshFilterTest itkWindowScalarsQuadEdgeMeshFilterTest.cxx )
TARGET_LINK_LIBRARIES(itkWindowScalarsQuadEdgeMeshFilterTest ITKQuadEdgeMesh ITKIO ITKNumerics)


###################
###################
#
#  Define Tests
#
###################
###################

ADD_TEST(itkShiftScalarsQuadEdgeMeshFilterTest
  ${CXX_TEST_PATH}/itkShiftScalarsQuadEdgeMeshFilterTest
  ${TEST_DATA_ROOT}/SimpleSurface1.vtk 
  ${TEMP}/SimpleSurface1_ScalarShifted.vtk
  $1.0 #shift
  $2.0 #scale
 )
 
ADD_TEST(itkWindowScalarsQuadEdgeMeshFilterTest
  ${CXX_TEST_PATH}/itkWindowScalarsQuadEdgeMeshFilterTest
  ${TEST_DATA_ROOT}/SimpleSurface1.vtk 
  ${TEMP}/SimpleSurface1_Windowed.vtk 
  $-2.0 #window minimum
  $0.0  #window maximum
  $0.0  #output minimum
  $1.0  #output maximum
 )
