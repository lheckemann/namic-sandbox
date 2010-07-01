# Install script for directory: /home/freak/Slicer3/Modules/NeuroEndoscopicNavigation

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/Slicer3/Modules/NeuroEndoscopicNavigation" TYPE FILE FILES
    "/home/freak/Slicer3/Modules/NeuroEndoscopicNavigation/vtkNeuroEndoscopicNavigationWin32Header.h"
    "/home/freak/Slicer3/Modules/NeuroEndoscopicNavigation/NeuroEndoscopicNavigationLib.h"
    "/home/freak/Slicer3/Modules/NeuroEndoscopicNavigation/vtkNeuroEndoscopicNavigationGUI.h"
    "/home/freak/Slicer3/Modules/NeuroEndoscopicNavigation/vtkNeuroEndoscopicNavigationLogic.h"
    "/home/freak/Slicer3/Modules/NeuroEndoscopicNavigation/vtkNeuroEndoscopicNavigationConfigure.h"
    "/home/freak/Slicer3/Modules/NeuroEndoscopicNavigation/vtkNeuroEndoscopicNavigationConfigure.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "RuntimeLibraries")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
    FILE(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so"
         RPATH "")
  ENDIF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules" TYPE SHARED_LIBRARY FILES "/home/freak/Slicer3/Modules/NeuroEndoscopicNavigation/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
    FILE(RPATH_REMOVE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "RuntimeLibraries")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "RuntimeLibraries")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
    FILE(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so"
         RPATH "")
  ENDIF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules" TYPE SHARED_LIBRARY FILES "/home/freak/Slicer3/Modules/NeuroEndoscopicNavigation/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
    FILE(RPATH_REMOVE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/Slicer3/Modules/libNeuroEndoscopicNavigation.so")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "RuntimeLibraries")

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/home/freak/Slicer3/Modules/NeuroEndoscopicNavigation/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/home/freak/Slicer3/Modules/NeuroEndoscopicNavigation/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
