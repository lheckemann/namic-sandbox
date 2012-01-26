#
# $Id$
#

ADD_EXECUTABLE(CompositeMRRobot_Test main.cpp)

ADD_CUSTOM_COMMAND(TARGET CompositeMRRobot_Test POST_BUILD
    COMMAND ln ARGS -fns ${CMAKE_CURRENT_SOURCE_DIR}/XMLConfig/ CompositeMRRobot_TestXMLConfig
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

TARGET_LINK_LIBRARIES(CompositeMRRobot_Test dl util)
CISST_REQUIRES("CompositeMRRobot_Test" "cisstCommon;cisstVector;cisstOSAbstraction;cisstInteractive;cisstMultiTask;cisstDevices")
TARGET_LINK_LIBRARIES(CompositeMRRobot_Test MRRobotDevices)


