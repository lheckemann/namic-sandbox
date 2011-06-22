PROJECT(vtkSlicer${EXTENSION_NAME}ModuleMRML)

#
# See CMake/SlicerMacroBuildModuleMRML.cmake for details
#

SET(module_mrml_name "${PROJECT_NAME}")

# The header '${module_mrml_name}Export.h' will be automatically configured.
SET(module_mrml_export_directive "VTK_SLICER_LIVEULTRASOUND_MODULE_MRML_EXPORT")
# Additional directories to include
SET(module_mrml_include_directories
  ${OpenIGTLink_SOURCE_DIR}
  ${OpenIGTLink_BINARY_DIR}
  ${Slicer_ModuleLogic_INCLUDE_DIRS}
  )

# Source files
SET(module_mrml_SRCS
  vtkMRMLLiveUltrasoundNode.cxx
  vtkMRMLLiveUltrasoundNode.h
  )

# Additional Target libraries
SET(module_mrml_target_libraries
  ${CTK_LIBRARIES}
  ${ITK_LIBRARIES}
  ${QT_LIBRARIES}
  ${VTK_LIBRARIES}
  ${MRML_LIBRARIES}
  ${OpenIGTLink_LIBRARIES}
  OpenIGTLinkIFModuleLogic
  )

SlicerMacroBuildModuleMRML(
  NAME ${module_mrml_name}
  EXPORT_DIRECTIVE ${module_mrml_export_directive}
  INCLUDE_DIRECTORIES ${module_mrml_include_directories}
  SRCS ${module_mrml_SRCS}
  TARGET_LIBRARIES ${module_mrml_target_libraries}
  DISABLE_WRAP_PYTHON
  )
