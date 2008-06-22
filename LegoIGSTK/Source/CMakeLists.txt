#
# Sources
#
SET(CMAKE_CXX_FLAGS
  "-funsigned-char"
)

#
# Include dirs
#
INCLUDE_DIRECTORIES(
  ${LegoIGSTK_SOURCE_DIR}
  ${LegoIGSTK_SOURCE_DIR}/../../LegoTutorial
  )

#
# Build the library
#
ADD_LIBRARY(LegoIGSTK
  ../../LegoTutorial/NXT_USB.cxx
  ../../LegoTutorial/NXT_USB_linux.cxx
  )

INCLUDE(FindUSB.cmake)

TARGET_LINK_LIBRARIES(LegoIGSTK
  ${LIBUSB_LIB}
  )


