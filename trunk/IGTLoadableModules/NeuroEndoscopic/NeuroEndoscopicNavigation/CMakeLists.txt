PROJECT(NeuroEndoscopicNavigation)

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
# Find OpenIGTLink

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

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
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkNeuroEndoscopicNavigationConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkNeuroEndoscopicNavigationConfigure.h
  )

file(GLOB headers 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  )

install(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkNeuroEndoscopicNavigationConfigure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources

set(NeuroEndoscopicNavigation_SRCS 
  vtkNeuroEndoscopicNavigationLogic.cxx
  vtkNeuroEndoscopicNavigationGUI.cxx
)


# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(NeuroEndoscopicNavigation NeuroEndoscopicNavigation_TCL_SRCS "${NeuroEndoscopicNavigation_SRCS}" "")



#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(NeuroEndoscopicNavigation_SRCS NeuroEndoscopicNavigation.txt)

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name NeuroEndoscopicNavigation)
add_library(${lib_name}
  ${NeuroEndoscopicNavigation_SRCS} 
  ${NeuroEndoscopicNavigation_TCL_SRCS}
  )

slicer3_set_modules_output_path(${lib_name})

target_link_libraries(${lib_name}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  OpenIGTLink 
  )

install(TARGETS ${lib_name}
  RUNTIME DESTINATION ${Slicer3_INSTALL_MODULES_BIN_DIR} COMPONENT RuntimeLibraries 
  LIBRARY DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT Development
  )

slicer3_install_modules(${lib_name})
