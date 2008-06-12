# This project is designed to be built outside the Insight source tree.
PROJECT(SliceContiguousImage)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
# IF(COMMAND CMAKE_POLICY)
#   CMAKE_POLICY(SET CMP0003 NEW)
# ENDIF(COMMAND CMAKE_POLICY)

# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

INCLUDE_DIRECTORIES(
  BEFORE
  ${SliceContiguousImage_SOURCE_DIR}/Source
  ${SliceContiguousImage_SOURCE_DIR}/Testing
  )

ADD_SUBDIRECTORY( Source )
ADD_SUBDIRECTORY( Testing )

