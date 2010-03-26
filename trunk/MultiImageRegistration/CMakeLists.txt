CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(MultiImageRegistration)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)
SET(ITK_VERSION ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR})
#ITK>=3.16
IF("${ITK_VERSION}" LESS 3.16)
MESSAGE(SEND_ERROR
          "This version of ITK is too old to build MultiImageRegistration.  "
          "Please upgrade to ITK 3.16 or greater.")
ELSE("${ITK_VERSION}" LESS 3.16)
  SET(EXECUTABLE_OUTPUT_PATH ${MultiImageRegistration_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
  SET(LIBRARY_OUTPUT_PATH ${MultiImageRegistration_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")

  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)


  IF( BUILD_AGAINST_SLICER3 )
    find_package(Slicer3 REQUIRED)
    include(${Slicer3_USE_FILE})

    slicer3_set_default_install_prefix_for_external_projects()
  ENDIF( BUILD_AGAINST_SLICER3 )
  IF( NOT BUILD_AGAINST_SLICER3 )
    ##  In many cases sub-projects depending on SlicerExectuion Model
    ##  that can be built stand alone are combined in larger packages
    ##  This logic will include SlicerExectionModel only if it
    ##  has not already been built.
     ADD_SUBDIRECTORY(SlicerExecutionModel)
  ENDIF( NOT BUILD_AGAINST_SLICER3 )
    FIND_PACKAGE(GenerateCLP NO_MODULE REQUIRED)
    IF(GenerateCLP_DIR)
      INCLUDE(${GenerateCLP_USE_FILE})
    ELSE(GenerateCLP_DIR)
      MESSAGE(FATAL_ERROR "Can't build without GenerateCLP. Please set GenerateCLP_DIR")
    ENDIF(GenerateCLP_DIR)
  INCLUDE_DIRECTORIES(LargestForegroundFilledMaskImageFilter)
  SUBDIRS( Source )
  SUBDIRS( Testing )

ENDIF("${ITK_VERSION}" LESS 3.16)

