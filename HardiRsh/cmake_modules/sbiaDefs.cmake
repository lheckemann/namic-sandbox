SET( CONTACT_INFO "sbia-software@uphs.upenn.edu" )

ADD_DEFINITIONS(-DCONTACT_INFO="${CONTACT_INFO}")

SET (CMAKE_EXE_LINKER_FLAGS -lm ${CMAKE_EXE_LINKER_FLAGS})

#make pedantic warnings into Errors
#ADD_DEFINITIONS( -pedantic-errors)

# setup output directories
IF(NOT DEFINED RUNTIME_OUTPUT_DIRECTORY )
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
ENDIF(NOT DEFINED RUNTIME_OUTPUT_DIRECTORY )
MARK_AS_ADVANCED (
    RUNTIME_OUTPUT_DIRECTORY
    LIBRARY_OUTPUT_DIRECTORY
    EXECUTABLE_OUTPUT_PATH
    LIBRARY_OUTPUT_PATH
    )

#### STUFF FOR SVN SUPPORT
IF(NOT DEFINED USE_SVN )
  SET (USE_SVN ON CACHE BOOL "Gather info from svn")
ENDIF(NOT DEFINED USE_SVN )

IF (USE_SVN)
  #get svninfo
  FIND_PACKAGE(Subversion)
    IF(Subversion_FOUND)
      Subversion_WC_INFO(${PROJECT_SOURCE_DIR} Project)
      SET(SVN_REV ${Project_WC_REVISION})
      ADD_DEFINITIONS(-DSVN_REV="${SVN_REV}")
    ENDIF(Subversion_FOUND)
ENDIF (USE_SVN)


