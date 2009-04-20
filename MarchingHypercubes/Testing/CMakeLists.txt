#
#
#

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${MarchingHypercubes_SOURCE_DIR})
SET(TEMP ${MarchingHypercubes_BINARY_DIR}/Testing/Temporary)

ADD_EXECUTABLE(DividingCubesFilterTest1 itkDividingCubesFilterTest1.cxx )
TARGET_LINK_LIBRARIES(DividingCubesFilterTest1 ITKIO ITKNumerics)

ADD_EXECUTABLE(DividingCubesFilterTest2 itkDividingCubesFilterTest2.cxx )
TARGET_LINK_LIBRARIES(DividingCubesFilterTest2 ITKIO ITKNumerics)

ADD_EXECUTABLE(MarchingCubesImageToMeshFilterTest1 itkMarchingCubesImageToMeshFilterTest1.cxx)
TARGET_LINK_LIBRARIES(MarchingCubesImageToMeshFilterTest1 ITKIO ITKNumerics)

ADD_EXECUTABLE(GenerateSyntheticSphere GenerateSyntheticSphere.cxx)
TARGET_LINK_LIBRARIES(GenerateSyntheticSphere ITKIO ITKNumerics)

ADD_TEST(GenerateSyntheticSphereTest1 
  ${CXX_TEST_PATH}/GenerateSyntheticSphere 
  ${TEMP}/SyntheticSphere.vtk
)

ADD_TEST(DividingCubesTest1 
  ${CXX_TEST_PATH}/DividingCubesFilterTest1  
  ${TEMP}/SyntheticSphere.vtk
  ${TEMP}/DividingCubesTest1.vtk
  180.0  # Iso-value
  4 4 4  # Subdivision factors
)

ADD_TEST(DividingCubesTest2 
  ${CXX_TEST_PATH}/DividingCubesFilterTest2 
  ${TEMP}/SyntheticSphere.vtk
  ${TEMP}/DividingCubesTest2.vtk
  180.0  # Iso-value
)

ADD_TEST(MarchingCubesImageToMeshFilterTest1 
  ${CXX_TEST_PATH}/MarchingCubesImageToMeshFilterTest1 
  ${TEMP}/SyntheticSphere.vtk
  ${TEMP}/MarchingCubesImageToMeshFilterTest1.vtk
  180.0  # Iso-value
)

