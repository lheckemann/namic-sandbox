# cmake file for JHU robot + simulators
# csabacsoma at gmail.com

PROJECT(OpenIGTLinkJHU)


OPTION(JHU_ROBOT_BUILD "Include the actual robot control source?" OFF)

IF(JHU_ROBOT_BUILD)
 ADD_DEFINITIONS(-DJHU_ROBOT_BUILD=1)
ENDIF(JHU_ROBOT_BUILD)

INCLUDE (CheckTypeSize)
  CHECK_TYPE_SIZE(int         CMAKE_SIZEOF_INT)
  CHECK_TYPE_SIZE(long        CMAKE_SIZEOF_LONG)
  CHECK_TYPE_SIZE("void*"     CMAKE_SIZEOF_VOID_P)
  CHECK_TYPE_SIZE(char        CMAKE_SIZEOF_CHAR)
  CHECK_TYPE_SIZE(short       CMAKE_SIZEOF_SHORT)
  CHECK_TYPE_SIZE(float       CMAKE_SIZEOF_FLOAT)
  CHECK_TYPE_SIZE(double      CMAKE_SIZEOF_DOUBLE)
  CHECK_TYPE_SIZE("long long" CMAKE_SIZEOF_LONG_LONG)
  CHECK_TYPE_SIZE("__int64"   CMAKE_SIZEOF___INT64)
  CHECK_TYPE_SIZE("int64_t"   CMAKE_SIZEOF_INT64_T) 
  
ADD_DEFINITIONS(-DIGTL_SIZEOF_CHAR=${CMAKE_SIZEOF_CHAR})
  ADD_DEFINITIONS(-DIGTL_SIZEOF_DOUBLE=${CMAKE_SIZEOF_DOUBLE})
  ADD_DEFINITIONS(-DIGTL_SIZEOF_FLOAT=${CMAKE_SIZEOF_FLOAT})
  ADD_DEFINITIONS(-DIGTL_SIZEOF_INT=${CMAKE_SIZEOF_INT})
  ADD_DEFINITIONS(-DIGTL_SIZEOF_LONG=${CMAKE_SIZEOF_LONG})
  ADD_DEFINITIONS(-DIGTL_SIZEOF_SHORT=${CMAKE_SIZEOF_SHORT})
  ADD_DEFINITIONS(-DIGTL_SIZEOF_FLOAT=${CMAKE_SIZEOF_FLOAT})
  ADD_DEFINITIONS(-DIGTL_SIZEOF_DOUBLE=${CMAKE_SIZEOF_DOUBLE}) 

 IF(CMAKE_SIZEOF_LONG_LONG)
    ADD_DEFINITIONS(-DIGTL_TYPE_USE_LONG_LONG=1)
    ADD_DEFINITIONS(-DIGTL_SIZEOF_LONG_LONG=${CMAKE_SIZEOF_LONG_LONG})
  ELSE(CMAKE_SIZEOF_LONG_LONG)
  IF(CMAKE_SIZEOF___INT64)
    ADD_DEFINITIONS(-DIGTL_TYPE_USE___INT64=1)
    ADD_DEFINITIONS(-DIGTL_SIZEOF___INT64=${CMAKE_SIZEOF___INT64})
  ELSE(CMAKE_SIZEOF___INT64)
  IF(CMAKE_SIZEOF_INT64_T)
    ADD_DEFINITIONS(-DIGTL_TYPE_USE_INT64_T=1)
    ADD_DEFINITIONS(-DIGTL_SIZEOF_INT64_T=${CMAKE_SIZEOF_INT64_T})
  ENDIF(CMAKE_SIZEOF_INT64_T)
  ENDIF(CMAKE_SIZEOF___INT64)
  ENDIF(CMAKE_SIZEOF_LONG_LONG) 
  
  

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


 # hack for cisst
 OPTION(CISST_HAS_SWIG_PYTHON "Build the Python wrappers for cisst with Swig." OFF)
 OPTION(CISST_HAS_SWIG_JAVA "Build the Java wrappers for cisst with Swig." OFF)
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
 common/igtlNetwork.cpp
 common/igtlMessage.cpp
)

SET(HEADERS_IGTL
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

IF(JHU_ROBOT_BUILD)
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
 )
ENDIF(JHU_ROBOT_BUILD)


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
)

SET(SOURCES_SLICER_SIM
 slicersim/main.cpp
 common/transmit/BRPtprMessagesProc.h
 common/transmit/BRPtprMessagesProc.cpp
 common/BRPtprMessages.h
 common/BRPtprMessages.cpp
 common/BRPTPRInterface.h
)


IF(WIN32)
 SET(BOOST_LIBS
  boost_thread-vc71-mt-gd-1_34_1
  boost_system-vc71-mt-gd-1_34_1
 )
ELSE(WIN32)
 SET(BOOST_LIBS
  boost_thread-gcc34-mt
  boost_system-gcc34-mt
 )
ENDIF(WIN32)

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

IF(JHU_ROBOT_BUILD)
 SUBDIRS(
  robot
 )
ENDIF(JHU_ROBOT_BUILD)

 ADD_EXECUTABLE(JHUslicerSim ${SOURCES_IGTL} ${HEADERS_IGTL} ${SOURCES_SLICER_SIM})
 ADD_EXECUTABLE(JHUrProxy ${SOURCES_IGTL} ${HEADERS_IGTL} ${SOURCES_PROXY})
 IF(JHU_ROBOT_BUILD)
   ADD_EXECUTABLE(JHUrobot ${SOURCES_ROBOT})
 ENDIF(JHU_ROBOT_BUILD)
 ADD_EXECUTABLE(JHUrobotSim ${SOURCES_ROBOT_SIM}) 


TARGET_LINK_LIBRARIES(JHUslicerSim
 ${LIBS}
)

TARGET_LINK_LIBRARIES(JHUrProxy
 ${LIBS}
)


IF(JHU_ROBOT_BUILD)
 TARGET_LINK_LIBRARIES(JHUrobot
  ${LIBS}
  ${CISST_LIBRARIES}
 )
ENDIF(JHU_ROBOT_BUILD)

TARGET_LINK_LIBRARIES(JHUrobotSim
 ${LIBS}
 ${CISST_LIBRARIES}
)

