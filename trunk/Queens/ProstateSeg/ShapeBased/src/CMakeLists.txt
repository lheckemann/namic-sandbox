PROJECT(ProstateSegSB)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
  CMAKE_POLICY(SET CMP0012 NEW)
ENDIF(COMMAND CMAKE_POLICY)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

OPTION(BUILD_SLICER_MODULE "Build Slicer CLI module." OFF)

################################
# include ITK, to avoid setting it everytime, do:
# set the environment variable ITK_DIR to the dir containing ITKConfig.cmake in .bashrc
FIND_PACKAGE (ITK)
IF (ITK_FOUND)
   INCLUDE(${USE_ITK_FILE})
ENDIF(ITK_FOUND)

################################
# include boost
FIND_PATH(BOOST_PATH boost/shared_ptr.hpp
  shared/boost_1_40_0
  )
IF(NOT BOOST_PATH)
   MESSAGE(FATAL_ERROR "Please enter the path of boost")
ENDIF(NOT BOOST_PATH)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/shared)
INCLUDE_DIRECTORIES(${BOOST_PATH})

ADD_SUBDIRECTORY(AlignTrainingShapes)
ADD_SUBDIRECTORY(ConvertImageToLevelSet)
ADD_SUBDIRECTORY(LearnShapeModel)
ADD_SUBDIRECTORY(SegmentImage)
