#
# $Id$
#


# name of project and executable
PROJECT(MRRobot_ControlThread)

OPTION(MRRobot_OldGUI "Use old GUI." OFF)

# use CMake provided script to locate FLTK
FIND_PACKAGE(FLTK REQUIRED)

# add include paths
INCLUDE_DIRECTORIES(${MRRobot_ControlThread_SOURCE_DIR}
                    ${FLTK_INCLUDE_DIR})

ADD_LIBRARY(MRRobot_ControlThreadLib SHARED
        ctfControlThread.cpp
        ireTask.cpp
        ctfControlThread.h
        ireTask.h
        BRPtprControl.cpp
        BRPtprControl.h
        BRPtprOpenTracker.cpp
        BRPtprOpenTracker.h
        BRPtprRingBuffer.cpp
        BRPtprRingBuffer.h
        BRPtprMessages.h
        ctfControl.cpp
        ctfControl.h
        ctfGainData.h
        BRPtprMessages.cpp
        ctfControlBase.h
        ctfControlBase.cpp
        BRPTPRInterface.h
        igtl_util.c
        crc32.c
        BRPplatform.cpp
        igtlMessage.cpp
        igtl_header.c
        )

CISST_REQUIRES("MRRobot_ControlThreadLib" "cisstCommon;cisstVector;cisstOSAbstraction;cisstInteractive;cisstMultiTask;cisstDevices")

# add include paths
INCLUDE_DIRECTORIES(${MRRobot_ControlThread_SOURCE_DIR}
                    ${FLTK_INCLUDE_DIR})

# magic CMake command, generate .cxx and .h from .fl files
# then compile them and link them with the executable
IF(MRRobot_OldGUI)
FLTK_WRAP_UI(MRRobot_ControlThread Switcher-Old.fl Console-Old.fl GlobalPosition.fl )
ADD_DEFINITIONS(-DOLD_GUI)
ELSE(MRRobot_OldGUI)
FLTK_WRAP_UI(MRRobot_ControlThread Switcher.fl Console.fl)
ENDIF(MRRobot_OldGUI)

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
    COMMAND ln ARGS -fns ${CMAKE_CURRENT_SOURCE_DIR}/XMLConfig/ MRRobot_ControlThreadXMLConfig
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# this program
TARGET_LINK_LIBRARIES(MRRobot_ControlThread MRRobot_ControlThreadLib MRRobotDevices dscud5 ${FLTK_LIBRARIES} GL GLU dl util)

# link with the cisst libraries
CISST_REQUIRES("MRRobot_ControlThread" "cisstCommon;cisstVector;cisstInteractive;cisstOSAbstraction;cisstMultiTask;cisstDevices")

# Allow generated c++ code to find header files
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

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
