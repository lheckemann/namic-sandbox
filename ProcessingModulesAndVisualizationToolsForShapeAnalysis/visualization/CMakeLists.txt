############################################################################################
# KWWidgets version
############################################################################################

FIND_PACKAGE(KWWidgets)
IF (KWWidgets_FOUND)
  INCLUDE(${KWWidgets_USE_FILE})

  SET(LIB_NAME "vtkKWMeshVisuLib")
  SET(LIB_SRCS "vtkKWMeshVisu.cxx")
  SET(LIB_EXTRA_SRCS "itkMeshTovtkPolyData.cxx")


  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
  INCLUDE("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
  IF("${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}" EQUAL "5.0") # VTK 5.0 compat
    SET(VTK_WRAP_TCL3_INIT_DIR "${VTK_SOURCE_DIR}/Wrapping")
  ENDIF("${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}" EQUAL "5.0")
  VTK_WRAP_TCL3(${LIB_NAME} LIB_TCL_SRCS ${LIB_SRCS} "")

  # Create the library (in static mode for convenience).

  ADD_LIBRARY(${LIB_NAME} STATIC ${LIB_TCL_SRCS} ${LIB_SRCS} ${LIB_EXTRA_SRCS})

  # The name of our executable and the corresponding source file.

  SET(EXE_NAME "KWMeshVisu")
  SET(EXE_SRCS "${EXE_NAME}.cxx")

  # Create the executable, and link it against the KWWidgets libraries and our
  # own library.

  ADD_EXECUTABLE(${EXE_NAME} WIN32 ${EXE_SRCS})
  TARGET_LINK_LIBRARIES(${EXE_NAME} ${KWWidgets_LIBRARIES} ${LIB_NAME})
ENDIF (KWWidgets_FOUND)
