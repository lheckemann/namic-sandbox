PROJECT(ReconstructTensor)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
  "ITK not found.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(ReconstructTensor ReconstructTensor.cxx)

TARGET_LINK_LIBRARIES(ReconstructTensor ITKCommon ITKNumerics ITKNrrdIO ITKIO)

INSTALL_TARGETS(/bin ReconstructTensor)
