# Generate the BRAINSCommonLib2DConfig.cmake file in the build tree and configure 
# one the installation tree.

# Settings specific to build trees
#
#
set(BRAINSCommonLib2D_USE_FILE_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/UseBRAINSCommonLib2D.cmake)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/UseBRAINSCommonLib2D.cmake.in
  ${BRAINSCommonLib2D_USE_FILE_CONFIG}
  @ONLY)

set(BRAINSCommonLib2D_DATA_DIRS_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/TestData)
set(BRAINSCommonLib2D_INCLUDE_DIRS_CONFIG ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
set(BRAINSCommonLib2D_LIBRARY_DIRS_CONFIG ${CMAKE_CURRENT_BINARY_DIR})
set(ITK_DIR_CONFIG ${ITK_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BRAINSCommonLib2DConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/BRAINSCommonLib2DConfig.cmake @ONLY)

# Settings specific for installation trees
#
#
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/UseBRAINSCommonLib2D.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/UseBRAINSCommonLib2D.cmake_install
  @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BRAINSCommonLib2DInstallConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/BRAINSCommonLib2DConfig.cmake_install @ONLY)
