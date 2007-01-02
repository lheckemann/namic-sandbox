PROJECT(WFTestClient)

SET(SRCS
WFTestClient.cxx
)


# Find the WFBaseEngine Library

#FIND_LIBRARY(WFENGINE_LIBRARY NAMES WFBaseEngine 
#  PATHS  ../bin /usr/lib /usr/local/lib)
#IF (WFENGINE_LIBRARY)
#  TARGET_ADD_LIBRARY (WFEConfigEditor WFENGINE_LIBRARY)
#ENDIF (WFENGINE_LIBRARY)

INCLUDE (${CMAKE_ROOT}/Modules/FindTCL.cmake)

INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)

## KWWidgets
FIND_PACKAGE(KWWidgets REQUIRED)
IF (NOT KWWidgets_FOUND)
  MESSAGE (FATAL_ERROR "Need KWWidgets to build")
ENDIF (NOT KWWidgets_FOUND)

INCLUDE(${KWWidgets_USE_FILE})

# This example uses some files from the KWWidgets distribution tree.
# Let's configure KWWidgets's vtkKWWidgetsPaths.h.in into our
# own header file so that we can find the paths to KWWidgets files.

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
CONFIGURE_FILE(
  ${KWWidgets_TEMPLATES_DIR}/vtkKWWidgetsPaths.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkKWWidgetsPaths.h)

ADD_EXECUTABLE(WFTestClient ${SRCS})
TARGET_LINK_LIBRARIES(WFTestClient ${KWWidgets_LIBRARIES})

