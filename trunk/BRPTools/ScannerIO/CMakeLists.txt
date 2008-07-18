PROJECT(ScannerIO)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

INCLUDE (CheckIncludeFiles)

# --------------------------------------------------------------------------
# Find Open IGT Link

if(NOT OpenIGTLink_SOURCE_DIR)
  find_package(OpenIGTLink REQUIRED)
  include(${OpenIGTLink_USE_FILE})
endif(NOT OpenIGTLink_SOURCE_DIR)


# --------------------------------------------------------------------------
# External Libraries

find_path(RSP_FILES
   NAMES rspProxyUtils.o
   PATH  ${ROOT_DIR}/../GEFiles/GE_Excite_v14
)

if(RSP_FILES)
  ADD_DEFINITIONS(-D_RSP_CONTROL)
endif(RSP_FILES)


# --------------------------------------------------------------------------
# Include dirs

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/
  ${CMAKE_CURRENT_SOURCE_DIR}/libs/
  ${OpenIGTLink_INCLUDE_DIRS}  
  ${RSP_FILES}/GEMS_Include_11.03/
  ${RSP_FILES}/./
)


# --------------------------------------------------------------------------
# Build options

option(BUILD_SHARED "Build Shared Libraries" ON)

# generate pisition independent code to link newmat static library
# to shared object
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


# --------------------------------------------------------------------------
# Sources

set(SOURCES
  src/io_base.c
  src/io_signa5.c
  src/Thread.cxx
  src/AcquisitionBase.cxx
  src/AcquisitionSimulator.cxx
  src/AcquisitionTrackingSimulator.cxx
  src/AcquisitionSignaSPSimulator.cxx
  src/AcquisitionGEExcite.cxx
  src/TransferBase.cxx
  src/TransferOpenIGTLink.cxx
  src/ScannerBase.cxx
)

# --------------------------------------------------------------------------
# Link settings

link_directories(
  libs/newmat
  ${RSP_FILES}/GEMS_Libs_11.03
  ${RSP_FILES}
)

if(RSP_FILES)
  set(LIBS
    ${LIBS}
    mrmailc
    CnvEndianBase
    eventmgr
    cupfound
    cupipc
    msghand
    gemshid
    readableString
  )
  if(EXISTS ${RSP_FILES}/GEMS_Include_11.03)
    set(SOURCES
      ${SOURCES}
      ${RSP_FILES}/rsp_ops.c
      ${RSP_FILES}/rspProxyUtil.c
    )
  else(EXISTS ${RSP_FILES}/GEMS_Include_11.03)
    set(LINK_W_O_FILES TRUE)
  endif(EXISTS ${RSP_FILES}/GEMS_Include_11.03)
endif(RSP_FILES)

if(BUILD_SHARED)
  add_library(ScannerIOLib SHARED
    ${SOURCES}
  )
else(BUILD_SHARED)
  add_library(ScannerIOLib
    ${SOURCES}
  )
endif(BUILD_SHARED)

set(LIBS
  ${LIBS}
  newmat
  ${OpenIGTLink_LIBRARIES}
)

if(WIN32)
  set(LIBS
    ${LIBS}
    ws2_32
    wsock32
  )
else(WIN32)
  set(LIBS
    ${LIBS}
    ncurses
    pthread
    dl
  )
  if(LINUX)
    set(EXTRA_LIBS
      rt
    )
  endif(LINUX)
endif(WIN32)


target_link_libraries(ScannerIOLib
  ${LIBS}
)

if(LINK_W_O_FILES)
  set_target_properties(GEExciteIGTL
    PROPERTIES LINK_FLAGS "${RSP_FILES}/rspProxyUtil.o ${RSP_FILES}/rsp_ops.o"
  )
endif(LINK_W_O_FILES)


# --------------------------------------------------------------------------
# Sub directories

subdirs(
  libs/newmat
)


# --------------------------------------------------------------------------
# Build

add_executable(ScannerSim src/simulator.cxx)
add_executable(SignaSPSim src/signasp.cxx)
add_executable(TrackerSim src/tracker.cxx)
add_executable(GEExciteIGTL src/GEExciteIGTL.cxx)

target_link_libraries(ScannerSim
  ${EXTRA_LIBS}
  ScannerIOLib
)

target_link_libraries(SignaSPSim
  ${EXTRA_LIBS}
  ScannerIOLib
)

target_link_libraries(TrackerSim
  ${EXTRA_LIBS}
  ScannerIOLib
)

target_link_libraries(GEExciteIGTL
  ${EXTRA_LIBS}
  ScannerIOLib
)

SET(EXECUTABLE_OUTPUT_PATH
bin
)


