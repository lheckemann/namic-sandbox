PROJECT( OpenIGTLink )

cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
     cmake_policy(SET CMP0003 NEW)
     endif(COMMAND cmake_policy)

ENABLE_TESTING()
INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)

#-----------------------------------------------------------------------------
# Output directories.

IF(NOT LIBRARY_OUTPUT_PATH)
  SET (LIBRARY_OUTPUT_PATH ${OpenIGTLink_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
ENDIF(NOT LIBRARY_OUTPUT_PATH)
IF(NOT EXECUTABLE_OUTPUT_PATH)
  SET (EXECUTABLE_OUTPUT_PATH ${OpenIGTLink_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
ENDIF(NOT EXECUTABLE_OUTPUT_PATH)
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

SET(OpenIGTLink_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
SET(OpenIGTLink_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")


#-----------------------------------------------------------------------------
# Installation vars.
# OpenIGTLink_INSTALL_BIN_DIR          - binary dir (executables)
# OpenIGTLink_INSTALL_LIB_DIR          - library dir (libs)
# OpenIGTLink_INSTALL_DATA_DIR         - share dir (say, examples, data, etc)
# OpenIGTLink_INSTALL_INCLUDE_DIR      - include dir (headers)
# OpenIGTLink_INSTALL_PACKAGE_DIR      - package/export configuration files
# OpenIGTLink_INSTALL_NO_DEVELOPMENT   - do not install development files
# OpenIGTLink_INSTALL_NO_RUNTIME       - do not install runtime files
# OpenIGTLink_INSTALL_NO_DOCUMENTATION - do not install documentation files

IF(NOT OpenIGTLink_INSTALL_BIN_DIR)
  SET(OpenIGTLink_INSTALL_BIN_DIR "bin")
ENDIF(NOT OpenIGTLink_INSTALL_BIN_DIR)

IF(NOT OpenIGTLink_INSTALL_LIB_DIR)
  SET(OpenIGTLink_INSTALL_LIB_DIR "lib")
ENDIF(NOT OpenIGTLink_INSTALL_LIB_DIR)

IF(NOT OpenIGTLink_INSTALL_DATA_DIR)
  SET(OpenIGTLink_INSTALL_DATA_DIR "share/igtl")
ENDIF(NOT OpenIGTLink_INSTALL_DATA_DIR)

IF(NOT OpenIGTLink_INSTALL_INCLUDE_DIR)
  SET(OpenIGTLink_INSTALL_INCLUDE_DIR "include")
ENDIF(NOT OpenIGTLink_INSTALL_INCLUDE_DIR)

IF(NOT OpenIGTLink_INSTALL_PACKAGE_DIR)
  SET(OpenIGTLink_INSTALL_PACKAGE_DIR ${OpenIGTLink_INSTALL_LIB_DIR}
    CACHE INTERNAL "")
ENDIF(NOT OpenIGTLink_INSTALL_PACKAGE_DIR)

IF(NOT OpenIGTLink_INSTALL_NO_DOCUMENTATION)
  SET(OpenIGTLink_INSTALL_NO_DOCUMENTATION 0)
ENDIF(NOT OpenIGTLink_INSTALL_NO_DOCUMENTATION)

SET(OpenIGTLink_DIR "${CMAKE_BINARY_DIR}" CACHE INTERNAL "OpenIGTLink dir to be used by subprojects")

#-----------------------------------------------------------------------------
# Include directories

SET(OpenIGTLink_INCLUDE_DIRS_BUILD_TREE ${OpenIGTLink_INCLUDE_DIRS_BUILD_TREE}
  ${OpenIGTLink_BINARY_DIR}
  ${OpenIGTLink_SOURCE_DIR}/Source
  ${OpenIGTLink_SOURCE_DIR}/Source/igtlutil
)

INCLUDE_DIRECTORIES(
  ${OpenIGTLink_INCLUDE_DIRS_BUILD_TREE}
  )

#SET(OpenIGTLink_LIBRARY_PATH  ${OpenIGTLink_LIBRARY_PATH}
#  ${OpenIGTLink_BINARY_DIR}/Source
#  ${OpenIGTLink_BINARY_DIR}/Source/igtlutil
#  )

#-----------------------------------------------------------------------------
# Generate Configuration Header files.

include (${OpenIGTLink_SOURCE_DIR}/OpenIGTLinkConfigPlatform.cmake)

CONFIGURE_FILE(${OpenIGTLink_SOURCE_DIR}/igtlConfigure.h.in 
  ${OpenIGTLink_BINARY_DIR}/igtlConfigure.h)

CONFIGURE_FILE(${OpenIGTLink_SOURCE_DIR}/igtl_typeconfig.h.in 
  ${OpenIGTLink_BINARY_DIR}/igtl_typeconfig.h)

#-----------------------------------------------------------------------------
# Help other projects use OpenIGTLink.

# Copy the UseOpenIGTLink.cmake file to the binary tree for backward compatability.
CONFIGURE_FILE(${OpenIGTLink_SOURCE_DIR}/UseOpenIGTLink.cmake.in
${OpenIGTLink_BINARY_DIR}/UseOpenIGTLink.cmake COPYONLY IMMEDIATE)

# Create the OpenIGTLinkConfig.cmake file containing the OpenIGTLink configuration.
INCLUDE (${OpenIGTLink_SOURCE_DIR}/GenerateOpenIGTLinkConfig.cmake)

# Save the compiler settings so another project can import them.
INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
CMAKE_EXPORT_BUILD_SETTINGS(${OpenIGTLink_BINARY_DIR}/OpenIGTLinkBuildSettings.cmake)

# Save library dependencies.
#EXPORT_LIBRARY_DEPENDENCIES(${OpenIGTLink_BINARY_DIR}/OpenIGTLinkLibraryDepends.cmake)

INSTALL(FILES
  ${OpenIGTLink_BINARY_DIR}/OpenIGTLinkBuildSettings.cmake
#  ${OpenIGTLink_BINARY_DIR}/OpenIGTLinkLibraryDepends.cmake
  ${OpenIGTLink_BINARY_DIR}/UseOpenIGTLink.cmake
  DESTINATION ${OpenIGTLink_INSTALL_PACKAGE_DIR}
  COMPONENT Development
  )

INSTALL(FILES
  ${OpenIGTLink_BINARY_DIR}/igtlConfigure.h
  DESTINATION ${OpenIGTLink_INSTALL_INCLUDE_DIR}
  COMPONENT Development
  )

INSTALL(FILES
  ${OpenIGTLink_BINARY_DIR}/igtl_typeconfig.h
  DESTINATION ${OpenIGTLink_INSTALL_INCLUDE_DIR}/igtlutil
  COMPONENT Development
  )

#-----------------------------------------------------------------------------
# Configure Subdirectories

ADD_SUBDIRECTORY( Source    )
ADD_SUBDIRECTORY( Testing   )
ADD_SUBDIRECTORY( Tools     )
#ADD_SUBDIRECTORY( Documents )
ADD_SUBDIRECTORY( Examples )



