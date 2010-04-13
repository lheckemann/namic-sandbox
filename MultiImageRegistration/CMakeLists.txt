project(MultiImageRegistration)
set(LOCAL_PROJECT_NAME MultiImageRegistration)
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

enable_testing()
include(Dart)
include(CPack)
include(ExternalProject)
cmake_minimum_required(VERSION 2.8)

SET(LOCAL_CMAKE_BUILD_OPTIONS
  -DCMAKE_SKIP_RPATH:BOOL=ON
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DCMAKE_CXX_FLAGS_RELEASE:STRING=${CMAKE_CXX_FLAGS_RELEASE}
  -DCMAKE_CXX_FLAGS_DEBUG:STRING=${CMAKE_CXX_FLAGS_DEBUG}
  -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
  -DCMAKE_C_FLAGS_RELEASE:STRING=${CMAKE_C_FLAGS_RELEASE}
  -DCMAKE_C_FLAGS_DEBUG:STRING=${CMAKE_C_FLAGS_DEBUG}
  -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
  -DBUILD_EXAMPLES:BOOL=OFF
  -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
  -DBUILD_TESTING:BOOL=OFF
  -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
)

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
ENDIF("${ITK_VERSION}" LESS 3.16)

# Compute -G arg for configuring external projects with the same CMake generator:
#
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()

set(GenerateCLP_DIR ${CMAKE_INSTALL_PREFIX}/lib/GenerateCLP)
##  Build the SlicerExecutionModel Once, and let all derived project use the same version
#-----------------------------------------------------------------------------
# Get and build SlicerExecutionModel
#FINDORBUILD_COMMON_SLICEREXECUTIONMODEL()
#ADDFIRSTSLICEREXECUTIONMODEL(${LOCAL_PROJECT_NAME})

set(proj SlicerExecutionModel)
ExternalProject_Add(${proj}
  SVN_REPOSITORY "http://svn.slicer.org/Slicer3/trunk/Libs/SlicerExecutionModel"
  SOURCE_DIR SlicerExecutionModel
  BINARY_DIR SlicerExecutionModel-build
  DEPENDS
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${LOCAL_CMAKE_BUILD_OPTIONS}
    -DITK_DIR:PATH=${ITK_DIR}
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
)

if(0)
if(NOT BRAINSCommonLib)
set(proj BRAINSCommonLib)
ExternalProject_Add(${proj}
  SVN_REPOSITORY "https://www.nitrc.org/svn/brains/BRAINSCommonLib/trunk"
  SOURCE_DIR ${proj}
  BINARY_DIR ${proj}-build
  DEPENDS Insight
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${LOCAL_CMAKE_BUILD_OPTIONS}
    -DITK_DIR:PATH=${ITK_DIR}
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
)
endif()
endif()

SUBDIRS( Source )
SUBDIRS( Testing )

