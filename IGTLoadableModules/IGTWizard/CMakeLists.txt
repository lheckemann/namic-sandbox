PROJECT(IGTWizard)

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


#
# Include dirs
#


include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
  ${OpenIGTLinkIF_SOURCE_DIR}
  ${OpenIGTLinkIF_BINARY_DIR}
  ${OpenIGTLink_INCLUDE_DIRS}
  ${Volumes_SOURCE_DIR}
  ${Volumes_BINARY_DIR}
  )


#
# Configure include file
#
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkIGTWizardConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkIGTWizardConfigure.h
  )

file(GLOB headers 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Wizard/*.h"
  )

install(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkIGTWizardConfigure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )


# --------------------------------------------------------------------------
# Sources

set(IGTWizard_SRCS 
  vtkIGTWizardLogic.cxx
  vtkIGTWizardGUI.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkIGTWizardStepBase.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkIGTWizardFirstStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkIGTWizardSecondStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkIGTWizardThirdStep.cxx
  )


# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(IGTWizard IGTWizard_TCL_SRCS "${IGTWizard_SRCS}" "")


#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(IGTWizard_SRCS IGTWizard.txt)


# --------------------------------------------------------------------------
# Build and install the library

set(lib_name IGTWizard)

add_library(${lib_name}
  ${IGTWizard_SRCS} 
  ${IGTWizard_TCL_SRCS}
  )
slicer3_set_modules_output_path(${lib_name})

#link_directories(
#  ${Slicer3_DIR}/${Slicer3_INSTALL_MODULES_LIB_DIR}
#)


target_link_libraries(${lib_name}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  ${ITK_LIBRARIES}
  OpenIGTLinkIF
  Volumes
  )


slicer3_install_modules(${lib_name})

