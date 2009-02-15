CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

#
# Set the project name.
#

PROJECT (QUEENSOPENIGT)

#
# Load CMake commands to e.g. bring VTK and ITK into the project
#

INCLUDE (${QUEENSOPENIGT_SOURCE_DIR}/CMakeOptions.cmake)

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
SET(QUEENSOPENIGT_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

#
# Here is where the sub-directories for the QueensOpenIGT classes are.
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
  ${QUEENSOPENIGT_SOURCE_DIR}/VTKLibs
  ${QUEENSOPENIGT_SOURCE_DIR}/VTKLibs/Ultrasound
  ${QUEENSOPENIGT_SOURCE_DIR}/VTKLibs/Tracking
  ${QUEENSOPENIGT_SOURCE_DIR}/Utilities/ndicapi
  ${QUEENSOPENIGT_SOURCE_DIR}/Utilities/igtl
  ${QUEENSOPENIGT_SOURCE_DIR}/Utilities/igtl/igtlutils
)

#
# Common configuration settings
#

CONFIGURE_FILE(
  ${QUEENSOPENIGT_SOURCE_DIR}/QueensOpenIGTLibsConfigure.h.in
  ${QUEENSOPENIGT_BINARY_DIR}/QueensOpenIGTLibsConfigure.h
)

CONFIGURE_FILE(
  ${QUEENSOPENIGT_SOURCE_DIR}/SynchroGrabConfigure.h.in
  ${QUEENSOPENIGT_BINARY_DIR}/SynchroGrabConfigure.h
)


##################

CONFIGURE_FILE(${QUEENSOPENIGT_SOURCE_DIR}/igtlConfigure.h.in 
  ${QUEENSOPENIGT_BINARY_DIR}/igtlConfigure.h)

CONFIGURE_FILE(${QUEENSOPENIGT_SOURCE_DIR}/igtl_typeconfig.h.in 
  ${QUEENSOPENIGT_BINARY_DIR}/igtl_typeconfig.h)
  
###########

INCLUDE_DIRECTORIES(${QUEENSOPENIGT_BINARY_DIR})
