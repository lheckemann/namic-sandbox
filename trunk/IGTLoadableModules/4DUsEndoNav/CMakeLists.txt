PROJECT(FourDUsEndoNav)

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

# Chinzei; Next 1 line is to include here to find FindXX.cmake files.
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# --------------------------------------------------------------------------
# OpenCV stuff

# Chinzei; This does everything you want!
FIND_PACKAGE(OpenCV REQUIRED)

include_directories(
${OpenCV_INCLUDE_DIRS}
)

# --------------------------------------------------------------------------
# OpenIGTLink stuff

FIND_PACKAGE(OpenIGTLink REQUIRED)

# --------------------------------------------------------------------------
# Include dirs

FIND_PATH(OPENIGTLINK REQUIRED)
INCLUDE_DIRECTORIES(${OPENIGTLINK_INCLUDE_DIR})

#FIND_PATH(OPENIGTLINKIF REQUIRED)
#INCLUDE_DIRECTORIES(${OPENIGTLINKIF_INCLUDE_DIR})
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
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkFourDUsEndoNavConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkFourDUsEndoNavConfigure.h
  )

file(GLOB headers 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Wizard/*.h"
  )

install(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkFourDUsEndoNavConfigure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )


# --------------------------------------------------------------------------
# Sources

set(FourDUsEndoNav_SRCS 
  vtkFourDUsEndoNavLogic.cxx
  vtkFourDUsEndoNavGUI.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkFourDUsEndoNavStepBase.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkFourDUsEndoNavFirstStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkFourDUsEndoNavSecondStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkFourDUsEndoNavThirdStep.cxx
  )


# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(FourDUsEndoNav FourDUsEndoNav_TCL_SRCS "${FourDUsEndoNav_SRCS}" "")


#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(FourDUsEndoNav_SRCS FourDUsEndoNav.txt)


# --------------------------------------------------------------------------
# Build and install the library

set(lib_name FourDUsEndoNav)

add_library(${lib_name}
  ${FourDUsEndoNav_SRCS} 
  ${FourDUsEndoNav_TCL_SRCS}
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
  ${OpenIGTLinklib}
  Volumes
  ${OpenCV_CV_LIBRARY}
  ${OpenCV_CVAUX_LIBRARY}
#  ${OpenCV_CVCAM_LIBRARY}
  ${OpenCV_CVHAARTRAINING_LIBRARY}
  ${OpenCV_CXCORE_LIBRARY}
#  ${OpenCV_CXTS_LIBRARY} 
  ${OpenCV_HIGHGUI_LIBRARY}
  ${OpenCV_ML_LIBRARY}
#  ${OpenCV_TRS_LIBRARY}
  )


slicer3_install_modules(${lib_name})

