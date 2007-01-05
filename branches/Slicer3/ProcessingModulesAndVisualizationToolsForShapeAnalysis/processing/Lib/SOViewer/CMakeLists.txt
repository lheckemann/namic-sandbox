
# USE_SOV()

SET(SOV_SRCS
  ColorScheme.cxx  
  sovAttributeMeshVTKRenderMethod3DFactory.cxx  
  VisualAttributeMeshProperty.cxx
)

ADD_LIBRARY(ShapeSOV ${SOV_SRCS})
 
