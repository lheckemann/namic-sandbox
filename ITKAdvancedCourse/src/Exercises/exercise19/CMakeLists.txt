PROJECT(Exercise19)

INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake)
IF (USE_ITK_FILE)
  INCLUDE (${USE_ITK_FILE})
ENDIF (USE_ITK_FILE)

INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
IF (USE_VTK_FILE)
  INCLUDE (${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

INCLUDE_DIRECTORIES(
  ${myProject_SOURCE_DIR}/../Auxiliary
)
  

ADD_EXECUTABLE( myProject myProject.cxx )

TARGET_LINK_LIBRARIES( myProject
    ITKMetaIO ITKIO ITKBasicFilters ITKNumerics ITKCommon 
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon )

ADD_EXECUTABLE( myProjectAnswer myProjectAnswer.cxx )

TARGET_LINK_LIBRARIES( myProjectAnswer
    ITKMetaIO ITKIO ITKBasicFilters ITKNumerics ITKCommon 
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon )

