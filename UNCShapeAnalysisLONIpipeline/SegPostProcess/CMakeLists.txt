PROJECT( SegPostProcess ) 

# Include itk
INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake)
INCLUDE(${USE_ITK_FILE})  


# Include OpenGL
INCLUDE (${CMAKE_ROOT}/Modules/FindOpenGL.cmake)

# add the  fltk and open gl libs & includes 
INCLUDE_DIRECTORIES( 
${OPENGL_INCLUDE_PATH} 
)

LINK_LIBRARIES ( 
${OPENGL_LIBRARY} 
)

ADD_EXECUTABLE(  SegPostProcess SegPostProcess.cxx ccl.c)

TARGET_LINK_LIBRARIES ( SegPostProcess ${ITK_LIBRARIES} )
