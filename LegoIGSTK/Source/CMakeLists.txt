#
# Sources
#
SET(CMAKE_CXX_FLAGS
  "-funsigned-char"
)

#
# Build the library
#
ADD_LIBRARY(LegoIGSTK
  ../../LegoTutorial/NXT_USB.cxx
  ../../LegoTutorial/NXT_USB_linux.cxx
  )

TARGET_LINK_LIBRARIES(LegoIGSTK
  ${USB_LIBRARIES}
  )


