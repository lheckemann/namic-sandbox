PROJECT(Exercise33)

FIND_PACKAGE(ITK REQUIRED)
IF( ITK_FOUND )
  INCLUDE (${USE_ITK_FILE})
ENDIF( ITK_FOUND )

FIND_PACKAGE(VTK REQUIRED)
IF( VTK_FOUND )
  INCLUDE (${USE_VTK_FILE})
ENDIF( VTK_FOUND )

INCLUDE_DIRECTORIES( 
  ${Exercise33_SOURCE_DIR} 
  )

ADD_EXECUTABLE( myProject33 itkReadITKImageSegmentShowVTK.cxx )

TARGET_LINK_LIBRARIES( myProject33
    ITKIO ITKBasicFilters ITKNumerics ITKCommon 
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon )

