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

SET( ANALYE_OBJECTMAPTEST_SRC
itkAnalyzeObjectMapTest.cxx)

ADD_EXECUTABLE( itkAnalyzeObjectMapTest ${ANALYE_OBJECTMAPTEST_SRC} )
TARGET_LINK_LIBRARIES( itkAnalyzeObjectMapTest AnalyzeObjectIO ITKIO )

#ADD_EXECUTABLE( ${ANALYE_OBJECTMAPTEST_SRC} )

ADD_TEST( itkAnalyzeObjectMapTest01 itkAnalyzeObjectMapTest ${AnalyzeObjectReader_SOURCE_DIR}/Data/test.obj)

