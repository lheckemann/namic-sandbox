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

set(SLICER_LOCAL_MODULES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Modules")
set(SLICER_DOWNLOADED_SOURCES_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Sources")

# Bring the macros

include(SlicerParseModule)
include(SlicerEnableModule)
include(SlicerDownloadModule)

# Parse all modules subdirs in a dir

slicer_parse_modules_directory("${SLICER_LOCAL_MODULES_PATH}")

# Parse a remote module

#slicer_parse_module_url("http://www.na-mic.org/ViewVC/index.cgi/trunk/BuildSystem/Modules/TestModule1/TestModule1.xml?root=NAMICSandBox&view=co")

slicer_is_module_unknown(TestModule1 unknown)
if(NOT unknown)
  #slicer_get_module_value(TestModule1 Name name)
  #slicer_get_module_value(TestModule1 Dependency deps)
  #slicer_get_module_value(TestModule1 Author authors)
  #message("Name: ${name}")
  #message("Dependency: ${deps}")
  #message("Author: ${authors}")
  #slicer_create_use_module_option(TestModule1 USE_TESTMODULE1)
endif(NOT unknown)

# Parse a local module

slicer_parse_module_file("${SLICER_LOCAL_MODULES_PATH}/TestModule2/")

# Create USE_FOOBAR options for all modules

slicer_get_parsed_modules(modules)
slicer_create_use_modules_options("${modules}")
slicer_get_used_modules("${modules}" used_modules)

# Find the unresolved dependencies, and the resolved modules

slicer_get_unresolved_modules_dependencies(
  "${used_modules}" unresolved_dependencies)
slicer_get_resolved_modules(
  "${used_modules}" resolved_modules)

# Create all download/update targets for the resolved modules

slicer_create_download_and_update_modules_targets(
  "${resolved_modules}" "${SLICER_DOWNLOADED_SOURCES_DIRECTORY}")

message("used: ${used_modules}\n\nunresolved dependencies: ${unresolved_dependencies}\n\nresolved modules: ${resolved_modules}")
