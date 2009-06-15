
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR})
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)

FIND_PATH(KNEE_DATA_DIR README.txt)


ADD_TEST(DicomToMeta58
  ${CXX_TEST_PATH}/DicomSeriesReadImageWrite
  ${KNEE_DATA_DIR}/58_MRI_Dataset
  ${TEMP}/58_MRI_Dataset.mhd
  )

ADD_TEST(DicomToMeta64
  ${CXX_TEST_PATH}/DicomSeriesReadImageWrite
  ${KNEE_DATA_DIR}/64_MRI_Dataset
  ${TEMP}/64_MRI_Dataset.mhd
  )

ADD_TEST(ShapeDetection42
  ${CXX_TEST_PATH}/ShapeDetectionLevelSetFilter
  ${KNEE_DATA_DIR}/42_MRI_Dataset.mhd
  ${TEMP}/42_MRI_DatasetMask.mhd
   0.7 # Sigma in mm
  -1.0 # Alpha
   5.0 # Beta
  25.0 # Initial radius in mm
  142  70.1307  34.2382
  166  201  32.6597
  )
 
ADD_TEST(Masking42
  ${CXX_TEST_PATH}/MaskImageForRegistration
  ${KNEE_DATA_DIR}/42_MRI_Dataset.mhd
  ${TEMP}/42_MRI_DatasetMask.mhd
  ${TEMP}/42_MRI_DatasetMasked.mhd
  ) 


ADD_TEST(ShapeDetection47
  ${CXX_TEST_PATH}/ShapeDetectionLevelSetFilter
  ${KNEE_DATA_DIR}/PFP47_mri.nhdr
  ${TEMP}/47_MRI_DatasetMask.mhd
   0.7 # Sigma in mm
  -1.0 # Alpha
   5.0 # Beta
  25.0 # Initial radius in mm
  142  34.3778  31.4447 # Seeds
  150  101.071  30.1554
  174  106.113  43.4066
  174  106.342  15.6865
  181  233.54  28.2931
  181  195.266  38.8224
  181  196.412  20.4856
  181  196.87  20.4856
  )
 
ADD_TEST(Masking47
  ${CXX_TEST_PATH}/MaskImageForRegistration
  ${TEMP}/47_MRI_Dataset.mhd
  ${TEMP}/47_MRI_DatasetMask.mhd
  ${TEMP}/47_MRI_DatasetMasked.mhd
  ) 

ADD_TEST(ShapeDetection58
  ${CXX_TEST_PATH}/ShapeDetectionLevelSetFilter
  ${TEMP}/58_MRI_Dataset.mhd
  ${TEMP}/58_MRI_DatasetMask.mhd
   0.7 # Sigma in mm
  -1.0 # Alpha
   5.0 # Beta
  25.0 # Initial radius in mm
  119  85.5  19.2188   # Seed 1
  119  79  46.0938     # Seed 2
  99  50  33.9062      # Seed 3
  135  178.5  35.4688  # Seed 4
  )
  
ADD_TEST(Masking58
  ${CXX_TEST_PATH}/MaskImageForRegistration
  ${TEMP}/58_MRI_Dataset.mhd
  ${TEMP}/58_MRI_DatasetMask.mhd
  ${TEMP}/58_MRI_DatasetMasked.mhd
  ) 

ADD_TEST(ShapeDetection64
  ${CXX_TEST_PATH}/ShapeDetectionLevelSetFilter
  ${TEMP}/64_MRI_Dataset.mhd
  ${TEMP}/64_MRI_DatasetMask.mhd
   0.7 # Sigma in mm
  -1.0 # Alpha
   5.0 # Beta
  30.0 # Initial radius in mm
  134  24.0645  25.4849  # Seed points
  165  72.6517  41.4443
  165  78.6106  14.1212
  165  217.726  29.0779
  165  162.263  35.5953
  165  160.888  17.2963
  )
  
ADD_TEST(Masking64
  ${CXX_TEST_PATH}/MaskImageForRegistration
  ${TEMP}/64_MRI_Dataset.mhd
  ${TEMP}/64_MRI_DatasetMask.mhd
  ${TEMP}/64_MRI_DatasetMasked.mhd
  ) 

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

REGISTER( 64 58 )
REGISTER( 47 58 )
REGISTER( 64 42 )

