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

#subdirs(Plugins)
