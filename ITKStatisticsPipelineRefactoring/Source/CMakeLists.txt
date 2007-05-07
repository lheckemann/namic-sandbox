# Sources for ITKStatisticsNew library
INCLUDE_DIRECTORIES(${ITK_SOURCE_DIR}/Utilities/vxl/v3p/netlib)

ADD_LIBRARY(ITKStatisticsNew 
  itkNormalVariateGenerator.cxx
  itkDenseFrequencyContainer.cxx
  itkSparseFrequencyContainer.cxx

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


