#
# $Id$
#

add_executable(lopomocotest main.cpp)

add_custom_command(TARGET lopomocotest POST_BUILD
    COMMAND ln ARGS -fns ${CMAKE_CURRENT_SOURCE_DIR}/XMLConfig/ LoPoMoCoTestXMLConfig
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

target_link_libraries(lopomocotest dl util ${sawLoPoMoCo_LIBRARIES})
cisst_target_link_libraries(lopomocotest cisstCommon cisstCommonXML cisstVector cisstOSAbstraction
                                         cisstInteractive cisstMultiTask)
