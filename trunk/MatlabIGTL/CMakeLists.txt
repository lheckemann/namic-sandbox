project(MatlabIGTL)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

find_program(MEX_COMPILER mex)

set(OpenIGTLinkMex_SOURCES
  igtlMexClientSocket.cpp
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
  -L${OpenIGTLink_DIR}/bin
  -L${CMAKE_CURRENT_BINARY_DIR}
)

set(LIB
  -lOpenIGTLink
  -lOpenIGTLinkMex
)

add_custom_target ( igtlopen ALL )
add_custom_target ( igtlclose ALL )
add_custom_target ( igtlsend ALL )
add_custom_target ( igtlreceive ALL)

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

