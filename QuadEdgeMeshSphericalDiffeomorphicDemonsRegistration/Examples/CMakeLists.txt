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

ADD_EXECUTABLE(QuadEdgeMeshSphericalDiffeomorphicDemonsFilter2
 QuadEdgeMeshSphericalDiffeomorphicDemonsFilter2.cxx) 
TARGET_LINK_LIBRARIES(QuadEdgeMeshSphericalDiffeomorphicDemonsFilter2
 ITKQuadEdgeMesh ITKNumerics ITKIO) 

ADD_EXECUTABLE(ReadFreesurferBinarySurfaceWriteVTKMesh
 ReadFreesurferBinarySurfaceWriteVTKMesh.cxx) 
TARGET_LINK_LIBRARIES(ReadFreesurferBinarySurfaceWriteVTKMesh
 ITKQuadEdgeMesh ITKNumerics ITKIO) 

ADD_EXECUTABLE(GenerateRegularSphere GenerateRegularSphere.cxx)
TARGET_LINK_LIBRARIES(GenerateRegularSphere
 ITKQuadEdgeMesh ITKNumerics ITKIO) 


MACRO( GENERATE_SPHERE RADIUS SUBDIVISIONS )

ADD_TEST(GenerateIC${SUBDIVISIONS}
  ${CXX_TEST_PATH}/GenerateRegularSphere
  ${TEMP}/IC${SUBDIVISIONS}.vtk
  ${RADIUS}         # Sphere radius
  ${SUBDIVISIONS}   # Number of subdivisions
  )

ENDMACRO( GENERATE_SPHERE )


GENERATE_SPHERE( 100.0  1 )
GENERATE_SPHERE( 100.0  2 )
GENERATE_SPHERE( 100.0  3 )
GENERATE_SPHERE( 100.0  4 )
GENERATE_SPHERE( 100.0  5 )
GENERATE_SPHERE( 100.0  6 )
GENERATE_SPHERE( 100.0  7 )

ADD_TEST(DemonsExample1
  ${CXX_TEST_PATH}/QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
  ${TEST_DATA_ROOT}/Input/fixedMeshWithScalarsIC1.vtk
  ${TEST_DATA_ROOT}/Input/movingMeshWithScalarsIC1.vtk
  ${TEMP}/MeshDemonsFilter1_ResampledMovingMesh.vtk
  ${TEMP}/MeshDemonsFilter1_DeformedFixedMesh.vtk
  100.0  # Sphere radius
  1.0    # Epsilon
  1.0    # SigmaX
  1.0    # Lambda
  10     # Smoothing Iterations
  30     # maximum number of iterations
  )

ADD_TEST(DemonsExample2
  ${CXX_TEST_PATH}/QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
  ${TEST_DATA_ROOT}/Input/SimpleSurface1Small.vtk
  ${TEST_DATA_ROOT}/Input/RigidRegistrationSimpleSurface2to1Small.vtk
  ${TEMP}/MeshDemonsFilterSimpleSurface2to1Small_ResampledMovingMesh.vtk
  ${TEMP}/MeshDemonsFilterSimpleSurface2to1Small_DeformedFixedMesh.vtk
  1.0    # Sphere radius
  0.5    # Epsilon
  0.5    # SigmaX
  1.0    # Lambda
  10     # Smoothing Iterations
  30     # maximum number of iterations
  )

ADD_TEST(DemonsExample3
  ${CXX_TEST_PATH}/QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
  ${TEST_DATA_ROOT}/Input/SimpleSurface1.vtk
  ${TEST_DATA_ROOT}/Input/RigidRegistrationSimpleSurface2to1.vtk
  ${TEMP}/MeshDemonsFilterSimpleSurface2to1_ResampledMovingMesh.vtk
  ${TEMP}/MeshDemonsFilterSimpleSurface2to1_DeformedFixedMesh.vtk
  100.0  # Sphere radius
  1.0    # Epsilon
  1.0    # SigmaX
  1.0    # Lambda
  10     # Smoothing Iterations
  30     # maximum number of iterations
  )

ADD_TEST(DemonsExample4
  ${CXX_TEST_PATH}/QuadEdgeMeshSphericalDiffeomorphicDemonsFilter2
  ${TEST_DATA_ROOT}/Input/SimpleSurface1Small.vtk
  ${TEST_DATA_ROOT}/Input/RigidRegistrationSimpleSurface2to1Small.vtk
  ${TEMP}/MeshDemonsFilterSimpleSurface2to1Small_ResampledMovingMesh.vtk
  ${TEMP}/MeshDemonsFilterSimpleSurface2to1Small_DeformedFixedMesh.vtk
  1.0    # Sphere radius
  0.5    # Epsilon
  0.5    # SigmaX
  1.0    # Lambda
  10     # Smoothing Iterations
  30     # maximum number of iterations
  )


IF( PATIENT_DATA_ROOT )

MACRO(CONVERT_ANON_DATA ID)

ADD_TEST(anonConvert${ID}a
  ${CXX_TEST_PATH}/ReadFreesurferBinarySurfaceWriteVTKMesh
  ${PATIENT_DATA_ROOT}/sub${ID}/surf/lh.sphere
  ${PATIENT_DATA_ROOT}/sub${ID}/surf/lh.inflated.H
  ${TEMP}/sub${ID}a.vtk
  )

ADD_TEST(anonConvert${ID}b
  ${CXX_TEST_PATH}/ReadFreesurferBinarySurfaceWriteVTKMesh
  ${PATIENT_DATA_ROOT}/sub${ID}/surf/lh.sphere
  ${PATIENT_DATA_ROOT}/sub${ID}/surf/lh.sulc
  ${TEMP}/sub${ID}b.vtk
  )
 
ADD_TEST(anonConvert${ID}c
  ${CXX_TEST_PATH}/ReadFreesurferBinarySurfaceWriteVTKMesh
  ${PATIENT_DATA_ROOT}/sub${ID}/surf/lh.sphere
  ${PATIENT_DATA_ROOT}/sub${ID}/surf/lh.curv
  ${TEMP}/sub${ID}c.vtk
  )
 
ENDMACRO(CONVERT_ANON_DATA)

CONVERT_ANON_DATA(01)
CONVERT_ANON_DATA(02)
CONVERT_ANON_DATA(03)
CONVERT_ANON_DATA(04)
CONVERT_ANON_DATA(05)
CONVERT_ANON_DATA(06)
CONVERT_ANON_DATA(07)
CONVERT_ANON_DATA(08)
CONVERT_ANON_DATA(09)
CONVERT_ANON_DATA(10)

ENDIF( PATIENT_DATA_ROOT )


#
# REGISTER( lh 01  15 )
#
# lh_DemonsRegistered_Sub01ToSub15.vtk
#
# lh_moving_to_fixed.vtk
#
# lh_MatlabSphericalDemons_Sub01ToSub15.vtk
# Sub01 is moving
# Sub15 is fixed.
#
