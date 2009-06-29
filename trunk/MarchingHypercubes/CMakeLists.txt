CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT(MarchingHypercubes)

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE INTERNAL "Where to put the executables") 
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE INTERNAL "Where to put the libraries") 

INCLUDE(${PROJECT_SOURCE_DIR}/Utilities/KWStyle/KWStyle.cmake)

ENABLE_TESTING()
INCLUDE(CTest)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/Source
  ${PROJECT_SOURCE_DIR}/../QuadEdgeMeshSphericalDiffeomorphicDemonsRegistration/Source
  )

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
# Configure the default ITK_BRAINWEB_DATA_ROOT for the location of ITK Data.
# When this data is available, additional 3D tests are enabled.
FIND_PATH(ITK_BRAINWEB_DATA_ROOT brainweb165a10f17.mha 
  DOC "Directory with data taken from http://public.kitware.com/pub/itk/Data/BrainWeb/")
MARK_AS_ADVANCED(ITK_BRAINWEB_DATA_ROOT)


ADD_SUBDIRECTORY( Source )
ADD_SUBDIRECTORY( Testing )
ADD_SUBDIRECTORY( Examples )
ADD_SUBDIRECTORY( Documents )
