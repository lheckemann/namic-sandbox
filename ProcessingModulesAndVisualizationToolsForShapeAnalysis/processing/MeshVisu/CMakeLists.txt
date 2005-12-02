
# Don't build anything unless the version of CMake is high enough.
# The matching ELSE/ENDIF should be the last lines in the file.
IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.2)

FIND_PACKAGE(OpenGL)

IF(OPENGL_INCLUDE_PATH)
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_PATH})
ENDIF(OPENGL_INCLUDE_PATH)

SET(SRC_CXX
AttrVisualizer.cxx
main.cxx
)

SET(APPLICATION_FLUIDS
 gui.fl
)

# Shorthand to source and binary directories
SET(MY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(MY_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

CONFIGURE_FILE(${MY_SOURCE_DIR}/pvalue.ppm ${EXECUTABLE_OUTPUT_PATH}/pvalue.ppm
COPYONLY)


# Include directories
INCLUDE_DIRECTORIES(
  ${MY_SOURCE_DIR}
  ${MY_BINARY_DIR}
  ${FLTK_INCLUDE_PATH}
  ${OPENGL_INCLUDE_PATH}
)


# Assign libraries to targets
LINK_LIBRARIES( 
  vtkHybrid
  ${VTK_LIBRARIES}
  ${FLTK_LIBRARIES}
  ${VTK_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${OPENGL_glu_LIBRARY}
  ${GLU_LIBRARY}
  SOVCommon
  SOVVTKRenderMethod
  SpatialObjectManager
  SOViewer

)

ADD_EXECUTABLE(MeshVisu ${SRC_CXX})

# FLTK Wrapping
FLTK_WRAP_UI(MeshVisu ${APPLICATION_FLUIDS})

# If the version of CMake was too old, complain and build nothing.
# These should be the last lines in this file.
ELSE("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.2)
  MESSAGE(STATUS
          "This version of CMake is too old to build this module.  "
          "Please upgrade to CMake 2.2.")
ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.2)
