SET(IGTL_UTIL_PROTOCOL_VERSION "1")

SET(IGTLUTIL_HEADS
  igtl_header.h
  igtl_image.h
  igtl_transform.h
  igtl_util.h
  igtl_types.h
#  igtl_position.h
  )

SET(IGTLUTIL_SRCS
  igtl_header.c
  igtl_image.c
  igtl_transform.c
  igtl_util.c
#  igtl_position.c
  )       

ADD_LIBRARY(igtlutil STATIC
  ${IGTLUTIL_HEADS}
  ${IGTLUTIL_SRCS}
  )

#TARGET_LINK_LIBRARIES(igtlutil
#  ${LIBS}
#  )
