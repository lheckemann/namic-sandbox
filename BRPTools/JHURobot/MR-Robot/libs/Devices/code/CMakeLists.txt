#
# CMakeLists.txt for MRRobotDevices library
#

PROJECT(MRRobotDevices)

SET(MRRobotDevices_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
INCLUDE_DIRECTORIES(${MRRobotDevices_INCLUDE})

# all source files
SET(SOURCE_FILES
    devDMM16AT.cpp
    devRMM1612.cpp
    )

# all header files
SET(HEADER_FILES
    ${MRRobotDevices_INCLUDE}/devDMM16AT.h
    ${MRRobotDevices_INCLUDE}/devRMM1612.h
    )

ADD_LIBRARY(MRRobotDevices ${IS_SHARED} ${SOURCE_FILES} ${HEADER_FILES})
TARGET_LINK_LIBRARIES(MRRobotDevices dscud5)
