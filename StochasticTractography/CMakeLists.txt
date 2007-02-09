PROJECT(StochasticTractography)

SET(CMAKE_VERBOSE_MAKEFILE ON)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR,"ITK not found.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

#ADD_EXECUTABLE(ProbabilisticTractographyFilterTest ProbabilisticTractographyFilterTest.cxx)
#TARGET_LINK_LIBRARIES(ProbabilisticTractographyFilterTest ITKIO ITKCommon)

#ADD_EXECUTABLE(NhdrReader NhdrReader.cxx)
#TARGET_LINK_LIBRARIES(NhdrReader ITKIO ITKCommon)

#ADD_EXECUTABLE(TensorModelFilterTest TensorModelFilterTest.cxx)
#TARGET_LINK_LIBRARIES(TensorModelFilterTest ITKIO ITKCommon)

#INCLUDE(/home/tringo/Slicer3/Libs/GenerateCLP/UseGenerateCLP.cmake)

SET(ProbabilisticTractographyFilter_SOURCE ProbabilisticTractographyFilter.cxx)
#GENERATECLP(ProbabilisticTractographyFilter_SOURCE ProbabilisticTractographyFilter.xml)

ADD_EXECUTABLE(ProbabilisticTractographyFilter ${ProbabilisticTractographyFilter_SOURCE})
TARGET_LINK_LIBRARIES(ProbabilisticTractographyFilter ITKIO ITKBasicFilters ITKCommon)
