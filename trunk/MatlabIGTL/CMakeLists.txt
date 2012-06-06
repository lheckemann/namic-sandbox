PROJECT(MatlabIGTL)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

## build for OpenIGTLink Version 2 (TDATA, POINT, BIND, etc.)
option(OpenIGTLink_PROTOCOL_VERSION_2 "Build Library for Protocol Version 2" OFF)

IF(OpenIGTLink_PROTOCOL_VERSION_2)
  SET(OpenIGTLink_PROTOCOL_VERSION "2")
ELSE(OpenIGTLink_PROTOCOL_VERSION_2)
  SET(OpenIGTLink_PROTOCOL_VERSION "1")
ENDIF(OpenIGTLink_PROTOCOL_VERSION_2)


## find the OpenIGTLink
FIND_PACKAGE(OpenIGTLink REQUIRED)
INCLUDE(${OpenIGTLink_USE_FILE})

##FIND_PACKAGE(Matlab REQUIRED) 
# this should be implemented in the future.
# However, this is needs to be combined with FindOctave.cmake.

## Find the mex program.  
IF(WIN32) #miktex has a mex program, so set the WIN32 to mex.bat.
    FIND_PROGRAM( MEX_COMPILER mex.bat )
ELSE()
    FIND_PROGRAM( MEX_COMPILER mex )
ENDIF()

### MEX compile only extensions are different.
# need to confirm this for all compiler versions.
IF (MSVC)
    SET(OBJ_EXT "obj")
ELSE()
    SET(OBJ_EXT "o")
ENDIF()

### Compiler (Matlab or Octave)
# if MEX_COMPILER is mkoctfile (for Octave)
IF (MEX_COMPILER MATCHES mkoctfile$)
    SET(MEX_OPTION --mex -g)
    SET(MEX_EXT "oct")
    SET(CLIENTSOCKET_OBJ_PATH_OPTION  -o ${CMAKE_CURRENT_BINARY_DIR}/igtlMexClientSocket.${OBJ_EXT})
    SET(SERVERSOCKET_OBJ_PATH_OPTION  -o ${CMAKE_CURRENT_BINARY_DIR}/igtlMexServerSocket.${OBJ_EXT})
    SET(SOCKET_OBJ_PATH_OPTION        -o ${CMAKE_CURRENT_BINARY_DIR}/igtlMexSocket.${OBJ_EXT})
ELSE(MEX_COMPILER MATCHES mkoctfile$)
    # get the mex file extension, i.e. *.mexw32
    IF( WIN32)
        FIND_PROGRAM( MEX_EXT_EXE mexext.bat )
    ELSE(WIN32)
        FIND_PROGRAM( MEX_EXT_EXE mexext )
    ENDIF(WIN32)
    #Run mexext to get the extension for this platform.
    EXECUTE_PROCESS(COMMAND ${MEX_EXT_EXE}
        OUTPUT_VARIABLE MEX_EXT
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    SET(CLIENTSOCKET_OBJ_PATH_OPTION  "")
    SET(SERVERSOCKET_OBJ_PATH_OPTION  "")
    SET(SOCKET_OBJ_PATH_OPTION        "")
ENDIF (MEX_COMPILER MATCHES mkoctfile$)

### Platform specific option
IF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  SET(LINK_LIBS
    -lws2_32
    -lwsock32
  )
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Windows")

IF(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")

IF(CMAKE_SYSTEM_NAME MATCHES "SunOS")
  SET(LINK_LIBS
    -lrt
    -lnsl
    -lsocket
  )
  IF(CMAKE_COMPILER_IS_GNUCXX)
     SET(LINK_LIBS
       -lstdc++
     )
  ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "SunOS")

FOREACH (dir ${OpenIGTLink_INCLUDE_DIRS})
  SET(INCDIR ${INCDIR} -I${dir})
ENDFOREACH(dir ${OpenIGTLink_INCLUDE_DIRS})

SET(INCDIR ${INCDIR} -I${CMAKE_CURRENT_SOURCE_DIR})

SET(LIBDIR
  -L${OpenIGTLink_LIBRARY_DIRS}
  -L${CMAKE_CURRENT_BINARY_DIR}
)

SET(LIB
  -lOpenIGTLink
  ${LINK_LIBS}
)

ADD_CUSTOM_TARGET ( igtlMexClientSocket ALL SOURCES igtlMexClientSocket.cpp igtlMexClientSocket.h)
ADD_CUSTOM_TARGET ( igtlMexServerSocket ALL SOURCES igtlMexServerSocket.cpp igtlMexServerSocket.h)
ADD_CUSTOM_TARGET ( igtlMexSocket ALL SOURCES igtlMexSocket.cpp igtlMexSocket.h)
ADD_CUSTOM_TARGET ( igtlopen ALL SOURCES igtlopen.cpp)
ADD_CUSTOM_TARGET ( igtlclose ALL SOURCES igtlclose.cpp)
ADD_CUSTOM_TARGET ( igtlsend ALL SOURCES igtlsend.cpp)
ADD_CUSTOM_TARGET ( igtlreceive ALL SOURCES igtlreceive.cpp)
ADD_CUSTOM_TARGET ( igtlwaitcon ALL SOURCES igtlwaitcon.cpp)


ADD_CUSTOM_COMMAND (
  TARGET igtlMexClientSocket
  PRE_BUILD
  COMMAND ${MEX_COMPILER}
  ARGS    ${MEX_OPTION} ${INCDIR} -c ${CMAKE_CURRENT_SOURCE_DIR}/igtlMexClientSocket.cpp ${CLIENTSOCKET_OBJ_PATH_OPTION}
)

ADD_CUSTOM_COMMAND (
  TARGET igtlMexServerSocket
  PRE_BUILD
  COMMAND ${MEX_COMPILER}
  ARGS    ${MEX_OPTION} ${INCDIR} -c ${CMAKE_CURRENT_SOURCE_DIR}/igtlMexServerSocket.cpp ${SERVERSOCKET_OBJ_PATH_OPTION}
)

ADD_CUSTOM_COMMAND (
  TARGET igtlMexSocket
  PRE_BUILD
  COMMAND ${MEX_COMPILER}
  ARGS    ${MEX_OPTION} ${INCDIR} -c ${CMAKE_CURRENT_SOURCE_DIR}/igtlMexSocket.cpp ${SOCKET_OBJ_PATH_OPTION}
)

ADD_CUSTOM_COMMAND (
  TARGET  igtlopen
  PRE_BUILD
  COMMAND ${MEX_COMPILER}
  ARGS    ${MEX_OPTION} ${INCDIR} ${LIBDIR} ${LIB} ${CMAKE_CURRENT_SOURCE_DIR}/igtlopen.cpp igtlMexClientSocket.${OBJ_EXT} igtlMexServerSocket.${OBJ_EXT} igtlMexSocket.${OBJ_EXT}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}igtlopen.cpp 
)
add_dependencies(igtlopen igtlMexClientSocket igtlMexServerSocket igtlMexSocket OpenIGTLink)

add_custom_command (
  TARGET  igtlclose
  PRE_BUILD
  COMMAND ${MEX_COMPILER}
  ARGS    ${MEX_OPTION} ${INCDIR} ${LIBDIR} ${LIB} ${CMAKE_CURRENT_SOURCE_DIR}/igtlclose.cpp igtlMexClientSocket.${OBJ_EXT} igtlMexServerSocket.${OBJ_EXT} igtlMexSocket.${OBJ_EXT}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}igtlclose.cpp
)
add_dependencies(igtlclose igtlMexClientSocket igtlMexServerSocket igtlMexSocket OpenIGTLink)

add_custom_command (
  TARGET  igtlsend
  PRE_BUILD
  COMMAND ${MEX_COMPILER}
  ARGS    ${MEX_OPTION} ${INCDIR} ${LIBDIR} ${LIB} ${CMAKE_CURRENT_SOURCE_DIR}/igtlsend.cpp igtlMexClientSocket.${OBJ_EXT} igtlMexServerSocket.${OBJ_EXT} igtlMexSocket.${OBJ_EXT}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}igtlsend.cpp
)
add_dependencies(igtlsend igtlMexClientSocket igtlMexServerSocket igtlMexSocket OpenIGTLink)

add_custom_command (
  TARGET  igtlreceive
  PRE_BUILD
  COMMAND ${MEX_COMPILER}
  ARGS    ${MEX_OPTION} ${INCDIR} ${LIBDIR} ${LIB} ${CMAKE_CURRENT_SOURCE_DIR}/igtlreceive.cpp igtlMexClientSocket.${OBJ_EXT} igtlMexServerSocket.${OBJ_EXT} igtlMexSocket.${OBJ_EXT}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}igtlreceive.cpp
)
add_dependencies(igtlreceive igtlMexClientSocket igtlMexServerSocket igtlMexSocket OpenIGTLink)

add_custom_command (
  TARGET  igtlwaitcon
  PRE_BUILD
  COMMAND ${MEX_COMPILER}
  ARGS    ${MEX_OPTION} ${INCDIR} ${LIBDIR} ${LIB} ${CMAKE_CURRENT_SOURCE_DIR}/igtlwaitcon.cpp igtlMexClientSocket.${OBJ_EXT} igtlMexServerSocket.${OBJ_EXT} igtlMexSocket.${OBJ_EXT}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}igtlwaitcon.cpp
)
add_dependencies(igtlwaitcon igtlMexClientSocket igtlMexServerSocket igtlMexSocket OpenIGTLink)

# INSTALL
INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/igtlopen.${MEX_EXT} DESTINATION lib )
INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/igtlclose.${MEX_EXT} DESTINATION lib )
INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/igtlsend.${MEX_EXT} DESTINATION lib )
INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/igtlreceive.${MEX_EXT} DESTINATION lib )
INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/igtlwaitcon.${MEX_EXT} DESTINATION lib )
