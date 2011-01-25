PROJECT (febio)

# There is nothing to build, just copy a precompiled release.
# The febio solver is open source and can quite simply built using CMake.
# However, to build a release that runs efficiently the Pardiso solver
# has to be obtained and build, too. So, it is easier to just download the
# release from http://mrl.sci.utah.edu/software/febio

IF(WIN32)
  FILE(COPY ${febio_SOURCE_DIR}/Release-Win32/FEBio
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release/
    FILE_PERMISSIONS WORLD_EXECUTE WORLD_READ
     GROUP_EXECUTE GROUP_READ OWNER_EXECUTE OWNER_READ)
  FILE(COPY ${febio_SOURCE_DIR}/Release-Win32/febio.xml
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release/
    FILE_PERMISSIONS WORLD_READ GROUP_READ OWNER_READ)
ENDIF(WIN32)

IF(UNIX)
  FILE(COPY ${febio_SOURCE_DIR}/Release-Linux32/FEBio
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/
    FILE_PERMISSIONS WORLD_EXECUTE WORLD_READ
     GROUP_EXECUTE GROUP_READ OWNER_EXECUTE OWNER_READ)
  FILE(COPY ${febio_SOURCE_DIR}/Release-Linux32/febio.xml
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/
    FILE_PERMISSIONS WORLD_READ GROUP_READ OWNER_READ)
ENDIF(UNIX)

IF(APPLE)
  MESSAGE(WARNING "FEBio is not available for your platform. Download it from http://mrl.sci.utah.edu/software/febio and copy the executable and configuration file to ${EXECUTABLE_OUTPUT_PATH}/Release.")
ENDIF(APPLE)
