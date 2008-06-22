
INCLUDE(${OpenIGTLink_SOURCE_DIR}/igtlCheckMacros.cmake)

igtl_platform_check()
igtl_type_check()


ADD_EXECUTABLE(igtlSocketTest   igtlSocketTest.cxx)

INCLUDE_DIRECTORIES( ${OpenIGTLink_SOURCE_DIR}/Source )

TARGET_LINK_LIBRARIES(igtlSocketTest OpenIGTLink)

ADD_TEST(igtlSocketTest igtlSocketTest)

ADD_SUBDIRECTORY( igtlutil )
