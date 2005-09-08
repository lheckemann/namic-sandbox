
NeuroLib_USE_ITK()
NeuroLib_USE_SHAPE()

SET(SpatialObject_SRCS
SphericalHarmonicSpatialObject.cxx
)

ADD_LIBRARY(SpatialObject ${SpatialObject_SRCS})
