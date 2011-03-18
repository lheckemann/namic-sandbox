PROJECT(SNRMRRobotController)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

INCLUDE (CheckIncludeFiles)

# --------------------------------------------------------------------------
# Build options

option(BUILD_SHARED "Build Shared Libraries" OFF)
option(USE_FBISIM   "Use FBISIM Libraries" ON)
option(USE_ARTAPI   "Use ARTLinux API" ON)
option(FULL_SCREEN  "Build for full screen program" OFF)
SET(FOX_LIBNAME "FOX-1.2" CACHE STRING "FOX Toolkit library name")


ADD_DEFINITIONS(-DHAVE_JPEG_H=1)
ADD_DEFINITIONS(-DHAVE_PNG_H=1)
ADD_DEFINITIONS(-DHAVE_TIFF_H=1)
ADD_DEFINITIONS(-DHAVE_ZLIB_H=1)
ADD_DEFINITIONS(-DHAVE_OPENGL)

if(NOT USE_ARTAPI)
  ADD_DEFINITIONS(-DWITHOUT_ART)
endif(NOT USE_ARTAPI)

if(FULL_SCREEN)
  ADD_DEFINITIONS(-DFULLSCREEN)
endif(FULL_SCREEN)

#ADD_DEFINITIONS(-DDEBUG)


if(USE_FBISIM)
  ADD_DEFINITIONS(-D_USE_FBISIM)
endif(USE_FBISIM)


# --------------------------------------------------------------------------
# Find OpenIGTLink

if(NOT OpenIGTLink_SOURCE_DIR)
  find_package(OpenIGTLink REQUIRED)
  include(${OpenIGTLink_USE_FILE})
endif(NOT OpenIGTLink_SOURCE_DIR)


# --------------------------------------------------------------------------
# External Libraries

find_path(FOX_INCLUDE_DIR
   NAMES FXApp.h
   PATH  /opt/local/include/fox-1.6
)
find_path(FOX_LINK_DIR
   NAMES libFOX-1.6.a
   PATH  /opt/local/lib
)


# --------------------------------------------------------------------------
# Include dirs

set(INC_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/newmat/
  ${CMAKE_CURRENT_SOURCE_DIR}/ZFrame/
  ${OpenIGTLink_INCLUDE_DIRS}
  ${FOX_INCLUDE_DIR}
)

if(USE_FBISIM)
  set(INC_DIR
    ${INC_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/fbisim/
  )  
endif(USE_FBISIM)

include_directories(
  ${INC_DIR}
)


# --------------------------------------------------------------------------
# Link directory
link_directories (
  ${CMAKE_CURRENT_BINARY_DIR}
  ${FOX_LINK_DIR}
)


# generate pisition independent code to link newmat static library
# to shared object
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


# --------------------------------------------------------------------------
# Sources

set(COMOBJ_SOURCES
  MrsvrSharedData.cpp
  MrsvrStatus.cpp
  MrsvrCommand.cpp
  MrsvrLog.cpp
  MrsvrRAS.cpp
)

set(IFOBJ_SOURCES
  MrsvrMessageServer.cpp
  MrsvrThread.cpp
  MrsvrMainWindow.cpp
  MrsvrPlotCanvas.cpp
  MrsvrZFrameRegistrationDialog.cpp
  MrsvrTransform.cpp
)

set(CTRLOBJ_SOURCES
  MrsvrDev.cpp
)

set(NEWMAT_SOURCES
  newmat/newmat9.cpp
  newmat/newmatrm.cpp
  newmat/nm_misc.cpp
  newmat/bandmat.cpp
  newmat/newmat1.cpp
  newmat/solution.cpp
  newmat/cholesky.cpp
  newmat/hholder.cpp
  newmat/newmat2.cpp
  newmat/newmatex.cpp
  newmat/newmat3.cpp
  newmat/sort.cpp
  newmat/evalue.cpp
  newmat/jacobi.cpp
  newmat/newmat4.cpp
  newmat/submat.cpp
  newmat/example.cpp
  newmat/newmat5.cpp
  newmat/svd.cpp
  newmat/myexcept.cpp
  newmat/newmat6.cpp
  newmat/newmatnl.cpp
  newmat/fft.cpp
  newmat/newmat7.cpp
  newmat/newfft.cpp
  newmat/newmat8.cpp
  newmat/ZLinAlg.cxx
)

set(ZFRAME_SOURCES
  ZFrame/ZFrameCalibration.cxx
)

if(USE_FBISIM)
  set(FBISIM_SOURCES
    fbisim/fbida_sim.c
    fbisim/fbidio_sim.c
    fbisim/fbipenc_sim.c
  )
endif(USE_FBISIM)

# --------------------------------------------------------------------------
# Libaries

if(BUILD_SHARED)
  if(USE_FBISIM)
    add_library(fbisim SHARED
      ${FBISIM_SOURCES}
    )
  endif(USE_FBISIM)
#  add_library(newmat SHARED
#    ${NEWMAT_SOURCES}
#  )
#  add_library(zframe SHARED
#    ${ZFRAME_SOURCES}
#  )
else(BUILD_SHARED)
  if(USE_FBISIM)
    add_library(fbisim
      ${FBISIM_SOURCES}
    )
  endif(USE_FBISIM)
#  add_library(newmat
#    ${NEWMAT_SOURCES}
#  )
#  add_library(zframe
#    ${ZFRAME_SOURCES}
#  )
endif(BUILD_SHARED)

#target_link_libraries(zframe
#    newmat
#)

add_library(MrsvrCom
    ${COMOBJ_SOURCES}
)

add_library(MrsvrIF
    ${IFOBJ_SOURCES}
    ${ZFRAME_SOURCES}
    ${NEWMAT_SOURCES}
)

target_link_libraries(MrsvrIF
    MrsvrCom    
)

add_library(MrsvrCtrl
    ${CTRLOBJ_SOURCES}
)

target_link_libraries(MrsvrCtrl
    MrsvrCom    
)


# --------------------------------------------------------------------------
# Build

add_executable(interface interface.cpp)
target_link_libraries(interface
  MrsvrCom
  MrsvrIF
  ${FOX_LIBNAME}
  OpenIGTLink
  pthread
  m
  )

set (CTRL_LIBS
  MrsvrCom
  MrsvrCtrl
  m
)  

if (USE_FBISIM)
  set (CTRL_LIBS
    ${CTRL_LIBS}
    fbisim
  )
else (USE_FBISIM)
  set (CTRL_LIBS
    ${CTRL_LIBS}
    gpg2000
    gpg3300
    gpg6204
    g2c
  )
endif (USE_FBISIM)

if(USE_ARTAPI)
  set (CTRL_LIBS
    ${CTRL_LIBS}
    /usr/lib/art_syscalls.o
  )
endif(USE_ARTAPI)

add_executable(controller controller.cpp)
target_link_libraries(controller
  ${CTRL_LIBS}
  )

#SET(EXECUTABLE_OUTPUT_PATH
#  bin
#)

