project(AtlasTools)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(AverageTemplate AverageTemplate.cxx)
target_link_libraries(AverageTemplate ITKIO ITKStatistics)

add_executable(HistogramMatchICC HistogramMatchICC.cxx)
target_link_libraries(HistogramMatchICC ITKIO ITKStatistics)
#subdirs(Plugins)
