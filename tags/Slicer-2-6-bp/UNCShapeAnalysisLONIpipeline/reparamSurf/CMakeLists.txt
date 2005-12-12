PROJECT( reparamSurf ) 

SET(reparamSurf_SRCS reparamSurfMain.cc Icosahedron_subdiv.cc
read_surface.cc reparamSurface.cc)

ADD_EXECUTABLE( reparamSurf ${reparamSurf_SRCS})

