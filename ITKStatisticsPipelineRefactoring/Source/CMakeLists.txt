# Sources for ITKStatisticsNew library

ADD_LIBRARY(ITKStatisticsNew 
  itkDenseFrequencyContainer2.cxx
  itkSparseFrequencyContainer2.cxx
 )

TARGET_LINK_LIBRARIES(ITKStatisticsNew ITKCommon)


