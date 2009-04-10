INCLUDE_DIRECTORIES( BEFORE
  ${StatisticsRefactoring_SOURCE_DIR}/SourceBC
  )

SET(ITK_DATA_ROOT ${StatisticsRefactoring_SOURCE_DIR}/Testing/Data)
SET(BASELINE ${StatisticsRefactoring_SOURCE_DIR}/Testing/Data/Baseline)
SET(TEMP ${StatisticsRefactoring_BINARY_DIR}/Testing/Temporary)

SET(STATISTICS_TESTS ${EXECUTABLE_OUTPUT_PATH}/itkStatisticsBCTests)

ADD_TEST(itkMeanCalculatorBCTest ${STATISTICS_TESTS} itkMeanCalculatorTest)
ADD_TEST(itkScalarImageTextureCalculatorBCTest ${STATISTICS_TESTS} itkScalarImageTextureCalculatorTest)
ADD_TEST(itkCovarianceCalculatorBCTest ${STATISTICS_TESTS} itkCovarianceCalculatorTest)
ADD_TEST(itkWeightedCovarianceCalculatorBCTest ${STATISTICS_TESTS} itkWeightedCovarianceCalculatorTest)
ADD_TEST(itkWeightedMeanCalculatorBCTest ${STATISTICS_TESTS} itkWeightedMeanCalculatorTest)
ADD_TEST(itkDenseFrequencyContainerBCTest ${STATISTICS_TESTS} itkDenseFrequencyContainerTest)

SET(Statistics_SRCS
  itkMeanCalculatorTest.cxx
  itkScalarImageTextureCalculatorTest.cxx
  itkCovarianceCalculatorTest.cxx
  itkWeightedCovarianceCalculatorTest.cxx
  itkWeightedMeanCalculatorTest.cxx
  itkKdTreeGeneratorTest.cxx
  itkDenseFrequencyContainerTest
)

ADD_EXECUTABLE(itkStatisticsBCTests itkStatisticsBCTests.cxx ${Statistics_SRCS})
TARGET_LINK_LIBRARIES(itkStatisticsBCTests ITKIO ITKCommon ITKNumerics ITKStatisticsNew)


