project(ChangeTrackerExtra_GrowthSimulation)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(GrowthSimulation GrowthSimulation.cxx)
target_link_libraries(GrowthSimulation ITKIO ITKStatistics)

add_executable(AffineToRigid AffineToRigid.cxx)
target_link_libraries(AffineToRigid ITKIO ITKStatistics)
