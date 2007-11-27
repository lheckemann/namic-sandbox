PROJECT(Exercise37)

FIND_PACKAGE(ITK REQUIRED)
IF( ITK_FOUND )
  INCLUDE (${USE_ITK_FILE})
ENDIF( ITK_FOUND )


INCLUDE_DIRECTORIES( 
  ${Exercise37_SOURCE_DIR} 
  )

ADD_EXECUTABLE( ImageRegistrationWithMask ImageRegistrationWithMask.cxx )
TARGET_LINK_LIBRARIES( ImageRegistrationWithMask ITKAlgorithms ITKIO ITKCommon )

ADD_EXECUTABLE( ModelToImageRegistartion ModelToImageRegistartion.cxx )
TARGET_LINK_LIBRARIES( ModelToImageRegistartion ITKAlgorithms ITKIO ITKCommon )

