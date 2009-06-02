#
# List of source files
#

SET(Groupwise_SRCS 
  GroupwiseRegistrationExample.cxx 
  MultiResolutionMultiImageRegistrationMethod.txx 
  MultiResolutionMultiImageRegistrationMethod.h
  VarianceMultiImageMetric.txx 
  VarianceMultiImageMetric.h 
  FixedTemplateVarianceMultiImageMetric.txx 
  FixedTemplateVarianceMultiImageMetric.h 
  UnivariateEntropyMultiImageMetric.h
  UnivariateEntropyMultiImageMetric.txx
  GradientDescentLineSearchOptimizer.h
  GradientDescentLineSearchOptimizer.cxx
  BSplineDeformableTransformOpt.h
  BSplineDeformableTransformOpt.txx
  MultiImageMetric.txx 
  MultiImageMetric.h 
  UserMacro.h
 )
  

ADD_EXECUTABLE(GroupwiseRegistration ${Groupwise_SRCS})
TARGET_LINK_LIBRARIES(GroupwiseRegistration ITKCommon ITKNumerics ITKIO ITKBasicFilters ITKStatistics )

ADD_EXECUTABLE(TransformImage TransformImage.cxx )
TARGET_LINK_LIBRARIES(TransformImage ITKCommon ITKBasicFilters ITKIO)

ADD_EXECUTABLE(ComputeDice ComputeDice.cxx )
TARGET_LINK_LIBRARIES(ComputeDice ITKCommon ITKBasicFilters ITKIO)

ADD_EXECUTABLE(CompareDeformationFields CompareDeformationFields.cxx )
TARGET_LINK_LIBRARIES(CompareDeformationFields ITKCommon ITKBasicFilters ITKIO)

ADD_EXECUTABLE(ComputeOutputs ComputeOutputs.cxx )
TARGET_LINK_LIBRARIES(ComputeOutputs ITKCommon ITKBasicFilters ITKIO)

ADD_EXECUTABLE(CreateImageSetAffine CreateImageSetAffine.cxx )
TARGET_LINK_LIBRARIES(CreateImageSetAffine ITKCommon ITKBasicFilters ITKIO ITKStatistics)
    
ADD_EXECUTABLE(CreateImageSetBspline CreateImageSetBspline.cxx )
TARGET_LINK_LIBRARIES(CreateImageSetBspline ITKCommon ITKBasicFilters ITKIO ITKStatistics)

ADD_EXECUTABLE(ComputeJacobianField ComputeJacobianField.cxx )
TARGET_LINK_LIBRARIES(ComputeJacobianField ITKCommon ITKBasicFilters ITKIO ITKStatistics)

ADD_EXECUTABLE(ComputeDeformationFields ComputeDeformationFields.cxx )
TARGET_LINK_LIBRARIES(ComputeDeformationFields ITKCommon ITKBasicFilters ITKIO)

ADD_EXECUTABLE(PriorGen SpatialPriorGenerator.cxx)
TARGET_LINK_LIBRARIES(PriorGen ITKCommon ITKIO)
