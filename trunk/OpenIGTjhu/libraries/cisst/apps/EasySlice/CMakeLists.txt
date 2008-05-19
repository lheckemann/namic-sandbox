#
# $Id: CMakeLists.txt,v 1.2 2005/08/04 15:15:50 anton Exp $
#

# backward compatibility
CMAKE_MINIMUM_REQUIRED(VERSION 2.0)
PROJECT(EasySlice)


# Where are the cisst libraries
SET(CISST_CMAKE
    ${cisstTests_BINARY_DIR}/../libs/cisstBuild.cmake
    CACHE FILE
    "Location of your cisstBuild.cmake or cisstInstalled.cmake")
MARK_AS_ADVANCED(CLEAR CISST_CMAKE)
INCLUDE(${CISST_CMAKE})
INCLUDE_DIRECTORIES(${CISST_INCLUDE_DIR})
LINK_DIRECTORIES(${CISST_LIBRARY_DIR})

# See if the libraries are DLLs
IF(CISST_BUILD_SHARED_LIBS)
  ADD_DEFINITIONS(-DCISST_DLL)
ENDIF(CISST_BUILD_SHARED_LIBS)


# Find wxWidgets
FIND_PACKAGE(wxWidgets REQUIRED)
IF(WXWINDOWS_INCLUDE_DIR)
  INCLUDE_DIRECTORIES(${WXWINDOWS_INCLUDE_DIR})
ELSE(WXWINDOWS_INCLUDE_DIR)
  SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${CMAKE_WXWINDOWS_CXX_FLAGS})
ENDIF(WXWINDOWS_INCLUDE_DIR)
LINK_DIRECTORIES(${WXWINDOWS_LINK_DIRECTORIES})


# Find dcmtk
FIND_PACKAGE(DCMTK REQUIRED)
INCLUDE_DIRECTORIES(${DCMTK_INCLUDE_DIR})
ADD_DEFINITIONS(-DHAVE_CONFIG_H)


# Source files
SET(EasySlice_SOURCES #eslMain.cpp)
                      eslApplication.cpp eslFrame.cpp eslCanvas.cpp
                      eslApplication.h   eslFrame.h   eslCanvas.h   eslImageInformation.h)
# Header files
# SET(EasySlice_HEADERS )

# Which program to build
ADD_EXECUTABLE(EasySlice ${EasySlice_SOURCES} ${EasySlice_HEADERS})

# Add the libraries to link
CISST_REQUIRES("EasySlice" "cisstCommon;cisstVector")
TARGET_LINK_LIBRARIES(EasySlice ${WXWINDOWS_LIBRARIES} ${DCMTK_LIBRARIES})


#
# $Log: CMakeLists.txt,v $
# Revision 1.2  2005/08/04 15:15:50  anton
# imported into cvstrac
#
# Revision 1.1.1.1  2005/07/29 21:30:45  anton
# Creation
#
#
