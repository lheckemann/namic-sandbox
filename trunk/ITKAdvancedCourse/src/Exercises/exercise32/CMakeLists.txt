PROJECT(Exercise32)

FIND_PACKAGE(ITK REQUIRED)
IF( ITK_FOUND )
  INCLUDE (${USE_ITK_FILE})
ENDIF( ITK_FOUND )

FIND_PACKAGE(VTK REQUIRED)
IF( VTK_FOUND )
  INCLUDE (${USE_VTK_FILE})
ENDIF( VTK_FOUND )

INCLUDE_DIRECTORIES( 
  ${Exercise32_SOURCE_DIR} 
  )

ADD_EXECUTABLE( myProject32 itkReadITKImageShowVTK.cxx )

TARGET_LINK_LIBRARIES( myProject32
    ITKIO ITKBasicFilters ITKNumerics ITKCommon 
    vtkRendering vtkGraphics vtkHybrid vtkImaging 
    vtkIO vtkFiltering vtkCommon )

