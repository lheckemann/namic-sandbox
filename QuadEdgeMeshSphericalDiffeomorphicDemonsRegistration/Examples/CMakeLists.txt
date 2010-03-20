#
# Add here command lines to build examples.
#
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR}/Data)
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)
SET(QuadEdgeMeshRigidRegistration ${PROJECT_SOURCE_DIR}/../QuadEdgeMeshRigidRegistration)
SET(QuadEdgeMeshFieldSmoothingFilters ${PROJECT_SOURCE_DIR}/../QuadEdgeMeshFieldSmoothingFilters)

INCLUDE_DIRECTORIES( 
  ${PROJECT_SOURCE_DIR}/Testing
  ${QuadEdgeMeshRigidRegistration}/Source/
  ${QuadEdgeMeshFieldSmoothingFilters}/Source/
  )


ADD_EXECUTABLE(QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
 QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1.cxx
 ${QuadEdgeMeshRigidRegistration}/Testing/RegistrationMonitor.cxx)
TARGET_LINK_LIBRARIES(QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
 ITKQuadEdgeMesh ITKNumerics ITKIO
 vtkRendering vtkFiltering vtkIO vtkHybrid)

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

ADD_EXECUTABLE(ResampleMeshUsingRegularSphere ResampleMeshUsingRegularSphere.cxx)
TARGET_LINK_LIBRARIES(ResampleMeshUsingRegularSphere
 ITKQuadEdgeMesh ITKNumerics ITKIO) 

IF(USE_VTK)
  ADD_EXECUTABLE(RigidMeshRegistration
    ${QuadEdgeMeshRigidRegistration}/Examples/MeanSquaresMeshToMeshMetricRegistrationWithMonitor1.cxx
    ${QuadEdgeMeshRigidRegistration}/Testing/AffineRegistrationMonitor.cxx
    ${QuadEdgeMeshRigidRegistration}/Testing/RegistrationMonitor.cxx)
  TARGET_LINK_LIBRARIES(RigidMeshRegistration ITKQuadEdgeMesh ITKIO ITKNumerics
    vtkRendering vtkFiltering vtkIO vtkHybrid)
ELSE(USE_VTK)
  ADD_EXECUTABLE(RigidMeshRegistration
    ${QuadEdgeMeshRigidRegistration}/Examples/MeanSquaresMeshToMeshMetricRegistration1.cxx)
  TARGET_LINK_LIBRARIES(RigidMeshRegistration ITKQuadEdgeMesh ITKIO ITKNumerics)
ENDIF(USE_VTK)

IF(USE_VTK)
  ADD_EXECUTABLE(MultiResolutionRegistration1 MultiResolutionRegistration1.cxx
    ${QuadEdgeMeshRigidRegistration}/Testing/AffineRegistrationMonitor.cxx
    ${QuadEdgeMeshRigidRegistration}/Testing/RegistrationMonitor.cxx)
  TARGET_LINK_LIBRARIES(MultiResolutionRegistration1 ITKQuadEdgeMesh ITKIO ITKNumerics
    vtkRendering vtkFiltering vtkIO vtkHybrid)
ELSE(USE_VTK)
  ADD_EXECUTABLE(MultiResolutionRegistration1 MultiResolutionRegistration1.cxx)
  TARGET_LINK_LIBRARIES(MultiResolutionRegistration1 ITKQuadEdgeMesh ITKIO ITKNumerics)
ENDIF(USE_VTK)

IF(USE_VTK)
  ADD_EXECUTABLE(RigidAndDemonsRegistration RigidAndDemonsRegistration.cxx
    ${QuadEdgeMeshRigidRegistration}/Testing/AffineRegistrationMonitor.cxx
    ${QuadEdgeMeshRigidRegistration}/Testing/RegistrationMonitor.cxx)
  TARGET_LINK_LIBRARIES(RigidAndDemonsRegistration ITKQuadEdgeMesh ITKIO ITKNumerics
    vtkRendering vtkFiltering vtkIO vtkHybrid)
ELSE(USE_VTK)
  ADD_EXECUTABLE(RigidAndDemonsRegistration RigidAndDemonsRegistration.cxx)
  TARGET_LINK_LIBRARIES(RigidAndDemonsRegistration ITKQuadEdgeMesh ITKIO ITKNumerics)
ENDIF(USE_VTK)

ADD_EXECUTABLE(QuadEdgeMeshSphericalRMSDistanceMetric
 QuadEdgeMeshSphericalRMSDistanceMetric.cxx) 
TARGET_LINK_LIBRARIES(QuadEdgeMeshSphericalRMSDistanceMetric
 ITKQuadEdgeMesh ITKNumerics ITKIO) 



MACRO( GENERATE_SPHERE RADIUS SUBDIVISIONS )

ADD_TEST(GenerateIC${SUBDIVISIONS}
  ${CXX_TEST_PATH}/GenerateRegularSphere
  ${TEMP}/IC${SUBDIVISIONS}.vtk
  ${RADIUS}         # Sphere radius
  ${SUBDIVISIONS}   # Number of subdivisions
  )

ENDMACRO( GENERATE_SPHERE )


GENERATE_SPHERE( 100.0  0 )
GENERATE_SPHERE( 100.0  1 )
GENERATE_SPHERE( 100.0  2 )
GENERATE_SPHERE( 100.0  3 )
GENERATE_SPHERE( 100.0  4 )
GENERATE_SPHERE( 100.0  5 )
GENERATE_SPHERE( 100.0  6 )
GENERATE_SPHERE( 100.0  7 )



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


MACRO( RESAMPLE_MESH INPUT_MESH_ID SUBDIVISIONS)


  ADD_TEST(Resample${INPUT_MESH_ID}_IC${SUBDIVISIONS}
    ${CXX_TEST_PATH}/ResampleMeshUsingRegularSphere
    ${TEMP}/IC${SUBDIVISIONS}.vtk
    ${TEMP}/sub${INPUT_MESH_ID}.vtk
    ${TEMP}/sub${INPUT_MESH_ID}_IC${SUBDIVISIONS}.vtk
    )

ENDMACRO( RESAMPLE_MESH )

MACRO( RESAMPLE_MESH_ALL_LEVELS INPUT_MESH_ID )

  RESAMPLE_MESH( ${INPUT_MESH_ID}a  4 )  #  ic4: with inflated.H
  RESAMPLE_MESH( ${INPUT_MESH_ID}b  5 )  #  ic5: with sulc
  RESAMPLE_MESH( ${INPUT_MESH_ID}b  6 )  #  ic6: with sulc
  RESAMPLE_MESH( ${INPUT_MESH_ID}c  7 )  #  ic7: with curv 

ENDMACRO( RESAMPLE_MESH_ALL_LEVELS )

MACRO( RIGIDLY_REGISTER MESHID1 MESHID2 SUBDIVISIONS )

SET(FIXEDMESH   ${TEMP}/sub${MESHID1}_IC${SUBDIVISIONS}.vtk)
SET(MOVINGMESH  ${TEMP}/sub${MESHID2}_IC${SUBDIVISIONS}.vtk)

ADD_TEST(RigidRegister_${MESHID1}_${MESHID2}_IC${SUBDIVISIONS} 
  ${CXX_TEST_PATH}/RigidMeshRegistration
  ${FIXEDMESH}
  ${MOVINGMESH}
  ${TEMP}/sub${MESHID2}_rigid_to_${MESHID1}_IC${SUBDIVISIONS}.vtk
  0 0 1  # Default initial axis
  0      # Default initial angle
  )

ENDMACRO( RIGIDLY_REGISTER )

MACRO( MULTIRES_REGISTER MESHID1 MESHID2 )

SET(FIXEDMESH1   ${TEMP}/sub${MESHID1}a_IC4.vtk)
SET(MOVINGMESH1  ${TEMP}/sub${MESHID2}a_IC4.vtk)

SET(FIXEDMESH2   ${TEMP}/sub${MESHID1}b_IC5.vtk)
SET(MOVINGMESH2  ${TEMP}/sub${MESHID2}b_IC5.vtk)

SET(FIXEDMESH3   ${TEMP}/sub${MESHID1}b_IC6.vtk)
SET(MOVINGMESH3  ${TEMP}/sub${MESHID2}b_IC6.vtk)

SET(FIXEDMESH4   ${TEMP}/sub${MESHID1}c_IC7.vtk)
SET(MOVINGMESH4  ${TEMP}/sub${MESHID2}c_IC7.vtk)

SET(FIXEDMESHFINAL  ${TEMP}/sub${MESHID1}c.vtk)
SET(MOVINGMESHFINAL ${TEMP}/sub${MESHID2}c.vtk)

ADD_TEST(MultiResRegistration_${MESHID1}_${MESHID2}
  ${CXX_TEST_PATH}/MultiResolutionRegistration1
  ${FIXEDMESH1}
  ${MOVINGMESH1}
  ${TEMP}/sub${MESHID2}_multiresolution_to_${MESHID1}_IC4.vtk
  ${FIXEDMESH2}
  ${MOVINGMESH2}
  ${TEMP}/sub${MESHID2}_multiresolution_to_${MESHID1}_IC5.vtk
  ${FIXEDMESH3}
  ${MOVINGMESH3}
  ${TEMP}/sub${MESHID2}_multiresolution_to_${MESHID1}_IC6.vtk
  ${FIXEDMESH4}
  ${MOVINGMESH4}
  ${TEMP}/sub${MESHID2}_multiresolution_to_${MESHID1}_IC7.vtk
  ${FIXEDMESHFINAL}
  ${MOVINGMESHFINAL}
  ${TEMP}/sub${MESHID2}_multiresolution_to_${MESHID1}_Final.vtk
  registrationLog_${MESHID2}_${MESHID1}_
  )

ENDMACRO( MULTIRES_REGISTER )


MACRO( RIGID_DEMONS_REGISTER MESHID1 MESHID2 RADIUS SIGMAX EPSILON NUMBEROFITERATIONS)

SET(FIXEDMESH1   ${TEMP}/sub${MESHID1}a_IC4.vtk)
SET(MOVINGMESH1  ${TEMP}/sub${MESHID2}a_IC4.vtk)

ADD_TEST(RigidDemonsRegistration_${MESHID1}_${MESHID2}
  ${CXX_TEST_PATH}/RigidAndDemonsRegistration
  ${FIXEDMESH1}
  ${MOVINGMESH1}
  ${RADIUS}
  ${SIGMAX}
  ${EPSILON}
  ${NUMBEROFITERATIONS}
  ${TEMP}/outputResampledRigid.vtk
  ${TEMP}/outputResampledDemons.vtk
  ${TEMP}/outputDeformedFixedMesh.vtk
  ${TEMP}/DeformationField.vtk 
  )

ENDMACRO()





MACRO( DEMONS_REGISTER MESHID1 MESHID2 SUBDIVISIONS )

SET(FIXEDMESH   ${TEMP}/sub${MESHID1}_IC${SUBDIVISIONS}.vtk)
SET(MOVINGMESH  ${TEMP}/sub${MESHID2}_rigid_to_${MESHID1}_IC${SUBDIVISIONS}.vtk)
SET(RESAMPLEDMOVINGMESH  ${TEMP}/sub${MESHID2}_demons_resampled_to_${MESHID1}_IC${SUBDIVISIONS}.vtk)
SET(DEFORMEDFIXEDMESH  ${TEMP}/sub${MESHID1}_demons_deformed_to_${MESHID2}_IC${SUBDIVISIONS}.vtk)

ADD_TEST(DemonsRegister_${MESHID1}_${MESHID2}_IC${SUBDIVISIONS} 
  ${CXX_TEST_PATH}/QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
  ${FIXEDMESH}
  ${MOVINGMESH}
  ${RESAMPLEDMOVINGMESH}
  ${DEFORMEDFIXEDMESH}
  100.0  # Sphere radius
  1.0    # Epsilon
  1.0    # SigmaX
  1.0    # Lambda
  10     # Smoothing Iterations
  30     # maximum number of iterations
  )

ENDMACRO( DEMONS_REGISTER )


MACRO( COMPARE_MESHES MESHID1 MESHID2 )

SET(REFERENCE_MESH_BASE_NAME
  lh_MatlabSphericalDemons_sub${MESHID1}Tosub${MESHID2})

SET(REFERENCE_MESH 
  ${PATIENT_RESULTS_DATA_ROOT}/${REFERENCE_MESH_BASE_NAME}.reg)

ADD_TEST(ConvertResult_${MESHID1}_${MESHID2}
  ${CXX_TEST_PATH}/ReadFreesurferBinarySurfaceWriteVTKMesh
  ${REFERENCE_MESH}
  ${PATIENT_DATA_ROOT}/sub${MESHID2}/surf/lh.curv
  ${TEMP}/${REFERENCE_MESH_BASE_NAME}.vtk
  )


ADD_TEST(CompareRegistration_${MESHID1}_${MESHID2}
  ${CXX_TEST_PATH}/QuadEdgeMeshSphericalRMSDistanceMetric
  ${TEMP}/sub${MESHID1}_multiresolution_to_${MESHID2}_Final.vtk
  ${TEMP}/${REFERENCE_MESH_BASE_NAME}.vtk
  ${TEMP}/sub${MESHID1}_multiresolution_to_${MESHID2}_Final_Difference.vtk
  )

ENDMACRO( COMPARE_MESHES )


ADD_TEST(DemonsExample1
  ${CXX_TEST_PATH}/QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
  ${TEST_DATA_ROOT}/Input/fixedMeshWithScalarsIC1.vtk
  ${TEST_DATA_ROOT}/Input/movingMeshWithScalarsIC1.vtk
  ${TEMP}/MeshDemonsFilter1_ResampledMovingMesh_IC1.vtk
  ${TEMP}/MeshDemonsFilter1_DeformedFixedMesh_IC1.vtk
  100.0  # Sphere radius
  1.0    # Epsilon
  1.0    # SigmaX
  1.0    # Lambda
   2     # Smoothing Iterations
  30     # maximum number of iterations
  )

ADD_TEST(DemonsExampleField1
  ${CXX_TEST_PATH}/itkQuadEdgeMeshGenerateDeformationFieldFilterTest1
  ${TEST_DATA_ROOT}/Input/fixedMeshWithScalarsIC1.vtk
  ${TEMP}/MeshDemonsFilter1_DeformedFixedMesh_IC1.vtk
  ${TEMP}/DemonsExampleField1DeformationFieldIC1.vtk
  )
SET_TESTS_PROPERTIES( DemonsExampleField1 PROPERTIES DEPENDS DemonsExample1)


ADD_TEST(DemonsExample2
  ${CXX_TEST_PATH}/QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
  ${TEST_DATA_ROOT}/Input/fixedMeshWithScalarsIC2.vtk
  ${TEST_DATA_ROOT}/Input/movingMeshWithScalarsIC2.vtk
  ${TEMP}/MeshDemonsFilter1_ResampledMovingMesh_IC2.vtk
  ${TEMP}/MeshDemonsFilter1_DeformedFixedMesh_IC2.vtk
  100.0  # Sphere radius
  1.0    # Epsilon
  1.0    # SigmaX
  1.0    # Lambda
   2     # Smoothing Iterations
  30     # maximum number of iterations
  )

ADD_TEST(DemonsExample3
  ${CXX_TEST_PATH}/QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
  ${TEST_DATA_ROOT}/Input/fixedMeshWithScalarsIC3.vtk
  ${TEST_DATA_ROOT}/Input/movingMeshWithScalarsIC3.vtk
  ${TEMP}/MeshDemonsFilter1_ResampledMovingMesh_IC3.vtk
  ${TEMP}/MeshDemonsFilter1_DeformedFixedMesh_IC3.vtk
  100.0  # Sphere radius
  1.0    # Epsilon
  1.0    # SigmaX
  1.0    # Lambda
   2     # Smoothing Iterations
  30     # maximum number of iterations
  )


ADD_TEST(DemonsExample4
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

ADD_TEST(DemonsExample5
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

ADD_TEST(DemonsExample6
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


IF( PATIENT_DATA_ROOT )

IF( USE_TESTING_SET )
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

  RESAMPLE_MESH_ALL_LEVELS( 01 )
  RESAMPLE_MESH_ALL_LEVELS( 02 )
  RESAMPLE_MESH_ALL_LEVELS( 03 )
  RESAMPLE_MESH_ALL_LEVELS( 04 )
  RESAMPLE_MESH_ALL_LEVELS( 05 )
  RESAMPLE_MESH_ALL_LEVELS( 06 )
  RESAMPLE_MESH_ALL_LEVELS( 07 )
  RESAMPLE_MESH_ALL_LEVELS( 08 )
  RESAMPLE_MESH_ALL_LEVELS( 09 )
  RESAMPLE_MESH_ALL_LEVELS( 10 )

  RIGIDLY_REGISTER( 01a  02a  4 )
  RIGIDLY_REGISTER( 03a  04a  4 )
  RIGIDLY_REGISTER( 05a  06a  4 )
  RIGIDLY_REGISTER( 07a  08a  4 )
  RIGIDLY_REGISTER( 09a  10a  4 )

  DEMONS_REGISTER( 01a  02a  4 )
  DEMONS_REGISTER( 03a  04a  4 )
  DEMONS_REGISTER( 05a  06a  4 )
  DEMONS_REGISTER( 07a  08a  4 )
  DEMONS_REGISTER( 09a  10a  4 )

  MULTIRES_REGISTER( 01 02 )
  MULTIRES_REGISTER( 03 04 )
  MULTIRES_REGISTER( 05 06 )
  MULTIRES_REGISTER( 07 08 )
  MULTIRES_REGISTER( 09 10 )
ENDIF( USE_TESTING_SET )

IF( USE_TRAINING_SET )
  CONVERT_ANON_DATA(11)
  CONVERT_ANON_DATA(12)
  CONVERT_ANON_DATA(13)
  CONVERT_ANON_DATA(14)
  CONVERT_ANON_DATA(15)
  CONVERT_ANON_DATA(16)
  CONVERT_ANON_DATA(17)
  CONVERT_ANON_DATA(18)
  CONVERT_ANON_DATA(19)
  CONVERT_ANON_DATA(20)

  RESAMPLE_MESH_ALL_LEVELS( 11 )
  RESAMPLE_MESH_ALL_LEVELS( 12 )
  RESAMPLE_MESH_ALL_LEVELS( 13 )
  RESAMPLE_MESH_ALL_LEVELS( 14 )
  RESAMPLE_MESH_ALL_LEVELS( 15 )
  RESAMPLE_MESH_ALL_LEVELS( 16 )
  RESAMPLE_MESH_ALL_LEVELS( 17 )
  RESAMPLE_MESH_ALL_LEVELS( 18 )
  RESAMPLE_MESH_ALL_LEVELS( 19 )
  RESAMPLE_MESH_ALL_LEVELS( 20 )

  RIGIDLY_REGISTER( 12a  11a  4 )
  RIGIDLY_REGISTER( 14a  13a  4 )
  RIGIDLY_REGISTER( 16a  15a  4 )
  RIGIDLY_REGISTER( 18a  17a  4 )
  RIGIDLY_REGISTER( 20a  19a  4 )

  DEMONS_REGISTER( 12a  11a  4 )
  DEMONS_REGISTER( 14a  13a  4 )
  DEMONS_REGISTER( 16a  15a  4 )
  DEMONS_REGISTER( 18a  17a  4 )
  DEMONS_REGISTER( 20a  19a  4 )

  MULTIRES_REGISTER( 12 11 )
  MULTIRES_REGISTER( 14 13 )
  MULTIRES_REGISTER( 16 15 )
  MULTIRES_REGISTER( 18 17 )
  MULTIRES_REGISTER( 20 19 )

  COMPARE_MESHES( 11 12 )
  COMPARE_MESHES( 13 14 )
  COMPARE_MESHES( 15 16 )
  COMPARE_MESHES( 17 18 )
  COMPARE_MESHES( 19 20 )

  RIGID_DEMONS_REGISTER( 12 11   100.0  10.0 0.1 1000 )

ENDIF( USE_TRAINING_SET )


ENDIF( PATIENT_DATA_ROOT )

