PROJECT(OsteoPlan)

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
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard
  ${CMAKE_CURRENT_SOURCE_DIR}/Widgets
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
#  ${VTK_CMAKE_DIR}/../../vtkBioeng
#  /projects/igtdev/lchauvin/Slicer3-lib/vtkBioeng
  )

#
# Configure include file
#
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkOsteoPlanConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkOsteoPlanConfigure.h
  )


file(GLOB headers 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Wizard/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Widgets/*.h"
  )

install(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkOsteoPlanConfigure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources

set(OsteoPlan_SRCS 
  vtkOsteoPlanLogic.cxx
  vtkOsteoPlanGUI.cxx
  vtkMRMLOsteoPlanNode.cxx  
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkOsteoPlanStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkOsteoPlanCuttingModelStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkOsteoPlanSelectingPartsStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkOsteoPlanMovingPartsStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkOsteoPlanPlacingScrewsStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkOsteoPlanReturningOriginalPositionStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkOsteoPlanDistractorStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkDistractorDefinition.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Widgets/vtkCylinderWidget.cxx
)


# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(OsteoPlan OsteoPlan_TCL_SRCS "${OsteoPlan_SRCS}" "")



#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(OsteoPlan_SRCS OsteoPlan.txt)

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name OsteoPlan)
add_library(${lib_name}
  ${OsteoPlan_SRCS} 
  ${OsteoPlan_TCL_SRCS}
  )

slicer3_set_modules_output_path(${lib_name})

target_link_libraries(${lib_name}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
 # vtkBioeng
  )

install(TARGETS ${lib_name}
  RUNTIME DESTINATION ${Slicer3_INSTALL_MODULES_BIN_DIR} COMPONENT RuntimeLibraries 
  LIBRARY DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT Development
  )

slicer3_install_modules(${lib_name})
