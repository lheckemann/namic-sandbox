project(BRAINSTools)
set(LOCAL_PROJECT_NAME BRAINSTools)
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

## Need to use local version that does not try to delete a local directory.
set(BRAINS_CMAKE_HELPER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/BuildScripts)
include(${BRAINS_CMAKE_HELPER_DIR}/CMakeBuildMacros.cmake)

add_subdirectory(BRAINSCommonLib2D)
set(BRAINSCommonLib2D_DIR ${CMAKE_CURRENT_BINARY_DIR}/BRAINSCommonLib2D)
add_subdirectory(BRAINSDemonWarp2D)

option(BUILD_BRAINSFitIGT "Build the customized version of BRAINSFit for IGT registration support" OFF)
if(BUILD_BRAINSFitIGT)
  add_subdirectory(BRAINSFitIGT)
endif(BUILD_BRAINSFitIGT)
