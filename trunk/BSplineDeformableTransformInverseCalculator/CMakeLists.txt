PROJECT(BSplineDeformableTransformInverseCalculators)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

SET(EXECUTABLE_OUTPUT_PATH ${BSplineDeformableTransformInverseCalculators_BINARY_DIR}/bin CACHE INTERNAL "Where to put the executables") 
SET(LIBRARY_OUTPUT_PATH ${BSplineDeformableTransformInverseCalculators_BINARY_DIR}/bin CACHE INTERNAL "Where to put the libraries") 

ADD_SUBDIRECTORY( Source )
ADD_SUBDIRECTORY( Testing )
ADD_SUBDIRECTORY( Paper )
