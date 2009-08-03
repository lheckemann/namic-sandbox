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

ADD_TEST(DemonsFilterExample1
  ${CXX_TEST_PATH}/QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
  ${TEST_DATA_ROOT}/Input/fixedMeshWithScalarsIC1.vtk
  ${TEST_DATA_ROOT}/Input/movingMeshWithScalarsIC1.vtk
  ${TEMP}/QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1.vtk
  1.0    # Gamma
  1.0    # SigmaX
  1.0    # Lambda
  1      # Smoothing Iterations
  3000   # maximum number of iterations
  )


