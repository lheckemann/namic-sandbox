
ADD_EXECUTABLE(ShapeDetectionLevelSetFilter ShapeDetectionLevelSetFilter.cxx)
TARGET_LINK_LIBRARIES(ShapeDetectionLevelSetFilter ITKBasicFilters ITKIO)

ADD_EXECUTABLE(DicomSeriesReadImageWrite DicomSeriesReadImageWrite2.cxx)
TARGET_LINK_LIBRARIES(DicomSeriesReadImageWrite ITKBasicFilters ITKIO)

ADD_EXECUTABLE(MaskImageForRegistration MaskImageForRegistration.cxx)
TARGET_LINK_LIBRARIES(MaskImageForRegistration ITKBasicFilters ITKIO)

ADD_EXECUTABLE(ImageRegistrationRigid ImageRegistrationRigid.cxx vtkKWImage.cxx vtkRegistrationMonitor.cxx)
TARGET_LINK_LIBRARIES(ImageRegistrationRigid
  ITKStatistics ITKBasicFilters ITKNumerics ITKIO ITKCommon
  vtkRendering vtkFiltering vtkIO vtkHybrid
  )

ADD_EXECUTABLE(ImageRegistrationAffine ImageRegistrationAffine.cxx vtkKWImage.cxx vtkRegistrationMonitor.cxx)
TARGET_LINK_LIBRARIES(ImageRegistrationAffine 
  ITKStatistics ITKBasicFilters ITKNumerics ITKIO ITKCommon
  vtkRendering vtkFiltering vtkIO vtkHybrid
  )

ADD_EXECUTABLE(DeformableRegistration01 DeformableRegistration01.cxx)
TARGET_LINK_LIBRARIES(DeformableRegistration01
  ITKStatistics ITKBasicFilters ITKNumerics ITKIO ITKCommon
  )

ADD_EXECUTABLE(BSplineResampleImage BSplineResampleImage.cxx)
TARGET_LINK_LIBRARIES(BSplineResampleImage
  ITKStatistics ITKBasicFilters ITKNumerics ITKIO ITKCommon
  )

ADD_EXECUTABLE(BSplineWarping BSplineWarping.cxx)
TARGET_LINK_LIBRARIES(BSplineWarping
  ITKStatistics ITKBasicFilters ITKNumerics ITKIO ITKCommon
  )


ADD_EXECUTABLE(MeanSquaresImageMetric MeanSquaresImageMetric.cxx)
TARGET_LINK_LIBRARIES(MeanSquaresImageMetric
  ITKStatistics ITKBasicFilters ITKNumerics ITKIO ITKCommon
  )

