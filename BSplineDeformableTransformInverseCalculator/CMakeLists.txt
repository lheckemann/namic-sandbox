PROJECT(BSplineDeformableTransformInverseCalculator)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

SUBDIRS(
  Source
  Testing
  Paper
  )
