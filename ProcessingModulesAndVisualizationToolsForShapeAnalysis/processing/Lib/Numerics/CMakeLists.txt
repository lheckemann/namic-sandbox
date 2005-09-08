
NeuroLib_USE_ITK()
NeuroLib_USE_SHAPE()

#SET(ShapeNumerics_SRCS
#SphericalHarmonicPolynomial.cxx
#)

#ADD_LIBRARY(ShapeNumerics ${ShapeNumerics_SRCS})
#INSTALL_TARGETS(/lib/NeuroLib ShapeNumerics)
INSTALL_FILES(/include/NeuroLib/Shape/Numerics "(\\.h|\\.txx)$")
