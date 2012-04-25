# See license at http://www.cisst.org/cisst/license.txt
# Find the cisst settings

project(MR-Robot)

cmake_minimum_required(VERSION 2.8)

option(MRRobot_HAS_IRE "Use IRE (Python shell)" ON)

find_package(cisst REQUIRED)

find_package(sawLoPoMoCo REQUIRED HINTS "${CISST_BINARY_DIR}/saw/components")

if(cisst_FOUND AND sawLoPoMoCo_FOUND)
  # Modify CMake configuration to use cisst
  include(${CISST_USE_FILE})

  include_directories(${sawLoPoMoCo_INCLUDE_DIR})
  link_directories(${sawLoPoMoCo_LIBRARY_DIR})

  # Add the include path for Devices library
  include_directories(${CMAKE_SOURCE_DIR}/libs/Devices/include)

  # Set path to dscud5 library (/pub/dscud5)
  set(DSCUD5_DIR "" CACHE PATH "Directory for DSCUD5 library")
  include_directories(${DSCUD5_DIR})
  link_directories(${DSCUD5_DIR})

  add_subdirectory(libs/Devices/code)
  add_subdirectory(apps/MRRobot_ControlThread)
  if (MRRobot_HAS_IRE)
    # These tests are meaningless unless the IRE is available
    add_subdirectory(tests/DMM16AT_Test)
    add_subdirectory(tests/RMM1612_Test)
    add_subdirectory(tests/LoPoMoCoTest)
    add_subdirectory(tests/CompositeMRRobot_Test)
  endif (MRRobot_HAS_IRE)

endif(cisst_FOUND AND sawLoPoMoCo_FOUND)
