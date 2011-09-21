PROJECT(Ultrasound4D)

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


  find_package(OpenIGTLink REQUIRED)
  include(${OpenIGTLink_USE_FILE})

# --------------------------------------------------------------------------
# Include dirs

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
  ${OpenIGTLinkIF_SOURCE_DIR}
  ${OpenIGTLinkIF_BINARY_DIR}
  ${OpenIGTLink_INCLUDE_DIRS}
  )

#
# Configure include file
#
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkUltrasound4DConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkUltrasound4DConfigure.h
  )

file(GLOB headers 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  )

install(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkUltrasound4DConfigure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources

set(Ultrasound4D_SRCS 
  vtkUltrasound4DLogic.cxx
  vtkUltrasound4DGUI.cxx
  vtkMRML4DVolumeNode.cxx
)


# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(Ultrasound4D Ultrasound4D_TCL_SRCS "${Ultrasound4D_SRCS}" "")



#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(Ultrasound4D_SRCS Ultrasound4D.txt)

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name Ultrasound4D)
add_library(${lib_name}
  ${Ultrasound4D_SRCS} 
  ${Ultrasound4D_TCL_SRCS}
  )

slicer3_set_modules_output_path(${lib_name})

target_link_libraries(${lib_name}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  OpenIGTLinkIF
  )

install(TARGETS ${lib_name}
  RUNTIME DESTINATION ${Slicer3_INSTALL_MODULES_BIN_DIR} COMPONENT RuntimeLibraries 
  LIBRARY DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT Development
  )

slicer3_install_modules(${lib_name})
