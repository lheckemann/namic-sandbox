project(TRProstateBiopsy)

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
  ${CMAKE_CURRENT_BINARY_DIR}/Wizard
  ${CMAKE_CURRENT_SOURCE_DIR}/Segmentation
  ${CMAKE_CURRENT_BINARY_DIR}/Segmentation
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
)

# --------------------------------------------------------------------------
# Configure include file

configure_file(
  ${TRProstateBiopsy_SOURCE_DIR}/vtkTRProstateBiopsyConfigure.h.in 
  ${TRProstateBiopsy_BINARY_DIR}/vtkTRProstateBiopsyConfigure.h
  )

# --------------------------------------------------------------------------
# Sources

set(TRProstateBiopsy_SRCS
  # module
  vtkTRProstateBiopsyLogic.cxx
  vtkTRProstateBiopsyGUI.cxx
  vtkTRProstateBiopsyMath.cxx
  vtkTRProstateBiopsyUSBOpticalEncoder.cxx
  vtkTRProstateBiopsyTargetDescriptor.cxx
  vtkTRProstateBiopsyCalibrationAlgo.cxx
  vtkTRProstateBiopsyConfigFile.cxx
  vtkTRProstateBiopsyRobotWidget.cxx
  vtkTRProstateBiopsyViewerWidget.cxx
  vtkMRMLTRProstateBiopsyModuleNode.cxx    
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkTRProstateBiopsyStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkTRProstateBiopsyCalibrationStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkTRProstateBiopsySegmentationStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkTRProstateBiopsyTargetingStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkTRProstateBiopsyVerificationStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Segmentation/randomWalkSeg.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Segmentation/cVolume.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Segmentation/cVolOp.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Segmentation/spdEqSolver.cxx
  )

if(WIN32)
  set( TRProstateBiopsy_SRCS ${TRProstateBiopsy_SRCS} 
  vtkTRProstateBiopsySecondaryMonitor.cxx )
endif(WIN32)

# Helper classes

set_source_files_properties(
  ${CMAKE_CURRENT_SOURCE_DIR}/Segmentation/randomWalkSeg.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Segmentation/cVolume.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Segmentation/cVolOp.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Segmentation/spdEqSolver.cxx
  WRAP_EXCLUDE
  )

# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(TRProstateBiopsy TRProstateBiopsy_TCL_SRCS
              "${TRProstateBiopsy_SRCS}" "")

#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(TRProstateBiopsy_SRCS TRProstateBiopsy.txt)

# --------------------------------------------------------------------------
# Build and install the library

set(LIB_NAME TRProstateBiopsy)

add_library(${LIB_NAME}
  ${TRProstateBiopsy_SRCS} 
  ${TRProstateBiopsy_TCL_SRCS}
  )

slicer3_set_modules_output_path(${LIB_NAME})

target_link_libraries(${LIB_NAME}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  ${ITK_LIBRARIES}
  )

slicer3_install_modules(${LIB_NAME})

