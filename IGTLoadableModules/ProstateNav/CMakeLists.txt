PROJECT(ProstateNav)

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
# Find Open IGT Link

if(NOT OpenIGTLink_SOURCE_DIR)
  find_package(OpenIGTLink REQUIRED)
  include(${OpenIGTLink_USE_FILE})
endif(NOT OpenIGTLink_SOURCE_DIR)


# --------------------------------------------------------------------------
# Include dirs


#
# Include dirs
#

set(OpenIGTLinkIF_SOURCE_DIR ../../Slicer3/Modules/OpenIGTLinkIF)
set(OpenIGTLinkIF_BUILD_DIR ../../Slicer3-build/Modules/OpenIGTLinkIF)


include_directories(
  ${OpenIGTLinkIF_SOURCE_DIR}
  ${OpenIGTLinkIF_BUILD_DIR}
  ${OpenIGTLink_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
  )

#
# Configure include file
#
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkProstateNavConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkProstateNavConfigure.h
  )

file(GLOB headers 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Wizard/*.h"
  )

# --------------------------------------------------------------------------
# Sources

set(ProstateNav_SRCS 
  vtkProstateNavLogic.cxx
  vtkProstateNavGUI.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkProstateNavStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkProstateNavConfigurationStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkProstateNavScanControlStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkProstateNavCalibrationStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkProstateNavTargetingStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkProstateNavManualControlStep.cxx
  )


# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(ProstateNav ProstateNav_TCL_SRCS "${ProstateNav_SRCS}" "")


#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(ProstateNav_SRCS ProstateNav.txt)


# --------------------------------------------------------------------------
# Build and install the library

set(lib_name ProstateNav)

add_library(${lib_name}
  ${ProstateNav_SRCS} 
  ${ProstateNav_TCL_SRCS}
  )


target_link_libraries(${lib_name}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  ${ITK_LIBRARIES}
  OpenIGTLinkIF.so
  )

install(TARGETS ${lib_name}
  RUNTIME DESTINATION ${Slicer3_INSTALL_MODULES_BIN_DIR} COMPONENT RuntimeLibraries 
  LIBRARY DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT Development
  )
