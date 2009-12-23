PROJECT(SbiaVolumes)

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

#we alse need to include the orig Volumes Module
#it should have a vtkVolumes.h in it
FIND_PATH(SLICER_VOLUMES_INCLUDE_DIR vtkVolumes.h
  ${Slicer3_DIR}/include/Slicer3/Modules/Volumes 
  ${Slicer3_DIR}/../Slicer3/Modules/Volumes )

#if Slicer3_DIR is an install dir then SLICER_VOLUMES_INCLUDE_DIR
#also has vtkVolumesConfigure.h in it 
FIND_PATH(ALT_SLICER_VOLUMES_INCLUDE_DIR vtkVolumesConfigure.h
  ${SLICER_VOLUMES_INCLUDE_DIR}
  ${Slicer3_DIR}/include/Slicer3/Modules/Volumes 
  ${Slicer3_DIR}/Modules/Volumes )

#TODO it would be awesome if Slicer3Config set these module includes correctly


include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
  ${SLICER_VOLUMES_INCLUDE_DIR}
  ${ALT_SLICER_VOLUMES_INCLUDE_DIR}
  )

#
# Configure include file
#
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkSbiaVolumesConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkSbiaVolumesConfigure.h
  )
#
# Configure orig Volumes configure file include file
#
#~ configure_file(
  #~ ${SLICER_VOLUMES_SRC_DIR}/vtkVolumesConfigure.h.in 
  #~ ${CMAKE_CURRENT_BINARY_DIR}/vtkVolumesConfigure.h
  #~ )

file(GLOB headers 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  )

install(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkSbiaVolumesConfigure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources

set(SbiaVolumes_SRCS 
  vtkSbiaVolumesLogic.cxx
  vtkSbiaVolumesGUI.cxx
  vtkSlicerDiffusionRSHVolumeGlyphDisplayWidget.cxx
  vtkSlicerDiffusionRSHVolumeDisplayWidget.cxx
  vtkSlicerDiffusionRSHGlyphDisplayWidget.cxx
)


# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(SbiaVolumes SbiaVolumes_TCL_SRCS "${SbiaVolumes_SRCS}" "")



#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(SbiaVolumes_SRCS SbiaVolumes.txt)

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name SbiaVolumes)
add_library(${lib_name}
  ${SbiaVolumes_SRCS} 
  ${SbiaVolumes_TCL_SRCS}
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
