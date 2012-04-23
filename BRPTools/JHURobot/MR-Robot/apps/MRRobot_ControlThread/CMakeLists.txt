#
# $Id: $
#


# name of project and executable
PROJECT(MRRobot_ControlThread)

OPTION(MRRobot_OldGUI "Use old GUI." OFF)

OPTION(MRRobot_HAS_PROXY "Use pipe interface to proxy." ON)

OPTION(MRRobot_HAS_IRE "Use IRE (Python shell)" ON)

# use CMake provided script to locate FLTK
FIND_PACKAGE(FLTK REQUIRED)

# add include paths
INCLUDE_DIRECTORIES(${MRRobot_ControlThread_SOURCE_DIR}
                    ${FLTK_INCLUDE_DIR})

# Find OpenIGTLink
find_package(OpenIGTLink)

if(OpenIGTLink_FOUND)
   message( STATUS "OpenIGTLink Found!" )
   include_directories(${OpenIGTLink_INCLUDE_DIRS})
   link_directories(${OpenIGTLink_LIBRARY_DIRS})
else(OpenIGTLink_FOUND)
   message( STATUS "OpenIGTLink not found" )
endif(OpenIGTLink_FOUND)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

set (SOURCE_FILES 
        ctfControlThread.cpp
        ctfControlThread.h
        BRPtprControl.cpp
        BRPtprControl.h
        BRPtprOpenTracker.cpp
        BRPtprOpenTracker.h
        BRPtprMessages.h
        igtlMessage.cpp
        ctfControl.cpp
        ctfControl.h
        ctfGainData.h
        BRPtprMessages.cpp
        ctfControlBase.h
        ctfControlBase.cpp
        igtl_util.c
        crc32.c)

if (MRRobot_HAS_PROXY)
    add_definitions(-DMRRobot_HAS_PROXY)
    set (SOURCE_FILES ${SOURCE_FILES}
        BRPtprRingBuffer.cpp
        BRPtprRingBuffer.h
        BRPTPRInterface.h
        BRPplatform.cpp
        igtl_header.c)
endif (MRRobot_HAS_PROXY)

set (REQUIRED_CISST_LIBS cisstCommon cisstCommonXML cisstVector 
                         cisstOSAbstraction cisstMultiTask)

if (MRRobot_HAS_IRE)
    add_definitions(-DMRRobot_HAS_IRE)
    set (SOURCE_FILES ${SOURCE_FILES}
        ireTask.cpp
        ireTask.h)
    set (REQUIRED_CISST_LIBS ${REQUIRED_CISST_LIBS}
                             cisstInteractive)
endif (MRRobot_HAS_IRE)

ADD_LIBRARY(MRRobot_ControlThreadLib SHARED ${SOURCE_FILES})

cisst_target_link_libraries(MRRobot_ControlThreadLib ${REQUIRED_CISST_LIBS})

# add include paths
include_directories(${MRRobot_ControlThread_SOURCE_DIR}
                    ${FLTK_INCLUDE_DIR})

# magic CMake command, generate .cxx and .h from .fl files
# then compile them and link them with the executable
IF(MRRobot_OldGUI)
FLTK_WRAP_UI(MRRobot_ControlThread Switcher-Old.fl Console-Old.fl GlobalPosition.fl )
ADD_DEFINITIONS(-DOLD_GUI)
ELSE(MRRobot_OldGUI)
FLTK_WRAP_UI(MRRobot_ControlThread Switcher.fl Console.fl GlobalPosition.fl)
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

add_custom_command(TARGET MRRobot_ControlThread POST_BUILD
    COMMAND ln ARGS -fns ${CMAKE_CURRENT_SOURCE_DIR}/XMLConfig/ MRRobot_ControlThreadXMLConfig
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# this program
target_link_libraries(MRRobot_ControlThread MRRobot_ControlThreadLib MRRobotDevices dscud5 ${FLTK_LIBRARIES} GL GLU dl util
                      ${sawLoPoMoCo_LIBRARIES})

if(OpenIGTLink_FOUND)
    target_link_libraries(MRRobot_ControlThread ${OpenIGTLink_LIBRARIES})
endif(OpenIGTLink_FOUND)

# link with the cisst libraries
cisst_target_link_libraries(MRRobot_ControlThread ${REQUIRED_CISST_LIBS})

# Allow generated c++ code to find header files
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# To link with build libraries
link_directories(${CMAKE_CURRENT_BINARY_DIR})

if(MRRobot_HAS_IRE)
  cisst_add_swig_module(MODULE MRRobot_ControlThreadLib
                        INTERFACE_FILENAME MRRobot_ControlThreadLibPython.i
                        INTERFACE_DIRECTORY .
                        MODULE_LINK_LIBRARIES MRRobot_ControlThreadLib)
endif(MRRobot_HAS_IRE)

if(CISST_HAS_LINUX_RTAI)
  target_link_libraries(MRRobot_ControlThread ${RTAI_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif(CISST_HAS_LINUX_RTAI)
