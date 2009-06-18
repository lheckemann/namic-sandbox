
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR})
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)

FIND_PATH(PNL_BRAIN_DATA_DIR README.txt)

MACRO(REGISTER DATASET1 DATASET2)

ADD_TEST(RegistrationBSpline_${DATASET1}_${DATASET2}
  ${EXECUTABLE_OUTPUT_PATH}/DeformableRegistration01
  ${TEMP}/${DATASET1}
  ${TEMP}/${DATASET2}
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}.mhd
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DifferenceAfter.mhd
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DifferenceBefore.mhd
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DeformationField.mhd
  1    # Use explicit PDF derivatives
  1    # Cache BSpline weights
  ${TEMP}/finalBSplineTransform_${DATASET1}_${DATASET2}.txt
   5   # Number of BSpline nodes in coarse grid
  10   # Number of BSpline nodes in coarse grid
  1.0  # Initial step length
  200  # Number of iterations
  )

ADD_TEST(BSplineResampleImage_${DATASET1}_${DATASET2}
  ${EXECUTABLE_OUTPUT_PATH}/BSplineResampleImage
  ${KNEE_DATA_DIR}/${DATASET1}
  ${KNEE_DATA_DIR}/${DATASET2}
  ${TEMP}/finalBSplineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/${DATASET2}_RegisteredTo_${DATASET1}.mhd
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DeformationField2.mhd
  )

ENDMACRO(REGISTER)

REGISTER( 64 42 )
