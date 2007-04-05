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

SET(STATISTICS_TESTS ${CXX_TEST_PATH}/itkStatisticsTests)
SET(STATISTICS_HEADER_TESTS ${CXX_TEST_PATH}/itkStatisticsHeaderTest)

ADD_TEST(itkStatisticsHeaderTest ${STATISTICS_HEADER_TESTS})
ADD_TEST(itkStatisticsPrintTest ${STATISTICS_TESTS} itkStatisticsPrintTest)

ADD_TEST(itkSampleTest ${STATISTICS_TESTS} itkSampleTest)


SET(Statistics_SRCS
  itkStatisticsPrintTest.cxx
  itkSampleTest.cxx
  )

ADD_EXECUTABLE(itkStatisticsTests itkStatisticsTests.cxx ${Statistics_SRCS})
TARGET_LINK_LIBRARIES(itkStatisticsTests ITKIO ITKCommon ITKNumerics ITKStatistics)

ADD_EXECUTABLE(itkStatisticsHeaderTest itkStatisticsHeaderTest.cxx)
TARGET_LINK_LIBRARIES(itkStatisticsHeaderTest ITKIO ITKCommon ITKNumerics ITKStatistics)



ENDIF( NOT ITK_DISABLE_CXX_TESTING )


