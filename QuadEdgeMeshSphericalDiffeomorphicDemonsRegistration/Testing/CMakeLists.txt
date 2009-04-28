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

ADD_EXECUTABLE(itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilterTest1 itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilterTest1.cxx )
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilterTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkVersorOperationsTest1 itkVersorOperationsTest1.cxx )
TARGET_LINK_LIBRARIES(itkVersorOperationsTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkNodeScalarGradientCalculatorTest1 itkNodeScalarGradientCalculatorTest1.cxx )
TARGET_LINK_LIBRARIES(itkNodeScalarGradientCalculatorTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkNodeScalarGradientCalculatorTest2 itkNodeScalarGradientCalculatorTest2.cxx )
TARGET_LINK_LIBRARIES(itkNodeScalarGradientCalculatorTest2 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkNodeVectorJacobianCalculatorTest1 itkNodeVectorJacobianCalculatorTest1.cxx )
TARGET_LINK_LIBRARIES(itkNodeVectorJacobianCalculatorTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkNodeVectorJacobianCalculatorTest2 itkNodeVectorJacobianCalculatorTest2.cxx )
TARGET_LINK_LIBRARIES(itkNodeVectorJacobianCalculatorTest2 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkQuadEdgeMeshPixelValuesSmoothingFilterTest1 itkQuadEdgeMeshPixelValuesSmoothingFilterTest1.cxx)
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshPixelValuesSmoothingFilterTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkTriangleBasisSystemTest1 
  ${QuadEdgeMeshRigidRegistration}/Testing/itkTriangleBasisSystemTest1.cxx )
TARGET_LINK_LIBRARIES(itkTriangleBasisSystemTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkTriangleListBasisSystemCalculatorTest1 
  ${QuadEdgeMeshRigidRegistration}/Testing/itkTriangleListBasisSystemCalculatorTest1.cxx )
TARGET_LINK_LIBRARIES(itkTriangleListBasisSystemCalculatorTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkTriangleBasisSystemCalculatorTest1 
  ${QuadEdgeMeshRigidRegistration}/Testing/itkTriangleBasisSystemCalculatorTest1.cxx )
TARGET_LINK_LIBRARIES(itkTriangleBasisSystemCalculatorTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkLinearInterpolateMeshFunctionTest2 
  ${QuadEdgeMeshRigidRegistration}/Testing/itkLinearInterpolateMeshFunctionTest2.cxx )
TARGET_LINK_LIBRARIES(itkLinearInterpolateMeshFunctionTest2 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkLinearInterpolateMeshFunctionTest1 
  ${QuadEdgeMeshRigidRegistration}/Testing/itkLinearInterpolateMeshFunctionTest1.cxx )
TARGET_LINK_LIBRARIES(itkLinearInterpolateMeshFunctionTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkFreeSurferBinarySurfaceReaderTest1 
  ${QuadEdgeMeshRigidRegistration}/Testing/itkFreeSurferBinarySurfaceReaderTest1.cxx )
TARGET_LINK_LIBRARIES(itkFreeSurferBinarySurfaceReaderTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkFreeSurferBinarySurfaceReaderTest2 
  ${QuadEdgeMeshRigidRegistration}/Testing/itkFreeSurferBinarySurfaceReaderTest2.cxx )
TARGET_LINK_LIBRARIES(itkFreeSurferBinarySurfaceReaderTest2 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkPointLocator2Test1 
  ${QuadEdgeMeshRigidRegistration}/Testing/itkPointLocator2Test1.cxx )
TARGET_LINK_LIBRARIES(itkPointLocator2Test1 ITKQuadEdgeMesh ITKIO ITKNumerics)



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

ADD_TEST(itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilterTest1
  ${CXX_TEST_PATH}/itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilterTest1
  ${TEMP}/fixedMesh01.vtk
  ${TEMP}/movingMesh01.vtk
  ${TEMP}/movingMesh01RegisteredToFixedMesh01.vtk
  )

ADD_TEST(itkVersorOperationsTest1
  ${CXX_TEST_PATH}/itkVersorOperationsTest1
  )

ADD_TEST(itkNodeScalarGradientCalculatorTest1
  ${CXX_TEST_PATH}/itkNodeScalarGradientCalculatorTest1
  ${TEMP}/fixedMesh01.vtk
  ${TEMP}/movingMesh01.vtk
  )

ADD_TEST(itkNodeScalarGradientCalculatorTest2
  ${CXX_TEST_PATH}/itkNodeScalarGradientCalculatorTest2
  )

ADD_TEST(itkNodeVectorJacobianCalculatorTest1
  ${CXX_TEST_PATH}/itkNodeVectorJacobianCalculatorTest1
  )

ADD_TEST(itkNodeVectorJacobianCalculatorTest2
  ${CXX_TEST_PATH}/itkNodeVectorJacobianCalculatorTest2
  )

ADD_TEST(itkQuadEdgeMeshPixelValuesSmoothingFilterTest1
  ${CXX_TEST_PATH}/itkQuadEdgeMeshPixelValuesSmoothingFilterTest1
  ${TEMP}/fixedMesh01.vtk
  ${TEMP}/fixedMesh01Smoothed.vtk
  )

ADD_TEST(itkTriangleBasisSystemTest1
  ${CXX_TEST_PATH}/itkTriangleBasisSystemTest1
 )

ADD_TEST(itkTriangleListBasisSystemCalculatorTest1
  ${CXX_TEST_PATH}/itkTriangleListBasisSystemCalculatorTest1
 )

ADD_TEST(itkTriangleBasisSystemCalculatorTest1
  ${CXX_TEST_PATH}/itkTriangleBasisSystemCalculatorTest1
 )

ADD_TEST(itkPointLocator2Test1
  ${CXX_TEST_PATH}/itkPointLocator2Test1
  ${TEMP}/itkFreeSurferBinarySurfaceReaderTest2.vtk
  )



IF( FREESURFER_DIR )

ADD_TEST(itkFreeSurferBinarySurfaceReaderTest1
  ${CXX_TEST_PATH}/itkFreeSurferBinarySurfaceReaderTest1
  ${FREESURFER_DIR}/subjects/bert/surf/lh.pial
  ${FREESURFER_DIR}/subjects/bert/surf/lh.curv.pial
  ${TEMP}/itkFreeSurferBinarySurfaceReaderTest1.vtk
  )

ADD_TEST(itkFreeSurferBinarySurfaceReaderTest2
  ${CXX_TEST_PATH}/itkFreeSurferBinarySurfaceReaderTest2
  ${FREESURFER_DIR}/subjects/bert/surf/lh.pial
  ${FREESURFER_DIR}/subjects/bert/surf/lh.curv.pial
  ${TEMP}/itkFreeSurferBinarySurfaceReaderTest2.vtk
  )

ADD_TEST(itkFreeSurferBinarySurfaceReaderTest3
  ${CXX_TEST_PATH}/itkFreeSurferBinarySurfaceReaderTest1
  ${FREESURFER_DIR}/subjects/bert/surf/rh.pial
  ${FREESURFER_DIR}/subjects/bert/surf/rh.curv.pial
  ${TEMP}/itkFreeSurferBinarySurfaceReaderTest3.vtk
  )

ADD_TEST(itkFreeSurferBinarySurfaceReaderTest4
  ${CXX_TEST_PATH}/itkFreeSurferBinarySurfaceReaderTest2
  ${FREESURFER_DIR}/subjects/bert/surf/rh.pial
  ${FREESURFER_DIR}/subjects/bert/surf/rh.curv.pial
  ${TEMP}/itkFreeSurferBinarySurfaceReaderTest4.vtk
  )

ADD_TEST(itkLinearInterpolateMeshFunctionTest2
  ${CXX_TEST_PATH}/itkLinearInterpolateMeshFunctionTest2
  ${TEMP}
  )

ADD_TEST(itkLinearInterpolateMeshFunctionTest1
  ${CXX_TEST_PATH}/itkLinearInterpolateMeshFunctionTest1
  ${FREESURFER_DIR}/subjects/bert/surf/lh.pial
  ${FREESURFER_DIR}/subjects/bert/surf/lh.curv.pial
  )

ENDIF( FREESURFER_DIR )

