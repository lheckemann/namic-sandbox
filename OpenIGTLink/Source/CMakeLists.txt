PROJECT( OpenIGTLink )

SET(OpenIGTLink_SOURCES
  igtlSocket.cxx
  )

SUBDIRS(
  igtlutil
  )

ADD_LIBRARY(OpenIGTLink
  ${OpenIGTLink_SOURCES}
  )

TARGET_LINK_LIBRARIES(OpenIGTLink
  igtlutil      
  )
