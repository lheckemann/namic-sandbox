#
# $Id$
#

ADD_EXECUTABLE(lopomocotest main.cpp)

ADD_CUSTOM_COMMAND(TARGET lopomocotest POST_BUILD
    COMMAND ln ARGS -fns ${CMAKE_CURRENT_SOURCE_DIR}/XMLConfig/ LoPoMoCoTestXMLConfig
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

TARGET_LINK_LIBRARIES(lopomocotest dl util)
CISST_REQUIRES("lopomocotest" "cisstCommon;cisstVector;cisstOSAbstraction;cisstInteractive;cisstMultiTask;cisstDevices")
