NeuroLib_USE_ITK()
NeuroLib_USE_VTK()

LINK_LIBRARIES(
 ${ITK_LIBRARIES}
)

SUBDIRS(
        IO
        SpatialObject
        Numerics
        Algorithms
   SOViewer
        #Filters
)

