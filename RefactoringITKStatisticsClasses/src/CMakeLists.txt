# Sources for ITKStatistics library

ADD_LIBRARY(ITKStatisticsChanged 
  itkNormalVariateGenerator.cxx
 )

TARGET_LINK_LIBRARIES(ITKStatisticsChanged ITKCommon)
