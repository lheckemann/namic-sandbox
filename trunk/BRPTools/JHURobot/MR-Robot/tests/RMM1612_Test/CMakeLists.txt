#
# CMakeLists.txt
#

add_executable(RMM1612_Test main.cpp)

add_custom_command(TARGET RMM1612_Test POST_BUILD
    COMMAND ln ARGS -fns ${CMAKE_CURRENT_SOURCE_DIR}/XMLConfig/ RMM1612_TestXMLConfig
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

target_link_libraries(RMM1612_Test dl util)
cisst_target_link_libraries(RMM1612_Test cisstCommon cisstCommonXML cisstVector cisstOSAbstraction
                                         cisstInteractive cisstMultiTask)
target_link_libraries(RMM1612_Test MRRobotDevices)
