
INCLUDE(${OpenIGTLink_SOURCE_DIR}/Source/igtlCheckMacros.cmake)

igtl_platform_check()
igtl_type_check()

IF(WIN32)
  SET(LINK_LIBS
    ws2_32
    wsock32
  )
ELSE(WIN32)
  SET(LINK_LIBS
    pthread
    m
  )
ENDIF(WIN32)

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

INCLUDE_DIRECTORIES(
  ${OpenIGTLink_SOURCE_DIR}/Source/igtlutil
)

ADD_SUBDIRECTORY( igtlutil )

ADD_LIBRARY(OpenIGTLink
  ${OpenIGTLink_SOURCES}
  )

TARGET_LINK_LIBRARIES(OpenIGTLink
  igtlutil
  ${LINK_LIBS}
  )
