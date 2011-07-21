project(BRAINSCommonLib2DProject)
set(LOCAL_PROJECT_NAME BRAINSCommonLib2D)
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

enable_testing()
include(Dart)
include(CPack)
#include(${BRAINS_CMAKE_HELPER_DIR}/CMakeBuildMacros.cmake)

include(GenerateBRAINSCommonLib2DConfig.cmake)
if(NOT SETIFEMPTY)
macro(SETIFEMPTY)
  set(KEY ${ARGV0})
  set(VALUE ${ARGV1})
  if(NOT ${KEY})
    set(${ARGV})
  endif(NOT ${KEY})
endmacro(SETIFEMPTY KEY VALUE)
endif(NOT SETIFEMPTY)

###
SETIFEMPTY(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
SETIFEMPTY(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
SETIFEMPTY(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
SETIFEMPTY(CMAKE_BUNDLE_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/bin)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

###
#CHECKIFSLICER3BUILD()  ## Call the convenience macro

if(NOT ITK_FOUND)
    find_package(ITK REQUIRED)
    include(${ITK_USE_FILE})
endif(NOT ITK_FOUND)


#-----------------------------------------------------------------------------
# Output directories.
#

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/BRAINSCommonLib2D.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/BRAINSCommonLib2D.h
  )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

set(BRAINSCommonLib2D_SRCS GenericTransformImage2D.cxx BRAINSFit2DHelper.cxx Slicer3LandmarkIO.cxx)

## ITKv4 requires that ITKReview is explicitly linked against
if(ITK_VERSION_MAJOR EQUAL 4 )
  set(ITKREVIEWLIB ITKReview)
endif(ITK_VERSION_MAJOR EQUAL 4 )


add_library(BRAINSCommonLib2D ${BRAINSCommonLib2D_SRCS})
target_link_libraries(BRAINSCommonLib2D ITKAlgorithms ITKIO ${ITKREVIEWLIB})


if (Slicer_SOURCE_DIR)

  # install each target in the production area (where it would appear in an
  # installation) and install each target in the developer area (for running
  # from a build)
  set(TARGETS
    BRAINSCommonLib2D
    )
  slicer3_install_plugins(${TARGETS})

else (Slicer_SOURCE_DIR)

  INSTALL(TARGETS BRAINSCommonLib2D
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib/BRAINSCommonLib2D
      ARCHIVE DESTINATION lib/BRAINSCommonLib2D
      )

  IF(NOT BRAINSCommonLib2DProject_INSTALL_NO_DEVELOPMENT)
    FILE(GLOB __files1 "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    FILE(GLOB __files2 "${CMAKE_CURRENT_SOURCE_DIR}/*.txx")
    INSTALL(FILES ${__files1} ${__files2} ${CMAKE_CURRENT_BINARY_DIR}/BRAINSCommonLib2D.h
      DESTINATION ${CMAKE_INSTALL_PREFIX}/include/BRAINSCommonLib2D
      COMPONENT Development)
  ENDIF(NOT BRAINSCommonLib2DProject_INSTALL_NO_DEVELOPMENT)

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BRAINSCommonLib2DInstallConfig.cmake.in
                 ${CMAKE_CURRENT_BINARY_DIR}/BRAINSCommonLib2DInstallConfig.cmake
                 @ONLY)

  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/BRAINSCommonLib2DInstallConfig.cmake
          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/BRAINSCommonLib2D
          COMPONENT Development
          RENAME BRAINSCommonLib2DConfig.cmake)

   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/UseBRAINSCommonLib2D.cmake.in
     ${CMAKE_CURRENT_BINARY_DIR}/UseBRAINSCommonLib2D.cmake @ONLY)

   # INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/UseBRAINSCommonLib2D.cmake.in
   #         DESTINATION lib/BRAINSCommonLib2D
   #         COMPONENT Development
   #         RENAME UseBRAINSCommonLib2D.cmake)
   INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/UseBRAINSCommonLib2D.cmake
     DESTINATION lib/BRAINSCommonLib2D
     COMPONENT Development)

endif (Slicer_SOURCE_DIR)

##HACK NEED BETTER TESTS add_directory( TestLargestForegroundFilledMaskImageFilter )
##HACK NEED BETTER TESTS add_directory( Test_FindCenterOfBrainFilter )
#if(BUILD_TESTING)
#  add_subdirectory(TestSuite)
#endif(BUILD_TESTING)

