PROJECT(MattesMetricCachingTest)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

IF(NOT EXECUTABLE_OUTPUT_PATH)
  SET (EXECUTABLE_OUTPUT_PATH ${MattesMetricCachingTest_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
ENDIF(NOT EXECUTABLE_OUTPUT_PATH)

ENABLE_TESTING()

SET( REPORT03_NEW_IMPLEMENTATION_GRID_SIZE_LIST "4;8;16;32;48;64;80;96;100" CACHE STRING "BSpline grid sizes to test for Report03. Each entry is one side of the grid." )
SET( REPORT03_OLD_IMPLEMENTATION_GRID_SIZE_LIST "4;8;16" CACHE STRING "BSpline grid sizes to test for Report03. Each entry is one side of the grid." )
SET( REPORT03_NUMBER_OF_SPATIAL_SAMPLES 1638 CACHE STRING "Number of spatial samples for experiments in Report03." )
SET( REPORT03_VOLUME_SIZE 100 CACHE STRING "Volume size for experiments in Report03." )
SET( REPORT03_NUMBER_OF_CALLS 1 CACHE STRING "Number of repeated calls to GetValueAndDerivative.")

# SIZE MUST BE LARGER THAN 32x32x16 (B-spline grid size)
SET( REPORT02_VOLUME_SIZE_LIST "32;48" CACHE STRING "Volume sizes to test for Report02" )
SET( SIZE_LIST 32;48 )

#SET( SIZE_LIST 8 16 32 )
#SET( SIZE_LIST 32 50 70 80 90 100 110 )
#SET( SIZE_LIST 256 )
SET(NUMBER_OF_CALLS 1)

SUBDIRS(
  Source
  Testing
  Documents
  )
