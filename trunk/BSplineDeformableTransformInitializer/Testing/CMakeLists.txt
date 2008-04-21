

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

INCLUDE_DIRECTORIES( 
  ${BSplineDeformableTransformInitializer}/Testing
  )

ADD_EXECUTABLE(BSplineDeformableTransformInitializerTest1 
    itkBSplineDeformableTransformInitializerTest1.cxx )

TARGET_LINK_LIBRARIES(BSplineDeformableTransformInitializerTest1
   ITKIO ITKNumerics)


SET(GRID_SIZE 5)

ADD_TEST( InitializerTest01  ${CXX_TEST_PATH}/BSplineDeformableTransformInitializerTest1 ${GRID_SIZE} )

