cmake_minimum_required(VERSION 2.5)
project(BuildSystem)

# We are not building anything in this pass

mark_as_advanced(
  CMAKE_BACKWARDS_COMPATIBILITY
  CMAKE_BUILD_TYPE
  CMAKE_INSTALL_PREFIX
  EXECUTABLE_OUTPUT_PATH
  LIBRARY_OUTPUT_PATH
  )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
set(SLICER_TEST_MODULES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Modules")
set(SLICER_MODULES_SOURCE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Sources")

# Bring the macros

include(SlicerParseModule)
include(SlicerEnableModule)
include(SlicerDownloadModule)

# Parse core modules

foreach(module ITK KWWidgets Slicer3 Teem VTK)
  slicer_parse_module_file("${SLICER_TEST_MODULES_PATH}/${module}")
endforeach(module)

# Parse test modules

slicer_parse_module_file("${SLICER_TEST_MODULES_PATH}/TestModule1")

#slicer_get_module_value(TestModule1 Name name)
#slicer_get_module_value(TestModule1 Dependency deps)
#slicer_get_module_value(TestModule1 Author authors)
#message("Name: ${name}")
#message("Dependency: ${deps}")
#message("Author: ${authors}")
#slicer_create_use_module_option(TestModule1 USE_TESTMODULE1)

slicer_parse_module_file("${SLICER_TEST_MODULES_PATH}/TestModule2/")

# Create all download/update targets
# "make update" will download everything

slicer_create_download_and_update_modules_targets(
  ${SLICER_MODULES_SOURCE_DIRECTORY})
