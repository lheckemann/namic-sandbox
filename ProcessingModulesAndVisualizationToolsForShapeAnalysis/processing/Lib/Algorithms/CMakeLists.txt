
NeuroLib_USE_ITK()
NeuroLib_USE_SHAPE()
NeuroLib_USE_SPARSEMATRIXLIB()

SET(ShapeAlgorithms_SRCS
 SphericalHarmonicMeshSource.cxx TSurfaceNet.cxx TVoxelVolume.cxx
 EqualAreaParametricMeshNewtonIterator.cxx
 ParametricMeshToSPHARMSpatialObjectFilter.cxx       
)

ADD_LIBRARY(ShapeAlgorithms ${ShapeAlgorithms_SRCS})
