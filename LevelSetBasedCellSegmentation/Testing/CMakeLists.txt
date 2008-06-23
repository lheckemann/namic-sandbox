
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${LevelSetBasedCellSegmentation_SOURCE_DIR})
SET(TEMP ${LevelSetBasedCellSegmentation_BINARY_DIR}/Testing/Temporary)

########################################
########################################
#
#  Define Executables
#
########################################
########################################

ADD_EXECUTABLE(itkLevelSetBasedCellSegmentationTest01 itkLevelSetBasedCellSegmentationTest01.cxx )
TARGET_LINK_LIBRARIES(itkLevelSetBasedCellSegmentationTest01 ITKIO ITKBasicFilters)



########################################
########################################
#
#  Define Tests
#
########################################
########################################

ADD_TEST(itkLevelSetBasedCellSegmentationTest01 
  ${CXX_TEST_PATH}/itkLevelSetBasedCellSegmentationTest01
 )

