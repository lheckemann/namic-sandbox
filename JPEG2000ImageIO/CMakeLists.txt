CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT(JPEG2000ImageIO)

#
# Make possible to find FindOpenJPEG.cmake
#
SET(JPEG2000ImageIO_CMAKE_DIR "${JPEG2000ImageIO_SOURCE_DIR}/CMake" CACHE INTERNAL "")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${JPEG2000ImageIO_CMAKE_DIR}")


FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

FIND_PACKAGE(OpenJPEG REQUIRED)

INCLUDE_DIRECTORIES(${OPENJPEG_INCLUDE_DIR})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE INTERNAL "Where to put the executables") 
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE INTERNAL "Where to put the libraries") 

INCLUDE(${PROJECT_SOURCE_DIR}/Utilities/KWStyle/KWStyle.cmake)

INCLUDE(CTest)


SET(CPACK_PACKAGE_NAME "JPEG2000ImageIO")
SET(CPACK_PACKAGE_VENDOR "Insight Software Consortium")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
INCLUDE(CPack)


INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/Source
  ${PROJECT_BINARY_DIR}/Source
  )

OPTION(BUILD_REPORTS "Generate articles for the Insight Journal" OFF)

# Disable MSVC 8 warnings
IF(WIN32)
  OPTION(DISABLE_MSVC8_DEPRECATED_WARNINGS
         "Disable Visual Studio 8 deprecated warnings" ON)
  MARK_AS_ADVANCED(FORCE DISABLE_MSVC8_DEPRECATED_WARNINGS)  
  IF(DISABLE_MSVC8_DEPRECATED_WARNINGS)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
  ENDIF(DISABLE_MSVC8_DEPRECATED_WARNINGS)
ENDIF(WIN32)


#-----------------------------------------------------------------------------
# Setup file for setting custom ctest vars
CONFIGURE_FILE(
  ${PROJECT_SOURCE_DIR}/CTestCustom.cmake.in
  ${PROJECT_BINARY_DIR}/CTestCustom.cmake
  @ONLY
  )


#
#  Path to the conformace JPEG2000 images that you can download from
#  http://www.crc.ricoh.com/~gormish/jpeg2000conformance/
#
#  According to instructions from
#  http://www.jpeg.org/jpeg2000/testlinks.html
#
#  If set, the images in this directory will be used for enabling
#  Acceptance tests. (testing level above unit-testing).
#
FIND_PATH(JPEG2000_DATA_ROOT COPYRIGHT)

#
#  Datasets from:
#  https://www.sdms.afrl.af.mil/datasets/clif2007/
#
FIND_PATH(CLIF2007_DATA_ROOT mosaic_im100010_05.jp2)


ADD_SUBDIRECTORY( Source )
ADD_SUBDIRECTORY( Testing )
ADD_SUBDIRECTORY( Examples )

IF( BUILD_REPORTS )
  ADD_SUBDIRECTORY( Documents )
ENDIF( BUILD_REPORTS )
