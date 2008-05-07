PROJECT(VectorImageRegistration)

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

SET(EXECUTABLE_OUTPUT_PATH ${VectorImageRegistration_BINARY_DIR}/bin CACHE INTERNAL "Where to put the executables") 
SET(LIBRARY_OUTPUT_PATH ${VectorImageRegistration_BINARY_DIR}/bin CACHE INTERNAL "Where to put the libraries") 

ENABLE_TESTING()

INCLUDE_DIRECTORIES(
  ${VectorImageRegistration_SOURCE_DIR}/Source
  )

ADD_SUBDIRECTORY( Source )
ADD_SUBDIRECTORY( Testing )
ADD_SUBDIRECTORY( Paper )

