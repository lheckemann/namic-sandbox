project(AtlasTools)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(AverageTemplate AverageTemplate.cxx)
target_link_libraries(AverageTemplate ITKIO ITKStatistics)

add_executable(HistogramMatchICC HistogramMatchICC.cxx)
target_link_libraries(HistogramMatchICC ITKIO ITKStatistics)

add_executable(GridWarp GridWarp.cxx)
target_link_libraries(GridWarp ITKIO ITKStatistics vtkIO vtkGraphics)

add_executable(InvertBSpline InvertBSpline.cxx)
target_link_libraries(InvertBSpline ITKIO ITKStatistics)

add_executable(BSplineLBFGSB BSplineLBFGSB.cxx)
target_link_libraries(BSplineLBFGSB ITKIO ITKStatistics ITKNumerics)

add_executable(BSplineGradientDescent BSplineGradientDescent.cxx)
target_link_libraries(BSplineGradientDescent ITKIO ITKStatistics ITKNumerics)

add_executable(MultiresDemons MultiresDemons.cxx)
target_link_libraries(MultiresDemons ITKIO ITKStatistics ITKNumerics)

add_executable(CombineMasks CombineMasks.cxx)
target_link_libraries(CombineMasks ITKIO ITKStatistics)

add_executable(AverageLabel AverageLabel.cxx)
target_link_libraries(AverageLabel ITKIO ITKStatistics)

add_executable(LabelFullOverlap LabelFullOverlap.cxx)
target_link_libraries(LabelFullOverlap ITKIO ITKStatistics)

add_executable(DilateLabel DilateLabel.cxx)
target_link_libraries(DilateLabel ITKIO ITKStatistics)

#subdirs(Plugins)
