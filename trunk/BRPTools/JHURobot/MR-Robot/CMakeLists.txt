# See license at http://www.cisst.org/cisst/license.txt
# Find the cisst settings

PROJECT(MR-Robot)

FIND_FILE(CISST_CMAKE
          cisstBuild.cmake
          DOC "Location of your cisstBuild.cmake")

# Configure only if cisstBuild.cmake was found
IF(EXISTS ${CISST_CMAKE})
  INCLUDE("${CISST_CMAKE}")

  # Extend CMake Module Path to find cisst defined Macros
  SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CISST_CMAKE_DIR})
  INCLUDE(cisstMacros) # To use CISST_REQUIRES

  # Add the include and lib paths for cisst
  INCLUDE_DIRECTORIES(${CISST_INCLUDE_DIR})
  LINK_DIRECTORIES(${CISST_LIBRARY_DIR})

  # Swig requirements
  FIND_PACKAGE(SWIG REQUIRED)
  INCLUDE(${SWIG_USE_FILE})

  IF(CISST_HAS_SWIG_PYTHON)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
    # Python libraries
    IF(WIN32)
      LINK_LIBRARIES(debug ${PYTHON_DEBUG_LIBRARIES} optimized ${PYTHON_LIBRARIES})
    ELSE(WIN32)
      LINK_LIBRARIES(${PYTHON_LIBRARIES})
    ENDIF(WIN32)
  ENDIF(CISST_HAS_SWIG_PYTHON)

  # Threads for Python
  FIND_PACKAGE(Threads REQUIRED)

  INCLUDE_DIRECTORIES(${CISST_SWIG_INCLUDE_DIR})

  # Add the include path for Devices library
  INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libs/Devices/include)

  # Set path to dscud5 library (/pub/dscud5)
  SET(DSCUD5_DIR "" CACHE PATH "Directory for DSCUD5 library")
  INCLUDE_DIRECTORIES(${DSCUD5_DIR})
  LINK_DIRECTORIES(${DSCUD5_DIR})

  SUBDIRS(libs/Devices/code)
  SUBDIRS(apps/MRRobot_ControlThread)
  SUBDIRS(tests/DMM16AT_Test)
  SUBDIRS(tests/RMM1612_Test)
  SUBDIRS(tests/LoPoMoCoTest)
  SUBDIRS(tests/CompositeMRRobot_Test)

ELSE(EXISTS ${CISST_CMAKE})
  # Warn that we really need the cisst.cmake file
  MESSAGE("Can not configure unless CISST_CMAKE is provided")
ENDIF(EXISTS ${CISST_CMAKE})
