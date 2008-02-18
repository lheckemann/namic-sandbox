
ADD_EXECUTABLE(BSplineDeformableTransformInverseCalculatorTest 
    itkBSplineDeformableTransformInverseCalculatorTest.cxx )

TARGET_LINK_LIBRARIES(BSplineDeformableTransformInverseCalculatorTest
   ITKIO ITKNumerics)

ENABLE_TESTING()

INCLUDE_DIRECTORIES( 
  ${BSplineDeformableTransformInverseCalculator_SOURCE_DIR}/Source )

ADD_TEST( InverseTest01  BSplineDeformableTransformInverseCalculatorTest )

