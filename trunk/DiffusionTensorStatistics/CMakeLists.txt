PROJECT(DiffusionTensorStatisticsTesting)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(TensorGeometryTest TensorGeometryTest.cxx)
ADD_EXECUTABLE(TensorStatisticsTest TensorStatisticsTest.cxx)

TARGET_LINK_LIBRARIES(TensorGeometryTest ITKCommon)
TARGET_LINK_LIBRARIES(TensorStatisticsTest ITKCommon)
