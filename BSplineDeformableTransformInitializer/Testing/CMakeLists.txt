
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${BSplineDeformableTransformInitializer_SOURCE_DIR})
SET(TEMP ${BSplineDeformableTransformInitializer_BINARY_DIR}/Testing/Temporary)

ADD_EXECUTABLE(BSplineDeformableTransformInitializerTest1 
    itkBSplineDeformableTransformInitializerTest1.cxx )

TARGET_LINK_LIBRARIES(BSplineDeformableTransformInitializerTest1
   ITKIO ITKNumerics)


ADD_TEST(BSplineWarping1Test 
        ${CXX_TEST_PATH}/BSplineDeformableTransformInitializerTest1 
        ${TEST_DATA_ROOT}/Data/BSplineDisplacements1.txt
        ${TEST_DATA_ROOT}/Data/BrainProtonDensitySliceBorder20.png
        ${TEST_DATA_ROOT}/Data/BrainProtonDensitySliceBorder20.png
        ${TEMP}/BSplineWarping1Test.png
        ${TEMP}/BSplineWarping1TestDeformationField.mhd
)

ADD_EXECUTABLE(BSplineDeformableTransformInitializerTest2 
    itkBSplineDeformableTransformInitializerTest2.cxx )

TARGET_LINK_LIBRARIES(BSplineDeformableTransformInitializerTest2
   ITKIO ITKNumerics)


ADD_TEST(BSplineWarping2Test 
        ${CXX_TEST_PATH}/BSplineDeformableTransformInitializerTest2 
        ${TEST_DATA_ROOT}/Data/BSplineDisplacements1.txt
        ${TEST_DATA_ROOT}/Data/BrainProtonDensitySliceBorder20.png
        ${TEST_DATA_ROOT}/Data/BrainProtonDensitySliceBorder20.png
        ${TEMP}/BSplineWarping2Test.png
        ${TEMP}/BSplineWarping2TestDeformationField.mhd
)


