
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
  

