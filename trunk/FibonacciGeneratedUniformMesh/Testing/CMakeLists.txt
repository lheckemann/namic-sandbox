
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEMP ${FibonacciGeneratedUniformMesh_BINARY_DIR}/Testing/Temporary)

ADD_EXECUTABLE(FibonacciSphereSourceTest1 vtkFibonacciSphereSourceTest1.cxx )
TARGET_LINK_LIBRARIES(FibonacciSphereSourceTest1
  FibonacciGeneratedUniformMesh 
  vtkRendering
  vtkIO
  )

ADD_TEST(FibonacciSphereSourceTest1 
  ${CXX_TEST_PATH}/FibonacciSphereSourceTest1
  ${TEMP}/FibonacciSphereSourceTest1Output1.vtk
  )

