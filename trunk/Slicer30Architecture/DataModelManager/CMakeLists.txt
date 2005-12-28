PROJECT(DataModelManager)

IF(NOT EXECUTABLE_OUTPUT_PATH)
  SET(EXECUTABLE_OUTPUT_PATH ${DataModelManager_BINARY_DIR}/bin)
ENDIF(NOT EXECUTABLE_OUTPUT_PATH)

IF(NOT LIBRARY_OUTPUT_PATH)
  SET(LIBRARY_OUTPUT_PATH ${DataModelManager_BINARY_DIR}/lib)
ENDIF(NOT LIBRARY_OUTPUT_PATH)

FIND_PACKAGE(ITK)
IF (USE_ITK_FILE)
  INCLUDE (${USE_ITK_FILE})
ELSE (USE_ITK_FILE)
  MESSAGE( FATAL_ERROR "This application requires ITK. One of these components is missing. Please verify configuration")
ENDIF (USE_ITK_FILE)

SUBDIRS(SQLite)

INCLUDE_DIRECTORIES( 
  ${DataModelManager_SOURCE_DIR}
  ${SQLite_SOURCE_DIR}
  )

LINK_DIRECTORIES( 
  ${SQLite_BINARY_DIR} 
  ${LIBRARY_OUTPUT_PATH}
  )

ADD_EXECUTABLE(DataModelManagerTest
  DataModelManagerTest.cxx 
  DataModelManager.cxx 
  )


TARGET_LINK_LIBRARIES( DataModelManagerTest 
  ITKStatistics ITKIO ITKCommon ITKNumerics sqlite)


