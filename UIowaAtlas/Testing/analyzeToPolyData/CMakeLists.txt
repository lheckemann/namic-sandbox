PROJECT(convertAnalyzeToVTKImage)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

FIND_PACKAGE(VTK REQUIRED)
IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
ENDIf(VTK_FOUND)

SET(Libraries
    ITKCommon
    ITKBasicFilters
    ITKIO
    vtkIO
    vtkVolumeRendering
)

ADD_EXECUTABLE(convertAnalyzeToVTKImage convertAnalyzeToVTKImage.cxx)

TARGET_LINK_LIBRARIES(convertAnalyzeToVTKImage ${Libraries})

