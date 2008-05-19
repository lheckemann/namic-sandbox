PROJECT(OpenIGTLinkJHU)

IF(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  ADD_DEFINITIONS(-DPLATFORM_OSX)
  IF(CMAKE_SYSTEM_VERSION MATCHES "9\\.[0-9]*\\.[0-9]*")
    ADD_DEFINITIONS(-DPLATFORM_OSX_LEOPARD)
  ENDIF(CMAKE_SYSTEM_VERSION MATCHES "9\\.[0-9]*\\.[0-9]*")
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

IF(CMAKE_SYSTEM_NAME MATCHES Linux)
 ADD_DEFINITIONS(-DPLATFORM_LINUX)
 SET(LINUX 1)
 SET(CMAKE_C_FLAGS -fPIC)
 SET(CMAKE_CXX_FLAGS -fPIC)
ENDIF(CMAKE_SYSTEM_NAME MATCHES Linux)

IF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
 ADD_DEFINITIONS(-DPLATFORM_WIN32)
 ADD_DEFINITIONS(-D_WIN32_WINDOWS)
 SET(WIN32 1)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Windows")

FIND_PATH(IGT_UTIL_DIR igtl_header.h
 ${CMAKE_CURRENT_SOURCE_DIR}/../OpenIGTLink/Source/igtlutil)

FIND_PATH(BOOST_DIR boost.css
 ${CMAKE_CURRENT_SOURCE_DIR}/../boost)


FIND_FILE(CISST_CMAKE
          cisstBuild.cmake
          ${CMAKE_CURRENT_SOURCE_DIR}/../cisst/build32
          DOC "Location of your cisstBuild.cmake")
          
INCLUDE(${CISST_CMAKE})

# Add the include and lib paths for cisst
IF (CISST_INCLUDE_DIR)
 INCLUDE_DIRECTORIES(${CISST_INCLUDE_DIR} ${CISSTNETLIB_INCLUDE_DIR})
 LINK_DIRECTORIES(${CISST_LIBRARY_DIR})
ENDIF (CISST_INCLUDE_DIR)

# For Microsoft compilers, see if the libraries are DLLs
# Useless and harmless with other compilers
IF(CISST_BUILD_SHARED_LIBS)
  ADD_DEFINITIONS(-DCISST_DLL)
ENDIF(CISST_BUILD_SHARED_LIBS)




INCLUDE_DIRECTORIES(
 /usr/realtime/include
 ${CMAKE_CURRENT_SOURCE_DIR}/common/
 ${CMAKE_CURRENT_SOURCE_DIR}/common/control
 ${CMAKE_CURRENT_SOURCE_DIR}/common/transmit
 ${IGT_UTIL_DIR}
 ${BOOST_DIR}
)

LINK_DIRECTORIES(
 ${BOOST_DIR}/stage/lib
)



SET(SOURCES_IGTL
 ${IGT_UTIL_DIR}/igtl_util.c
 ${IGT_UTIL_DIR}/igtl_header.c
 #${IGT_UTIL_DIR}/igtl_image.c
 ${IGT_UTIL_DIR}/igtl_transform.c
 ${IGT_UTIL_DIR}/crc32.c
 common/igtlNetwork.cpp
 common/igtlMessage.cpp
)

SET(HEADERS_IGTL
 ${IGT_UTIL_DIR}/crc32.h
 ${IGT_UTIL_DIR}/igtl_header.h
 #${IGT_UTIL_DIR}/igtl_image.h
 ${IGT_UTIL_DIR}/igtl_transform.h
 ${IGT_UTIL_DIR}/igtl_util.h 
 common/igtlNetwork.h
 common/igtlMessage.h
)


 
SET(SOURCES_PROXY
 proxy/main.cpp
 common/transmit/BRPtprMessagesProc.h
 common/transmit/BRPtprMessagesProc.cpp
 common/transmit/BRPtprMessageToPipe.h
 common/transmit/BRPtprMessageToPipe.cpp
 common/BRPtprMessages.h
 common/BRPtprMessages.cpp
 common/control/BRPtprPipe.h
 common/control/BRPtprPipe.cpp
 common/BRPTPRInterface.h
 common/BRPplatform.cpp
 common/BRPplatform.h
)

SET(SOURCES_ROBOT
 robot/ctfMain.cpp
 common/control/BRPtprControl.h
 common/control/BRPtprControl.cpp
 common/control/ctfControlBase.h
 common/control/ctfControlBase.cpp
 common/control/ctfControl.h
 common/control/ctfControl.cpp
 common/BRPplatform.cpp
 common/BRPplatform.h
 common/control/BRPtprOpenTracker.h
 common/control/BRPtprOpenTracker.cpp
 common/BRPtprMessages.h
 common/BRPtprMessages.cpp

 ${IGT_UTIL_DIR}/igtl_header.c
 ${IGT_UTIL_DIR}/igtl_header.h
 ${IGT_UTIL_DIR}/igtl_util.c
 ${IGT_UTIL_DIR}/igtl_util.h
 common/igtlMessage.cpp
 common/igtlMessage.h
 ${IGT_UTIL_DIR}/crc32.c
 ${IGT_UTIL_DIR}/crc32.h
)

SET(SOURCES_ROBOT_SIM
 robotsim/ctfMain.cpp
 common/BRPtprMessages.h
 common/BRPtprMessages.cpp
 common/control/BRPtprOpenTracker.h
 common/control/BRPtprOpenTracker.cpp
 common/control/BRPtprControl.h
 common/control/BRPtprControl.cpp
 common/control/ctfControlBase.h
 common/control/ctfControlBase.cpp
 common/control/ctfControl.h
 common/control/ctfControl.cpp
 common/BRPplatform.cpp
 common/BRPplatform.h
 
 ${IGT_UTIL_DIR}/igtl_header.c
 ${IGT_UTIL_DIR}/igtl_header.h
 ${IGT_UTIL_DIR}/igtl_util.c
 ${IGT_UTIL_DIR}/igtl_util.h
 common/igtlMessage.cpp
 common/igtlMessage.h
 ${IGT_UTIL_DIR}/crc32.c
 ${IGT_UTIL_DIR}/crc32.h
)

SET(SOURCES_SLICER_SIM
 slicersim/main.cpp
 common/transmit/BRPtprMessagesProc.h
 common/transmit/BRPtprMessagesProc.cpp
 common/BRPtprMessages.h
 common/BRPtprMessages.cpp
 common/BRPTPRInterface.h
)

SET(BOOST_LIBS
 boost_thread-gcc34-mt
 boost_system-gcc34-mt
)

SET(LIBS
 ${BOOST_LIBS}
)


IF(NOT WIN32)
 SET(LIBS
  ${LIBS}
  ncurses
  pthread
  dl
 )
 IF(LINUX)
  SET(EXTRA_LIBS
   rt
  )
 ENDIF(LINUX)
ENDIF(NOT WIN32)


SUBDIRS(
 robot
)


 ADD_EXECUTABLE(JHUslicerSim ${SOURCES_IGTL} ${HEADERS_IGTL} ${SOURCES_SLICER_SIM})
 ADD_EXECUTABLE(JHUrProxy ${SOURCES_IGTL} ${HEADERS_IGTL} ${SOURCES_PROXY})
 ADD_EXECUTABLE(JHUrobot ${SOURCES_ROBOT})
 ADD_EXECUTABLE(JHUrobotSim ${SOURCES_ROBOT_SIM}) 


TARGET_LINK_LIBRARIES(JHUslicerSim
 ${LIBS}
)

TARGET_LINK_LIBRARIES(JHUrProxy
 ${LIBS}
)

TARGET_LINK_LIBRARIES(JHUrobot
 ${LIBS}
 ${CISST_LIBRARIES}
)

TARGET_LINK_LIBRARIES(JHUrobotSim
 ${LIBS}
 ${CISST_LIBRARIES}
)

