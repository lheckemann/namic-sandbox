PROJECT(LevelSetBasedCellSegmentation)

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

SET(EXECUTABLE_OUTPUT_PATH ${LevelSetBasedCellSegmentation_BINARY_DIR}/bin CACHE INTERNAL "Where to put the executables") 
SET(LIBRARY_OUTPUT_PATH ${LevelSetBasedCellSegmentation_BINARY_DIR}/bin CACHE INTERNAL "Where to put the libraries") 

ENABLE_TESTING()

INCLUDE_DIRECTORIES(
  ${LevelSetBasedCellSegmentation_SOURCE_DIR}/Source
  )

ADD_SUBDIRECTORY( Original )
ADD_SUBDIRECTORY( Source )
ADD_SUBDIRECTORY( Testing )
ADD_SUBDIRECTORY( Paper )

