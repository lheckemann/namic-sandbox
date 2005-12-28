PROJECT(DataModelManager)

FIND_PACKAGE(ITK)
IF (USE_ITK_FILE)
  INCLUDE (${USE_ITK_FILE})
ELSE (USE_ITK_FILE)
  MESSAGE( FATAL_ERROR "This application requires ITK. One of these components is missing. Please verify configuration")
ENDIF (USE_ITK_FILE)


SUBDIRS(SQLite)

FIND_LIBRARY(SQLITE_LIBRARY  
   NAMES sqlite 
   PATHS ${HOME}/local/lib  /usr/local/lib 
   DOC "SQLite Library"
)

INCLUDE_DIRECTORIES( 
  ${DataModelManager_SOURCE_DIR}
  ${SQLite_SOURCE_DIR}
  )

ADD_EXECUTABLE(DataModelManagerTest
  DataModelManagerTest.cxx 
  DataModelManager.cxx 
  )

TARGET_LINK_LIBRARIES( DataModelManagerTest 
  ITKStatistics ITKIO ITKCommon ITKNumerics ${SQLITE_LIBRARY})


