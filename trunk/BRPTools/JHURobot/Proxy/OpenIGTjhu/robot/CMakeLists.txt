# use CMake provided script to locate FLTK
FIND_PACKAGE(FLTK REQUIRED)

# add include paths
INCLUDE_DIRECTORIES(
 /usr/realtime/include
 ${MRRobot_ControlThread_SOURCE_DIR}
 ${FLTK_INCLUDE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/common/
 ${CMAKE_CURRENT_SOURCE_DIR}/common/control
 ${CMAKE_CURRENT_SOURCE_DIR}/common/transmit
 ${IGT_UTIL_DIR}
)

# Swig requirements
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

# Python libraries
IF(WIN32)
  LINK_LIBRARIES(debug ${PYTHON_DEBUG_LIBRARIES} optimized ${PYTHON_LIBRARIES})
ELSE(WIN32)
  LINK_LIBRARIES(${PYTHON_LIBRARIES})
ENDIF(WIN32)

# Threads for Python
FIND_PACKAGE(Threads REQUIRED)

# Create SineGenerator as a shared library
ADD_LIBRARY(MRRobot_ControlThreadLib SHARED
        ctfControlThread.cpp
        ireTask.cpp
        ctfControlThread.h
        ireTask.h
        ../common/control/BRPtprControl.cpp
        ../common/control/BRPtprControl.h
        ../common/control/BRPtprOpenTracker.cpp
        ../common/control/BRPtprOpenTracker.h
        #BRPtprRingBuffer.cpp
        #BRPtprRingBuffer.h
        ../common/BRPtprMessages.h
        ../common/control/ctfControl.cpp
        ../common/control/ctfControl.h
        ../common/BRPtprMessages.cpp
        ../common/control/ctfControlBase.h
        ../common/control/ctfControlBase.cpp
        ../common/BRPTPRInterface.h
        ../common/BRPplatform.cpp
        ../common/BRPplatform.h

        ${IGT_UTIL_DIR}/igtl_header.c
 ${IGT_UTIL_DIR}/igtl_header.h
 ${IGT_UTIL_DIR}/igtl_util.c
 ${IGT_UTIL_DIR}/igtl_util.h
 ../common/igtlMessage.cpp
 ../common/igtlMessage.h
 ${IGT_UTIL_DIR}/crc32.c
 ${IGT_UTIL_DIR}/crc32.h
        )

CISST_REQUIRES("MRRobot_ControlThreadLib" "cisstCommon;cisstVector;cisstOSAbstraction;cisstDeviceInterface;cisstRealTime")


# add include paths
INCLUDE_DIRECTORIES(${MRRobot_ControlThread_SOURCE_DIR}
                    ${FLTK_INCLUDE_DIR})

# magic CMake command, generate .cxx and .h from .fl files
# then compile them and link them with the executable
FLTK_WRAP_UI(MRRobot_ControlThread Switcher.fl Console.fl)

# name the main executable and specifies with source files to use
ADD_EXECUTABLE(MRRobot_ControlThread
               # code files to compile
           ctfMainUIThread.cpp
           main.cpp
           ${MRRobot_ControlThread_FLTK_UI_SRCS}
               # header files are provided for information
           ctfMainUIThread.h
           ctfMainUIIncludes.h
          )

ADD_CUSTOM_COMMAND(TARGET MRRobot_ControlThread POST_BUILD
    COMMAND ln ARGS -fs ../examples/MRRobot_ControlThread/XMLConfig/ MRRobot_ControlThreadXMLConfig
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# this program
TARGET_LINK_LIBRARIES(MRRobot_ControlThread MRRobot_ControlThreadLib ${FLTK_LIBRARIES}  GL GLU dl util)

# link with the cisst libraries
CISST_REQUIRES("MRRobot_ControlThread" "cisstCommon;cisstVector;cisstInteractive;cisstOSAbstraction;cisstDeviceInterface;cisstRealTime")

# Allow generated c++ code to find header files
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CISST_SWIG_INCLUDE_DIR})

# To link with build libraries
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})


# Run swig for each interface file
SET(MODULES MRRobot_ControlThreadLib)

FOREACH(module ${MODULES})
  SET(interface ${CMAKE_CURRENT_SOURCE_DIR}/${module}Python.i)
  # We are using C++ code
  SET_SOURCE_FILES_PROPERTIES(${interface} PROPERTIES CPLUSPLUS ON)
  # Set the verbose flag
  SET_SOURCE_FILES_PROPERTIES(${interface}
                              PROPERTIES SWIG_FLAGS "-v;-modern;-fcompact;-fvirtual")
  SWIG_ADD_MODULE(${module}Python python ${interface})
  SWIG_LINK_LIBRARIES(${module}Python ${module})
  # Copy the .py file generated to wherever the libraries are
  ADD_CUSTOM_COMMAND(TARGET _${module}Python
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND}
                     ARGS -E copy
                             ${CMAKE_CURRENT_BINARY_DIR}/${module}Python.py
                             ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${module}Python.py)
  # Build list of files to clean
  SET(CISST_CLEAN_FILES ${CISST_CLEAN_FILES}
                        ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${module}Python.py
                        ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${module}Python.pyc
                        ${module}Python_wrap.cxx)
ENDFOREACH(module)

IF(CISST_HAS_LINUX_RTAI)
  TARGET_LINK_LIBRARIES(MRRobot_ControlThread ${RTAI_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
ENDIF(CISST_HAS_LINUX_RTAI)

#
# $Log: CMakeLists.txt,v $
# Revision 1.6  2006/07/09 04:05:08  kapoor
# cissstRobot: Flushing the current state of pre-robot libraries for safe keeping. Use at own risk.
# Adds New eye robot and original black steady hand (7 DoF) kinematics.
#
# Revision 1.5  2006/06/03 00:38:55  kapoor
# cisstRobot: Flushing the current state of pre-robot libraries for safe keeping.
# Use at own risk.
#
# Revision 1.4  2006/05/27 03:46:29  kapoor
# cisstMRC: more code in examples, this should eventually make its way into
# the main library. For now it is checked in as examples.
#
# Revision 1.3  2006/05/14 03:26:28  kapoor
# cisstDeviceInterface: Cleanup of controlThread and addition of IRE to this
# example. Also location of XML files is now in a more decent location.
#
# Revision 1.2  2005/08/28 16:44:51  kapoor
# main.cpp
#
# Revision 1.1  2005/04/16 18:27:28  kapoor
# checking in controlthread code using FLTK for safe keeping
#
#
