
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR})
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)

FIND_PATH(KNEE_DATA_DIR DOC "Directory where Knee MRI DICOM images are ")


ADD_TEST(DicomToMeta58
  ${CXX_TEST_PATH}/DicomSeriesReadImageWrite
  ${KNEE_DATA_DIR}/58_MRI_Dataset
  ${TEMP}/58_MRI_Dataset.mhd
  )


