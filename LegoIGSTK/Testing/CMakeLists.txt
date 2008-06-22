
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

ADD_EXECUTABLE(igstkLegoMotorTest igstkLegoMotorTest.cxx )
TARGET_LINK_LIBRARIES(igstkLegoMotorTest LegoIGSTK IGSTK)



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

ADD_TEST(igstkLegoMotorTest01 ${CXX_TEST_PATH}/igstkLegoMotorTest  A  50  10 )
ADD_TEST(igstkLegoMotorTest02 ${CXX_TEST_PATH}/igstkLegoMotorTest  A -50  10 )
ADD_TEST(igstkLegoMotorTest03 ${CXX_TEST_PATH}/igstkLegoMotorTest  B  50  10 )
ADD_TEST(igstkLegoMotorTest04 ${CXX_TEST_PATH}/igstkLegoMotorTest  B -50  10 )
ADD_TEST(igstkLegoMotorTest05 ${CXX_TEST_PATH}/igstkLegoMotorTest  C  50  10 )
ADD_TEST(igstkLegoMotorTest06 ${CXX_TEST_PATH}/igstkLegoMotorTest  C -50  10 )

