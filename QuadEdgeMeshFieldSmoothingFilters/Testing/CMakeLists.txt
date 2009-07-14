SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR}/Data)
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)

SET(QuadEdgeMeshRigidRegistration ${PROJECT_SOURCE_DIR}/../QuadEdgeMeshRigidRegistration)

FIND_PATH(FREESURFER_DIR
  SetUpFreeSurfer.sh
  PATHS
  $ENV{FREESURFER_HOME}
  DOC "Directory where FreeSurfer is installed"
  )

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/Testing
  )

#######################
#######################
#
#  Define Executables
#
#######################
#######################

ADD_EXECUTABLE(itkMeshGeneratorHelperTest1 itkMeshGeneratorHelperTest1.cxx )
TARGET_LINK_LIBRARIES(itkMeshGeneratorHelperTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkMeshGeneratorHelperTest2 itkMeshGeneratorHelperTest2.cxx )
TARGET_LINK_LIBRARIES(itkMeshGeneratorHelperTest2 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkMeshGeneratorHelperTest3 itkMeshGeneratorHelperTest3.cxx )
TARGET_LINK_LIBRARIES(itkMeshGeneratorHelperTest3 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkMeshGeneratorHelperTest4 itkMeshGeneratorHelperTest4.cxx )
TARGET_LINK_LIBRARIES(itkMeshGeneratorHelperTest4 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkQuadEdgeMeshVTKPolyDataReader1 itkQuadEdgeMeshVTKPolyDataReader1.cxx )
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshVTKPolyDataReader1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkQuadEdgeMeshVTKPolyDataReader2 itkQuadEdgeMeshVTKPolyDataReader2.cxx )
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshVTKPolyDataReader2 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkQuadEdgeMeshVectorPixelValuesSmoothingFilterTest1 itkQuadEdgeMeshVectorPixelValuesSmoothingFilterTest1.cxx)
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshVectorPixelValuesSmoothingFilterTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkQuadEdgeMeshScalarPixelValuesSmoothingFilterTest1 itkQuadEdgeMeshScalarPixelValuesSmoothingFilterTest1.cxx)
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshScalarPixelValuesSmoothingFilterTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

###################
###################
#
#  Define Tests
#
###################
###################

ADD_TEST(itkMeshGeneratorHelperTest1
  ${CXX_TEST_PATH}/itkMeshGeneratorHelperTest1
  ${TEMP}/fixedMesh01.vtk
  ${TEMP}/movingMesh01.vtk
  )

ADD_TEST(itkMeshGeneratorHelperTest2
  ${CXX_TEST_PATH}/itkMeshGeneratorHelperTest2
  ${TEMP}/meshWithVectors.vtk
  )

ADD_TEST(itkMeshGeneratorHelperTest3a
  ${CXX_TEST_PATH}/itkMeshGeneratorHelperTest3
  ${TEMP}/meshWithVectorsIC1.vtk
  1      # resolution
  100.0  # Scale (radius)
  )

ADD_TEST(itkMeshGeneratorHelperTest3b
  ${CXX_TEST_PATH}/itkMeshGeneratorHelperTest3
  ${TEMP}/meshWithVectorsIC4.vtk
  4      # resolution
  100.0  # Scale (radius)
  )

ADD_TEST(itkMeshGeneratorHelperTest3c
  ${CXX_TEST_PATH}/itkMeshGeneratorHelperTest3
  ${TEMP}/meshWithVectorsIC7.vtk
  7      # resolution
  100.0  # Scale (radius)
  )

ADD_TEST(itkMeshGeneratorHelperTest4a
  ${CXX_TEST_PATH}/itkMeshGeneratorHelperTest4
  ${TEMP}/meshWithScalarsIC1.vtk
  1      # resolution
  100.0  # Scale (radius)
  )

ADD_TEST(itkMeshGeneratorHelperTest4b
  ${CXX_TEST_PATH}/itkMeshGeneratorHelperTest4
  ${TEMP}/meshWithScalarsIC4.vtk
  4      # resolution
  100.0  # Scale (radius)
  )

ADD_TEST(itkMeshGeneratorHelperTest4c
  ${CXX_TEST_PATH}/itkMeshGeneratorHelperTest4
  ${TEMP}/meshWithScalarsIC7.vtk
  7      # resolution
  100.0  # Scale (radius)
  )

ADD_TEST(itkQuadEdgeMeshVTKPolyDataReader1
  ${CXX_TEST_PATH}/itkQuadEdgeMeshVTKPolyDataReader1
  ${TEMP}/fixedMesh01.vtk
  ${TEMP}/fixedMesh01verification.vtk
  )

ADD_TEST(itkQuadEdgeMeshVTKPolyDataReader2
  ${CXX_TEST_PATH}/itkQuadEdgeMeshVTKPolyDataReader2
  ${TEMP}/meshWithVectors.vtk
  ${TEMP}/meshWithVectorsVerification.vtk
  )

ADD_TEST(ScalarSmoothingFilterTest1
  ${CXX_TEST_PATH}/itkQuadEdgeMeshScalarPixelValuesSmoothingFilterTest1
  ${TEMP}/fixedMesh01.vtk
  ${TEMP}/fixedMesh01Smoothed.vtk
  1.0   # Lambda
  10    # Iterations
  )

ADD_TEST(VectorSmoothingFilterTest1
  ${CXX_TEST_PATH}/itkQuadEdgeMeshVectorPixelValuesSmoothingFilterTest1
  ${TEMP}/meshWithVectors.vtk
  ${TEMP}/meshWithVectorsSmoothed.vtk
  1.0   # Lambda
  10    # Iterations
  )

ADD_TEST(VectorSmoothingFilterTest2
  ${CXX_TEST_PATH}/itkQuadEdgeMeshVectorPixelValuesSmoothingFilterTest1
  ${TEMP}/meshWithVectorsIC1.vtk
  ${TEMP}/meshWithVectorsIC1Smoothed.vtk
  1.0   # Lambda
  10    # Iterations
  )

ADD_TEST(VectorSmoothingFilterTest3
  ${CXX_TEST_PATH}/itkQuadEdgeMeshVectorPixelValuesSmoothingFilterTest1
  ${TEMP}/meshWithVectorsIC4.vtk
  ${TEMP}/meshWithVectorsIC4Smoothed.vtk
  1.0   # Lambda
  10    # Iterations
  )

ADD_TEST(VectorSmoothingFilterTest4
  ${CXX_TEST_PATH}/itkQuadEdgeMeshVectorPixelValuesSmoothingFilterTest1
  ${TEMP}/meshWithVectorsIC7.vtk
  ${TEMP}/meshWithVectorsIC7Smoothed.vtk
  1.0   # Lambda
  10    # Iterations
  )

