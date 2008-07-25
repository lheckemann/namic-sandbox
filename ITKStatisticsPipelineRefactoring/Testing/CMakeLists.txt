IF( NOT ITK_DISABLE_CXX_TESTING )
IF(ITK_BINARY_DIR)
  IF(TCL_TCLSH)
    ADD_TEST(PrintSelf-Statistics ${TCL_TCLSH}
             ${ITK_SOURCE_DIR}/Utilities/Dart/PrintSelfCheck.tcl  
             ${ITK_SOURCE_DIR}/Code/Numerics/Statistics)
  ENDIF(TCL_TCLSH)
ENDIF(ITK_BINARY_DIR)

SET(BASELINE ${ITK_DATA_ROOT}/Baseline/Statistics)
SET(TEMP ${ITKTesting_BINARY_DIR}/Temporary)

SET(STATISTICS_TESTS ${EXECUTABLE_OUTPUT_PATH}/itkStatisticsTests)
SET(STATISTICS_HEADER_TESTS ${EXECUTABLE_OUTPUT_PATH}/itkStatisticsHeaderTest)

ADD_TEST(itkStatisticsHeaderTest ${STATISTICS_HEADER_TESTS})
ADD_TEST(itkStatisticsPrintTest ${STATISTICS_TESTS} itkStatisticsPrintTest)
ADD_TEST(itkStatisticsTypesTest ${STATISTICS_TESTS} itkStatisticsTypesTest)

ADD_TEST(itkCovarianceFilterTest ${STATISTICS_TESTS} itkCovarianceFilterTest)
ADD_TEST(itkCovarianceFilterTest2 ${STATISTICS_TESTS} itkCovarianceFilterTest2)
ADD_TEST(itkCovarianceFilterTest3 ${STATISTICS_TESTS} itkCovarianceFilterTest3)
ADD_TEST(itkHistogramToTextureFeaturesFilterTest ${STATISTICS_TESTS} itkHistogramToTextureFeaturesFilterTest)
ADD_TEST(itkHistogramTest ${STATISTICS_TESTS} itkHistogramTest)
ADD_TEST(itkImageToListSampleFilterTest ${STATISTICS_TESTS} itkImageToListSampleFilterTest)
ADD_TEST(itkImageToListSampleFilterTest2 ${STATISTICS_TESTS} itkImageToListSampleFilterTest2)
ADD_TEST(itkImageToListSampleFilterTest3 ${STATISTICS_TESTS} itkImageToListSampleFilterTest3)
ADD_TEST(itkListSampleTest1 ${STATISTICS_TESTS} itkListSampleTest 1)
ADD_TEST(itkListSampleTest2 ${STATISTICS_TESTS} itkListSampleTest 5)
ADD_TEST(itkListSampleTest3 ${STATISTICS_TESTS} itkListSampleTest 10)
ADD_TEST(itkListSampleTest4 ${STATISTICS_TESTS} itkListSampleTest 100)
ADD_TEST(itkSampleToHistogramFilterTest ${STATISTICS_TESTS} itkSampleToHistogramFilterTest)
ADD_TEST(itkSampleToHistogramFilterTest2 ${STATISTICS_TESTS} itkSampleToHistogramFilterTest2)
ADD_TEST(itkSampleToHistogramFilterTest3 ${STATISTICS_TESTS} itkSampleToHistogramFilterTest3)
ADD_TEST(itkSampleToHistogramFilterTest4 ${STATISTICS_TESTS} itkSampleToHistogramFilterTest4)
ADD_TEST(itkSampleToHistogramFilterTest5 ${STATISTICS_TESTS} itkSampleToHistogramFilterTest5
   ${ITK_SOURCE_DIR}/Testing/Data/Input/VisibleWomanEyeSlice.png
   )
ADD_TEST(itkSampleToHistogramFilterTest6 ${STATISTICS_TESTS} itkSampleToHistogramFilterTest6)
ADD_TEST(itkSampleToHistogramFilterTest7 ${STATISTICS_TESTS} itkSampleToHistogramFilterTest7)
ADD_TEST(itkMeanFilterTest ${STATISTICS_TESTS} itkMeanFilterTest)
ADD_TEST(itkMeasurementVectorTraitsTest ${STATISTICS_TESTS} itkMeasurementVectorTraitsTest)
ADD_TEST(itkMembershipSampleTest1 ${STATISTICS_TESTS} itkMembershipSampleTest1)
ADD_TEST(itkMembershipSampleTest2 ${STATISTICS_TESTS} itkMembershipSampleTest2)
ADD_TEST(itkMembershipSampleTest3 ${STATISTICS_TESTS} itkMembershipSampleTest3)
ADD_TEST(itkNeighborhoodSamplerTest1 ${STATISTICS_TESTS} itkNeighborhoodSamplerTest1)
ADD_TEST(itkScalarImageToCooccurrenceMatrixFilterTest ${STATISTICS_TESTS} itkScalarImageToCooccurrenceMatrixFilterTest)
ADD_TEST(itkScalarImageToCooccurrenceMatrixFilterTest2 ${STATISTICS_TESTS} itkScalarImageToCooccurrenceMatrixFilterTest2)
ADD_TEST(itkScalarImageToCooccurrenceListSampleFilterTest ${STATISTICS_TESTS} itkScalarImageToCooccurrenceListSampleFilterTest)
ADD_TEST(itkSampleTest ${STATISTICS_TESTS} itkSampleTest)
ADD_TEST(itkSampleTest2 ${STATISTICS_TESTS} itkSampleTest2)
ADD_TEST(itkSampleTest3 ${STATISTICS_TESTS} itkSampleTest3)
ADD_TEST(itkSampleTest4 ${STATISTICS_TESTS} itkSampleTest4)
ADD_TEST(itkSampleClassifierFilterTest1 ${STATISTICS_TESTS} itkSampleClassifierFilterTest1)
ADD_TEST(itkSampleToSubsampleFilterTest1 ${STATISTICS_TESTS} itkSampleToSubsampleFilterTest1)
ADD_TEST(itkSubsampleTest ${STATISTICS_TESTS} itkSubsampleTest)
ADD_TEST(itkSubsampleTest2 ${STATISTICS_TESTS} itkSubsampleTest2)
ADD_TEST(itkSubsampleTest3 ${STATISTICS_TESTS} itkSubsampleTest3)
ADD_TEST(itkStatisticsAlgorithmTest ${STATISTICS_TESTS} itkStatisticsAlgorithmTest)
ADD_TEST(itkWeightedMeanFilterTest ${STATISTICS_TESTS} itkWeightedMeanFilterTest)
ADD_TEST(itkWeightedCovarianceFilterTest ${STATISTICS_TESTS} itkWeightedCovarianceFilterTest)
ADD_TEST(itkImageToListSampleAdaptorTest ${STATISTICS_TESTS} itkImageToListSampleAdaptorTest)
ADD_TEST(itkImageToListSampleAdaptorTest2 ${STATISTICS_TESTS} itkImageToListSampleAdaptorTest2)
ADD_TEST(itkJointDomainImageToListSampleAdaptorTest ${STATISTICS_TESTS} itkJointDomainImageToListSampleAdaptorTest)
ADD_TEST(itkPointSetToListSampleAdaptorTest ${STATISTICS_TESTS} itkPointSetToListSampleAdaptorTest)
ADD_TEST(itkDenseFrequencyContainer2Test ${STATISTICS_TESTS} itkDenseFrequencyContainer2Test)
ADD_TEST(itkSparseFrequencyContainer2Test ${STATISTICS_TESTS} itkSparseFrequencyContainer2Test)
ADD_TEST(itkScalarImageToTextureFeaturesFilterTest ${STATISTICS_TESTS} itkScalarImageToTextureFeaturesFilterTest)


ADD_TEST(itkMembershipFunctionBaseTest ${STATISTICS_TESTS} itkMembershipFunctionBaseTest)
ADD_TEST(itkDistanceToCentroidMembershipFunctionTest ${STATISTICS_TESTS} itkDistanceToCentroidMembershipFunctionTest)
ADD_TEST(itkDistanceMetricTest ${STATISTICS_TESTS} itkDistanceMetricTest)
ADD_TEST(itkEuclideanDistanceMetricTest ${STATISTICS_TESTS} itkEuclideanDistanceMetricTest)

SET(Statistics_SRCS
  itkCovarianceFilterTest.cxx
  itkCovarianceFilterTest2.cxx
  itkCovarianceFilterTest3.cxx
  itkHistogramTest.cxx
  itkHistogramToTextureFeaturesFilterTest.cxx
  itkImageToListSampleFilterTest.cxx
  itkImageToListSampleFilterTest2.cxx
  itkImageToListSampleFilterTest3.cxx
  itkListSampleTest.cxx
  itkSampleToHistogramFilterTest.cxx
  itkSampleToHistogramFilterTest2.cxx
  itkSampleToHistogramFilterTest3.cxx
  itkSampleToHistogramFilterTest4.cxx
  itkSampleToHistogramFilterTest5.cxx
  itkSampleToHistogramFilterTest6.cxx
  itkSampleToHistogramFilterTest7.cxx
  itkMeanFilterTest.cxx
  itkMeasurementVectorTraitsTest.cxx
  itkMembershipSampleTest1.cxx
  itkMembershipSampleTest2.cxx
  itkMembershipSampleTest3.cxx
  itkNeighborhoodSamplerTest1.cxx
  itkScalarImageToCooccurrenceMatrixFilterTest.cxx
  itkScalarImageToCooccurrenceMatrixFilterTest2.cxx
  itkScalarImageToCooccurrenceListSampleFilterTest.cxx
  itkStatisticsPrintTest.cxx
  itkStatisticsTypesTest.cxx
  itkStatisticsAlgorithmTest.cxx
  itkSampleTest.cxx
  itkSampleTest2.cxx
  itkSampleTest3.cxx
  itkSampleTest4.cxx
  itkSampleClassifierFilterTest1.cxx
  itkSampleToSubsampleFilterTest1.cxx
  itkSubsampleTest.cxx
  itkSubsampleTest2.cxx
  itkSubsampleTest3.cxx
  itkWeightedMeanFilterTest.cxx
  itkWeightedCovarianceFilterTest.cxx
  itkImageToListSampleAdaptorTest.cxx
  itkImageToListSampleAdaptorTest2.cxx
  itkJointDomainImageToListSampleAdaptorTest.cxx
  itkPointSetToListSampleAdaptorTest.cxx
  itkDenseFrequencyContainer2Test.cxx
  itkSparseFrequencyContainer2Test.cxx
  itkScalarImageToTextureFeaturesFilterTest.cxx


  itkMembershipFunctionBaseTest.cxx
  itkDistanceToCentroidMembershipFunctionTest.cxx
  itkDistanceMetricTest.cxx
  itkEuclideanDistanceMetricTest.cxx
  )

ADD_EXECUTABLE(itkStatisticsTests itkStatisticsTests.cxx ${Statistics_SRCS})
TARGET_LINK_LIBRARIES(itkStatisticsTests ITKIO ITKCommon ITKNumerics ITKStatistics ITKStatisticsNew)

ADD_EXECUTABLE(itkStatisticsHeaderTest itkStatisticsHeaderTest.cxx)
TARGET_LINK_LIBRARIES(itkStatisticsHeaderTest ITKIO ITKCommon ITKNumerics ITKStatistics ITKStatisticsNew)



ENDIF( NOT ITK_DISABLE_CXX_TESTING )


