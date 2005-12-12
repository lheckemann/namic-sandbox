PROJECT(JointRegistrationSegmentation)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)


INCLUDE_DIRECTORIES( 
  ${JointRegistrationSegmentation_SOURCE_DIR}
  )

ADD_EXECUTABLE(ImageRegistrationTest
  ImageRegistrationTest.cxx 
  )

TARGET_LINK_LIBRARIES(ImageRegistrationTest ITKStatistics ITKIO ITKCommon ITKNumerics)


