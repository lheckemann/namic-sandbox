PROJECT(volumeRender)

INCLUDE(${CMAKE_SOURCE_DIR}/../IJMacros.txt)

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

FIND_PACKAGE(VTK REQUIRED)
IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
ENDIf(VTK_FOUND)

SET(IO_SRCS
  ${CMAKE_SOURCE_DIR}/../Source/vtkKWImage.cxx
  ${CMAKE_SOURCE_DIR}/../Source/vtkKWImageIO.cxx
)

SET(Libraries
    ITKCommon
    ITKBasicFilters
    ITKIO
    vtkIO
    vtkVolumeRendering
)

ADD_EXECUTABLE(volumeRender volumeRender.cxx ${IO_SRCS})

TARGET_LINK_LIBRARIES(volumeRender ${Libraries})
