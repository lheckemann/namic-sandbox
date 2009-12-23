PROJECT(SbiaHardiSlicerIntegration)
SET(SbiaHardiSlicerIntegration_VERSION 0.1)
cmake_minimum_required(VERSION 2.6)
cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


# Include dirs
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Lib
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
  )

file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(FILES
  ${headers}
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources

set(SbiaHardiSlicerIntegration_SRCS
  vtkMRMLDiffusionRSHVolumeNode.cxx
  vtkMRMLDiffusionRSHVolumeDisplayNode.cxx
  vtkMRMLDiffusionRSHVolumeSliceDisplayNode.cxx
  vtkMRMLDiffusionRSHDisplayPropertiesNode.cxx
  vtkDiffusionRSHMathematicsSimple.cxx
  vtkDiffusionRSHGlyph.cxx
  vtkSbiaSphereSource.cxx
  )


# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(SbiaHardiSlicerIntegration
  SbiaHardiSlicerIntegration_TCL_SRCS
  "${SbiaHardiSlicerIntegration_SRCS}" "")

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name SbiaHardiSlicerIntegration)
add_library(${lib_name}
  ${SbiaHardiSlicerIntegration_SRCS}
  ${SbiaHardiSlicerIntegration_TCL_SRCS}
  ${SpecialFunctions_SOURCES}
  )
slicer3_set_modules_output_path(${lib_name})

target_link_libraries(${lib_name}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${VTK_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  ${ITK_LIBRARIES}
  )


slicer3_install_modules(${lib_name})

get_target_property(LIB_LOCATION ${lib_name} LOCATION)
get_filename_component( LIB_FILENAME ${LIB_LOCATION} NAME)

# --------------------------------------------------------------------------
# Copy over the Python files

if(Slicer3_USE_PYTHON)

  file(GLOB_RECURSE PYTHON_PLUGIN_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/Plugins" "Plugins/*.py")

  foreach(file ${PYTHON_PLUGIN_FILES})
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/Plugins/${file}
      ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_PLUGINS_LIB_DIR}/${file}
      @ONLY
      )
    install(
      FILES ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_PLUGINS_LIB_DIR}/${file}
      DESTINATION ${Slicer3_INSTALL_PLUGINS_LIB_DIR}
      )
  endforeach(file {PYTHON_PLUGIN_FILES})

endif(Slicer3_USE_PYTHON)


subdirs(sbiaVolumes)

# --------------------------------------------------------------------------
# Testing

if(BUILD_TEST)
  subdirs(Testing)
endif(BUILD_TEST)


