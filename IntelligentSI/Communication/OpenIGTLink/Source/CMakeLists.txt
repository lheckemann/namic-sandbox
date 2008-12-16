IF(OpenIGTLink_PLATFORM_WIN32)
  SET(LINK_LIBS
    ws2_32
    wsock32
  )
ELSE(OpenIGTLink_PLATFORM_WIN32)
  IF(OpenIGTLink_PLATFORM_QNX)
    SET(LINK_LIBS
      c
      socket
      m
    )
  ELSE(OpenIGTLink_PLATFORM_QNX)
    SET(LINK_LIBS
      pthread
      m
    )
  ENDIF(OpenIGTLink_PLATFORM_QNX)
ENDIF(OpenIGTLink_PLATFORM_WIN32)

ADD_SUBDIRECTORY( igtlutil )

SET(OpenIGTLink_SOURCES
  igtlutil/igtl_header.c
  igtlutil/igtl_image.c
  igtlutil/igtl_transform.c
  igtlutil/igtl_status.c
  igtlutil/igtl_util.c
  igtlutil/igtl_vfixture.c
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
  igtlStatusMessage.cxx
  igtlTimeStamp.cxx
  igtlTransformMessage.cxx
  igtlVFixtureMessage.cxx
  igtlScalarValueMessage.cxx
  )

ADD_LIBRARY(OpenIGTLink
  ${OpenIGTLink_SOURCES}
  )

TARGET_LINK_LIBRARIES(OpenIGTLink
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
