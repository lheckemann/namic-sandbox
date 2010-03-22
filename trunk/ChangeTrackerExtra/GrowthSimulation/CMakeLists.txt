project(ChangeTrackerExtra_GrowthSimulation)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(GrowthSimulation GrowthSimulation.cxx)
target_link_libraries(GrowthSimulation ITKIO ITKStatistics)

add_executable(AffineToRigid AffineToRigid.cxx)
target_link_libraries(AffineToRigid ITKIO ITKStatistics)

add_executable(EstimateDeformedVolume EstimateDeformedVolume.cxx)
target_link_libraries(EstimateDeformedVolume ITKIO ITKStatistics vtkIO vtkGraphics ITKQuadEdgeMesh)
