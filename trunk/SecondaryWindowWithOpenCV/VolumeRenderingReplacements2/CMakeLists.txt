project(VolumeRenderingReplacements2)

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

configure_file(
  ${VolumeRenderingReplacements2_SOURCE_DIR}/vtkVolumeRenderingReplacements2Configure.h.in 
  ${VolumeRenderingReplacements2_BINARY_DIR}/vtkVolumeRenderingReplacements2Configure.h
  )

file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkVolumeRenderingReplacements2Configure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources

set(VolumeRenderingReplacements2_SRCS
  vtkSlicerVolumeTextureMapper3D2.cxx
  vtkSlicerOpenGLVolumeTextureMapper3D2.cxx
  vtkSlicerVolumeRenderingFactory2.cxx
  #GPU Ray Cast stuff
  vtkSlicerGPURayCastVolumeMapper2.cxx
  vtkSlicerGPUVolumeMapper2.cxx
  vtkSlicerGPURayCastVolumeTextureMapper3D2.cxx
  vtkSlicerGPUVolumeTextureMapper3D2.cxx
  #Ray Cast stuff
  vtkSlicerFixedPointRayCastImage2.cxx
  vtkSlicerFixedPointVolumeRayCastCompositeGOHelper2.cxx
  vtkSlicerFixedPointVolumeRayCastCompositeGOShadeHelper2.cxx
  vtkSlicerFixedPointVolumeRayCastCompositeHelper2.cxx
  vtkSlicerFixedPointVolumeRayCastCompositeShadeHelper2.cxx
  vtkSlicerFixedPointVolumeRayCastHelper2.cxx
  vtkSlicerFixedPointVolumeRayCastMapper2.cxx
  vtkSlicerFixedPointVolumeRayCastMIPHelper2.cxx
  vtkSlicerRayCastImageDisplayHelper2.cxx
  vtkSlicerOpenGLRayCastImageDisplayHelper2.cxx
  #GUI
  #vtkSlicerBoxWidget.cxx
  vtkSlicerVolumePropertyWidget2.cxx
  #vtkSlicerBoxWidget2.cxx
  #vtkSlicerBoxRepresentation.cxx
  )

# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(VolumeRenderingReplacements2 
  VolumeRenderingReplacements2_TCL_SRCS 
  "${VolumeRenderingReplacements2_SRCS}" "")

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name VolumeRenderingReplacements2)
add_library(${lib_name}
  ${VolumeRenderingReplacements2_SRCS}
  ${VolumeRenderingReplacements2_TCL_SRCS}
  )
slicer3_set_modules_output_path(${lib_name})

target_link_libraries(${lib_name}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  ${ITK_LIBRARIES}
  )

slicer3_install_modules(${lib_name})
