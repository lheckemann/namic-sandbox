#
# List of source files
#
SET(MultiImageRegistration_SRCS 
  VarianceMultiImageRegistration.cxx 
  MultiImageRegistrationMethod.cxx 
  MultiImageRegistrationMethod.h 
  VarianceMultiImageMetric.cxx 
  VarianceMultiImageMetric.h 
  MultiImageMetric.cxx 
  MultiImageMetric.h 
  UserMacro.h
 )


ADD_EXECUTABLE(variance ${MultiImageRegistration_SRCS})

TARGET_LINK_LIBRARIES(variance ITKNumerics ITKIO )

