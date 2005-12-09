#
# Find the native SOViewer includes and libraries
#
# This module defines
#
# SOViewer_SOURCE_DIR - where is the source tree 
# SOViewer_BINARY_DIR - where is the binary tree 
#
#
# Look for a binary tree
# 

IF(CYGWIN)
FIND_PATH(
SOV_BINARY_DIR 
sovConfigure.h.in
${CMAKE_CURRENT_SOURCE_DIR}/../SOViewer-bin
${CMAKE_CURRENT_SOURCE_DIR}/../SOViewer-Bin
${CMAKE_CURRENT_SOURCE_DIR}/../SOViewer-Cygwin
)
ELSE(CYGWIN)

FIND_PATH(
SOV_BINARY_DIR 
sovConfigure.h.in
${CMAKE_CURRENT_SOURCE_DIR}/../SOViewer-VC++
${CMAKE_CURRENT_SOURCE_DIR}/../SOViewer-bin
${CMAKE_CURRENT_SOURCE_DIR}/../SOViewer-Bin
${CMAKE_CURRENT_SOURCE_DIR}/../SOViewer-Cygwin
${CMAKE_CURRENT_SOURCE_DIR}/../SOViewer-Linux
${CMAKE_CURRENT_SOURCE_DIR}/../SOViewerBorland
${CMAKE_CURRENT_SOURCE_DIR}/../SOViewer-Darwin
${CMAKE_CURRENT_SOURCE_DIR}/../SOViewer-SunOS


  # Read from the CMakeSetup registry entries.  It is likely that
  # ITK will have been recently built.
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild1]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild2]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild3]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild4]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild5]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild6]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild7]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild8]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild9]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild10]
)
ENDIF(CYGWIN)

IF(SOV_BINARY_DIR)
  SET(USE_SOV_FILE ${SOV_BINARY_DIR}/UseSOV.cmake)
ENDIF(SOV_BINARY_DIR)




