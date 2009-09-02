#
# Add here command lines to build examples.
#
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR}/Data)
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)

INCLUDE_DIRECTORIES( 
  ${QuadEdgeMeshSphericalDiffeomorphicDemonsRegistration_SOURCE_DIR}/Testing
  ${QuadEdgeMeshSphericalDiffeomorphicDemonsRegistration_SOURCE_DIR}/../QuadEdgeMeshFieldSmoothingFilters/Source/
  )

ADD_EXECUTABLE(QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
 QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1.cxx) 
TARGET_LINK_LIBRARIES(QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
 ITKQuadEdgeMesh ITKNumerics ITKIO) 

ADD_TEST(DemonsExample1
  ${CXX_TEST_PATH}/QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
  ${TEST_DATA_ROOT}/Input/fixedMeshWithScalarsIC1.vtk
  ${TEST_DATA_ROOT}/Input/movingMeshWithScalarsIC1.vtk
  ${TEMP}/MeshDemonsFilter1.vtk
  1.0    # Epsilon
  1.0    # SigmaX
  1.0    # Lambda
  10     # Smoothing Iterations
  30     # maximum number of iterations
  )

ADD_TEST(DemonsExample2
  ${CXX_TEST_PATH}/QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
  ${TEMP}/itkSimpleSurface1.vtk
  ${TEMP}/itkSimpleSurface2.vtk
  ${TEMP}/MeshDemonsFilterSimpleSurface2to1.vtk
  1.0    # Epsilon
  1.0    # SigmaX
  1.0    # Lambda
  10     # Smoothing Iterations
  30     # maximum number of iterations
  )

