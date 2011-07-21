project(BRAINSDemonWarp2D)
set(LOCAL_PROJECT_NAME BRAINSDemonWarp2D)
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

enable_testing()
include(Dart)
include(CPack)
include(${BRAINS_CMAKE_HELPER_DIR}/PreventInSourceBuilds.cmake)
include(${BRAINS_CMAKE_HELPER_DIR}/CMakeBuildMacros.cmake)

###
SETIFEMPTY(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
SETIFEMPTY(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
SETIFEMPTY(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
SETIFEMPTY(CMAKE_BUNDLE_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/bin)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

SETIFEMPTY(MIDAS_REST_URL "http://midas.kitware.com/api/rest" CACHE STRING "The MIDAS server where testing data resides")

###
CHECKIFSLICER3BUILD()  ## Call the convenience macro

if(NOT ITK_FOUND)
    find_package(ITK REQUIRED)
    include(${ITK_USE_FILE})
endif(NOT ITK_FOUND)

find_package(BRAINSCommonLib2D NO_MODULE REQUIRED)
include(${BRAINSCommonLib2D_USE_FILE})

#-----------------------------------------------------------------------------
# Output directories.
#
include(${BRAINS_CMAKE_HELPER_DIR}/CMakeBRAINS3BuildMacros.cmake)
#SETOPTIONALDEBUGIMAGEVIEWER()


## No need to add with nothing to compile. add_subdirectory(BRAINSFit_Common)
#include(${BRAINS_CMAKE_HELPER_DIR}/IJMacros.txt)

###
configure_file(${BRAINSDemonWarp2D_SOURCE_DIR}/CTestCustom.ctest ${BRAINSDemonWarp2D_BINARY_DIR}/CTestCustom.ctest COPYONLY)

include(${BRAINS_CMAKE_HELPER_DIR}/CMakeBRAINS3BuildMacros.cmake)
###

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/BRAINSDemonWarp2DCommonLib.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/BRAINSDemonWarp2DCommonLib.h
  )


set(BRAINSDemonWarp2DTemplatesLIB_SRCS
         BRAINSDemonWarp2DTemplates_Vdouble.cxx
         BRAINSDemonWarp2DTemplates_Vshort.cxx
         BRAINSDemonWarp2DTemplates_Vushort.cxx
         BRAINSDemonWarp2DTemplates_int.cxx
         BRAINSDemonWarp2DTemplates_uint.cxx
         BRAINSDemonWarp2DTemplates_Vfloat.cxx
         BRAINSDemonWarp2DTemplates_Vuchar.cxx
         BRAINSDemonWarp2DTemplates_double.cxx
         BRAINSDemonWarp2DTemplates_short.cxx
         BRAINSDemonWarp2DTemplates_ushort.cxx
         BRAINSDemonWarp2DTemplates_Vint.cxx
         BRAINSDemonWarp2DTemplates_Vuint.cxx
         BRAINSDemonWarp2DTemplates_float.cxx
         BRAINSDemonWarp2DTemplates_uchar.cxx
         BRAINSDemonWarp2DTemplates.cxx
)

set(CLP BRAINSDemonWarp2DCommonLib)
set(${CLP}_SOURCE ${BRAINSDemonWarp2DTemplatesLIB_SRCS})
## NOTE:  The BRAINSDemonWarp2DCommonLib includes BRAINSDemonWarp2DPrimary.cxx and must be configured against BRAINSDemonWarp2DPrimary.xml
CONFIGUREBRAINSORSLICERLIBRARY(${CLP} BRAINSDemonWarp2DPrimary.xml "${${CLP}_SOURCE}" "")
## NOTE:  The BRAINSDemonWarp2DCommonLib includes VBRAINSDemonWarp2DPrimary.cxx and must be configured against VBRAINSDemonWarp2DPrimary.xml

set(CLP BRAINSDemonWarp2D)
set(${CLP}_SOURCE BRAINSDemonWarp2D.cxx )
set(${CLP}_PRIMARY_SOURCE BRAINSDemonWarp2DPrimary.cxx)
CONFIGUREBRAINSORSLICERPROPERTIES(${CLP} BRAINSDemonWarp2DPrimary.xml "${${CLP}_SOURCE}" "${${CLP}_PRIMARY_SOURCE}" "BRAINSDemonWarp2DPrimary" "BRAINSDemonWarp2DCommonLib")

if(BRAINS_BUILD) ## ONLY BUILD FOR BACKWARDS COMPATIBILITY IN BRAINS ENVIRONMENT
  set(CLP BRAINSDemonWarp2DCLP) ## NOTE: This is completely redundant, but is needed to support all the old scripts that use the original name
  set(${CLP}_SOURCE BRAINSDemonWarp2D.cxx )
  set(${CLP}_PRIMARY_SOURCE BRAINSDemonWarp2DPrimary.cxx)
  CONFIGUREBRAINSORSLICERPROPERTIES(${CLP} BRAINSDemonWarp2DPrimary.xml "${${CLP}_SOURCE}" "${${CLP}_PRIMARY_SOURCE}" "BRAINSDemonWarp2DPrimary" "BRAINSDemonWarp2DCommonLib")
## HACK -- Need to write other backwards compatibility wrappers for the old names of BRAINSDemonWarp2DCLP
endif(BRAINS_BUILD) ## ONLY BUILD FOR BACKWARDS COMPATIBILITY IN BRAINS ENVIRONMENT

set(CLP VBRAINSDemonWarp2D)
set(${CLP}_SOURCE VBRAINSDemonWarp2D.cxx )
set(${CLP}_PRIMARY_SOURCE VBRAINSDemonWarp2DPrimary.cxx)
#CONFIGUREBRAINSORSLICERPROPERTIES(${CLP} VBRAINSDemonWarp2DPrimary.xml "${${CLP}_SOURCE}" "BRAINSDemonWarp2DCommonLib")
CONFIGUREBRAINSORSLICERPROPERTIES(${CLP} VBRAINSDemonWarp2DPrimary.xml "${${CLP}_SOURCE}" "${${CLP}_PRIMARY_SOURCE}" "VBRAINSDemonWarp2DPrimary" "BRAINSDemonWarp2DCommonLib")

### Add the Testing Subdirectory.
#if(BUILD_TESTING)
#  add_subdirectory(TestSuite)
#endif(BUILD_TESTING)
#if(NOT Slicer3_SOURCE_DIR)
#  add_subdirectory(Example)
#endif(NOT Slicer3_SOURCE_DIR)
