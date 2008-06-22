
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${LegoIGSTK_SOURCE_DIR})
SET(TEMP ${LegoIGSTK_BINARY_DIR}/Testing/Temporary)

#######################
#######################
#
#  Define Executables
#
#######################
#######################

ADD_EXECUTABLE(igstkLegoTest01 igstkLegoTest01.cxx )
TARGET_LINK_LIBRARIES(igstkLegoTest01 LegoIGSTK IGSTK)



###################
###################
#
#  Define Tests
#
###################
###################

ADD_TEST(igstkLegoTest01
  ${CXX_TEST_PATH}/igstkLegoTest01
 )


