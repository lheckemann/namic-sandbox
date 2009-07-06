# This project is designed to be built outside the Insight source tree.
PROJECT(AlternativeMemoryModels)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

INCLUDE_DIRECTORIES(
  BEFORE
  ${AlternativeMemoryModels_SOURCE_DIR}/Source
  ${AlternativeMemoryModels_SOURCE_DIR}/Testing
  )

ADD_SUBDIRECTORY(Source)
OPTION(BUILD_TESTING "Build the Testing directory." ON)
IF(BUILD_TESTING)
  ENABLE_TESTING()
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)
