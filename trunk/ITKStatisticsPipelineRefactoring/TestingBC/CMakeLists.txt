INCLUDE_REGULAR_EXPRESSION("^.*$")

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
ADD_TEST(itkKdTreeBasedKmeansEstimatorBCTest ${STATISTICS_TESTS} 
         itkKdTreeBasedKmeansEstimatorTest 
         ${ITK_DATA_ROOT}/Input/Statistics/TwoDimensionTwoGaussian.dat 
         1 28.54746 0.07)
ADD_TEST(itkVariableDimensionHistogramBCTest ${STATISTICS_TESTS} itkVariableDimensionHistogramTest)
ADD_TEST(itkListSampleToHistogramFilterBCTest ${STATISTICS_TESTS} itkListSampleToHistogramFilterTest)

ADD_TEST(itkHistogramBCTest ${STATISTICS_TESTS} itkHistogramTest)

ADD_TEST(itkChiSquareDistributionBCTest ${STATISTICS_TESTS}
  itkChiSquareDistributionTest
  )
ADD_TEST(itkGaussianDistributionBCTest ${STATISTICS_TESTS}
  itkGaussianDistributionTest
  )
ADD_TEST(itkTDistributionBCTest ${STATISTICS_TESTS} itkTDistributionTest)

ADD_TEST(itkGaussianDensityFunctionBCTest ${STATISTICS_TESTS} itkGaussianDensityFunctionTest)

SET(Statistics_SRCS
  itkChiSquareDistributionTest.cxx
  itkCovarianceCalculatorTest.cxx
  itkDenseFrequencyContainerTest.cxx
  itkGaussianDistributionTest.cxx
  itkGaussianDensityFunctionTest.cxx
  itkHistogramTest.cxx
  itkKdTreeBasedKmeansEstimatorTest.cxx
  itkKdTreeGeneratorTest.cxx
  itkListSampleToHistogramFilterTest.cxx
  itkMeanCalculatorTest.cxx
  itkScalarImageTextureCalculatorTest.cxx
  itkTDistributionTest.cxx
  itkVariableDimensionHistogramTest.cxx
  itkWeightedCovarianceCalculatorTest.cxx
  itkWeightedMeanCalculatorTest.cxx
)

ADD_EXECUTABLE(itkStatisticsBCTests itkStatisticsBCTests.cxx ${Statistics_SRCS})
TARGET_LINK_LIBRARIES(itkStatisticsBCTests ITKIO ITKCommon ITKNumerics ITKStatisticsNew)


