project(FiberInteractiveSelection)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

 
# --------------------------------------------------------------------------
# Find Slicer3

if(NOT Slicer3_SOURCE_DIR)
  find_package(Slicer3 REQUIRED)
  include(${Slicer3_USE_FILE})
  slicer3_set_default_install_prefix_for_external_projects()
endif(NOT Slicer3_SOURCE_DIR)

# --------------------------------------------------------------------------
# Find VTK

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

# --------------------------------------------------------------------------
# Include dirs

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
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

set(FiberInteractiveSelection_SRCS
#    vtkCellPlaneLocator.cxx
    vtkLineBoxLocator.cxx
#    vtkExtractPolyDataGeometry2.cxx
#     vtkExtractLineInBox.cxx
  )


# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(FiberInteractiveSelection
  FiberInteractiveSelection_TCL_SRCS 
  "${FiberInteractiveSelection_SRCS}" "")
# --------------------------------------------------------------------------
# Build and install the library

set(lib_name FiberInteractiveSelection)
add_library(${lib_name}
  ${FiberInteractiveSelection_SRCS}
  ${FiberInteractiveSelection_TCL_SRCS}
  )

slicer3_set_modules_output_path(${lib_name})

target_link_libraries(${lib_name}
  #  ${Slicer3_Libs_LIBRARIES}
  # ${Slicer3_Base_LIBRARIES}
  ${VTK_LIBRARIES}
  # ${TCL_LIBRARIES}
  )


# Apply user-defined properties to the library target.
IF(Slicer3_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(${lib_name} PROPERTIES
    ${Slicer3_LIBRARY_PROPERTIES}
  )
ENDIF(Slicer3_LIBRARY_PROPERTIES)

slicer3_install_modules(${lib_name})

get_target_property(LIB_LOCATION ${lib_name} LOCATION)
get_filename_component( LIB_FILENAME ${LIB_LOCATION} NAME)

# --------------------------------------------------------------------------
# Copy over the Python files


if(Slicer3_USE_PYTHON)
#  #file(GLOB_RECURSE PYTHON_PLUGIN_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/Plugins" "Plugins/*.py")
#  file(GLOB_RECURSE PYTHON_PLUGIN_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.py")
#
#  foreach(file ${PYTHON_PLUGIN_FILES})
#    configure_file(
#      #${CMAKE_CURRENT_SOURCE_DIR}/Plugins/${file}
#      ${CMAKE_CURRENT_SOURCE_DIR}/${file}
#      ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_PLUGINS_LIB_DIR}/${file}
#      @ONLY
#    )
#    install(
#      FILES ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_PLUGINS_LIB_DIR}/${file}
#      DESTINATION ${Slicer3_INSTALL_PLUGINS_LIB_DIR}
#    )
#  endforeach(file {PYTHON_PLUGIN_FILES})


  file(GLOB PYTHONFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.py")

  foreach(file ${PYTHONFILES})
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/${file}
      ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_PLUGINS_BIN_DIR}/${file}
      @ONLY
    )
  endforeach(file)

  install(
    FILES ${PYTHONFILES}
    DESTINATION ${Slicer3_INSTALL_PLUGINS_BIN_DIR}
    )

#  file(GLOB PYTHONFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "StochasticTractographyServer/*.py")
#  foreach(pythonfile ${PYTHONFILES})
#    configure_file(
#      ${CMAKE_CURRENT_SOURCE_DIR}/${pythonfile}
#      ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_PLUGINS_BIN_DIR}/${pythonfile}
#      COPYONLY)
#  endforeach(pythonfile)
#
#  install(
#    FILES ${PYTHONFILES}
#    DESTINATION ${Slicer3_INSTALL_PLUGINS_BIN_DIR}/StochasticTractographyServer
#    )
#
#
#  file(GLOB PYTHONFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "StochasticTractographyServerMP/*.py")
#  foreach(pythonfile ${PYTHONFILES})
#    configure_file(
#      ${CMAKE_CURRENT_SOURCE_DIR}/${pythonfile}
#      ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_PLUGINS_BIN_DIR}/${pythonfile}
#      COPYONLY)
#  endforeach(pythonfile)
#
#  install(
#    FILES ${PYTHONFILES}
#    DESTINATION ${Slicer3_INSTALL_PLUGINS_BIN_DIR}/StochasticTractographyServerMP
#    )


endif(Slicer3_USE_PYTHON)
