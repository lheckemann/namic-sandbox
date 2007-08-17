#
# List of source files
#

SET(Groupwise_SRCS 
  MultiResImageRegistration.cxx 
  MultiResolutionImageRegistrationMethod.cxx 
  MultiResolutionImageRegistrationMethod.h 
  VarianceMultiImageMetric.txx 
  VarianceMultiImageMetric.h 
  ParzenWindowEntropyMultiImageMetric.h
  ParzenWindowEntropyMultiImageMetric.cxx
  GradientDescentLineSearchOptimizer.h
  GradientDescentLineSearchOptimizer.cxx
  UserBSplineDeformableTransform.h
  UserBSplineDeformableTransform.cxx
  MultiImageMetric.cxx 
  MultiImageMetric.h 
  AddImageFilter.h
  UserMacro.h
 )
  

ADD_EXECUTABLE(GroupwiseRegistration ${Groupwise_SRCS})
TARGET_LINK_LIBRARIES(GroupwiseRegistration ITKCommon ITKNumerics ITKIO ITKBasicFilters ITKStatistics )

ADD_EXECUTABLE(TransformImage TransformImage.cxx )
TARGET_LINK_LIBRARIES(TransformImage ITKCommon ITKBasicFilters ITKIO)

ADD_EXECUTABLE(ComputeDice ComputeDice.cxx )
TARGET_LINK_LIBRARIES(ComputeDice ITKCommon ITKBasicFilters ITKIO)

ADD_EXECUTABLE(ComputeOutputs ComputeOutputs.cxx )
TARGET_LINK_LIBRARIES(ComputeOutputs ITKCommon ITKBasicFilters ITKIO)

ADD_EXECUTABLE(CreateImageSetAffine CreateImageSetAffine.cxx )
TARGET_LINK_LIBRARIES(CreateImageSetAffine ITKCommon ITKBasicFilters ITKIO ITKStatistics)
    
ADD_EXECUTABLE(CreateImageSetBspline CreateImageSetBspline.cxx )
TARGET_LINK_LIBRARIES(CreateImageSetBspline ITKCommon ITKBasicFilters ITKIO ITKStatistics)

ADD_EXECUTABLE(ComputeJacobianField ComputeJacobianField.cxx )
TARGET_LINK_LIBRARIES(ComputeJacobianField ITKCommon ITKBasicFilters ITKIO ITKStatistics)
