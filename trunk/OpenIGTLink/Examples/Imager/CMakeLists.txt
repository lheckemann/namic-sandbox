PROJECT(Tracker)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

INCLUDE_DIRECTORIES( ${OpenIGTLink_SOURCE_DIR} )

ADD_EXECUTABLE(ImagerClient   TrackerClient.cxx)
TARGET_LINK_LIBRARIES(TrackerClient OpenIGTLink)

ADD_EXECUTABLE(ImagerServer   TrackerServer.cxx)
TARGET_LINK_LIBRARIES(TrackerServer OpenIGTLink)

