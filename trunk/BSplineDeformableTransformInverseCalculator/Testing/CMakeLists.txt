
ADD_EXECUTABLE(BSplineDeformableTransformInverseCalculatorTest 
    itkBSplineDeformableTransformInverseCalculatorTest.cxx )

TARGET_LINK_LIBRARIES(BSplineDeformableTransformInverseCalculatorTest
   ITKIO ITKNumerics)

INCLUDE_DIRECTORIES( 
  ${BSplineDeformableTransformInverseCalculators_SOURCE_DIR}/Testing
  )


ADD_EXECUTABLE(MachineInformationExample MachineInformationExample.cxx MachineInformation.cxx)
TARGET_LINK_LIBRARIES(MachineInformationExample itksys)

ADD_TEST( InverseTest01  BSplineDeformableTransformInverseCalculatorTest )

