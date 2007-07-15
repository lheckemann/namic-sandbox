PROJECT(StochasticTractography)

SET(CMAKE_VERBOSE_MAKEFILE ON)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR,"ITK not found.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

#INCLUDE(/projects/schiz/software/Slicer3/Libs/GenerateCLP/UseGenerateCLP.cmake)
INCLUDE(/home/tringo/Projects/Slicer3/Libs/GenerateCLP/UseGenerateCLP.cmake)
#INCLUDE(/afs/csail.mit.edu/u/t/tringo/subversion/Slicer3/Libs/GenerateCLP/UseGenerateCLP.cmake)
SET(StochasticTractographyFilter_SOURCE StochasticTractographyFilter.cxx)
GENERATECLP(StochasticTractographyFilter_SOURCE StochasticTractographyFilter.xml)

ADD_EXECUTABLE(StochasticTractographyFilter ${StochasticTractographyFilter_SOURCE})
TARGET_LINK_LIBRARIES(StochasticTractographyFilter ITKIO ITKBasicFilters ITKCommon)
