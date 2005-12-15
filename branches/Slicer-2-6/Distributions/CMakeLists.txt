PROJECT(Distributions)

SET( LIBRARY_OUTPUT_PATH ${Distributions_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
SET( EXECUTABLE_OUTPUT_PATH ${Distributions_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

# 
# Find ITK
#
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build distribution applications without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

INCLUDE_DIRECTORIES(${ITK_SOURCE_DIR}/Utilities/vxl/v3p/netlib)

# Turn on testing
#
ENABLE_TESTING()

ADD_LIBRARY(Distributions
  itkProbabilityDistribution.cxx
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
TARGET_LINK_LIBRARIES(Distributions ITKCommon)

ADD_TEST(itkChiSquareDistributionTest ${EXECUTABLE_OUTPUT_PATH}/itkChiSquareDistributionTest itkChiSquareDistributionTest)
ADD_TEST(itkGaussianDistributionTest ${EXECUTABLE_OUTPUT_PATH}/itkGaussianDistributionTest itkGaussianDistributionTest)
ADD_TEST(itkTDistributionTest ${EXECUTABLE_OUTPUT_PATH}/itkTDistributionTest itkTDistributionTest)

ADD_EXECUTABLE(itkChiSquareDistributionTest itkChiSquareDistributionTest.cxx)
TARGET_LINK_LIBRARIES(itkChiSquareDistributionTest Distributions ITKCommon)

ADD_EXECUTABLE(itkGaussianDistributionTest itkGaussianDistributionTest.cxx)
TARGET_LINK_LIBRARIES(itkGaussianDistributionTest Distributions ITKCommon)

ADD_EXECUTABLE(itkTDistributionTest itkTDistributionTest.cxx)
TARGET_LINK_LIBRARIES(itkTDistributionTest Distributions ITKCommon)
