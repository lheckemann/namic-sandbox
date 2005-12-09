
INCLUDE_DIRECTORIES( 
  ${SlicerClustering_SOURCE_DIR}/Code
  )

ADD_EXECUTABLE(testCluster testITKCluster.cxx )
ADD_TEST(testCluster ${EXECUTABLE_OUTPUT_PATH}/testCluster )
TARGET_LINK_LIBRARIES(testCluster SlicerClustering ITKStatistics ITKIO ITKCommon)



