project(ChangeTrackerExtra_ChangeDetection)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(ChangeDetection ChangeDetection.cxx)
target_link_libraries(ChangeDetection ITKIO ITKStatistics)
