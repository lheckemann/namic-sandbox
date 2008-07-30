PROJECT(Performance)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(PerformanceTestClient   PerformanceTestClient.cxx igtlLoopController.cxx)
TARGET_LINK_LIBRARIES(PerformanceTestClient OpenIGTLink)

ADD_EXECUTABLE(PerformanceTestServer   PerformanceTestServer.cxx igtlLogger.cxx)
TARGET_LINK_LIBRARIES(PerformanceTestServer OpenIGTLink)

