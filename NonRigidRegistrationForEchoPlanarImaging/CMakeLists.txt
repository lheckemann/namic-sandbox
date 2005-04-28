
PROJECT(MultiModalityRigidRegistration)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)


INCLUDE_DIRECTORIES( 
  ${MultiModalityRigidRegistration_SOURCE_DIR}
  )

ADD_EXECUTABLE(MultiModalityRigidRegistration 
  MultiModalityRigidRegistration.cxx 
  MultiModalityRigidRegistrationCommand.cxx 
  )

TARGET_LINK_LIBRARIES(MultiModalityRigidRegistration ITKIO ITKCommon ITKNumerics)

ADD_EXECUTABLE(DicomSeriesReadImageWrite DicomSeriesReadImageWrite.cxx )
TARGET_LINK_LIBRARIES(DicomSeriesReadImageWrite ITKIO ITKCommon ITKNumerics)


