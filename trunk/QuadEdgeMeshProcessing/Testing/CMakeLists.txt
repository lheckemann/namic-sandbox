
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

ADD_EXECUTABLE(itkConformalFlatteningQuadEdgeMeshFilterTest itkConformalFlatteningQuadEdgeMeshFilterTest.cxx )
TARGET_LINK_LIBRARIES(itkConformalFlatteningQuadEdgeMeshFilterTest ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkQuadEdgeMeshSplitFilterTest itkQuadEdgeMeshSplitFilterTest.cxx)
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshSplitFilterTest ITKQuadEdgeMesh ITKIO)

ADD_EXECUTABLE(itkQuadEdgeMeshToSphereFilterTest
itkQuadEdgeMeshToSphereFilterTest.cxx )
TARGET_LINK_LIBRARIES( itkQuadEdgeMeshToSphereFilterTest ITKQuadEdgeMesh ITKIO)

ADD_EXECUTABLE(itkQuadEdgeMeshSphericalParameterizationTest
itkQuadEdgeMeshSphericalParameterizationTest.cxx )
TARGET_LINK_LIBRARIES( itkQuadEdgeMeshSphericalParameterizationTest
ITKQuadEdgeMesh ITKIO)

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


# ADD ONE SMALL MESH AS DATA...
# ADD_TEST(itkQuadEdgeMeshSplitFilterTest
#   ${CXX_TEST_PATH}/itkQuadEdgeMeshSplitFilterTest ...)
#ADD_TEST(itkQuadEdgeMeshToSphereFilterTest
#   ${CXX_TEST_PATH}/itkQuadEdgeMeshToSphereFilterTest ..)

ADD_TEST(itkMeanSquaresMeshToMeshMetricTest1
  ${CXX_TEST_PATH}/itkMeanSquaresMeshToMeshMetricTest1
  )

ADD_TEST(itkConformalFlatteningQuadEdgeMeshFilterTest01
  ${CXX_TEST_PATH}/itkConformalFlatteningQuadEdgeMeshFilterTest
  ${TEMP}/itkFreeSurferBinarySurfaceReaderTest2.vtk
  ${TEMP}/itkConformalFlatteningQuadEdgeMeshFilterTest01.vtk
  0    # Polar cell Id
  1.0  # Scale
  1    # Map to sphere
  )

ADD_TEST(itkConformalFlatteningQuadEdgeMeshFilterTest02
  ${CXX_TEST_PATH}/itkConformalFlatteningQuadEdgeMeshFilterTest
  ${TEMP}/itkFreeSurferBinarySurfaceReaderTest4.vtk
  ${TEMP}/itkConformalFlatteningQuadEdgeMeshFilterTest02.vtk
  0    # Polar cell Id
  1.0  # Scale
  1    # Map to sphere
  )
