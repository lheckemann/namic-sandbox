PROJECT(LiveUltrasound)

#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.2)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# See http://cmake.org/cmake/help/cmake-2-8-docs.html#section_Policies for details
#-----------------------------------------------------------------------------
if(POLICY CMP0016)
  CMAKE_POLICY(SET CMP0016 NEW)
endif()
if(POLICY CMP0017)
  CMAKE_POLICY(SET CMP0017 OLD)
endif()

# Module name (usually identique to the directory name)
SET(EXTENSION_NAME ${PROJECT_NAME})

IF(NOT Slicer_SOURCE_DIR)

  SET(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Slicer4:Developers:Projects:QtSlicer/Tutorials/ExtensionWriting")
  SET(EXTENSION_CATEGORY "Examples")
  SET(EXTENSION_STATUS "Beta")
  SET(EXTENSION_DESCRIPTION "This is an example of Qt loadable module built as an extension")

  FIND_PACKAGE(Slicer REQUIRED)
  
  SET(ADDITIONAL_C_FLAGS "" CACHE STRING "Additional ${EXTENSION_NAME} C Flags")
  SET(ADDITIONAL_CXX_FLAGS "" CACHE STRING "Additional ${EXTENSION_NAME} CXX Flags")
  
  INCLUDE(${Slicer_USE_FILE})
  
  SET(EXTENSION_LICENSE_FILE ${Slicer_LICENSE_FILE})
  SET(EXTENSION_README_FILE ${Slicer_README_FILE})

  INCLUDE(SlicerEnableExtensionTesting)

  # ------------------------------------------------------------------------
  # Find Plus use file
  FIND_PACKAGE (OpenIGTLink REQUIRED PATHS "${OpenIGTLink_DIR}" NO_DEFAULT_PATH)
  IF ( OpenIGTLink_FOUND)
    INCLUDE( ${OpenIGTLink_USE_FILE} )
  ELSE( OpenIGTLink_FOUND )
    MESSAGE( FATAL_ERROR "This project requires Slicer with OpenIGTLink enabled. One of these components is missing. Please verify configuration")
  ENDIF( OpenIGTLink_FOUND )

ENDIF()

# Build module sub libraries
ADD_SUBDIRECTORY(MRML)
ADD_SUBDIRECTORY(Logic)

SET(qt_module_export_directive "Q_SLICER_QTMODULES_LIVEULTRASOUND_EXPORT")

# Additional includes - Current_{source,binary} and Slicer_{Libs,Base} already included
SET(qt_module_include_directories
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_BINARY_DIR}/Logic
  ${CMAKE_CURRENT_SOURCE_DIR}/MRML
  ${CMAKE_CURRENT_BINARY_DIR}/MRML
  ${CMAKE_CURRENT_SOURCE_DIR}/MRMLDisplayableManager
  ${CMAKE_CURRENT_BINARY_DIR}/MRMLDisplayableManager
  )

  # Displayable manager sources
SET(qt_module_displayable_manager_SRCS
  MRMLDisplayableManager/vtkMRMLLiveUltrasoundDisplayableManager.cxx
  MRMLDisplayableManager/vtkMRMLLiveUltrasoundDisplayableManager.h
  )
  
# Create the instantiator for DisplayableManager classes.
SET(VTK_USE_INSTANTIATOR_NEW 1)
VTK_MAKE_INSTANTIATOR3("${PROJECT_NAME}Instantiator"
  qt_module_displayable_manager_instantiator_SRCS
  "${qt_module_displayable_manager_SRCS}"
  "${qt_module_export_directive}"
  ${CMAKE_CURRENT_BINARY_DIR}
  "qSlicer${PROJECT_NAME}ModuleExport.h"
  )
  
# Source files
SET(qt_module_SRCS
  ${qt_module_displayable_manager_SRCS}
  ${qt_module_displayable_manager_instantiator_SRCS} 
  qSlicerLiveUltrasoundModule.cxx
  qSlicerLiveUltrasoundModule.h
  qSlicerLiveUltrasoundModuleWidget.cxx
  qSlicerLiveUltrasoundModuleWidget.h
  )

# Headers that should run through moc
SET(qt_module_MOC_SRCS
  qSlicerLiveUltrasoundModule.h
  qSlicerLiveUltrasoundModuleWidget.h
  )

# UI files
SET(qt_module_UI_SRCS
  Resources/UI/qSlicerLiveUltrasoundModule.ui
)

# Additional Target libraries
SET(qt_module_target_libraries
  vtkSlicerLiveUltrasoundModuleMRML
  vtkSlicerLiveUltrasoundModuleLogic
)

# Resources
SET(qt_module_resources
  Resources/qSlicerLiveUltrasoundModule.qrc
)

slicerMacroBuildQtModule(
  NAME ${EXTENSION_NAME}
  EXPORT_DIRECTIVE ${qt_module_export_directive}
  INCLUDE_DIRECTORIES ${qt_module_include_directories}
  SRCS ${qt_module_SRCS}
  MOC_SRCS ${qt_module_MOC_SRCS}
  UI_SRCS ${qt_module_UI_SRCS}
  TARGET_LIBRARIES ${qt_module_target_libraries}
  RESOURCES ${qt_module_resources}
  )


if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()

IF(NOT Slicer_SOURCE_DIR)
  INCLUDE(SlicerFunctionGenerateExtensionDescription)
  slicerFunctionGenerateExtensionDescription(
    EXTENSION_NAME ${EXTENSION_NAME}
    EXTENSION_CATEGORY ${EXTENSION_CATEGORY} 
    EXTENSION_STATUS ${EXTENSION_STATUS}
    EXTENSION_HOMEPAGE ${EXTENSION_HOMEPAGE}
    EXTENSION_DESCRIPTION ${EXTENSION_DESCRIPTION}
    DESTINATION_DIR ${CMAKE_CURRENT_BINARY_DIR}
    SLICER_WC_REVISION ${Slicer_WC_REVISION}
    SLICER_WC_ROOT ${Slicer_WC_ROOT}
    )
  INCLUDE(${Slicer_EXTENSION_CPACK})
ENDIF()
