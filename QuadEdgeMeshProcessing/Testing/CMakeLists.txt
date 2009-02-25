
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

ADD_EXECUTABLE(itkLinearInterpolateMeshFunctionTest1 itkLinearInterpolateMeshFunctionTest1.cxx )
TARGET_LINK_LIBRARIES(itkLinearInterpolateMeshFunctionTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkNearestNeighborInterpolateMeshFunctionTest1 itkNearestNeighborInterpolateMeshFunctionTest1.cxx )
TARGET_LINK_LIBRARIES(itkNearestNeighborInterpolateMeshFunctionTest1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkConformalFlatteningQuadEdgeMeshFilterTest itkConformalFlatteningQuadEdgeMeshFilterTest.cxx )
TARGET_LINK_LIBRARIES(itkConformalFlatteningQuadEdgeMeshFilterTest ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkQuadEdgeMeshKdTreeGeneratorTest itkQuadEdgeMeshKdTreeGeneratorTest.cxx )
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshKdTreeGeneratorTest ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkPointLocator2Test1 itkPointLocator2Test1.cxx )
TARGET_LINK_LIBRARIES(itkPointLocator2Test1 ITKQuadEdgeMesh ITKIO ITKNumerics)

ADD_EXECUTABLE(itkQuadEdgeMeshSplitFilterTest itkQuadEdgeMeshSplitFilterTest.cxx)
TARGET_LINK_LIBRARIES(itkQuadEdgeMeshSplitFilterTest ITKQuadEdgeMesh ITKIO)

ADD_EXECUTABLE(itkQuadEdgeMeshToSphereFilterTest
itkQuadEdgeMeshToSphereFilterTest.cxx )
TARGET_LINK_LIBRARIES( itkQuadEdgeMeshToSphereFilterTest ITKQuadEdgeMesh ITKIO)

ADD_EXECUTABLE(itkQuadEdgeMeshSphericalParameterizationTest
itkQuadEdgeMeshSphericalParameterizationTest.cxx )
TARGET_LINK_LIBRARIES( itkQuadEdgeMeshSphericalParameterizationTest
ITKQuadEdgeMesh ITKIO)

ADD_EXECUTABLE(itkMeshToMeshRegistrationMethodTest1 itkMeshToMeshRegistrationMethodTest1.cxx)
TARGET_LINK_LIBRARIES(itkMeshToMeshRegistrationMethodTest1 ITKQuadEdgeMesh ITKIO)

ADD_EXECUTABLE(itkMeshToMeshMetricTest1 itkMeshToMeshMetricTest1.cxx)
TARGET_LINK_LIBRARIES(itkMeshToMeshMetricTest1 ITKQuadEdgeMesh ITKIO)

ADD_EXECUTABLE(itkMeanSquaresMeshToMeshMetricTest1 itkMeanSquaresMeshToMeshMetricTest1.cxx)
TARGET_LINK_LIBRARIES(itkMeanSquaresMeshToMeshMetricTest1 ITKQuadEdgeMesh ITKNumerics ITKIO)

ADD_EXECUTABLE(itkMeanSquaresMeshToMeshMetricTest2 itkMeanSquaresMeshToMeshMetricTest2.cxx)
TARGET_LINK_LIBRARIES(itkMeanSquaresMeshToMeshMetricTest2 ITKQuadEdgeMesh ITKNumerics ITKIO)

ADD_EXECUTABLE(itkAnalyticalMeshToMeshMetricTest1 itkAnalyticalMeshToMeshMetricTest1.cxx)
TARGET_LINK_LIBRARIES(itkAnalyticalMeshToMeshMetricTest1 ITKQuadEdgeMesh ITKNumerics ITKIO)

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

# ADD ONE SMALL MESH AS DATA...
# ADD_TEST(itkQuadEdgeMeshSplitFilterTest
#   ${CXX_TEST_PATH}/itkQuadEdgeMeshSplitFilterTest ...)
#ADD_TEST(itkQuadEdgeMeshToSphereFilterTest
#   ${CXX_TEST_PATH}/itkQuadEdgeMeshToSphereFilterTest ..)

ADD_TEST(itkMeanSquaresMeshToMeshMetricTest1
  ${CXX_TEST_PATH}/itkMeanSquaresMeshToMeshMetricTest1
  )

ADD_TEST(itkMeanSquaresMeshToMeshMetricTest2
  ${CXX_TEST_PATH}/itkMeanSquaresMeshToMeshMetricTest2
  )

ADD_TEST(itkAnalyticalMeshToMeshMetricTest1
  ${CXX_TEST_PATH}/itkAnalyticalMeshToMeshMetricTest1
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

ADD_TEST(itkLinearInterpolateMeshFunctionTest1
  ${CXX_TEST_PATH}/itkLinearInterpolateMeshFunctionTest1
  ${FREESURFER_DIR}/subjects/bert/surf/lh.pial
  ${FREESURFER_DIR}/subjects/bert/surf/lh.curv.pial
  )

ADD_TEST(itkNearestNeighborInterpolateMeshFunctionTest1
  ${CXX_TEST_PATH}/itkNearestNeighborInterpolateMeshFunctionTest1
  ${FREESURFER_DIR}/subjects/bert/surf/lh.pial
  ${FREESURFER_DIR}/subjects/bert/surf/lh.curv.pial
  )

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

ADD_TEST(itkQuadEdgeMeshKdTreeGeneratorTest1
  ${CXX_TEST_PATH}/itkQuadEdgeMeshKdTreeGeneratorTest
  ${TEMP}/itkFreeSurferBinarySurfaceReaderTest2.vtk
  ${TEMP}/itkQuadEdgeMeshKdTreeGeneratorTest1.vtk
  )

ADD_TEST(itkPointLocator2Test1
  ${CXX_TEST_PATH}/itkPointLocator2Test1
  ${TEMP}/itkFreeSurferBinarySurfaceReaderTest2.vtk
  )

ADD_TEST(itkMeshToMeshRegistrationMethodTest1a
  ${CXX_TEST_PATH}/itkMeshToMeshRegistrationMethodTest1
  ${FREESURFER_DIR}/subjects/bert/surf/rh.pial
  ${FREESURFER_DIR}/subjects/bert/surf/rh.curv.pial
  ${FREESURFER_DIR}/subjects/bert/surf/rh.pial
  ${FREESURFER_DIR}/subjects/bert/surf/rh.curv.pial
  )

ADD_TEST(itkMeshToMeshMetricTest1a
  ${CXX_TEST_PATH}/itkMeshToMeshMetricTest1
  ${FREESURFER_DIR}/subjects/bert/surf/rh.pial
  ${FREESURFER_DIR}/subjects/bert/surf/rh.curv.pial
  ${FREESURFER_DIR}/subjects/bert/surf/rh.pial
  ${FREESURFER_DIR}/subjects/bert/surf/rh.curv.pial
  )


ENDIF( FREESURFER_DIR )

