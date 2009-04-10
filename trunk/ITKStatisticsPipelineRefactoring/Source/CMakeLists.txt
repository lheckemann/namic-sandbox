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

  # from SLATEC/FNLIB
  d1mach.c
  d9lgmc.c
  d9gmit.c
  d9lgic.c
  d9lgit.c
  dbetai.c
  dcsevl.c
  dgami.c
  dgamit.c
  dgamlm.c
  dgamma.c
  dgamr.c
  dlbeta.c
  dlgams.c
  dlngam.c
  dlnrel.c
  initds.c
  xermsg.c
  d_int.c
  d_mod.c  
 )

TARGET_LINK_LIBRARIES(ITKStatisticsNew ITKCommon)


