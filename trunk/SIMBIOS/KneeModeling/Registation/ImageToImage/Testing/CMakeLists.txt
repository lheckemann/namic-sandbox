
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR})
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)


FIND_PATH(KNEE_DATA_DIR README.txt)


MACRO(PREPAREMASK DATASETID )

ADD_TEST(ShapeDetection${DATASETID}
  ${CXX_TEST_PATH}/ShapeDetectionLevelSetFilter
  ${KNEE_DATA_DIR}/${DATASETID}.mhd
  ${TEMP}/1_Mask.mhd
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
  ${TEMP}/${DATASET1}_MRI_DatasetMasked.mhd
  ${TEMP}/${DATASET2}_MRI_DatasetMasked.mhd
  ${TEMP}/RegisteredRigid_${DATASET2}_${DATASET1}.mhd
  ${TEMP}/DifferenceAfterRigidRegistration.mhd
  ${TEMP}/RigidRegistrationScreenshots
  ${TEMP}/initialRigidTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/finalRigidTransform_${DATASET1}_${DATASET2}.txt
  )

ADD_TEST(RegistrationAffine_${DATASET1}_${DATASET2}
  ${EXECUTABLE_OUTPUT_PATH}/ImageRegistrationAffine
  ${TEMP}/${DATASET1}_MRI_DatasetMasked.mhd
  ${TEMP}/${DATASET2}_MRI_DatasetMasked.mhd
  ${TEMP}/finalRigidTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/RegisteredAffine_${DATASET2}_${DATASET1}.mhd
  ${TEMP}/DifferenceAfterAffineRegistration.mhd
  ${TEMP}/AffineRegistrationScreenshots
  ${TEMP}/initialAffineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/finalAffineTransform_${DATASET1}_${DATASET2}.txt
  )

ENDMACRO(REGISTER)

PREPAREMASK( 1 )
PREPAREMASK( 56 )

REGISTER( 1 56 )

