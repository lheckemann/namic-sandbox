# Generate the MultiImageRegistrationConfig.cmake file in the build tree and configure 
# one the installation tree.

# Settings specific to build trees
#
#
set(MultiImageRegistration_USE_FILE_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/UseMultiImageRegistration.cmake)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/UseMultiImageRegistration.cmake.in
  ${MultiImageRegistration_USE_FILE_CONFIG}
  @ONLY IMMEDIATE)

set(MultiImageRegistration_INCLUDE_DIRS_CONFIG ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Source ${CMAKE_CURRENT_SOURCE_DIR}/Source/Common )
set(MultiImageRegistration_LIBRARY_DIRS_CONFIG ${CMAKE_CURRENT_BINARY_DIR})
set(ITK_DIR_CONFIG ${ITK_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/MultiImageRegistrationConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/MultiImageRegistrationConfig.cmake @ONLY IMMEDIATE)

# Settings specific for installation trees
#
#
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/UseMultiImageRegistration.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/UseMultiImageRegistration.cmake_install
  @ONLY IMMEDIATE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/MultiImageRegistrationInstallConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/MultiImageRegistrationConfig.cmake_install @ONLY IMMEDIATE)
