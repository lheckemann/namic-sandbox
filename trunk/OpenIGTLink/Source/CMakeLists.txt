PROJECT( OpenIGTLink )

SET(OpenIGTLink_SOURCES
  igtlSocket.cxx
  )

ADD_SUBDIRECTORY( igtlutil )

ADD_LIBRARY(OpenIGTLink
  ${OpenIGTLink_SOURCES}
  )

TARGET_LINK_LIBRARIES(OpenIGTLink
  igtlutil      
  )
