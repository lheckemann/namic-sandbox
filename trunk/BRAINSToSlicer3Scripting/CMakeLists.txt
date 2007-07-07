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

ENABLE_TESTING()

FIND_PROGRAM(SLICER3_BIN Slicer3 ../Slicer3-bin ../../Slicer3-bin ../../../Slicer3-bin)
## Needed to find the 1.2GB of BRAINS Regression data needed for extensive testing.
#FIND_PACKAGE(BrainsRegressionData)
#    IF(NOT BrainsRegressionData_FOUND)
#      MESSAGE(FATAL_ERROR "Cannot test brains2 without the regressiontest tree.")
#ENDIF(NOT BrainsRegressionData_FOUND)

SET(EXECUTABLE_OUTPUT_PATH ${BRAINS_SLICER3_TEST_SUITE_BINARY_DIR})

SUBDIRS(Common Shell Tcl Python Data)
