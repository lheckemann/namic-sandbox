# Sources for ITKStatisticsNew library

ADD_LIBRARY(ITKStatisticsNew 
  itkDenseFrequencyContainer2.cxx
  itkSparseFrequencyContainer2.cxx
  itkDecisionRule.cxx
 )

TARGET_LINK_LIBRARIES(ITKStatisticsNew ITKCommon)


