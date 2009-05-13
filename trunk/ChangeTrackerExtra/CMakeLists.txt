project(ChangeTrackerExtra)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(stapler stapler.cxx)
target_link_libraries(stapler ITKIO ITKStatistics)

#subdirs(Plugins)
