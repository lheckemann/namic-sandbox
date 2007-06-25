PROJECT(AnalyzeObjectReaderTesting)

SET(AnalyzeObjectReaderTesting_SRCS
  itkAnalyzeObjectImageIOTest.cxx
)

ADD_EXECUTABLE(AnalyzeObjectReaderTest ${AnalyzeObjectReaderTesting_SRCS})

TARGET_LINK_LIBRARIES(AnalyzeObjectReaderTest  AnalyzeObjectITKIO)

ADD_TEST(AnalyzeObjectReaderTest01 AnalyzeObjectReaderTest 
  ${AnalyzeObjectReaderTest_DATA_ROOT}/na01_2d.obj
  )

