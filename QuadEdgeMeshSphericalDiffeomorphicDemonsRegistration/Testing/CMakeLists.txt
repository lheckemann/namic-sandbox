SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR}/Data)
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)

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



IF( FREESURFER_DIR )


ENDIF( FREESURFER_DIR )

