PROJECT(SlicerClustering)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
        INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
        MESSAGE(FATAL_ERROR "Could not find ITK")
ENDIF(ITK_FOUND)

FIND_PACKAGE(VTK)
IF(VTK_FOUND)
        INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
        MESSAGE(FATAL_ERROR "Could not find VTK")
ENDIF(VTK_FOUND)

# include the superclass from its Sandbox directory
INCLUDE_DIRECTORIES( 
  ${SpectralClustering_SOURCE_DIR}
  )

SUBDIRS(Code Testing)

