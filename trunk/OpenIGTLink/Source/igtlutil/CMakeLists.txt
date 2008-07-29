SET(IGTLUTIL_HEADS
  igtl_header.h
  igtl_image.h
  igtl_transform.h
  igtl_status.h
  igtl_util.h
  igtl_types.h
  )

SET(IGTLUTIL_SRCS
  igtl_header.c
  igtl_image.c
  igtl_transform.c
  igtl_util.c
  )       

ADD_LIBRARY(igtlutil
  ${IGTLUTIL_SRCS}
  )

TARGET_LINK_LIBRARIES(igtlutil
  )

INSTALL(TARGETS igtlutil
  RUNTIME DESTINATION ${OpenIGTLink_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
  LIBRARY DESTINATION ${OpenIGTLink_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${OpenIGTLink_INSTALL_LIB_DIR} COMPONENT Development)
