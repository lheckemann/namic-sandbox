PROJECT(Exercise19)

FIND_PACKAGE(ITK REQUIRED)
IF( ITK_FOUND )
  INCLUDE (${USE_ITK_FILE})
ENDIF( ITK_FOUND )

FIND_PACKAGE(ITK REQUIRED)
IF( VTK_FOUND )
  INCLUDE (${USE_VTK_FILE})
ENDIF( VTK_FOUND )

INCLUDE_DIRECTORIES( 
  ${CMAKE_CURRENT_SOURCE_DIR} 
  )

ADD_EXECUTABLE( myProject19 myProject.cxx )

TARGET_LINK_LIBRARIES( myProject19
    ITKIO ITKBasicFilters ITKNumerics ITKCommon 
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon )

ADD_EXECUTABLE( myProject19Answer myProjectAnswer.cxx )

TARGET_LINK_LIBRARIES( myProject19Answer
    ITKIO ITKBasicFilters ITKNumerics ITKCommon 
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon )

