PROJECT( ReworkStatistics )

FIND_PACKAGE(ITK)

IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build Application without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)
 
IF( NOT ITK_SOURCE_DIR )
  MESSAGE(FATAL_ERROR "ITK source directory is not set")
ENDIF( NOT ITK_SOURCE_DIR )

SET( ITK_DATA_ROOT "${ITK_SOURCE_DIR}/Testing/Data" )

 


SUBDIRS( src )
SUBDIRS( Tests )
SUBDIRS( Examples )
