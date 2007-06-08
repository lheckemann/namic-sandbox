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
 

SET(Groupwise_SRCS 
  MultiResImageRegistration.cxx 
  MultiResolutionImageRegistrationMethod.cxx 
  MultiResolutionImageRegistrationMethod.h 
  VarianceMultiImageMetric.cxx 
  VarianceMultiImageMetric.h 
  ParzenWindowEntropyMultiImageMetric.h
  ParzenWindowEntropyMultiImageMetric.cxx
  JointEntropyMultiImageMetric.h
  JointEntropyMultiImageMetric.cxx
  JointEntropyKNNMultiImageMetric.h
  JointEntropyKNNMultiImageMetric.cxx
  JointEntropyKNNGraphMultiImageMetric.h
  JointEntropyKNNGraphMultiImageMetric.cxx
  JointEntropyFixedMultiImageMetric.h
  JointEntropyFixedMultiImageMetric.cxx
  JointEntropyInterpolateArtefactMultiImageMetric.h
  JointEntropyInterpolateArtefactMultiImageMetric.cxx
  RegisterToMeanMultiImageMetric.h
  RegisterToMeanMultiImageMetric.cxx
  RegisterToMeanHistogramMultiImageMetric.h
  RegisterToMeanHistogramMultiImageMetric.cxx
  RegisterToMeanKNNMultiImageMetric.h
  RegisterToMeanKNNMultiImageMetric.cxx
  RegisterToMeanAndVarianceMultiImageMetric.h
  RegisterToMeanAndVarianceMultiImageMetric.cxx
  MutualInformationLinearInterpolateImageFunction.h
  MutualInformationLinearInterpolateImageFunction.cxx
  GradientDescentLineSearchOptimizer.h
  GradientDescentLineSearchOptimizer.cxx
  UserBSplineDeformableTransform.h
  UserBSplineDeformableTransform.cxx
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


ADD_EXECUTABLE(GroupwiseRegistration ${Groupwise_SRCS})
TARGET_LINK_LIBRARIES(GroupwiseRegistration ITKCommon ITKNumerics ITKIO ITKBasicFilters ITKStatistics ANN)

ADD_EXECUTABLE(TransformImage TransformImage.cxx )
TARGET_LINK_LIBRARIES(TransformImage ITKCommon ITKBasicFilters ITKIO)

ADD_EXECUTABLE(ComputeDice ComputeDice.cxx )
TARGET_LINK_LIBRARIES(ComputeDice ITKCommon ITKBasicFilters ITKIO)

ADD_EXECUTABLE(PairwiseRegistration ${Pairwise_SRCS} )
TARGET_LINK_LIBRARIES(PairwiseRegistration ITKCommon ITKNumerics ITKIO ITKBasicFilters)

ADD_EXECUTABLE(CreateImageSetAffine CreateImageSetAffine.cxx )
TARGET_LINK_LIBRARIES(CreateImageSetAffine ITKCommon ITKBasicFilters ITKIO ITKStatistics)
    
ADD_EXECUTABLE(CreateImageSetBspline CreateImageSetBspline.cxx )
TARGET_LINK_LIBRARIES(CreateImageSetBspline ITKCommon ITKBasicFilters ITKIO ITKStatistics)


