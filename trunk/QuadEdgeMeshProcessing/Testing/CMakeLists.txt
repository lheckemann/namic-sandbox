
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${QuadEdgeMeshProcessing_SOURCE_DIR}/Data)
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

ADD_EXECUTABLE(itkQuadEdgeMeshSplitFilterTest1 itkQuadEdgeMeshSplitFilterTest1.cxx)
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshSplitFilterTest1 ITKQuadEdgeMesh ITKIO)

ADD_EXECUTABLE(itkQuadEdgeMeshSplitFilterTest2 itkQuadEdgeMeshSplitFilterTest2.cxx)
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshSplitFilterTest2 ITKQuadEdgeMesh ITKIO)

ADD_EXECUTABLE(itkQuadEdgeMeshSplitFilterTest3 itkQuadEdgeMeshSplitFilterTest3.cxx)
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshSplitFilterTest3 ITKQuadEdgeMesh ITKIO)

ADD_EXECUTABLE(itkQuadEdgeMeshToSphereFilterTest1
itkQuadEdgeMeshToSphereFilterTest1.cxx )
TARGET_LINK_LIBRARIES( itkQuadEdgeMeshToSphereFilterTest1 ITKQuadEdgeMesh ITKIO)

ADD_EXECUTABLE(itkQuadEdgeMeshToSphereFilterTest2
itkQuadEdgeMeshToSphereFilterTest2.cxx )
TARGET_LINK_LIBRARIES( itkQuadEdgeMeshToSphereFilterTest2 ITKQuadEdgeMesh ITKIO)

ADD_EXECUTABLE(itkQuadEdgeMeshSphericalParameterizationTest
itkQuadEdgeMeshSphericalParameterizationTest.cxx )
TARGET_LINK_LIBRARIES( itkQuadEdgeMeshSphericalParameterizationTest
ITKQuadEdgeMesh ITKIO)

ADD_EXECUTABLE(itkQuadEdgeMeshBoundarySmoothFilterTest
itkQuadEdgeMeshBoundarySmoothFilterTest.cxx )
TARGET_LINK_LIBRARIES( itkQuadEdgeMeshBoundarySmoothFilterTest
ITKQuadEdgeMesh ITKIO)


# Decimation
ADD_EXECUTABLE( QuadricDecimate 
  itkQuadEdgeMeshQuadricDecimationTest.cxx 
  )
TARGET_LINK_LIBRARIES( QuadricDecimate ITKIO ITKQuadEdgeMesh )

ADD_EXECUTABLE( Decimate 
  itkQuadEdgeMeshSquaredEdgeLengthDecimationTest.cxx 
  )
TARGET_LINK_LIBRARIES( Decimate ITKIO ITKQuadEdgeMesh )

# Smoothing
ADD_EXECUTABLE( Smooth
  itkQuadEdgeMeshSmoothingTest.cxx
  )
TARGET_LINK_LIBRARIES( Smooth ITKIO ITKQuadEdgeMesh )

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

ADD_TEST(itkQuadEdgeMeshSplitFilterTest1
  ${CXX_TEST_PATH}/itkQuadEdgeMeshSplitFilterTest1
  ${TEST_DATA_ROOT}/SimpleSurface1.vtk 
  ${TEMP}/Split1_0.vtk
  ${TEMP}/Split1_1.vtk
 )
 
ADD_TEST(itkQuadEdgeMeshSplitFilterTest2
  ${CXX_TEST_PATH}/itkQuadEdgeMeshSplitFilterTest2
  ${TEST_DATA_ROOT}/SimpleSurface1.vtk 
  ${TEMP}/Split2_0.vtk
  ${TEMP}/Split2_1.vtk
 )
 
ADD_TEST(itkQuadEdgeMeshSplitFilterTest3
  ${CXX_TEST_PATH}/itkQuadEdgeMeshSplitFilterTest3
  ${TEST_DATA_ROOT}/SimpleSurface1.vtk 
  0  #seeds direction
  ${TEMP}/Split3_0.vtk
  ${TEMP}/Split3_1.vtk
 )
 
ADD_TEST(itkQuadEdgeMeshSplitFilterTest4
  ${CXX_TEST_PATH}/itkQuadEdgeMeshSplitFilterTest3
  ${TEST_DATA_ROOT}/SimpleSurface1.vtk 
  1  #seeds direction
  ${TEMP}/Split4_0.vtk
  ${TEMP}/Split4_1.vtk
 )
 
ADD_TEST(itkQuadEdgeMeshSplitFilterTest5
  ${CXX_TEST_PATH}/itkQuadEdgeMeshSplitFilterTest3
  ${TEST_DATA_ROOT}/SimpleSurface1.vtk 
  2  #seeds direction
  ${TEMP}/Split5_0.vtk
  ${TEMP}/Split5_1.vtk
 )
 
ADD_TEST(itkQuadEdgeMeshBoundarySmoothFilterTest
  ${CXX_TEST_PATH}/itkQuadEdgeMeshBoundarySmoothFilterTest
  ${TEMP}/Split1_0.vtk
  ${TEMP}/Split1_1.vtk
  ${TEMP}/SmoothedSplit1_0.vtk
  ${TEMP}/SmoothedSplit1_1.vtk
  5 #iterations
 )
SET_TESTS_PROPERTIES( itkQuadEdgeMeshBoundarySmoothFilterTest PROPERTIES DEPENDS itkQuadEdgeMeshSplitFilterTest1) 

ADD_TEST(itkQuadEdgeMeshToSphereFilterTest1
  ${CXX_TEST_PATH}/itkQuadEdgeMeshToSphereFilterTest1
  ${TEST_DATA_ROOT}/SimpleSurface1.vtk 
  ${TEMP}/Sphere1.vtk
 )
 
ADD_TEST(itkQuadEdgeMeshToSphereFilterTest2
  ${CXX_TEST_PATH}/itkQuadEdgeMeshToSphereFilterTest2
  ${TEST_DATA_ROOT}/SimpleSurface1.vtk 
  ${TEMP}/Sphere2.vtk
 )
    
# ADD ONE SMALL MESH AS DATA...
# ADD_TEST(itkQuadEdgeMeshSplitFilterTest
#   ${CXX_TEST_PATH}/itkQuadEdgeMeshSplitFilterTest ...)
#ADD_TEST(itkQuadEdgeMeshToSphereFilterTest
#   ${CXX_TEST_PATH}/itkQuadEdgeMeshToSphereFilterTest ..)


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
