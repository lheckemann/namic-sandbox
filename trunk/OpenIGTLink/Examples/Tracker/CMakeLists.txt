PROJECT(Tracker)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

INCLUDE(${OpenIGTLink_SOURCE_DIR}/igtlCheckMacros.cmake)

igtl_platform_check()
igtl_type_check()

ADD_EXECUTABLE(Tracker   Tracker.cxx)
INCLUDE_DIRECTORIES( ${OpenIGTLink_SOURCE_DIR} )
TARGET_LINK_LIBRARIES(Tracker OpenIGTLink)

