# Sources for ITKStatisticsNew library

ADD_LIBRARY(ITKStatisticsNew 
  itkDenseFrequencyContainer2.cxx
  itkSparseFrequencyContainer2.cxx
  itkDecisionRule.cxx
  itkMaximumDecisionRule2.cxx
  itkMaximumRatioDecisionRule2.cxx
  itkMinimumDecisionRule2.cxx
  itkNormalVariateGenerator.cxx

  # Distributions
  itkGaussianDistribution.cxx
  itkTDistribution.cxx
  itkChiSquareDistribution.cxx
 )

TARGET_LINK_LIBRARIES(ITKStatisticsNew ITKCommon itkNetlibSlatec)
