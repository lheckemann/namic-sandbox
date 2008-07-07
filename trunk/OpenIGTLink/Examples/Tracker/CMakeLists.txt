PROJECT(Tracker)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#INCLUDE(${OpenIGTLink_SOURCE_DIR}/igtlCheckMacros.cmake)
#
#igtl_platform_check()
#igtl_type_check()
#
#INCLUDE_DIRECTORIES( ${OpenIGTLink_SOURCE_DIR} )
#
#ADD_EXECUTABLE(TrackerClient   TrackerClient.cxx)
#TARGET_LINK_LIBRARIES(TrackerClient OpenIGTLink)
#
#ADD_EXECUTABLE(TrackerServer   TrackerServer.cxx)
#TARGET_LINK_LIBRARIES(TrackerServer OpenIGTLink)


find_package(OpenIGTLink REQUIRED)
if(NOT OpenIGTLink_FOUND)
  message(FATAL_ERROR "Need OpenIGTLink to build")
endif(NOT OpenIGTLink_FOUND)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(TrackerClient   TrackerClient.cxx)
TARGET_LINK_LIBRARIES(TrackerClient OpenIGTLink)
ADD_EXECUTABLE(TrackerServer   TrackerServer.cxx)
TARGET_LINK_LIBRARIES(TrackerServer OpenIGTLink)
