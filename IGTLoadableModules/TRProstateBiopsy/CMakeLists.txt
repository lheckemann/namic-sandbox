PROJECT(TRProstateBiopsy)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

INCLUDE(${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)

IF(NOT USE_IGSTK)
 ADD_DEFINITIONS(-DIGSTK_OFF)
ENDIF(NOT USE_IGSTK)

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
INCLUDE_DIRECTORIES(
  ${ModulesBaseClasses_SOURCE_DIR}
  ${ModulesBaseClasses_BINARY_DIR}
  ${TRProstateBiopsy_SOURCE_DIR}
  ${TRProstateBiopsy_BINARY_DIR}
  ${TRProstateBiopsy_SOURCE_DIR}/Wizard
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
  ${SlicerBase_SOURCE_DIR}
  ${SlicerBase_BINARY_DIR}
  ${SlicerBaseLogic_SOURCE_DIR}
  ${SlicerBaseLogic_BINARY_DIR}
  ${SlicerBaseGUI_SOURCE_DIR}
  ${SlicerBaseGUI_BINARY_DIR}
  ${vtkITK_SOURCE_DIR}
  ${vtkITK_BINARY_DIR}
  ${ITK_INCLUDE_DIR}
  ${VTK_INCLUDE_DIR}
  ${MRML_SOURCE_DIR}
  ${MRML_BINARY_DIR}
  ${IGT_SOURCE_DIR}
  ${IGT_BINARY_DIR}
  ${NAVITRACK_INC}
  ${IGSTK_INCLUDE_DIRS}
  )

# --------------------------------------------------------------------------
# Sources
#
SET(TRProstateBiopsy_SRCS 
  # module
  vtkTRProstateBiopsyLogic.cxx
  vtkTRProstateBiopsyGUI.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkTRProstateBiopsyStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkTRProstateBiopsyCalibrationStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkTRProstateBiopsyTargetingStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkTRProstateBiopsyVerificationStep.cxx
  )

# --------------------------------------------------------------------------
# Wrapping
#
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
VTK_WRAP_TCL3(TRProstateBiopsy TRProstateBiopsy_TCL_SRCS
              "${TRProstateBiopsy_SRCS}" "")

#---------------------------------------------------------------------------
# Add Loadable Module support
#
generatelm(TRProstateBiopsy_SRCS TRProstateBiopsy.txt)

# --------------------------------------------------------------------------
# Build and install the library
#
ADD_LIBRARY(TRProstateBiopsy
  ${TRProstateBiopsy_SRCS} 
  ${TRProstateBiopsy_TCL_SRCS}
  )

SLICER3_SET_MODULES_OUTPUT_PATH(TRProstateBiopsy)

TARGET_LINK_LIBRARIES(TRProstateBiopsy
  SlicerBaseLogic 
  SlicerBaseGUI 
  MRML
  ${ITK_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  vtkITK
  vtkCommonTCL
  vtkImagingTCL
  vtkFilteringTCL
  vtkIOTCL
  vtkHybridTCL
  IGT
  ${IGSTK_LIB}
  )

SLICER3_INSTALL_MODULES(TRProstateBiopsy)

# --------------------------------------------------------------------------
# Configure include file
#
CONFIGURE_FILE(
  ${TRProstateBiopsy_SOURCE_DIR}/vtkTRProstateBiopsyConfigure.h.in 
  ${TRProstateBiopsy_BINARY_DIR}/vtkTRProstateBiopsyConfigure.h
  )
