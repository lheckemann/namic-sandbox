PROJECT(raw2mesh)

# Include this directory before the ITK_DIR. If we are going to make changes
# to the bayesian filters, we want the file in the sandbox to be picked up 
# prior to the one in ITK.
INCLUDE_DIRECTORIES( 
  ${raw2mesh_SOURCE_DIR}
  )

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

ADD_EXECUTABLE(raw2genus0 raw2genus0.cxx )
TARGET_LINK_LIBRARIES(raw2genus0 ITKIO ITKCommon ITKBasicFilters ITKStatistics)

ADD_EXECUTABLE(labels2genus0 labels2genus0.cxx )
TARGET_LINK_LIBRARIES(labels2genus0 ITKIO ITKCommon ITKBasicFilters ITKStatistics)

ADD_EXECUTABLE(genus02mesh genus02mesh.cxx )
TARGET_LINK_LIBRARIES(genus02mesh vtkRendering vtkIO ITKCommon ITKBasicFilters ITKIO itkvnl)

#ADD_EXECUTABLE(ls_raw2genus0 ls_raw2genus0.cxx )
#TARGET_LINK_LIBRARIES(ls_raw2genus0 ITKIO ITKCommon ITKBasicFilters ITKAlgorithms)

ADD_EXECUTABLE(TPGACImageFilter TPGACImageFilter.cxx )
TARGET_LINK_LIBRARIES(TPGACImageFilter ITKNumerics ITKIO)
