PROJECT(MRML)

INCLUDE_DIRECTORIES(
  ${MRML_BINARY_DIR}
  )

SET(MRML_SRCS
  mrmlCommand.cxx
  mrmlEventObject.cxx
  mrmlFastMutexLock.cxx
  mrmlIndent.cxx
  mrmlLightObject.cxx
  mrmlObject.cxx
  #mrmlObjectFactoryBase.cxx
  mrmlSimpleFastMutexLock.cxx
  mrmlTimeStamp.cxx
  )

INCLUDE (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)

CONFIGURE_FILE(${MRML_SOURCE_DIR}/mrmlConfigure.h.in 
               ${MRML_BINARY_DIR}/mrmlConfigure.h)

ADD_LIBRARY(mrml ${MRML_SRCS})

