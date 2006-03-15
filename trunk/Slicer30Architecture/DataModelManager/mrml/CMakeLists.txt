PROJECT(MRML)

#-----------------------------------------------------------------------------
SET(MRML_VERSION_MAJOR 0)
SET(MRML_VERSION_MINOR 0)
SET(MRML_VERSION_PATCH 1)
SET(MRML_VERSION
    "${MRML_VERSION_MAJOR}.${MRML_VERSION_MINOR}.${MRML_VERSION_PATCH}")

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${USE_VTK_FILE})

INCLUDE_DIRECTORIES(
  ${MRML_BINARY_DIR}
  )

#-----------------------------------------------------------------------------
# ITK build configuration options.
OPTION(BUILD_SHARED_LIBS "Build MRML with shared libraries." OFF)
SET(MRML_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

SET(MRML_SRCS
  mrmlCommand.cxx
  mrmlModel.cxx
  mrmlEventObject.cxx
  mrmlFastMutexLock.cxx
  mrmlIndent.cxx
  mrmlLightObject.cxx
  mrmlObject.cxx
  mrmlObjectFactoryBase.cxx
  mrmlSimpleFastMutexLock.cxx
  mrmlTimeStamp.cxx
  #DynamicLoader.cxx
  #MRMLModelNode.cxx
  mrmlNode.cxx
  mrmlModelNode.cxx
  mrmlModelStorageNode.cxx
  mrmlVolumeNode.cxx
  mrmlStorageNode.cxx
  mrmlScene.cxx
  mrmlParser.cxx
  mrmlVolumeArchetypeStorageNode.cxx
  #mrmlDataObject.cxx
  #mrmlTree.cxx
  )

INCLUDE (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)

CONFIGURE_FILE(${MRML_SOURCE_DIR}/mrmlConfigure.h.in
               ${MRML_BINARY_DIR}/mrmlConfigure.h)

#-----------------------------------------------------------------------------
# Create the kwsys library for MRML
SET(KWSYS_NAMESPACE mrmlsys)
SET(KWSYS_USE_SystemTools 1)
SET(KWSYS_USE_RegularExpression 1)
SET(KWSYS_USE_Directory 1)
SET(KWSYS_USE_DynamicLoader 1)
SET(KWSYS_USE_Base64 1)
SET(KWSYS_USE_FundamentalType 1)
SET(KWSYS_USE_CommandLineArguments 1)
SET(KWSYS_HEADER_ROOT ${MRML_BINARY_DIR})
#SET(KWSYS_LIBRARY_INSTALL_DIR /lib/InsightToolkit)
#SET(KWSYS_HEADER_INSTALL_DIR /include/InsightToolkit/Utilities)
SUBDIRS(kwsys)

ADD_LIBRARY(mrml ${MRML_SRCS})
TARGET_LINK_LIBRARIES(mrml mrmlsys)
TARGET_LINK_LIBRARIES(mrml vtkIO)
IF(UNIX)
  TARGET_LINK_LIBRARIES(mrml dl)
ENDIF(UNIX)

#-----------------------------------------------------------------------------
# Add the testing directories
OPTION(BUILD_TESTING "Build testing." ON)

IF(BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
  MARK_AS_ADVANCED(DART_ROOT)
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)

#SUBDIRS(Examples)
SUBDIRS(vtkmrml)
