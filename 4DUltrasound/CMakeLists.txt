CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

#
# Set the project name.
#

PROJECT (FOURDULTRASOUND)

#
# Load CMake commands to e.g. bring VTK and ITK into the project
#

INCLUDE (${FOURDULTRASOUND_SOURCE_DIR}/CMakeOptions.cmake)

#
# Try to find OpenIGTLink and include its settings (otherwise complain)
#
FIND_PACKAGE(OpenIGTLink REQUIRED)
INCLUDE(${OpenIGTLink_USE_FILE})

FIND_PATH(OPENIGTLINKSOURCE REQUIRED)
INCLUDE_DIRECTORIES(${OPENIGTLINKSOURCE_INCLUDE_DIR}/Source
                    ${OPENIGTLINKSOURCE_INCLUDE_DIR}/Source/igtlutil)

#
# Avoid dev warnings in CMake 2.6
#

IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

#
# Little configuration to force OpenIGTLink to compile statically
#
ADD_DEFINITIONS(-DIGTLSTATIC=1)

#
# Enable / Disable Hardware Simulators
#
OPTION(USE_ULTRASOUND_DEVICE "Use Ultrasound Device." OFF)

# 
# Whether to build with shared libs: define our own symbol
#

OPTION(BUILD_SHARED_LIBS "Build shared libraries." OFF)
SET(FOURDULTRASOUND_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})


#
# Here is where the sub-directories for the FOURDULTRASOUND classes are.
#

SUBDIRS (
  Utilities
  VTKLibs
  Applications
#  ITKLibs
)

#
# Include paths go here
#
INCLUDE_DIRECTORIES(
  ${FOURDULTRASOUND_SOURCE_DIR}/VTKLibs
  ${FOURDULTRASOUND_SOURCE_DIR}/VTKLibs/Ultrasound
  ${FOURDULTRASOUND_SOURCE_DIR}/VTKLibs/Tracking
  ${FOURDULTRASOUND_SOURCE_DIR}/Utilities/ndicapi
  #${FOURDULTRASOUND_SOURCE_DIR}/Utilities/igtl
  #${FOURDULTRASOUND_SOURCE_DIR}/Utilities/igtl/igtlutils
)


#
# Common configuration settings
#

#CONFIGURE_FILE(
#  ${FOURDULTRASOUND_SOURCE_DIR}/FOURDULTRASOUNDLibsConfigure.h.in
#  ${FOURDULTRASOUND_BINARY_DIR}/FOURDULTRASOUNDLibsConfigure.h
#)

CONFIGURE_FILE(
  ${FOURDULTRASOUND_SOURCE_DIR}/4DUltrasoundConfigure.h.in
  ${FOURDULTRASOUND_BINARY_DIR}/4DUltrasoundConfigure.h
)


##################

#CONFIGURE_FILE(${FOURDULTRASOUND_SOURCE_DIR}/igtlConfigure.h.in 
#  ${FOURDULTRASOUND_BINARY_DIR}/igtlConfigure.h)

#CONFIGURE_FILE(${FOURDULTRASOUND_SOURCE_DIR}/igtl_typeconfig.h.in 
#  ${FOURDULTRASOUND_BINARY_DIR}/igtl_typeconfig.h)
  
###########

INCLUDE_DIRECTORIES(${FOURDULTRASOUND_BINARY_DIR})
