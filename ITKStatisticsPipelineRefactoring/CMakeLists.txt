PROJECT(StatisticsRefactoring)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

# The "BEFORE" modified ensures that our Source directory will 
# be visited before the ITK Code/Numerics/Statistics directory.
INCLUDE_DIRECTORIES( BEFORE
  ${StatisticsRefactoring_SOURCE_DIR}
  ${StatisticsRefactoring_SOURCE_DIR}/Source
  ${StatisticsRefactoring_SOURCE_DIR}/Testing
  )

CONFIGURE_FILE(
  DartConfiguration.tcl.in
  ${PROJECT_BINARY_DIR}/CTestConfiguration.ini )

ENABLE_TESTING()

INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)

SUBDIRS(
  Source
  Testing
  Examples
  )

OPTION(KWSTYLE_USE_VIM_FORMAT "Set KWStyle to generate errors with a VIM-compatible format." OFF)
OPTION(KWSTYLE_USE_MSVC_FORMAT "Set KWStyle to generate errors with a VisualStudio-compatible format." OFF)

FIND_PROGRAM(KWSTYLE_EXECUTABLE
NAMES KWStyle 
PATHS
/usr/local/bin
)

CONFIGURE_FILE(
  Utilities/KWStyle/ITKFiles.txt.in
  ${PROJECT_BINARY_DIR}/Utilities/KWStyle/ITKFiles.txt)

CONFIGURE_FILE(
  Utilities/KWStyle/ITK.kws.xml.in
  ${PROJECT_BINARY_DIR}/Utilities/KWStyle/ITK.kws.xml)

SET(KWSTYLE_ARGUMENTS
  -xml ${PROJECT_BINARY_DIR}/Utilities/KWStyle/ITK.kws.xml -v -D ${PROJECT_BINARY_DIR}/Utilities/KWStyle/ITKFiles.txt  
  )

IF(KWSTYLE_USE_VIM_FORMAT)
  SET(KWSTYLE_ARGUMENTS -vim ${KWSTYLE_ARGUMENTS})
ENDIF(KWSTYLE_USE_VIM_FORMAT)

IF(KWSTYLE_USE_MSVC_FORMAT)
  SET(KWSTYLE_ARGUMENTS -msvc ${KWSTYLE_ARGUMENTS})
ENDIF(KWSTYLE_USE_MSVC_FORMAT)

ADD_CUSTOM_COMMAND(
  OUTPUT ${StatisticsRefactoring_BINARY_DIR}/KWStyleReport.txt
  COMMAND ${KWSTYLE_EXECUTABLE}
  ARGS    ${KWSTYLE_ARGUMENTS}
  COMMENT "Coding Style Checker"
  )
ADD_CUSTOM_TARGET(StyleCheck DEPENDS ${StatisticsRefactoring_BINARY_DIR}/KWStyleReport.txt)
ADD_TEST(KWStyleTest ${KWSTYLE_EXECUTABLE} ${KWSTYLE_ARGUMENTS})

