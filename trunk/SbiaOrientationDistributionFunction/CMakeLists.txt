# This is the root ITK CMakeLists file.
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# This project is designed to be built outside the Insight source tree.
PROJECT(SBIAITK)
# setup output directories
SET ( RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/bin
    CACHE PATH
    "Single directory for all executables."
    )

# setup output directories
SET ( LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/lib
    CACHE PATH
    "Single directory for all Libraries."
    )

SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")

SET( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib CACHE PATH "Single output directory for building all executables.")

SET (CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH} )

SET (BUILD_TEST ON CACHE BOOL "Do you want to build tests")

# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

MARK_AS_ADVANCED (
    RUNTIME_OUTPUT_DIRECTORY
    LIBRARY_OUTPUT_DIRECTORY
    )
ENABLE_TESTING()

SET (SBIA_INCLUDE_DIRECTORY
  ${SBIAITK_SOURCE_DIR}/src/common
  ${SBIAITK_SOURCE_DIR}/src/filters
)
INCLUDE_DIRECTORIES(
  ${SBIA_INCLUDE_DIRECTORY}
)

ADD_SUBDIRECTORY(src)

IF (BUILD_TEST)
  ADD_SUBDIRECTORY(testing)
ENDIF (BUILD_TEST)
