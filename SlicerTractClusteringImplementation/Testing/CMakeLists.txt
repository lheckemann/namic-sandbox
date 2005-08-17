
INCLUDE_DIRECTORIES( 
  ${SpectralClustering_SOURCE_DIR}/Code
  )

ADD_EXECUTABLE(testCluster testITKCluster.cxx )
TARGET_LINK_LIBRARIES(testCluster SpectralClustering ITKStatistics ITKIO ITKCommon)



