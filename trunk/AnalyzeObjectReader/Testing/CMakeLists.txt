PROJECT(AnalyzeObjectReaderTesting)

#SET(AnalyzeObjectReaderTesting_SRCS
#  itkAnalyzeObjectImageIOTest.cxx
#)

# SET(AnalyzeObjectReaderTestingOutputDir
#   ${AnalyzeObjectReaderTesting_BINARY_DIR}/Testing/Temporary)
# 
# ADD_EXECUTABLE(AnalyzeObjectReaderTest ${AnalyzeObjectReaderTesting_SRCS})
# 
# TARGET_LINK_LIBRARIES(AnalyzeObjectReaderTest  AnalyzeObjectITKIO)
# 
# ADD_TEST(AnalyzeObjectReaderTest01 AnalyzeObjectReaderTest
#   ${AnalyzeObjectReaderTest_DATA_ROOT}/na01_2d.obj
#   ${AnalyzeObjectReaderTestingOutputDir}/AnalyzeObjectReaderTest01Output.png
#   )

INCLUDE_DIRECTORIES( ${AnalyzeObjectLibrary_SOURCE_DIR} )

SET( ANALYE_OBJECTENTRYTEST_SRC
itkAnalyzeObjectEntryTest.cxx)

ADD_EXECUTABLE( itkAnalyzeObjectEntryTest ${ANALYE_OBJECTENTRYTEST_SRC} )
TARGET_LINK_LIBRARIES( itkAnalyzeObjectEntryTest ITKIO )

ADD_TEST( itkAnalyzeObjectEntryTest01 itkAnalyzeObjectEntryTest )

