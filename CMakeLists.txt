PROJECT( NAMICSandBox )

SET (LIBRARY_OUTPUT_PATH ${NAMICSandBox_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all libraries." FORCE)
SET (EXECUTABLE_OUTPUT_PATH ${NAMICSandBox_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all executables." FORCE)

# Add the testing directories
OPTION(NAMICSandBox_BUILD_TESTING "Test the project" ON)
IF(NAMICSandBox_BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
ENDIF(NAMICSandBox_BUILD_TESTING)

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

IF(NOT NAMICSandBox_SETUP_DASHBOARD)

#
#  FIND ITK
#
FIND_PACKAGE(ITK)
IF (USE_ITK_FILE)
  INCLUDE (${USE_ITK_FILE})
ELSE (USE_ITK_FILE)
  MESSAGE( FATAL_ERROR "This application requires ITK. One of these components is missing. Please verify configuration")
ENDIF (USE_ITK_FILE)


#
# FIND VTK
#
FIND_PACKAGE(VTK)
IF (USE_VTK_FILE)
  INCLUDE (${USE_VTK_FILE})
ELSE (USE_VTK_FILE)
  MESSAGE( FATAL_ERROR "This application requires VTK. One of these components is missing. Please verify configuration")
ENDIF (USE_VTK_FILE)


# The NAMICSandBox Library and any Utilities that it requires
SUBDIRS(
  SpectralClustering
  SlicerTractClusteringImplementation
  Distributions
  SlicerIO
  )

# Add the testing directories
IF(NAMICSandBox_BUILD_TESTING)
   SUBDIRS(Testing)
ENDIF(NAMICSandBox_BUILD_TESTING)

# Add the examples directories
OPTION(NAMICSandBox_BUILD_EXAMPLES "Examples of the project" ON)
IF(NAMICSandBox_BUILD_EXAMPLES)
    SUBDIRS(Examples)
ENDIF(NAMICSandBox_BUILD_EXAMPLES)

#-----------------------------------------------------------------------------
# Configure the default NAMICSandBox_DATA_ROOT for the location of NAMICSandBox Data.
#FIND_PATH(NAMICSandBox_DATA_ROOT NAMICSandBoxData.readme ${NAMICSandBox_SOURCE_DIR}/Testing/Data $ENV{NAMICSandBox_DATA_ROOT})
#MARK_AS_ADVANCED(NAMICSandBox_DATA_ROOT)

INCLUDE_DIRECTORIES(
${NAMICSandBox_SOURCE_DIR}/Source
)

ENDIF(NOT NAMICSandBox_SETUP_DASHBOARD)
