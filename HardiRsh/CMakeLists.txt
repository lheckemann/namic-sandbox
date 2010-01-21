########################################################################
#
#    Date:    $Date$
#    Version: $Revision$
#    Author:  $Author$ 
#    ID:      $Id: CMakeLists.txt 154 2009-09-17 00:39:17Z bloyl@UPHS.PENNHEALTH.PRV $
#
#    File Description
#        Blah blah blah
#
#    Copyright (c) Blah Blah Blah
#    
#    This software is distributed WITHOUT ANY WARRANTY; without even
#    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#    PURPOSE.  See the above copyright notices for more information.
########################################################################

#TODO TRACK DOWN DEPENDENCIES

PROJECT(SBIA_HARDI)

cmake_minimum_required(VERSION 2.6)
SET(PROJECT_VERSION 0.0.0)

#add #defines, for the Project Name and the Project Release
ADD_DEFINITIONS(-DPROJECT_NAME="${PROJECT_NAME}")
ADD_DEFINITIONS(-DPROJECT_VERSION="${PROJECT_VERSION}")

#find the sbiaDefs.cmake file in the system places
find_file(SBIADEFS_file sbiaDefs.cmake)
  
IF (NOT SBIADEFS_file)
  SET(SBIADEFS_file ${PROJECT_SOURCE_DIR}/cmake_modules/sbiaDefs.cmake
   CACHE FILEPATH "location of sbiaDefs.cmake" FORCE)
  MESSAGE (STATUS "Can't find global sbiaDefs.cmake. Using local (project) version.")
ENDIF (NOT SBIADEFS_file)

MARK_AS_ADVANCED ( SBIADEFS_file ) #we don't want to see this in the GUI

IF (EXISTS "${SBIADEFS_file}")
  INCLUDE (${SBIADEFS_file})
ELSE (EXISTS "${SBIADEFS_file}")
  MESSAGE (FATAL_ERROR "Can't Find ANY sbiaDefs.cmake. There should be one in Cmake Directory?")
ENDIF (EXISTS "${SBIADEFS_file}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

SET (BUILD_TEST ON CACHE BOOL "Do you want to build tests")
SET (BUILD_DOC  ON CACHE BOOL "Do you want to build Documentation (Needs Doxygen) ")

SET (BUILD_SLICER ON CACHE BOOL "Do you want to build Slicer Support")
SET (BUILD_EXECS OFF CACHE BOOL "Do you want to build the exectuables")

SET (SKIP_ITK_BUILD_SETTINGS_FILE OFF CACHE BOOL "Skip including the itk Build settings")

#Setup the buildname if it isn't set by hand!
IF(NOT BUILDNAME)
  INCLUDE(CTest)
  SET(BUILDNAME "${BUILDNAME}-HardiRSH-${CMAKE_BUILD_TYPE}" CACHE STRING "Build name for cdash submisions")
ELSE(NOT BUILDNAME)
  INCLUDE(CTest)
ENDIF(NOT BUILDNAME)

IF(BUILD_SLICER)
  #Find Slicer Dependencies and Use Slicer ITK
  if(NOT Slicer3_SOURCE_DIR)
    find_package(Slicer3 REQUIRED)
    include(${Slicer3_USE_FILE})
    slicer3_set_default_install_prefix_for_external_projects()
  endif(NOT Slicer3_SOURCE_DIR)

  # Find VTK
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})

  find_package(GenerateCLP REQUIRED)
  include(${GenerateCLP_USE_FILE})

  # Find ITK.
  FIND_PACKAGE(ITK REQUIRED)
  INCLUDE(${ITK_USE_FILE})
  
ELSE(BUILD_SLICER)
  # Find ITK.
  FIND_PACKAGE(ITK REQUIRED)
  INCLUDE(${ITK_USE_FILE})
ENDIF(BUILD_SLICER)

#You may need to change this based on where stuff is loacted
#this is where the *.h s are!
SET( ${PROJECT_NAME}_INCLUDES
  Code/Source
)

#Include out headers
INCLUDE_DIRECTORIES( ${${PROJECT_NAME}_INCLUDES} )

##Enable code coverage for debug builds!
SET( CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fprofile-arcs -ftest-coverage "
 CACHE STRING "Flags used by the compiler during debug builds." FORCE)
SET( CMAKE_C_FLAGS_DEBUG "-g -O0 -fprofile-arcs -ftest-coverage "
 CACHE STRING "Flags used by the compiler during debug builds." FORCE)

SET( SpecialFunctions_SOURCES
  ${PROJECT_SOURCE_DIR}/Code/Source/itkReplaceSpecialFunctions.cxx
  )

ENABLE_TESTING()

ADD_SUBDIRECTORY(Code)

IF (BUILD_TEST)
  ADD_SUBDIRECTORY(Testing)
ENDIF (BUILD_TEST)

IF (BUILD_DOC)
  ADD_SUBDIRECTORY(Doc)
ENDIF (BUILD_DOC)
