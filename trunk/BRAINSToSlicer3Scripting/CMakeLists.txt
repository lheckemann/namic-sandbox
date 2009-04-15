# Build a testing suite for Slicer-3
# This directory will build up a series
# of tests that are designed to stress
# test the Slicer-3 architechture.
#
# For all cases where Slicer-3 has undesirable
# behavior (SegFaults, or Bus Errors) a test will
# be written.
#
PROJECT(BRAINS_SLICER3_TEST_SUITE)
cmake_minimum_required(VERSION 2.6)

ENABLE_TESTING()

FIND_PROGRAM(SLICER3_EXEC Slicer3 ../Slicer3-build ../Slicer3-bin ../../Slicer3-bin ../../../Slicer3-bin)

SET(CMAKE_MODULE_PATH ${BRAINS_SLICER3_TEST_SUITE_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

OPTION(BRAINS_TESTING_DATA "Determine if extended BRAINS test suite data 1.2GB should be found" ON)
IF(BRAINS_TESTING_DATA)
## Needed to find the 1.2GB of BRAINS Regression data needed for extensive testing.
FIND_PACKAGE(BrainsRegressionData)
    IF(NOT BrainsRegressionData_FOUND)
      MESSAGE(FATAL_ERROR "Cannot test brains2 without the regressiontest tree. Data can be downloaded with \"svn co https://www.nitrc.org/svn/brainstestdata/trunk regressiontest\"")
ENDIF(NOT BrainsRegressionData_FOUND)
ENDIF(BRAINS_TESTING_DATA)

SET(EXECUTABLE_OUTPUT_PATH ${BRAINS_SLICER3_TEST_SUITE_BINARY_DIR})

SUBDIRS(Common Shell Tcl Python Data)
