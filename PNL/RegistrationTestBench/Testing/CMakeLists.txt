
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR})
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)

FIND_PATH(PNL_BRAIN_DATA_DIR README.txt)

MACRO(REGISTER_IMAGES CASEID DATASET1 DATASET2 )

ADD_TEST(RegistrationBSpline_${CASEID}
  ${EXECUTABLE_OUTPUT_PATH}/DeformableRegistration01
  ${PNL_BRAIN_DATA_DIR}/${DATASET1}.nrrd
  ${PNL_BRAIN_DATA_DIR}/${DATASET2}.nrrd
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

ADD_TEST(BSplineResampleImage_${CASEID}
  ${EXECUTABLE_OUTPUT_PATH}/BSplineResampleImage
  ${PNL_BRAIN_DATA_DIR}/${DATASET1}.nrrd
  ${PNL_BRAIN_DATA_DIR}/${DATASET2}.nrrd
  ${TEMP}/finalBSplineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/${DATASET2}_RegisteredTo_${DATASET1}.mhd
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DeformationField2.mhd
  )

ENDMACRO(REGISTER_IMAGES)



REGISTER_IMAGES( T2-DWI
  01053-dwi-filt-Ed-Basline-masked
  01053-t2w-realign-masked
  )

REGISTER_IMAGES( T2-DTI
  01053-DTI-baseline.nrrd
  01031-t2w.nrrd
  )

