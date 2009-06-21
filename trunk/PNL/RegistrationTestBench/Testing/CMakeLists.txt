
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR})
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)

FIND_PATH(PNL_BRAIN_DATA_DIR README.txt)

MACRO(REGISTER_IMAGES CASEID DIR1 DATASET1 EXTENSION1 DIR2 DATASET2 EXTENSION2)

ADD_TEST(RegistrationBSpline_${CASEID}
  ${EXECUTABLE_OUTPUT_PATH}/DeformableRegistration01
  ${DIR1}/${DATASET1}.${EXTENSION1}
  ${DIR2}/${DATASET2}.${EXTENSION2}
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}.mha
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DifferenceAfter.mha
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DifferenceBefore.mha
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DeformationField.mha
  1    # Use explicit PDF derivatives
  1    # Cache BSpline weights
  ${TEMP}/finalAffineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/finalBSplineTransform_${DATASET1}_${DATASET2}.txt
   5   # Number of BSpline nodes in coarse grid
  10   # Number of BSpline nodes in coarse grid
  1.0  # Initial step length
  200  # Number of iterations
  )

ADD_TEST(BSplineResampleImage_${CASEID}
  ${EXECUTABLE_OUTPUT_PATH}/BSplineResampleImage
  ${DIR1}/${DATASET1}.${EXTENSION1}
  ${DIR2}/${DATASET2}.${EXTENSION2}
  ${TEMP}/finalAffineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/finalBSplineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/${DATASET2}_RegisteredTo_${DATASET1}.mha
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DeformationField2.mha
  )

ENDMACRO(REGISTER_IMAGES)




#
#  Note that the REGISTER_IMAGES macro expects filenames
#  without extension. It internally assumes .nrrd.
#

REGISTER_IMAGES( T2-DWI
  ${PNL_BRAIN_DATA_DIR}  01053-dwi-filt-Ed-Basline-masked  nrrd
  ${PNL_BRAIN_DATA_DIR}  01053-t2w-realign-masked          nrrd
  )

REGISTER_IMAGES( T2-T2
  ${PNL_BRAIN_DATA_DIR}  01053-t2w-realign-masked   nrrd
  ${PNL_BRAIN_DATA_DIR}  01031-t2w                  nrrd
  )

#
# cross verification
#
REGISTER_IMAGES( T2-T2-Back
  ${PNL_BRAIN_DATA_DIR}  01031-t2w   nrrd
  ${TEMP}  01031-t2w_BSplineTo_01053-t2w  mha
  )

REGISTER_IMAGES( T1-T2
  ${PNL_BRAIN_DATA_DIR}  01053-t1w-realign      nrrd
  ${PNL_BRAIN_DATA_DIR}  01031-t2w              nrrd
  )


