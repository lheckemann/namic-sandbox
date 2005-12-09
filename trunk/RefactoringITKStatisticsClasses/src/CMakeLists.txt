# Sources for ITKStatistics library

SET( CHANGED_STATISTICS_SOURCES "${ReworkStatistics_SOURCE_DIR}/src" )
INCLUDE_DIRECTORIES( BEFORE ${CHANGED_STATISTICS_SOURCES} )

ADD_LIBRARY(ITKStatisticsChanged 
  itkNormalVariateGenerator.cxx
 )

TARGET_LINK_LIBRARIES(ITKStatisticsChanged ITKCommon)
