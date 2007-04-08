#
# List of source files
#
SET(MultiImageRegistration_SRCS 
  VarianceMultiImageRegistrationAffine.cxx 
  MultiImageRegistrationMethod.cxx 
  MultiImageRegistrationMethod.h 
  VarianceMultiImageMetric.cxx 
  VarianceMultiImageMetric.h 
  MultiImageMetric.cxx 
  MultiImageMetric.h 
  AddImageFilter.h
  UserMacro.h
 )
 
SET(MultiImageRegistrationBspline_SRCS 
  VarianceMultiImageRegistrationBspline.cxx 
  MultiImageRegistrationMethod.cxx 
  MultiImageRegistrationMethod.h 
  VarianceMultiImageMetric.cxx 
  VarianceMultiImageMetric.h 
  MultiImageMetric.cxx 
  MultiImageMetric.h 
  AddImageFilter.h
  UserMacro.h
 )
 

SET(Congealing_SRCS 
  MultiResImageRegistration.cxx 
  MultiResolutionImageRegistrationMethod.cxx 
  MultiResolutionImageRegistrationMethod.h 
  VarianceMultiImageMetric.cxx 
  VarianceMultiImageMetric.h 
  ParzenWindowEntropyMultiImageMetric.h
  ParzenWindowEntropyMultiImageMetric.cxx
  JointEntropyMultiImageMetric.h
  JointEntropyMultiImageMetric.cxx
  GradientDescentLineSearchOptimizer.h
  GradientDescentLineSearchOptimizer.cxx
  MultiImageMetric.cxx 
  MultiImageMetric.h 
  AddImageFilter.h
  UserMacro.h
 )
 
SET(Pairwise_SRCS 
  PairwiseRegistration.cxx 
  GradientDescentLineSearchOptimizer.h
  GradientDescentLineSearchOptimizer.cxx
  MutualInformationImageToImageMetricBsplineRegularization.h
  MutualInformationImageToImageMetricBsplineRegularization.cxx
  AddImageFilter.h
 )


ADD_EXECUTABLE(Congeal ${Congealing_SRCS})
TARGET_LINK_LIBRARIES(Congeal ITKCommon ITKNumerics ITKIO ITKBasicFilters)

ADD_EXECUTABLE(TransformImage TransformImage.cxx )
TARGET_LINK_LIBRARIES(TransformImage ITKCommon ITKBasicFilters ITKIO)

ADD_EXECUTABLE(PairwiseRegistration ${Pairwise_SRCS} )
TARGET_LINK_LIBRARIES(PairwiseRegistration ITKCommon ITKNumerics ITKIO ITKBasicFilters)

ADD_EXECUTABLE(CreateImageSetAffine CreateImageSetAffine.cxx )
TARGET_LINK_LIBRARIES(CreateImageSetAffine ITKCommon ITKBasicFilters ITKIO ITKStatistics)
    
ADD_EXECUTABLE(CreateImageSetBspline CreateImageSetBspline.cxx )
TARGET_LINK_LIBRARIES(CreateImageSetBspline ITKCommon ITKBasicFilters ITKIO ITKStatistics)


