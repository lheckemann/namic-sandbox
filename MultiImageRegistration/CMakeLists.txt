include(ExternalProject)
cmake_minimum_required(VERSION 2.8)
PROJECT(MultiImageRegistration)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)
SET(ITK_VERSION ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR})
#ITK>=3.16
IF("${ITK_VERSION}" LESS 3.16)
MESSAGE(SEND_ERROR
          "This version of ITK is too old to build MultiImageRegistration.  "
          "Please upgrade to ITK 3.16 or greater.")
ELSE("${ITK_VERSION}" LESS 3.16)

if(NOT FINDORBUILD_COMMON_BRAINSCOMMONLIB)
macro(FINDORBUILD_COMMON_BRAINSCOMMONLIB)
IF(NOT COMMON_BRAINSCOMMONLIB_BINARY_DIR)
set(proj BRAINSCommonLibExternal)
ExternalProject_Add(${proj}
  SVN_REPOSITORY "https://www.nitrc.org/svn/brains/BRAINSCommonLib/trunk"
  SOURCE_DIR BRAINSCommonLib
  BINARY_DIR BRAINSCommonLib-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
    -DBUILD_TESTING:BOOL=${BUILD_TESTING}
    -DCMAKE_SKIP_RPATH:BOOL=ON
    -DITK_DIR:PATH=${ITK_DIR}
  INSTALL_COMMAND ""
)
SET(COMMON_BRAINSCOMMONLIB_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/BRAINSCommonLib)
SET(COMMON_BRAINSCOMMONLIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/BRAINSCommonLib-build)
ENDIF(NOT COMMON_BRAINSCOMMONLIB_BINARY_DIR)
endmacro(FINDORBUILD_COMMON_BRAINSCOMMONLIB)
endif(NOT FINDORBUILD_COMMON_BRAINSCOMMONLIB)
FINDORBUILD_COMMON_BRAINSCOMMONLIB()


  SET(EXECUTABLE_OUTPUT_PATH ${MultiImageRegistration_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
  SET(LIBRARY_OUTPUT_PATH ${MultiImageRegistration_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")

  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)


  IF( BUILD_AGAINST_SLICER3 )
    find_package(Slicer3 REQUIRED)
    include(${Slicer3_USE_FILE})

    slicer3_set_default_install_prefix_for_external_projects()
  ENDIF( BUILD_AGAINST_SLICER3 )
 
  IF( NOT BUILD_AGAINST_SLICER3 )
    ##  In many cases sub-projects depending on SlicerExectuion Model
    ##  that can be built stand alone are combined in larger packages
    ##  This logic will include SlicerExectionModel only if it
    ##  has not already been built.
     ADD_SUBDIRECTORY(SlicerExecutionModel)
  ENDIF( NOT BUILD_AGAINST_SLICER3 )
    FIND_PACKAGE(GenerateCLP NO_MODULE REQUIRED)
    IF(GenerateCLP_DIR)
      INCLUDE(${GenerateCLP_USE_FILE})
    ELSE(GenerateCLP_DIR)
      MESSAGE(FATAL_ERROR "Can't build without GenerateCLP. Please set
                    GenerateCLP_DIR")
    ENDIF(GenerateCLP_DIR)
#  INCLUDE_DIRECTORIES(LargestForegroundFilledMaskImageFilter)
 
include_directories(${COMMON_BRAINSCOMMONLIB_SOURCE_DIR})
link_directories(${COMMON_BRAINSCOMMONLIB_BINARY_DIR})

 
  SUBDIRS( Source )
  SUBDIRS( Testing )

ENDIF("${ITK_VERSION}" LESS 3.16)
