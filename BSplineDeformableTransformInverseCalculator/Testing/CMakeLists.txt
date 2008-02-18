

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

INCLUDE_DIRECTORIES( 
  ${BSplineDeformableTransformInverseCalculators_SOURCE_DIR}/Testing
  )

ADD_EXECUTABLE(MachineInformationExample MachineInformationExample.cxx MachineInformation.cxx)
TARGET_LINK_LIBRARIES(MachineInformationExample itksys)

ADD_EXECUTABLE(BSplineDeformableTransformInverseCalculatorTest 
    itkBSplineDeformableTransformInverseCalculatorTest.cxx )

TARGET_LINK_LIBRARIES(BSplineDeformableTransformInverseCalculatorTest
   ITKIO ITKNumerics)

ADD_EXECUTABLE(BSplineDeformableTransformToKernelTransformInverseCalculatorTest 
    itkBSplineDeformableTransformToKernelTransformInverseCalculatorTest.cxx )

TARGET_LINK_LIBRARIES(BSplineDeformableTransformToKernelTransformInverseCalculatorTest
   ITKIO ITKNumerics)


SET(GRID_SIZE 5)

ADD_TEST( InverseTest01  ${CXX_TEST_PATH}/BSplineDeformableTransformInverseCalculatorTest ${GRID_SIZE} )
ADD_TEST( InverseTest02  ${CXX_TEST_PATH}/BSplineDeformableTransformToKernelTransformInverseCalculatorTest ${GRID_SIZE} )

