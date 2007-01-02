PROJECT(WFEngine)
SET(SRCS
WFBaseSocket.cxx
WFServerConnection.cxx
WFClientConnection.cxx
WFSocketCollection.cxx)

#FIND_PACKAGE(Threads)

ADD_LIBRARY(WFInterface ${SRCS})

TARGET_LINK_LIBRARIES(WFInterface)
