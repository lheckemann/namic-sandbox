
NeuroLib_USE_ITK()
NeuroLib_USE_VTK()
NeuroLib_USE_SOV()
NeuroLib_USE_SHAPE()

SET(SOV_SRCS
ColorScheme.cxx  
sovAttributeMeshVTKRenderMethod3DFactory.cxx  
VisualAttributeMeshProperty.cxx
)

ADD_LIBRARY(ShapeSOV ${SOV_SRCS})
 
