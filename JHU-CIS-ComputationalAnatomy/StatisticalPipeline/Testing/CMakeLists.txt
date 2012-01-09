SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR}/Data)
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)


#######################
#######################
#
#  Define Executables
#
#######################
#######################

ADD_EXECUTABLE(itkStatisticalPermutationTest itkStatisticalPermutationTest.cxx )
TARGET_LINK_LIBRARIES(itkStatisticalPermutationTest ${ITK_LIBRARIES})

ADD_EXECUTABLE(itkTestIndexSort itkTestIndexSort.cxx)
TARGET_LINK_LIBRARIES(itkTestIndexSort ${ITK_LIBRARIES})
###################
###################
#
#  Define Tests
#
###################
###################

ADD_TEST(itkStatisticalPermutationTest
  ${CXX_TEST_PATH}/itkStatisticalPermutationTest
  )


ADD_TEST(itkTestIndexSort
  ${CXX_TEST_PATH}/itkTestIndexSort
  -n 20
  )
