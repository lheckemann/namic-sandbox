
ADD_EXECUTABLE(DeformableRegistration01 DeformableRegistration01.cxx)
TARGET_LINK_LIBRARIES(DeformableRegistration01
  ITKStatistics ITKBasicFilters ITKNumerics ITKIO ITKCommon
  )

ADD_EXECUTABLE(BSplineResampleImage BSplineResampleImage.cxx)
TARGET_LINK_LIBRARIES(BSplineResampleImage
  ITKStatistics ITKBasicFilters ITKNumerics ITKIO ITKCommon
  )

