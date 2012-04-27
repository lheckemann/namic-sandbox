#
# $Id: $
#

if (MRRobot_HAS_IRE)
  add_executable(DMM16AT_Test main.cpp)

  add_custom_command(TARGET DMM16AT_Test POST_BUILD
      COMMAND ln ARGS -fns ${CMAKE_CURRENT_SOURCE_DIR}/XMLConfig/ DMM16AT_TestXMLConfig
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

  target_link_libraries(DMM16AT_Test dl util)
  cisst_target_link_libraries(DMM16AT_Test cisstCommon cisstCommonXML cisstVector cisstOSAbstraction
                                           cisstInteractive cisstMultiTask)
  target_link_libraries(DMM16AT_Test MRRobotDevices)
else (MRRobot_HAS_IRE)
  add_custom_target(DMM16AT_Test ALL
      COMMAND ln -fns ${CMAKE_CURRENT_SOURCE_DIR}/XMLConfig/ DMM16AT_TestXMLConfig
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif (MRRobot_HAS_IRE)

