#
# Add here command lines to build examples.
#

INCLUDE_DIRECTORIES( 
  ${QuadEdgeMeshSphericalDiffeomorphicDemonsRegistration_SOURCE_DIR}/Testing
  ${QuadEdgeMeshSphericalDiffeomorphicDemonsRegistration_SOURCE_DIR}/../QuadEdgeMeshFieldSmoothingFilters/Source/
  )

ADD_EXECUTABLE(QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
 QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1.cxx) 
TARGET_LINK_LIBRARIES(QuadEdgeMeshSphericalDiffeomorphicDemonsFilter1
 ITKQuadEdgeMesh ITKNumerics ITKIO) 

