SET(projName "itkSWaveletTest")
PROJECT(${projName})

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)

FIND_PACKAGE(VTK)
IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR "VTK not found. Please set VTK_DIR.")
ENDIF(VTK_FOUND)

# Add the executable
ADD_EXECUTABLE(${projName} ${projName}.cxx)
TARGET_LINK_LIBRARIES(${projName} vtkRendering vtkIO ITKCommon ITKBasicFilters ITKIO)
