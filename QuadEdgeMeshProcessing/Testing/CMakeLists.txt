
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${QuadEdgeMeshProcessing_SOURCE_DIR})
SET(TEMP ${QuadEdgeMeshProcessing_BINARY_DIR}/Testing/Temporary)

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

ADD_EXECUTABLE(itkQuadEdgeMeshTest1 itkQuadEdgeMeshTest1.cxx )
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkRegularSphereQuadEdgeMeshSourceTest1 itkRegularSphereQuadEdgeMeshSourceTest1.cxx )
TARGET_LINK_LIBRARIES(itkRegularSphereQuadEdgeMeshSourceTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkFreeSurferBinarySurfaceReaderTest1 itkFreeSurferBinarySurfaceReaderTest1.cxx )
TARGET_LINK_LIBRARIES(itkFreeSurferBinarySurfaceReaderTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkFreeSurferBinarySurfaceReaderTest2 itkFreeSurferBinarySurfaceReaderTest2.cxx )
TARGET_LINK_LIBRARIES(itkFreeSurferBinarySurfaceReaderTest2 ITKQuadEdgeMesh ITKIO ITKNumerics)



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

ADD_TEST(itkRegularSphereQuadEdgeMeshSourceTest1
  ${CXX_TEST_PATH}/itkRegularSphereQuadEdgeMeshSourceTest1
  ${TEMP}/itkRegularSphereQuadEdgeMeshSourceTest1.vtk
 )

IF( FREESURFER_DIR )

ADD_TEST(itkFreeSurferBinarySurfaceReaderTest1
  ${CXX_TEST_PATH}/itkFreeSurferBinarySurfaceReaderTest1
  ${FREESURFER_DIR}/subjects/bert/surf/lh.pial
  ${TEMP}/itkFreeSurferBinarySurfaceReaderTest1.vtk
  )

ADD_TEST(itkFreeSurferBinarySurfaceReaderTest2
  ${CXX_TEST_PATH}/itkFreeSurferBinarySurfaceReaderTest2
  ${FREESURFER_DIR}/subjects/bert/surf/lh.pial
  ${TEMP}/itkFreeSurferBinarySurfaceReaderTest2.vtk
  )

ENDIF( FREESURFER_DIR )

