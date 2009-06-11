
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
  1.5  # Sigma in mm
  -1.0 # Alpha
   4.0 # Beta
  15.0 # Initial radius in mm
  143  174  24   # Indexes for seed 1
  112.5  81  24  # Indexes for seed 2 
  )
  

