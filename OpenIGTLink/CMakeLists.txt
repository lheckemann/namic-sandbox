PROJECT( OpenIGTLink )

cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
     cmake_policy(SET CMP0003 NEW)
     endif(COMMAND cmake_policy)

ENABLE_TESTING()
INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)


#-----------------------------------------------------------------------------
# Include directories from the build tree.
SET(OpenIGTLink_INCLUDE_DIRS_BUILD_TREE ${OpenIGTLink_BINARY_DIR})

# These directories are always needed.
SET(OpenIGTLink_INCLUDE_DIRS_BUILD_TREE ${OpenIGTLink_INCLUDE_DIRS_BUILD_TREE}
  ${OpenIGTLink_SOURCE_DIR}/Source
  ${OpenIGTLink_SOURCE_DIR}/Source/igtlutil
)


#-----------------------------------------------------------------------------
#  There should be only one INCLUDE_DIRECTORIES command 
#  in the entire tree in order to enforce a particular
#  order for the search.
#
INCLUDE_DIRECTORIES(
  ${OpenIGTLink_INCLUDE_DIRS_BUILD_TREE}
  )

SET(OpenIGTLink_LIBRARY_PATH  ${OpenIGTLink_LIBRARY_PATH}
  ${OpenIGTLink_BINARY_DIR}/Source
  ${OpenIGTLink_BINARY_DIR}/Source/igtlutil
  )


#-----------------------------------------------------------------------------
# Help other projects use OpenIGTLink.

# Copy the UseITK.cmake file to the binary tree for backward compatability.
CONFIGURE_FILE(${OpenIGTLink_SOURCE_DIR}/UseOpenIGTLink.cmake.in
${OpenIGTLink_BINARY_DIR}/UseOpenIGTLink.cmake COPYONLY IMMEDIATE)

# Create the OpenIGTLinkConfig.cmake file containing the OpenIGTLink configuration.
INCLUDE (${OpenIGTLink_SOURCE_DIR}/GenerateOpenIGTLinkConfig.cmake)

# Save the compiler settings so another project can import them.
INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
CMAKE_EXPORT_BUILD_SETTINGS(${OpenIGTLink_BINARY_DIR}/OpenIGTLinkBuildSettings.cmake)

# Save library dependencies.
EXPORT_LIBRARY_DEPENDENCIES(${OpenIGTLink_BINARY_DIR}/OpenIGTLinkLibraryDepends.cmake)

#IF(NOT OpenIGTLink_INSTALL_NO_DEVELOPMENT)
#  INSTALL(FILES
#    ${OpenIGTLink_BINARY_DIR}/OpenIGTLinkBuildSettings.cmake
#    ${OpenIGTLink_BINARY_DIR}/OpenIGTLinkLibraryDepends.cmake
#    ${OpenIGTLink_BINARY_DIR}/UseOpenIGTLink.cmake
#    DESTINATION ${OpenIGTLink_INSTALL_PACKAGE_DIR_CM24}
#    COMPONENT Development
#    )
#ENDIF(NOT OpenIGTLink_INSTALL_NO_DEVELOPMENT)


#-----------------------------------------------------------------------------
# Configure Subdirectories

ADD_SUBDIRECTORY( Source    )
ADD_SUBDIRECTORY( Testing   )
ADD_SUBDIRECTORY( Tools     )
#ADD_SUBDIRECTORY( Documents )
ADD_SUBDIRECTORY( Examples )



