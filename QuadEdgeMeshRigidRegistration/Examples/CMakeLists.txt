#
# Add here command lines to build examples.
#

ADD_EXECUTABLE(MeanSquaresMeshToMeshMetricTest1 MeanSquaresMeshToMeshMetricTest1.cxx)
TARGET_LINK_LIBRARIES(MeanSquaresMeshToMeshMetricTest1 ITKQuadEdgeMesh ITKNumerics ITKIO) 
