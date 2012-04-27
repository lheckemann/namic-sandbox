#
# $Id$
#

if (MRRobot_HAS_IRE)
  add_executable(CompositeMRRobot_Test main.cpp)

  add_custom_command(TARGET CompositeMRRobot_Test POST_BUILD
      COMMAND ln ARGS -fns ${CMAKE_CURRENT_SOURCE_DIR}/XMLConfig/ CompositeMRRobot_TestXMLConfig
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

  target_link_libraries(CompositeMRRobot_Test dl util ${sawLoPoMoCo_LIBRARIES})
  cisst_target_link_libraries(CompositeMRRobot_Test cisstCommon cisstCommonXML cisstVector cisstOSAbstraction
                                                  cisstInteractive cisstMultiTask)
  target_link_libraries(CompositeMRRobot_Test MRRobotDevices)
else (MRRobot_HAS_IRE)
  add_custom_target(CompositeMRRobot_Test ALL
      COMMAND ln -fns ${CMAKE_CURRENT_SOURCE_DIR}/XMLConfig/ CompositeMRRobot_TestXMLConfig
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif (MRRobot_HAS_IRE)


