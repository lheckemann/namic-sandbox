
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

ADD_TEST(RegistrationRigid_64_58
  ${EXECUTABLE_OUTPUT_PATH}/ImageRegistrationRigid
  ${TEMP}/64_MRI_DatasetMasked.mhd
  ${TEMP}/58_MRI_DatasetMasked.mhd
  ${TEMP}/RegisteredRigid_58_64.mhd
  ${TEMP}/DifferenceAfterRigidRegistration.mhd
  ${TEMP}/RigidRegistrationScreenshots
  ${TEMP}/initialRigidTransform_64_58.txt
  ${TEMP}/finalRigidTransform_64_58.txt
  )

ADD_TEST(RegistrationAffine_64_58
  ${EXECUTABLE_OUTPUT_PATH}/ImageRegistrationAffine
  ${TEMP}/64_MRI_DatasetMasked.mhd
  ${TEMP}/58_MRI_DatasetMasked.mhd
  ${TEMP}/finalRigidTransform_64_58.txt
  ${TEMP}/RegisteredAffine_58_64.mhd
  ${TEMP}/DifferenceAfterAffineRegistration.mhd
  ${TEMP}/AffineRegistrationScreenshots
  ${TEMP}/initialAffineTransform_64_58.txt
  ${TEMP}/finalAffineTransform_64_58.txt
  )

