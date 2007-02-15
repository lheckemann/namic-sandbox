PROJECT(StochasticTractography)

SET(CMAKE_VERBOSE_MAKEFILE ON)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR,"ITK not found.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

#ADD_EXECUTABLE(StochasticTractographyFilterTest StochasticTractographyFilterTest.cxx)
#TARGET_LINK_LIBRARIES(StochasticTractographyFilterTest ITKIO ITKCommon)

#ADD_EXECUTABLE(NhdrReader NhdrReader.cxx)
#TARGET_LINK_LIBRARIES(NhdrReader ITKIO ITKCommon)

#ADD_EXECUTABLE(TensorModelFilterTest TensorModelFilterTest.cxx)
#TARGET_LINK_LIBRARIES(TensorModelFilterTest ITKIO ITKCommon)

#INCLUDE(/home/tringo/Slicer3/Libs/GenerateCLP/UseGenerateCLP.cmake)

SET(StochasticTractographyFilter_SOURCE StochasticTractographyFilter.cxx)
#GENERATECLP(StochasticTractographyFilter_SOURCE StochasticTractographyFilter.xml)

ADD_EXECUTABLE(StochasticTractographyFilter ${StochasticTractographyFilter_SOURCE})
TARGET_LINK_LIBRARIES(StochasticTractographyFilter ITKIO ITKBasicFilters ITKCommon)
