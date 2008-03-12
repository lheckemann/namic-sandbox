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

# Bring the macros

include(SlicerParseModule)
include(SlicerEnableModule)
include(SlicerDownloadModule)

# Parse a module

slicer_parse_module_file(
  "${SLICER_TEST_MODULES_PATH}/TestModule1/TestModule1.xml" TestModule1)

slicer_get_module_value(TestModule1 Name name)
slicer_get_module_value(TestModule1 Dependency deps)
slicer_get_module_value(TestModule1 Author authors)

#MESSAGE("Name: ${name}")
#MESSAGE("Dependency: ${deps}")
#MESSAGE("Author: ${authors}")

slicer_create_use_module_option(TestModule1 USE_TESTMODULE1)

slicer_create_download_module_target(TestModule1 M1_download "${CMAKE_CURRENT_BINARY_DIR}/Modules/TestModule1")
slicer_create_update_module_target(TestModule1 M1_update "${CMAKE_CURRENT_BINARY_DIR}/Modules/TestModule1")

# Another one

slicer_parse_module_file("${SLICER_TEST_MODULES_PATH}/TestModule2/" TestModule2)

slicer_create_download_module_target(TestModule2 M2_download "${CMAKE_CURRENT_BINARY_DIR}/Modules/TestModule2")
slicer_create_update_module_target(TestModule2 M2_update "${CMAKE_CURRENT_BINARY_DIR}/Modules/TestModule2")

#slicer_get_parsed_modules_list(foo)
#message("list: ${foo}")
