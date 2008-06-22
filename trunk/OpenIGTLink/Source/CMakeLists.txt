PROJECT( OpenIGTLink )

INCLUDE(${OpenIGTLink_SOURCE_DIR}/igtlCheckMacros.cmake)

igtl_platform_check()
igtl_type_check()


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
  ${CMAKE_CURRENT_SOURCE_DIR}/igtlutil
)

ADD_SUBDIRECTORY( igtlutil )

ADD_LIBRARY(OpenIGTLink
  ${OpenIGTLink_SOURCES}
  )

TARGET_LINK_LIBRARIES(OpenIGTLink
  igtlutil      
  )
