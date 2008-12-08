
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEMP ${FibonacciGeneratedUniformMesh_BINARY_DIR}/Testing/Temporary)

ADD_EXECUTABLE(FibonacciSphereSourceTest1 vtkFibonacciSphereSourceTest1.cxx )
TARGET_LINK_LIBRARIES(FibonacciSphereSourceTest1
  FibonacciGeneratedUniformMesh 
  vtkRendering
  vtkIO
  )

ADD_EXECUTABLE(FibonacciSphereSourceTest2 vtkFibonacciSphereSourceTest2.cxx )
TARGET_LINK_LIBRARIES(FibonacciSphereSourceTest2
  FibonacciGeneratedUniformMesh 
  vtkRendering
  vtkIO
  )


ADD_TEST(FibonacciSphereSourceTest1 
  ${CXX_TEST_PATH}/FibonacciSphereSourceTest1
  ${TEMP}/FibonacciSphereSourceTest1Output1.vtk
  100
  )

ADD_TEST(FibonacciSphereSourceTest2a 
  ${CXX_TEST_PATH}/FibonacciSphereSourceTest2
  ${TEMP}/FibonacciSphereSourceTest2Output1.vtk
  100
  )

ADD_TEST(FibonacciSphereSourceTest2b 
  ${CXX_TEST_PATH}/FibonacciSphereSourceTest2
  ${TEMP}/FibonacciSphereSourceTest2Output2.vtk
  1000
  )

