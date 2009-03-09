SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR}/Data)
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)

FIND_PATH(FREESURFER_DIR
  SetUpFreeSurfer.sh
  PATHS
  $ENV{FREESURFER_HOME}
  DOC "Directory where FreeSurfer is installed"
  )

#######################
#######################
#
#  Define Executables
#
#######################
#######################

ADD_EXECUTABLE(itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilterTest1 itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilterTest1.cxx )
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilterTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)



###################
###################
#
#  Define Tests
#
###################
###################

ADD_TEST(itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilterTest1
  ${CXX_TEST_PATH}/itkQuadEdgeMeshSphericalDiffeomorphicDemonsFilterTest1
  ${TEST_DATA_ROOT}/Input/mesh01.vtk
  ${TEST_DATA_ROOT}/Input/mesh02.vtk
  ${TEMP}/Input/mesh02registeredtToMesh01.vtk
 )


IF( FREESURFER_DIR )


ENDIF( FREESURFER_DIR )

