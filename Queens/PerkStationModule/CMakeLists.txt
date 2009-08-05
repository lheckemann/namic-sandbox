project(PerkStationModule)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

INCLUDE(${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)

# --------------------------------------------------------------------------
# Find Slicer3

if(NOT Slicer3_SOURCE_DIR)
  find_package(Slicer3 REQUIRED)
  include(${Slicer3_USE_FILE})
  slicer3_set_default_install_prefix_for_external_projects()
endif(NOT Slicer3_SOURCE_DIR)

# --------------------------------------------------------------------------
# Find NDI  tracking stuff
option(BUILD_USING_NDIOAPI "Build Perk station with NDI tracking support." ON)
if(BUILD_USING_NDIOAPI)
    FIND_PATH(NDIOAPI_INCLUDE_DIR ndopto.h)
    FIND_LIBRARY(NDIOAPI_LIBRARY oapi)
    SUBDIRS (
    ndicapi
    )   
endif(BUILD_USING_NDIOAPI)
# --------------------------------------------------------------------------
# Include dirs

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
   )

if(BUILD_USING_NDIOAPI) 
    include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Tracking
    ${CMAKE_CURRENT_SOURCE_DIR}/ndicapi
    ${NDIOAPI_INCLUDE_DIR}
    ${NDICAPI_BINARY_DIR}
    ${NDIOAPI_INCLUDE_DIR}
    )   
endif(BUILD_USING_NDIOAPI)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkPerkStationModuleConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkPerkStationModuleConfigure.h
)
file(GLOB headers 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Wizard/*.h"
   )
install(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkPerkStationModuleConfigure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources

SET(PerkStationModule_SRCS
  vtkPerkStationModuleLogic.cxx
  vtkPerkStationModuleGUI.cxx
  vtkMRMLPerkStationModuleNode.cxx
  vtkPerkStationSecondaryMonitor.cxx
   # wizard
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkPerkStationStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkPerkStationCalibrateStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkPerkStationPlanStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkPerkStationInsertStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkPerkStationValidateStep.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/Wizard/vtkPerkStationEvaluateStep.cxx  
)

if(BUILD_USING_NDIOAPI)
 set(PerkStationModule_SRCS 
    ${PerkStationModule_SRCS} 
     #  tracking
    ${CMAKE_CURRENT_SOURCE_DIR}/Tracking/vtkTracker.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/Tracking/vtkTrackerTool.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/Tracking/vtkTrackerBuffer.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/Tracking/vtkFrameToTimeConverter.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/Tracking/vtkNDITracker.cxx
    )
endif(BUILD_USING_NDIOAPI)

# Abstract/pure virtual classes

#SET_SOURCE_FILES_PROPERTIES(
  #vtkModule.cxx
  #PROPERTIES
  #ABSTRACT "TRUE"
  #)

# Helper classes

#SET_SOURCE_FILES_PROPERTIES(
  #vtkModule.cxx
  #PROPERTIES
  #WRAP_EXCLUDE "TRUE"
  #)


# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(PerkStationModule PerkStationModule_TCL_SRCS "${PerkStationModule_SRCS}" "")

#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(PerkStationModule_SRCS PerkStationModule.txt)

# --------------------------------------------------------------------------
# Build and install the library
SET(Tracking_LIBS)

if(BUILD_USING_NDIOAPI)
SET (Tracking_LIBS ndicapi ${NDIOAPI_LIBRARY})
endif(BUILD_USING_NDIOAPI)

set(lib_name PerkStationModule)
add_library(${lib_name} 
  ${PerkStationModule_SRCS}
  ${PerkStationModule_TCL_SRCS}
  
  )
slicer3_set_modules_output_path(${lib_name})

target_link_libraries(${lib_name}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES} 
  ${ITK_LIBRARIES}
  ${Tracking_LIBS}
  )

slicer3_install_modules(${lib_name})

# --------------------------------------------------------------------------
# Testing (requires some of the examples)

#IF(BUILD_TESTING)
 # SUBDIRS(Testing)
#ENDIF(BUILD_TESTING)
