PROJECT(JointRegistrationSegmentation)

# Put this first, so duplicate header are taken 
# first from this subdirectory instead of ITKs.
INCLUDE_DIRECTORIES( 
  ${JointRegistrationSegmentation_SOURCE_DIR}
  )


FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)


ADD_EXECUTABLE(ImageRegistrationTest
  ImageRegistrationTest.cxx 
  )

TARGET_LINK_LIBRARIES(ImageRegistrationTest ITKStatistics ITKIO ITKCommon ITKNumerics)


