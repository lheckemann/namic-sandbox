project(MatlabIGTL)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

find_program(MEX_COMPILER mex)


### Compiler (Matlab or Octave)
# if MEX_COMPILER is mkoctfile (for Octave)
if (MEX_COMPILER MATCHES mkoctfile$)
  set(MEX_OPTION --mex -g)
endif (MEX_COMPILER MATCHES mkoctfile$)


### Platform specific option
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  SET(LINK_LIBS
    -lws2_32
    -lwsock32
  )
endif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
endif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
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
endif(CMAKE_SYSTEM_NAME MATCHES "SunOS")


set(OpenIGTLinkMex_SOURCES
  igtlMexClientSocket.cpp
  igtlMexServerSocket.cpp
  igtlMexSocket.cpp
)

add_library(OpenIGTLinkMex
  ${OpenIGTLinkMex_SOURCES}
)

set(LIBS
  ${MATLAB_LIBRARIES}
  OpenIGTLink
  OpenIGTLinkMex
)  


set(INCDIR
  -I${OpenIGTLink_SOURCE_DIR}/Source
  -I${OpenIGTLink_DIR}
)

set(LIBDIR
  -L${OpenIGTLink_LIBRARY_DIRS}/${CMAKE_CFG_INTDIR}
  -L${CMAKE_CURRENT_BINARY_DIR}
)

set(LIB
  -lOpenIGTLink
  -lOpenIGTLinkMex
  ${LINK_LIBS}
)


add_custom_target ( igtlopen ALL )
add_custom_target ( igtlclose ALL )
add_custom_target ( igtlsend ALL )
add_custom_target ( igtlreceive ALL )
add_custom_target ( igtlwaitcon ALL )

add_custom_command (
  TARGET  igtlopen
  PRE_BUILD
  COMMAND ${MEX_COMPILER}
  ARGS    ${MEX_OPTION} ${INCDIR} ${LIBDIR} ${LIB} ${CMAKE_CURRENT_SOURCE_DIR}/igtlopen.cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}igtlopen.cpp
)

add_custom_command (
  TARGET  igtlclose
  PRE_BUILD
  COMMAND ${MEX_COMPILER}
  ARGS    ${MEX_OPTION} ${INCDIR} ${LIBDIR} ${LIB} ${CMAKE_CURRENT_SOURCE_DIR}/igtlclose.cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}igtlclose.cpp
)

add_custom_command (
  TARGET  igtlsend
  PRE_BUILD
  COMMAND ${MEX_COMPILER}
  ARGS    ${MEX_OPTION} ${INCDIR} ${LIBDIR} ${LIB} ${CMAKE_CURRENT_SOURCE_DIR}/igtlsend.cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}igtsend.cpp
)

add_custom_command (
  TARGET  igtlreceive
  PRE_BUILD
  COMMAND ${MEX_COMPILER}
  ARGS    ${MEX_OPTION} ${INCDIR} ${LIBDIR} ${LIB} ${CMAKE_CURRENT_SOURCE_DIR}/igtlreceive.cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}igtlreceive.cpp
)

add_custom_command (
  TARGET  igtlwaitcon
  PRE_BUILD
  COMMAND ${MEX_COMPILER}
  ARGS    ${MEX_OPTION} ${INCDIR} ${LIBDIR} ${LIB} ${CMAKE_CURRENT_SOURCE_DIR}/igtlwaitcon.cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}igtlreceive.cpp
)

