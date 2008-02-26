PROJECT(ScannerIO)

INCLUDE (CheckIncludeFiles)

IF(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  ADD_DEFINITIONS(-DPLATFORM_OSX)
  IF(CMAKE_SYSTEM_VERSION MATCHES "9\\.[0-9]*\\.[0-9]*")
    ADD_DEFINITIONS(-DPLATFORM_OSX_LEOPARD)
  ENDIF(CMAKE_SYSTEM_VERSION MATCHES "9\\.[0-9]*\\.[0-9]*")
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

IF(CMAKE_SYSTEM_NAME MATCHES Linux)
ADD_DEFINITIONS(-DPLATFORM_LINUX)
SET(LINUX 1)
SET(CMAKE_C_FLAGS -fPIC)
SET(CMAKE_CXX_FLAGS -fPIC)
ENDIF(CMAKE_SYSTEM_NAME MATCHES Linux)

IF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
ADD_DEFINITIONS(-DPLATFORM_WIN32)
SET(WIN32 1)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Windows")

INCLUDE_DIRECTORIES(
${CMAKE_CURRENT_SOURCE_DIR}/src/
${CMAKE_CURRENT_SOURCE_DIR}/src/igtlutil
libs/
)

ADD_DEFINITIONS(-DACE_AS_STATIC_LIBS)

OPTION(BUILD_SHARED "Build Shared Libraries" ON)

LINK_DIRECTORIES(
libs/ace
libs/newmat
)

SET(SOURCES
src/igtlutil/igtl_util.c
src/igtlutil/igtl_header.c
src/igtlutil/igtl_image.c
src/igtlutil/igtl_transform.c
src/igtlutil/crc32.c
src/igtlImageMessage.cxx
src/igtlTransformMessage.cxx
src/igtlMath.cxx
src/igtlMessageBase.cxx
src/igtlObject.cxx
src/Thread.cxx
src/AcquisitionBase.cxx
src/AcquisitionSimulator.cxx
src/AcquisitionGEExcite.cxx
src/TransferBase.cxx
src/TransferOpenIGTLink.cxx
src/ScannerBase.cxx
)

SUBDIRS(
libs/ace
libs/newmat
)

IF(BUILD_SHARED)
ADD_LIBRARY(ScannerIOLib SHARED
${SOURCES}
)
ELSE(BUILD_SHARED)
ADD_LIBRARY(ScannerIOLib
${SOURCES}
)
ENDIF(BUILD_SHARED)

SET(LIBS
${LIBS}
ACE
newmat
)

IF(NOT WIN32)
SET(LIBS
${LIBS}
ncurses
pthread
dl
)
IF(LINUX)
SET(EXTRA_LIBS
rt
)
ENDIF(LINUX)
ENDIF(NOT WIN32)

TARGET_LINK_LIBRARIES(ScannerIOLib
${LIBS}
)

ADD_EXECUTABLE(ScannerSim src/main.cxx)
ADD_EXECUTABLE(TrackerSim src/tracker.cxx)
ADD_EXECUTABLE(GEExciteIGTL src/GEExciteIGTL.cxx)

TARGET_LINK_LIBRARIES(ScannerSim
${EXTRA_LIBS}
ScannerIOLib
)

TARGET_LINK_LIBRARIES(TrackerSim
${EXTRA_LIBS}
ScannerIOLib
)

TARGET_LINK_LIBRARIES(GEExciteIGTL
${EXTRA_LIBS}
ScannerIOLib
)

SET(EXECUTABLE_OUTPUT_PATH
bin
)


