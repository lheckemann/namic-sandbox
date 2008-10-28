PROJECT(VFixture)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(VFixtureClient   VFixtureClient.cxx)
TARGET_LINK_LIBRARIES(VFixtureClient OpenIGTLink)

ADD_EXECUTABLE(VFixtureServer   VFixtureServer.cxx)
TARGET_LINK_LIBRARIES(VFixtureServer OpenIGTLink)

