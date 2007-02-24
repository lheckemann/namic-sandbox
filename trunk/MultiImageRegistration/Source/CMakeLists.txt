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
 

SET(MultiResImageRegistration_SRCS 
  MultiResImageRegistration.cxx 
  MultiResolutionImageRegistrationMethod.cxx 
  MultiResolutionImageRegistrationMethod.h 
  VarianceMultiImageMetric.cxx 
  VarianceMultiImageMetric.h 
  ParzenWindowEntropyMultiImageMetric.h
  ParzenWindowEntropyMultiImageMetric.cxx
  GradientDescentLineSearchOptimizer.h
  GradientDescentLineSearchOptimizer.cxx
  MultiImageMetric.cxx 
  MultiImageMetric.h 
  AddImageFilter.h
  UserMacro.h
 )
 


#ADD_EXECUTABLE(RegisterVarianceAffine ${MultiImageRegistration_SRCS})
#TARGET_LINK_LIBRARIES(RegisterVarianceAffine ITKNumerics ITKIO )

#ADD_EXECUTABLE(RegisterVarianceBspline ${MultiImageRegistrationBspline_SRCS})
#TARGET_LINK_LIBRARIES(RegisterVarianceBspline ITKNumerics ITKIO )

ADD_EXECUTABLE(RegisterVarianceAffineMultiRes ${MultiResImageRegistration_SRCS})
TARGET_LINK_LIBRARIES(RegisterVarianceAffineMultiRes ITKNumerics ITKIO )

ADD_EXECUTABLE(CreateImageSetAffine CreateImageSetAffine.cxx )
TARGET_LINK_LIBRARIES(CreateImageSetAffine ITKCommon ITKBasicFilters ITKIO ITKStatistics)
    
ADD_EXECUTABLE(CreateImageSetBspline CreateImageSetBspline.cxx )
TARGET_LINK_LIBRARIES(CreateImageSetBspline ITKCommon ITKBasicFilters ITKIO ITKStatistics)

