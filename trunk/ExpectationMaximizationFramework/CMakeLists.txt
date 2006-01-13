PROJECT(ExpectationMaximizationFramework)

# Put this first, so duplicate header are taken 
# first from this subdirectory instead of ITKs.
INCLUDE_DIRECTORIES( 
  ${ExpectationMaximizationFramework_SOURCE_DIR}
  )


FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)


ADD_EXECUTABLE(itkExpectationMaximizationMethodTest
  itkExpectationMaximizationMethodTest.cxx 
  )

TARGET_LINK_LIBRARIES(itkExpectationMaximizationMethodTest ITKStatistics ITKIO ITKCommon ITKNumerics)


