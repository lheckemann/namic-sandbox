project(MultiImageRegistration)
set(LOCAL_PROJECT_NAME MultiImageRegistration)
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

enable_testing()
include(Dart)
#include(CPack)

include(GenerateMultiImageRegistrationConfig.cmake)


## A simple macro to set variables ONLY if it has not been set
## This is needed when stand-alone packages are combined into
## a larger package, and the desired behavior is that all the
## binary results end up in the combined directory.
if(NOT SETIFEMPTY)
macro(SETIFEMPTY)
  set(KEY ${ARGV0})
  set(VALUE ${ARGV1})
  if(NOT ${KEY})
    set(${KEY} ${VALUE})
  endif(NOT ${KEY})
endmacro(SETIFEMPTY KEY VALUE)
endif(NOT SETIFEMPTY)
###
SETIFEMPTY(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
SETIFEMPTY(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
SETIFEMPTY(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

##  Before looking for ITK or VTK, or many other packages, determine if
##  we are supposed to be building against Slicer3, and if so, then
##  be sure to get the Slicer3 version of the software first.
if(NOT CHECKIFSLICER3BUILD)
macro(CHECKIFSLICER3BUILD)
if(BUILD_AGAINST_SLICER3 OR Slicer3_SOURCE_DIR)
    find_package(Slicer3 REQUIRED)
    if (Slicer3_FOUND)
      include(${Slicer3_USE_FILE})
    else (Slicer3_FOUND)
      message(FATAL_ERROR "Cannot build without a Slicer3 build tree or a Slicer3 installation when BUILD_AGAINST_SLICER3 was specified. Please set Slicer3_DIR when BUILD_AGAINST_SLICER3:BOOL=ON.")
    endif (Slicer3_FOUND OR Slicer3_SOURCE_DIR)
    slicer3_set_default_install_prefix_for_external_projects()
  endif(BUILD_AGAINST_SLICER3 OR Slicer3_SOURCE_DIR)
endmacro(CHECKIFSLICER3BUILD)
endif(NOT CHECKIFSLICER3BUILD)
CHECKIFSLICER3BUILD()  ## Call the convenience macro

##  In many cases sub-projects depending on SlicerExectuion Model
##  that can be built stand alone are combined in larger packages.
##  This logic will include SlicerExectionModel only if it
##  has not already been included by a previous package.
if(NOT ADDFIRSTSLICEREXECUTIONMODEL)
macro(ADDFIRSTSLICEREXECUTIONMODEL LOCAL_PROJECT_NAMESPACE)

  if(NOT BUILDSEMFROM${LOCAL_PROJECT_NAMESPACE})
    find_package(GenerateCLP NO_MODULE QUIET)
  endif(NOT BUILDSEMFROM${LOCAL_PROJECT_NAMESPACE})

  find_package(GenerateCLP NO_MODULE REQUIRED)
  if(GenerateCLP_DIR)
    include(${GenerateCLP_USE_FILE})
  else(GenerateCLP_DIR)
    message(FATAL_ERROR "Can't build without GenerateCLP. Please set GenerateCLP_DIR")
  endif(GenerateCLP_DIR)

endmacro(ADDFIRSTSLICEREXECUTIONMODEL)
endif(NOT ADDFIRSTSLICEREXECUTIONMODEL)
###
ADDFIRSTSLICEREXECUTIONMODEL(${LOCAL_PROJECT_NAME})


if(NOT ITK_FOUND)
    find_package(ITK REQUIRED)
    include(${ITK_USE_FILE})
endif(NOT ITK_FOUND)

SET(ITK_VERSION ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR})
#ITK>=3.16
IF("${ITK_VERSION}" LESS 3.16)
   MESSAGE(SEND_ERROR
          "This version of ITK is too old to build MultiImageRegistration.  "
          "Please upgrade to ITK 3.16 or greater.")
ENDIF("${ITK_VERSION}" LESS 3.16)

find_package(BRAINSCommonLib NO_MODULE REQUIRED)
include(${BRAINSCommonLib_USE_FILE})

SUBDIRS( Source )
SUBDIRS( Testing )
