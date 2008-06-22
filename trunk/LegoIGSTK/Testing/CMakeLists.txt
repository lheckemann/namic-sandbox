
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${LegoIGSTK_SOURCE_DIR})
SET(TEMP ${LegoIGSTK_BINARY_DIR}/Testing/Temporary)

########################################
########################################
#
#  Define Executables
#
########################################
########################################

########################################
#
#  Executables not requiring IGSTK
#
########################################

ADD_EXECUTABLE(igstkLegoTest01 igstkLegoTest01.cxx )
TARGET_LINK_LIBRARIES(igstkLegoTest01 LegoIGSTK)

ADD_EXECUTABLE(igstkLegoMotorTest igstkLegoMotorTest.cxx )
TARGET_LINK_LIBRARIES(igstkLegoMotorTest LegoIGSTK)

ADD_EXECUTABLE(igstkLegoPlayToneTest igstkLegoPlayToneTest.cxx )
TARGET_LINK_LIBRARIES(igstkLegoPlayToneTest LegoIGSTK)

ADD_EXECUTABLE(igstkLegoUSSensorTest igstkLegoUSSensorTest.cxx )
TARGET_LINK_LIBRARIES(igstkLegoUSSensorTest LegoIGSTK)

ADD_EXECUTABLE(igstkLegoTouchSensorTest igstkLegoTouchSensorTest.cxx )
TARGET_LINK_LIBRARIES(igstkLegoTouchSensorTest LegoIGSTK)

ADD_EXECUTABLE(igstkLegoSoundSensorTest igstkLegoSoundSensorTest.cxx )
TARGET_LINK_LIBRARIES(igstkLegoSoundSensorTest LegoIGSTK)

ADD_EXECUTABLE(igstkLegoLightSensorTest igstkLegoLightSensorTest.cxx )
TARGET_LINK_LIBRARIES(igstkLegoLightSensorTest LegoIGSTK)



########################################
#
#  Executables requiring IGSTK
#
########################################

IF(IGSTK_FOUND)

ADD_EXECUTABLE(igstkLegoSimulatedTrackerTest igstkLegoSimulatedTrackerTest.cxx )
TARGET_LINK_LIBRARIES(igstkLegoSimulatedTrackerTest 
  LegoIGSTK 
  ${IGSTK_LIBRARIES}
  ITKIO ITKBasicFilters ITKNumerics ITKCommon ITKSpatialObject
  vtkRendering vtkGraphics vtkHybrid vtkImaging 
  vtkIO vtkFiltering vtkCommon vtksys
)

ENDIF(IGSTK_FOUND)




########################################
########################################
#
#  Define Tests
#
########################################
########################################

########################################
#
#  Test not requiring IGSTK
#
########################################

ADD_TEST(igstkLegoTest01
  ${CXX_TEST_PATH}/igstkLegoTest01
 )

ADD_TEST(igstkLegoMotorTest01 ${CXX_TEST_PATH}/igstkLegoMotorTest  A  50  10 )
ADD_TEST(igstkLegoMotorTest02 ${CXX_TEST_PATH}/igstkLegoMotorTest  A -50  10 )
ADD_TEST(igstkLegoMotorTest03 ${CXX_TEST_PATH}/igstkLegoMotorTest  B  50  10 )
ADD_TEST(igstkLegoMotorTest04 ${CXX_TEST_PATH}/igstkLegoMotorTest  B -50  10 )
ADD_TEST(igstkLegoMotorTest05 ${CXX_TEST_PATH}/igstkLegoMotorTest  C  50  10 )
ADD_TEST(igstkLegoMotorTest06 ${CXX_TEST_PATH}/igstkLegoMotorTest  C -50  10 )

ADD_TEST(igstkLegoPlayToneTest01 ${CXX_TEST_PATH}/igstkLegoPlayToneTest  1000  1 )

SET(US_SENSOR_PORT 4)
ADD_TEST(igstkLegoUSSensorTest01 ${CXX_TEST_PATH}/igstkLegoUSSensorTest ${US_SENSOR_PORT})

SET(TOUCH_SENSOR_PORT 3)
ADD_TEST(igstkLegoTouchSensorTest01 ${CXX_TEST_PATH}/igstkLegoTouchSensorTest ${TOUCH_SENSOR_PORT})

SET(LIGHT_SENSOR_PORT 1)
ADD_TEST(igstkLegoLightSensorTest01 ${CXX_TEST_PATH}/igstkLegoLightSensorTest ${LIGHT_SENSOR_PORT})

SET(SOUND_SENSOR_PORT 2)
ADD_TEST(igstkLegoSoundSensorTest01 ${CXX_TEST_PATH}/igstkLegoSoundSensorTest ${SOUND_SENSOR_PORT})


########################################
#
#  Test requiring IGSTK
#
########################################

IF(IGSTK_FOUND)

ENDIF(IGSTK_FOUND)

