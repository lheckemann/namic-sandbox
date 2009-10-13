PROJECT(HybridNav)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# --------------------------------------------------------------------------
# Find Slicer3

if(NOT Slicer3_SOURCE_DIR)
  find_package(Slicer3 REQUIRED)
  include(${Slicer3_USE_FILE})
  slicer3_set_default_install_prefix_for_external_projects()
endif(NOT Slicer3_SOURCE_DIR)


# --------------------------------------------------------------------------
# Include dirs

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
  )

#
# Configure include file
#
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkHybridNavConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkHybridNavConfigure.h
  )

file(GLOB headers 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  )

install(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkHybridNavConfigure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources

set(HybridNav_SRCS 
  vtkHybridNavLogic.cxx
  vtkHybridNavGUI.cxx
  vtkPivotCalibration.cxx
)


# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(HybridNav HybridNav_TCL_SRCS "${HybridNav_SRCS}" "")



#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(HybridNav_SRCS HybridNav.txt)

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name HybridNav)
add_library(${lib_name}
  ${HybridNav_SRCS} 
  ${HybridNav_TCL_SRCS}
  )

slicer3_set_modules_output_path(${lib_name})

target_link_libraries(${lib_name}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  )

install(TARGETS ${lib_name}
  RUNTIME DESTINATION ${Slicer3_INSTALL_MODULES_BIN_DIR} COMPONENT RuntimeLibraries 
  LIBRARY DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT Development
  )

slicer3_install_modules(${lib_name})
