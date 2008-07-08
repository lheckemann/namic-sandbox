#INCLUDE(${OpenIGTLink_SOURCE_DIR}/Source/igtlCheckMacros.cmake)
#
#igtl_platform_check()
#igtl_type_check()

IF(OpenIGTLink_PLATFORM_WIN32)
  SET(LINK_LIBS
    ws2_32
    wsock32
  )
ELSE(OpenIGTLink_PLATFORM_WIN32)
  SET(LINK_LIBS
    pthread
    m
  )
ENDIF(OpenIGTLink_PLATFORM_WIN32)

SET(OpenIGTLink_SOURCES
  igtlClientSocket.cxx
  igtlConditionVariable.cxx
  igtlFastMutexLock.cxx
  igtlImageMessage.cxx
  igtlLightObject.cxx
  igtlMath.cxx
  igtlMessageBase.cxx
  igtlMultiThreader.cxx
  igtlMutexLock.cxx
  igtlOSUtil.cxx
  igtlObject.cxx
  igtlObjectFactoryBase.cxx
  igtlServerSocket.cxx
  igtlSimpleFastMutexLock.cxx
  igtlSocket.cxx
  igtlTransformMessage.cxx
  )

ADD_SUBDIRECTORY( igtlutil )

ADD_LIBRARY(OpenIGTLink
  ${OpenIGTLink_SOURCES}
  )

TARGET_LINK_LIBRARIES(OpenIGTLink
  igtlutil
  ${LINK_LIBS}
  )

FILE(GLOB __files1 "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${__files1}
  DESTINATION ${OpenIGTLink_INSTALL_INCLUDE_DIR}
  COMPONENT Development)

FILE(GLOB __files2 "${CMAKE_CURRENT_SOURCE_DIR}/igtlutil/*.h")
INSTALL(FILES ${__files2}
  DESTINATION ${OpenIGTLink_INSTALL_INCLUDE_DIR}/igtlutil
  COMPONENT Development)

INSTALL(TARGETS OpenIGTLink
  RUNTIME DESTINATION ${OpenIGTLink_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
  LIBRARY DESTINATION ${OpenIGTLink_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${OpenIGTLink_INSTALL_LIB_DIR} COMPONENT Development)
