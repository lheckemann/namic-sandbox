#
# Add here command lines to build examples.
#

INCLUDE_DIRECTORIES( ${QuadEdgeMeshRigidRegistration_SOURCE_DIR}/Testing )

ADD_EXECUTABLE(MeanSquaresMeshToMeshMetricRegistration1 MeanSquaresMeshToMeshMetricRegistration1.cxx) 
TARGET_LINK_LIBRARIES(MeanSquaresMeshToMeshMetricRegistration1 ITKQuadEdgeMesh ITKNumerics ITKIO) 

ADD_EXECUTABLE(MeanSquaresMeshToMeshMetricRegistrationWithMonitor1 
  MeanSquaresMeshToMeshMetricRegistrationWithMonitor1.cxx
  ${QuadEdgeMeshRigidRegistration_SOURCE_DIR}/Testing/RegistrationMonitor.cxx
  ) 
TARGET_LINK_LIBRARIES(MeanSquaresMeshToMeshMetricRegistrationWithMonitor1 
  ITKQuadEdgeMesh ITKNumerics ITKIO
  vtkRendering vtkFiltering vtkIO vtkHybrid
  ) 

ADD_EXECUTABLE(ConvertFreeSurferBinarySurfaceToVTKSurface ConvertFreeSurferBinarySurfaceToVTKSurface.cxx)
TARGET_LINK_LIBRARIES(ConvertFreeSurferBinarySurfaceToVTKSurface ITKQuadEdgeMesh ITKNumerics ITKIO) 
