
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${QuadEdgeMeshProcessing_SOURCE_DIR})
SET(TEMP ${QuadEdgeMeshProcessing_BINARY_DIR}/Testing/Temporary)

#######################
#######################
#
#  Define Executables
#
#######################
#######################

ADD_EXECUTABLE(itkQuadEdgeMeshTest1 itkQuadEdgeMeshTest1.cxx )
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkRegularSphereQuadEdgeMeshSourceTest1 itkRegularSphereQuadEdgeMeshSourceTest1.cxx )
TARGET_LINK_LIBRARIES(itkRegularSphereQuadEdgeMeshSourceTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)



###################
###################
#
#  Define Tests
#
###################
###################

ADD_TEST(itkQuadEdgeMeshTest1 
  ${CXX_TEST_PATH}/itkQuadEdgeMeshTest1
 )

ADD_TEST(itkRegularSphereQuadEdgeMeshSourceTest1
  ${CXX_TEST_PATH}/itkRegularSphereQuadEdgeMeshSourceTest1
  ${TEMP}/itkRegularSphereQuadEdgeMeshSourceTest1.vtk
 )


