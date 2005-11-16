PROJECT(DataModelManager)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)


INCLUDE_DIRECTORIES( 
  ${DataModelManager_SOURCE_DIR}
  )

ADD_EXECUTABLE(DataModelManager 
  DataModelManager.cxx 
  )

TARGET_LINK_LIBRARIES( DataModelManager 
  ITKStatistics ITKIO ITKCommon ITKNumerics)


