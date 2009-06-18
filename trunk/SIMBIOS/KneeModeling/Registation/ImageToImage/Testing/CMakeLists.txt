
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR})
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)


FIND_PATH(KNEE_DATA_DIR README.txt)


MACRO(PREPAREMASK DATASETID )

ADD_TEST(ShapeDetection${DATASETID}
  ${CXX_TEST_PATH}/ShapeDetectionLevelSetFilter
  ${KNEE_DATA_DIR}/${DATASETID}.mhd
  ${TEMP}/${DATASETID}_Mask.mhd
   0.7 # Sigma in mm
  -1.0 # Alpha
   5.0 # Beta
  25.0 # Initial radius in mm
  ${KNEE_DATA_DIR}/${DATASETID}_Seeds.txt
  )
 
ADD_TEST(Masking${DATASETID}
  ${CXX_TEST_PATH}/MaskImageForRegistration
  ${KNEE_DATA_DIR}/${DATASETID}.mhd
  ${TEMP}/${DATASETID}_Mask.mhd
  ${TEMP}/${DATASETID}_Masked.mhd
  ) 

ENDMACRO(PREPAREMASK)


MACRO(REGISTER DATASET1 DATASET2)

ADD_TEST(RegistrationRigid_${DATASET1}_${DATASET2}
  ${EXECUTABLE_OUTPUT_PATH}/ImageRegistrationRigid
  ${TEMP}/${DATASET1}_Masked.mhd
  ${TEMP}/${DATASET2}_Masked.mhd
  ${TEMP}/RegisteredRigid_${DATASET2}_${DATASET1}.mhd
  ${TEMP}/DifferenceAfterRigidRegistration.mhd
  ${TEMP}/RigidRegistrationScreenshots
  ${TEMP}/initialRigidTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/finalRigidTransform_${DATASET1}_${DATASET2}.txt
  )

ADD_TEST(RegistrationAffine_${DATASET1}_${DATASET2}
  ${EXECUTABLE_OUTPUT_PATH}/ImageRegistrationAffine
  ${TEMP}/${DATASET1}_Masked.mhd
  ${TEMP}/${DATASET2}_Masked.mhd
  ${TEMP}/finalRigidTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/RegisteredAffine_${DATASET2}_${DATASET1}.mhd
  ${TEMP}/DifferenceAfterAffineRegistration.mhd
  ${TEMP}/AffineRegistrationScreenshots
  ${TEMP}/initialAffineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/finalAffineTransform_${DATASET1}_${DATASET2}.txt
  )

ADD_TEST(RegistrationBSpline_${DATASET1}_${DATASET2}
  ${EXECUTABLE_OUTPUT_PATH}/DeformableRegistration01
  ${TEMP}/${DATASET1}_Masked.mhd
  ${TEMP}/${DATASET2}_Masked.mhd
  ${TEMP}/${DATASET1}_Mask.mhd
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
  100  # Number of iterations
  )

ADD_TEST(BSplineResampleImage_${DATASET1}_${DATASET2}
  ${EXECUTABLE_OUTPUT_PATH}/BSplineResampleImage
  ${KNEE_DATA_DIR}/${DATASET1}.mhd
  ${KNEE_DATA_DIR}/${DATASET2}.mhd
  ${TEMP}/finalBSplineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/${DATASET2}_RegisteredTo_${DATASET1}.mhd
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DeformationField2.mhd
  )

ENDMACRO(REGISTER)

PREPAREMASK( 1 )
PREPAREMASK( 42 )
PREPAREMASK( 56 )
PREPAREMASK( 58 )
PREPAREMASK( 64 )

REGISTER( 64 42 )

