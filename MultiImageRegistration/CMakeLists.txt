CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

PROJECT(MultiImageRegistration)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

SET(EXECUTABLE_OUTPUT_PATH ${MultiImageRegistration_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")

SET(LIBRARY_OUTPUT_PATH ${MultiImageRegistration_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")

SUBDIRS( Source )

ENABLE_TESTING()
INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)

SUBDIRS( Testing )
