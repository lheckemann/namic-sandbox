
NeuroLib_USE_ITK()
NeuroLib_USE_SHAPE()


SET(ShapeIO_SRCS
SphericalHarmonicCoefficientFileReader.cxx
SphericalHarmonicCoefficientFileWriter.cxx
)

ADD_LIBRARY(ShapeIO ${ShapeIO_SRCS})
                                                                                                                                                                                                    
